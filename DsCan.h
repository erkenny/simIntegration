/******************************************************************************
*
* MODULE
*   Partially generic DsCAN API for DS4302, DS2211, DS2210, DS1401 & DS1103
*
* FILE
*   dscan.h
*
* RELATED FILES
*   dscan.c
*
* DESCRIPTION
*   This module provides a generic (platform independent) CAN interface for
*   most dSPACE CAN platforms.
*   It is focused on message handling right now but may be extended further in
*   the future.
*
* AUTHOR(S)
*   O. Jabczinski
*   AlexanderSt
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: DsCan.h $ $Revision: 1.4 $ $Date: 2009/08/27 16:35:25GMT+01:00 $
******************************************************************************/
#ifndef DSCAN_H_
#define DSCAN_H_


#include <DSMSG.H>
#include <dsstd.h>
#include <Dstypes.h>

/* $DSVERSION: DSCAN - CAN Interface */
#define DSCAN_VER_MAR 1
#define DSCAN_VER_MIR 0
#define DSCAN_VER_MAI 0
#define DSCAN_VER_SPB VCM_VERSION_RELEASE
#define DSCAN_VER_SPN 0
#define DSCAN_VER_PLV 0


/** No error occurred. */
#define DSCAN_NO_ERROR                              (DsTError)(0x00)
#define DSCAN_NO_ERROR_TXT                          "" /* This condition emits no error/warning/info */

/** At least one function argument was a NULL pointer. */
#define DSCAN_ERROR_NULL_POINTER                    (DsTError)(0x01)
#define DSCAN_ERROR_NULL_POINTER_TXT                "%s: Argument %s of function %s was a NULL pointer."

/** The selected object (board, channel, message, etc.) does already exist. */
#define DSCAN_ERROR_OBJECT_REUSED                   (DsTError)(0x02)
#define DSCAN_ERROR_OBJECT_REUSED_TXT               "" /* This condition emits no error/warning/info */

/** A memory allocation error occurred. */
#define DSCAN_ERROR_MEMORY_ALLOCATION               (DsTError)(0x03)
#define DSCAN_ERROR_MEMORY_ALLOCATION_TXT           "%s: A memory allocation error occured in function %s."

/** The selected operation is not supported for the given message type. */
#define DSCAN_ERROR_INVALID_MSG_TYPE                (DsTError)(0x20)
#define DSCAN_ERROR_INVALID_MSG_TYPE_TXT            "%s: The selected operation %s is not supported for the given message type %d."

/* The selected message format is not valid for CAN frames. */
#define DSCAN_ERROR_INVALID_MSG_FORMAT              (DsTError)(0x21)
#define DSCAN_ERROR_INVALID_MSG_FORMAT_TXT          "%s: The selected message format %d passed to %s is not valid for CAN frames."

/* The selected message identifier is not valid for the chosen format. */
#define DSCAN_ERROR_INVALID_MSG_IDENTIFIER          (DsTError)(0x22)
#define DSCAN_ERROR_INVALID_MSG_IDENTIFIER_TXT      "%s: The selected message identifier 0x%08x passed to %s is not valid for the chosen format %d."

/* The selected message direction is not valid for CAN frames. */
#define DSCAN_ERROR_INVALID_MSG_DIR                 (DsTError)(0x21)
#define DSCAN_ERROR_INVALID_MSG_DIR_TXT             "%s: The selected message direction %d passed to %s is not valid for CAN frames."

/** The selected remote bit state is not supported. */
#define DSCAN_ERROR_INVALID_MSG_RTR_STATE           (DsTError)(0x2A)
#define DSCAN_ERROR_INVALID_MSG_RTR_STATE_TXT       "%s: The selected remote bit state %d passed to %s is not supported."

/** A frame to be transmitted was lost. */
#define DSCAN_ERROR_TRANSMIT_MSG_LOST               (DsTError)(0x26)
#define DSCAN_ERROR_TRANSMIT_MSG_LOST_TXT           "%s: A frame to be transmitted was lost (0x%08x, %d)."

/** The selected target hardware was not found. */
#define DSCAN_ERROR_NODE_NOT_EXISTS                 (DsTError)(0x05)
#define DSCAN_ERROR_NODE_NOT_EXISTS_TXT             "%s:  The selected target hardware was not found."

/** A communication error was detected. */
#define DSCAN_ERROR_COMMUNICATION                   (DsTError)(0x04)
#define DSCAN_ERROR_COMMUNICATION_TXT               "%s: A communication error was detected by function %s."


/** Maximum queue length for transmit messages or monitors */
#define DSCAN_MSG_QUEUE_SIZE_MAX                    0xFFFF


/** Message status */
enum DsECanMsg_Status
{
    DSCAN_MSG_NO_DATA = 1,                          /**< No new data available */
    DSCAN_MSG_NEW     = 2,                          /**< New data available */
    DSCAN_MSG_OVERRUN = 3,                          /**< A buffer overrun occured, data was lost */
    DSCAN_MSG_LOST    = 4                           /**< A message was lost due to timeout or overwrite */
};
typedef enum DsECanMsg_Status DsECanMsg_Status;

