/******************************************************************************
*
* MODULE
*
* FILE
*   dsvcm.c
*
* RELATED FILES
*   dsvcm.h
*
* DESCRIPTION
*   Version and Config section Management module
*
* REMARKS
*
* AUTHOR(S)
*   G. Theidel, V. Lang, T. Witteler
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsvcm.c $ $Revision: 1.14 $ $Date: 2009/04/03 10:37:41GMT+01:00 $
*
******************************************************************************/

#include <dstypes.h>
#include <dsvcm.h>

#if defined(_DS1003)
#include <stdlib.h>

  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif

#elif (defined (_DS1004) || defined(__alpha__))
#include <dpm1004.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif

#elif defined (_DS1103)
#include <dsmem.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif

#elif defined (_DS1005)
#include <dsmem.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif

#elif defined (_DS1006)
#include <dsmem.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif

#elif defined (_DS2510) || defined(_DS6001) || defined(_DS1402)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif

#elif defined (_DS1401)
#include <dsmem.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif

#elif defined (_DS1104)
#include <dsmem.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif

#elif defined (_RP565)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif

#elif defined (_RP5554)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif

#elif defined (_DSHOST)
  #define __INLINE
#else
  #error dsvcm.h: no processor board defined (e.g. _DS1003)
#endif

#if !defined _RP565 && !defined _RP5554

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/


/******************************************************************************
  object declarations
******************************************************************************/


/******************************************************************************
  variables
******************************************************************************/
#if defined (DS_PLATFORM_SMART)
int __cfg_vcm_first_entry;
#endif

/******************************************************************************
  function prototypes
******************************************************************************/

static int uint2ch(char* buffer, UInt32 val);

/******************************************************************************
  version information
******************************************************************************/

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 ||defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401

