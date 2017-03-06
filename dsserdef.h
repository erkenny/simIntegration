/**
 * @file dsserdef.h
 *
 * This module contains defines to access the serial interfaces
 * of DS1103, DS1005, DS1006, DS2210, DS2211, DS4201S, DS1401, DS2202.
 *
 * dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
 */

/*
 * $RCSfile: dsserdef.h $
 * $Revision: 1.8 $
 * $Date: 2008/12/16 09:20:28GMT+01:00 $
 * $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSER.pj $
 */

#ifndef __dsserdef__
#define __dsserdef__

#include <dstypes.h>
#include <dsfifo.h>

/** @name Function Return Codes */

/** No error occurs during the operation. */
#define   DSSER_NO_ERROR                 0

/** 
 * The operation of the command is failed with no effect 
 * on the input or output data. No data is written to
 * or read from the FIFO.
 */
#define   DSSER_COMMUNICATION_FAILED     4 

/** No new data is read from the FIFO. */
#define   DSSER_NO_DATA                  5

/** The dsser_free command is not executed if TX data is currently processed. */
#define   DSSER_TX_FIFO_NOT_EMPTY        6


/**  
 * For dsser_transmit():
 *
 * The function dsser_transmit returns this error when the 
 * FIFO is filled and or not all data can't placed into the FIFO.
 *           
 * For dsser_receive() and dsser_receive_term():
 * 
 * The meaning of the error DSSER_FIFO_OVERFLOW is depending 
 * of the fifo_mode adjusted with the function dsser_config().
 *              
 * fifo_mode = DSSER_FIFO_MODE_BLOCKED
 * The FIFO is filled and not all data can't placed into the FIFO.
 *              
 * fifo_mode >= DSSER_FIFO_MODE_OVERWRITE
 * The old data is rejected.
 */
#define   DSSER_FIFO_OVERFLOW            DSFIFO_BUFFER_OVERFLOW      /* 202 */

/** @name Return codes for function dsser_error_read() */

/** 
 * The allocation of memory on the slave processor during the
 * initialization of the serial interface is failed. 
 */
#define   DSSER_SLAVE_ALLOC_ERROR        DSMCOM_SLV_ALLOC_ERROR        /* 8 */

/**
 * Slave to master buffer overflow (of the communication queue, not
 * of the FIFO from the serial interface).
 */
#define   DSSER_SLAVE_BUFFER_OVERFLOW    DSMCOM_SLV_BUFFER_OVERFLOW    /* 9 */

/** No error. (Acknowledge code from slave) */
#define   DSSER_SLAVE_INIT_ACK           250

#define   DSSER_SLAVE_DATA_LOST          251

#define   DSSER_SLAVE_FIFO_OVERFLOW      252

/** @} */

/* only internal used error codes! */

#define   DSSER_ALLOC_ERROR              1
#define   DSSER_DATA_LOST                DSMCOM_DATA_LOST              /* 6 */
#define   DSSER_TIMEOUT                  DSMCOM_TIMEOUT                /* 7 */
/******************************************************************************
  function parameter codes:
******************************************************************************/

/************************************************** dsser_init(...); */

/** 
 * Use this constant as base address for serial interfaces
 * which are present on the real time processor 
 * itself, for example: DS1005 or DS1103
 */
#define   DSSER_ONBOARD                    0

/************************************************ dsser_config(...); */
/*--- fifo mode */
#define   DSSER_FIFO_MODE_BLOCKED          0
#define   DSSER_FIFO_MODE_OVERWRITE        1

/*--- stop bits */
#define   DSSER_1_STOPBIT                  0            /* 1 stop bit */
#define   DSSER_2_STOPBIT                  1    /* 1.5 or 2 stop bits */

/*--- parity bit */
#define   DSSER_NO_PARITY                  0
#define   DSSER_ODD_PARITY                 1
#define   DSSER_EVEN_PARITY                2
#define   DSSER_FORCED_PARITY_ONE          3
#define   DSSER_FORCED_PARITY_ZERO         4

