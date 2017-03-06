/*
 * SD_jf_median.c
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

#include "SD_jf_median_trc_ptr.h"
#include "SD_jf_median.h"
#include "SD_jf_median_private.h"

/* Block signals (auto storage) */
BlockIO_SD_jf_median SD_jf_median_B;

/* Continuous states */
ContinuousStates_SD_jf_median SD_jf_median_X;

/* Block states (auto storage) */
D_Work_SD_jf_median SD_jf_median_DWork;

/* Real-time model */
RT_MODEL_SD_jf_median SD_jf_median_M_;
RT_MODEL_SD_jf_median *SD_jf_median_M = &SD_jf_median_M_;
static void SD_jf_median_circshift(const real_T eml_a[500], real_T eml_a_0[500]);
static real_T SD_jf_median_median(const real_T eml_x[500]);
static void SD_jf_median_eml_sort_idx(const real_T eml_x[500], int32_T eml_idx
  [500]);
static void SD_jf_median_diff(const real_T eml_x[3], real_T eml_y[2]);
static void SD_jf_median_refp1_circshift(real_T eml_a[500]);

/* This function updates continuous states using the ODE1 fixed-step
 * solver algorithm
 */
static void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  time_T tnew = rtsiGetSolverStopTime(si);
  time_T h = rtsiGetStepSize(si);
  real_T *x = rtsiGetContStates(si);
  ODE1_IntgData *id = (ODE1_IntgData *)rtsiGetSolverData(si);
  real_T *f0 = id->f[0];
  int_T i;
  int_T nXc = 1;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);
  rtsiSetdX(si, f0);
  SD_jf_median_derivatives();
  rtsiSetT(si, tnew);
  for (i = 0; i < nXc; i++) {
    *x += h * f0[i];
    x++;
  }

  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

/* Functions for block: '<Root>/Embedded MATLAB Function' */
static void SD_jf_median_circshift(const real_T eml_a[500], real_T eml_a_0[500])
{
  memcpy((void *)&eml_a_0[0], (void *)&eml_a[0], 500U * sizeof(real_T));
  SD_jf_median_refp1_circshift(eml_a_0);
}

static real_T SD_jf_median_median(const real_T eml_x[500])
{
  int32_T eml_iidx[500];
  int32_T eml_i1;
  real_T eml_v[500];
  real_T eml_a;
  real_T eml_b;
  SD_jf_median_eml_sort_idx(eml_x, eml_iidx);
  for (eml_i1 = 0; eml_i1 < 500; eml_i1++) {
    eml_v[eml_i1] = eml_x[eml_iidx[eml_i1] - 1];
  }

  if (rtIsNaN(eml_v[499])) {
    return eml_v[499];
  } else {
    eml_a = eml_v[249];
    eml_b = eml_v[250];
    if (((eml_a < 0.0) != (eml_b < 0.0)) || rtIsInf(eml_a) || rtIsInf(eml_b)) {
      return (eml_a + eml_b) / 2.0;
    } else {
      return eml_a + (eml_b - eml_a) / 2.0;
    }
  }
}

static void SD_jf_median_eml_sort_idx(const real_T eml_x[500], int32_T eml_idx
  [500])
{
  int32_T eml_k;
  int16_T eml_idx0[500];
  real_T eml_b;
  boolean_T eml_p;
  int32_T eml_i;
  int32_T eml_i2;
  int32_T eml_pEnd;
  int32_T eml_p_0;
  int32_T eml_q;
  int32_T eml_qEnd;
  int32_T eml_k_0;
  int32_T eml_kEnd;
  for (eml_k = 0; eml_k < 500; eml_k++) {
    eml_idx0[eml_k] = 1;
    eml_idx[eml_k] = 1 + eml_k;
  }

  for (eml_k = 0; eml_k < 500; eml_k += 2) {
    eml_b = eml_x[eml_k + 1];
    if ((eml_x[eml_k] <= eml_b) || rtIsNaN(eml_b)) {
      eml_p = true;
    } else {
      eml_p = false;
    }

    if (!eml_p) {
      eml_idx[eml_k] = eml_k + 2;
      eml_idx[eml_k + 1] = eml_k + 1;
    }
  }

  for (eml_i = 2; eml_i <= 500; eml_i = eml_i2) {
    eml_i2 = eml_i << 1;
    eml_k = 1;
    for (eml_pEnd = 1 + eml_i; eml_pEnd < 501; eml_pEnd = eml_qEnd + eml_i) {
      eml_p_0 = eml_k;
      eml_q = eml_pEnd;
      eml_qEnd = eml_k + eml_i2;
      if (eml_qEnd > 501) {
        eml_qEnd = 501;
      }

      eml_k_0 = 1;
      eml_kEnd = eml_qEnd - eml_k;
      while (eml_k_0 <= eml_kEnd) {
        eml_b = eml_x[eml_idx[eml_q - 1] - 1];
        if ((eml_x[eml_idx[eml_p_0 - 1] - 1] <= eml_b) || rtIsNaN(eml_b)) {
          eml_p = true;
        } else {
          eml_p = false;
        }

        if (eml_p) {
          eml_idx0[eml_k_0 - 1] = (int16_T)eml_idx[eml_p_0 - 1];
          eml_p_0++;
          if (eml_p_0 == eml_pEnd) {
            while (eml_q < eml_qEnd) {
              eml_k_0++;
              eml_idx0[eml_k_0 - 1] = (int16_T)eml_idx[eml_q - 1];
              eml_q++;
            }
          }
        } else {
          eml_idx0[eml_k_0 - 1] = (int16_T)eml_idx[eml_q - 1];
          eml_q++;
          if (eml_q == eml_qEnd) {
            while (eml_p_0 < eml_pEnd) {
              eml_k_0++;
              eml_idx0[eml_k_0 - 1] = (int16_T)eml_idx[eml_p_0 - 1];
              eml_p_0++;
            }
          }
        }

        eml_k_0++;
      }

      eml_p_0 = eml_k - 1;
      for (eml_k = 1; eml_k <= eml_kEnd; eml_k++) {
        eml_idx[(eml_p_0 + eml_k) - 1] = eml_idx0[eml_k - 1];
      }

      eml_k = eml_qEnd;
    }
  }
}

