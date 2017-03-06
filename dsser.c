/**
* @file dsser.c
*
* DESCRIPTION:
* This module contains functions to access the UARTs (serial interfaces) of the:
*           DS1005,
*           DS1006,
*           DS1401,
*           DS1103,
*           DS1104,
*           DS2210,
*           DS2211,
*           DS4201S,
*           DS2202
*
* HISTORY:
*           1.4	  16.01.2003    R.GB added DS1006 support
*           1.3.1 06.09.2000    O.J  bugfix in dsser_status_read()
*           1.3   28.08.2000    O.J  added DS1104 support
*           1.2.1 08.06.2000    O.J  dsser_status_read(), bugfix
*           1.2   17.05.2000    O.J  added dsser_set(..) (frequency)
*           1.1   18.04.2000    O.J  added DS1401, DS1103, DS4201S support
*           1.0   13.12.1999    O.J  initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsser.c $ $Revision: 1.9 $ $Date: 2008/12/15 16:19:10GMT+01:00 $
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <dsmsg.h>
#include <dsstd.h>
#include <dsfifo.h>
#include <dsser.h>
#include <dsvcm.h>

#if defined (_DS1401)
#include <ds1401.h>
#include <ser_tp1.h>
#include <ser1401_tp1.h>
#endif

#if defined (_DS1005)
#include <int1005.h>
#include <dsphs.h>
#include <ds2202.h>
#include <ds2210.h>
#include <ds2211.h>
#include <ser1005.h>
#include <ser4201.h>
#endif

#if defined (_DS1006)
#include <int1006.h>
#include <dsphs.h>
#include <ds2202.h>
#include <ds2210.h>
#include <ds2211.h>
#include <ser1006.h>
#include <ser4201.h>
#endif

#if defined (_DS1003)
#include <dsphs.h>
#include <ds2210.h>
#include <ser4201.h>
#endif

#if defined (_DS1103)
#define PHS_BT_NO_BOARD  0
#include <def1103.h>
#include <ds1103.h>
#include <ser1103.h>
#include <dsser1103.h>
#endif

#if defined (_DS1104)
#include <ds1104.h>
#include <ser1104.h>
#endif

#define DSSER_MAX_SUPPORTED_BOARD_TYPES 9
#define DSSER_MAX_SUPPORTED_PROPERTYS   8

#define DSSER_SER_TP1                   0
#define DSSER_DS1005                    1
#define DSSER_DS1103                    2
#define DSSER_DS2210                    3
#define DSSER_DS4201S                   4
#define DSSER_DS1104                    5
#define DSSER_DS1006                    6
#define DSSER_DS2211                    7
#define DSSER_DS2202                    8

#define DSSER_SUPPORTED_CHANNELS        0
#define DSSER_SUPPORTED_AUTOFLOW        1
#define DSSER_SUPPORTED_RS232           2
#define DSSER_SUPPORTED_RS422           3
#define DSSER_SUPPORTED_RS485           4
#define DSSER_SUPPORTED_KLINE           5
#define DSSER_SUPPORTED_PARITY          6
#define DSSER_SUPPORTED_FREQUENCY       7

#ifndef _INLINE

Int32 dsser_board_descr[DSSER_MAX_SUPPORTED_PROPERTYS]
                       [DSSER_MAX_SUPPORTED_BOARD_TYPES] = {
/*                    SER_TP1  DS1005 DS1103 DS2210 DS4201S  DS1104   DS1006  DS2211  DS2202   */
/*                    |        |      |      |      |        |        |       |       |        */
/* Channels:      */ {2,       1,     1,     1,     4,       1,       1,      1,      1       },
/* Autoflow:      */ {0,       1,     1,     0,     0,       1,       1,      0,      0       },
/* RS232:         */ {1,       1,     1,     1,     1,       1,       1,      1,      1       },
/* RS422:         */ {0,       0,     1,     1,     1,       1,       0,      1,      1       },
/* RS485:         */ {0,       0,     0,     0,     1,       1,       0,      0,      0       },
/* KLINE:         */ {1,       0,     0,     0,     0,       0,       0,      0,      0       },
/* Forced parity: */ {1,       1,     1,     1,     0,       1,       1,      1,      1       },
/* UART frequency:*/ {3686400, 0,     0,     0,     1843200, 16000000,0,      0,      0       } /* frequency in Hz */
};

