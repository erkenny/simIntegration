/******************************************************************************
*
* MODULE
*   CAN controller access functions for DS1103 board.
*
*   Version for PPC master CPU.
*
* FILE
*   can1103.h
*
* RELATED FILES
*   can1103.c, dsmcom.h, def1103.h, ds1103.h, dstypes.h
*
* DESCRIPTION
*   DS1103 CAN controller access functions for the CAN bus.
*
*
* AUTHOR(S)
*   A.Gropengieﬂer, O.Jabczinski
*
* Copyright dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: can1103.h $   $Revision: 1.12 $   $Date: 2009/08/31 09:11:07GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#ifndef __CAN1103_H__
#define __CAN1103_H__

#include <dstypes.h>
#include <dsmcom.h>
#include <def1103.h>
#include <ds1103.h>
#include <dslist.h>
#include <dsmcr.h>                        /* multi client ring buffer module */

/******************************************************************************
* constant and macro definitions
******************************************************************************/

/* $DSVERSION: DS1103_CAN_SOFTWARE - DS1103 Can Software */
#define DS1103_CAN_SOFTWARE_VER_MAR 2
#define DS1103_CAN_SOFTWARE_VER_MIR 6
#define DS1103_CAN_SOFTWARE_VER_MAI 0
#define DS1103_CAN_SOFTWARE_VER_SPB VCM_VERSION_RELEASE
#define DS1103_CAN_SOFTWARE_VER_SPN 0
#define DS1103_CAN_SOFTWARE_VER_PLV 0

/* $DSVERSION: REQ_DS1103_CAN_FIRMWARE - Required DS1103 Can Firmware */
#define REQ_DS1103_CAN_FIRMWARE_VER_MAR 2
#define REQ_DS1103_CAN_FIRMWARE_VER_MIR 6
#define REQ_DS1103_CAN_FIRMWARE_VER_MAI 0
#define REQ_DS1103_CAN_FIRMWARE_VER_SPB VCM_VERSION_RELEASE
#define REQ_DS1103_CAN_FIRMWARE_VER_SPN 0
#define REQ_DS1103_CAN_FIRMWARE_VER_PLV 0

/*----- constants for slave MC control */
#define DS1103_CAN_RSTSLV 0x4
#define DS1103_CAN_BOOTCAN_FLASH 0x1000
#define DS1103_CAN_TIMER_PRESCALER  8

/******************************************************************************
* can message register and channel setup definitions
******************************************************************************/

/*---- interrupt control */
#define DS1103_CAN_INT_DISABLE          DEF1103_INT_DISABLE
#define DS1103_CAN_INT_ENABLE           DEF1103_INT_ENABLE

/*---- subinterrupts  */
#define DS1103_CAN_NO_SUBINT            -1  /* no interrupt */
#define DS1103_CAN_SUBINT_BUFFERWARN    DEF1103_CAN_SUBINT_BUFFERWARN
/*----- format */
/* 11-bit Identifier Standard CAN (CAN 2.0A) */
#define DS1103_CAN_STD                  0
/* 29-bit Identifier Extended CAN (CAN 2.0B) */
#define DS1103_CAN_EXT                  4

/*----- repetition time */
#define DS1103_CAN_TRIGGER_MSG          0  /* for trigger-Msg. */

/*----- timeout */
#define DS1103_CAN_TIMEOUT_NORMAL       0  /* for normal timeout */

/*----- inform parameter */
#define DS1103_CAN_NO_INFO              DEF1103_CAN_NO_INFO         /* delivers no info */
#define DS1103_CAN_DATA_INFO            DEF1103_CAN_DATA_INFO
#define DS1103_CAN_MSG_INFO             DEF1103_CAN_MSG_INFO    
#define DS1103_CAN_TIMECOUNT_INFO       DEF1103_CAN_TIMECOUNT_INFO  /* delivers Timecount */
#define DS1103_CAN_DELAYCOUNT_INFO      DEF1103_CAN_DELAYTIME_INFO  /* delivers Delaycount */

/*******************************************************************************
* return values
*******************************************************************************/

/*---- processed flag */
#define DS1103_CAN_NOT_PROCESSED        0
#define DS1103_CAN_PROCESSED            1

/*---- busstatus */
#define DS1103_CAN_ACTIVE_STATE         0
#define DS1103_CAN_WARN_STATE           1
#define DS1103_CAN_BUSOFF_STATE         2

/*******************************************************************************
*  error codes
*******************************************************************************/

