/******************************************************************************
*
* FILE:
*   Ser1103.c
*
* RELATED FILES:
*   Ser1103.h
*
* DESCRIPTION:
*    This module contains functions to access the UART (serial interface)
*    of the DS1103.
*
* HISTORY: RK initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: SER1103.C $ $Revision: 1.5 $ $Date: 2003/11/14 11:20:16GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSER1103.pj $
******************************************************************************/


#include <stdlib.h>
#include <Ser1103.h>
#include <Dsmsg.h>
#include <Int1103.h>
#include <Init1103.h>
#include <dsstd.h>
#include <ds1103_msg.h>
#include <dsav1103.h>


#include <dsserdef.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

#define DS1103_SER_FIFOSHF 4



/******************************************************************************
  object declarations
******************************************************************************/

#ifndef _INLINE

UInt16 ds1103_ser_trigger_level[]= {0, 0, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80,
                                    0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,
                                    0xC0, 0xC0};

UInt16 ds1103_ser_rest_data_ind = 0;

UInt16 ds1103_ser_autoflow[] = { 0xFFDD, 0x22 };

Int8 ds1103_ser_sw_fifo[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Int8 ds1103_ser_sw_fifo_ridx = 0;
Int8 ds1103_ser_sw_fifo_widx = 0;
Int16 ds1103_ser_sw_lsr_register = 0;
Int16 ds1103_ser_sw_msr_register = 0;

#else

extern UInt16 ds1103_ser_trigger_level[];
extern UInt16 ds1103_ser_rest_data_ind;
extern UInt16 ds1103_ser_autoflow[];

extern Int8 ds1103_ser_sw_fifo[];
extern Int8 ds1103_ser_sw_fifo_ridx;
extern Int8 ds1103_ser_sw_fifo_widx;
extern Int16 ds1103_ser_sw_lsr_register;
extern Int16 ds1103_ser_sw_msr_register;

#endif


/******************************************************************************
  function prototypes
******************************************************************************/


/******************************************************************************
  function declarations
******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE
#endif


/******************************************************************************
*
* FUNCTION:
*            void ds1103_serial_init(UInt32 baudrate, UInt16 data_bits,
*                                    UInt16 stop_bits, UInt16 parity,
*                                    UInt16 rsmode, UInt16 trigger_level,
*                                    UInt16 autoflow, UInt16 interrupt_mode)
*
*
* SYNTAX:
*                 ds1103_serial_init(UInt32 baudrate, UInt16 data_bits,
*                                    UInt16 stop_bits, UInt16 parity,
*                                    UInt16 rsmode, UInt16 trigger_level,
*                                    UInt16 autoflow, UInt16 interrupt_mode)
*
* DESCRIPTION: This function initializes the serial interface (UART).
*
* PARAMETERS: 
*              baudrate   transmission speed in bits per second
*                         (300, 600, 1200, 2400, 4800, 9600 19200,
*                          38400, 115000, 1000000)
*              data_bits  number of data bits (range: up to 8) 
*              stop_bits  number of stop bits (range: up to 2, dependend 
*                         on the number of data bits)                         
*              parity     no, even or odd parity bit
*                         use DS1103_SER_NO_PARITY, DS1103_SER_ODD_PARITY or
*                         DS1103_SER_EVEN_PARITY
*              rsmode     mode of the serial port (RS232 or RS422)
*                         use DS1103_SER_RS232 or DS1103_SER_RS422
*              trigger_level  enables/disables the Fifo 
*                             use 0 to disable the Fifo
*                             use x (range: 1 to 14) to enable the Fifo with
*                                 a trigger level of x bytes
*              autoflow   enables/disables the autoflow-mode (handshaking)
*                         use DS1103_SER_AUTOFLOW_DISABLE or  
*                             DS1103_SER_AUTOFLOW_ENABLE
*              interrupt_mode enables/disables the UART-interrupts
*                         use DS1103_SER_NO_INT for no interrupt
*                             DS1103_SER_RECDATA_INT for receive interrupt
*                             DS1103_SER_THR_EMPTY_INT for transmit buffer empty 
*                                                      interrupt
*                             DS1103_SER_RECLINE_ST_INT for receiver line status
*                                                      interrupt
*                             DS1103_SER_MODEM_ST_INT for modem status
*                                                      interrupt               
*   
*
* RETURNS: void
*
* NOTE:
*
*
******************************************************************************/

#ifndef _INLINE

 void ds1103_serial_init(UInt32 baudrate, UInt16 data_bits, UInt16 stop_bits,
                         UInt16 parity, UInt16 rsmode, UInt16 trigger_level,
                         UInt16 autoflow, UInt16 interrupt_mode)
{

  volatile ds1103_uart_reg* uartptr = (volatile ds1103_uart_reg*)DS1103_UART;
  volatile UInt16* resetptr = (volatile UInt16*)DS1103_RESET;
  volatile UInt16* setupptr = (volatile UInt16*)DS1103_SETUP;
  UInt16   pen, stb, eps;
  Int8     dummy ;
  Float64 divisor;
  Float32 f_target, f_actual;
  
  UInt8 vco_div = 0;
  UInt8 ref_div = 0;
  Int16 error;

  eps = 0;
  stb = 0;


  *resetptr = *resetptr & 0xFFFD;

  if(!ds1103_advanced_io)
	  {
		divisor = DS1103_SER_CLK_SRC / (16 * (Float64) baudrate) ;
		divisor = divisor + 0.5;
	  }
  else
	  {
		/* calculate divisor with default clock source of the UART */
        divisor = DS1103_SER_CLK_SRC / (16 * (Float64) baudrate);
        f_target = ((UInt32) divisor << 4) * (Float64) baudrate;
        error = ds1103_av9110_clock_calc_ser( f_target, &f_actual,
                         &vco_div, &ref_div);

       
        if( error == AV9110_PLLFREQ_ERROR )
            msg_error_printf(MSG_SM_RTLIB, DSSER_ILLEGAL_BAUDRATE, DSSER_ILLEGAL_BAUDRATE_TXT,baudrate,0);
        else
            ds1103_av9110_clock_write_ser( vco_div,ref_div );
       }
  
  data_bits = (data_bits - 5) & 0x3;

  if (parity == DS1103_SER_NO_PARITY)
     pen = 0;                                        /* disable parity */
  else
  {
    pen = 0x08;                                 /* enable parity */
    if (parity == DS1103_SER_EVEN_PARITY)
      eps = 0x10;                                 /* even parity */
  }

  if(stop_bits == DS1103_SER_2_STOPBIT)
    stb = 0x04;                            /* 1.5 or 2 stop bits */

  uartptr->lcr  =    data_bits
                   | stb
                   | pen
                   | eps
                   | 0x0          /* stick parity = 0 */
                   | 0x0          /* break = 0 */
                   | 0x80;         /* set divisor latch access bit to progr. dlm + dll*/


  DS1103_EIEIO;
  uartptr->ier = ( (UInt32)divisor >> 8) & 0xff;
  uartptr->rbr_thr = (UInt32)divisor & 0xff;
  DS1103_EIEIO;
  uartptr->lcr = uartptr->lcr & 0x0000007f;         /* reset divisor latch access bit */
  uartptr->ier = 0;                                 /* disable interrupts */
  uartptr->mcr = 0;

  if(rsmode == DS1103_SER_RS422)
   *setupptr = (*setupptr & 0xFFEF) | 0x10;
  else *setupptr = (*setupptr & 0xFFEF) ;

 uartptr->iir_fcr = 0;

 DS1103_EIEIO;
 if( ds1103_debug & DS1103_DEBUG_POLL )
   DS1103_MSG_INFO_SET(DS1103_UART_TX_EMPTY_POLL_START);
 while( (uartptr->lsr & 0x20) != 0x20 );   /* wait until tx_empty */
 if( ds1103_debug & DS1103_DEBUG_POLL )
   DS1103_MSG_INFO_SET(DS1103_UART_TX_EMPTY_POLL_END);

 /* while rec.buf. full, read buf. */
 DS1103_EIEIO;
 if( ds1103_debug & DS1103_DEBUG_POLL )
   DS1103_MSG_INFO_SET(DS1103_UART_REC_READ_POLL_START);
 while ( ds1103_serial_data_ready() )  /* read receiver buffer until empty */
    ds1103_serial_rx_byte(&dummy);
 if( ds1103_debug & DS1103_DEBUG_POLL )
   DS1103_MSG_INFO_SET(DS1103_UART_REC_READ_POLL_END);

 if( trigger_level != 0 )
 uartptr->iir_fcr = DS1103_SER_FIFO_ENABLE | DS1103_SER_FIFO_RECRESET
                   | DS1103_SER_FIFO_TRMRESET | ds1103_ser_trigger_level[trigger_level];
 else uartptr->iir_fcr = 0;


 if(autoflow == DS1103_SER_AUTOFLOW_ENABLE)
 {
  uartptr->mcr = uartptr->mcr | ds1103_ser_autoflow[DS1103_SER_AUTOFLOW_ENABLE];
 }
 else
 {
   uartptr->mcr = uartptr->mcr & ds1103_ser_autoflow[DS1103_SER_AUTOFLOW_DISABLE];
 };

  uartptr->ier = interrupt_mode & DS1103_SER_IER_MSK;

  DS1103_EIEIO;


}

#endif




/******************************************************************************
*
* FUNCTION:
*             Int16 ds1103_serial_tx_byte(UInt16 data)
*
* SYNTAX:
*             ds1103_serial_tx_byte(UInt16 data)
*
* DESCRIPTION: This function sends one byte (LSB of data).
*
* PARAMETERS: UInt16 data: data-byte (LSB)
*
* RETURNS: DS1103_SER_TX_FAILED if transmission wasn't successful.
*          DS1103_SER_TX_READY if the byte was transmitted successful.
*
* NOTE:    It's recommended to poll this function until DS1103_SER_TX_READY.
*
*
******************************************************************************/

__INLINE Int16 ds1103_serial_tx_byte(UInt16 data)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  if( (uartptr->lsr & 0x20) != 0x20 )
  {
    DS1103_SYNC;
    DS1103_EIEIO;
    return(DS1103_SER_TX_FAILED);
  };

  uartptr->rbr_thr = data;
  DS1103_SYNC;
  DS1103_EIEIO;
  return(DS1103_SER_TX_READY);

}