dsserChannel* dsser_lookup_table[DSSER_MAX_BOARDS][DSSER_MAX_DEVICES] =
       {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
        
int dsser_is_used[DSSER_MAX_BOARDS][DSSER_MAX_DEVICES] =
       {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};

#else

extern Int32 dsser_board_descr[DSSER_MAX_SUPPORTED_PROPERTYS][DSSER_MAX_SUPPORTED_BOARD_TYPES];
extern int dsser_is_used[DSSER_MAX_BOARDS][DSSER_MAX_DEVICES];
#endif

#define DSSER_TRY(func) if( (error=(func)) > 0 ) {goto Error;}
#define DSSER_MSG_ERROR_PRINTF(error)   {strcat(function, error##_TXT); msg_error_printf (MSG_SM_DSSER, error, function, offs, ch );}

/*--- subinterrupt interrupt enable and disable */
#define DSSER_NO_INT         0x0
#define DSSER_RXRDY_INT      0x1           /* received data interrupt */
#define DSSER_THR_EMPTY_INT  0x2       /*THR register empty interrupt */
#define DSSER_LINE_STATE_INT 0x4    /* receiver line status interrupt */
#define DSSER_MODEM_ST_INT   0x8            /* modem status interrupt */

/******************************************************************************
  function declarations
******************************************************************************/

#if defined (_DS1003)
#  if _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#elif defined (_DS1005)
#  if _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#elif defined (_DS1006)
#  if _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#elif defined (_DS1103)
#  if _INLINE
#    define __INLINE static
#  else
#    define __INLINE extern
#  endif
#elif defined (_DS1104)
#  if _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#elif defined (_DS1401)
#  if _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#else
#  error dsser.c: no processor board defined (e.g. _DS1003)
#endif

void dsser_message_wrapper(UInt32 base, UInt32 ch, Int32 error, char* function);

__INLINE Int32 dsser_io_status_read ( dsserChannel* serCh, UInt8 register_type, UInt32* value );
__INLINE Int32 dsser_io_ier_set(dsserChannel* serCh, UInt8 value );
__INLINE Int32 dsser_fifo_realloc( dsserChannel* serCh, UInt32 fifo_size );

#if defined(_DS1401)
extern void dsser_tp1_dummy_fcn ( dsserChannel* serCh, Int32 subint );
#endif
/******************************************************************************
  version information
******************************************************************************/

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 || defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401

DS_VERSION_SYMBOL(_dsser_ver_mar, DSSER_VER_MAR);
DS_VERSION_SYMBOL(_dsser_ver_mir, DSSER_VER_MIR);
DS_VERSION_SYMBOL(_dsser_ver_mai, DSSER_VER_MAI);
DS_VERSION_SYMBOL(_dsser_ver_spb, DSSER_VER_SPB);
DS_VERSION_SYMBOL(_dsser_ver_spn, DSSER_VER_SPN);
DS_VERSION_SYMBOL(_dsser_ver_plv, DSSER_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/*******************************************************************************
*
*  FUNCTION
*
*  SYNTAX
*    dsser_vcm_register(void)
*
*  DESCRIPTION
*    Registers the serial communication module in VCM.
*
*  PARAMETERS
*    void
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

#if defined VCM_AVAILABLE
#ifndef _INLINE
void dsser_vcm_register(void)
{
  vcm_module_descriptor_type *dsser_version_ptr;

  dsser_version_ptr = vcm_module_register(VCM_MID_DSSER,
                                          vcm_module_find(VCM_MID_RTLIB, NULL),
                                          VCM_TXT_DSSER,
                                          DSSER_VER_MAR,
                                          DSSER_VER_MIR,
                                          DSSER_VER_MAI,
                                          DSSER_VER_SPB,
                                          DSSER_VER_SPN,
                                          DSSER_VER_PLV,
                                          0, 0);

  if (dsser_version_ptr == 0)
  {
    msg_error_printf( MSG_SM_DSSER, MSG_MEM_ALLOC_ERROR, MSG_MEM_ALLOC_ERROR_TXT );
    exit(1);
  }

  vcm_module_status_set(dsser_version_ptr,VCM_STATUS_INITIALIZED);
}
#endif
#endif

#ifndef _INLINE
/**
* To initialize the serial interface and install the interrupt handler.
*
* PARAMETERS
*
*   base           base address of the serial interface.
*
*                  Board   |  Permitted Values
*                  --------+-------------------
*                  DS1005  |  DSSER_ONBOARD
*                  DS1401  |  DS1401_IMBUS_MODULE_2
*                  DS2202  |  DS2202_1_BASE, DS2202_2_BASE, ...
*                  DS2210  |  DS2210_1_BASE, DS2210_2_BASE, ...
*                  DS2211  |  DS2211_1_BASE, DS2211_2_BASE, ...
*                  DS4201S |  DS4201S_1_BASE, DS4201S_2_BASE, ...
*                  DS1103  |  DSSER_ONBOARD
*                  DS1104  |  DSSER_ONBOARD
*                  DS1006  |  DSSER_ONBOARD
*
*
*   channel        Number of the channel. The permitted values depend on
*                  the board used:
*
*                  Board   |  Permitted Values
*                  --------+-------------------
*                  DS1005  |  0
*                  DS1401  |  0 or 1
*                  DS2202  |  0
*                  DS2210  |  0
*                  DS2211  |  0
*                  DS4201S |  0 to 3
*                  DS1103  |  0
*                  DS1104  |  0
*                  DS1005  |  0
*
*
*   fifo_size      Size of the transmit and receive buffer in bytes. The size
*                  must be a power of two (2^N) and at least 64 bytes.
* RETURNS:
*                  Pointer to the serial channel structure.
*
*/
dsserChannel* dsser_init(UInt32 base, UInt32 channel, UInt32 fifo_size )
{
  UInt32 error = DSSER_NO_ERROR;

  UInt32 board;

  dsserChannel* serCh = NULL;

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))

  Int32 board_bt = 0;

  #if (defined (_DS1005) || defined (_DS1006)) 
  if(base == DSSER_ONBOARD)
  {
    board_bt = PHS_BT_NO_BOARD;
  }
  else
  #endif
  {
    board_bt = phs_board_type_get(base);

    if (board_bt == PHS_BT_NO_BOARD || board_bt == PHS_BT_INVALID_BASE || board_bt == PHS_BT_UNKNOWN )
    {
      error = DSSER_NOT_FOUND; goto Error;
    }
  }

  switch(board_bt)
  {
    case PHS_BT_DS2202:
      board = DSSER_DS2202;
    break;
    case PHS_BT_DS2210:
      board = DSSER_DS2210;
    break;
    case PHS_BT_DS2211:
      board = DSSER_DS2211;
    break;
    case PHS_BT_DS4201S:
      board = DSSER_DS4201S;
    break;
    #if defined (_DS1005)
    case PHS_BT_NO_BOARD:
      board = DSSER_DS1005;
    break;
    #endif
	#if defined (_DS1006)
    case PHS_BT_NO_BOARD:
      board = DSSER_DS1006;
    break;
    #endif
    default:
      error = DSSER_NOT_FOUND;
      goto Error;
  }
#endif
#if defined(_DS1401)
  board = DSSER_SER_TP1;
#endif
#if defined(_DS1103)
  board = DSSER_DS1103;
#endif
#if defined(_DS1104)
  board = DSSER_DS1104;
#endif

  if (channel >= dsser_board_descr[DSSER_SUPPORTED_CHANNELS][board])
  {
    error = DSSER_ILLEGAL_CHANNEL; goto Error;
  }

  if (fifo_size < 64)
  {
    error = DSSER_ILLEGAL_FIFO_SIZE; goto Error;
  }

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))

  switch(board_bt)
  {
    case PHS_BT_DS2202:
      serCh = dsser_lookup_table[DSSER_PARA_OFFS][channel];
      
      if (!serCh)
      {
        DSSER_TRY( ds2202_ser_init( base, fifo_size, &serCh ) );
      }
      else
      { 
        if (dsser_fifo_realloc(serCh,fifo_size))
        {
          return NULL;
        }
      }
      dsser_is_used[DSSER_PARA_OFFS][channel]=1;        
    break;
    case PHS_BT_DS2210:
      serCh = dsser_lookup_table[DSSER_PARA_OFFS][channel];
      
      if (!serCh)
      {
        DSSER_TRY( ds2210_ser_init( base, fifo_size, &serCh ) );
      }
      else
      { 
        if (dsser_fifo_realloc(serCh,fifo_size))
        {
          return NULL;
        }
      }
      dsser_is_used[DSSER_PARA_OFFS][channel]=1;        
    break;
    case PHS_BT_DS2211:
      serCh = dsser_lookup_table[DSSER_PARA_OFFS][channel];
      
      if (!serCh)
      {
        DSSER_TRY( ds2211_ser_init( base, fifo_size, &serCh ) );
      }
      else
      { 
        if (dsser_fifo_realloc(serCh,fifo_size))
        {
          return NULL;
        }
      }
      dsser_is_used[DSSER_PARA_OFFS][channel]=1;        
    break;
    case PHS_BT_DS4201S:
      serCh = dsser_lookup_table[(((base / PHS_AF) & 0xF0 ) >> 4)][channel];
      
      if (!serCh)
      {
        DSSER_TRY( ds4201_ser_init( base, channel, fifo_size, &serCh ) );
      }
      else
      { 
        if (dsser_fifo_realloc(serCh,fifo_size))
        {
          return NULL;
        }
      }
      dsser_is_used[(((base / PHS_AF) & 0xF0 ) >> 4)][channel]=1;
    break;
    #if defined (_DS1005)
    case PHS_BT_NO_BOARD:
      serCh = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];
      
      if (!serCh)
      {
         DSSER_TRY( ds1005_ser_init( DS1005_UART, fifo_size, &serCh ) );
      }
      else
      { 
        if (dsser_fifo_realloc(serCh,fifo_size))
        {
          return NULL;
        }
      }
      dsser_is_used[DSSER_MAX_BOARDS-1][0]=1;
    break;
    #endif
	#if defined (_DS1006)
    case PHS_BT_NO_BOARD:
      serCh = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];
      
      if (!serCh)
      {
         DSSER_TRY( ds1006_ser_init( DS1006_UART, fifo_size, &serCh ) );
      }
      else
      { 
        if (dsser_fifo_realloc(serCh,fifo_size))
        {
          return NULL;
        }
      }
      dsser_is_used[DSSER_MAX_BOARDS-1][0]=1;
    break;
    #endif
    default:
      error = DSSER_NOT_FOUND; goto Error;
  }

  serCh->board_bt = board_bt;
#endif

#if defined(_DS1401)
  serCh = dsser_lookup_table[DSSER_PARA_OFFS][channel];
  
  if (!serCh)
  {
    DSSER_TRY ( ser_tp1_init_internal (base, channel, fifo_size ));
    
    serCh = dsser_lookup_table[DSSER_PARA_OFFS][channel];
  }

  dsser_is_used[DSSER_PARA_OFFS][channel] = 1;  
#endif

#if defined(_DS1103)
  serCh = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];
  
  if (!serCh)
  {
    DSSER_TRY(ds1103_dsser_init(DS1103_UART, fifo_size));
    
    serCh = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];
  }
  else
  { 
    if (dsser_fifo_realloc(serCh,fifo_size))
    {
      return NULL;
    }
  }
 
  dsser_is_used[DSSER_MAX_BOARDS-1][0] = 1;  
#endif

#if defined(_DS1104)
  serCh = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];
  
  if (!serCh)
  {
    DSSER_TRY(ds1104_ser_init(DS1104_UART, fifo_size, &serCh));
    
    serCh = dsser_lookup_table[DSSER_MAX_BOARDS-1][0];    
  }
  else
  { 
    if (dsser_fifo_realloc(serCh,fifo_size))
    {
      return NULL;
    }
  }
  dsser_is_used[DSSER_MAX_BOARDS-1][0] = 1;
#endif

  serCh->board = board;

  serCh->channel = channel;

  serCh->interrupt_mode = 0;

  serCh->frequency = dsser_board_descr[DSSER_SUPPORTED_FREQUENCY][board];

  serCh->subint = 0;

  return serCh;

Error:
  dsser_message_wrapper(DSSER_PARA_OFFS, channel, error, "dsser_init");
  return (NULL);
}

