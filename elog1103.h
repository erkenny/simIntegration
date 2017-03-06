/*******************************************************************************
*
*  MODULE
*    DSELOG - Event Logger
*
*  FILE
*    elog1103.h
*
*  RELATED FILES
*    dselog.c
*    dselog.h
*
*  DESCRIPTION
*    dSPACE event logging, Platform specific defines and structures
*
*  REMARKS
*
*  AUTHOR(S)
*    T. Woelfer
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: elog1103.h $ $Revision: 1.2 $ $Date: 2003/08/07 12:12:08GMT+01:00 $
*******************************************************************************/

#ifndef __elog1103_h__
#define __elog1103_h__

#include <dsmem.h>

/*******************************************************************************
  constant, macro and type definitions
*******************************************************************************/

/* number of elog entries for the buffer, must be divisible by 2 */
#define ELOG_BUFFER_SIZE 128

/* number of characters for the event symbol, must be divisible by four */
#define ELOG_SYMBOL_LENGTH 4

/* number of characters for the display label, must be divisible by four */
#define ELOG_DISPLAY_LABEL_LENGTH 32


/*---- macros, which cover platform differences -----------------------------*/

#define ELOG_STORE_ADDR(buffer_no) ((UInt32) elog_info_struct_ptr->buffers_ptr[(buffer_no)])
#define ELOG_STORE_NUM (sizeof(elog_buffer_type) / sizeof(Float64))

#define ELOG_ASSIGN_HOST_BUFFER(buffer_ptr) (elog_buffer_type *) malloc_global(sizeof(elog_buffer_type))

#define ELOG_MALLOC_GLOBAL malloc_global
#define ELOG_FREE_GLOBAL   free_global
#define ELOG_MEMCPY_GLOBAL ds_memcpyx

#define ELOG_BUFFER_STORE()                                                 \
{                                                                           \
  int n;                                                                    \
                                                                            \
  for (n = 0; n < elog_store_num; n++)                                      \
  {                                                                         \
    ((Float64 *) elog_store_addr)[n] = ((Float64 *) elog_buffer_ptr)[n];    \
  }                                                                         \
}


/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/


#endif /* __elog1103_h__ */

