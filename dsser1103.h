/******************************************************************************
*
* FILE:
*    dsser1103.h
*
* RELATED FILES:
*    dsser1103.c
*
* DESCRIPTION:
*    This module contains functions to access the UART (serial interface)
*    of the DS1103.
*
*
* HISTORY: MH initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsser1103.h $ $Revision: 1.2 $ $Date: 2003/08/07 12:19:01GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSER1103.pj $
******************************************************************************/

#ifndef __DSSER1103__
#define __DSSER1103__

#include <dstypes.h>
#include <dsfifo.h>
#include <dsserdef.h>

/************************************************** ds1103_dsser_config(...); */
/*--- stop bits */

#define   DS1103_DSSER_1_STOPBIT                 0            /* 1 stop bit */
#define   DS1103_DSSER_2_STOPBIT                 1    /* 1.5 or 2 stop bits */

/*--- parity bit */

#define   DS1103_DSSER_NO_PARITY                 0
#define   DS1103_DSSER_ODD_PARITY                1
#define   DS1103_DSSER_EVEN_PARITY               2
#define   DS1103_DSSER_FORCED_PARITY_ONE         3
#define   DS1103_DSSER_FORCED_PARITY_ZERO        4

/*--- UART trigger level */

#define   DS1103_DSSER_1_BYTE_TRIGGER_LEVEL      0
#define   DS1103_DSSER_4_BYTE_TRIGGER_LEVEL      1
#define   DS1103_DSSER_8_BYTE_TRIGGER_LEVEL      2
#define   DS1103_DSSER_14_BYTE_TRIGGER_LEVEL     3

/*--- User trigger level */

#define   DS1103_DSSER_DEFAULT_TRIGGER_LEVEL    -1

/*--- Autoflow */

#define   DS1103_DSSER_AUTOFLOW_DISABLE          0
#define   DS1103_DSSER_AUTOFLOW_ENABLE           1

/*--- RS mode */
#define   DS1103_DSSER_RS232                     2
#define   DS1103_DSSER_RS422                     0

/******************************************* ds1103_dsserial_dummy_fcn (...); */

/*--- returned subinterrupt number */

#define   DS1103_DSSER_NO_SUBINT                -1
#define   DS1103_DSSER_TRIGGER_LEVEL_SUBINT      0
#define   DS1103_DSSER_TX_FIFO_EMPTY_SUBINT      1
#define   DS1103_DSSER_RECEIVER_LINE_SUBINT      2
#define   DS1103_DSSER_MODEM_STATE_SUBINT        3

/* low level! ********************************* ds1103_dsserial_config (...); */
/*--- UART interrupt enable */

#define   DS1103_DSSER_NO_INT         0x0
#define   DS1103_DSSER_RXRDY_INT      0x1          /* received data interrupt */
#define   DS1103_DSSER_THR_EMPTY_INT  0x2      /*THR register empty interrupt */
#define   DS1103_DSSER_LINE_STATE_INT 0x4   /* receiver line status interrupt */
#define   DS1103_DSSER_MODEM_ST_INT   0x8           /* modem status interrupt */

/* low level! ******************** ds1103_dsserial_register_read/write(...);  */

#define   DS1103_DSSER_RBR_THR  0x00    /* Rec.buf. (rd), transm.holding (wr) */
#define   DS1103_DSSER_IER      0x01             /* Interrupt enable register */
#define   DS1103_DSSER_IIR_FCR  0x02 /* Intpt. Ident. (rd), FIFO control (wr) */
#define   DS1103_DSSER_LCR      0x03                 /* line control register */
#define   DS1103_DSSER_MCR      0x04                /* modem control register */
#define   DS1103_DSSER_LSR      0x05                  /* line status register */
#define   DS1103_DSSER_MSR      0x06                 /* modem status register */
#define   DS1103_DSSER_SCR      0x07                      /* scratch register */

/*******************************************************************************
  function prototypes
*******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE extern
#endif

/*******************************************************************************
 high level section : UART
*******************************************************************************/

extern Int32 ds1103_dsser_init(const UInt32 uart_addr,
                                       const UInt32 fifo_size);

extern Int32 ds1103_dsser_config(dsserChannel* serCh,
                                  const UInt32 fifo_mode,
                                  const UInt32 baudrate,
                                  const UInt32 databits,
                                  const UInt32 stopbits,
                                  const UInt32 parity,
                                  const UInt32 uart_trigger_level,
                                  const  Int32 user_trigger_level,
                                  const UInt32 rs_mode,
                                  const UInt32 autoflow);

__INLINE Int32 ds1103_dsser_transmit(dsserChannel* serCh, 
                                      const UInt32 datalen,
                                      UInt8* data,
                                      UInt32* count);

__INLINE Int32 ds1103_dsser_fifo_reset(dsserChannel* serCh);

/******************************************************************************
 low level section : UART
******************************************************************************/

dsserChannel* ds1103_dsserial_init(const UInt32 uart_addr);

Int32 ds1103_dsserial_config( dsserChannel* serCh,
                         const UInt32 baudrate,
                         const UInt32 databits,
                         const UInt32 stopbits,
                         const UInt32 parity,
                         const UInt32 uart_trigger_level,
                         const UInt32 autoflow,
                         const UInt32 rs_mode,
                         const UInt32 interrupt_mode );

          void ds1103_dsserial_free(dsserChannel* serCh);

__INLINE Int32 ds1103_dsserial_receive(const dsserChannel* serCh, 
                                                    UInt32 len, 
                                                    UInt8* data);
__INLINE Int32 ds1103_dsserial_transmit(const dsserChannel* serCh,
                                                     UInt32 len,
                                                     UInt8* data);
__INLINE void  ds1103_dsserial_register_write(const dsserChannel* serCh,
                                              const UInt8 register_type,
                                              const UInt8 register_value);
__INLINE UInt8 ds1103_dsserial_register_read(const dsserChannel* serCh, 
                                             const UInt8 register_type);

/******************************************************************************
 internal used functions
******************************************************************************/

__INLINE Int32 ds1103_dsserial_io_ier_set(const dsserChannel* serCh,
                                                        UInt8 value);

#undef __INLINE

/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
#include <dsser1103.c>
#endif

#endif /* dsser1103.h */



