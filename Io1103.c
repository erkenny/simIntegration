/******************************************************************************
*
* FILE:
*   Io1103.c
*
* RELATED FILES:  
*   Io1103.h
*   Int1103.h
*   Tmr1103.h
*   Cfg1103.h
*   Message.h 
*
* DESCRIPTION:
*   DS1103 on-board I/O access functions for:
*    - ADC input
*    - DAC output
*    - incremental encoder support
*    - parallel port digital I/O
*    - cpu-temperature reading
*   
* 
*
* HISTORY:  RK initial version
*           JS changes for redesign board rev. 9.0
*
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Io1103.c $ $Revision: 1.20 $ $Date: 2004/09/14 17:53:19GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#include <Io1103.h>
#include <tic1103.h>
#include <Int1103.h>
#include <Init1103.h>
#include <Tmr1103.h> 
#include <Cfg1103.h>
#include <Dsmsg.h>
#include <Ds1103_msg.h>
/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

#define DS1103_EIEIO asm( " eieio " )
  
#define DS1103_INC_RSTONIDX_PASSIVE DS1103_INC_NO_RESETONIDX
#define DS1103_INC_RSTONIDX_ACTIVE  DS1103_INC_RESETONIDX

  
/******************************************************************************
  object declarations
******************************************************************************/
extern volatile UInt16 ds1103_advanced_io;

#ifndef _INLINE

/* tables for dac */
UInt16 ds1103_dac_ind1[] = { 1, 1, 0, 3, 2, 5, 4, 7, 6};
UInt16 ds1103_dac_ind2[] = { 0, 0, 0, 1, 1, 2, 2, 3, 3};


/* tables for adc */
UInt16 ds1103_adc_chan1[] = { 0 , 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 8, 8,
                              8, 8, 16, 32, 64, 128 };
UInt16 ds1103_adc_convert[] = { 0, 1, 2, 4, 8, 16, 32, 64, 128 };


UInt16 ds1103_adc_addr[] = { 1, 1, 1, 1, 1, 0, 0, 0, 0, 3, 3, 3, 3, 2, 2,
                             2, 2, 5, 4, 7, 6 };

UInt16 ds1103_adc_conv_addr[] = { 1, 1, 0, 3, 2, 5, 4, 7, 6 };


Int16 ds1103_adc_ch_msk[] = { 0xFFFF,
                               0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
                               0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
                               0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
                               0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
                               0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0};

Int16 ds1103_adc_conv_msk[] = { 0xFFFF,
                                 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
                                 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0}; 


UInt32 ds1103_adc2_msk[] = { 0xFFFFFFFF,
                             0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                             0xFFF0FFF0, 0xFFF0FFF0, 0xFFF0FFF0, 0xFFF0FFF0};



/* tables for inc */
Int32 ds1103_fine_count_offs = 0;
Int32 ds1103_fine_count = 0;
Int32 ds1103_ch6ch7_latch[] = {0, 0};
UInt16 ds1103_rstonidx_flag[] = { 0, 0, 0, 0, 0, 0, 0, 0};

UInt16 ds1103_inc_rstonidx_state[] =   { DS1103_INC_RSTONIDX_PASSIVE, DS1103_INC_RSTONIDX_PASSIVE, 
                                         DS1103_INC_RSTONIDX_PASSIVE, DS1103_INC_RSTONIDX_PASSIVE, 
                                         DS1103_INC_RSTONIDX_PASSIVE, DS1103_INC_RSTONIDX_PASSIVE, 
                                         DS1103_INC_RSTONIDX_PASSIVE, DS1103_INC_RSTONIDX_PASSIVE};

Int32 ds1103_inc_counter[] = { 0, 0, 0, 0, 0, 0, 0, 0};
UInt16 ds1103_inc_chan[] = { 0, 0, 1, 2, 3, 4, 5, 5}; /* used for IOSTB */
UInt16 ds1103_inc_dig_offs[] = { 0, 0, 1, 2, 3, 16};
UInt16 ds1103_inc_iir[] = { 0, 0, 1, 2, 3, 4, 5, 6}; /* used for IIR */
UInt16 ds1103_inc_ch6_shift[]= { 0, 0, 0, 8 };

UInt16 ds1103_inc_ch6_mode = 0;
UInt16 ds1103_inc_ch7_mode = 0;
UInt32 ds1103_inc_iirmsk[] = { 0x0, 0x8000, 0x10000, 0x20000, 0x40000,
                               0x80000, 0x100000, 0x200000 };
UInt16 ds1103_inc_rstonidx[] = { 0, 0, 0, 0, 0, 0, 0, 0};
UInt16 ds1103_inc_setup_rst[] = { 0, 0x4000, 0x8000, 0x1, 0x2, 0x4 };
                                 /* Bit 14, 15, 0, 1, 2 */
UInt32 ds1103_inc_ch6_msk[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFF };


UInt16  ds1103_haninc_ldmde_msk[2][4] = {
      {  0x8FFF, 0xF8FF, 0xF8FF, 0xF8FF },
      {  0xF8FF, 0x8FFF, 0x8FFF, 0x8FFF }
    };

UInt16  ds1103_haninc_idxmde_msk[2][4] = {
      {  0x7FFF, 0xF7FF, 0xF7FF, 0xF7FF },
      {  0xF7FF, 0x7FFF, 0x7FFF, 0x7FFF }
    };


UInt16 ds1103_haninc_idx_mde[2][4] = {
     { DS1103_HANINC_CT2LEN, DS1103_HANINC_CT1LEN, DS1103_HANINC_CT1LEN,
       DS1103_HANINC_CT1LEN },
     { DS1103_HANINC_CT1LEN, DS1103_HANINC_CT2LEN, DS1103_HANINC_CT2LEN,
       DS1103_HANINC_CT2LEN }
   };


UInt16 ds1103_haninc_ldmde[2][4] = {
    { DS1103_HANINC_CT2_LMDE2, DS1103_HANINC_CT1_LMDE2,
      DS1103_HANINC_CT1_LMDE2, DS1103_HANINC_CT1_LMDE2 },
    { DS1103_HANINC_CT1_LMDE2, DS1103_HANINC_CT2_LMDE2,
      DS1103_HANINC_CT2_LMDE2, DS1103_HANINC_CT2_LMDE2 }
   };


UInt32 ds1103_inc_subdiv_countmsk[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                                        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                                        0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFC,
                                        0xFFFFFFF8, 0xFFFFFFF0, 0xFFFFFFE0};


UInt32 ds1103_inc_subdiv_finecountmsk[] = { 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFC,
                                            0xFFFFFFF8, 0xFFFFFFF0, 0xFFFFFFE0,
                                            0xFFFFFFC0, 0xFFFFFF80, 0x0,
                                            0x0, 0x0, 0x0}; 

Int16 volatile ds1103_ppc_temperature = 0;

UInt32  ds1103_sync_enabled = 0;


#else


/* tables for dac */
extern UInt16 ds1103_dac_ind1[];
extern UInt16 ds1103_dac_ind2[];

/* tables for adc */
extern UInt16 ds1103_adc_chan1[];
extern UInt16 ds1103_adc_convert[];

extern UInt16 ds1103_adc_addr[];
extern UInt16 ds1103_adc_conv_addr[];
extern Int16 ds1103_adc_ch_msk[]; 
extern Int16 ds1103_adc_conv_msk[]; 
extern Int32 ds1103_adc2_msk[];

/* tables for inc */
extern Int32 ds1103_fine_count_offs;
extern Int32 ds1103_fine_count;
extern Int32 ds1103_ch6ch7_latch[];
extern UInt16 ds1103_rstonidx_flag[];
extern UInt16 ds1103_inc_rstonidx_state[];
extern Int32 ds1103_inc_counter[];
extern UInt16 ds1103_inc_chan[]; /* used for IOSTB */
extern UInt16 ds1103_inc_dig_offs[];
extern UInt16 ds1103_inc_iir[]; /* used for IIR */
extern UInt16 ds1103_inc_ch6_shift[]; 
extern UInt16 ds1103_inc_ch6_mode;
extern UInt16 ds1103_inc_ch7_mode; 
extern UInt32 ds1103_inc_iirmsk[]; 
extern UInt16 ds1103_inc_rstonidx[];
extern UInt16 ds1103_inc_setup_rst[]; /* Bit 14, 15, 0, 1, 2 */
extern UInt32 ds1103_inc_ch6_msk[];     
extern UInt32 ds1103_inc_subdiv_countmsk[];
extern UInt16 ds1103_haninc_ldmde_msk[2][4]; 
extern UInt16 ds1103_haninc_idxmde_msk[2][4];
extern UInt16 ds1103_haninc_ldmde[2][4];
extern UInt16 ds1103_haninc_idx_mde[2][4]; 
extern UInt32 ds1103_inc_subdiv_finecountmsk[];     
extern volatile Int16 ds1103_ppc_temperature;
extern UInt32  ds1103_sync_enabled;

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


/*******************************************************************************
*
* FUNCTION:
*   void ds1103_syncin_edge_setup (UInt16 edge)
*
* SYNTAX:
*    void ds1103_syncin_edge_setup(UInt16 edge)
*
* DESCRIPTION:
*    This function sets up the edge for the SYNCIN event via the T1PWM
*    pin, upon which the input components are triggered by PWM or external
*    input.
*
* PARAMETERS:
*    edge : determines the signal edge, which triggers the I/O components
*             DS1103_SYNC_TRIGGER_RISING (1) : triggers on rising edge
*             DS1103_SYNC_TRIGGER_FALLING (0): triggers on falling edge
*
* RETURNS:
*
* REMARKS: 
*     Only available with board revisions >= 9.0
*
*******************************************************************************/
__INLINE  void ds1103_syncin_edge_setup(UInt16 edge)
{
  rtlib_int_status_t int_status;

  if(!ds1103_advanced_io)
  {
    msg_error_printf(MSG_SM_RTLIB, DS1103_WRONG_BOARD_REV, DS1103_WRONG_BOARD_REV_TXT,"ds1103_syncin_edge_setup()",">= 9.0");
    RTLIB_EXIT(1);
  }
 
  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if(edge)
  {
    sync_ctrl_reg&= ~DS1103_SYNC_TRIG_EDGE_ADC;
  }  
  else
  {
    sync_ctrl_reg|= DS1103_SYNC_TRIG_EDGE_ADC;
  }
  RTLIB_INT_RESTORE(int_status);

  RTLIB_FORCE_IN_ORDER();
}


/*******************************************************************************
*
* FUNCTION:
*   void ds1103_syncout_edge_setup (UInt16 edge)
*
* SYNTAX:
*   ds1103_syncout_edge_setup(UInt16 edge)
*
* DESCRIPTION:
*    This function sets up the edge for the SYNCOUT event via the T1PWM
*    pin, upon which the output components are triggered by PWM or external
*    input.
*
* PARAMETERS:
*    edge : determines the signal edge, which triggers the I/O components
*             DS1103_SYNC_TRIGGER_RISING (1) : triggers on rising edge
*             DS1103_SYNC_TRIGGER_FALLING (0): triggers on falling edge
*
* RETURNS:
*
* REMARKS:
*     Only available with board revisions >= 9.0
*
*******************************************************************************/
__INLINE void ds1103_syncout_edge_setup(UInt16 edge)
{
  rtlib_int_status_t int_status;

  if(!ds1103_advanced_io)
  {
    msg_error_printf(MSG_SM_RTLIB, DS1103_WRONG_BOARD_REV, DS1103_WRONG_BOARD_REV_TXT,"ds1103_syncout_edge_setup()",">= 9.0");
    RTLIB_EXIT(1);
  }
  
  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if(edge)
     sync_ctrl_reg&= ~DS1103_SYNC_TRIG_EDGE_DAC;
    
  else
     sync_ctrl_reg|= DS1103_SYNC_TRIG_EDGE_DAC;

  
  RTLIB_INT_RESTORE(int_status);
  RTLIB_FORCE_IN_ORDER();
}

/*******************************************************************************
*
* FUNCTION:
*   void ds1103_syncin_trigger (void)
*
* SYNTAX:
*   ds1103_syncin_trigger(void)
*
* DESCRIPTION:
*   This function starts/strobes the I/O components which are enabled for
*   trigger by the SYNCIN line like AD converter and incremental encoder.
*
* PARAMETERS:
*
* RETURNS:
*
* REMARKS:
*   Only available with board revisions >= 9.0
*
*******************************************************************************/
__INLINE void ds1103_syncin_trigger(void)
{
  if(!ds1103_advanced_io)
  {                    
    msg_error_printf(MSG_SM_RTLIB, DS1103_WRONG_BOARD_REV, DS1103_WRONG_BOARD_REV_TXT,"ds1103_syncin_trigger()",">= 9.0");
    RTLIB_EXIT(1);
  }
  *DS1103_IOSTB_REG = ds1103_sync_enabled & DS1103_STRB_SYNCIN_MSK;
  RTLIB_FORCE_IN_ORDER();
}

/*******************************************************************************
*
* FUNCTION:
*   void ds1103_syncout_trigger (void)
*
* SYNTAX:
*   ds1103_syncout_trigger(void)
*
* DESCRIPTION:
*   This function strobes the I/O components which are enabled for
*   trigger by the SYNCOUT line like DA converter.
*
* PARAMETERS:
*
* RETURNS:
*
* REMARKS:
*   Only available with board revisions >= 9.0
*
*******************************************************************************/
__INLINE void ds1103_syncout_trigger(void)
{
  if(!ds1103_advanced_io)
  {
    msg_error_printf(MSG_SM_RTLIB, DS1103_WRONG_BOARD_REV, DS1103_WRONG_BOARD_REV_TXT,"ds1103_syncout_trigger()",">= 9.0");
    RTLIB_EXIT(1);
  }
 
  *DS1103_IOSTB_REG = ds1103_sync_enabled & DS1103_STRB_SYNCOUT_MSK;
  RTLIB_FORCE_IN_ORDER();
}


/*************************************************************************adc*/

