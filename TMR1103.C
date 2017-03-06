/******************************************************************************
*
* FILE:
*   Tmr1103.c
*
* RELATED FILES:
*   Tmr1103.h
*
* DESCRIPTION:
*   DS1103 on-board Timerfunctions for:
*    - on-chip Timebase 
*    - on-chip Decrementer
*    - external TimerA 
*    - external TimerB
*
* HISTORY: 20.07.98 RK initial version
*    
*
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: TMR1103.C $ $Revision: 1.3 $ $Date: 2007/08/02 08:30:45GMT+01:00 $
******************************************************************************/

asm(".supervisoron");

#include <Tmr1103.h>
#include <Int1103.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/
#define DS1103_EIEIO asm( " eieio " )

/******************************************************************************
  object declarations
******************************************************************************/

#ifndef _INLINE

/* multiplier to derive timerB frequency from bus clock */
Float64 ds1103_tmrb_frq_mltply[8] = { 0.25,  0.125, 0.0625,  0.03125,  0.015625,
                                      0.0078125, 0.0, 0.0 };

Float64 ds1103_tmra_frq = 0;
Float64 ds1103_tmra_prd = 0;
Float64 ds1103_decr_frq = 0;
Float64 ds1103_decr_prd = 0;
Float64 ds1103_bus_clck = 0;
Float64 ds1103_bus_prd = 0;
Float64 ds1103_tmrb_frq = 0;
Float64 ds1103_tmrb_prd = 0;
Float64 ds1103_timerB_sampling_period = 0;

#else

/* multiplier to derive timerB period and frequency from bus clock */
extern Float64 ds1103_tmrb_frq_mltply[];  
extern Float64 ds1103_tmra_frq;
extern Float64 ds1103_tmra_prd;
extern Float64 ds1103_decr_frq;
extern Float64 ds1103_decr_prd;
extern Float64 ds1103_bus_clck;
extern Float64 ds1103_bus_prd;
extern Float64 ds1103_tmrb_frq;
extern Float64 ds1103_tmrb_prd;
extern Float64 ds1103_timerB_sampling_period;

#endif


/******************************************************************************
  function prototypes
******************************************************************************/


/******************************************************************************
  function declarations
******************************************************************************/

#ifdef _INLINE
  #define __INLINE static
#else
  #define __INLINE
#endif

/******************************************************************************
*
* FUNCTION:
*           UInt32 ds1103_timebase_low_read(void)        
*   
* SYNTAX:
*           ds1103_timebase_low_read() 
*
*
* DESCRIPTION: Reads the lower part (32 bit wide TBL-register) of the on-chip
*              64-bit timebase.
*              
*
* PARAMETERS: no
*
* RETURNS: The 32 bit value of TBL (timebase low).
*
* NOTE:
*
******************************************************************************/
__INLINE UInt32 ds1103_timebase_low_read(void)          
{
 UInt32 timeb_low;
 
 timeb_low = asm( " mftb 3 " );
 return(timeb_low);
}

/******************************************************************************
*
* FUNCTION:
*            Int64 ds1103_timebase_read(void)
*
* SYNTAX:
*            ds1103_timebase_read(void)
*
*
* DESCRIPTION: Reads the on-chip 64-bit timebase.
*
* PARAMETERS: no
*
* RETURNS: The 64 bit value of the timebase (TBU and TBL). 
*          The value is returned in a struct of type Int64.
*          The type of the returned value is a structure of a 32 bit Int32
*          and a 32 bit UInt32 (defined in Dstypes.h):
*          typedef struct { UInt32 low; Int32 high;  } Int64;
*     
*
* NOTE: Example:  {
*                   Int64 timebase;
*                   Int32 tbu;
*                   UInt32 tbl;
*
*                   timebase = ds1103_timebase_read(); 
*                   tbu = timebase.high;
*                   tbl = timebase.low;
*
*                   ...
*
******************************************************************************/
__INLINE Int64 ds1103_timebase_read(void)          
{
 Int64 timebase;
 UInt32 new_timeb_high, old_timeb_high, timeb_low;
 
 do
 {
   /* read TBRU */
  old_timeb_high = asm( " mftbu 3 " );

  /* read TBRL */
  timeb_low = asm( " mftb 3 " );

  /* read TBRU again */
  new_timeb_high = asm( " mftbu 3 " );

 }while(new_timeb_high != old_timeb_high);

 timebase.high = (Int32)new_timeb_high;
 timebase.low =  timeb_low;

 return(timebase);
}

