/******************************************************************************
*
* FILE:
*   Tmr1103.h
*
* RELATED FILES:
*   Tmr1103.c
*
* DESCRIPTION:
*   DS1103 on-board Timerfunctions for:
*    - on-chip Timebase 
*    - on-chip Decrementer
*    - external TimerA 
*    - external TimerB
*
*
* HISTORY: RK initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: TMR1103.H $ $Revision: 1.2 $ $Date: 2003/08/07 14:54:10GMT+01:00 $
******************************************************************************/

#ifndef __Tmr1103__
#define __Tmr1103__

#include <Dstypes.h>


/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

/* definitions of io-registers */
#define DS1103_TMR           0x10000000  
#define DS1103_TMRA_COUNTER	 0x10000000
#define DS1103_TMRA_PERIOD   0x10000004
#define DS1103_TMRB_COUNTER  0x10000008
#define DS1103_TMRB_COMPARE	 0x1000000C
#define DS1103_TMR_CTRL      0x10000010  
#define DS1103_TMRSTB        0x10000082
#define DS1103_IMR           0x10000024


#define DS1103_TMRA_CNT_RG ((volatile UInt32*)0x10000000)
#define DS1103_TMRA_PRD_RG ((volatile UInt32*)0x10000004)
#define DS1103_TMRB_CNT_RG ((volatile UInt32*)0x10000008)
#define DS1103_TMRB_CMP_RG ((volatile UInt32*)0x1000000C)
#define DS1103_TMR_CTRL_RG ((volatile UInt32*)0x10000010)
#define DS1103_TMRSTB_RG   ((volatile UInt16*)0x10000082)


#define tmr_ctrl_bit ((volatile ds1103_tmrctrl_reg*)DS1103_TMR_CTRL)->_ctrl_bit
#define tmr_ctrl_reg ((volatile ds1103_tmrctrl_reg*)DS1103_TMR_CTRL)->_ctrl_reg
#define tmrstb_bit ((volatile ds1103_tmrstb_reg*)DS1103_TMRSTB)->_tmrstb_bit
#define tmrstb_reg ((volatile ds1103_tmrstb_reg*)DS1103_TMRSTB)->_tmrstb_rg

/* def. for Interrupt enable */
#define INT_TIMERA_EN    0x1FFFFE
#define INT_TIMERB_EN    0x1FFFFD


/* predef. scales for TimerB - Resolution: for use with ds1103_timerB_init(scale) */
#define DS1103_TIMERB_1_4_BCLK       0
#define DS1103_TIMERB_1_8_BCLK       1
#define DS1103_TIMERB_1_16_BCLK      2
#define DS1103_TIMERB_1_32_BCLK      3
#define DS1103_TIMERB_1_64_BCLK      4
#define DS1103_TIMERB_1_128_BCLK     5


/* predef. for ds1103_timerA_autostart */
#define DS1103_TMRST_ADC1     0x1
#define DS1103_TMRST_ADC2     0x2
#define DS1103_TMRST_ADC3     0x4
#define DS1103_TMRST_ADC4     0x8
#define DS1103_TMRST_ADC5    0x10
#define DS1103_TMRST_ADC6    0x20
#define DS1103_TMRST_ADC7    0x40
#define DS1103_TMRST_ADC8    0x80
#define DS1103_TMRST_DAC    0x100
#define DS1103_TMRST_INC1   0x400
#define DS1103_TMRST_INC2   0x800
#define DS1103_TMRST_INC3  0x1000 
#define DS1103_TMRST_INC4  0x2000
#define DS1103_TMRST_INC5  0x4000
#define DS1103_TMRST_INC6  0x8000




/*****************************************************************************
  structure for timer-access
*****************************************************************************/


typedef  struct {
	 volatile UInt32 tmra;		
	 volatile UInt32 tpra;
   volatile UInt32 tmrb;		
	 volatile UInt32 tcmb;
   volatile UInt32 tmrctrl;		
	     } ds1103_tmr_access;




/*****************************************************************************
  union for timer-control register
*****************************************************************************/

typedef union {
  struct {
   UInt32 reserved               :26;
   UInt32 hostint                :1;
   UInt32 seltmrb                :3;
   UInt32 entmrb                 :1;
   UInt32 entmra                 :1;
    }_ctrl_bit;
   UInt32 _ctrl_reg;          
 } ds1103_tmrctrl_reg;



