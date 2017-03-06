/******************************************************************************
*
* FILE:
*   PpcExc.h
*
* RELATED FILES:
*   dstypes.h
*
* DESCRIPTION:
*   arithmetical exception support for PPC
*
* HISTORY:
*   20.03.98  Rev 1.0  H.-G. Limberg,  initial version for DS1103
*   22.06.98  Rev 1.1  R. Busche,   common version for PPC (DS1103 and DS1401)
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: PpcExc.h $ $Revision: 1.4 $ $Date: 2004/09/30 12:00:12GMT+01:00 $
*
******************************************************************************/
#ifndef _PPCEXC_H_
#define _PPCEXC_H_

/* $DSVERSION: PPCEXC - PPC Exception Handling */
#define PPCEXC_VER_MAR 1
#define PPCEXC_VER_MIR 1
#define PPCEXC_VER_MAI 0
#define PPCEXC_VER_SPB VCM_VERSION_RELEASE
#define PPCEXC_VER_SPN 0
#define PPCEXC_VER_PLV 0

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE extern
#endif

#include <dstypes.h>

#define PPC_NUMBER_OF_EXCEPTIONS 12

/* ExcID numbers*/

#define PPC_EXC_DZE  0    /* division by zero ZX 5*/
#define PPC_EXC_FOV  1    /* fp overflow OX 3 */
#define PPC_EXC_UNF  2    /* underflow UX 4*/
#define PPC_EXC_INE  3    /* inexact result XX 6*/
                             /* invalid operations: */
#define PPC_EXC_SNAN 4    /* invalid operation: signaling not a number */
#define PPC_EXC_ISI  5    /* invalid operation: infinity substract infinity */
#define PPC_EXC_IDI  6    /* invalid operation: infinity divide infinity */
#define PPC_EXC_ZDZ  7    /* invalid operation: zero divide zero */
#define PPC_EXC_IMZ  8    /* invalid operation: infinity multiply zero */
#define PPC_EXC_VC   9    /* invalid operation: invalid compare */
#define PPC_EXC_SQRT 10   /* invalid operation: square root exception */
#define PPC_EXC_ICON 11   /* invalid operation: invalid integer convert */

/* exception modes */

#define PPC_EM_STOP      0x00000001  /* stop execution after exception handling */
#define PPC_EM_CHKERR    0x00000002  /* write information for CHKERR utility to global mem */
#define PPC_EM_DUMP      0x00000004  /* write full register dump to global mem */
#define PPC_EM_LAST_EXC  0x00000008  /* report the last occured exception instead of the
                                           first (default) */

/******************************************************************************
  type definitions
******************************************************************************/

#define PPC_EXC_MAX_FRAMES  32  

typedef struct {
    UInt32              exception_reason;
    struct SavedRegs    *reg_dump_ptr;
    UInt32              stack_start;
    UInt32              stack_end;
    UInt32              text_sect_start;
    UInt32              text_sect_end;
    UInt32              heap_start;
    UInt32              heap_end;
    UInt32              *additional_info;
    UInt32              num_stack_frames;
    UInt32              frame_pointer[PPC_EXC_MAX_FRAMES];
    UInt32              frame_info[PPC_EXC_MAX_FRAMES];
    UInt32              frame_return_addr[PPC_EXC_MAX_FRAMES];
    UInt32              reserved[24]; 
} exception_info_struct;


struct SavedRegs
{
  UInt32 integer[32];
  UInt64 fp[32];
  UInt32* return_addr;
};


/******************************************************************************
  function prototypes
******************************************************************************/
typedef void (*Ppc_Exc_Handler_Type)(UInt32 ExcID,
                                        UInt32 *ExcAddr,
                                        UInt32 Counter,
                                        struct SavedRegs *Regs);


/******************************************************************************
*
* FUNCTION:
*   int ppc_exception_init(void)
*
* DESCRIPTION:
*   initializes the PPC exception handling module
*
* PARAMETERS:
*   none
*
* RETURNS:
*   none
*
* NOTE:
*   0       success
*   else    error
*
* HISTORY:
*   15.08.2004  Ralf G.B., initial version
*
******************************************************************************/
__INLINE int ppc_exception_init(void);



