/******************************************************************************
*
* MODULE
*   Definitions for special memory-management and copy-routines.
*
*
* FILE
*   Dsmem.h
*
* RELATED FILES
*   dsmem.c
*
* DESCRIPTION
*   Definition of
*   - types for memorymanagement of global RAM
*
* AUTHOR(S)
*   R. Kraft
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Dsmem.h $ $Revision: 1.2 $ $Date: 2003/08/07 12:18:06GMT+01:00 $
******************************************************************************/



#ifndef _DSMEM_H_
#define _DSMEM_H_


#include <stddef.h>
#include <stdlib.h>

#include <Dstypes.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

typedef UInt64 align;

union global_header
{
  struct
  {
    union global_header *ptr;
    UInt32 size;

  }s;
  align x;
};

typedef union global_header Global_Header;

#if (defined (_DS1005) || defined (_DS1104))
  extern volatile UInt32* __cfg_memptr_malloc_start ;
  extern volatile UInt32* __cfg_memptr_malloc_ptr ;
  extern volatile UInt32* __cfg_memptr_malloc_end ;
#elif (defined (_DS1103) || defined (_DS1401))
  extern volatile UInt32* __cfg_MallocSecStart ;
  extern volatile UInt32* __cfg_MallocSecPtr ;
  extern volatile UInt32* __cfg_MallocSecEnd ;
#endif






/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.       */
/* All rights reserved                                                      */
/****************************************************************************/
/*
;The software source code contained in this file (functions ds_sbrk, ds_malloc,
;and ds_free) and in the related header files is the property of Microtec Research, Inc.
;and may only be used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/


/************************************************************************/
/*                                                                      */
/*  char *sbrk(int incr);                                               */
/*                                                                      */
/*  Allocate memory                                                     */
/*                                                                      */
/*  sbrk:                                                               */
/*      Allocates memory, in general for use by heap management         */
/*      routines.  If successful, sbrk returns the address of the       */
/*      allocated memory.  If memory is not available, sbrk returns -1. */
/*                                                                      */
/************************************************************************/
char *ds_sbrk(unsigned incr);

/************************************************************************/
/* NAME                         M A L L O C                             */
/*                                                                      */
/* FUNCTION     malloc - allocates a block of at least 'size' bytes     */
/*              beginning on a word boundary.                           */
/*                                                                      */
/* SYNOPSIS     char *malloc(size)                                      */
/*              unsigned int size ; size of block in bytes              */
/*                                                                      */
/* RETURN       Pointer to a block of memory. NULL if out of memory     */
/*              or free list is corrupted.                              */
/*                                                                      */
/* DESCRIPTION                                                          */
/* malloc() searches the circular free block list, starting from        */
/* _avail, and allocates the first block big enough for size bytes.    */
/* It calls () to get more memory from the system when there is         */
/* no suitable free space.  It only asks for memory in multiple blocks  */
/* of HEADER's.  The allocated block of memory also has a header so     */
/* that free() can determine how much to free.                          */
/************************************************************************/
void *ds_malloc (size_t size);
/************************************************************************/
/* NAME                         F R E E                                 */
/*                                                                      */
/* FUNCTION     free - Frees a block of memory previously               */
/*              allocated by one of the allocation functions.  Its      */
/*              contents remain unchanged                               */
/*                                                                      */
/* SYNOPSIS     void free(ptr)                                          */
/*              void *ptr ;     pointer to previously allocated block   */
/*                                                                      */
/* RETURN       no return                                               */
/*                                                                      */
/* DESCRIPTION                                                          */
/*      free() adds the given block of memory to the free block list    */
/* which is sorted by memory address, i.e. p->next > p except the end   */
/* of the list when p->next equals the head of the list.  free() also   */
/* coalesces (merges) adjacent free blocks to form larger free blocks.  */
/*      NOTE that free() does not disturb the content of the given      */
/* block except the header info.  This is necessary because some UNIX   */
/* programs require that the freed space remain unchanged.              */
/************************************************************************/
void ds_free (void *ptr);


/******************************************************************************
*
* FUNCTION:
*   void dsmem_init (void)
*
* DESCRIPTION:
*   init the Malloc Section variable for the malloc_global function
*
* PARAMETERS:
*
* RETURNS:
*   0  : no errors
*
* NOTE:
*
* HISTORY:
*   23.07.98  R. Busche    , initial version
*
******************************************************************************/

void dsmem_init (void) ;

/******************************************************************************
*
* FUNCTION:
*   void dsmem_vcm_register (void)
*
* DESCRIPTION:
*   Registers the memory management module in the VCM
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
*   18.07.00  R. Leinfellner, initial version
*
******************************************************************************/

void dsmem_vcm_register(void);


/* function prototypes go here */


extern void free_global(void *ap);

extern Global_Header* more_globalmem(UInt32 size);

extern void* malloc_global(UInt32 nbytes);

extern void* ds_memcpyx(void* s1, const void* s2, size_t n);


#undef __INLINE


#endif /*_DSMEM_H_*/
