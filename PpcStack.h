/***************************************************************************
*
*  MODULE
*
*  FILE
*    PpcStack.h
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
*  $RCSfile: PpcStack.h $ $Revision: 1.2 $ $Date: 2003/08/07 12:01:04GMT+01:00 $
***************************************************************************/

#ifndef __PPCSTACK_H__
#define __PPCSTACK_H__

#include <dstypes.h>


/***************************************************************************
  constant, macro and type definitions
***************************************************************************/


/***************************************************************************
  function prototypes
***************************************************************************/


#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif


void ppc_stack_control_enable(void);

void ppc_stack_control_disable(void);

__INLINE UInt32 ppc_stack_size_get(void);

__INLINE UInt32 ppc_available_stack_size_get(void);

__INLINE Float64 ppc_available_relative_stack_size_get(void);






#undef __INLINE

/***************************************************************************
  inclusion of source file(s) for inline expansion
***************************************************************************/

#ifdef _INLINE
#include <ppcstack.c>
#endif

#endif /* __PPCSTACK_H__ */

