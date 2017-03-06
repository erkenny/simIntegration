/******************************************************************************
*
* MODULE
*   dual port accesses for master on DS1103 board
*
* FILE
*   dpm1103.h
*
* RELATED FILES
*   dpm1103.c
*
* DESCRIPTION
*    - definitions for dual port memory accesses from the master (PowerPC)(always direct accesses)
*
* AUTHOR(S)
*   H.-G. Limberg
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dpm1103.h $ $Revision: 1.2 $ $Date: 2003/08/07 12:19:25GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSSINTMCOM1103.pj $
******************************************************************************/

#ifndef __DPM1103_H__
#define __DPM1103_H__

#include <dstypes.h>
/* #include <tmr1103.h> */

/* interrupt control */

/*
  note: any function using these macros must declare an unsigned long variable for int_status
*/

/* DS1103 interrupt definitions */
/*
#include <int1103.h>

#define DPM_INT_DISABLE(int_status)             \
{                                               \
  int_status = ds1103_get_interrupt_status();   \
  DS1103_GLOBAL_INTERRUPT_DISABLE();            \
}

#define DPM_INT_RESTORE(int_status)             \
{                                               \
  ds1103_set_interrupt_status(int_status);      \
}
*/

#define DPM_TARGET_DIRECT 0  /* target for dpm_access_desc with direct dpm access */
#define DPM_ACCESS_DIRECT 0  /* write/read function pointer for dpm_access_desc with direct dpm access */


typedef unsigned long (*dpm_write_fcn_t)(long target, unsigned long offset, unsigned long ptr_inc, unsigned long data);
typedef unsigned long (*dpm_read_fcn_t) (long target, unsigned long offset, unsigned long ptr_inc);



#define DPM_WRITE(dpm_access_desc, offset, ptr_inc, data) \
*(((UInt16*)(offset)) + (ptr_inc)) = (UInt16)(data)

#define DPM_READ(dpm_access_desc, offset, ptr_inc) \
*(((UInt16*)(offset)) + (ptr_inc))

#define DPM_TIMER_BIT_WIDTH 32               /* valid timer width */
#define DPM_TIMER_PERIOD ds1103_decr_prd /* period of the timer clock */


#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif


/* function prototypes */

__INLINE unsigned long dpm_timer_read (void);

#undef __INLINE


/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
#include <dpm1103.c>
#endif

#endif












