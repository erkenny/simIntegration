/******************************************************************************
*
* FILE:
*   int1103.c
*
* RELATED FILES:
*   int1103.h
*   dstypes.h
*   io1103.h
*
* DESCRIPTION:
*   interrupt support for DS1103 board
*
* HISTORY:
*   19.02.98  Rev 1.0  H.-G. Limberg     initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: INT1103.C $ $Revision: 1.3 $ $Date: 2003/12/09 18:46:35GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/
asm(".supervisoron");
#include <dstypes.h>
#include <int1103.h>
#include <std1103.h>
#include <dsstd.h>


/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

#define COUNT_OF_HW_INTERRUPTS  24


#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif


/******************************************************************************
  program global variables
******************************************************************************/
extern UInt32 DS1103_Dec_Int_Enable_Flag;
extern DS1103_Int_Handler_Type* ptVecTable;
extern UInt32* ptSaveRegTable;

#ifndef _INLINE
 UInt32 ds1103_interrupt_mask = 0xFFFFFFFF;    /* copy of the related register */
#else
 extern UInt32 ds1103_interrupt_mask;
#endif


/******************************************************************************
  modul global variables
******************************************************************************/

static volatile UInt32* ds1103_interrupt_mask_register = (volatile UInt32*)DS1103_INT_MASK_REG;


/******************************************************************************
*
* FUNCTION:
*   void ds1103_enable_hardware_int (UInt32 IntID)
*
* DESCRIPTION:
*   enables the specified hardware interrupt by clearing the related bit in the
*   interrupt mask register respectively by setting the enable flag for the decrementer
*
* PARAMETERS:
*   IntID : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg, initial version
*
******************************************************************************/

__INLINE void ds1103_enable_hardware_int(UInt32 IntID)
{
  UInt32 status = 0;

  if (IntID == 0)
  {
    DS1103_Dec_Int_Enable_Flag = 0x1;
  }
  else
  {
    /* this part must not be interrupted because ds1103_interrupt_mask is a global variable */
    RTLIB_INT_SAVE_AND_DISABLE(status);
    ds1103_interrupt_mask = (UInt32)(ds1103_interrupt_mask & (0xFFFFFFFF ^ (0x1 << (IntID - 1))));
    *ds1103_interrupt_mask_register = ds1103_interrupt_mask;
    RTLIB_INT_RESTORE(status);
  }
}

/******************************************************************************
*
* FUNCTION:
*   void ds1103_disable_hardware_int (UInt32 IntID)
*
* DESCRIPTION:
*   disables the specified hardware interrupt by setting the related bit in the
*   interrupt mask register respectively by clearing the enable flag for the decrementer
*
* PARAMETERS:
*   IntID : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_disable_hardware_int(UInt32 IntID)
{
  UInt32 status = 0;

  if (IntID == 0)
  {
    DS1103_Dec_Int_Enable_Flag = 0x0;
  }
  else
  {
    /* this part must not be interrupted because ds1103_interrupt_mask is a global variable */
    RTLIB_INT_SAVE_AND_DISABLE(status);
    ds1103_interrupt_mask = (UInt32)(ds1103_interrupt_mask | (0x1 << (IntID - 1)));
    *ds1103_interrupt_mask_register = ds1103_interrupt_mask;
    RTLIB_INT_RESTORE(status);
  }
}