/******************************************************************************
*
* FUNCTION:
*        void ds1103_serial_intmode_txbyte(UInt16 data)
*
* SYNTAX:
*        ds1103_serial_intmode_txbyte(UInt16 data)
*
* DESCRIPTION: This function sends the LSB of data. The Tx-Interrupt
*              (DS1103_SER_INT_THR_EMPTY) and the fifo must be enabled.
*              Call this function in an UART-Interrupt-handler
*
* PARAMETERS: UInt16 data: the LSB of data contains the byte which
*                          is transmitted.
*
* RETURNS: void
*
* NOTE:  use the function with 1-Byte-Trigger-Fifo and Autoflow 
*
*
******************************************************************************/

__INLINE void ds1103_serial_intmode_txbyte(UInt16 data)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  uartptr->rbr_thr = data;
  DS1103_SYNC;
  DS1103_EIEIO;

}





/******************************************************************************
*
* FUNCTION:
*   Int16 ds1103_serial_rx_byte(Int8 *data)
*
* SYNTAX: ds1103_serial_rx_byte(Int8 *data)
*
* DESCRIPTION: code for serial input
*              Received value is read from the receiver buffer register
*              as right adjusted value within range 0 .. 255
*
* PARAMETERS: Addr. of data
*
* RETURNS: DS1103_SER_RX_FAILED if no byte was received
*          DS1103_SER_RX_READY if byte was received
*
* NOTE: The address of data must be an address of the Local RAM!
*
******************************************************************************/

