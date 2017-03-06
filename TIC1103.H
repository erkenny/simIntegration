/******************************************************************************
*
* FILE:
*   Tic1103.h
*
* RELATED FILES:
*   Tic1103.c
*   Tmr1103.h
*
* DESCRIPTION: Macros for execution-time measurement with the on-chip timebase
*              (TBL).
*
* HISTORY: 20.07.98 RK initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: TIC1103.H $ $Revision: 1.4 $ $Date: 2003/11/25 08:25:44GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#ifndef __Tic1103__
#define __Tic1103__

#include <Tmr1103.h>

/******************************************************************************
 * data type for counter value of the time base
******************************************************************************/

typedef UInt32  rtlib_tic_t;

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

#define DS1103_TIME_PER_TICK (ds1103_bus_prd * 4)


/******************************************************************************
*
* MACRO:
*           ds1103_tic_start()
*
* SYNTAX:
*           ds1103_tic_start()
*
* DESCRIPTION: Starts the executiontime-measurement. Resets the global variables.
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/

#define ds1103_tic_start() { \
 tic_halt_count = 0;               \
 tic_idle_count = 0;               \
 tic_start_count = asm("mftb 3");  \
}


/******************************************************************************
*
* MACRO:
*          ds1103_tic_halt()
*
* SYNTAX: 
*          ds1103_tic_halt()
*
* DESCRIPTION: Halts the executiontime-measurement. Stores the actual countervalue
*              of the timebase.  
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE: 
*
******************************************************************************/

#define ds1103_tic_halt() { \
  tic_halt_count = asm("mftb 3"); \
} 



/******************************************************************************
*
* MACRO:
*          ds1103_tic_continue()
*   
*
* SYNTAX: 
*          ds1103_tic_continue()
*
* DESCRIPTION: Continues the executiontime-measurement, which was halted before.
*              Calculates and stores the halt-time (tic_idle_count).
*              
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE: 
*
******************************************************************************/

#define ds1103_tic_continue() { \
  rtlib_tic_t timebase;  \
  timebase = asm("mftb 3");   \
  tic_idle_count = tic_idle_count + ( timebase - tic_halt_count); \
}


/******************************************************************************
*
* MACRO:
*          ds1103_tic_read() 
*   
*
* SYNTAX:
*          ds1103_tic_read() 
*
*
* DESCRIPTION: Returns the time (s).  
*
* PARAMETERS: no
*
* RETURNS: the time (s, Float64) from beginning of the exec.time-measurem. 
*          until the end without the idle-time.
*
* NOTE:
*
******************************************************************************/

 #define ds1103_tic_read() (DS1103_TIME_PER_TICK * (Float64) ( asm("mftb 3") - tic_start_count - tic_idle_count ))


/******************************************************************************
*
* MACRO:
*          ds1103_tic_total_read()
*
* SYNTAX:
*          ds1103_tic_total_read() 
*
* DESCRIPTION: Returns the total time (s) of the executiontime-measurem.
*              (with idle-time).
*
* PARAMETERS: no
*
* RETURNS: the total time (s, Float64)
*
* NOTE:
*
******************************************************************************/

#define ds1103_tic_total_read() (DS1103_TIME_PER_TICK * (Float64) ( asm("mftb 3") - tic_start_count ))


/******************************************************************************
*
* MACRO:
*         ds1103_tic_delay(duration)
*
* SYNTAX:
*         ds1103_tic_delay(duration) 
*
* DESCRIPTION: This function can be used for delays. The delay is defined
*              by duration (s).
*
* PARAMETERS: Float64 duration: delay-time in s
*
* RETURNS: nothing
*
* NOTE: 
*
******************************************************************************/

#define ds1103_tic_delay(duration) {  \
  rtlib_tic_t t0;    \
  rtlib_tic_t timebase;      \
  t0 = asm("mftb 3");   \
  timebase = t0;        \
  while( ( (Float64)( timebase - t0) * DS1103_TIME_PER_TICK ) < duration)    \
  {                           \
    timebase = asm("mftb 3");          \
  };                  \
}



/******************************************************************************
*
* MACRO:
*              ds1103_tic_count()
*
* SYNTAX:
*              ds1103_tic_count()
*
* DESCRIPTION: Reads the actual counter value of the time base.
*
* PARAMETERS:  no
*
* RETURNS:     the counter value of the time base (rtlib_tic_t)
*
* NOTE:
*
******************************************************************************/

#define ds1103_tic_count()    ((rtlib_tic_t) asm("mftb 3"))



/******************************************************************************
*
* MACRO:
*              ds1103_tic_elapsed(tmr_cnt)
*
*
* SYNTAX:
*              ds1103_tic_elapsed(tmr_cnt)
*
*
* DESCRIPTION: Returns the time difference between the previous time base
*              counter value 'tmr_cnt' and the actual time base value in
*              seconds.
*
* PARAMETERS:  tmr_cnt: a time base counter value, read by ds1103_tic_count(), (rtlib_tic_t)
*
* RETURNS:     the time difference in seconds (Float64)
*
* NOTE:
*
******************************************************************************/

 #define ds1103_tic_elapsed(tmr_cnt) (DS1103_TIME_PER_TICK * (Float64) ( (rtlib_tic_t) asm("mftb 3") - (rtlib_tic_t) tmr_cnt))



/******************************************************************************
*
* MACRO:
*              ds1103_tic_diff(tmr_cnt1, tmr_cnt2)
*
*
* SYNTAX:
*              ds1103_tic_diff()
*
*
* DESCRIPTION: Returns the time difference in seconds between the two time base
*              counter values tmr_cnt1 and tmr_cnt2.
*
* PARAMETERS:  tmr_cnt1: time base counter value, read by ds1103_tic_count(), (rtlib_tic_t)
*              tmr_cnt2: time base counter value, read by ds1103_tic_count(), (rtlib_tic_t)
*
* RETURNS:     the time difference in seconds (Float64)
*
* NOTE:
*
******************************************************************************/

 #define ds1103_tic_diff(tmr_cnt1, tmr_cnt2) (DS1103_TIME_PER_TICK * (Float64) ((rtlib_tic_t) tmr_cnt2 - (rtlib_tic_t) tmr_cnt1) )



/******************************************************************************
  object declarations
******************************************************************************/

extern rtlib_tic_t tic_start_count;                          /* decl. in Tic1103.c */
extern rtlib_tic_t tic_halt_count;                           /* decl. in Tic1103.c */
extern rtlib_tic_t tic_idle_count;                           /* decl. in Tic1103.c */



#ifdef _INLINE
 #include <Tic1103.c>
#endif

#endif /*__Tic1103__*/

