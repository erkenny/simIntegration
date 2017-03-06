/******************************************************************************
*
* FILE:
*   Tic1103.c
*
* RELATED FILES:
*   Tic1103.h 
*
* DESCRIPTION:
*   DS1103 on-board executiontime-measurement
*
*
* HISTORY: RK initial version
*    
*
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: TIC1103.C $ $Revision: 1.3 $ $Date: 2003/11/25 08:25:43GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#include <Tic1103.h>

#ifdef _INLINE
  #define __INLINE static
#else
  #define __INLINE
#endif
    
         
/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

/******************************************************************************
  object declarations
******************************************************************************/



#ifndef _INLINE

rtlib_tic_t tic_start_count = 0;      /* rtlib_tic_t is an UInt32 datatype */
rtlib_tic_t tic_halt_count = 0 ;
rtlib_tic_t tic_idle_count = 0;

#else

extern rtlib_tic_t tic_start_count;
extern rtlib_tic_t tic_halt_count;
extern rtlib_tic_t tic_idle_count;

#endif




#undef __INLINE
