/******************************************************************************
*
* FILE:
*   int1103.h
*
* RELATED FILES:
*   dstypes.h
*
* DESCRIPTION:
*   interrupt support for DS1103 board
*
* HISTORY:
*   19.02.98  Rev 1.0  H.-G. Limberg     initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Int1103.h $ $Revision: 1.3 $ $Date: 2003/12/09 18:59:51GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/
#ifndef __INT1103_H__
#define __INT1103_H__

#include <dstypes.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

/* interrupt control registers */
#define DS1103_INT_IN_REG     0x10000020               /*32*/
#define DS1103_INT_MASK_REG   0x10000024               /*32*/
#define DS1103_INT_FLAG_REG   0x10000028               /*32*/


/* interrupt ids */

#define DS1103_INT_DECREMENTER        0
#define DS1103_INT_TIMER_A            1
#define DS1103_INT_TIMER_B            2
#define DS1103_INT_EXTERNAL_0         7
#define DS1103_INT_EXTERNAL_1         8
#define DS1103_INT_EXTERNAL_2         9
#define DS1103_INT_EXTERNAL_3        10
#define DS1103_INT_CAN               11
#define DS1103_INT_SLAVE_DSP         12
#define DS1103_INT_SLAVE_DSP_PWM     13 
#define DS1103_INT_SERIAL_UART       14
#define DS1103_INT_HOST              15
#define DS1103_INT_INC_ENC_CH1       16
#define DS1103_INT_INC_ENC_CH2       17
#define DS1103_INT_INC_ENC_CH3       18
#define DS1103_INT_INC_ENC_CH4       19
#define DS1103_INT_INC_ENC_CH5       20
#define DS1103_INT_INC_ENC_CH6       21
#define DS1103_INT_INC_ENC_CH7       22
#define DS1103_INT_SLAVE_DSP_PWM_INV 23 


#define SAVE_REGS_ON  1
#define SAVE_REGS_OFF 0


#define DS1103_GLOBAL_INTERRUPT_DISABLE() \
asm(int,  ".supervisoron"          "\n" \
          " lis   5,0"             "\n" \
          " ori   5,5,0x8000"      "\n" \
          " mfmsr 3"               "\n" \
          " andc  4,3,5"           "\n" \
          " mtmsr 4"               "\n" \
          " and   3,3,5"    )


#define DS1103_GLOBAL_INTERRUPT_ENABLE() \
asm(int,  ".supervisoron"          "\n" \
          " mfmsr 3"               "\n" \
          " ori   3,3,0x8000"      "\n" \
          " mtmsr 3"   )


/* timer interrupt status variables (from brtenv.c) */

extern UInt32 ds1103_isr_status_timerA;
extern UInt32 ds1103_isr_status_timerB;
extern UInt32 ds1103_isr_status_decrementer;

/* values for interrupt status */

#define DS1103_PREEMPTABLE  0


/* start_isr macros */

#define ds1103_start_isr_timerA(sampling_period, isr_function_name) {         \
    ds1103_isr_status_timerA = DS1103_PREEMPTABLE;       \
    ds1103_timerA_period_set (sampling_period); \
    ds1103_set_interrupt_vector( DS1103_INT_TIMER_A, (DS1103_Int_Handler_Type)&isr_function_name, SAVE_REGS_ON); \
    ds1103_enable_hardware_int(DS1103_INT_TIMER_A); \
    DS1103_GLOBAL_INTERRUPT_ENABLE(); \
    ds1103_timerA_start(); }

#define ds1103_start_isr_timerB(scale, sampling_period, isr_function_name) {         \
    ds1103_isr_status_timerB = DS1103_PREEMPTABLE;       \
    ds1103_timerB_init(scale); \
    ds1103_timerB_compare_set(sampling_period); \
    ds1103_timerB_sampling_period = sampling_period; \
    ds1103_set_interrupt_vector( DS1103_INT_TIMER_B, (DS1103_Int_Handler_Type)&isr_function_name, SAVE_REGS_ON); \
    ds1103_enable_hardware_int(DS1103_INT_TIMER_B); \
    DS1103_GLOBAL_INTERRUPT_ENABLE(); \
    ds1103_timerB_start(); }

#define ds1103_start_isr_decrementer(sampling_period, isr_function_name) {         \
  ds1103_isr_status_decrementer = DS1103_PREEMPTABLE; \
    ds1103_decrementer_period_set(sampling_period);   \
    ds1103_set_interrupt_vector( DS1103_INT_DECREMENTER, (DS1103_Int_Handler_Type)&isr_function_name, SAVE_REGS_ON); \
    ds1103_enable_hardware_int(DS1103_INT_DECREMENTER); \
    DS1103_GLOBAL_INTERRUPT_ENABLE(); }



/* begin_isr macros */

#define ds1103_begin_isr_timerA()   {             \
  if (ds1103_isr_status_timerA != DS1103_PREEMPTABLE)      \
    {  ds1103_disable_hardware_int(DS1103_INT_TIMER_A);    \
       DS1103_MSG_ERROR_SET(MSG_OVERRUN);                  \
       return;}    \
    else        \
    {  ds1103_isr_status_timerA = !DS1103_PREEMPTABLE ;   \
       DS1103_GLOBAL_INTERRUPT_ENABLE(); \
     } }



#define ds1103_begin_isr_timerB()   {                 \
    if (ds1103_isr_status_timerB != DS1103_PREEMPTABLE)           \
    {  ds1103_disable_hardware_int(DS1103_INT_TIMER_B);    \
       DS1103_MSG_ERROR_SET(MSG_OVERRUN);                  \
       return;} \
    else                                        \
    {  ds1103_isr_status_timerB = !DS1103_PREEMPTABLE ;   \
       ds1103_timerB_compare_set_periodically(ds1103_timerB_sampling_period); \
       DS1103_GLOBAL_INTERRUPT_ENABLE(); \
     } }


