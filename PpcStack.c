/***************************************************************************
*
*  MODULE
*
*  FILE
*    PpcStack.c
*
*  RELATED FILES
*
*  DESCRIPTION
*    Module to control stack operations on PPC boards
*
*  REMARKS
*
*  AUTHOR(S)
*    H.-G. Limberg
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: PpcStack.c $ $Revision: 1.2 $ $Date: 2003/08/07 12:01:03GMT+01:00 $
***************************************************************************/

#include <PpcStack.h>
#include <dsmsg.h>

/***************************************************************************
  constant, macro and type definitions
***************************************************************************/
#define STWU_1_1_CODE 0x94210000
#define STACK_CONTROL_OPCODE   0x47FF0002

#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif


/***************************************************************************
  object declarations
***************************************************************************/
extern UInt32 __cfg_stacksize;

#ifdef _DS1005
extern UInt32 ds1005_stack_pointer_init;
extern UInt32 ds1005_stack_bottom;
#define ppc_stack_pointer_init ds1005_stack_pointer_init
#define ppc_stack_bottom ds1005_stack_bottom
#elif defined _DS1103
extern UInt32 ds1103_stack_pointer_init;
extern UInt32 ds1103_stack_bottom;
#define ppc_stack_pointer_init ds1103_stack_pointer_init
#define ppc_stack_bottom ds1103_stack_bottom
#elif defined _DS1401
extern UInt32 ds1401_stack_pointer_init;
extern UInt32 ds1401_stack_bottom;
#define ppc_stack_pointer_init ds1401_stack_pointer_init
#define ppc_stack_bottom ds1401_stack_bottom
#elif defined _DS1104
extern UInt32 ds1104_stack_pointer_init;
extern UInt32 ds1104_stack_bottom;
#define ppc_stack_pointer_init ds1104_stack_pointer_init
#define ppc_stack_bottom ds1104_stack_bottom
#else
  #error target platform undefined
#endif


/***************************************************************************
  function prototypes
***************************************************************************/
void ppc_data_and_instruct_caches_clear(UInt32 address);
UInt32 ppc_text_section_start_get(void);
UInt32 ppc_text_section_end_get(void);

/***************************************************************************
  function declarations
***************************************************************************/
#ifndef _INLINE
#pragma asm

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_data_and_instruct_caches_clear
*
*  DESCRIPTION
*    Flushes and the data cache and invalidates the instruction cache.
*
*  PARAMETERS
*    address to flush and invalidate
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/
.globl ppc_data_and_instruct_caches_clear
ppc_data_and_instruct_caches_clear:
        sync
        dcbf  0,3
        sync
        icbi  0,3
        sync
        blr

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_text_section_start_get
*
*  DESCRIPTION
*    Gets the start address of the .text section of the program
*
*  PARAMETERS
*
*  RETURNS
*    start address of the .text section
*
*  REMARKS
*
*******************************************************************************/
.globl ppc_text_section_start_get
ppc_text_section_start_get:
        lis 3,secthi(.text)
        ori 3,3,sectlo(.text)
        blr

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_text_section_end_get
*
*  DESCRIPTION
*    Gets the end address of the .text section of the program
*
*  PARAMETERS
*
*  RETURNS
*    end address of the .text section
*
*  REMARKS
*
*******************************************************************************/
.globl ppc_text_section_end_get
ppc_text_section_end_get:
        lis  3,secthi(.text)
        ori  3,3,sectlo(.text)
        lis  4,hi(sizeof(.text))
        ori  4,4,lo(sizeof(.text))
        add  3,3,4
        subi 3,3,1
        blr
#pragma endasm


/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_stack_control_enable(void)
*
*  DESCRIPTION
*    Replaces the "stwu r1,d(r1)" instruction in the .text section through
*    special system call instruction.
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/
void ppc_stack_control_enable(void)
{
  UInt32 i;
  
  for (i = ppc_text_section_start_get(); i < ppc_text_section_end_get(); i+=4)
  {
    if ( (*((UInt32*)i) & 0xFFFF0000) == STWU_1_1_CODE )
    {
      *((UInt32*)i) = (*((UInt32*)i) & 0x0000FFFF) | STACK_CONTROL_OPCODE;
      ppc_data_and_instruct_caches_clear(i);
    }
  }
}

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_stack_control_disable(void)
*
*  DESCRIPTION
*    Replaces the special system call instruction in the .text section through
*    "stwu r1,d(r1)" instruction.
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/
void ppc_stack_control_disable(void)
{
  UInt32 i;

  for (i = ppc_text_section_start_get(); i < ppc_text_section_end_get(); i+=4)
  {
    if ( (*((UInt32*)i) & 0xFFFF0000) == (STACK_CONTROL_OPCODE & 0xFFFF0000))
    {
      *((UInt32*)i) = (*((UInt32*)i) & 0x0000FFFD) | STWU_1_1_CODE;
      ppc_data_and_instruct_caches_clear(i);
    }
  }
}

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_stack_overflow_fcn(UInt32 address)
*
*  DESCRIPTION
*    Is called if the stack has overflow.
*
*  PARAMETERS
*    address : program address of the instruction which causes the overflow
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

void ppc_stack_overflow_fcn(UInt32 address)
{
  msg_error_printf(MSG_SM_RTLIB,MSG_STACK_CTRL_OVERFLOW_ERROR,
                                MSG_STACK_CTRL_OVERFLOW_ERROR_TXT, address);
  ppc_stack_control_disable();
  exit(1);
}

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_stack_underflow_fcn(UInt32 address)
*
*  DESCRIPTION
*    Is called if the stack has underflow.
*
*  PARAMETERS
*    address : program address of the instruction at which the underflow was
*              detected.
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/
void ppc_stack_underflow_fcn(UInt32 address)
{
  msg_error_printf(MSG_SM_RTLIB,MSG_STACK_CTRL_UNDERFLOW_ERROR,
                                MSG_STACK_CTRL_UNDERFLOW_ERROR_TXT, address);

  ppc_stack_control_disable();
  exit(1);
}

#endif

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_stack_size_get(void)
*
*  DESCRIPTION
*    Gets the stack size of the application in bytes.
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/
__INLINE UInt32 ppc_stack_size_get(void)
{
  return(__cfg_stacksize);
}


/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_available_stack_size_get(void)
*
*  DESCRIPTION
*    Gets the currently available stack size in bytes.
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

__INLINE UInt32 ppc_available_stack_size_get(void)
{
  return(asm("mr 3,1") - ppc_stack_bottom);
}


/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    ppc_available_relative_stack_size_get(void)
*
*  DESCRIPTION
*    Gets the relation between stack size and currently available stack size.
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/
__INLINE Float64 ppc_available_relative_stack_size_get(void)
{
  return(((Float64)ppc_available_stack_size_get() / (Float64)__cfg_stacksize));
}







#undef __INLINE