/*--- UART trigger level */
#define   DSSER_1_BYTE_TRIGGER_LEVEL       0
#define   DSSER_4_BYTE_TRIGGER_LEVEL       1
#define   DSSER_8_BYTE_TRIGGER_LEVEL       2
#define   DSSER_14_BYTE_TRIGGER_LEVEL      3

/*--- User trigger level */
#define   DSSER_TRIGGER_LEVEL_DISABLE      0
#define   DSSER_DEFAULT_TRIGGER_LEVEL     -1

/*--- UART mode */
#define   DSSER_AUTOFLOW_DISABLE           0 
#define   DSSER_AUTOFLOW_ENABLE            1
#define   DSSER_RS232                      0
#define   DSSER_RS422                      2 
#define   DSSER_RS485                      4 
#define   DSSER_KLINE                      6 

/****************************************** dsser_status_read(...); */
#define   DSSER_STATUS_LSR                 1
#define   DSSER_STATUS_MSR                 2 
#define   DSSER_STATUS_IIR_FCR             4

/****************************************** dsser_set(...); */

#define   DSSER_SET_UART_FREQUENCY         1

/************************************* dsser_subint_en/disable (...); */

/*--- Use this defines only to enable or disable the subinterrupts */

#define   DSSER_TRIGGER_LEVEL_SUBINT_MASK  0x01
#define   DSSER_TX_FIFO_EMPTY_SUBINT_MASK  0x02
#define   DSSER_RECEIVER_LINE_SUBINT_MASK  0x04
#define   DSSER_MODEM_STATE_SUBINT_MASK    0x08

/************************************* dsser_dummy_fcn (...); */

/*--- returned subinterrupt number */

#define   DSSER_NO_SUBINT                  -1
#define   DSSER_TRIGGER_LEVEL_SUBINT        0
#define   DSSER_TX_FIFO_EMPTY_SUBINT        1
#define   DSSER_RECEIVER_LINE_SUBINT        2
#define   DSSER_MODEM_STATE_SUBINT          3


/******************************************************************************
  common constants for DSSER:
******************************************************************************/

#define   DSSER_MAX_BOARDS              17  /* 16 IO-Boards + 1 Master */
#define   DSSER_MAX_DEVICES              4  /* max four channel per board */
#define   DSSER_UART_QUEUE             0x3

/******************************************************************************
  common macros for DSSER:
******************************************************************************/

/* error messages of the serial interafce */

#define   DSSER_NOT_FOUND                        100
#define   DSSER_NOT_FOUND_TXT                      \
"( 0x%02lX, ch=%d, Board not found!"

#define   DSSER_ALLOC_ERROR_MSG                  501
#define   DSSER_ALLOC_ERROR_MSG_TXT                \
"( 0x%02lX, ch=%d, memory: Allocation error on master."

#define   DSSER_ILLEGAL_CHANNEL                  508
#define   DSSER_ILLEGAL_CHANNEL_TXT                \
"( 0x%02lX, ch=%d, channel: out of range !"

#define   DSSER_CHANNEL_INIT_ERROR               601
#define   DSSER_CHANNEL_INIT_ERROR_TXT             \
"( 0x%02lX, ch=%d, serCh: The UART channel wasn't initialized."

#define   DSSER_ILLEGAL_BAUDRATE                 602 
#define   DSSER_ILLEGAL_BAUDRATE_TXT               \
"( 0x%02lX, ch=%d, baudrate: Illegal !"

#define   DSSER_ILLEGAL_DATABIT                  603 
#define   DSSER_ILLEGAL_DATABIT_TXT                \
"( 0x%02lX, ch=%d, databits: Use range 5..8 bits!"

#define   DSSER_ILLEGAL_STOPBIT                  604
#define   DSSER_ILLEGAL_STOPBIT_TXT                \
"( 0x%02lX, ch=%d, stopbits: Illegal number (1-2 bits allowed)!"

#define   DSSER_ILLEGAL_PARITY                   605 
#define   DSSER_ILLEGAL_PARITY_TXT                 \
"( 0x%02lX, ch=%d, parity: Illegal parity!"

