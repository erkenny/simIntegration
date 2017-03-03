/* Copyright 2007 The MathWorks, Inc. */
/* Define imported data */
#ifndef _DEFINEIMPORTDATA_
#define _DEFINEIMPORTDATA_
#include "rtwtypes.h"

/*********
#ifdef EXTERN_C
  #undef EXTERN_C
  #endif

  #ifdef __cplusplus
  #define EXTERN_C                       extern "C"
  #else
  #define EXTERN_C                       extern
  #endif
/*********/
#ifdef __cplusplus
extern "C" {
#endif

extern real_T fbk_1;
extern real_T fbk_2;
extern real_T *pos_rqst; 

/* Define the external function */
extern void defineImportData(void);
#ifdef __cplusplus
}
#endif

#endif               
 