/** Returns the handle to an already initialized and used dsserChannel or NULL. */
dsserChannel* dsser_handle_get( UInt32 base, UInt32 channel)
{
#if defined(_DS1401)
  if (dsser_is_used[DSSER_PARA_OFFS][channel])
    return dsser_lookup_table[DSSER_PARA_OFFS][channel];
  else
    return NULL;
#endif

#if defined(_DS1103) || defined(_DS1104)
  if (dsser_is_used[DSSER_MAX_BOARDS-1][channel])
    return dsser_lookup_table[DSSER_MAX_BOARDS-1][channel];
  else
    return NULL;
#endif

#if (defined (_DS1005) || defined (_DS1006)) 
  if(base == DSSER_ONBOARD)
  {
      if (dsser_is_used[DSSER_MAX_BOARDS-1][0])
        return dsser_lookup_table[DSSER_MAX_BOARDS-1][0];
      else
        return NULL;
  }
  else
  {
    Int32 board_bt = phs_board_type_get(base);

    if (board_bt == PHS_BT_NO_BOARD || board_bt == PHS_BT_INVALID_BASE || board_bt == PHS_BT_UNKNOWN )
    {
        return NULL;
    }
    else
    {
      switch(board_bt)
      {
        case PHS_BT_DS2202:
        case PHS_BT_DS2210:
        case PHS_BT_DS2211:
              if (dsser_is_used[DSSER_PARA_OFFS][channel])
                return dsser_lookup_table[DSSER_PARA_OFFS][channel];
              else
                return NULL;
        case PHS_BT_DS4201S:
              if (dsser_is_used[(((base / PHS_AF) & 0xF0 ) >> 4)][channel])
                 return dsser_lookup_table[(((base / PHS_AF) & 0xF0 ) >> 4)][channel];
              else
                return NULL;
        default:
            return NULL;
      }
    }
  }
#endif  
}

#endif
#ifndef _INLINE
/**
* To configure the serial interface.
*
* PARAMETERS:
*
*  serCh              Pointer to the serial channel structure (see dsser_init).
*
*  fifo_mode          Mode of the receive buffer.
* 
*                     DSSER_FIFO_MODE_BLOCKED
*                     If the receive buffer is full, new data is rejected.
*
*                     DSSER_FIFO_MODE_OVERWRITE
*                     If the receive buffer is full, new data replaces the
*                     oldest data in the buffer. 
*
*  baudrate           300, 600, 1200, 2400, 4800, 9600 19200, 38400, 115000
*
*                     A DS2202/DS2210/DS2211 in RS422 mode (uart_mode=DSSER_RS422) can
*                     also use 1000000.
*
*  databits           number of data bits. Values are: 5, 6, 7, 8.
*
*  stop_bits          number of stop bits. The following symbols are predefined:
*
*                     DSSER_1_STOPBIT
*                     1 stop bit
*
*                     DSSER_2_STOPBIT
*                     the number of stop bits depends on the number of the
*                     specified data bits:
*
*                     5 data bits: 1.5 stop bits
*                     6 data bits: 2 stop bits
*                     7 data bits: 2 stop bits
*                     8 data bits: 2 stop bits
*
*  parity             Specifies whether and how parity bits are generated. The 
*                     following symbols are predefined:
*
*                     DSSER_NO_PARITY
*                     no parity bits
*
*                     DSSER_ODD_PARITY
*                     Parity bit is set so that there is an odd number of "1" 
*                     bits in the byte, including the parity bit.
*
*                     DSSER_EVEN_PARITY
*                     Parity bit is set so that there is an even number of "1"
*                     bits in the byte, including the parity bit.
*                     
*                     DSSER_FORCED_PARITY_ONE
*                     Parity bit is forced to a logic 1
*                     
*                     DSSER_FORCED_PARITY_ZERO
*                     Parity bit is forced to a logic 1
*
* uart_trigger_level  The UART trigger level is hardware dependent. After the 
*                     specified number of bytes is received, the UART generates
*                     an interrupt and the bytes are copied into the receive
*                     buffer.
*
*                     Use the highest UART trigger level possible to generate
*                     fewer interrupts.
*
*                     DSSER_1_BYTE_TRIGGER_LEVEL
*                     DSSER_4_BYTE_TRIGGER_LEVEL
*                     DSSER_8_BYTE_TRIGGER_LEVEL
*                     DSSER_14_BYTE_TRIGGER_LEVEL
*
* user_trigger_level  The user trigger level is hardware independent and can
*                     be adjusted in smaller steps than the UART trigger level.
*                     After a specified number of bytes is received in the
*                     receive buffer, the subinterrupt handler is called.
*
*                     DSSER_TRIGGER_LEVEL_DISABLE
*                     No receive subinterrupt handling for the serial interface.
*
*                     DSSER_DEFAULT_TRIGGER_LEVEL
*                     Synchronizes the UART trigger level and the user
*                     trigger level.
*
*                     1..(fifo_size - 1) 
*                     Sets the user trigger level.
*
* uart_mode           Sets the mode of the UART transceiver. The following 
*                     symbols are predefined:
*
*                     DSSER_AUTOFLOW_DISABLE
*                     Transfer without HW handshake (RTS/CTS).
*
*                     DSSER_AUTOFLOW_ENABLE
*                     Transfer with HW handshake (RTS/CTS).
*                     
*                     DSSER_RS232
*                     
*                     DSSER_RS422
*
*                     DSSER_RS485
*                     
*                     DSSER_KLINE
*                     
*
* RETURNS: void
*
*/
void dsser_config( dsserChannel* serCh,
                         UInt32 fifo_mode,
                         UInt32 baudrate,
                         UInt32 databits,
                         UInt32 stopbits,
                         UInt32 parity,
                         UInt32 uart_trigger_level,
                          Int32 user_trigger_level,
                         UInt32 uart_mode )
{
  UInt32 error = DSSER_NO_ERROR;

  if(serCh == NULL)
  {
    error = DSSER_CHANNEL_INIT_ERROR; goto Error;
  }

  /* some checks of the input parameters */

  /* databits */
  if( (databits < 5) || (databits > 8) )
  {
    error = DSSER_ILLEGAL_DATABIT; goto Error;
  }

  /* stopbits */
  if( (stopbits != DSSER_1_STOPBIT) && (stopbits != DSSER_2_STOPBIT) )
  {
    error = DSSER_ILLEGAL_STOPBIT; goto Error;
  }

  /* uart_trigger_level */
  if( uart_trigger_level > DSSER_14_BYTE_TRIGGER_LEVEL)
  {
    error = DSSER_ILLEGAL_UART_TRIGGER_LEVEL; goto Error;
  }

  /* parity */
  if( ( (parity == DSSER_FORCED_PARITY_ONE) || (parity == DSSER_FORCED_PARITY_ZERO ) ) &&
         !dsser_board_descr[DSSER_SUPPORTED_PARITY][serCh->board])
  {
    error = DSSER_ILLEGAL_PARITY; goto Error;
  }

  if(parity > DSSER_FORCED_PARITY_ZERO)
  {
    error = DSSER_ILLEGAL_PARITY; goto Error;
  }

  /* uart_mode */

  if( ( (uart_mode & DSSER_AUTOFLOW_ENABLE) == DSSER_AUTOFLOW_ENABLE ) &&
          !dsser_board_descr[DSSER_SUPPORTED_AUTOFLOW][serCh->board] )
  {
    error = DSSER_ILLEGAL_AUTOFLOW; goto Error;
  }

  if( ( (uart_mode & 0xE) == DSSER_RS232 ) &&
         !dsser_board_descr[DSSER_SUPPORTED_RS232][serCh->board])
  {
    error = DSSER_ILLEGAL_UART_RSMODE; goto Error;
  }

  if( ( (uart_mode & 0xE) == DSSER_RS422 ) &&
         !dsser_board_descr[DSSER_SUPPORTED_RS422][serCh->board])
  {
    error = DSSER_ILLEGAL_UART_RSMODE; goto Error;
  }

  if( ( (uart_mode & 0xE) == DSSER_RS485 ) &&
         !dsser_board_descr[DSSER_SUPPORTED_RS485][serCh->board])
  {
    error = DSSER_ILLEGAL_UART_RSMODE; goto Error;
  }

  if( ( (uart_mode & 0xE) == DSSER_KLINE ) &&
         !dsser_board_descr[DSSER_SUPPORTED_KLINE][serCh->board])
  {
    error = DSSER_ILLEGAL_UART_RSMODE; goto Error;
  }

  serCh->rs_mode = uart_mode;

  if ( (user_trigger_level < DSSER_DEFAULT_TRIGGER_LEVEL   ) ||
       (user_trigger_level > (Int32)(serCh->fifo_size - 1) ) )
  {
    error = DSSER_ILLEGAL_USER_TRIGGER_LEVEL; goto Error;
  }

  if (user_trigger_level == DSSER_DEFAULT_TRIGGER_LEVEL)
  {
    switch(uart_trigger_level)
    {
      case DSSER_1_BYTE_TRIGGER_LEVEL:
         serCh->user_trigger_level = 1;
        break;
      case DSSER_4_BYTE_TRIGGER_LEVEL:
         serCh->user_trigger_level = 4;
        break;
      case DSSER_8_BYTE_TRIGGER_LEVEL:
        serCh->user_trigger_level  = 8;
        break;
      case DSSER_14_BYTE_TRIGGER_LEVEL:
        serCh->user_trigger_level  = 14;
        break;
    }
  }
  else
  {
    serCh->user_trigger_level = user_trigger_level;
  }

  if (fifo_mode > (serCh->fifo_size - 1) )
  {
    error = DSSER_ILLEGAL_FIFO_MODE; goto Error;
  }
  else
  {
    (serCh->rxFifo)->mode = fifo_mode;
    serCh->fifo_mode      = fifo_mode;
  }

  /* configure the serial interface */

  serCh->interrupt_mode |= DSSER_RXRDY_INT | DSSER_THR_EMPTY_INT;

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))

  switch( serCh->board_bt )
  {
    case PHS_BT_DS2202:
      {
        UInt32 mode = DS2202_SER_RS232;

        if( (uart_mode & 0xE) == DSSER_RS422 )
        {
          mode = DS2202_SER_RS422;
        }

        DSSER_TRY( ds2202_ser_config( serCh, fifo_mode, baudrate, databits,
                   stopbits, parity, uart_trigger_level, user_trigger_level, mode ) );
      }
    break;
    case PHS_BT_DS2210:
      {
        UInt32 mode = DS2210_SER_RS232;

        if( (uart_mode & 0xE) == DSSER_RS422 )
        {
          mode = DS2210_SER_RS422;
        }

        DSSER_TRY( ds2210_ser_config( serCh, fifo_mode, baudrate, databits,
                   stopbits, parity, uart_trigger_level, user_trigger_level, mode ) );
      }
    break;
    case PHS_BT_DS2211:
      {
        UInt32 mode = DS2211_SER_RS232;

        if( (uart_mode & 0xE) == DSSER_RS422 )
        {
          mode = DS2211_SER_RS422;
        }

        DSSER_TRY( ds2211_ser_config( serCh, fifo_mode, baudrate, databits,
                   stopbits, parity, uart_trigger_level, user_trigger_level, mode ) );
      }
    break;
    case PHS_BT_DS4201S:

      DSSER_TRY( ds4201_ser_config( serCh, fifo_mode, baudrate, databits,
                 stopbits, parity, uart_trigger_level, user_trigger_level, uart_mode ) );

    break;
    #if defined (_DS1005)
    case PHS_BT_NO_BOARD:

      DSSER_TRY( ds1005_ser_config( serCh, fifo_mode, baudrate, databits,
                 stopbits, parity, uart_trigger_level, user_trigger_level, uart_mode ) );
    break;
    #endif
    #if defined (_DS1006)
    case PHS_BT_NO_BOARD:

      DSSER_TRY( ds1006_ser_config( serCh, fifo_mode, baudrate, databits,
                 stopbits, parity, uart_trigger_level, user_trigger_level, uart_mode ) );
    break;
    #endif
  }
