/******************************************************************************
*
* FILE:
*   dsser1103.c
*
* RELATED FILES:
*   dsser1103.h
*
* DESCRIPTION:
*    This module contains functions to access the UART (serial interface)
*    of the DS1103.
*
* HISTORY:
*             1.1     04.09.2000 OJ transceiver depending baurate check
*             1.0                MH initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsser1103.c $ $Revision: 1.6 $ $Date: 2008/12/15 15:23:13GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSER1103.pj $
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <dsmsg.h>
#include <dsstd.h>
#include <dsfifo.h>
#include <dsserdef.h>
#include <dsser1103.h>
#include <ser1103.h>
#include <ds1103_msg.h>
#include <dsav1103.h>


/******************************************************************************
  macros
******************************************************************************/

/*--- UART common defines */

#define   DS1103_DSSER_CHANNEL_NUMBER         0x01
#define   DS1103_DSSER_DEFAULT_BUFFER_SIZE    0x40
#define   DS1103_DSSER_BAUDRATE_MIN              5
#define   DS1103_DSSER_BAUDRATE_MAX_RS232     115200
#define   DS1103_DSSER_BAUDRATE_MAX_RS422    1000000
                                              
#define DS1103_DSSER_IER_MSK    0x0F

#define DS1103_DSSER_CLK_SRC   16.000000e6       /* clock source of UART in Hz */

/*--- UART initialization */

#define   DS1103_DSSER_FIFOLEN        16
#define   DS1103_DSSER_FIFO_ENABLE   0x1                       /* enable FIFO */
#define   DS1103_DSSER_FIFO_DISABLE  0x0
#define   DS1103_DSSER_FIFO_RECRESET 0x2               /* reset receiver FIFO */
#define   DS1103_DSSER_FIFO_TRMRESET 0x4             /*reset transmitter FIFO */
#define   DS1103_DSSER_FIFO_STATE   0xC0

/*--- UART interrupt identification */

#define   DS1103_DSSER_INT_MODEM_STATE         0x0
#define   DS1103_DSSER_INT_NO_PENDING          0x1
#define   DS1103_DSSER_INT_THR_EMPTY           0x2
#define   DS1103_DSSER_INT_RECDATA_AVAIL       0x4
#define   DS1103_DSSER_INT_RECLINE_STATE       0x6
#define   DS1103_DSSER_INT_CHAR_TIMEOUT        0xC

/******************************************************************************
  function declarations
******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE
#endif

__INLINE void ds1103_dsser_interrupt_handler(void);

void ds1103_dsser_dummy_fcn(dsserChannel* serCh, Int32 subint);

#ifndef _INLINE
void  ds1103_dsser_dummy_fcn(dsserChannel* serCh, Int32 subint){ }
#endif

/*******************************************************************************
*
*  FUNCTION
*    Allocates the serial channel object and initializes the transmit and
*    receive FIFO.
*
*  SYNTAX
*    ds1103_dsser_init(UInt32 uart_addr, UInt32 fifo_size)
*
*  DESCRIPTION
*    This function allocates a channel object of the UART interface where
*    multpile allocation is protected. The transmit and receive FIFOs are also
*    allocated and initialized. After successful initialization the
*    corresponding interrupt handler is installed.
*
*  PARAMETERS
*    uart_addr : address of the UART, use: DS1103_UART
*
*    fifo_size : Size of the transmit and receive FIFO in bytes. The FIFO size
*                must be a power of two (2^N).
*
*  RETURNS
*    Int32 : error code
*
*  REMARKS
*
*******************************************************************************/
#ifndef _INLINE

Int32 ds1103_dsser_init(const UInt32 uart_addr, const UInt32 fifo_size)
{
  dsserChannel* serCh = NULL;

  if (uart_addr != DS1103_UART)
  {
    return(DSSER_ILLEGAL_CHANNEL);
  }

  serCh = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];

  if ( serCh == NULL )
  {

    serCh = ds1103_dsserial_init( uart_addr );

    if (serCh == NULL)
    {
      return (DSSER_ALLOC_ERROR);
    }
  }
  else
  {
   dsfifo_free(serCh->txFifo);
   dsfifo_free(serCh->rxFifo);
  }

  serCh->txFifo = dsfifo_init(fifo_size,0);
  serCh->rxFifo = dsfifo_init(fifo_size,1);

  if (serCh->txFifo == NULL || serCh->rxFifo == NULL)
  {
    return(DSSER_ALLOC_ERROR);
  }

  serCh->fifo_size = fifo_size;

  ds1103_set_interrupt_vector( DS1103_INT_SERIAL_UART,
      (DS1103_Int_Handler_Type) &ds1103_dsser_interrupt_handler, SAVE_REGS_ON );

  return (DSSER_NO_ERROR);

}

