/* Copyright 2007 The MathWorks, Inc. */
/* Define structures */
#ifndef __throttlebus
#define __throttlebus


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


typedef struct  
{
    double pos_cmd_raw[2];
    double pos_cmd_act;
    double pos_failure_mode;
    double err_cnt;
} ThrottleCommands;

   
typedef struct 
{
    double fail_safe_pos;    
    double max_diff;    
    double error_reset;
} ThrottleParams;

#ifdef __cplusplus
}
#endif

#endif