/*******************************************************************************
*
* FUNCTION:
*   void ds1103_adc_trigger_setup(UInt16 converter, UInt16 state)
*
* SYNTAX:
*   void ds1103_adc_trigger_setup(UInt16 converter, UInt16 state)
*
* DESCRIPTION:
*   This function enables/disables the external trigger for the specified
*   AD converter. The converter can be started by the T1PWM pin of the
*   slave-DSP, which is also available on the DS1103 bracket.
*
* PARAMETERS:
*   converter : AD converter to be set up (1..8)
*   state     : enables or disables external trigger
*                  DS1103_TRIGGER_DISABLE (0): disables external trigger
*                  DS1103_TRIGGER_ENABLE (1) : enables external trigger
*
* RETURNS:
*   
* REMARKS:
*           Only available with board revisions >= 9.0.
*           The function return an error message, if the trigger is set to enable 
*           on board revisions < 9.0. Calling trigger_setup with state= 
*           DS1103_TRIGGER_DISABLE will be ignored for board revisions < 9.0
*
*
*******************************************************************************/
__INLINE void ds1103_adc_trigger_setup(UInt16 converter, UInt16 state)
{
  rtlib_int_status_t int_status;

  if(!ds1103_advanced_io)
  {
    if(state==DS1103_TRIGGER_ENABLE)
    {
      msg_error_printf(MSG_SM_RTLIB, DS1103_WRONG_BOARD_REV, DS1103_WRONG_BOARD_REV_TXT,"ds1103_adc_trigger_setup()",">= 9.0");
      RTLIB_EXIT(1);
    };
    /* ignore trigger setup(x,disable) for board rev < 9.0 */
    return; 
  }

  RTLIB_INT_SAVE_AND_DISABLE(int_status);
  
  if(state)
  {
    sync_ctrl_reg|= ( DS1103_SYNC_START_ADC1 << (converter-1));
    ds1103_sync_enabled |= (DS1103_STRB_ADC1 << (converter -1));
  }
  else
  {
    sync_ctrl_reg &= ~( DS1103_SYNC_START_ADC1 << (converter-1));
    ds1103_sync_enabled &= ~(DS1103_STRB_ADC1 << (converter -1));

  }
  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_adc_start (UInt16 mask)
*
* SYNTAX:
*      ds1103_adc_start (UInt16 mask)
*
* DESCRIPTION: This function starts the A/D converters.
*
* PARAMETERS: mask is the bitmask, which defines the converters.
*             To start the desired converters, the corresponding bits in the
*             mask must be set.
*
* RETURNS:    nothing
*
* NOTE:       set Bit 0 for converter 1 ..., Bit 7 for converter 8.
*
******************************************************************************/
__INLINE void ds1103_adc_start(UInt16 mask)
{

  *DS1103_IO_STROBE_REG =   ( mask & DS1103_ADC_START_MSK );

  RTLIB_FORCE_IN_ORDER();

}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_adc_delayed_start (UInt16 mask)
*
* SYNTAX:
*        ds1103_adc_delayed_start (UInt16 mask)
*
* DESCRIPTION: This function starts the A/D converters with a delay of 2 us
*              and can be called directly after adjusting a multiplexer.
*              advantage: never mind the necessary delay of 2 us after ad-
*                         justing the mux.
*              disadvantage: the PPC does nothing during the delay time.
*
*             usage:  ds1103_adc_mux_all(2, 6, 11, 15);
*                     ds1103_adc_delayed_start(DS1103_ADC_CH2 | DS1103_ADC_CH6
*                                          DS1103_ADC_CH11 | DS1103_ADC_CH15);
*
*
* PARAMETERS: mask is the bitmask, which defines the converters.
*             To start the desired converters, the corresponding bits in the
*             mask must be set.
*
* RETURNS:    nothing
*
* NOTE:       set Bit 0 for converter 1 ..., Bit 7 for converter 8.
*             Reduced delay time by board revisions >= 9.0 to 1 us.             
*
******************************************************************************/
__INLINE void ds1103_adc_delayed_start(UInt16 mask)
{
  if(ds1103_advanced_io)
    ds1103_delay_loop(DS1103_ADC_MUX_DELAY_ADVANCED)
  else
    ds1103_delay_loop(DS1103_ADC_MUX_DELAY);

  *DS1103_IO_STROBE_REG =   ( mask & DS1103_ADC_START_MSK );

  RTLIB_FORCE_IN_ORDER();

}






/******************************************************************************
*
* FUNCTION:
*   void ds1103_adc_mux(UInt16 channel)
*
* SYNTAX:
*       ds1103_adc_mux(UInt16 channel)
*
* DESCRIPTION: This function adjusts one of the for multiplexers to the desired
*              channel.
*
* PARAMETERS:  channel defines, which multiplexer is adjusted to the
*              corresponding A/D-channel.
*
* RETURNS:     nothing
*
* NOTE:        range of channel: 1...16
*
******************************************************************************/
__INLINE void ds1103_adc_mux(UInt16 channel)
{

  rtlib_int_status_t int_status;

   RTLIB_INT_SAVE_AND_DISABLE(int_status);

  switch(channel)
  {
    case 1: 
            adc_ctrl_bit.seladc1 = 0;
            break;

    case 2:
            adc_ctrl_bit.seladc1 = 1;
            break;

    case 3:
            adc_ctrl_bit.seladc1 = 2;
            break;

    case 4:
            adc_ctrl_bit.seladc1 = 3;
            break;

    case 5:
            adc_ctrl_bit.seladc2 = 0;
            break;

    case 6:
            adc_ctrl_bit.seladc2 = 1;
            break;

    case 7:
            adc_ctrl_bit.seladc2 = 2;
            break;

    case 8:
            adc_ctrl_bit.seladc2 = 3;
            break;

    case 9:
            adc_ctrl_bit.seladc3 = 0;
            break;

    case 10:
            adc_ctrl_bit.seladc3 = 1;
            break;

    case 11:
            adc_ctrl_bit.seladc3 = 2;
            break;

    case 12:
            adc_ctrl_bit.seladc3 = 3;
            break;

    case 13:
            adc_ctrl_bit.seladc4 = 0;
            break;

    case 14:
            adc_ctrl_bit.seladc4 = 1;
            break;

    case 15:
            adc_ctrl_bit.seladc4 = 2;
            break;

    case 16:
            adc_ctrl_bit.seladc4 = 3;
            break;

    default: break;
  }

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_adc_mux_all(UInt16 adc1_chan, UInt16 adc2_chan,
*                           UInt16 adc3_chan, UInt16 adc4_chan)
*
* SYNTAX:
*        ds1103_adc_mux_all(UInt16 adc1_chan, UInt16 adc2_chan,
*                           UInt16 adc3_chan, UInt16 adc4_chan)
*
* DESCRIPTION: This function adjusts all multiplexers to the desired
*              A/D-channels given by the parameters.
*
* PARAMETERS: adc1_chan is the desired A/D-channel for mux1,
*             adc2_chan is the desired A/D-channel for mux2,
*             adc3_chan is the desired A/D-channel for mux3,
*             adc4_chan is the desired A/D-channel for mux4.
*
* RETURNS:    nothing
*
* NOTE: range of adc1_chan: 1...4
*                adc2_chan: 5...8
*                adc3_chan: 9...12
*                adc4_chan: 13...16
*
******************************************************************************/
__INLINE void ds1103_adc_mux_all(UInt16 adc1_chan, UInt16 adc2_chan,
                                  UInt16 adc3_chan, UInt16 adc4_chan)
{

  ds1103_adcctrl_reg adcctrlmsk1, adcctrlmsk2;
  UInt32 msr_state;


  if(adc1_chan)
  {
    adcctrlmsk1._adcctrl_bit.seladc1 = (adc1_chan - 1);
    adcctrlmsk2._adcctrl_bit.seladc1 = 0; 
  }
  else
  {
    adcctrlmsk1._adcctrl_bit.seladc1 = 0;
    adcctrlmsk2._adcctrl_bit.seladc1 = 3;
  };
  
  if(adc2_chan) 
  {
    adcctrlmsk1._adcctrl_bit.seladc2 = (adc2_chan - 5);
    adcctrlmsk2._adcctrl_bit.seladc2 = 0; 
  }
  else
  {
    adcctrlmsk1._adcctrl_bit.seladc2 = 0;
    adcctrlmsk2._adcctrl_bit.seladc2 = 3;
  };


  if(adc3_chan)
  {
    adcctrlmsk1._adcctrl_bit.seladc3 = (adc3_chan - 9);
    adcctrlmsk2._adcctrl_bit.seladc3 = 0;
  }
  else
  {
    adcctrlmsk1._adcctrl_bit.seladc3 = 0;
    adcctrlmsk2._adcctrl_bit.seladc3 = 3;
  };


  if(adc4_chan)
  {
    adcctrlmsk1._adcctrl_bit.seladc4 = (adc4_chan - 13);
    adcctrlmsk2._adcctrl_bit.seladc4 = 0;
  }
  else
  {
    adcctrlmsk1._adcctrl_bit.seladc4 = 0;
    adcctrlmsk2._adcctrl_bit.seladc4 = 3;
  };


  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE();

  *DS1103_ADC_CTRL_REG = (*DS1103_ADC_CTRL_REG & adcctrlmsk2._adcctrl_reg)
                         | adcctrlmsk1._adcctrl_reg;


  RTLIB_FORCE_IN_ORDER();
  ds1103_set_interrupt_status(msr_state);

}





/******************************************************************************
*
* FUNCTION:
*   Float64 ds1103_adc_read_ch(UInt16 channel)
*
* SYNTAX:
*          ds1103_adc_read_ch(UInt16 channel)
*
* DESCRIPTION: This function reads the A/D-converter of the desired A/D-channel.
*
* PARAMETERS: channel defines the desired A/D-channel.
*
* RETURNS: the scaled value of the corresponding A/D-converter.
*
* NOTE: scale-range: -1.0 ... +1.0
*       The function reads
*          A/D-converter 1 for the channels 1...4 (multiplexed),
*          A/D-converter 2 for the channels 5...8 (multiplexed),
*          A/D-converter 3 for the channels 9...12 (multiplexed),
*          A/D-converter 4 for the channels 13...16 (multiplexed),
*          A/D-converter 5 for the channel 17,
*          A/D-converter 6 for the channel 18,
*          A/D-converter 7 for the channel 19,
*          A/D-converter 8 for the channel 20.
*       The function polls the corresponding busy flag.
*
******************************************************************************/
__INLINE Float64 ds1103_adc_read_ch(UInt16 channel)
{
  Float64 value;
  Int16 mask=0xFFFF;

  if((channel>16)&&(ds1103_advanced_io==0)) mask=0xFFF0;
#ifdef DEBUG_POLL
  DS1103_MSG_INFO_SET(DS1103_ADC_CH_POLL_START);
#endif
    while( ( ( *DS1103_ADC_CTRL_REG ) & ds1103_adc_chan1[channel] )
              != ds1103_adc_chan1[channel]); /* check busy flag */

#ifdef DEBUG_POLL
  DS1103_MSG_INFO_SET(DS1103_ADC_CH_POLL_END);
#endif
    RTLIB_FORCE_IN_ORDER();
    
      value = (Float64) ( DS1103_ADC_REG[ ds1103_adc_addr[channel] ]
                      & mask ) * DS1103_ADC_SCALE;
     RTLIB_FORCE_IN_ORDER();
  return( value );
}




/******************************************************************************
*
* FUNCTION:
*   Float64 ds1103_adc_read_conv(UInt16 converter)
*
* SYNTAX:
*        ds1103_adc_read_conv(UInt16 converter)
*
* DESCRIPTION: This function reads the desired A/D-converter.
*
* PARAMETERS: converter defines the desired A/D-converter.
*
* RETURNS: the scaled value of the A/D-converter.
*
* NOTE: scale-range: -1.0 ... +1.0
*       range of converter: 1...8
*       The function polls the corresponding busy flag.
*
******************************************************************************/
__INLINE Float64 ds1103_adc_read_conv(UInt16 converter)
{
  Float64 value;
  
  if(ds1103_advanced_io||(converter>4))
    {
    /* 16 Bit Converter */
  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_CONV_POLL_START);
  #endif
  
    while( ( (*DS1103_ADC_CTRL_REG) & ds1103_adc_convert[converter] )
              != ds1103_adc_convert[converter] ); /* check busy flag */

  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_CONV_POLL_END);
  #endif
    RTLIB_FORCE_IN_ORDER();
  /* read converter */
    value = (Float64) ( DS1103_ADC_REG[ ds1103_adc_conv_addr[converter] ]
                /*      & LocalMask  */) * DS1103_ADC_SCALE;
  
    RTLIB_FORCE_IN_ORDER();
    return( value );
    }
  else
    {
      /* 12 Bit Converter */
  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_CONV_POLL_START);
  #endif
  
    while( ( (*DS1103_ADC_CTRL_REG) & ds1103_adc_convert[converter] )
              != ds1103_adc_convert[converter] ); /* check busy flag */

  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_CONV_POLL_END);
  #endif
    RTLIB_FORCE_IN_ORDER();
  /* read converter */
    value = (Float64) ( DS1103_ADC_REG[ ds1103_adc_conv_addr[converter] ]
                      &  0xFFF0 ) * DS1103_ADC_SCALE;
  
    RTLIB_FORCE_IN_ORDER();

    return( value );
    }
}







/******************************************************************************
*
* FUNCTION:
*   ds1103_adc_read2(UInt16 converter, Float64 *value1, Float64 *value2)
*
* SYNTAX:
*        ds1103_adc_read2(UInt16 converter, Float64 *value1, Float64 *value2)
*
* DESCRIPTION: This function reads a pair of A/D-converters. This is faster than
*              calling two times the single-read function.
*
* PARAMETERS: converter defines the first of the two converters,
*             *value1  is the address, on which the scaled value of the first
*                      converter is written,
*             *value2  is the address, on which the scaled value of the second
*                      converter is written.
*
*
* RETURNS: nothing
*
* NOTE:    correct values for converter are
*              1 for reading converter 1 and 2,
*              3 for reading converter 3 and 4,
*              5 for reading converter 5 and 6,
*              7 for reading converter 7 and 8.
*       scale-range: -1.0 ... +1.0
*       The function polls the corresponding busy flags.
*
******************************************************************************/
__INLINE void ds1103_adc_read2(UInt16 converter, Float64 *value1,
                               Float64 *value2)
{

  UInt32 value;

  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_READ2_POLL_START);
  #endif

  /* check busy flag */
  while(  ( (*DS1103_ADC_CTRL_REG) & (ds1103_adc_convert[converter]
           + ds1103_adc_convert[converter+1]) ) !=
    (ds1103_adc_convert[converter] + ds1103_adc_convert[converter+1])  );

  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_READ2_POLL_END);
  #endif
  
  
  RTLIB_FORCE_IN_ORDER();

  value = *( (volatile UInt32*)( ( (UInt16*)DS1103_ADC ) + converter - 1) );
  
  if(!(ds1103_advanced_io||(converter>4)))
    value = value & 0xFFF0FFF0; 
    //ds1103_adc2_msk[converter];

  *value1 = (    (Float64)( (Int16)(value & 0xFFFF) ) * DS1103_ADC_SCALE );
  *value2 = (    (Float64)( (Int16)( value >> 16) ) * DS1103_ADC_SCALE );

  RTLIB_FORCE_IN_ORDER();

}



/******************************************************************************
*
* FUNCTION:
*   void ds1103_adc_read_all(Float64 *value1, Float64 *value2, Float64 *value3,
*                            Float64 *value4, Float64 *value5, Float64 *value6,
*                            Float64 *value7, Float64 *value8)
*
* SYNTAX:
*        ds1103_adc_read_all(Float64 *value1, Float64 *value2, Float64 *value3,
*                            Float64 *value4, Float64 *value5, Float64 *value6,
*                            Float64 *value7, Float64 *value8)
*
* DESCRIPTION: This function reads all converters.
*
* PARAMETERS:
*             *value1  is the address, on which the scaled value of the first
*                      converter is written,
*             *value2  is the address, on which the scaled value of the second
*                      converter is written,
*             *value3  is the address, on which the scaled value of the third
*                      converter is written,
*             *value4  is the address, on which the scaled value of the fourth
*                      converter is written,
*             *value5  is the address, on which the scaled value of the fifth
*                      converter is written,
*             *value6  is the address, on which the scaled value of the sixed
*                      converter is written,
*             *value7  is the address, on which the scaled value of the seventh
*                      converter is written,
*             *value7  is the address, on which the scaled value of the eighth
*                      converter is written
*       scale-range: -1.0 ... +1.0
*
* RETURNS: nothing
*
* NOTE:    The function polls the corresponding busy flags.
*
******************************************************************************/

__INLINE void ds1103_adc_read_all( Float64 *value1, Float64 *value2,
                                   Float64 *value3, Float64 *value4,
                                   Float64 *value5, Float64 *value6,
                                   Float64 *value7, Float64 *value8)
{
  UInt32 value;

  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_READ_ALL_12BIT_POLL_START);
  #endif
  
  /* poll the fast 12 Bit ADC's */
  while( ( (*(DS1103_ADC_CTRL_REG) ) & DS1103_ADC_BUSY_MSK_12B )
             != DS1103_ADC_BUSY_MSK_12B ) ;


  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_READ_ALL_12BIT_POLL_END);
  #endif
  

  RTLIB_FORCE_IN_ORDER();
  
  if(ds1103_advanced_io)
    value =*( DS1103_ADC_REG2 + 2 );
  else
    value =*( DS1103_ADC_REG2 + 2 )& 0xFFF0FFF0;
    
  
  *value5 = (    (Float64)( (Int16)(value & 0xFFFF) ) * DS1103_ADC_SCALE );
  *value6 = (    (Float64)( (Int16)( value >> 16) ) * DS1103_ADC_SCALE );

  if(ds1103_advanced_io)
    value =*( DS1103_ADC_REG2 + 3 );
  else
    value =*( DS1103_ADC_REG2 + 3 )& 0xFFF0FFF0;
  
  *value7 = (    (Float64)( (Int16)(value & 0xFFFF) ) * DS1103_ADC_SCALE );
  *value8 = (    (Float64)( (Int16)( value >> 16) ) * DS1103_ADC_SCALE );


  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_READ_ALL_16BIT_POLL_START);
  #endif

  /* poll the slower 16 Bit ADC's */
  while( ( (*(DS1103_ADC_CTRL_REG) ) & DS1103_ADC_BUSY_MSK_16B )
             != DS1103_ADC_BUSY_MSK_16B ) ;


  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_ADC_READ_ALL_16BIT_POLL_END);
  #endif


  RTLIB_FORCE_IN_ORDER();

  value =  *( DS1103_ADC_REG2 );

  *value1 = (    (Float64)( (Int16)(value & 0xFFFF) ) * DS1103_ADC_SCALE );
  *value2 = (    (Float64)( (Int16)( value >> 16) ) * DS1103_ADC_SCALE ); 

  value =  *( DS1103_ADC_REG2 + 1 );

  *value3 = (    (Float64)( (Int16)(value & 0xFFFF) ) * DS1103_ADC_SCALE );
  *value4 = (    (Float64)( (Int16)( value >> 16) ) * DS1103_ADC_SCALE );


}





