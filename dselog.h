/*******************************************************************************
*
*  MODULE
*    DSELOG - Event Logger
*
*  FILE
*    dselog.h
*
*  RELATED FILES
*    dselog.c
*    elogxxxx.h
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
*  $RCSfile: dselog.h $ $Revision: 1.4 $ $Date: 2006/08/09 15:12:10GMT+01:00 $
*******************************************************************************/

#ifndef __dselog_h__
#define __dselog_h__

#include <dsts.h>

#if defined(_DS1005)
#include <elog1005.h>
#elif defined(_DS1006)
#include <elog1006.h>
#elif defined(_DS1103)
#include <elog1103.h>
#elif defined(_DS1104)
#include <elog1104.h>
#elif defined(_DS1401)
#include <elog1401.h>
#endif

#define ELOG_AVAILABLE

/*******************************************************************************
  constant, macro and type definitions
*******************************************************************************/

/* 64 bit event data type */
typedef struct
{
  UInt32 high;
  UInt32 low;
} elog_data_type;

/* data structure for one event entry */
typedef struct
{
  UInt32            count1;
  ts_timestamp_type ts;
  UInt32            spec;
  elog_data_type    data;
  UInt32            count2;
} elog_event_type;

/* event buffer data structure */
typedef struct
{
  elog_event_type   entry[ELOG_BUFFER_SIZE];
} elog_buffer_type;

/* structure, holding a descriptive text for the specified event and
   information how the event shall be interpreted by a frint-end */
typedef struct
{
  UInt32                id;
  UInt32                display_id;
  UInt32                control;
  UInt32                key_high;
  UInt32                key_low;
  char                  symbol[ELOG_SYMBOL_LENGTH];
} elog_descriptor_type;

/* structure, holding a lable for a defined display line */
typedef struct
{
  char   label[ELOG_DISPLAY_LABEL_LENGTH];
} elog_display_entry_type;


/* central data structure containing pointers to the elog buffers, the event
   descriptor table, the display table and their sizes for host access */
typedef struct
{
  elog_buffer_type        *buffer_ptr;             /* pointer to the first buffer */
  UInt32                   buffer_size;
  elog_display_entry_type *display_table_ptr;
  UInt32                   display_table_size;
  elog_descriptor_type    *descriptor_table_ptr;
  UInt32                   descriptor_table_size;
  UInt32                   display_label_length;
  UInt32                   symbol_length;
  elog_buffer_type        *buffers_ptr[2];         /* pointer to the second buffer */
  UInt32                   current_buffer;         /* indexes the buffer being currently written */
  UInt32                   buffer_switch;          /* flag for signaling a buffer switch */
  UInt32                   category;               /* field for communicating the category */
  UInt32                   category_set;           /* field for communicating a category set */
} elog_info_struct_type;


/* map elog_buffer_store() to elog_service() for compatibility to DSELOG 1.0 */
#define elog_buffer_store elog_service

/* return codes */
#define ELOG_NO_ERROR            0
#define ELOG_ALREADY_INITIALIZED 1
#define ELOG_NO_DISPLAY          2
#define ELOG_NO_MEM              3

/* category set bitmasks */
#define ELOG_CAT_SET_NONE  0x00000000 /* idle */
#define ELOG_CAT_SET_RTP   0x00000001 /* category set by the RTP */
#define ELOG_CAT_SET_HOST  0x00000002 /* category set request by the host */

/* buffer switch commands */
#define ELOG_BUFFER_SWITCH_IDLE     0 /* do nothing */
#define ELOG_BUFFER_SWITCH_REQUEST  1 /* buffer switch is requested */

/* elog mode constants */
#define ELOG_MODE_OVERWRITE 0
#define ELOG_MODE_BLOCKING  1

/* event specifier bit masks */
#define ELOG_ID_MASK       0x000000FF
#define ELOG_CATEGORY_MASK 0x0FFFFF00