#define DS1103_CAN_NO_ERROR               0
#define DS1103_CAN_BUFFER_OVERFLOW        DSMCOM_BUFFER_OVERFLOW     /* 4 */
#define DS1103_CAN_NO_DATA                DSMCOM_NO_DATA             /* 5 */
#define DS1103_CAN_DATA_LOST              DSMCOM_DATA_LOST           /* 6 */
#define DS1103_CAN_SLAVE_ALLOC_ERROR      DSMCOM_SLV_ALLOC_ERROR     /* 8 */
#define DS1103_CAN_SLAVE_BUFFER_OVERFLOW  DSMCOM_SLV_BUFFER_OVERFLOW /* 9 */
#define DS1103_CAN_INIT_ACK               DSMCOM_SLV_INIT_ACK       /* 10 */
#define DS1103_CAN_SLAVE_UNDEF_ERROR      DSMCOM_SLV_UNDEFINED      /* 11 */


/*******************************************************************************
* can services
*******************************************************************************/

#define DS1103_CAN_SERVICE_TX_OK                DEF1103_CAN_SVC_TX_OK
#define DS1103_CAN_SERVICE_RX_OK                DEF1103_CAN_SVC_RX_OK
#define DS1103_CAN_SERVICE_CRC_ERR              DEF1103_CAN_SVC_CRC_ERR
#define DS1103_CAN_SERVICE_ACK_ERR              DEF1103_CAN_SVC_ACK_ERR
#define DS1103_CAN_SERVICE_FORM_ERR             DEF1103_CAN_SVC_FORM_ERR
#define DS1103_CAN_SERVICE_BIT1_ERR             DEF1103_CAN_SVC_BIT1_ERR
#define DS1103_CAN_SERVICE_BIT0_ERR             DEF1103_CAN_SVC_BIT0_ERR
#define DS1103_CAN_SERVICE_STUFFBIT_ERR         DEF1103_CAN_SVC_STUFFBIT_ERR
#define DS1103_CAN_SERVICE_RX_LOST              DEF1103_CAN_SVC_RX_LOST
#define DS1103_CAN_SERVICE_DATA_LOST            DEF1103_CAN_SVC_DATA_LOST
#define DS1103_CAN_SERVICE_MAILBOX_ERR          DEF1103_CAN_SVC_MAILBOX_ERR
#define DS1103_CAN_SERVICE_BUSSTATUS            DEF1103_CAN_SVC_BUSSTATUS        
#define DS1103_CAN_SERVICE_STDMASK              DEF1103_CAN_SVC_STDMASK  
#define DS1103_CAN_SERVICE_EXTMASK              DEF1103_CAN_SVC_EXTMASK        
#define DS1103_CAN_SERVICE_MSG_MASK15           DEF1103_CAN_SVC_MSG_MASK15        
#define DS1103_CAN_SERVICE_VERSION              DEF1103_CAN_SVC_VERSION
#define DS1103_CAN_SERVICE_TXQUEUE_OVERFLOW_COUNT    DEF1103_CAN_SVC_TXQUEUE_OVERFLOW_COUNT


#define DS1103_CAN_SERVICE_MAILBOX_ID(a)  (((a) << 8) | 0x00800000) 

/* obsolete defines */
#define DS1103_CAN_SERVICE_GLOBAL_STDMASK       DS1103_CAN_SERVICE_STDMASK
#define DS1103_CAN_SERVICE_GLOBAL_EXTMASK       DS1103_CAN_SERVICE_EXTMASK

/*******************************************************************************
* can channel set
*******************************************************************************/

#define DS1103_CAN_CHANNEL_SET_MASK15       DEF1103_CAN_MASK15_SET  
#define DS1103_CAN_CHANNEL_SET_ARBMASK15    DEF1103_CAN_ARBMASK15_SET
#define DS1103_CAN_CHANNEL_SET_BAUDRATE     DEF1103_CAN_BAUDRATE_SET

/*******************************************************************************
* can msg set
*******************************************************************************/

#define DS1103_CAN_MSG_MASK                 DEF1103_CAN_MSG_MASK
#define DS1103_CAN_MSG_PERIOD               DEF1103_CAN_MSG_PERIOD
#define DS1103_CAN_MSG_ID                   DEF1103_CAN_MSG_ID
#define DS1103_CAN_MSG_QUEUE                DEF1103_CAN_MSG_QUEUE
#define DS1103_CAN_MSG_DSMCR_BUFFER         DEF1103_CAN_MSG_DSMCR_BUFFER
#define DS1103_CAN_MSG_LEN                  DEF1103_CAN_MSG_LEN

/*******************************************************************************
* overrun policies for can_dsmcr_client_create
*******************************************************************************/

