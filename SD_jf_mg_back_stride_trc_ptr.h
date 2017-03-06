  /*********************** dSPACE target specific file *************************

   Header file SD_jf_mg_back_stride_trc_ptr.h:

   Declaration of function that initializes the global TRC pointers

   RTI1103 6.4 (03-Dec-2009)
   Sun Feb 12 17:44:28 2017

   (c) Copyright 2008, dSPACE GmbH. All rights reserved.

  *****************************************************************************/
  #ifndef RTI_HEADER_SD_jf_mg_back_stride_trc_ptr_h_
  #define RTI_HEADER_SD_jf_mg_back_stride_trc_ptr_h_
  /* Include the model header file. */
  #include "SD_jf_mg_back_stride.h"
  #include "SD_jf_mg_back_stride_private.h"

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
              EXTERN_C volatile  real_T *p_SD_jf_mg_back_stride_B_real_T_0;
              EXTERN_C volatile  uint8_T *p_SD_jf_mg_back_stride_B_uint8_T_1;
              EXTERN_C volatile  real_T *p_SD_jf_mg_back_stride_P_real_T_0;
              EXTERN_C volatile  real_T *p_SD_jf_mg_back_stride_DWork_real_T_1;

   #define RTI_INIT_TRC_POINTERS() \
              p_SD_jf_mg_back_stride_B_real_T_0 = &SD_jf_mg_back_stride_B.SFunction[0];\
              p_SD_jf_mg_back_stride_B_uint8_T_1 = &SD_jf_mg_back_stride_B.Compare;\
              p_SD_jf_mg_back_stride_P_real_T_0 = &SD_jf_mg_back_stride_P.CH15Raw_Gain;\
              p_SD_jf_mg_back_stride_DWork_real_T_1 = &SD_jf_mg_back_stride_DWork.DigitalFilter_FILT_STATES[0];\

   #endif                       /* RTI_HEADER_SD_jf_mg_back_stride_trc_ptr_h_ */