#define ds1103_begin_isr_decrementer()   {  \
    if (ds1103_isr_status_decrementer != DS1103_PREEMPTABLE )\
    {  ds1103_disable_hardware_int(DS1103_INT_DECREMENTER);\
       DS1103_MSG_ERROR_SET(MSG_OVERRUN);                  \
       return;}\
    else\
    {  ds1103_isr_status_decrementer = !DS1103_PREEMPTABLE ;\
       DS1103_GLOBAL_INTERRUPT_ENABLE();\
    } }


/* end_isr macros */

#define ds1103_end_isr_timerA() {    \
        DS1103_GLOBAL_INTERRUPT_DISABLE(); \
        ds1103_isr_status_timerA = DS1103_PREEMPTABLE; }

#define ds1103_end_isr_timerB() {    \
        DS1103_GLOBAL_INTERRUPT_DISABLE(); \
        ds1103_isr_status_timerB = DS1103_PREEMPTABLE; }

#define ds1103_end_isr_decrementer() {    \
        DS1103_GLOBAL_INTERRUPT_DISABLE(); \
        ds1103_isr_status_decrementer = DS1103_PREEMPTABLE; }

/******************************************************************************
  function prototypes
******************************************************************************/

typedef void (*DS1103_Int_Handler_Type)(void);

#ifdef _INLINE
#define __INLINE static 
#else
#define __INLINE
#endif


/******************************************************************************
*
* FUNCTION:
*   void ds1103_enable_hardware_int (UInt32 IntID)
*
* DESCRIPTION:
*   enables the specified hardware interrupt by clearing the related bit in the 
*   interrupt mask register respectively by setting the enable flag for the decrementer
*
* PARAMETERS:
*   IntID : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_enable_hardware_int(UInt32 IntID);  

/******************************************************************************
*
* FUNCTION:
*   void ds1103_disable_hardware_int (UInt32 IntID)
*
* DESCRIPTION:
*   disables the specified hardware interrupt by setting the related bit in the 
*   interrupt mask register respectively by clearing the enable flag for the decrementer
*
* PARAMETERS:
*   IntID : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg, initial version
*
******************************************************************************/
__INLINE void ds1103_disable_hardware_int(UInt32 IntID); 

/******************************************************************************
*
* FUNCTION:
*   DS1103_Int_Handler_Type ds1103_set_interrupt_vector(UInt32 IntID, 
*                                                       DS1103_Int_Handler_Type Handler,
*                                                       int SaveRegs)
*
* DESCRIPTION:
*
* PARAMETERS:
*   IntID   : ID number of the interrupt, defined in INT1103.h
*   Handler : handler to connect with the interrupt
*   SaveRegs: boolean variable, indicates if the processor registers should be saved
*
* RETURNS:
*   address of the handler which was connected to the interrupt before
*
* NOTE:
*
* HISTORY:
*   25.02.98   H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE DS1103_Int_Handler_Type ds1103_set_interrupt_vector(UInt32 IntID, DS1103_Int_Handler_Type Handler,int SaveRegs); 

/******************************************************************************
*
* FUNCTION:
*   DS1103_Int_Handler_Type ds1103_get_interrupt_vector(UInt32 IntID)
*
* DESCRIPTION:
*   get the vector of the handler related to the interrupt
*
* PARAMETERS:
*   IntID   : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*   vector of the handler related to the interrupt
*
* NOTE:
*
* HISTORY:                                        
*   25.02.98  H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE DS1103_Int_Handler_Type ds1103_get_interrupt_vector(UInt32 IntID);  

/******************************************************************************
*
* FUNCTION:
*   UInt32 ds1103_get_interrupt_status(void)
*
* DESCRIPTION:
*   gets the status of the EE bit in MSR of the processor
*
* PARAMETERS:
*
* RETURNS:
*   0x00008000 if EE bit is set (interrupts global enabled)
*   0x00000000 if EE bit is clear (interrupts global disabled)
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE UInt32 ds1103_get_interrupt_status(void); 

/******************************************************************************
*
* FUNCTION:
*   void ds1103_set_interrupt_status(UInt32 status)
*
* DESCRIPTION:
*   set the EE bit in MSR of the processor to the status, which was delivered
*   from the function ds1103_get_interrupt_status
*
* PARAMETERS:
*   0x00008000 to set EE bit
*   0x00000000 to not change EE bit 
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*
******************************************************************************/
__INLINE void ds1103_set_interrupt_status(UInt32 status); 

/******************************************************************************
*
* FUNCTION:
*   void ds1103_reset_interrupt_flag(UInt32 IntID)  
*
* DESCRIPTION:
*   reset the flag for the related interrupt in IIR if IntID != 0
*   decrementer interrupt (IntID =0) has no flag
*
* PARAMETERS:
*   IntID   : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE void ds1103_reset_interrupt_flag(UInt32 IntID);

/******************************************************************************
*
* FUNCTION:
*   void ds1103_reset_interrupt_flag(UInt32 IntID)  
*
* DESCRIPTION:
*   reset the flag for the related interrupt in IIR if IntID != 0
*   decrementer interrupt (IntID =0) has no flag
*
* PARAMETERS:
*   IntID   : ID number of the interrupt, defined in INT1103.h
*
* RETURNS:
*
* NOTE:
*
* HISTORY:
*   25.02.98  H.-G. Limberg  initial version
*
******************************************************************************/
__INLINE int ds1103_get_interrupt_flag(UInt32 IntID);  



#undef __INLINE

#ifdef _INLINE
 #include <Int1103.c>
#endif

#endif

