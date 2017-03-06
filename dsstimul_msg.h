/******************************************************************************
  FILE:
    dsstimul_msg.h

  DESCRIPTION:
    Stimulus Engine message module definitions

  REMARKS:

  AUTHOR(S): A. Trepel

  Copyright (c) 1999 dSPACE GmbH, GERMANY

  $RCSfile: dsstimul_msg.h $ $Revision: 1.3 $ $Date: 2003/10/21 13:30:47GMT+01:00 $
  $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSTIMUL.pj $
******************************************************************************/
#ifndef _TA_STIMULUS_ENGINE_MSG_H_
#define _TA_STIMULUS_ENGINE_MSG_H_

#if !defined(_DSHOST) && !defined(MATLAB_MEX_FILE)
  #include "dsmsg.h"
#endif

/* *** Definitions of Errors *** */

/* *** Numbers *** */

#define TA_STIMUL_ALLOC_ERROR       100
#define TA_STIMUL_ZERO_DEVISION     101

/* *** Text strings *** */

#ifdef _DSHOST
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine."
#elif defined _DS1102
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine on DS1102 failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine on DS1102."
#elif defined _DS1003
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine on DS1003 failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine on DS1003."
#elif defined _DS1004
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine on DS1004 failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine on DS1004."
#elif defined _DS1103
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine on DS1103 failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine on DS1103."
#elif defined _DS1005
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine on DS1005 failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine on DS1005."
#elif defined _DS1006 
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine on DS1006 failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine on DS1006."
#elif defined _DS1401
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine on DS1401 failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine on DS1401."
#elif defined _DS1104
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine on DS1104 failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine on DS1104."
#elif defined MATLAB_MEX_FILE
  #define TA_STIMUL_ALLOC_ERROR_TXT	    "Allocation of memory for Stimulus Engine in Simulink S-function failed."
  #define TA_STIMUL_ZERO_DEVISION_TXT	"Zero division within Stimulus Engine in Simulink S-function ."
#else
  #error target platform undefined
#endif
 
/* *** Definitions of Warnings *** */

/* none warnings */

/* *** Definitions of Infos *** */

/* none infos */

/* *** Definition of macro *** */

#if defined _DSHOST

  #define TA_STIMUL_MSG_ERROR_SET(err) 

#elif defined MATLAB_MEX_FILE

  extern int TASL_Error;
  #define TA_STIMUL_MSG_ERROR_SET(err) TASL_Error = err
  #define TA_STIMUL_MSG_ERROR_MSG(err) (err)==TA_STIMUL_ALLOC_ERROR?TA_STIMUL_ALLOC_ERROR_TXT:TA_STIMUL_ZERO_DEVISION_TXT

#else

  #define TA_STIMUL_MSG_ERROR_SET(err) msg_error_set(MSG_SM_TA_STIMUL, err, err##_TXT)

#endif




#endif /* _TA_STIMULUS_ENGINE_MSG_H_ */
