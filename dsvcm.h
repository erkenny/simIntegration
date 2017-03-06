/******************************************************************************
*  FILE:
*    DSVCM.H
*
*  DESCRIPTION:
*    Version and Config section Management
*
*  REMARKS:
*
*  AUTHOR(S):  G. Theidel, V. Lang, T. Witteler
*
*  Copyright (c) 1999 - 2000 dSPACE GmbH, GERMANY
*
* $RCSfile: dsvcm.h $ $Revision: 1.10 $ $Date: 2009/04/02 11:25:54GMT+01:00 $
*
******************************************************************************/

#ifndef __DSVCM_H__
#define __DSVCM_H__

/* $DSVERSION: DSVCM - Version and Config Section Management Module */

#define DSVCM_VER_MAR 1
#define DSVCM_VER_MIR 6
#define DSVCM_VER_MAI 0
#define DSVCM_VER_SPB VCM_VERSION_RELEASE
#define DSVCM_VER_SPN 0
#define DSVCM_VER_PLV 0

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

#define VCM_CFG_OFFS_PTR(OFFSET) \
        ((vcm_module_descriptor_type*)((char*)CFG_SECTION_START+(OFFSET)))

#define VCM_CFG_PTR_OFFS(PTR) \
        ((Int32)PTR-(Int32)CFG_SECTION_START)

/******************************************************************************
  DS1003 specific definitions
******************************************************************************/
#if defined(_DS1003)

#include <cfg1003.h>

#ifdef _INLINE
#define __INLINE static inline
#else
#define __INLINE
#endif

#ifndef _DSHOST
extern unsigned long _cfg_rev;
static volatile cfg_section_type *cfg_section_ptr = (volatile cfg_section_type *) &_cfg_rev;
#endif

#define VCM_MID_RTLIB VCM_MID_RTLIB1003
#define VCM_AF 4
#define CFG_SECTION_START  (void *)cfg_section_ptr
#define CFG_PTR cfg_section_ptr
#define CFG_VCM_FIRST_MODULE_OFSS  (CFG_PTR->__cfg_vcm_first_entry)
#define CFG_VCM_SET_FIRST_MODULE_OFFS(FIRST_PTR)  (CFG_PTR->__cfg_vcm_first_entry=FIRST_PTR)
#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))

/******************************************************************************
  DS1004 specific definitions
******************************************************************************/
#elif (defined (_DS1004) || defined(__alpha__))

#include <cfg1004.h>

#ifdef _INLINE
#define __INLINE static inline
#else
#define __INLINE
#endif

#define VCM_MID_RTLIB VCM_MID_RTLIB1004
#define VCM_AF 1
#define CFG_SECTION_START ((void*)DS1004_DPMEM_BASE)
#define CFG_PTR ((cfg_section_type *)DS1004_DPMEM_BASE)
#define CFG_VCM_FIRST_MODULE_OFSS  (CFG_PTR->__cfg_vcm_first_entry)
#define CFG_VCM_SET_FIRST_MODULE_OFFS(FIRST_PTR)  (CFG_PTR->__cfg_vcm_first_entry=FIRST_PTR)
#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))

/******************************************************************************
  DS1005 specific definitions
******************************************************************************/
#elif defined (_DS1005)

#include <cfg1005.h>
#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif

#define VCM_MID_RTLIB     VCM_MID_RTLIB1005
#define VCM_MID_FIRMWARE  VCM_MID_FIRMWARE1005
#define VCM_MID_BOARD     VCM_MID_BOARD1005
#define VCM_AF 1
#define CFG_SECTION_START ((void*)DS1005_CFG_STRUCT_ADDR)
#define CFG_PTR ((cfg_section_type *)DS1005_CFG_STRUCT_ADDR)
#define CFG_VCM_FIRST_MODULE_OFSS  (CFG_PTR->__cfg_vcm_first_entry)
#define CFG_VCM_SET_FIRST_MODULE_OFFS(FIRST_PTR)  (CFG_PTR->__cfg_vcm_first_entry=FIRST_PTR)
#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))
/*
 * macros needed for adding specific symbols to the ELF file
 * <expands to asm(" sym, val ") >
 */
