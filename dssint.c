/******************************************************************************
* MODULE
*   universal sub interrupt module for master / slave interrupt handling
*
* FILE:
*   dssint.c
*
* RELATED FILES:
*   dssint.h
*   special header file for every application
*
* DESCRIPTION:
*   sub interrupt handling between two processors
*
* AUTHOR(S)
*   H.-G. Limberg, V. Lang
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*   $RCSfile: dssint.c $ $Revision: 1.5 $ $Date: 2009/02/13 16:06:44GMT+01:00 $
******************************************************************************/
#include <stdlib.h>
#include <dssint.h>

#define BITS_PER_BYTE 8

#if defined(_DSECU)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#else
  #if defined (_DS1103)
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1103_SLAVE_MC)
    #ifdef _INLINE
      #define __INLINE _inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1103_SLAVE_DSP)
    #undef BITS_PER_BYTE
    #define BITS_PER_BYTE 16
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1104)
    #include <int1104.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1104_SLAVE_DSP)
    #undef BITS_PER_BYTE
    #define BITS_PER_BYTE 16
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1005)
    #include <dpm1005.h>
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1006)
    #include <dpm1006.h>
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1003)
    #undef BITS_PER_BYTE
    #define BITS_PER_BYTE 32
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS4302)
    #undef  BITS_PER_BYTE
    #define BITS_PER_BYTE 32
    #ifdef _INLINE
      #define __INLINE static inline
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1401)
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_DS1401_DIO_TP1)
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #elif defined (_RPCU)
    #ifdef _INLINE
      #define __INLINE static
    #else
      #define __INLINE
    #endif
  #else
    #error ERROR: No target defined.
  #endif
#endif

/******************************************************************************
  version information
******************************************************************************/

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 || defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401

