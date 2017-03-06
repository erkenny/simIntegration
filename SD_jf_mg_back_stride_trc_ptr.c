/***************************************************************************

   Source file SD_jf_mg_back_stride_trc_ptr.c:

   Definition of function that initializes the global TRC pointers

   RTI1103 6.4 (03-Dec-2009)
   Sun Feb 12 17:44:28 2017

   (c) Copyright 2008, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

/* Include header file. */
#include "SD_jf_mg_back_stride_trc_ptr.h"

/* Definition of Global pointers to data type transitions (for TRC-file access) */
volatile real_T *p_SD_jf_mg_back_stride_B_real_T_0 = 0;
volatile uint8_T *p_SD_jf_mg_back_stride_B_uint8_T_1 = 0;
volatile real_T *p_SD_jf_mg_back_stride_P_real_T_0 = 0;
volatile real_T *p_SD_jf_mg_back_stride_DWork_real_T_1 = 0;