#define __DSVER_STR(x) #x
#define DSVER_STR(x) __DSVER_STR(x)
#define DS_VERSION_SYMBOL(sym,val) asm(".equ " #sym ","  DSVER_STR(val))

/******************************************************************************
  DS1006 specific definitions
******************************************************************************/
#elif defined (_DS1006)

#include <cfg1006.h>
#ifdef _INLINE
#define __INLINE static inline
#else
#define __INLINE
#endif

#define VCM_MID_RTLIB     VCM_MID_RTLIB1006
#define VCM_MID_FIRMWARE  VCM_MID_FIRMWARE1006
#define VCM_MID_BOARD     VCM_MID_BOARD1006
#define VCM_AF 1
#define CFG_SECTION_START ((void*)DS1006_CFG_STRUCT_ADDR)
#define CFG_PTR ((cfg_section_type *)DS1006_CFG_STRUCT_ADDR)
#define CFG_VCM_FIRST_MODULE_OFSS  (CFG_PTR->__cfg_vcm_first_entry)
#define CFG_VCM_SET_FIRST_MODULE_OFFS(FIRST_PTR)  (CFG_PTR->__cfg_vcm_first_entry=FIRST_PTR)
#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))
/*
 * macros needed for adding specific symbols to the ELF file
 * <expands to asm(" sym, val ") >
 */
#define __DSVER_STR(x) #x
#define DSVER_STR(x) __DSVER_STR(x)
#define DS_VERSION_SYMBOL(sym,val) asm(".equ " #sym ","  DSVER_STR(val))

/******************************************************************************
  DS2510 specific definitions
******************************************************************************/
#elif defined (DS_PLATFORM_SMART)

/*
 * We have no config section.
 * Only the first vcm entry.
 */
//#include <cfg2510.h>
extern int __cfg_vcm_first_entry;


#ifdef _INLINE
#define __INLINE static inline
#else
#define __INLINE
#endif

#define VCM_MID_RTLIB     VCM_MID_RTLIB2510
//#define VCM_MID_FIRMWARE  VCM_MID_FIRMWARE2510
#define VCM_AF 1
/* 
 * All VCM-Offsets are relative to CFG_SECTION_START.
 * 
 * All VCM-Offsets are signed 32-Bit integer.
 * ==> We could describe positive offsets up to 2GB and
 * negative offsets down to -2GB.
 * 
 * We have 4GB-Memory.
 * ==> If we set CFG_SECTION_START to 2GB (the middle from
 * the 4GB-Memory), all positions in the 4GB-Memory could
 * be described as an offset from CFG_SECTION_START.
 */
#define CFG_SECTION_START 0x80000000
/*
 * We have no config section.
 * #define CFG_PTR ((cfg_section_type *)0x80000000)
 */
#define CFG_VCM_FIRST_MODULE_OFSS __cfg_vcm_first_entry
#define CFG_VCM_SET_FIRST_MODULE_OFFS(FIRST_PTR)  (__cfg_vcm_first_entry=FIRST_PTR)
#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))
/*
 * macros needed for adding specific symbols to the ELF file
 * <expands to asm(" sym, val ") >
 */
#define __DSVER_STR(x) #x
#define DSVER_STR(x) __DSVER_STR(x)
#define DS_VERSION_SYMBOL(sym,val) asm(".equ " #sym ","  DSVER_STR(val))

/******************************************************************************
  DS1102 specific definitions ( dummy definitions for common board class )
******************************************************************************/
#elif defined (_DS1102)

#include <cfg1102.h>
#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif

#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))

/******************************************************************************
  DS1103 specific definitions
******************************************************************************/
#elif defined (_DS1103)

#include <cfg1103.h>
#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif

