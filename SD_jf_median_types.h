/*
 * SD_jf_median_types.h
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
 */
#ifndef RTW_HEADER_SD_jf_median_types_h_
#define RTW_HEADER_SD_jf_median_types_h_
#include "rtwtypes.h"
#ifndef _CSC2_SD_JF_MEDIAN_CHARTSTRUCT
#define _CSC2_SD_JF_MEDIAN_CHARTSTRUCT
#ifndef struct_sPk8KX86oBjv2lJavBJkkIB
#define struct_sPk8KX86oBjv2lJavBJkkIB

typedef struct sPk8KX86oBjv2lJavBJkkIB {
  real_T slide[500];
  real_T run_med;
  real_T last_state;
  real_T maxFz;
  real_T mp_vec[3];
  real_T stride;
  real_T len;
  real_T stime[3];
} c2_sPk8KX86oBjv2lJavBJkkIB;

#else

typedef struct sPk8KX86oBjv2lJavBJkkIB c2_sPk8KX86oBjv2lJavBJkkIB;

#endif
#endif                                 /* _CSC2_SD_JF_MEDIAN_CHARTSTRUCT */

/* Parameters (auto storage) */
typedef struct Parameters_SD_jf_median_ Parameters_SD_jf_median;

/* Forward declaration for rtModel */
typedef struct RT_MODEL_SD_jf_median RT_MODEL_SD_jf_median;

#ifdef __cplusplus
}
#endif

#endif                                 /* RTW_HEADER_SD_jf_median_types_h_ */