/* elog categories */
#define ELOG_CAT_NONE 0x00000000
#define ELOG_CAT_1    0x00000100
#define ELOG_CAT_2    0x00000200
#define ELOG_CAT_3    0x00000400
#define ELOG_CAT_4    0x00000800
#define ELOG_CAT_5    0x00001000
#define ELOG_CAT_6    0x00002000
#define ELOG_CAT_7    0x00004000
#define ELOG_CAT_8    0x00008000
#define ELOG_CAT_9    0x00010000
#define ELOG_CAT_10   0x00020000
#define ELOG_CAT_11   0x00040000
#define ELOG_CAT_12   0x00080000
#define ELOG_CAT_13   0x00100000
#define ELOG_CAT_14   0x00200000
#define ELOG_CAT_15   0x00400000
#define ELOG_CAT_16   0x00800000
#define ELOG_CAT_17   0x01000000
#define ELOG_CAT_18   0x02000000
#define ELOG_CAT_19   0x04000000
#define ELOG_CAT_20   0x08000000
#define ELOG_CAT_ALL  0x0fffff00

/* predefined categories */
#define ELOG_CAT_TASK            ELOG_CAT_1
#define ELOG_CAT_TASK_REG        ELOG_CAT_2
#define ELOG_CAT_OVERRUN         ELOG_CAT_3
#define ELOG_CAT_ISR             ELOG_CAT_4
#define ELOG_CAT_IDLE_MODE       ELOG_CAT_5
#define ELOG_CAT_IRPT_SWITCH     ELOG_CAT_6
#define ELOG_CAT_COMMUNICATION   ELOG_CAT_7
#define ELOG_CAT_BLOCK           ELOG_CAT_8
#define ELOG_CAT_EVENT           ELOG_CAT_9
#define ELOG_CAT_BACKGROUND      ELOG_CAT_10
#define ELOG_CAT_RTT             ELOG_CAT_11
#define ELOG_CAT_LAST_PREDEFINED ELOG_CAT_11

/* predefined event ID's */
#define ELOG_ID_TASK_START           0
#define ELOG_ID_TASK_END             1
#define ELOG_ID_TASK_REGISTER        2
#define ELOG_ID_OVERRUN              3
#define ELOG_ID_ISR_START            4
#define ELOG_ID_ISR_END              5
#define ELOG_ID_ENABLE_SVC           6
#define ELOG_ID_DISABLE_SVC          7
#define ELOG_ID_ENTER_IDLE           8
#define ELOG_ID_LEAVE_IDLE           9
#define ELOG_ID_GLOBAL_ENABLE       10
#define ELOG_ID_GLOBAL_DISABLE      11
#define ELOG_ID_COM_SEND            12
#define ELOG_ID_COM_WAIT            13
#define ELOG_ID_COM_RECEIVE         14
#define ELOG_ID_BLOCK_START         15
#define ELOG_ID_BLOCK_END           16
#define ELOG_ID_EVENT               17
#define ELOG_ID_BACKGROUND_EVENT    18
#define ELOG_ID_BACKGROUND_START    19
#define ELOG_ID_BACKGROUND_END      20
#define ELOG_ID_DL_VIOLATION        21
#define ELOG_ID_RTT_START           22
#define ELOG_ID_RTT_END             23
#define ELOG_ID_LAST_PREDEFINED     23