/******************************************************************************
*
* FUNCTION:
*   void ds1103_adc_read_mux(UInt16* adc_scantable, UInt16 scantable_size,
*                            Float64* pvalues)
*
* SYNTAX:
*        ds1103_adc_read_mux(UInt16* adc_scantable, UInt16 scantable_size,
*                            Float64* pvalues)
*
* DESCRIPTION: This function allows to scan the A/D-channels of a multiplexer.
*              It does the whole procedure of adjusting the mux, starting the
*              right converter and reading of the converter.
*
* PARAMETERS:  *adc_scantable is the address to an array which includes the
*                             sequence of channels of a multiplexer,
*              scantable_size defines the size of adc_scantable,
*              *pvalues       is the address on which the scaled values of the
*                             scanned A/D-channels are written.
*
* RETURNS:   nothing
*
* NOTE:   The first channel defines the converter, which is used.
*         The function polls the corresponding busy flag.
*         The scantable must include channels of only one multiplexer:
*             1...4 for multiplexer 1,
*             5...8 for multiplexer 2,
*             9...12 for multiplexer 3,
*             13...16 for multiplexer 4.
*         scale-range: -1.0 ... +1.0
*
******************************************************************************/
__INLINE void ds1103_adc_read_mux(UInt16* adc_scantable,
                                  UInt16 scantable_size, Float64* pvalues)
{

  UInt16 converter, table_ind;
  rtlib_tic_t mux_start;
  Float64 mux_delay;
    
  ds1103_adc_mux(adc_scantable[0]);
  mux_start=RTLIB_TIC_COUNT();

  if(ds1103_advanced_io)
    mux_delay=DS1103_ADC_MUX_DELAY_ADVANCED;
  else
    mux_delay=DS1103_ADC_MUX_DELAY;
  converter = ds1103_adc_chan1[ adc_scantable[0] ];
  
  while(RTLIB_TIC_ELAPSED(mux_start)< mux_delay) {};
  
  ds1103_adc_start(converter);
      
  for(table_ind = 1; table_ind < scantable_size; table_ind++)
  {
    ds1103_adc_mux(adc_scantable[table_ind]); /* adjust mux for next value */
    mux_start=RTLIB_TIC_COUNT();
    pvalues[table_ind - 1] = ds1103_adc_read_ch( adc_scantable[table_ind - 1] );
    while(RTLIB_TIC_ELAPSED(mux_start)< mux_delay) {};
    ds1103_adc_start(converter);
  };
   pvalues[table_ind - 1] = ds1103_adc_read_ch( adc_scantable[table_ind - 1] );
}


/******************************************************************************
*
* FUNCTION:
*   void ds1103_adc_multi_conv_read (UInt32 channel_mask, Float64 pvalues[20])
*
* SYNTAX:
*        ds1103_adc_multi_conv_read (UInt32 channel_mask, Float64 pvalues[20])
*                            
*
* DESCRIPTION: This function scans selectable A/D channels of different AD-Converters.
*              It is performance optimized by starting the conversion of each selected adc 
*              simultaneous. 
*
* PARAMETERS:  channel_mask  Bit-Mask for selecting the ADC Channels.
*                            Example:  DS1103_ADC_CHANNEL1 | DS1103_ADC_CHANNEL5
*
*              pvalues[20]   is the address on which the scaled values of the
*                            scanned A/D-channels are written. Convertion result entries
*                            of non-masked channels will be left untouched.
*
* RETURNS:   nothing
*
* NOTE:   - This function should not be used for ADCs with external trigger mode setting.
*         - The function polls the corresponding busy flag.
*         - The channel_mask can include channels of all multiplexed and 
*           non-multipexed channels:
*             DS1103_ADC_CHANNEL1...DS1103_ADC_CHANNEL4 for ADC 1,
*             DS1103_ADC_CHANNEL5...DS1103_ADC_CHANNEL8 for ADC 2,
*             DS1103_ADC_CHANNEL9...DS1103_ADC_CHANNEL12 for ADC 3,
*             DS1103_ADC_CHANNEL13..DS1103_ADC_CHANNEL16 for ADC 4.
*		      DS1103_ADC_CHANNEL17..DS1103_ADC_CHANNEL20 for non-multiplexed ADC 5-8.
*
*         - scale-range: -1.0 ... +1.0
*         - Do not use the constants DS1103_ADC_CH1 - DS1103_ADC_CH20 
*           because they are different mapped !
*
******************************************************************************/
__INLINE void ds1103_adc_multi_conv_read(UInt32 channel_mask,Float64 *pvalues)
{
  UInt16 i;
  UInt32 temp_mask;
  UInt32 sample_pos;
  UInt16 val=0;
  UInt16 adc;
  UInt16 mux_mask_or;
  UInt16 mux_mask_and;
  UInt16 mux_mask_hw;
  UInt16 mux_mask;
  UInt16 adc_start_seperate;

  UInt16 adc_mask;
  UInt16 adc_mask_non_multiplexed;
  UInt16 adc_mask_multiplexed;
  UInt16 mux_mask_pos;

  UInt16 adc_act_mask;
  UInt32 msr_state;
  UInt32 value_32;

  UInt16 turn;
  UInt16 mux_mask_and_map[5];
  UInt16 mux_mask_or_map[5];
  UInt16 adc_mask_map[5];
  UInt16 pos[5];
  Float64 mux_delay;
  rtlib_tic_t mux_start=0;

  volatile UInt32 *adc_result_reg_32;
  Float64 *p_act_value;


  mux_start=RTLIB_TIC_COUNT();
  if(ds1103_advanced_io)
    {
      mux_delay=DS1103_ADC_MUX_DELAY_ADVANCED;
    }
    else
    {
      mux_delay=DS1103_ADC_MUX_DELAY;
    };

    /* calculate scantable */
    for ( i = 0; i < 5; i++ )
    {
        pos[i] = 0;
        adc_mask_map[i] = 0;
        mux_mask_and_map[i] = 0xFF00;
        mux_mask_or_map[i] = 0;
    };
    sample_pos = 1;
    temp_mask = channel_mask;
    adc = 0;
    mux_mask_pos = 0x300;
 
    /* calculate scantable for adc with mux */
    for ( i = 0; i < 16; i++ )  
    {
        if ( temp_mask & 0x01 )
        {
            val = i & ( 0x03 );
            mux_mask_and_map[pos[adc]] &= ~( mux_mask_pos );
            mux_mask_or_map[pos[adc]] |= ( (val) << (adc * 2 + 8) );
            adc_mask_map[pos[adc]] |= ( 1 << adc );
            pos[adc]++;
        };
        if ( (i & 0x03) == 0x03 )
        {
            adc++;
            mux_mask_pos <<= 2;
        };
        temp_mask >>= 1;
        sample_pos <<= 1;
    }
    /* calculate scantable for adc without mux */
    if( temp_mask )
    {
        adc_mask_map[0] |= ( temp_mask << 4 );
    }

/* start scantable */
 
  turn = 0;
  adc_start_seperate = 0;
  adc_mask = adc_mask_map[0];
  while ( adc_mask )
  {

    if ( turn == 0 )
    {
    /* first turn */
        if ( adc_mask & 0x0F )
        {
            /* setting mux if necessary */
            msr_state = ds1103_get_interrupt_status();
            DS1103_GLOBAL_INTERRUPT_DISABLE();
            mux_mask_hw = ( *(DS1103_ADC_CTRL_REG) );
            mux_mask = ( (mux_mask_hw & mux_mask_and_map[turn]) | mux_mask_or_map[turn] );
            if ( mux_mask != (mux_mask_hw & 0xFF00) )
            {
                ( *DS1103_ADC_CTRL_REG ) = mux_mask;
                adc_start_seperate = 1;
                /* capture time */
                mux_start=RTLIB_TIC_COUNT(); 
            }
            RTLIB_FORCE_IN_ORDER();
            ds1103_set_interrupt_status( msr_state );
        };

        if ( adc_start_seperate == 0 )
        {
            /* starting all adc */
            ds1103_adc_start( adc_mask );   
        }

        adc_mask_non_multiplexed = adc_mask & 0xF0;
        if ( adc_mask_non_multiplexed )
        {
            if ( adc_start_seperate )
            {   /* start fast non-multipexed ADC's: */
                ds1103_adc_start( adc_mask_non_multiplexed );
            }

            /* we have to wait for non-multipexed ...*/
#ifdef DEBUG_POLL
DS1103_MSG_INFO_SET(DS1103_ADC_CH_POLL_START);
#endif
            while ( ((*DS1103_ADC_CTRL_REG) & adc_mask_non_multiplexed) != adc_mask_non_multiplexed );
#ifdef DEBUG_POLL
DS1103_MSG_INFO_SET(DS1103_ADC_CH_POLL_END);
#endif
            adc_result_reg_32 = DS1103_ADC_REG2;
            adc_result_reg_32 += 2;
            p_act_value = pvalues + 16;
            adc_act_mask = adc_mask >> 4;

            /* read the fast 12 BIT ADC's: */
            for ( i = 3; i < 5; i++ )
            {
                if ( adc_act_mask & 0x03 )
                {
                    value_32 = *( adc_result_reg_32 );
                  
                    if(ds1103_advanced_io==0) value_32 = value_32 & 0xFFF0FFF0; 
                    if ( adc_act_mask & 0x01 )
                    {
                        p_act_value[0] = ( (Float64) ((Int16) (value_32 & 0xFFFF)) * DS1103_ADC_SCALE );

                    }
                    if ( adc_act_mask & 0x02 )
                    {
                        p_act_value[1] = ( (Float64) ((Int16) (value_32 >> 16)) * DS1103_ADC_SCALE );
                    }
                }
                adc_act_mask >>= 2;
                p_act_value += 2;
                adc_result_reg_32++;
            }; /* end of for-loop */
        }; /* end of adc_mask_non_multiplexed */

     }; /* end of turn 0 */

    
    adc_mask_multiplexed = adc_mask & 0x0F;
    if ( adc_mask_multiplexed )
    {
        if ( (adc_start_seperate) )
        {
            while(RTLIB_TIC_ELAPSED(mux_start)< mux_delay) {};
            ds1103_adc_start( adc_mask_multiplexed );
        }
        adc_start_seperate=1;
        mux_mask_or = 0xFFFF;
        mux_mask_and = 0;
        if ( adc_mask_map[turn + 1] )
        {
        /* setting mux_mask for next turn */ 
            mux_mask_and = mux_mask_and_map[turn + 1];
            mux_mask_or = mux_mask_or_map[turn + 1];
        }
        else
        {
            if ( turn )
            {
                /* setting mux_mask for first turn */ 
                mux_mask_and = mux_mask_and_map[0];
                mux_mask_or = mux_mask_or_map[0];
            }           
  
        }
        if ( mux_mask_or != 0xFFFF )
        {
            msr_state = ds1103_get_interrupt_status();
            DS1103_GLOBAL_INTERRUPT_DISABLE();
            ( *DS1103_ADC_CTRL_REG ) = ( ((*(DS1103_ADC_CTRL_REG)) & mux_mask_and) | mux_mask_or );
            RTLIB_FORCE_IN_ORDER();
            ds1103_set_interrupt_status( msr_state );
            /* capture time */
            mux_start=RTLIB_TIC_COUNT();
        }

        /* poll the slow 16 BIT ADC's: */
 #ifdef DEBUG_POLL
DS1103_MSG_INFO_SET(DS1103_ADC_CH_POLL_START);
#endif
        while ( ((*DS1103_ADC_CTRL_REG) & adc_mask_multiplexed) != adc_mask_multiplexed );
#ifdef DEBUG_POLL
DS1103_MSG_INFO_SET(DS1103_ADC_CH_POLL_END);
#endif

        /* read the slow 16 BIT ADC's: */
        p_act_value = pvalues;
        adc_result_reg_32 = DS1103_ADC_REG2;
        mux_mask_or = ( mux_mask_or_map[turn] ) >> 8;
      
        for ( i = 0; i < 2; i++ )
        {
            if ( adc_mask_multiplexed & 0x03 )
            {   
                value_32 = *( adc_result_reg_32 );
                
                if ( adc_mask_multiplexed & 0x01 )
                {
                    p_act_value[mux_mask_or & 0x3] = ( (Float64) ((Int16) (value_32 & 0xFFFF)) * DS1103_ADC_SCALE );
                }
                if ( adc_mask_multiplexed & 0x02 )
                {
                    p_act_value[( (mux_mask_or >> 2) & 0x3 ) + 4] =
                        ((Float64) ((Int16) (value_32 >> 16)) * DS1103_ADC_SCALE);
                }
            }
            mux_mask_or >>= 4;
            adc_mask_multiplexed >>= 2;
            p_act_value += 8;
            adc_result_reg_32++;
        };

        //for
    };

    //if adc_multiplexed
    turn++;
    adc_mask = adc_mask_map[turn];
  }   //while
}



/*************************************************************************dac*/

/******************************************************************************
*
* FUNCTION:
*   void ds1103_dac_init (UInt16 dac_mode)
*
* SYNTAX:
*        ds1103_dac_init (UInt16 dac_mode)
*
* DESCRIPTION: This function initialized the 8 DAC's.
*
* PARAMETERS: dac_mode defines the operating-mode of the DAC's:
*             DS1103_DACMODE_LATCHED (0) for the latched mode,
*             DS1103_DACMODE_TRANSPARENT (1) for the transparent mode.
*
* RETURNS: nothing
*
* NOTE: if the latched mode is choosed, the DAC's must be strobed after writing.
*       In the transparent mode the written value appears directly at the output.
*
******************************************************************************/
__INLINE void ds1103_dac_init (UInt16 dac_mode)
{
  UInt16 x;
  rtlib_int_status_t int_status;


   RTLIB_INT_SAVE_AND_DISABLE(int_status);

  ( *DS1103_RESET_REG ) = ( *DS1103_RESET_REG ) & 0xFFFE;

  for(x=0; x<=3;x++)
    DS1103_DAC_REG2[x] = 0x80008000;

  setup_bit.dactr = dac_mode;

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

}



/******************************************************************************
*
* FUNCTION:
*   void ds1103_dac_write(UInt16 channel, Float64 value)
*
* SYNTAX:
*       ds1103_dac_write(UInt16 channel, Float64 value)
*
* DESCRIPTION: This function writes a value to the desired DAC-channel.
*
* PARAMETERS: channel defines the DAC-channel,
*             value is the value which is scaled and written to the DAC.
*
* RETURNS: nothing
*
* NOTE: range of channel: 1...8
*       range of value: -1.0 ... +1.0
*                       -1.0 appears as -10.0 V at the output,
*                       +1.0 as 10.0 V.
*
******************************************************************************/
__INLINE void ds1103_dac_write(UInt16 channel, Float64 value)
{

  if (value < DS1103_DAC_MIN) value = DS1103_DAC_MIN;
  if (value > DS1103_DAC_MAX) value = DS1103_DAC_MAX; 
   
  DS1103_DAC_REG[ ds1103_dac_ind1[channel] ] =
       ( (Int16)(value * (Float64)DS1103_DAC_SCALE) ) ^ (UInt16)0x8000;

  
  RTLIB_FORCE_IN_ORDER();

}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_dac_write2(UInt16 channel, Float64 value1, Float64 value2)
*
* SYNTAX:
*      ds1103_dac_write2(UInt16 channel, Float64 value1, Float64 value2)
*
* DESCRIPTION: This function writes two values to two neighboured DAC's.
*
* PARAMETERS: channel is the first of the neighboured DAC's,
*             value1 is the value which is scaled and written to the first DAC-
*                    channel defined by parameter channel,
*             value2 is the value which is scaled and written to the second DAC-
*                    channel (channel + 1).
* RETURNS: nothing
*
* NOTE: senseful values for channel are: 1 (channel 1 and 2 are written),
*                                        3 (channel 3 and 4 are written),
*                                        5 (channel 5 and 6 are written),
*                                        7 (channel 7 and 8 are written).
*       range of value1 or value2 : -1.0 ... +1.0
*                       -1.0 appears as -10.0 V at the output,
*                       +1.0 as 10.0 V.
*
******************************************************************************/
__INLINE void ds1103_dac_write2(UInt16 channel, Float64 value1, Float64 value2)
{

  if (value1 < DS1103_DAC_MIN) value1 = DS1103_DAC_MIN;
  if (value1 > DS1103_DAC_MAX) value1 = DS1103_DAC_MAX;

  if (value2 < DS1103_DAC_MIN) value2 = DS1103_DAC_MIN;
  if (value2 > DS1103_DAC_MAX) value2 = DS1103_DAC_MAX;


  DS1103_DAC_REG2[ ds1103_dac_ind2[channel] ] =
           (UInt32)((UInt16)(value1 * DS1103_DAC_SCALE)^0x8000)
       | (((UInt32)((UInt16)(value2 * DS1103_DAC_SCALE)^0x8000)) << 16);

  RTLIB_FORCE_IN_ORDER();


}



/******************************************************************************
*
* FUNCTION:
*   void ds1103_dac_reset(void)
*
* SYNTAX:
*       ds1103_dac_reset(void)
*
* DESCRIPTION: This function resets the 8 DAC's.
*
* PARAMETERS: nothing
*
* RETURNS: nothing
*
* NOTE: after the reset the DAC's are set to zero (0 V).
*
******************************************************************************/
__INLINE void ds1103_dac_reset(void)
{

  rtlib_int_status_t int_status;

   RTLIB_INT_SAVE_AND_DISABLE(int_status);

  ( *DS1103_RESET_REG ) = ( *DS1103_RESET_REG ) | 1;

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

}
  
  

 