/** Message direction (only relevant for receive + monitor message objects) */
enum DsECanMsg_Direction
{
    DSCAN_MSG_DIR_TRANSMIT = 1,                     /**< Message was received using internal loopback */
    DSCAN_MSG_DIR_RECEIVE  = 2                      /**< Message was received from external node */
};
typedef enum DsECanMsg_Direction DsECanMsg_Direction;

/** Message remote transmission modes */
enum DsECanMsg_Rtr
{
    DSCAN_MSG_RTR_DISABLED = 1,                     /**< Normal frame */
    DSCAN_MSG_RTR_ENABLED  = 2,                     /**< Frame with remote transmission bit set */
    DSCAN_MSG_RTR_BOTH     = 3                      /**< Don't care (use only with monitors or EFGs) */
};
typedef enum DsECanMsg_Rtr DsECanMsg_Rtr;

/** CAN message format */
enum DsECanMsg_Format
{
    DSCAN_MSG_FORMAT_STD  = 1,                      /**< Standard message */
    DSCAN_MSG_FORMAT_EXT  = 2,                      /**< Extended message */
    DSCAN_MSG_FORMAT_BOTH = 3                       /**< Ignore format */
};
typedef enum DsECanMsg_Format DsECanMsg_Format;

/** CAN message retry mode */
enum DsECanMsg_RetryMode
{
    DSCAN_MSG_RETRY_MODE_ACTIVE   = 1,              /**< Message transmission is repeated only a fixed number of times in case of a bus error */
    DSCAN_MSG_RETRY_MODE_INACTIVE = 2               /**< Message transmission is repeated indefinitely in case of bus errors (CAN specification behaviour) */
};
typedef enum DsECanMsg_RetryMode DsECanMsg_RetryMode;

/** Message states */
enum DsECanMsg_State
{
    DSCAN_MSG_STATE_INACTIVE  = 1,                  /**< Message is disabled, no message can be received or transmitted */
    DSCAN_MSG_STATE_ACTIVE    = 2                   /**< Message is enabled */
};
typedef enum DsECanMsg_State DsECanMsg_State;


/** CAN message item structure (data exchange between DsCan API and application) */
struct DsSCanMsg_Item
{
    Float64             TimeStamp;                  /**< Timestamp in seconds (intended usage: read / write)*/
    UInt32              ProcCount;                  /**< Processing count (intended usage: read) */
    DsECanMsg_State     IsEnabled;                  /**< Message state (intended usage: read / write) */
    DsECanMsg_Direction Dir;                        /**< Message direction (intended usage: read) */
    DsECanMsg_Status    Status;                     /**< Message status (intended usage: read) */
    UInt32              Id;                         /**< Identifier: 0x0 to 0x7FF or 0x1FFFFFFF (intended usage: read / write) */
    UInt32              Dlc;                        /**< Data length code: 0 to 8 (intended usage: read / write) */
    DsECanMsg_Format    Format;                     /**< Message format (intended usage: read / write) */
    DsECanMsg_Rtr       Rtr;                        /**< Remote frame indicator (intended usage: read / write) */
    UInt8               Data[8];                    /**< Frame data bytes (intended usage: read / write) */
};
typedef struct DsSCanMsg_Item  DsSCanMsg_Item;


/** CAN board type definition */
typedef struct DsSCanBoard* DsTCanBoard;

/** CAN board address type definition */
typedef UInt32 DsTCanBoard_Address;

/** CAN channel handle */
/* typedef struct DsSCanCh* DsTCanCh; */
typedef void* DsTCanCh;

/** CAN Message handle */
typedef struct DsSCanMsg* DsTCanMsg;


DsTError DsCanMsg_createRxMonitor(DsTCanMsg* pMsg, DsTCanCh Ch);

DsTError DsCanMsg_setFilter(DsTCanMsg Msg, UInt32 Identifier, UInt32 Mask, DsECanMsg_Format Format, DsECanMsg_Rtr Rtr);

DsTError DsCanMsg_setQueueSize(DsTCanMsg Msg, UInt32 QueueSize);

DsTError DsCanMsg_apply(DsTCanMsg Msg);

DsTError DsCanMsg_createTxQueue(DsTCanMsg* pMsg, DsTCanCh Ch);

DsTError DsCanMsg_setTxRetryCount(DsTCanMsg Msg, DsECanMsg_RetryMode Mode, UInt32 Retries);

DsTError DsCanMsg_readRxItem(DsTCanMsg Msg, DsSCanMsg_Item* pItem);

DsTError DsCanMsg_transmitItem(DsTCanMsg Msg, const DsSCanMsg_Item* pItem);

DsTError DsCanMsg_start(DsTCanMsg Msg);

DsTError DsCanMsg_stop(DsTCanMsg Msg);

DsTError DsCanMsg_getTxQueueCount(DsTCanMsg Msg, UInt32* pQueueCount);

DsTError DsCanBoard_create(DsTCanBoard* pBoard, DsTCanBoard_Address Address);

DsTError DsCanBoard_update(DsTCanBoard Board);


#endif /* DSCAN_H_ */
