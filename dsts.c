/******************************************************************************
*
* MODULE
*   Timestamp generation functions
*
* FILE
*   dsts.c
*
* RELATED FILES
*   dsts.h, tsxxxx.h
*
* DESCRIPTION
*
* REMARKS
*
* AUTHOR(S)
*   T. Woelfer
*   R. Leinfellner
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsts.c $ $Revision: 1.7 $ $Date: 2007/06/06 10:07:30GMT+01:00 $
******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <dsts.h>
#include <dsmsg.h>
#include <dsstd.h>
#include <dsvcm.h>

/* ------------------------------------------------------------------------- */

/* $DSVERSION: TS - TimeStamp Module */

#define TS_VER_MAR 1
#define TS_VER_MIR 4
#define TS_VER_MAI 1
#define TS_VER_SPB VCM_VERSION_RELEASE
#define TS_VER_SPN 0
#define TS_VER_PLV 0

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 ||defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401 || defined _RP565 || defined _RP5554

DS_VERSION_SYMBOL(_ts_ver_mar, TS_VER_MAR);
DS_VERSION_SYMBOL(_ts_ver_mir, TS_VER_MIR);
DS_VERSION_SYMBOL(_ts_ver_mai, TS_VER_MAI);
DS_VERSION_SYMBOL(_ts_ver_spb, TS_VER_SPB);
DS_VERSION_SYMBOL(_ts_ver_spn, TS_VER_SPN);
DS_VERSION_SYMBOL(_ts_ver_plv, TS_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/*******************************************************************************
  constant, macro, and type definitions
*******************************************************************************/

typedef struct
{
  int       mode;
  UInt32    mit_per_mat;
  dsfloat   mat_period;
  dsfloat   mit_period;
  dsfloat   host_period;
  UInt32    host_ticks;
} ts_host_parameter_type;

/*******************************************************************************
  object declarations
*******************************************************************************/

#ifndef _INLINE

int                               dsts_mode;
dsfloat                           dsts_mit_period;
dsfloat                           dsts_mat_period;
UInt32                            dsts_mit_per_mat;
UInt32                            dsts_clock_mat;
dsfloat                           dsts_host_period;
ts_host_parameter_type            *dsts_host_parameter_ptr;
UInt32                            dsts_last_mit;

#ifdef VCM_AVAILABLE
vcm_module_descriptor_type        *dsts_vcm_rtlib_ptr;
vcm_module_descriptor_type        *dsts_vcm_module_ptr;
#endif

#ifdef _DS1006 
unsigned long long                dsts_offset;
#endif

#else

extern int                        dsts_mode;
extern dsfloat                    dsts_mit_period;
extern dsfloat                    dsts_mat_period;
extern UInt32                     dsts_mit_per_mat;
extern UInt32                     dsts_clock_mat;
extern dsfloat                    dsts_host_period;
extern ts_host_parameter_type     *dsts_host_parameter_ptr;
extern UInt32                     dsts_last_mit;

#ifdef VCM_AVAILABLE
extern vcm_module_descriptor_type *dsts_vcm_rtlib_ptr;
extern vcm_module_descriptor_type *dsts_vcm_module_ptr;
#endif

#ifdef _DS1006 
extern unsigned long long         dsts_offset;
#endif

#endif /* _INLINE */

/*******************************************************************************
  function declarations
*******************************************************************************/

#if defined _DS1003 || defined _DS1004 || defined _DS1102
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#elif defined _DS1005 || defined _DS1103 || defined _DS1401 || defined _DS1104 || defined _RP565 || defined _RP5554
#  ifdef _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#elif defined _DS1006 
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#else
#  error dsts.c: no processor board defined (e.g. _DS1003)
#endif

#ifndef _INLINE

/*******************************************************************************
*
* FUNCTION
*   Initialization function
*
* SYNTAX
*   int ts_init (int mode, dsfloat mat_period)
*
* DESCRIPTION
*   Initialize the timestamping module. The initialization is carried out the
*   first time the function is called only. The return value is one of the
*   following constants:
*     TS_INIT_DONE     the module has been initialized
*     TS_INIT_FAILED   the module was already initialized, nothing is done
*
* PARAMETERS
*   mode               mode of the time-stamping module:
*                        TS_MODE_SINGLE          single-processor system
*                        TS_MODE_MULTI_MASTER    multi-processor system (master)
*                        TS_MODE_MULTI_SLAVE     multi-processor system (slave)
*
*   mat_period         time for one macrotick period in seconds
*
* RETURNS
*   an integer value which flags if the initialization has been carried out
*
* REMARKS
*
*******************************************************************************/

void ts_init(int mode, dsfloat mat_period)
{
  static int init_once = 0;

  if (!init_once)
  {
    /* store some informations for the host */
#ifdef VCM_AVAILABLE
    dsts_vcm_rtlib_ptr = vcm_module_find(VCM_MID_RTLIB, NULL);

    dsts_vcm_module_ptr = vcm_module_register(VCM_MID_TIMESTAMP,
                                            dsts_vcm_rtlib_ptr,
                                            VCM_TXT_TIMESTAMP,
                                            TS_VER_MAR, TS_VER_MIR, TS_VER_MAI,
                                            TS_VER_SPB, TS_VER_SPN, TS_VER_PLV,
                                            0, 0);

    dsts_host_parameter_ptr = (ts_host_parameter_type *) vcm_cfg_malloc(sizeof(ts_host_parameter_type));

    vcm_memory_ptr_set(dsts_vcm_module_ptr,
                       (vcm_cfg_mem_ptr_type)dsts_host_parameter_ptr,
                       sizeof(ts_host_parameter_type));
#else
    dsts_host_parameter_ptr = (ts_host_parameter_type *) malloc(sizeof(ts_host_parameter_type));
#endif

    if (!dsts_host_parameter_ptr)
    {
      msg_error_printf(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "ts_init(): " MSG_MEM_ALLOC_ERROR_TXT);
      exit(1);
    }

    init_once = 1;
  }

  dsts_mode = mode;

  /* STBU mode specific initialization */
  switch (mode)
  {
    case TS_MODE_SINGLE:       {
                                 /* initialize mit period */
                                 dsts_mit_period = TS_MIT_PERIODE_SINGLE;
                                 dsts_mat_period = TS_MAT_PERIODE_SINGLE;

                                 TS_TIMEBASE_INIT_SINGLE();

                                 break;
                               }
    case TS_MODE_MULTI_MASTER: {
                                 /* initialize mit period */
                                 dsts_mit_period = TS_MIT_PERIODE_MULTI;
                                 dsts_mat_period = TS_MAT_PERIODE_MULTI;

                                 TS_TIMEBASE_INIT_MULTI_MASTER();

                                 break;
                               }
    case TS_MODE_MULTI_SLAVE:  {
                                 /* initialize mit period */
                                 dsts_mit_period = TS_MIT_PERIODE_MULTI;
                                 dsts_mat_period = TS_MAT_PERIODE_MULTI;

                                 TS_TIMEBASE_INIT_MULTI_SLAVE();

                                 break;
                               }
    default:                   {
                                 msg_info_set(MSG_SM_RTLIB, 0, "ts_init(): invalid mode");
                                 exit(1);
                               }
  }

  /* initialize variables for 32 bit timer support */
  dsts_clock_mat = 0;
  dsts_last_mit = 0;


  /* initialize mit per mat ratio */
  switch (mode)
  {
  case TS_MODE_SINGLE:       {
      /*
       * subtract 1 to prevent a 32-Bit overflow
       */
      dsts_mit_per_mat = (UInt32)(TS_SINGLE_RELOAD_VALUE - 1);
      break;
                             }
  case TS_MODE_MULTI_MASTER:
  case TS_MODE_MULTI_SLAVE:  {
      dsts_mit_per_mat = (UInt32)(dsts_mat_period / dsts_mit_period);
      break;
                             }
  default:                   {
      msg_info_set(MSG_SM_RTLIB, 0, "ts_init(): invalid mode");
      exit(1);
                             }
  }

  /* initialize host parameters */
  dsts_host_parameter_ptr->mode = dsts_mode;

  dsts_host_parameter_ptr->mat_period = dsts_mat_period;
  dsts_host_parameter_ptr->mit_period = dsts_mit_period;
  dsts_host_parameter_ptr->mit_per_mat = dsts_mit_per_mat;

  dsts_host_parameter_ptr->host_period = dsts_mat_period;
  dsts_host_parameter_ptr->host_ticks = 0;

#ifdef VCM_AVAILABLE
  vcm_module_status_set(dsts_vcm_module_ptr, VCM_STATUS_INITIALIZED);
#endif

  if (mode == TS_MODE_SINGLE)
  {
    /* reset time base */
    ts_reset(); /* nur bei single */
  }
}


/*******************************************************************************
*
* FUNCTION
*   read time in seconds
*
* SYNTAX
*   dsfloat ts_time_read (void)
*
* DESCRIPTION
*
* PARAMETERS
*   none
*
* RETURNS
*   time in seconds since ts_init() or the last ts_timestamp_reset()
*
* REMARKS
*
*******************************************************************************/

dsfloat ts_time_read(void)
{
  ts_timestamp_type ts;

  ts_timestamp_read(&ts);

  return ((dsfloat)ts.mat * dsts_mat_period) + ((dsfloat)ts.mit * dsts_mit_period);
}

/*******************************************************************************
*
* FUNCTION
*   read a timestamp
*
* SYNTAX
*   void ts_timestamp_read (ts_timestamp_ptr_type ts)
*
* DESCRIPTION
*   the function calculates a timestamp from the current timestamping counter values
*
* PARAMETERS
*    ts         pointer to a timestamp data structure
*
* RETURNS
*   void
*
* REMARKS
*
*******************************************************************************/

void ts_timestamp_read(ts_timestamp_type *ts)
{
  TS_TIMEBASE_READ(ts);
}


/* The same function for environments with disabled interrupts */

void ts_timestamp_read_fast(ts_timestamp_type *ts)
{
  TS_TIMEBASE_READ_FAST(ts);
}


#endif /* !_INLINE */

/*******************************************************************************
*
* FUNCTION
*   macrotick and synchronization with slaves
*
* SYNTAX
*   void ts_sync (void)
*
* DESCRIPTION
*   updates the MAT, MIT and host_tick for MP systems, in single systems it
*   just updates the host_tick
*
* PARAMETERS
*   none
*
* RETURNS
*   void
*
* REMARKS
*
*******************************************************************************/

__INLINE void ts_sync (void)
{
  rtlib_int_status_t int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  switch (dsts_mode)
  {
    case TS_MODE_SINGLE:       break;
    case TS_MODE_MULTI_MASTER:
    case TS_MODE_MULTI_SLAVE:  {
                                 /* increment mat */
                                 dsts_clock_mat++;

                                 /* reset mit */
                                 TS_TIMEBASE_RESET_LOW();

                                 break;
                               }
  }

  dsts_host_parameter_ptr->host_ticks++;

  RTLIB_INT_RESTORE(int_status);
}

/*******************************************************************************
*
* FUNCTION
*   compare two timestamps
*
* SYNTAX
*   int ts_timestamp_compare (ts_timestamp_ptr_type ts1, ts_timestamp_ptr_type ts2, int op)
*
* DESCRIPTION
*   the function compares two timestamps with the selected operation
*
* PARAMETERS
*   ts1          pointer to the first timestamp
*   ts2          pointer to the second timestamp
*   op           compare operator
*                  TS_COMPARE_LT  -  Less Than
*                  TS_COMPARE_LE  -  Less or Equal
*                  TS_COMPARE_EQ  -  Equal
*                  TS_COMPARE_GE  -  Greater or Equal
*                  TS_COMPARE_GT  -  Greater Than
*
* RETURNS
*   0   result is true
*   !0  result is false
*
* REMARKS
*
*******************************************************************************/

__INLINE int ts_timestamp_compare (ts_timestamp_ptr_type ts1, ts_timestamp_ptr_type ts2, int op)
{
  int result;

  switch (op)
  {
    case TS_COMPARE_LT:
      result = ((ts1->mat < ts2->mat) || ((ts1->mat == ts2->mat) & (ts1->mit < ts2->mit)));
      break;
    case TS_COMPARE_LE:
      result = ((ts1->mat < ts2->mat) || ((ts1->mat == ts2->mat) & (ts1->mit <= ts2->mit)));
      break;
    case TS_COMPARE_EQ:
      result = ((ts1->mat == ts2->mat) && (ts1->mit == ts2->mit));
      break;
    case TS_COMPARE_GE:
      result = ((ts1->mat > ts2->mat) || ((ts1->mat == ts2->mat) & (ts1->mit >= ts2->mit)));
      break;
    case TS_COMPARE_GT:
      result = ((ts1->mat > ts2->mat) || ((ts1->mat == ts2->mat) & (ts1->mit > ts2->mit)));
      break;
    default:
      result = 0;
  }

  return result;
}


/*******************************************************************************
*
* FUNCTION
*   reset the timestamping counter
*
* SYNTAX
*   void ts_reset (void)
*
* DESCRIPTION
*   resets the time-stamping module to absolute time 0
*
* PARAMETERS
*   none
*
* RETURNS
*   void
*
* REMARKS
*
*******************************************************************************/

__INLINE void ts_reset (void)
{
  rtlib_int_status_t   int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  TS_TIMEBASE_RESET();
  dsts_clock_mat = 0;
  dsts_last_mit = 0;

  dsts_host_parameter_ptr->host_ticks  = 0;

  RTLIB_INT_RESTORE(int_status);
}

/*******************************************************************************
*
*  FUNCTION
*    calculate an interval
*
*  SYNTAX
*    dsfloat ts_timestamp_interval (ts_timestamp_ptr_type ts1, ts_timestamp_ptr_type ts2);
*
*  DESCRIPTION
*    calculates the interval in seconds between two timestamps
*
*  PARAMETERS
*    ts1  pointer to time stamp 1
*    ts2  pointer to time stamp 2
*
*  RETURNS
*    interval in seconds from ts1 to ts2
*
*  REMARKS
*    ts2 is assumed to be occured after ts1
*
*******************************************************************************/

__INLINE dsfloat ts_timestamp_interval (ts_timestamp_ptr_type ts1, ts_timestamp_ptr_type ts2)
{
  return ((((dsfloat)ts2->mat - (dsfloat)ts1->mat) * dsts_mat_period) +
          (((dsfloat)ts2->mit - (dsfloat)ts1->mit) * dsts_mit_period));
}


/*******************************************************************************
*
*  FUNCTION
*    add an interval to a time value
*
*  SYNTAX
*    void ts_time_offset (dsfloat time, ts_timestamp_ptr_type ts1,
*                         ts_timestamp_ptr_type ts2, ts_timestamp_ptr_type ts_ta)
*
*  DESCRIPTION
*    The function calculates the interval in seconds between time stamp 1 and
*    2 by subtracting time stamp 1 from time stamp 2 and adds the difference
*    to the given time. The absolute time is returned as a timestamp.
*
*  PARAMETERS
*    time  : base time
*    ts1   : pointer to time stamp 1
*    ts2   : pointer to time stamp 2
*    ts_ta : pointer to the time stamp holding the absolute time
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

__INLINE void ts_time_offset (dsfloat               reference_time,
                              ts_timestamp_ptr_type ts1,
                              ts_timestamp_ptr_type ts2,
                              ts_timestamp_ptr_type ts_ta)
{
  dsfloat tmpf_mit, int_part;
  UInt32  tmpi_mit, tmpi_mat, tmp_ta_mat, additional_mat;


  additional_mat = reference_time / dsts_mat_period;
  reference_time = reference_time - (additional_mat * dsts_mat_period);

  if (ts1->mit > ts2->mit)
  {
    tmp_ta_mat = ts2->mat - (ts1->mat + 1);
    tmpi_mit = ts1->mit - ts2->mit;
    if (tmpi_mit > dsts_mit_per_mat)
    {
      tmpi_mat = (tmpi_mit - 1) / dsts_mit_per_mat;
      tmp_ta_mat = tmp_ta_mat - tmpi_mat;
      tmpi_mit = tmpi_mit - (tmpi_mat * dsts_mit_per_mat);
    }
    tmpf_mit = dsts_mit_per_mat - tmpi_mit + (reference_time / dsts_mit_period);
    if (dsts_mode == TS_MODE_SINGLE)
    {
      tmpf_mit++;
    }
  }
  else
  {
    tmp_ta_mat = ts2->mat - ts1->mat;
    tmpf_mit = ts2->mit - ts1->mit + (reference_time / dsts_mit_period);
  }

#ifdef _DS1603
  if (dsmpc5554_modff(tmpf_mit, &int_part) < 0.5f)
  {
    tmpf_mit = int_part;
  }
  else
  {
    tmpf_mit = int_part + 1.0f;
  }
#else
  if (modf(tmpf_mit, &int_part) < 0.5)
  {
    tmpf_mit = int_part;
  }
  else
  {
    tmpf_mit = int_part + 1.0;
  }
#endif

  if (dsts_mode == TS_MODE_SINGLE)
  {
    if (tmpf_mit > dsts_mit_per_mat)
    {
      tmpf_mit = tmpf_mit - dsts_mit_per_mat - 1;
      additional_mat++;
    }
  }
  else
  {
    if (tmpf_mit >= dsts_mit_per_mat)
    {
      tmpf_mit = tmpf_mit - dsts_mit_per_mat;
      additional_mat++;
    }
  }

  ts_ta->mit = tmpf_mit;
  ts_ta->mat = tmp_ta_mat + additional_mat;
}


/*******************************************************************************
*
*  FUNCTION
*    add an interval to a time stamp
*
*  SYNTAX
*    void ts_timestamp_offset (ts_timestamp_ptr_type ts_reference,
*                              ts_timestamp_ptr_type ts1,
*                              ts_timestamp_ptr_type ts2,
*                              ts_timestamp_ptr_type ts_ta)
*
*  DESCRIPTION
*    The function calculates the interval in seconds between time stamp 1 and
*    2 by subtracting time stamp 1 from time stamp 2 and adds the difference
*    to the reference time stamp. The absolute time is returned as a timestamp.
*
*  PARAMETERS
*    ts_reference  : pointer to the basic time stamp stucture
*    ts1           : pointer to time stamp 1
*    ts2           : pointer to time stamp 2
*    ts_ta         : pointer to the time stamp holding the absolute time
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

__INLINE void ts_timestamp_offset (ts_timestamp_ptr_type ts_reference,
                                   ts_timestamp_ptr_type ts1,
                                   ts_timestamp_ptr_type ts2,
                                   ts_timestamp_ptr_type ts_ta)
{
  UInt32  tmp_mit, tmp_mat, mat_over;
  dsfloat  tmpf_mit;

  if (ts1->mit > ts2->mit)
  {
    tmp_mat = ts2->mat - (ts1->mat + 1);
    tmp_mit = ts1->mit - ts2->mit;
    if (tmp_mit > dsts_mit_per_mat)
    {
      mat_over = (tmp_mit - 1) / dsts_mit_per_mat;
      tmp_mat = tmp_mat - mat_over;
      tmp_mit = tmp_mit - (mat_over * dsts_mit_per_mat);
    }
    tmp_mit = dsts_mit_per_mat - tmp_mit;
    if (dsts_mode == TS_MODE_SINGLE)
    {
      tmp_mit++;
    }
  }
  else
  {
    tmp_mat = ts2->mat - ts1->mat;
    tmp_mit = ts2->mit - ts1->mit;
  }

  tmp_mat = tmp_mat + ts_reference->mat;
  tmpf_mit = (dsfloat)((dsfloat)tmp_mit + (dsfloat)ts_reference->mit);

  if (dsts_mode == TS_MODE_SINGLE)
  {
    if (tmpf_mit > dsts_mit_per_mat)
    {
      tmp_mat++;
      tmpf_mit = tmpf_mit - dsts_mit_per_mat - 1;
    }
  }
  else
  {
    if (tmpf_mit >= dsts_mit_per_mat)
    {
      mat_over = tmpf_mit / dsts_mit_per_mat;
      tmp_mat = tmp_mat + mat_over;
      tmpf_mit = tmpf_mit - (mat_over * (dsfloat)dsts_mit_per_mat);
    }
  }

  ts_ta->mit = tmpf_mit;
  ts_ta->mat = tmp_mat;
}

/*******************************************************************************
*
*  FUNCTION
*    add two timestamps
*
*  SYNTAX
*    void ts_timestamp_add(ts_timestamp_ptr_type ts1,
*                          ts_timestamp_ptr_type ts2,
*                          ts_timestamp_ptr_type ts_tsum) 
*
*  DESCRIPTION
*    The function adds two timestamps and returns the calculated time as a
*    timestamp.
*
*  PARAMETERS
*    ts1     : pointer to the first timestamp addend
*    ts2     : pointer to the second timestamp addend 
*    ts_tsum : pointer to the sum timestamp
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

__INLINE void ts_timestamp_add (ts_timestamp_ptr_type ts1,
                                ts_timestamp_ptr_type ts2,
                                ts_timestamp_ptr_type ts_tsum)
{
  UInt32 mat_over;
  dsfloat tmpf_mit;

  ts_tsum->mat = ts2->mat + ts1->mat;

  if (dsts_mode == TS_MODE_SINGLE)
  {
    ts_tsum->mit = ts2->mit + ts1->mit;
    if ((ts_tsum->mit < ts2->mit) || (ts_tsum->mit < ts1->mit))
    {
      ts_tsum->mat++;
    }
  }
  else
  {
    tmpf_mit = (dsfloat)((dsfloat)ts2->mit + (dsfloat)ts1->mit);
    if (tmpf_mit >= dsts_mit_per_mat)
    {
      mat_over = tmpf_mit / dsts_mit_per_mat;
      ts_tsum->mat = ts_tsum->mat + mat_over;
      tmpf_mit = tmpf_mit - (mat_over * (dsfloat)dsts_mit_per_mat);
    }
    ts_tsum->mit = tmpf_mit;
  }
}


/*******************************************************************************
*
*  FUNCTION
*    calculates the time in seconds from a time stamp
*
*  SYNTAX
*    dsfloat ts_time_calculate (ts_timestamp_ptr_type ts)
*
*  DESCRIPTION
*    The function calculates the time based on the time stamp ts referred to
*    the initialization ts_init() or the last ts_reset().
*
*  PARAMETERS
*    ts : pointer to a time stamp structure
*
*  RETURNS
*    the time corresponding to the time stamp
*
*  REMARKS
*
*******************************************************************************/

__INLINE dsfloat ts_time_calculate (ts_timestamp_ptr_type ts)
{
  return ((dsfloat)ts->mat * dsts_mat_period) + ((dsfloat)ts->mit * dsts_mit_period);
}


/*******************************************************************************
*
*  FUNCTION
*    calculates the time stamp from a time value
*
*  SYNTAX
*    void ts_timestamp_calculate (dsfloat time, ts_timestamp_ptr_type ts)
*
*  DESCRIPTION
*    The function calculates the time stamp based on the time in seconds
*    referred to the initialization ts_init() or the last ts_reset().
*
*  PARAMETERS
*    time : the time in seconds
*    ts   : pointer to a time stamp structure
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

__INLINE void  ts_timestamp_calculate (dsfloat time, ts_timestamp_ptr_type ts)
{
  ts->mat = time / dsts_mat_period;

  time = time - (ts->mat * dsts_mat_period);

  ts->mit = time / dsts_mit_period;
}

#undef __INLINE