#endif
#if defined(_DS1401)
  DSSER_TRY(dsser_fifo_reset(serCh));
   
  if( DSSER_TRIGGER_LEVEL_DISABLE == user_trigger_level )
  {
    DSSER_TRY( ser_tp1_config_internal( serCh, fifo_mode, baudrate, databits,
             stopbits,  parity, uart_trigger_level, SER_TP1_FIFO_SIZE-1 ) );

    serCh->user_trigger_level = DSSER_TRIGGER_LEVEL_DISABLE;
  }
  else
  {
    DSSER_TRY( ser_tp1_config_internal( serCh, fifo_mode, baudrate, databits,
             stopbits,  parity, uart_trigger_level, user_trigger_level ) );
  }
  
  DSSER_TRY(dsser_enable(serCh));
#endif
#if defined(_DS1103)
  {
    UInt32 rs_mode;
    UInt32 autoflow;

    if( uart_mode & DSSER_RS422)
    {
      rs_mode = DS1103_DSSER_RS422;
    }
    else
    {
      rs_mode = DS1103_DSSER_RS232;
    }

    if( uart_mode & DSSER_AUTOFLOW_ENABLE )
    {
      autoflow = DS1103_DSSER_AUTOFLOW_ENABLE;
    }
    else
    {
      autoflow = DS1103_DSSER_AUTOFLOW_DISABLE;
    }

    DSSER_TRY( ds1103_dsser_config( serCh, fifo_mode, baudrate, databits,
               stopbits, parity, uart_trigger_level, user_trigger_level,
                     rs_mode, autoflow ) );
  }
#endif

#if defined(_DS1104)

  {
    UInt32 rs_mode;
    UInt32 autoflow;


    if( uart_mode & DSSER_RS422)
    {
      rs_mode = DS1104_SER_RS422;
    }
    else if( uart_mode & DSSER_RS485)
    {
      rs_mode = DS1104_SER_RS485;
    }
    else
    {
      rs_mode = DS1104_SER_RS232;
    }

    if( uart_mode & DSSER_AUTOFLOW_ENABLE )
    {
      autoflow = DS1104_SER_AUTOFLOW_ENABLE;
    }
    else
    {
      autoflow = DS1104_SER_AUTOFLOW_DISABLE;
    }

    DSSER_TRY( ds1104_ser_config( serCh, fifo_mode, baudrate, databits,
               stopbits, parity, uart_trigger_level, user_trigger_level,
                     rs_mode, autoflow ) );
  }

#endif

  return;

Error:
  dsser_message_wrapper(serCh->module, serCh->channel, error, "dsser_config");
}

#endif
/**
* Enables the UART interrupt.
*
* PARAMETERS:
*
* serCh          Pointer to the serial channel structure (see dsser_init).
*
* RETURNS:       DSSER_NO_ERROR
*                DSSER_COMMUNICATION_FAILED  
*/
__INLINE Int32 dsser_enable( dsserChannel* serCh )
{
  Int32 error = DSSER_NO_ERROR;
  
  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103) || defined (_DS1104))
  error = dsser_io_ier_set( serCh, serCh->interrupt_mode );
#endif 

#if defined(_DS1401)
  if (serCh->subint_handler_saved)
  {
      /* Restore the original interrupt handler. */
      dsser_subint_handler_inst (serCh, serCh->subint_handler_saved); 
  }
  error = ser_tp1_enable( serCh );
#endif

  return error;
}

/**
* Disables the UART interrupt.
*
* The UART stops transmitting data and incoming
* data is not stored to the receive buffer.
*
* PARAMETERS:
*   serCh        Pointer to the serial channel structure (see dsser_init).
*
* RETURNS:       DSSER_NO_ERROR
*                DSSER_COMMUNICATION_FAILED
*/
__INLINE Int32 dsser_disable ( dsserChannel* serCh )
{
  Int32 error = DSSER_NO_ERROR;

  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103) || defined (_DS1104) )
  error = dsser_io_ier_set( serCh, 0 );
#endif 

#if defined(_DS1401)
  /* 
   * Save the original interrupt handler to disable the task calls. 
   */
  serCh->subint_handler_saved = dsser_subint_handler_inst(serCh, (dsser_subint_handler_t)dsser_tp1_dummy_fcn); 

  error = ser_tp1_disable( serCh );
#endif

  return error;
}

/**
* Transmits datalen bytes from the data buffer to the UART and
* returns the real number of transmitted bytes in count.
*
* PARAMETERS:
*
* serCh          Pointer to the serial channel structure (see dsser_init).
*
* datalen        Number of bytes to be transmitted.
*
* data           Pointer to the data buffer.
*
* count          Pointer to the number of transmitted bytes.
*
* RETURNS:       DSSER_NO_ERROR
*                DSSER_COMMUNICATION_FAILED                
*                DSSER_FIFO_OVERFLOW
*/
__INLINE Int32 dsser_transmit ( dsserChannel* serCh, UInt32 datalen, UInt8* data, UInt32* count)
{
  Int32 error = DSSER_NO_ERROR;
  
  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))
  switch(serCh->board_bt)
  {
    case PHS_BT_DS2202:
        error = ds2202_ser_transmit ( serCh, datalen, data, count);
      break;
    case PHS_BT_DS2210:
        error = ds2210_ser_transmit ( serCh, datalen, data, count);
      break;
    case PHS_BT_DS2211:
        error = ds2211_ser_transmit ( serCh, datalen, data, count);
      break;
    case PHS_BT_DS4201S:
        error = ds4201_ser_transmit ( serCh, datalen, data, count);
      break;
    #if defined (_DS1005)
    case PHS_BT_NO_BOARD:
        error = ds1005_ser_transmit ( serCh, datalen, data, count);
      break;
    #endif
	#if defined (_DS1006)
    case PHS_BT_NO_BOARD:
        error = ds1006_ser_transmit ( serCh, datalen, data, count);
      break;
    #endif
    default:
        error = DSSER_COMMUNICATION_FAILED;
      break;
  }
#endif   