#define   DSSER_ILLEGAL_UART_TRIGGER_LEVEL       606 
#define   DSSER_ILLEGAL_UART_TRIGGER_LEVEL_TXT     \
"( 0x%02lX, ch=%d, trigger_level: Illegal UART trigger level!"

#define   DSSER_ILLEGAL_USER_TRIGGER_LEVEL       607 
#define   DSSER_ILLEGAL_USER_TRIGGER_LEVEL_TXT     \
"( 0x%02lX, ch=%d, trigger_level: Illegal user trigger level!"

#define   DSSER_ILLEGAL_FIFO_MODE                608
#define   DSSER_ILLEGAL_FIFO_MODE_TXT              \
"( 0x%02lX, ch=%d, fifo_mode: Use range 0..(fifo_size - 1) bytes!"

#define   DSSER_ILLEGAL_UART_RSMODE              609
#define   DSSER_ILLEGAL_UART_RSMODE_TXT            \
"( 0x%02lX, ch=%d, uart_mode: Transceiver not supported!"

#define   DSSER_ILLEGAL_AUTOFLOW                 611
#define   DSSER_ILLEGAL_AUTOFLOW_TXT               \
"( 0x%02lX, ch=%d, uart_mode: Autoflow is not supported!"

#define   DSSER_ILLEGAL_FIFO_SIZE                612
#define   DSSER_ILLEGAL_FIFO_SIZE_TXT              \
"( 0x%02lX, ch=%d, Buffersize: Illegal"

#define   DSSER_ILLEGAL_FREQUENCY                613
#define   DSSER_ILLEGAL_FREQUENCY_TXT              \
"( 0x%02lX, ch=%d, frequency: Illegal"

#ifdef _DS1003
  #define DSSER_PARA_OFFS     (((base) & 0xF0) >> 4)
#endif

#ifdef _DS1005
  #define DSSER_PARA_OFFS     (((base) & 0x7800000) >> 23)
#endif

#ifdef _DS1006
  #define DSSER_PARA_OFFS     (((base) & 0x7800000) >> 23) 
#endif

#ifdef _DS1401
  #define DSSER_PARA_OFFS     ((base >> 21) & 0x0F)
#endif

#ifdef _DS1103
  #define DSSER_PARA_OFFS     0 
#endif

#ifdef _DS1104
  #define DSSER_PARA_OFFS     0 
#endif

/*******************************************************************************
*  typedefs
*******************************************************************************/

/*--- UART ISR register struct */

typedef union
{
  UInt32    Byte;
  struct
  {
	 
  #if (defined(_DS1003) || defined(_DS1006))
    unsigned DSSER_INT_STATUS        : 1;
    unsigned DSSER_INT_PRIORITY_BIT0 : 1;
    unsigned DSSER_INT_PRIORITY_BIT1 : 1;
    unsigned DSSER_INT_PRIORITY_BIT2 : 1;
    unsigned DSSER_BIT4              : 1;
    unsigned DSSER_BIT5              : 1; 
    unsigned DSSER_FIFO_STATUS_BIT0  : 1;
    unsigned DSSER_FIFO_STATUS_BIT1  : 1;
    unsigned dummy                   : 24;
  #else
    unsigned dummy                   : 24;
    unsigned DSSER_FIFO_STATUS_BIT1  : 1;
    unsigned DSSER_FIFO_STATUS_BIT0  : 1;
    unsigned DSSER_BIT5              : 1;
    unsigned DSSER_BIT4              : 1;
    unsigned DSSER_INT_PRIORITY_BIT2 : 1;
    unsigned DSSER_INT_PRIORITY_BIT1 : 1;
    unsigned DSSER_INT_PRIORITY_BIT0 : 1;
    unsigned DSSER_INT_STATUS        : 1;                     
  #endif 
  }Bit;
}
 dsser_ISR;

/*--- UART LSR register struct */

