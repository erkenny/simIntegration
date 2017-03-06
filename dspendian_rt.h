/*
 *  dspendian_rt.h
 *
 *  Copyright 1995-2004 The MathWorks, Inc.
 *  $Revision: 1.2.4.4 $ $Date: 2004/12/26 21:02:10 $
 */
#ifndef dspendian_rt_h
#define dspendian_rt_h

#include "dsp_rt.h"

#ifdef DSPENDIAN_EXPORTS
#define DSPENDIAN_EXPORT EXPORT_FCN
#else
#define DSPENDIAN_EXPORT extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

DSPENDIAN_EXPORT int_T isLittleEndian(void);

#ifdef __cplusplus
}
#endif

#endif  /* dspendian_rt_h */

/* [EOF] dspendian_rt.h */