#endif

/*******************************************************************************
*
*  FUNCTION
*    Configuration of the UART interface.
*
*  SYNTAX
*    Int32 ds1103_dsser_config(dsserChannel* serCh, const UInt32 fifo_mode,
*      const UInt32 baudrate, const UInt32 databits, const UInt32 stopbits,
*      const UInt32 parity, const UInt32 uart_trigger_level,
*      const Int32 user_trigger_level, const UInt32 rs_mode)
*
*  DESCRIPTION
*    This function configures the UART interface for transmission using the
*    parameters described below.
*
*  PARAMETERS
*    serCh              : pointer to the related UART channel object
*
*    fifo_mode          : mode of the receive FIFO
*
*    baudrate           : adjusted baudrate of the UART
*                         RS232 mode: 300...115200 baud
*                         RS422 mode: 300...1000000 baud
*
*    databits           : number of transmitted/received databits
*                         5...8
*
*    stopbits           : number of stopbits
*                         DS1103_DSSER_1_STOPBIT
*                         DS1103_DSSER_2_STOPBIT
*
*    parity             : parity mode
*                         DS1103_DSSER_NO_PARITY
*                         DS1103_DSSER_ODD_PARITY
*                         DS1103_DSSER_EVEN_PARITY
*                         DS1103_DSSER_FORCED_PARITY_ONE
*                         DS1103_DSSER_FORCED_PARITY_ZERO
*
*    uart_trigger_level : size of the UART trigger level
*                         DS1103_DSSER_1_BYTE_TRIGGER_LEVEL
*                         DS1103_DSSER_2_BYTE_TRIGGER_LEVEL
*                         DS1103_DSSER_4_BYTE_TRIGGER_LEVEL
*                         DS1103_DSSER_8_BYTE_TRIGGER_LEVEL
*
*    user_trigger_level : size of the user trigger level, defined for the
*                         receive interrupt
*                         DS1103_DSSER_DEFAULT_TRIGGER_LEVEL or
*                         1...(fifo_size - 1)
*
*    rs_mode            : mode of the UART transceiver
*                         DS1103_DSSER_RS422
*                         DS1103_DSSER_RS232
*
*    autoflow           : autoflow option
*                         DS1103_DSSER_AUTOFLOW_DISABLE
*                         DS1103_DSSER_AUTOFLOW_ENABLE
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/
#ifndef _INLINE

Int32 ds1103_dsser_config(dsserChannel* serCh,
                         const UInt32 fifo_mode,
                         const UInt32 baudrate,
                         const UInt32 databits,
                         const UInt32 stopbits,
                         const UInt32 parity,
                         const UInt32 uart_trigger_level,
                         const  Int32 user_trigger_level,
                         const UInt32 rs_mode,
                         const UInt32 autoflow)
{
  if (serCh == NULL)
  {
    return(DSSER_CHANNEL_INIT_ERROR);
  }

  if( (rs_mode & DS1103_DSSER_RS232) == DS1103_DSSER_RS232 )
  {
    if(baudrate > DS1103_DSSER_BAUDRATE_MAX_RS232)
    {
      return( DSSER_ILLEGAL_BAUDRATE );
    }
    else if( baudrate < DS1103_DSSER_BAUDRATE_MIN )
    {
      return( DSSER_ILLEGAL_BAUDRATE );
    }
  }
  else
  {
    if( baudrate > (UInt32)( DS1103_DSSER_BAUDRATE_MAX_RS422 ) )
    {
      return( DSSER_ILLEGAL_BAUDRATE );
    }
    else if( baudrate < DS1103_DSSER_BAUDRATE_MIN )
    {
      return( DSSER_ILLEGAL_BAUDRATE );
    }
  }

  ds1103_disable_hardware_int (DS1103_INT_SERIAL_UART) ;

  ds1103_dsserial_config(serCh, baudrate, databits, stopbits, parity,
                     uart_trigger_level, autoflow, rs_mode, serCh->interrupt_mode);

  ds1103_enable_hardware_int(DS1103_INT_SERIAL_UART) ;

  return(DSSER_NO_ERROR);
}

#endif

/*******************************************************************************
*
*  FUNCTION
*    UART function to transmit a number of data bytes.
*
*  SYNTAX
*    Int32 ds1103_dsser_transmit (dsserChannel* serCh, UInt32 datalen,
*            UInt8* data, UInt32* count)
*
*  DESCRIPTION
*    This function transmits datalen bytes from the transmit FIFO to the UART
*    and starts the transmission if necessary. The number of data bytes which
*    could really be transmitted is returned by the parameter count.
*    In the case of successful transmission the parameters count and datalen
*    should be equal.
*
*  PARAMETERS
*    serCh   : pointer to the related UART channel object
*
*    datalen : number of data bytes to be transmitted
*
*    data    : address of data buffer
*
*    count   : pointer to the number of real transmitted data bytes
*
*  RETURNS
*    Int32   : DS1103_DSSER_NO_ERROR
*              DS1103_DSSER_FIFO_OVERFLOW
*
*  REMARKS
*
*******************************************************************************/

