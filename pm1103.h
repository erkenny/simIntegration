/******************************************************************************
*
* FILE:
*   pm1103.h
*
* RELATED FILES:
*   dstypes.h
*
* DESCRIPTION:
*   performance measurement support for DS1103 board
*
* HISTORY:
*   06.04.98  Rev 1.0  H.-G. Limberg     initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: pm1103.h $ $Revision: 1.2 $ $Date: 2003/08/07 14:54:17GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/
#ifndef __PM1103_H__
#define __PM1103_H__

#include <dstypes.h>

#ifdef _INLINE
  #define __INLINE static
#else
  #define __INLINE
#endif


/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/


typedef void (*DS1103_PC_Handler_Type)(void);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ds1103_pcounter_get (UInt32 counter)
*
* DESCRIPTION:
*   gets the value of the specified performance counter 
*
*
* PARAMETERS:
*   counter : number of the performance counter, defined in PM1103.h
*
* RETURNS:
*   value of the counter
*
* NOTE:
*
* HISTORY:
*   06.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ds1103_pcounter_get(UInt32 counter);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ds1103_pcounter_reset (UInt32 counter)
*
* DESCRIPTION:
*   resets the specified performance counter and return the previous counter value
*
*
* PARAMETERS:
*   counter : number of the performance counter, defined in PM1103.h
*
* RETURNS:
*   value of the counter
*
* NOTE:
*
* HISTORY:
*   06.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ds1103_pcounter_reset(UInt32 counter);

/******************************************************************************
*
* FUNCTION:
*   void ds1103_pcounters_enable (void)
*
* DESCRIPTION:
*   enable all counters to be changed by hardware
*
* PARAMETERS:
*   none
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   06.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_pcounters_enable (void);

/******************************************************************************
*
* FUNCTION:
*   void ds1103_pcounters_disable (void)
*
* DESCRIPTION:
*   disable all counters to be changed by hardware
*
* PARAMETERS:
*   none
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   06.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_pcounters_disable (void);

/******************************************************************************
*
* FUNCTION:
*   void ds1103_pcounters_interrupt_signaling_enable (void)
*
* DESCRIPTION:
*   enable interrupt signaling for all counters
*
* PARAMETERS:
*   none
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   06.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_pcounters_interrupt_signaling_enable (void);
/******************************************************************************
*
* FUNCTION:
*   void ds1103_pcounters_interrupt_signaling_disable (void)
*
* DESCRIPTION:
*   disable interrupt signaling for all counters
*
* PARAMETERS:
*   none
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   06.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_pcounters_interrupt_signaling_disable (void);

/******************************************************************************
*
* FUNCTION:
*   void ds1103_pcounter_mode_set(UInt32 counter,
*                                 UInt32 select,
*                                 UInt32 int_counter,
*                                 DS1103_PC_Handler_Type handler)
*
* DESCRIPTION:
*   
*
* PARAMETERS:
*   counter     : number of the performance counter, 1 or 2
*   select      : bit pattern to select the count event for the specified counter
*   int_counter : count of counter incrementions until an interrupt happens
*   handler     : pointer to a function to trigger from an interrupt
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   06.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_pcounter_mode_set(UInt32 counter,
                              UInt32 select,
                              UInt32 int_counter,
                              DS1103_PC_Handler_Type handler);



#undef __INLINE

#ifdef _INLINE
 #include <Pm1103.c>
#endif

#endif