#if defined(_DS1401)
  error = ser_tp1_transmit_internal ( serCh, datalen, data, count);
  
  switch(error)
  {
   case SER_TP1_FIFO_OVERFLOW:
       error = DSSER_FIFO_OVERFLOW;
     break;
   case SER_TP1_BUFFER_OVERFLOW: /* == DSSER_COMMUNICATION_FAILED */
       error = DSSER_COMMUNICATION_FAILED;
     break;
   default:
       error = DSSER_NO_ERROR;
     break;
  }
#endif   

#if defined(_DS1103)
  error = ds1103_dsser_transmit( serCh, datalen, data, count );
#endif   

#if defined(_DS1104)
  error = ds1104_ser_transmit( serCh, datalen, data, count );
#endif
  
  return error;
}
/**
* Receives datalen bytes from the receive FIFO and stores them
* into the data buffer. The number of real received bytes
* is stored into count.
*
* PARAMETERS:
*
* serCh           Pointer to the serial channel structure (see dsser_init).
*
* datalen         Max number of bytes to receive.
*
* data            Pointer to the destination data buffer.
*
* count           Pointer to the number of received bytes.
*
* RETURNS:        DSSER_NO_ERROR
*                 DSSER_NO_DATA                
*                 DSSER_FIFO_OVERFLOW
*                 DSSER_COMMUNICATION_FAILED
*/
__INLINE Int32 dsser_receive ( dsserChannel* serCh, UInt32 datalen, UInt8* data, UInt32* count)
{
  Int32 error = DSSER_NO_ERROR;

  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103) || defined (_DS1104) )
 *count = dsfifo_read( serCh->rxFifo, datalen, data);
 
 if(*count == 0)
 {
   error = DSSER_NO_DATA;
 }
 else if ( (serCh->rxFifo)->error == DSFIFO_BUFFER_OVERFLOW)
 {
   (serCh->rxFifo)->error = 0;
   error = DSSER_FIFO_OVERFLOW;
 }
 else
 {
   error = DSSER_NO_ERROR;
 }
#endif 

#if defined(_DS1401)
  error =  ser_tp1_receive ( serCh, (Int32)datalen, data, count );
  
  switch(error)
  {
   case SER_TP1_SLAVE_FIFO_OVERFLOW:
       error = DSSER_FIFO_OVERFLOW;
     break;
   case SER_TP1_SLAVE_DATA_LOST:
       error = DSSER_FIFO_OVERFLOW;
     break;
   case SER_TP1_SEMA_ACCESS_TIMEOUT:   
       error = DSSER_COMMUNICATION_FAILED;
     break;
   case SER_TP1_SEMA_ACCESS_ERROR:
       /* conceal the error */
       error = DSSER_NO_ERROR;
     break;
   case SER_TP1_NO_DATA:
       error = DSSER_NO_DATA;
     break;
   default:
       error = DSSER_NO_ERROR;
     break;
  }
#endif 
 
 return error;
}

/**
*
* This function reads from the receive FIFO until count
* is reached or the terminating character occurs in the FIFO. 
* When the terminating character is found in the FIFO
* the terminating character is also stored in the data buffer.
*
* PARAMETERS:
*
* serCh           Pointer to the serial channel structure (see dsser_init).
*
* datalen         Max number of bytes to receive.
*
* data            Pointer to the destination data buffer.
*
* count           Pointer to the number of received bytes.
*
* term            Character which terminates the receiving e.g: 
*                 '\n' New line
*                 '\r' Carriage return
*
* RETURNS:        DSSER_NO_ERROR
*                 DSSER_NO_DATA                
*                 DSSER_FIFO_OVERFLOW
*                 DSSER_COMMUNICATION_FAILED
*/
__INLINE Int32 dsser_receive_term ( dsserChannel* serCh, UInt32 datalen, UInt8* data, UInt32* count, UInt8 term)
{
  Int32 error = DSSER_NO_ERROR;

  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

 #if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103) || defined (_DS1104) )
 *count = dsfifo_read_term( serCh->rxFifo, datalen, data, term);
 
 if(*count == 0)
 {
   error = DSSER_NO_DATA;
 }
 else if ( (serCh->rxFifo)->error == DSFIFO_BUFFER_OVERFLOW)
 {
   (serCh->rxFifo)->error = 0;
   error = DSSER_FIFO_OVERFLOW;
 }
 else
 {
   error = DSSER_NO_ERROR;
 }
#endif

#if defined(_DS1401)
  error = ser_tp1_receive_term ( serCh, (Int32)datalen, data, count, term );

  switch(error)
  {
   case SER_TP1_SLAVE_FIFO_OVERFLOW:
       error = DSSER_FIFO_OVERFLOW;
     break;
   case SER_TP1_SLAVE_DATA_LOST:
       error = DSSER_FIFO_OVERFLOW;
     break;
   case SER_TP1_SEMA_ACCESS_TIMEOUT:   
       error = DSSER_COMMUNICATION_FAILED;
     break;
   case SER_TP1_SEMA_ACCESS_ERROR:
       error = DSSER_COMMUNICATION_FAILED;
     break;
   case SER_TP1_NO_DATA:
       error = DSSER_NO_DATA;
     break;
   default:
       error = DSSER_NO_ERROR;
     break;
  }
#endif 
 
 return error;
}

/**
* Stores the status information into corresponding member of serCh.
*
* It is possible to read several registers by combining the constants with a logical OR.
*
* PARAMETERS:
*
*   serCh           Pointer to the serial channel structure (see dsser_init).
*
*   register_type   DSSER_STATUS_IIR_FCR
*                   interrupt status register
*
*                   DSSER_STATUS_LSR
*                   line status register
*
*                   DSSER_STATUS_MSR
*                   modem status register
*
* RETURNS:          DSSER_NO_ERROR
*                   DSSER_COMMUNICATION_FAILED
*/
__INLINE Int32 dsser_status_read ( dsserChannel* serCh, UInt8 register_type )
{
   Int32 error = DSSER_COMMUNICATION_FAILED;

   rtlib_int_status_t int_status;

   if(serCh == NULL)
   {
     return DSSER_COMMUNICATION_FAILED;
   }

   RTLIB_INT_SAVE_AND_DISABLE( int_status );

   if( register_type & DSSER_STATUS_IIR_FCR )
   { 
        if( serCh->isr )
        {
          serCh->intStatusReg.Byte = serCh->isr;
          error = DSSER_NO_ERROR;
          serCh->isr = 0;
        }
        else
        {
          error = dsser_io_status_read ( serCh, DSSER_STATUS_IIR_FCR,
           &serCh->intStatusReg.Byte );
        }
   }
   if( register_type & DSSER_STATUS_LSR )
   { 
        if( serCh->lsr )
        {
          serCh->lineStatusReg.Byte = serCh->lsr;
          error = DSSER_NO_ERROR;
          serCh->lsr = 0;
        }
        else
        {
          error = dsser_io_status_read ( serCh, DSSER_STATUS_LSR,
           &serCh->lineStatusReg.Byte );
        }
   }
   if( register_type & DSSER_STATUS_MSR )
   {
        if( serCh->msr )
        {
          serCh->modemStatusReg.Byte = serCh->msr;
          error = DSSER_NO_ERROR;
          serCh->msr = 0;
        }
        else
        {
          error = dsser_io_status_read ( serCh, DSSER_STATUS_MSR,
           &serCh->modemStatusReg.Byte );
        }
   }
 
   RTLIB_INT_RESTORE(int_status);
   
   return error;
}


#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103) || defined (_DS1104) )

/**
* Read the error flag of the serial interface. 
*
* PARAMETERS:
*
* serCh            Pointer to the serial channel structure.
*
* RETURNS:         DSSER_NO_ERROR
*                  DSSER_FIFO_OVERFLOW 
*                  DSSER_SLAVE_INIT_ACK
*                  DSSER_SLAVE_DATA_LOST
*                  DSSER_SLAVE_FIFO_OVERFLOW
*/
__INLINE Int32 dsser_error_read ( dsserChannel* serCh )
{
  Int32 error = DSSER_NO_ERROR;
 
  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }
 
  if ((serCh->rxFifo)->error)
  {
    error = (serCh->rxFifo)->error;
   
    (serCh->rxFifo)->error = 0;
  } 

  return error;
}

#endif 

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103) || defined (_DS1104) )

/**
* Returns the number of bytes in the transmit FIFO
*
* PARAMETERS:
*
*   serCh          Pointer to the serial channel structure (see dsser_init).
*
* RETURNS:         Number of bytes in the transmit FIFO
*
*/
__INLINE Int32 dsser_transmit_fifo_level ( dsserChannel* serCh )
{
  if(serCh == NULL)
  {
    return 0;
  }

  return dsfifo_in_count_get(serCh->txFifo); 
}

