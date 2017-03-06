/******************************************************************************
*
* FILE:
*   info1103.h
*
* RELATED FILES:
*
* DESCRIPTION:
*   configuration and revision information modul
*
* HISTORY:
*   14.04.98  Rev 1.0  H.-G. Limberg     initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: INFO1103.H $ $Revision: 1.1 $ $Date: 1998/08/18 09:45:16GMT+01:00 $
******************************************************************************/
#ifndef __INFO1103_H__
#define __INFO1103_H__

#include <dstypes.h>

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE extern
#endif


/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

/******************************************************************************
*
* FUNCTION:
*   void ds1103_info_board_version_get(UInt32 *version,
*                                      UInt32 *revision,
*                                      UInt32 *sub_version)
*
* DESCRIPTION:
*   gets the board version
*
*
* PARAMETERS:
*   version     : address of the variable holding the board version
*   revision    : address of the variable holding the board revision
*   sub_version : address of the variable holding the board sub version
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   14.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_info_version_board_get(UInt32 *version,
                                   UInt32 *revision,
                                   UInt32 *sub_version);

/******************************************************************************
*
* FUNCTION:
*   void ds1103_info_memory_get(UInt32 *local_size, UInt32 *local_base,
*                               UInt32 *global_size, UInt32 * global_base,
*                               UInt32 *slave_dp, UInt32 *slave_dp_base)
*
* DESCRIPTION:
*   gets the sizes and the base addresses of the local, global and slave DSP dual port memory
*   in bytes from the config section
*
*
* PARAMETERS:
*   local_size    : address of the variable holding the local memory size
*   local_base    : address of the variable holding the local memory base address
*   global_size   : address of the variable holding the global memory size
*   global_base   : address of the variable holding the global memory base address
*   slave_dp_size : address of the variable holding the slave DSP dual port memory size
*   slave_dp_base : address of the variable holding the slave DSP dual port memory base address
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   14.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_info_memory_get(UInt32 *local_size,    UInt32 *local_base,
                            UInt32 *global_size,   UInt32 *global_base,
                            UInt32 *slave_dp_size, UInt32 *slave_dp_base);

/******************************************************************************
*
* FUNCTION:
*   void ds1103_info_clocks_get(UInt32 *cpu_clock, UInt32 *bus_clock)
*
* DESCRIPTION:
*   gets the value of the cpu and bus clock in Hz from the config section
*
*
* PARAMETERS:
*   cpu_clock : address of the variable holding the cpu clock
*   bus_clock : address of the variable holding the bus clock
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   14.04.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_info_clocks_get(UInt32 *cpu_clock, UInt32 *bus_clock);

/******************************************************************************
*
* FUNCTION:
*   Int8 ds1103_info_cpu_temperature_get(void)
*
* DESCRIPTION:
*   gets the value of the cpu tempearature
*
*
* PARAMETERS:
*   none
*
* RETURNS:
*   the current cpu temperature in degree Celsius
*
* NOTE:
*
* HISTORY:
*   16.04.98  H.-G. Limberg, initial version
*   24.04.98  H.-G. Limberg, read the temperature from sensor into config section
*                            before return config section value
*
******************************************************************************/
__INLINE Int8 ds1103_info_cpu_temperature_get(void);


#undef __INLINE


#ifdef _INLINE
  #include <info1103.c>
#endif


#endif