__INLINE Int16 ds1103_serial_rx_byte(Int8 *data)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

    if( (uartptr->lsr & 0x1) == 0 )
    {
     DS1103_SYNC;
     DS1103_EIEIO;
     return(DS1103_SER_RX_FAILED);
    };
    *data = (Int8) uartptr->rbr_thr;
    DS1103_SYNC;
    DS1103_EIEIO;
    return(DS1103_SER_RX_READY);

}





/******************************************************************************
*
* FUNCTION:
*   void ds1103_serial_intmode_rxbyte(Int8 *data)
*
* SYNTAX: 
*        ds1103_serial_intmode_rxbyte(Int8 *data)
*
* DESCRIPTION: code for serial input
*              Received value is read from the receiver buffer register
*              as right adjusted value within range 0 .. 255
*              You can use this function in an UART-Interrupthandler.
*
* PARAMETERS: Adr. of data
*
* RETURNS: void
*
* NOTE: The address of data must be an address of the Local RAM!
*       Use this function in the Interruptmode with Fifo and Autoflow. 
*
******************************************************************************/

__INLINE void ds1103_serial_intmode_rxbyte(Int8 *data)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

    *data = uartptr->rbr_thr;
    DS1103_SYNC;
    DS1103_EIEIO;

}





/******************************************************************************
*
* FUNCTION:
*   Int16 ds1103_serial_transmitter_empty(void)
*
* SYNTAX:
*        ds1103_serial_transmitter_empty(void)
*
* DESCRIPTION: code for reading status of THRE-bit (Transmitter Holding Register
*              Empty) of the Line Status Register (Bit 5)
*
*
* PARAMETERS: no
*
* RETURNS: status of THRE-bit   
*
* NOTE:
*
*
******************************************************************************/
__INLINE Int16 ds1103_serial_transmitter_empty(void)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;
  Int16 val;

  val = (uartptr->lsr & 0x20) != 0x20;
  DS1103_SYNC;
  DS1103_EIEIO;
  return (val);

}