DS_VERSION_SYMBOL(_dssint_ver_mar, DSSINT_VER_MAR);
DS_VERSION_SYMBOL(_dssint_ver_mir, DSSINT_VER_MIR);
DS_VERSION_SYMBOL(_dssint_ver_mai, DSSINT_VER_MAI);
DS_VERSION_SYMBOL(_dssint_ver_spb, DSSINT_VER_SPB);
DS_VERSION_SYMBOL(_dssint_ver_spn, DSSINT_VER_SPN);
DS_VERSION_SYMBOL(_dssint_ver_plv, DSSINT_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/******************************************************************************
  module global variables
******************************************************************************/
  
/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

/******************************************************************************
  object declarations
******************************************************************************/

/******************************************************************************
  function prototypes
******************************************************************************/

/******************************************************************************
*
* FUNCTION:
*    dssint_sender_type* dssint_define_int_sender_fcn(unsigned int    nr_subinterrupts,
*                                    unsigned long   subint_addr,
*                                    unsigned long   ack_addr,
*                                    unsigned long   sender_addr,
*                                    long            target,
*                                    unsigned int    sint_mem_width,
*                                    dpm_write_fcn_t write_fcn,
*                                    dpm_read_fcn_t  read_fcn,
*                                    unsigned int    init_mode)
*
* DESCRIPTION:
*   The function defines an interrupt sender and returns a handle to it.
*
* PARAMETERS:
*   nr_subinterrupts : number of different subinterrupts to be sent
*   subint_addr      : memory location where the subinterrupt information is passed
*   ack_addr         : memory location where the acknowledgement information is passed
*   sender_addr      : writing this memory location triggers the interrupt
*   target           : target number or address, set to zero for direct target access
*   sint_mem_width   : width of the dual port memory
*   write_fcn        : function to perform write accesses to dpm
*   read_fcn         : function to perform read accesses to dpm
*   init_mode        : kind of initialization : SINT_INIT_OLD or SINT_INIT_NEW
*
* RETURNS:
*   0(NULL) if an error occurs
*   handle to an interrupt sender else
*
* NOTE: init_mode = SINT_INIT_OLD is only allowed for compatibility reasons.
*       Use the macro dssint_define_int_sender than.
*
******************************************************************************/
#ifndef _INLINE
dssint_sender_type* dssint_define_int_sender_fcn(unsigned int    nr_subinterrupts,
                                    unsigned long   subint_addr,
                                    unsigned long   ack_addr,
                                    unsigned long   sender_addr,
                                    long            target,
                                    unsigned int    sint_mem_width,
                                    dpm_write_fcn_t write_fcn,
                                    dpm_read_fcn_t  read_fcn,
                                    unsigned int    init_mode)
{
   dssint_sender_type* sender;
   unsigned int i;
   unsigned long value;
   int bcnt=-1;
   unsigned int clear_sender_flag = 0;

   if ((sender = (dssint_sender_type*)malloc(sizeof(dssint_sender_type))) == NULL)
     return(NULL);

   sender->nr_sint        = nr_subinterrupts;
   sender->sint_addr      = subint_addr;
   sender->ack_addr       = ack_addr;
   sender->sender_addr    = sender_addr;
   sender->target         = target;
   sender->sint_mem_width = sint_mem_width;
   sender->write_fcn      = write_fcn;
   sender->read_fcn       = read_fcn;

   value = sint_mem_width ;
   while(value)
   {
      value = value >> 1;
      bcnt++;
   }
   sender->sint_mem_shift = bcnt;

   sender->nr_words =  (nr_subinterrupts + sint_mem_width - 1) / sint_mem_width;

   if ((sender->request = malloc(sizeof(long) * sender->nr_words)) == NULL)
     return(NULL);

   if (init_mode == SINT_INIT_NEW)
   {
     /* initialize sender */
     for (i = 0; i < sender->nr_words; i++)
     {
       /* initialize request (local copy) with value from dpm */
       *(sender->request + i) = DPM_READ(sender, sender->sint_addr, i);
     }
   }
   else
   {
     /* initialize sender */
     for (i = 0; i < sender->nr_words; i++)
     {
       *(sender->request + i) = 0;            /* initialize request (local copy) */

       DPM_WRITE(sender, sender->ack_addr, i, 0); /* initialize ack (in DPM) */

       /* initialize sint (in DPM), if sender part of sint, initialize at last  */
       if ( ((long)sender->sint_addr + i * sint_mem_width / BITS_PER_BYTE) != ((long)sender->sender_addr))
       {
         DPM_WRITE(sender, sender->sint_addr, i, 0);
       }
       else
         clear_sender_flag = 1;
     }

     if (clear_sender_flag)
       DPM_WRITE(sender, sender->sender_addr,0 , 0);
   }

   return(sender);
}
#endif

/******************************************************************************
*
* FUNCTION:
*   void dssint_interrupt(dssint_sender_type *sender,
*                       unsigned int sub_interrupt);
*
* DESCRIPTION:
*    The function writes the subinterrupt information to the appropriate memory location
*    and triggers the interrupt. The handle identifies where to send the interrupt.
*
* PARAMETERS:
*   sender        : handle of the sender
*   sub_interrupt : subinterrupt number to be triggered (>=1)
*
* RETURNS:
*   none
*
* NOTE:
*
******************************************************************************/
__INLINE void dssint_interrupt(dssint_sender_type *sender,
                             unsigned int sub_interrupt)
{
  unsigned long acknowledge, state, request, addr_offset;
  unsigned long bit_position;

  addr_offset = (sub_interrupt >> sender->sint_mem_shift );

  bit_position =  ((unsigned long)1) << (sub_interrupt & (sender->sint_mem_width -1 ));

  request = *(sender->request + addr_offset);

  acknowledge = DPM_READ(sender, sender->ack_addr, addr_offset);

  state = (request^acknowledge);
  if ((state & bit_position) == 0)
  {
    request = request^bit_position;     /* negate the request bit */

    DPM_WRITE(sender, sender->sint_addr, addr_offset, request);
    *(sender->request + addr_offset) = request;

    if (sender->sint_addr != sender->sender_addr)  /* write dummy to trigger interrupt */
      DPM_WRITE(sender, sender->sender_addr, 0, 0);
  }
}

/******************************************************************************
*
* FUNCTION:
*   void dssint_atomic_interrupt(dssint_sender_type *sender,
*                                unsigned int sub_interrupt);
*
* DESCRIPTION:
*    The function writes the subinterrupt information to the appropriate memory location
*    and triggers the interrupt. The handle identifies where to send the interrupt.
*    The difference to function dssint_interrupt() is that interrupts are globally
*    disabled for read-modify-write-access which is necessary to set the trigger 
*    bit. So it is avoided that an interrupt of an interrupting routine which 
*    even triggers an interrupt on the same sender will be overwritten
*
* PARAMETERS:
*   sender        : handle of the sender
*   sub_interrupt : subinterrupt number to be triggered (>=1)
*
* RETURNS:
*   none
*
* NOTE:
*
******************************************************************************/
#if defined _RPCU

__INLINE void dssint_atomic_interrupt(dssint_sender_type *sender,
                                      unsigned int sub_interrupt)
{
  unsigned long acknowledge, state, request, addr_offset;
  unsigned long bit_position;
  UInt32 SaveInt;

  addr_offset = (sub_interrupt >> sender->sint_mem_shift );

  bit_position =  ((unsigned long)1) << (sub_interrupt & (sender->sint_mem_width -1 ));

  RTLIB_INT_SAVE_AND_DISABLE(SaveInt);    

  request = *(sender->request + addr_offset);

  acknowledge = DPM_READ(sender, sender->ack_addr, addr_offset);

  state = (request^acknowledge);
  if ((state & bit_position) == 0)
  {
    request = request^bit_position;     /* negate the request bit */

    DPM_WRITE(sender, sender->sint_addr, addr_offset, request);
    *(sender->request + addr_offset) = request;

    RTLIB_INT_RESTORE(SaveInt);

    if (sender->sint_addr != sender->sender_addr)  /* write dummy to trigger interrupt */
      DPM_WRITE(sender, sender->sender_addr, 0, 0);
  }
  else{
    RTLIB_INT_RESTORE(SaveInt);
  }
}
#endif

#if (!(defined _DSECU) || (defined _DSECU_RTP_TO_ECU_SINT))

/******************************************************************************
*
* FUNCTION:
*    dssint_receiver_type *dssint_define_int_receiver_fcn(unsigned int        nr_subinterrupts,
*                                            unsigned long       subint_addr,
*                                            unsigned long       ack_addr,
*                                            unsigned long       receiver_addr,
*                                            long                target,
*                                            unsigned int        sint_mem_width,
*                                            dpm_write_fcn_t     write_fcn,
*                                            dpm_read_fcn_t      read_fcn,
*                                            unsigned int        init_mode)
*
* DESCRIPTION:
*   The function defines an interrupt receiver and returns a handle to it.
*
* PARAMETERS:
*   nr_subinterrupts  : number of different subinterrupts to be received
*   subint_addr       : memory location where the subinterrupt information is passed
*   ack_addr          : memory location where the acknowledgement information is passed
*   receiver_addr     : reading this memory location acknowledges the interrupt
*   target            : target number or address, set to zero for direct target access
*   sint_dpm_width    : width of the dual port memory
*   write_fcn         : function to perform write accesses to dpm
*   read_fcn          : function to perform read accesses to dpm
*   init_mode        : kind of initialization : SINT_INIT_OLD or SINT_INIT_NEW
*
*
* RETURNS:
*   0(NULL) if an error occures
*   handle to an interrupt receiver else
*
* NOTE: init_mode = SINT_INIT_OLD is only allowed for compatibility reasons.
*       Use the macro dssint_define_int_receiver than.
*
*
******************************************************************************/
#ifndef _INLINE
dssint_receiver_type *dssint_define_int_receiver_fcn(unsigned int nr_subinterrupts,
                                  unsigned long     subint_addr,
                                  unsigned long     ack_addr,
                                  unsigned long     receiver_addr,
                                  long              target,
                                  unsigned int      sint_mem_width,
                                  dpm_write_fcn_t   write_fcn,
                                  dpm_read_fcn_t    read_fcn,
                                  unsigned int      init_mode)
{
  volatile dssint_receiver_type* receiver;
  unsigned int i;
  unsigned long value;
  int bcnt=-1;
  volatile unsigned long dssint_dummy;


   if ((receiver = (dssint_receiver_type*)malloc(sizeof(dssint_receiver_type))) == NULL)
     return(NULL);

   receiver->nr_sint        = nr_subinterrupts;
   receiver->sint_addr      = subint_addr;
   receiver->ack_addr       = ack_addr;
   receiver->receiver_addr  = receiver_addr;
   receiver->target         = target;
   receiver->sint_mem_width = sint_mem_width;

   value = sint_mem_width ;
   while(value)
   {
      value = value >> 1;
      bcnt++;
   }
   receiver->sint_mem_shift = bcnt;

   receiver->state_position = 0;
   receiver->write_fcn      = write_fcn;
   receiver->read_fcn       = read_fcn;

   receiver->nr_words = (nr_subinterrupts + sint_mem_width - 1) / sint_mem_width;

   if ((receiver->acknowledge = malloc(sizeof(long)*receiver->nr_words)) == NULL)
     return(NULL);

   if ((receiver->state = malloc(sizeof(long)*receiver->nr_words)) == NULL)
     return(NULL);

   if ((receiver->enable_flags = malloc(sizeof(long)*receiver->nr_words)) == NULL)
     return(NULL);


   /* sub interrupts are enabled by default */
   for (i = 0; i < receiver->nr_words; i++)
   {
     *(receiver->enable_flags + i) = 0xFFFFFFFF;
   }

   if (init_mode == SINT_INIT_NEW)
   {
     for (i = 0; i < receiver->nr_words; i++)
     {
       /* initialize acknowledge (local copy) with sint value from dpm */
       *(receiver->acknowledge + i) = DPM_READ(receiver, receiver->sint_addr, i);
       /* initialize ack value in dpm with sint value from dpm */
       DPM_WRITE(receiver, receiver->ack_addr, i, (*(receiver->acknowledge + i)));
       *(receiver->state + i) = 0;
     }
   }
   else
   {
     /* initialize state and acknowledge (local copy) with zero */
     for (i = 0; i < receiver->nr_words; i++)
     {
       *(receiver->acknowledge + i) = 0;
       *(receiver->state + i) = 0;
     }
   }

   /* acknowledge to enable interrupts by reading memory */
   dssint_dummy = DPM_READ(receiver, receiver->receiver_addr, 0);
   return((dssint_receiver_type*)receiver);
}

#endif
/******************************************************************************
*
* FUNCTION:
*   void dssint_acknowledge(dssint_receiver_type *receiver)
*
* DESCRIPTION:
*   The function acknowledges the interrupt (hardware and software)
*
* PARAMETERS:
*   receiver : handle to the receiver, which arbitrates the interrupt
*
* RETURNS:
*
* NOTE:
*
******************************************************************************/
__INLINE void dssint_acknowledge(dssint_receiver_type *receiver)
{
  unsigned int i;
  unsigned long workaround_cc240;     /* the F240 compiler doesn't work correct
                                         without this help variable */
  unsigned long interim_state=0;
  volatile unsigned long dssint_dummy;

  receiver->state_position = 0;       /* reset state position */

  if (receiver->sint_addr != receiver->receiver_addr)  /* read dummy to acknowledge interrupt */
    dssint_dummy = DPM_READ(receiver, receiver->receiver_addr, 0);

  for (i = 0; i < receiver->nr_words; i++)
  {
    interim_state = (DPM_READ(receiver,receiver->sint_addr, i))^(*(receiver->acknowledge + i));
    *(receiver->state + i) |= interim_state;
    interim_state &= *(receiver->state + i);  /* has no effect, workaround for F240 compiler */

    *(receiver->acknowledge + i) = workaround_cc240 = (interim_state)^(*(receiver->acknowledge + i));

    DPM_WRITE(receiver, receiver->ack_addr, i, workaround_cc240);
  }
#ifdef _DS1104
  DS1104_LEVEL_TRIGGERED_DELAYED_INT_ACKNOWLEDGE();
#endif
}

/******************************************************************************
*
* FUNCTION:
*   int dssint_decode(dssint_receiver_type *receiver)
*
* DESCRIPTION:
*   function is called repetitively within an interrupt handler and processes the
*   sub-interrupt information copied to the receiver data structure by dssint_acknowledge.
*
* PARAMETERS:
*   receiver : handle to the receiver
*
* RETURNS:
*   SINT_NO_SUBINT if there is no pending sub-interrupt
*   number of highest priority pending sub-interrupt
*
* NOTE:
*
******************************************************************************/

__INLINE int dssint_decode(dssint_receiver_type *receiver)
{
  unsigned int i;
  unsigned long addr_offset;
  unsigned long bit_position;

  for ( i = (receiver->state_position); i < (receiver->nr_sint); i++)
  {
    addr_offset = (i >> receiver->sint_mem_shift );
    bit_position =  ((unsigned long)1) << ( i & (receiver->sint_mem_width - 1));
    if (*(receiver->state + addr_offset) & (bit_position & (*(receiver->enable_flags + addr_offset))))
    {
      *(receiver->state + addr_offset) &= ~bit_position;   /* clear bit in state */
      receiver->state_position=i+1;

      return(i);
    }
  }

  return((int)SINT_NO_SUBINT);
}


/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    void dssint_subint_enable(dssint_receiver_type *receiver,
*                              unsigned int sub_interrupt)
*
*  DESCRIPTION
*    Enables a sub interrupt. After initialization all sub interrupts are
*    enabled. This function is only used if a sub interrupt was disabled
*    via dssint_subint_disable before.
*
*  PARAMETERS
*    receiver : handle to the receiver, which arbitrates the interrupt
*    sub_interrupt : number of sub interrupt to enable
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

__INLINE void dssint_subint_enable(dssint_receiver_type *receiver,
                                   unsigned int sub_interrupt)
{
  unsigned long addr_offset;
  unsigned long bit_position;

  addr_offset = (sub_interrupt >> receiver->sint_mem_shift );
  bit_position =  ((unsigned long)1) << ( sub_interrupt & (receiver->sint_mem_width - 1));

  *(receiver->enable_flags + addr_offset) |= bit_position;
}


/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    void dssint_subint_disable(dssint_receiver_type *receiver,unsigned int sub_interrupt){}__INLINE void dssint_subint_reset(dssint_receiver_type *receive¬¦osigned int sub_interrupt)
*
*  DESCRIPTION
*    Disables a sub interrupt. After initialization all sub interrupts are
*    enabled. They must be disabled explicitly via this function.
*
*  PARAMETERS
*    receiver : handle to the receiver, which arbitrates the interrupt
*    sub_interrupt : number of sub interrupt to disable
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

__INLINE void dssint_subint_disable(dssint_receiver_type *receiver,
                                    unsigned int sub_interrupt)
{
  unsigned long addr_offset;
  unsigned long bit_position;

  addr_offset = (sub_interrupt >> receiver->sint_mem_shift );
  bit_position =  ((unsigned long)1) << ( sub_interrupt & (receiver->sint_mem_width - 1));

  *(receiver->enable_flags + addr_offset) &= ~bit_position;
}


/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    void dssint_subint_reset(dssint_receiver_type *receiver,unsigned int sub_interrupt)
*
*  DESCRIPTION
*    Clears a pending sub interrupt.
*
*  PARAMETERS
*    receiver : handle to the receiver, which arbitrates the interrupt
*    sub_interrupt : number of pending sub interrupt to clear
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

__INLINE void dssint_subint_reset(dssint_receiver_type *receiver,
                                  unsigned int sub_interrupt)
{
  unsigned long addr_offset;
  unsigned long bit_position;

  dssint_acknowledge(receiver);

  addr_offset = (sub_interrupt >> receiver->sint_mem_shift );
  bit_position =  ((unsigned long)1) << ( sub_interrupt & (receiver->sint_mem_width - 1));

  *(receiver->state + addr_offset) &= ~bit_position;
}



#endif



#undef __INLINE
