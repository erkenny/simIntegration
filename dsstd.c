/*******************************************************************************
*
*  MODULE
*
*  FILE
*    dsstd.c
*
*  RELATED FILES
*
*  DESCRIPTION
*    Standard RTLib Macros and Functions
*
*  REMARKS
*
*  AUTHOR(S)
*    V. Lang
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: dsstd.c $ $Revision: 1.13 $ $Date: 2009/02/18 08:11:04GMT+01:00 $
*******************************************************************************/

#include <dstypes.h>
#include <dsstd.h>

#if !defined _DS1102 && !defined _RP565
  #include <dsmodule.h>
  #include <dsvcm.h>
  #include <stdlib.h>
#endif

/* $DSVERSION: DSSTD -  dSPACE Standard Macros */

#define DSSTD_VER_MAR 1
#define DSSTD_VER_MIR 9
#define DSSTD_VER_MAI 1
#define DSSTD_VER_SPB VCM_VERSION_RELEASE
#define DSSTD_VER_SPN 0
#define DSSTD_VER_PLV 0

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 || defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401

DS_VERSION_SYMBOL(_dsstd_ver_mar, DSSTD_VER_MAR);
DS_VERSION_SYMBOL(_dsstd_ver_mir, DSSTD_VER_MIR);
DS_VERSION_SYMBOL(_dsstd_ver_mai, DSSTD_VER_MAI);
DS_VERSION_SYMBOL(_dsstd_ver_spb, DSSTD_VER_SPB);
DS_VERSION_SYMBOL(_dsstd_ver_spn, DSSTD_VER_SPN);
DS_VERSION_SYMBOL(_dsstd_ver_plv, DSSTD_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/*******************************************************************************
  object declarations
*******************************************************************************/

volatile UInt32 dsstd_init_flag = 0;

rtlib_bg_fcn_table_t rtlib_bg_fcn_table = {0, 0};

/*******************************************************************************
  function declarations
*******************************************************************************/

/******************************************************************************
*
* FUNCTION:
*   void dsstd_vcm_register (void)
*
* DESCRIPTION:
*   Registers the DSSTD module in the VCM
*
* PARAMETERS:
*   none
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   18.07.00  V. Lang, initial version
*
******************************************************************************/

#if !defined _DS1102 && !defined _RP565 && !defined _RP5554

void dsstd_vcm_register(void)
{
  vcm_module_descriptor_type *dsstd_module_ptr;

  if (vcm_module_status_get(vcm_module_find(VCM_MID_DSSTD, NULL)) != VCM_STATUS_INITIALIZED)
  {
    /* register memory management module */
    dsstd_module_ptr = vcm_module_register(VCM_MID_DSSTD,
                                           vcm_module_find(VCM_MID_RTLIB, NULL),
                                           VCM_TXT_DSSTD,
                                           DSSTD_VER_MAR, DSSTD_VER_MIR, DSSTD_VER_MAI,
                                           DSSTD_VER_SPB, DSSTD_VER_SPN, DSSTD_VER_PLV,
                                           0, 0);

    /* set status to 'initialized' */
    vcm_module_status_set(dsstd_module_ptr, VCM_STATUS_INITIALIZED);
  }
}
#endif

/*******************************************************************************
*
*  FUNCTION
*    rtlib_background_hook
*
*
*  SYNTAX
*    int rtlib_background_hook(rtlib_bg_fcn_t* fcnptr) 
*
*  DESCRIPTION
*    register a function to be cyclically executed in the background
*    several functions can be registered by susequent calls, whereas the
*    last registered function is called first
*
*  PARAMETERS
*    fcnptr: pointer to the function to be called
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

int rtlib_background_hook(rtlib_bg_fcn_t *fcnptr)
{
  /* resize function pointer array */
  rtlib_bg_fcn_table.fcnptr_table = (rtlib_bg_fcn_t **) realloc((void *) rtlib_bg_fcn_table.fcnptr_table, (rtlib_bg_fcn_table.length + 1) * sizeof(rtlib_bg_fcn_t *));

  if (!rtlib_bg_fcn_table.fcnptr_table)
    return 1;

  /*
   * the new hook function is added to the end of the table
   */
  rtlib_bg_fcn_table.fcnptr_table[rtlib_bg_fcn_table.length] = fcnptr;
  rtlib_bg_fcn_table.length++;

  return 0;
}

/*******************************************************************************
*
*  FUNCTION
*    rtlib_background_hook_process
*
*
*  SYNTAX
*    void rtlib_background_hook_process() 
*
*  DESCRIPTION
*    execute all background hook functions registered with the function
*    rtlib_background_hook(), beginning with the last registered function
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

void rtlib_background_hook_process()
{
  if (rtlib_bg_fcn_table.fcnptr_table)
  {
    int fcn_no;

    for (fcn_no = rtlib_bg_fcn_table.length - 1; fcn_no >= 0; fcn_no--)
    {
      rtlib_bg_fcn_table.fcnptr_table[fcn_no]();
    }
  }
}

/*****************************************************************************/

/** <!------------------------------------------------------------------------->
*   interrupt-protected calloc function.
*
*   @description
*     This function is protected against interupts. It can thus be used in the
*     RTLib background or lower priority tasks
*
*<!-------------------------------------------------------------------------->*/
void * rtlib_calloc(size_t nelem, size_t elsize)
{
    long status;
    void *ptr;

    RTLIB_INT_SAVE_AND_DISABLE(status);    
    ptr = calloc(nelem, elsize);                
    RTLIB_INT_RESTORE(status);             
    return ptr;
}

/** <!------------------------------------------------------------------------->
*   interrupt-protected free function.
*
*   @description
*     This function is protected against interrupts. It can thus be used in the
*     RTLib background or lower priority tasks
*
*<!-------------------------------------------------------------------------->*/
void   rtlib_free(void *ptr)
{
    long status;

    RTLIB_INT_SAVE_AND_DISABLE(status);
    free(ptr);                
    RTLIB_INT_RESTORE(status);  
}

/** <!------------------------------------------------------------------------->
*   interrupt-protected malloc function.
*
*   @description
*     This function is protected against interruption. It can thus be used in the
*     RTLib background or lower priority tasks
*
*<!-------------------------------------------------------------------------->*/
void * rtlib_malloc(size_t size)
{
    long status;
    void *ptr;

    RTLIB_INT_SAVE_AND_DISABLE(status);    
    ptr = malloc(size);                
    RTLIB_INT_RESTORE(status);             
    return ptr;
}

/** <!------------------------------------------------------------------------->
*   interrupt-protected realloc function.
*
*   @description
*     This function is protected against interruption. It can thus be used in the
*     RTLib background or lower priority tasks
*
*<!-------------------------------------------------------------------------->*/
void * rtlib_realloc(void *ptr, size_t size)
{
    long status;

    RTLIB_INT_SAVE_AND_DISABLE(status);    
    ptr = realloc(ptr, size); 
    RTLIB_INT_RESTORE(status);             
    return ptr;
}
