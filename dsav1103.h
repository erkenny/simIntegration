/******************************************************************************
*
* FILE:
*   dsav9110_1103.h
*
* RELATED FILES:
*   dsav9110_1103.c
*
* DESCRIPTION:
*
*  ds1103 dependent access functions for the serially programmable frequency generator AV9110
*
* HISTORY: 
*           1.0   28.09.2003    Jochen Sauer  initial version        
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsav1103.h $ $Revision: 1.5 $ $Date: 2007/05/15 14:28:54GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
*
******************************************************************************/

#ifndef __av9110_1103__
#define __av9110_1103__

#include <Dstypes.h>

/******************************************************************************
  error codes
******************************************************************************/

#define   AV9110_NO_ERROR                           0
#define   AV9110_PLLFREQ_ERROR                      1

/******************************************************************************
* constant definitions
******************************************************************************/

/******************************************************************************
* type definitions
******************************************************************************/

/******************************************************************************
  function prototypes
******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE
#endif

__INLINE Int16 ds1103_av9110_clock_calc_ser( Float32 freq_in, Float32* freq_p,
                         UInt8 *vco_div_p, UInt8 *ref_div_p);

__INLINE void ds1103_av9110_clock_write_ser( UInt8 vco_div, UInt8 ref_div );

__INLINE Int16 ds1103_av9110_clock_calc_can( Float32 freq_in, Float32* freq_p,
                         UInt8 *vco_div_p, UInt8 *ref_div_p);

__INLINE void ds1103_av9110_clock_write_can( UInt8 vco_div, UInt8 ref_div );

#endif /* av9110_ds1103.h */