/******************************************************************************
*
* FUNCTION:
*            void ds1103_decrementer_set(UInt32 decrementer_value) 
*
* SYNTAX:
*            ds1103_decrementer_set(UInt32 decrementer_value)
*
* DESCRIPTION: Sets the on-chip decrementer.
*
* PARAMETERS: UInt32 decrementer_value: the value you want to load into the
*                                       decrementer.
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_decrementer_set(UInt32 decrementer_value)
{
  asm(" mtdec 3");
}

/******************************************************************************
*
* FUNCTION:
*             void ds1103_decrementer_period_set(Float64 time)
*
* SYNTAX:
*             ds1103_decrementer_period_set(Float64 time)
*
* DESCRIPTION: Sets the on-chip decrementer with a period (s).
*
* PARAMETERS: Float64 time: period in s.
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_decrementer_period_set(Float64 time)
{
  UInt32 dec_counter;

  dec_counter = (UInt32)(time * ds1103_decr_frq);
  ds1103_dec_period_reg = (dec_counter - 1);
  ds1103_decrementer_set(ds1103_dec_period_reg);
}

/******************************************************************************
*
* FUNCTION:
*           void ds1103_decrementer_read(Float64* time)
*
* SYNTAX:
*           ds1103_decrementer_read(Float64* time)
*
* DESCRIPTION: Reads the period (s) of the on-chip decrementer.
*
* PARAMETERS: Float64* time: address, the period is written to.
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_decrementer_read(Float64* time)
{
  UInt32 dec_counter;

  dec_counter = asm( " mfspr 3, 22 " );
  *time = ( (Float64) dec_counter ) * ds1103_decr_prd;
}

/******************************************************************************
*
* FUNCTION:
*             void ds1103_timerA_period_set (Float64 time)
*
* SYNTAX:
*             ds1103_timerA_period_set (Float64 time)
*
* DESCRIPTION: Sets the period (s, period-register) of TimerA.
*
* PARAMETERS: Float64 time: period in s.
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerA_period_set (Float64 time)
{
  DS1103_TMRA_PRD_RG[0] = (UInt32)( (time * ds1103_tmra_frq) - 1 );
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*             void ds1103_timerA_period_reload (Float64 time)
*
* SYNTAX:
*             ds1103_timerA_period_reload (Float64 time)
*
* DESCRIPTION: Sets the period (s, period-register) of TimerA.
*              The counter is reloaded immediately.
*
* PARAMETERS: Float64 time: period in s.
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerA_period_reload (Float64 time)
{

  UInt32 msr_state;

  msr_state = ds1103_get_interrupt_status();        /* save interrupt status */
  DS1103_GLOBAL_INTERRUPT_DISABLE();

  tmr_ctrl_bit.entmra = 0;                                     /* timer stop */
  DS1103_EIEIO;

  /* new period set */
  DS1103_TMRA_PRD_RG[0] = ((UInt32)( (time * ds1103_tmra_frq) - 1 ));

  DS1103_EIEIO;
  tmr_ctrl_bit.entmra = 1;                                    /* timer start */

  DS1103_EIEIO;
  ds1103_set_interrupt_status(msr_state);         /* reload interrupt status */

}

/******************************************************************************
*
* FUNCTION:
*             void ds1103_timerA_read (Float64 *time)
*
* SYNTAX:
*             ds1103_timerA_read (Float64 *time)
*
* DESCRIPTION: Reads the TimerA.
*
* PARAMETERS: Float64* time: address, the time is written to.
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerA_read (Float64* time)
{
   UInt32 timera;

   timera = DS1103_TMRA_CNT_RG[0];
   *time = ( (Float64) timera ) * ds1103_tmra_prd;
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*            void ds1103_timerA_start (void)
*
*
* SYNTAX:
*            ds1103_timerA_start (void)
*
* DESCRIPTION: Starts counting of TimerA.
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerA_start (void)
{
  tmr_ctrl_bit.entmra = 1 ;
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*            void ds1103_timerA_stop (void)
*
* SYNTAX:
*            ds1103_timerA_stop (void)
*
* DESCRIPTION: Stops counting of TimerA.
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerA_stop (void)
{
  tmr_ctrl_bit.entmra = 0 ;
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*            void ds1103_timerA_autostart (UInt16 mask)
*
* SYNTAX:
*            ds1103_timerA_autostart (UInt16 mask)
*
* DESCRIPTION: Activates pre-trigger of I/O-components with the
*              TMRSTB-register.
*
* PARAMETERS: UInt16 mask: the mask defines, which I/O components are
*                          pre-triggered (->Hardware-Manual, TMRSTB-Register).
*
*
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerA_autostart (UInt16 mask)
{
  tmrstb_reg = mask;
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*          void ds1103_timerB_init (UInt16 scale)
*
* SYNTAX:
*          ds1103_timerB_init (UInt16 scale)
*
* DESCRIPTION: Initialized the prescaler of TimerB. 
*
* PARAMETERS: UInt16 scale: defines the prescaler of TimerB.
*          
*        predef. Symbols (Tmr1103.h):        
*
*             DS1103_TIMERB_1_4_BCLK   :   1/4 busclock
*             DS1103_TIMERB_1_8_BCLK   :   1/8 busclock      
*             DS1103_TIMERB_1_16_BCLK  :  1/16 busclock
*             DS1103_TIMERB_1_32_BCLK  :  1/32 busclock
*             DS1103_TIMERB_1_64_BCLK  :  1/64 busclock
*             DS1103_TIMERB_1_128_BCLK : 1/128 busclock
*
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerB_init (UInt16 scale)
{
  tmr_ctrl_bit.entmrb = 0;                                    /* hold timerb */
  DS1103_EIEIO;
  tmr_ctrl_bit.seltmrb = scale ;                          /* inp. frq.select */
  ds1103_tmrb_frq = ds1103_bus_clck * ds1103_tmrb_frq_mltply[scale];
  ds1103_tmrb_prd = 1/ds1103_tmrb_frq;
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*          void ds1103_timerB_compare_set (Float64 delta_time)
*
* SYNTAX:
*          ds1103_timerB_compare_set (Float64 delta_time)
*
* DESCRIPTION: This function adds the delta_time to the value of the compare
*              register. This function is used by the macros
*              ds1103_start_isr_timerB and ds1103_begin_isr_timerB to generate
*              an periodical interrupt.
*
* PARAMETERS: Float64 delta_time:
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerB_compare_set (Float64 delta_time)
{
  volatile ds1103_tmr_access* accessptr;

  accessptr = (volatile ds1103_tmr_access*)DS1103_TMR;
  accessptr->tcmb = (UInt32)( accessptr->tmrb +
                    (UInt32) (delta_time * ds1103_tmrb_frq) );
  DS1103_EIEIO;
}


