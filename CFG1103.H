/******************************************************************************
*
* FILE:
*   Cfg1103.h
*
* RELATED FILES:
*
* DESCRIPTION:
*     This file contains definitions for config section access. 
*
* HISTORY:
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: CFG1103.H $ $Revision: 1.2 $ $Date: 2003/08/07 14:53:47GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#ifndef __Cfg1103__
#define __Cfg1103__

#include <Dstypes.h>
#include <Ds1103.h>

#define VCM_AVAILABLE 1                    /* this RTLib uses the VCM module */

#define DS1103_CFG_DYNAMIC_START  0x08010000

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/


 typedef struct {                            /* Config section */
   UInt32  __cfg_config_version;
   UInt32  __cfg_boardname;
   UInt32  __cfg_serial_nr;
   UInt32  __cfg_version;
   UInt32  __cfg_revision;
   UInt32  __cfg_sub_version;
   UInt32  __cfg_cpu_clock;
   UInt32  __cfg_bus_clock;
   UInt32  __cfg_global_ram_size;
   UInt32  __cfg_local_ram_size;
   UInt32  __cfg_reserved;
   UInt32  __cfg_end_of_prog;
   UInt32  __cfg_stacksize;
   UInt32  __cfg_hostmem_start;
   UInt32  __cfg_hostmem_size;
   UInt32  __cfg_virt_hostmem_start;
   UInt32  __cfg_comm_flag;
   UInt32  __cfg_except_flag;
   UInt32  __cfg_STP;
   UInt32  __cfg_STRP;
   UInt32  __cfg_CQP;
   UInt32  __cfg_AQP;
   UInt32  __cfg_HostService_Struct;
   UInt32  __cfg_MallocSecStart;
   UInt32  __cfg_MallocSecEnd;
   UInt32  __cfg_MallocSecPtr;
   UInt32  __cfg_ver_string_tbl_ptr;
   Int32   __cfg_cpu_temp;
   UInt32  __cfg_overtemp_flag;
   UInt32  __cfg_obsolete[2];
   UInt32  __cfg_register_dump[96];
   UInt32  __cfg_exc_addr;
   UInt32  __cfg_exc_srr1;
   UInt32  __cfg_exc_summary;
   UInt32  __cfg_exc_counter[12];
   UInt32  __cfg_msg_offs;
   UInt32  __cfg_slave_dsp_status;
   UInt32  __cfg_slave_dsp_ds_fwrev;
   UInt32  __cfg_slave_dsp_usr_fwrev[3];
   UInt32  __cfg_slave_mc_ds_fwrev;
   UInt32  __cfg_slave_mc_usr_fwrev[3];
   UInt32  __cfg_slv_boot_flag;
   UInt32  __cfg_iabr;
   UInt32  __cfg_kw_iabr[4];
   UInt32  __cfg_exc_dsisr;
   UInt32  __cfg_exc_dar;
   UInt32  __cfg_cr_register_dump;
   UInt32  __cfg_xer_register_dump;
   UInt32  __cfg_ctr_register_dump;
   UInt32  __cfg_lr_register_dump;
   UInt32  __cfg_vcm_version_high;
   UInt32  __cfg_vcm_version_low;
   UInt32  __cfg_vcm_num_modules;
   UInt32  __cfg_vcm_max_name_length;
   UInt32  __cfg_vcm_valid_word;
   Int32   __cfg_vcm_first_entry;
   UInt32  __cfg_data_cache_flushed;
   UInt32  __cfg_dummy[120];
   UInt32  __cfg_host_subint;
 } cfg_section_type;

#define cfg_msg_offs __cfg_msg_offs          /* definition for SCOUT */

#define cfg_section               ds1103_cfg_section
extern volatile cfg_section_type* ds1103_cfg_section;


 /* constants for communication flag states */

#define CFG_FL_CLEARED                 0x0    /* cleared by host PC */
#define CFG_FL_RESET_SUCCESS           0x1    /* reset was successful */
#define CFG_FL_BAT_REGISTERS_SET       0x2    /* BAT registers are set */
#define CFG_FL_ADDRESS_TRANSLATION_ON  0x3    /* address translation on */
#define CFG_FL_CACHE_ON                0x4    /* instruct. and data cache on */
#define CFG_FL_INTERRUPT_REGISTERS_SET 0x5    /* interrupt input/mask register is cleared/set */
#define CFG_FL_STACK_INIT              0x6    /* stackpointer is set */
#define CFG_FL_HEAP_INIT               0x7    /* heap is created */
#define CFG_FL_WAIT                    0x8    /* processor waits until flag is cleared*/
#define CFG_FL_RECEIVE_FLAG            0x9    /* processor received cleared flag */
#define CFG_FL_MEMORY_CLEARED          0xa    /* bss sections and hostmem section are cleared */
#define CFG_FL_END_STARTUP             0xb    /* startup code is left */
#define CFG_FL_OVER_TEMPERATURE        0xc    /* processor temperature is higher than allowed */
#define CFG_FL_EXIT                    0xd    /* program was left with exit */
/***********************************************************************/

/* constants for exception flag states */

#define CFG_EXC_NO_EXCEPT                0x0  /* no exception */
#define CFG_EXC_MACHINE_CHECK            0x1  /* machine check */
#define CFG_EXC_DATA_ACCESS_ERR          0x2  /* data access error */
#define CFG_EXC_INSTR_ACCESS_ERR         0x3  /* instruction access error */
#define CFG_EXC_ILLEGAL_INT_ERR          0x4  /* illegal (uninitialized) interrupt */
#define CFG_EXC_ALIGN_ERR                0x5  /* alignment error */
#define CFG_EXC_PROG_ERR                 0x6  /* program error */
#define CFG_EXC_FP_ARITHMETICAL          0x7  /* fp arithmetical exception */
#define CFG_EXC_FP_UNAVAIL               0x8  /* floating point unavailable */
#define CFG_EXC_SYSTEM_CALL              0x9  /* system call */
#define CFG_EXC_INST_ADDR_BREAKPOINT     0xa  /* instruction address breakpoint */
/***********************************************************************/


/* code words */

#define CFG1103_VALID_KEYWORD_0  0x2A526169         /* keyword */
#define CFG1103_VALID_KEYWORD_1  0x6E65722A
#define CFG1103_VALID_KEYWORD_2  0x23427573
#define CFG1103_VALID_KEYWORD_3  0x63686523


#define STACK_INIT_CODE 0x53544143       /* STAC... */

#endif

