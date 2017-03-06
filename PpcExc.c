/******************************************************************************
*
* FILE:
*   PpcExc.c
*
* RELATED FILES:
*   PpcExc.h
*   cfgXXXX.h
*   dsXXXX.h
*
* DESCRIPTION:
*   arithmetical exception support for PPC
*
* HISTORY:
*   20.03.98  Rev 1.0  H.-G. Limberg   initial version (DS1103)
*   22.06.98  Rev 1.1  R. Busche       common version for PPC (DS1103 and DS1401)
*   24.07.00  Rev 1.2  V. Lang         support of DS1104
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: PpcExc.c $ $Revision: 1.4 $ $Date: 2004/08/17 17:31:23GMT+01:00 $
******************************************************************************/

#ifndef _PPCEXC_C_
#define _PPCEXC_C_


#include <PpcExc.h>

#ifdef _DS1005
  #include <cfg1005.h>
  #include <ds1005.h>
  #define PPC_CFG_STRUCT_START DS1005_CFG_STRUCT_ADDR
#endif

#ifdef _DS1104
  #include <cfg1104.h>
  #include <ds1104.h>
  #define PPC_CFG_STRUCT_START DS1104_CFG_STRUCT_ADDR
#endif

#ifdef _DS1103
  #include <cfg1103.h>
  #include <ds1103.h>
  #define PPC_CFG_STRUCT_START DS1103_GLOBAL_RAM_START
#endif


#ifdef _DS1401
  #include <cfg1401.h>
  #include <ds1401.h>
  #define PPC_CFG_STRUCT_START DS1401_GLOBAL_RAM_START
#endif

#include <dsvcm.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/
#define get_section_limits(start, size, sect)   \
  asm("lis 3, secthi("sect")");   \
  start = asm("ori 3, 3, sectlo("sect")"); \
  asm(" lis     3,hi(sizeof("sect"))");    \
  size  = asm(" ori     3,3,lo(sizeof("sect"))");


/******************************************************************************
  program global variables
******************************************************************************/
extern UInt32 ppc_exc_global_enabled;
extern UInt32 ppc_exc_mask_register;
extern UInt32* ptExcModeTable;
extern Ppc_Exc_Handler_Type* ptExcTable;
extern UInt32* ptExcCounterTable;

/******************************************************************************
  modul global variables
******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE
#endif

/******************************************************************************
*
* FUNCTION:
*   int ppc_exception_init(void)
*
* DESCRIPTION:
*   initializes the PPC exception handling module
*
* PARAMETERS:
*   none
*
* RETURNS:
*   none
*
* NOTE:
*   0       success
*   else    error
*
* HISTORY:
*   15.08.2004  Ralf G.B., initial version
*
******************************************************************************/
__INLINE int ppc_exception_init(void)
{
    exception_info_struct *exception_vcm_additonal_mem;
    vcm_module_descriptor_type *rtlib_version_ptr;
    vcm_module_descriptor_type *exception_vcm_entry;
    UInt32 text_start, text_size;


    rtlib_version_ptr = vcm_module_find(VCM_MID_RTLIB, NULL);
    if (rtlib_version_ptr == NULL)
        return 1;

    exception_vcm_entry = vcm_module_register(
        VCM_MID_PPCEXC,
        rtlib_version_ptr, 
        VCM_TXT_PPCEXC,
        PPCEXC_VER_MAR,
        PPCEXC_VER_MIR,
        PPCEXC_VER_MAI,
        PPCEXC_VER_SPB,
        PPCEXC_VER_SPN,
        PPCEXC_VER_PLV,
        0,
        0); 
    if (exception_vcm_entry == NULL)
        return 1;

    /*
    * allocate additional memory in cfg section
    */
    exception_vcm_additonal_mem = (exception_info_struct*) vcm_cfg_malloc(sizeof(exception_info_struct));
    if (exception_vcm_additonal_mem == NULL)
        return 1;
    
    memset(exception_vcm_additonal_mem, 0, sizeof(exception_info_struct));
    vcm_memory_ptr_set( exception_vcm_entry,
                        (vcm_cfg_mem_ptr_type)exception_vcm_additonal_mem,
                        sizeof(exception_info_struct));

    /*
     * write ELF section limits to VCM entry
     */
	get_section_limits(text_start, text_size, ".text");
    exception_vcm_additonal_mem->text_sect_start = text_start;
    exception_vcm_additonal_mem->text_sect_end = text_start + text_size;

    vcm_module_status_set(exception_vcm_entry, VCM_STATUS_INITIALIZED);

    return 0;
}