/******************************************************************************
*
* FUNCTION:
*        Int16 ds1103_serial_data_ready(void)
*
* SYNTAX:
*        ds1103_serial_data_ready(void)
*
* DESCRIPTION: code for reading status of DR-bit (Data Ready) of the Line 
*              Status Register 
*
* PARAMETERS: no
*
* RETURNS: status of DR-bit   true = Data in RBR or Fifo, false = no Data in RBR or Fifo
*
* NOTE:
*
*
******************************************************************************/
__INLINE Int16 ds1103_serial_data_ready(void)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  Int16 val;

  val =  (uartptr->lsr & 0x01) == 0x01;
  DS1103_SYNC;
  DS1103_EIEIO;
  return (val);

 }



/******************************************************************************
*
* FUNCTION:
*     void ds1103_serial_fifo_enable(UInt16 trigger_level)
*
* SYNTAX:
*      ds1103_serial_fifo_enable(UInt16 trigger_level)
*
* DESCRIPTION: This function enables the fifo.
*
*
* PARAMETERS: UInt16 trigger_level: Fifo interrupt occurs (if enabled) if the
*                                   Fifo trigger level (number of received 
*                                   bytes) is reached
*
*
* RETURNS: void
*
* NOTE: 
*
*
******************************************************************************/

__INLINE void ds1103_serial_fifo_enable(UInt16 trigger_level)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  uartptr->iir_fcr = DS1103_SER_FIFO_ENABLE | DS1103_SER_FIFO_RECRESET
                     | DS1103_SER_FIFO_TRMRESET | ds1103_ser_trigger_level[trigger_level];
  DS1103_SYNC;
  DS1103_EIEIO;

 }




