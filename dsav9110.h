/******************************************************************************
*
* FILE:
*   dsav9110.h
*
* RELATED FILES:
*   dsav9110.c
*
* DESCRIPTION:
*
*  Access functions for the serially programmable frequency generator AV9110
*
* HISTORY: 
*           1.0   28.08.2000    O.J                 initial version
*           1.1   01.09.2003    Jochen Sauer        structur changes
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsav9110.h $ $Revision: 1.3 $ $Date: 2007/05/15 14:29:06GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
*
******************************************************************************/

#ifndef __dsav9110__
#define __dsav9110__

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


typedef struct av9110_device {
  UInt32                ref_freq;
  volatile UInt16*      reg_addr;
  UInt16                data_set;
  UInt16                clock_set;
  UInt16                cs_off;
  UInt16                data_oe;
   } av9110_device_t;


/******************************************************************************
  function prototypes
******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE
#endif

__INLINE Int16 av9110_clock_calc( UInt32 ref_freq,Float32 freq_in, Float32* freq_p,
                         UInt8 *vco_div_p, UInt8 *ref_div_p);

__INLINE void av9110_clock_write( av9110_device_t device,UInt8 vco_div, UInt8 ref_div );


#endif /* dsav9110.h */
