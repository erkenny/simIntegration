/******************************************************************************
  FILE:
    DSMSG.C

  DESCRIPTION:
    Message Module

  REMARKS:

  AUTHORS:  V. Lang, R. Leinfellner

  Copyright (c) 1998 - 2000 dSPACE GmbH, GERMANY

 $RCSfile: DSMSG.C $ $Revision: 1.10 $ $Date: 2009/06/24 10:27:05GMT+01:00 $
******************************************************************************/

#include <dsmsg.h>
#include <stdlib.h>
#include <string.h>

#if defined(_DS1003)
  #include <cfg1003.h>
  #include <dsvcm.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif (defined(_DS1004) || defined (__alpha__))
  #include <cfg1004.h>
  #include <dsvcm.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif defined(_DS1005)
  #include <cfg1005.h>
  #include <dsvcm.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined(_DS1006)
  #include <cfg1006.h>
  #include <dsvcm.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif defined(_DS1102)
  #include <cfg1102.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif defined(_DS1103)
  #include <cfg1103.h>
  #include <dsvcm.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined(_DS1104)
  #include <cfg1104.h>
  #include <dsvcm.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined(_DS1401)
  #include <cfg1401.h>
  #include <dsvcm.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1602)
#ifndef _INLINE
  void *  msg_rp565_message_buffer_p = NULL;        /*** message buffer pointer ***/
#else
  extern void *  msg_rp565_message_buffer_p;
#endif
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1603)
#ifndef _INLINE
  void *  msg_rp5554_message_buffer_p = NULL;        /*** message buffer pointer ***/
#else
  extern void *  msg_rp5554_message_buffer_p;
#endif
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#else
  #error dsmsg.c: no processor board defined (e.g. _DS1003)
#endif

/* $DSVERSION: MSG - Message Module */
#define MSG_VER_MAR 1
#define MSG_VER_MIR 3
#define MSG_VER_MAI 2
#define MSG_VER_SPB VCM_VERSION_RELEASE
#define MSG_VER_SPN 0
#define MSG_VER_PLV 0

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 ||defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401

