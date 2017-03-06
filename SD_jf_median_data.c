/*
 * SD_jf_median_data.c
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

#include "SD_jf_median.h"
#include "SD_jf_median_private.h"

/* Block parameters (auto storage) */
Parameters_SD_jf_median SD_jf_median_P = {
  10000.0,                             /* CH15Raw_Gain : '<Root>/CH15 Raw'
                                        */

  /*  DigitalFilter_RTP1COEFF : '<S3>/Digital Filter'
   */
  { 1.0, 2.0006113539350801E+000, 1.0002985532341100E+000, 1.0,
    2.0250193739316198E+000, 1.0253400499723999E+000, 1.0,
    1.9993892120670600E+000, 9.9970229664562504E-001, 1.0,
    1.9749800600662299E+000, 9.7528545678875400E-001 },

  /*  DigitalFilter_RTP2COEFF : '<S3>/Digital Filter'
   */
  { -1.7126128056791099E+000, 7.3396074887227702E-001, -1.7480119456744401E+000,
    7.6980112862158701E-001, -1.8174237773599100E+000, 8.4007819034566800E-001,
    -1.9168758297098201E+000, 9.4076992859855002E-001 },
  200.0,                               /* Constant_Value : '<S11>/Constant'
                                        */
  8500.0,                              /* CH13Raw_Gain : '<Root>/CH13 Raw'
                                        */

  /*  DigitalFilter_RTP1COEFF_j : '<S1>/Digital Filter'
   */
  { 1.0, 2.0006113539350801E+000, 1.0002985532341100E+000, 1.0,
    2.0250193739316198E+000, 1.0253400499723999E+000, 1.0,
    1.9993892120670600E+000, 9.9970229664562504E-001, 1.0,
    1.9749800600662299E+000, 9.7528545678875400E-001 },

  /*  DigitalFilter_RTP2COEFF_n : '<S1>/Digital Filter'
   */
  { -1.7126128056791099E+000, 7.3396074887227702E-001, -1.7480119456744401E+000,
    7.6980112862158701E-001, -1.8174237773599100E+000, 8.4007819034566800E-001,
    -1.9168758297098201E+000, 9.4076992859855002E-001 },
  10000.0,                             /* CH14Raw_Gain : '<Root>/CH14 Raw'
                                        */

  /*  DigitalFilter_RTP1COEFF_i : '<S2>/Digital Filter'
   */
  { 1.0, 2.0006113539350801E+000, 1.0002985532341100E+000, 1.0,
    2.0250193739316198E+000, 1.0253400499723999E+000, 1.0,
    1.9993892120670600E+000, 9.9970229664562504E-001, 1.0,
    1.9749800600662299E+000, 9.7528545678875400E-001 },

  /*  DigitalFilter_RTP2COEFF_b : '<S2>/Digital Filter'
   */
  { -1.7126128056791099E+000, 7.3396074887227702E-001, -1.7480119456744401E+000,
    7.6980112862158701E-001, -1.8174237773599100E+000, 8.4007819034566800E-001,
    -1.9168758297098201E+000, 9.4076992859855002E-001 },
  10000.0,                             /* CH11Raw_Gain : '<Root>/CH11 Raw'
                                        */

  /*  DigitalFilter_RTP1COEFF_l : '<S6>/Digital Filter'
   */
  { 1.0, 2.0006113539350801E+000, 1.0002985532341100E+000, 1.0,
    2.0250193739316198E+000, 1.0253400499723999E+000, 1.0,
    1.9993892120670600E+000, 9.9970229664562504E-001, 1.0,
    1.9749800600662299E+000, 9.7528545678875400E-001 },

  /*  DigitalFilter_RTP2COEFF_e : '<S6>/Digital Filter'
   */
  { -1.7126128056791099E+000, 7.3396074887227702E-001, -1.7480119456744401E+000,
    7.6980112862158701E-001, -1.8174237773599100E+000, 8.4007819034566800E-001,
    -1.9168758297098201E+000, 9.4076992859855002E-001 },
  200.0,                               /* Constant_Value_m : '<S12>/Constant'
                                        */
  8500.0,                              /* CH9Raw_Gain : '<Root>/CH9 Raw'
                                        */

  /*  DigitalFilter_RTP1COEFF_f : '<S4>/Digital Filter'
   */
  { 1.0, 2.0006113539350801E+000, 1.0002985532341100E+000, 1.0,
    2.0250193739316198E+000, 1.0253400499723999E+000, 1.0,
    1.9993892120670600E+000, 9.9970229664562504E-001, 1.0,
    1.9749800600662299E+000, 9.7528545678875400E-001 },

  /*  DigitalFilter_RTP2COEFF_o : '<S4>/Digital Filter'
   */
  { -1.7126128056791099E+000, 7.3396074887227702E-001, -1.7480119456744401E+000,
    7.6980112862158701E-001, -1.8174237773599100E+000, 8.4007819034566800E-001,
    -1.9168758297098201E+000, 9.4076992859855002E-001 },
  10000.0,                             /* CH10Raw_Gain : '<Root>/CH10 Raw'
                                        */

  /*  DigitalFilter_RTP1COEFF_o : '<S5>/Digital Filter'
   */
  { 1.0, 2.0006113539350801E+000, 1.0002985532341100E+000, 1.0,
    2.0250193739316198E+000, 1.0253400499723999E+000, 1.0,
    1.9993892120670600E+000, 9.9970229664562504E-001, 1.0,
    1.9749800600662299E+000, 9.7528545678875400E-001 },

  /*  DigitalFilter_RTP2COEFF_el : '<S5>/Digital Filter'
   */
  { -1.7126128056791099E+000, 7.3396074887227702E-001, -1.7480119456744401E+000,
    7.6980112862158701E-001, -1.8174237773599100E+000, 8.4007819034566800E-001,
    -1.9168758297098201E+000, 9.4076992859855002E-001 },
  1.0,                                 /* ProportionalGain_Gain : '<S13>/Proportional Gain'
                                        */
  0.0,                                 /* Integrator_IC : '<S13>/Integrator'
                                        */
  1.0,                                 /* DerivativeGain_Gain : '<S13>/Derivative Gain'
                                        */
  0.0                                  /* IntegralGain_Gain : '<S13>/Integral Gain'
                                        */
};

/* Constant parameters (auto storage) */
const ConstParam_SD_jf_median SD_jf_median_ConstP = {
  /* Expression: a.IC
   * Referenced by blocks:
   * '<S1>/Digital Filter'
   * '<S2>/Digital Filter'
   * '<S3>/Digital Filter'
   * '<S4>/Digital Filter'
   * '<S5>/Digital Filter'
   * '<S6>/Digital Filter'
   */
  0.0,

  /* Computed Parameter: RTP3COEFF
   * Referenced by blocks:
   * '<S1>/Digital Filter'
   * '<S2>/Digital Filter'
   * '<S3>/Digital Filter'
   * '<S4>/Digital Filter'
   * '<S5>/Digital Filter'
   * '<S6>/Digital Filter'
   */
  { 9.8355911241042295E-010, 0.0, 0.0, 0.0, 0.0 }
};