/******************************************************************************
*
* FUNCTION:
*   void ppc_exception_enable(UInt32 ExcID)
*
* DESCRIPTION:
*   clears the specified arithmetical exception bit in the mask register
*
* PARAMETERS:
*   ExcID : ID number of the exception, defined in Ppc_Exc.h
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   20.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_exception_enable(UInt32 ExcID);

/******************************************************************************
*
* FUNCTION:
*   void ppc_exception_disable(UInt32 ExcID)
*
* DESCRIPTION:
*   sets the specified arithmetical exception bit in the mask register
*
* PARAMETERS:
*   ExcID : ID number of the exception, defined in Ppc_Exc.h
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   20.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_exception_disable(UInt32 ExcID);

/******************************************************************************
*
* FUNCTION:
*   void ppc_global_exception_enable(void)
*
* DESCRIPTION:
*   enables all arithmetical exceptions which mask bit is cleared
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
*   20.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_global_exception_enable(void);

/******************************************************************************
*
* FUNCTION:
*   void ppc_global_exception_disable(void)
*
* DESCRIPTION:
*   disables all arithmetical exceptions
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
*   20.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_global_exception_disable(void);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_mode_set(UInt32 ExcID, UInt32 ExcMode)
*
* DESCRIPTION:
*   set the exception mode for the specified exception
*
* PARAMETERS:
*   ExcID   : specifies the exception for which the mode will be set
*   ExcMode : exception mode
*
* RETURNS:
*   the old exception mode of the specified exception
*
* NOTE:
*
* HISTORY:
*   23.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_mode_set(UInt32 ExcID, UInt32 ExcMode);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_mode_get(UInt32 ExcID, UInt32 ExcMode)
*
* DESCRIPTION:
*   set the exception mode for the specified exception
*
* PARAMETERS:
*   ExcID   : specifies the exception for which the mode will be set
*
* RETURNS:
*   the exception mode of the specified exception
*
* NOTE:
*
* HISTORY:
*   23.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_mode_get(UInt32 ExcID);

/******************************************************************************
*
* FUNCTION:
*   Ppc_Exc_Handler_Type ppc_exception_handler_set(UInt32 ExcID,
*                                                        Ppc_Exc_Handler_Type Handler,
*                                                        UInt32 ExcMode)
*
* DESCRIPTION:
*   Installs an handler for the specified exception, sets its mode and return the address of
*   the old handler. The handler can be deinstall by passing a zero as second argument.
*
* PARAMETERS:
*   ExcID   : specifies the exception for which the handler will be installed
*   Handler : address of the new handler
*   ExcMode : mode will be set for the specified exception
*
* RETURNS:
*   address of the old handler
*
* NOTE:
*
* HISTORY:
*   23.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE Ppc_Exc_Handler_Type ppc_exception_handler_set(UInt32 ExcID,
                                                     Ppc_Exc_Handler_Type Handler,
                                                     UInt32 ExcMode);

/******************************************************************************
*
* FUNCTION:
*   void ppc_all_exception_handlers_set(Ppc_Exc_Handler_Type Handler,
*                                         UInt32 ExcMode)
*
* DESCRIPTION:
*   Installs the same handler for all exceptions.
*   To deinstall all handlers pass a zero as second argument.
*
* PARAMETERS:
*   Handler : address of the new handler
*   ExcMode : mode will be set for the specified exception
*
* RETURNS:
*   none
*
* NOTE:
*
* HISTORY:
*   23.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ppc_all_exception_handlers_set(Ppc_Exc_Handler_Type Handler,
                                       UInt32 ExcMode);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_counter_get(UInt32 ExcID)
*
* DESCRIPTION:
*   returns the internal counter value for the specified exception
*
* PARAMETERS:
*   ExcID   : specifies the exception
*
* RETURNS:
*   internal counter value for the specified exception
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_counter_get(UInt32 ExcID);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_counter_reset(UInt32 ExcID)
*
* DESCRIPTION:
*   returns the internal and external counter value for the specified exception
*   and resets the counter values
*
* PARAMETERS:
*   ExcID   : specifies the exception
*
* RETURNS:
*   internal counter value for the specified exception before reset
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_counter_reset(UInt32 ExcID);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_total_exception_count_get(void)
*
* DESCRIPTION:
*   returns the total sum of all internal exception counters
*
* PARAMETERS:
*   none
*
* RETURNS:
*   total count of all arithmetical exceptions
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_total_exception_count_get(void);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_flag_get(void)
*
* DESCRIPTION:
*   returns the exception flag from the config section
*
* PARAMETERS:
*   none
*
* RETURNS:
*  zero if no exception happens
*  7  if an arithmetic fp exception happens
*
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_flag_get(void);

/******************************************************************************
*
* FUNCTION:
*   UInt32 ppc_exception_flag_reset(void)
*
* DESCRIPTION:
*   returns the exception flag and reset the flag
*
* PARAMETERS:
*   none
*
* RETURNS:
*  zero if no exception happens
*  7  if an arithmetic fp exception happens
*
*
* NOTE:
*
* HISTORY:
*   25.03.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE UInt32 ppc_exception_flag_reset(void);



#ifdef _INLINE
  #include <ppcexc.c>
#endif


#undef __INLINE

#endif      /* _PPCEXC_H_ */