DS_VERSION_SYMBOL(_dsvcm_ver_mar, DSVCM_VER_MAR);
DS_VERSION_SYMBOL(_dsvcm_ver_mir, DSVCM_VER_MIR);
DS_VERSION_SYMBOL(_dsvcm_ver_mai, DSVCM_VER_MAI);
DS_VERSION_SYMBOL(_dsvcm_ver_spb, DSVCM_VER_SPB);
DS_VERSION_SYMBOL(_dsvcm_ver_spn, DSVCM_VER_SPN);
DS_VERSION_SYMBOL(_dsvcm_ver_plv, DSVCM_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/******************************************************************************
  function declarations
******************************************************************************/


/******************************************************************************
*
* FUNCTION vcm_module_find
*
* DESCRIPTION
*   Find a module with a given module id.
*
* PARAMETERS
*   Int32 mod_id                         - module number
*   vcm_module_descriptor_type* prev_ptr - pointer to previously found module
*
* RETURNS
*   vcm_module_descriptor_type* -
*     pointer to vcm module descriptor, or 0 if not found
*
* REMARKS
*
******************************************************************************/

__INLINE vcm_module_descriptor_type* vcm_module_find(Int32 mod_id,
                                                     vcm_module_descriptor_type *prev_ptr)
{
  vcm_module_descriptor_type *ptr;
  Int32 offset;

  if (prev_ptr) offset=prev_ptr->vcm_lin_offs;
  else  offset=CFG_VCM_FIRST_MODULE_OFSS;

   while(offset)
   {
     ptr=VCM_CFG_OFFS_PTR(offset);
     if (ptr->vcm_mod_id==mod_id) return ptr;
     offset=ptr->vcm_lin_offs;
   }
   return  (vcm_module_descriptor_type*)0;
}


/******************************************************************************
*
* FUNCTION vcm_memory_ptr_get
*
* DESCRIPTION
*   Get the pointer to additional cfg memory of a vcm module entry.
*
* PARAMETERS
*   vcm_module_descriptor_type* ptr - pointer to module descriptor
*
* RETURNS
*   vcm_cfg_mem_ptr_type
*     pointer to additional cfg memory, or 0 if none available
*
* REMARKS
*
******************************************************************************/

__INLINE vcm_cfg_mem_ptr_type vcm_memory_ptr_get(vcm_module_descriptor_type* ptr)
{
  if (ptr)
    if (ptr->vcm_cfg_mem_offs)
      return VCM_CFG_OFFS_PTR(ptr->vcm_cfg_mem_offs);
  return (vcm_cfg_mem_ptr_type)0;
}


/******************************************************************************
*
* FUNCTION vcm_entry_changed
*
* DESCRIPTION
*  increment magic number of vcm root entry
*  to show that a vcm entry has been changed
*
* PARAMETERS
*
* RETURNS
*   void
*
******************************************************************************/

__INLINE void vcm_entry_changed(void)
{
  dsvcm_entry_t *vcm_add_mem_ptr;
  vcm_add_mem_ptr = (dsvcm_entry_t *)vcm_memory_ptr_get(vcm_module_find(VCM_MID_DSVCM,0));

  if (vcm_add_mem_ptr)
  {
    vcm_add_mem_ptr->magic_number++;
  }
}


/******************************************************************************
*
* FUNCTION uint2ch
*
* DESCRIPTION
*   Convert unsigned integer to character string (decimal format)
*
* PARAMETERS
*   *buffer     -  pointer to character buffer
*   UInt32 val  -  value
*
* RETURNS
*   number of digits
*
* REMARKS
*
******************************************************************************/

__INLINE int uint2ch(char* buffer,UInt32 val)
{
  UInt32 div=1000000000,count=0;
  while(div)
  {
    if ((val/div)||div==1)
    {
       *buffer++=('0'+((val/div)%10));
       count++;
    }
    div/=10;
  }
  return count;
}


#ifndef _INLINE

/******************************************************************************
*
* FUNCTION vcm_init
*
* DESCRIPTION
*   initialize VCM module
*
* PARAMETERS
*   none
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
void vcm_init(void)
{
  vcm_module_descriptor_type *vcm_ptr;
  dsvcm_entry_t *vcm_add_mem_ptr;


#if !defined(DS_PLATFORM_SMART)
  CFG_PTR->__cfg_vcm_version_high = (DSVCM_VER_MAR << 24) +
                                    (DSVCM_VER_MIR << 16) +
                                    (DSVCM_VER_MAI <<  8) +
                                     DSVCM_VER_SPB ;

  CFG_PTR->__cfg_vcm_version_low  = (DSVCM_VER_SPN << 16) +
                                     DSVCM_VER_PLV ;

  CFG_PTR->__cfg_vcm_num_modules      = 0 ;
  CFG_PTR->__cfg_vcm_max_name_length  = VCM_MAX_NAME_LENGTH ;
  CFG_PTR->__cfg_vcm_first_entry      = 0 ;
#endif

  vcm_ptr = vcm_module_register(VCM_MID_DSVCM, 0,
                                VCM_TXT_DSVCM,
                                DSVCM_VER_MAR, DSVCM_VER_MIR, DSVCM_VER_MAI,
                                DSVCM_VER_SPB, DSVCM_VER_SPN, DSVCM_VER_PLV,
                                0,  VCM_CTRL_HIDDEN );

  vcm_add_mem_ptr = (dsvcm_entry_t *)vcm_cfg_malloc(sizeof(dsvcm_entry_t));

  vcm_add_mem_ptr->magic_number=0;

  vcm_memory_ptr_set(vcm_ptr,
                     vcm_add_mem_ptr,
                     sizeof(dsvcm_entry_t));

  vcm_module_status_set(vcm_ptr,VCM_STATUS_INITIALIZED);

#if !defined(DS_PLATFORM_SMART)
  CFG_PTR->__cfg_vcm_valid_word = VCM_VALID_WORD ;
#endif
}


/******************************************************************************
*
* FUNCTION vcm_memory_ptr_set
*
* DESCRIPTION
*  Set pointer and size of additional config memory block
*
* PARAMETERS
*  vcm_module_descriptor_type* ptr  - pointer to module descriptor
*  vcm_cfg_mem_ptr_type cfg_mem_ptr - pointer to additional config memory block
*  UInt32 size                      - size of additional config memory block
*
* RETURNS
*   VCM_NO_ERROR        -  mem pointer succesfully set
*   VCM_INVALID_MODULE  -  descriptor pointer was 0
* REMARKS
*
******************************************************************************/

Int32 vcm_memory_ptr_set(vcm_module_descriptor_type* ptr,
                         vcm_cfg_mem_ptr_type cfg_mem_ptr,
                         UInt32 size)
{
  if (ptr)
  {
    ptr->vcm_cfg_mem_size=size;
    ptr->vcm_cfg_mem_offs=(char*)cfg_mem_ptr-(char*)CFG_SECTION_START;
    vcm_entry_changed();
    return VCM_NO_ERROR;
  }
  return VCM_INVALID_MODULE;
}

/******************************************************************************
*
* FUNCTION vcm_module_status_get
*
* DESCRIPTION
*   Get module status
*
* PARAMETERS
*   vcm_module_descriptor_type* ptr  - pointer to module descriptor
*
* RETURNS
*   modul status, or 0 if ptr was 0
*
* REMARKS
*
******************************************************************************/

Int32 vcm_module_status_get(vcm_module_descriptor_type* ptr)
{
  return (ptr) ?  ptr->vcm_status : 0 ;
}

/******************************************************************************
*
* FUNCTION vcm_module_status_set
*
* DESCRIPTION
*   Set module status
*
* PARAMETERS
*   vcm_module_descriptor_type* ptr  - pointer to module descriptor
*   Int32 status                     - status to be set
*
* RETURNS
*   VCM_NO_ERROR        -  status set
*   VCM_INVALID_MODULE  -  descriptor pointer was 0
*
* REMARKS
*
******************************************************************************/

Int32 vcm_module_status_set(vcm_module_descriptor_type* ptr, Int32 status)
{
  if (ptr)
  {
     ptr->vcm_status = status ;
     vcm_entry_changed();
     return VCM_NO_ERROR;
  }
  return VCM_INVALID_MODULE;
}

/******************************************************************************
*
* FUNCTION vcm_version_get
*
* DESCRIPTION
*   Get the module version
*
* PARAMETERS
*   vcm_module_descriptor_type* ptr  - pointer to module descriptor
*
* RETURNS
*   version of the module, or 0 version if ptr was 0
*
* REMARKS
*
******************************************************************************/

vcm_version_type vcm_version_get(vcm_module_descriptor_type* ptr)
{
  vcm_version_type null_version;

  if (ptr)
  {
     return ptr->vcm_version;
  }
  else
  {
     null_version.version.high=0;
     null_version.version.low=0;
     return null_version;
  }
}

/******************************************************************************
*
* FUNCTION vcm_version_print
*
* DESCRIPTION
*   prints a version string of a given version into a char buffer
*
* PARAMETERS
*   UInt8  mar  -  major release
*   UInt8  mir  -  minor release
*   UInt8  mai  -  maintenance number
*   UInt8  spb  -  special build
*   UInt16 spn  -  special build number
*   UInt16 plv  -  patch level
*
* RETURNS
*  number of chars used for version string
*  ( 0 : no version printed )
*
* REMARKS
*
******************************************************************************/


Int32 vcm_version_print(char *buffer,
                        UInt8 mar,
                        UInt8 mir,
                        UInt8 mai,
                        UInt8 spb,
                        UInt16 spn,
                        UInt16 plv)
{
  char *oldbuffer;
  oldbuffer=buffer;
  buffer+=uint2ch(buffer,mar);
  *(buffer++)='.';
  buffer+=uint2ch(buffer,mir);

  if (mai)
  {
    *(buffer++)='.';
    buffer+=uint2ch(buffer,mai);
  }

  if (spb==VCM_VERSION_ALPHA)
  {
    *(buffer++)='a';
    buffer+=uint2ch(buffer,spn);
  }
  if (spb==VCM_VERSION_BETA)
  {
    *(buffer++)='b';
    buffer+=uint2ch(buffer,spn);
  }

  if (plv)
  {
    *(buffer++)='p';
    buffer+=uint2ch(buffer,plv);
  }

  *buffer=0;

  return (Int32)(buffer-oldbuffer);
}


/******************************************************************************
*
* FUNCTION vcm_module_version_print
*
* DESCRIPTION
*   prints the version of a module into a char buffer
*
* PARAMETERS
*   vcm_module_descriptor_type* ptr - pointer to module descriptor
*
* RETURNS
*  number of chars used for version string
*  ( 0 : no version printed )
*
* REMARKS
*
******************************************************************************/

Int32 vcm_module_version_print(char *buffer,
                               vcm_module_descriptor_type* ptr)
{
  volatile vcm_version_type temp_vs;

  if (!ptr) return 0;

  /* only 32 bit access to (uncached) global memory */
  temp_vs.version.low = ptr->vcm_version.version.low;
  temp_vs.version.high = ptr->vcm_version.version.high;

  return vcm_version_print(buffer,
                           temp_vs.vs.mar,
                           temp_vs.vs.mir,
                           temp_vs.vs.mai,
                           temp_vs.vs.spb,
                           temp_vs.vs.spn,
                           temp_vs.vs.plv );

}


/******************************************************************************
*
* FUNCTION vcm_version_compare
*
* DESCRIPTION
*   compares the version of a module with a given version
*
* PARAMETERS
*   vcm_module_descriptor_type* ptr - pointer to module descriptor
*
*   Int32 operation  - one of these operations :
*                       VCM_VERSION_LT  -  Less Than
*                       VCM_VERSION_LE  -  Less or Equal
*                       VCM_VERSION_EQ  -  EQual
*                       VCM_VERSION_GE  -  Greater or Equal
*                       VCM_VERSION_GT  -  Greater Than
*
*   UInt8  mar  -  major release
*   UInt8  mir  -  minor release
*   UInt8  mai  -  maintenance number
*   UInt8  spb  -  special build
*   UInt16 spn  -  special build number
*   UInt16 plv  -  patch level
*
* RETURNS
*    0  : result is false
*  <>0  : result is true
*
* REMARKS
*
******************************************************************************/

Int32 vcm_version_compare(vcm_module_descriptor_type* ptr,
                          Int32 operation,
                          UInt8 mar,
                          UInt8 mir,
                          UInt8 mai,
                          UInt8 spb,
                          UInt16 spn,
                          UInt16 plv)
{
  vcm_version_type cmp_vs;

  if (!ptr) return 0;

  cmp_vs.vs.mar=mar;
  cmp_vs.vs.mir=mir;
  cmp_vs.vs.mai=mai;
  cmp_vs.vs.spb=spb;
  cmp_vs.vs.spn=spn;
  cmp_vs.vs.plv=plv;

  if (ptr->vcm_version.version.high>cmp_vs.version.high)
    return operation & VCM_VERSION_GT;
  if (ptr->vcm_version.version.high<cmp_vs.version.high)
    return operation & VCM_VERSION_LT;

  if (ptr->vcm_version.version.low>cmp_vs.version.low)
    return operation & VCM_VERSION_GT;
  if (ptr->vcm_version.version.low<cmp_vs.version.low)
    return operation & VCM_VERSION_LT;

  return operation & VCM_VERSION_EQ;

}

/******************************************************************************
*
* FUNCTION vcm_module_register
*
* DESCRIPTION
*   This function registers a module in the VCM module and returns a
*   pointer to the module descriptor. The module status of newly registered
*   modules is set to VCM_STATUS_UNINITIALIZED.
*
* PARAMETERS
*   UInt32 mod_id     -   module number
*   vcm_module_descriptor_type* main_ptr - pointer to an already registered module,
*                                          if current module is a sub module to this, or 0
*   char* module_name -  module description string
*   UInt8 mar         -  major release
*   UInt8 mir         -  minor release
*   UInt8 mai         -  maintenance number
*   UInt8 spb         -  special build type
*   UInt16 spn        -  special build number
*   UInt16 plv        -  patch level
*   UInt32 xtra_info  -  extra module specific data
*   UInt32 control    -  special Control bits, e.g., VCM_CTRL_HIDDEN,
*                        if module should not be displayed on host PC.
*
* RETURNS
*   pointer to the module descriptor, or 0 if registration failed
*
* REMARKS
*
******************************************************************************/

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
                                                UInt32  control)
{
  vcm_module_descriptor_type *ptr, *lastptr=NULL;
  UInt32 count;

  /* allocate memory for vcm descriptor */
  ptr=(vcm_module_descriptor_type *)vcm_cfg_malloc(sizeof(vcm_module_descriptor_type));
  if(!ptr) return NULL;   /* not enough memory */

  ptr->vcm_mod_id=mod_id;

#if (defined _DS1103) || (defined _DS1104) || (defined _DS1005) || (defined _DS1401)
  {
    /* only 32 bit access to (uncached) global memory */
    volatile vcm_version_type temp_vs;
    temp_vs.vs.mar=mar;
    temp_vs.vs.mir=mir;
    temp_vs.vs.mai=mai;
    temp_vs.vs.spb=spb;
    temp_vs.vs.spn=spn;
    temp_vs.vs.plv=plv;
    ptr->vcm_version.version.low=temp_vs.version.low;
    ptr->vcm_version.version.high=temp_vs.version.high;
  }
#elif (defined _DS1006) || (defined DS_PLATFORM_SMART)
  ptr->vcm_version.vs.mar=mar;
  ptr->vcm_version.vs.mir=mir;
  ptr->vcm_version.vs.mai=mai;
  ptr->vcm_version.vs.spb=spb;
  ptr->vcm_version.vs.spn=spn;
  ptr->vcm_version.vs.plv=plv;
#endif

  ptr->vcm_status=VCM_STATUS_UNINITIALIZED;

  ptr->vcm_sub_offs=0;

  ptr->vcm_cfg_mem_offs=0;
  ptr->vcm_cfg_mem_size=0;

  ptr->vcm_timestamp.mat=0;
  ptr->vcm_timestamp.mit=0;

  ptr->vcm_next_offs=0;
  ptr->vcm_lin_offs=0;

  ptr->vcm_xtra_info=xtra_info;

  ptr->vcm_control=control;
  ptr->vcm_reserved_offs=0;


  /* copy module name */


#if defined(__BOOT_FW__) && ( defined(_DS1401) || defined(_DS1005) || defined(_DS1104) )
{
/*
  Only 32 Bit aligned accesses to module_name (uncached, Flash) possible
 */
  char temp[VCM_MAX_NAME_LENGTH+4] ;

  for (count=0; count <(VCM_MAX_NAME_LENGTH/4*VCM_AF)+1; count++)
    ((UInt32*)temp)[count]=((UInt32*)((UInt32)module_name&~3))[count];

  for (count=0; count <(VCM_MAX_NAME_LENGTH/4*VCM_AF)-1; count++)
    ((UInt32*)(ptr->vcm_module_name))[count]=((UInt32*)((UInt32)temp+((UInt32)module_name&3)))[count];

	/*
	 * null termination
	 */
  ((UInt32*)(ptr->vcm_module_name))[count]=((UInt32*)((UInt32)temp+(UInt32)module_name&3))[count]&0xFFFFFF00;
}
#else

  for (count=0; count <(VCM_MAX_NAME_LENGTH/4*VCM_AF)-1; count++)
      ((UInt32*)(ptr->vcm_module_name))[count]=((UInt32*)module_name)[count];
	/*
	 * null termination
	 */
#if (defined(_DS1103) || defined(_DS1401) || defined(_DS1005) || defined(_DS1104) || defined(_DS6001))
  ((UInt32*)(ptr->vcm_module_name))[count]=(((UInt32*)module_name)[count])&0xFFFFFF00;
#elif defined(_DS1006)
  ((UInt32*)(ptr->vcm_module_name))[count]=(((UInt32*)module_name)[count])&0x00FFFFFF;
#elif defined(_DS2510)
  ((UInt32*)(ptr->vcm_module_name))[count]=(((UInt32*)module_name)[count])&0x00FFFFFF;
#elif defined(_DS1004)
  ((UInt32*)(ptr->vcm_module_name))[count]=(((UInt32*)module_name)[count])&0x00FFFFFF;
#elif defined(_DS1003)
  ((UInt32*)(ptr->vcm_module_name))[count]=0x0;
#endif

#endif


  if (main_ptr)
  {
     if (main_ptr->vcm_sub_offs)
     {
        lastptr=VCM_CFG_OFFS_PTR(main_ptr->vcm_sub_offs);
        while(lastptr->vcm_next_offs)
        {
           lastptr=VCM_CFG_OFFS_PTR(lastptr->vcm_next_offs);
        }
        lastptr->vcm_next_offs=(char*)ptr-(char*)CFG_SECTION_START;
     }
     else
     {
        main_ptr->vcm_sub_offs=(char*)ptr-(char*)CFG_SECTION_START;
     }
  }
  else if (CFG_VCM_FIRST_MODULE_OFSS)
  {
    lastptr=VCM_CFG_OFFS_PTR(CFG_VCM_FIRST_MODULE_OFSS);
    while(lastptr->vcm_next_offs)
    {
       lastptr=VCM_CFG_OFFS_PTR(lastptr->vcm_next_offs);
    }
    lastptr->vcm_next_offs=(char*)ptr-(char*)CFG_SECTION_START;
  }

  /* insert pointer to linear list */
  if (CFG_VCM_FIRST_MODULE_OFSS)
  {
    lastptr=VCM_CFG_OFFS_PTR(CFG_VCM_FIRST_MODULE_OFSS);
    while(lastptr->vcm_lin_offs)
    {
       lastptr=VCM_CFG_OFFS_PTR(lastptr->vcm_lin_offs);
    }
    lastptr->vcm_lin_offs=(char*)ptr-(char*)CFG_SECTION_START;
  }
  else  /* first module */
  {
    CFG_VCM_SET_FIRST_MODULE_OFFS((char*)ptr-(char*)CFG_SECTION_START);
  }
#if !defined(DS_PLATFORM_SMART)
  (CFG_PTR->__cfg_vcm_num_modules)++;
#endif

  vcm_entry_changed();

  return ptr;

}

