/*
 *  dspblms_rt.h   Runtime functions for DSP Blockset BLOCK LMS Adaptive filter block.
 *
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 *  $Revision: 1.1.6.5 $  $Date: 2005/12/22 18:02:13 $
 */
#ifndef dspblms_rt_h
#define dspblms_rt_h

#include "dsp_rt.h"

#ifdef DSPBLMS_EXPORTS
#define DSPBLMS_EXPORT EXPORT_FCN
#else
#define DSPBLMS_EXPORT MWDSP_IDECL
#endif

/*
 * Function naming glossary
 * ---------------------------
 *
 * MWDSP = MathWorks DSP Blockset
 *
 * Data types - (describe inputs to functions, not outputs)
 * R = real single-precision
 * C = complex single-precision
 * D = real double-precision
 * Z = complex double-precision
 */

/* Function naming convention
 * --------------------------
 *
 * MWDSP_blms_a[y/n]_w[y/n]_<Input_Signal_DataType><Desired_Signal_DataType>
 *
 *    1) MWDSP_ is a prefix used with all Mathworks DSP runtime library
 *       functions.
 *    2) The second field indicates that this function is implementing the
 *       BLOCK LMS Adaptive filter algorithm.
 *    3) The third field a[y/n] indicates whether there is adapt input port or not
 *       'ay' means there is adapt input port, 'an' means no adapt input port
 *    4) The fourth field w[y/n] indicates whether there is weight output port or not
 *       'wy' means there is weight output port, 'wn' means no weight output port
 *    5) The third field enumerates the data type from the above list.
 *       Single/double precision and complexity are specified within a single letter.
 *       The input data type is indicated.
 *
 *    Examples:
 *       MWDSP_blms_ay_wn_ZZ is the Block LMS Adaptive filter run time function for
 *       double precision complex input signal, double precision complex desired signal
 *       with adapt input port and no weight output port
 */