/******************************************************************************
*
* FUNCTION:
*   void ppc_exception_enable(UInt32 ExcID)
*
* DESCRIPTION:
*   clears the specified arithmetical exception bit in the mask register
*
* PARAMETERS:
*   ExcID : ID number of the exception, defined in Exc1401.h
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   20.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_exception_enable(UInt32 ExcID)
{
   ppc_exc_mask_register =  ppc_exc_mask_register & (0xFFFFFFFF ^ (1<<ExcID));


   if (ppc_exc_global_enabled)          /* if exceptions global enabled already */
   {                                       /* set enable bit immediately */
                                           /* else bit will be set in global enable function */

     if (ExcID > PPC_EXC_SNAN)     /* to set VE bit set ID to SNAN for all other invalid operations  */
       ExcID = PPC_EXC_SNAN;

     switch (ExcID)
     {
       case PPC_EXC_SNAN: asm(" mtfsb0  7");    /* clear all invalid operation sticky bits */
                             asm(" mtfsfi  2,0x0");
                             asm(" mtfsb0  12");
                             asm(" mtfsb0  22");
                             asm(" mtfsb0  23");
                             asm(" mtfsb1  24");   /* set FPSCR[VE] bit */

                     break;
       case PPC_EXC_DZE: asm(" mtfsb0  5");   /* clear FPSCR[ZX] bit */
                            asm(" mtfsb1  27");   /* set FPSCR[ZE] bit */

                     break;
       case PPC_EXC_FOV: asm(" mtfsb0  3");   /* clear FPSCR[OX] bit */
                            asm(" mtfsb1  25");   /* set FPSCR[OE] bit */

                     break;
       case PPC_EXC_UNF: asm(" mtfsb0  4");   /* clear FPSCR[UX] bit */
                            asm(" mtfsb1  26");   /* set FPSCR[UE] bit */

                     break;
       case PPC_EXC_INE: asm(" mtfsb0  6");   /* clear FPSCR[XX] bit */
                            asm(" mtfsb1  28");   /* set FPSCR[XE] bit */
                     break;
     }
   }
}



/******************************************************************************
*
* FUNCTION:
*   void ppc_exception_disable(UInt32 ExcID)
*
* DESCRIPTION:
*   sets the specified arithmetical exception bit in the mask register
*
* PARAMETERS:
*   ExcID : ID number of the exception, defined in Exc1401.h
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   20.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_exception_disable(UInt32 ExcID)
{
   ppc_exc_mask_register =  ppc_exc_mask_register | (1<<ExcID);

   if (ppc_exc_global_enabled)          /* if exceptions still global enabled */
   {                                       /* clear enable bit immediately */
                                           /* else bit is already cleared */
     if (ExcID > PPC_EXC_SNAN)     /* to set VE bit set ID to SNAN for all other invalid operations  */
       ExcID = PPC_EXC_SNAN;

     switch (ExcID)
     {                                                         /* if no invalid operation enabled */
       case PPC_EXC_SNAN: if ((ppc_exc_mask_register & 0x00000FF0) == 0x00000FF0)
                       asm(" mtfsb0  24");   /* clear FPSCR[VE] bit */
                     break;
       case PPC_EXC_DZE: asm(" mtfsb0  27");   /* clear FPSCR[ZE] bit */
                     break;
       case PPC_EXC_FOV: asm(" mtfsb0  25");   /* clear FPSCR[OE] bit */
                     break;
       case PPC_EXC_UNF: asm(" mtfsb0  26");   /* clear FPSCR[UE] bit */
                     break;
       case PPC_EXC_INE: asm(" mtfsb0  28");   /* clear FPSCR[XE] bit */
                     break;
     }
   }
}

