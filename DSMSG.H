/******************************************************************************
  FILE:
    DSMSG.H

  DESCRIPTION:
    Message Module: Board Independent Header

  REMARKS:

  AUTHORS:  V. Lang, R. Leinfellner

  Copyright (c) 1998 - 2000 dSPACE GmbH, GERMANY

 $RCSfile: DSMSG.H $ $Revision: 1.16 $ $Date: 2009/06/24 10:28:51GMT+01:00 $
******************************************************************************/


#ifndef __DSMSG_H__
#define __DSMSG_H__

#include <stdarg.h>
#include <dstypes.h>

/* ---- standard error messages -------------------------------------------- */

#define MSG_NO_ERROR                 0

#define MSG_UNINITIALIZED            11
#define MSG_UNINITIALIZED_TXT        "Initialization failure."

#define MSG_OVERRUN                  12
#define MSG_OVERRUN_TXT              "Task Overrun: Program cannot be executed in real-time."

#define MSG_TERMINATED               14
#define MSG_TERMINATED_TXT           "Application terminated."

#define MSG_MEM_ALLOC_ERROR          15
#define MSG_MEM_ALLOC_ERROR_TXT      "Memory allocation error."

#define MSG_INV_DEF_DIALOG_ERROR     16
#define MSG_INV_DEF_DIALOG_ERROR_TXT "msg_default_dialog_set(): Invalid default dialog specification!"

#define MSG_INV_DEF_CLASS_ERROR      17
#define MSG_INV_DEF_CLASS_ERROR_TXT  "msg_default_dialog_set(): Invalid message class specification!"

#define MSG_INV_DIALOG_ERROR         18
#define MSG_INV_DIALOG_ERROR_TXT     "Message Module: Invalid dialog specification!"

#define MSG_INV_CLASS_ERROR          19
#define MSG_INV_CLASS_ERROR_TXT      "Message Module: Invalid message class specification!"

#define MSG_STACK_OVERFLOW_ERROR     20
#define MSG_STACK_OVERFLOW_ERROR_TXT "Stack overflow!"

#define MSG_STACK_CTRL_OVERFLOW_ERROR     21
#define MSG_STACK_CTRL_OVERFLOW_ERROR_TXT "Stack overflow at address 0x%08lX!"

#define MSG_STACK_CTRL_UNDERFLOW_ERROR     22
#define MSG_STACK_CTRL_UNDERFLOW_ERROR_TXT "Stack underflow detected at address 0x%08lX!"



/* ---- default dialog modes ----------------------------------------------- */

#define MSG_DEFDM_ERROR       DSERR_DLG_OKCAN
#define MSG_DEFDM_INFO        DSERR_DLG_NOBOX
#define MSG_DEFDM_WARNING     DSERR_DLG_NOBOX

/* ---- submodule identifier ----------------------------------------------- */

