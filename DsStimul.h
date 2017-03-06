/******************************************************************************
  FILE:
    dsstimul.h

  DESCRIPTION:
    Stimulus Engine for real-time systems

  REMARKS:

  AUTHOR(S): A. Trepel

  Copyright (c) 1999 dSPACE GmbH, GERMANY

  $Workfile: DsStimul.h $ $Revision: 1.5 $ $Date: 2005/06/21 17:17:23GMT+01:00 $
  $Archive: /sw/python/dSPACE_Extensions/sdmlib/src/DsStimul.h $
******************************************************************************/
#ifndef _TA_STIMULUS_ENGINE_
#define _TA_STIMULUS_ENGINE_

/*---------------------------------------------------------------------------*/
/* TA Stimulus Engine export interface                                       */
/*                                                                           */               
/* This export interface is used by RTI or may be used in handcoded programs,*/
/* to implement the TA Stimulus Engine in real-time applications.            */
/*---------------------------------------------------------------------------*/

void ta_stimulus_engine_init();
void ta_stimulus_engine_service();
void ta_stimulus_engine_terminate();

/*---------------------------------------------------------------------------*/
/* ANYTHING BEHIND THIS LINE IS ONLY FOR INTERNAL USE                        */
/*---------------------------------------------------------------------------*/

#if defined(_MSC_VER) && !defined(MATLAB_MEX_FILE)
  #define _DSHOST                       /* host evaluation with Python module*/
#endif

#include "dstypes.h"

#if !defined(_DS1102) && !defined(MATLAB_MEX_FILE) && !defined(_DSHOST)
  #include <dsvcm.h>           /* version and config section management module */
  #include <dsstd.h>           /* Standard RTLib Macros and Functions        */
#endif

#if _INLINE
  #define __TA_SE_INLINE            static inline /* never used              */
#else
  #define __TA_SE_INLINE
#endif

/*---------------------------------------------------------------------------*/
/* TA Stimulus Engine specific definitions                                   */
/*---------------------------------------------------------------------------*/

#define TA_SE_TABLE_SIZE 5000           /* instruction table size            */
#define TA_SE_RING_BUFFER_SIZE 4096     /* size of buffers for data replay 
                                        1ms * 4096 = 4 seconds data buffer   */
#define RING_BUFFER_LOAD    0
#define RING_BUFFER_REFRESH 1
#define RING_BUFFER_FULL    2
#define RING_BUFFER_WORK    3

#define TA_SE_BUFFER_ERR_FILL  -2
#define TA_SE_BUFFER_ERR_NO    -1

#define TA_SE_STOP  0
#define TA_SE_RUN   1
#define TA_SE_PAUSE 2

/*---------------------------------------------------------------------------*/
/* TA Stimulus Engine specific data type definitions                         */
/*---------------------------------------------------------------------------*/
#if defined _DSHOST || defined _DS1102 || defined _DS1003 ||\
    defined _DS1103 || defined _DS1005 || defined _DS1401 ||\
    defined _DS1104 || defined MATLAB_MEX_FILE || defined _DS1006 /* TBD */
  typedef Float32  ta_rtp_flt_tp;
  typedef Int32    ta_rtp_int_tp;
  typedef UInt32   ta_rtp_uint_tp;

#elif defined _DS1004 
  typedef Float64  ta_rtp_flt_tp;
  typedef Int64    ta_rtp_int_tp;
  typedef UInt64   ta_rtp_uint_tp;

#else
  #error target platform undefined
#endif

/* ta_instr_table_item: elmement of the instruction table                    */
/* sizeof the union: DSP: 32-bit                                             */
/*                   PPC: 32-bit, (pointer uses 32-bit)                      */
/*                   ALPHA: 64-bit, (pointer uses always 64-bit)             */

typedef union
{                                       /* entry can be                      */
  ta_rtp_flt_tp   f;                    /* ta_rtp_flt_tp                     */
  ta_rtp_int_tp   l;                    /* ta_rtp_int_tp                     */
  void           *p;                    /* pointer                           */
} ta_instr_table_item;


/* ta_item_ptr_tp: script entry pointer                                      */

typedef ta_instr_table_item *ta_item_ptr_tp;

/* t_ta_se_target_var: target variable type                                  */

