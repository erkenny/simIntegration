/*
 * SD_jf_mg_back_stride.c
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

#include "SD_jf_mg_back_stride_trc_ptr.h"
#include "SD_jf_mg_back_stride.h"
#include "SD_jf_mg_back_stride_private.h"

/* Block signals (auto storage) */
BlockIO_SD_jf_mg_back_stride SD_jf_mg_back_stride_B;

/* Block states (auto storage) */
D_Work_SD_jf_mg_back_stride SD_jf_mg_back_stride_DWork;

/* Real-time model */
RT_MODEL_SD_jf_mg_back_stride SD_jf_mg_back_stride_M_;
RT_MODEL_SD_jf_mg_back_stride *SD_jf_mg_back_stride_M = &SD_jf_mg_back_stride_M_;
static boolean_T SD_jf_mg_back_stride_eml_strcmp(const char_T eml_a[2]);
static void SD_jf_mg_back_s_refp1_circshift(real_T eml_a[3]);

/* Functions for block: '<Root>/Embedded MATLAB Function' */
static boolean_T SD_jf_mg_back_stride_eml_strcmp(const char_T eml_a[2])
{
  boolean_T eml_bool;
  int32_T eml_k;
  int32_T eml_exitg1;
  static char_T eml_cv0[2] = { 'D', 'S' };

  eml_bool = false;
  eml_k = 1;
  do {
    eml_exitg1 = 0U;
    if (eml_k <= 2) {
      if (eml_a[eml_k - 1] != eml_cv0[eml_k - 1]) {
        eml_exitg1 = 1U;
      } else {
        eml_k++;
      }
    } else {
      eml_bool = true;
      eml_exitg1 = 1U;
    }
  } while (eml_exitg1 == 0U);

  return eml_bool;
}

static void SD_jf_mg_back_s_refp1_circshift(real_T eml_a[3])
{
  int32_T eml_ia;
  int32_T eml_k;
  real_T eml_atmp[3];
  eml_ia = 1;
  for (eml_k = 0; eml_k < 3; eml_k++) {
    eml_atmp[eml_k] = eml_a[eml_ia - 1];
    eml_ia++;
  }

  eml_a[0] = eml_atmp[2];
  eml_ia = 2;
  for (eml_k = 0; eml_k < 2; eml_k++) {
    eml_a[eml_ia - 1] = eml_atmp[eml_k];
    eml_ia++;
  }
}