#endif 

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103) || defined (_DS1104) )

/**
* Returns the number of bytes in the receive FIFO.
*
* PARAMETERS:
*
* serCh           Pointer to the serial channel structure (see dsser_init).
*
* RETURNS:        Number of bytes in the receive FIFO.
*/
__INLINE Int32 dsser_receive_fifo_level ( dsserChannel* serCh )
{
  if(serCh == NULL)
  {
    return 0;
  }

  return dsfifo_in_count_get( serCh->rxFifo ); 
}

#endif 

/**
* Reallocates the SW FIFO with a new size or clears only if the size is unchanged.
*
* @param serCh        Pointer to the serial channel structure (see dsser_init).
*
* @param fifo_size    Size of the transmit and receive buffer in bytes. The size
*                     must be a power of two (2^N) and at least 64 bytes.
*
* @return             DSSER_NO_ERROR or
*                     DSSER_ALLOC_ERROR
*/
__INLINE Int32 dsser_fifo_realloc( dsserChannel* serCh, UInt32 fifo_size )
{
    if (serCh == 0)
    {
        return DSSER_CHANNEL_INIT_ERROR;
    }

    if (fifo_size != serCh->fifo_size)
    {
        rtlib_int_status_t int_status;

        RTLIB_INT_SAVE_AND_DISABLE( int_status );

        dsfifo_free(serCh->txFifo);
        dsfifo_free(serCh->rxFifo);

        serCh->txFifo = dsfifo_init(fifo_size,0);
        serCh->rxFifo = dsfifo_init(fifo_size,1);

        RTLIB_INT_RESTORE(int_status);
        
        if (serCh->txFifo == NULL || serCh->rxFifo == NULL)
        {
            return DSSER_ALLOC_ERROR;
        }
    }
    else
    {
        rtlib_int_status_t int_status;

        RTLIB_INT_SAVE_AND_DISABLE( int_status );

        dsfifo_clear(serCh->txFifo);
        dsfifo_clear(serCh->rxFifo);
        
        RTLIB_INT_RESTORE(int_status);
    }
    
    return DSSER_NO_ERROR;
}


/**
* @fn dsser_fifo_reset 
*
* Clears the SW and HW-FIFOs and diasables the UART interrupt.
*
* PARAMETERS:
*   serCh        Pointer to the serial channel structure (see dsser_init).
*
* RETURNS:       DSSER_NO_ERROR
*                DSSER_COMMUNICATION_FAILED
*/
#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))

__INLINE Int32 dsser_fifo_reset ( dsserChannel* serCh )
{
  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

  switch(serCh->board_bt)
  {
    case PHS_BT_DS2202:
      return ds2202_ser_fifo_reset (serCh);
    case PHS_BT_DS2210:
      return ds2210_ser_fifo_reset (serCh);
    case PHS_BT_DS2211:
      return ds2211_ser_fifo_reset (serCh);
    case PHS_BT_DS4201S:
      return ds4201_ser_fifo_reset (serCh);
    #if defined (_DS1005)
    case PHS_BT_NO_BOARD:
      return  ds1005_ser_fifo_reset ( serCh );
    #endif
	#if defined (_DS1006)
    case PHS_BT_NO_BOARD:
      return  ds1006_ser_fifo_reset ( serCh );
    #endif
  }
 
  return 0;
}

#endif 


#if defined(_DS1401)

__INLINE Int32 dsser_fifo_reset ( dsserChannel* serCh )
{
  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

  switch(ser_tp1_fifo_reset (serCh))
  {
   case SER_TP1_BUFFER_OVERFLOW:
     return DSSER_COMMUNICATION_FAILED;
   
   case SER_TP1_TIMEOUT:
     return DSSER_COMMUNICATION_FAILED;

   default:
     return DSSER_NO_ERROR;
  }
}

#endif 

#if defined(_DS1103)

__INLINE Int32 dsser_fifo_reset ( dsserChannel* serCh )
{
  if(serCh == NULL)
  {
    return 0;
  }

  return  ds1103_dsser_fifo_reset ( serCh );
}

#endif

#if defined(_DS1104)

__INLINE Int32 dsser_fifo_reset ( dsserChannel* serCh )
{
  if(serCh == NULL)
  {
    return 0;
  }

  return  ds1104_ser_fifo_reset ( serCh );
}

#endif


/**
* PARAMETERS:
*   serCh        Pointer to the serial channel structure (see dsser_init).
*
*   type         DSSER_SET_UART_FREQUENCY
*
* RETURNS:       DSSER_NO_ERROR DSSER_COMMUNICATION_FAILED
*/
__INLINE Int32 dsser_set( dsserChannel* serCh, UInt32 type, void* value_p)
{
  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

  switch( type )
  {
    case DSSER_SET_UART_FREQUENCY:
        serCh->frequency = *(UInt32*)value_p;
      break;
    default:
      break;
  } 

  return DSSER_NO_ERROR;
}

#ifndef _INLINE
/** 
* To install a subinterrupt handler for the serial interface.
*
* PARAMETERS:
*
* serCh           Pointer to the serial channel structure (see dsser_init).
*
* subint_handler  Pointer to your subinterrupt handler.
*
*                 The handler must have the following form:
*
*                 void handler( dsserChannel* serCh, Int32 subint );
*
*                 serCh:
*                 The parameter serCh contains the pointer to the serial
*                 channel which calls the handler.
*
*                 subint:
*                 The subinterrupt wich calls the handler is stored in 
*                 the variable subint.
*
*                 The following symbols are predefined:
*
*                 DSSER_NO_SUBINT               -1
*                 flag which is sent after the last triggered subinterrupt
*
*                 DSSER_TRIGGER_LEVEL_SUBINT     0
*                 interrupt triggered when user trigger level is reached
*
*                 DSSER_TX_FIFO_EMPTY_SUBINT     1
*                 
*                 DSSER_REVEIVER_LINE_SUBINT     2
*                 line status interrupt of the UART
*
*                 DSSER_MODEM_STATE_SUBINT       3
*                 modem status interrupt of the UART
*
* RETURNS:        Pointer to the previous installed subinterrupt handler.
*/
dsser_subint_handler_t dsser_subint_handler_inst (dsserChannel* serCh, dsser_subint_handler_t subint_handler)
{
  if(serCh == NULL)
  {
    return 0;
  }

  dsser_subint_handler_t subint_handler_temp = serCh->subint_handler;

  serCh->subint_handler = subint_handler;

  return (subint_handler_temp);
}
#endif

/**
* To enable one or several subinterrupts of the serial interface.
*
* PARAMETERS:
*   serCh         Pointer to the serial channel structure (see dsser_init).
*
*  subint         Specifies the subinterrupts to be enabled. You can combine 
*                 the predefined symbols with the logical operator OR to enable
*                 several subinterrupts. The following symbols are predefined:
*
*                 DSSER_TRIGGER_LEVEL_SUBINT_MASK     
*                 interrupt triggered when user trigger level is reached
*
*                 DSSER_TX_FIFO_EMPTY_SUBINT_MASK     
*                 interrupt triggered when transmit buffer is empty
*
*                 DSSER_REVEIVER_LINE_SUBINT_MASK     
*                 line status interrupt of the UART
*
*                 DSSER_MODEM_STATE_SUBINT_MASK       
*                 modem status interrupt of the UART
*
* RETURNS:        DSSER_NO_ERROR
*                 DSSER_COMMUNICATION_FAILED
*/
__INLINE Int32 dsser_subint_enable( dsserChannel* serCh, UInt8 subint )
{
  rtlib_int_status_t int_status;

  Int32 error = DSSER_NO_ERROR;
  
  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  serCh->subint_mask = serCh->subint_mask | subint;

  if ( subint )
  {
#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103)|| defined (_DS1104) )

    if (subint & DSSER_MODEM_STATE_SUBINT_MASK)
    {
     serCh->interrupt_mode = serCh->interrupt_mode | DSSER_MODEM_ST_INT;
    }
    if (subint & DSSER_RECEIVER_LINE_SUBINT_MASK)
    {
     serCh->interrupt_mode = serCh->interrupt_mode | DSSER_LINE_STATE_INT;
    }
 
    error = dsser_io_ier_set( serCh, serCh->interrupt_mode );

#endif 
#if defined(_DS1401)

  {
    UInt8 subint_temp = 0;

    if (subint & DSSER_TRIGGER_LEVEL_SUBINT_MASK)
    {
     serCh->interrupt_mode = serCh->interrupt_mode | SER_TP1_RECEIVE_READY_SUBINT;
     subint_temp = subint_temp | SER_TP1_RECEIVE_READY_SUBINT;
    }
    if (subint & DSSER_TX_FIFO_EMPTY_SUBINT_MASK)
    {
     serCh->interrupt_mode = serCh->interrupt_mode | SER_TP1_TRANSMIT_EMPTY_SUBINT;
     subint_temp = subint_temp | SER_TP1_TRANSMIT_EMPTY_SUBINT;
    }
    if (subint & DSSER_RECEIVER_LINE_SUBINT_MASK)
    {
     serCh->interrupt_mode = serCh->interrupt_mode | SER_TP1_LINE_STATUS_SUBINT;
     subint_temp = subint_temp | SER_TP1_LINE_STATUS_SUBINT;
    }
    if (subint & DSSER_MODEM_STATE_SUBINT_MASK)
    {
     serCh->interrupt_mode = serCh->interrupt_mode | SER_TP1_MODEM_STATUS_SUBINT;
     subint_temp = subint_temp | SER_TP1_MODEM_STATUS_SUBINT;
    }

    error = ser_tp1_subint_enable( serCh, subint_temp );

    if ( error != DSSER_NO_ERROR )
    {
     error = DSSER_COMMUNICATION_FAILED;
    }
  }
#endif 
  }

  RTLIB_INT_RESTORE(int_status);
  
  return error;
}