#define MSG_SM_NONE           0     /* No Module                             */
#define MSG_SM_RTI            1     /* RTI                                   */
#define MSG_SM_RTIMP          2     /* RTI-MP                                */
#define MSG_SM_TEXTIO         3     /* TEXTIO                                */
#define MSG_SM_CODELINK       4     /* CodeLink, Autocode Interface          */
#define MSG_SM_RTLIB          5     /* Real-Time Board Library               */
#define MSG_SM_MC40LIB        6     /* Multi C40 Library                     */
#define MSG_SM_TA_STIMUL      7     /* Testautomation Signalgeneration       */
#define MSG_SM_TA_PROTOCOL    8     /* Testautomation Protocol               */
#define MSG_SM_DS1103SLVLIB   9     /* DS1103 Slave DSP Library              */
#define MSG_SM_DS1103MCLIB    10    /* RTLIB: DS1103 CAN                     */
#define MSG_SM_FFTACC         11    /* FFT Accelerator                       */
#define MSG_SM_CAN1103        12    /* RTI: DS1103 CAN Blockset              */
#define MSG_SM_CAN4301        13    /* RTLIB: DS4301 CAN                     */
#define MSG_SM_CAN4302        14    /* RTLIB: DS4302 CAN                     */
#define MSG_SM_DIO1401        15    /* RTLIB: DS1401 Digital IO              */
#define MSG_SM_CAN1401        16    /* RTLIB: DS1401 CAN                     */
#define MSG_SM_RTI_DS4301CAN  17    /* RTI: DS4301 CAN Blockset              */
#define MSG_SM_RTICAN4302     18    /* RTI: RTICAN Blockset (DS4302)         */
#define MSG_SM_RTICAN1103     19    /* RTI: RTICAN Blockset (DS1103)         */
#define MSG_SM_RTICAN1401     21    /* RTI: RTICAN Blockset (DS1401)         */
#define MSG_SM_ECU1401        22    /* RTLIB: DS1401 ECU Interface           */
#define MSG_SM_RTKERNEL       23    /* RTKernel                              */
#define MSG_SM_REALMOTION     24    /* Real Motion                           */
#define MSG_SM_RTICAN2210     25    /* RTI: RTICAN Blockset (DS2210)         */
#define MSG_SM_DS1104SLVLIB   26    /* DS1104 Slave DSP Library              */
#define MSG_SM_ECU_POD        27    /* ECU PODs (DS5xx)                      */
#define MSG_SM_LIN            28    /* RTLIB: LIN                            */
#define MSG_SM_DSSER          29    /* RTLIB: DSER                           */
#define MSG_SM_DSFR           30    /* RTLIB: FlexRay                        */ 
#define MSG_SM_DSMBX          31    /* RTLIB: MicroBox                       */
#define MSG_SM_DS4501         32    /* RTLib: DS4501                         */
#define MSG_SM_DS4502         33    /* RTLib: DS4502                         */
#define MSG_SM_RTIFLEXRAY     34    /* RTI: RTI FlexRay Blockset             */
#define MSG_SM_CAN2210        35    /* RTLIB: DS2210 CAN                     */
#define MSG_SM_CAN2211        36    /* RTLIB: DS2211 CAN                     */
#define MSG_SM_RTICAN2211     37    /* RTI: RTICAN Blockset (DS2211)         */
#define MSG_SM_DSBYPASS       38    /* RTI: DSBypass Blockset                */
#define MSG_SM_CAN2202        39    /* RTLIB: DS2202 CAN                     */
#define MSG_SM_RTICAN2202     40    /* RTI: RTICAN Blockset (DS2202)         */
#define MSG_SM_RTOSAL         41    /* RTOS Abstractionlayer                 */
#define MSG_SM_SIMENG         42    /* RTI: Simengine                        */
#define MSG_SM_RTIFLEXRAYCONFIG 43  /* RTI: RTI FlexRay Configuration Blockset             */
#define MSG_SM_RTPYTHON       44    /* RTPythoninterpreter                   */
#define MSG_SM_DSCAN          45    /* RTLIB: DsCAN                          */
#define MSG_SM_DSJ1939        46    /* RTLIB: DsJ1939                        */

#define MSG_SM_HOSTSERV       MSG_SM_RTLIB /* Host Services                  */

#define MSG_SM_USER           2047  /* User Messages                         */

/* ---- fundamental typedefs ----------------------------------------------- */

typedef Int32 msg_no_type;
typedef UInt32 msg_submodule_type;
typedef int (*msg_hookfcn_type)(msg_submodule_type, msg_no_type);
typedef UInt32 DsTError;

/* ---- board specific includes -------------------------------------------- */

#if defined(_DS1003)
  #include <msg1003.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif (defined (_DS1004) || defined(__alpha__))
  #include <msg1004.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif defined (_DS1005)
  #include <msg1005.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1006)
  #include <msg1006.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif defined (_DS1102)
  #include <msg1102.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif defined (_DS1103)
  #include <msg1103.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
 #elif defined (_DS1104)
  #include <msg1104.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1401)
  #include <msg1401.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS230x)
  #include <msg230x.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1602)
  #include <DS1602Msg.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1603)
  #include <DS1603Msg.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DSHOST)
  #define __INLINE
  #define msg_string_type char
  #define MSG_STRING_LENGTH 1
  #define MSG_BUFFER_LENGTH 1024
#else
  #error dsmsg.h: no processor board defined (e.g. _DS1003)
#endif

/* ---- shared definitions for SCOUT and board libraries -------------- */

#include <dsmsgdef.h>

/************************************************************************
  definitions for message handling on processor boards
************************************************************************/

#define MSG_MC_VERSION     1                 /* message module version */
#define MSG_MC_DLL         0                 /* string source          */

#define MSG_BLOCKING       0     /* message buffer stores first x msg. */
#define MSG_OVERWRITE      1     /* message buffer works in loop mode  */

/* message length extended to the next UInt32 boundary */
#define MSG_LENGTH      (((((UInt32)(MSG_STRING_LENGTH + 1 + (sizeof(UInt32)-1))) &          \
        ((UInt32)(~(sizeof(UInt32) - 1))))))

#define MSG_TEMP_BUFFER_SIZE 1024

/************************************************************************
  error descriptor type definition
************************************************************************/

typedef enum
{
  MSG_MC_LOG =     DSERR_CLASS_LOG,
  MSG_MC_INFO =    DSERR_CLASS_TEXT,
  MSG_MC_WARNING = DSERR_CLASS_WARN,
  MSG_MC_ERROR =   DSERR_CLASS_ERROR
} msg_class_type;