/*****************************************************************************
  union for the TMRSTB - Register 
*****************************************************************************/


typedef  union {
	 struct {
	 UInt16  		tstbinc6	  :1;	
   UInt16  		tstbinc5	  :1;	
   UInt16  		tstbinc4	  :1;	
   UInt16  		tstbinc3	  :1;	
   UInt16  		tstbinc2	  :1;	
   UInt16  		tstbinc1	  :1;	
   UInt16  		reserved    :1;	
   UInt16  		tupdac	    :1;	
   UInt16  		tscadc8	    :1;	
   UInt16  		tscadc7	    :1;	
   UInt16  		tscadc6	    :1;	
   UInt16  		tscadc5	    :1;	
   UInt16  		tscadc4	    :1;	
   UInt16 		tscadc3	    :1;	
   UInt16 		tscadc2	    :1;	
   UInt16  		tscadc1     :1;	
       }_tmrstb_bit;				
   UInt16 _tmrstb_rg;
  } ds1103_tmrstb_reg;



/******************************************************************************
  object declarations
******************************************************************************/

extern Float64 ds1103_tmrb_frq_mltply[8];
extern Float64 ds1103_tmra_frq;
extern Float64 ds1103_tmra_prd;
extern Float64 ds1103_decr_frq;
extern Float64 ds1103_decr_prd;
extern Float64 ds1103_bus_clck;
extern Float64 ds1103_bus_prd;
extern Float64 ds1103_timerB_sampling_period;

extern UInt32 ds1103_dec_period_reg;                    /* decl. in startup */

/******************************************************************************
  function prototypes
******************************************************************************/

#ifdef _INLINE
  #define __INLINE static
#else
  #define __INLINE
#endif


#ifndef _INLINE

extern void ds1103_timerA_period_set (Float64 time);
extern void ds1103_timerA_period_reload (Float64 time);
extern void ds1103_timerA_read (Float64 *time);
extern void ds1103_timerA_period_read (Float64 *time);
extern void ds1103_timerA_start (void);
extern void ds1103_timerA_stop (void);
extern void ds1103_timerA_autostart (UInt16 mask);
extern void ds1103_timerB_init (UInt16 scale);
extern void ds1103_timerB_compare_set (Float64 delta_time);
extern void ds1103_timerB_compare_set_periodically (Float64 delta_time);
extern void ds1103_timerB_read (Float64 *time);
extern void ds1103_timerB_start (void);
extern void ds1103_timerB_stop (void);
extern void ds1103_decrementer_set(UInt32 decrementer_value);          
extern void ds1103_decrementer_period_set(Float64 time);
extern void ds1103_decrementer_read(Float64* time);
extern UInt32 ds1103_timebase_low_read(void);
extern Int64 ds1103_timebase_read(void);          
extern Float64 ds1103_timebase_fltread(void);

#else

__INLINE void ds1103_timerA_period_set (Float64 time);
__INLINE void ds1103_timerA_period_reload (Float64 time);
__INLINE void ds1103_timerA_read (Float64 *time);
__INLINE void ds1103_timerA_start (void);
__INLINE void ds1103_timerA_stop (void);
__INLINE void ds1103_timerA_autostart (UInt16 mask);
__INLINE void ds1103_timerB_init (UInt16 scale);
__INLINE void ds1103_timerB_compare_set (Float64 delta_time);
__INLINE void ds1103_timerB_compare_set_periodically (Float64 delta_time);
__INLINE void ds1103_timerB_read (Float64 *time);
__INLINE void ds1103_timerB_start (void);
__INLINE void ds1103_timerB_stop (void);
__INLINE void ds1103_decrementer_set(UInt32 decrementer_value);
__INLINE void ds1103_decrementer_period_set(Float64 time);
__INLINE void ds1103_decrementer_read(Float64* time);
__INLINE UInt32 ds1103_timebase_low_read(void);
__INLINE Int64 ds1103_timebase_read(void);
__INLINE Float64 ds1103_timebase_fltread(void);

#endif

#undef __INLINE


/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
 #include <Tmr1103.c>
#endif

#endif