#define VCM_MID_RTLIB VCM_MID_RTLIB1103
#define VCM_MID_BOARD VCM_MID_BOARD1103
#define VCM_AF 1
#define CFG_SECTION_START ((void*)DS1103_GLOBAL_RAM_START)
#define CFG_PTR ((cfg_section_type *)DS1103_GLOBAL_RAM_START)
#define CFG_VCM_FIRST_MODULE_OFSS  (CFG_PTR->__cfg_vcm_first_entry)
#define CFG_VCM_SET_FIRST_MODULE_OFFS(FIRST_PTR)  (CFG_PTR->__cfg_vcm_first_entry=FIRST_PTR)
#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))
/*
 * macros needed for adding specific symbols to the ELF file
 * <expands to asm(" sym, val ") >
 */
#define __DSVER_STR(x) #x
#define DSVER_STR(x) __DSVER_STR(x)
#define DS_VERSION_SYMBOL(sym,val) asm(".equ " #sym ","  DSVER_STR(val))


/******************************************************************************
  DS1401 specific definitions
******************************************************************************/
#elif defined (_DS1401)

#include <cfg1401.h>

#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif

#define VCM_MID_RTLIB     VCM_MID_RTLIB1401
#define VCM_MID_FIRMWARE  VCM_MID_FIRMWARE1401
#define VCM_MID_BOARD     VCM_MID_BOARD1401
#define VCM_AF 1
#define CFG_SECTION_START ((void*)DS1401_GLOBAL_RAM_START)
#define CFG_PTR ((volatile cfg_section_type*)DS1401_GLOBAL_RAM_START)
#define CFG_VCM_FIRST_MODULE_OFSS  (CFG_PTR->__cfg_vcm_first_entry)
#define CFG_VCM_SET_FIRST_MODULE_OFFS(FIRST_PTR)  (CFG_PTR->__cfg_vcm_first_entry=FIRST_PTR)
#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))
/*
 * macros needed for adding specific symbols to the ELF file
 * <expands to asm(" sym, val ") >
 */
#define __DSVER_STR(x) #x
#define DSVER_STR(x) __DSVER_STR(x)
#define DS_VERSION_SYMBOL(sym,val) asm(".equ " #sym ","  DSVER_STR(val))


/******************************************************************************
  DS1104 specific definitions
******************************************************************************/
#elif defined (_DS1104)

#include <cfg1104.h>
#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif

#define VCM_MID_RTLIB     VCM_MID_RTLIB1104
#define VCM_MID_FIRMWARE  VCM_MID_FIRMWARE1104
#define VCM_MID_BOARD     VCM_MID_BOARD1104
#define VCM_AF 1
#define CFG_SECTION_START ((void*)DS1104_CFG_STRUCT_ADDR)
#define CFG_PTR ((cfg_section_type *)DS1104_CFG_STRUCT_ADDR)
#define CFG_VCM_FIRST_MODULE_OFSS  (CFG_PTR->__cfg_vcm_first_entry)
#define CFG_VCM_SET_FIRST_MODULE_OFFS(FIRST_PTR)  (CFG_PTR->__cfg_vcm_first_entry=FIRST_PTR)
#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))
/*
 * macros needed for adding specific symbols to the ELF file
 * <expands to asm(" sym, val ") >
 */
#define __DSVER_STR(x) #x
#define DSVER_STR(x) __DSVER_STR(x)
#define DS_VERSION_SYMBOL(sym,val) asm(".equ " #sym ","  DSVER_STR(val))


/******************************************************************************
  Rapid Pro Control Unit - MPC565 specific definitions
******************************************************************************/
#elif defined (_RP565)

#define __DSVER_STR(x) #x
#define DSVER_STR(x) __DSVER_STR(x)
#define DS_VERSION_SYMBOL(sym,val) asm(".equ " #sym ","  DSVER_STR(val))

#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))

/******************************************************************************
  Rapid Pro Control Unit - MPC5554 specific definitions
******************************************************************************/
#elif defined (_RP5554)
#define __INLINE
#define __DSVER_STR(x) #x
#define DSVER_STR(x) __DSVER_STR(x)
#define DS_VERSION_SYMBOL(sym,val) asm(".equ " #sym ","  DSVER_STR(val))