/******************************************************************************
*
* FUNCTION vcm_cfg_malloc
*
* DESCRIPTION
*   Allocates a block of config section memory of the specified size.
*
* PARAMETERS
*    vcm_size_tsize - size of the cfg memory block
*
* RETURNS
*   The pointer to the allocated config section memory block or 0,
*   if the block couldn't be allocated.
*
* REMARKS
*
******************************************************************************/

#if defined (_DSHOST)
#include <malloc.h>
#include <stdio.h>
#endif

void *vcm_cfg_malloc(vcm_size_t size)
{

#if defined (_DS1004)
  return dpm_alloc(size);

#elif defined (_DS1003)
  return malloc(size);

#elif ((defined (_DS1005)) || (defined (_DS1104)))

#ifdef __BOOT_FW__
  return (void*)((*((UInt32*)&__cfg_memptr_malloc_end)=((*((UInt32*)&__cfg_memptr_malloc_end)-(size-1))&~0x03)-1)+1);
#else
  return malloc_global(size);
#endif

#elif defined (_DS1006)
#ifdef __BOOT_FW__
  return (void*)((*((UInt32*)&CFG_SECTION_UINT32(__cfg_memptr_malloc_end))=((*((UInt32*)&CFG_SECTION_UINT32(__cfg_memptr_malloc_end))-(size-1))&~0x03)-1)+1);
#else
  return malloc_global(size);
#endif


#elif defined (DS_PLATFORM_SMART)
  /*
   * We have no seperate config section memory,
   * so we have to allocate memory from the heap.
   * See comment above CFG_SECTION_START in dsvcm.h.
   */
  return malloc(size);

#elif defined (_DS1103)
  return malloc_global(size);

#elif defined (_DS1401)
#ifdef __BOOT_FW__
{
  void *MallocSecStartPtr = (void*) *((volatile UInt32*)&__cfg_MallocSecStart) ;

  *((volatile UInt32*)&__cfg_MallocSecStart) =
                  ((*((volatile UInt32*)&__cfg_MallocSecStart)+(size+3))&~0x03);
  return (MallocSecStartPtr);

}
#else
  return malloc_global(size);
#endif

#elif defined (_DSHOST)
  return malloc(size);

#endif

}