/******************************************************************************
*
* FUNCTION:
*   void ds1103_dac_strobe(void)
*
* SYNTAX:
*       ds1103_dac_strobe(void)
*
* DESCRIPTION: This function strobes the DAC's (latched mode).
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE: This function should only be used in the latched mode.
*
******************************************************************************/
__INLINE void ds1103_dac_strobe(void)
{

  ( *DS1103_IO_STROBE_REG ) = DS1103_DAC_STROBE;

  RTLIB_FORCE_IN_ORDER();

}


/*******************************************************************************
*
*  FUNCTION
*
*  SYNTAX
*    void ds1103_dac_trigger_setup(UInt16 state)
*
*  DESCRIPTION
*    This function enables/disables the external trigger for all
*    DA converter. The converter can be strobed by the T1PWM pin of the
*    slave-DSP, which is also available on the DS1103 bracket.
*
*  PARAMETERS
*    state     : enables or disables external trigger
*                  DS1103_TRIGGER_DISABLE (0): disables external trigger
*                  DS1103_TRIGGER_ENABLE (1) : enables external trigger
*
*  RETURNS
*
*           Only available with board revisions >= 9.0.
*           The function return an error message, if the trigger is set to enable 
*           on board revisions < 9.0. Calling trigger_setup with state= 
*           DS1103_TRIGGER_DISABLE will be ignored for board revisions < 9.0
*
*******************************************************************************/

__INLINE void ds1103_dac_trigger_setup(UInt16 state)
{
  rtlib_int_status_t int_status;

  if(!ds1103_advanced_io)
  {
    if(state)
    {
      msg_error_printf(MSG_SM_RTLIB, DS1103_WRONG_BOARD_REV, DS1103_WRONG_BOARD_REV_TXT,"ds1103_dac_trigger_setup()",">= 9.0");
      RTLIB_EXIT(1);
    };
     /* ignore disable trigger for bord revision < 9.0 */
    return;
  }

   RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if(state)
  { 
     // enables external trigger
     sync_ctrl_reg|= DS1103_SYNC_EXT_STRB_DAC;
     ds1103_sync_enabled |=DS1103_STRB_DAC;
 
  }
  else
  {  
    // disables external trigger
     sync_ctrl_reg&= ~DS1103_SYNC_EXT_STRB_DAC;
     ds1103_sync_enabled &=~(DS1103_STRB_DAC);
  }
  RTLIB_INT_RESTORE(int_status);
  RTLIB_FORCE_IN_ORDER();
}





/**************************************************************************inc*/



/*******************************************************************************
*
*  FUNCTION
*    void ds1103_inc_trigger_setup(UInt16 channel, UInt16 state)
*  SYNTAX
*    void ds1103_inc_trigger_setup(UInt16 channel, UInt16 state)
*
*  DESCRIPTION
*    This function enables/disables the external trigger for the specified
*    incremental encoder channel. The encoder can be strobed by the T1PWM pin
*    of the slave-DSP, which is also available on the DS1103 bracket.
*
*  PARAMETERS
*    channel : incremental encoder channel number (1..7)
*    state     : enables or disables external trigger
*                  DS1103_TRIGGER_DISABLE (0): disables external trigger
*                  DS1103_TRIGGER_ENABLE (1) : enables external trigger
*
*  RETURNS
*
*  REMARKS
*
*           Only available with board revisions >= 9.0.
*           The function return an error message, if the trigger is set to enable 
*           on board revisions < 9.0. Calling trigger_setup with state= 
*           DS1103_TRIGGER_DISABLE will be ignored for board revisions < 9.0
*
*           The external trigger for the hanning chip ( channel 6 and 7) are combined 
*           and can only be enable/disable together.
*
*            
*******************************************************************************/

__INLINE void ds1103_inc_trigger_setup(UInt16 channel, UInt16 state)
{
  rtlib_int_status_t int_status;

  if(!ds1103_advanced_io)
  {
    if(state)
    {
      msg_error_printf(MSG_SM_RTLIB, DS1103_WRONG_BOARD_REV, DS1103_WRONG_BOARD_REV_TXT,"ds1103_inc_trigger_setup()",">= 9.0");
      RTLIB_EXIT(1);
    };
    /* ignore disable trigger for bord revision < 9.0 */
    return;
  }

  /* The hanning chip can only trigger for both channels*/
  if(channel==7) channel=6;  
  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if(state)
  {
    sync_ctrl_reg|= ( DS1103_SYNC_EXT_STRB_INC1 << (channel-1));
    ds1103_sync_enabled |= (DS1103_STRB_INC1 << (channel-1));
  }
  else
  {
    sync_ctrl_reg &= ~( DS1103_SYNC_EXT_STRB_INC1 << (channel-1));
    ds1103_sync_enabled &= ~(DS1103_STRB_INC1 << (channel-1));
  }

  
  RTLIB_INT_RESTORE(int_status);

  RTLIB_FORCE_IN_ORDER();
}




#ifndef _INLINE


/******************************************************************************
*
* FUNCTION:
*   void ds1103_inc_init(UInt16 channel, UInt16 inc_mode)
*
* SYNTAX:
*       ds1103_inc_init(UInt16 channel, UInt16 inc_mode)
*
* DESCRIPTION: The function inits the desired incremental encoder channel.
*
* PARAMETERS:  channel defines the incremental encoder channel which is
*                      initialized,
*              inc_mode is a bitmask, which defines the operating mode of the
*                       incremental channel.
*
*                       For the digital channels 1 to 5, the inc_mode-bit
*                       bit0 defines, if the channel-interface works in the
*                       TTL (bit0 = 0) or in the RS422 mode (bit0 = 1).
*                       The other bits have no meaning.
*
*                       For the digital channel 6 and the analogue channel 7 the
*                       bit0 and bit1 defines the counting-mode of the channel.
*                       Because of the dependencies between the operating modes
*                       of channel6 and channel7, adjusting of one of them has
*                       influence on the mode of the other channel.
*
* bit0 + bit1:   00 : inc_mode = DS1103_INC_CH6_16BIT | DS1103_INC_CH7_38BIT |...
*                     channel6  = 16 Bit counter
*                     channel7  = 38 Bit counter ( 32 bit + 6 bit finecount)
*
*                01 : inc_mode = DS1103_INC_CH6_32BIT | DS1103_INC_CH7_22BIT |...
*                     channel6  = 32 Bit counter
*                     channel7  = 22 Bit counter ( 16 bit + 6 bit finecount)
*
*                11 : inc_mode = DS1103_INC_CH6_24BIT | DS1103_INC_CH7_22BIT |...
*                     channel6  = 24 Bit counter
*                     channel7  = 22 Bit counter ( 16 bit + 6 bit finecount)
*
*  The analogue channel 7 has an 6 bit finecount, which counts from 0 to its
*  maximum during one electrical period.
*
*                       For the digital channel 6 bit2 defines if the interface
*                       of channel 6 works in the TTL (bit2 = 0) or in the RS422
*                       mode (bit2 = 1).
*
*    bit 2:      0xx: TTL, inc_mode = ... | DS1103_INC_CH6_TTL ... |
*                1xx: RS422, inc_mode = ... | DS1103_INC_CH6_RS422 ... |
*
*                       For the analogue channel 7 bit2 defines if the interface
*                       of channel 7 works in the Vpp (bit2 = 0) or in the uA
*                       mode (bit2 = 1).
*
*    bit 2:      0xx: uA, inc_mode = ... | DS1103_INC_CH7_uA ... |
*                1xx: Vpp, inc_mode = ... | DS1103_INC_CH7_VPP ... |
*
* The other bits have no meaning.
*
* RETURNS: nothing
*
* NOTE: Io1103.h includes predefinitions of the different modes. Using the
*       logical or of these definitions makes it easier to handle the parameter
*       inc_mode.
*
******************************************************************************/

 void ds1103_inc_init(UInt16 channel, UInt16 inc_mode)
{

   UInt16 cntcon;
   static UInt16 hanning_reset = 0;
   rtlib_int_status_t int_status;

    RTLIB_INT_SAVE_AND_DISABLE(int_status);

   /* channel 1-5: Bit0: TTL/RS422
      channel 6:   Bit2: TTL/RS422
      channel 7:   Bit2: uA/Vpp */

   switch( channel )
   {
       case 1: ds1103_inc_reset(1);
               setup_bit.incmode1 = inc_mode & DS1103_BITSELECT0;
               setup_bit.rstonidx1 = DS1103_INC_NO_RESETONIDX;
               ds1103_inc_rstonidx[1] = 0;
         break;

       case 2: ds1103_inc_reset(2);
               setup_bit.incmode2 = inc_mode & DS1103_BITSELECT0;
               setup_bit.rstonidx2 = DS1103_INC_NO_RESETONIDX;
               ds1103_inc_rstonidx[2] = 0;
         break;

       case 3: ds1103_inc_reset(3);
               setup_bit.incmode3 = inc_mode & DS1103_BITSELECT0;
               setup_bit.rstonidx3 = DS1103_INC_NO_RESETONIDX;
               ds1103_inc_rstonidx[3] = 0;
         break;

       case 4: ds1103_inc_reset(4);
               setup_bit.incmode4 = inc_mode & DS1103_BITSELECT0;
               setup_bit.rstonidx4 = DS1103_INC_NO_RESETONIDX;
               ds1103_inc_rstonidx[4] = 0;
         break;

       case 5: ds1103_inc_reset(5);
               setup_bit.incmode5 = inc_mode & DS1103_BITSELECT0;
               setup_bit.rstonidx5 = DS1103_INC_NO_RESETONIDX;
               ds1103_inc_rstonidx[5] = 0;
         break;

       case 6: /* avoids second reset of the hanning chip */
               if( hanning_reset == 0 )
               {
                 hanning_reset = 1;
                 ds1103_inc_reset(6);
                 inc_cntcon_reg = 0;
                 inc_hanning->syscon = 0;
               };

               /* TTL or RS422 */
               setup_bit.incmode6 = (inc_mode & DS1103_BITSELECT2) >> 2;
               ds1103_inc_rstonidx[6] = 0;
               cntcon = inc_cntcon_reg;
               cntcon = (cntcon & (~DS1103_HANINC_CT2ENC) ) & (~DS1103_HANINC_CT1ENC);

              /* Enc.2: 16/32 Bit */
              if( (inc_mode & DS1103_BITSELECT0) != DS1103_BITSELECT0 )
              {

               /* Ch7 = 32 bit + 6 bit, switch enc.1 to counter 1 */
               /* Ch6 = 16 bit , switch enc.2 to counter 2 */
                cntcon = cntcon | DS1103_HANINC_CT2ENC;
                ds1103_inc_ch6_mode = DS1103_INC_CH6_16BIT;
                ds1103_inc_ch7_mode = DS1103_INC_CH7_38BIT;
              }
              else
              {
                cntcon = cntcon | DS1103_HANINC_CT1ENC;
                ds1103_inc_ch7_mode = DS1103_INC_CH7_22BIT;

                if( (inc_mode & DS1103_BITSELECT1) != DS1103_BITSELECT1)
                  ds1103_inc_ch6_mode = DS1103_INC_CH6_32BIT;
                else ds1103_inc_ch6_mode = DS1103_INC_CH6_24BIT;
              };

              /* fourfold-sampling */
              cntcon = cntcon | DS1103_HANINC_CT1_FOURFOLD
                       | DS1103_HANINC_CT2_FOURFOLD;
              inc_cntcon_reg = cntcon;

              break;

       case 7: /* avoids second reset of the hanning chip */
               if( hanning_reset == 0 )
               {
                 hanning_reset = 1;
                 ds1103_inc_reset(7);
                 inc_cntcon_reg = 0;
                 inc_hanning->syscon = 0;
               };
               setup_bit.incmode7 = (inc_mode & DS1103_BITSELECT2) >> 2;
               ds1103_inc_rstonidx[7] = 0;
               cntcon = inc_cntcon_reg;
               cntcon = (cntcon & (~DS1103_HANINC_CT2ENC) ) & (~DS1103_HANINC_CT1ENC);

               /* Enc.2: 16/32 Bit */
               if( (inc_mode & DS1103_BITSELECT0) != DS1103_BITSELECT0)
               {
                 /* Ch6 = 16 bit , switch enc.2 to counter 2 */
                 /* Ch7 = 32 bit + 6 bit , switch enc.1 to counter 1 */
                 cntcon = cntcon | DS1103_HANINC_CT2ENC;
                 ds1103_inc_ch6_mode = DS1103_INC_CH6_16BIT;
                 ds1103_inc_ch7_mode = DS1103_INC_CH7_38BIT;
               }
               else
               {
                 /* Ch6 = 32 bit, switch enc.2 to counter 1 */
                 /* Ch7 = 16 bit , switch enc.1 to counter 2 */
                 cntcon = cntcon | DS1103_HANINC_CT1ENC;
                 ds1103_inc_ch7_mode = DS1103_INC_CH7_22BIT;
                 ds1103_inc_ch6_mode = DS1103_INC_CH6_32BIT
                                      | ds1103_inc_ch6_mode;
               };

               /* LOADMODE0, Load Disable, fourfold-sampling */
               cntcon = cntcon | DS1103_HANINC_CT1_FOURFOLD
                               | DS1103_HANINC_CT2_FOURFOLD;
               inc_cntcon_reg = cntcon;
               
               /* clear interrupt */
               ds1103_reset_interrupt_flag(DS1103_INT_INC_ENC_CH7);
               break;

       default: break;

   }; /* switch */

   RTLIB_FORCE_IN_ORDER();
   RTLIB_INT_RESTORE(int_status);

}


#endif


/******************************************************************************
*
* FUNCTION:
*   void ds1103_inc_set_idxmode(UInt16 channel, UInt16 idx_mode)
*
* SYNTAX:
*   void ds1103_inc_set_idxmode(UInt16 channel, UInt16 idx_mode)
*
* DESCRIPTION: This function enables/disables the Reset-on-Index-Mode.
*
* PARAMETERS:  channel: defines the incrementalencoder channel (range: 1...7)
*              idx_mode: DS1103_INC_CHx_NO_RESETONIDX (0)
*                         disables the Reset-on-Index
*                        DS1103_INC_CHx_RESETONIDX (1)
*                         enables the Reset-on-Index
*
* RETURNS: nothing
*
* Example:  ds1103_inc_set_idxmode( 1,  DS1103_INC_CH1_NO_RESETONIDX )
*           sets the incrementalencoder channel 1 to no reset on index
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_inc_set_idxmode(UInt16 channel, UInt16 idx_mode)
{
  UInt16 cntcon;
  rtlib_int_status_t int_status;


   RTLIB_INT_SAVE_AND_DISABLE(int_status);

  switch(channel)
  {

   case 1: 
           setup_bit.rstonidx1 = idx_mode;
           ds1103_inc_rstonidx[1] = idx_mode;
      break;

   case 2: 
           setup_bit.rstonidx2 = idx_mode;
           ds1103_inc_rstonidx[2] = idx_mode;
      break;

   case 3: 
           setup_bit.rstonidx3 = idx_mode;
           ds1103_inc_rstonidx[3] = idx_mode;
      break;

   case 4: 
           setup_bit.rstonidx4 = idx_mode;
           ds1103_inc_rstonidx[4] = idx_mode;
      break;

   case 5: 
           setup_bit.rstonidx5 = idx_mode;
           ds1103_inc_rstonidx[5] = idx_mode;
      break;

   case 6: 
           ds1103_inc_rstonidx[6] = idx_mode;
           cntcon =  ( inc_cntcon_reg
                       & ds1103_haninc_ldmde_msk[0][ds1103_inc_ch6_mode] )
                       & ds1103_haninc_idxmde_msk[0][ds1103_inc_ch6_mode];

           if( idx_mode == DS1103_INC_CH6_RESETONIDX )
           {

             /* load the latch with 0 */
             if(ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT)
               inc_hanning->ct2dat =  0;
             else
             {
               inc_hanning->ct1datl =  0;
               inc_hanning->ct1dath =  0;
             }; 

             cntcon = cntcon  | ds1103_haninc_ldmde[0][ds1103_inc_ch6_mode]
                              | ds1103_haninc_idx_mde[0][ds1103_inc_ch6_mode];
           };

          inc_cntcon_reg = cntcon;
          break;

   case 7: 
           ds1103_inc_rstonidx[7] = idx_mode;
           cntcon = ( inc_cntcon_reg
                      & ds1103_haninc_ldmde_msk[1][ds1103_inc_ch7_mode] )
                      & ds1103_haninc_idxmde_msk[1][ds1103_inc_ch7_mode];

           if( idx_mode == DS1103_INC_CH7_RESETONIDX )
           {
             
             /* load the latch with 0 */
             if(ds1103_inc_ch6_mode == DS1103_INC_CH7_22BIT)
               inc_hanning->ct2dat =  0;
             else
             {
               inc_hanning->ct1datl =  0;
               inc_hanning->ct1dath =  0;
             }; 

             cntcon = cntcon | ds1103_haninc_ldmde[1][ds1103_inc_ch7_mode]
                           | ds1103_haninc_idx_mde[1][ds1103_inc_ch7_mode];

           };

           inc_cntcon_reg = cntcon;
           break;

   default: break;

  }/*switch*/

  ds1103_inc_rstonidx_state[channel] = idx_mode;

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

}