#define DS1103_CAN_DSMCR_OVERRUN_FUNCTION    DSMCR_OVERRUN_FUNCTION     /* 4 */
#define DS1103_CAN_DSMCR_OVERRUN_ERROR       DSMCR_OVERRUN_ERROR        /* 3 */
#define DS1103_CAN_DSMCR_OVERRUN_IGNORE      DSMCR_OVERRUN_IGNORE       /* 2 */
#define DS1103_CAN_DSMCR_OVERRUN_OVERWRITE   DSMCR_OVERRUN_OVERWRITE    /* 1 */


/*******************************************************************************
* common definitions 
*******************************************************************************/

#define DS1103_CAN_AUTO_INDEX           DSCOMDEF_AUTO_INDEX
#define DS1103_CAN_MAX_NO_OF_MESSAGES   100
#define DS1103_CAN_MAX_NO_OF_RM_MSG     10  
#define DS1103_CAN_MAX_NO_OF_SUBINT     15
#define DS1103_CAN_BUSOFF_INT           1


/* can message types */
#define DS1103_CAN_TX                   0x10 
#define DS1103_CAN_RX                   0x11  
#define DS1103_CAN_RM                   0x12  
#define DS1103_CAN_RQTX                 0x13  
#define DS1103_CAN_RQRX                 0x14  


/* can controller init/setup definitions */
#define DS1103_CAN_MIN_BAUDRATE           10000    /* min. 10 kBaud */  
#define DS1103_CAN_MAX_BAUDRATE         1000000    /* max. 1 MBaud */

#define DS1103_CAN_MINCLOCK              5000000
#define DS1103_CAN_LOWCLOCK             10000000
#define DS1103_CAN_MAXCLOCK             20000000


/* timing parameter */
#define DS1103_CAN_MAX_STARTTIME        420.0  /* max. starttime in s */ 
#define DS1103_CAN_MAX_REP_TIME         100.0  /* max. repetition_time in s */ 
#define DS1103_CAN_MAX_TIMEOUT          100.0  /* max. timeout in s */ 
#define DS1103_CAN_TXQUEUE_INIT_MIN_DELAY_TIME    0.0  /* min. tx-queue delay time in s */ 
#define DS1103_CAN_TXQUEUE_INIT_MAX_DELAY_TIME   10.0  /* max. tx-queue delay time in s */ 

#define DS1103_CAN_MIN_REP_TIME          1   /* min.   1x CAN DATA FRAME TIME */
#define DS1103_CAN_TIMEOUT_MIN           3   /* min.   3x CAN DATA FRAME TIME */
#define DS1103_CAN_TIMEOUT_NORM_MPY    100   /* min. 100x CAN DATA FRAME TIME */


/* defines for the setup register and the dpm */
#define DS1103_CAN_DPMEM_START          DS1103_SLAVE_MC_DP_RAM_START

/* defines for overrun_policy of ds1103_can_msg_txqueue_init() */
#define DS1103_CAN_TXQUEUE_OVERRUN_OVERWRITE   DEF1103_CAN_MSG_TXQUEUE_OVERRUN_OVERWRITE  
#define DS1103_CAN_TXQUEUE_OVERRUN_IGNORE      DEF1103_CAN_MSG_TXQUEUE_OVERRUN_IGNORE     


/* define for DSMCR-buffer handling */
#define DS1103_CAN_MSG_DSMCR_NOT_INITIALIZED   -1

typedef void (*ds1103_can_subint_handler_t)( void* subint_data, Int32 subint );

typedef struct
{
  Int32 index;
 UInt32 fifo;
 UInt32 btr0;
 UInt32 btr1;
 UInt32 frequency;
 UInt32 mb15_format;
 UInt32 busoff_subinterrupt;
} ds1103_canChannel;



typedef struct
{

 /* public member */
 UInt32 busstatus;
 UInt32 stdmask;
 UInt32 extmask;
 UInt32 msg_mask15;
 UInt32 tx_ok;
 UInt32 rx_ok;
 UInt32 crc_err;
 UInt32 ack_err;
 UInt32 form_err;
 UInt32 stuffbit_err;
 UInt32 bit1_err;
 UInt32 bit0_err;
 UInt32 rx_lost;
 UInt32 data_lost;
 UInt32 version;
 UInt32 mailbox_err;
 UInt32 data0;   /* output data0 */
 UInt32 data1;   /* output data1 */
 UInt16 txqueue_overflowcnt_std;
 UInt16 txqueue_overflowcnt_ext;

 /* private member */
 UInt32 queue;
 UInt32 type;
  Int32 index;
} ds1103_canService;