/******************************************************************************
*
* FUNCTION:
*          void ds1103_timerB_compare_set_periodically (Float64 delta_time)
*
* SYNTAX:
*          ds1103_timerB_compare_set_periodically (Float64 delta_time)
*
* DESCRIPTION: This function adds the delta_time to the value of the compare
*              register. This function is used by the macro 
*              ds1103_begin_isr_timerB to generate a periodical interrupt.
*
* PARAMETERS: Float64 delta_time:
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerB_compare_set_periodically (Float64 delta_time)
{
  volatile ds1103_tmr_access* accessptr;

  accessptr = (volatile ds1103_tmr_access*)DS1103_TMR;
  accessptr->tcmb = (UInt32)( accessptr->tcmb +
                    (UInt32) (delta_time * ds1103_tmrb_frq) );
  DS1103_EIEIO;
}


/******************************************************************************
*
* FUNCTION:
*           void ds1103_timerB_read (Float64 *time)
*
* SYNTAX:
*           ds1103_timerB_read (Float64 *time)
*
* DESCRIPTION: Reads the TimerB and returns the time in s.
*
* PARAMETERS: Float64* time: address where the time is stored.
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerB_read (Float64 *time)
{
  UInt32 timerb;

  timerb = *DS1103_TMRB_CNT_RG;
  *time = ( (Float64)(timerb) - 1 ) * ds1103_tmrb_prd;
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*            void ds1103_timerB_start (void)
*
* SYNTAX: 
*            ds1103_timerB_start (void)
*
* DESCRIPTION: Starts the counting of TimerB.
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerB_start (void)
{
  tmr_ctrl_bit.entmrb = 1 ;
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*            void ds1103_timerB_stop (void)
*
* SYNTAX:
*            ds1103_timerB_stop (void)
*
* DESCRIPTION: Stops counting of TimerB.
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_timerB_stop (void)
{
  tmr_ctrl_bit.entmrb = 0 ;
  DS1103_EIEIO;
}

/******************************************************************************
*
* FUNCTION:
*            Float64 ds1103_timebase_fltread(void)
*
* SYNTAX:
*            ds1103_timebase_fltread(void)
*
*
* DESCRIPTION: Reads the on-chip 64-bit timebase.
*
* PARAMETERS:  no
*
* RETURNS:     value of the timebase register in s
*
*
* NOTE:
*
******************************************************************************/
__INLINE Float64 ds1103_timebase_fltread(void)
{
  Float64 abs_ppc_time=0;
  UInt32 new_timeb_high, old_timeb_high, timeb_low;

  do
  {
    /* read TBRU */
    old_timeb_high = asm( " mftbu 3 " );

    /* read TBRL */
    timeb_low = asm( " mftb 3 " );

    /* read TBRU again */
    new_timeb_high = asm( " mftbu 3 " );

  }while(new_timeb_high != old_timeb_high);

  abs_ppc_time = timeb_low * ds1103_decr_prd +
               ( (Float64)new_timeb_high * ds1103_decr_prd * 4.294967296e9);

  return abs_ppc_time;
}

#undef __INLINE