__INLINE Int32 ds1103_dsser_transmit(dsserChannel* serCh,
                                    const UInt32 datalen, UInt8* data, UInt32* count)
{
  Int32 error = DSSER_NO_ERROR , cnt;
  UInt8 data_temp[DS1103_DSSER_FIFOLEN];
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;
  
  rtlib_int_status_t int_status;        

  *count = dsfifo_write( serCh->txFifo, datalen, data );

  if( *count < datalen )
  {
    error = DSSER_FIFO_OVERFLOW;
  }
  
  RTLIB_INT_SAVE_AND_DISABLE(int_status);    
 
  if( uartptr->lsr & 0x20 )      /* is thr empty? */
  {
    RTLIB_FORCE_IN_ORDER();
    
    cnt = dsfifo_read( serCh->txFifo, DS1103_DSSER_FIFOLEN, data_temp );
    
    ds1103_dsserial_transmit( serCh, cnt, data_temp );
  }
  
  RTLIB_INT_RESTORE(int_status);  

  return (error);
}

/*******************************************************************************
*
*  FUNCTION
*    Resets the SW-FIFOs and the HW-FIFOs of the UART.
*
*  SYNTAX
*    Int32 ds1103_dsser_fifo_reset(dsserChannel* serCh)
*
*  DESCRIPTION
*    This function clears both the transmit and receive SW-FIFOs and the
*    FIFOs of the UART interface.
*
*  PARAMETERS
*    serCh : pointer to the related UART channel object
*
*  RETURNS
*    Int32 : DS1103_DSSER_NO_ERROR
*
*  REMARKS
*
*******************************************************************************/

__INLINE Int32 ds1103_dsser_fifo_reset(dsserChannel* serCh)
{
  Int32 error = DSSER_NO_ERROR;

  rtlib_int_status_t int_status;

  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  if (serCh != NULL)
  {
    RTLIB_INT_SAVE_AND_DISABLE(int_status);

    uartptr->ier = 0;

    RTLIB_SYNC();
    RTLIB_FORCE_IN_ORDER();

    /* clear SW fifos */
    dsfifo_clear(serCh->rxFifo);
    dsfifo_clear(serCh->txFifo);

    /* clear UART fifos */
    uartptr->iir_fcr = DS1103_DSSER_FIFO_ENABLE
                      | DS1103_DSSER_FIFO_RECRESET
                      | DS1103_DSSER_FIFO_TRMRESET
                      | (serCh->uart_trigger_level << 6);

    RTLIB_SYNC();
    RTLIB_FORCE_IN_ORDER();
    RTLIB_INT_RESTORE(int_status);
  }
  return (error);
}


