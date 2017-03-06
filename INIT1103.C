/******************************************************************************
*
* MODULE
*   This module contains the functions for initializing the DS1103.
*
*
* FILE
*   Init1103.c
*
* RELATED FILES
*   Init1103.h
*
* DESCRIPTION
*   - ds1103_init_register inits the I/O and some other registers.
*   - ds1103_board_init calls several init-functions for I/O and
*     services.
*
* AUTHOR(S)
*   R. Kraft
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: INIT1103.C $ $Revision: 1.7 $ $Date: 2008/07/01 16:05:59GMT+01:00 $
*
******************************************************************************/


#include <Dstypes.h>
#include <Init1103.h>
#include <Hostsvc.h>
#include <Dsmem.h>
#include <Cfg1103.h>
#include <Dsmsg.h>
#include <ds1103_msg.h>

#include <Io1103.h>
#include <Tmr1103.h>
#include <Ser1103.h>
#include <Int1103.h>
#include <def1103.h>
#include <tic1103.h>
#include <slvdsp1103.h>
#include <can1103.h>
#include <dsstd.h>
#include <dsvcm.h>
#include <dselog.h>
#include <dsmcr.h>
#include <dsser.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/
  #define SINT_INIT_NO_ERROR     0
  #define SINT_INIT_ERROR       -1

/******************************************************************************
  global variables
******************************************************************************/
extern UInt32* ds1103_stack_bottom;

extern UInt32 ds1103_stack_ctrl_overflow;

dssint_receiver_type *dssint_slvdsp_fw_receiver,
                     *dssint_slvdsp_usr_receiver,
                     *dssint_slvmc_receiver;
dssint_sender_type   *dssint_slvdsp_sender,
                     *dssint_slvmc_sender;

/* global variable for fast board differentiation between Boards Rev > 1.7*/
volatile UInt16 ds1103_advanced_io;
/******************************************************************************
  object declarations
******************************************************************************/

/******************************************************************************
  function prototypes
******************************************************************************/
void ds1103_power_down(void);   /* function is defined in crt0 */
                                /* function is defined in crt0 */
void ds1103_data_cache_block_flush(UInt32 address);

/******************************************************************************
  version information
******************************************************************************/

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#pragma asm

.equ  _rtlib1103_ver_mar, RTLIB1103_VER_MAR
.equ  _rtlib1103_ver_mir, RTLIB1103_VER_MIR
.equ  _rtlib1103_ver_mai, RTLIB1103_VER_MAI
.equ  _rtlib1103_ver_spb, RTLIB1103_VER_SPB
.equ  _rtlib1103_ver_spn, RTLIB1103_VER_SPN
.equ  _rtlib1103_ver_plv, RTLIB1103_VER_PLV

#pragma endasm
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/******************************************************************************
  function declarations
******************************************************************************/


/******************************************************************************
*
* FUNCTION:
*           void ds1103_init_register(void)
*
* SYNTAX:
*           ds1103_init_register(void)
*
* DESCRIPTION:
*            This function inits the I/O registers.
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE: This function is called by ds1103_board_init().
*
******************************************************************************/

 void ds1103_init_register(void)
{


DS1103_GLOBAL_INTERRUPT_DISABLE();


/* resets the interrupt-mask-register: all interrupts disabled */
*(volatile UInt32*)DS1103_INT_MASK_REG = 0xFFFFFFFF;


/* resets the interrupt-register */
*(volatile UInt32*)DS1103_INTERRUPT = (*(volatile UInt32*)DS1103_INTERRUPT) & 0xFFE00000;


/* resets DAC, UART, CAN-Ctrl. Analog-Inc, Slave-Dsp, DIG-IO: input,
Bootcan: Flash, Delayed Access: disabled */
*(volatile UInt16*)DS1103_RESET = 0x101F;

/* wake up DAC, UART */
*(volatile UInt16*)DS1103_RESET = (*(volatile UInt16*)DS1103_RESET) & 0xFFFC;

/*resets Digital-Inc by writing ones*/
*(volatile UInt16*)DS1103_DIGINC_RESET = 0x1F;


/* sets the TMRSTB-register to zero */
*(volatile UInt16*)DS1103_TMRSTB = 0;


/* set analog ch 7 in Vpp-mode, leave slvboot unchanged,
   set rest of setup register to zero */
*(volatile UInt16*)DS1103_SETUP = (*(volatile UInt16*)DS1103_SETUP & 0x40) | 8;


/*resets ADCCTRL- register*/
*(volatile UInt16*)DS1103_ADC_CTRL = 0;

/*resets SYNCCTRL - register (only for p750 boards)*/
*(volatile UInt32*)DS1103_SYNC_CTRL = 0;


/* timer */

/* sets period-register to 0xffffffff */
*(volatile UInt32*)DS1103_TMRA_PERIOD = 0xFFFFFFFF;


/* sets compare-register to 0xffffffff */
*(volatile UInt32*)DS1103_TMRB_COMPARE = 0xFFFFFFFF;


/* holds timera, holds timerb, set input clock-period of timerb to 60ns */
*(volatile UInt32*)DS1103_TMR_CTRL = *(volatile UInt16*)DS1103_TMR_CTRL & 0xFFFFFFC0;


}