typedef struct ds1103canMsg* ds1103_canMsg_p;

/** Message event callback function prototype */
typedef void (*ds1103canMsgCb)(ds1103_canMsg_p Msg, UInt32* pData);

typedef struct ds1103canMsg
{
 /* output data */
 Float64 timestamp;
 Float32 deltatime;
 Float32 delaytime;
   Int32 processed;
  UInt32 datalen;
  UInt32 data[8];
 /* internal used data */
 UInt32  identifier;
 UInt32  format;
 UInt32  queue;
  Int32  index;
 UInt32  msg_no;
 UInt32  type;
 UInt32  inform;
 UInt32  timecount;
 UInt32  wrapcount;
 UInt32  error;
 UInt32  isTxqueued;
 dsList* msgQueue;
 ds1103_canChannel* canChannel;
 ds1103_canService* msgService;
 UInt32 dsmcr_buffer;
 ds1103_canMsg_p* clientMsgs;
 UInt32  no_of_clients;
 UInt32  clientNumber;
 ds1103_canMsg_p rootMsg;
 ds1103canMsgCb pDsCanMsgCb;
} ds1103_canMsg;




/******************************************************************************
* function prototypes
******************************************************************************/

#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE extern
#endif

/*--- Basic initialization --------------------------------------------------*/

         void ds1103_can_channel_init (const UInt32 baudrate,
                                       const UInt32 bufferwarn,
                                       const UInt32 mb15_format,
                                       const  Int32 busoff_subinterrupt);


         void ds1103_can_channel_init_advanced (const UInt32 frequency,
                                                const UInt32 btr0,
                                                const UInt32 btr1,
                                                const UInt32 bufferwarn,
                                                const UInt32 mb15_format,
                                                const  Int32 busoff_subinterrupt);


Int32 ds1103_can_msg_txqueue_init ( ds1103_canMsg* canMsg,
                                 const UInt32 overrun_policy,
                                 Float32 delay);

Int32 ds1103_can_dsmcr_client_create(ds1103_canMsg* canMsg,
                                  const UInt32 overrun_policy,
                                  dsmcr_t_p_overrun_fcn p_overrun_fcn,
                                  ds1103_canMsg** clientMsg);


/*--- Functions for handle the CAN channel ----------------------------------*/

         void  ds1103_can_channel_start (const UInt32 status_int);

__INLINE Int32 ds1103_can_channel_all_sleep (void);

__INLINE Int32 ds1103_can_channel_all_wakeup (void);

__INLINE Int32 ds1103_can_channel_BOff_go (void);

__INLINE Int32 ds1103_can_channel_BOff_return (void);

__INLINE Int32 ds1103_can_channel_set (const UInt32 mask_type,
                                       const UInt32 mask_value );

Int32 ds1103_can_channel_txqueue_clear(void);

/*--- Functions for message object handling ---------------------------------*/

ds1103_canMsg* ds1103_can_msg_tx_register (const  Int32  queue,
                                           const UInt32  identifier,
                                           const UInt32  format,
                                           const UInt32  inform,
                                           const  Int32  subinterrupt,
                                           const Float32 start_time,
                                           const Float32 repetition_time,
                                           const Float32 timeout);

ds1103_canMsg* ds1103_can_msg_rm_register (const  Int32 queue,
                                           const UInt32 identifier,
                                           const UInt32 format,
                                           const UInt32 inform,
                                           const  Int32 subinterrupt);

ds1103_canMsg* ds1103_can_msg_rx_register (const  Int32 queue,
                                           const UInt32 identifier,
                                           const UInt32 format,
                                           const UInt32 inform,
                                           const  Int32 subinterrupt);

ds1103_canMsg* ds1103_can_msg_rqtx_register (const  Int32  queue,
                                             const UInt32  identifier,
                                             const UInt32  format,
                                             const UInt32  inform,
                                             const  Int32  subinterrupt,
                                             const Float32 start_time,
                                             const Float32 repetition_time,
                                             const Float32 timeout);

ds1103_canMsg* ds1103_can_msg_rqrx_register (const ds1103_canMsg*   rqtxMsg,
                                             const UInt32 inform,
                                             const  Int32 subinterrupt);

__INLINE Int32 ds1103_can_msg_rqtx_activate (const ds1103_canMsg* canMsg);

__INLINE Int32 ds1103_can_msg_write (const ds1103_canMsg* canMsg,
                                     const UInt32 data_len,
                                     const UInt32* data);

__INLINE Int32 ds1103_can_msg_send (const ds1103_canMsg* canMsg,
                                    const UInt32 data_len,
                                    const UInt32* data,
                                    const Float32 delay);