/* predefined event specifiers */
#define ELOG_SPEC_TASK_START       (ELOG_ID_TASK_START       | ELOG_CAT_TASK)
#define ELOG_SPEC_TASK_END         (ELOG_ID_TASK_END         | ELOG_CAT_TASK)
#define ELOG_SPEC_TASK_REGISTER    (ELOG_ID_TASK_REGISTER    | ELOG_CAT_TASK_REG)
#define ELOG_SPEC_OVERRUN          (ELOG_ID_OVERRUN          | ELOG_CAT_OVERRUN)
#define ELOG_SPEC_ISR_START        (ELOG_ID_ISR_START        | ELOG_CAT_ISR)
#define ELOG_SPEC_ISR_END          (ELOG_ID_ISR_END          | ELOG_CAT_ISR)
#define ELOG_SPEC_ENABLE_SVC       (ELOG_ID_ENABLE_SVC       | ELOG_CAT_IDLE_MODE)
#define ELOG_SPEC_DISABLE_SVC      (ELOG_ID_DISABLE_SVC      | ELOG_CAT_IDLE_MODE)
#define ELOG_SPEC_ENTER_IDLE       (ELOG_ID_ENTER_IDLE       | ELOG_CAT_IDLE_MODE)
#define ELOG_SPEC_LEAVE_IDLE       (ELOG_ID_LEAVE_IDLE       | ELOG_CAT_IDLE_MODE)
#define ELOG_SPEC_GLOBAL_ENABLE    (ELOG_ID_GLOBAL_ENABLE    | ELOG_CAT_IRPT_SWITCH)
#define ELOG_SPEC_GLOBAL_DISABLE   (ELOG_ID_GLOBAL_DISABLE   | ELOG_CAT_IRPT_SWITCH)
#define ELOG_SPEC_COM_SEND         (ELOG_ID_COM_SEND         | ELOG_CAT_COMMUNICATION)
#define ELOG_SPEC_COM_WAIT         (ELOG_ID_COM_WAIT         | ELOG_CAT_COMMUNICATION)
#define ELOG_SPEC_COM_RECEIVE      (ELOG_ID_COM_RECEIVE      | ELOG_CAT_COMMUNICATION)
#define ELOG_SPEC_BLOCK_START      (ELOG_ID_BLOCK_START      | ELOG_CAT_BLOCK)
#define ELOG_SPEC_BLOCK_END        (ELOG_ID_BLOCK_END        | ELOG_CAT_BLOCK)
#define ELOG_SPEC_EVENT            (ELOG_ID_EVENT            | ELOG_CAT_EVENT)
#define ELOG_SPEC_BACKGROUND_EVENT (ELOG_ID_BACKGROUND_EVENT | ELOG_CAT_BACKGROUND)
#define ELOG_SPEC_BACKGROUND_START (ELOG_ID_BACKGROUND_START | ELOG_CAT_BACKGROUND)
#define ELOG_SPEC_BACKGROUND_END   (ELOG_ID_BACKGROUND_END   | ELOG_CAT_BACKGROUND)
#define ELOG_SPEC_DL_VIOLATION     (ELOG_ID_DL_VIOLATION     | ELOG_CAT_OVERRUN)
#define ELOG_SPEC_RTT_START        (ELOG_ID_RTT_START        | ELOG_CAT_RTT)
#define ELOG_SPEC_RTT_END          (ELOG_ID_RTT_END          | ELOG_CAT_RTT)

/* control bit masks */
#define ELOG_CTRL_NONE            0x00000000
#define ELOG_CTRL_EXT_ID          0x00000003
#define ELOG_CTRL_EXT_ID_LOW      0x00000001
#define ELOG_CTRL_EXT_ID_HIGH     0x00000002
#define ELOG_CTRL_BLOCK_START     0x00000004
#define ELOG_CTRL_BLOCK_END       0x00000008
#define ELOG_CTRL_BLOCK_PREPARE   0x00000010
#define ELOG_CTRL_EVENT           0x00000020
#define ELOG_CTRL_BLOCK_SUSPEND   0x00000040
#define ELOG_CTRL_BLOCK_RESUME    0x00000080
#define ELOG_CTRL_BLOCK_STATIC    0x00000100

#define ELOG_LINK_DATA    0x10000000

/******************************************************************************
  object declarations
******************************************************************************/

extern elog_buffer_type *elog_buffer_ptr;
extern unsigned int elog_buffer_index;
extern int elog_buffer_mode;
extern UInt32 elog_current_category;
extern UInt32 elog_event_count;
extern UInt32 elog_buffer_size;
extern UInt32 elog_byte_count;


/******************************************************************************
  function prototypes
******************************************************************************/

#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif

int elog_init(void);
int elog_buffer_mode_set(int buffer_mode);
UInt32 elog_define_display(char *label);
int elog_set_display_label(UInt32 display_id, char *label);
int elog_describe_event(UInt32  id,
                        UInt32  display_id,
                        UInt32  control,
                        UInt32  key_high,
                        UInt32  key_low,
                        char   *symbol_ptr);