typedef union
{                                       /* entry can be                      */
#if defined _DS1102 || defined _DS1003 ||\
    defined _DSHOST  || defined MATLAB_MEX_FILE
  Float32   f;                          /* 32-bit single precision floating 
                                        point type in TMS320 format          */
#elif defined _DS1004 || defined _DS1103 || defined _DS1005 || defined _DS1401 || defined _DS1104 || defined _DS1006 /* TBD */
  Float64   f;                          /* 64-bit double precision floating
                                        point type in IEEE 754 format        */
#endif

#if defined _DS1004 || defined _DS1103 || defined _DS1005 ||\
    defined _DS1401 || defined _DS1104 || defined _DSHOST || defined _DS1006 /* TBD */
  Int8      i8;                         /* 8-bit signed char                 */
  Int8Ptr   i8p;                        /* 8-bit signed char                 */
  UInt8     ui8;                        /* 8-bit unsigned char               */
  Int16     i16;                        /* 16-bit signed short               */
  UInt16    ui16;                       /* 16-bit unsigned short             */
#endif

  Int32     i32;                        /* 32-bit signed integer             */
  UInt32    ui32;                       /* 32-bit unsigned integer           */
} ta_target_var_tp;


/* ta_hdr_ptr_tp: instruction table header                                   */

typedef struct
{
  ta_rtp_int_tp     control;            /* start/stop control                */
  ta_rtp_flt_tp     tframe;             /* frame time                        */
  ta_rtp_int_tp     start;              /* start index of script             */
  ta_rtp_int_tp     nsig;               /* number of function generators     */
  ta_item_ptr_tp    comb_start;         /* start index of compare entries    */
  ta_rtp_int_tp     nTable;             /* number of tables                  */
} *ta_hdr_ptr_tp;


/* ta_comp_ptr_tp: structure for combine instructions                        */

typedef struct
{
  ta_rtp_int_tp         cmd;
  ta_rtp_flt_tp         limit;
  ta_rtp_int_tp         addr;
} *ta_comp_ptr_tp;


/* funct: function pointer                                                   */

typedef void (*funct)();               /* pointer to void function           */


/* GetParaElement: list elements for get signal values                       */

typedef struct
{
  ta_rtp_int_tp         CmdIdx; 
  funct                 CmdType;
  ta_rtp_int_tp         SourceSignal;   /* offset to signal value which should
                                        be used as parameter                 */
  ta_target_var_tp     *SourceVar;
} *GetParaElement;


/* ta_comb_element_tp: list elements for combine                             */

typedef struct comb_element_struct ta_comb_element_tp; /* Vorabdeklaration   */


/* signal: signal structure                                                  */

typedef struct
 {
  union                                 /* target variable selector          */
   {
    ta_target_var_tp   *d;              /* direct pointer to target          */
    ta_target_var_tp   **i;             /* indirect pointer to target        */
   }                   target;
  ta_rtp_int_tp        idx;             /* index for indirect targets        */
  ta_rtp_int_tp        address_type;    /* 0: direct, 1: indirect address    */
  ta_target_var_tp     *target_var;     /* target variable address           */
  funct                type;            /* pointer to signal shape generator */
  ta_instr_table_item  arg1;            /* signal argument 1                 */
  ta_instr_table_item  arg2;            /* signal argument 2                 */
  ta_instr_table_item  arg3;            /* signal argument 3                 */
  ta_instr_table_item  arg4;            /* signal argument 4                 */
  ta_rtp_int_tp        flag;            /* signal flag                       */
  ta_rtp_int_tp        target_type;     /* type of target variable           */
  ta_comb_element_tp   *pcomb;          /* combine element pointer (add,mult)*/
  ta_rtp_int_tp        NumGetPara;      /* number of get parameter elements  */
  GetParaElement       GetPara;         /* pointer to get parameter elements */
} *ta_signal_ptr_tp;


/* list elements for arithmetic (add and multiply) operations                */

struct comb_element_struct
{
  funct                 type;
  ta_signal_ptr_tp      comb_signal;
  ta_target_var_tp      value;
  ta_comb_element_tp    *next;
};


/* structs for data replay (ring) buffer                                     */

typedef struct
{
  Int32   iRead;                        /* read index, set by RT processor   */
  Int32   iWrite;                       /* write index, set by host processor*/
  Int32   iState;                       /* state of the ring buffer          */
  Int32   piDeltaT[TA_SE_RING_BUFFER_SIZE]; /* sampling periods between 
                                            interpolation points             */
  Float32 pfValues[TA_SE_RING_BUFFER_SIZE]; /* interpolation points          */
} ta_data_buffer_tp;

/*---------------------------------------------------------------------------*/
/* globales variables not defined as static                                  */
/*---------------------------------------------------------------------------*/

extern ta_item_ptr_tp      ta_se_instr_table;
extern ta_data_buffer_tp  *ta_se_buffer; /* pointer to memory for data replay*/
extern ta_hdr_ptr_tp       ta_se_header; /* pointer to script header         */
extern ta_signal_ptr_tp    ta_se_sig_1;  /* pointer to first signal          */
extern ta_rtp_int_tp       ta_se_num_sig;/* number of signals                */
extern ta_rtp_int_tp       ta_se_item_size; /* size of the union 
                                            ta_instr_table_item              */

#undef __TA_SE_INLINE
#endif /* _TA_STIMULUS_ENGINE_ */

