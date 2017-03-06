/******************************************************************************
*
* MODULE
*   Partially generic DsCAN API for DS4302, DS2211, DS2210, DS1401 & DS1103
*
* FILE
*   dscan.c
*
* RELATED FILES
*   dscan.c
*
* DESCRIPTION
*   This module provides a generic (platform independent) CAN interface for
*   most dSPACE CAN platforms.
*   It is focused on message handling right now but may be extended further in
*   the future.
*   This file contains the (platform specific) implementation.
*
* AUTHOR(S)
*   AlexanderSt
*   O. Jabczinski
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: DsCan.c $ $Revision: 1.15 $ $Date: 2009/08/31 09:43:49GMT+01:00 $
******************************************************************************/
#include <dscan.h>
#include <dsmcr.h>

#if defined _DS1005 || defined _DS1006
    #include <ds4302.h>
    #include <ds2211.h>
    #include <ds2210.h>
    #include <ds2202.h>
#elif defined _DS1401
    #include <ds1401.h>
    #include <CAN1401Tp1.h>
    #include <CAN1401Init.h>
#elif defined _DS1103
    #include <ds1103.h>
    #include <can1103.h>
#else
    #error DsCan: The selected platform is not supported!
#endif


/** Queue size used for transmit and receive messages */
#define DSCAN_QUEUE_SIZE                    64

/** As long as there are multiple DPMEM queues a single one has to be selected
 *  and used for all messages in order to preserve the message order */
#define DSCAN_SPECIAL_DPMEM_QUEUE           0

/** Maximum number of slave boards supported by this module */
#define DSCAN_MAX_SLAVE_BOARDS              16

/** Maximum number of channels per board supported by this module */
#define DSCAN_MAX_CHANNELS                  4

/** This mask is used for range checks (29 bit CAN extended identifier) */
#define DSCAN_MSG_EXT_ID_MASK               0x1FFFFFFF

/** This mask is used for range checks (11 bit CAN standard identifier) */
#define DSCAN_MSG_STD_ID_MASK               0x000007FF

/** Maximum data length code for CAN frames */
#define DSCAN_MSG_MAX_DLC                   8

/** Delivers timeout information for TX messages */
#define DSXXXX_CAN_TIMEOUT_INFO            	32

/*
*  Uncomment the following line to print exception informations.
*/
/* #define DSCAN_DEBUG_ON 1 */

#ifdef DSCAN_DEBUG_ON
  #define DSCAN_DEBUG()  (msg_info_printf(MSG_SM_RTLIB, MSG_SM_DSCAN,"Exception from: %s, line %d", __FILE__, __LINE__ ))
#else
 #define DSCAN_DEBUG()
#endif


/** Wrapper for msg_error_printf function including return statement */
#define DSCAN_ERROR_PRINTF(Code, ...)                                   \
{                                                                       \
    msg_error_printf(MSG_SM_DSCAN, Code, Code##_TXT, __VA_ARGS__);      \
    return Code;                                                        \
}

/** Wrapper for msg_warning_printf function including return statement*/
#define DSCAN_WARNING_PRINTF(Code, ...)                                 \
{                                                                       \
    msg_warning_printf(MSG_SM_DSCAN, Code, Code##_TXT, __VA_ARGS__);    \
    return Code;                                                        \
}

/** Wrapper for msg_info_printf function including return statement */
#define DSCAN_INFO_PRINTF(Code, ...)                                    \
{                                                                       \
    msg_info_printf(MSG_SM_DSCAN, Code, Code##_TXT, __VA_ARGS__);       \
    return Code;                                                        \
}


/** Board type */
enum DsECanBoard_Type
{
    DSCAN_BOARD_TYPE_INVALID = 1,
    DSCAN_BOARD_TYPE_DS4302  = 2,
    DSCAN_BOARD_TYPE_DS2211  = 3,
    DSCAN_BOARD_TYPE_DS2210  = 4,
    DSCAN_BOARD_TYPE_DS2202  = 5,
    DSCAN_BOARD_TYPE_DS1401  = 6,
    DSCAN_BOARD_TYPE_DS1103  = 7
};
typedef enum DsECanBoard_Type DsECanBoard_Type;

/** Message types, not all DsCAN message types are supported */
enum DsECanMsg_Type
{
    /* DSCAN_MSG_TYPE_TX      = 1, */           /**< Transmit message object */
    DSCAN_MSG_TYPE_TX_QUEUE   = 2,              /**< Transmit queue message object */
    /* DSCAN_MSG_TYPE_RX      = 3, */           /**< Receive message object */
    DSCAN_MSG_TYPE_RX_MONITOR = 4               /**< Monitor message object */
};
typedef enum DsECanMsg_Type DsECanMsg_Type;


/** Board object */
struct DsSCanBoard
{
    UInt32           BoardNumber;               /**< Board number */
    DsECanBoard_Type Type;                      /**< Board type */
};
typedef struct DsSCanBoard DsSCanBoard;

/** Message object */
struct DsSCanMsg
{
    DsECanMsg_Type   MsgType;                   /**< Message type (tx, rx, ...) */
    DsECanBoard_Type BoardType;                 /**< Board type (DsXXXX) */
    UInt32           BufferIndex;               /**< Buffer index of associated DSMCR */
    UInt32           ClientNumber;              /**< Client number of associated DSMCR */
    UInt32           IsEnabled;                 /**< Enable flag */
    UInt32           ProcCount;                 /**< Processing count */
    UInt32           TxQueueCount;              /**< Number of frames in tx queue */
    UInt32           BoardNumber;               /**< Board number */
    UInt32           ChannelNumber;             /**< Channel number */
    UInt32           TxMsgLostError;            /**< Marks that error message has already been sent to host */
    void*            pStdMsg;                   /**< Pointer to standard tx/rx message */
    void*            pExtMsg;                   /**< Pointer to extended tx/rx message */
};
typedef struct DsSCanMsg DsSCanMsg;


/** Lists could be used for object management as well, this would require a
 *  little more glue logic. Static allocation is used at the moment. */

/** Created boards */
static DsSCanBoard* g_DsCanBoards[DSCAN_MAX_SLAVE_BOARDS] = {NULL};

/** Array with created receive monitors, each channel may manage multiple
 *  monitor messages. */
static DsSCanMsg** g_DsCanRxMsgs[DSCAN_MAX_SLAVE_BOARDS][DSCAN_MAX_CHANNELS] = {{NULL}};

/** Created transmit queues, each channel can manage only a single transmit
 *  queue at the moment. Support for multiple queues may be added later. */
static DsSCanMsg*  g_DsCanTxMsgs[DSCAN_MAX_SLAVE_BOARDS][DSCAN_MAX_CHANNELS] = {{NULL}};

/** Number of receive monitors per channel */
static UInt32 g_DsCanNumRxMonitors[DSCAN_MAX_SLAVE_BOARDS][DSCAN_MAX_CHANNELS] = {{0}};

/** Default descriptor for messages */
static const char* g_DsCanDefaultDescriptor = "DsCan";


/* Forward declarations */
static DsTError DsCan_initBoard(DsTCanBoard_Address Address, DsECanBoard_Type* pType, UInt32* pBoardNumber);
static DsTError DsCan_update(DsTCanBoard Board);
static DsTError DsCan_initRx(DsTCanCh Ch, DsECanBoard_Type Type, void** ppStdRxMsg, void** ppExtRxMsg);
static DsTError DsCan_initTx(DsTCanCh Ch, DsECanBoard_Type Type, void** ppStdTxMsg, void** ppExtTxMsg);
static DsTError DsCan_getChannelProperties(DsTCanCh Ch, UInt32* pBoardNumber, UInt32* pChannelNumber, DsECanBoard_Type* pType);
static DsTError DsCan_transmit(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem);

#if defined _DS1103
static DsTError DsCan_initRxDs1103(ds1103_canChannel* pCh, ds1103_canMsg** ppStdRxMsg, ds1103_canMsg** ppExtRxMsg);
static DsTError DsCan_initTxDs1103(ds1103_canChannel* pCh, ds1103_canMsg** ppStdTxMsg, ds1103_canMsg** ppExtTxMsg);
static DsTError DsCan_updateDs1103(UInt32 BoardNumber);
static void DsCan_unpackBlockDs1103(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size );
#endif
#if defined _DS1401
static DsTError DsCan_initRxDs1401(can_tp1_canChannel* pCh, can_tp1_canMsg** ppStdRxMsg, can_tp1_canMsg** ppExtRxMsg);
static DsTError DsCan_initTxDs1401(can_tp1_canChannel* pCh, can_tp1_canMsg** ppStdTxMsg, can_tp1_canMsg** ppExtTxMsg);
static DsTError DsCan_updateDs1401(UInt32 BoardNumber);
static void DsCan_unpackBlockDs1401(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size );
#endif
#if defined _DS1005 || defined _DS1006
static DsTError DsCan_updateDs2202(UInt32 BoardNumber);
static DsTError DsCan_updateDs2210(UInt32 BoardNumber);
static DsTError DsCan_updateDs2211(UInt32 BoardNumber);
static DsTError DsCan_updateDs4302(UInt32 BoardNumber);
static DsTError DsCan_initRxDs2202(ds2202_canChannel* pCh, ds2202_canMsg** ppStdRxMsg, ds2202_canMsg** ppExtRxMsg);
static DsTError DsCan_initRxDs2210(ds2210_canChannel* pCh, ds2210_canMsg** ppStdRxMsg, ds2210_canMsg** ppExtRxMsg);
static DsTError DsCan_initRxDs2211(ds2211_canChannel* pCh, ds2211_canMsg** ppStdRxMsg, ds2211_canMsg** ppExtRxMsg);
static DsTError DsCan_initRxDs4302(ds4302_canChannel* pCh, ds4302_canMsg** ppStdRxMsg, ds4302_canMsg** ppExtRxMsg);
static DsTError DsCan_initTxDs2202(ds2202_canChannel* pCh, ds2202_canMsg** ppStdTxMsg, ds2202_canMsg** ppExtTxMsg);
static DsTError DsCan_initTxDs2210(ds2210_canChannel* pCh, ds2210_canMsg** ppStdTxMsg, ds2210_canMsg** ppExtTxMsg);
static DsTError DsCan_initTxDs2211(ds2211_canChannel* pCh, ds2211_canMsg** ppStdTxMsg, ds2211_canMsg** ppExtTxMsg);
static DsTError DsCan_initTxDs4302(ds4302_canChannel* pCh, ds4302_canMsg** ppStdTxMsg, ds4302_canMsg** ppExtTxMsg);
static void DsCan_unpackBlockDs2202(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size );
static void DsCan_unpackBlockDs2210(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size );
static void DsCan_unpackBlockDs2211(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size );
static void DsCan_unpackBlockDs4302(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size );
#endif
#if defined _DS1103
static DsTError DsCan_transmitDs1103(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem);
#endif
#if defined _DS1401
static DsTError DsCan_transmitDs1401(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem);
#endif
#if defined _DS1005 || defined _DS1006
static DsTError DsCan_transmitDs2202(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem);
static DsTError DsCan_transmitDs2210(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem);
static DsTError DsCan_transmitDs2211(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem);
static DsTError DsCan_transmitDs4302(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem);
#endif
#if defined _DS1103
static void DsCan_initBoardDs1103(UInt32 Base, UInt32* pBoardNumber);
#endif
#if defined _DS1401
static void DsCan_initBoardDs1401(UInt32 Base, UInt32* pBoardNumber);
#endif
#if defined _DS1005 || defined _DS1006
static void DsCan_initBoardDs2202(UInt32 Base, UInt32* pBoardNumber);
static void DsCan_initBoardDs2210(UInt32 Base, UInt32* pBoardNumber);
static void DsCan_initBoardDs2211(UInt32 Base, UInt32* pBoardNumber);
static void DsCan_initBoardDs4302(UInt32 Base, UInt32* pBoardNumber);
#endif

/******************************************************************************
 * Real functions
 * (for a list of fake functions look at the end of the file)
 *****************************************************************************/

/** <!------------------------------------------------------------------------->
*   Creates a receive monitor message object.
*
*   @description
*   - To receive various messages on the CAN bus a receive monitor message can be used.
*   - The created message is by default disabled but configured to receive all
*     messages once it is enabled (format and RTR are thus set to don't care and the
*     mask is zero).
*   - If the number of messages exceeds the monitors FIFO size older messages are
*     overwritten and thus lost.
*
*   See also:
*   - DsCanMsg_start()
*   - DsCanMsg_stop()
*   - DsCanMsg_readRxItem()
*
*   @parameters
*       @param pMsg Returned pointer to the message handle. The function
*                   returns NULL for this argument if it fails.
*       @param Ch Channel handle (each message is connected to a specific channel).
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the pointer to the CAN message handle or the CAN
*     channel handle is NULL or
*   - #DSCAN_ERROR_MEMORY_ALLOCATION if an allocation error occurred.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_createRxMonitor(DsTCanMsg* pMsg, DsTCanCh Ch)
{
    DsTError Error;
    UInt32 BoardNumber, ChannelNumber;
    UInt32 BufferIndex, ClientNumber;
    void* pStdRxMsg, *pExtRxMsg;
    DsECanBoard_Type Type;
    DsSCanMsg* pNewMsg;

    /* Check for valid handles */
    if (NULL == pMsg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "pMsg", __FUNCTION__);
    }

    /* The DS1103 does not provide a channel handle (single channel) */
#if ! defined _DS1103
    if (NULL == Ch)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Ch", __FUNCTION__);
    }
#endif

    *pMsg = NULL;

    Error = DsCan_getChannelProperties(Ch, &BoardNumber, &ChannelNumber, &Type);

    if (DSCAN_NO_ERROR != Error)
    {
        DSCAN_DEBUG();

        return Error;
    }

    /* Do intialization of channel rx/tx and buffer only once */
    if (0 == g_DsCanNumRxMonitors[BoardNumber][ChannelNumber])
    {
        Error = DsCan_initRx(Ch, Type, &pStdRxMsg, &pStdRxMsg);

        if (Error != DSCAN_NO_ERROR)
        {
            DSCAN_DEBUG();
            return Error;
        }

        /* Create an additional buffer that will hold mixed data (std & ext, rx & loop-back) */
        Error = dsmcr_buffer(sizeof(DsSCanMsg_Item), DSCAN_QUEUE_SIZE, &BufferIndex);

        if (DSCAN_NO_ERROR != Error)
        {
            DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
        }
    }
    else
    {
        /* Get buffer index from first created monitor, all monitors of a
         * channel share a single buffer */
        BufferIndex = g_DsCanRxMsgs[BoardNumber][ChannelNumber][0]->BufferIndex;
        
        pStdRxMsg = g_DsCanRxMsgs[BoardNumber][ChannelNumber][0]->pStdMsg;
        pExtRxMsg = g_DsCanRxMsgs[BoardNumber][ChannelNumber][0]->pExtMsg;
    }

    /* All message handles are internally stored in an array, this array needs
     * to be expanded for each new message. */
    g_DsCanRxMsgs[BoardNumber][ChannelNumber] =
        rtlib_realloc(g_DsCanRxMsgs[BoardNumber][ChannelNumber],
                      sizeof(DsSCanMsg) * (g_DsCanNumRxMonitors[BoardNumber][ChannelNumber] + 1));

    if (NULL == g_DsCanRxMsgs[BoardNumber][ChannelNumber])
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pNewMsg = rtlib_malloc(sizeof(DsSCanMsg));

    if (NULL == pNewMsg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pNewMsg->BufferIndex    = BufferIndex;
    pNewMsg->IsEnabled      = DSCAN_MSG_STATE_INACTIVE;
    pNewMsg->MsgType        = DSCAN_MSG_TYPE_RX_MONITOR;
    pNewMsg->BoardType      = Type;
    pNewMsg->ProcCount      = 0;
    pNewMsg->TxQueueCount   = 0;
    pNewMsg->BoardNumber    = BoardNumber;
    pNewMsg->ChannelNumber  = ChannelNumber;
    pNewMsg->TxMsgLostError = 0;
    pNewMsg->pStdMsg        = pStdRxMsg;
    pNewMsg->pExtMsg        = pExtRxMsg;

    Error = dsmcr_client(BufferIndex, DSMCR_OVERRUN_IGNORE, NULL, &ClientNumber);

    if (DSMCR_NO_ERROR != Error)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pNewMsg->ClientNumber = ClientNumber;

    dsmcr_client_disconnect(BufferIndex, ClientNumber);

    g_DsCanRxMsgs[BoardNumber][ChannelNumber][g_DsCanNumRxMonitors[BoardNumber][ChannelNumber]] = pNewMsg;
    *pMsg = pNewMsg;

    g_DsCanNumRxMonitors[BoardNumber][ChannelNumber]++;

    return DSCAN_NO_ERROR;
}

/** <!------------------------------------------------------------------------->
*   Creates a transmit queue message object.
*
*   @description
*   - To transmit a number of consecutive messages on the CAN bus a transmit queue
*     can be used.
*   - Use this object type to reach the maximum bus load.
*   - The created message object is by default disabled.
*   - Once the queue is full all additional transmit requests are lost.
*
*   See also:
*   - DsCanMsg_start()
*   - DsCanMsg_stop()
*   - DsCanMsg_transmitItem()
*
*   @parameters
*       @param pMsg Returned pointer to the message handle. The function
*                   returns NULL for this argument if it fails.
*       @param Ch Channel handle (each message is connected to a specific channel).
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the pointer to the CAN message handle or the CAN
*     channel handle is NULL or
*   - #DSCAN_ERROR_MEMORY_ALLOCATION if an allocation error occurred.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_createTxQueue(DsTCanMsg* pMsg, DsTCanCh Ch)
{
    DsTError Error;
    UInt32 BoardNumber, ChannelNumber;
    UInt32 BufferIndex, ClientNumber;
    void* pStdTxMsg, *pExtTxMsg;
    DsECanBoard_Type Type;
    DsSCanMsg* pNewMsg;

    /* Check for valid handles */
    if (NULL == pMsg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "pMsg", __FUNCTION__);
    }

    /* The DS1103 does not provide a channel handle (single channel) */
