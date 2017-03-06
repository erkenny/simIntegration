/******************************************************************************
*
* FILE:
*   info1103.c
*
* RELATED FILES:
*   info1103.h
*
* DESCRIPTION:
*   configuration and revision information modul
*
* HISTORY:
*   14.04.98  Rev 1.0   H.-G. Limberg     initial version
*   16.04.98  Rev 1.0.1 H.-G. Limberg     add temperature info
*
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: INFO1103.C $ $Revision: 1.1 $ $Date: 2000/08/16 09:22:45GMT+01:00 $
******************************************************************************/

#include <cfg1103.h>
#include <info1103.h>
#include <ds1103.h>
#include <Io1103.h>

/******************************************************************************
  program global variables
******************************************************************************/

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE
#endif


/******************************************************************************
  modul global variables
******************************************************************************/

/******************************************************************************
*
* FUNCTION:
*   void ds1103_info__board_version_get(UInt32 *version,
*                                       UInt32 *revision,
*                                       UInt32 *sub_version)
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
__INLINE void ds1103_info_board_version_get(UInt32 *version,
                                   UInt32 *revision,
                                   UInt32 *sub_version)
{
  volatile cfg_section_type *config_section = (volatile cfg_section_type*)DS1103_GLOBAL_RAM_START;

  *version     = config_section->__cfg_version;
  *revision    = config_section->__cfg_revision;
  *sub_version = config_section->__cfg_sub_version;
}

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
                            UInt32 *slave_dp_size, UInt32 *slave_dp_base)
{
  volatile cfg_section_type *config_section = (volatile cfg_section_type*)DS1103_GLOBAL_RAM_START;

  *local_size    = (UInt32)config_section->__cfg_local_ram_size;
  *local_base    = (UInt32)DS1103_LOCAL_RAM_START;
  *global_size   = (UInt32)config_section->__cfg_global_ram_size;
  *global_base   = (UInt32)DS1103_GLOBAL_RAM_START;
  *slave_dp_size = (UInt32)0x2000;     /* the slave DSP dp memory has got a fixed size of 8K */
  *slave_dp_base = (UInt32)DS1103_SLAVE_DSP_DP_RAM_START;
}


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
__INLINE void ds1103_info_clocks_get(UInt32 *cpu_clock, UInt32 *bus_clock)
{
  volatile cfg_section_type *config_section = (volatile cfg_section_type*)DS1103_GLOBAL_RAM_START;

  *cpu_clock = config_section->__cfg_cpu_clock;
  *bus_clock = config_section->__cfg_bus_clock;

}

/******************************************************************************
*
* FUNCTION:
*   Int8 ds1103_info_cpu_temperature_get(void)
*
* DESCRIPTION:
*   gets the value of the cpu temperature
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
__INLINE Int8 ds1103_info_cpu_temperature_get(void)
{
  volatile cfg_section_type *config_section = (volatile cfg_section_type*)DS1103_GLOBAL_RAM_START;

  ds1103_cpu_temperature();

  return((Int8)config_section->__cfg_cpu_temp);
}


#undef __INLINE
