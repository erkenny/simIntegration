/*******************************************************************************
*
*  MODULE
*    DSELOG - Event Logger
*
*  FILE
*    dselog.c
*
*  RELATED FILES
*    dselog.h
*
*  DESCRIPTION
*    dSPACE event logging function API
*
*  REMARKS
*
*  AUTHOR(S)
*    T. Woelfer
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: dselog.c $ $Revision: 1.6 $ $Date: 2006/08/09 15:30:30GMT+01:00 $
*******************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <dsvcm.h>
#include <dsstd.h>
#include <dselog.h>

#include <dsmsg.h>

/*******************************************************************************
  constant, macro and type definitions
*******************************************************************************/

/* $DSVERSION: ELOG - Event Logger Module */

#define ELOG_VER_MAR 2
#define ELOG_VER_MIR 1
#define ELOG_VER_MAI 1
#define ELOG_VER_SPB VCM_VERSION_RELEASE
#define ELOG_VER_SPN 0
#define ELOG_VER_PLV 0

#ifndef _INLINE
#if defined _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 ||defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401

DS_VERSION_SYMBOL(_elog_ver_mar, ELOG_VER_MAR);
DS_VERSION_SYMBOL(_elog_ver_mir, ELOG_VER_MIR);
DS_VERSION_SYMBOL(_elog_ver_mai, ELOG_VER_MAI);
DS_VERSION_SYMBOL(_elog_ver_spb, ELOG_VER_SPB);
DS_VERSION_SYMBOL(_elog_ver_spn, ELOG_VER_SPN);
DS_VERSION_SYMBOL(_elog_ver_plv, ELOG_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/*******************************************************************************
  object declarations
*******************************************************************************/

#ifndef _INLINE

elog_info_struct_type *elog_info_struct_ptr;
elog_buffer_type *elog_buffer_ptr;      /* pointer to the current elog buffer */
elog_buffer_type *elog_buffers_ptr[2];
unsigned int elog_buffer_index;
int elog_buffer_mode;
UInt32 elog_current_category;
UInt32 elog_event_count;
UInt32 elog_buffer_size;
UInt32 elog_byte_count;
UInt32 elog_store_addr;
UInt32 elog_store_num;

#else

extern elog_info_struct_type *elog_info_struct_ptr;
extern elog_buffer_type *elog_buffer_ptr;
extern elog_buffer_type *elog_buffers_ptr[2];
extern unsigned int elog_buffer_index;
extern int elog_buffer_mode;
extern UInt32 elog_current_category;
extern UInt32 elog_event_count;
extern UInt32 elog_buffer_size;
extern UInt32 elog_byte_count;
extern UInt32 elog_store_addr;
extern UInt32 elog_store_num;

#endif

/*******************************************************************************
  function prototypes
*******************************************************************************/


/*******************************************************************************
  function declarations
*******************************************************************************/

#if defined _DS1005 || defined _DS1103 || defined _DS1401 || defined _DS1104
#  ifdef _INLINE
#    define __INLINE static
#  else
#    define __INLINE
#  endif
#elif defined _DS1006 
#  ifdef _INLINE
#    define __INLINE static inline
#  else
#    define __INLINE
#  endif
#else
#  error dselog.c: no processor board defined (e.g. _DS1003)
#endif


#ifndef _INLINE


/*******************************************************************************
*
*  FUNCTION
*    Initialization function
*
*  SYNTAX
*    elog_init()
*
*  DESCRIPTION
*    initialization of the event logging module
*
*  PARAMETERS
*
*  RETURNS
*    error code
*
*  REMARKS
*
*******************************************************************************/

int elog_init (void)
{
  vcm_module_descriptor_type  *elog_module_ptr;
  int entry;
  int buffer_no;

  if (VCM_STATUS_INITIALIZED == vcm_module_status_get(vcm_module_find(VCM_MID_ELOG, NULL)))
    return ELOG_ALREADY_INITIALIZED;

  elog_module_ptr = vcm_module_register(VCM_MID_ELOG,
                                        vcm_module_find(VCM_MID_RTLIB, NULL),
                                        VCM_TXT_ELOG,
                                        ELOG_VER_MAR, ELOG_VER_MIR, ELOG_VER_MAI,
                                        ELOG_VER_SPB, ELOG_VER_SPN, ELOG_VER_PLV,
                                        0, 0);

  /* set up the elog info structure for the host */
  elog_info_struct_ptr = vcm_cfg_malloc(sizeof(elog_info_struct_type));
  if (!elog_info_struct_ptr)
  {
    msg_error_set(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "elog_init(): " MSG_MEM_ALLOC_ERROR_TXT);
    exit(1);
  }

  /* enter the info struct pointer at the VCM */
  vcm_memory_ptr_set(elog_module_ptr,
                     (vcm_cfg_mem_ptr_type) elog_info_struct_ptr,
                     sizeof(elog_info_struct_type));


  /* create the elog buffers */
  for (buffer_no = 0; buffer_no < 2; buffer_no++)
  {
    elog_buffers_ptr[buffer_no] = (elog_buffer_type *) malloc(sizeof(elog_buffer_type));

    if (!elog_buffers_ptr[buffer_no])
    {
      msg_error_set(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "elog_init(): " MSG_MEM_ALLOC_ERROR_TXT);
      exit(1);
    }

    /* initialize buffer data */
    for (entry = 0; entry < ELOG_BUFFER_SIZE; entry++)
    {
      elog_buffers_ptr[buffer_no]->entry[entry].count1 = 0x0;
      elog_buffers_ptr[buffer_no]->entry[entry].count2 = 0xFFFFFFFF;
      elog_buffers_ptr[buffer_no]->entry[entry].data.high = 0;
      elog_buffers_ptr[buffer_no]->entry[entry].data.low = 0;
      elog_buffers_ptr[buffer_no]->entry[entry].spec = 0;
      elog_buffers_ptr[buffer_no]->entry[entry].ts.mat = 0xFFFFFFFF;
      elog_buffers_ptr[buffer_no]->entry[entry].ts.mit = 0xFFFFFFFF;
    }
  }

  /* assign the current buffer pointer */
  elog_buffer_ptr = elog_buffers_ptr[0];

  elog_buffer_index = 0;
  elog_buffer_size = ELOG_BUFFER_SIZE;
  elog_buffer_mode = ELOG_MODE_OVERWRITE;

  /* event logging is switched off initially */
  elog_current_category = ELOG_CAT_NONE;
  elog_event_count = 0;

  /* fill out the elog info structure */
  elog_info_struct_ptr->buffers_ptr[0] = ELOG_ASSIGN_HOST_BUFFER(elog_buffers_ptr[0]);
  if (!elog_info_struct_ptr->buffers_ptr[0])
  {
    msg_error_set(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "elog_init(): " MSG_MEM_ALLOC_ERROR_TXT);
    exit(1);
  }
  elog_info_struct_ptr->buffers_ptr[1] = ELOG_ASSIGN_HOST_BUFFER(elog_buffers_ptr[1]);
  if (!elog_info_struct_ptr->buffers_ptr[1])
  {
    msg_error_set(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "elog_init(): " MSG_MEM_ALLOC_ERROR_TXT);
    exit(1);
  }
  elog_info_struct_ptr->buffer_ptr = elog_info_struct_ptr->buffers_ptr[0];
  elog_info_struct_ptr->buffer_size = ELOG_BUFFER_SIZE;
  elog_info_struct_ptr->display_table_ptr = NULL;
  elog_info_struct_ptr->display_table_size = 0;
  elog_info_struct_ptr->descriptor_table_ptr = NULL;
  elog_info_struct_ptr->descriptor_table_size = 0;
  elog_info_struct_ptr->display_label_length = ELOG_DISPLAY_LABEL_LENGTH;
  elog_info_struct_ptr->symbol_length = ELOG_SYMBOL_LENGTH;
  elog_info_struct_ptr->current_buffer = 0;
  elog_info_struct_ptr->buffer_switch = 0;
  elog_info_struct_ptr->category = elog_current_category;
  elog_info_struct_ptr->category_set = 0;

  elog_store_num  = ELOG_STORE_NUM;

  /* initially store both buffers */
  elog_store_addr= ELOG_STORE_ADDR(1);
  elog_buffer_ptr = elog_buffers_ptr[1];
  ELOG_BUFFER_STORE();

  elog_store_addr= ELOG_STORE_ADDR(0);
  elog_buffer_ptr = elog_buffers_ptr[0];
  ELOG_BUFFER_STORE();

  vcm_module_status_set(elog_module_ptr, VCM_STATUS_INITIALIZED);

  return ELOG_NO_ERROR;
}


/*******************************************************************************
*
*  FUNCTION
*    elog_buffer_mode_set
*
*
*  SYNTAX
*    int elog_buffer_mode_set(int buffer_mode) 
*
*  DESCRIPTION
*    set the buffer mode to overwrite or blocking
*
*  PARAMETERS
*    buffer_mode: ELOG_MODE_OVERWRITE
*                 ELOG_MODE_BLOCKING
*
*  RETURNS
*    the previous buffer mode
*
*  REMARKS
*
*******************************************************************************/

int elog_buffer_mode_set(int buffer_mode)
{
  int buffer_mode_old;

  buffer_mode_old = elog_buffer_mode;
  elog_buffer_mode = buffer_mode;

  return buffer_mode_old;
}


/*******************************************************************************
*
*  FUNCTION
*    create a new entry for a display line
*
*  SYNTAX
*    UInt32 elog_define_display(char *label);
*
*  DESCRIPTION
*    This function creates a new entry in the display table. The returned
*    display ID can be used to associate an event to the display line.
*
*  PARAMETERS
*    *label   display label string
*
*  RETURNS
*    display ID handle
*    
*    0xFFFFFFFF if no display line could be created
*
*  REMARKS
*
*******************************************************************************/
UInt32 elog_define_display(char *label)
{
  rtlib_int_status_t int_status;
  elog_display_entry_type *tmp_display_table_ptr;
  size_t label_length;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* if there is no display table, create it */
  if (elog_info_struct_ptr->display_table_ptr == NULL)
  {
    elog_info_struct_ptr->display_table_ptr = ELOG_MALLOC_GLOBAL(sizeof(elog_display_entry_type));
    if (!elog_info_struct_ptr->display_table_ptr)
    {
      RTLIB_INT_RESTORE(int_status);
      return 0xFFFFFFFF;
    }
  }
  else
  {
    tmp_display_table_ptr = ELOG_MALLOC_GLOBAL(sizeof(elog_display_entry_type) * (elog_info_struct_ptr->display_table_size + 1));

    if (!tmp_display_table_ptr)
    {
      RTLIB_INT_RESTORE(int_status);
      return 0xFFFFFFFF;
    }

    ELOG_MEMCPY_GLOBAL(tmp_display_table_ptr, 
                        elog_info_struct_ptr->display_table_ptr,
                        sizeof(elog_display_entry_type) * (elog_info_struct_ptr->display_table_size + 1));

    ELOG_FREE_GLOBAL(elog_info_struct_ptr->display_table_ptr);
    elog_info_struct_ptr->display_table_ptr = tmp_display_table_ptr;
  }

  label_length = strlen(label) + 1;
  if (label_length > ELOG_DISPLAY_LABEL_LENGTH )
  {
    label_length = ELOG_DISPLAY_LABEL_LENGTH;
    strcpy(&label[ELOG_DISPLAY_LABEL_LENGTH - 1], "\0");
  }

  ELOG_MEMCPY_GLOBAL(&(elog_info_struct_ptr->display_table_ptr[elog_info_struct_ptr->display_table_size]),
                     label,
                     label_length);

  elog_info_struct_ptr->display_table_size++;

  RTLIB_INT_RESTORE(int_status);

  return (elog_info_struct_ptr->display_table_size - 1);
}

/*******************************************************************************
*
*  FUNCTION
*    set the label for a display line
*
*  SYNTAX
*    UInt32 elog_set_display_label(int display_id, char *label);
*
*  DESCRIPTION
*    This function sets the label for an existent display. It can be used
*    to change the label string initially set by elog_define_display().
*
*  PARAMETERS
*     display_id     Display ID obtained by elog_define_display(), this ID
*                    determines for which log line the label is set.
*
*    *label          display label string
*
*  RETURNS
*    ELOG_NO_ERROR      Event described successfully.
*
*    ELOG_NO_DISPLAY    The display ID specified wasn't defined before by
*                       elog_define_display().
*
*  REMARKS
*
*******************************************************************************/
int elog_set_display_label(UInt32 display_id, char *label)
{
  size_t label_length;

  /* check for the id in the display table */
  if (display_id >= elog_info_struct_ptr->display_table_size)
    return(ELOG_NO_DISPLAY);

  label_length = strlen(label) + 1;
  if (label_length > ELOG_DISPLAY_LABEL_LENGTH )
  {
    label_length = ELOG_DISPLAY_LABEL_LENGTH;
    strcpy(&label[ELOG_DISPLAY_LABEL_LENGTH - 1], "\0");
  }

  ELOG_MEMCPY_GLOBAL(&(elog_info_struct_ptr->display_table_ptr[display_id]),
                     label,
                     label_length);

  return ELOG_NO_ERROR;
}

/*******************************************************************************
*
*  FUNCTION
*    create a new entry for an event description
*
*  SYNTAX
*    int elog_describe_event(UInt32  id,
*                            UInt32  display_id,
*                            UInt32  control,
*                            UInt32  key_high,
*                            UInt32  key_low,
*                            char   *symbol_ptr)
*
*
*  DESCRIPTION
*    This function creates a new entry in the event descriptor table for the
*    specified event.
*
*  PARAMETERS
*     id             event ID or Specifier
*
*     display_id     Display ID obtained by elog_define_display(), this ID
*                    determines in which log line a front-end displays this event.
*
*     control        Describes how a front-end displays the event. It can be an
*                    ored combination of the following constants:
*                       ELOG_XT_ID
*                       ELOG_BLOCK_START
*                       ELOG_BLOCK_END
*                       ELOG_BLOCK_PREPARE
*                       ELOG_EVENT_OCCURRENCE
*
*
*     key_hig        If ELOG_XT_ID is used, this is the high word of the data value,
*                    which further identifies the event. Otherwise 0.
*
*     key_low        If ELOG_XT_ID is used, this is the low word of the data value,
*                    which further identifies the event. Otherwise 0.
*
*    *symbol_ptr     A symbol string, which can be displayed by the front-end, at
*                    the position of the events occurrence.
*
*  RETURNS
*    ELOG_NO_ERROR      Event described successfully.
*
*    ELOG_NO_DISPLAY    The display ID specified wasn't defined before by
*                       elog_define_display().
*
*    ELOG_NO_MEM        The event description could not be saved because
*                       the global memory could not be allocated.
*
*  REMARKS
*
*******************************************************************************/
int elog_describe_event(UInt32  id,
                        UInt32  display_id,
                        UInt32  control,
                        UInt32  key_high,
                        UInt32  key_low,
                        char   *symbol_ptr)
{
  rtlib_int_status_t int_status;
  elog_descriptor_type *tmp_descriptor_table_ptr;
  size_t symbol_length;

  /* check for the id in the display table */
  if (display_id >= elog_info_struct_ptr->display_table_size)
    return(ELOG_NO_DISPLAY);

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* if there is no descriptor table table, create it... */
  if (elog_info_struct_ptr->descriptor_table_ptr == NULL)
  {
    elog_info_struct_ptr->descriptor_table_ptr = ELOG_MALLOC_GLOBAL(sizeof(elog_descriptor_type));

    if (!elog_info_struct_ptr->descriptor_table_ptr)
    {
      RTLIB_INT_RESTORE(int_status);
      return ELOG_NO_MEM;
    }
  }
  else
  {
    /* ...otherwise expand it */
    tmp_descriptor_table_ptr = ELOG_MALLOC_GLOBAL(sizeof(elog_descriptor_type) * (elog_info_struct_ptr->descriptor_table_size + 1));

    if (!tmp_descriptor_table_ptr)
    {
      RTLIB_INT_RESTORE(int_status);
      return ELOG_NO_MEM;
    }

    ELOG_MEMCPY_GLOBAL(tmp_descriptor_table_ptr,
                       elog_info_struct_ptr->descriptor_table_ptr,
                       sizeof(elog_descriptor_type) * (elog_info_struct_ptr->descriptor_table_size + 1));

    ELOG_FREE_GLOBAL(elog_info_struct_ptr->descriptor_table_ptr);
    elog_info_struct_ptr->descriptor_table_ptr = tmp_descriptor_table_ptr;
  }

  /* fill out the event descriptor entry */
  elog_info_struct_ptr->descriptor_table_ptr[elog_info_struct_ptr->descriptor_table_size].id = id & ELOG_ID_MASK;
  elog_info_struct_ptr->descriptor_table_ptr[elog_info_struct_ptr->descriptor_table_size].display_id = display_id;
  elog_info_struct_ptr->descriptor_table_ptr[elog_info_struct_ptr->descriptor_table_size].control = control;
  elog_info_struct_ptr->descriptor_table_ptr[elog_info_struct_ptr->descriptor_table_size].key_high = key_high;
  elog_info_struct_ptr->descriptor_table_ptr[elog_info_struct_ptr->descriptor_table_size].key_low = key_low;

  symbol_length = strlen(symbol_ptr) + 1;
  if (symbol_length > ELOG_SYMBOL_LENGTH)
  {
    symbol_length = ELOG_SYMBOL_LENGTH;
    strcpy(&symbol_ptr[ELOG_SYMBOL_LENGTH - 1], "\0");
  }

  ELOG_MEMCPY_GLOBAL(&(elog_info_struct_ptr->descriptor_table_ptr[elog_info_struct_ptr->descriptor_table_size].symbol),
                     symbol_ptr,
                     symbol_length);
  
  elog_info_struct_ptr->descriptor_table_size++;

  RTLIB_INT_RESTORE(int_status);

  return 0;
}

#endif  /* !_INLINE */


/*******************************************************************************
*
*  FUNCTION
*    carry out basic background services
*
*  SYNTAX
*    void elog_service()
*
*  DESCRIPTION
*    The function handles the buffer switching and category setting requests
*    from the Host and stores the elog buffer content out to the host memory
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*    The function elog_buffer_store() has been extended and renamed to 
*    elog_service(), it is still valid as elog_buffer_store() is mapped to
*    the new name elog_service(), but should not be used any more as it may
*    be omitted in future versions of DSELOG.
*
*******************************************************************************/

__INLINE void elog_service (void)
{
  static int store_flag = 1;

  /* check if the host has requested a category set */
  if (elog_info_struct_ptr->category_set & ELOG_CAT_SET_HOST)
  {
    elog_current_category = elog_info_struct_ptr->category;
    elog_info_struct_ptr->category_set = ELOG_CAT_SET_NONE;
  }

  /* store the buffer only if at least one of the categories is set or
     if the buffer hasn't been stored since the categories have been disabled */
  if ((elog_current_category & ELOG_CAT_ALL) || store_flag)
  {
    ELOG_BUFFER_STORE();
    store_flag = 1;
  }

  if (!elog_current_category)
  {
    store_flag = 0;
  }

  /* check if the host has requested a buffer switch */
  if (elog_info_struct_ptr->buffer_switch)
  {
    /* enter the new buffer number */
    elog_info_struct_ptr->current_buffer = 1 - elog_info_struct_ptr->current_buffer;

    /* update the global buffer pinter in local memory */
    elog_buffer_ptr = elog_buffers_ptr[elog_info_struct_ptr->current_buffer];

    /* reset the buffer switch request flag */
    elog_info_struct_ptr->buffer_switch = ELOG_BUFFER_SWITCH_IDLE;

    /* update the store address information */
    elog_store_addr = ELOG_STORE_ADDR(elog_info_struct_ptr->current_buffer);
  }
}

/*******************************************************************************
*
*  FUNCTION
*    set the log category
*
*  SYNTAX
*    void elog_category_write(UInt32 category) 
*
*  DESCRIPTION
*    the function sets the category as specified and communicates it to the host
*
*  PARAMETERS
*    category_mask: the new category bit mask
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

__INLINE void elog_category_write(UInt32 category_mask)
{
  elog_current_category = category_mask;
  elog_info_struct_ptr->category = category_mask;
  elog_info_struct_ptr->category_set = ELOG_CAT_SET_RTP;
}


/*******************************************************************************
*
*  FUNCTION
*    elog_category_set
*
*
*  SYNTAX
*    void elog_category_set(UInt32 category_mask) 
*
*  DESCRIPTION
*    sets the categories specified by the category mask, all others remain
*    unchanged
*
*  PARAMETERS
*    category_mask: bit mask, containing a "1" for all categories to be set
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

__INLINE void elog_category_set(UInt32 category_mask)
{
  elog_current_category |= category_mask;
  elog_info_struct_ptr->category = elog_current_category;
  elog_info_struct_ptr->category_set = ELOG_CAT_SET_RTP;
}


/*******************************************************************************
*
*  FUNCTION
*    elog_category_reset
*
*
*  SYNTAX
*    void elog_category_reset(UInt32 category_mask) 
*
*  DESCRIPTION
*    resets the categories specified by the category mask, all others remain
*    unchanged
*
*  PARAMETERS
*    category_mask: bit mask, containing a "1" for all categories to be reset
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

__INLINE void elog_category_reset(UInt32 category_mask)
{
  elog_current_category &= ~category_mask;
  elog_info_struct_ptr->category = elog_current_category;
  elog_info_struct_ptr->category_set = ELOG_CAT_SET_RTP;
}

#undef __INLINE