/******************************************************************************
*
* FUNCTION:
*   DS1103_Int_Handler_Type ds1103_set_interrupt_vector(UInt32 IntID,
*                                                       DS1103_Int_Handler_Type Handler,
*                                                       int SaveRegs)
*
* DESCRIPTION:
*
* PARAMETERS:
*   IntID   : ID number of the interrupt, defined in INT1103.h
*   Handler : handler to connect with the interrupt
*   SaveRegs: boolean variable, indicates if the processor registers should be saved
*
* RETURNS:
*   address of the handler which was connected to the interrupt before
*
* NOTE:
*
* HISTORY:
*   25.02.98   H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE DS1103_Int_Handler_Type ds1103_set_interrupt_vector(UInt32 IntID, DS1103_Int_Handler_Type Handler,int SaveRegs)
{
  volatile DS1103_Int_Handler_Type* ptVecTableEntry = ptVecTable;
  volatile UInt32* ptSaveRegTableEntry = ptSaveRegTable;

  DS1103_Int_Handler_Type old_vector;

  if (IntID > (COUNT_OF_HW_INTERRUPTS-1))
    return((DS1103_Int_Handler_Type)0);

  ptVecTableEntry     += IntID;
  ptSaveRegTableEntry += IntID;

  old_vector = *ptVecTableEntry;

 *ptVecTableEntry = Handler;
 *ptSaveRegTableEntry = (UInt32)SaveRegs;

  return(old_vector);
}

/******************************************************************************
*
* FUNCTION:
*   DS1103_Int_Handler_Type ds1103_get_interrupt_vector(UInt32 IntID)
*
* DESCRIPTION:
*   get the vector of the handler related to the interrupt
*
* PARAMETERS:
*   IntID   : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*   vector of the handler related to the interrupt
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE DS1103_Int_Handler_Type ds1103_get_interrupt_vector(UInt32 IntID)
{
  volatile DS1103_Int_Handler_Type * ptVecTableEntry = ptVecTable;

  if (IntID > (COUNT_OF_HW_INTERRUPTS-1))
    return((DS1103_Int_Handler_Type)0);

  ptVecTableEntry += IntID;

  return(*ptVecTableEntry);
}


/******************************************************************************
*
* FUNCTION:
*   UInt32 ds1103_get_interrupt_status(void)
*
* DESCRIPTION:
*   gets the status of the EE bit in MSR of the processor
*
* PARAMETERS:
*
* RETURNS:
*   0x00008000 if EE bit is set (interrupts global enabled)
*   0x00000000 if EE bit is clear (interrupts global disabled)
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE UInt32 ds1103_get_interrupt_status(void)
{
  asm(" mfmsr 3");
  asm(" lis   4,0x0000");
  asm(" ori   4,4,0x8000");
  return(asm(" and   3,3,4"));
}

/******************************************************************************
*
* FUNCTION:
*   void ds1103_set_interrupt_status(UInt32 status)
*
* DESCRIPTION:
*   set the EE bit in MSR of the processor to the status, which was delivered
*   from the function ds1103_get_interrupt_status
*
* PARAMETERS:
*   0x00008000 to set EE bit
*   0x00000000 to not change EE bit
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*
******************************************************************************/
__INLINE void ds1103_set_interrupt_status(UInt32 status)
{
  asm(" mfmsr 4");
  asm(" or    4,3,4");
  asm(" sync");
  asm(" mtmsr 4");
  asm(" isync");
}

/******************************************************************************
*
* FUNCTION:
*   void ds1103_reset_interrupt_flag(UInt32 IntID)
*
* DESCRIPTION:
*   reset the flag for the related interrupt in IIR if IntID != 0
*   decrementer interrupt (IntID =0) has no flag
*
* PARAMETERS:
*   IntID   : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE void ds1103_reset_interrupt_flag(UInt32 IntID)  
{
  volatile UInt32* interrupt_input_register =(volatile UInt32*) DS1103_INT_IN_REG;

  if (IntID)
    *interrupt_input_register = (UInt32)(0x1 << (IntID - 1));
    
}

/******************************************************************************
*
* FUNCTION:
*   void ds1103_get_interrupt_flag(UInt32 IntID)  
*
* DESCRIPTION:
*   returns the state of the related interrupt in IIR
*
* PARAMETERS:
*   IntID   : ID number of the interrupt, defined in INT1103.h
*
* RETURNS: 0 if the flag is cleared
*          1 if the flag is set 
*
* NOTE: the function returns always 0 for the decrementer-interrupt 
*
* HISTORY:
*   25.02.98  H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE int ds1103_get_interrupt_flag(UInt32 IntID)
{
  volatile UInt32* interrupt_input_register =(volatile UInt32*) DS1103_INT_IN_REG;

  if (IntID)
  {
    if ((*interrupt_input_register & (0x1 << (IntID -1))) == 0)
      return (0);
    else
      return(1);
  }
  else 
    return(0);     /* decrementer interrupt (IntID = 0) has no interrupt flag, always return 0 */
}


#undef __INLINE