/* Model output function */
static void SD_jf_mg_back_stride_output(int_T tid)
{
  {
    real_T accum;
    int32_T cfIdx;
    int32_T memIdx;
    int32_T j;
    int32_T nextMemIdx;

    /* This comment workarounds a Real-Time Workshop code generation problem */

    /* Gain: '<Root>/CH15 Raw' */
    SD_jf_mg_back_stride_B.CH15Raw = SD_jf_mg_back_stride_P.CH15Raw_Gain *
      SD_jf_mg_back_stride_B.SFunction[2];

    /* S-Function (sdspfilter2): '<S3>/Digital Filter' */
    memIdx = 0;
    nextMemIdx = 1;
    accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES[0];
    accum += SD_jf_mg_back_stride_B.CH15Raw *
      SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF[0];
    cfIdx = 1;
    SD_jf_mg_back_stride_B.DigitalFilter = accum;
    for (j = 0; j < 101; j++) {
      accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES[nextMemIdx];
      nextMemIdx++;
      accum += SD_jf_mg_back_stride_B.CH15Raw *
        SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF[cfIdx];
      cfIdx++;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES[memIdx] = accum;
      memIdx++;
    }

    /* RelationalOperator: '<S10>/Compare' incorporates:
     *  Constant: '<S10>/Constant'
     */
    SD_jf_mg_back_stride_B.Compare = (SD_jf_mg_back_stride_B.DigitalFilter >=
      SD_jf_mg_back_stride_P.Constant_Value);

    /* DataTypeConversion: '<Root>/Data Type Conversion' */
    SD_jf_mg_back_stride_B.DataTypeConversion = (real_T)
      SD_jf_mg_back_stride_B.Compare;

    /* Gain: '<Root>/CH13 Raw' */
    SD_jf_mg_back_stride_B.CH13Raw = SD_jf_mg_back_stride_P.CH13Raw_Gain *
      SD_jf_mg_back_stride_B.SFunction[0];

    /* S-Function (sdspfilter2): '<S1>/Digital Filter' */
    memIdx = 0;
    nextMemIdx = 1;
    accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_e[0];
    accum += SD_jf_mg_back_stride_B.CH13Raw *
      SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_h[0];
    cfIdx = 1;
    SD_jf_mg_back_stride_B.DigitalFilter_b = accum;
    for (j = 0; j < 101; j++) {
      accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_e[nextMemIdx];
      nextMemIdx++;
      accum += SD_jf_mg_back_stride_B.CH13Raw *
        SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_h[cfIdx];
      cfIdx++;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_e[memIdx] = accum;
      memIdx++;
    }

    /* Gain: '<Root>/CH14 Raw' */
    SD_jf_mg_back_stride_B.CH14Raw = SD_jf_mg_back_stride_P.CH14Raw_Gain *
      SD_jf_mg_back_stride_B.SFunction[1];

    /* S-Function (sdspfilter2): '<S2>/Digital Filter' */
    memIdx = 0;
    nextMemIdx = 1;
    accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_d[0];
    accum += SD_jf_mg_back_stride_B.CH14Raw *
      SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_a[0];
    cfIdx = 1;
    SD_jf_mg_back_stride_B.DigitalFilter_e = accum;
    for (j = 0; j < 101; j++) {
      accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_d[nextMemIdx];
      nextMemIdx++;
      accum += SD_jf_mg_back_stride_B.CH14Raw *
        SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_a[cfIdx];
      cfIdx++;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_d[memIdx] = accum;
      memIdx++;
    }

    /* This comment workarounds a Real-Time Workshop code generation problem */

    /* Gain: '<Root>/CH11 Raw' */
    SD_jf_mg_back_stride_B.CH11Raw = SD_jf_mg_back_stride_P.CH11Raw_Gain *
      SD_jf_mg_back_stride_B.SFunction_g[2];

    /* S-Function (sdspfilter2): '<S6>/Digital Filter' */
    memIdx = 0;
    nextMemIdx = 1;
    accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_o[0];
    accum += SD_jf_mg_back_stride_B.CH11Raw *
      SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_b[0];
    cfIdx = 1;
    SD_jf_mg_back_stride_B.DigitalFilter_k = accum;
    for (j = 0; j < 101; j++) {
      accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_o[nextMemIdx];
      nextMemIdx++;
      accum += SD_jf_mg_back_stride_B.CH11Raw *
        SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_b[cfIdx];
      cfIdx++;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_o[memIdx] = accum;
      memIdx++;
    }

    /* RelationalOperator: '<S11>/Compare' incorporates:
     *  Constant: '<S11>/Constant'
     */
    SD_jf_mg_back_stride_B.Compare_p = (SD_jf_mg_back_stride_B.DigitalFilter_k >=
      SD_jf_mg_back_stride_P.Constant_Value_i);

    /* DataTypeConversion: '<Root>/Data Type Conversion1' */
    SD_jf_mg_back_stride_B.DataTypeConversion1 = (real_T)
      SD_jf_mg_back_stride_B.Compare_p;

    /* Gain: '<Root>/CH9 Raw' */
    SD_jf_mg_back_stride_B.CH9Raw = SD_jf_mg_back_stride_P.CH9Raw_Gain *
      SD_jf_mg_back_stride_B.SFunction_g[0];

    /* S-Function (sdspfilter2): '<S4>/Digital Filter' */
    memIdx = 0;
    nextMemIdx = 1;
    accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_ej[0];
    accum += SD_jf_mg_back_stride_B.CH9Raw *
      SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_i[0];
    cfIdx = 1;
    SD_jf_mg_back_stride_B.DigitalFilter_kd = accum;
    for (j = 0; j < 101; j++) {
      accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_ej[nextMemIdx];
      nextMemIdx++;
      accum += SD_jf_mg_back_stride_B.CH9Raw *
        SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_i[cfIdx];
      cfIdx++;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_ej[memIdx] = accum;
      memIdx++;
    }

    /* Gain: '<Root>/CH10 Raw' */
    SD_jf_mg_back_stride_B.CH10Raw = SD_jf_mg_back_stride_P.CH10Raw_Gain *
      SD_jf_mg_back_stride_B.SFunction_g[1];

    /* S-Function (sdspfilter2): '<S5>/Digital Filter' */
    memIdx = 0;
    nextMemIdx = 1;
    accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_n[0];
    accum += SD_jf_mg_back_stride_B.CH10Raw *
      SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_ah[0];
    cfIdx = 1;
    SD_jf_mg_back_stride_B.DigitalFilter_l = accum;
    for (j = 0; j < 101; j++) {
      accum = SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_n[nextMemIdx];
      nextMemIdx++;
      accum += SD_jf_mg_back_stride_B.CH10Raw *
        SD_jf_mg_back_stride_P.DigitalFilter_RTP1COEFF_ah[cfIdx];
      cfIdx++;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_n[memIdx] = accum;
      memIdx++;
    }

    /* SignalConversion: '<S9>/TmpHiddenBufferAt SFunction Inport1' */
    SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[0] =
      SD_jf_mg_back_stride_B.DataTypeConversion;
    SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[1] =
      SD_jf_mg_back_stride_B.DigitalFilter_b;
    SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[2] =
      SD_jf_mg_back_stride_B.DigitalFilter_e;
    SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[3] =
      SD_jf_mg_back_stride_B.DigitalFilter;
    SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[4] =
      SD_jf_mg_back_stride_B.DataTypeConversion1;
    SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[5] =
      SD_jf_mg_back_stride_B.DigitalFilter_kd;
    SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[6] =
      SD_jf_mg_back_stride_B.DigitalFilter_l;
    SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[7] =
      SD_jf_mg_back_stride_B.DigitalFilter_k;

    /* Embedded MATLAB Block: '<Root>/Embedded MATLAB Function'
     */
    {
      real_T eml_LON;
      real_T eml_LMx;
      real_T eml_LFy;
      real_T eml_LFz;
      real_T eml_RMx;
      int32_T eml_i0;
      real_T eml_hoistedExpr[3];

      /* Embedded MATLAB Function 'Embedded MATLAB Function': '<S9>:1' */
      /* %data_parse */
      /*  this function takes input in the form of the follwoing vector  */
      /* [LON LMx LFy LFz RON RMx RFy RFz] and outputs the following variables: */
      /* Back Foot Center of Pressure */
      /* The estimated location of the hip */
      /* The y (A/P) component of force of the basdf ack-foot */
      /* The z (vertical) */
      /* '<S9>:1:9' */
      /* Right plate offset from origin */
      /* '<S9>:1:8' */
      /* manually demux the collected signals */
      /* '<S9>:1:18' */
      eml_LON = SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[0];

      /* '<S9>:1:19' */
      eml_LMx = SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[1];

      /* '<S9>:1:20' */
      eml_LFy = SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[2];

      /* '<S9>:1:21' */
      eml_LFz = SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[3];

      /* '<S9>:1:22' */
      /* '<S9>:1:23' */
      eml_RMx = SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[5];

      /* '<S9>:1:24' */
      SD_jf_mg_back_stride_B.BackFy =
        SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[6];

      /* '<S9>:1:25' */
      SD_jf_mg_back_stride_B.BackFz =
        SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[7];
      if ((eml_LON != 0.0) &&
          (SD_jf_mg_back_stride_B.TmpHiddenBufferAtSFunctionInpor[4] != 0.0)) {
        /* '<S9>:1:30' */
        /* double support */
        /* '<S9>:1:32' */
        /* '<S9>:1:33' */
        if (eml_RMx / SD_jf_mg_back_stride_B.BackFz + 0.00295 < eml_LMx /
            eml_LFz) {
          /* '<S9>:1:34' */
          /*  Right is back leg */
          /* '<S9>:1:35' */
          SD_jf_mg_back_stride_B.BackCoP = eml_RMx /
            SD_jf_mg_back_stride_B.BackFz;

          /* '<S9>:1:36' */
          if ((SD_jf_mg_back_stride_B.BackFz < 400.0) || rtIsNaN
              (SD_jf_mg_back_stride_B.BackFz)) {
            eml_LON = 400.0;
          } else {
            eml_LON = SD_jf_mg_back_stride_B.BackFz;
          }

          SD_jf_mg_back_stride_B.Hip = SD_jf_mg_back_stride_B.BackCoP -
            SD_jf_mg_back_stride_B.BackFy / sqrt(rt_pow_snf
            (SD_jf_mg_back_stride_B.BackFy, 2.0) + rt_pow_snf(eml_LON, 2.0));

          /* '<S9>:1:37' */
          /* '<S9>:1:38' */
        } else {
          /*  Left is back leg */
          /* '<S9>:1:40' */
          SD_jf_mg_back_stride_B.BackCoP = eml_LMx / eml_LFz;

          /* '<S9>:1:41' */
          if ((eml_LFz < 400.0) || rtIsNaN(eml_LFz)) {
            eml_LON = 400.0;
          } else {
            eml_LON = eml_LFz;
          }

          SD_jf_mg_back_stride_B.Hip = SD_jf_mg_back_stride_B.BackCoP - eml_LFy /
            sqrt(rt_pow_snf(eml_LFy, 2.0) + rt_pow_snf(eml_LON, 2.0));

          /* '<S9>:1:42' */
          SD_jf_mg_back_stride_B.BackFy = eml_LFy;

          /* '<S9>:1:43' */
          SD_jf_mg_back_stride_B.BackFz = eml_LFz;
        }
      } else if (eml_LON != 0.0) {
        /* '<S9>:1:46' */
        /*  Left single support */
        /* '<S9>:1:48' */
        SD_jf_mg_back_stride_B.BackCoP = eml_LMx / eml_LFz;

        /* '<S9>:1:49' */
        if ((eml_LFz < 400.0) || rtIsNaN(eml_LFz)) {
          eml_LON = 400.0;
        } else {
          eml_LON = eml_LFz;
        }

        SD_jf_mg_back_stride_B.Hip = SD_jf_mg_back_stride_B.BackCoP - eml_LFy /
          sqrt(rt_pow_snf(eml_LFy, 2.0) + rt_pow_snf(eml_LON, 2.0));

        /* '<S9>:1:50' */
        SD_jf_mg_back_stride_B.BackFy = eml_LFy;

        /* '<S9>:1:51' */
        SD_jf_mg_back_stride_B.BackFz = eml_LFz;
        if (SD_jf_mg_back_stride_eml_strcmp
            (SD_jf_mg_back_stride_DWork.curr_struct.last_state)) {
          /* '<S9>:1:53' */
          /* new stride detected */
          /* '<S9>:1:54' */
          for (eml_i0 = 0; eml_i0 < 3; eml_i0++) {
            eml_hoistedExpr[eml_i0] =
              SD_jf_mg_back_stride_DWork.curr_struct.mp_vec[eml_i0];
          }

          SD_jf_mg_back_s_refp1_circshift(eml_hoistedExpr);
          for (eml_i0 = 0; eml_i0 < 3; eml_i0++) {
            SD_jf_mg_back_stride_DWork.curr_struct.mp_vec[eml_i0] =
              eml_hoistedExpr[eml_i0];
          }

          /* '<S9>:1:55' */
          SD_jf_mg_back_stride_DWork.curr_struct.mp_vec[2] =
            SD_jf_mg_back_stride_DWork.curr_struct.stride;

          /* '<S9>:1:56' */
          SD_jf_mg_back_stride_DWork.curr_struct.stride = 0.7;          // midpoint

          /* holds hip data for the current stride */
          /* '<S9>:1:57' */
          SD_jf_mg_back_stride_DWork.curr_struct.len = 0.0;
        } else {
          /* '<S9>:1:59' */
          SD_jf_mg_back_stride_DWork.curr_struct.stride =
            SD_jf_mg_back_stride_B.Hip;

          /* '<S9>:1:60' */
          SD_jf_mg_back_stride_DWork.curr_struct.len =
            SD_jf_mg_back_stride_DWork.curr_struct.len + 1.0;
        }

        /* '<S9>:1:63' */
        for (eml_i0 = 0; eml_i0 < 2; eml_i0++) {
          SD_jf_mg_back_stride_DWork.curr_struct.last_state[eml_i0] = 'S';
        }
      } else {
        /*  Right Single support */
        /* '<S9>:1:66' */
        SD_jf_mg_back_stride_B.BackCoP = eml_RMx / SD_jf_mg_back_stride_B.BackFz
          + 0.00295;

        /* '<S9>:1:67' */
        if ((SD_jf_mg_back_stride_B.BackFz < 400.0) || rtIsNaN
            (SD_jf_mg_back_stride_B.BackFz)) {
          eml_LON = 400.0;
        } else {
          eml_LON = SD_jf_mg_back_stride_B.BackFz;
        }

        SD_jf_mg_back_stride_B.Hip = SD_jf_mg_back_stride_B.BackCoP -
          SD_jf_mg_back_stride_B.BackFy / sqrt(rt_pow_snf
          (SD_jf_mg_back_stride_B.BackFy, 2.0) + rt_pow_snf(eml_LON, 2.0));

        /* '<S9>:1:68' */
        /* '<S9>:1:69' */
        if (SD_jf_mg_back_stride_eml_strcmp
            (SD_jf_mg_back_stride_DWork.curr_struct.last_state)) {
          /* '<S9>:1:71' */
          /* new stride detected */
          /* '<S9>:1:72' */
          for (eml_i0 = 0; eml_i0 < 3; eml_i0++) {
            eml_hoistedExpr[eml_i0] =
              SD_jf_mg_back_stride_DWork.curr_struct.mp_vec[eml_i0];
          }

          SD_jf_mg_back_s_refp1_circshift(eml_hoistedExpr);
          for (eml_i0 = 0; eml_i0 < 3; eml_i0++) {
            SD_jf_mg_back_stride_DWork.curr_struct.mp_vec[eml_i0] =
              eml_hoistedExpr[eml_i0];
          }

          /* '<S9>:1:73' */
          SD_jf_mg_back_stride_DWork.curr_struct.mp_vec[2] =
            SD_jf_mg_back_stride_DWork.curr_struct.stride;

          /* '<S9>:1:74' */
          SD_jf_mg_back_stride_DWork.curr_struct.stride = 0.7;

          /* holds hip data for the current stride */
          /* '<S9>:1:75' */
          SD_jf_mg_back_stride_DWork.curr_struct.len = 0.0;
        } else {
          /* '<S9>:1:77' */
          SD_jf_mg_back_stride_DWork.curr_struct.stride =
            SD_jf_mg_back_stride_B.Hip;

          /* '<S9>:1:78' */
          SD_jf_mg_back_stride_DWork.curr_struct.len =
            SD_jf_mg_back_stride_DWork.curr_struct.len + 1.0;
        }

        /* '<S9>:1:81' */
        for (eml_i0 = 0; eml_i0 < 2; eml_i0++) {
          SD_jf_mg_back_stride_DWork.curr_struct.last_state[eml_i0] = 'S';
        }
      }

      /* '<S9>:1:83' */
      for (eml_i0 = 0; eml_i0 < 3; eml_i0++) {
        SD_jf_mg_back_stride_B.mp_vec[eml_i0] =
          SD_jf_mg_back_stride_DWork.curr_struct.mp_vec[eml_i0];
      }
    }

    /* user code (Output function Trailer) */
    /* dSPACE Data Capture block: (none) */
    /* ... Service number: 1 */
    /* ... Service name:   (default) */
    {
      host_service(1, rtk_current_task_absolute_time_ptr_get());
    }
  }

  /* tid is required for a uniform function interface.
   * Argument tid is not used in the function. */
  UNUSED_PARAMETER(tid);
}