/******************************************************************************
*
* FUNCTION:
*   void ds1103_inc_reset(UInt16 channel)
*
* SYNTAX:
*     ds1103_inc_reset(UInt16 channel)
*
* DESCRIPTION: The function resets the incremental encoder channels 1 - 5 or the
*              incremental encoder interface 2 ( digital channel 6 and analogue
*              channel 7).
*
*
* PARAMETERS:  channel : logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_inc_reset(UInt16 channel)
{
   rtlib_int_status_t int_status;

    RTLIB_INT_SAVE_AND_DISABLE(int_status);

   if(channel < 6)
   {
     ds1103_inc_counter[channel] = 0;
     /* resets the dig. counter */
     *DS1103_DIGINC_RESET_REG = ( 1 << (channel - 1) );
   }
   else   /*for ch6 + ch7 */
   {
     ds1103_inc_counter[6] = 0;
     ds1103_inc_counter[7] = 0;
     ds1103_fine_count = 0;
     *DS1103_RESET_REG = *DS1103_RESET_REG | 8;
     *DS1103_RESET_REG = *DS1103_RESET_REG & 0xFFF7;
     ds1103_inc_counter_clear(6);
     ds1103_inc_counter_clear(7);
     ds1103_inc_ch6_mode = 0;
     ds1103_inc_ch7_mode = 0;

     /* counter2 = normal 16 bit counter */
     inc_hanning->lincnt = 0xFFFF;

   };

   RTLIB_FORCE_IN_ORDER();
   RTLIB_INT_RESTORE(int_status);

}


     


/******************************************************************************
*
* FUNCTION:
*   Float64 ds1103_inc_position_read(UInt16 channel, UInt16 line_subdiv)
*
* SYNTAX:
*      ds1103_inc_position_read(UInt16 channel, UInt16 line_subdiv)
*
* DESCRIPTION: The function delivers the actual position of the channel (lines).
*
* PARAMETERS:  channel : logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*              line_subdiv: defines the resolution of the counter (predefined in
*                           Io1103.h).
*
*                           For channel 1 to 6
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_4
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 4 of the counter are masked (not
*                           mentioned in the line position).
*
*                           For channel 7
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_256
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 11 of the 38/22 Bit counter (with
*                           finecount!) are masked (not  mentioned in the line
*                           position).
*
* RETURNS: encoder lines
*
* Example:  ds1103_inc_position_read(7, DS1103_INC_LINE_SUBDIV_256)
*           reads the line position value of channel 7 with the full resolution
*           of 22 or 38 bit.
*
* NOTE:
*
******************************************************************************/
__INLINE Float64 ds1103_inc_position_read(UInt16 channel, UInt16 line_subdiv)
{

  Float64 inc_pos=0;
  Int32 finec;
  Int32 tmp;
  UInt16 datl, dath;
  rtlib_int_status_t int_status;


  if(channel < 6)
  {
    /*disable interrupt */
    RTLIB_INT_SAVE_AND_DISABLE(int_status);
    /*strobe io_reg    */
    *DS1103_IO_STROBE_REG = (0x400 << ds1103_inc_chan[channel] );
    RTLIB_FORCE_IN_ORDER();
    /*hw-access        */
    tmp = ( DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] ) >> 8;
    /*restore interrupt*/
    RTLIB_FORCE_IN_ORDER();
    RTLIB_INT_RESTORE(int_status);
    /*calculation*/
    tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
    inc_pos = (Float64)tmp * 0.25;
  };

  if(channel==6)
   {
   if(ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT)
       {
        /*disable interrupt */
        RTLIB_INT_SAVE_AND_DISABLE(int_status);
        /*strobe io_reg    */
        *DS1103_IO_STROBE_REG = (0x8000);
        RTLIB_FORCE_IN_ORDER();
        /*hw-access*/
        datl = inc_hanning->ct2dat; 
        /*restore interrupt*/
        RTLIB_FORCE_IN_ORDER();
        RTLIB_INT_RESTORE(int_status);
        /*calculation*/
        tmp = (Int32) ( (Int16)datl );
       }
   else /*ds1103_inc_ch6_mode!=DS1103_INC_CH6_16BIT*/
       {
        /*disable interrupt */
        RTLIB_INT_SAVE_AND_DISABLE(int_status);
        /*strobe io_reg    */
        *DS1103_IO_STROBE_REG = (0x8000);
        RTLIB_FORCE_IN_ORDER();
        /*hw-access*/
        datl = inc_hanning->ct1datl;
        dath = inc_hanning->ct1dath;
        /*restore interrupt*/
        RTLIB_FORCE_IN_ORDER();
        RTLIB_INT_RESTORE(int_status);
        /*calculation*/
        tmp =  ( ( (UInt32)datl) | ( (UInt32)dath << 16 ) );
       }
    tmp = tmp << ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
    tmp = tmp >> ds1103_inc_ch6_shift[ds1103_inc_ch6_mode]; /* sign ext. 24B */
    tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
    inc_pos = (Float64)tmp * 0.25;
   }

  if(channel==7)
   {
    if(ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT)
        {
        /*disable interrupt */
        RTLIB_INT_SAVE_AND_DISABLE(int_status);
        /*strobe io_reg    */
        *DS1103_IO_STROBE_REG = (0x8000);
        RTLIB_FORCE_IN_ORDER();
        /*hw-access*/
        finec = inc_hanning->encsign;
        datl = inc_hanning->ct2dat;
        /*restore interrupt*/
        RTLIB_FORCE_IN_ORDER();
        RTLIB_INT_RESTORE(int_status);
        /*calculation*/
        tmp = (Int32) ( (Int16)datl );
        }
    else
        {
        /*disable interrupt */
        RTLIB_INT_SAVE_AND_DISABLE(int_status);
        /*strobe io_reg    */
        *DS1103_IO_STROBE_REG = (0x8000);
        RTLIB_FORCE_IN_ORDER();
        /*hw-access*/
        finec =inc_hanning->encsign;
        datl = inc_hanning->ct1datl;
        dath = inc_hanning->ct1dath;
        /*restore interrupt*/
        RTLIB_FORCE_IN_ORDER();     
        RTLIB_INT_RESTORE(int_status);
        /*calculation*/
        tmp =  ( ( (UInt32)datl) | ( (UInt32)dath << 16 ) );
        };
    /* take the 6 lower bits of the fine_count and subtr. offset */
    finec =    (Int32) ( ( ( finec >> 8) & DS1103_INC_CH7_FINECMSK) );
    /* build new 8 bit fine_count: 6 bit fine_count and the lower 2 bits of the counter */
    finec = ( ((tmp & 0x3) << 6) | finec); /* 8 bit fine_count */
    finec = finec - ds1103_fine_count_offs;
    /* mask the 8 bit dep. from line_subdiv */
    finec = finec & ds1103_inc_subdiv_finecountmsk[line_subdiv];
    tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
    tmp = tmp >> 2; /* 30 bit ot the counter */
    /* mask the count dep. from line_subdiv */
    inc_pos = (Float64)tmp + (Float64)finec * DS1103_INC2_LINESCAL_FINEC;
   }; /*channel 7*/
return inc_pos;
}


/******************************************************************************
*
* FUNCTION:
*   Float64 ds1103_inc_position_read_immediately(UInt16 channel, UInt16 line_subdiv)
*
* SYNTAX:
*      ds1103_inc_position_read_immediately(UInt16 channel, UInt16 line_subdiv)
*
* DESCRIPTION: 
*    This function reads the incremental encoder position of the specified
*    channel immediately, without a preceding strobe of the output register.
*    This function can be used in an interrupt service routine for the T1PWM
*    slave-master interrupt, when the SYNCIN trigger of the incremental encoder
*    is enabled.

*
* PARAMETERS:  channel : logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*              line_subdiv: defines the resolution of the counter (predefined in
*                           Io1103.h).
*
*                           For channel 1 to 6
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_4
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 4 of the counter are masked (not
*                           mentioned in the line position).
*
*                           For channel 7
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_256
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 11 of the 38/22 Bit counter (with
*                           finecount!) are masked (not  mentioned in the line
*                           position).
*
* RETURNS: encoder lines
*
* Example:  ds1103_inc_position_read_immediately(7, DS1103_INC_LINE_SUBDIV_256)
*           reads the line position value of channel 7 with the full resolution
*           of 22 or 38 bit.
*
* NOTE:
*
******************************************************************************/
__INLINE Float64 ds1103_inc_position_read_immediately(UInt16 channel, UInt16 line_subdiv)
{
  
  Float64 inc_pos=0;
  Int32 finec;
  Int32 tmp;
  UInt16 datl, dath;
  rtlib_int_status_t int_status;

  if(channel < 6)
  {
    /*disable interrupt */
    RTLIB_INT_SAVE_AND_DISABLE(int_status);
    /*hw-access        */
    tmp = ( DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] ) >> 8;
    /*restore interrupt*/
    RTLIB_FORCE_IN_ORDER();
    RTLIB_INT_RESTORE(int_status);
    /*calculation*/
    tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
    inc_pos = (Float64)tmp * 0.25;
  };

  if(channel==6)
   {
   if(ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT)
       {
        /*disable interrupt */
        RTLIB_INT_SAVE_AND_DISABLE(int_status);
        /*hw-access*/
        datl = inc_hanning->ct2dat; 
        /*restore interrupt*/
        RTLIB_FORCE_IN_ORDER();
        RTLIB_INT_RESTORE(int_status);
        /*calculation*/
        tmp = (Int32) ( (Int16)datl );
       }
   else /*ds1103_inc_ch6_mode!=DS1103_INC_CH6_16BIT*/
       {
        /*disable interrupt */
        RTLIB_INT_SAVE_AND_DISABLE(int_status);
        /*hw-access*/
        datl = inc_hanning->ct1datl;
        dath = inc_hanning->ct1dath;
        /*restore interrupt*/
        RTLIB_FORCE_IN_ORDER();
        RTLIB_INT_RESTORE(int_status);
        /*calculation*/
        tmp =  ( ( (UInt32)datl) | ( (UInt32)dath << 16 ) );
       }
    tmp = tmp << ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
    tmp = tmp >> ds1103_inc_ch6_shift[ds1103_inc_ch6_mode]; /* sign ext. 24B */
    tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
    inc_pos = (Float64)tmp * 0.25;
   }

  if(channel==7)
   {
    if(ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT)
        {
        /*disable interrupt */
        RTLIB_INT_SAVE_AND_DISABLE(int_status);
        /*hw-access*/
        finec = inc_hanning->encsign;
        datl = inc_hanning->ct2dat;
        /*restore interrupt*/
        RTLIB_FORCE_IN_ORDER();
        RTLIB_INT_RESTORE(int_status);
        /*calculation*/
        tmp = (Int32) ( (Int16)datl );
        }
    else
        {
        /*disable interrupt */
        RTLIB_INT_SAVE_AND_DISABLE(int_status);
        /*strobe io_reg    */
        /*hw-access*/
        finec =inc_hanning->encsign;
        datl = inc_hanning->ct1datl;
        dath = inc_hanning->ct1dath;
        /*restore interrupt*/
        RTLIB_FORCE_IN_ORDER();     
        RTLIB_INT_RESTORE(int_status);
        /*calculation*/
        tmp =  ( ( (UInt32)datl) | ( (UInt32)dath << 16 ) );
        };
    /* take the 6 lower bits of the fine_count and subtr. offset */
    finec =    (Int32) ( ( ( finec >> 8) & DS1103_INC_CH7_FINECMSK) );
    /* build new 8 bit fine_count: 6 bit fine_count and the lower 2 bits of the counter */
    finec = ( ((tmp & 0x3) << 6) | finec); /* 8 bit fine_count */
    finec = finec - ds1103_fine_count_offs;
    /* mask the 8 bit dep. from line_subdiv */
    finec = finec & ds1103_inc_subdiv_finecountmsk[line_subdiv];
    tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
    tmp = tmp >> 2; /* 30 bit of the counter */
    /* mask the count dep. from line_subdiv */
    inc_pos = (Float64)tmp + (Float64)finec * DS1103_INC2_LINESCAL_FINEC;
   }; /*channel 7*/
return inc_pos;
}










/******************************************************************************
*
* FUNCTION:
*   Float64 ds1103_inc_delta_position_read(UInt16 channel, UInt16 line_subdiv)
*
* SYNTAX:
*      ds1103_inc_delta_position_read(UInt16 channel, UInt16 line_subdiv)
*
* DESCRIPTION: The function returns the difference between the actual position
*              of the channel and the position of the last call of the funct. .
*
*
* PARAMETERS:channel : logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*            line_subdiv: defines the resolution of the counter (predefined in
*                           Io1103.h).
*
*                           For channel 1 to 6
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_4
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 4 of the counter are masked (not
*                           mentioned in the line position).
*
*                           For channel 7
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_256
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 11 of the 38/22 Bit counter (with
*                           finecount!) are masked (not  mentioned in the line
*                           position).
*
* RETURNS: deltaposition (encoder lines)
*
*
* NOTE:
*
******************************************************************************/
__INLINE Float64 ds1103_inc_delta_position_read(UInt16 channel, UInt16 line_subdiv)
{
  
  Float64 inc_pos;
  Int32 prev_val, prev_finec;
  UInt32 msr_state, mask;             
  Int32 tmp, finec;
  UInt16 datl, dath;
   

  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE(); 

  /* strobes the output reg. */
  *DS1103_IO_STROBE_REG = (0x400 << ds1103_inc_chan[channel] ); 
  RTLIB_FORCE_IN_ORDER();     
 
  if ( !ds1103_inc_rstonidx[channel])
   prev_val =  ds1103_inc_counter[channel];
  else
  {
    /* check, if there was an index-signal */
    /* reads the int-reg. */
    mask =   ( (*DS1103_INTERRUPT_REG) & (0x8000 << ds1103_inc_iir[channel]) ) ;
   
    if( (DS1103_INC_RSTONIDX_ACTIVE == ds1103_inc_rstonidx_state[channel]) &&
        ( (mask == ds1103_inc_iirmsk[channel]) || (ds1103_rstonidx_flag[channel] == DS1103_ON) ) )
    {
      if(channel < 6)
        prev_val = 0;
      else
        prev_val = ds1103_ch6ch7_latch[channel - 6];
    
      ds1103_rstonidx_flag[channel] = DS1103_OFF;
    }
    else
      prev_val = ds1103_inc_counter[channel];
  };

  if(channel < 6)
  {
    tmp = ( ( DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] )
              & DS1103_DIGINC2_MASK );
    
    ds1103_inc_counter[channel] = tmp; /*stores counter */
    tmp = tmp - prev_val;
    tmp = tmp >> 8; 
    tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv];
    inc_pos = ( (Float64)tmp ) * 0.25;
    
    RTLIB_FORCE_IN_ORDER();     
    ds1103_set_interrupt_status(msr_state); 
    return(inc_pos);
  };

  
 
  if(channel == 6)  /* for dig.inc. ch6 */
  {

    if(ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT)
    { 
      *( (Int16*)&datl) = inc_hanning->ct2dat; 
      *( (Int16*)&tmp) = *( (Int16*)&datl);
      ds1103_inc_counter[channel] = *( (Int16*)&tmp);
      *( (Int16*)&tmp) = *( (Int16*)&tmp) - prev_val; 
      *( (Int16*)&tmp) = *( (Int16*)&tmp)
         & ds1103_inc_subdiv_countmsk[line_subdiv];
      inc_pos = (Float64)*( (Int16*)&tmp) * 0.25; 
    }
    else
    {
      datl = inc_hanning->ct1datl;
      dath = inc_hanning->ct1dath;
      tmp =  ( ( (UInt32)datl) | ( (UInt32)dath << 16 ) );
      tmp = tmp << ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
      ds1103_inc_counter[channel] = tmp >> ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
      prev_val = prev_val << ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
      tmp = tmp - prev_val; 
      tmp = tmp >> ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
      tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
      inc_pos = (Float64)tmp * 0.25; 
    }; 

    RTLIB_FORCE_IN_ORDER();
    ds1103_set_interrupt_status(msr_state);
    return(inc_pos);
  };


 if(channel == 7)
 {

   if( ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT )
   {
      finec = inc_hanning->encsign;
      *( (Int16*)&datl) = inc_hanning->ct2dat; 
      *( (Int16*)&tmp) = *( (Int16*)&datl);
      ds1103_inc_counter[channel] = *( (Int16*)&tmp);
      *( (Int16*)&tmp) = *( (Int16*)&tmp) - prev_val; 
      prev_finec = ds1103_fine_count; 
      /* take the 6 lower bits of the fine_count and subtr. offset */
      finec =    (Int32) ( ( ( finec >> 8) & DS1103_INC_CH7_FINECMSK) );
      ds1103_fine_count = finec; /* store finecount */
      /* build new 8 bit fine_count: 6 bit fine_count and the lower
         2 bits of the counter */
      finec = ( ( ( *( (Int16*)&tmp) & 0x3) << 6) | finec); /* 8 bit fine_count */
      finec = finec - prev_finec; 
      /* mask the 8 bit dep. from line_subdiv */
      finec = finec & ds1103_inc_subdiv_finecountmsk[line_subdiv];
      /* mask the count dep. from line_subdiv */
      *( (Int16*)&tmp) = *( (Int16*)&tmp)
         & ds1103_inc_subdiv_countmsk[line_subdiv];
      *( (Int16*)&tmp) = *( (Int16*)&tmp) >> 2;  /* 14 bit ot the counter */
      inc_pos = ( (Float64) *( (Int16*)&tmp) )
                +  (Float64)finec * DS1103_INC2_LINESCAL_FINEC;
   }
   else
   {
      finec = inc_hanning->encsign;
      datl = inc_hanning->ct1datl;
      dath = inc_hanning->ct1dath;
      tmp =  ( ( (UInt32)datl) | ( (UInt32)dath << 16 ) );
      ds1103_inc_counter[channel] = tmp;
      tmp = tmp - prev_val; 
      prev_finec = ds1103_fine_count;
      /* take the 6 lower bits of the fine_count and subtr. offset */
      finec =    (Int32) ( ( ( finec >> 8) & DS1103_INC_CH7_FINECMSK) );
      ds1103_fine_count = finec; /* store finecount */
      /* build new 8 bit fine_count: 6 bit fine_count and the
         lower 2 bits of the counter */
      finec = ( ( (tmp & 0x3) << 6) | finec); /* 8 bit fine_count */
      finec = (Int32)finec - prev_finec;
      /* mask the 8 bit dep. from line_subdiv */
      finec = finec & ds1103_inc_subdiv_finecountmsk[line_subdiv];
      /* mask the count dep. from line_subdiv */
      tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
      tmp = tmp >> 2; /* 30 bit of the counter */
      inc_pos = (Float64)tmp +  (Float64)finec * DS1103_INC2_LINESCAL_FINEC;
   } 
      RTLIB_FORCE_IN_ORDER();     
      ds1103_set_interrupt_status(msr_state);
      return(inc_pos);
 };

    return(0);

}