/*******************************************************************************
*
*  FUNCTION
*    UART interrupt handler.
*
*  SYNTAX
*    void ds1103_dsser_interrupt_handler(UInt32 offs)
*
*  DESCRIPTION
*    This function serves the hardware interrupt of the UART interface and
*    calls the user defined sub-interrupt handler if the related sub-interrupt
*    is enabled.
*
*  PARAMETERS
*    offs : board specific offset, needed to get the corresponding channel
*           object of the board
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

__INLINE void ds1103_dsser_interrupt_handler( void )
{
  Int8 ser_intreg;
  rtlib_int_status_t int_status;
  volatile ds1103_uart_reg* uartptr;
  dsserChannel* serCh;
  dsfifo_t* rxFifo;
  Int32  subint;
  Int32  count;
  UInt8  data[DS1103_DSSER_FIFOLEN];
  UInt32 has_subint;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  serCh   = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];
  uartptr = (ds1103_uart_reg*)DS1103_UART;
  rxFifo  = serCh->rxFifo;
  subint  = -1;

  serCh->subint = 0;

  has_subint = 0;

  do
  {
  serCh->isr = uartptr->iir_fcr & 0xFF;

  ser_intreg = serCh->isr & 0xF; /* get interrupt identifier */

  RTLIB_SYNC();
  RTLIB_FORCE_IN_ORDER();

  switch(ser_intreg)
  {
    case DS1103_DSSER_INT_RECLINE_STATE:                /* receiver line status */
        RTLIB_FORCE_IN_ORDER();
        serCh->lsr = uartptr->lsr & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        serCh->subint = DSSER_RECEIVER_LINE_SUBINT_MASK;
        subint = DS1103_DSSER_RECEIVER_LINE_SUBINT;
    break;

    case DS1103_DSSER_INT_RECDATA_AVAIL:        /* new bytes in FIFO to be read */
    case DS1103_DSSER_INT_CHAR_TIMEOUT :        /* receiver time out */

       count = ds1103_dsserial_receive ( serCh, DS1103_DSSER_FIFOLEN, data );

       count = dsfifo_write  ( rxFifo, count, data);

       count = dsfifo_in_count_get ( rxFifo );

       if ( (count >= serCh->user_trigger_level) && (serCh->user_trigger_level > 0) )
       {
         serCh->subint = DSSER_TRIGGER_LEVEL_SUBINT_MASK;
                subint = DS1103_DSSER_TRIGGER_LEVEL_SUBINT;
       }

    break;

    case DS1103_DSSER_INT_THR_EMPTY:         /* UART is ready to send next byte */

       count = dsfifo_read( serCh->txFifo, DS1103_DSSER_FIFOLEN, data);

       if ( count == 0 )
       {
         serCh->subint = DSSER_TX_FIFO_EMPTY_SUBINT_MASK;
                subint = DS1103_DSSER_TX_FIFO_EMPTY_SUBINT;
       }
       else
       {
         count = ds1103_dsserial_transmit ( serCh, count, data );
         subint = DS1103_DSSER_NO_SUBINT;
       }
    break;

    case DS1103_DSSER_INT_MODEM_STATE:                            /* modem staus */
       RTLIB_FORCE_IN_ORDER();
       serCh->msr = uartptr->msr & 0xFF;
       RTLIB_SYNC();
       RTLIB_FORCE_IN_ORDER();
       serCh->subint = DSSER_MODEM_STATE_SUBINT_MASK;
       subint = DS1103_DSSER_MODEM_STATE_SUBINT;
    break;

    default:
      ser_intreg = 0x1;                               /* no pending interrupts */
    break;
  }

   if(serCh->subint & serCh->subint_mask)
   {
    (*(serCh->subint_handler))( serCh, subint );

    serCh->subint = 0;

    has_subint = 1;
   }

  }while(ser_intreg != DS1103_DSSER_INT_NO_PENDING);

  if (has_subint)
  {
   (*(serCh->subint_handler))( serCh, DS1103_DSSER_NO_SUBINT );
  }

  RTLIB_INT_RESTORE(int_status);

  return;
}


/*******************************************************************************
*
*  FUNCTION
*    Allocates the serial channel object.
*
*  SYNTAX
*    dsserChannel* ds1103_dsserial_init(UInt32 uart_addr)
*
*  DESCRIPTION
*    This function allocates a channel object of the UART interface whereas
*    multpile allocation is protected.
*
*  PARAMETERS
*    uart_addr     : address of the UART, use: DS1103_UART
*
*  RETURNS
*    dsserChannel* : pointer to the allocated channel object
*
*  REMARKS
*
*******************************************************************************/
#ifndef _INLINE