typedef union
{
  UInt32    Byte;
  struct
  {
  #if (defined(_DS1003) || defined(_DS1006))
    unsigned DSSER_RECEIVE_DATA_RDY  : 1;
    unsigned DSSER_OVERRUN_ERR       : 1;
    unsigned DSSER_PARITY_ERR        : 1;
    unsigned DSSER_FRAMING_ERR       : 1;
    unsigned DSSER_BREAK_STATUS      : 1;
    unsigned DSSER_THR_STATUS        : 1; 
    unsigned DSSER_THR_TSR_STATUS    : 1;
    unsigned DSSER_FIFO_DATA_ERR     : 1;
    unsigned dummy                   : 24;
  #else
    unsigned dummy                   : 24;
    unsigned DSSER_FIFO_DATA_ERR     : 1;
    unsigned DSSER_THR_TSR_STATUS    : 1;
    unsigned DSSER_THR_STATUS        : 1; 
    unsigned DSSER_BREAK_STATUS      : 1;
    unsigned DSSER_FRAMING_ERR       : 1;
    unsigned DSSER_PARITY_ERR        : 1;
    unsigned DSSER_OVERRUN_ERR       : 1;
    unsigned DSSER_RECEIVE_DATA_RDY  : 1;
  #endif 
  }Bit;


}
 dsser_LSR;

/*--- UART MSR register struct */

typedef union
{
  UInt32    Byte;
  struct
  {
  #if (defined(_DS1003) || defined(_DS1006))
    unsigned DSSER_CTS_STATUS        : 1;
    unsigned DSSER_DSR_STATUS        : 1;
    unsigned DSSER_RI_STATUS         : 1;
    unsigned DSSER_CD_STATUS         : 1;
    unsigned DSSER_RTS_STATUS        : 1;
    unsigned DSSER_DTR_STATUS        : 1; 
    unsigned DSSER_OP1_STATUS        : 1;
    unsigned DSSER_OP2_STATUS        : 1;
    unsigned dummy                   : 24;
  #else
    unsigned dummy                   : 24;
    unsigned DSSER_OP2_STATUS        : 1;
    unsigned DSSER_OP1_STATUS        : 1;
    unsigned DSSER_DTR_STATUS        : 1;
    unsigned DSSER_RTS_STATUS        : 1;
    unsigned DSSER_CD_STATUS         : 1;
    unsigned DSSER_RI_STATUS         : 1;
    unsigned DSSER_DSR_STATUS        : 1;
    unsigned DSSER_CTS_STATUS        : 1;                     
  #endif 
  }Bit;
}
 dsser_MSR;

typedef struct
{
  UInt32 data0;
  UInt32 data1;
  UInt32 data2;
  UInt32 module;
  UInt32 queue;
  UInt32 type;
   Int32 index;
}
 dsserService;

typedef void (*dsser_subint_handler_t)( void* serCh, Int32 subint );

/** SERIAL CHANNEL struct. */
typedef struct
{
  /*--- public ----------------------------*/
  
  /* interrupt status register */
  dsser_ISR              intStatusReg;    
    
  /* line status register */
  dsser_LSR              lineStatusReg;
    
  /* modem status register */
  dsser_MSR              modemStatusReg;

  /*--- protected ----------------------------*/

  /*--- serial channel allocation ---*/
  UInt32                 module;
  UInt32                 channel;
  Int32                  board_bt;
  UInt32                 board;
  UInt32                 fifo_size;
  UInt32                 frequency;
  /*--- serial channel configuration ---*/
  UInt32                 baudrate;     
  UInt32                 databits;     
  UInt32                 stopbits;     
  UInt32                 parity;
  UInt32                 rs_mode;
  UInt32                 fifo_mode;
  UInt32                 uart_trigger_level;
  UInt32                 user_trigger_level;

  dsser_subint_handler_t subint_handler;
  dsser_subint_handler_t subint_handler_saved;  
  dsserService*          serService;
  dsfifo_t*              txFifo;
  dsfifo_t*              rxFifo;
  UInt32                 queue;
  UInt8                  isr;
  UInt8                  lsr;
  UInt8                  msr;
  UInt32                 interrupt_mode;
  UInt8                  subint_mask;
  Int8                   subint;        
}
 dsserChannel;

extern dsserChannel* dsser_lookup_table[DSSER_MAX_BOARDS][DSSER_MAX_DEVICES];

/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#endif /* __dsserdef__ */