/******************************************************************************
*
* FUNCTION:
*   Float64 ds1103_inc_delta_position_read_immediately(UInt16 channel, UInt16 line_subdiv)
*
* SYNTAX:
*      ds1103_inc_delta_position_read_immediately(UInt16 channel, UInt16 line_subdiv)
*
* DESCRIPTION: 
*      The function returns the difference between the actual position
*      of the channel and the position of the last call of the function 
*      It reads the incremental encoder position of the specified
*      channel immediately, without a preceding strobe of the output register.
*      This function can be used in an interrupt service routine for the T1PWM
*      slave-master interrupt, when the SYNCIN trigger of the incremental encoder
*      is enabled.

*
*
* PARAMETERS:channel : logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*            line_subdiv: defines the resolution of the counter (predefined in
*                           Io1103.h).
*
*                           For channel 1 to 6
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_4
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 4 of the counter are masked (not
*                           mentioned in the line position).
*
*                           For channel 7
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_256
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 11 of the 38/22 Bit counter (with
*                           finecount!) are masked (not  mentioned in the line
*                           position).
*
* RETURNS: deltaposition (encoder lines)
*
*
* NOTE:
*
******************************************************************************/
__INLINE Float64 ds1103_inc_delta_position_read_immediately(UInt16 channel, UInt16 line_subdiv)
{
  
  Float64 inc_pos;
  Int32 prev_val, prev_finec;
  UInt32 msr_state, mask;             
  Int32 tmp, finec;
  UInt16 datl, dath;
   

  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE(); 

  /* do not strobe the output reg. */
 
  if ( !ds1103_inc_rstonidx[channel])
   prev_val =  ds1103_inc_counter[channel];
  else
  {
    /* check, if there was an index-signal */
    /* reads the int-reg. */
    mask =   ( (*DS1103_INTERRUPT_REG) & (0x8000 << ds1103_inc_iir[channel]) ) ;
   
    if( (DS1103_INC_RSTONIDX_ACTIVE == ds1103_inc_rstonidx_state[channel]) &&
        ( (mask == ds1103_inc_iirmsk[channel]) || (ds1103_rstonidx_flag[channel] == DS1103_ON) ) )
    {
      if(channel < 6)
        prev_val = 0;
      else
        prev_val = ds1103_ch6ch7_latch[channel - 6];
    
      ds1103_rstonidx_flag[channel] = DS1103_OFF;
    }
    else
      prev_val = ds1103_inc_counter[channel];
  };

  if(channel < 6)
  {
    tmp = ( ( DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] )
              & DS1103_DIGINC2_MASK );
    
    ds1103_inc_counter[channel] = tmp; /*stores counter */
    tmp = tmp - prev_val;
    tmp = tmp >> 8; 
    tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv];
    inc_pos = ( (Float64)tmp ) * 0.25;
    
    RTLIB_FORCE_IN_ORDER();     
    ds1103_set_interrupt_status(msr_state); 
    return(inc_pos);
  };

  
 
  if(channel == 6)  /* for dig.inc. ch6 */
  {

    if(ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT)
    { 
      *( (Int16*)&datl) = inc_hanning->ct2dat; 
      *( (Int16*)&tmp) = *( (Int16*)&datl);
      ds1103_inc_counter[channel] = *( (Int16*)&tmp);
      *( (Int16*)&tmp) = *( (Int16*)&tmp) - prev_val; 
      *( (Int16*)&tmp) = *( (Int16*)&tmp)
         & ds1103_inc_subdiv_countmsk[line_subdiv];
      inc_pos = (Float64)*( (Int16*)&tmp) * 0.25; 
    }
    else
    {
      datl = inc_hanning->ct1datl;
      dath = inc_hanning->ct1dath;
      tmp =  ( ( (UInt32)datl) | ( (UInt32)dath << 16 ) );
      tmp = tmp << ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
      ds1103_inc_counter[channel] = tmp >> ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
      prev_val = prev_val << ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
      tmp = tmp - prev_val; 
      tmp = tmp >> ds1103_inc_ch6_shift[ds1103_inc_ch6_mode];
      tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
      inc_pos = (Float64)tmp * 0.25; 
    }; 

    RTLIB_FORCE_IN_ORDER();
    ds1103_set_interrupt_status(msr_state);
    return(inc_pos);
  };


 if(channel == 7)
 {

   if( ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT )
   {
     
      finec = inc_hanning->encsign;
      *( (Int16*)&datl) = inc_hanning->ct2dat; 
      *( (Int16*)&tmp) = *( (Int16*)&datl);
      ds1103_inc_counter[channel] = *( (Int16*)&tmp);
      *( (Int16*)&tmp) = *( (Int16*)&tmp) - prev_val; 
      prev_finec = ds1103_fine_count; 
      /* take the 6 lower bits of the fine_count and subtr. offset */
      finec =    (Int32) ( ( ( finec >> 8) & DS1103_INC_CH7_FINECMSK) );
      ds1103_fine_count = finec; /* store finecount */
      /* build new 8 bit fine_count: 6 bit fine_count and the lower
         2 bits of the counter */
      finec = ( ( ( *( (Int16*)&tmp) & 0x3) << 6) | finec); /* 8 bit fine_count */
      finec = finec - prev_finec; 
      /* mask the 8 bit dep. from line_subdiv */
      finec = finec & ds1103_inc_subdiv_finecountmsk[line_subdiv];
      /* mask the count dep. from line_subdiv */
      *( (Int16*)&tmp) = *( (Int16*)&tmp)
         & ds1103_inc_subdiv_countmsk[line_subdiv];
      *( (Int16*)&tmp) = *( (Int16*)&tmp) >> 2;  /* 14 bit ot the counter */
      inc_pos = ( (Float64) *( (Int16*)&tmp) )
                +  (Float64)finec * DS1103_INC2_LINESCAL_FINEC;
   }
   else
   {
   	  finec = inc_hanning->encsign;
      datl = inc_hanning->ct1datl;
      dath = inc_hanning->ct1dath;
      tmp =  ( ( (UInt32)datl) | ( (UInt32)dath << 16 ) );
      ds1103_inc_counter[channel] = tmp;
      tmp = tmp - prev_val; 
      prev_finec = ds1103_fine_count;
      /* take the 6 lower bits of the fine_count and subtr. offset */
      finec =    (Int32) ( ( ( finec >> 8) & DS1103_INC_CH7_FINECMSK) );
      ds1103_fine_count = finec; /* store finecount */
      /* build new 8 bit fine_count: 6 bit fine_count and the
         lower 2 bits of the counter */
      finec = ( ( (tmp & 0x3) << 6) | finec); /* 8 bit fine_count */
      finec = (Int32)finec - prev_finec;
      /* mask the 8 bit dep. from line_subdiv */
      finec = finec & ds1103_inc_subdiv_finecountmsk[line_subdiv];
      /* mask the count dep. from line_subdiv */
      tmp = tmp & ds1103_inc_subdiv_countmsk[line_subdiv]; 
      tmp = tmp >> 2; /* 30 bit of the counter */
      inc_pos = (Float64)tmp +  (Float64)finec * DS1103_INC2_LINESCAL_FINEC;
   } 
      RTLIB_FORCE_IN_ORDER();     
      ds1103_set_interrupt_status(msr_state);
      return(inc_pos);
 };

    return(0);

}












/******************************************************************************
*
* FUNCTION:
*   void ds1103_inc_position_write(UInt16 channel, Float64 position,
*                                  UInt16 line_subdiv)
*
* SYNTAX:
*      ds1103_inc_position_write(UInt16 channel, Float64 position,
*                                  UInt16 line_subdiv)
*
* DESCRIPTION: function to write a line-position to the incremental encoder
*              interface
*
* PARAMETERS: channel : logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*
*             position:  new value for line-position
*
*              line_subdiv: defines the resolution of the counter (predefined in
*                           Io1103.h).
*
*                           For channel 1 to 6
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_4
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 4 of the counter are masked (not
*                           mentioned in the line position).
*
*                           For channel 7
*                           line_subdiv = DS1103_INC_LINE_SUBDIV_256
*                           chooses the full resolution.
*                           With line_subdiv = DS1103_INC_LINE_SUBDIV_1_8
*                           Bit 0 to Bit 11 of the 38/22 Bit counter (with
*                           finecount!) are masked (not  mentioned in the line
*                           position).
*
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_inc_position_write(UInt16 channel, Float64 position,
                                        UInt16 line_subdiv)
{

  UInt32 msr_state;
  Int32 tmp_pos;
  UInt16 fine_count;
  

  if(channel < 6)
  {
    tmp_pos = (Int32) ( (UInt32)(position * 4) );
    tmp_pos = tmp_pos & ds1103_inc_subdiv_countmsk[line_subdiv];
    msr_state = ds1103_get_interrupt_status();
    DS1103_GLOBAL_INTERRUPT_DISABLE();
    DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] = tmp_pos << 8;
    ds1103_inc_counter[channel] = tmp_pos << 8; /* store the counter */
    ds1103_set_interrupt_status(msr_state);
  }
  else
  {
    if(channel == 7)
    {
      msr_state = ds1103_get_interrupt_status();
      DS1103_GLOBAL_INTERRUPT_DISABLE();
      /* strobes the output reg. */
      *DS1103_IOSTB_REG = (0x400 << ds1103_inc_chan[channel] );
      RTLIB_FORCE_IN_ORDER();
      fine_count = inc_hanning->encsign;
      /* stores the finecount */
      ds1103_fine_count_offs = (Int32) ( ( fine_count >> 8)
                               & DS1103_INC_CH7_FINECMSK);
      ds1103_fine_count = ds1103_fine_count_offs;
      tmp_pos =  (Int32) ( (UInt32)(position * 4) ); 
      tmp_pos = tmp_pos & ds1103_inc_subdiv_countmsk[line_subdiv];  
    }       
    else
    {
      tmp_pos = (Int32) ( (UInt32)(position * 4) ); 
      tmp_pos = tmp_pos << ds1103_inc_ch6_shift[ds1103_inc_ch6_mode]; 
      tmp_pos = tmp_pos >> ds1103_inc_ch6_shift[ds1103_inc_ch6_mode]; /* sign. ext 24B */
      tmp_pos = tmp_pos & ds1103_inc_subdiv_countmsk[line_subdiv];
      msr_state = ds1103_get_interrupt_status();
      DS1103_GLOBAL_INTERRUPT_DISABLE();
    };

    ds1103_inc_counter[channel] = tmp_pos;  /* store counter */ 
 
    if( ( (channel == 6 ) && (ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT) )
         || ( (channel == 7 ) && (ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT) ) )
    {
        /* write counter 2 */
        inc_hanning->ct2dat =  (Int16)tmp_pos;
        RTLIB_FORCE_IN_ORDER();                       
        ds1103_inc2_strobe_inp2();
        ds1103_set_interrupt_status(msr_state);
    }
    else
    {
      /* write counter 1 */
      inc_hanning->ct1datl =  (UInt16) tmp_pos;
      inc_hanning->ct1dath =  (Int16) (tmp_pos >> 16); 
      RTLIB_FORCE_IN_ORDER();
      ds1103_inc2_strobe_inp1();
      ds1103_set_interrupt_status(msr_state);
    };

        
  } /* else */

  RTLIB_FORCE_IN_ORDER();
    
}
 
    
 

/******************************************************************************
*
* FUNCTION:
*   Int64 ds1103_inc_counter_read(UInt16 channel)
*
* SYNTAX:
*        ds1103_inc_counter_read(UInt16 channel)
*
* DESCRIPTION:The function updates the incremental encoder output register and
*             returns the counter value.
*
*
* PARAMETERS: channel : logical channel number 1, 2, 3, 4, 5, 6, 7
*
*
*
* RETURNS: the type of the returned value is a structure of a 32 bit Int32
*          and a 32 bit UInt32 (defined in Dstypes.h):
*          typedef struct { UInt32 low; Int32 high;  } Int64;
*          The counter of the desired channel is returned in 
*          the Int32. For channel 7 the UInt32 contains the 6 bit finecount.
*          For channel 1 to 6 the UInt32 contains no valid data.
*
* Example: {
*             Int64 enc_val;
*             Int32 counter;
*             UInt32 finecount;  
*             
*             enc_val = ds1103_inc_counter_read(7);
*             counter = enc_val.high;    
*             finecount = enc_val.low;
*          } 
*          This code reads the counter and the finecount of channel 7.
*           
*
* NOTE:    This function is obsolete. Please use ds1103_inc_counter_rd instead.
*
******************************************************************************/
__INLINE Int64 ds1103_inc_counter_read(UInt16 channel) 
{
  Int64 value;
  UInt32 msr_state;
  UInt16 datl, dath, fine_count;

  value.low = 0;

  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE();

  /* strobes the output reg. */
  *DS1103_IO_STROBE_REG = (0x400 << ds1103_inc_chan[channel] ); 
  RTLIB_FORCE_IN_ORDER();

  if(channel < 6)
  {
    value.high = ((Int32) DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] ) >> 8;
    RTLIB_FORCE_IN_ORDER();
    ds1103_set_interrupt_status(msr_state);
    return(value);
  };


  if(channel == 6)  /* for dig.inc. ch6 */
  {

    if(ds1103_inc_ch6_mode != DS1103_INC_CH6_16BIT)
    {
      /* 32 or 24 bit */

      datl = inc_hanning->ct1datl;
      dath = inc_hanning->ct1dath;
      value.high =   (UInt32) ( ( (UInt32)datl ) | ( (UInt32)dath << 16 ) )
                     & ds1103_inc_ch6_msk[ds1103_inc_ch6_mode]; /* mask for 24 Bit */
      RTLIB_FORCE_IN_ORDER();
      ds1103_set_interrupt_status(msr_state);
      return(value);
    }
    else
    {
      /* 16 bit */
      datl = inc_hanning->ct2dat;
      value.high = (UInt32)datl ;
      RTLIB_FORCE_IN_ORDER();
      ds1103_set_interrupt_status(msr_state);
      return(value) ;
    }

  };

  fine_count = inc_hanning->encsign;
  value.low =    (UInt32) ( ( ( fine_count >> 8) & DS1103_INC_CH7_FINECMSK)
                 - ds1103_fine_count_offs);

  if(ds1103_inc_ch7_mode == DS1103_INC_CH7_38BIT)
  {
    datl = inc_hanning->ct1datl;
    dath = inc_hanning->ct1dath;
    value.high =   (Int32)( (UInt32)datl | ( (UInt32)dath << 16 ) );
  }
  else
  {
    datl = inc_hanning->ct2dat;
    value.high = (Int32)datl;
  }

  RTLIB_FORCE_IN_ORDER();
  ds1103_set_interrupt_status(msr_state);
  return(value);

}


