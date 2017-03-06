/******************************************************************************
*
* FILE:
*   pm1103.c
*
* RELATED FILES:
*   pm1103.h
*
* DESCRIPTION:
*   performance measurement support for DS1103 board
*
* HISTORY:
*   06.04.98  Rev 1.0  H.-G. Limberg     initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: pm1103.c $ $Revision: 1.2 $ $Date: 2003/08/07 14:54:16GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/
asm(".supervisoron");
#include <pm1103.h>


/******************************************************************************
  program global variables
******************************************************************************/
extern DS1103_PC_Handler_Type* ptPMVecTable;

/******************************************************************************
  modul global variables
******************************************************************************/

#ifdef _INLINE
  #define __INLINE static
#else
  #define __INLINE
#endif


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
__INLINE UInt32 ds1103_pcounter_get(UInt32 counter)
{
  if (counter == 1)
  {
    asm("mfspr 5,953");          /* read counter value in r5 */
  }
  else if ( counter == 2)
  {
    asm("mfspr 5,954");
  }
  else if ( counter == 3)
  {
    asm("mfspr 5,957");
  }
  else if ( counter == 4)
  {
    asm("mfspr 5,958");
  }

  return(asm("mr 3,5"));          /* return counter value */
}



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
__INLINE UInt32 ds1103_pcounter_reset(UInt32 counter)
{
  if (counter == 1)
  {
    asm("mfspr 5,953");          /* save counter value in r5 */
    asm("lis   4,0x0");          /* reset counter */
    asm("mtspr 953,4");
  }
  else if ( counter == 2)
  {
    asm("mfspr 5,954");
    asm("lis   4,0x0");
    asm("mtspr 954,4");
  }
  else if ( counter == 3)
  {
    asm("mfspr 5,957");
    asm("lis   4,0x0");
    asm("mtspr 957,4");
  }
  else if ( counter == 4)
  {
    asm("mfspr 5,958");
    asm("lis   4,0x0");
    asm("mtspr 958,4");
  }
  return(asm("mr 3,5"));          /* return counter value */
}

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
__INLINE void ds1103_pcounters_enable (void)
{
  asm("mfspr  3,952");
  asm("lis    4,0x7FFF");
  asm("ori    4,4,0xFFFF");
  asm("and    3,3,4");
  asm("mtspr  952,3");
}


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
__INLINE void ds1103_pcounters_disable (void)
{
  asm("mfspr  3,952");
  asm("lis    4,0x8000");
  asm("or     3,3,4");
  asm("mtspr  952,3");
}
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
__INLINE void ds1103_pcounters_interrupt_signaling_enable (void)
{
  asm("mfspr  3,952");
  asm("lis    4,0x0400");
  asm("or     3,4,3");
  asm("mtspr  952,3");
}

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
__INLINE void ds1103_pcounters_interrupt_signaling_disable (void)
{
  asm("mfspr  3,952");
  asm("lis    4,0xFBFF");
  asm("ori    4,4,0xFFFF");
  asm("and    3,4,3");
  asm("mtspr  952,3");
}

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
*   counter     : number of the performance counter, 1..4
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
                              DS1103_PC_Handler_Type handler)
{
  asm("lis  7,0x8000");      /* load r7 with interrupt causing value (first negativ value) */
  asm("subf 5,5,7");         /* substract int_count from r7, result in r5 */
  if (counter == 1)
  {
    *ptPMVecTable  = handler;
    asm("mtspr 953,5");     /* load performance counter1 with r5 */
    asm("li    5,0x007F");  /* set mask for counter 1 event (7bits) */
    asm("and   4,4,5");     /* mask out the other bits of "select" */
    asm("slwi  4,4,6");     /* shift left to the counter1 event position in MMCR0 */
    asm("mfspr 5,952");     /* load MMCR0 */
    asm("ori   5,5,0x1FC0"); /* clear bit 19..25 */
    asm("xori  5,5,0x1FC0");
    asm("or    5,5,4");     /* set pcounter event */
    asm("mtspr 952,5");
  }

  else if (counter == 2)
  {
    *(ptPMVecTable + 1)  = handler;
    asm("mtspr 954,5");     /* load performance counter2 with r5 */
    asm("li    5,0x003F");  /* set mask for counter 2 event (6bits) */
    asm("and   4,4,5");     /* mask out the other bits of "select" */
    asm("mfspr 5,952");     /* load MMCR0 */
    asm("ori   5,5,0x003F"); /* clear bit 26..31 */
    asm("xori  5,5,0x003F");
    asm("or    5,5,4");     /* set pcounter event */
    asm("mtspr 952,5");
  }

  else if (counter == 3)
  {
    *(ptPMVecTable + 2)  = handler;
    asm("mtspr 957,5");     /* load performance counter3 with r5 */
    asm("li    5,0x001F");  /* set mask for counter 3 event (5bits) */
    asm("and   4,4,5");     /* mask out the other bits of "select" */
    asm("slwi  4,4,27");    /* shift left to the counter3 event position in MMCR0 */
    asm("mfspr 5,956");     /* load select value to MMCR1 */
    asm("oris  5,5,0xF800"); /* clear bit 0..4 */
    asm("xoris 5,5,0xF800");
    asm("or    5,5,4");
    asm("mtspr 956,5");
  }

  else if (counter == 4)
  {
    *(ptPMVecTable + 3)  = handler;
    asm("mtspr 958,5");     /* load performance counter4 with r5 */
    asm("li    5,0x001F");  /* set mask for counter 4 event (5bits) */
    asm("and   4,4,5");     /* mask out the other bits of "select" */
    asm("slwi  4,4,22");    /* shift left to the counter4 event position in MMCR0 */
    asm("mfspr 5,956");     /* load select value to MMCR1 */
    asm("oris  5,5,0x07C0"); /* clear bit 5..9 */
    asm("xoris 5,5,0x07C0");
    asm("or    5,5,4");
    asm("mtspr 956,5");
  }
}


#undef __INLINE