/******************************************************************************
*
* FUNCTION:
*     void ds1103_serial_fifo_disable(void)
*   
*
* SYNTAX:
*       ds1103_serial_fifo_disable(void)
*
* DESCRIPTION: This function disables the fifo:
*
*
* PARAMETERS: no
*
* RETURNS: void
*
* NOTE:
*
******************************************************************************/

 __INLINE void ds1103_serial_fifo_disable(void)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  uartptr->iir_fcr = 0;
  DS1103_SYNC;
  DS1103_EIEIO;

 }


/******************************************************************************
*
* FUNCTION:
*   Int16 ds1103_serial_fifo_state(void)
*
* SYNTAX:
*        ds1103_serial_fifo_state(void)
*
* DESCRIPTION: This function returns the fifo state.
*
* PARAMETERS: no
*
* RETURNS: fifo state   DS1103_SER_FALSE if fifos are disabled
*                       DS1103_SER_TRUE if fifos are enabled  
*
* NOTE:
*
*
******************************************************************************/

 __INLINE Int16 ds1103_serial_fifo_state(void)
{

  Int16 value;

  value = (DS1103_SERIAL_REG_IIR & DS1103_SER_FIFO_STATE) == DS1103_SER_FIFO_STATE ;
  DS1103_SYNC;
  DS1103_EIEIO;

  return(value);

}





 /******************************************************************************
*
* FUNCTION:
*      void ds1103_serial_autoflow_enable(void)
*
* SYNTAX:
*      ds1103_serial_autoflow_enable(void)
*
* DESCRIPTION: This function enables the autoflow mode (RTS/CTS handshaking).
*
*
* PARAMETERS: no
*
* RETURNS: void
*
* NOTE:
*
*
******************************************************************************/

 __INLINE void ds1103_serial_autoflow_enable(void)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  uartptr->mcr = uartptr->mcr | DS1103_SER_AUTOFLOW_ENABLE;
  DS1103_SYNC;
  DS1103_EIEIO;

 }


/******************************************************************************
*
* FUNCTION:
*       void ds1103_serial_autoflow_disable(void)
*
* SYNTAX:
*       ds1103_serial_autoflow_disable(void)
*
* DESCRIPTION: This function disables the autoflow mode (RTS/CTS handshaking).
*
* PARAMETERS: no
*
* RETURNS: void
*
* NOTE:
*
*
******************************************************************************/

 __INLINE void ds1103_serial_autoflow_disable(void)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  uartptr->mcr = uartptr->mcr & DS1103_SER_AUTOFLOW_DISABLE;
  DS1103_SYNC;
  DS1103_EIEIO;

 }








/******************************************************************************
*
* FUNCTION:
*     void ds1103_serial_interrupt_set(UInt16 interrupt_mode)
*
* SYNTAX:
*      ds1103_serial_interrupt_set(UInt16 interrupt_mode)
*
* DESCRIPTION: code for enable the UART interrupts of DS1103 - UART
*              interface
*
* PARAMETERS:  mask    : value written into IER register to enable
*                        UART interrupts
*
* RETURNS: void
*
* NOTE:
*
*
******************************************************************************/

__INLINE void ds1103_serial_interrupt_set(UInt16 interrupt_mode)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  uartptr->ier = interrupt_mode & DS1103_SER_IER_MSK;
  DS1103_SYNC;
  DS1103_EIEIO;

}



