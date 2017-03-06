/*
 * SD_jf_mg_back_stride_types.h
 *
 * Real-Time Workshop code generation for Simulink model "SD_jf_mg_back_stride.mdl".
 *
 * Model Version              : 1.20
 * Real-Time Workshop version : 7.3  (R2009a)  15-Jan-2009
 * C source code generated on : Sun Feb 12 17:44:28 2017
 *
 * Target selection: rti1103.tlc
 *   Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Generic->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#ifndef RTW_HEADER_SD_jf_mg_back_stride_types_h_
#define RTW_HEADER_SD_jf_mg_back_stride_types_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "rtwtypes.h"
#ifndef _CSC2_SD_JF_MG_BACK_STRIDE_CHART
#define _CSC2_SD_JF_MG_BACK_STRIDE_CHART
#ifndef struct_sMmBGyhP8Oni5W3IfA9ChOH
#define struct_sMmBGyhP8Oni5W3IfA9ChOH

typedef struct sMmBGyhP8Oni5W3IfA9ChOH {
  real_T stride;
  real_T len;
  real_T mp_vec[3];
  char_T last_state[2];
} c2_sMmBGyhP8Oni5W3IfA9ChOH;

#else

typedef struct sMmBGyhP8Oni5W3IfA9ChOH c2_sMmBGyhP8Oni5W3IfA9ChOH;

#endif
#endif                                 /* _CSC2_SD_JF_MG_BACK_STRIDE_CHART */

/* Parameters (auto storage) */
typedef struct Parameters_SD_jf_mg_back_stride_ Parameters_SD_jf_mg_back_stride;

/* Forward declaration for rtModel */
typedef struct RT_MODEL_SD_jf_mg_back_stride RT_MODEL_SD_jf_mg_back_stride;


#ifdef __cplusplus
}
#endif

#endif                                 /* RTW_HEADER_SD_jf_mg_back_stride_types_h_ */