static void SD_jf_median_diff(const real_T eml_x[3], real_T eml_y[2])
{
  int32_T eml_ixLead;
  int32_T eml_iyLead;
  real_T eml_work;
  int32_T eml_m;
  real_T eml_tmp1;
  real_T eml_tmp2;
  eml_ixLead = 2;
  eml_iyLead = 1;
  eml_work = eml_x[0];
  for (eml_m = 2; eml_m < 4; eml_m++) {
    eml_tmp1 = eml_x[eml_ixLead - 1];
    eml_tmp2 = eml_work;
    eml_work = eml_tmp1;
    eml_tmp1 -= eml_tmp2;
    eml_ixLead++;
    eml_y[eml_iyLead - 1] = eml_tmp1;
    eml_iyLead++;
  }
}

static void SD_jf_median_refp1_circshift(real_T eml_a[500])
{
  int32_T eml_ia;
  int32_T eml_k;
  real_T eml_atmp[500];
  eml_ia = 1;
  for (eml_k = 0; eml_k < 500; eml_k++) {
    eml_atmp[eml_k] = eml_a[eml_ia - 1];
    eml_ia++;
  }

  eml_a[0] = eml_atmp[499];
  eml_ia = 2;
  for (eml_k = 0; eml_k < 499; eml_k++) {
    eml_a[eml_ia - 1] = eml_atmp[eml_k];
    eml_ia++;
  }
}

