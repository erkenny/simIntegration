/******************************************************************************
*
* FILE:
*   Ser1103.h
*
* RELATED FILES:
*    Ser1103.c
*
* DESCRIPTION:
*    This module contains functions to access the UART (serial interface)
*    of the DS1103. 
*
*
* HISTORY: RK initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: SER1103.H $ $Revision: 1.2 $ $Date: 2003/08/07 12:18:59GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSER1103.pj $
******************************************************************************/

#ifndef __Ser1103__
#define __Ser1103__

#include <Dstypes.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

/* Addresses */

#define DS1103_UART      0x11000000
#define DS1103_SETUP     0x10000088
#define DS1103_RESET     0x10000040


/* UART interrupt enable */

#define DS1103_SER_NO_INT         0x0
#define DS1103_SER_RECDATA_INT    0x1                    /* received data interrupt */
#define DS1103_SER_THR_EMPTY_INT  0x2                  /*THR register empty interrupt */
#define DS1103_SER_RECLINE_ST_INT 0x4                 /* receiver line status interrupt */
#define DS1103_SER_MODEM_ST_INT   0x8                   /* modem status interrupt */

#define DS1103_SER_INTERRUPT_DISABLE 0x0

/* UART interrupt identification */

#define DS1103_SER_INT_MODEM_STATE         0x0
#define DS1103_SER_INT_THR_EMPTY           0x2
#define DS1103_SER_INT_RECDATA_AVAIL       0x4
#define DS1103_SER_INT_RECLINE_STATE       0x5
#define DS1103_SER_INT_CHAR_TIMEOUT        0xC


/* UART Linestatus register */

#define DS1103_SER_LSR_DATARDY     0x1
#define DS1103_SER_LSR_OVERRUN_ERR 0x2
#define DS1103_SER_LSR_PARITY_ERR  0x4
#define DS1103_SER_LSR_FRAMING_ERR 0x8
#define DS1103_SER_LSR_BREAK_INT  0x10
#define DS1103_SER_LSR_THRE       0x20
#define DS1103_SER_LSR_TEMT       0x40
#define DS1103_SER_RECFIFO_ERR    0x80



/* FIFO initialization */

#define DS1103_SER_FIFOLEN     16
#define DS1103_SER_FIFO_ENABLE 0x1                   /* enable FIFO */
#define DS1103_SER_FIFO_DISABLE 0x0
#define DS1103_SER_FIFO_RECRESET 0x2                 /* reset receiver FIFO */
#define DS1103_SER_FIFO_TRMRESET 0x4                 /*reset transmitter FIFO */
#define DS1103_SER_FIFO_STATE   0xC0


/* Autoflow */

#define DS1103_SER_AUTOFLOW_DISABLE 0x0
#define DS1103_SER_AUTOFLOW_ENABLE 0x1

/* parity bit generation */

#define DS1103_SER_NO_PARITY    0                            /* no parity */
#define DS1103_SER_ODD_PARITY   1                           /* odd parity */
#define DS1103_SER_EVEN_PARITY  2                          /* even parity */

/* RTS signal */

#define DS1103_SER_RTS_ON       0x00000002               /* RTS signal on */
#define DS1103_SER_RTS_OFF      0x00000000              /* RTS signal off */

/* line transceivers */

#define DS1103_SER_RS232        0x00000000           /* RS232 transceiver */
#define DS1103_SER_RS422        0x00000001           /* RS422 transceiver */

/* stop bits */

#define DS1103_SER_1_STOPBIT    0                           /* 1 stop bit */
#define DS1103_SER_2_STOPBIT    1                   /* 1.5 or 2 stop bits */


#define DS1103_SERIAL_REG_IIR ( (UInt16) ( (volatile ds1103_uart_reg*)DS1103_UART)->iir_fcr  & 0xFF )