/******************************************************************************
*
* FUNCTION:
*   void ppc_global_exception_enable(void)
*
* DESCRIPTION:
*   enables all arithmetical exceptions which mask bit is cleared
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
*   20.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_global_exception_enable(void)
{
  ppc_exc_global_enabled = 0x1;

  asm("mtfsfi   0,0x0");        /* clear all pending interrupts */
  asm("mtfsfi   1,0x0");
  asm("mtfsfi   2,0x0");
  asm("mtfsfi   3,0x0");
  asm("mtfsfi   5,0x0");


  if (!(ppc_exc_mask_register & 0x00000001))
    asm(" mtfsb1  27");   /* set FPSCR[ZE] bit */
  if (!(ppc_exc_mask_register & 0x00000002))
    asm(" mtfsb1  25");   /* set FPSCR[OE] bit */
  if (!(ppc_exc_mask_register & 0x00000004))
    asm(" mtfsb1  26");   /* set FPSCR[UE] bit */
  if (!(ppc_exc_mask_register & 0x00000008))
    asm(" mtfsb1  28");   /* set FPSCR[XE] bit */
  if ((ppc_exc_mask_register & 0x00000FF0) != 0x00000FF0 )
    asm(" mtfsb1  24");   /* set FPSCR[VE] bit */
}

/******************************************************************************
*
* FUNCTION:
*   void ppc_global_exception_disable(void)
*
* DESCRIPTION:
*   disables all arithmetical exceptions
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
*   20.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_global_exception_disable(void)
{
  ppc_exc_global_enabled = 0x0;
  asm(" mtfsb0  24");   /* clear FPSCR[VE] bit */
  asm(" mtfsb0  27");   /* clear FPSCR[ZE] bit */
  asm(" mtfsb0  25");   /* clear FPSCR[OE] bit */
  asm(" mtfsb0  26");   /* clear FPSCR[UE] bit */
  asm(" mtfsb0  28");   /* clear FPSCR[XE] bit */
}

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_mode_set(UInt32 ExcID, UInt32 ExcMode)
*
* DESCRIPTION:
*   set the exception mode for the specified exception
*
* PARAMETERS:
*   ExcID   : specifies the exception for which the mode will be set
*   ExcMode : exception mode
*
* RETURNS:
*   the old exception mode of the specified exception
*
* NOTE:
*
* HISTORY:
*   23.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_mode_set(UInt32 ExcID, UInt32 ExcMode)
{
   UInt32* ptModeTbl = ptExcModeTable;
   UInt32  return_buffer = 0;

   ptModeTbl += ExcID;
   return_buffer = *ptModeTbl;

   *ptModeTbl = ExcMode;
   return (return_buffer);
}

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_mode_get(UInt32 ExcID)
*
* DESCRIPTION:
*   get the exception mode for the specified exception
*
* PARAMETERS:
*   ExcID   : specifies the exception for which the mode will be set
*
* RETURNS:
*   the exception mode of the specified exception
*
* NOTE:
*
* HISTORY:
*   23.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_mode_get(UInt32 ExcID)
{
   UInt32* ptModeTbl = ptExcModeTable;

   ptModeTbl += ExcID;

   return (*ptModeTbl);
}