#if ! defined _DS1103
    if (NULL == Ch)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Ch", __FUNCTION__);
    }
#endif

    *pMsg = NULL;

    Error = DsCan_getChannelProperties(Ch, &BoardNumber, &ChannelNumber, &Type);

    if (DSCAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return Error;
    }

    if (NULL == g_DsCanTxMsgs[BoardNumber][ChannelNumber])
    {
        Error = DsCan_initTx(Ch, Type, &pStdTxMsg, &pExtTxMsg);

        if (Error != DSCAN_NO_ERROR)
        {
            DSCAN_DEBUG();
            return Error;
        }

        /* Create an additional buffer that will hold incomplete loop-back data */
        Error = dsmcr_buffer(sizeof(DsSCanMsg_Item), DSCAN_QUEUE_SIZE, &BufferIndex);

        if (DSMCR_NO_ERROR != Error)
        {
            DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
        }

        Error = dsmcr_client(BufferIndex, DSMCR_OVERRUN_IGNORE, NULL, &ClientNumber);

        if (DSMCR_NO_ERROR != Error)
        {
            DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
        }

        pNewMsg = rtlib_malloc(sizeof(DsSCanMsg));

        if (NULL == pNewMsg)
        {
            DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
        }

        pNewMsg->BufferIndex    = BufferIndex;
        pNewMsg->ClientNumber   = 0; /* Not used */
        pNewMsg->IsEnabled      = DSCAN_MSG_STATE_INACTIVE;
        pNewMsg->MsgType        = DSCAN_MSG_TYPE_TX_QUEUE;
        pNewMsg->BoardType      = Type;
        pNewMsg->ProcCount      = 0;
        pNewMsg->TxQueueCount   = 0;
        pNewMsg->BoardNumber    = BoardNumber;
        pNewMsg->ChannelNumber  = ChannelNumber;
        pNewMsg->TxMsgLostError = 0;
        pNewMsg->pStdMsg        = pStdTxMsg;
        pNewMsg->pExtMsg        = pExtTxMsg;
    }
    else
    {
        pNewMsg = g_DsCanTxMsgs[BoardNumber][ChannelNumber];
    }

    g_DsCanTxMsgs[BoardNumber][ChannelNumber] = pNewMsg;
    *pMsg = pNewMsg;

    return DSCAN_NO_ERROR;
}

/** <!------------------------------------------------------------------------->
*   Reads a receive message object or monitor.
*
*   @description
*   - This function copies the content of a receive (monitor) message object
*     into a message item. All fields in the structure are updated.
*   - The application may use ProcCount or Status to detect new available messages.
*   - If Status == #DSCAN_MSG_NO_DATA all other information has not been updated.
*   - If the monitor buffer is full and new messages arrive old items are lost.
*     The application can detect that event be reading the message Status
*     (#DSCAN_MSG_OVERRUN).
*
*   See also:
*   - DsCanMsg_createRxMonitor()
*
*   @parameters
*       @param Msg A CAN message handle.
*       @param pItem Pointer to a message item structure.
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the CAN message handle or the pointer to the
*     CAN message item structure is NULL or
*   - #DSCAN_ERROR_INVALID_MSG_TYPE if the message has the wrong type.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_readRxItem(DsTCanMsg Msg, DsSCanMsg_Item* pItem)
{
    DsTError Error;
    UInt32   Status;

    /* Check for invalid function arguments */
    if (NULL == Msg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Msg", __FUNCTION__);
    }
    else if (NULL == pItem)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "pItem", __FUNCTION__);
    }
    else if (DSCAN_MSG_TYPE_RX_MONITOR == Msg->MsgType)
    {
        RTLIB_INT_SAVE_AND_DISABLE(Status);

        Error = dsmcr_read (Msg->BufferIndex,
                            Msg->ClientNumber,
                            (void*) pItem);

        if (DSMCR_NO_ERROR != Error)
        {
            RTLIB_INT_RESTORE(Status);

            if (DSMCR_BUFFER_EMPTY == Error)
            {
                pItem->ProcCount = Msg->ProcCount;
                pItem->Status    = DSCAN_MSG_NO_DATA;
                return DSCAN_NO_ERROR;
            }
            else
            {
                /* Implementation error */
                pItem->ProcCount = Msg->ProcCount;
                pItem->Status    = DSCAN_MSG_NO_DATA;
                return DSCAN_NO_ERROR;
            }
        }
        else
        {
            pItem->ProcCount = ++Msg->ProcCount;
            RTLIB_INT_RESTORE(Status);

            return DSCAN_NO_ERROR;
        }
    }
    else
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_TYPE, g_DsCanDefaultDescriptor, __FUNCTION__, Msg->MsgType);
    }
}

/** <!------------------------------------------------------------------------->
*   Transmits a message item using transmit message object.
*
*   @description
*   - This function is intended to override the parameters of a transmit message
*     object and to transmit free configurable message items.
*   - If the message item or the transmit message object is disabled no
*     transmission takes place and no error is returned.
*
*   See also:
*   - DsCanMsg_createTxQueue()
*
*   @parameters
*       @param Msg A CAN message handle.
*       @param pItem A CAN message item handle.
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_TRANSMIT_MSG_LOST if the queue is full and the current frame has been discarded or
*   - #DSCAN_ERROR_NULL_POINTER if the CAN message handle or the pointer to the
*     CAN message item structure is NULL or
*   - #DSCAN_ERROR_INVALID_MSG_TYPE if the message has the wrong type or
*   - #DSCAN_ERROR_INVALID_MSG_FORMAT if a wrong message format was used or
*   - #DSCAN_ERROR_INVALID_MSG_IDENTIFIER if a wrong identifier was used or
*   - #DSCAN_ERROR_COMMUNICATION if a communication error occurred.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_transmitItem(DsTCanMsg Msg, const DsSCanMsg_Item* pItem)
{
    DsTError Error;
    UInt32   Status;

    /* Check for valid function arguments */
    if (NULL == Msg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Msg", __FUNCTION__);
    }
    else if (NULL == pItem)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "pItem", __FUNCTION__);
    }
    else if (DSCAN_MSG_TYPE_TX_QUEUE != Msg->MsgType)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_TYPE, g_DsCanDefaultDescriptor, __FUNCTION__, Msg->MsgType);
    }
    else if (! ((DSCAN_MSG_FORMAT_STD == pItem->Format) ||(DSCAN_MSG_FORMAT_EXT == pItem->Format)))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_FORMAT, g_DsCanDefaultDescriptor, pItem->Format, __FUNCTION__);
    }
    else if (   ((DSCAN_MSG_FORMAT_STD == pItem->Format) && ((~DSCAN_MSG_STD_ID_MASK & pItem->Id) != 0))
             || ((DSCAN_MSG_FORMAT_EXT == pItem->Format) && ((~DSCAN_MSG_EXT_ID_MASK & pItem->Id) != 0)))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_IDENTIFIER, g_DsCanDefaultDescriptor, pItem->Id, __FUNCTION__, pItem->Format);
    }
    else if (DSCAN_MSG_RTR_DISABLED != pItem->Rtr)
    {
        /* Not supported on this platform */
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_RTR_STATE, g_DsCanDefaultDescriptor, pItem->Rtr, __FUNCTION__);
    }
    else if (   (DSCAN_MSG_STATE_ACTIVE != pItem->IsEnabled)
             || (DSCAN_MSG_STATE_ACTIVE != Msg->IsEnabled))
    {
        return DSCAN_NO_ERROR;
    }
    else
    {
        RTLIB_INT_SAVE_AND_DISABLE(Status);

        if (Msg->TxQueueCount >= DSCAN_QUEUE_SIZE)
        {
            /* TBD: Update lost messages if such a counter exists */

            /* Avoid emission of multiple messages in a row, so set error flag */
            if (0 == Msg->TxMsgLostError)
            {
                Msg->TxMsgLostError = 1;
                RTLIB_INT_RESTORE(Status);
                DSCAN_DEBUG();
                DSCAN_WARNING_PRINTF(DSCAN_ERROR_TRANSMIT_MSG_LOST, g_DsCanDefaultDescriptor, pItem->Id, pItem->Format);
            }
            else
            {
                RTLIB_INT_RESTORE(Status);
                DSCAN_DEBUG();
                return DSCAN_ERROR_TRANSMIT_MSG_LOST;
            }
        }

        Error = DsCan_transmit(Msg, pItem);

        if (DSCAN_NO_ERROR == Error)
        {
            dsmcr_write(Msg->BufferIndex, (void*)pItem);
            Msg->TxQueueCount++;
        }
        else
        {
            DSCAN_DEBUG();
        }

        RTLIB_INT_RESTORE(Status);

        return Error;
    }
}

