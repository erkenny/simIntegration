/******************************************************************************
* MODULE
*
*
* FILE:
*   dpm1103.c
*
* RELATED FILES:
*   dpm1103.h
*
* DESCRIPTION:
*   hardware dependent functions for working with dual port memories
*
* AUTHOR(S)
*   H.-G. Limberg
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*   $RCSfile: dpm1103.c $ $Revision: 1.2 $ $Date: 2003/08/07 12:19:24GMT+01:00 $
*   $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSINTMCOM1103.pj $
*
******************************************************************************/

#include <dpm1103.h>
/*
#include <tmr1103.h>
*/                                               /* DS1103 timer definitions */

#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif



/******************************************************************************
*
* FUNCTION
*   Timer read function
*
* SYNTAX
*   double mstcom_time_read (void)
*
* DESCRIPTION
*
* PARAMETERS
*   void
*
* RETURNS
*   timer value scaled to sec
*
* REMARKS
*
******************************************************************************/
/*
__INLINE unsigned long dpm_timer_read (void)
{
  return((unsigned long) ds1103_timebase_low_read());
}
*/