dsserChannel* ds1103_dsserial_init(UInt32 uart_addr)
{
  dsserChannel* serCh;

  if( dsser_lookup_table[DSSER_MAX_BOARDS-1][0] == NULL )
  {
    serCh = (dsserChannel*) malloc(sizeof(dsserChannel));

    if (serCh != NULL)
    {
      serCh->module         = DSSER_ONBOARD;
      serCh->subint         = 0;
      serCh->subint_mask    = 0;
      serCh->txFifo         = NULL;
      serCh->rxFifo         = NULL;
      serCh->channel        = 0;
      serCh->frequency      = DS1103_SER_CLK_SRC;
      serCh->subint_handler = (dsser_subint_handler_t)ds1103_dsser_dummy_fcn;
      dsser_lookup_table[DSSER_MAX_BOARDS-1][0] = serCh;
    }
  }
  else
  {
    serCh = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];
  }

  return (serCh);
}

#endif
/******************************************************************************
*
*  FUNCTION
*    Frees the UART environment.
*
*  SYNTAX
*    void ds1103_dsserial_free(dsserChannel* serCh)
*
* DESCRIPTION
*   Frees the memory allocated with the function ds1103_dsserial_init(...)
*   or ds1103_dsser_init(...).
*   The hardewareinterrupt for the UART is disabled.
*
* PARAMETERS
*   serCh        Pointer to the serial object to be freed.
*
* RETURNS:
*   void
*
*  REMARKS
*
******************************************************************************/

#ifndef _INLINE

void ds1103_dsserial_free(dsserChannel* serCh)
{
  rtlib_int_status_t int_status;

  RTLIB_INT_SAVE_AND_DISABLE( int_status );

  if (dsser_lookup_table[DSSER_MAX_BOARDS-1][0] != NULL)
  {
    ds1103_disable_hardware_int( DS1103_INT_SERIAL_UART );

    dsfifo_free( serCh->txFifo );
    dsfifo_free( serCh->rxFifo );
    free( serCh );

    dsser_lookup_table[DSSER_MAX_BOARDS-1][0] = NULL;
  }

  RTLIB_INT_RESTORE( int_status );
}

#endif


/*******************************************************************************
*
*  FUNCTION
*    Configuration of the UART interface.
*
*  SYNTAX
*    Int32 ds1103_dsserial_config(dsserChannel* serCh, const UInt32 baudrate,
*      const UInt32 databits, const UInt32 stopbits, const UInt32 parity,
*      const UInt32 uart_trigger_level, const UInt32 interrupt_mode,
*      const UInt32 rs_mode)
*
*  DESCRIPTION
*    This function configures the UART interface for transmission using the
*    parameters described below.
*
*  PARAMETERS
*    serCh              : pointer to the related UART channel object
*
*    baudrate           : adjusted baudrate of the UART
*                         RS232 mode: 300...115200 baud
*                         RS422 mode: 300...1000000 baud
*
*    databits           : number of transmitted/received databits
*                         5...8
*
*    stopbits           : number of stopbits
*                         DS1103_DSSER_1_STOPBIT
*                         DS1103_DSSER_2_STOPBIT
*
*    parity             : parity mode
*                         DS1103_DSSER_NO_PARITY
*                         DS1103_DSSER_ODD_PARITY
*                         DS1103_DSSER_EVEN_PARITY
*                         DS1103_DSSER_FORCED_PARITY_ONE
*                         DS1103_DSSER_FORCED_PARITY_ZERO
*
*    uart_trigger_level : size of the UART trigger level
*                         DS1103_DSSER_1_BYTE_TRIGGER_LEVEL
*                         DS1103_DSSER_2_BYTE_TRIGGER_LEVEL
*                         DS1103_DSSER_4_BYTE_TRIGGER_LEVEL
*                         DS1103_DSSER_8_BYTE_TRIGGER_LEVEL
*
*    interrupt_mode     : specifies the UART interrupts, which should be enabled
*                         DS1103_DSSER_NO_INT
*                         DS1103_DSSER_RECDATA_INT
*                         DS1103_DSSER_THR_EMPTY_INT
*                         DS1103_DSSER_RECLINE_ST_INT
*                         DS1103_DSSER_MODEM_ST_INT
*
*    rs_mode            : mode of the UART transceiver
*                         DS1103_DSSER_RS422
*                         DS1103_DSSER_RS232
*
*  RETURNS
*    Int32 : DSSER_SER_NO_ERROR
*
*  REMARKS
*
*******************************************************************************/
#ifndef _INLINE