__INLINE Int32 ds1103_can_msg_send_id (ds1103_canMsg* canMsg,
                                       const UInt32 id,
                                       const UInt32  data_len,
                                       const UInt32* data,
                                       const Float32 delay);

__INLINE Int32 ds1103_can_msg_send_id_queued (  ds1103_canMsg* canMsg,
                                             const UInt32 id,
                                             const UInt32 data_len,
                                             const UInt32* data);

__INLINE UInt32 ds1103_can_msg_txqueue_level_read (const ds1103_canMsg* canMsg);

__INLINE Int32 ds1103_can_msg_set( ds1103_canMsg* canMsg,
                                   const UInt32 type,
                                   const void* value_p);

__INLINE Int32 ds1103_can_msg_read (ds1103_canMsg* canMsg);

__INLINE Int32 ds1103_can_msg_read_from_mem ( ds1103_canMsg* canMsg );

__INLINE void ds1103_can_msg_copy_all_to_mem(void);

__INLINE Int32 ds1103_can_msg_read_from_dsmcr ( ds1103_canMsg* clientMsg );

__INLINE Int32 ds1103_can_tx_msg_write_to_dsmcr ( const ds1103_canMsg* txMsg,
                                               const UInt32 data_len,
                                               const UInt32* msg_data,
                                               ds1103_canMsg* rxMsg );

__INLINE Int32 ds1103_can_tx_msg_read_loopback ( ds1103_canMsg* txclientMsg,
                                                 const UInt32 data_len,
                                                 const UInt32* msg_data, 
                                                 ds1103_canMsg* rxMsg );

__INLINE Int32 ds1103_can_msg_queue_level( ds1103_canMsg* canMsg );

__INLINE Int32 ds1103_can_msg_trigger (const ds1103_canMsg* canMsg,
                                       const Float32 delay);

__INLINE Int32 ds1103_can_msg_sleep (const ds1103_canMsg* canMsg);

__INLINE Int32 ds1103_can_msg_wakeup (const ds1103_canMsg* canMsg);

          void ds1103_can_msg_processed_register (ds1103_canMsg* canMsg);

__INLINE Int32 ds1103_can_msg_processed_request (const ds1103_canMsg* canMsg);

__INLINE Int32 ds1103_can_msg_processed_read (ds1103_canMsg* canMsg,
                                              Float64* timestamp,
                                               UInt32* processed);

__INLINE  void ds1103_can_msg_clear (ds1103_canMsg* canMsg);


/*--- Service functions -----------------------------------------------------*/

ds1103_canService* ds1103_can_service_register (const UInt32 service_type);

__INLINE Int32 ds1103_can_service_request (const ds1103_canService* service);

__INLINE Int32 ds1103_can_service_read (ds1103_canService* service);


/*--- Utility functions -----------------------------------------------------*/

__INLINE Int32 ds1103_can_error_read (const Int32 queue);

__INLINE  void ds1103_can_all_data_clear (void);


/*--- Function for setting time base ----------------------------------------*/

__INLINE  void ds1103_can_timecount_set (const Float64 time);


/*--- Functions for subinterrupt handling -----------------------------------*/

ds1103_can_subint_handler_t 
ds1103_can_subint_handler_install( ds1103_can_subint_handler_t subint_handler );

         void ds1103_can_bufferwarn_handler (void);

__INLINE Int32 ds1103_can_subint_get (void);

__INLINE  void ds1103_can_subint_acknowledge (void);

/* Only for test purposes */
__INLINE Int32 ds1103_can_subint_request (const UInt32 subint);

/*--- Data memory access ----------------------------------------------------*/

          void ds1103_can_mem_read_register (const Int32 queue,
                                             Int32 *index,
                                             const UInt32 addr,
                                             const UInt32 len);

__INLINE Int32 ds1103_can_mem_read_request (const Int32 queue,
                                            const Int32 index);

__INLINE Int32 ds1103_can_mem_read (const Int32 queue,
                                    const Int32 index,
                                    UInt32 *data);

          void ds1103_can_mem_write_register (const Int32 queue,
                                              Int32 *index,
                                              const UInt32 addr,
                                              const UInt32 len);

__INLINE Int32 ds1103_can_mem_write (const  Int32 queue,
                                     const  Int32 index,
                                     const UInt32 *data);


/*--- Misc ------------------------------------------------------------------*/

__INLINE Int32 ds1103_can_msg_update_all(const UInt32 queue);


/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
#include <can1103.c>
#endif

#endif /*__CAN1103_H__*/