/******************************************************************************
*
* FUNCTION:
*   Int64 ds1103_inc_counter_rd_immediately(UInt16 channel)
*
* SYNTAX:
*        ds1103_inc_counter_read_immediately(UInt16 channel)
*
* DESCRIPTION:
*    This function reads the encoder counter register of the specified channel
*    immediately, without a preceding strobe of the output register.
*    This function can be used in an interrupt service routine for the T1PWM
*    slave-master interrupt, when the SYNCIN trigger of the incremental encoder
*    is enabled.
*
*
* PARAMETERS: channel : logical channel number 1, 2, 3, 4, 5, 6, 7
*
*
*
* RETURNS: the type of the returned value is a structure of a 32 bit Int32
*          and a 32 bit UInt32 (defined in Dstypes.h):
*          typedef struct { UInt32 low; Int32 high;  } Int64;
*          The counter of the desired channel is returned in 
*          the Int32. For channel 7 the UInt32 contains the 6 bit finecount.
*          For channel 1 to 6 the UInt32 contains no valid data.
*
* Example: {
*             Int64 enc_val;
*             Int32 counter;
*             UInt32 finecount;  
*             
*             enc_val = ds1103_inc_counter_read_immediately(7);
*             counter = enc_val.high;    
*             finecount = enc_val.low;
*          } 
*          This code reads the counter and the finecount of channel 7.
*           
*
* NOTE:    This function is obsolete. Please use ds1103_inc_counter_rd instead.
*
******************************************************************************/
__INLINE Int64 ds1103_inc_counter_read_immediately(UInt16 channel) 
{
  Int64 value;
  UInt32 msr_state;
  UInt16 datl, dath, fine_count;

  value.low = 0;

  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE();

  /* do not strobe the output reg. */
 
  if(channel < 6)
  {
    value.high = ((Int32) DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] ) >> 8;
    RTLIB_FORCE_IN_ORDER();
    ds1103_set_interrupt_status(msr_state);
    return(value);
  };


  if(channel == 6)  /* for dig.inc. ch6 */
  {

    if(ds1103_inc_ch6_mode != DS1103_INC_CH6_16BIT)
    {
      /* 32 or 24 bit */

      datl = inc_hanning->ct1datl;
      dath = inc_hanning->ct1dath;
      value.high =   (UInt32) ( ( (UInt32)datl ) | ( (UInt32)dath << 16 ) )
                     & ds1103_inc_ch6_msk[ds1103_inc_ch6_mode]; /* mask for 24 Bit */
      RTLIB_FORCE_IN_ORDER();
      ds1103_set_interrupt_status(msr_state);
      return(value);
    }
    else
    {
      /* 16 bit */
      datl = inc_hanning->ct2dat;
      value.high = (UInt32)datl ;
      RTLIB_FORCE_IN_ORDER();
      ds1103_set_interrupt_status(msr_state);
      return(value) ;
    }

  };

  fine_count = inc_hanning->encsign;
  value.low =    (UInt32) ( ( ( fine_count >> 8) & DS1103_INC_CH7_FINECMSK)
                 - ds1103_fine_count_offs);

  if(ds1103_inc_ch7_mode == DS1103_INC_CH7_38BIT)
  {
    datl = inc_hanning->ct1datl;
    dath = inc_hanning->ct1dath;
    value.high =   (Int32)( (UInt32)datl | ( (UInt32)dath << 16 ) );
  }
  else
  {
    datl = inc_hanning->ct2dat;
    value.high = (Int32)datl;
  }

  RTLIB_FORCE_IN_ORDER();
  ds1103_set_interrupt_status(msr_state);
  return(value);

}




/******************************************************************************
*
* FUNCTION:
*   Int64 ds1103_inc_counter_rd(UInt16 channel)
*
* SYNTAX:
*        ds1103_inc_counter_rd(UInt16 channel)
*
* DESCRIPTION:
The function updates the incremental encoder output register and
*             returns the counter value.
*
*
* PARAMETERS: channel : logical channel number 1, 2, 3, 4, 5, 6, 7
*
*
*
* RETURNS: the type of the returned value is a structure of a 32 bit Int32
*          and a 32 bit UInt32 (defined in Dstypes.h):
*          typedef struct { UInt32 low; Int32 high; } Int64;
*          The counter of the desired channel is returned in 
*          the Int32. For channel 7 the UInt32 contains the 6 bit finecount.
*          For channel 1 to 6 the UInt32 contains no valid data.
*
* Example: {
*             Int64 enc_val;
*             Int32 counter;
*             UInt32 finecount;  
*             
*             enc_val = ds1103_inc_counter_rd(7);
*             counter = enc_val.high;    
*             finecount = enc_val.low;
*          } 
*          This code reads the counter and the finecount of channel 7.
*           
*
* NOTE:
*
******************************************************************************/
__INLINE Int64 ds1103_inc_counter_rd(UInt16 channel) 
{
  Int64 value;
  UInt32 msr_state;
  UInt16 datl, dath, fine_count;

  value.low = 0;

  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE();

  /* strobes the output reg. */
  *DS1103_IO_STROBE_REG = (0x400 << ds1103_inc_chan[channel] ); 
  RTLIB_FORCE_IN_ORDER();

  if(channel < 6)
  {
    value.high = ((Int32) DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] ) >> 8;
    RTLIB_FORCE_IN_ORDER();
    ds1103_set_interrupt_status(msr_state);
    return(value);
  };


  if(channel == 6)  /* for dig.inc. ch6 */
  {

    if(ds1103_inc_ch6_mode != DS1103_INC_CH6_16BIT)
    {
      /* 32 or 24 bit */

      datl = inc_hanning->ct1datl;
      dath = inc_hanning->ct1dath;
      value.high = (UInt32) ( ( (UInt32)datl ) | ( (UInt32)dath << 16 ) )
                   & ds1103_inc_ch6_msk[ds1103_inc_ch6_mode]; /* mask for 24 Bit */
      
      /* sign extension */
      value.high = value.high << 8;
      value.high = value.high >> 8;
      
      RTLIB_FORCE_IN_ORDER();
      ds1103_set_interrupt_status(msr_state);
      return(value);
    }
    else
    {
      /* 16 bit */
      datl = inc_hanning->ct2dat;
      value.high = (UInt32)datl ;
      RTLIB_FORCE_IN_ORDER();
      ds1103_set_interrupt_status(msr_state);
      return(value) ;
    }

  };

  fine_count = inc_hanning->encsign;
  value.low =    (UInt32) ( ( ( fine_count >> 8) & DS1103_INC_CH7_FINECMSK)
                 - ds1103_fine_count_offs);

  if(ds1103_inc_ch7_mode == DS1103_INC_CH7_38BIT)
  {
    datl = inc_hanning->ct1datl;
    dath = inc_hanning->ct1dath;
    value.high =   (Int32)( (UInt32)datl | ( (UInt32)dath << 16 ) );
  }
  else
  {
    datl = inc_hanning->ct2dat;
    value.high = (Int32)datl;
  }

  RTLIB_FORCE_IN_ORDER();
  ds1103_set_interrupt_status(msr_state);
  return(value);

}


/******************************************************************************
*
* FUNCTION:
*   Int64 ds1103_inc_counter_rd_immediately(UInt16 channel)
*
* SYNTAX:
*        ds1103_inc_counter_rd_immediately(UInt16 channel)
*
* DESCRIPTION:
*    This function reads the encoder counter register of the specified channel
*    immediately, without a preceding strobe of the output register.
*    This function can be used in an interrupt service routine for the T1PWM
*    slave-master interrupt, when the SYNCIN trigger of the incremental encoder
*    is enabled.
*
*
* PARAMETERS: channel : logical channel number 1, 2, 3, 4, 5, 6, 7
*
*
*
* RETURNS: the type of the returned value is a structure of a 32 bit Int32
*          and a 32 bit UInt32 (defined in Dstypes.h):
*          typedef struct { UInt32 low; Int32 high; } Int64;
*          The counter of the desired channel is returned in 
*          the Int32. For channel 7 the UInt32 contains the 6 bit finecount.
*          For channel 1 to 6 the UInt32 contains no valid data.
*
* Example: {
*             Int64 enc_val;
*             Int32 counter;
*             UInt32 finecount;  
*             
*             enc_val = ds1103_inc_counter_rd(7);
*             counter = enc_val.high;    
*             finecount = enc_val.low;
*          } 
*          This code reads the counter and the finecount of channel 7.
*           
*
* NOTE:
*
******************************************************************************/
__INLINE Int64 ds1103_inc_counter_rd_immediately(UInt16 channel) 
{
  Int64 value;
  UInt32 msr_state;
  UInt16 datl, dath, fine_count;

  value.low = 0;

  msr_state = ds1103_get_interrupt_status();
  DS1103_GLOBAL_INTERRUPT_DISABLE();

  /* do not strobe the output reg. */
  
  if(channel < 6)
  {
    value.high = ((Int32) DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] ) >> 8;
    RTLIB_FORCE_IN_ORDER();
    ds1103_set_interrupt_status(msr_state);
    return(value);
  };


  if(channel == 6)  /* for dig.inc. ch6 */
  {

    if(ds1103_inc_ch6_mode != DS1103_INC_CH6_16BIT)
    {
      /* 32 or 24 bit */

      datl = inc_hanning->ct1datl;
      dath = inc_hanning->ct1dath;
      value.high = (UInt32) ( ( (UInt32)datl ) | ( (UInt32)dath << 16 ) )
                   & ds1103_inc_ch6_msk[ds1103_inc_ch6_mode]; /* mask for 24 Bit */
      
      /* sign extension */
      value.high = value.high << 8;
      value.high = value.high >> 8;
      
      RTLIB_FORCE_IN_ORDER();
      ds1103_set_interrupt_status(msr_state);
      return(value);
    }
    else
    {
      /* 16 bit */
      datl = inc_hanning->ct2dat;
      value.high = (UInt32)datl ;
      RTLIB_FORCE_IN_ORDER();
      ds1103_set_interrupt_status(msr_state);
      return(value) ;
    }

  };

  fine_count = inc_hanning->encsign;
  value.low =    (UInt32) ( ( ( fine_count >> 8) & DS1103_INC_CH7_FINECMSK)
                 - ds1103_fine_count_offs);

  if(ds1103_inc_ch7_mode == DS1103_INC_CH7_38BIT)
  {
    datl = inc_hanning->ct1datl;
    dath = inc_hanning->ct1dath;
    value.high =   (Int32)( (UInt32)datl | ( (UInt32)dath << 16 ) );
  }
  else
  {
    datl = inc_hanning->ct2dat;
    value.high = (Int32)datl;
  }

  RTLIB_FORCE_IN_ORDER();
  ds1103_set_interrupt_status(msr_state);
  return(value);

}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_inc_counter_clear(UInt16 channel)
*
* SYNTAX:
*       ds1103_inc_counter_clear(UInt16 channel)
*
* DESCRIPTION: function to clear the incremental encoder interface counter
*
* PARAMETERS: channel:  logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_inc_counter_clear(UInt16 channel)
{
   UInt16 fine_count;
   rtlib_int_status_t int_status;

    RTLIB_INT_SAVE_AND_DISABLE(int_status);
  
   switch( channel )
   {
   case 1:  
            ds1103_inc_counter[1] = 0;
            *DS1103_RSTENC_REG = 0x01; /* resets the dig. counter */
            break;

   case 2: 
            ds1103_inc_counter[2] = 0;
            *DS1103_RSTENC_REG = 0x02; /* resets the dig. counter */
            break;

   case 3:
            ds1103_inc_counter[3] = 0;
            *DS1103_RSTENC_REG = 0x04; /* resets the dig. counter */
            break;

   case 4:  
            ds1103_inc_counter[4] = 0;
            *DS1103_RSTENC_REG = 0x08; /* resets the dig. counter */
            break;

   case 5:  
            ds1103_inc_counter[5] = 0;
            *DS1103_RSTENC_REG = 0x10; /* resets the dig. counter */
            break;

   case 6: 
           ds1103_inc_counter[6] = 0;
           if(ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT)
           {
             inc_hanning->ct2dat =  0;
             RTLIB_FORCE_IN_ORDER();
             ds1103_inc2_strobe_inp2();
           }
           else
           {
             inc_hanning->ct1datl =  0;
             inc_hanning->ct1dath =  0;
             RTLIB_FORCE_IN_ORDER();                   
             ds1103_inc2_strobe_inp1();
           };
           break;

   case 7: 
           ds1103_inc_counter[7] = 0;

           /* strobes the output reg. */
           *DS1103_IO_STROBE_REG = (0x400 << ds1103_inc_chan[channel] );
           RTLIB_FORCE_IN_ORDER();
           fine_count = inc_hanning->encsign;
           
           ds1103_fine_count_offs = (Int32) ( ( fine_count >> 8)
              & DS1103_INC_CH7_FINECMSK);  /* stores the finecount */
           ds1103_fine_count = ds1103_fine_count_offs;

           if(ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT)
           {
             inc_hanning->ct2dat =  0;
             ds1103_inc2_strobe_inp2();
           }
           else
           {
             inc_hanning->ct1datl =  0;
             inc_hanning->ct1dath =  0;
             RTLIB_FORCE_IN_ORDER();
             ds1103_inc2_strobe_inp1();
           };
           break;

   default: break;

  } /*switch*/
 
  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

}

 
    
    

/******************************************************************************
*
* FUNCTION:
*   void ds1103_inc_counter_write(UInt16 channel, Int32 count)
*
* SYNTAX:
*     ds1103_inc_counter_write(UInt16 channel, Int32 count)
*
* DESCRIPTION: function to write to the incremental encoder interface counter
*
* PARAMETERS: channel: logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*
*             count:   new value for the counter register
*
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_inc_counter_write(UInt16 channel, Int32 count)
{
  UInt16 fine_count;
  rtlib_int_status_t int_status;
  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if(channel < 6)
  {
    ds1103_inc_counter[channel] = count << 8;
    DS1103_DIGITENCOD_REG[ ds1103_inc_dig_offs[channel] ] = count << 8; /* 24 bit reg. */
  }
  else
  {
    ds1103_inc_counter[channel] = count;

	if(channel == 7)
    {
      /* strobes the output reg. */
      *DS1103_IO_STROBE_REG = (0x400 << ds1103_inc_chan[channel] );
      RTLIB_FORCE_IN_ORDER();
      fine_count = inc_hanning->encsign;
      ds1103_fine_count_offs = (Int32) ( ( fine_count >> 8)
        & DS1103_INC_CH7_FINECMSK);  /* stores the finecount */
      ds1103_fine_count = ds1103_fine_count_offs;
    };

    if( ( (channel == 6 ) && (ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT) )
       || ( (channel == 7 ) && (ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT) ) )
    {

      inc_hanning->ct2dat =  (Int16)(count);
      RTLIB_FORCE_IN_ORDER();
      ds1103_inc2_strobe_inp2();
    }
    else
    {
      inc_hanning->ct1datl =  (Int16)(count);
      inc_hanning->ct1dath =  (Int16)((count) >> 16);
      RTLIB_FORCE_IN_ORDER();
      ds1103_inc2_strobe_inp1();
    };

      };

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

}



/******************************************************************************
*
* FUNCTION:
*   void ds1103_inc_ch6ch7_write_latch(UInt16 channel, UInt32 count)
*
* SYNTAX:
*       ds1103_inc_ch6ch7_write_latch(UInt16 channel, UInt32 count)
*
* DESCRIPTION: function to write to the input latch of the incremental encoder
*              counter channel 6 and channel 7 (Hanning Tacho Controller)
*
* PARAMETERS:  count: new value for the counter register
*
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void    ds1103_inc_ch6ch7_write_latch(UInt16 channel, UInt32 count)
{
  rtlib_int_status_t int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* store the counter */
  ds1103_ch6ch7_latch[channel - 6] = count;

  if( ( (channel == 6 ) && (ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT) )
     || ( (channel == 7 ) && (ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT) ) )
  {
      inc_hanning->ct2dat =  (Int16)(count);
  }
  else
  {
      /* add the upper two bits of the finecount which are stored in the offs */
      inc_hanning->ct1datl =  (Int16) count; /* add upper two bits of finecount to count */
      inc_hanning->ct1dath =  (Int16)((count) >> 16);
  };

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

}






