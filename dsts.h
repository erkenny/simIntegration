/*******************************************************************************
*
* MODULE
*
* FILE
*   dsts.h
*
* RELATED FILES
*   dsts.c
*
* DESCRIPTION
*
* REMARKS
*
* AUTHOR(S)
*   T. Woelfer
*   R. Leinfellner
*   V. Lang
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsts.h $ $Revision: 1.6 $ $Date: 2007/06/06 10:01:57GMT+01:00 $
*******************************************************************************/

#ifndef __dsts_h__
#define __dsts_h__

#include <dstypes.h>

#ifdef _DS1005
#  include <ts1005.h>
#  ifdef _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#elif defined (_DS1006)
#  include <ts1006.h>
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#elif defined _DS1103
#  include <ts1103.h>
#  ifdef _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#elif defined _DS1104
#  include <ts1104.h>
#  ifdef _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#elif defined _DS1401
#  include <ts1401.h>
#  ifdef _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#elif defined _DS1003
#  include <ts1003.h>
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#elif defined _DS1102
#  include <ts1102.h>
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#elif defined _DS1004
#  include <ts1004.h>
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#elif defined _RP565
#  include <DS1602TS.h>
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#elif defined _RP5554
#  include <DS1603TS.h>
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#else
#  error dsts.h: no processor board defined (e.g. _DS1003)
#endif

/*******************************************************************************
  constant, macro, and type definitions
*******************************************************************************/

#define TS_INIT_DONE   0
#define TS_INIT_FAILED 1

#define TS_COMPARE_LT  0
#define TS_COMPARE_LE  1
#define TS_COMPARE_EQ  2
#define TS_COMPARE_GE  3
#define TS_COMPARE_GT  4

#define TS_MODE_SINGLE        0
#define TS_MODE_MULTI_MASTER  1
#define TS_MODE_MULTI_SLAVE   2

typedef struct
{
  UInt32  mat;
  UInt32  mit;
} ts_timestamp_type;

typedef ts_timestamp_type *    ts_timestamp_ptr_type;

/*******************************************************************************
  function prototypes
*******************************************************************************/

         void    ts_init (int mode, dsfloat mat_period);

         dsfloat ts_time_read (void);

         void    ts_timestamp_read (ts_timestamp_ptr_type ts);
         void    ts_timestamp_read_fast (ts_timestamp_ptr_type ts);

__INLINE void    ts_sync (void);

__INLINE int     ts_timestamp_compare (ts_timestamp_ptr_type ts1,
                                       ts_timestamp_ptr_type ts2,
                                       int op);

__INLINE void    ts_reset (void);

__INLINE dsfloat ts_timestamp_interval (ts_timestamp_ptr_type ts1,
                                        ts_timestamp_ptr_type ts2);

__INLINE void    ts_time_offset (dsfloat               reference_time,
                                 ts_timestamp_ptr_type ts1,
                                 ts_timestamp_ptr_type ts2,
                                 ts_timestamp_ptr_type ts_ta);

__INLINE void    ts_timestamp_offset (ts_timestamp_ptr_type ts_reference,
                                      ts_timestamp_ptr_type ts1,
                                      ts_timestamp_ptr_type ts2,
                                      ts_timestamp_ptr_type ts_ta);

__INLINE void    ts_timestamp_add (ts_timestamp_ptr_type ts1,
                                   ts_timestamp_ptr_type ts2,
                                   ts_timestamp_ptr_type ts_tsum);

__INLINE dsfloat ts_time_calculate (ts_timestamp_ptr_type ts);

__INLINE void    ts_timestamp_calculate (dsfloat               time, 
                                         ts_timestamp_ptr_type ts);

#undef __INLINE

/*******************************************************************************
  inclusion of source file(s) for inline expansion
*******************************************************************************/

#ifdef _INLINE
#  include <dsts.c>
#endif

#endif /* __dsts_h__ */