#define DS1103_SERIAL_REG_LSR ( (UInt16) ( (volatile ds1103_uart_reg*)DS1103_UART)->lsr & 0xF )
#define DS1103_SERIAL_REG_INT_ID ( (UInt16) ( (volatile ds1103_uart_reg*)DS1103_UART)->iir_fcr & 0xF )

#define DS1103_SER_FALSE     0x00
#define DS1103_SER_TRUE      0x01

#define DS1103_SER_IER_MSK    0x0F

#define DS1103_SER_CLK_SRC   16.000000e6       /* clock source of UART in Hz */

/* macros and defines for new interrupt handling */

#define DS1103_SER_BYTE_AVAILABLE    (ds1103_ser_sw_fifo_ridx != ds1103_ser_sw_fifo_widx)
#define DS1103_SER_INC(idx)          ((idx) = ((idx) + 1) & 0x1F)
#define DS1103_SER_SW_FIFO_EMPTY     0xFF00

typedef struct {
  UInt16 rbr_thr;
  UInt16 ier;
  UInt16 iir_fcr;
  UInt16 lcr;
  UInt16 mcr;
  UInt16 lsr;
  UInt16 msr;
  UInt16 scr;
}ds1103_uart_reg;


typedef enum uart_tx_state
{

  DS1103_SER_TX_READY = 0,
  DS1103_SER_TX_FAILED

} uart_state_tx_enum;


typedef enum uart_rx_state
{
  DS1103_SER_RX_READY = 0,
  DS1103_SER_RX_FAILED

} uart_state_rx_enum;


/******************************************************************************
  object declarations
******************************************************************************/

extern UInt16 ds1103_ser_trigger_level[];
extern UInt16 ds1103_ser_rest_data_ind;
extern UInt16 ds1103_ser_autoflow[];

extern Int8 ds1103_ser_sw_fifo[];
extern Int8 ds1103_ser_sw_fifo_ridx;
extern Int8 ds1103_ser_sw_fifo_widx;
extern Int16 ds1103_ser_sw_lsr_register;
extern Int16 ds1103_ser_sw_msr_register;

/******************************************************************************
  function prototypes
******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE extern
#endif

/* function prototypes go here */


extern void ds1103_serial_init(UInt32 baudrate, UInt16 data_bits, UInt16 stop_bits,
                               UInt16 parity, UInt16 rsmode, UInt16 trigger_level, UInt16 autoflow, UInt16 interrupt_mode );

__INLINE Int16 ds1103_serial_tx_byte(UInt16 data);
__INLINE Int16 ds1103_serial_rx_byte(Int8 *data);
__INLINE void ds1103_serial_intmode_txbyte(UInt16 data);
__INLINE void ds1103_serial_intmode_rxbyte(Int8 *data);
__INLINE UInt16 ds1103_serial_pollmode_send(Int8* data, UInt16 len);
__INLINE UInt16 ds1103_serial_intmode_send(Int8* data, UInt16 len);
__INLINE Int16 ds1103_serial_pollmode_receive(Int8* data, UInt16 *len);
__INLINE void ds1103_serial_intmode_receive(Int8* data, UInt16 *len);
__INLINE void ds1103_serial_fifo_enable(UInt16 trigger_level);
__INLINE void ds1103_serial_fifo_disable(void);
__INLINE void ds1103_serial_autoflow_enable(void);
__INLINE void ds1103_serial_autoflow_disable(void);
__INLINE Int16 ds1103_serial_transmitter_empty(void);
__INLINE Int16 ds1103_serial_data_ready(void);
__INLINE void ds1103_serial_interrupt_set(UInt16 mask);
__INLINE Int16 ds1103_serial_fifo_state(void);

__INLINE Int16 ds1103_serial_fifo_byte_get(void);
__INLINE Int8 ds1103_serial_msr_get(void);
__INLINE Int8 ds1103_serial_lsr_get(void);
__INLINE Int8 ds1103_serial_acknowledge(void);

#undef __INLINE

/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
#include <Ser1103.c>
#endif

#endif /* Ser1103.h */



