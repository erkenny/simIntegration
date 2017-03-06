/******************************************************************************
*
* FILE:
*   dsav9110.c
*
* RELATED FILES:
*   dsav9110.h
*
* DESCRIPTION:
*
*  Access functions for the serially programmable frequency generator AV9110
*
* HISTORY: 
*           1.0   28.08.2000    O.J             initial version       
*           1.1   01.09.2003    Jochen Sauer    structure changes
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsav9110.c $ $Revision: 1.3 $ $Date: 2007/05/15 14:30:56GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
*
******************************************************************************/

#include <math.h>
#include <dsstd.h>
#include <stdlib.h>
#include <dstypes.h>
#include <dsav9110.h>

/******************************************************************************
  function declarations
******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE
#endif

/******************************************************************************
* constant and macro definitions
******************************************************************************/

#ifndef _INLINE

#define VCO_PRESCALE_1  1
#define VCO_PRESCALE_8  8
#define CLK_XDIV_1      1
#define CLK_XDIV_2      2
#define CLK_XDIV_4      4
#define CLK_XDIV_8      8
#define VCO_OUTDIV_1    1
#define VCO_OUTDIV_2    2
#define VCO_OUTDIV_4    4
#define VCO_OUTDIV_8    8
#endif



/******************************************************************************
*
* DESCRIPTION 
*   This function calculates the exactly frequency of the 
*   clock and the values for PLL circuit.
*
* PARAMETERS 
*
*   freq_in      desired frequency
*
*   freq_p       calculated frequency
*
*   vco_div_p    calculated vco-divisor
*
*   ref_div_p    calculated reference-divisor
*
* RETURNS 
*
*                AV9110_NO_ERROR or AV9110_PLLFREQ_ERROR           
*
******************************************************************************/

Int16 av9110_clock_calc( UInt32 ref_freq, Float32 freq_in, Float32* freq_p,
                         UInt8 *vco_div_p, UInt8 *ref_div_p)
{
  Float32 ref_div_r;
  Float64 ref_div_r2;
  Float32 best_freq;
  Float32 freq_a;
  Float32 fvco;
  UInt32 vco_div_temp;
  UInt32 ref_div_temp;
  
 
  UInt32 vco_outdiv;
  UInt32 clk_xdiv;

  *freq_p = freq_in;

  vco_outdiv = VCO_OUTDIV_4;
  clk_xdiv = CLK_XDIV_2;

  *ref_div_p = 255;
  best_freq = (Float32)10e30;

  for( vco_div_temp = 3; vco_div_temp <= 127; vco_div_temp++)
  {

    ref_div_r = (Float32)vco_div_temp / (*freq_p) * (Float32)ref_freq * VCO_PRESCALE_8
                / (Float32)clk_xdiv / (Float32)vco_outdiv;

    ref_div_r =  ref_div_r + (Float32)0.5; 
    modf( ref_div_r, &ref_div_r2);
    ref_div_temp = (UInt32)ref_div_r2;
    freq_a = (Float32)vco_div_temp * (Float32)ref_freq * VCO_PRESCALE_8 / (Float32)ref_div_temp /
             (Float32) clk_xdiv / (Float32)vco_outdiv;

    fvco =   (Float32)ref_freq * (Float32)vco_div_temp / (Float32)ref_div_temp * VCO_PRESCALE_8;

    if(
       ( fabs(freq_a - (*freq_p) ) < fabs(best_freq - (*freq_p) ) ) &&
       ( fvco > 50e6 ) &&
       ( ref_div_temp < 128 ) &&
       ( ref_div_temp > 2) &&
       ( freq_a < 130e6)   &&
       ( ( 200e3 * ref_div_temp ) < ref_freq ) &&
       ( ( 5e6 * ref_div_temp ) > ref_freq )
      )
    {
      *ref_div_p = (UInt8)ref_div_temp;
      *vco_div_p = (UInt8)vco_div_temp;
      best_freq = freq_a;
    }; /*if*/


  }; /*for*/

   if(*ref_div_p == 255)
     return(AV9110_PLLFREQ_ERROR);
   else
     *freq_p = best_freq;

   return(AV9110_NO_ERROR);

}

/******************************************************************************
*
* DESCRIPTION 
*   This function writes the parameters to the PLL circuit.
*
* PARAMETERS 
*
*   vco_div        vco-divisor
*
*   ref_div        reference-divisor
*
* RETURNS 
*   void
*
******************************************************************************/


void av9110_clock_write( av9110_device_t device, UInt8 vco_div, UInt8 ref_div )
{
  UInt32 data;
  UInt16 i;

  vco_div = vco_div & 0x7F;
  ref_div = ref_div & 0x7F;
  data =   ( (UInt32)vco_div ) |
         ( ( (UInt32)ref_div ) << 7 ) |
         ( ( (UInt32)0x3CD ) << 14 );

  /* start sequence */
  (*device.reg_addr)&=((~device.data_set) & (~device.clock_set));
  (*device.reg_addr)&=(~device.cs_off);
  (*device.reg_addr)|= device.data_oe;

  RTLIB_FORCE_IN_ORDER();

  /* write sequence */

  for(i=1; i<=24; i++)
  {
     if(data & 1)
      (*device.reg_addr)|= device.data_set;
     else
      (*device.reg_addr)&=(~device.data_set);

     (*device.reg_addr)|= device.clock_set;
     (*device.reg_addr)&=(~device.clock_set);
     data = data >> 1;
  }

  RTLIB_FORCE_IN_ORDER();

  /* end sequence */
  (*device.reg_addr)|=device.cs_off;
  (*device.reg_addr)&=(~device.data_oe);
  RTLIB_FORCE_IN_ORDER();
}
