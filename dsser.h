/**
 * @file dsser.h
 *
 * DESCRIPTION:
 *    This module contains functions to access the serial interfaces
 *    of DS1103, DS1005, DS1006, DS2210, DS2211, DS4201S, DS1401, DS2202.
 *
 * HISTORY:
 *           1.3   28.08.2000    O.J  added DS1104 support    
 *           1.2   17.05.2000    O.J  added dsser_set(..)
 *           1.1   18.04.2000    O.J  added DS1401, DS1103, DS4201S support    
 *           1.0   13.12.1999    O.J  initial version       
 *
 * dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
 */

/*
 * $RCSfile: dsser.h $ 
 * $Revision: 1.8 $ 
 * $Date: 2008/12/11 09:23:02GMT+01:00 $
 * $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSER.pj $
 */

#ifndef __dsser__
#define __dsser__

#include <dstypes.h>
#include <dsfifo.h>
#include <dsserdef.h>

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
#  error dsser.h: no processor board defined (e.g. _DS1003)
#endif

/* ---- DSSER version -------------------------------------------------- */

/* $DSVERSION: DSSER - Serial Communication Module */

#define DSSER_VER_MAR 2
#define DSSER_VER_MIR 3
#define DSSER_VER_MAI 0
#define DSSER_VER_SPB VCM_VERSION_RELEASE
#define DSSER_VER_SPN 0
#define DSSER_VER_PLV 0

/******************************************************************************
  functions
******************************************************************************/
/** @name Serial Interface Initialization and Configuring */

/** Initializes the serial interface. */
dsserChannel* dsser_init( UInt32 base, UInt32 channel, UInt32 fifo_size );

/** Returns the handle to an already initialized dsserChannel or NULL. */
dsserChannel* dsser_handle_get( UInt32 base, UInt32 channel);

/** Use this function to configure the serial interface. */
void dsser_config( dsserChannel* serCh,
                          UInt32 fifo_mode,
                          UInt32 baudrate,
                          UInt32 databits,
                          UInt32 stopbits,
                          UInt32 parity,
                          UInt32 uart_trigger_level,
                           Int32 user_trigger_level,
                          UInt32 uart_mode ) ;

/** @internal Frees the memory allocated with dsser_init and releases the interrupts. */
Int32 dsser_free( dsserChannel* serCh );

/** Clears the SW and HW-FIFOs and diasables the UART interrupt.   */
__INLINE Int32 dsser_fifo_reset( dsserChannel* serCh );

/** Enables the UART interrupt. */
__INLINE Int32 dsser_enable( dsserChannel* serCh );

/** Disables the UART interrupt. */
__INLINE Int32 dsser_disable( dsserChannel* serCh );


/** @name Receiving and Transmiting Data */

/** Transmit data through the serial interface.*/
__INLINE Int32 dsser_transmit( dsserChannel* serCh, UInt32 datalen, UInt8* data, UInt32* count);

/* Receive data through the serial interface. */
__INLINE Int32 dsser_receive( dsserChannel* serCh, UInt32 datalen, UInt8* data, UInt32* count);

/**
 * Receive data through the serial interface with termination.   
 *
 * The receiving is terminated when the terminating character is detected in the data buffer.
 */
__INLINE Int32 dsser_receive_term( dsserChannel* serCh, UInt32 datalen, UInt8* data, UInt32* count, UInt8 term);

/** @name Serial Interface Status and Error */

/**
 * Read the status.
 *
 * Read the value of one or more status registers and store 
 * the values in the corresponding fields of the channel structure.
 */
__INLINE Int32 dsser_status_read( dsserChannel* serCh, UInt8 register_type );

/** Sets a property of the UART */
__INLINE Int32 dsser_set( dsserChannel* serCh, UInt32 type, void* value_p);

#if (defined(_DS1003) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103))

/** Reads an error flag from the serial interface.*/
__INLINE Int32 dsser_error_read( dsserChannel* serCh );

/** Returns the number of bytes in the transmit buffer. */
__INLINE Int32 dsser_transmit_fifo_level( dsserChannel* serCh );

/** Returns the number of bytes in the receive buffer. */
__INLINE Int32 dsser_receive_fifo_level( dsserChannel* serCh );

#endif

#if defined(_DS1401)
#define dsser_error_read                 ser_tp1_error_read
#define dsser_transmit_fifo_level        ser_tp1_transmit_fifo_level
#define dsser_receive_fifo_level         ser_tp1_receive_fifo_level
#endif

/** @name Subinterrupt Handling */

/** Installs a subinterrupt handler for the serial interface. */
dsser_subint_handler_t dsser_subint_handler_inst( dsserChannel* serCh, dsser_subint_handler_t callback_fcn);

/** Enables one or several subinterrupts of the serial interface.  */
__INLINE Int32 dsser_subint_enable( dsserChannel* serCh, UInt8 subint );

/** Disables one or several subinterrupts of the serial interface. */
__INLINE Int32 dsser_subint_disable( dsserChannel* serCh, UInt8 subint );

/** @name Array Converting Functions */

/** Converts a word (max 4bytes long) into a byte array.   */
__INLINE UInt8* dsser_word2bytes( UInt32* word, UInt8* bytes, int bytesInWord );

/** Converts a byte array with a maximum of 4elements into a single word. */
__INLINE UInt32* dsser_bytes2word(UInt8* bytes_p, UInt32* word_p, int bytesInWord );

#undef __INLINE

/** @internal */
void dsser_vcm_register(void);

/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
#include <dsser.c>
#endif

#endif /* __dsser__ */