/* Model update function */
static void SD_jf_mg_back_stride_update(int_T tid)
{
  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++SD_jf_mg_back_stride_M->Timing.clockTick0))
    ++SD_jf_mg_back_stride_M->Timing.clockTickH0;
  SD_jf_mg_back_stride_M->Timing.t[0] =
    SD_jf_mg_back_stride_M->Timing.clockTick0 *
    SD_jf_mg_back_stride_M->Timing.stepSize0 +
    SD_jf_mg_back_stride_M->Timing.clockTickH0 *
    SD_jf_mg_back_stride_M->Timing.stepSize0 * 4294967296.0;

  /* tid is required for a uniform function interface.
   * Argument tid is not used in the function. */
  UNUSED_PARAMETER(tid);
}

/* Model initialize function */
void SD_jf_mg_back_stride_initialize(boolean_T firstTime)
{
  (void)firstTime;

  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)SD_jf_mg_back_stride_M,0,
                sizeof(RT_MODEL_SD_jf_mg_back_stride));

  /* Initialize timing info */
  {
    int_T *mdlTsMap = SD_jf_mg_back_stride_M->Timing.sampleTimeTaskIDArray;
    mdlTsMap[0] = 0;
    SD_jf_mg_back_stride_M->Timing.sampleTimeTaskIDPtr = (&mdlTsMap[0]);
    SD_jf_mg_back_stride_M->Timing.sampleTimes =
      (&SD_jf_mg_back_stride_M->Timing.sampleTimesArray[0]);
    SD_jf_mg_back_stride_M->Timing.offsetTimes =
      (&SD_jf_mg_back_stride_M->Timing.offsetTimesArray[0]);

    /* task periods */
    SD_jf_mg_back_stride_M->Timing.sampleTimes[0] = (0.001);

    /* task offsets */
    SD_jf_mg_back_stride_M->Timing.offsetTimes[0] = (0.0);
  }

  rtmSetTPtr(SD_jf_mg_back_stride_M, &SD_jf_mg_back_stride_M->Timing.tArray[0]);

  {
    int_T *mdlSampleHits = SD_jf_mg_back_stride_M->Timing.sampleHitArray;
    mdlSampleHits[0] = 1;
    SD_jf_mg_back_stride_M->Timing.sampleHits = (&mdlSampleHits[0]);
  }

  rtmSetTFinal(SD_jf_mg_back_stride_M, -1);
  SD_jf_mg_back_stride_M->Timing.stepSize0 = 0.001;
  SD_jf_mg_back_stride_M->solverInfoPtr = (&SD_jf_mg_back_stride_M->solverInfo);
  SD_jf_mg_back_stride_M->Timing.stepSize = (0.001);
  rtsiSetFixedStepSize(&SD_jf_mg_back_stride_M->solverInfo, 0.001);
  rtsiSetSolverMode(&SD_jf_mg_back_stride_M->solverInfo,
                    SOLVER_MODE_SINGLETASKING);

  /* block I/O */
  SD_jf_mg_back_stride_M->ModelData.blockIO = ((void *) &SD_jf_mg_back_stride_B);
  (void) memset(((void *) &SD_jf_mg_back_stride_B),0,
                sizeof(BlockIO_SD_jf_mg_back_stride));

  /* parameters */
  SD_jf_mg_back_stride_M->ModelData.defaultParam = ((real_T *)
    &SD_jf_mg_back_stride_P);

  /* states (dwork) */
  SD_jf_mg_back_stride_M->Work.dwork = ((void *) &SD_jf_mg_back_stride_DWork);
  (void) memset((void *)&SD_jf_mg_back_stride_DWork, 0,
                sizeof(D_Work_SD_jf_mg_back_stride));

  {
    /* user code (registration function declaration) */
    /*Call the macro that initializes the global TRC pointers
       inside the model initialization/registration function. */
    RTI_INIT_TRC_POINTERS();
  }
}