/* Model output function */
void SD_jf_median_output(int_T tid)
{
  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(SD_jf_median_M)) {
    SD_jf_median_M->Timing.t[0] = rtsiGetT(&SD_jf_median_M->solverInfo);
  }

  if (rtmIsMajorTimeStep(SD_jf_median_M)) {
    /* set solver stop time */
    rtsiSetSolverStopTime(&SD_jf_median_M->solverInfo,
                          ((SD_jf_median_M->Timing.clockTick0+1)*
      SD_jf_median_M->Timing.stepSize0));
  }                                    /* end MajorTimeStep */

  {
    real_T numAccum;
    real_T stageIn;
    if (rtmIsMajorTimeStep(SD_jf_median_M)) {
      /* This comment workarounds a Real-Time Workshop code generation problem */

      /* Gain: '<Root>/CH15 Raw' */
      SD_jf_median_B.CH15Raw = SD_jf_median_P.CH15Raw_Gain *
        SD_jf_median_B.SFunction[2];

      /* S-Function (sdspbiquad): '<S3>/Digital Filter' */
      stageIn = SD_jf_median_ConstP.pooled2[0] * SD_jf_median_B.CH15Raw;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF[0] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[0];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[1];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF[0] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[0];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[1];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[1] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[0];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[0] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[2];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF[3] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[3];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF[3] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[2];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[3];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[3] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[2];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[2] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[4];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[5];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF[6] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[4];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF[8] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[5];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[5] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[4];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[4] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF[6] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[6];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[7];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF[9] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF[10] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[6];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF[11] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[7];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[7] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES[6];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[6] = stageIn;
      SD_jf_median_B.DigitalFilter = numAccum;

      /* RelationalOperator: '<S11>/Compare' incorporates:
       *  Constant: '<S11>/Constant'
       */
      SD_jf_median_B.Compare = (SD_jf_median_B.DigitalFilter >=
        SD_jf_median_P.Constant_Value);

      /* DataTypeConversion: '<Root>/Data Type Conversion' */
      SD_jf_median_B.DataTypeConversion = (real_T)SD_jf_median_B.Compare;

      /* Gain: '<Root>/CH13 Raw' */
      SD_jf_median_B.CH13Raw = SD_jf_median_P.CH13Raw_Gain *
        SD_jf_median_B.SFunction[0];

      /* S-Function (sdspbiquad): '<S1>/Digital Filter' */
      stageIn = SD_jf_median_ConstP.pooled2[0] * SD_jf_median_B.CH13Raw;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_n[0] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[0];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_n[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[1];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_j[0] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_j[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[0];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_j[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[1];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[1] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[0];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[0] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_n[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[2];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_n[3] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[3];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_j[3] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_j[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[2];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_j[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[3];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[3] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[2];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[2] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_n[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[4];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_n[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[5];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_j[6] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_j[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[4];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_j[8] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[5];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[5] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[4];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[4] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_n[6] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[6];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_n[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[7];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_j[9] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_j[10] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[6];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_j[11] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[7];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[7] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[6];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[6] = stageIn;
      SD_jf_median_B.DigitalFilter_d = numAccum;

      /* Gain: '<Root>/CH14 Raw' */
      SD_jf_median_B.CH14Raw = SD_jf_median_P.CH14Raw_Gain *
        SD_jf_median_B.SFunction[1];

      /* S-Function (sdspbiquad): '<S2>/Digital Filter' */
      stageIn = SD_jf_median_ConstP.pooled2[0] * SD_jf_median_B.CH14Raw;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_b[0] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[0];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_b[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[1];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_i[0] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_i[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[0];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_i[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[1];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[1] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[0];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[0] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_b[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[2];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_b[3] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[3];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_i[3] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_i[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[2];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_i[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[3];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[3] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[2];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[2] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_b[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[4];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_b[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[5];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_i[6] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_i[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[4];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_i[8] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[5];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[5] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[4];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[4] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_b[6] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[6];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_b[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[7];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_i[9] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_i[10] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[6];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_i[11] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[7];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[7] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[6];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[6] = stageIn;
      SD_jf_median_B.DigitalFilter_j = numAccum;

      /* This comment workarounds a Real-Time Workshop code generation problem */

      /* Gain: '<Root>/CH11 Raw' */
      SD_jf_median_B.CH11Raw = SD_jf_median_P.CH11Raw_Gain *
        SD_jf_median_B.SFunction_m[2];

      /* S-Function (sdspbiquad): '<S6>/Digital Filter' */
      stageIn = SD_jf_median_ConstP.pooled2[0] * SD_jf_median_B.CH11Raw;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_e[0] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[0];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_e[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[1];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_l[0] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_l[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[0];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_l[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[1];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[1] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[0];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[0] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_e[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[2];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_e[3] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[3];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_l[3] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_l[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[2];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_l[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[3];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[3] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[2];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[2] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_e[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[4];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_e[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[5];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_l[6] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_l[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[4];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_l[8] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[5];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[5] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[4];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[4] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_e[6] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[6];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_e[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[7];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_l[9] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_l[10] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[6];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_l[11] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[7];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[7] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[6];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[6] = stageIn;
      SD_jf_median_B.DigitalFilter_n = numAccum;

      /* RelationalOperator: '<S12>/Compare' incorporates:
       *  Constant: '<S12>/Constant'
       */
      SD_jf_median_B.Compare_g = (SD_jf_median_B.DigitalFilter_n >=
        SD_jf_median_P.Constant_Value_m);

      /* DataTypeConversion: '<Root>/Data Type Conversion1' */
      SD_jf_median_B.DataTypeConversion1 = (real_T)SD_jf_median_B.Compare_g;

      /* Gain: '<Root>/CH9 Raw' */
      SD_jf_median_B.CH9Raw = SD_jf_median_P.CH9Raw_Gain *
        SD_jf_median_B.SFunction_m[0];

      /* S-Function (sdspbiquad): '<S4>/Digital Filter' */
      stageIn = SD_jf_median_ConstP.pooled2[0] * SD_jf_median_B.CH9Raw;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_o[0] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[0];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_o[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[1];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_f[0] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_f[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[0];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_f[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[1];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[1] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[0];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[0] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_o[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[2];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_o[3] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[3];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_f[3] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_f[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[2];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_f[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[3];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[3] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[2];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[2] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_o[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[4];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_o[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[5];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_f[6] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_f[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[4];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_f[8] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[5];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[5] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[4];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[4] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_o[6] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[6];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_o[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[7];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_f[9] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_f[10] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[6];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_f[11] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[7];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[7] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[6];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[6] = stageIn;
      SD_jf_median_B.DigitalFilter_h = numAccum;

      /* Gain: '<Root>/CH10 Raw' */
      SD_jf_median_B.CH10Raw = SD_jf_median_P.CH10Raw_Gain *
        SD_jf_median_B.SFunction_m[1];

      /* S-Function (sdspbiquad): '<S5>/Digital Filter' */
      stageIn = SD_jf_median_ConstP.pooled2[0] * SD_jf_median_B.CH10Raw;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_el[0] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[0];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_el[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[1];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_o[0] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_o[1] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[0];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_o[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[1];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[1] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[0];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[0] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_el[2] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[2];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_el[3] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[3];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_o[3] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_o[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[2];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_o[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[3];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[3] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[2];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[2] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_el[4] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[4];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_el[5] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[5];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_o[6] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_o[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[4];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_o[8] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[5];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[5] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[4];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[4] = stageIn;
      stageIn = numAccum;
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_el[6] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[6];
      stageIn -= SD_jf_median_P.DigitalFilter_RTP2COEFF_el[7] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[7];
      numAccum = SD_jf_median_P.DigitalFilter_RTP1COEFF_o[9] * stageIn;
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_o[10] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[6];
      numAccum += SD_jf_median_P.DigitalFilter_RTP1COEFF_o[11] *
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[7];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[7] =
        SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[6];
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[6] = stageIn;
      SD_jf_median_B.DigitalFilter_l = numAccum;
    }

    /* Clock Block: '<Root>/Clock'
     */
    SD_jf_median_B.Clock = SD_jf_median_M->Timing.t[0];

    /* SignalConversion: '<S9>/TmpHiddenBufferAt SFunction Inport1' */
    SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[0] =
      SD_jf_median_B.DataTypeConversion;
    SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[1] =
      SD_jf_median_B.DigitalFilter_d;
    SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[2] =
      SD_jf_median_B.DigitalFilter_j;
    SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[3] =
      SD_jf_median_B.DigitalFilter;
    SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[4] =
      SD_jf_median_B.DataTypeConversion1;
    SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[5] =
      SD_jf_median_B.DigitalFilter_h;
    SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[6] =
      SD_jf_median_B.DigitalFilter_l;
    SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[7] =
      SD_jf_median_B.DigitalFilter_n;

    /* Embedded MATLAB Block: '<Root>/Embedded MATLAB Function'
     */
    {
      real_T eml_LON;
      real_T eml_LMx;
      real_T eml_LFy;
      real_T eml_LFz;
      real_T eml_RMx;
      real_T eml_RFy;
      real_T eml_RFz;
      int32_T eml_i0;
      real_T eml_dv0[500];
      real_T eml_unusedExpr[2];
      real_T eml_unusedExpr_0[2];

      /* Embedded MATLAB Function 'Embedded MATLAB Function': '<S9>:1' */
      /* '<S9>:1:2' */
      /* Right plate offset from origin */
      /* '<S9>:1:1' */
      /* '<S9>:1:13' */
      /* '<S9>:1:1' */
      /* manually demux the collected signals */
      /* '<S9>:1:23' */
      eml_LON = SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[0];

      /* '<S9>:1:24' */
      eml_LMx = SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[1];

      /* '<S9>:1:25' */
      eml_LFy = SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[2];

      /* '<S9>:1:26' */
      eml_LFz = SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[3];

      /* '<S9>:1:27' */
      /* '<S9>:1:28' */
      eml_RMx = SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[5];

      /* '<S9>:1:29' */
      eml_RFy = SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[6];

      /* '<S9>:1:30' */
      eml_RFz = SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[7];

      /* '<S9>:1:1' */
      if ((eml_LON == 1.0) && (SD_jf_median_B.TmpHiddenBufferAtSFunctionInpor[4]
           == 1.0)) {
        /* '<S9>:1:37' */
        /* double support */
        /* '<S9>:1:38' */
        SD_jf_median_B.BackCoP = eml_LMx / eml_LFz;

        /* '<S9>:1:39' */
        eml_LON = eml_RMx / eml_RFz + 0.00295;
        if (eml_LON < SD_jf_median_B.BackCoP) {
          /* '<S9>:1:40' */
          /*  Right is back leg */
          /* '<S9>:1:41' */
          SD_jf_median_B.BackCoP = eml_LON;

          /* '<S9>:1:42' */
          if ((eml_RFz < SD_jf_median_DWork.minFz) || rtIsNaN(eml_RFz)) {
          } else {
            SD_jf_median_DWork.minFz = eml_RFz;
          }

          SD_jf_median_B.Hip = eml_LON - eml_RFy / sqrt(rt_pow_snf(eml_RFy, 2.0)
            + rt_pow_snf(SD_jf_median_DWork.minFz, 2.0));

          /* '<S9>:1:44' */
          eml_LFz = eml_RFz;
        } else {
          /*  Left is back leg */
          /* '<S9>:1:46' */
          /* '<S9>:1:47' */
          if ((eml_LFz < SD_jf_median_DWork.minFz) || rtIsNaN(eml_LFz)) {
          } else {
            SD_jf_median_DWork.minFz = eml_LFz;
          }

          SD_jf_median_B.Hip = SD_jf_median_B.BackCoP - eml_LFy / sqrt
            (rt_pow_snf(eml_LFy, 2.0) + rt_pow_snf(SD_jf_median_DWork.minFz, 2.0));

          /* '<S9>:1:49' */
        }

        /* grow current stride */
        /* '<S9>:1:53' */
        SD_jf_median_DWork.curr_struct.last_state = 2.0;

        /* '<S9>:1:54' */
        SD_jf_median_DWork.curr_struct.stride = SD_jf_median_B.Hip;

        /* '<S9>:1:55' */
        SD_jf_median_DWork.curr_struct.len = SD_jf_median_DWork.curr_struct.len
          + 1.0;

        /* sliding median */
        /* '<S9>:1:58' */
        for (eml_i0 = 0; eml_i0 < 500; eml_i0++) {
          eml_dv0[eml_i0] = SD_jf_median_DWork.curr_struct.slide[eml_i0];
        }

        SD_jf_median_circshift(eml_dv0, SD_jf_median_DWork.curr_struct.slide);

        /* '<S9>:1:59' */
        SD_jf_median_DWork.curr_struct.slide[499] = SD_jf_median_B.Hip;

        /* '<S9>:1:60' */
        SD_jf_median_DWork.curr_struct.run_med = SD_jf_median_median
          (SD_jf_median_DWork.curr_struct.slide);

        /* '<S9>:1:61' */
        SD_jf_median_DWork.new_stride = 0.0;
        if (eml_LFz > SD_jf_median_DWork.curr_struct.maxFz) {
          /* '<S9>:1:62' */
          /* '<S9>:1:63' */
          SD_jf_median_DWork.curr_struct.maxFz = eml_LFz;
        }
      } else if (eml_LON == 1.0) {
        /* '<S9>:1:66' */
        /*  Left single support */
        /* '<S9>:1:67' */
        SD_jf_median_B.BackCoP = eml_LMx / eml_LFz;

        /* '<S9>:1:68' */
        if ((eml_LFz < SD_jf_median_DWork.minFz) || rtIsNaN(eml_LFz)) {
        } else {
          SD_jf_median_DWork.minFz = eml_LFz;
        }

        SD_jf_median_B.Hip = SD_jf_median_B.BackCoP - eml_LFy / sqrt(rt_pow_snf
          (eml_LFy, 2.0) + rt_pow_snf(SD_jf_median_DWork.minFz, 2.0));
        if (SD_jf_median_DWork.curr_struct.last_state == 2.0) {
          /* '<S9>:1:71' */
          /* new stride detected */
          /* update mp_vec */
          /* '<S9>:1:73' */
          /* '<S9>:1:74' */
          SD_jf_median_DWork.curr_struct.mp_vec[2] =
            SD_jf_median_DWork.curr_struct.stride;

          /* rest stride */
          /* '<S9>:1:77' */
          SD_jf_median_DWork.curr_struct.stride = SD_jf_median_B.Hip;

          /* holds hip data for the current stride */
          /* '<S9>:1:78' */
          SD_jf_median_DWork.curr_struct.len = 1.0;

          /* '<S9>:1:79' */
          SD_jf_median_DWork.curr_struct.maxFz = eml_LFz;

          /* '<S9>:1:80' */
          /* '<S9>:1:81' */
          SD_jf_median_DWork.curr_struct.stime[2] = SD_jf_median_B.Clock;

          /* '<S9>:1:82' */
          /* '<S9>:1:83' */
          SD_jf_median_DWork.I = SD_jf_median_DWork.I +
            (SD_jf_median_DWork.curr_struct.stime[2] -
             SD_jf_median_DWork.curr_struct.stime[1]) *
            (SD_jf_median_DWork.curr_struct.mp_vec[2] - 0.9);

          /* sliding median */
          /* '<S9>:1:86' */
          for (eml_i0 = 0; eml_i0 < 500; eml_i0++) {
            eml_dv0[eml_i0] = SD_jf_median_DWork.curr_struct.slide[eml_i0];
          }

          SD_jf_median_circshift(eml_dv0, SD_jf_median_DWork.curr_struct.slide);

          /* '<S9>:1:87' */
          SD_jf_median_DWork.curr_struct.slide[499] = SD_jf_median_B.Hip;

          /* '<S9>:1:88' */
          SD_jf_median_DWork.curr_struct.run_med = SD_jf_median_median
            (SD_jf_median_DWork.curr_struct.slide);

          /* '<S9>:1:89' */
          SD_jf_median_DWork.curr_struct.maxFz = eml_LFz;

          /* '<S9>:1:90' */
          SD_jf_median_DWork.new_stride = 1.0;
        } else {
          /* grow current stride */
          /* '<S9>:1:93' */
          SD_jf_median_DWork.curr_struct.stride = SD_jf_median_B.Hip;

          /* '<S9>:1:94' */
          SD_jf_median_DWork.curr_struct.len =
            SD_jf_median_DWork.curr_struct.len + 1.0;

          /* sliding median */
          /* '<S9>:1:97' */
          for (eml_i0 = 0; eml_i0 < 500; eml_i0++) {
            eml_dv0[eml_i0] = SD_jf_median_DWork.curr_struct.slide[eml_i0];
          }

          SD_jf_median_circshift(eml_dv0, SD_jf_median_DWork.curr_struct.slide);

          /* '<S9>:1:98' */
          SD_jf_median_DWork.curr_struct.slide[499] = SD_jf_median_B.Hip;

          /* '<S9>:1:99' */
          SD_jf_median_DWork.curr_struct.run_med = SD_jf_median_median
            (SD_jf_median_DWork.curr_struct.slide);

          /* '<S9>:1:100' */
          SD_jf_median_DWork.curr_struct.maxFz = eml_LFz;

          /* '<S9>:1:101' */
          SD_jf_median_DWork.new_stride = 0.0;
          if (eml_LFz > SD_jf_median_DWork.curr_struct.maxFz) {
            /* '<S9>:1:102' */
            /* '<S9>:1:103' */
            SD_jf_median_DWork.curr_struct.maxFz = eml_LFz;
          }
        }

        /* '<S9>:1:107' */
        SD_jf_median_DWork.curr_struct.last_state = 1.0;
      } else {
        /*  Right Single support */
        /* '<S9>:1:109' */
        SD_jf_median_B.BackCoP = eml_RMx / eml_RFz + 0.00295;

        /* '<S9>:1:110' */
        if ((eml_RFz < SD_jf_median_DWork.minFz) || rtIsNaN(eml_RFz)) {
        } else {
          SD_jf_median_DWork.minFz = eml_RFz;
        }

        SD_jf_median_B.Hip = SD_jf_median_B.BackCoP - eml_RFy / sqrt(rt_pow_snf
          (eml_RFy, 2.0) + rt_pow_snf(SD_jf_median_DWork.minFz, 2.0));
        if (SD_jf_median_DWork.curr_struct.last_state == 2.0) {
          /* '<S9>:1:114' */
          /* new stride detected */
          /* update mp_vec */
          /* '<S9>:1:116' */
          /* '<S9>:1:117' */
          SD_jf_median_DWork.curr_struct.mp_vec[2] =
            SD_jf_median_DWork.curr_struct.stride;

          /* reset stride */
          /* '<S9>:1:120' */
          SD_jf_median_DWork.curr_struct.stride = SD_jf_median_B.Hip;

          /* holds hip data for the current stride */
          /* '<S9>:1:121' */
          SD_jf_median_DWork.curr_struct.len = 1.0;

          /* '<S9>:1:122' */
          SD_jf_median_DWork.curr_struct.maxFz = eml_RFz;

          /* '<S9>:1:123' */
          /* '<S9>:1:124' */
          SD_jf_median_DWork.curr_struct.stime[2] = SD_jf_median_B.Clock;

          /* '<S9>:1:126' */
          /* '<S9>:1:127' */
          SD_jf_median_DWork.I = SD_jf_median_DWork.I +
            (SD_jf_median_DWork.curr_struct.stime[2] -
             SD_jf_median_DWork.curr_struct.stime[1]) *
            (SD_jf_median_DWork.curr_struct.mp_vec[2] - 0.9);

          /* sliding median */
          /* '<S9>:1:130' */
          for (eml_i0 = 0; eml_i0 < 500; eml_i0++) {
            eml_dv0[eml_i0] = SD_jf_median_DWork.curr_struct.slide[eml_i0];
          }

          SD_jf_median_circshift(eml_dv0, SD_jf_median_DWork.curr_struct.slide);

          /* '<S9>:1:131' */
          SD_jf_median_DWork.curr_struct.slide[499] = SD_jf_median_B.Hip;

          /* '<S9>:1:132' */
          SD_jf_median_DWork.curr_struct.run_med = SD_jf_median_median
            (SD_jf_median_DWork.curr_struct.slide);

          /* '<S9>:1:133' */
          SD_jf_median_DWork.curr_struct.maxFz = eml_RFz;

          /* '<S9>:1:134' */
          SD_jf_median_DWork.new_stride = 1.0;
        } else {
          /* grow current stride */
          /* '<S9>:1:137' */
          SD_jf_median_DWork.curr_struct.stride = SD_jf_median_B.Hip;

          /* '<S9>:1:138' */
          SD_jf_median_DWork.curr_struct.len =
            SD_jf_median_DWork.curr_struct.len + 1.0;

          /* sliding median */
          /* '<S9>:1:141' */
          for (eml_i0 = 0; eml_i0 < 500; eml_i0++) {
            eml_dv0[eml_i0] = SD_jf_median_DWork.curr_struct.slide[eml_i0];
          }

          SD_jf_median_circshift(eml_dv0, SD_jf_median_DWork.curr_struct.slide);

          /* '<S9>:1:142' */
          SD_jf_median_DWork.curr_struct.slide[499] = SD_jf_median_B.Hip;

          /* '<S9>:1:143' */
          SD_jf_median_DWork.curr_struct.run_med = SD_jf_median_median
            (SD_jf_median_DWork.curr_struct.slide);

          /* '<S9>:1:144' */
          SD_jf_median_DWork.curr_struct.maxFz = eml_RFz;

          /* '<S9>:1:145' */
          SD_jf_median_DWork.new_stride = 0.0;
          if (eml_RFz > SD_jf_median_DWork.curr_struct.maxFz) {
            /* '<S9>:1:146' */
            /* '<S9>:1:147' */
            SD_jf_median_DWork.curr_struct.maxFz = eml_RFz;
          }
        }

        /* '<S9>:1:151' */
        SD_jf_median_DWork.curr_struct.last_state = 1.0;
      }

      /* '<S9>:1:153' */
      SD_jf_median_DWork.minFz = 0.8 * SD_jf_median_DWork.curr_struct.maxFz;

      /* manual  triggered PID     */
      /* '<S9>:1:155' */
      SD_jf_median_diff(SD_jf_median_DWork.curr_struct.mp_vec, eml_unusedExpr);
      SD_jf_median_diff(SD_jf_median_DWork.curr_struct.stime, eml_unusedExpr_0);

      /* velocity of last two strides */
      /* most recent position error */
      /* integral error */
      /*  most recent velocity  (of error) */
      /* '<S9>:1:160' */
      /* '<S9>:1:161' */
      /* '<S9>:1:162' */
      SD_jf_median_B.run_med = SD_jf_median_DWork.curr_struct.run_med;
      SD_jf_median_B.new_s = SD_jf_median_DWork.new_stride;
      SD_jf_median_B.mp = SD_jf_median_DWork.curr_struct.mp_vec[2];
    }

    if (rtmIsMajorTimeStep(SD_jf_median_M)) {
    }

    /* Gain: '<S13>/Proportional Gain' */
    SD_jf_median_B.ProportionalGain = SD_jf_median_P.ProportionalGain_Gain *
      SD_jf_median_B.run_med;

    /* Integrator: '<S13>/Integrator' */
    SD_jf_median_B.Integrator = SD_jf_median_X.Integrator_CSTATE;

    /* Gain: '<S13>/Derivative Gain' */
    SD_jf_median_B.DerivativeGain = SD_jf_median_P.DerivativeGain_Gain *
      SD_jf_median_B.run_med;

    /* Derivative Block: '<S13>/Ideal Derivative' */
    {
      real_T t = SD_jf_median_M->Timing.t[0];
      real_T timeStampA = SD_jf_median_DWork.IdealDerivative_RWORK.TimeStampA;
      real_T timeStampB = SD_jf_median_DWork.IdealDerivative_RWORK.TimeStampB;
      if (timeStampA >= t && timeStampB >= t) {
        SD_jf_median_B.IdealDerivative = 0.0;
      } else {
        real_T deltaT;
        real_T *lastBank = &SD_jf_median_DWork.IdealDerivative_RWORK.TimeStampA;
        if (timeStampA < timeStampB) {
          if (timeStampB < t) {
            lastBank += 2;
          }
        } else if (timeStampA >= t) {
          lastBank += 2;
        }

        deltaT = t - *lastBank++;
        SD_jf_median_B.IdealDerivative = (SD_jf_median_B.DerivativeGain -
          *lastBank++) / deltaT;
      }
    }

    /* Sum: '<S13>/Sum' */
    SD_jf_median_B.Sum = (SD_jf_median_B.ProportionalGain +
                          SD_jf_median_B.Integrator) +
      SD_jf_median_B.IdealDerivative;

    /* Embedded MATLAB: '<Root>/Embedded MATLAB Function1' */
    /* Embedded MATLAB Function 'Embedded MATLAB Function1': '<S10>:1' */
    /* '<S10>:1:2' */
    if (SD_jf_median_DWork.thrown == 1.0) {
      /* '<S10>:1:7' */
      /* '<S10>:1:8' */
      SD_jf_median_B.Vcomm = 0.0;
    } else if (SD_jf_median_B.BackCoP <= 0.3) {
      /* '<S10>:1:9' */
      /* '<S10>:1:10' */
      SD_jf_median_DWork.thrown = 1.0;

      /* '<S10>:1:11' */
      SD_jf_median_B.Vcomm = 0.0;
    } else {
      /* '<S10>:1:13' */
      SD_jf_median_B.Vcomm = SD_jf_median_B.Sum;
    }

    if (rtmIsMajorTimeStep(SD_jf_median_M)) {
    }

    /* Gain: '<S13>/Integral Gain' */
    SD_jf_median_B.IntegralGain = SD_jf_median_P.IntegralGain_Gain *
      SD_jf_median_B.run_med;

    /* user code (Output function Trailer) */
    /* dSPACE Data Capture block: (none) */
    /* ... Service number: 1 */
    /* ... Service name:   (default) */
    if (rtmIsContinuousTask(SD_jf_median_M, 0) && rtmIsMajorTimeStep
        (SD_jf_median_M)) {
      host_service(1, rtk_current_task_absolute_time_ptr_get());
    }
  }

  /* tid is required for a uniform function interface.
   * Argument tid is not used in the function. */
  UNUSED_PARAMETER(tid);
}

/* Model update function */
void SD_jf_median_update(int_T tid)
{
  /* Derivative Block: '<S13>/Ideal Derivative' */
  {
    real_T timeStampA = SD_jf_median_DWork.IdealDerivative_RWORK.TimeStampA;
    real_T timeStampB = SD_jf_median_DWork.IdealDerivative_RWORK.TimeStampB;
    real_T *lastBank = &SD_jf_median_DWork.IdealDerivative_RWORK.TimeStampA;
    if (timeStampA != rtInf) {
      if (timeStampB == rtInf) {
        lastBank += 2;
      } else if (timeStampA >= timeStampB) {
        lastBank += 2;
      }
    }

    *lastBank++ = SD_jf_median_M->Timing.t[0];
    *lastBank++ = SD_jf_median_B.DerivativeGain;
  }

  if (rtmIsMajorTimeStep(SD_jf_median_M)) {
    rt_ertODEUpdateContinuousStates(&SD_jf_median_M->solverInfo);
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++SD_jf_median_M->Timing.clockTick0))
    ++SD_jf_median_M->Timing.clockTickH0;
  SD_jf_median_M->Timing.t[0] = SD_jf_median_M->Timing.clockTick0 *
    SD_jf_median_M->Timing.stepSize0 + SD_jf_median_M->Timing.clockTickH0 *
    SD_jf_median_M->Timing.stepSize0 * 4294967296.0;
  if (rtmIsMajorTimeStep(SD_jf_median_M)) {
    /* Update absolute timer for sample time: [0.001s, 0.0s] */
    /* The "clockTick1" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick1"
     * and "Timing.stepSize1". Size of "clockTick1" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick1 and the high bits
     * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++SD_jf_median_M->Timing.clockTick1))
      ++SD_jf_median_M->Timing.clockTickH1;
    SD_jf_median_M->Timing.t[1] = SD_jf_median_M->Timing.clockTick1 *
      SD_jf_median_M->Timing.stepSize1 + SD_jf_median_M->Timing.clockTickH1 *
      SD_jf_median_M->Timing.stepSize1 * 4294967296.0;
  }

  /* tid is required for a uniform function interface.
   * Argument tid is not used in the function. */
  UNUSED_PARAMETER(tid);
}

/* Derivatives for root system: '<Root>' */
void SD_jf_median_derivatives(void)
{
  /* Derivatives for Integrator: '<S13>/Integrator' */
  ((StateDerivatives_SD_jf_median *) SD_jf_median_M->ModelData.derivs)
    ->Integrator_CSTATE = SD_jf_median_B.IntegralGain;
}

/* Model initialize function */
void SD_jf_median_initialize(boolean_T firstTime)
{
  (void)firstTime;

  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)SD_jf_median_M,0,
                sizeof(RT_MODEL_SD_jf_median));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&SD_jf_median_M->solverInfo,
                          &SD_jf_median_M->Timing.simTimeStep);
    rtsiSetTPtr(&SD_jf_median_M->solverInfo, &rtmGetTPtr(SD_jf_median_M));
    rtsiSetStepSizePtr(&SD_jf_median_M->solverInfo,
                       &SD_jf_median_M->Timing.stepSize0);
    rtsiSetdXPtr(&SD_jf_median_M->solverInfo, &SD_jf_median_M->ModelData.derivs);
    rtsiSetContStatesPtr(&SD_jf_median_M->solverInfo,
                         &SD_jf_median_M->ModelData.contStates);
    rtsiSetNumContStatesPtr(&SD_jf_median_M->solverInfo,
      &SD_jf_median_M->Sizes.numContStates);
    rtsiSetErrorStatusPtr(&SD_jf_median_M->solverInfo, (&rtmGetErrorStatus
      (SD_jf_median_M)));
    rtsiSetRTModelPtr(&SD_jf_median_M->solverInfo, SD_jf_median_M);
  }

  rtsiSetSimTimeStep(&SD_jf_median_M->solverInfo, MAJOR_TIME_STEP);
  SD_jf_median_M->ModelData.intgData.f[0] = SD_jf_median_M->ModelData.odeF[0];
  SD_jf_median_M->ModelData.contStates = ((real_T *) &SD_jf_median_X);
  rtsiSetSolverData(&SD_jf_median_M->solverInfo, (void *)
                    &SD_jf_median_M->ModelData.intgData);
  rtsiSetSolverName(&SD_jf_median_M->solverInfo,"ode1");

  /* Initialize timing info */
  {
    int_T *mdlTsMap = SD_jf_median_M->Timing.sampleTimeTaskIDArray;
    mdlTsMap[0] = 0;
    mdlTsMap[1] = 1;
    SD_jf_median_M->Timing.sampleTimeTaskIDPtr = (&mdlTsMap[0]);
    SD_jf_median_M->Timing.sampleTimes =
      (&SD_jf_median_M->Timing.sampleTimesArray[0]);
    SD_jf_median_M->Timing.offsetTimes =
      (&SD_jf_median_M->Timing.offsetTimesArray[0]);

    /* task periods */
    SD_jf_median_M->Timing.sampleTimes[0] = (0.0);
    SD_jf_median_M->Timing.sampleTimes[1] = (0.001);

    /* task offsets */
    SD_jf_median_M->Timing.offsetTimes[0] = (0.0);
    SD_jf_median_M->Timing.offsetTimes[1] = (0.0);
  }

  rtmSetTPtr(SD_jf_median_M, &SD_jf_median_M->Timing.tArray[0]);

  {
    int_T *mdlSampleHits = SD_jf_median_M->Timing.sampleHitArray;
    mdlSampleHits[0] = 1;
    mdlSampleHits[1] = 1;
    SD_jf_median_M->Timing.sampleHits = (&mdlSampleHits[0]);
  }

  rtmSetTFinal(SD_jf_median_M, -1);
  SD_jf_median_M->Timing.stepSize0 = 0.001;
  SD_jf_median_M->Timing.stepSize1 = 0.001;
  SD_jf_median_M->solverInfoPtr = (&SD_jf_median_M->solverInfo);
  SD_jf_median_M->Timing.stepSize = (0.001);
  rtsiSetFixedStepSize(&SD_jf_median_M->solverInfo, 0.001);
  rtsiSetSolverMode(&SD_jf_median_M->solverInfo, SOLVER_MODE_SINGLETASKING);

  /* block I/O */
  SD_jf_median_M->ModelData.blockIO = ((void *) &SD_jf_median_B);
  (void) memset(((void *) &SD_jf_median_B),0,
                sizeof(BlockIO_SD_jf_median));

  /* parameters */
  SD_jf_median_M->ModelData.defaultParam = ((real_T *) &SD_jf_median_P);

  /* states (continuous) */
  {
    real_T *x = (real_T *) &SD_jf_median_X;
    SD_jf_median_M->ModelData.contStates = (x);
    (void) memset((void *)x,0,
                  sizeof(ContinuousStates_SD_jf_median));
  }

  /* states (dwork) */
  SD_jf_median_M->Work.dwork = ((void *) &SD_jf_median_DWork);
  (void) memset((void *)&SD_jf_median_DWork, 0,
                sizeof(D_Work_SD_jf_median));

  {
    /* user code (registration function declaration) */
    /*Call the macro that initializes the global TRC pointers
       inside the model initialization/registration function. */
    RTI_INIT_TRC_POINTERS();
  }
}

/* Model terminate function */
void SD_jf_median_terminate(void)
{
  /* (no terminate code required) */
}

/*========================================================================*
 * Start of GRT compatible call interface                                 *
 *========================================================================*/
void MdlOutputs(int_T tid)
{
  SD_jf_median_output(tid);
}

void MdlUpdate(int_T tid)
{
  SD_jf_median_update(tid);
}

void MdlInitializeSizes(void)
{
  SD_jf_median_M->Sizes.numContStates = (1);/* Number of continuous states */
  SD_jf_median_M->Sizes.numY = (0);    /* Number of model outputs */
  SD_jf_median_M->Sizes.numU = (0);    /* Number of model inputs */
  SD_jf_median_M->Sizes.sysDirFeedThru = (0);/* The model is not direct feedthrough */
  SD_jf_median_M->Sizes.numSampTimes = (2);/* Number of sample times */
  SD_jf_median_M->Sizes.numBlocks = (52);/* Number of blocks */
  SD_jf_median_M->Sizes.numBlockIO = (32);/* Number of block outputs */
  SD_jf_median_M->Sizes.numBlockPrms = (132);/* Sum of parameter "widths" */
}

void MdlInitializeSampleTimes(void)
{
}

void MdlInitialize(void)
{
  {
    int32_T i;
    for (i = 0; i < 8; i++) {
      /* InitializeConditions for S-Function (sdspbiquad): '<S3>/Digital Filter' */
      SD_jf_median_DWork.DigitalFilter_FILT_STATES[i] =
        SD_jf_median_ConstP.pooled1;

      /* InitializeConditions for S-Function (sdspbiquad): '<S1>/Digital Filter' */
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m[i] =
        SD_jf_median_ConstP.pooled1;

      /* InitializeConditions for S-Function (sdspbiquad): '<S2>/Digital Filter' */
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_m5[i] =
        SD_jf_median_ConstP.pooled1;

      /* InitializeConditions for S-Function (sdspbiquad): '<S6>/Digital Filter' */
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_my[i] =
        SD_jf_median_ConstP.pooled1;

      /* InitializeConditions for S-Function (sdspbiquad): '<S4>/Digital Filter' */
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_h[i] =
        SD_jf_median_ConstP.pooled1;

      /* InitializeConditions for S-Function (sdspbiquad): '<S5>/Digital Filter' */
      SD_jf_median_DWork.DigitalFilter_FILT_STATES_p[i] =
        SD_jf_median_ConstP.pooled1;
    }

    /* Initialize code for chart: '<Root>/Embedded MATLAB Function' */
    {
      static struct sPk8KX86oBjv2lJavBJkkIB eml_r0 = { { 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
          0.9, 0.9, 0.9, 0.9, 0.9 }, 0.9, 1.0, 200.0, { 0.9, 0.9, 0.9 }, 0.9,
        0.0, { 0.0, 0.0, 0.0 } };

      SD_jf_median_DWork.I = 0.0;
      SD_jf_median_DWork.new_stride = 0.0;
      SD_jf_median_DWork.curr_struct = eml_r0;
      SD_jf_median_DWork.minFz = 800.0;
    }

    /* InitializeConditions for Integrator: '<S13>/Integrator' */
    SD_jf_median_X.Integrator_CSTATE = SD_jf_median_P.Integrator_IC;

    /* Derivative Block: '<S13>/Ideal Derivative' */
    SD_jf_median_DWork.IdealDerivative_RWORK.TimeStampA = rtInf;
    SD_jf_median_DWork.IdealDerivative_RWORK.TimeStampB = rtInf;

    /* InitializeConditions for Embedded MATLAB: '<Root>/Embedded MATLAB Function1' */
    SD_jf_median_DWork.thrown = 0.0;
  }
}

void MdlStart(void)
{
  MdlInitialize();
}

RT_MODEL_SD_jf_median *SD_jf_median(void)
{
  SD_jf_median_initialize(1);
  return SD_jf_median_M;
}

void MdlTerminate(void)
{
  SD_jf_median_terminate();
}

/*========================================================================*
 * End of GRT compatible call interface                                   *
 *========================================================================*/