typedef enum
{
  MSG_DLG_NONE =     DSERR_DLG_NOBOX,
  MSG_DLG_OKCANCEL = DSERR_DLG_OKCAN,
  MSG_DLG_DEFAULT =  0xFFFF
} msg_dialog_type;

/* ---- msg_buffer_entry_type ---- */

#define MSG_ENTRY_COUNT_1 0x0000
#define MSG_ENTRY_CODE_H  0x0004
#define MSG_ENTRY_CODE_L  0x0008
#define MSG_ENTRY_COUNT_2 0x000C
#define MSG_ENTRY_MESSAGE 0x0010

typedef struct
{
  UInt32          count_1;
  UInt32          code_h;
  UInt32          code_l;
  UInt32          count_2;
  msg_string_type message[MSG_LENGTH];
} msg_buffer_entry_type;

/* ---- msg_timestamp_constants and variables ----*/

/* the timestamp functionality is only on PPC Boards available
   in sense of compatibility these defines are on each platform available*/
#define MSG_TIMESTAMP_ON 1
#define MSG_TIMESTAMP_OFF 0

/* ---- msg_descriptor_type ----*/

#define MSG_DESC_BUFFER_MODE    0x0000
#define MSG_DESC_BUFFER_LENGTH  0x0004
#define MSG_DESC_MSG_LENGTH     0x0008
#define MSG_DESC_MSG_COUNTER    0x000C
#define MSG_DESC_MSG_BUFFER     0x0010

typedef struct
{
  UInt32                msg_buffer_mode;
  UInt32                msg_buffer_length;
  UInt32                msg_length;
  UInt32                msg_counter;
  msg_buffer_entry_type msg_buffer[MSG_BUFFER_LENGTH];
} msg_descriptor_type;


extern msg_descriptor_type *msg_descriptor;
extern volatile UInt32 msg_temp;
extern char * msg_temp_buffer;

/* the timestamp functionality is only available on PPC Boards and DS1006 */
#if defined _DS1103 || defined _DS1005 || defined _DS1006 || defined _DS1401 || defined _DS1104
extern volatile int msg_timestamp_enable;
extern volatile int msg_timestamp_digits;
extern volatile char msg_timestamp_separator[];
#endif

/************************************************************************
  function prototypes
************************************************************************/

/* init message module */
void msg_init(void);

/* signal an error */
void msg_error_set(msg_submodule_type submodule, msg_no_type no,
   const char *msg);

/* signal an info */
void msg_info_set(msg_submodule_type submodule, msg_no_type no,
   const char *msg);

/* signal a warning */
void msg_warning_set(msg_submodule_type submodule, msg_no_type no,
   const char *msg);

/* return the last error number */
__INLINE msg_no_type msg_last_error_number();

/* return the last error submodule */
__INLINE msg_submodule_type msg_last_error_submodule();

/* install an error-hook function */
__INLINE void msg_error_hook_set(msg_hookfcn_type err_hook);

/* set the mode of the message-buffer */
__INLINE void msg_mode_set(UInt32 mode);

/* reset the message-buffer */
__INLINE void msg_reset();

/* clear last error code and submodule */
__INLINE void msg_error_clear();

/* message set */
void msg_set(msg_class_type     msg_class,
             msg_dialog_type    msg_dialog,
             msg_submodule_type submodule,
             msg_no_type        msg_no,
             const char         *msg);



/* message printf */
int msg_printf(msg_class_type     msg_class,
               msg_dialog_type    msg_dialog,
               msg_submodule_type submodule,
               msg_no_type        no,
               const char         *format, ...);

/* write an error message using the printf format */
int msg_error_printf(msg_submodule_type submodule, msg_no_type no,
                     const char *format, ...);

/* write an info message using the printf format */
int msg_info_printf(msg_submodule_type submodule, msg_no_type no,
                    const char *format, ...);

/* write a warning message using the printf format */
int msg_warning_printf(msg_submodule_type submodule, msg_no_type no,
                       const char *format, ...);

/* change the default dialog of a message class */
void msg_default_dialog_set(msg_class_type msg_class, msg_dialog_type msg_dialog);

#ifdef _DS1602
__INLINE  UInt32  msg_buffer_get_next_entry(msg_buffer_entry_type **buffer);
__INLINE  UInt32  msg_buffer_quit_next_entry();
__INLINE  UInt32  msg_buffer_get_no_of_entries();
#endif

#ifdef _DS1603
__INLINE  UInt32  msg_buffer_get_next_entry(msg_buffer_entry_type **buffer);
__INLINE  UInt32  msg_buffer_quit_next_entry();
__INLINE  UInt32  msg_buffer_get_no_of_entries();
#endif

#undef __INLINE

/* ---- Inline function include -------------------------------------------- */

#ifdef _INLINE
  #include <dsmsg.c>
#endif

/* ------------------------------------------------------------------------- */


#endif /* __DSMSG_H__ */