/******************************************************************************
*
* FUNCTION:
*       UInt16 ds1103_serial_pollmode_send(Int8* data, UInt16 len)
*
* SYNTAX:
*       ds1103_serial_pollmode_send(Int8* data, UInt16 len)
*
* DESCRIPTION: This function sends len bytes. You have to poll this function 
*              until it returns DS1103_SER_TX_READY.
*
* PARAMETERS: Int8* data: address of first databyte
*             UInt16 len: number of bytes
*
* RETURNS: DS1103_SER_TX_READY, if all bytes are transmitted,
*          or the number of bytes, which aren't transmitted.
*
* NOTE: The address of data must be an address of the Local RAM!
*
*
******************************************************************************/

 __INLINE UInt16 ds1103_serial_pollmode_send(Int8* data, UInt16 len)
 {
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  UInt16 x, y;
  UInt16 full_fifo, rest_fifo;
  static UInt16 state = 0;
  static UInt16 ind = 0;
  UInt32 msr_state;

  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE();

  full_fifo = len >> DS1103_SER_FIFOSHF;
  rest_fifo = len % DS1103_SER_FIFOLEN;

  if( state == DS1103_SER_FALSE)
  {
    for( x=ds1103_ser_rest_data_ind; x < full_fifo; x++)
    {

      if( !(uartptr->lsr & 0x20) )      /* is thr empty? */
      {
        DS1103_SYNC;
        ds1103_ser_rest_data_ind = x;
        DS1103_EIEIO;
        ds1103_set_interrupt_status(msr_state);
        return(len - (x*DS1103_SER_FIFOLEN));
      };
   
      for(y=0; y < DS1103_SER_FIFOLEN; y++)
        uartptr->rbr_thr = data[ind++];

    };
  };

  state = DS1103_SER_TRUE;

  if( !(uartptr->lsr & 0x20) )          /* is thr empty? */
  {
    DS1103_SYNC;
    if(!rest_fifo)
    {
      state = DS1103_SER_FALSE;
      ind = 0;  
    };
    ds1103_ser_rest_data_ind = rest_fifo;
    DS1103_EIEIO;
    ds1103_set_interrupt_status(msr_state);
    return( rest_fifo );
  };


  for( x=0; x < rest_fifo; x++)
    uartptr->rbr_thr = data[ind++];


  state = DS1103_SER_FALSE;
  ds1103_ser_rest_data_ind = 0;
  ind = 0;
  DS1103_EIEIO;
  ds1103_set_interrupt_status(msr_state);
  return(DS1103_SER_TX_READY);


}





/******************************************************************************
*
* FUNCTION:
*   UInt16 ds1103_serial_intmode_send(Int8* data, UInt16 len)
*
* SYNTAX:
*        ds1103_serial_intmode_send(Int8* data, UInt16 len)
*
* DESCRIPTION:  This function sends len bytes. You can use this function 
*               in an UART-Interrupthandler. The function returns
*               DS1103_SER_TX_READY, if the transmission of all bytes is
*               finished.
*
* PARAMETERS: Int8* data: address of first databyte
*             UInt16 len: number of bytes
*
* RETURNS:  DS1103_SER_TX_READY, if all bytes are transmitted,
*           or the number of bytes, which aren't transmitted.
*
* NOTE: The address of data must be an address of the Local RAM!
*
*
******************************************************************************/

 __INLINE UInt16 ds1103_serial_intmode_send(Int8* data, UInt16 len)
 {
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  UInt16 x, y;
  UInt16 full_fifo, rest_fifo;
  static UInt16 ind = 0;
  static UInt16 state = DS1103_SER_FALSE;

  if( state == DS1103_SER_FALSE)
  {
    full_fifo = len >> DS1103_SER_FIFOSHF;
    rest_fifo = len % DS1103_SER_FIFOLEN;
    ds1103_ser_rest_data_ind = len;
  }
  else
  {
    full_fifo = ds1103_ser_rest_data_ind  >> DS1103_SER_FIFOSHF;
    rest_fifo = ds1103_ser_rest_data_ind  % DS1103_SER_FIFOLEN; 
  };

  state = DS1103_SER_TRUE;

  /* enough data to fill fifo ? */
  if( full_fifo >= 1 )
  {
    /* write 16 bytes to fifo */
    for(y=0; y < DS1103_SER_FIFOLEN; y++)
       uartptr->rbr_thr = data[ind++];

    ds1103_ser_rest_data_ind = ds1103_ser_rest_data_ind - DS1103_SER_FIFOLEN;
    if(!ds1103_ser_rest_data_ind)
    {
        state = DS1103_SER_FALSE;
        ind = 0;
    };
    DS1103_EIEIO;
    return(ds1103_ser_rest_data_ind);
  };

  /* write the restbytes to the fifo */
  for( x=0; x < rest_fifo; x++)
    uartptr->rbr_thr = data[ind++];

  DS1103_SYNC;

  state = DS1103_SER_FALSE;
  ds1103_ser_rest_data_ind = 0;
  ind = 0;
  DS1103_EIEIO;
  return(DS1103_SER_TX_READY);


}

 
 


