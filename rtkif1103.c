/******************************************************************************
  FILE:
    RTKIF1103.C

  DESCRIPTION:
    RTKernel DS1103 specific inline functions

  REMARKS:

  AUTHOR(S): R. Leinfellner

  Copyright (c) 1998 dSPACE GmbH, GERMANY

  $RCSfile: rtkif1103.c $ $Revision: 1.2 $ $Date: 2003/08/07 12:40:13GMT+01:00 $
******************************************************************************/

#ifndef __RTKIF1103_H__
#define __RTKIF1103_H__

#include <tmr1103.h>

#ifdef _INLINE 
# define __INLINE static 
#else
# define __INLINE
#endif

#define DS1103_EIEIO asm( " eieio " )

/* ------------------------------------------------------------------------- */
/*     system timer functions                                                */
/* ------------------------------------------------------------------------- */  

/* system timer wrap around value */
#define RTK_SYS_TIMER_MAX UINT_MAX

/* check system timer wrap around */
#define RTK_CHECK_TIME_WRAP(t2, t1) ((t2) - (t1))

/* calculate a timedifference from two timestamps */
#define RTK_CALCULATE_TIME(ts2, ts1)   ts_timestamp_interval(&(ts1), &(ts2))

/* assign the system time to t */
#define RTK_READ_SYSTEM_TIME(t)  ts_timestamp_read(&(t))

/* calculate the absolute time */
#define RTK_CALCULATE_ABSOLUTE_TIME()                                            \
  {                                                                              \
    if (task->tm_task_type == rtk_tt_periodic)                                   \
    {                                                                            \
      /* Periodic task (rtk_tt_periodic).                                        \
       * ts_abs_time =                                                           \
       *   ( task_calls - 1 ) * sample_time + sample_rate_offset                 \
       */                                                                        \
      ts_timestamp_calculate((((task->tm_task_calls - 1) * task->sample_time) +  \
                              task->sample_rate_offset),                         \
                             &(task->tm_ts_absolute_time));                      \
    }                                                                            \
    else if (task->tm_task_type == rtk_tt_aperiodic)                             \
    {                                                                            \
      if (task->tm_baserate_task && (task->tm_baserate_task->tm_task_calls > 0)) \
      {                                                                          \
        if (task->tm_baserate_task->tm_task_calls >                              \
            task->tm_baserate_task->tm_task_calls_idle)                          \
        {                                                                        \
          /* Aperiodic task _with_ reference task (rtk_tt_aperiodic).            \
           * "Normal" case.                                                      \
           * ts_abs_time =                                                       \
           *   ts_ref_abs_time + ( ts_trigger_time - ts_ref_trigger_time )       \
           */                                                                    \
          ts_timestamp_offset(&((task->tm_baserate_task)->tm_ts_absolute_time),  \
                              &((task->tm_baserate_task)->tm_ts_trigger_time),   \
                              &(task->tm_ts_trigger_time),                       \
                              &(task->tm_ts_absolute_time));                     \
        }                                                                        \
        else                                                                     \
        {                                                                        \
          /* Aperiodic task _with_ reference task (rtk_tt_aperiodic).            \
           * Exception 1: After recovery from idle mode and the reference task   \
           *              has still not been executed.                           \
           * ts_abs_time =                                                       \
           *   ref_sample_time + ( ts_ref_abs_time - ZERO )                      \
           */                                                                    \
          ts_timestamp_type ts_zero = {0, 0};                                    \
          ts_time_offset( task->tm_baserate_task->sample_time,                   \
                         &ts_zero,                                               \
                         &((task->tm_baserate_task)->tm_ts_absolute_time),       \
                         &(task->tm_ts_absolute_time));                          \
        }                                                                        \
      }                                                                          \
      else if (task->tm_baserate_task)                                           \
      {                                                                          \
        /* Aperiodic task _with_ reference task (rtk_tt_aperiodic).              \
         * Exception 2: The reference task has still not been executed           \
         *              (e.g., directly after application start).                \
         * ts_abs_time =                                                         \
         *   ZERO                                                                \
         */                                                                      \
        task->tm_ts_absolute_time.mat = 0;                                       \
        task->tm_ts_absolute_time.mit = 0;                                       \
      }                                                                          \
      else                                                                       \
      {                                                                          \
        /* Aperiodic task _without_ reference task (rtk_tt_aperiodic).           \
         * ts_abs_time =                                                         \
         *   ts_trigger_time                                                     \
         */                                                                      \
        task->tm_ts_absolute_time = task->tm_ts_trigger_time;                    \
      }                                                                          \
    }                                                                            \
    else                                                                         \
    {                                                                            \
      /* Inherited task (rtk_tt_inherited).                                      \
       * ts_abs_time =                                                           \
       *   ts_preempted_absolute_time                                            \
       */                                                                        \
      task->tm_ts_absolute_time = (task->preempted)->tm_ts_absolute_time;        \
    }                                                                            \
  }

/* set an rtk time stamp to zero */
#define RTK_RESET_TIME_STAMP(ts_name)                                         \
  {                                                                           \
    (ts_name).mat = 0;                                                        \
    (ts_name).mit = 0;                                                        \
  }

/* read system timer */
__INLINE unsigned long rtk_read_system_timer()
{
  return ds1103_timebase_low_read();
}

#endif
