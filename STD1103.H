/******************************************************************************
*
* MODULE
*   Common definitions for the DS1103
*
* FILE
*   Std1103.h
*
* RELATED FILES
* 
*
* DESCRIPTION
*    - def. for interrupt enable/disable
*    - def. for debugging
*    - def. for dSPACE BIOS
*
* AUTHOR(S)
*   R. Kraft
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: STD1103.H $ $Revision: 1.2 $ $Date: 2003/08/07 14:54:03GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/


#ifndef __Std1103__
#define __Std1103__

#include <dstypes.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/
#define DS1103_EIEIO asm( " eieio " )
#define DS1103_SYNC  asm( " sync " )

/* definitions for debugging via msg-module */
#define DS1103_DEBUG_INIT 1
#define DS1103_DEBUG_POLL 2

/******************************************************************************
  object declarations
******************************************************************************/

extern volatile UInt16 ds1103_debug;


/******************************************************************************
  function prototypes
******************************************************************************/

#ifdef _INLINE
#define __INLINE static 
#else
#define __INLINE
#endif

/* function prototypes go here */

#undef __INLINE


#endif