/* Model terminate function */
void SD_jf_mg_back_stride_terminate(void)
{
  /* (no terminate code required) */
}

/*========================================================================*
 * Start of GRT compatible call interface                                 *
 *========================================================================*/
void MdlOutputs(int_T tid)
{
  SD_jf_mg_back_stride_output(tid);
}

void MdlUpdate(int_T tid)
{
  SD_jf_mg_back_stride_update(tid);
}

void MdlInitializeSizes(void)
{
  SD_jf_mg_back_stride_M->Sizes.numContStates = (0);/* Number of continuous states */
  SD_jf_mg_back_stride_M->Sizes.numY = (0);/* Number of model outputs */
  SD_jf_mg_back_stride_M->Sizes.numU = (0);/* Number of model inputs */
  SD_jf_mg_back_stride_M->Sizes.sysDirFeedThru = (0);/* The model is not direct feedthrough */
  SD_jf_mg_back_stride_M->Sizes.numSampTimes = (1);/* Number of sample times */
  SD_jf_mg_back_stride_M->Sizes.numBlocks = (42);/* Number of blocks */
  SD_jf_mg_back_stride_M->Sizes.numBlockIO = (24);/* Number of block outputs */
  SD_jf_mg_back_stride_M->Sizes.numBlockPrms = (620);/* Sum of parameter "widths" */
}

