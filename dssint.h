/******************************************************************************
* MODULE
*   universal sub interrupt module for master / slave interrupt handling
*
* FILE:
*   dssint.c
*
* RELATED FILES:
*   dpmxxxx.h (special header file for every application)
*
* DESCRIPTION:
*   sub interrupt handling between two processors
*
* AUTHOR(S)
*   H.-G. Limberg, V. Lang
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*   $RCSfile: dssint.h $ $Revision: 1.6 $ $Date: 2006/05/08 15:58:13GMT+01:00 $
******************************************************************************/

#ifndef __DSSINT_H__
#define __DSSINT_H__

#if defined (_DSECU)
  #include <dpmECU.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#else
  #if defined (_DS1103)
    #include <dpm1103.h>
    #include <dsvcm.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1103_SLAVE_MC)
    #include <dpmslv.h>
    #ifdef _INLINE
      #define __INLINE _inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1103_SLAVE_DSP)
    #include <dpmslv.h>
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1104)
    #include <dpm1104.h>
    #include <dsvcm.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1104_SLAVE_DSP)
    #include <dpmslv.h>
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1005)
    #include <dsvcm.h>
    #include <dpm1005.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1006)
    #include <dsvcm.h>
    #include <dpm1006.h>
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1003)
    #include <dsvcm.h>
    #include <dpm1003.h>
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS4302)
    #include <dpmslv.h>
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1401)
    #include <dsvcm.h>
    #include <dpm1401.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1401_DIO_TP1)
    #include <DpmDio.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1603)
    #include <ds1603dpm.h>
    #include <dsstd.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_RPCU)
    #include <dsDpmsmc.h>
    #include <dsstd.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #else
    #error ERROR: No target defined.
  #endif
#endif

/* ---- DSSINT version -------------------------------------------------- */

/* $DSVERSION: DSSINT - Subinterrupt Module */

#define DSSINT_VER_MAR 1
#define DSSINT_VER_MIR 2
#define DSSINT_VER_MAI 0
#define DSSINT_VER_SPB VCM_VERSION_RELEASE
#define DSSINT_VER_SPN 0
#define DSSINT_VER_PLV 0

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

#define dssint_vcm_register()    /* use this macro only if the dsvcm module is present */    \
{                                                                                            \
  vcm_module_descriptor_type *dssint_version_ptr;                                            \
                                                                                             \
                                                                                             \
                                                                                             \
  dssint_version_ptr = vcm_module_register(VCM_MID_DSSINT,                                   \
                                           vcm_module_find(VCM_MID_RTLIB, NULL),             \
                                           VCM_TXT_DSSINT,                                   \
                                           DSSINT_VER_MAR, DSSINT_VER_MIR, DSSINT_VER_MAI,   \
                                           DSSINT_VER_SPB, DSSINT_VER_SPN, DSSINT_VER_PLV,   \
                                           0, 0);                                            \
                                                                                             \
  vcm_module_status_set(dssint_version_ptr,VCM_STATUS_INITIALIZED);                          \
}


#define SINT_NO_SUBINT -1       /* return value of decode function for no pending subinterrupt */

#define SINT_INIT_OLD   0
#define SINT_INIT_NEW   1

/* defines for old initialization mode for compatibility with old implementations */
#define dssint_define_int_sender(nr_subinterrupts, subint_addr, ack_addr, sender_addr, target,   \
                                 sint_mem_width, write_fcn, read_fcn)                            \
        dssint_define_int_sender_fcn(nr_subinterrupts, subint_addr, ack_addr, sender_addr,   \
                                         target, sint_mem_width, write_fcn, read_fcn, SINT_INIT_OLD)

#define dssint_define_int_receiver(nr_subinterrupts, subint_addr, ack_addr, receiver_addr, target,  \
                                   sint_mem_width, write_fcn, read_fcn)                             \
        dssint_define_int_receiver_fcn(nr_subinterrupts, subint_addr, ack_addr, receiver_addr,  \
                                           target, sint_mem_width, write_fcn, read_fcn, SINT_INIT_OLD)