#if !defined(DS_PLATFORM_SMART)
/******************************************************************************
*
* FUNCTION vcm_version_update
*
* DESCRIPTION
*   Update Version Information of VCM Module
*
* PARAMETERS
*   vcm_module_descriptor_type* ptr  - pointer to module descriptor
*   UInt8  mar                       -  major release
*   UInt8  mir                       -  minor release
*   UInt8  mai                       -  maintenance number
*   UInt8  spb                       -  special build
*   UInt16 spn                       -  special build number
*   UInt16 plv                       -  patch level
*
* RETURNS
*   modul status, or 0 if ptr was 0
*
* REMARKS
*
******************************************************************************/
Int32 vcm_version_update(vcm_module_descriptor_type* ptr,
                         UInt8  mar,
                         UInt8  mir,
                         UInt8  mai,
                         UInt8  spb,
                         UInt16 spn,
                         UInt16 plv)
{
    if(ptr)
    {
        /* Write given Version Information into tem. Structure */
        volatile vcm_version_type temp_vs;
        temp_vs.vs.mar = mar;
        temp_vs.vs.mir = mir;
        temp_vs.vs.mai = mai;
        temp_vs.vs.spb = spb;
        temp_vs.vs.spn = spn;
        temp_vs.vs.plv = plv;

        /* only 32 bit access to (uncached) global memory */
        ptr->vcm_version.version.high = temp_vs.version.high;
        ptr->vcm_version.version.low  = temp_vs.version.low ;

        vcm_entry_changed();
        return VCM_NO_ERROR;
    }
    /* ptr to Module invalid */
    return VCM_INVALID_MODULE;
}