/******************************************************************************
*
* FUNCTION:
*         Exit function
*
* SYNTAX:
*         ds1103_exit(void)
*
* DESCRIPTION: In the case of exit, this function disables the interrupts,
*              sets an error message and calls the host_service(0,0)
*              (for Cockpit).
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE: This function is installed (atexit) by ds1103_init().
*
******************************************************************************/

void ds1103_exit(void)
{
  UInt32 address = 0;
  global_disable();                           /* disable interrupts */

  if ((*ds1103_stack_bottom != STACK_INIT_CODE) && (!ds1103_stack_ctrl_overflow))
  {
    DS1103_MSG_ERROR_SET(MSG_STACK_OVERFLOW_ERROR);
  }

  DS1103_MSG_ERROR_SET(MSG_TERMINATED);

  if (!ds1103_cfg_section->__cfg_overtemp_flag)
  {
    while(1)
    {
      if (address < ds1103_cfg_section->__cfg_local_ram_size)
      {
        ds1103_data_cache_block_flush(address);
        address += 4;
      }
      else   /* cache flushing is ready */
      {
        ds1103_cfg_section->__cfg_data_cache_flushed = 1;
      }
      master_cmd_server();
      host_service(0,0);
    }
  }
  else
    ds1103_power_down();
}

/******************************************************************************
*
* FUNCTION
*   sub-interrupt initialization function for PPC on DS1103
*
* SYNTAX
*   int sint1103_init(void)
*
* DESCRIPTION
*    The function sint1103_init initializes the global sender and receiver variables
*    on the PowerPC side using the defines from ds1103.h
*
* PARAMETERS
*
* RETURNS
*   zero if no error occures
*   SINT_INITIALIZATION_ERROR if no memory could be allocated.
*
* REMARKS
*
******************************************************************************/
int sint1103_init(void)
{
  static UInt32 lock = 0;

  if (lock)
    return (SINT_INIT_NO_ERROR);
  lock = 1;

  if (SINT1103_DSP2PPC_FW_SINT_NUMBER)
  {
    if ( (dssint_slvdsp_fw_receiver = dssint_define_int_receiver((UInt16)SINT1103_DSP2PPC_FW_SINT_NUMBER,
                                                             (unsigned long)SINT1103_DSP2PPC_FW_SUBINT_ADDR,
                                                             (unsigned long)SINT1103_DSP2PPC_FW_ACKNOWLEDGE_ADDR,
                                                             (unsigned long)SINT1103_DSP2PPC_SEND_ADDR,
                                                             (int)           DPM_TARGET_DIRECT,
                                                             (unsigned int)  SINT1103_SINT_MEM_WIDTH,
                                                             (dpm_write_fcn_t) DPM_ACCESS_DIRECT,
                                                             (dpm_read_fcn_t) DPM_ACCESS_DIRECT ) ) == NULL)
      return(SINT_INIT_ERROR);
  }

  if (SINT1103_DSP2PPC_USR_SINT_NUMBER)
  {
    if ( (dssint_slvdsp_usr_receiver = dssint_define_int_receiver((UInt16)SINT1103_DSP2PPC_USR_SINT_NUMBER,
                                                              (unsigned long)SINT1103_DSP2PPC_USR_SUBINT_ADDR,
                                                              (unsigned long)SINT1103_DSP2PPC_USR_ACKNOWLEDGE_ADDR,
                                                              (unsigned long)SINT1103_DSP2PPC_SEND_ADDR,
                                                              (int)           DPM_TARGET_DIRECT,
                                                              (unsigned int)  SINT1103_SINT_MEM_WIDTH,
                                                              (dpm_write_fcn_t) DPM_ACCESS_DIRECT,
                                                              (dpm_read_fcn_t) DPM_ACCESS_DIRECT ) ) ==NULL)

      return(SINT_INIT_ERROR);
  }

  if (SINT1103_MC2PPC_SINT_NUMBER)
  {
    if ( (dssint_slvmc_receiver = dssint_define_int_receiver((UInt16)SINT1103_MC2PPC_SINT_NUMBER,
                                                         (unsigned long)SINT1103_MC2PPC_SUBINT_ADDR,
                                                         (unsigned long)SINT1103_MC2PPC_ACKNOWLEDGE_ADDR,
                                                         (unsigned long)SINT1103_MC2PPC_SEND_ADDR,
                                                         (int)           DPM_TARGET_DIRECT,
                                                         (unsigned int)  SINT1103_SINT_MEM_WIDTH,
                                                         (dpm_write_fcn_t) DPM_ACCESS_DIRECT,
                                                         (dpm_read_fcn_t) DPM_ACCESS_DIRECT ) ) ==NULL)
      return(SINT_INIT_ERROR);
  }

  if (SINT1103_PPC2DSP_SINT_NUMBER)
  {
    if ( (dssint_slvdsp_sender = dssint_define_int_sender((UInt16)SINT1103_PPC2DSP_SINT_NUMBER,
                                                (unsigned long)SINT1103_PPC2DSP_SUBINT_ADDR,
                                                (unsigned long)SINT1103_PPC2DSP_ACKNOWLEDGE_ADDR,
                                                (unsigned long)SINT1103_PPC2DSP_SEND_ADDR,
                                                (int)           DPM_TARGET_DIRECT,
                                                (unsigned int)  SINT1103_SINT_MEM_WIDTH,
                                                (dpm_write_fcn_t) DPM_ACCESS_DIRECT,
                                                (dpm_read_fcn_t) DPM_ACCESS_DIRECT ) ) ==NULL)

      return(SINT_INIT_ERROR);
  }

  if (SINT1103_PPC2MC_SINT_NUMBER)
  {
    if ( (dssint_slvmc_sender = dssint_define_int_sender((UInt16)SINT1103_PPC2MC_SINT_NUMBER,
                                                (unsigned long)SINT1103_PPC2MC_SUBINT_ADDR,
                                                (unsigned long)SINT1103_PPC2MC_ACKNOWLEDGE_ADDR,
                                                (unsigned long)SINT1103_PPC2MC_SEND_ADDR,
                                                (int)           DPM_TARGET_DIRECT,
                                                (unsigned int)  SINT1103_SINT_MEM_WIDTH,
                                                (dpm_write_fcn_t) DPM_ACCESS_DIRECT,
                                                (dpm_read_fcn_t) DPM_ACCESS_DIRECT ) ) ==NULL)

      return(SINT_INIT_ERROR);
  };


  return(SINT_INIT_NO_ERROR);
}