void MdlInitializeSampleTimes(void)
{
}

void MdlInitialize(void)
{
  {
    int32_T j;

    /* InitializeConditions for S-Function (sdspfilter2): '<S3>/Digital Filter' */
    for (j = 0; j < 101; j++) {
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES[j] =
        SD_jf_mg_back_stride_ConstP.pooled1;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_e[j] =
        SD_jf_mg_back_stride_ConstP.pooled1;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_d[j] =
        SD_jf_mg_back_stride_ConstP.pooled1;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_o[j] =
        SD_jf_mg_back_stride_ConstP.pooled1;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_ej[j] =
        SD_jf_mg_back_stride_ConstP.pooled1;
      SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_n[j] =
        SD_jf_mg_back_stride_ConstP.pooled1;
    }

    SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES[101] = 0.0;

    /* InitializeConditions for S-Function (sdspfilter2): '<S1>/Digital Filter' */
    SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_e[101] = 0.0;

    /* InitializeConditions for S-Function (sdspfilter2): '<S2>/Digital Filter' */
    SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_d[101] = 0.0;

    /* InitializeConditions for S-Function (sdspfilter2): '<S6>/Digital Filter' */
    SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_o[101] = 0.0;

    /* InitializeConditions for S-Function (sdspfilter2): '<S4>/Digital Filter' */
    SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_ej[101] = 0.0;

    /* InitializeConditions for S-Function (sdspfilter2): '<S5>/Digital Filter' */
    SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES_n[101] = 0.0;

    /* Initialize code for chart: '<Root>/Embedded MATLAB Function' */
    {
      static struct sMmBGyhP8Oni5W3IfA9ChOH eml_r0 = { 0.7, 0.0, { 0.0, 0.0, 0.0
        }, { 'F', 'S' } };

      SD_jf_mg_back_stride_DWork. = eml_r0;
    }
  }
}

void MdlStart(void)
{
  MdlInitialize();
}

RT_MODEL_SD_jf_mg_back_stride *SD_jf_mg_back_stride(void)
{
  SD_jf_mg_back_stride_initialize(1);curr_struct
  return SD_jf_mg_back_stride_M;
}

void MdlTerminate(void)
{
  SD_jf_mg_back_stride_terminate();
}

/*========================================================================*
 * End of GRT compatible call interface                                   *
 *========================================================================*/