/* defines for new initialization mode */
#define dssint_define_int_sender_1(nr_subinterrupts, subint_addr, ack_addr, sender_addr, target,   \
                                 sint_mem_width, write_fcn, read_fcn)                            \
        dssint_define_int_sender_fcn(nr_subinterrupts, subint_addr, ack_addr, sender_addr,   \
                                         target, sint_mem_width, write_fcn, read_fcn, SINT_INIT_NEW)

#define dssint_define_int_receiver_1(nr_subinterrupts, subint_addr, ack_addr, receiver_addr, target,  \
                                   sint_mem_width, write_fcn, read_fcn)                             \
        dssint_define_int_receiver_fcn(nr_subinterrupts, subint_addr, ack_addr, receiver_addr,  \
                                           target, sint_mem_width, write_fcn, read_fcn, SINT_INIT_NEW)



typedef struct{
  unsigned int     nr_sint;
  unsigned long    sint_addr;
  unsigned long    ack_addr;
  unsigned long    sender_addr;
  unsigned int     nr_words;
  unsigned long*   request;
  long             target;
  unsigned int     sint_mem_width;
  dpm_write_fcn_t  write_fcn;
  dpm_read_fcn_t   read_fcn;
  unsigned int     sint_mem_shift;
}dssint_sender_type;

typedef struct{
  unsigned int     nr_sint;
  unsigned long    sint_addr;
  unsigned long    ack_addr;
  unsigned long    receiver_addr;
  unsigned int     nr_words;
  unsigned long*   acknowledge;
  unsigned long*   state;
  long             target;
  unsigned int     sint_mem_width;
  unsigned int     state_position;
  dpm_write_fcn_t  write_fcn;
  dpm_read_fcn_t   read_fcn;
  unsigned int     sint_mem_shift;
  unsigned long*   enable_flags;
}dssint_receiver_type;


/******************************************************************************
  function prototypes
******************************************************************************/


dssint_sender_type* dssint_define_int_sender_fcn(unsigned int    nr_subinterrupts,
                                    unsigned long   subint_addr,
                                    unsigned long   ack_addr,
                                    unsigned long   sender_addr,
                                    long            target,
                                    unsigned int    sint_mem_width,
                                    dpm_write_fcn_t write_fcn,
                                    dpm_read_fcn_t  read_fcn,
                                    unsigned int    init_mode);

__INLINE void dssint_interrupt     (dssint_sender_type  *sender, 
                                    unsigned int sub_interrupt);

__INLINE void dssint_atomic_interrupt(dssint_sender_type *sender,
                                    unsigned int sub_interrupt);

dssint_receiver_type *dssint_define_int_receiver_fcn(unsigned int nr_subinterrupts,
                                    unsigned long   subint_addr,
                                    unsigned long   ack_addr,
                                    unsigned long   receiver_addr,
                                    long            target,
                                    unsigned int    sint_mem_width,
                                    dpm_write_fcn_t write_fcn,
                                    dpm_read_fcn_t  read_fcn,
                                    unsigned int    init_mode);

__INLINE void dssint_acknowledge   (dssint_receiver_type *receiver);

__INLINE int dssint_decode         (dssint_receiver_type *receiver);

__INLINE void dssint_subint_enable (dssint_receiver_type *receiver,
                                    unsigned int sub_interrupt);
__INLINE void dssint_subint_disable(dssint_receiver_type *receiver,
                                    unsigned int sub_interrupt);
__INLINE void dssint_subint_reset  (dssint_receiver_type *receiver,
                                    unsigned int sub_interrupt);




#undef __INLINE
/* ---- Inline function include -------------------------------------------- */

#ifdef _INLINE
  #include <dssint.c>
#endif

/* ------------------------------------------------------------------------- */


#endif

