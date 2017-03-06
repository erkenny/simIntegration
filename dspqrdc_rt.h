/*
 *  dspqrdc_rt.h - DSP Blockset QR Factorization Run Time Functions
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 *  $Revision: 1.6.4.7 $ $Date: 2008/04/11 15:36:08 $
 */

#ifndef dspqrdc_rt_h
#define dspqrdc_rt_h

#include "dsp_rt.h"

#ifdef DSPQRDC_EXPORTS
#define DSPQRDC_EXPORT EXPORT_FCN
#else
#define DSPQRDC_EXPORT MWDSP_IDECL
#endif

/* Do not inline functions that call other runtime functions */
#ifdef MWDSP_INLINE_DSPRTLIB
#define DSPQRDCNOINLINE_EXPORT extern
#else
#define DSPQRDCNOINLINE_EXPORT DSPQRDC_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Run-time functions for QR Factorization block */
DSPQRDCNOINLINE_EXPORT void MWDSP_QRE_D(int_T m, int_T n, real_T *q, real_T *r,
                                real_T *e, real_T *qraux, real_T *work,
                                int_T *jpvt, real_T *s, boolean_T economy);
DSPQRDCNOINLINE_EXPORT void MWDSP_QRE_R(int_T m, int_T n, real32_T *q, real32_T *r,
                                real32_T *e, real32_T *qraux, real32_T *work,
                                int_T *jpvt, real32_T *s, boolean_T economy);
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_QRE_C(int_T m, int_T n, creal32_T *q, creal32_T *r,
                                real32_T *e, creal32_T *qraux, creal32_T *work,
                                int_T *jpvt, creal32_T *s, boolean_T economy);
#endif /* CREAL_T */
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_QRE_Z(int_T m, int_T n, creal_T *q, creal_T *r,
                                real_T *e, creal_T *qraux, creal_T *work,
                                int_T *jpvt, creal_T *s, boolean_T economy);
#endif /* CREAL_T */

/* The following are the run-time functions used in QR Solver block */
DSPQRDCNOINLINE_EXPORT void MWDSP_qreslvD(int_T m, int_T n, int_T p, real_T *qr,
                                  real_T *bx, real_T *qraux, int_T *jpvt,
                                  real_T *x, real_T eps);
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_qreslvZ(int_T m, int_T n, int_T p, creal_T *qr,
                                  creal_T *bx, creal_T *qraux,int_T *jpvt,
                                  creal_T *x, real_T eps);
#endif /* CREAL_T */
DSPQRDCNOINLINE_EXPORT void MWDSP_qreslvR(int_T m, int_T n, int_T p, real32_T *qr,
                                  real32_T *bx, real32_T *qraux, int_T *jpvt,
                                  real32_T *x,real32_T eps);
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_qreslvC(int_T m, int_T n, int_T p, creal32_T *qr,
                                  creal32_T *bx, creal32_T *qraux, int_T *jpvt,
                                  creal32_T *x, real32_T eps);
#endif /* CREAL_T */
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_qreslvMixdZ(int_T m, int_T n, int_T p, real_T *qr,
                                      creal_T *bx, real_T *qraux, int_T *jpvt,
                                      creal_T *x, real_T eps);
#endif /* CREAL_T */
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_qreslvMixdC(int_T m, int_T n, int_T p, real32_T *qr,
                                      creal32_T *bx, real32_T *qraux,
                                      int_T *jpvt, creal32_T *x, real32_T eps);
#endif /* CREAL_T */


/* Common run-time functions used by both QR Factorization and QR Solver */

/*
 * dspcompqy - compute q*y or q'*y in place over y
 * Datatype - D -> Double precision real
 *            R -> Single precision real
 *            Z -> Double precision complex
 *            C -> Single precision complex
 *        MixdZ -> Both real and complex Double precision variables used.
 *        MixdC -> Both real and complex Single precision variables used.
 */
DSPQRDC_EXPORT void MWDSP_qrCompqyD(int_T n, int_T j, real_T *qr,
                                    real_T *qrauxj, real_T *y);
DSPQRDC_EXPORT void MWDSP_qrCompqyR(int_T n, int_T j, real32_T *qr,
                                    real32_T *qrauxj, real32_T *y);
#ifdef CREAL_T
DSPQRDC_EXPORT void MWDSP_qrCompqyZ(int_T n, int_T j, creal_T *qr,
                                    creal_T *qrauxj, creal_T *y);
#endif /* CREAL_T */
#ifdef CREAL_T
DSPQRDC_EXPORT void MWDSP_qrCompqyC(int_T n, int_T j, creal32_T *qr,
                                    creal32_T *qrauxj, creal32_T *y);
#endif /* CREAL_T */
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_qrCompqyMixdZ(int_T n, int_T j, real_T *qr,
                                        real_T *qrauxj, creal_T *y);
#endif /* CREAL_T */
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_qrCompqyMixdC(int_T n, int_T j, real32_T *qr,
                                        real32_T *qrauxj, creal32_T *y);
#endif /* CREAL_T */

/*
 * Compute the qr factorization of an m by n matrix x.
 * Information needed for the orthogonal matrix q is
 * overwritten in the lower triangle of x and in the
 * auxilliary array qraux.
 * r overwrites the upper triangle of x and its diagonal
 * entries are guaranteed to decrease in magnitude.
 * Column pivot information is stored in jpvt.
 * Datatypes - D -> Double precision real
 *             R -> Single precision real
 *             Z -> Double precision complex
 *             C -> Single precision complex
 */
DSPQRDCNOINLINE_EXPORT void MWDSP_qrdcD(int_T m, int_T n, real_T *x,
                                real_T *qraux, int_T *jpvt, real_T *work);
DSPQRDCNOINLINE_EXPORT void MWDSP_qrdcR(int_T m, int_T n, real32_T *x,
                                real32_T *qraux, int_T *jpvt, real32_T *work);
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_qrdcC(int_T m, int_T n, creal32_T *x,
                                creal32_T *qraux, int_T *jpvt, creal32_T *work);
#endif /* CREAL_T */
#ifdef CREAL_T
DSPQRDCNOINLINE_EXPORT void MWDSP_qrdcZ(int_T m, int_T n, creal_T *x, creal_T *qraux,
                                int_T *jpvt, creal_T *work);
#endif /* CREAL_T */

#ifdef __cplusplus
}
#endif

#ifdef MWDSP_INLINE_DSPRTLIB
#   include "dspqrdc/qrcompqy_d_rt.c"
#   include "dspqrdc/qrcompqy_r_rt.c"
#ifdef CREAL_T
#   include "dspqrdc/qrcompqy_c_rt.c"
#   include "dspqrdc/qrcompqy_z_rt.c"
#   include "dspqrdc/qrcompqy_mixd_z_rt.c"
#   include "dspqrdc/qrcompqy_mixd_c_rt.c"
#endif
#endif


#endif /* dspqrdc_rt_h */