/**
* To disable one or several subinterrupts of the serial interface.
*
* PARAMETERS:
*   serCh         Pointer to the serial channel structure (see dsser_init).
*
*  subint         Specifies the subinterrupts to be disbled. You can combine 
*                 the predefined symbols with the logical operator OR to enable
*                 several subinterrupts. The following symbols are predefined:
*
*                 DSSER_TRIGGER_LEVEL_SUBINT_MASK     
*                 interrupt triggered when user trigger level is reached
*
*                 DSSER_TX_FIFO_EMPTY_SUBINT_MASK     
*                 interrupt triggered when transmit buffer is empty
*
*                 DSSER_REVEIVER_LINE_SUBINT_MASK     
*                 line status interrupt of the UART
*
*                 DSSER_MODEM_STATE_SUBINT_MASK       
*                 modem status interrupt of the UART
*
* RETURNS:        DSSER_NO_ERROR
*                 DSSER_COMMUNICATION_FAILED
*
*/
__INLINE Int32 dsser_subint_disable ( dsserChannel* serCh, UInt8 subint )
{
  Int32 error = DSSER_NO_ERROR;
  
  rtlib_int_status_t int_status;

  if(serCh == NULL)
  {
    return DSSER_COMMUNICATION_FAILED;
  }

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  serCh->subint_mask = serCh->subint_mask & ~subint;

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103)|| defined (_DS1104) )

  if (subint & DSSER_MODEM_STATE_SUBINT_MASK)
  {
   serCh->interrupt_mode = serCh->interrupt_mode & ~DSSER_MODEM_ST_INT;
   
   error = dsser_io_ier_set( serCh, serCh->interrupt_mode );
  }

#endif 
#if defined(_DS1401)
 { 
  UInt8 subint_temp = 0;

  if (subint & DSSER_TRIGGER_LEVEL_SUBINT_MASK)
  {
    serCh->interrupt_mode = serCh->interrupt_mode & ~SER_TP1_RECEIVE_READY_SUBINT;
    subint_temp = SER_TP1_RECEIVE_READY_SUBINT;
  }
  if (subint & DSSER_TX_FIFO_EMPTY_SUBINT_MASK)
  {
    serCh->interrupt_mode = serCh->interrupt_mode & ~SER_TP1_TRANSMIT_EMPTY_SUBINT;
    subint_temp = subint_temp | SER_TP1_TRANSMIT_EMPTY_SUBINT;
  }
  if (subint & DSSER_RECEIVER_LINE_SUBINT_MASK)
  {
    serCh->interrupt_mode = serCh->interrupt_mode & ~SER_TP1_LINE_STATUS_SUBINT;
    subint_temp = subint_temp | SER_TP1_LINE_STATUS_SUBINT;
  }
  if (subint & DSSER_MODEM_STATE_SUBINT_MASK)
  {
    serCh->interrupt_mode = serCh->interrupt_mode & ~SER_TP1_MODEM_STATUS_SUBINT;
    subint_temp = subint_temp | SER_TP1_MODEM_STATUS_SUBINT;
  }

  error = ser_tp1_subint_disable( serCh, subint_temp );
 }
 
 if ( error != DSSER_NO_ERROR )
 {
   error = DSSER_COMMUNICATION_FAILED;
 }

#endif 
  
  RTLIB_INT_RESTORE(int_status);

  return error;
}

#ifndef _INLINE
/**
* Tries to Close the serial interface to be used by another process.
*
* @param serCh   Pointer to the serial channel to be freed.
*
* @return        - DSSER_NO_ERROR if the serial interface is closed.
*                - DSSER_TX_FIFO_NOT_EMPTY if the TX FIFO is not empty. 
*                - DSSER_CHANNEL_INIT_ERROR if serCh == 0
*/
Int32 dsser_free( dsserChannel* serCh )
{
#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))
    UInt32 base;
#endif
    
    if (serCh == 0)
    {
        return DSSER_CHANNEL_INIT_ERROR;
    }

    /* Cancel the command if the TX FIFO is not empty */
    if (dsser_transmit_fifo_level(serCh))
    {
        return DSSER_TX_FIFO_NOT_EMPTY;
    }
    
    /* Disable the UART interrupts */
    dsser_disable(serCh); 
    
#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))
  base = serCh->module;
  
  switch(serCh->board_bt)
  {
    case PHS_BT_DS2202:
      dsser_is_used[DSSER_PARA_OFFS][serCh->channel] = 0;        
      break;
    case PHS_BT_DS2210:
      dsser_is_used[DSSER_PARA_OFFS][serCh->channel] = 0;        
      break;
    case PHS_BT_DS2211:
      dsser_is_used[DSSER_PARA_OFFS][serCh->channel] = 0;        
      break;
    case PHS_BT_DS4201S:
      dsser_is_used[(((base / PHS_AF) & 0xF0 ) >> 4)][serCh->channel] = 0;
      break;
    #if defined (_DS1005)
    case PHS_BT_NO_BOARD:
      dsser_is_used[DSSER_MAX_BOARDS-1][0] = 0;
      break;
    #endif
    #if defined (_DS1006)
    case PHS_BT_NO_BOARD:
      dsser_is_used[DSSER_MAX_BOARDS-1][0] = 0;
      break;
    #endif
  }
#endif

#if defined(_DS1103)
     dsser_is_used[DSSER_MAX_BOARDS-1][serCh->channel] = 0;
#endif
#if defined(_DS1104)
     dsser_is_used[DSSER_MAX_BOARDS-1][serCh->channel] = 0;
#endif
#if defined(_DS1401)
     dsser_is_used[serCh->module][serCh->channel]    = 0;        
#endif

  return DSSER_NO_ERROR;
}

#endif

__INLINE UInt8* dsser_word2bytes ( UInt32* word, UInt8* bytes, int bytesInWord )
{
#if defined (_DS1003) 
 
 switch( bytesInWord ) 
 {
      case 2 :
         bytes[0] = ( *word & 0x000000FF );
         bytes[1] = ( *word & 0x0000FF00 ) >> 8;
         bytes[2] = 0;
         bytes[3] = 0;
      break;
       case 3 :
         bytes[0] = ( *word & 0x000000FF );
         bytes[1] = ( *word & 0x0000FF00 ) >> 8;
         bytes[2] = ( *word & 0x00FF0000 ) >> 16;
         bytes[3] = 0;
      break;
       case 4 :
         bytes[0] = ( *word & 0x000000FF );
         bytes[1] = ( *word & 0x0000FF00 ) >> 8;
         bytes[2] = ( *word & 0x00FF0000 ) >> 16;
         bytes[3] = ( *word & 0xFF000000 ) >> 24;
      break;
      default:
      break;
 }

 return (bytes);
#else
 return (UInt8*)(word);
#endif

}

__INLINE UInt32* dsser_bytes2word( UInt8* bytes_p, UInt32* word_p, int bytesInWord  )
{
#if defined (_DS1003)

 UInt32  shift = 0, i;
 UInt32  temp  = 0;
 
 for (i = 0; i < bytesInWord ; i++)
 {          
   temp = temp | ((bytes_p[i] & 0xFF) << shift); 
   shift += 8;
 }

 *word_p = temp;

 return (word_p);

#else
 return ((UInt32*)bytes_p);
#endif

}

#ifndef _INLINE