/******************************************************************************
*
* FUNCTION:
*   Int16 ds1103_inc_index_read(UInt16 channel, UInt16 reset_enable)
*
* SYNTAX:
*       ds1103_inc_index_read(UInt16 channel, UInt16 reset_enable)
*
* DESCRIPTION: function to read index pulse of incremental encoder channel
*
* PARAMETERS:  logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*
*              reset_enable: DS1103_INC_CHx_NO_RESETONIDX (0)
*                             disables the next Reset-on-Index
*                            DS1103_INC_CHx_RESETONIDX (1)
*                             enables the next Reset-on-Index
*
* RETURNS: DS1103_INC_IDX_NOT_SET if the indexflag is cleared,
*          DS1103_INC_IDX_SET if the indexflag is set.
*
* NOTE: 
*
*
******************************************************************************/
__INLINE Int16 ds1103_inc_index_read(UInt16 channel, UInt16 reset_enable)
{
  UInt32  mask;
  rtlib_int_status_t int_status;
  UInt16 cntcon;
  

   RTLIB_INT_SAVE_AND_DISABLE(int_status);
   
  /* reads the int-reg. */
  mask =   ( (*DS1103_INTERRUPT_REG) & (0x8000 << ds1103_inc_iir[channel]) ) ;
  RTLIB_FORCE_IN_ORDER();
  
  if( mask != 0)
    *DS1103_INTERRUPT_REG = mask;  /* clear index line */


  /* for reset on index with the Hanning: set ctl1len for counter 1 or
     ctl2len for counter 2*/
  if( (mask == ds1103_inc_iirmsk[channel] ) && ( ds1103_inc_rstonidx[channel] == DS1103_ON ) )
  {
    /* for delta_pos: rstonidx is happen */
    ds1103_rstonidx_flag[channel] = DS1103_ON;

    ds1103_inc_rstonidx_state[channel] = reset_enable;

    if(channel < 6)
    {   
      *DS1103_SETUP_REG =  ( *DS1103_SETUP_REG & ( ~ds1103_inc_setup_rst[channel] ) )
                            |  ( ds1103_inc_setup_rst[channel] * reset_enable ); 
    }
    else
    {
      if(reset_enable == DS1103_ON)
      {
        /* see ds1103_inc_reset_enable() for comments */

        if( ( (channel == 6 ) && (ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT) )
          || ( (channel == 7 ) && (ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT) ) )
        {
          cntcon=inc_cntcon_reg;
          cntcon=cntcon & 0x77FF;
          inc_cntcon_reg = cntcon ;   
          inc_cntcon_reg = cntcon | DS1103_HANINC_CT2LEN;   
        }
        else
        {
          cntcon=inc_cntcon_reg;
          cntcon=cntcon & 0x77FF;
          inc_cntcon_reg = cntcon ;   
          inc_cntcon_reg = cntcon | DS1103_HANINC_CT1LEN;   
        }
      }   /*reset_enable*/
   }   /*ch6-7*/
  }   /*index*/

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);

  return ( (Int16) (mask != 0) );

}


 
/******************************************************************************
*
* FUNCTION:
*   void ds1103_inc_reset_enable(UInt16 channel)
*
* SYNTAX:
*       ds1103_inc_reset_enable(UInt16 channel)
*
* DESCRIPTION: function to enable reset on index, after an index signal is
*              occured. Does the same as the function ds1103_inc_index_read
*              without reading the Interruptregister.
*
* PARAMETERS:  logical channel number
*                        1, 2, 3, 4, 5, 6, 7
*
*
* RETURNS: nothing
*
* NOTE: 
*
*
******************************************************************************/
__INLINE void ds1103_inc_reset_enable(UInt16 channel)
{
    rtlib_int_status_t int_status;
    UInt16 cntcon;

    RTLIB_INT_SAVE_AND_DISABLE(int_status);

    ds1103_inc_rstonidx_state[channel] = DS1103_INC_RSTONIDX_ACTIVE;

    if(channel < 6)
      *DS1103_SETUP_REG =  ( *DS1103_SETUP_REG & ( ~ds1103_inc_setup_rst[channel] ) )
                            |  ( ds1103_inc_setup_rst[channel] );  
    else
    {
     
      /* for reset on index with the Hanning: 
       * reset ctlXlen and 
       * set ctl1len for counter 1 
       * or ctl2len for counter 2*/
      if( ( (channel == 6 ) && (ds1103_inc_ch6_mode == DS1103_INC_CH6_16BIT) )
        || ( (channel == 7 ) && (ds1103_inc_ch7_mode == DS1103_INC_CH7_22BIT) ) )
      {
       /* get control register state from hanning chip */
       cntcon=inc_cntcon_reg;
       /* mask CT1LEN and CT2LEN for deactivating load from latch*/
       cntcon=cntcon & 0x77FF;
       /* deactivate CT1LEN and CT2LEN */
       inc_cntcon_reg = cntcon ;   
       /* activate load from latch counter 2 on next index*/
       inc_cntcon_reg = cntcon | DS1103_HANINC_CT2LEN;   
      }
      else
      {
       /* get control register state from hanning chip */
       cntcon=inc_cntcon_reg;
       /* mask CT1LEN and CT2LEN */
       cntcon=cntcon & 0x77FF;
       /* deactivate CT1LEN and CT2LEN */
       inc_cntcon_reg = cntcon ;   
       /* activate load from latch counter 1 on next index*/
       inc_cntcon_reg = cntcon | DS1103_HANINC_CT1LEN;   
      };
    }; 

    RTLIB_FORCE_IN_ORDER();
    RTLIB_INT_RESTORE(int_status);
}


/******************************************************************************
*
* FUNCTION:
*   void ds1103_bit_io_init(UInt16 mask)
*
* SYNTAX:
*      ds1103_bit_io_init(UInt16 mask)
*
* DESCRIPTION: This function initializes the Digital Port. Each byte (channel) of
*              the 32 bit wide port can be configured as in- or output.
*
*
* PARAMETERS: mask: bit 0 to bit 3 of mask defines the operating modes of the
*                   channel. A zero configures the corresponding 8 bits as an
*                   input-channel, a one as an output-channel.
*
*            Example: mask = 1 | 2 | 4   configures byte 0 to byte 2 as an
*                                        output-channel, byte 3 as an input-
*                                        channel.  
*                 
*
*
* RETURNS: nothing
*
* NOTE: use the definitions DS1103_DIOx_OUT
*           and             DS1103_DIOx_IN   (def. in Io1103.h)
*       
*       Example: mask = DS1103_DIO1_IN | DS1103_DIO2_IN | DS1103_DIO3_IN |
*                       DS1103_DIO4_OUT 
*                does the same as the example before.
*
******************************************************************************/
__INLINE void ds1103_bit_io_init(UInt16 mask)
{
  rtlib_int_status_t int_status;

   RTLIB_INT_SAVE_AND_DISABLE(int_status);

  rest_bit.digsel = mask;
  ds1103_bit_io_write(0);

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);
}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_bit_io_config(UInt16 mask)
*
* SYNTAX:
*      ds1103_bit_io_config(UInt16 mask)
*
* DESCRIPTION: This function configures the Digital Port. Each byte (channel) of
*              the 32 bit wide port can be configured as in- or output.
*
*
* PARAMETERS: mask: bit 0 to bit 3 of mask defines the operating modes of the
*                   channel. A zero configures the corresponding 8 bits as an
*                   input-channel, a one as an output-channel.
*
*            Example: mask = 1 | 2 | 4   configures byte 0 to byte 2 as an
*                                        output-channel, byte 3 as an input-
*                                        channel.  
*                 
*
*
* RETURNS: nothing
*
* NOTE: use the definitions DS1103_DIOx_OUT
*           and             DS1103_DIOx_IN   (def. in Io1103.h)
*       
*       Example: mask = DS1103_DIO1_IN | DS1103_DIO2_IN | DS1103_DIO3_IN |
*                       DS1103_DIO4_OUT 
*                does the same as the example before.
*
******************************************************************************/
__INLINE void ds1103_bit_io_config(UInt16 mask)
{
  rtlib_int_status_t int_status;

   RTLIB_INT_SAVE_AND_DISABLE(int_status);

  rest_bit.digsel = mask;

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);
}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_bit_io_write(UInt32 value)
*
* SYNTAX:
*      ds1103_bit_io_write(UInt32 value)
*
* DESCRIPTION: This function writes an 32 bit value to the digital port.
*              A one sets the corresponding bit, a zero clears it.
*
* PARAMETERS: value: 32 bit wide value which is written to the digital port.
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
__INLINE void ds1103_bit_io_write(UInt32 value)
{
  ( *DS1103_DIGIO_REG ) = value;
  RTLIB_FORCE_IN_ORDER();
}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_bit_io_set(UInt32 mask)
*
* SYNTAX:
*      ds1103_bit_io_set(UInt32 mask)
*
* DESCRIPTION: This function sets bits of the digital port according to the
*              parameter mask.
*
* PARAMETERS: mask: defines the bits. A one sets the corresponding bits, a 
*                   zero has no effect.
*
* RETURNS: nothing
*
* NOTE: use the definitions: DS1103_DIOx_SET (def. in Io1103.h)
*    
*       Example: mask = DS1103_DIO4_SET | DS1103_DIO31_SET
*                sets bit 4 and bit 31 of the digital port.
*
******************************************************************************/
__INLINE void ds1103_bit_io_set(UInt32 mask)
{
  rtlib_int_status_t int_status;
  
   RTLIB_INT_SAVE_AND_DISABLE(int_status);

  ( *DS1103_DIGIO_REG ) = ( *DS1103_DIGIO_REG ) | mask;
    
  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);
}




/******************************************************************************
*
* FUNCTION:
*   void ds1103_bit_io_clear(UInt32 mask)
*
* SYNTAX:
*      ds1103_bit_io_clear(UInt32 mask)
*
* DESCRIPTION: This functions clears the bits which are defined by the parameter
*              mask.
*
* PARAMETERS: mask: a one clears the corresponding bits, a zero has no effect.
*
* RETURNS: nothing
*
* NOTE: use the definitions: DS1103_DIOx_CLEAR (def. in Io1103.h)
*    
*       Example: mask = DS1103_DIO4_CLEAR | DS1103_DIO31_CLEAR 
*                clears bit 4 and bit 31 of the digital port.
*
******************************************************************************/
__INLINE void ds1103_bit_io_clear(UInt32 mask)
{
  rtlib_int_status_t int_status;
  
   RTLIB_INT_SAVE_AND_DISABLE(int_status);
    
  ( *DS1103_DIGIO_REG ) = ( ( *DS1103_DIGIO_REG ) & (~mask) );

  RTLIB_FORCE_IN_ORDER();
  RTLIB_INT_RESTORE(int_status);
}






/******************************************************************************
*
* FUNCTION:
*   UInt32 ds1103_bit_io_read(void)
*
* SYNTAX:
*      ds1103_bit_io_read(void)
*
* DESCRIPTION: This function reads the contents of the 32 bit digital port.
*
* PARAMETERS: no
*
* RETURNS: the 32 bit value of the digital port.
*
* NOTE:
*
******************************************************************************/
__INLINE UInt32 ds1103_bit_io_read(void)
{
  UInt32 value;

  value = ( *DS1103_DIGIO_REG );
  RTLIB_FORCE_IN_ORDER();
  return(value);

}





/******************************************************************************
*
* FUNCTION:
*   Int16 ds1103_cpu_temp_read(Int16* temperature)
*
* SYNTAX:
*      ds1103_cpu_temp_read(Int16* temperature)
*
* DESCRIPTION: This function reads the temperature of the CPU.
*              This is done in 3 cycles.
*              If the function returns DS1103_RDTMP_FINISHED, the value
*              for the temperature is valid.
*
* PARAMETERS: Int16* temperature: destination address where the temperature 
*                                 is written to.
*
* RETURNS: DS1103_RDTMP_WAIT_RDY1: after the 1. cycle.
*          DS1103_RDTMP_WAIT_RDY2: after the 2. cycle. 
*          DS1103_RDTMP_FINISHED:  if reading is finished.
*          DS1103_RDTMP_ERROR: if an error occured.
*
* NOTE:    The function writes the temperature into the Config-Section.
*          If this function is called in the background, the Platform
*          Manager of ControlDesk is able to display the temperature of
*          the CPU. 
*          Instead of using this function, the application can call 
*          ds1103_cpu_temperature().
*          The function is not reentrant.
*
******************************************************************************/
__INLINE Int16 ds1103_cpu_temp_read(Int16* temperature)
{
  static Int16 temp_state = 0;
  Int8 temp = 0;
  Int16 eeprom = 0;


  if(temp_state == 0)  /*reading in process? */
  {
    /*write addr. of temp.sensor */
    eeprom_reg = (DS1103_EEPROM_RD_TMP + DS1103_TMP_SENSOR);
    temp_state = 1;
    RTLIB_FORCE_IN_ORDER();
    return(DS1103_RDTMP_WAIT_RDY1);
  }
  else if(temp_state == 1)
  {
    if( (eeprom_bit.eerdy) && (eeprom_bit.eeack) )  /*addr. rdy? */
    { 
      temp_state = 2;
      RTLIB_FORCE_IN_ORDER();
      eeprom_reg = (DS1103_RD_ACCESS + DS1103_TMP_SENSOR);
      RTLIB_FORCE_IN_ORDER();
      return(DS1103_RDTMP_WAIT_RDY2);
    }
    else
    {
      RTLIB_FORCE_IN_ORDER();
      return(DS1103_RDTMP_WAIT_RDY1);
    };
  }
  else if(temp_state == 2)
  {
    eeprom = eeprom_reg;
    if( ( ( eeprom & DS1103_EEPROM_EERDY ) && ( eeprom & DS1103_EEPROM_EERACK ) ) )
    {
      temp_state = 0;
      RTLIB_FORCE_IN_ORDER();  
      temp = (Int8) (eeprom >> 8);
      (ds1103_cfg_section->__cfg_cpu_temp) = (Int32)temp;
      ds1103_ppc_temperature = temp;
      *temperature = (Int16)temp;
      RTLIB_FORCE_IN_ORDER();
      return(DS1103_RDTMP_FINISHED);
    }
    else
    {
      RTLIB_FORCE_IN_ORDER();
      return(DS1103_RDTMP_WAIT_RDY2);
    };
  };
  return(DS1103_RDTMP_ERROR);

}




/******************************************************************************
*
* FUNCTION:
*   Int16 ds1103_cpu_temperature(void)
*
* SYNTAX:
*   ds1103_cpu_temperature(void)
*
* DESCRIPTION: This function reads the cpu-temperature in one cycle and needs 
*              very much time (>100 us)! 
*
* PARAMETERS: no
*
* RETURNS: the temperature of the CPU.
*
* NOTE:    The function writes the temperature into the Config-Section.
*          The function is not reentrant.
*
******************************************************************************/
__INLINE Int16 ds1103_cpu_temperature(void)
{
  Int8 temp = 0;

    /*write addr. of temp.sensor */
    eeprom_reg = (DS1103_EEPROM_RD_TMP + DS1103_TMP_SENSOR);
    RTLIB_FORCE_IN_ORDER();
    /* wait until addr. rdy */

  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_CPU_TEMP_POLL_START);
  #endif

    while( !( (eeprom_bit.eerdy) && (eeprom_bit.eeack) ) );

  #ifdef DEBUG_POLL
    DS1103_MSG_INFO_SET(DS1103_CPU_TEMP_POLL_END);
  #endif
   
    eeprom_reg = (DS1103_RD_ACCESS + DS1103_TMP_SENSOR);
    RTLIB_FORCE_IN_ORDER();
    /* wait until addr. rdy */

  #ifdef DEBUG_POLL
   DS1103_MSG_INFO_SET(DS1103_CPU_TEMP_POLL_START);
  #endif
    
    while( !( (eeprom_bit.eerdy) && (eeprom_bit.eeack) ) );

  #ifdef DEBUG_POLL
   DS1103_MSG_INFO_SET(DS1103_CPU_TEMP_POLL_END);
  #endif

    temp =  (Int8) (eeprom_reg >> 8);
    (ds1103_cfg_section->__cfg_cpu_temp) = (Int32)temp;
    ds1103_ppc_temperature = (Int16)temp;
    RTLIB_FORCE_IN_ORDER();

    return((Int16)temp);
}


/******************************************************************************
*
* FUNCTION:
*   void ds1103_external_trigger_enable()
*
* SYNTAX: 
*   ds1103_external_trigger_enable()
*
* DESCRIPTION: This macro enables the external trigger
*
* PARAMETERS: none
*
* RETURNS: nothing
*
* NOTE: Restart of ds1103_slave_dsp_pwm3_init() is necessary to disable external trigger.
*       macro is defined in IO1103.h
*
******************************************************************************/

#undef __INLINE