Int32 ds1103_dsserial_config(dsserChannel* serCh,
                               const UInt32 baudrate,
                               const UInt32 databits,
                               const UInt32 stopbits,
                               const UInt32 parity,
                               const UInt32 uart_trigger_level,
                               const UInt32 autoflow,
                               const UInt32 rs_mode,
                               const UInt32 interrupt_mode)

{
  volatile ds1103_uart_reg* uartptr = (volatile ds1103_uart_reg*)DS1103_UART;
  volatile UInt16* resetptr = (volatile UInt16*)DS1103_RESET;
  volatile UInt16* setupptr = (volatile UInt16*)DS1103_SETUP;
  UInt32   databits_temp = (databits - 5) & 0x3;
  Int32    error = DSSER_NO_ERROR;
  UInt16   pen = 0, stb = 0, eps = 0, stick = 0;
  UInt8     dummy ;
  Float64 divisor;
  rtlib_int_status_t int_status;
  Float32 f_target, f_actual;
  
  UInt8 vco_div = 0;
  UInt8 ref_div = 0;


  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if(!ds1103_advanced_io)
	  {
		divisor = DS1103_SER_CLK_SRC / (16 * (Float64) baudrate) ;
		divisor = divisor + 0.5;
        serCh->baudrate           = baudrate;
 
      }
  else
	  {
		/* calculate divisor with default clock source of the UART */
        divisor = DS1103_SER_CLK_SRC / (16 * (Float64) baudrate);
        f_target = ((UInt32) divisor << 4) * (Float64) baudrate;
        error = ds1103_av9110_clock_calc_ser( f_target, &f_actual, &vco_div, &ref_div );
        if( error == AV9110_PLLFREQ_ERROR )
            msg_error_printf(MSG_SM_RTLIB, DSSER_ILLEGAL_BAUDRATE, DSSER_ILLEGAL_BAUDRATE_TXT,baudrate,0);
        else
            ds1103_av9110_clock_write_ser( vco_div, ref_div );
        
        serCh->frequency = (UInt32) f_actual;
        serCh->baudrate  = ( serCh->frequency / ((UInt32)divisor << 4) );
   
     }



  /* store config data to channel object */
  serCh->databits           = databits;
  serCh->stopbits           = stopbits;
  serCh->parity             = parity;
  serCh->rs_mode            = rs_mode;
  serCh->uart_trigger_level = uart_trigger_level;

  *resetptr = *resetptr & 0xFFFD;

  
  /* set patity mask */
  if (parity != DS1103_DSSER_NO_PARITY)
  {
    pen = 0x08;                                  /* enable parity */
    switch(parity)
    {
     case DS1103_DSSER_EVEN_PARITY:
      eps   = 0x10;                              /* even parity */
      stick = 0x00;
     break;
     case DS1103_DSSER_FORCED_PARITY_ZERO:
      eps   = 0x10;
      stick = 0x20;
     break;
     case DS1103_DSSER_FORCED_PARITY_ONE:
      eps   = 0x00;
      stick = 0x20;
     break;
    }
  }

  /* set databits and stopbits masks */
  if(stopbits == DS1103_DSSER_2_STOPBIT)      /* 1 */
    stb = 0x04;                               /* 1.5 or 2 stop bits */

  uartptr->lcr = databits_temp | stb | pen | eps | stick
                | 0x0      /* break = 0 */
                | 0x80;    /* set divisor latch access bit to progr. dlm + dll*/

  RTLIB_FORCE_IN_ORDER();

  uartptr->ier = ((UInt32)divisor >> 8) & 0xff;
  uartptr->rbr_thr = (UInt32)divisor & 0xff;

  RTLIB_FORCE_IN_ORDER();

  uartptr->lcr = uartptr->lcr & 0x0000007f; /* reset divisor latch access bit */
  uartptr->ier = 0;                                     /* disable interrupts */
  uartptr->mcr = 0;

  if(rs_mode == DS1103_DSSER_RS422)
    *setupptr = (*setupptr & 0xFFEF) | 0x10;
  else
    *setupptr = (*setupptr & 0xFFEF) ;

  uartptr->iir_fcr = 0;

  RTLIB_FORCE_IN_ORDER();

  /* clear transmit */
  if(ds1103_debug & DS1103_DEBUG_POLL)
    DS1103_MSG_INFO_SET(DS1103_UART_TX_EMPTY_POLL_START);
  while((uartptr->lsr & 0x20) != 0x20);                /* wait until tx_empty */
  if(ds1103_debug & DS1103_DEBUG_POLL)
    DS1103_MSG_INFO_SET(DS1103_UART_TX_EMPTY_POLL_END);

  RTLIB_FORCE_IN_ORDER();

  /* clear receive */
  if(ds1103_debug & DS1103_DEBUG_POLL)
    DS1103_MSG_INFO_SET(DS1103_UART_REC_READ_POLL_START);
  while(ds1103_dsserial_receive( serCh, 1, &dummy));
  if(ds1103_debug & DS1103_DEBUG_POLL)
    DS1103_MSG_INFO_SET(DS1103_UART_REC_READ_POLL_END);

  /* FIFO settings */
  uartptr->iir_fcr = DS1103_DSSER_FIFO_ENABLE
                    | DS1103_DSSER_FIFO_RECRESET
                    | DS1103_DSSER_FIFO_TRMRESET
                    | (uart_trigger_level << 6);

  /* set autoflow option */
  if((autoflow & DS1103_SER_AUTOFLOW_ENABLE) == DS1103_DSSER_AUTOFLOW_ENABLE)
  {
    uartptr->mcr = uartptr->mcr | 0x22;
    RTLIB_FORCE_IN_ORDER();
  }
  else
  {
    uartptr->mcr = uartptr->mcr & 0xFFDD;
    RTLIB_FORCE_IN_ORDER();
  }

  /* set interrupt mode */
  uartptr->ier = interrupt_mode & DS1103_DSSER_IER_MSK;

  RTLIB_FORCE_IN_ORDER();

  RTLIB_INT_RESTORE(int_status);

  return(error);
}