__INLINE void elog_service(void);
__INLINE void elog_category_write(UInt32 category);
__INLINE void elog_category_set(UInt32 category);
__INLINE void elog_category_reset(UInt32 category);

#undef __INLINE


/******************************************************************************
  function macros
******************************************************************************/

/*******************************************************************************
*
*  FUNCTION
*    Add one entry to the event log
*
*  SYNTAX
*    ELOG_ENTRY_SET()
*
*  DESCRIPTION
*    The function updates the buffer index and adds, dependant of the current
*    category, an event consisting of 64 Bit data and an implicitly taken time
*    stamp.
*
*  PARAMETERS
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

#define ELOG_ENTRY_SET(event_spec, data_ptr)                                   \
{                                                                              \
  rtlib_int_status_t int_status;                                               \
  elog_event_type *entry_ptr;                                                  \
                                                                               \
  if (((event_spec) & elog_current_category) &&                                \
      ((elog_buffer_mode == ELOG_MODE_OVERWRITE) ||                            \
       (elog_buffer_index < elog_buffer_size)))                                \
  {                                                                            \
    RTLIB_INT_SAVE_AND_DISABLE(int_status);                                    \
                                                                               \
    entry_ptr = &(elog_buffer_ptr->entry[elog_buffer_index]);                  \
    if ((++elog_buffer_index >= elog_buffer_size) &&                           \
        (elog_buffer_mode == ELOG_MODE_OVERWRITE))                             \
    {                                                                          \
      elog_buffer_index = 0;                                                   \
    }                                                                          \
                                                                               \
    entry_ptr->count1 = elog_event_count;                                      \
                                                                               \
    ts_timestamp_read_fast(&entry_ptr->ts);                                    \
                                                                               \
    entry_ptr->spec = (event_spec);                                            \
                                                                               \
    *(double *) &(entry_ptr->data) = *((double *) (data_ptr));                 \
                                                                               \
    entry_ptr->count2 = elog_event_count;                                      \
                                                                               \
    elog_event_count++;                                                        \
                                                                               \
    RTLIB_INT_RESTORE(int_status);                                             \
  }                                                                            \
}


/*******************************************************************************
*
*  FUNCTION
*    Add one entry to the event log
*
*  SYNTAX
*    ELOG_TS_ENTRY_SET()
*
*  DESCRIPTION
*    The function updates the buffer index and adds, dependant of the current
*    category, an event consisting of 64 Bit data and the specified taken time
*    stamp.
*
*  PARAMETERS
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

#define ELOG_TS_ENTRY_SET(event_spec, data_ptr, ts_ptr)                        \
{                                                                              \
  rtlib_int_status_t int_status;                                               \
  elog_event_type *entry_ptr;                                                  \
                                                                               \
  if (((event_spec) & elog_current_category) &&                                \
      ((elog_buffer_mode == ELOG_MODE_OVERWRITE) ||                            \
       (elog_buffer_index < elog_buffer_size)))                                \
  {                                                                            \
    RTLIB_INT_SAVE_AND_DISABLE(int_status);                                    \
                                                                               \
    entry_ptr = &(elog_buffer_ptr->entry[elog_buffer_index]);                  \
    if ((++elog_buffer_index >= elog_buffer_size) &&                           \
        (elog_buffer_mode == ELOG_MODE_OVERWRITE))                             \
    {                                                                          \
      elog_buffer_index = 0;                                                   \
    }                                                                          \
                                                                               \
    entry_ptr->count1 = elog_event_count;                                      \
                                                                               \
    *(double *) &(entry_ptr->ts) = *(double *) (ts_ptr);                       \
                                                                               \
    entry_ptr->spec = (event_spec);                                            \
                                                                               \
    *(double *) &((entry_ptr->data).high) = *((double *) (data_ptr));          \
                                                                               \
    entry_ptr->count2 = elog_event_count;                                      \
                                                                               \
    elog_event_count++;                                                        \
                                                                               \
    RTLIB_INT_RESTORE(int_status);                                             \
  }                                                                            \
}


/*******************************************************************************
*
*  FUNCTION
*    Add one entry to the event log
*
*  SYNTAX
*    ELOG_ENTRY_SET32()
*
*  DESCRIPTION
*    The function updates the buffer index and adds, dependant of the current
*    category, an event consisting of 2 * 32 Bit data and an implicitly taken
*    time stamp.
*
*  PARAMETERS
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

#define ELOG_ENTRY_SET32(event_spec, data_high, data_low)                      \
{                                                                              \
  rtlib_int_status_t int_status;                                               \
  elog_event_type *entry_ptr;                                                  \
                                                                               \
  if (((event_spec) & elog_current_category) &&                                \
      ((elog_buffer_mode == ELOG_MODE_OVERWRITE) ||                            \
       (elog_buffer_index < elog_buffer_size)))                                \
  {                                                                            \
    RTLIB_INT_SAVE_AND_DISABLE(int_status);                                    \
                                                                               \
    entry_ptr = &(elog_buffer_ptr->entry[elog_buffer_index]);                  \
    if ((++elog_buffer_index >= elog_buffer_size) &&                           \
        (elog_buffer_mode == ELOG_MODE_OVERWRITE))                             \
    {                                                                          \
      elog_buffer_index = 0;                                                   \
    }                                                                          \
                                                                               \
    entry_ptr->count1 = elog_event_count;                                      \
                                                                               \
    ts_timestamp_read_fast(&entry_ptr->ts);                                    \
                                                                               \
    entry_ptr->spec = (event_spec);                                            \
                                                                               \
    (entry_ptr->data).high = (data_high);                                      \
    (entry_ptr->data).low = (data_low);                                        \
                                                                               \
    entry_ptr->count2 = elog_event_count;                                      \
                                                                               \
    elog_event_count++;                                                        \
                                                                               \
    RTLIB_INT_RESTORE(int_status);                                             \
  }                                                                            \
}


/*******************************************************************************
*
*  FUNCTION
*    Add one entry to the event log
*
*  SYNTAX
*    ELOG_TS_ENTRY_SET32()
*
*  DESCRIPTION
*    The function updates the buffer index and adds, dependant of the current
*    category, an event consisting of 2 * 32 Bit data and the specified time
*    stamp.
*
*  PARAMETERS
*
*  RETURNS
*    void
*
*  REMARKS
*
*******************************************************************************/

