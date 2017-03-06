/******************************************************************************
  FILE:
    DSMSGPRN.C

  DESCRIPTION:
    Printf Extension for the Message Module

  REMARKS:

  AUTHORS:  R. Leinfellner

  Copyright (c) 1998 - 2000 dSPACE GmbH, GERMANY

  $RCSfile: DSMSGPRN.C $ $Revision: 1.5 $ $Date: 2007/04/24 16:06:38GMT+01:00 $
******************************************************************************/


#include <stdlib.h>
#include <dsprintf.h>
#include <dsmsg.h>

#ifdef _DS1003
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #include <cfg1003.h>
#elif (defined(_DS1004) || defined(__alpha__))
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #include <cfg1004.h>
#elif defined(_DS1005)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
  #include <cfg1005.h>
#elif defined(_DS1006)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #include <cfg1006.h>
#elif defined(_DS1102)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #include <cfg1102.h>
#elif defined(_DS1103)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #include <cfg1103.h>
#elif defined(_DS1104)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #include <cfg1104.h>
#elif defined(_DS1401)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #include <cfg1401.h>
#elif defined (_DS1602)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif defined (_DS1603)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#else
  #error dsmsgprn.c: no processor board defined (e.g. _DS1003)
#endif

/* workaround : function prototype is missing in header file stdarg.h */
#if defined _DS1103 || defined _DS1005 || defined _DS1006 || defined _DS1401 || defined _DS1104
extern int __va_funcx(int, ...);

/* the timestamp functionality is only on PPC Boards available*/
#include <dsts.h>
#endif

/* ---- msg_do_printf --------------------------------------------------- */

int msg_do_printf(msg_class_type     msg_class,
                  msg_dialog_type    msg_dialog,
                  msg_submodule_type submodule,
                  msg_no_type        no,
                  const char         *format,
                  va_list            args_list)
{
  int     num_chars = 0;
  char*   err_buffer;
  UInt32  msg_int_state;
  
  /* disable interrupts */
  MSG_INT_DISABLE();

  /* allocate temporary buffer (must be dynamic to avoid problems in the     */
  /* hook function concerning re-entrancy)                                   */
  if (msg_class == MSG_MC_ERROR)
  {
    err_buffer = (char *)malloc(MSG_TEMP_BUFFER_SIZE);

    if (!err_buffer)
    {
      msg_error_set(submodule, no, "msg_printf(): Out of memory, can't display message.");

      MSG_INT_RESTORE();
      return 0;
    }
  } else
    err_buffer = msg_temp_buffer;

/* the timestamp functionality is only available on PPC Boards and DS1006 */
#if defined _DS1103 || defined _DS1005 || defined _DS1006 || defined _DS1401 || defined _DS1104
  { 
    int     hours;
    int     minutes;
    int     count = 0;
    double  seconds;

    if (msg_timestamp_enable)
    {
      seconds = ts_time_read();
      hours = seconds / 3600;  
      seconds -= (hours*3600);
      minutes = seconds / 60;  
      seconds -= (minutes*60);
      
      count = sprintf(err_buffer,"%02d:%02d:%0*.*f%s",hours,minutes,msg_timestamp_digits+(msg_timestamp_digits?3:2),msg_timestamp_digits,seconds,msg_timestamp_separator);
    }

    /* print argument list in the string buffer */
    num_chars = vsprintf(&err_buffer[count], format, args_list);
  }
#else

  /* print argument list in the string buffer */
  num_chars = vsprintf(err_buffer, format, args_list);
#endif

  /* set the error message */
  msg_set(msg_class, msg_dialog, submodule, no, err_buffer);

  /* free error buffer memory */
  if (msg_class == MSG_MC_ERROR)
    free(err_buffer);

  /* restore interrupts */
  MSG_INT_RESTORE();

  return num_chars;
}

/* ---- msg_printf --------------------------------------------------------- */

int msg_printf(msg_class_type     msg_class,
               msg_dialog_type    msg_dialog,
               msg_submodule_type submodule,
               msg_no_type        no,
               const char         *format,
               ...)
{
  va_list args_list;
  int num_chars;

  va_start(args_list, format);
  num_chars = msg_do_printf(msg_class, msg_dialog, submodule, no, format, args_list);
  va_end(args_list);

  return num_chars;
}

/* ---- msg_error_printf --------------------------------------------------- */

int msg_error_printf(msg_submodule_type submodule,
                     msg_no_type        no,
                     const char         *format,
                     ...)
{
  va_list args_list;
  int num_chars;

  va_start(args_list, format);
  num_chars = msg_do_printf(MSG_MC_ERROR, MSG_DLG_DEFAULT, submodule, no, format, args_list);
  va_end(args_list);

  return num_chars;
}

/* ---- msg_info_printf ---------------------------------------------------- */

int msg_info_printf(msg_submodule_type submodule,
                    msg_no_type        no,
                    const char         *format,
                    ...)
{
  va_list args_list;
  int num_chars;

  va_start(args_list, format);
  num_chars = msg_do_printf(MSG_MC_INFO, MSG_DLG_DEFAULT, submodule, no, format, args_list);
  va_end(args_list);

  return num_chars;
}

/* ---- msg_warning_printf ------------------------------------------------- */

int msg_warning_printf(msg_submodule_type submodule,
                       msg_no_type        no,
                       const char         *format,
                       ...)
{
  va_list args_list;
  int num_chars;

  va_start(args_list, format);
  num_chars = msg_do_printf(MSG_MC_WARNING, MSG_DLG_DEFAULT, submodule, no, format, args_list);
  va_end(args_list);

  return num_chars;
}

/* ------------------------------------------------------------------------- */

#undef __INLINE