#endif

/*******************************************************************************
*
*  FUNCTION
*    Function to read data received by the UART.
*
*  SYNTAX
*    Int32 ds1103_dsserial_receive(const dsserChannel* serCh,UInt32 len,
*            UInt8* data)
*
*  DESCRIPTION
*    If data is available this function tries to read len bytes from the receive
*    FIFO of the UART interface. The real number of read data bytes is returned.
*
*  PARAMETERS
*    serCh : pointer to the related UART channel object
*
*    len   : number of data bytes, which should be read
*
*    data  : address of data buffer
*
*  RETURNS
*    Int32 : number of read data bytes
*
*  REMARKS
*
*******************************************************************************/

__INLINE Int32 ds1103_dsserial_receive(const dsserChannel* serCh,
                                  UInt32 len, UInt8* data)
{
  UInt16 count = 0;
  rtlib_int_status_t int_status;
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check if receive FIFO contains data bytes */
  if((uartptr->lsr & 0x1) == 0)
  {
    RTLIB_SYNC();
    RTLIB_FORCE_IN_ORDER();
  }
  else
  { /* read from device */
    do
    {
     RTLIB_SYNC();
     (*data++) = uartptr->rbr_thr & 0xFF;
     RTLIB_SYNC();
     RTLIB_FORCE_IN_ORDER();
     count++;
    }while( ((uartptr->lsr & 0x1) != 0)  && (count < len));
  }

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

  return(count);
}

/*******************************************************************************
*
*  FUNCTION
*    UART function to transmit a number of data bytes.
*
*  SYNTAX
*    Int32 ds1103_dsserial_transmit(const dsserChannel* serCh, UInt32 len,
*            UInt8* data)
*
*  DESCRIPTION
*    This function transmits len bytes from the UART interface.
*    The number of data bytes which are transmitted is returned.
*    In the case of successful transmission the parameters count and len
*    should be equal. However, the maximum number of data which could be
*    transmitted within one function call is limited to 16 bytes.
*
*  PARAMETERS
*    serCh : pointer to the related UART channel object
*
*    len   : number of data bytes to be transmitted
*
*    data  : address of data buffer
*
*  RETURNS
*    Int32 : number of transmitted data bytes
*
*  REMARKS
*
*******************************************************************************/