/******************************************************************************
*
* FUNCTION:
*         void ds1103_init (void)
*
* SYNTAX:
*         ds1103_init(void)
*
* DESCRIPTION: This function calls several init-functions of the DS1103-SWENV.
*
* PARAMETERS:
*
* RETURNS: nothing
*
* NOTE: This function is called by init().
*
******************************************************************************/

 void ds1103_init (void)
{
  volatile UInt16 *rst = (UInt16 *) 0x10000040;            /* reset register */
  UInt32 i;
  /* rtlib vcm entry pointer */
  vcm_module_descriptor_type *rtlib_version_ptr;
  static int rtlib_initialized = 0;
  Float64 time;
  ts_timestamp_type start_time, end_time;
  

  if (!rtlib_initialized)
  {

    /* ---- DSVCM module ---------------------------------------------------- */

    vcm_init();


    /* ---- register settings ----------------------------------------------- */

    ds1103_init_register();


    /* ---- global frequency and period variables --------------------------- */

    ds1103_bus_clck = (Float64) (ds1103_cfg_section->__cfg_bus_clock);
    ds1103_bus_prd  = 1/ds1103_bus_clck;
    ds1103_tmra_frq = ds1103_bus_clck * 0.5;
    ds1103_tmra_prd = ds1103_bus_prd * 2;
    ds1103_decr_frq = ds1103_bus_clck * 0.25;
    ds1103_decr_prd = ds1103_bus_prd * 4;

    /* ---- memory management ----------------------------------------------- */

    dsmem_init();

    /* ---- register rtlib1103 ---------------------------------------------- */

    rtlib_version_ptr = vcm_module_register(
                         VCM_MID_RTLIB1103, 0, VCM_TXT_RTLIB1103,
                         RTLIB1103_VER_MAR, RTLIB1103_VER_MIR, RTLIB1103_VER_MAI,
                         RTLIB1103_VER_SPB, RTLIB1103_VER_SPN, RTLIB1103_VER_PLV,
                         0, 0);

    /* ---- register DSMCOM module ------------------------------------------ */

    dsmcom_vcm_register();

	/* ---- Advanced IO   -------------------------------------------------- */

	if(ds1103_cfg_section->__cfg_version < 8) 
		ds1103_advanced_io=0;
	else 
		ds1103_advanced_io=1;



    /* ---- message module -------------------------------------------------- */

    msg_init();

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_MSG_INIT);

	
    /* ---- register DSMEM module ------------------------------------------- */

    dsmem_vcm_register();


    /* ---- register DSSTD module ------------------------------------------- */

    dsstd_vcm_register();

    /* ---- DSSER module ---------------------------------------------------- */

    dsser_vcm_register();

    /* ---- host services --------------------------------------------------- */

    hsvc_init_hostsvc();

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_HOSTSVC_INIT);


    /* ---- DSSINT module --------------------------------------------------- */

    if(sint1103_init() != SINT_INIT_NO_ERROR)
    {
      DS1103_MSG_ERROR_SET(DS1103_SUBINT_MODULE_ERROR);
      exit(1);
    };


    /* ---- register DSSINT module ------------------------------------------ */

    dssint_vcm_register();

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_SUBINT_INIT);


    /* ---- time stamping module -------------------------------------------- */

    ts_init(TS_MODE_SINGLE, 0.0);

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_DSTS_INIT);


    /* ---- IO and interfaces ----------------------------------------------- */

    ds1103_adc_mux(1);
    ds1103_adc_mux(5);
    ds1103_adc_mux(9);
    ds1103_adc_mux(13);

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_ADC_MUX_INIT);

    ds1103_dac_init(DS1103_DACMODE_TRANSPARENT);

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_DAC_INIT);

    ds1103_inc_init(1, DS1103_INC_CH1_TTL);
    ds1103_inc_init(2, DS1103_INC_CH2_TTL);
    ds1103_inc_init(3, DS1103_INC_CH3_TTL);
    ds1103_inc_init(4, DS1103_INC_CH4_TTL);
    ds1103_inc_init(5, DS1103_INC_CH5_TTL);
    ds1103_inc_init(6, DS1103_INC_CH6_16BIT | DS1103_INC_CH6_RS422);
    ds1103_inc_init(7, DS1103_INC_CH7_38BIT | DS1103_INC_CH7_VPP);

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_INC_ENCODER_INIT);


    /* dig. IO to input */
    ds1103_bit_io_init(DS1103_DIO1_IN + DS1103_DIO2_IN + DS1103_DIO3_IN + DS1103_DIO4_IN);

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_DIG_BIO_INIT);

    ds1103_serial_init(19200, 8, 1, DS1103_SER_NO_PARITY, DS1103_SER_RS232,
                          14, DS1103_SER_AUTOFLOW_DISABLE, DS1103_SER_INTERRUPT_DISABLE);

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_UART_INIT);

    /* resets the interrupt-register */
    *(volatile UInt32*)DS1103_INTERRUPT = (*(volatile UInt32*)DS1103_INTERRUPT) &
                                            0xFFE00000;

    /* get slave DSP firmware revision */
    /* clear slave DSP DPMEM, except sub-interrupt locations */
    for (i = DS1103_SLAVE_DSP_DP_RAM_START;
        i < (DS1103_SLAVE_DSP_DP_RAM_START + DS1103_SLAVE_DSP_DP_RAM_SIZE - 0x10);
        i += 2 )
      * (UInt16 *) i = 0 ;

    *rst |= SLVDSP1103_RSTSLV;                              /* stop slave DSP */
    *rst &= ~SLVDSP1103_RSTSLV;                            /* start slave DSP */

    /* wait for slave DSP writing firmware revision to DPMEM */
    ts_timestamp_read(&start_time);
 
    do
    {
      ts_timestamp_read(&end_time);
      time= ts_timestamp_interval(&start_time,&end_time);
      if ( time > DSCOMDEF_TIMEOUT)
      {
        break;                               /* timeout, if no slave response */
      }
    } while (*(volatile UInt16 *)DEF1103_SLVDSP_DSPACE_FW_ADDR == 0);

    /* read firmware rev. and entry to config section */
    ds1103_cfg_section->__cfg_slave_dsp_ds_fwrev =
      *(volatile UInt16 *)DEF1103_SLVDSP_DSPACE_FW_ADDR;

    /* read user firmware rev. and entry to config section */
    ds1103_cfg_section->__cfg_slave_dsp_usr_fwrev[0] =
      *(volatile UInt16 *)DEF1103_SLVDSP_USER_MAJ_FW_ADDR;
    ds1103_cfg_section->__cfg_slave_dsp_usr_fwrev[1] =
      *(volatile UInt16 *)DEF1103_SLVDSP_USER_MIN_FW_ADDR;
    ds1103_cfg_section->__cfg_slave_dsp_usr_fwrev[2] =
      *(volatile UInt16 *)DEF1103_SLVDSP_USER_SUB_FW_ADDR;

    *rst |= SLVDSP1103_RSTSLV;                              /* stop slave DSP */

    /* get CAN-MC firmware revision */
    /* clear CAN DPMEM, except sub-interrupt locations */
    for (i = DS1103_SLAVE_MC_DP_RAM_START;
         i < (DS1103_SLAVE_MC_DP_RAM_START + DS1103_SLAVE_MC_DP_RAM_SIZE - 0x10);
         i += 2 )
      * (UInt16 *) i = 0 ;

    *rst |= DS1103_CAN_RSTSLV;                                 /* stop CAN-MC */
    *rst |= DS1103_CAN_BOOTCAN_FLASH;                      /* set BOOTCAN-Bit */
    *rst &= ~DS1103_CAN_RSTSLV;                               /* start CAN-MC */

    /* wait for CAN-MC writing firmware revision to DPMEM */
    ts_timestamp_read(&start_time);
 
    do
    {
      ts_timestamp_read(&end_time);
      time= ts_timestamp_interval(&start_time,&end_time);
      if ( time > DSCOMDEF_TIMEOUT)
      {
        break;                               /* timeout, if no slave response */
      }
    }
    while (*(volatile UInt16 *)DEF1103_CAN_FIRMWARE_ADDR == 0);

    /* read firmware rev. and entry to config section */
    ds1103_cfg_section->__cfg_slave_mc_ds_fwrev =
      *(volatile UInt16 *)DEF1103_CAN_FIRMWARE_ADDR;

    /* set user firmware rev. entry in config section to zero */
    ds1103_cfg_section->__cfg_slave_mc_usr_fwrev[0] = 0;
    ds1103_cfg_section->__cfg_slave_mc_usr_fwrev[1] = 0;
    ds1103_cfg_section->__cfg_slave_mc_usr_fwrev[2] = 0;

    /* stop CAN-MC */
    *rst |= DS1103_CAN_RSTSLV;

    /* ---- event logging module -------------------------------------------- */

    if (elog_init() == ELOG_NO_ERROR)
    {
      if( ds1103_debug & DS1103_DEBUG_INIT )
        DS1103_MSG_INFO_SET(DS1103_ELOG_INIT);
    }

    /* ---- multi client ringbuffer module ---------------------------------- */

    if (dsmcr_init() == DSMCR_NO_ERROR)
    {
      if( ds1103_debug & DS1103_DEBUG_INIT )
        DS1103_MSG_INFO_SET(DS1103_DSMCR_INIT);
    }

    /* ---- log board information ------------------------------------------- */
    msg_printf(MSG_MC_LOG, MSG_DLG_NONE, MSG_SM_RTLIB, 0, "DS1103 serial number: %d", cfg_section->__cfg_serial_nr);

    /* ---------------------------------------------------------------------- */

    vcm_module_status_set(rtlib_version_ptr,VCM_STATUS_INITIALIZED);

    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_INIT);

    /* ---- exit function --------------------------------------------------- */

    atexit(ds1103_exit);

#if defined(RTLIB_IS_CPP_APPL) && !defined(__cplusplus)
    atexit((void (*)())_cxxfini); /* destroy static object */
#endif /* RTLIB_IS_CPP_APPL */

    /* lock init function */
    rtlib_initialized = 1;
  }
  else /* RTLib already initialized */
  {
    if( ds1103_debug & DS1103_DEBUG_INIT )
      DS1103_MSG_INFO_SET(DS1103_INIT_LOCK);
  }

}

/******************************************************************************
*
* FUNCTION:
*         void ds1103_slv_boot_finished(void)
*
* SYNTAX:
*         ds1103_slv_boot_finished(void)
*
* DESCRIPTION:
*         This function clears the __cfg_slv_boot_flag
*         in the config section.
*
* PARAMETERS:
*         none
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/

void ds1103_slv_boot_finished(void)
{
  ds1103_cfg_section->__cfg_slv_boot_flag = 0;
}