/** <!------------------------------------------------------------------------->
*   Activates a message object.
*
*   @description
*   - This command takes immediate effect and does not need to be applied.
*   - Note: All message objects are by default disabled and need to be in enabled
*     in order to use them for message transmission / reception.
*   - Enabling a monitor message object cleans all messages within it.
*   - The ProcCount is reset to zero for all message object types when this
*     function is called.
*   - A monitor message is been reset (cleared) when executing this function
*     (only for state transitions inactive -> active).
*
*   See also:
*   - DsCanMsg_createTxQueue()
*   - DsCanMsg_createRxMonitor()
*   - DsCanMsg_stop()
*
*   @parameters
*       @param Msg The CAN message handle.
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the CAN message handle is NULL or
*   - #DSCAN_ERROR_INVALID_MSG_TYPE if the message handle is not valid.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_start(DsTCanMsg Msg)
{
    UInt32 Status;

    /* Check for invalid function arguments */
    if (NULL == Msg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Msg", __FUNCTION__);
    }

    if (DSCAN_MSG_TYPE_TX_QUEUE == Msg->MsgType)
    {
        RTLIB_INT_SAVE_AND_DISABLE(Status);

        if (DSCAN_MSG_STATE_INACTIVE == Msg->IsEnabled)
        {
            Msg->IsEnabled = DSCAN_MSG_STATE_ACTIVE;
            Msg->TxMsgLostError = 0;
            Msg->ProcCount = 0;
        }

        RTLIB_INT_RESTORE(Status);
    }
    else if (DSCAN_MSG_TYPE_RX_MONITOR == Msg->MsgType)
    {
        if (DSCAN_MSG_STATE_INACTIVE == Msg->IsEnabled)
        {
            RTLIB_INT_SAVE_AND_DISABLE(Status);

            Msg->IsEnabled = DSCAN_MSG_STATE_ACTIVE;
            Msg->ProcCount = 0;

            /* Monitor buffer gets cleaned when message is enabled */
            dsmcr_client_connect(Msg->BufferIndex, Msg->ClientNumber);

            RTLIB_INT_RESTORE(Status);
        }
    }
    else
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_TYPE, g_DsCanDefaultDescriptor, __FUNCTION__, Msg->MsgType);
    }

    return DSCAN_NO_ERROR;
}

/** <!------------------------------------------------------------------------->
*   Deactivates a message object.
*
*   @description
*   - This command takes immediate effect and doesn't need to be applied.
*   - All received frames matching a specific but disabled message object are
*     discarded. For transmit message / queue objects any frames to be transmitted
*     will be discarded without generating a warning.
*   - All previously pending transmit requests will be processed before the object
*     is actually been deactivated.
*   - Note: Every created message object is by default disabled.
*
*   See also:
*   - DsCanMsg_createTxQueue()
*   - DsCanMsg_createRxMonitor()
*   - DsCanMsg_start()
*
*   @parameters
*       @param Msg The CAN message handle.
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the CAN message handle is NULL or
*   - #DSCAN_ERROR_INVALID_MSG_TYPE if the message handle is not valid.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_stop(DsTCanMsg Msg)
{
    UInt32 Status;

    /* Check for invalid function arguments */
    if (NULL == Msg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Msg", __FUNCTION__);
    }

    if (DSCAN_MSG_TYPE_TX_QUEUE == Msg->MsgType)
    {
        RTLIB_INT_SAVE_AND_DISABLE(Status);
        Msg->IsEnabled = DSCAN_MSG_STATE_INACTIVE;
        RTLIB_INT_RESTORE(Status);
    }
    else if (DSCAN_MSG_TYPE_RX_MONITOR == Msg->MsgType)
    {
        if (DSCAN_MSG_STATE_ACTIVE == Msg->IsEnabled)
        {
            RTLIB_INT_SAVE_AND_DISABLE(Status);

            Msg->IsEnabled = DSCAN_MSG_STATE_INACTIVE;

            /* Monitor buffer gets cleaned when message is enabled */
            dsmcr_client_disconnect(Msg->BufferIndex, Msg->ClientNumber);

            RTLIB_INT_RESTORE(Status);
        }
    }
    else
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_TYPE, g_DsCanDefaultDescriptor, __FUNCTION__, Msg->MsgType);
    }

    return DSCAN_NO_ERROR;
}

/** <!------------------------------------------------------------------------->
*  Returns the number of queue elements for a transmit queue message object.
*
*   @description
*   - This function returns the current number of elements within a transmit queue.
*   - To get the most recent results call DsCanBoard_update() periodically.
*
*   See also:
*   - DsCanMsg_createTxQueue()
*
*   @parameters
*       @param Msg A CAN message handle.
*       @param pQueueCount The current number of elements.
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the CAN message handle is NULL or
*   - #DSCAN_ERROR_INVALID_MSG_TYPE if the message has the wrong type.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_getTxQueueCount(DsTCanMsg Msg, UInt32* pQueueCount)
{
    UInt32 Status;

    /* Check for valid function arguments */
    if (NULL == Msg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Msg", __FUNCTION__);
    }

    if (NULL == pQueueCount)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "pQueueCount", __FUNCTION__);
    }

    /* Check for proper message type */
    if (DSCAN_MSG_TYPE_TX_QUEUE != Msg->MsgType)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_TYPE, g_DsCanDefaultDescriptor, __FUNCTION__, Msg->MsgType);
    }

    RTLIB_INT_SAVE_AND_DISABLE(Status);
    *pQueueCount = Msg->TxQueueCount;
    RTLIB_INT_RESTORE(Status);

    return DSCAN_NO_ERROR;
}

/** <!------------------------------------------------------------------------->
*  Returns the selected internal message handle.
*
*   @description
*   - This function is not generic for all CAN platforms and has been introduced
*     due to legacy issues.
*   - It returns the internally used platform specific CAN RTLIB message handles.
*   - Avoid using this function.
*
*   @parameters
*       @param Msg A CAN message handle.
*       @param pQueueCount The current number of elements.
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the CAN channel handle or ppMsg is NULL or
*   - #DSCAN_ERROR_INVALID_MSG_FORMAT if a wrong message format was used or
*   - #DSCAN_ERROR_INVALID_MSG_DIR if a wrong message direction was used.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanCh_getInternalMsgHandle(DsTCanCh Ch, DsECanMsg_Format Format, DsECanMsg_Direction Dir, void** ppMsg)
{
    UInt32 BoardNumber, ChannelNumber;
    DsECanBoard_Type Type;
    DsTError Error;
    
    /* Check for valid function arguments */
    if (NULL == Ch)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Ch", __FUNCTION__);
    }
    else if (NULL == ppMsg)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "ppMsg", __FUNCTION__);
    }
    else if (! ((DSCAN_MSG_FORMAT_STD == Format) ||(DSCAN_MSG_FORMAT_EXT == Format)))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_FORMAT, g_DsCanDefaultDescriptor, Format, __FUNCTION__);
    }
    else if (! ((DSCAN_MSG_DIR_TRANSMIT == Dir) ||(DSCAN_MSG_DIR_RECEIVE == Dir)))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_INVALID_MSG_DIR, g_DsCanDefaultDescriptor, Dir, __FUNCTION__);
    }
    else
    {
        Error = DsCan_getChannelProperties(Ch, &BoardNumber, &ChannelNumber, &Type);

        if (DSCAN_NO_ERROR != Error)
        {
            DSCAN_DEBUG();
            return Error;
        }
        
        if (DSCAN_MSG_DIR_TRANSMIT == Dir)
        {
            if (NULL == g_DsCanTxMsgs[BoardNumber][ChannelNumber])
            {
                *ppMsg = NULL;
            }
            else
            {
                if (DSCAN_MSG_FORMAT_STD == Format)
                {
                    *ppMsg = g_DsCanTxMsgs[BoardNumber][ChannelNumber]->pStdMsg;
                }
                else /* DSCAN_MSG_FORMAT_EXT */
                {
                    *ppMsg = g_DsCanTxMsgs[BoardNumber][ChannelNumber]->pExtMsg;
                }
            }
        }
        else /* DSCAN_MSG_DIR_RECEIVE */
        {
            if (0 == g_DsCanNumRxMonitors[BoardNumber][ChannelNumber])
            {
                *ppMsg = NULL;
            }
            else
            {
                if (DSCAN_MSG_FORMAT_STD == Format)
                {
                    *ppMsg = g_DsCanRxMsgs[BoardNumber][ChannelNumber][0]->pStdMsg;
                }
                else /* DSCAN_MSG_FORMAT_EXT */
                {
                    *ppMsg = g_DsCanRxMsgs[BoardNumber][ChannelNumber][0]->pExtMsg;
                }
            }
        }

        return DSCAN_NO_ERROR;
    }
}

/** <!------------------------------------------------------------------------->
*   Initializes the communication between CN and ION.
*
*   @description
*   - This function initializes the communication channel with the specified
*     target board.
*   - Multiple calls of this function with the same target as argument (which
*     means identical pointer into the resource map) will return the same
*     board handle.
*   - A valid board handle is required to setup CAN channels and all further
*     DsCan objects.
*
*   See also:
*   - DsCanBoard_update()
*
*   @parameters
*       @param pBoard Returned pointer to the CAN board handle.
*       @param Address The bus board address.
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the pointer to the board handle or the board
*     address is NULL or
*   - #DSCAN_ERROR_MEMORY_ALLOCATION if an memory allocation error occurred or
*   - #DSCAN_ERROR_OBJECT_REUSED if the board already exists or
*   - #DSCAN_ERROR_NODE_NOT_EXISTS if the target board is not present or
*   - #DSCAN_ERROR_COMMUNICATION if a communication error occurred.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanBoard_create(DsTCanBoard* pBoard, DsTCanBoard_Address Address)
{
    DsTError Error = DSCAN_NO_ERROR;
    UInt32 BoardNumber;
    DsECanBoard_Type Type;
    DsSCanBoard* pNewBoard;

    if (NULL == pBoard)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "pBoard", __FUNCTION__);
    }

    *pBoard = NULL;

    Error = DsCan_initBoard(Address, &Type, &BoardNumber);

    if (DSCAN_NO_ERROR != Error)
    {
        DSCAN_DEBUG();
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NODE_NOT_EXISTS, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    if (NULL == g_DsCanBoards[BoardNumber])
    {
        pNewBoard = (DsSCanBoard*)rtlib_malloc(sizeof(DsSCanBoard));

        if (NULL == pNewBoard)
        {
            DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
        }

        pNewBoard->BoardNumber = BoardNumber;
        pNewBoard->Type        = Type;
    }
    else
    {
        pNewBoard = g_DsCanBoards[BoardNumber];
        Error = DSCAN_ERROR_OBJECT_REUSED;
    }

    *pBoard = pNewBoard;

    return Error;
}

/** <!------------------------------------------------------------------------->
*   Processes the incoming messages for all channels of a board.
*
*   @description
*   - This function processes all new messages received from any connected
*     IO nodes and invokes their corresponding service handlers.
*     The messages of all CAN channels on the specified target board are
*     processed with one call to this function. But don't rely on that behaviour
*     and call DsCanBoard_update() for every created board handle.
*   - Make sure to call this function recurrently (e.g. at the beginning of
*     each task) to always work with the latest bus information and avoid
*     overruns of the limited communication buffer.
*
*   See also:
*   - DsCanBoard_create()
*
*   @parameters
*       @param Board A DsCAN board handle.
*
*   @return
*   - #DSCAN_NO_ERROR or
*   - #DSCAN_ERROR_NULL_POINTER if the board handle is NULL or
*   - #DSCAN_ERROR_COMMUNICATION if a communication error occurred.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanBoard_update(DsTCanBoard Board)
{
    if (NULL == Board)
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_NULL_POINTER, g_DsCanDefaultDescriptor, "Board", __FUNCTION__);
    }

    return DsCan_update(Board);
}

/******************************************************************************
 * Platform specific helper functions
 * The functions below do whatever is necessary to receive and transmit frames
 * on a specific platform.
 *****************************************************************************/

#if defined _DS1005 || defined _DS1006

/**************************************
 * DS4302
 *************************************/

void DsCan_rxCbDs4302(ds4302_canMsg* pMsg, UInt32* pData);

static void DsCan_initBoardDs4302(UInt32 Base, UInt32* pBoardNumber)
{
    ds4302_init(Base);
    ds4302_can_communication_init(Base, DS4302_CAN_INT_ENABLE);
    *pBoardNumber = PHS_INDEX_GET(Base);
}