#ifdef __cplusplus
extern "C" {
#endif

/* 000 */
DSPBLMS_EXPORT void MWDSP_blms_an_wn_DD(const real_T *inSigU,
                                const real_T *deSigU,
                                const real_T  muU,
                                real_T       *inBuff,
                                real_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real_T  LkgFactor,
                                const int_T   FrmLen,
                                real_T       *outY,
                                real_T       *errY);
/* 001 */
#ifdef CREAL_T
DSPBLMS_EXPORT void MWDSP_blms_an_wn_ZZ(const creal_T *inSigU,
                                const creal_T *deSigU,
                                const real_T  muU,
                                creal_T       *inBuff,
                                creal_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real_T  LkgFactor,
                                const int_T   FrmLen,
                                creal_T       *outY,
                                creal_T       *errY);
#endif /* CREAL_T */

/* 002 */
DSPBLMS_EXPORT void MWDSP_blms_an_wn_RR(const real32_T *inSigU,
                                const real32_T *deSigU,
                                const real32_T  muU,
                                real32_T       *inBuff,
                                real32_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real32_T  LkgFactor,
                                const int_T   FrmLen,
                                real32_T       *outY,
                                real32_T       *errY);
/* 003 */
#ifdef CREAL_T
DSPBLMS_EXPORT void MWDSP_blms_an_wn_CC(const creal32_T *inSigU,
                                const creal32_T *deSigU,
                                const real32_T  muU,
                                creal32_T       *inBuff,
                                creal32_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real32_T  LkgFactor,
                                const int_T   FrmLen,
                                creal32_T       *outY,
                                creal32_T       *errY);
#endif /* CREAL_T */

/* 004 */
DSPBLMS_EXPORT void MWDSP_blms_an_wy_DD(const real_T *inSigU,
                                const real_T *deSigU,
                                const real_T  muU,
                                real_T       *inBuff,
                                real_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real_T  LkgFactor,
                                const int_T   FrmLen,
                                real_T       *outY,
                                real_T       *errY,
                                real_T       *wgtY);
/* 005 */
#ifdef CREAL_T
DSPBLMS_EXPORT void MWDSP_blms_an_wy_ZZ(const creal_T *inSigU,
                                const creal_T *deSigU,
                                const real_T  muU,
                                creal_T       *inBuff,
                                creal_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real_T  LkgFactor,
                                const int_T   FrmLen,
                                creal_T       *outY,
                                creal_T       *errY,
                                creal_T       *wgtY);
#endif /* CREAL_T */

/* 006 */
DSPBLMS_EXPORT void MWDSP_blms_an_wy_RR(const real32_T *inSigU,
                                const real32_T *deSigU,
                                const real32_T  muU,
                                real32_T       *inBuff,
                                real32_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real32_T  LkgFactor,
                                const int_T   FrmLen,
                                real32_T       *outY,
                                real32_T       *errY,
                                real32_T       *wgtY);
/* 007 */
#ifdef CREAL_T
DSPBLMS_EXPORT void MWDSP_blms_an_wy_CC(const creal32_T *inSigU,
                                const creal32_T *deSigU,
                                const real32_T  muU,
                                creal32_T       *inBuff,
                                creal32_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real32_T  LkgFactor,
                                const int_T   FrmLen,
                                creal32_T       *outY,
                                creal32_T       *errY,
                                creal32_T       *wgtY);
#endif /* CREAL_T */

/* 008 */
DSPBLMS_EXPORT void MWDSP_blms_ay_wn_DD(const real_T *inSigU,
                                const real_T *deSigU,
                                const real_T  muU,
                                real_T       *inBuff,
                                real_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real_T  LkgFactor,
                                const int_T   FrmLen,
                                real_T       *outY,
                                real_T       *errY,
                                const boolean_T     NeedAdapt);
/* 009 */
#ifdef CREAL_T
DSPBLMS_EXPORT void MWDSP_blms_ay_wn_ZZ(const creal_T *inSigU,
                                const creal_T *deSigU,
                                const real_T  muU,
                                creal_T       *inBuff,
                                creal_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real_T  LkgFactor,
                                const int_T   FrmLen,
                                creal_T       *outY,
                                creal_T       *errY,
                                const boolean_T     NeedAdapt);
#endif /* CREAL_T */

/* 010 */
DSPBLMS_EXPORT void MWDSP_blms_ay_wn_RR(const real32_T *inSigU,
                                const real32_T *deSigU,
                                const real32_T  muU,
                                real32_T       *inBuff,
                                real32_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real32_T  LkgFactor,
                                const int_T   FrmLen,
                                real32_T       *outY,
                                real32_T       *errY,
                                const boolean_T     NeedAdapt);
/* 011 */
#ifdef CREAL_T
DSPBLMS_EXPORT void MWDSP_blms_ay_wn_CC(const creal32_T *inSigU,
                                const creal32_T *deSigU,
                                const real32_T  muU,
                                creal32_T       *inBuff,
                                creal32_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real32_T  LkgFactor,
                                const int_T   FrmLen,
                                creal32_T       *outY,
                                creal32_T       *errY,
                                const boolean_T     NeedAdapt);
#endif /* CREAL_T */

/* 012 */
DSPBLMS_EXPORT void MWDSP_blms_ay_wy_DD(const real_T *inSigU,
                                const real_T *deSigU,
                                const real_T  muU,
                                real_T       *inBuff,
                                real_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real_T  LkgFactor,
                                const int_T   FrmLen,
                                real_T       *outY,
                                real_T       *errY,
                                real_T       *wgtY,
                                const boolean_T     NeedAdapt);
/* 013 */
#ifdef CREAL_T
DSPBLMS_EXPORT void MWDSP_blms_ay_wy_ZZ(const creal_T *inSigU,
                                const creal_T *deSigU,
                                const real_T  muU,
                                creal_T       *inBuff,
                                creal_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real_T  LkgFactor,
                                const int_T   FrmLen,
                                creal_T       *outY,
                                creal_T       *errY,
                                creal_T       *wgtY,
                                const boolean_T     NeedAdapt);
#endif /* CREAL_T */

/* 014 */
DSPBLMS_EXPORT void MWDSP_blms_ay_wy_RR(const real32_T *inSigU,
                                const real32_T *deSigU,
                                const real32_T  muU,
                                real32_T       *inBuff,
                                real32_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real32_T  LkgFactor,
                                const int_T   FrmLen,
                                real32_T       *outY,
                                real32_T       *errY,
                                real32_T       *wgtY,
                                const boolean_T     NeedAdapt);
/* 015 */
#ifdef CREAL_T
DSPBLMS_EXPORT void MWDSP_blms_ay_wy_CC(const creal32_T *inSigU,
                                const creal32_T *deSigU,
                                const real32_T  muU,
                                creal32_T       *inBuff,
                                creal32_T       *wgtBuff,
                                const int_T   FilterLength,
                                const int_T   BlockLength,
                                const real32_T  LkgFactor,
                                const int_T   FrmLen,
                                creal32_T       *outY,
                                creal32_T       *errY,
                                creal32_T       *wgtY,
                                const boolean_T     NeedAdapt);
#endif /* CREAL_T */

#ifdef __cplusplus
}
#endif

#ifdef MWDSP_INLINE_DSPRTLIB
/* BLOCK LMS ONLY */
# include "dspblms/blms_an_wn_dd_rt.c"
# include "dspblms/blms_an_wn_rr_rt.c"
# include "dspblms/blms_an_wy_dd_rt.c"
# include "dspblms/blms_an_wy_rr_rt.c"
# include "dspblms/blms_ay_wn_dd_rt.c"
# include "dspblms/blms_ay_wn_rr_rt.c"
# include "dspblms/blms_ay_wy_dd_rt.c"
# include "dspblms/blms_ay_wy_rr_rt.c"
# ifdef CREAL_T
#  include "dspblms/blms_an_wn_cc_rt.c"
#  include "dspblms/blms_an_wn_zz_rt.c"
#  include "dspblms/blms_an_wy_cc_rt.c"
#  include "dspblms/blms_an_wy_zz_rt.c"
#  include "dspblms/blms_ay_wn_cc_rt.c"
#  include "dspblms/blms_ay_wn_zz_rt.c"
#  include "dspblms/blms_ay_wy_cc_rt.c"
#  include "dspblms/blms_ay_wy_zz_rt.c"
# endif /* CREAL_T */
#endif /* MWDSP_INLINE_DSPRTLIB */

#endif /*  dspblms_rt_h */
