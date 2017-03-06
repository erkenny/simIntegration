/******************************************************************************
  FILE:
    MSG1103.H

  DESCRIPTION:
    Message Module: Board Specific Header

  REMARKS:

  AUTHOR(S):  R. Leinfellner

  Copyright (c) 1997 dSPACE GmbH, GERMANY

  $RCSfile: Msg1103.H $ $Revision: 1.3 $ $Date: 2007/07/31 18:33:50GMT+01:00 $
  $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSMSG1103.pj $
******************************************************************************/


#ifndef __MSG1103_H__
#define __MSG1103_H__


/* ---- Message buffer definitions ----------------------------------------- */

#define MSG_STRING_LENGTH 80              /* Message string length           */
#define MSG_BUFFER_LENGTH 16              /* Buffer length                   */

#ifndef __ASSEMBLER__

#define MSG_MC_MAIN_MODULE DS_1103_ERR_MOD /* Main module identifier DS1103  */

/* ---- Typedefs ----------------------------------------------------------- */

typedef char msg_string_type;           /* Data type of a string character */

/* ---- Message header file include ---------------------------------------- */
#ifndef _DSHOST

#include <int1103.h>
#include <Dsmem.h>

/* ---- Base Address of the Config Section --------------------------------- */

#define CFG_MSG_DESC_PTR ((msg_descriptor_type *)(DS1103_GLOBAL_RAM_START + ((cfg_section_type *)DS1103_GLOBAL_RAM_START)->__cfg_msg_offs))

#define CFG_SECTION_PTR (&((( cfg_section_type *)DS1103_GLOBAL_RAM_START)->__cfg_msg_descriptor))


/* ---- Board specific macros ---------------------------------------------- */

/* Save interrupt and disable interrupts */


#define MSG_INT_DISABLE() { msg_int_state = ds1103_get_interrupt_status(); DS1103_GLOBAL_INTERRUPT_DISABLE(); }

#define MSG_INT_RESTORE()  ds1103_set_interrupt_status(msg_int_state)


/* Copy s2 to s1 */
#define MSG_STRCPY(s1, s2, len) {                                             \
                             if (s2[0])                                       \
                             {                                                \
                               ds_memcpyx(s1, s2, len);                       \
                               ds_memcpyx(s1 + len, "\0", 1);                 \
                             } else                                           \
                               ds_memcpyx(s1, "\0", 1);                       \
                           }

#define MSG_SET_OLD_ERRORCODE(code, string)                                   \
                           {                                                  \
                           }

#define MSG_INIT_BUFFER()  {                                                                                                                \
                             void * tmp_p ;                                                                                                 \
                             tmp_p = malloc_global (sizeof (msg_descriptor_type));                                                          \
                             if(tmp_p)                                                                                                      \
                             {                                                                                                              \
                               ((cfg_section_type *) DS1103_GLOBAL_RAM_START)->__cfg_msg_offs = ((UInt32)tmp_p) - DS1103_GLOBAL_RAM_START ; \
                             }                                                                                                              \
                             else                                                                                                           \
                             {                                                                                                              \
                               ((cfg_section_type *) DS1103_GLOBAL_RAM_START)->__cfg_msg_offs = 0;                                          \
                                 exit(1);                                                                                                   \
                             };                                                                                                             \
                           }

#define MSG_VCM_PTR_SET() vcm_memory_ptr_set(msg_vcm_module_ptr,                           \
                            (vcm_cfg_mem_ptr_type)(DS1103_GLOBAL_RAM_START +                \
                            ((cfg_section_type *) DS1103_GLOBAL_RAM_START)->__cfg_msg_offs), \
                            sizeof(msg_descriptor_type));

/* ---- Message Module Definitions ----------------------------------------- */

#include <Dsmsg.h>


/* ------------------------------------------------------------------------- */

#endif  /* __ASSEMBLER__ */

#endif /* __MSG1103_H__ */
#endif /* _DSHOST */
