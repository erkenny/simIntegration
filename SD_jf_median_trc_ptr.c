/***************************************************************************

   Source file SD_jf_median_trc_ptr.c:

   Definition of function that initializes the global TRC pointers

   RTI1103 6.4 (03-Dec-2009)
   Tue Feb 28 20:02:00 2017

   (c) Copyright 2008, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

/* Include header file. */
#include "SD_jf_median_trc_ptr.h"

/* Definition of Global pointers to data type transitions (for TRC-file access) */
volatile real_T *p_SD_jf_median_B_real_T_0 = 0;
volatile uint8_T *p_SD_jf_median_B_uint8_T_1 = 0;
volatile real_T *p_SD_jf_median_P_real_T_0 = 0;
volatile real_T *p_SD_jf_median_DWork_real_T_1 = 0;
volatile real_T *p_SD_jf_median_X_real_T_0 = 0;