/******************************************************************************
*
* FUNCTION:
*       Int16 ds1103_serial_pollmode_receive(Int8 *data, UInt16* len)
*
* SYNTAX:
*        ds1103_serial_pollmode_receive(Int8 *data, UInt16* len)
*
* DESCRIPTION: This function receives len databytes. You have to poll this
*              function until it returns DS1103_SER_RX_READY.
*
* PARAMETERS: Int8* data: address of first databyte
*             UInt16 len: number of bytes
*
* RETURNS: DS1103_SER_RX_READY : if fifo is empty,
*          DS1103_SER_RX_FAILED :  if fifo contains no data. 
*                                 
*
* NOTE: The address of data must be an address of the Local RAM!
*       This function works only with enabled fifo.
*
*
******************************************************************************/

__INLINE Int16 ds1103_serial_pollmode_receive(Int8 *data, UInt16* len)
{
  UInt16 ind;
  UInt32 msr_state;
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;


  ind = 0;
  *len = 0;

  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE();

  /* check if receive FIFO contains data bytes */
  if( (uartptr->lsr & 0x1) == 0 )
  {
      DS1103_SYNC;
      DS1103_EIEIO;
      ds1103_set_interrupt_status(msr_state);
      return(DS1103_SER_RX_FAILED);
  }
  else
  { /* write databytes */
    do
    {
     DS1103_SYNC;
     data[ind] = (Int8) uartptr->rbr_thr;
     DS1103_SYNC;
     ind = ind + 1;
    }while( (uartptr->lsr & 0x1) != 0);
  };


  *len = ind;

  DS1103_EIEIO;
  ds1103_set_interrupt_status(msr_state);

  return(DS1103_SER_RX_READY);

}



/******************************************************************************
*
* FUNCTION:
*         void ds1103_serial_intmode_receive(Int8 *data, UInt16* len)
*
* SYNTAX:
*         ds1103_serial_intmode_receive(Int8 *data, UInt16* len)
*
* DESCRIPTION: This function reads len bytes from the receiver fifo. You can
*              use this function in an UART-Interrupthandler.
*
*
* PARAMETERS: Int8* data: address of first databyte
*             UInt16 len: number of bytes
*
* RETURNS: void
*
* NOTE: The address of data must be an address of the Local RAM!
*
*
******************************************************************************/

__INLINE void ds1103_serial_intmode_receive(Int8 *data, UInt16* len)
{
  UInt16 ind;
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  ind = 0;
  *len = 0;

  do /* write databytes */
  {
    DS1103_SYNC;
    data[ind] = (Int8) uartptr->rbr_thr;
    DS1103_SYNC;
    ind = ind + 1;
  }while( (uartptr->lsr & 0x1) != 0);

  *len = ind;
  DS1103_EIEIO;

}

