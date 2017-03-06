/******************************************************************************
  FILE:
    RTK1103.H

  DESCRIPTION:
    RTKernel generic header for DS1103

  REMARKS:

  AUTHOR(S): R. Leinfellner

  Copyright (c) 1998 dSPACE GmbH, GERMANY

  $RCSfile: rtk1103.h $ $Revision: 1.3 $ $Date: 2003/12/09 19:12:01GMT+01:00 $
 *****************************************************************************/

#ifndef __RTK1103_H__
#define __RTK1103_H__

#include <dstypes.h>
#include <dsts.h>
#include <dssint.h>

/* ---- interrupt sources -------------------------------------------------- */

#define S_NONE            0               /* no service                        */ 
#define S_TIMERA          2               /* timer A                           */
#define S_EXT0            3               /* external interrupt 0              */
#define S_EXT1            4               /* external interrupt 1              */
#define S_EXT2            5               /* external interrupt 2              */
#define S_EXT3            6               /* external interrupt 3              */
#define S_CAN             7               /* CAN interrupt                     */
#define S_SLAVE           8               /* slave DSP interrupt               */
#define S_SLAVE_PWM       9               /* slave DSP PWM interrupt           */
#define S_UART           10               /* serial UART interrupt             */
#define S_HOST           11               /* host interrupt                    */
#define S_IEIL1          12               /* incremental encoder 1 index line  */
#define S_IEIL2          13               /* incremental encoder 2 index line  */
#define S_IEIL3          14               /* incremental encoder 3 index line  */
#define S_IEIL4          15               /* incremental encoder 4 index line  */
#define S_IEIL5          16               /* incremental encoder 5 index line  */
#define S_IEIL6          17               /* incremental encoder 6 index line  */
#define S_IEIL7          18               /* incremental encoder 7 index line  */
#define S_DEC            19               /* decrementer interrupt             */
#define S_SOFTTASK       20               /* software triggered tasks          */
#define S_SAMPLERATE     21               /* software triggered sample rate    */
#define S_TIMERB         22               /* timer B, periodically             */
#define S_SLAVE_PWM_INV  23               /* inverted slave DSP PWM interrupt  */


#define LAST_SERVICE   S_TIMERB         /* last predefined service identifier*/

#define S_PERIODIC_A   S_DEC            /* 1st periodic triggering service   */
#define S_PERIODIC_B   S_TIMERA         /* 2nd periodic triggering service   */
#define S_PERIODIC_C   S_TIMERB         /* 3rd periodic triggering service   */

#define S_INTERVAL_A   S_TIMERB         /* service for interval timed tasks  */

#define S_BASERATE     S_PERIODIC_A     /* sample rate timer service         */
#define S_TIMER1       S_TIMERA         /* compatibility to older versions   */

/* ---- communication channel definitions ---------------------------------- */

#define C_NONE          0                                       /* no target */
#define C_LOCAL         1                                       /* local     */

/* ---- RTKernel Board ----------------------------------------------------- */

#define VCM_MID_RTK_BOARD VCM_MID_RTK1103

/* ---- supported kernel modules ------------------------------------------- */

#define RTK_KM_IT

/* ---- typedefs ----------------------------------------------------------- */

typedef  ts_timestamp_type rtk_timestamp_type;
typedef  dssint_receiver_type rtk_sint_receiver_type;
typedef  unsigned long rtk_sint_wb_receiver_type;

/* ---- functions defines -------------------------------------------------- */

/* push interrupt status and disable globally */
#define RTK_INT_SAVE_AND_DISABLE(status) RTLIB_INT_SAVE_AND_DISABLE(status)

/* pop interrupt status */
#define RTK_INT_RESTORE(status) RTLIB_INT_RESTORE(status)

/* global interrupt control */
#define rtk_global_int_enable()    DS1103_GLOBAL_INTERRUPT_ENABLE()
#define rtk_global_int_disable()   DS1103_GLOBAL_INTERRUPT_DISABLE()

/* ------------------------------------------------------------------------- */

#endif
