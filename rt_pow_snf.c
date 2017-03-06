/*
 * rt_pow_snf.c
 *
 * Real-Time Workshop code generation for Simulink model "SD_jf_median.mdl".
 *
 * Model Version              : 1.154
 * Real-Time Workshop version : 7.3  (R2009a)  15-Jan-2009
 * C source code generated on : Tue Feb 28 20:02:00 2017
 *
 * Target selection: rti1103.tlc
 *   Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Generic->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 *
 */

#include "rt_pow_snf.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function: rt_pow_snf =======================================================
 * Abstract:
 *   Calls double-precision version of POW, with guard against domain error
 *   and guards against non-finites
 */
real_T rt_pow_snf(const real_T xr, const real_T yr)
{
  real_T ret, axr, ayr;
  if (rtIsNaN(xr) || rtIsNaN(yr)) {
    ret = (rtNaN);
  } else {
    axr = fabs(xr);
    ayr = fabs(yr);
    if (rtIsInf(ayr)) {
      if (axr == 1.0) {
        ret = (rtNaN);
      } else if (axr > 1.0) {
        if (yr > 0.0) {
          ret = (rtInf);
        } else {
          ret = 0.0;
        }
      } else {
        if (yr > 0.0) {
          ret = 0.0;
        } else {
          ret = (rtInf);
        }
      }
    } else {
      if (ayr == 0.0) {
        ret = 1.0;
      } else if (ayr == 1.0) {
        if (yr > 0.0) {
          ret = xr;
        } else {
          ret = 1.0/xr;
        }
      } else if (yr == 2.0) {
        ret = xr*xr;
      } else if (yr == 0.5 && xr >= 0.0) {
        ret = sqrt(xr);
      } else if ((xr < 0.0) && (yr > floor(yr))) {
        ret = (rtNaN);
      } else {
        ret = pow(xr,yr);
      }
    }
  }

  return ret;
}                                      /* end rt_pow_snf */