#define VCM_NAME_LENGTH 32
#define VCM_MAX_NAME_LENGTH  ((VCM_NAME_LENGTH+3)&(~3))

/******************************************************************************/
#elif defined (_DSHOST)
#define __INLINE
/* code changed by sho on 16.11.99 for ControlDesk 1.2.1 */
#ifndef  VCM_MAX_NAME_LENGTH
#define VCM_MAX_NAME_LENGTH 32
#endif
/* end of code changed by sho on 16.11.99 for ControlDesk 1.2.1 */
#else
#error dsvcm.h: no processor board defined (e.g. _DS1003)
#endif


/******************************************************************************
  module defines
******************************************************************************/

#include <dsmodule.h>

#define VCM_VERSION_LT              0x01    /* Less Than        */
#define VCM_VERSION_LE              0x03    /* Less or Equal    */
#define VCM_VERSION_EQ              0x02    /* EQual            */
#define VCM_VERSION_GE              0x06    /* Greater or Equal */
#define VCM_VERSION_GT              0x04    /* Greater Than     */

#define VCM_VERSION_RELEASE         0x0F    /*  Release version */
#define VCM_VERSION_BETA            0x0B    /*  Beta version    */
#define VCM_VERSION_ALPHA           0x0A    /*  Alpha version   */

#define VCM_VERSION_INVALID         0x0C    /*  Invalid version */


#define VCM_STATUS_ERROR            0x02
#define VCM_STATUS_INITIALIZED      0x01
#define VCM_STATUS_UNINITIALIZED    0x00

#define VCM_CTRL_HIDDEN             0x01    /* hide complete module    */
#define VCM_CTRL_NO_VS              0x02    /* hide version            */
#define VCM_CTRL_NO_ST              0x04    /* hide status             */
#define VCM_CTRL_NAME_ONLY          0x06    /* hide version and status */

#define VCM_CTRL_SHOW_TS            0x10    /* future use */
#define VCM_CTRL_SHOW_XI            0x20    /* future use */
#define VCM_CTRL_SHOW_AM            0x40    /* future use */


#define VCM_NO_ERROR                   0
#define VCM_AUTO_ID                    0

#define VCM_INVALID_MODULE            -1
#define VCM_REGISTER_FAILED           -2
#define VCM_ILLEGAL_SIZE_OF_BUFFER    -3

#define VCM_VALID_WORD        0xA56BDCE7


#define VCM_OFFS_MOD_ID          (0x0000/VCM_AF)
#define VCM_OFFS_STATUS          (0x0004/VCM_AF)
#define VCM_OFFS_VERSION         (0x0008/VCM_AF)
#define VCM_OFFS_VERSION_HIGH    (0x0008/VCM_AF)
#define VCM_OFFS_VERSION_LOW     (0x000C/VCM_AF)
#define VCM_OFFS_TIMESTAMP       (0x0010/VCM_AF)
#define VCM_OFFS_TIMESTAMP_HIGH  (0x0010/VCM_AF)
#define VCM_OFFS_TIMESTAMP_LOW   (0x0014/VCM_AF)
#define VCM_OFFS_NEXT_OFFS       (0x0018/VCM_AF)
#define VCM_OFFS_SUB_OFFS        (0x001C/VCM_AF)
#define VCM_OFFS_LIN_OFFS        (0x0020/VCM_AF)
#define VCM_OFFS_XTRA_INFO       (0x0024/VCM_AF)
#define VCM_OFFS_CFG_MEM_OFFS    (0x0028/VCM_AF)
#define VCM_OFFS_CFG_MEM_SIZE    (0x002C/VCM_AF)
#define VCM_OFFS_CONTROL         (0x0030/VCM_AF)
#define VCM_OFFS_RESERVED_OFFS   (0x0034/VCM_AF)
#define VCM_OFFS_MODULE_NAME     (0x0038/VCM_AF)