/******************************************************************************
*
* FUNCTION vcm_memory_update
*
* DESCRIPTION
*   Update Content of Additional Config Section Memory Block
*   ATTENTION: Function uses Memory allocated previously. In case you use more memory
*              than allocated function will break.
*
* PARAMETERS
*   vcm_module_descriptor_type* ptr     - pointer to module descriptor
*   UInt32*                     buffer  - ptr to [] containing content to
*                                         be written into memory
*   UInt32                      size    - length of Buffer in Byte, divisable by sizeof(UInt32)
*
* RETURNS
*   modul status
*
* REMARKS
*
******************************************************************************/
Int32 vcm_memory_update(vcm_module_descriptor_type* ptr,
                        UInt32*                     buffer,
                        UInt32                      size /* in bytes! */ )
{
    /* break with errorcode, if module given is not valid */
    if (ptr)
    {
        int idx;
        /* get ptr of additional config mem */
        vcm_cfg_mem_ptr_type* cfg_mem_ptr = vcm_memory_ptr_get(ptr);
        /* Check, if enough memory was allocated */
        if (size > (ptr->vcm_cfg_mem_size))
        {
            /* Due to freeing of allocated memory is not supported by VCM,
             * (view 'Basic Principles of VCM' at Helpdesk)
             * a new allocation of Memory is at least critical -> will cause Memory
             * leak. */
            return VCM_ILLEGAL_SIZE_OF_BUFFER;
        }

        /* write buffer to add. conf mem */
        for(idx = 0; (idx < (size / sizeof(UInt32))) ; idx++)
        {
            cfg_mem_ptr[idx] = (UInt32*) buffer[idx];
        }

        vcm_entry_changed();
        return VCM_NO_ERROR;
    }
    return VCM_INVALID_MODULE;
}
#endif



#endif                                                     /* #ifndef _INLINE */

#endif                                                 /* #if !defined _RP565 */

#undef __INLINE