#define ELOG_TS_ENTRY_SET32(event_spec, data_high, data_low, ts_ptr)           \
{                                                                              \
  rtlib_int_status_t int_status;                                               \
  elog_event_type *entry_ptr;                                                  \
                                                                               \
  if (((event_spec) & elog_current_category) &&                                \
      ((elog_buffer_mode == ELOG_MODE_OVERWRITE) ||                            \
       (elog_buffer_index < elog_buffer_size)))                                \
  {                                                                            \
    RTLIB_INT_SAVE_AND_DISABLE(int_status);                                    \
                                                                               \
    entry_ptr = &(elog_buffer_ptr->entry[elog_buffer_index]);                  \
    if ((++elog_buffer_index >= elog_buffer_size) &&                           \
        (elog_buffer_mode == ELOG_MODE_OVERWRITE))                             \
    {                                                                          \
      elog_buffer_index = 0;                                                   \
    }                                                                          \
                                                                               \
    entry_ptr->count1 = elog_event_count;                                      \
                                                                               \
    *(double *) &(entry_ptr->ts) = *(double *) (ts_ptr);                       \
                                                                               \
    entry_ptr->spec = (event_spec);                                            \
                                                                               \
    (entry_ptr->data).high = (data_high);                                      \
    (entry_ptr->data).low = (data_low);                                        \
                                                                               \
    entry_ptr->count2 = elog_event_count;                                      \
                                                                               \
    elog_event_count++;                                                        \
                                                                               \
    RTLIB_INT_RESTORE(int_status);                                             \
  }                                                                            \
}


/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
#include <dselog.c>
#endif

#endif /* __dselog_h__ */