__INLINE Int32 ds1103_dsserial_transmit(const dsserChannel* serCh, UInt32 len,
                                   UInt8* data)
{
  UInt16 count = 0, length = DS1103_DSSER_FIFOLEN;
  rtlib_int_status_t int_status;
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if (len < DS1103_DSSER_FIFOLEN)
  {
   length = len;
  }

  RTLIB_SYNC();

  if (uartptr->lsr & 0x20)      /* is thr empty? */
  {
    for(count = 0; count < length ; count++)
    {
      RTLIB_SYNC();
      RTLIB_FORCE_IN_ORDER();
      uartptr->rbr_thr = *(data++) & 0xFF;
    }
  }

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

  return(count);
}

/*******************************************************************************
*
*  FUNCTION
*    Writes a value to a certain UART register.
*
*  SYNTAX
*    void ds1103_dsserial_register_write(const dsserChannel* serCh,
*           const UInt8 register_type, const UInt8 register_value)
*
*  DESCRIPTION
*    This function writes a data byte specified by the parameter register_value
*    to the register, which is selected by register_type. It is recommended to
*    use the constants listed below.
*
*  PARAMETERS
*    serCh          : pointer to the related UART channel object
*
*    register_type  : specifies the register, which should be written to
*                     DS1103_DSSER_RBR_THR, DS1103_DSSER_IER,
*                     DS1103_DSSER_IIR_FCR, DS1103_DSSER_LCR,
*                     DS1103_DSSER_MCR, DS1103_DSSER_LSR,
*                     DS1103_DSSER_MSR, DS1103_DSSER_SCR
*
*    register_value : value, which should be written
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

__INLINE void ds1103_dsserial_register_write(const dsserChannel* serCh,
                                             const UInt8 register_type,
                                             const UInt8 register_value)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  switch(register_type)
  {
      case DS1103_DSSER_RBR_THR:
        uartptr->rbr_thr = register_value & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_IER:
        uartptr->ier     = register_value & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_IIR_FCR:
        uartptr->iir_fcr = register_value & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_LCR:
        uartptr->lcr     = register_value & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_MCR:
        uartptr->mcr     = register_value & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_LSR:
        uartptr->lsr     = register_value & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_MSR:
        uartptr->msr     = register_value & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_SCR:
        uartptr->scr     = register_value & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      default:
        break;

   }
}

/*******************************************************************************
*
*  FUNCTION
*    Reads a certain UART register.
*
*  SYNTAX
*    UInt8 ds1103_dsserial_register_read(const dsserChannel* serCh,
*            const UInt8 register_type)
*
*  DESCRIPTION
*    This function reads the UART register specified by the parameter
*    register_type. It is recommended to use the constants listed below.
*
*  PARAMETERS
*    serCh         : pointer to the related UART channel object
*
*    register_type : specifies the register, which should be read
*                    DS1103_DSSER_RBR_THR, DS1103_DSSER_IER,
*                    DS1103_DSSER_IIR_FCR, DS1103_DSSER_LCR,
*                    DS1103_DSSER_MCR, DS1103_DSSER_LSR,
*                    DS1103_DSSER_MSR, DS1103_DSSER_SCR
*
*
*  RETURNS
*    UInt8         : register value
*
*  REMARKS
*
*******************************************************************************/

__INLINE UInt8 ds1103_dsserial_register_read(const dsserChannel* serCh,
                                        const UInt8 register_type)
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  UInt8 register_value;

  switch(register_type)
  {
      case DS1103_DSSER_RBR_THR:
        register_value = uartptr->rbr_thr & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_IER:
        register_value = uartptr->ier & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_IIR_FCR:
        register_value = uartptr->iir_fcr & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_LCR:
        register_value = uartptr->lcr & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_MCR:
        register_value = uartptr->mcr & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_LSR:
        register_value = uartptr->lsr & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_MSR:
        register_value = uartptr->msr & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      case DS1103_DSSER_SCR:
        register_value = uartptr->scr & 0xFF;
        RTLIB_SYNC();
        RTLIB_FORCE_IN_ORDER();
        break;
      default:
        register_value = 0;
        break;
   }

 return (register_value);

}

/******************************************************************************
*  not exported functions !
*
******************************************************************************/

__INLINE Int32 ds1103_dsserial_io_ier_set(const dsserChannel* serCh, UInt8 value )
{
  volatile ds1103_uart_reg* uartptr = (ds1103_uart_reg*)DS1103_UART;

  uartptr->ier = value;

  RTLIB_SYNC();
  RTLIB_FORCE_IN_ORDER();

  return(DSSER_NO_ERROR);
}

#undef __INLINE
