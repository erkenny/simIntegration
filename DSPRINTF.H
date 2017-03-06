/******************************************************************************
  FILE:
    DSPRINTF.H

  DESCRIPTION:
    Header for Sprintf Module

  REMARKS:

  AUTHORS:  R. Leinfellner, Markus Gunske

  Copyright (c) 1998 dSPACE GmbH, GERMANY

  $RCSfile: DSPRINTF.H $ $Revision: 1.5 $ $Date: 2008/07/28 14:26:39GMT+01:00 $
******************************************************************************/

#ifndef __DSPRINTF_H__
#define __DSPRINTF_H__


#include <stdarg.h>
#include <stdlib.h>

/* write formatted data to a string using an argument list */ 
int vsprintf(char *buf, const char *fmt, va_list args);

/* write formatted data to a string using an argument list, limited by the 
size of the destination*/
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

/* write formatted data to a string using the variable arguments */
int sprintf(char * buf, const char *fmt, ...);

/* write formatted data to a string using the variable arguments, limited by the 
size of the destination*/
#ifndef _DS1006
int snprintf(char* buf, size_t size, const char* fmt, ...);
#else
/* 
 * GCC attribute for printf-style argument checking via -Wformat 
 */
int snprintf(char* buf, size_t size, const char* fmt, ...) __attribute__ ((format (printf, 3, 4))); 
#endif

#endif
