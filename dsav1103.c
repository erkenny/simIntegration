/******************************************************************************
*
* FILE:
*   dsav1103.c
*
* RELATED FILES:
*   dsav9110.c
*   dsav1103.h
*
* DESCRIPTION:
*
*  Access functions for the serially programmable frequency generator AV9110
*
* HISTORY: 
*           1.0   28.09.2003    Jochen Sauer  initial version       
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsav1103.c $ $Revision: 1.6 $ $Date: 2003/11/14 11:30:45GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
*
******************************************************************************/

#include <math.h>
#include <dsstd.h>
#include <stdlib.h>
#include <dstypes.h>
#include <dsav1103.h>
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
/* constant for serial and can */
#define AV9110_DS1103_REG_ADDR      (volatile UInt16*)0x10000042
#define AV9110_DS1103_DATA_SET      0x4
#define AV9110_DS1103_CLOCK_SET     0x20
#define AV9110_DS1103_DATA_OE       0x08

/* constant for can */
#define AV9110_DS1103_CAN_FREQ_REF  14318180
#define AV9110_DS1103_CAN_CS_OFF        0x10

/* constant for serial */
#define AV9110_DS1103_SER_FREQ_REF  20000000
#define AV9110_DS1103_SER_CS_OFF        0x40
#endif

/******************************************************************************
*
* FUNCTION:
*
* SYNTAX:
*   Int16 ds1103_av9110_clock_calc_ser((Float32 freq_in, Float32* freq_p,
                         UInt8 *vco_div_p, UInt8 *ref_div_p)
*
* DESCRIPTION: 
*   This function calculates the exactly frequency of the serial controller
*   clock and the values for PLL circuit.
*
* PARAMETERS: 
*   Float32* freq: -> desired frequency
*                  <- calculated frequency
*   UInt8* vco_div: calculated vco-divisor
*   UInt8* ref_div: calculated reference-divisor
*
* RETURNS: 
*   errorcode
*
* NOTE: 
*   This function is used by ds1103_dsserial_config 
*   
*
******************************************************************************/

__INLINE Int16 ds1103_av9110_clock_calc_ser(Float32 freq_in, Float32* freq_p,
                         UInt8 *vco_div_p, UInt8 *ref_div_p)
{
  return av9110_clock_calc(AV9110_DS1103_SER_FREQ_REF,freq_in,freq_p,vco_div_p,ref_div_p);
}

/******************************************************************************
*
* FUNCTION:
*
* SYNTAX:
*   void ds1103_av9110_clock_write_ser(UInt8 vco_div, UInt8 ref_div)
*
*
* DESCRIPTION: 
*   This function writes the parameters to the baudrate generator of the serial interface.
*
* PARAMETERS: 
*   UInt8 vco_div: vco-divisor
*   UInt8 ref_div: reference-divisor
*
* RETURNS: 
*   void
*
* NOTE: 
*   This function is used by ds1103_dsserial_config 
*   
*
******************************************************************************/


__INLINE void ds1103_av9110_clock_write_ser(UInt8 vco_div, UInt8 ref_div)
{
  struct av9110_device dev=
      {
        AV9110_DS1103_SER_FREQ_REF,
        AV9110_DS1103_REG_ADDR,
        AV9110_DS1103_DATA_SET,
        AV9110_DS1103_CLOCK_SET,
        AV9110_DS1103_SER_CS_OFF,
        AV9110_DS1103_DATA_OE
      };
  av9110_clock_write(dev,vco_div,ref_div);
  return;
}

/******************************************************************************
*
* FUNCTION:
*
* SYNTAX:
*   Int16 ds1103_av9110_clock_calc_can((Float32 freq_in, Float32* freq_p,
                         UInt8 *vco_div_p, UInt8 *ref_div_p)
*
* DESCRIPTION: 
*   This function calculates the exactly frequency of the CAN controller
*   clock and the values for PLL circuit.
*
* PARAMETERS: 
*   Float32* freq: -> desired frequency
*                  <- calculated frequency
*   UInt8* vco_div: calculated vco-divisor
*   UInt8* ref_div: calculated reference-divisor
*
* RETURNS: 
*   errorcode
*
* NOTE: 
*   This function is used by can1103.c
*   
*
******************************************************************************/

__INLINE Int16 ds1103_av9110_clock_calc_can(Float32 freq_in, Float32* freq_p,
                         UInt8 *vco_div_p, UInt8 *ref_div_p)
{
  return av9110_clock_calc(AV9110_DS1103_CAN_FREQ_REF,freq_in,freq_p,vco_div_p,ref_div_p);
}

/******************************************************************************
*
* FUNCTION:
*
* SYNTAX:
*   void ds1103_av9110_clock_write_can(UInt8 vco_div, UInt8 ref_div)
*
*
* DESCRIPTION: 
*   This function writes the parameters to the baudrate generator of the CAN interface.
*
* PARAMETERS: 
*   UInt8 vco_div: vco-divisor
*   UInt8 ref_div: reference-divisor
*
* RETURNS: 
*   void
*
* NOTE: 
*   This function is used by can1103.c 
*   
*
******************************************************************************/


__INLINE void  ds1103_av9110_clock_write_can(UInt8 vco_div, UInt8 ref_div)
{
  struct av9110_device dev=
      {
        AV9110_DS1103_CAN_FREQ_REF,
        AV9110_DS1103_REG_ADDR,
        AV9110_DS1103_DATA_SET,
        AV9110_DS1103_CLOCK_SET,
        AV9110_DS1103_CAN_CS_OFF,
        AV9110_DS1103_DATA_OE
      };
  av9110_clock_write(dev,vco_div,ref_div);
  return;
}