DS_VERSION_SYMBOL(_msg_ver_mar, MSG_VER_MAR);
DS_VERSION_SYMBOL(_msg_ver_mir, MSG_VER_MIR);
DS_VERSION_SYMBOL(_msg_ver_mai, MSG_VER_MAI);
DS_VERSION_SYMBOL(_msg_ver_spb, MSG_VER_SPB);
DS_VERSION_SYMBOL(_msg_ver_spn, MSG_VER_SPN);
DS_VERSION_SYMBOL(_msg_ver_plv, MSG_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/* ------------------------------------------------------------------------- */

#ifndef _INLINE

/* ---- config section pointer --------------------------------------------- */

msg_descriptor_type *msg_descriptor;

/* ---- message management variables --------------------------------------- */

volatile msg_hookfcn_type msg_error_hook;

volatile UInt32 msg_index;
volatile UInt32 msg_last_error_no;
volatile UInt32 msg_last_submodule;
volatile UInt32 msg_temp;

volatile msg_dialog_type msg_dlg_preset_error   = MSG_DLG_OKCANCEL;
volatile msg_dialog_type msg_dlg_preset_info    = MSG_DLG_NONE;
volatile msg_dialog_type msg_dlg_preset_warning = MSG_DLG_NONE;
volatile msg_dialog_type msg_dlg_preset_log     = MSG_DLG_NONE;

/* the timestamp functionality is only available on PPC Boards and DS1006 */
#if defined _DS1103 || defined _DS1005 || defined _DS1006 || defined _DS1401 || defined _DS1104
volatile int   msg_timestamp_enable;
volatile char  msg_timestamp_separator[4]=" ";
volatile int   msg_timestamp_digits = 6;
#endif

/* ---- buffer for temporary string storage -------------------------------- */

char * msg_temp_buffer = 0;

#if defined (_DS1602) || defined (_DS1603)
UInt32  dsmsg_buffer_out_index = 0;
UInt32  dsmsg_buffer_out_count = 0;
#endif


#else

/* ---- config section pointer --------------------------------------------- */

extern msg_descriptor_type *msg_descriptor;

/* ---- message management variables --------------------------------------- */

extern volatile msg_hookfcn_type msg_error_hook;

extern volatile UInt32 msg_index;
extern volatile UInt32 msg_last_error_no;
extern volatile UInt32 msg_last_submodule;
extern volatile UInt32 msg_temp;

extern volatile msg_dialog_type msg_dlg_preset_error;
extern volatile msg_dialog_type msg_dlg_preset_info;
extern volatile msg_dialog_type msg_dlg_preset_warning;
extern volatile msg_dialog_type msg_dlg_preset_log;

/* the timestamp functionality is only available on PPC Boards and DS1006 */
#if defined _DS1103 || defined _DS1005 || defined _DS1006 || defined _DS1401 || defined _DS1104
extern volatile int  msg_timestamp_enable;
extern volatile char msg_timestamp_separator[];
extern volatile int  msg_timestamp_digits;
#endif

#if defined (_DS1602) || defined (_DS1603)
extern UInt32  dsmsg_buffer_out_index;
extern UInt32  dsmsg_buffer_out_count;
#endif

#endif


#ifndef _INLINE

/* ---- msg_init ----------------------------------------------------------- */

void msg_init(void)
{
  static int msg_initialized = 0;
#ifdef VCM_AVAILABLE
  vcm_module_descriptor_type *msg_vcm_module_ptr;
#endif

  if (msg_initialized == 0)
  {
  MSG_INIT_BUFFER();
  msg_descriptor = CFG_MSG_DESC_PTR;
   
#ifdef VCM_AVAILABLE
  msg_vcm_module_ptr = vcm_module_register(VCM_MID_MESSAGE,
                                           vcm_module_find(VCM_MID_RTLIB, NULL),
                                           VCM_TXT_MESSAGE,
                                           MSG_VER_MAR, MSG_VER_MIR, MSG_VER_MAI,
                                           MSG_VER_SPB, MSG_VER_SPN, MSG_VER_PLV,
                                           0, 0);

 MSG_VCM_PTR_SET();
#endif

  msg_descriptor->msg_buffer_mode = MSG_OVERWRITE;
  msg_descriptor->msg_buffer_length = MSG_BUFFER_LENGTH;
  msg_descriptor->msg_length = MSG_LENGTH;
  msg_descriptor->msg_counter = 0;

#if defined (_DS1602) || defined (_DS1603)
  dsmsg_buffer_out_index = 0;
  dsmsg_buffer_out_count = 0;
#endif

  msg_error_hook = 0;
  msg_index = 0;
  msg_last_error_no = 0;
  msg_last_submodule = MSG_SM_NONE;

  MSG_SET_OLD_ERRORCODE(0, "\0");

  msg_temp_buffer = (char *)malloc(MSG_TEMP_BUFFER_SIZE);
  if (!msg_temp_buffer)
  {
    exit(1);
  }

  msg_initialized = 1;

#ifdef VCM_AVAILABLE
  vcm_module_status_set(msg_vcm_module_ptr,VCM_STATUS_INITIALIZED);
#endif
  }
  
}

/* ---- msg_default_dialog_set --------------------------------------------- */

void msg_default_dialog_set(msg_class_type msg_class, msg_dialog_type msg_dialog)
{
  if ((msg_dialog != MSG_DLG_OKCANCEL) &&
      (msg_dialog != MSG_DLG_NONE))
  {
    msg_error_set(MSG_SM_RTLIB, MSG_INV_DEF_DIALOG_ERROR, MSG_INV_DEF_DIALOG_ERROR_TXT);
    exit(1);
  }

  switch(msg_class)
  {
    case MSG_MC_ERROR:   msg_dlg_preset_error = msg_dialog;
                         break;
    case MSG_MC_INFO:    msg_dlg_preset_info = msg_dialog;
                         break;
    case MSG_MC_WARNING: msg_dlg_preset_warning = msg_dialog;
                         break;
    case MSG_MC_LOG:     msg_dlg_preset_log = msg_dialog;
                         break;
    default:             msg_error_set(MSG_SM_RTLIB, MSG_INV_DEF_CLASS_ERROR, MSG_INV_DEF_CLASS_ERROR_TXT);
  }
}


/* ---- msg_set ------------------------------------------------------------ */

void msg_set(msg_class_type     msg_class,
             msg_dialog_type    msg_dialog,
             msg_submodule_type submodule,
             msg_no_type        msg_no,
             const char         *msg)
{
  int do_continue = 1;
  UInt32 msg_int_state;
  UInt32 msg_length;

  if ((msg_class != MSG_MC_ERROR) && (msg_class != MSG_MC_INFO) && (msg_class != MSG_MC_WARNING) && (msg_class != MSG_MC_LOG))
  {
    msg_error_set(MSG_SM_RTLIB, MSG_INV_CLASS_ERROR, MSG_INV_CLASS_ERROR_TXT);
    exit(1);
  }

  if ((msg_dialog != MSG_DLG_OKCANCEL) &&
      (msg_dialog != MSG_DLG_NONE) &&
      (msg_dialog != MSG_DLG_DEFAULT))
  {
    msg_error_set(MSG_SM_RTLIB, MSG_INV_DIALOG_ERROR, MSG_INV_DIALOG_ERROR_TXT);
    exit(1);
  }

  if ((msg_error_hook) && (msg_class == MSG_MC_ERROR))
    do_continue = msg_error_hook(submodule, msg_no);

  if (do_continue)
  {
    /* disable interrupts */
    MSG_INT_DISABLE();

    /* update counter */
    msg_descriptor->msg_counter++;

#if defined (_DS1602) || defined (_DS1603)
    if((msg_descriptor->msg_counter - dsmsg_buffer_out_count) > msg_descriptor->msg_buffer_length)
    {
      dsmsg_buffer_out_count++;
      if(++dsmsg_buffer_out_index >= msg_descriptor->msg_buffer_length)
      {
        dsmsg_buffer_out_index = 0;
      }
    }
#endif


    /* set last error number and submodule */
    if (msg_class == MSG_MC_ERROR)
    {
      msg_last_error_no = msg_no;
      msg_last_submodule = submodule;
    }

    /* return if overwrite-mode disabled and buffer full */
    if ((msg_descriptor->msg_buffer_mode == MSG_BLOCKING) &&
        (msg_index == msg_descriptor->msg_buffer_length))
    {
      MSG_INT_RESTORE();
      return;
    }

    /* determine the default dialog, if desired */
    if (msg_dialog == MSG_DLG_DEFAULT)
    {
      switch (msg_class)
      {
        case MSG_MC_ERROR:   msg_dialog = msg_dlg_preset_error;
                             break;
        case MSG_MC_INFO:    msg_dialog = msg_dlg_preset_info;
                             break;
        case MSG_MC_WARNING: msg_dialog = msg_dlg_preset_warning;
                             break;
        case MSG_MC_LOG:     msg_dialog = msg_dlg_preset_log;
                             break;
        default:             break;
      }
    }

    /* write count first time */
    msg_descriptor->msg_buffer[msg_index].count_1 = msg_descriptor->msg_counter;

    /* write error code */
    msg_descriptor->msg_buffer[msg_index].code_h = (MSG_MC_VERSION     << OFFSET_VERSION)  |
                                                   (msg_dialog         << OFFSET_DIALOG)   |
                                                   (MSG_MC_DLL         << OFFSET_DLL)      |
                                                   (msg_class          << OFFSET_CLASS)    |
                                                   (submodule          << OFFSET_SUBMODUL) |
                                                   (MSG_MC_MAIN_MODULE << 0);
    msg_descriptor->msg_buffer[msg_index].code_l = msg_no;

	msg_length = strlen(msg);

    /* if the message string is longer as the MSG_STRING_LENGTH mark the message with "~" at its end*/
	if (msg_length > MSG_STRING_LENGTH ) {
      ((char*)msg)[MSG_STRING_LENGTH-1] = '~';
      msg_length = MSG_STRING_LENGTH;
	}

    /* write error message */
    MSG_STRCPY(msg_descriptor->msg_buffer[msg_index].message, msg, msg_length);

    /* write count second time */
    msg_descriptor->msg_buffer[msg_index].count_2 = msg_descriptor->msg_counter;

    /* update index */
    if (++msg_index >= msg_descriptor->msg_buffer_length)
    {
      if (msg_descriptor->msg_buffer_mode == MSG_OVERWRITE)
        msg_index = 0;
    }

#ifdef VCM_AVAILABLE
    /* update magic number */
    vcm_entry_changed();
#endif

    /* restore interrupts */
    MSG_INT_RESTORE();
  }
}

/* ---- msg_error_set ------------------------------------------------------ */

void msg_error_set(msg_submodule_type submodule,
                   msg_no_type        msg_no,
                   const char         *msg)
{
  msg_set(MSG_MC_ERROR, MSG_DLG_DEFAULT, submodule, msg_no, msg);
}

/* ---- msg_info_set ------------------------------------------------------- */

void msg_info_set(msg_submodule_type submodule,
                  msg_no_type        msg_no,
                  const char         *msg)
{
  msg_set(MSG_MC_INFO, MSG_DLG_DEFAULT, submodule, msg_no, msg);
}


/* ---- msg_warning_set ---------------------------------------------------- */

void msg_warning_set(msg_submodule_type submodule,
                     msg_no_type        msg_no,
                     const char         *msg)
{
  msg_set(MSG_MC_WARNING, MSG_DLG_DEFAULT, submodule, msg_no, msg);
}

#endif /* !_INLINE */


/* ---- msg_mode_set ------------------------------------------------------- */

__INLINE void msg_mode_set(UInt32 mode)
{
  msg_descriptor->msg_buffer_mode = mode;

  if (msg_index >= msg_descriptor->msg_buffer_length)
    msg_index = 0;

#ifdef VCM_AVAILABLE
    /* update magic number */
    vcm_entry_changed();
#endif
}

/* ---- msg_last_error_no -------------------------------------------------- */

__INLINE msg_no_type msg_last_error_number()
{
  return msg_last_error_no;
}

/* ---- msg_last_error_submodule ------------------------------------------- */

__INLINE msg_submodule_type msg_last_error_submodule()
{
  return msg_last_submodule;
}

/* ---- msg_error_hook_set ------------------------------------------------- */

__INLINE void msg_error_hook_set(msg_hookfcn_type error_hook)
{
  msg_error_hook = error_hook;
}

/* ---- msg_reset ---------------------------------------------------------- */

__INLINE void msg_reset()
{
  msg_last_error_no = 0;
  msg_last_submodule = 0;
  msg_index = 0;
  msg_descriptor->msg_counter = 0;
#if defined (_DS1602) || defined (_DS1603)
  dsmsg_buffer_out_count = 0;
  dsmsg_buffer_out_index = 0;
#endif
}

/* ---- msg_error_clear ---------------------------------------------------------- */

__INLINE void msg_error_clear()
{
  msg_last_error_no = 0;
  msg_last_submodule = 0;
}


/* the timestamp functionality is only available on PPC Boards and DS1006 */
#if defined _DS1103 || defined _DS1005 || defined _DS1006 || defined _DS1401 || defined _DS1104

/* ---- msg_timestamp_mode_set --------------------------------------------- */

__INLINE int msg_timestamp_mode_set(int mode)
{
  int current_mode = msg_timestamp_enable;

  msg_timestamp_enable = mode ? MSG_TIMESTAMP_ON : MSG_TIMESTAMP_OFF;

  return(current_mode);
}

/* ---- msg_timestamp_format_set ------------------------------------------- */
__INLINE int msg_timestamp_format_set(int digits, char* sepa_string_p)
{
  if (sepa_string_p)
  {
    *(UInt32*)msg_timestamp_separator=*(UInt32*)sepa_string_p;
    msg_timestamp_separator[3] = 0;
  }
  
  if((0<=digits)&&(9>=digits))
  {
    msg_timestamp_digits = digits;
  }
  else
  {
    msg_timestamp_digits = 6;
    return(0);
  }

  return(1);
}

#else

/* ---- msg_timestamp_mode_set ------ for none PPC Boards ------------------ */
__INLINE int msg_timestamp_mode_set(int mode)
{
  return(0);
}

/* ---- msg_timestamp_format_set ---- for none PPC Boards ------------------ */
__INLINE int msg_timestamp_format_set(int digits, char* sepa_string_p)
{
  return(1);
}

#endif

#if defined (_DS1602) || defined (_DS1603)


/* --- msg_buffer_get_next_entry ----- for polling of stored messages --- */

__INLINE UInt32  msg_buffer_get_next_entry(msg_buffer_entry_type **buffer)
{    
  msg_buffer_entry_type *  temp_buffer_p  = NULL;
  UInt8                 *  string_p       = NULL;
  UInt32                   sizeof_string  = 0;
  
  /* Dsmsg service initialized ? */
  if(msg_descriptor == NULL)
  {    
    *buffer       =NULL;
     return 0;
  }

  if(msg_descriptor->msg_counter == dsmsg_buffer_out_count)
  { 
    /* No messages in buffer */
    *buffer       = NULL;
     return 0;
  }

  /* Checking next message in buffer */
  if(msg_descriptor->msg_buffer[dsmsg_buffer_out_index].count_1 != msg_descriptor->msg_buffer[dsmsg_buffer_out_index].count_2)
  {
    return 0;
  }

  temp_buffer_p =        &msg_descriptor->msg_buffer[dsmsg_buffer_out_index];  
  *buffer       =         temp_buffer_p;
  string_p      = (UInt8*)temp_buffer_p->message;

  sizeof_string = 1;

  while(*string_p != 0x00)
  {
    string_p++;
    sizeof_string++;
  }
  return  sizeof_string;
}

/* --- msg_buffer_quit_next_entry ----- sets pointer to next message in buffer --- */

__INLINE UInt32 msg_buffer_quit_next_entry()
{
  if(msg_descriptor->msg_counter != dsmsg_buffer_out_count)
  {
    if(++dsmsg_buffer_out_index >= msg_descriptor->msg_buffer_length)
    {
      dsmsg_buffer_out_index = 0;
    }
    dsmsg_buffer_out_count++;    
  }
  return msg_descriptor->msg_counter - dsmsg_buffer_out_count;
}

/* --- msg_buffer_get_no_of_entries ----- returns the number of messages in the message buffer --- */

__INLINE UInt32 msg_buffer_get_no_of_entries()
{
  return msg_descriptor->msg_counter - dsmsg_buffer_out_count;
}

#endif /*defined _DS1602*/

/* ------------------------------------------------------------------------- */

#undef __INLINE