#define VCM_MODUL_ENTRY_SIZE ((VCM_OFFS_MODULE_NAME*VCM_AF)+VCM_MAX_NAME_LENGTH)

/******************************************************************************
  C data structure
******************************************************************************/

#ifndef __ASSEMBLER_CODE__   /* some things are only valid for C programmers */
#include <dstypes.h>

typedef struct
{
  UInt32  mat;
  UInt32  mit;
} vcm_timestamp_type;

typedef  UInt32 vcm_size_t;
typedef  void* vcm_cfg_mem_ptr_type;

typedef struct vcm_module_descriptor_struct
{
  Int32               vcm_mod_id;
  Int32               vcm_status;
  vcm_version_type    vcm_version;
  vcm_timestamp_type  vcm_timestamp;
  Int32               vcm_next_offs;
  Int32               vcm_sub_offs;
  Int32               vcm_lin_offs;
  UInt32              vcm_xtra_info;
  Int32               vcm_cfg_mem_offs;
  UInt32              vcm_cfg_mem_size;
  UInt32              vcm_control;
  Int32               vcm_reserved_offs;
  char                vcm_module_name[VCM_MAX_NAME_LENGTH];
} vcm_module_descriptor_type;


/******************************************************************************
  function prototypes
******************************************************************************/

#if !defined _RP565

__INLINE vcm_module_descriptor_type* vcm_module_find(Int32 mod_id,
                                                     vcm_module_descriptor_type *prev_ptr);
__INLINE vcm_cfg_mem_ptr_type vcm_memory_ptr_get(vcm_module_descriptor_type* ptr);

__INLINE void vcm_entry_changed(void);

void vcm_init(void);

void *vcm_cfg_malloc(vcm_size_t size);

vcm_module_descriptor_type* vcm_module_register(UInt32  mod_id,
                                                vcm_module_descriptor_type *main_ptr,
                                                char*   module_name,
                                                UInt8   mar,
                                                UInt8   mir,
                                                UInt8   mai,
                                                UInt8   spb,
                                                UInt16  spn,
                                                UInt16  plv,
                                                UInt32  xtra_info,
                                                UInt32  control);

Int32 vcm_memory_ptr_set(vcm_module_descriptor_type* ptr,
                         vcm_cfg_mem_ptr_type cfg_mem_ptr,
                         UInt32 size);

Int32 vcm_module_status_get(vcm_module_descriptor_type* ptr);

Int32 vcm_module_status_set(vcm_module_descriptor_type* ptr,
                            Int32 status);

vcm_version_type vcm_version_get(vcm_module_descriptor_type* ptr);

Int32 vcm_version_compare(vcm_module_descriptor_type* ptr,
                          Int32 operation,
                          UInt8 mar,
                          UInt8 mir,
                          UInt8 mai,
                          UInt8 spb,
                          UInt16 spn,
                          UInt16 plv);

Int32 vcm_module_version_print(char *buffer,
                               vcm_module_descriptor_type* ptr);

Int32 vcm_version_print(char *buffer,
                        UInt8 mar,
                        UInt8 mir,
                        UInt8 mai,
                        UInt8 spb,
                        UInt16 spn,
                        UInt16 plv);
                        
#if !defined(DS_PLATFORM_SMART)
Int32 vcm_version_update(vcm_module_descriptor_type* ptr,
                                          UInt8 mar,
                                          UInt8 mir,
                                          UInt8 mai,
                                          UInt8 spb,
                                          UInt16 spn,
                                          UInt16 plv);

Int32 vcm_memory_update(vcm_module_descriptor_type* ptr,
                        UInt32*                     buffer,
                        UInt32                      size_of_buffer /* in bytes! */ );                        
#endif


#undef __INLINE

#endif                                                /* #if !defined _RP565 */

/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
  #include <dsvcm.c>
#endif

#endif /* __ASSEMBLER_CODE__ */

#endif /* __DSVCM_H__ */
