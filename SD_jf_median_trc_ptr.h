  /*********************** dSPACE target specific file *************************

   Header file SD_jf_median_trc_ptr.h:

   Declaration of function that initializes the global TRC pointers

   RTI1103 6.4 (03-Dec-2009)
   Tue Feb 28 20:02:00 2017

   (c) Copyright 2008, dSPACE GmbH. All rights reserved.

  *****************************************************************************/
  #ifndef RTI_HEADER_SD_jf_median_trc_ptr_h_
  #define RTI_HEADER_SD_jf_median_trc_ptr_h_

  #ifdef __cplusplus
  extern "C" {
  #endif

  /* Include the model header file. */
  #include "SD_jf_median.h"
  #include "SD_jf_median_private.h"
/*
  #ifdef EXTERN_C
  #undef EXTERN_C
  #endif

  #ifdef __cplusplus
  #define EXTERN_C                       extern "C"
  #else
  #define EXTERN_C                       extern
  #endif

  /*
   *  Declare the global TRC pointers
   */
              EXTERN_C volatile  real_T *p_SD_jf_median_B_real_T_0;
              EXTERN_C volatile  uint8_T *p_SD_jf_median_B_uint8_T_1;
              EXTERN_C volatile  real_T *p_SD_jf_median_P_real_T_0;
              EXTERN_C volatile  real_T *p_SD_jf_median_DWork_real_T_1;
              EXTERN_C volatile  real_T *p_SD_jf_median_X_real_T_0;

   #define RTI_INIT_TRC_POINTERS() \
              p_SD_jf_median_B_real_T_0 = &SD_jf_median_B.SFunction[0];\
              p_SD_jf_median_B_uint8_T_1 = &SD_jf_median_B.Compare;\
              p_SD_jf_median_P_real_T_0 = &SD_jf_median_P.CH15Raw_Gain;\
              p_SD_jf_median_DWork_real_T_1 = &SD_jf_median_DWork.DigitalFilter_FILT_STATES[0];\
              p_SD_jf_median_X_real_T_0 = &SD_jf_median_X.Integrator_CSTATE;\


   #ifdef __cplusplus
   }
   #endif

   #endif                       /* RTI_HEADER_SD_jf_median_trc_ptr_h_ */