/******************************************************************************
*
* FUNCTION:
*   Ppc_Exc_Handler_Type ppc_exception_handler_set(UInt32 ExcID,
*                                                        Ppc_Exc_Handler_Type Handler,
*                                                        UInt32 ExcMode)
*
* DESCRIPTION:
*   Installs an handler for the specified exception, sets its mode and return the address of
*   the old handler. The handler can be deinstall by passing a zero as second argument.
*
* PARAMETERS:
*   ExcID   : specifies the exception for which the handler will be installed
*   Handler : address of the new handler
*   ExcMode : mode will be set for the specified exception
*
* RETURNS:
*   address of the old handler
*
* NOTE:
*
* HISTORY:
*   23.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE Ppc_Exc_Handler_Type ppc_exception_handler_set(UInt32 ExcID,
                                                     Ppc_Exc_Handler_Type Handler,
                                                     UInt32 ExcMode)
{
  volatile Ppc_Exc_Handler_Type* pt_handler_tbl = ptExcTable;
  volatile UInt32* pt_mode_tbl = ptExcModeTable;

  Ppc_Exc_Handler_Type handler_buffer;

  pt_handler_tbl += ExcID;
  *pt_mode_tbl = ExcMode;

  pt_mode_tbl = ptExcModeTable;
  pt_mode_tbl += ExcID;

  handler_buffer = *pt_handler_tbl;

  *pt_handler_tbl = Handler;
  *pt_mode_tbl = ExcMode;

  return (handler_buffer);

}


/******************************************************************************
*
* FUNCTION:
*   void ppc_all_exception_handlers_set(Ppc_Exc_Handler_Type Handler,
*                                         UInt32 ExcMode)
*
* DESCRIPTION:
*   Installs the same handler for all exceptions.
*   To deinstall all handlers pass a zero as second argument.
*
* PARAMETERS:
*   Handler : address of the new handler
*   ExcMode : mode will be set for the specified exception
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   23.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_all_exception_handlers_set(Ppc_Exc_Handler_Type Handler,
                                      UInt32 ExcMode)
{
  volatile Ppc_Exc_Handler_Type* pt_handler_tbl = ptExcTable;
  volatile UInt32* pt_mode_tbl = ptExcModeTable;
  int i;

  for (i = PPC_EXC_DZE; i <= PPC_EXC_ICON; i++)
  {
    *pt_handler_tbl = Handler;
    pt_handler_tbl++;
    *pt_mode_tbl = ExcMode;
    pt_mode_tbl++;
  }
}

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_counter_get(UInt32 ExcID)
*
* DESCRIPTION:
*   returns the internal counter value for the specified exception
*
* PARAMETERS:
*   ExcID   : specifies the exception
*
* RETURNS:
*   internal counter value for the specified exception
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_counter_get(UInt32 ExcID)
{
   return (*(ptExcCounterTable + ExcID));
}

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_counter_reset(UInt32 ExcID)
*
* DESCRIPTION:
*   returns the internal and external counter value for the specified exception
*   and resets the counter values
*
* PARAMETERS:
*   ExcID   : specifies the exception
*
* RETURNS:
*   internal counter value for the specified exception before reset
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_counter_reset(UInt32 ExcID)
{
  volatile cfg_section_type* config_section =(volatile cfg_section_type*) PPC_CFG_STRUCT_START;

  UInt32 return_buffer = *(ptExcCounterTable + ExcID);
  *(ptExcCounterTable + ExcID) = 0x0;
  config_section->__cfg_exc_counter[ExcID] =0x0;
  return (return_buffer);
}

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_total_exception_count_get(void)
*
* DESCRIPTION:
*   returns the total sum of all internal exception counters
*
* PARAMETERS:
*   none
*
* RETURNS:
*   total count of all arithmetical exceptions
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_total_exception_count_get(void)
{
  UInt32 sum=0;
  int i;

  for (i= PPC_EXC_DZE; i<PPC_NUMBER_OF_EXCEPTIONS; i++)
  {
    sum = sum +  (*(ptExcCounterTable + i));
  }

  return (sum);
}


/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_flag_get(void)
*
* DESCRIPTION:
*   returns the exception flag from the config section
*
* PARAMETERS:
*   none
*
* RETURNS:
*  zero if no exception happens
*  7  if an arithmetic fp exception happens
*
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_flag_get(void)
{
  volatile cfg_section_type* config_section =(volatile cfg_section_type*) PPC_CFG_STRUCT_START;

  return (config_section->__cfg_except_flag);
}


/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_flag_reset(void)
*
* DESCRIPTION:
*   returns the exception flag and reset the flag
*
* PARAMETERS:
*   none
*
* RETURNS:
*  zero if no exception happens
*  7  if an arithmetic fp exception happens
*
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_flag_reset(void)
{
  volatile cfg_section_type* config_section =(volatile cfg_section_type*) PPC_CFG_STRUCT_START;
  UInt32 return_buffer;

  return_buffer = config_section->__cfg_except_flag;
  config_section->__cfg_except_flag = 0;

  return (return_buffer);
}


#undef __INLINE

#endif   /* _PPCEXC_C_ */