/******************************************************************************
*
* FUNCTION:
*         Int16 ds1103_serial_fifo_byte_get(void)
*
* SYNTAX:
*         ds1103_serial_fifo_byte_get(void)
*
* DESCRIPTION:
*
*
* PARAMETERS:
*
* RETURNS:
*
*
******************************************************************************/
__INLINE Int16 ds1103_serial_fifo_byte_get(void)
{
  Int16 byte;
  rtlib_int_status_t int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if (DS1103_SER_BYTE_AVAILABLE)
  {
    byte = ds1103_ser_sw_fifo[ds1103_ser_sw_fifo_ridx];
    DS1103_SER_INC(ds1103_ser_sw_fifo_ridx);
  }
  else
  {
    byte = DS1103_SER_SW_FIFO_EMPTY;
  }

  DS1103_EIEIO;
  RTLIB_INT_RESTORE(int_status);

  return(byte);
}

/******************************************************************************
*
* FUNCTION:
*         Int8 ds1103_serial_msr_get(void)
*
* SYNTAX:
*         ds1103_serial_msr_get(void)
*
* DESCRIPTION:
*
*
* PARAMETERS:
*
* RETURNS:
*
*
******************************************************************************/
__INLINE Int8 ds1103_serial_msr_get(void)
{
  return(ds1103_ser_sw_msr_register);
}

/******************************************************************************
*
* FUNCTION:
*         Int8 ds1103_serial_lsr_get(void)
*
* SYNTAX:
*         ds1103_serial_lsr_get(void)
*
* DESCRIPTION:
*
*
* PARAMETERS:
*
* RETURNS:
*
*
******************************************************************************/
__INLINE Int8 ds1103_serial_lsr_get(void)
{
  return(ds1103_ser_sw_lsr_register);
}

/******************************************************************************
*
* FUNCTION:
*         Int8 ds1103_serial_acknowledge(void)
*
* SYNTAX:
*         ds1103_serial_acknowledge(void)
*
* DESCRIPTION:
*
*
* PARAMETERS:
*
* RETURNS:
*
*
******************************************************************************/
__INLINE Int8 ds1103_serial_acknowledge(void)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;
  Int8 ser_intreg;
  rtlib_int_status_t int_status;


  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  ser_intreg = DS1103_SERIAL_REG_INT_ID;          /* get interrupt identifier */

  switch(ser_intreg)
  {
    case DS1103_SER_INT_RECLINE_STATE:                /* receiver line status */
      /* copy line status register */
      ds1103_ser_sw_lsr_register = (Int8) uartptr->lsr;
      break;

    case DS1103_SER_INT_RECDATA_AVAIL:        /* new bytes in FIFO to be read */
      /* copy UART-FIFO to SW-FIFO */
      do
      {
        DS1103_SYNC;
        ds1103_ser_sw_fifo[ds1103_ser_sw_fifo_widx] = (Int8) uartptr->rbr_thr;
        DS1103_SYNC;
        DS1103_SER_INC(ds1103_ser_sw_fifo_widx);
      }while( (uartptr->lsr & 0x1) != 0);
      DS1103_EIEIO;
      break;

    case DS1103_SER_INT_CHAR_TIMEOUT:                    /* receiver time out */
      /* copy UART-FIFO to SW-FIFO */
      do
      {
        DS1103_SYNC;
        ds1103_ser_sw_fifo[ds1103_ser_sw_fifo_widx] = (Int8) uartptr->rbr_thr;
        DS1103_SYNC;
        DS1103_SER_INC(ds1103_ser_sw_fifo_widx);
      }while( (uartptr->lsr & 0x1) != 0);
      DS1103_EIEIO;
      break;

    case DS1103_SER_INT_THR_EMPTY:         /* UART is ready to send next byte */
      /* no action */
      break;

   case DS1103_SER_INT_MODEM_STATE:                            /* modem staus */
     /* copy modem status register */
     ds1103_ser_sw_msr_register = (Int8) uartptr->msr;
     break;

    default:
      ser_intreg = -1;                               /* no pending interrupts */
      break;
  }

  RTLIB_INT_RESTORE(int_status);

  return(ser_intreg);                          /* return interrupt identifier */

}

#undef __INLINE