void dsser_message_wrapper(UInt32 offs, UInt32 ch, Int32 error, char* function)
{
  switch (error)
  {
    case DSSER_NO_ERROR:
      break;

    case DSSER_NOT_FOUND:
      DSSER_MSG_ERROR_PRINTF(DSSER_NOT_FOUND);
      break;

    case DSSER_ALLOC_ERROR:
      DSSER_MSG_ERROR_PRINTF(DSSER_ALLOC_ERROR_MSG);
      break;

    case DSSER_CHANNEL_INIT_ERROR:
      DSSER_MSG_ERROR_PRINTF(DSSER_CHANNEL_INIT_ERROR);
      break;

    case DSSER_ILLEGAL_BAUDRATE:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_BAUDRATE);
      break;

    case DSSER_ILLEGAL_DATABIT:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_DATABIT);
      break;

    case DSSER_ILLEGAL_STOPBIT:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_STOPBIT);
      break;

    case DSSER_ILLEGAL_PARITY:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_PARITY);
      break;

    case DSSER_ILLEGAL_UART_RSMODE:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_UART_RSMODE);
      break;

    case DSSER_ILLEGAL_UART_TRIGGER_LEVEL:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_UART_TRIGGER_LEVEL);
      break;

    case DSSER_ILLEGAL_USER_TRIGGER_LEVEL:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_USER_TRIGGER_LEVEL);
      break;

    case DSSER_ILLEGAL_FIFO_MODE:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_FIFO_MODE);
      break;

    case DSSER_ILLEGAL_AUTOFLOW:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_AUTOFLOW);
      break;

    case DSSER_ILLEGAL_FIFO_SIZE:
      DSSER_MSG_ERROR_PRINTF(DSSER_ILLEGAL_FIFO_SIZE);
      break;

    default:
      msg_error_printf (MSG_SM_DSSER, error, "%s( 0x%02lX, ch=%d)", function, offs, ch );
      break;
    }
    
    exit(1);   

    return;
}

#endif /* _INLINE */

__INLINE Int32 dsser_io_status_read( dsserChannel* serCh, UInt8 register_type, UInt32* value )
{
  Int32 error = DSSER_NO_ERROR;

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))

  UInt8 register_type_lowlevel;

  UInt32 register_type_lowlevel2202;

  UInt32 register_type_lowlevel2210;

  UInt32 register_type_lowlevel2211;

  switch(serCh->board_bt)
  {
    case PHS_BT_DS2202:

       if( DSSER_STATUS_IIR_FCR == register_type )
       {
         register_type_lowlevel2202 = DS2202_SER_IIR_FCR;
       }
       else if( DSSER_STATUS_LSR == register_type )
       {
         register_type_lowlevel2202 = DS2202_SER_LSR;
       }
       else
       {
         register_type_lowlevel2202 = DS2202_SER_MSR;
       }

       *value = ds2202_serial_register_read ( serCh, register_type_lowlevel2202 );

    case PHS_BT_DS2210:

       if( DSSER_STATUS_IIR_FCR == register_type )
       {
         register_type_lowlevel2210 = DS2210_SER_IIR_FCR;
       }
       else if( DSSER_STATUS_LSR == register_type )
       {
         register_type_lowlevel2210 = DS2210_SER_LSR;
       }
       else
       {
         register_type_lowlevel2210 = DS2210_SER_MSR;
       }

       *value = ds2210_serial_register_read ( serCh, register_type_lowlevel2210 );

    case PHS_BT_DS2211:

       if( DSSER_STATUS_IIR_FCR == register_type )
       {
         register_type_lowlevel2211 = DS2211_SER_IIR_FCR;
       }
       else if( DSSER_STATUS_LSR == register_type )
       {
         register_type_lowlevel2211 = DS2211_SER_LSR;
       }
       else
       {
         register_type_lowlevel2211 = DS2211_SER_MSR;
       }

       *value = ds2211_serial_register_read ( serCh, register_type_lowlevel2211 );

    case PHS_BT_DS4201S:

       if( DSSER_STATUS_IIR_FCR == register_type )
       {
         register_type_lowlevel = DS4201_SER_IIR_FCR;
       }
       else if( DSSER_STATUS_LSR == register_type )
       {
         register_type_lowlevel = DS4201_SER_LSR;
       }
       else
       {
         register_type_lowlevel = DS4201_SER_MSR;
       }

      *value = ds4201_serial_register_read ( serCh, register_type_lowlevel );

    #if defined (_DS1005)
    case PHS_BT_NO_BOARD:

       if( DSSER_STATUS_IIR_FCR == register_type )
       {
         register_type_lowlevel = DS1005_SER_IIR_FCR;
       }
       else if( DSSER_STATUS_LSR == register_type )
       {
         register_type_lowlevel = DS1005_SER_LSR;
       }
       else
       {
         register_type_lowlevel = DS1005_SER_MSR;
       }

      *value = ds1005_serial_register_read ( serCh, register_type_lowlevel );

    #endif /* DS1005 */
	#if defined (_DS1006)
    case PHS_BT_NO_BOARD:

       if( DSSER_STATUS_IIR_FCR == register_type )
       {  
         register_type_lowlevel = DS1006_SER_IIR_FCR;
       }
       else if( DSSER_STATUS_LSR == register_type )
       {
         register_type_lowlevel = DS1006_SER_LSR;
       }
       else
       {
         register_type_lowlevel = DS1006_SER_MSR;
       }

      *value = ds1006_serial_register_read ( serCh, register_type_lowlevel );

    #endif /* DS1006 */
    default:
      error = DSSER_COMMUNICATION_FAILED; 
    break;
  }

#endif
#if defined(_DS1401)
 
  SER_TP1_TRY (ser_tp1_service_request( serCh->serService ));

  do
  {
    error = ser_tp1_service_read( serCh->serService );
  
  }while(error == DSSER_NO_DATA);

  if (register_type & SER_TP1_LINE_STATUS_REG)
  {
    serCh->lineStatusReg.Byte = serCh->serService->data0;
  }
  else if (register_type & SER_TP1_MODEM_STATUS_REG)
  {
    serCh->modemStatusReg.Byte = serCh->serService->data1;
  }
  else if (register_type & SER_TP1_INT_STATUS_REG)
  {
    serCh->intStatusReg.Byte = serCh->serService->data2;
  }

  return DSSER_NO_ERROR;

Error:

  error = DSSER_COMMUNICATION_FAILED;

#endif
#if defined(_DS1103)

       UInt8 register_type_lowlevel;

       if( DSSER_STATUS_IIR_FCR == register_type )
       {  
         register_type_lowlevel = DS1103_DSSER_IIR_FCR;
       }
       else if( DSSER_STATUS_LSR == register_type )
       {
         register_type_lowlevel = DS1103_DSSER_LSR;
       }
       else
       {
         register_type_lowlevel = DS1103_DSSER_MSR;
       }

       *value = ds1103_dsserial_register_read( serCh, register_type_lowlevel );

#endif

#if defined(_DS1104)

       UInt8 register_type_lowlevel;

       if( DSSER_STATUS_IIR_FCR == register_type )
       {  
         register_type_lowlevel = DS1104_SER_IIR_FCR;
       }
       else if( DSSER_STATUS_LSR == register_type )
       {
         register_type_lowlevel = DS1104_SER_LSR;
       }
       else
       {
         register_type_lowlevel = DS1104_SER_MSR;
       }

       *value = ds1104_serial_register_read( serCh, register_type_lowlevel );

#endif


 return error;
}

__INLINE Int32 dsser_io_ier_set( dsserChannel* serCh, UInt8 value )
{
  Int32 error = DSSER_NO_ERROR;

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006))
  switch(serCh->board_bt)
  {
    case PHS_BT_DS2202:
      ds2202_serial_register_write( serCh, DS2202_SER_IER, value );
    break;
    case PHS_BT_DS2210:
      ds2210_serial_register_write( serCh, DS2210_SER_IER, value );
    break;
    case PHS_BT_DS2211:
      ds2211_serial_register_write( serCh, DS2211_SER_IER, value );
    break;
    case PHS_BT_DS4201S:
      ds4201_serial_register_write( serCh, DS4201_SER_IER, value );
    break;
    #if defined (_DS1005)
    case PHS_BT_NO_BOARD:
      ds1005_serial_io_ier_set( serCh, value );
    break;
    #endif /* DS1005 */
    #if defined (_DS1006)
    case PHS_BT_NO_BOARD:
      ds1006_serial_io_ier_set( serCh, value );  
    break;
    #endif /* DS1006 */
  }
#endif

#if defined(_DS1401)
   /* do nothing */
#endif

#if defined(_DS1103)
  ds1103_dsserial_io_ier_set( serCh, value );
#endif

#if defined(_DS1104)
  ds1104_serial_io_ier_set( serCh, value );
#endif

  return error;
}

#undef __INLINE