static DsTError DsCan_initRxDs4302(ds4302_canChannel* pCh, ds4302_canMsg** ppStdRxMsg, ds4302_canMsg** ppExtRxMsg)
{
    Int32 Error;
    ds4302_canMsg* pTempStd;
    ds4302_canMsg* pTempExt;
    UInt32 DefaultMask = 0;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds4302_can_msg_rx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS4302_CAN_STD,
                                          DS4302_CAN_TIMECOUNT_INFO | DS4302_CAN_DATA_INFO | DS4302_CAN_MSG_INFO,
                                          DS4302_CAN_NO_SUBINT);

    pTempExt = ds4302_can_msg_rx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS4302_CAN_EXT,
                                          DS4302_CAN_TIMECOUNT_INFO | DS4302_CAN_DATA_INFO | DS4302_CAN_MSG_INFO,
                                          DS4302_CAN_NO_SUBINT);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs4302;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs4302;
    
    *ppStdRxMsg = pTempStd;
    *ppExtRxMsg = pTempExt;

    Error = ds4302_can_msg_set(pTempStd, DS4302_CAN_MSG_MASK, &DefaultMask);

    if (DS4302_CAN_NO_ERROR != Error)
    {
        DSCAN_DEBUG();
        /* msg_error_printf(...) */
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds4302_can_msg_set(pTempExt, DS4302_CAN_MSG_MASK, &DefaultMask);

    if (DS4302_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_initTxDs4302(ds4302_canChannel* pCh, ds4302_canMsg** ppStdTxMsg, ds4302_canMsg** ppExtTxMsg)
{
    Int32 Error;
    ds4302_canMsg* pTempStd;
    ds4302_canMsg* pTempExt;

    *ppStdTxMsg = NULL;
    *ppExtTxMsg = NULL;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds4302_can_msg_tx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS4302_CAN_STD,
                                          DS4302_CAN_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO |DS4302_CAN_MSG_INFO,
                                          DS4302_CAN_NO_SUBINT,
                                          0,
                                          DS4302_CAN_TRIGGER_MSG,
                                          DS4302_CAN_TIMEOUT_NORMAL);

    pTempExt = ds4302_can_msg_tx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS4302_CAN_EXT,
                                          DS4302_CAN_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | DS4302_CAN_MSG_INFO,
                                          DS4302_CAN_NO_SUBINT,
                                          0,
                                          DS4302_CAN_TRIGGER_MSG,
                                          DS4302_CAN_TIMEOUT_NORMAL);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs4302;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs4302;

    *ppStdTxMsg = pTempStd;
    *ppExtTxMsg = pTempExt;

    Error = ds4302_can_msg_txqueue_init(pTempStd, DS4302_CAN_TXQUEUE_OVERRUN_IGNORE, 0.0);

    if (DS4302_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds4302_can_msg_txqueue_init(pTempExt, DS4302_CAN_TXQUEUE_OVERRUN_IGNORE, 0.0);

    if (DS4302_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_transmitDs4302(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem)
{
    UInt32 Data[DSCAN_MSG_MAX_DLC];
    DsTError Error;
    UInt32 i;

    for(i = 0; i < DSCAN_MSG_MAX_DLC; i++)
    {
        Data[i] = pItem->Data[i];
    }

    if (DSCAN_MSG_FORMAT_STD == pItem->Format)
    {
        Error = ds4302_can_msg_send_id_queued((ds4302_canMsg*)pMsg->pStdMsg, pItem->Id, pItem->Dlc, Data);
    }
    else
    {
        Error = ds4302_can_msg_send_id_queued((ds4302_canMsg*)pMsg->pExtMsg, pItem->Id, pItem->Dlc, Data);
    }

    if (DS4302_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_updateDs4302(UInt32 BoardNumber)
{
    if (DS4302_CAN_NO_ERROR != ds4302_can_msg_update_all(BoardNumber, DSCAN_SPECIAL_DPMEM_QUEUE))
    {
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

extern double ds4302_time_convert (const UInt32 timecount, UInt32 wrapcount);

extern UInt32 can_len_arr[];

static void DsCan_unpackBlockDs4302(UInt32 *packed_data, UInt8* unpacked_data, UInt32 size)
{
    UInt32 x;
    UInt32 len;

    len = (UInt32) can_len_arr[size];

    switch( len )
    {
        case 1 :
        {
            unpacked_data[ 0 ]  = (UInt8)(( packed_data[ 0 ] & 0x000000FF ) >> 0);
            unpacked_data[ 1 ]  = (UInt8)(( packed_data[ 0 ] & 0x0000FF00 ) >> 8);
            unpacked_data[ 2 ]  = (UInt8)(( packed_data[ 0 ] & 0x00FF0000 ) >> 16);
            unpacked_data[ 3 ]  = (UInt8)(( packed_data[ 0 ] & 0xFF000000 ) >> 24);
            break;
        }
        case 2 :
        {
            unpacked_data[ 0 ]  = (UInt8)(( packed_data[ 0 ] & 0x000000FF ) >> 0);
            unpacked_data[ 1 ]  = (UInt8)(( packed_data[ 0 ] & 0x0000FF00 ) >> 8);
            unpacked_data[ 2 ]  = (UInt8)(( packed_data[ 0 ] & 0x00FF0000 ) >> 16);
            unpacked_data[ 3 ]  = (UInt8)(( packed_data[ 0 ] & 0xFF000000 ) >> 24);
            unpacked_data[ 4 ]  = (UInt8)(( packed_data[ 1 ] & 0x000000FF ) >> 0);
            unpacked_data[ 5 ]  = (UInt8)(( packed_data[ 1 ] & 0x0000FF00 ) >> 8);
            unpacked_data[ 6 ]  = (UInt8)(( packed_data[ 1 ] & 0x00FF0000 ) >> 16);
            unpacked_data[ 7 ]  = (UInt8)(( packed_data[ 1 ] & 0xFF000000 ) >> 24);
            break;
        }
    }
    
    /* Zero out unused bytes */
    for(x = size; x < 8; x++)
    {
        unpacked_data[x] = 0;
    }
}

void DsCan_rxCbDs4302(ds4302_canMsg* pMsg, UInt32* pData)
{
    DsSCanMsg_Item Item;
    UInt32 Index = 0;
    UInt32 TimeCount, WrapCount;
    DsTError Error;

    /* Called when frame is read for DsCan message
     * Converts it to DsSCanMsg_Item structure
     * Places it in rx monitor buffer
     * For each received loop-back information decrements "tx elements used" counter
     * Updates processing count of affected tx messages
     * Used code from ds4302_can_msg_update_all(...) */
    if (DS4302_CAN_TX == pMsg->type)
    {
        if (NULL != g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel])
        {
            Error = dsmcr_read(g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->BufferIndex,
                               g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ClientNumber,
                               (void*)&Item);
        }
        else
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if (DSMCR_BUFFER_EMPTY == Error)
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if (Item.Id != (pData[Index] & 0x1FFFFFFF))
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if((pData[Index] & 0x40000000) >> 30)
        {
            Item.Status = DSCAN_MSG_NEW;
        }
        else
        {
            Item.Status = DSCAN_MSG_LOST;
        }

        Index++;

        TimeCount = pData[Index++];
        WrapCount = pData[Index++];

        Item.TimeStamp = ds4302_time_convert(TimeCount, WrapCount);

        Item.Dir = DSCAN_MSG_DIR_TRANSMIT;

        g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ProcCount++;

        if (0==g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount)
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
        }
        else
        {
            g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount--;
        }
    }
    else
    {
        Item.Id = (pData[Index] & 0x1FFFFFFF);

        if (0 != (pData[Index] & 0x20000000))
        {
            Item.Format = DSCAN_MSG_FORMAT_EXT;
        }
        else
        {
            Item.Format = DSCAN_MSG_FORMAT_STD;
        }

        Index++;

        TimeCount = pData[Index++];
        WrapCount = pData[Index++];

        Item.TimeStamp = ds4302_time_convert(TimeCount, WrapCount);

        Item.Dlc = pData[Index++];

        DsCan_unpackBlockDs4302(&pData[Index], Item.Data, Item.Dlc);

        Item.Dir = DSCAN_MSG_DIR_RECEIVE;
        Item.Status = DSCAN_MSG_NEW;
    }

    Item.IsEnabled = DSCAN_MSG_STATE_ACTIVE;
    Item.ProcCount = 0;
    Item.Rtr = DSCAN_MSG_RTR_DISABLED;

    if (NULL != g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel])
    {
        dsmcr_write(g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel][0]->BufferIndex, (void*)&Item);
    }
}

/**************************************
 * DS2211
 *************************************/

void DsCan_rxCbDs2211(ds2211_canMsg* pMsg, UInt32* pData);

static void DsCan_initBoardDs2211(UInt32 Base, UInt32* pBoardNumber)
{
    ds2211_init(Base);
    ds2211_can_communication_init(Base, DS2211_CAN_INT_ENABLE);
    *pBoardNumber = PHS_INDEX_GET(Base);
}

static DsTError DsCan_initRxDs2211(ds2211_canChannel* pCh, ds2211_canMsg** ppStdRxMsg, ds2211_canMsg** ppExtRxMsg)
{
    Int32 Error;
    ds2211_canMsg* pTempStd;
    ds2211_canMsg* pTempExt;
    UInt32 DefaultMask = 0;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds2211_can_msg_rx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2211_CAN_STD,
                                          DS2211_CAN_TIMECOUNT_INFO | DS2211_CAN_DATA_INFO | DS2211_CAN_MSG_INFO,
                                          DS2211_CAN_NO_SUBINT);

    pTempExt = ds2211_can_msg_rx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2211_CAN_EXT,
                                          DS2211_CAN_TIMECOUNT_INFO | DS2211_CAN_DATA_INFO | DS2211_CAN_MSG_INFO,
                                          DS2211_CAN_NO_SUBINT);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs2211;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs2211;
    
    *ppStdRxMsg = pTempStd;
    *ppExtRxMsg = pTempExt;

    Error = ds2211_can_msg_set(pTempStd, DS2211_CAN_MSG_MASK, &DefaultMask);

    if (DS2211_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds2211_can_msg_set(pTempExt, DS2211_CAN_MSG_MASK, &DefaultMask);

    if (DS2211_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_initTxDs2211(ds2211_canChannel* pCh, ds2211_canMsg** ppStdTxMsg, ds2211_canMsg** ppExtTxMsg)
{
    Int32 Error;
    ds2211_canMsg* pTempStd;
    ds2211_canMsg* pTempExt;

    *ppStdTxMsg = NULL;
    *ppExtTxMsg = NULL;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds2211_can_msg_tx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2211_CAN_STD,
                                          DS2211_CAN_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | DS2211_CAN_MSG_INFO,
                                          DS2211_CAN_NO_SUBINT,
                                          0,
                                          DS2211_CAN_TRIGGER_MSG,
                                          DS2211_CAN_TIMEOUT_NORMAL);

    pTempExt = ds2211_can_msg_tx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2211_CAN_EXT,
                                          DS2211_CAN_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | DS2211_CAN_MSG_INFO,
                                          DS2211_CAN_NO_SUBINT,
                                          0,
                                          DS2211_CAN_TRIGGER_MSG,
                                          DS2211_CAN_TIMEOUT_NORMAL);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs2211;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs2211;

    *ppStdTxMsg = pTempStd;
    *ppExtTxMsg = pTempExt;

    Error = ds2211_can_msg_txqueue_init(pTempStd,
                                        DS2211_CAN_TXQUEUE_OVERRUN_IGNORE,
                                        0.0);

    if (DS2211_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds2211_can_msg_txqueue_init(pTempExt,
                                        DS2211_CAN_TXQUEUE_OVERRUN_IGNORE,
                                        0.0);

    if (DS2211_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_transmitDs2211(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem)
{
    UInt32 Data[DSCAN_MSG_MAX_DLC];
    DsTError Error;
    UInt32 i;

    for(i = 0; i < DSCAN_MSG_MAX_DLC; i++)
    {
        Data[i] = pItem->Data[i];
    }

    if (DSCAN_MSG_FORMAT_STD == pItem->Format)
    {
        Error = ds2211_can_msg_send_id_queued((ds2211_canMsg*)pMsg->pStdMsg, pItem->Id, pItem->Dlc, Data);
    }
    else
    {
        Error = ds2211_can_msg_send_id_queued((ds2211_canMsg*)pMsg->pExtMsg, pItem->Id, pItem->Dlc, Data);
    }

    if (DS2211_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_updateDs2211(UInt32 BoardNumber)
{
    if (DS2211_CAN_NO_ERROR != ds2211_can_msg_update_all(BoardNumber,
                                                         DSCAN_SPECIAL_DPMEM_QUEUE))
    {
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

extern Float64 ds2211_can_tcount2tstamp(const UInt32 module,
                                        const UInt32 timecount,
                                        const UInt32 wrapcount);

extern UInt32 ds2211_can_len_arr[];

static void DsCan_unpackBlockDs2211(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size)
{
    UInt32 x;
    UInt32 len;

    len = (UInt32) ds2211_can_len_arr[size];

    switch( len )
    {
        case 1 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            break;
        }
        case 2 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            break;
        }
        case 3 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            unpacked_data[4] = ( packed_data[2] & 0x000000FF );
            unpacked_data[5] = ( packed_data[2] & 0x0000FF00 ) >> 8;
            break;
        }
        case 4 :
        {
            for(x = 0; x < len; x++)
            {
                unpacked_data[x * 2]       = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[(x * 2) + 1] = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
        default :
        {
            len = 4;
            for(x = 0; x < len; x++)
            {
                unpacked_data[ x * 2 ]        = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[ (x * 2) + 1 ]  = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
    }
    
    /* Zero out unused bytes */
    for(x = size; x < 8; x++)
    {
        unpacked_data[x] = 0;
    }
}

void DsCan_rxCbDs2211(ds2211_canMsg* pMsg, UInt32* pData)
{
    DsSCanMsg_Item Item;
    UInt32 Index = 0;
    UInt32 TimeCount, WrapCount;
    DsTError Error;

    /* Called when frame is read for DsCan message
     * Converts it to DsSCanMsg_Item structure
     * Places it in rx monitor buffer
     * For each received loop-back information decrements "tx elements used" counter
     * Updates processing count of affected tx messages
     * Used code from ds2211_can_msg_update_all(...) */
    if (DS2211_CAN_TX == pMsg->type)
    {
        UInt32 TmpId;
    
        if (NULL != g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel])
        {
            Error = dsmcr_read(g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->BufferIndex,
                               g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ClientNumber,
                               (void*)&Item);
        }
        else
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if (DSMCR_BUFFER_EMPTY == Error)
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        TmpId = (pData[Index++] & 0xFFFF);
        
        TmpId |=  (pData[Index++] << 16);

        if (Item.Id != (TmpId & 0x1FFFFFFF))
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }
        
        if((TmpId & 0x40000000) >> 30)
        {
            Item.Status = DSCAN_MSG_NEW;
        }
        else
        {
            Item.Status = DSCAN_MSG_LOST;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = ds2211_can_tcount2tstamp(pMsg->module, TimeCount, WrapCount);

        Item.Dir = DSCAN_MSG_DIR_TRANSMIT;

        g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ProcCount++;

        if (0==g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount)
        {
            DSCAN_DEBUG();
            /* msg_error_printf(...) */
        }
        else
        {
            g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount--;
        }
    }
    else
    {
        Item.Id = pData[Index++] & 0xFFFF;
        Item.Id |= (pData[Index++] << 16);

        if (0 != pData[Index++])
        {
            Item.Format = DSCAN_MSG_FORMAT_EXT;
        }
        else
        {
            Item.Format = DSCAN_MSG_FORMAT_STD;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = ds2211_can_tcount2tstamp(pMsg->module, TimeCount, WrapCount);

        Item.Dlc = pData[Index++];
        DsCan_unpackBlockDs2211(&pData[Index], Item.Data, Item.Dlc);

        Item.Dir = DSCAN_MSG_DIR_RECEIVE;
        Item.Status = DSCAN_MSG_NEW;
    }

    Item.IsEnabled = DSCAN_MSG_STATE_ACTIVE;
    Item.ProcCount = 0;
    Item.Rtr = DSCAN_MSG_RTR_DISABLED;

    if (NULL != g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel])
    {
        dsmcr_write(g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel][0]->BufferIndex,
                    (void*)&Item);
    }
}

/**************************************
 * DS2210
 *************************************/

void DsCan_rxCbDs2210(ds2210_canMsg* pMsg, UInt32* pData);

static void DsCan_initBoardDs2210(UInt32 Base, UInt32* pBoardNumber)
{
    ds2210_init(Base);
    ds2210_can_communication_init(Base, DS2210_CAN_INT_ENABLE);
    *pBoardNumber = PHS_INDEX_GET(Base);
}

static DsTError DsCan_initRxDs2210(ds2210_canChannel* pCh, ds2210_canMsg** ppStdRxMsg, ds2210_canMsg** ppExtRxMsg)
{
    Int32 Error;
    ds2210_canMsg* pTempStd;
    ds2210_canMsg* pTempExt;
    UInt32 DefaultMask = 0;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds2210_can_msg_rx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2210_CAN_STD,
                                          DS2210_CAN_TIMECOUNT_INFO | DS2210_CAN_DATA_INFO | DS2210_CAN_MSG_INFO,
                                          DS2210_CAN_NO_SUBINT);

    pTempExt = ds2210_can_msg_rx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2210_CAN_EXT,
                                          DS2210_CAN_TIMECOUNT_INFO | DS2210_CAN_DATA_INFO | DS2210_CAN_MSG_INFO,
                                          DS2210_CAN_NO_SUBINT);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs2210;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs2210;
    
    *ppStdRxMsg = pTempStd;
    *ppExtRxMsg = pTempExt;

    Error = ds2210_can_msg_set(pTempStd, DS2210_CAN_MSG_MASK, &DefaultMask);

    if (DS2210_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds2210_can_msg_set(pTempExt, DS2210_CAN_MSG_MASK, &DefaultMask);

    if (DS2210_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_initTxDs2210(ds2210_canChannel* pCh, ds2210_canMsg** ppStdTxMsg, ds2210_canMsg** ppExtTxMsg)
{
    Int32 Error;
    ds2210_canMsg* pTempStd;
    ds2210_canMsg* pTempExt;

    *ppStdTxMsg = NULL;
    *ppExtTxMsg = NULL;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds2210_can_msg_tx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2210_CAN_STD,
                                          DS2210_CAN_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | DS2210_CAN_MSG_INFO,
                                          DS2210_CAN_NO_SUBINT,
                                          0,
                                          DS2210_CAN_TRIGGER_MSG,
                                          DS2210_CAN_TIMEOUT_NORMAL);

    pTempExt = ds2210_can_msg_tx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2210_CAN_EXT,
                                          DS2210_CAN_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | DS2210_CAN_MSG_INFO,
                                          DS2210_CAN_NO_SUBINT,
                                          0,
                                          DS2210_CAN_TRIGGER_MSG,
                                          DS2210_CAN_TIMEOUT_NORMAL);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs2210;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs2210;
    
    *ppStdTxMsg = pTempStd;
    *ppExtTxMsg = pTempExt;

    Error = ds2210_can_msg_txqueue_init(pTempStd,
                                        DS2210_CAN_TXQUEUE_OVERRUN_IGNORE,
                                        0.0);

    if (DS2210_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds2210_can_msg_txqueue_init(pTempExt,
                                        DS2210_CAN_TXQUEUE_OVERRUN_IGNORE,
                                        0.0);

    if (DS2210_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_transmitDs2210(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem)
{
    UInt32 Data[DSCAN_MSG_MAX_DLC];
    DsTError Error;
    UInt32 i;

    for(i = 0; i < DSCAN_MSG_MAX_DLC; i++)
    {
        Data[i] = pItem->Data[i];
    }

    if (DSCAN_MSG_FORMAT_STD == pItem->Format)
    {
        Error = ds2210_can_msg_send_id_queued((ds2210_canMsg*)pMsg->pStdMsg, pItem->Id, pItem->Dlc, Data);
    }
    else
    {
        Error = ds2210_can_msg_send_id_queued((ds2210_canMsg*)pMsg->pExtMsg, pItem->Id, pItem->Dlc, Data);
    }

    if (DS2210_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_updateDs2210(UInt32 BoardNumber)
{
    if (DS2210_CAN_NO_ERROR != ds2210_can_msg_update_all(BoardNumber,
                                                         DSCAN_SPECIAL_DPMEM_QUEUE))
    {
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

extern double ds2210_can_time_convert(const UInt32 timecount, UInt32 wrapcount);

extern UInt32 ds2210_can_len_arr[];

static void DsCan_unpackBlockDs2210(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size)
{
    UInt32 x;
    UInt32 len;

    len = (UInt32) ds2210_can_len_arr[size];

    switch( len )
    {
        case 1 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            break;
        }
        case 2 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            break;
        }
        case 3 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            unpacked_data[4] = ( packed_data[2] & 0x000000FF );
            unpacked_data[5] = ( packed_data[2] & 0x0000FF00 ) >> 8;
            break;
        }
        case 4 :
        {
            for(x = 0; x < len; x++)
            {
                unpacked_data[x * 2]       = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[(x * 2) + 1] = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
        default :
        {
            len = 4;
            for(x = 0; x < len; x++)
            {
                unpacked_data[ x * 2 ]        = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[ (x * 2) + 1 ]  = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
    }
    
    /* Zero out unused bytes */
    for(x = size; x < 8; x++)
    {
        unpacked_data[x] = 0;
    }
}

void DsCan_rxCbDs2210(ds2210_canMsg* pMsg, UInt32* pData)
{
    DsSCanMsg_Item Item;
    UInt32 Index = 0;
    UInt32 TimeCount, WrapCount;
    DsTError Error;

    /* Called when frame is read for DsCan message
     * Converts it to DsSCanMsg_Item structure
     * Places it in rx monitor buffer
     * For each received loop-back information decrements "tx elements used" counter
     * Updates processing count of affected tx messages
     * Used code from ds2211_can_msg_update_all(...) */
    if (DS2210_CAN_TX == pMsg->type)
    {  
        UInt32 TmpId; 

        if (NULL != g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel])
        {
            Error = dsmcr_read(g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->BufferIndex,
                               g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ClientNumber,
                               (void*)&Item);
        }
        else
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if (DSMCR_BUFFER_EMPTY == Error)
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        TmpId = (pData[Index++] & 0xFFFF);
        
        TmpId |=  (pData[Index++] << 16);
        
        if (Item.Id != (TmpId & 0x1FFFFFFF))
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }
        
        if((TmpId & 0x40000000) >> 30)
        {
            Item.Status = DSCAN_MSG_NEW;
        }
        else
        {
            Item.Status = DSCAN_MSG_LOST;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = ds2210_can_time_convert(TimeCount, WrapCount);

        Item.Dir = DSCAN_MSG_DIR_TRANSMIT;

        g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ProcCount++;

        if (0==g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount)
        {
            DSCAN_DEBUG();
            /* msg_error_printf(...) */
        }
        else
        {
            g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount--;
        }
    }
    else
    {
        Item.Id = pData[Index++] & 0xFFFF;
        Item.Id |= (pData[Index++] << 16);

        if (0 != pData[Index++])
        {
            Item.Format = DSCAN_MSG_FORMAT_EXT;
        }
        else
        {
            Item.Format = DSCAN_MSG_FORMAT_STD;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = ds2210_can_time_convert(TimeCount, WrapCount);

        Item.Dlc = pData[Index++];
        DsCan_unpackBlockDs2210(&pData[Index], Item.Data, Item.Dlc);

        Item.Dir = DSCAN_MSG_DIR_RECEIVE;
        Item.Status = DSCAN_MSG_NEW;
    }

    Item.IsEnabled = DSCAN_MSG_STATE_ACTIVE;
    Item.ProcCount = 0;
    Item.Rtr = DSCAN_MSG_RTR_DISABLED;

    if (NULL != g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel])
    {
        dsmcr_write(g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel][0]->BufferIndex,
                    (void*)&Item);
    }
}

/**************************************
 * DS2202
 *************************************/

void DsCan_rxCbDs2202(ds2202_canMsg* pMsg, UInt32* pData);

static void DsCan_initBoardDs2202(UInt32 Base, UInt32* pBoardNumber)
{
    ds2202_init(Base);
    ds2202_can_communication_init(Base, DS2202_CAN_INT_ENABLE);
    *pBoardNumber = PHS_INDEX_GET(Base);
}

static DsTError DsCan_initRxDs2202(ds2202_canChannel* pCh, ds2202_canMsg** ppStdRxMsg, ds2202_canMsg** ppExtRxMsg)
{
    Int32 Error;
    ds2202_canMsg* pTempStd;
    ds2202_canMsg* pTempExt;
    UInt32 DefaultMask = 0;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds2202_can_msg_rx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2202_CAN_STD,
                                          DS2202_CAN_TIMECOUNT_INFO | DS2202_CAN_DATA_INFO | DS2202_CAN_MSG_INFO,
                                          DS2202_CAN_NO_SUBINT);

    pTempExt = ds2202_can_msg_rx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2202_CAN_EXT,
                                          DS2202_CAN_TIMECOUNT_INFO | DS2202_CAN_DATA_INFO | DS2202_CAN_MSG_INFO,
                                          DS2202_CAN_NO_SUBINT);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs2202;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs2202;
    
    *ppStdRxMsg = pTempStd;
    *ppExtRxMsg = pTempExt;

    Error = ds2202_can_msg_set(pTempStd, DS2202_CAN_MSG_MASK, &DefaultMask);

    if (DS2202_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds2202_can_msg_set(pTempExt, DS2202_CAN_MSG_MASK, &DefaultMask);

    if (DS2202_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_initTxDs2202(ds2202_canChannel* pCh, ds2202_canMsg** ppStdTxMsg, ds2202_canMsg** ppExtTxMsg)
{
    Int32 Error;
    ds2202_canMsg* pTempStd;
    ds2202_canMsg* pTempExt;

    *ppStdTxMsg = NULL;
    *ppExtTxMsg = NULL;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds2202_can_msg_tx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2202_CAN_STD,
                                          DS2202_CAN_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | DS2202_CAN_MSG_INFO,
                                          DS2202_CAN_NO_SUBINT,
                                          0,
                                          DS2202_CAN_TRIGGER_MSG,
                                          DS2202_CAN_TIMEOUT_NORMAL);

    pTempExt = ds2202_can_msg_tx_register(pCh,
                                          DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS2202_CAN_EXT,
                                          DS2202_CAN_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | DS2202_CAN_MSG_INFO,
                                          DS2202_CAN_NO_SUBINT,
                                          0,
                                          DS2202_CAN_TRIGGER_MSG,
                                          DS2202_CAN_TIMEOUT_NORMAL);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs2202;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs2202;
    
    *ppStdTxMsg = pTempStd;
    *ppExtTxMsg = pTempExt;

    Error = ds2202_can_msg_txqueue_init(pTempStd,
                                        DS2202_CAN_TXQUEUE_OVERRUN_IGNORE,
                                        0.0);

    if (DS2202_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds2202_can_msg_txqueue_init(pTempExt,
                                        DS2202_CAN_TXQUEUE_OVERRUN_IGNORE,
                                        0.0);

    if (DS2202_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_transmitDs2202(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem)
{
    UInt32 Data[DSCAN_MSG_MAX_DLC];
    DsTError Error;
    UInt32 i;

    for(i = 0; i < DSCAN_MSG_MAX_DLC; i++)
    {
        Data[i] = pItem->Data[i];
    }

    if (DSCAN_MSG_FORMAT_STD == pItem->Format)
    {
        Error = ds2202_can_msg_send_id_queued((ds2202_canMsg*)pMsg->pStdMsg, pItem->Id, pItem->Dlc, Data);
    }
    else
    {
        Error = ds2202_can_msg_send_id_queued((ds2202_canMsg*)pMsg->pExtMsg, pItem->Id, pItem->Dlc, Data);
    }

    if (DS2202_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_updateDs2202(UInt32 BoardNumber)
{
    if (DS2202_CAN_NO_ERROR != ds2202_can_msg_update_all(BoardNumber,
                                                         DSCAN_SPECIAL_DPMEM_QUEUE))
    {
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

extern Float64 ds2202_can_tcount2tstamp(const UInt32 module,
                                        const UInt32 timecount,
                                        const UInt32 wrapcount);

extern UInt32 ds2202_can_len_arr[];

static void DsCan_unpackBlockDs2202(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size)
{
    UInt32 x;
    UInt32 len;

    len = (UInt32) ds2202_can_len_arr[size];

    switch( len )
    {
        case 1 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            break;
        }
        case 2 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            break;
        }
        case 3 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            unpacked_data[4] = ( packed_data[2] & 0x000000FF );
            unpacked_data[5] = ( packed_data[2] & 0x0000FF00 ) >> 8;
            break;
        }
        case 4 :
        {
            for(x = 0; x < len; x++)
            {
                unpacked_data[x * 2]       = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[(x * 2) + 1] = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
        default :
        {
            len = 4;
            for(x = 0; x < len; x++)
            {
                unpacked_data[ x * 2 ]        = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[ (x * 2) + 1 ]  = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
    }
    
    /* Zero out unused bytes */
    for(x = size; x < 8; x++)
    {
        unpacked_data[x] = 0;
    }
}

void DsCan_rxCbDs2202(ds2202_canMsg* pMsg, UInt32* pData)
{
    DsSCanMsg_Item Item;
    UInt32 Index = 0;
    UInt32 TimeCount, WrapCount;
    DsTError Error;

    /* Called when frame is read for DsCan message
     * Converts it to DsSCanMsg_Item structure
     * Places it in rx monitor buffer
     * For each received loop-back information decrements "tx elements used" counter
     * Updates processing count of affected tx messages
     * Used code from ds2211_can_msg_update_all(...) */
    if (DS2202_CAN_TX == pMsg->type)
    {
        UInt32 TmpId;
        
        if (NULL != g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel])
        {
            Error = dsmcr_read(g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->BufferIndex,
                               g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ClientNumber,
                               (void*)&Item);
        }
        else
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if (DSMCR_BUFFER_EMPTY == Error)
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        TmpId = (pData[Index++] & 0xFFFF);
        
        TmpId |=  (pData[Index++] << 16);

        if (Item.Id != (TmpId & 0x1FFFFFFF))
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if((TmpId & 0x40000000) >> 30)
        {
            Item.Status = DSCAN_MSG_NEW;
        }
        else
        {
            Item.Status = DSCAN_MSG_LOST;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = ds2202_can_tcount2tstamp(pMsg->module, TimeCount, WrapCount);

        Item.Dir = DSCAN_MSG_DIR_TRANSMIT;

        g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ProcCount++;

        if (0==g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount)
        {
            DSCAN_DEBUG();
            /* msg_error_printf(...) */
        }
        else
        {
            g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount--;
        }
    }
    else
    {
        Item.Id = pData[Index++] & 0xFFFF;
        Item.Id |= (pData[Index++] << 16);

        if (0 != pData[Index++])
        {
            Item.Format = DSCAN_MSG_FORMAT_EXT;
        }
        else
        {
            Item.Format = DSCAN_MSG_FORMAT_STD;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = ds2202_can_tcount2tstamp(pMsg->module, TimeCount, WrapCount);

        Item.Dlc = pData[Index++];
        DsCan_unpackBlockDs2202(&pData[Index], Item.Data, Item.Dlc);

        Item.Dir = DSCAN_MSG_DIR_RECEIVE;
        Item.Status = DSCAN_MSG_NEW;
    }

    Item.IsEnabled = DSCAN_MSG_STATE_ACTIVE;
    Item.ProcCount = 0;
    Item.Rtr = DSCAN_MSG_RTR_DISABLED;

    if (NULL != g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel])
    {
        dsmcr_write(g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel][0]->BufferIndex,
                    (void*)&Item);
    }
}


#elif defined _DS1401

/**************************************
 * DS1401
 *************************************/

void DsCan_rxCbDs1401(can_tp1_canMsg* pMsg, UInt32* pData);

static void DsCan_initBoardDs1401(UInt32 Base, UInt32* pBoardNumber)
{
    UInt32 ModuleAddr = Base;
    ds1401_init();
    can_tp1_communication_init(Base, CAN_TP1_INT_ENABLE);
    *pBoardNumber = MODULE_NB;    
}

static DsTError DsCan_initRxDs1401(can_tp1_canChannel* pCh, can_tp1_canMsg** ppStdRxMsg, can_tp1_canMsg** ppExtRxMsg)
{
    Int32 Error;
    can_tp1_canMsg* pTempStd;
    can_tp1_canMsg* pTempExt;
    UInt32 DefaultMask = 0;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = can_tp1_msg_rx_register(pCh,
                                       DSCAN_SPECIAL_DPMEM_QUEUE,
                                       0,  /* identifier */
                                       CAN_TP1_STD,
                                       CAN_TP1_TIMECOUNT_INFO | CAN_TP1_DATA_INFO | CAN_TP1_MSG_INFO,
                                       CAN_TP1_NO_SUBINT);

    pTempExt = can_tp1_msg_rx_register(pCh,
                                       DSCAN_SPECIAL_DPMEM_QUEUE,
                                       0,  /* identifier */
                                       CAN_TP1_EXT,
                                       CAN_TP1_TIMECOUNT_INFO | CAN_TP1_DATA_INFO | CAN_TP1_MSG_INFO,
                                       CAN_TP1_NO_SUBINT);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs1401;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs1401;
    
    *ppStdRxMsg = pTempStd;
    *ppExtRxMsg = pTempExt;

    Error = can_tp1_msg_set(pTempStd, CAN_TP1_MSG_MASK, &DefaultMask);

    if (CAN_TP1_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = can_tp1_msg_set(pTempExt, CAN_TP1_MSG_MASK, &DefaultMask);

    if (CAN_TP1_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }
    
    return DSCAN_NO_ERROR;
}

static DsTError DsCan_initTxDs1401(can_tp1_canChannel* pCh, can_tp1_canMsg** ppStdTxMsg, can_tp1_canMsg** ppExtTxMsg)
{
    Int32 Error;
    can_tp1_canMsg* pTempStd;
    can_tp1_canMsg* pTempExt;

    *ppStdTxMsg = NULL;
    *ppExtTxMsg = NULL;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = can_tp1_msg_tx_register(pCh,
                                       DSCAN_SPECIAL_DPMEM_QUEUE,
                                       0,  /* identifier */
                                       CAN_TP1_STD,
                                       CAN_TP1_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | CAN_TP1_MSG_INFO,
                                       CAN_TP1_NO_SUBINT,
                                       0,
                                       CAN_TP1_TRIGGER_MSG,
                                       CAN_TP1_TIMEOUT_NORMAL);

    pTempExt = can_tp1_msg_tx_register(pCh,
                                       DSCAN_SPECIAL_DPMEM_QUEUE,
                                       0,  /* identifier */
                                       CAN_TP1_EXT,
                                       CAN_TP1_TIMECOUNT_INFO | DSXXXX_CAN_TIMEOUT_INFO | CAN_TP1_MSG_INFO,
                                       CAN_TP1_NO_SUBINT,
                                       0,
                                       CAN_TP1_TRIGGER_MSG,
                                       CAN_TP1_TIMEOUT_NORMAL);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs1401;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs1401;

    *ppStdTxMsg = pTempStd;
    *ppExtTxMsg = pTempExt;

    Error = can_tp1_msg_txqueue_init(pTempStd,
                                      CAN_TP1_TXQUEUE_OVERRUN_IGNORE,
                                      0.0);

    if (CAN_TP1_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = can_tp1_msg_txqueue_init(pTempExt,
                                     CAN_TP1_TXQUEUE_OVERRUN_IGNORE,
                                     0.0);

    if (CAN_TP1_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }
    
    return DSCAN_NO_ERROR;
}

static DsTError DsCan_transmitDs1401(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem)
{
    UInt32 Data[DSCAN_MSG_MAX_DLC];
    DsTError Error;
    UInt32 i;

    for(i = 0; i < DSCAN_MSG_MAX_DLC; i++)
    {
        Data[i] = pItem->Data[i];
    }

    if (DSCAN_MSG_FORMAT_STD == pItem->Format)
    {
        Error = can_tp1_msg_send_id_queued((can_tp1_canMsg*)pMsg->pStdMsg, pItem->Id, pItem->Dlc, Data);
    }
    else
    {
        Error = can_tp1_msg_send_id_queued((can_tp1_canMsg*)pMsg->pExtMsg, pItem->Id, pItem->Dlc, Data);
    }

    if (CAN_TP1_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_updateDs1401(UInt32 BoardNumber)
{
    if (CAN_TP1_NO_ERROR != can_tp1_msg_update_all(BoardNumber, DSCAN_SPECIAL_DPMEM_QUEUE))
    {
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

extern Float64 can_tp1_tcount2tstamp(const UInt32 ModuleAddr, const UInt32 timecount, UInt32 wrapcount);

extern UInt32 can_len_arr[];

static void DsCan_unpackBlockDs1401(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size)
{
    UInt32 x;
    UInt32 len;

    len = (UInt32) can_len_arr[size];

    switch( len )
    {
        case 1 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            break;
        }
        case 2 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            break;
        }
        case 3 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            unpacked_data[4] = ( packed_data[2] & 0x000000FF );
            unpacked_data[5] = ( packed_data[2] & 0x0000FF00 ) >> 8;
            break;
        }
        case 4 :
        {
            for(x = 0; x < len; x++)
            {
                unpacked_data[x * 2]       = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[(x * 2) + 1] = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }


            break;
        }
        default :
        {
            len = 4;
            for(x = 0; x < len; x++)
            {
                unpacked_data[ x * 2 ]        = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[ (x * 2) + 1 ]  = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
    }
    
    /* Zero out unused bytes */
    for(x = size; x < 8; x++)
    {
        unpacked_data[x] = 0;
    }
}

void DsCan_rxCbDs1401(can_tp1_canMsg* pMsg, UInt32* pData)
{
    DsSCanMsg_Item Item;
    UInt32 Index = 0;
    UInt32 TimeCount, WrapCount;
    DsTError Error;

    /* Called when frame is read for DsCan message
     * Converts it to DsSCanMsg_Item structure
     * Places it in rx monitor buffer
     * For each received loop-back information decrements "tx elements used" counter
     * Updates processing count of affected tx messages
     * Used code from ds2211_can_msg_update_all(...) */
    if (CAN_TP1_TX == pMsg->type)
    {   
        UInt32 TmpId;
    
        if (NULL != g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel])
        {
            Error = dsmcr_read(g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->BufferIndex,
                               g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ClientNumber,
                               (void*)&Item);
        }
        else
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if (DSMCR_BUFFER_EMPTY == Error)
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        TmpId = (pData[Index++] & 0xFFFF);
        
        TmpId |=  (pData[Index++] << 16);

        if (Item.Id != (TmpId & 0x1FFFFFFF))
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if((TmpId & 0x40000000) >> 30)
        {
            Item.Status = DSCAN_MSG_NEW;
        }
        else
        {
            Item.Status = DSCAN_MSG_LOST;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = can_tp1_tcount2tstamp(pMsg->canChannel->module_addr, TimeCount, WrapCount);

        Item.Dir = DSCAN_MSG_DIR_TRANSMIT;

        g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->ProcCount++;

        if (0==g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount)
        {
            DSCAN_DEBUG();
            /* msg_error_printf(...) */
        }
        else
        {
            g_DsCanTxMsgs[pMsg->module][pMsg->canChannel->channel]->TxQueueCount--;
        }
    }
    else
    {
        Item.Id = pData[Index++] & 0xFFFF;
        Item.Id |= (pData[Index++] << 16);

        if (0 != pData[Index++])
        {
            Item.Format = DSCAN_MSG_FORMAT_EXT;
        }
        else
        {
            Item.Format = DSCAN_MSG_FORMAT_STD;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = can_tp1_tcount2tstamp(pMsg->canChannel->module_addr, TimeCount, WrapCount);

        Item.Dlc = pData[Index++];
        DsCan_unpackBlockDs1401(&pData[Index], Item.Data, Item.Dlc);

        Item.Dir = DSCAN_MSG_DIR_RECEIVE;
        Item.Status = DSCAN_MSG_NEW;
    }

    Item.IsEnabled = DSCAN_MSG_STATE_ACTIVE;
    Item.ProcCount = 0;
    Item.Rtr = DSCAN_MSG_RTR_DISABLED;

    if (NULL != g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel])
    {
        dsmcr_write(g_DsCanRxMsgs[pMsg->module][pMsg->canChannel->channel][0]->BufferIndex,
                    (void*)&Item);
    }
}

#elif defined _DS1103

/**************************************
 * DS1103
 *************************************/

void DsCan_rxCbDs1103(ds1103_canMsg* pMsg, UInt32* pData);

static void DsCan_initBoardDs1103(UInt32 Base, UInt32* pBoardNumber)
{
    ds1103_init();
    *pBoardNumber = 0;
}

static DsTError DsCan_initRxDs1103(ds1103_canChannel* pCh, ds1103_canMsg** ppStdRxMsg, ds1103_canMsg** ppExtRxMsg)
{
    Int32 Error;
    ds1103_canMsg* pTempStd;
    ds1103_canMsg* pTempExt;
    UInt32 DefaultMask = 0;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds1103_can_msg_rx_register(DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS1103_CAN_STD,
                                          DS1103_CAN_TIMECOUNT_INFO | DS1103_CAN_DATA_INFO | DS1103_CAN_MSG_INFO,
                                          DS1103_CAN_NO_SUBINT);

    pTempExt = ds1103_can_msg_rx_register(DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS1103_CAN_EXT,
                                          DS1103_CAN_TIMECOUNT_INFO | DS1103_CAN_DATA_INFO | DS1103_CAN_MSG_INFO,
                                          DS1103_CAN_NO_SUBINT);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs1103;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs1103;

    *ppStdRxMsg = pTempStd;
    *ppExtRxMsg = pTempExt;
    
    Error = ds1103_can_msg_set(pTempStd, DS1103_CAN_MSG_MASK, &DefaultMask);

    if (DS1103_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds1103_can_msg_set(pTempExt, DS1103_CAN_MSG_MASK, &DefaultMask);

    if (DS1103_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_initTxDs1103(ds1103_canChannel* pCh, ds1103_canMsg** ppStdTxMsg, ds1103_canMsg** ppExtTxMsg)
{
    Int32 Error;
    ds1103_canMsg* pTempStd;
    ds1103_canMsg* pTempExt;

    *ppStdTxMsg = NULL;
    *ppExtTxMsg = NULL;

    /* Register messages for both formats (we are interested in all CAN frames) */
    pTempStd = ds1103_can_msg_tx_register(DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS1103_CAN_STD,
                                          DS1103_CAN_TIMECOUNT_INFO | DEF1103_CAN_PROCESSED_INFO | DS1103_CAN_MSG_INFO,
                                          DS1103_CAN_NO_SUBINT,
                                          0,
                                          DS1103_CAN_TRIGGER_MSG,
                                          DS1103_CAN_TIMEOUT_NORMAL);

    pTempExt = ds1103_can_msg_tx_register(DSCAN_SPECIAL_DPMEM_QUEUE,
                                          0,  /* identifier */
                                          DS1103_CAN_EXT,
                                          DS1103_CAN_TIMECOUNT_INFO | DEF1103_CAN_PROCESSED_INFO | DS1103_CAN_MSG_INFO,
                                          DS1103_CAN_NO_SUBINT,
                                          0,
                                          DS1103_CAN_TRIGGER_MSG,
                                          DS1103_CAN_TIMEOUT_NORMAL);

    if ((NULL == pTempStd) || (NULL == pTempExt))
    {
        DSCAN_ERROR_PRINTF(DSCAN_ERROR_MEMORY_ALLOCATION, g_DsCanDefaultDescriptor, __FUNCTION__);
    }

    pTempStd->pDsCanMsgCb = DsCan_rxCbDs1103;
    pTempExt->pDsCanMsgCb = DsCan_rxCbDs1103;

    *ppStdTxMsg = pTempStd;
    *ppExtTxMsg = pTempExt;

    Error = ds1103_can_msg_txqueue_init(pTempStd,
                                        DS1103_CAN_TXQUEUE_OVERRUN_IGNORE,
                                        0.0);

    if (DS1103_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    Error = ds1103_can_msg_txqueue_init(pTempExt,
                                        DS1103_CAN_TXQUEUE_OVERRUN_IGNORE,
                                        0.0);

    if (DS1103_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_transmitDs1103(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem)
{
    UInt32 Data[DSCAN_MSG_MAX_DLC];
    DsTError Error;
    UInt32 i;

    for(i = 0; i < DSCAN_MSG_MAX_DLC; i++)
    {
        Data[i] = pItem->Data[i];
    }

    if (DSCAN_MSG_FORMAT_STD == pItem->Format)
    {
        Error = ds1103_can_msg_send_id_queued((ds1103_canMsg*)pMsg->pStdMsg, pItem->Id, pItem->Dlc, Data);
    }
    else
    {
        Error = ds1103_can_msg_send_id_queued((ds1103_canMsg*)pMsg->pExtMsg, pItem->Id, pItem->Dlc, Data);
    }

    if (DS1103_CAN_NO_ERROR != Error)
    {
        /* msg_error_printf(...) */
        DSCAN_DEBUG();
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_updateDs1103(UInt32 BoardNumber)
{
    if (DS1103_CAN_NO_ERROR != ds1103_can_msg_update_all(DSCAN_SPECIAL_DPMEM_QUEUE))
    {
        return DSCAN_ERROR_COMMUNICATION;
    }

    return DSCAN_NO_ERROR;
}

extern Float64 ds1103_can_time_convert (const UInt32 timecount, UInt32 wrapcount);

extern UInt32 ds1103_can_len_arr[];

static void DsCan_unpackBlockDs1103(UInt32 *packed_data, UInt8 *unpacked_data, UInt32 size)
{
    UInt32 x;
    UInt32 len;

    len = (UInt32) ds1103_can_len_arr[size];

    switch( len )
    {
        case 1 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            break;
        }
        case 2 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            break;
        }
        case 3 :
        {
            unpacked_data[0] = ( packed_data[0] & 0x000000FF );
            unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
            unpacked_data[2] = ( packed_data[1] & 0x000000FF );
            unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
            unpacked_data[4] = ( packed_data[2] & 0x000000FF );
            unpacked_data[5] = ( packed_data[2] & 0x0000FF00 ) >> 8;
            break;
        }
        case 4 :
        {
            for(x = 0; x < len; x++)
            {
                unpacked_data[x * 2]       = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[(x * 2) + 1] = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
        default :
        {
            len = 4;
            for(x = 0; x < len; x++)
            {
                unpacked_data[ x * 2 ]        = ( packed_data[ x ] & 0x000000FF );
                unpacked_data[ (x * 2) + 1 ]  = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
            }

            break;
        }
    }
    
    /* Zero out unused bytes */
    for(x = size; x < 8; x++)
    {
        unpacked_data[x] = 0;
    }
}

void DsCan_rxCbDs1103(ds1103_canMsg* pMsg, UInt32* pData)
{
    DsSCanMsg_Item Item;
    UInt32 Index = 0;
    UInt32 TimeCount, WrapCount;
    DsTError Error;

    /* Called when frame is read for DsCan message
     * Converts it to DsSCanMsg_Item structure
     * Places it in rx monitor buffer
     * For each received loop-back information decrements "tx elements used" counter
     * Updates processing count of affected tx messages
     * Used code from ds1103_can_msg_update_all(...) */
    if (DS1103_CAN_TX == pMsg->type)
    {
        UInt32 TmpId;
         
        if (NULL != g_DsCanTxMsgs[0][0])
        {
            Error = dsmcr_read(g_DsCanTxMsgs[0][0]->BufferIndex,
                               g_DsCanTxMsgs[0][0]->ClientNumber,
                               (void*)&Item);
        }
        else
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        if (DSMCR_BUFFER_EMPTY == Error)
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        TmpId = (pData[Index++] & 0xFFFF);
        
        TmpId |=  (pData[Index++] << 16);

        if (Item.Id != (TmpId & 0x1FFFFFFF))
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            return;
        }

        /* The ds1103 transfers the format always as seperate field */        
        Index++; /* Format */

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = ds1103_can_time_convert(TimeCount, WrapCount);
        
        /* read the processed field */
        if(pData[Index++])
        {
            Item.Status = DSCAN_MSG_NEW;
        }
        else
        {
            Item.Status = DSCAN_MSG_LOST;
        }

        Item.Dir = DSCAN_MSG_DIR_TRANSMIT;

        g_DsCanTxMsgs[0][0]->ProcCount++;

        if (0==g_DsCanTxMsgs[0][0]->TxQueueCount)
        {
            DSCAN_DEBUG();
            /* msg_error_printf(...) */
        }
        else
        {
            g_DsCanTxMsgs[0][0]->TxQueueCount--;
        }
    }
    else
    {
        Item.Id = pData[Index++] & 0xFFFF;
        Item.Id |= (pData[Index++] << 16);

        if (0 != pData[Index++])
        {
            Item.Format = DSCAN_MSG_FORMAT_EXT;
        }
        else
        {
            Item.Format = DSCAN_MSG_FORMAT_STD;
        }

        TimeCount = pData[Index++] & 0x0000FFFF;
        TimeCount = TimeCount | ((pData[Index++] << 16) & 0xFFFF0000);
        WrapCount = pData[Index++];

        Item.TimeStamp = ds1103_can_time_convert(TimeCount, WrapCount);

        Item.Dlc = pData[Index++];
        DsCan_unpackBlockDs1103(&pData[Index], Item.Data, Item.Dlc);

        Item.Dir = DSCAN_MSG_DIR_RECEIVE;
        Item.Status = DSCAN_MSG_NEW;
    }

    Item.IsEnabled = DSCAN_MSG_STATE_ACTIVE;
    Item.ProcCount = 0;
    Item.Rtr = DSCAN_MSG_RTR_DISABLED;
    
    if (NULL != g_DsCanRxMsgs[0][0])
    {
        dsmcr_write(g_DsCanRxMsgs[0][0][0]->BufferIndex, (void*)&Item);
    }
}

#endif

static DsTError DsCan_getBoardType(DsTCanCh Ch, UInt32 Base, DsECanBoard_Type* pType)
{
    DsTError Error = DSCAN_NO_ERROR;

#if defined (_DS1005) || defined (_DS1006)
    Int32 Board;

    /* Attention: This is a temporary hack which relies on the fact that "base"
     *            is the first field in any existing channel structure for PHS
     *            bus systems. If DsCanCh and DsCanBoard interfaces are
     *            provided in the future this hack should be unnecessary */
    if (NULL != Ch)
    {
        Board = phs_board_type_get(((ds4302_canChannel*)Ch)->base);
        /* Board = phs_board_type_get(((ds2211_canChannel*)Ch)->base); */
        /* Board = phs_board_type_get(((ds2210_canChannel*)Ch)->base); */
        /* Board = phs_board_type_get(((ds2202_canChannel*)Ch)->base); */
    }
    else
    {
        Board = phs_board_type_get(Base);
    }

    if (   (Board == PHS_BT_NO_BOARD)
        || (Board == PHS_BT_INVALID_BASE)
        || (Board == PHS_BT_UNKNOWN))
    {
        DSCAN_DEBUG();
        return DSCAN_ERROR_NODE_NOT_EXISTS;
    }

    switch(Board)
    {
        case PHS_BT_DS2202:
        {
            *pType = DSCAN_BOARD_TYPE_DS2202;
            break;
        }
        case PHS_BT_DS2210:
        {
            *pType = DSCAN_BOARD_TYPE_DS2210;
            break;
        }
        case PHS_BT_DS2211:
        {
            *pType = DSCAN_BOARD_TYPE_DS2211;
            break;
        }
        case PHS_BT_DS4302:
        {
            *pType = DSCAN_BOARD_TYPE_DS4302;
            break;
        }
        default:
        {
            DSCAN_DEBUG();
            *pType = DSCAN_BOARD_TYPE_INVALID;
            Error = DSCAN_ERROR_NODE_NOT_EXISTS;
        }
    }
#elif defined(_DS1401)
    *pType = DSCAN_BOARD_TYPE_DS1401;
#elif defined(_DS1103)
    *pType = DSCAN_BOARD_TYPE_DS1103;
#endif

    return Error;
}

static DsTError DsCan_initBoard(DsTCanBoard_Address Address, DsECanBoard_Type* pType, UInt32* pBoardNumber)
{
    DsTError Error;
    DsECanBoard_Type Type;

    *pBoardNumber = 0;

    Error = DsCan_getBoardType(NULL, Address, &Type);

    if (DSCAN_NO_ERROR != Error)
    {
        /* msg_error_printf() */
        DSCAN_DEBUG();
        return Error;
    }

    *pType = Type;

    switch(Type)
    {
#if defined _DS1005 || defined _DS1006
        case DSCAN_BOARD_TYPE_DS4302:
        {
            DsCan_initBoardDs4302(Address, pBoardNumber);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2211:
        {
            DsCan_initBoardDs2211(Address, pBoardNumber);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2210:
        {
            DsCan_initBoardDs2210(Address, pBoardNumber);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2202:
        {
            DsCan_initBoardDs2202(Address, pBoardNumber);
            break;
        }
#endif
#if defined _DS1401
        case DSCAN_BOARD_TYPE_DS1401:
        {
            DsCan_initBoardDs1401(Address, pBoardNumber);
            break;
        }
#endif
#if defined _DS1103
        case DSCAN_BOARD_TYPE_DS1103:
        {
            DsCan_initBoardDs1103(Address, pBoardNumber);
            break;
        }
#endif
        default:
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            break;
        }
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_update(DsTCanBoard Board)
{
    DsTError Error = DSCAN_NO_ERROR;

    switch(Board->Type)
    {
#if defined _DS1005 || defined _DS1006
        case DSCAN_BOARD_TYPE_DS4302:
        {
            Error = DsCan_updateDs4302(Board->BoardNumber);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2211:
        {
            Error = DsCan_updateDs2211(Board->BoardNumber);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2210:
        {
            Error = DsCan_updateDs2210(Board->BoardNumber);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2202:
        {
            Error = DsCan_updateDs2202(Board->BoardNumber);
            break;
        }
#endif
#if defined _DS1401
        case DSCAN_BOARD_TYPE_DS1401:
        {
            Error = DsCan_updateDs1401(Board->BoardNumber);
            break;
        }
#endif
#if defined _DS1103
        case DSCAN_BOARD_TYPE_DS1103:
        {
            Error = DsCan_updateDs1103(Board->BoardNumber);
            break;
        }
#endif
        default:
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            break;
        }
    }

    return Error;
}

static DsTError DsCan_getChannelProperties(DsTCanCh Ch, UInt32* pBoardNumber, UInt32* pChannelNumber, DsECanBoard_Type* pType)
{
    DsTError Error;

    Error = DsCan_getBoardType(Ch, 0, pType);

    if (DSCAN_NO_ERROR != Error)
    {
        /* msg_error_printf() */
        DSCAN_DEBUG();
        return Error;
    }

    switch(*pType)
    {
#if defined _DS1005 || defined _DS1006
        case DSCAN_BOARD_TYPE_DS4302:
        {
            *pBoardNumber = ((ds4302_canChannel*)Ch)->module;
            *pChannelNumber = ((ds4302_canChannel*)Ch)->channel;
            break;
        }
        case DSCAN_BOARD_TYPE_DS2211:
        {
            *pBoardNumber = ((ds2211_canChannel*)Ch)->module;
            *pChannelNumber = ((ds2211_canChannel*)Ch)->channel;
            break;
        }
        case DSCAN_BOARD_TYPE_DS2210:
        {
            *pBoardNumber = ((ds2210_canChannel*)Ch)->module;
            *pChannelNumber = ((ds2210_canChannel*)Ch)->channel;
            break;
        }
        case DSCAN_BOARD_TYPE_DS2202:
        {
            *pBoardNumber = ((ds2202_canChannel*)Ch)->module;
            *pChannelNumber = ((ds2202_canChannel*)Ch)->channel;
            break;
        }
#endif
#if defined _DS1401
        case DSCAN_BOARD_TYPE_DS1401:
        {
            *pBoardNumber = ((can_tp1_canChannel*)Ch)->module;
            *pChannelNumber = ((can_tp1_canChannel*)Ch)->channel;

            break;
        }
#endif
#if defined _DS1103
        case DSCAN_BOARD_TYPE_DS1103:
        {

            /* Single board, single channel */
            *pBoardNumber = 0;
            *pChannelNumber = 0;
            break;
        }
#endif
        default:
        {
            /* msg_error_printf(...) */
            *pBoardNumber = 0;
            *pChannelNumber = 0;
            DSCAN_DEBUG();
            break;
        }
    }

    return DSCAN_NO_ERROR;
}

static DsTError DsCan_initRx(DsTCanCh Ch, DsECanBoard_Type Type, void** ppStdRxMsg, void** ppExtRxMsg)
{
    DsTError Error = DSCAN_NO_ERROR;

    switch(Type)
    {
#if defined _DS1005 || defined _DS1006
        case DSCAN_BOARD_TYPE_DS4302:
        {
            Error = DsCan_initRxDs4302((ds4302_canChannel*)Ch,
                                       (ds4302_canMsg**)ppStdRxMsg,
                                       (ds4302_canMsg**)ppStdRxMsg);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2211:
        {
            Error = DsCan_initRxDs2211((ds2211_canChannel*)Ch,
                                       (ds2211_canMsg**)ppStdRxMsg,
                                       (ds2211_canMsg**)ppExtRxMsg);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2210:
        {
            Error = DsCan_initRxDs2210((ds2210_canChannel*)Ch,
                                       (ds2210_canMsg**)ppStdRxMsg,
                                       (ds2210_canMsg**)ppExtRxMsg);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2202:
        {
            Error = DsCan_initRxDs2202((ds2202_canChannel*)Ch,
                                       (ds2202_canMsg**)ppStdRxMsg,
                                       (ds2202_canMsg**)ppExtRxMsg);
            break;
        }
#endif
#if defined _DS1401
        case DSCAN_BOARD_TYPE_DS1401:
        {
            Error = DsCan_initRxDs1401((can_tp1_canChannel*)Ch,
                                       (can_tp1_canMsg**)ppStdRxMsg,
                                       (can_tp1_canMsg**)ppExtRxMsg);
            break;
        }
#endif
#if defined _DS1103
        case DSCAN_BOARD_TYPE_DS1103:
        {
            Error = DsCan_initRxDs1103((ds1103_canChannel*)Ch,
                                       (ds1103_canMsg**)ppStdRxMsg,
                                       (ds1103_canMsg**)ppExtRxMsg);
            break;
        }
#endif
        default:
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            break;
        }
    }

    return Error;
}

static DsTError DsCan_initTx(DsTCanCh Ch, DsECanBoard_Type Type, void** ppStdTxMsg, void** ppExtTxMsg)
{
    DsTError Error = DSCAN_NO_ERROR;

    switch(Type)
    {
#if defined _DS1005 || defined _DS1006
        case DSCAN_BOARD_TYPE_DS4302:
        {
            Error = DsCan_initTxDs4302((ds4302_canChannel*)Ch,
                                       (ds4302_canMsg**)ppStdTxMsg,
                                       (ds4302_canMsg**)ppExtTxMsg);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2211:
        {
            Error = DsCan_initTxDs2211((ds2211_canChannel*)Ch,
                                       (ds2211_canMsg**)ppStdTxMsg,
                                       (ds2211_canMsg**)ppExtTxMsg);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2210:
        {
            Error = DsCan_initTxDs2210((ds2210_canChannel*)Ch,
                                       (ds2210_canMsg**)ppStdTxMsg,
                                       (ds2210_canMsg**)ppExtTxMsg);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2202:
        {
            Error = DsCan_initTxDs2202((ds2202_canChannel*)Ch,
                                       (ds2202_canMsg**)ppStdTxMsg,
                                       (ds2202_canMsg**)ppExtTxMsg);
            break;
        }
#endif
#if defined _DS1401
        case DSCAN_BOARD_TYPE_DS1401:
        {
            Error = DsCan_initTxDs1401((can_tp1_canChannel*)Ch,
                                       (can_tp1_canMsg**)ppStdTxMsg,
                                       (can_tp1_canMsg**)ppExtTxMsg);
            break;
        }
#endif
#if defined _DS1103
        case DSCAN_BOARD_TYPE_DS1103:
        {
            Error = DsCan_initTxDs1103((ds1103_canChannel*)Ch,
                                       (ds1103_canMsg**)ppStdTxMsg,
                                       (ds1103_canMsg**)ppExtTxMsg);
            break;
        }
#endif
        default:
        {
            /* msg_error_printf(...) */
            DSCAN_DEBUG();
            break;
        }
    }

    return Error;
}

static DsTError DsCan_transmit(DsSCanMsg* pMsg, const DsSCanMsg_Item* pItem)
{
    DsTError Error = DSCAN_NO_ERROR;

    switch(pMsg->BoardType)
    {
#if defined _DS1005 || defined _DS1006
        case DSCAN_BOARD_TYPE_DS4302:
        {
            Error = DsCan_transmitDs4302(pMsg, pItem);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2211:
        {
            Error = DsCan_transmitDs2211(pMsg, pItem);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2210:
        {
            Error = DsCan_transmitDs2210(pMsg, pItem);
            break;
        }
        case DSCAN_BOARD_TYPE_DS2202:
        {
            Error = DsCan_transmitDs2202(pMsg, pItem);
            break;
        }
#endif
#if defined _DS1401
        case DSCAN_BOARD_TYPE_DS1401:
        {
            Error = DsCan_transmitDs1401(pMsg, pItem);
            break;
        }
#endif
#if defined _DS1103
        case DSCAN_BOARD_TYPE_DS1103:
        {
            Error = DsCan_transmitDs1103(pMsg, pItem);
            break;
        }
#endif
        default:
        {
            DSCAN_DEBUG();
            break;
        }
    }

    return Error;
}

/******************************************************************************
 * Fake functions
 * Not all DsCAN API functions are realized completely as long as there is no
 * necessity. The functions are provided in order to allow RTICANMM and other
 * applications to use identical code on different platforms.
 *****************************************************************************/

/** <!------------------------------------------------------------------------->
*   Sets the number of message repetitions (in error case).
*
*   @description
*   - This function is currently not supported.
*
*   @parameters
*       @param Msg A CAN message handle.
*       @param Mode The retry mode (active or inactive).
*       @param Count The number of used message repetitions in case of an error.
*
*   @return
*   - #DSCAN_NO_ERROR.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_setTxRetryCount(DsTCanMsg Msg, DsECanMsg_RetryMode Mode, UInt32 Retries)
{
    (void)Msg;
    (void)Mode;
    (void)Retries;
    /* Just a dummy
     * This feature is not provided by this platform
     * No error checks are implemented as long as the arguments are not used */
    return DSCAN_NO_ERROR;
}

/** <!------------------------------------------------------------------------->
*   Configures a receive monitor message object.
*
*   @description
*   - This function is currently not supported.
*   - All messages are received by default.
*
*   @parameters
*       @param Msg The CAN message handle.
*       @param Identifier The identifier for the message object.
*       @param Mask The mask for the message object.
*       @param Format The format of the message object.
*       @param Rtr The RTR bit state of the message object.
*
*   @return
*   - #DSCAN_NO_ERROR.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_setFilter(DsTCanMsg Msg, UInt32 Identifier, UInt32 Mask, DsECanMsg_Format Format, DsECanMsg_Rtr Rtr)
{
    (void)Msg;
    (void)Identifier;
    (void)Mask;
    (void)Format;
    (void)Rtr;
    /* Just a dummy
     * This function does intentionally nothing right now
     * We expect the application to receive all CAN frames and do filtering on its own
     * No error checks are implemented as long as the arguments are not used */
    return DSCAN_NO_ERROR;
}

/** <!------------------------------------------------------------------------->
*   Sets the queue size (depth) of transmit and receive monitor message objects.
*
*   @description
*   - This function is currently not supported.
*   - 64 elements is used as default queue size.
*
*   @parameters
*       @param Msg The CAN message handle.
*       @param QueueSize The new queue depth.
*
*   @return
*   - #DSCAN_NO_ERROR.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_setQueueSize(DsTCanMsg Msg, UInt32 QueueSize)
{
    (void)Msg;
    (void)QueueSize;
    /* Just a dummy
     * We use 64 elements for both rx monitors and tx queues
     * No error checks are implemented as long as the arguments are not used */
    return DSCAN_NO_ERROR;
}

/** <!------------------------------------------------------------------------->
*   Applies modifications to message objects.
*
*   @description
*   - This function is currently not supported.
*
*   @parameters
*       @param Msg A CAN message handle.
*
*   @return
*   - #DSCAN_NO_ERROR.
*<!-------------------------------------------------------------------------->*/
DsTError DsCanMsg_apply(DsTCanMsg Msg)
{
    (void)Msg;
    /* Just a dummy
     * To simplify coding settings are immediately applied
     * No error checks are implemented as long as the arguments are not used */
    return DSCAN_NO_ERROR;
}
