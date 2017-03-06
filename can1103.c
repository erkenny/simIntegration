/******************************************************************************
*
* MODULE  
*   CAN controller access functions for DS1103 board.
*
*   Version for PPC master CPU. 
*
* FILE  
*   can1103.c   
*
* RELATED FILES
*   can1103.h
*
* DESCRIPTION
*   DS1103 CAN controller access functions for the CAN bus.
*
*
* AUTHOR(S)
*    R. Kraft, A. Gropengieﬂer
*
* Copyright dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: can1103.c $   $Revision: 1.14 $   $Date: 2009/07/02 17:44:03GMT+01:00 $
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <dstypes.h>                      /* platform independent data types */
#include <def1103.h>       /* definitions for master-slave DSP communication */
#include <ds1103.h>                            /* DS1103 address definitions */
#include <can1103.h>
#include <Io1103.h>
#include <dsmcom.h>

#include <math.h>

#include <dsmsg.h>
#include <can1103_msg.h>
#include <dssint.h>
#include <tmr1103.h>
#include <dsstd.h>                            /* dSPACE standard definitions */
#include <dsvcm.h>

#include <dsav1103.h>

/******************************************************************************
* constant and macro definitions
******************************************************************************/

#define DS1103_CAN_TRY(func) if( ((error=(func)) > 0) && (error != DS1103_CAN_DATA_LOST) ) \
                                  {goto Error; }

/* defines for busstatus_read */
#define DS1103_CAN_BUSOFF   0x80
#define DS1103_CAN_WARN     0x40

/* Definements for baudrate calculations */
#define DS1103_CAN_TSYNC        1
#define DS1103_CAN_BRP_MAX      64
#define DS1103_CAN_BRP_MIN      1
#define DS1103_CAN_TSEG1_MAX    16
#define DS1103_CAN_TSEG1_MIN    3
#define DS1103_CAN_TSEG2_MAX    8
#define DS1103_CAN_TSEG2_MIN    2
#define DS1103_CAN_NBT_MAX     (DS1103_CAN_TSEG1_MAX + DS1103_CAN_TSEG2_MAX + DS1103_CAN_TSYNC)
#define DS1103_CAN_NBT_MIN     (DS1103_CAN_TSEG1_MIN + DS1103_CAN_TSEG2_MIN + DS1103_CAN_TSYNC)
#define DS1103_CAN_MUL_MIN     (DS1103_CAN_TSEG1_MIN + DS1103_CAN_TSEG2_MIN + DS1103_CAN_TSYNC)
#define DS1103_CAN_SJW_MAX     4

#define TASK0 0
#define TASK6 6

/* wrap-multp. for 31 Bit Timer on C164 */
#define DS1103_CAN_WRAPCOUNT 2.147483648e9

/* comp. fw 1.x */
#define DS1103_CAN_PROCESSED_INFO   4     


#define DS1103_CAN_SERVICE_MB_ID      0x00800000  

#define DS1103_CAN_FIRMWARE_MAJOR     0xF800
#define DS1103_CAN_FIRMWARE_MINOR     0x0780
#define DS1103_CAN_FIRMWARE_MAINT     0x0078
#define DS1103_CAN_FIRMWARE_VERSION   0x0006
#define DS1103_CAN_FIRMWARE_ORIGIN    0x0001
#define DS1103_CAN_FIRMWARE_FINAL     0x0000
#define DS1103_CAN_FIRMWARE_BETA      0x0002
#define DS1103_CAN_FIRMWARE_ALPHA     0x0004

/* txqueue_init */
#define DS1103_CAN_IS_NOT_TXQUEUED      0
#define DS1103_CAN_IS_TXQUEUED          1

/* channel init */
#define DS1103_CAN_CH_NOT_STARTED       0
#define DS1103_CAN_CH_WAS_STARTED       1

/******************************************************************************
* global objects
******************************************************************************/
extern dssint_receiver_type *dssint_slvmc_receiver;
extern dssint_sender_type *dssint_slvmc_sender;

#ifndef _INLINE

UInt32  ds1103_can_len_arr[12] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 0, 0, 0 };
Float64 ds1103_can_timerticks;
UInt32  ds1103_can_init = 0;
UInt32  ds1103_can_msg_no;
UInt32  ds1103_can_rm_msg_no;
UInt32* ds1103_can_rm_id_list;
UInt32  ds1103_can_glob_mask;
UInt32  ds1103_can_glob_id;
UInt32  ds1103_can_glob_no;
UInt32  ds1103_can_firmware_version;
 Int32  ds1103_can_subints          [ SINT1103_SINT_MEM_WIDTH ];
UInt32  ds1103_can_max_idx          [ DEF1103_CAN_BUFFER_NUM ];
ds1103_canMsg** ds1103_can_msg_map  [ DEF1103_CAN_BUFFER_NUM ];
UInt32 ds1103_can_channel_started; 
ds1103_canChannel *ds1103_canCh = NULL;
dsmcom_cmdtbl_header_t **ds1103_can_task_list;
ds1103_can_subint_handler_t ds1103_can_subint_handler = NULL;

#else
extern UInt32  ds1103_can_len_arr[12];
extern Float64 ds1103_can_timerticks;
extern UInt32  ds1103_can_init;
extern UInt32  ds1103_can_msg_no;
extern UInt32  ds1103_can_rm_msg_no;
extern UInt32* ds1103_can_rm_id_list;
extern UInt32  ds1103_can_glob_mask;
extern UInt32  ds1103_can_glob_id;
extern UInt32  ds1103_can_glob_no;
extern UInt32  ds1103_can_firmware_version;
extern  Int32  ds1103_can_subints           [ SINT1103_SINT_MEM_WIDTH ];
extern UInt32  ds1103_can_max_idx           [ DEF1103_CAN_BUFFER_NUM ];
extern ds1103_canMsg** ds1103_can_msg_map   [ DEF1103_CAN_BUFFER_NUM  ];
extern UInt32 ds1103_can_channel_started; 
extern ds1103_canChannel *ds1103_canCh;
extern dsmcom_cmdtbl_header_t **ds1103_can_task_list;
extern ds1103_can_subint_handler_t ds1103_can_subint_handler;

#endif /* _INLINE */

Int32 ds1103_can_baudrate_calculate_inv (const Int32 baudrate,
                                              UInt32 *BTR0,
                                              UInt32 *BTR1,
                                              UInt32 *clock);

UInt32 ds1103_can_calculate_baudrate(const  UInt32 BTR0,
                                     const  UInt32 BTR1,
                                     const  UInt32 clock);

Int32 ds1103_can_check_all_id (void);

Int32 ds1103_can_check_format(const UInt32 format);

Int32 ds1103_can_check_ident (const ds1103_canMsg* canMsg);

Int32 ds1103_can_check_subint (const Int32 subinterrupt);

Int32 ds1103_can_check_timing (const UInt32  type,
                               const Float32 start_time,
                               const Float32 repetition_time,
                                     Float32* timeout);

Int32 ds1103_can_check_txrm_id (const ds1103_canMsg* canMsg);

Int32 ds1103_can_check_version(const UInt32 fw_version);

void ds1103_can_print_version(UInt32* fw_rev);

/*
  calculates the register values of the
  av9110-clock-generator depending on a given frequency
*/
Int16 ds1103_can_clock_calc (Float32 *freq, UInt8 *vco_div, UInt8 *ref_div);

/* writes vco_div (N) and ref_div (M) into the av9110 */
void ds1103_can_clock_write(UInt8 vco_div, UInt8 ref_div);

void ds1103_can_communication_init (const UInt32 bufferwarn);

void ds1103_can_error_handler (const Int32 error, const char* function);

Int32 ds1103_can_register (ds1103_canMsg* canMsg,
                                  Int32* index,
                           const UInt32 op_code,
                           const UInt32 doutcnt,
                           const UInt32 reg_pcnt,
                           const UInt32* parms);

Int32 ds1103_can_sint_init (void);

void ds1103_can_callback_dummy_fcn( void* subint_data, Int32 subint );

void ds1103_can_int_handler( void );

#ifdef _INLINE
 #define __INLINE static
#else
 #define __INLINE
#endif

__INLINE Float64 ds1103_can_time_convert (const UInt32 timecount, UInt32 wrapcount);

__INLINE UInt32 ds1103_can_time2count (const Float32 time);

__INLINE void   ds1103_can_pack_block_8in16 (const UInt32 *unpacked_data,
                                             UInt32 *packed_data,
                                             const UInt32 len);

__INLINE void ds1103_can_unpack_block_16in8 ( UInt32 *packed_data,
                                              UInt32 *unpacked_data,
                                              const UInt32 size );

/******************************************************************************
  version information
******************************************************************************/

#if defined  _DSHOST || defined _CONSOLE
#else
#pragma asm

.equ  _ds1103_can_software_ver_mar, DS1103_CAN_SOFTWARE_VER_MAR
.equ  _ds1103_can_software_ver_mir, DS1103_CAN_SOFTWARE_VER_MIR
.equ  _ds1103_can_software_ver_mai, DS1103_CAN_SOFTWARE_VER_MAI
.equ  _ds1103_can_software_ver_spb, DS1103_CAN_SOFTWARE_VER_SPB
.equ  _ds1103_can_software_ver_spn, DS1103_CAN_SOFTWARE_VER_SPN
.equ  _ds1103_can_software_ver_plv, DS1103_CAN_SOFTWARE_VER_PLV

.equ  _req_ds1103_can_firmware_ver_mar, REQ_DS1103_CAN_FIRMWARE_VER_MAR
.equ  _req_ds1103_can_firmware_ver_mir, REQ_DS1103_CAN_FIRMWARE_VER_MIR
.equ  _req_ds1103_can_firmware_ver_mai, REQ_DS1103_CAN_FIRMWARE_VER_MAI
.equ  _req_ds1103_can_firmware_ver_spb, REQ_DS1103_CAN_FIRMWARE_VER_SPB
.equ  _req_ds1103_can_firmware_ver_spn, REQ_DS1103_CAN_FIRMWARE_VER_SPN
.equ  _req_ds1103_can_firmware_ver_plv, REQ_DS1103_CAN_FIRMWARE_VER_PLV

#pragma endasm
#endif /* #if defined  _DSHOST || defined _CONSOLE */

/******************************************************************************
*
* FUNCTION:
*   CAN controller initialization with baudrate.
*
* SYNTAX: 
*   void ds1103_can_channel_init (UInt32 baudrate, UInt32 bufferwarn,
*                                 UInt32 mb15_format,
*                                 Int32 busoff_subinterrupt)
*
* DESCRIPTION: 
*   Initializes the CAN controller with the desired baudrate
*   and busoff-interrupt number.
*
* PARAMETERS: 
*   UInt32 baudrate:            CAN-bus baudrate (between 10 and 1000 kBaud)
*
*   UInt32 bufferwarn:          enables/disables the bufferwarn subinterrupt
*                               disabled: DS1103_CAN_INT_DISABLE
*                               enabled:  DS1103_CAN_INT_ENABLE
*
*   UInt32 mb15_format:         specifies the format for mailbox 15
*
*   Int32 busoff_subinterrupt:  enables/disables the busoff-interrupt
*                               from CAN controller to master PPC.
*                               disabled: DS1103_CAN_NO_SUBINT
*                               enabled:  0 <= busoff_subinterrupt <=
*                                         DS1103_CAN_SUBINT_BUFFERWARN
*
* RETURNS: 
*   void
*
* NOTE:
*   If the busoff-interrupt is enabled, it is necessary to install an
*   busoff-interrupt handler!
*
******************************************************************************/
#ifndef _INLINE

void ds1103_can_channel_init (const UInt32 baudrate,
                              const UInt32 bufferwarn,
                              const UInt32 mb15_format,
                              const  Int32 busoff_subinterrupt)
{
  UInt32 clock, btr0, btr1;
  Int32 error;

  if(ds1103_can_init == 1)
  {
    return;
  }

  if (baudrate < DS1103_CAN_MIN_BAUDRATE)
  {
    error =  DS1103_CAN_BAUDRATE_L_ERROR; goto Error;
  }

  if (baudrate > DS1103_CAN_MAX_BAUDRATE)
  {
    error =  DS1103_CAN_BAUDRATE_H_ERROR; goto Error;
  }

  DS1103_CAN_TRY (ds1103_can_baudrate_calculate_inv(baudrate, &btr0, &btr1, &clock));

  ds1103_can_channel_init_advanced (clock, btr0, btr1, bufferwarn,
                                    mb15_format, busoff_subinterrupt);
  return;

Error:
  ds1103_can_error_handler (error, "ds1103_can_channel_init");
  return;
 }

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION:
*   CAN controller initialization with bit-timing.
*
* SYNTAX:
*   void ds1103_can_channel_init_advanced (UInt32 frequency,
*                                          UInt32 bit_timing0,
*                                          UInt32 bit_timing1,
*                                          UInt32 bufferwarn,
*                                          UInt32 mb15_format,
*                                          Int32 busoff_subinterrupt)
*
* DESCRIPTION:
*   Initializes the CAN controller with the desired frequency, bittiming-
*   parameters and busoff-interrupt number.
*
* PARAMETERS:
*   UInt32 frequency:             frequency of the CAN controller,
*   UInt32 btr0:                  value for the bittiming0-register,
*   UInt32 btrt:                  value for the bittiming1-register,
*   UInt32 bufferwarn:
*    Int32 busoff_subinterrupt:   enables/disables the busoff-interrupt
*                                 from CAN controller to master PPC.
*          disabled: busoff_subinterrupt = DS1103_CAN_NO_SUBINT
*          enabled:  0 <= busoff_subinterrupt <= DS1103_CAN_SUBINT_BUFFERWARN
*
* RETURNS:
*    void
*
* NOTE: 
*    If the busoff-interrupt is enabled, it is necessary to install an
*    busoff-interrupt handler!
*
******************************************************************************/
#ifndef _INLINE

void ds1103_can_channel_init_advanced ( const UInt32 frequency,
                                        const UInt32 btr0,
                                        const UInt32 btr1,
                                        const UInt32 bufferwarn,
                                        const UInt32 mb15_format,
                                        const  Int32 busoff_subinterrupt )
{
  Float32 freq;
  Int32 error;
  UInt8 vco_div = 0;
  UInt8 ref_div = 0;
  unsigned long dummy;

  rtlib_int_status_t int_status;
  
  if(ds1103_can_init == 1)
  {
    return;
  }

  ds1103_can_channel_started = DS1103_CAN_CH_NOT_STARTED;

  if( frequency < DS1103_CAN_MINCLOCK )
  {
    error = DS1103_CAN_MIN_OSCCLOCK_ERROR; goto Error;
  }

  if ( frequency > DS1103_CAN_MAXCLOCK )
  {
    error = DS1103_CAN_MAX_OSCCLOCK_ERROR; goto Error;
  }

  if( frequency <= DS1103_CAN_LOWCLOCK )
  {
    msg_warning_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_OSCCLOCK_WARN, 
                       DS1103_CAN_OSCCLOCK_WARN_TXT);
  }
 
  freq = (Float32)frequency;

  DS1103_CAN_TRY (ds1103_av9110_clock_calc_can (freq,&freq, &vco_div, &ref_div));
  
  /* adjust the clock-generator */
  ds1103_av9110_clock_write_can (vco_div, ref_div);
  //ds1103_can_clock_write(vco_div, ref_div);
  if( ( busoff_subinterrupt < DEF1103_CAN_SUBINT_BUFFERWARN ) &&
      ( busoff_subinterrupt >= DS1103_CAN_NO_SUBINT ) )
  {
    ds1103_can_subints [busoff_subinterrupt] = DS1103_CAN_BUSOFF_INT;
  }
  else
  {
    error = DS1103_CAN_SUBINT_OUT_OF_RANGE; goto Error;
  }
  
  ds1103_can_glob_id    = 0x0; 
  ds1103_can_glob_no    = 0x0; 
  ds1103_can_glob_mask  = 0xFFFFFFFF;
  
  DS1103_CAN_TRY(ds1103_can_check_format(mb15_format));

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  ds1103_canCh = (ds1103_canChannel*) malloc (sizeof(ds1103_canChannel));

  RTLIB_INT_RESTORE(int_status);

  if(ds1103_canCh==NULL)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }
  
  ds1103_canCh->frequency           = freq;
  ds1103_canCh->btr0                = btr0;     
  ds1103_canCh->btr1                = btr1;     
  ds1103_canCh->busoff_subinterrupt = busoff_subinterrupt;
  ds1103_canCh->mb15_format         = mb15_format;
  ds1103_canCh->index               = DS1103_CAN_AUTO_INDEX;

  ds1103_can_timerticks = DS1103_CAN_TIMER_PRESCALER * 1 / (Float64)freq;

  ds1103_can_communication_init(bufferwarn);

  /* send busoff-enable command to CAN controller */
  if( busoff_subinterrupt != DS1103_CAN_NO_SUBINT )
  {
    DS1103_CAN_TRY( dsmcom_send ( ds1103_can_task_list, TASK0,
                                  DEF1103_CAN_BUSOFF_ENABLE,
                                  DEF1103_CAN_DATA0_DIRECT_PCNT,
                                  (unsigned long*) &dummy));
  }

  ds1103_can_init = 1;
  return;

Error:
  ds1103_can_error_handler (error, "ds1103_can_channel_init_advanced (1)");
  return;
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Basic initialization of PPC - CAN controller communication.
*
* SYNTAX
*   void ds1103_can_communication_init (const UInt32 bufferwarn)
*
* DESCRIPTION
*   The command table and communication buffers are initialized for the
*   basic communication channel.
*
* PARAMETERS:
*   bufferwarn status
*
* RETURNS:
*   void
*
* NOTE
*
******************************************************************************/
#ifndef _INLINE

void ds1103_can_communication_init (const UInt32 bufferwarn)
{
  unsigned long   dummy;
  unsigned long   parms[DEF1103_CAN_INIT_INFO_PCNT];
   Int32          x;
   Int32          error;
  UInt32          version[5];
  Float64         master_time;
  Float64         time;
  ts_timestamp_type start_time, end_time;
  volatile UInt16 *rst          = (volatile UInt16*) 0x10000040;
  volatile UInt16 *version_data = (volatile UInt16*) DEF1103_CAN_FIRMWARE_ADDR;
  UInt16 prgcountl, prgcounth;
  rtlib_int_status_t int_status;  

  *rst |= DS1103_CAN_RSTSLV;
  
  DS1103_CAN_TRY (dsmcom_init (&ds1103_can_task_list,
                               DS1103_CAN_DPMEM_START,  DPM_TARGET_DIRECT, 
                               DEF1103_CAN_BUFFER_NUM,  DEF1103_CAN_BUFFER_SIZE,
                               ADDR_FACTOR,  DEF1103_CAN_ADDR_INC,
                               (dpm_write_fcn_t) DPM_ACCESS_DIRECT,
                               (dpm_read_fcn_t)  DPM_ACCESS_DIRECT));
  
  version_data[0] = 0xABCD;
  
  *rst |= DS1103_CAN_BOOTCAN_FLASH;  /* set BOOTCAN-Bit */
  *rst &= ~DS1103_CAN_RSTSLV;        /* start MC */

  ds1103_can_msg_no = 0;
  ds1103_can_rm_msg_no = 0;
  ds1103_can_firmware_version = 0;

  ts_timestamp_read(&start_time);
  do
  {
    version[0] = version_data[0];

    ts_timestamp_read(&end_time);
    time = ts_timestamp_interval(&start_time, &end_time);

    if (time > DSCOMDEF_TIMEOUT)
    {
      error = DS1103_CAN_TIMEOUT_OR_WRONG_FIRMWARE; goto Error;
    }
  }
  while (version[0] == 0xABCD);

  prgcountl = *((volatile UInt16*) DEF1103_CAN_FW_BURNC_LO_ADDR);
  prgcounth = *((volatile UInt16*) DEF1103_CAN_FW_BURNC_HI_ADDR);
  version[1] = *((volatile UInt16*) DEF1103_CAN_USER_MAJ_FW_ADDR);
  version[2] = *((volatile UInt16*) DEF1103_CAN_USER_MIN_FW_ADDR);
  version[3] = *((volatile UInt16*) DEF1103_CAN_USER_SUB_ADDR);
  RTLIB_FORCE_IN_ORDER(); 
  *((volatile UInt16*) DEF1103_CAN_FW_BURNC_LO_ADDR) = 0;
  *((volatile UInt16*) DEF1103_CAN_FW_BURNC_HI_ADDR) = 0;
  *((volatile UInt16*) DEF1103_CAN_USER_MAJ_FW_ADDR) = 0;
  *((volatile UInt16*) DEF1103_CAN_USER_MIN_FW_ADDR) = 0;
  *((volatile UInt16*) DEF1103_CAN_USER_SUB_ADDR) = 0;
  
  RTLIB_FORCE_IN_ORDER(); 
  version_data[0] = 0;

  ds1103_can_firmware_version = version[0];

  ds1103_can_print_version(version);

  /* VCM support ***********************************************************/
  {   
     UInt32 special_build = 0;
     vcm_version_type temp_vs;
     vcm_module_descriptor_type* fw_vcm_module_ptr = (void*)0; 
     vcm_module_descriptor_type* user_fw_vcm_module_ptr = (void*)0; 
     vcm_module_descriptor_type* can1103_vcm_module_ptr = (void*)0;
     vcm_module_descriptor_type* additional_mem_ptr;
     common_fw_vcm_entry_t* common_fw_vcm_entry_ptr;

     if ( ((version[0] & DS1103_CAN_FIRMWARE_VERSION) >> 1) == 2 )
       special_build = VCM_VERSION_ALPHA;
     else if ( ((version[0] & DS1103_CAN_FIRMWARE_VERSION) >> 1) == 1 )
       special_build = VCM_VERSION_BETA;
     else if ( ((version[0] & DS1103_CAN_FIRMWARE_VERSION) >> 1) == 0 )
       special_build = VCM_VERSION_RELEASE;

     temp_vs.vs.mar=(version[0] & DS1103_CAN_FIRMWARE_MAJOR) >> 11;
     temp_vs.vs.mir=(version[0] & DS1103_CAN_FIRMWARE_MINOR) >>  7;
     temp_vs.vs.spb=special_build;
     temp_vs.vs.plv=0;
     if (special_build == VCM_VERSION_RELEASE)
     {
       temp_vs.vs.mai = (version[0] & DS1103_CAN_FIRMWARE_MAINT) >> 3;
       temp_vs.vs.spn = 0; 
     }
     else
     {
       temp_vs.vs.mai = 0;
       temp_vs.vs.spn = (version[0] & DS1103_CAN_FIRMWARE_MAINT) >> 3;
     }

     can1103_vcm_module_ptr = vcm_module_register(VCM_MID_CAN1103,
                     vcm_module_find(VCM_MID_RTLIB, NULL),
                     VCM_TXT_CAN1103,
                     DS1103_CAN_SOFTWARE_VER_MAR, DS1103_CAN_SOFTWARE_VER_MIR,
                     DS1103_CAN_SOFTWARE_VER_MAI, DS1103_CAN_SOFTWARE_VER_SPB,
                     DS1103_CAN_SOFTWARE_VER_SPN, DS1103_CAN_SOFTWARE_VER_PLV,
                     0, 0);
    

     if(version[0] & DS1103_CAN_FIRMWARE_ORIGIN)
     {
       fw_vcm_module_ptr = vcm_module_register(VCM_MID_CAN1103_FW,
                      0,
                      VCM_TXT_CAN1103_FW,
                      temp_vs.vs.mar,
                      temp_vs.vs.mir,
                      temp_vs.vs.mai,
                      temp_vs.vs.spb,
                      temp_vs.vs.spn,
                      0, 0, VCM_CTRL_NO_ST);

       additional_mem_ptr = fw_vcm_module_ptr;
     }
     else
     {
       user_fw_vcm_module_ptr = vcm_module_register(VCM_MID_USER_CAN1103_FW,
                      0,
                      VCM_TXT_USER_CAN1103_FW,
                      version[1],
                      version[2],
                      version[3],
                      0, 0, 0, 0, VCM_CTRL_NO_ST);

       fw_vcm_module_ptr = vcm_module_register(VCM_MID_BASED_ON_FW,
                             user_fw_vcm_module_ptr,
                             VCM_TXT_BASED_ON_FW,
                             temp_vs.vs.mar,
                             temp_vs.vs.mir,
                             temp_vs.vs.mai,
                             temp_vs.vs.spb,
                             temp_vs.vs.spn,
                             0, 0, VCM_CTRL_NO_ST);

       additional_mem_ptr = user_fw_vcm_module_ptr;

     }

      temp_vs.vs.mar = REQ_DS1103_CAN_FIRMWARE_VER_MAR;
      temp_vs.vs.mir = REQ_DS1103_CAN_FIRMWARE_VER_MIR;
      temp_vs.vs.mai = REQ_DS1103_CAN_FIRMWARE_VER_MAI;
      temp_vs.vs.spb = REQ_DS1103_CAN_FIRMWARE_VER_SPB;
      temp_vs.vs.plv = REQ_DS1103_CAN_FIRMWARE_VER_PLV;
      temp_vs.vs.spn = REQ_DS1103_CAN_FIRMWARE_VER_SPN; 


      /* the additional memory block of the firmware */
      common_fw_vcm_entry_ptr = (common_fw_vcm_entry_t *) vcm_cfg_malloc(sizeof(common_fw_vcm_entry_t));
      common_fw_vcm_entry_ptr->fw_burn_count = prgcountl | (prgcounth << 16);
      common_fw_vcm_entry_ptr->fw_origin = version[0] & DS1103_CAN_FIRMWARE_ORIGIN;
      common_fw_vcm_entry_ptr->fw_location = 0;
      common_fw_vcm_entry_ptr->req_fw_ver.version.high = temp_vs.version.high;
      common_fw_vcm_entry_ptr->req_fw_ver.version.low = temp_vs.version.low;
      vcm_memory_ptr_set(additional_mem_ptr, common_fw_vcm_entry_ptr, sizeof(common_fw_vcm_entry_t));

      if(user_fw_vcm_module_ptr)
        vcm_module_status_set(user_fw_vcm_module_ptr, VCM_STATUS_INITIALIZED);

      vcm_module_status_set(can1103_vcm_module_ptr, VCM_STATUS_INITIALIZED); 
      vcm_module_status_set(fw_vcm_module_ptr, VCM_STATUS_INITIALIZED);

  }
  /* VCM support ***********************************************************/

  DS1103_CAN_TRY (ds1103_can_check_version(version[0]) );

  DS1103_CAN_TRY (dsmcom_slave_acknowledge_check (ds1103_can_task_list,
                                                DPM_TARGET_DIRECT));

  for(x=0; x < DEF1103_CAN_BUFFER_NUM; x++)
  {
    ds1103_can_max_idx[x] = 0;
    ds1103_can_msg_map[x] = NULL;
  }

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  ds1103_can_rm_id_list = (UInt32*)malloc(sizeof(UInt32) * DS1103_CAN_MAX_NO_OF_RM_MSG);

  RTLIB_INT_RESTORE(int_status);
  
  if( ds1103_can_rm_id_list == NULL )
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }
    
  for(x=0; x < DS1103_CAN_MAX_NO_OF_RM_MSG; x++)
  {
    ds1103_can_rm_id_list[x] = 0;
  }
  
  parms[DEF1103_CAN_INIT_INFO_CLCKL_LOC]      = (UInt16) ds1103_canCh->frequency;
  parms[DEF1103_CAN_INIT_INFO_CLCKH_LOC]      = (UInt16) (ds1103_canCh->frequency >> 16);
  parms[DEF1103_CAN_INIT_INFO_BTR0_LOC]       = (UInt16) ds1103_canCh->btr0;
  parms[DEF1103_CAN_INIT_INFO_BTR1_LOC]       = (UInt16) ds1103_canCh->btr1;
  parms[DEF1103_CAN_INIT_INFO_BOFF_INTNO_LOC] = (UInt16) ds1103_canCh->busoff_subinterrupt;
  parms[DEF1103_CAN_INIT_INFO_MB15FORMAT]     = (UInt16) ds1103_canCh->mb15_format;

  DS1103_CAN_TRY (dsmcom_send(ds1103_can_task_list, 0, DEF1103_CAN_INIT_INFO,
                            DEF1103_CAN_INIT_INFO_PCNT, (unsigned long *) parms));

  DS1103_CAN_TRY (dsmcom_slave_acknowledge_check (ds1103_can_task_list,
                                                  DPM_TARGET_DIRECT));
  
  master_time = ds1103_timebase_fltread();

  ds1103_can_timecount_set(master_time);

  if( NULL == ds1103_can_subint_handler  )
  {
    ds1103_can_subint_handler_install( &ds1103_can_callback_dummy_fcn );
  }

  ds1103_set_interrupt_vector( DS1103_INT_CAN, 
                               (DS1103_Int_Handler_Type) &ds1103_can_int_handler,
                               SAVE_REGS_ON);

  ds1103_enable_hardware_int( DS1103_INT_CAN );
  
  if (bufferwarn == DS1103_CAN_INT_ENABLE)
  {
    DS1103_CAN_TRY(dsmcom_send ( ds1103_can_task_list, 0,
                                 DEF1103_CAN_BUFFERWARN_ENABLE, 
                                 DEF1103_CAN_DATA0_DIRECT_PCNT,
                                 (unsigned long*) &dummy ));
  }


  return;

Error:
  ds1103_can_error_handler (error, "ds1103_can_channel_init_advanced (2)" );
  return;
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*    Start CAN Controller
*
* SYNTAX
*    void ds1103_can_channel_start (const UInt32 status_int)
*
* DESCRIPTION
*    This function finish the initialization of the CAN-Controller.
*    The controller go bus on.
*
* PARAMETERS
*    status_int           parameter defines whether a status interrupt should
*                         be generated or not. Use DS1103_CAN_INT_ENABLE for
*                         enabling and DS1103_CAN_INT_DISABLE to disable the
*                         status interrupt
*
* RETURNS
*    void
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

void ds1103_can_channel_start (const UInt32 status_int)
{
   Int32 error;
  UInt32 parms[DEF1103_CAN_DATA_DIRECT_PCNT];

  if(ds1103_canCh==NULL)
  {
    error = DS1103_CAN_CHANNEL_NOT_INIT_ERROR; goto Error;
  }

  if(DS1103_CAN_CH_WAS_STARTED != ds1103_can_channel_started)
  {
    parms[0] = status_int;

    DS1103_CAN_TRY(dsmcom_send (ds1103_can_task_list,
                                TASK0, DEF1103_CAN_START_CONTR,
                                DEF1103_CAN_DATA_DIRECT_PCNT,
                                (unsigned long*) parms));

    ds1103_can_channel_started = DS1103_CAN_CH_WAS_STARTED;
  }
  else
  {
    ds1103_can_channel_txqueue_clear();
  }

  return;

Error:
  ds1103_can_error_handler (error, "ds1103_can_channel_start");
  return;
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION  Clears the TX-queues of the CAN-channel.  
*
* DESCRIPTION
*      After calling this function the TX-queues (STD/EXT) are cleared. All
*      messages within the TX-queues will be erased!.
*
* PARAMETERS
*   canCh        : pointer to the CAN channel object
*
* RETURNS
*    errorcode
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_channel_txqueue_clear(void)
{
  unsigned long dummy;

  return dsmcom_send ( ds1103_can_task_list, TASK0,
       DEF1103_CAN_TXQUEUE_CLEAR, DEF1103_CAN_TXQUEUE_CLEAR_PCNT,
       (unsigned long*) &dummy );
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Deactivates all messages.
*
* FUNCTION
*   Int32 ds1103_can_channel_all_sleep(void)
*
* DESCRIPTION
*   This function stops sending of TX-, RM- and RQ TX-messages and stops
*   sending of received data (RX- and RQ RX-message) to the master.
*
* PARAMETERS
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_channel_all_sleep (void)
{
  unsigned long dummy;

  return ( dsmcom_send (ds1103_can_task_list, TASK0, DEF1103_CAN_MSG_ALL_SLEEP,
                        DEF1103_CAN_DATA0_DIRECT_PCNT, (unsigned long *) &dummy) );
}

/******************************************************************************
*
* FUNCTION
*   Wakes up all sleeping messages.
*
* SYNTAX
*   Int32 ds1103_can_channel_all_wakeup(void)
*
* DESCRIPTION
*   This function reactivates sleeping messages.
*
* PARAMETERS
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_channel_all_wakeup (void)
{
  unsigned long dummy;
  
  return ( dsmcom_send( ds1103_can_task_list, TASK0, DEF1103_CAN_MSG_ALL_WAKEUP,
                        DEF1103_CAN_DATA0_DIRECT_PCNT, (unsigned long *) &dummy) );
}

/******************************************************************************
*
* FUNCTION
*   Gives order to the CAN controller to go busoff.
* SYNTAX
*   Int32 ds1103_can_channel_BOff_go(void)
*
* DESCRIPTION
*   After calling this function the CAN-Controller goes busoff.
*
* PARAMETERS
*
* RETURNS
*   errorcode
*
* REMARKS
*   ds1103_can_Boff_go will not return DS1103_CAN_BUSOFF_STATE !
*
******************************************************************************/
__INLINE Int32 ds1103_can_channel_BOff_go (void)
{
  unsigned long dummy;

  return (dsmcom_send ( ds1103_can_task_list, TASK0, DEF1103_CAN_BUSOFF_GO,
                        DEF1103_CAN_DATA0_DIRECT_PCNT,(unsigned long*) &dummy) );
}

/******************************************************************************
*
* FUNCTION
*   Gives order to the CAN controller to return from busoff.
*
* SYNTAX
*   Int32 ds1103_can_channel_BOff_return(void)
*
* DESCRIPTION
*   After calling this function the CAN-Controller returns from busoff.
*
* PARAMETERS
*
* RETURNS
*    errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_channel_BOff_return (void)
{
  unsigned long dummy;

  return (dsmcom_send (ds1103_can_task_list, TASK0, DEF1103_CAN_BUSOFF_RETURN,
                       DEF1103_CAN_DATA0_DIRECT_PCNT, (unsigned long*) &dummy) );
}

/******************************************************************************
*
* FUNCTION
*   Sets masks.
*
* SYNTAX
*   Int32 ds1103_can_channel_set ( const UInt32 mask_type,
*                                  const UInt32 mask_value )
*
* DESCRIPTION
*   Sets the masks of mailbox 15.
*
* PARAMETERS
*   mask_type      mask to be set:
*                  DS1103_CAN_CHANNEL_SET_MASK15: mask of mailbox 15
*                  DS1103_CAN_CHANNEL_SET_ARBMASK15: arbitration mask of
*                                                    mailbox 15
*   mask_value     value the mask to be set
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_channel_set ( const UInt32 mask_type,
                                        const UInt32 mask_value )
{
  UInt32 parms[ DEF1103_CAN_MASK_SET_PCNT ];
  Int32  error = DS1103_CAN_NO_ERROR;

  switch(mask_type)
  {

     case DS1103_CAN_CHANNEL_SET_MASK15:
     case DS1103_CAN_CHANNEL_SET_ARBMASK15:
       {
         parms[ DEF1103_CAN_MASK_MASKL_LOC ]    = mask_value;
         parms[ DEF1103_CAN_MASK_MASKH_LOC ]    = mask_value >> 16;

         error = dsmcom_send ( ds1103_can_task_list, TASK0, mask_type, 
                              DEF1103_CAN_MASK_SET_PCNT, (unsigned long*) parms );
       }
       break;

     case DS1103_CAN_CHANNEL_SET_BAUDRATE:
       {
         UInt32 btr0, btr1;
         UInt32 baudrate = mask_value;
         UInt32 real_baudrate;
         UInt32 clock;


         if( baudrate < DS1103_CAN_MIN_BAUDRATE ) 
         {
           return DS1103_CAN_BAUDRATE_L_ERROR;
         }

         if( baudrate > DS1103_CAN_MAX_BAUDRATE )
         {
           return DS1103_CAN_BAUDRATE_H_ERROR;
         }
       
         /* Calculate new bittiming register contents. */
         error = ds1103_can_baudrate_calculate_inv((Int32) baudrate,
                                                   &btr0, &btr1, &clock);

         if(DS1103_CAN_NO_ERROR != error)
         {
            return DS1103_CAN_BAUDRATE_SET_BAUDR_ERROR;
         }

         /* Check if baudrate settings are ok. */
         {
           real_baudrate = ds1103_can_calculate_baudrate(btr0, btr1,
                                                         ds1103_canCh->frequency);

           if( (real_baudrate > (baudrate + (baudrate / 100))) || 
               (real_baudrate < (baudrate - (baudrate / 100))) )
           {
             msg_warning_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_BAUDRATE_WARN, 
                                DS1103_CAN_BAUDRATE_WARN_TXT, real_baudrate);
           }
         }

         // Update channel object
         ds1103_canCh->btr0 = btr0;
         ds1103_canCh->btr1 = btr1;

         parms[DEF1103_CAN_CHANNEL_SET_BAUDRATE_BTR0_LOC] = btr0;     
         parms[DEF1103_CAN_CHANNEL_SET_BAUDRATE_BTR1_LOC] = btr1;    

         error = dsmcom_send ( ds1103_can_task_list, TASK0, DS1103_CAN_CHANNEL_SET_BAUDRATE, 
                               DEF1103_CAN_CHANNEL_SET_BAUDRATE_PCNT, (unsigned long*) parms );
       }
       break;

      default:
          error = DS1103_CAN_CHANNEL_SET_PARAM_ERROR;
        break;
  }

  return error;

}



/******************************************************************************
*
* FUNCTION
*   Installs a CAN-message for sending (TX-message).
*
* SYNTAX
*   void ds1103_can_msg_tx_register (Int16 queue, UInt32 identifier,
*                                    UInt8 format, UInt16 inform,
*                                    Int16 subinterrupt, Float32 start_time,
*                                    Float32 repetition_time,
*                                    Float32 timeout)
*
* DESCRIPTION
*   This function installs a TX-message.
*
* PARAMETERS
*   queue               communication channel
*   identifier          identifier of the tx-message
*   format              format of the message (DS1103_CAN_STD for 11 bit
*                       identifier (CAN 2.0A) or DS1103_CAN_EXT for 29 bit
*                       identifier (CAN 2.0B) )
*   start_time          start of transmitting of the message
*   repetition_time     cyclic message (controlled by timer):
*                         0 < repetition_time <= DS1103_CAN_MAX_REPETITIONTIME
*                       acyclic message (send by trigger):
*                         repetition_time = DS1103_CAN_TRIGGER_MSG
*   timeout             timeout time
*                       DS1103_CAN_MSG_NOTIMEOUT <= timeout <=
*                       DS1103_CAN_MAX_TIMEOUT
*   inform              defines how much information is sended to the
*                       master if a message is successfully sended.
*                use    DS1103_CAN_PROCESSED_INFO  (delivers processed-flag)
*                       DS1103_CAN_TIMECOUNT_INFO  (delivers Timecount)
*                       DS1103_CAN_DELAYCOUNT_INFO (delivers Delaycount)
*   subinterrupt        enables/disables an interrupt from slave to master
*                disabled: subinterrupt = DS1103_CAN_NO_SUBINT
*                enabled:  0 <= subinterrupt < DS1103_SUBINT_BUFFERWARN
*                          (interrupt handler is required)
*
* RETURNS
*   void
*
* REMARKS  the sending of the TX-message is enabled by writing of
*          the data (ds1103_can_msg_write).
******************************************************************************/
#ifndef _INLINE

ds1103_canMsg* ds1103_can_msg_tx_register (const  Int32 queue,
                                           const UInt32 identifier,
                                           const UInt32 format,
                                           const UInt32 inform,
                                           const Int32 subinterrupt,
                                           const Float32 start_time, 
                                           const Float32 repetition_time,
                                           const Float32 timeout)
{
  UInt32 parms[DEF1103_CAN_TX_MSG_REG_PCNT];
   Int32 error;
  Float32 timeout_int = timeout;
  
  ds1103_canMsg* canMsg; 
    
  rtlib_int_status_t int_status;  

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  canMsg = (ds1103_canMsg*) malloc (sizeof(ds1103_canMsg));

  RTLIB_INT_RESTORE(int_status);

  if(canMsg==NULL)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }

  canMsg->msg_no     = ds1103_can_msg_no++;
  canMsg->queue      = queue;
  canMsg->identifier = identifier;
  canMsg->format     = format;
  canMsg->type       = DS1103_CAN_TX;
  canMsg->inform     = inform & ~DS1103_CAN_DATA_INFO;
  canMsg->index      = ds1103_canCh->index;
  canMsg->msgService = NULL;
  canMsg->msgQueue   = NULL;
  canMsg->isTxqueued = DS1103_CAN_IS_NOT_TXQUEUED;
  canMsg->clientMsgs = NULL;
  canMsg->no_of_clients = 0;
  canMsg->clientNumber = 0;
  canMsg->rootMsg = canMsg;
  canMsg->pDsCanMsgCb = NULL;
  
  DS1103_CAN_TRY (ds1103_can_check_subint (subinterrupt));
  DS1103_CAN_TRY (ds1103_can_check_format (format));
  DS1103_CAN_TRY (ds1103_can_check_timing (DS1103_CAN_TX, start_time, 
                                           repetition_time, &timeout_int));
  DS1103_CAN_TRY (ds1103_can_check_ident (canMsg));
  ds1103_can_msg_clear(canMsg);

  parms[ DEF1103_CAN_TX_MSG_REG_IDENTL_LOC ]   = identifier;
  parms[ DEF1103_CAN_TX_MSG_REG_IDENTH_LOC ]   = identifier >> 16;
  parms[ DEF1103_CAN_TX_MSG_REG_FORMAT_LOC ]   = format;
  parms[ DEF1103_CAN_TX_MSG_REG_STARTL_LOC ]   = ds1103_can_time2count(start_time);
  parms[ DEF1103_CAN_TX_MSG_REG_STARTH_LOC ]   = ds1103_can_time2count(start_time) >> 16;
  parms[ DEF1103_CAN_TX_MSG_REG_REPETL_LOC ]   = ds1103_can_time2count(repetition_time);
  parms[ DEF1103_CAN_TX_MSG_REG_REPETH_LOC]    = ds1103_can_time2count(repetition_time) >> 16;
  parms[ DEF1103_CAN_TX_MSG_REG_TIMEOUTL_LOC ] = ds1103_can_time2count(timeout_int);
  parms[ DEF1103_CAN_TX_MSG_REG_TIMEOUTH_LOC ] = ds1103_can_time2count(timeout_int) >> 16;
  parms[ DEF1103_CAN_TX_MSG_REG_INFORM_LOC ]   = inform & ~DS1103_CAN_DATA_INFO;
  parms[ DEF1103_CAN_TX_MSG_REG_INTNO_LOC ]    = subinterrupt;
  parms[ DEF1103_CAN_TX_MSG_REG_MSGNO_LOC ]    = canMsg->msg_no;
  
  DS1103_CAN_TRY (ds1103_can_register (canMsg, &canMsg->index, 
                                       DEF1103_CAN_TX_MSG_REG, 
                                       DEF1103_CAN_TX_MSG_REG_DOUTCNT, 
                                       DEF1103_CAN_TX_MSG_REG_PCNT, parms));                 

  return canMsg;

Error:
  free(canMsg);
  ds1103_can_error_handler (error, "ds1103_can_msg_tx_register");
  return NULL;
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Installs a Remote-message.
*
* SYNTAX
*   ds1103_canMsg* ds1103_can_msg_rm_register ( const Int32 queue,
*                                               const UInt32 identifier,
*                                               const UInt32 format,
*                                               const UInt32 inform,
*                                               const  Int32 subinterrupt )
*
* DESCRIPTION
*   This function installs a RM-message (requestable by a other CAN-node).
*
* PARAMETERS
*   queue               ID of communication channel
*   identifier          identifier of the TX-message
*   format              format of the message (DS1103_CAN_STD for 11 bit
*                       identifier (CAN 2.0A) or DS1103_CAN_XTD for 29 bit
*                       identifier (CAN 2.0B) )
*   inform              defines how much information is sended to the
*                       master if a message is successfully sended.
*                use    DS1103_CAN_TIMECOUNT_INFO  (delivers Timecount)
*                       DS1103_CAN_DELAYCOUNT_INFO (delivers Delaycount)
*   subinterrupt        enables/disables an interrupt from slave to master
*
*                disabled: subinterrupt = DS1103_CAN_NO_SUBINT
*                enabled:  0 <= subinterrupt < DS1103_CAN_SUBINT_BUFFERWARN
*                          (interrupt handler is required)
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

ds1103_canMsg* ds1103_can_msg_rm_register ( const Int32 queue,
                                            const UInt32 identifier,
                                            const UInt32 format,
                                            const UInt32 inform,
                                            const  Int32 subinterrupt )
{

  UInt32 parms[ DEF1103_CAN_RM_MSG_REG_PCNT ];
   Int32 error;
  
  ds1103_canMsg* canMsg; 
   
  rtlib_int_status_t int_status;  

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  canMsg = (ds1103_canMsg*) malloc (sizeof(ds1103_canMsg));

  RTLIB_INT_RESTORE(int_status);
    
  if(canMsg==NULL)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }

  ds1103_can_rm_msg_no++;
  
  ds1103_can_rm_id_list[ds1103_can_rm_msg_no] =  identifier | (format << 31); 

  if ( ds1103_can_rm_msg_no > DS1103_CAN_MAX_NO_OF_RM_MSG )
  {
    error = DS1103_CAN_NO_RM_MAILBOX_FREE; goto Error;
  }

  canMsg->msg_no     = ds1103_can_msg_no++;
  canMsg->msgService = NULL;
  canMsg->queue      = queue;
  canMsg->identifier = identifier;
  canMsg->format     = format;
  canMsg->type       = DS1103_CAN_RM;
  canMsg->inform     = inform & ~DS1103_CAN_DATA_INFO;
  canMsg->index      = ds1103_canCh->index;
  canMsg->msgQueue   = NULL;
  canMsg->isTxqueued = DS1103_CAN_IS_NOT_TXQUEUED;
  canMsg->clientMsgs = NULL;
  canMsg->no_of_clients = 0;
  canMsg->clientNumber = 0;
  canMsg->rootMsg = canMsg;
  canMsg->pDsCanMsgCb = NULL;

  DS1103_CAN_TRY (ds1103_can_check_subint (subinterrupt));
  DS1103_CAN_TRY (ds1103_can_check_format (format));
  DS1103_CAN_TRY (ds1103_can_check_ident (canMsg));
  ds1103_can_msg_clear (canMsg);

  parms[ DEF1103_CAN_RM_MSG_REG_IDENTL_LOC ]   = identifier;
  parms[ DEF1103_CAN_RM_MSG_REG_IDENTH_LOC ]   = identifier >> 16;
  parms[ DEF1103_CAN_RM_MSG_REG_FORMAT_LOC ]   = format;
  parms[ DEF1103_CAN_RM_MSG_REG_INFORM_LOC ]   = inform & ~DS1103_CAN_DATA_INFO;
  parms[ DEF1103_CAN_RM_MSG_REG_INTNO_LOC ]    = subinterrupt;
  parms[ DEF1103_CAN_RM_MSG_REG_MSGNO_LOC ]    = canMsg->msg_no;
  
  DS1103_CAN_TRY (ds1103_can_register (canMsg, &canMsg->index, 
                                       DEF1103_CAN_RM_MSG_REG, 
                                       DEF1103_CAN_RM_MSG_REG_DOUTCNT,
                                       DEF1103_CAN_RM_MSG_REG_PCNT, parms));

  return canMsg;

Error:
  free(canMsg);
  ds1103_can_error_handler (error, "ds1103_can_msg_rm_register");
  return NULL;
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Installs a CAN-message for revceiving (RX-message).
*
* SYNTAX
*   ds1103_canMsg* ds1103_can_msg_rx_register (const  Int32 queue,
*                                              const UInt32 identifier,
*                                              const UInt32 format,
*                                              const UInt32 inform,
*                                              const  Int32 subinterrupt )
*
* DESCRIPTION
*   Installs a CAN-message for revceiving (RX-message).
*
* PARAMETERS
*   queue               ID of communication channel
*   identifier          identifier of the RX-message
*   format              format of the message (DS1103_CAN_STD for 11 bit
*                       identifier (CAN 2.0A) or DS1103_CAN_EXT for 29 bit
*                       identifier (CAN 2.0B) )
*   inform              defines how much information is sended to the
*                       master if a message is successfully revceived.
*                use    DS1103_CAN_DATA_INFO       (delivers data and number of bytes)
*                       DS1103_CAN_TIMECOUNT_INFO  (delivers Timecount)
*   subinterrupt        enables/disables an interrupt from slave to master
*
*                disabled: subinterrupt = DS1103_CAN_NO_SUBINT
*                enabled:  0 <= subinterrupt < DS1103_CAN_SUBINT_BUFFERWARN
*                          (interrupt handler is required)
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

ds1103_canMsg* ds1103_can_msg_rx_register (const  Int32 queue,
                                           const UInt32 identifier,
                                           const UInt32 format,
                                           const UInt32 inform,
                                           const  Int32 subinterrupt )
{
  UInt32 parms[DEF1103_CAN_RX_MSG_REG_PCNT];
   Int32 error;

  ds1103_canMsg* canMsg;
  
  rtlib_int_status_t int_status;  

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  canMsg = (ds1103_canMsg*) malloc (sizeof(ds1103_canMsg));

  RTLIB_INT_RESTORE(int_status);

  if(canMsg==NULL)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }
  
  canMsg->msg_no     = ds1103_can_msg_no++;
  canMsg->msgService = NULL;
  canMsg->queue      = queue;
  canMsg->identifier = identifier;
  canMsg->format     = format;
  canMsg->type       = DS1103_CAN_RX;
  canMsg->inform     = inform & ~DS1103_CAN_DELAYCOUNT_INFO;
  canMsg->index      = ds1103_canCh->index;
  canMsg->msgQueue   = NULL;
  canMsg->isTxqueued = DS1103_CAN_IS_NOT_TXQUEUED;
  canMsg->clientMsgs = NULL;
  canMsg->no_of_clients = 0;
  canMsg->clientNumber = 0;
  canMsg->rootMsg = canMsg;
  canMsg->pDsCanMsgCb = NULL;

        
  DS1103_CAN_TRY (ds1103_can_check_subint (subinterrupt));
  DS1103_CAN_TRY (ds1103_can_check_format (format));
  DS1103_CAN_TRY (ds1103_can_check_ident (canMsg));
  ds1103_can_msg_clear (canMsg);

  parms[ DEF1103_CAN_RX_MSG_REG_IDENTL_LOC ]  = identifier;
  parms[ DEF1103_CAN_RX_MSG_REG_IDENTH_LOC ]  = identifier >> 16;
  parms[ DEF1103_CAN_RX_MSG_REG_FORMAT_LOC ]  = format;
  parms[ DEF1103_CAN_RX_MSG_REG_INFORM_LOC ]  = inform & ~DS1103_CAN_DELAYCOUNT_INFO;
  parms[ DEF1103_CAN_RX_MSG_REG_INTNO_LOC ]   = subinterrupt;
  parms[ DEF1103_CAN_RX_MSG_REG_MSGNO_LOC ]   = canMsg->msg_no;

  DS1103_CAN_TRY (ds1103_can_register ( canMsg, &canMsg->index, 
                                        DEF1103_CAN_RX_MSG_REG,
                                        DEF1103_CAN_RX_MSG_REG_DOUTCNT,
                                        DEF1103_CAN_RX_MSG_REG_PCNT, parms ));

  return canMsg;

Error:
  free(canMsg);
  ds1103_can_error_handler (error, "ds1103_can_msg_rx_register");
  return NULL;
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Installs a CAN-message for requesting.
*
* SYNTAX
*   ds1103_canMsg* ds1103_can_msg_rqtx_register ( const  Int32  queue,
*                                                 const UInt32  identifier,
*                                                 const UInt32  format,
*                                                 const UInt32  inform,
*                                                 const  Int32  subinterrupt,
*                                                 const Float32 start_time,
*                                                 const Float32 repetition_time,
*                                                 const Float32 timeout)
*
* DESCRIPTION
*   The function installs a rqtx-message.
*
* PARAMETERS
*   queue               ID of communication channel
*   identifier          identifier of the TX-message
*   format              format of the message (DS1103_CAN_STD for 11 bit
*                       identifier (CAN 2.0A) or DS1103_CAN_EXT for 29 bit
*                       identifier (CAN 2.0B) )
*   start_time          start of transmitting of the message
*   repetition_time     cyclic message (controlled by timer):
*                         0 < repetition_time <= DS1103_CAN_MAX_REPETITIONTIME
*                       acyclic message (send by trigger):
*                         repetition_time = DS1103_CAN_TRIGGER_MSG
*   timeout             timeout time
*                       DS1103_CAN_MSG_NOTIMEOUT <= timeout <=
*                       DS1103_CAN_MAX_TIMEOUT
*   inform              defines how much information is sended to the
*                       master if a message is successfully sended.
*                use    DS1103_CAN_TIMECOUNT_INFO  (delivers Timecount)
*                       DS1103_CAN_DELAYCOUNT_INFO (delivers Delaycount)
*   subinterrupt        enables/disables an interrupt from slave to master
*                       
*                disabled: subinterrupt = DS1103_CAN_NO_SUBINT
*                enabled:  0 <= subinterrupt < DS1103_CAN_SUBINT_BUFFERWARN
*                          (interrupt handler is required)
* RETURNS
*   void 
*
* REMARKS        the sending of the rq tx-message is enabled by calling
*                ds1103_can_rqtx_msg_activate.
*
*
******************************************************************************/
#ifndef _INLINE

ds1103_canMsg* ds1103_can_msg_rqtx_register ( const  Int32  queue,
                                              const UInt32  identifier,
                                              const UInt32  format,
                                              const UInt32  inform,
                                              const  Int32  subinterrupt,
                                              const Float32 start_time,
                                              const Float32 repetition_time,
                                              const Float32 timeout)
{
   UInt32 parms[DEF1103_CAN_RQTX_MSG_REG_PCNT];
  Float32 timeout_int = timeout;
    Int32 error;

  ds1103_canMsg* canMsg; 

  rtlib_int_status_t int_status;  

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  canMsg = (ds1103_canMsg*) malloc (sizeof(ds1103_canMsg));

  RTLIB_INT_RESTORE(int_status);

  if(canMsg==NULL)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }
 
  
  canMsg->msg_no     = ds1103_can_msg_no++;
  canMsg->msgService = NULL;
  canMsg->type       = DS1103_CAN_RQTX;
  canMsg->identifier = identifier;
  canMsg->format     = format;
  canMsg->queue      = queue;
  canMsg->inform     = inform & ~DS1103_CAN_DATA_INFO;
  canMsg->index      = ds1103_canCh->index;
  canMsg->msgQueue   = NULL;
  canMsg->isTxqueued = DS1103_CAN_IS_NOT_TXQUEUED;
  canMsg->clientMsgs = NULL;
  canMsg->no_of_clients = 0;
  canMsg->clientNumber = 0;
  canMsg->rootMsg = canMsg;
  canMsg->pDsCanMsgCb = NULL;

  DS1103_CAN_TRY(ds1103_can_check_timing (DS1103_CAN_RQTX, start_time, 
                                          repetition_time, &timeout_int));
  DS1103_CAN_TRY (ds1103_can_check_subint (subinterrupt));
  DS1103_CAN_TRY (ds1103_can_check_format (format));
  DS1103_CAN_TRY (ds1103_can_check_ident (canMsg));
  ds1103_can_msg_clear (canMsg);
  
  parms[ DEF1103_CAN_RQTX_MSG_REG_IDENTL_LOC ]   = identifier;
  parms[ DEF1103_CAN_RQTX_MSG_REG_IDENTH_LOC ]   = identifier >> 16;
  parms[ DEF1103_CAN_RQTX_MSG_REG_FORMAT_LOC ]   = format;
  parms[ DEF1103_CAN_RQTX_MSG_REG_STARTL_LOC ]   = ds1103_can_time2count(start_time);
  parms[ DEF1103_CAN_RQTX_MSG_REG_STARTH_LOC ]   = ds1103_can_time2count(start_time) >> 16;
  parms[ DEF1103_CAN_RQTX_MSG_REG_REPETL_LOC ]   = ds1103_can_time2count(repetition_time);
  parms[ DEF1103_CAN_RQTX_MSG_REG_REPETH_LOC]    = ds1103_can_time2count(repetition_time) >> 16;
  parms[ DEF1103_CAN_RQTX_MSG_REG_TIMEOUTL_LOC ] = ds1103_can_time2count(timeout_int);
  parms[ DEF1103_CAN_RQTX_MSG_REG_TIMEOUTH_LOC ] = ds1103_can_time2count(timeout_int) >> 16;
  parms[ DEF1103_CAN_RQTX_MSG_REG_TXINFORM_LOC ] = inform & ~DS1103_CAN_DATA_INFO;
  parms[ DEF1103_CAN_RQTX_MSG_REG_TXINTNO_LOC ]  = subinterrupt;
  parms[ DEF1103_CAN_RQTX_MSG_REG_MSGNO_LOC ]    = canMsg->msg_no;
  
  DS1103_CAN_TRY (ds1103_can_register ( canMsg, &canMsg->index, 
                                        DEF1103_CAN_RQTX_MSG_REG,
                                        DEF1103_CAN_RQTX_MSG_REG_DOUTCNT,
                                        DEF1103_CAN_RQTX_MSG_REG_PCNT, parms ));

  return canMsg;

Error:
  free(canMsg);
  ds1103_can_error_handler (error, "ds1103_can_msg_rqtx_register");
  return NULL;
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Installs a CAN-message for the receiving of requested data.
*
* SYNTAX
*   ds1103_canMsg* ds1103_can_msg_rqrx_register (const ds1103_canMsg*  rqtxMsg,
*                                                const UInt32 inform,
*                                                const Int32 subinterrupt )
*
* DESCRIPTION
*   This function installs a CAN-message for the receiving of requested data
*   (rqrx-message).
*
* PARAMETERS
*   rqtxMsg             handle to the rqtx message structure
*   inform              defines how much information is sended to the
*                       master if a message is successfully sended.
*                use    DS1103_CAN_DATA_INFO (delivers data and number of bytes)
*                       DS1103_CAN_TIMECOUNT_INFO  (delivers Timecount)
*   subinterrupt        enables/disables an interrupt from slave to master
*                disabled: subinterrupt = DS1103_CAN_NO_SUBINT
*                enabled:  0 <= subinterrupt < DS1103_CAN_SUBINT_BUFFERWARN
*                          (interrupt handler is required)
*
* RETURNS
*   rqrxMsg             handle to the message structure
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

ds1103_canMsg* ds1103_can_msg_rqrx_register (const ds1103_canMsg*  rqtxMsg,
                                             const UInt32 inform,
                                             const Int32 subinterrupt )
{
  UInt32 parms[ DEF1103_CAN_RQRX_MSG_REG_PCNT ];
   Int32 error;
  
  ds1103_canMsg* canMsg;     

  rtlib_int_status_t int_status;  

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  canMsg = (ds1103_canMsg*) malloc (sizeof(ds1103_canMsg));

  RTLIB_INT_RESTORE(int_status);
  
  if(canMsg==NULL)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }
  if(rqtxMsg == NULL)
  {
   error = DS1103_CAN_RQTX_NOT_REG; goto Error; 
  }
  
  canMsg->type       = DS1103_CAN_RQRX;
  canMsg->msgService = NULL;
  canMsg->msg_no     = rqtxMsg->msg_no;
  canMsg->queue      = rqtxMsg->queue;
  canMsg->identifier = rqtxMsg->identifier;
  canMsg->format     = rqtxMsg->format;
  canMsg->inform     = inform  & ~DS1103_CAN_DELAYCOUNT_INFO;
  canMsg->index      = ds1103_canCh->index; 
  canMsg->msgQueue   = NULL;
  canMsg->isTxqueued = DS1103_CAN_IS_NOT_TXQUEUED;
  canMsg->clientMsgs = NULL;
  canMsg->no_of_clients = 0;
  canMsg->clientNumber = 0;
  canMsg->rootMsg = canMsg;
  canMsg->pDsCanMsgCb = NULL;

  DS1103_CAN_TRY (ds1103_can_check_subint (subinterrupt));
  DS1103_CAN_TRY (ds1103_can_check_ident (canMsg));
  ds1103_can_msg_clear (canMsg);
 
  parms[ DEF1103_CAN_RQRX_MSG_REG_RXINFORM_LOC ] = inform  & ~DS1103_CAN_DELAYCOUNT_INFO;
  parms[ DEF1103_CAN_RQRX_MSG_REG_RXINTNO_LOC ]  = subinterrupt;
  parms[ DEF1103_CAN_RQRX_MSG_REG_MSGNO_LOC ]    = canMsg->msg_no;
  
  DS1103_CAN_TRY (ds1103_can_register ( canMsg,(Int32*)&canMsg->index,
                                        DEF1103_CAN_RQRX_MSG_REG,
                                        DEF1103_CAN_RQRX_MSG_REG_DOUTCNT,
                                        DEF1103_CAN_RQRX_MSG_REG_PCNT, parms ));
  
  return canMsg;

Error:
  free(canMsg);
  ds1103_can_error_handler (error, "ds1103_can_msg_rqrx_register");
  return NULL;
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Activates sending of a RQTX message.
*
* SYNTAX
*   Int32 ds1103_can_msg_rqtx_activate (const ds1103_canMsg* canMsg)
*
* DESCRIPTION
*   Activates a RQTX message: enables sending of a timer-triggered (cyclic)
*   or an user-triggered (acyclic) RQTX-message.
*
* PARAMETERS
*   canMsg            handle to RQTX message structure
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_msg_rqtx_activate ( const ds1103_canMsg* canMsg )
{
  return ds1103_can_msg_wakeup (canMsg) ;
}

/******************************************************************************
*
* FUNCTION
*   Writes the data of a tx or rm message.
*
* SYNTAX
*   Int32 ds1103_can_msg_write ( const ds1103_canMsg* canMsg,
*                                const UInt32 data_len,
*                                const UInt32* data)
*
* DESCRIPTION
*   This function writes the data of an tx- or rm-message which is registered
*   before. The first use of this function enables the sending of the
*   message.
*
* PARAMETERS
*   canMsg              handle to message structure
*   data_len            number of bytes of the data
*   data                data-string
*
* RETURNS
*   errorcode           DS1103_CAN_NO_ERROR  if the execution was error free
*                       DS1103_BUFFER_OVERFLOW if the buffer is full
*                       (slave isn't able to read the buffer)
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_msg_write ( const ds1103_canMsg* canMsg,
                                      const UInt32  data_len,
                                      const UInt32* data )
{
  UInt32 parms[DEF1103_CAN_MSG_WRITE_PCNT];
  UInt32 param_count;
  UInt32 len=0;
  UInt32 opcode;

  switch(canMsg->type)
  {
   case DS1103_CAN_TX:
    opcode = DEF1103_CAN_TX_MSG_WRITE;
    break;
   case DS1103_CAN_RM:
    opcode = DEF1103_CAN_RM_MSG_WRITE;
    break;
   default:
    return DS1103_CAN_TYPE_ERROR;
  }    

  /* mapping data_len -> word-len*/
  len = ds1103_can_len_arr[ data_len ];
     
  param_count = DEF1103_CAN_MSG_WRITE_PCNT2 + len;
  
  parms[ DEF1103_CAN_MSG_WRITE_IDX_LOC ]     = canMsg->msg_no;
  parms[ DEF1103_CAN_MSG_WRITE_DATALEN_LOC ] = data_len;
    
  ds1103_can_pack_block_8in16 (data, &parms[ DEF1103_CAN_MSG_WRITE_DATA_LOC ], len );  

  return dsmcom_var_send (ds1103_can_task_list, canMsg->queue, opcode, param_count, 
                          (unsigned long*) parms);
}

/******************************************************************************
*
* FUNCTION
*   Writes the data of a TX-message and perform a trigger.
*
* SYNTAX
*   Int32 ds1103_can_msg_send ( const ds1103_canMsg* canMsg,
*                               const UInt32 data_len,
*                               const UInt32 data)
*
* DESCRIPTION
*   This function writes the data of an TX-message which is registered
*   before. The message must registered as a trigger message. After writing
*   data a trigger is performed. The first use of this function enables the
*   sending of the message.
*
* PARAMETERS
*   canMsg              handle to the message structure
*   data_len            number of bytes of the data
*   data                data-string
*
* RETURNS
*   errorcode           DS1103_CAN_NO_ERROR  if the execution was error free
*                       DS1103_BUFFER_OVERFLOW if the buffer is full
*                       (slave isn't able to read the buffer)
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_msg_send ( const ds1103_canMsg* canMsg,
                                     const UInt32  data_len,
                                     const UInt32* data,
                                     const Float32 delay)
{
  UInt32 parms[DEF1103_CAN_MSG_SEND_PCNT];
  UInt32 param_count;
  UInt32 len=0;
  UInt32 opcode;

  switch(canMsg->type)
  {
    case DS1103_CAN_TX:
      opcode = DEF1103_CAN_TX_MSG_SEND;
      break;
    default:
      return DS1103_CAN_TYPE_ERROR;
  } 

  /* mapping data_len -> word-len*/
  len = ds1103_can_len_arr[ data_len ];

  param_count = DEF1103_CAN_MSG_SEND_PCNT2 + len;
  
  parms[ DEF1103_CAN_MSG_SEND_IDX_LOC ]     = canMsg->msg_no;
  parms[ DEF1103_CAN_MSG_SEND_DATALEN_LOC ] = data_len;
  parms[ DEF1103_CAN_MSG_SEND_DELAYL_LOC ]  = ds1103_can_time2count(delay);
  parms[ DEF1103_CAN_MSG_SEND_DELAYH_LOC ]  = ds1103_can_time2count(delay) >> 16;
    
  ds1103_can_pack_block_8in16 (data, &parms[ DEF1103_CAN_MSG_SEND_DATA_LOC ], len);

  return dsmcom_var_send (ds1103_can_task_list, canMsg->queue, opcode, 
                          param_count, (unsigned long*) parms );
  
}





/******************************************************************************
*
* DESCRIPTION
*                 This function sends a CAN message with the  message
*                 identifier as parameter.  
*                 The format of the message to be sent is determined by
*                 the function ds1103_can_msg_tx_register and can not
*                 be altered.
*
* PARAMETERS
*   canMsg        Pointer to the CAN message.
*
*   id            Identifier of the message.
*
*   data_len      Number of bytes of the data.
*
*   data          Pointer to the data buffer.
*
*   delay         Send the message after the delay in seconds.
*
* RETURNS
*                 DEF1103_CAN_NO_ERROR 
*                 If the execution was error free.
*
*                 DEF1103_CAN_BUFFER_OVERFLOW
*                 If the buffer is full. 
*	               (The master isn't able to write the DPM.)
*
******************************************************************************/

__INLINE Int32 ds1103_can_msg_send_id (ds1103_canMsg* canMsg,
                                       const UInt32 id,
                                       const UInt32  data_len,
                                       const UInt32* data,
                                       const Float32 delay)

{
  UInt32 parms[ DEF1103_CAN_TX_MSG_SEND_ID_PCNT];
  UInt32 param_count, delaycnt;
  UInt32 len;
 
  if( DS1103_CAN_TX != canMsg->type )
  {
    return DS1103_CAN_TYPE_ERROR;
  } 

  /* mapping data_len -> word-len*/
  len = ds1103_can_len_arr[ data_len ];
    
  delaycnt = ds1103_can_time2count(delay);
  param_count = DEF1103_CAN_TX_MSG_SEND_ID_PCNT2 + len;

  parms[ DEF1103_CAN_TX_MSG_SEND_ID_IDX_LOC ]     = canMsg->msg_no;
  parms[ DEF1103_CAN_TX_MSG_SEND_ID_IDENTL_LOC ]  = id;
  parms[ DEF1103_CAN_TX_MSG_SEND_ID_IDENTH_LOC ]  = id >> 16;
  parms[ DEF1103_CAN_TX_MSG_SEND_ID_DELAY_L_LOC ] = delaycnt & 0xFFFF;
  parms[ DEF1103_CAN_TX_MSG_SEND_ID_DELAY_H_LOC ] = delaycnt >> 16;
  parms[ DEF1103_CAN_TX_MSG_SEND_ID_DATALEN_LOC ] = data_len;

  canMsg->identifier = id;
  ds1103_can_pack_block_8in16 (data, &parms[ DEF1103_CAN_TX_MSG_SEND_ID_DATA_LOC ], len );  
 
  return dsmcom_var_send (ds1103_can_task_list, canMsg->queue, DEF1103_CAN_MSG_SEND_ID, 
                          param_count, (unsigned long*) parms );


}



/******************************************************************************
*
* DESCRIPTION
*      This function sends a TX-message with the message
*      identifier as parameter. The TX-message is written
*      to the corresponding transmit-queue.
*      The format of the message to be sent is determined by
*      the function ds1103_can_msg_tx_register and can not
*      be altered.
*
* PARAMETERS
*      canMsg   pointer to the ds1103_canMsg structure 
*
*      id       identifier of the message.
*
*      datalen  length of the message data. Valid range is 0 ... 8 bytes. 
*
*      data     buffer for CAN message data 
*
*
* RETURNS
*      errorcode; the following symbols are predefined: 
*  
*      DS1103_CAN_NO_ERROR
*      The function has been performed without error.
* 
*      DS1103_CAN_SEND_ID_QUEUED_INIT_ERROR
*      The transmit-queue for the TX-message is not initialized.
*
*      DS1103_CAN_BUFFER_OVERFLOW
*      Not enough memory space between the master write pointer and
*      the slave read pointer. The operation is aborted.
*      Repeat the function until it returns DS1103_CAN_NO_ERROR.
*
******************************************************************************/

__INLINE Int32 ds1103_can_msg_send_id_queued (  ds1103_canMsg* canMsg,
                                             const UInt32 id,
                                             const UInt32 data_len,
                                             const UInt32* data)
{
  UInt32 parms[DEF1103_CAN_TXQUEUE_SEND_ID_PCNT2];
  UInt32 param_count;
  UInt32 len;
 

  if(canMsg->isTxqueued)
  {
    /* mapping data_len -> word-len */
    len = ds1103_can_len_arr[ data_len ];
     
    param_count = DEF1103_CAN_TXQUEUE_SEND_ID_PCNT + len;
  
    parms[ DEF1103_CAN_TXQUEUE_SEND_ID_MSGNO_LOC ]   = canMsg->msg_no;
    parms[ DEF1103_CAN_TXQUEUE_SEND_ID_ID_LOC_L ]    = id;
    parms[ DEF1103_CAN_TXQUEUE_SEND_ID_ID_LOC_H ]    = id >> 16;
    parms[ DEF1103_CAN_TXQUEUE_SEND_ID_DATALEN_LOC ] = data_len;

    canMsg->identifier = id;
    ds1103_can_pack_block_8in16 (data, (UInt32*)&parms[DEF1103_CAN_TXQUEUE_SEND_ID_DATA_LOC],
                              len );  

    return dsmcom_var_send (ds1103_can_task_list,
                            canMsg->queue, DEF1103_CAN_TXQUEUE_SEND_ID,
                            param_count, (unsigned long *) parms);
  }
  else
  {
    ds1103_can_error_handler ( DS1103_CAN_SEND_ID_QUEUED_INIT_ERROR, "ds1103_can_msg_send_id_queued" );
    return DS1103_CAN_SEND_ID_QUEUED_INIT_ERROR;
  }
    
}



/******************************************************************************
*
* DESCRIPTION
*      This function initializes the transmit-queue for a TX-message, which is 
*      used by the function dsXXXX_can_msg_send_id_queued(). It causes the
*      allocation of a circular buffer on slave with the specified overrun policy
*      where the transmit-orders from ds1103_can_msg_send_id_queued() are stored.
*      The queue is able to store 64 message entries.
*
* PARAMETERS
*      canMsg    pointer to the TX-message
*
*      overrun_policy  The overrun policy can be one of the following
*                      two constants:
*                      DS1103_CAN_TXQUEUE_OVERRUN_OVERWRITE
*                        overwrite the oldest message entry with
*                        the new one
*                      DS1103_CAN_TXQUEUE_OVERRUN_IGNORE
*                        keep oldest message and ignore the
*                        overrun (new message is lost)
*
*      delay    This parameter specifies the timely distance in
*               seconds between the messages of the transmit-queue
*               (min. 0.0 s, max. 10 s)
*
* RETURNS
*      errorcode; the following symbols are predefined: 
*
*      DS1103_CAN_TXQUEUE_INIT_MSG_NOT_REG_ERROR
*      The message is not registered. The operation is 
*      aborted.
*
*      DS1103_CAN_TXQUEUE_INIT_MSG_TYPE_ERROR
*      The message is not a TX-message. The operation is 
*      aborted.
*      
*      DS1103_CAN_BUFFER_OVERFLOW
*      Not enough memory space between the master write pointer and
*      the slave read pointer. The operation is aborted.
*      Repeat the function until it returns DS1103_CAN_NO_ERROR.
*  
*      DS1103_CAN_NO_ERROR
*      The function has been performed without error.
* 
* REMARKS 
*      Only one transmit-queue for a STD-message and one for a EXT-message
*      for every channel possible!
*
******************************************************************************/

#ifndef _INLINE

Int32 ds1103_can_msg_txqueue_init ( ds1103_canMsg* canMsg,
                                 const UInt32 overrun_policy,
                                 Float32 delay)
{
  UInt32 delaycnt;
  UInt32 parms[DEF1103_CAN_MSG_TXQUEUE_INIT_PCNT];
 
  if( NULL == canMsg )
  {
    ds1103_can_error_handler (DS1103_CAN_TXQUEUE_INIT_MSG_NOT_REG_ERROR,
                             "ds1103_can_msg_txqueue_init");

    return DS1103_CAN_TXQUEUE_INIT_MSG_NOT_REG_ERROR;
  }

  if( DS1103_CAN_TX != canMsg->type )
  {
    ds1103_can_error_handler (DS1103_CAN_TXQUEUE_INIT_MSG_TYPE_ERROR,
                             "ds1103_can_msg_txqueue_init");

    return DS1103_CAN_TXQUEUE_INIT_MSG_TYPE_ERROR;
  } 

  /* range check: delay */
  if( delay < DS1103_CAN_TXQUEUE_INIT_MIN_DELAY_TIME )
    delay = DS1103_CAN_TXQUEUE_INIT_MIN_DELAY_TIME;

  if( delay > DS1103_CAN_TXQUEUE_INIT_MAX_DELAY_TIME )
  {
    msg_warning_printf(MSG_SM_CAN1401, DS1103_CAN_TXQUEUE_INIT_DELAYTIME_WARN, 
    DS1103_CAN_TXQUEUE_INIT_DELAYTIME_WARN_TXT);

    delay = DS1103_CAN_TXQUEUE_INIT_MAX_DELAY_TIME;
  } 

  delaycnt = ds1103_can_time2count(delay);

  parms[ DEF1103_CAN_MSG_TXQUEUE_INIT_MSGNO_LOC ]    = canMsg->msg_no;
  parms[ DEF1103_CAN_MSG_TXQUEUE_INIT_POLICY_LOC ]   = overrun_policy;
  parms[ DEF1103_CAN_MSG_TXQUEUE_INIT_DELAY_L_LOC ] = delaycnt & 0xFFFF;
  parms[ DEF1103_CAN_MSG_TXQUEUE_INIT_DELAY_H_LOC ] = delaycnt >> 16;

  canMsg->isTxqueued = DS1103_CAN_IS_TXQUEUED;

  return dsmcom_send (ds1103_can_task_list,
                      canMsg->queue, DEF1103_CAN_TXQUEUE_INIT,
                      DEF1103_CAN_MSG_TXQUEUE_INIT_PCNT, (unsigned long *) parms);
  
}

#endif



/******************************************************************************
*
* DESCRIPTION     Sets the properties for a CAN message
*
* PARAMETERS
*   canMsg        Pointer to the CAN message object.
*
*   type          use one of the following symbols:
*
*                 DS1103_CAN_MSG_QUEUE
*                 to change the queue size for a message. The default queue
*                 for a message after registration can store only one message. 
*
*                 DS1103_CAN_MSG_MASK 
*                 to receive more than one message over a CAN message.
*                 You must specify a bit mask as value. "0" in the bit 
*                 mask means "don't care" and "1" in the bit mask means 
*                 "must match". 
*                 
*                 DS1103_CAN_MSG_PERIOD
*                 to adjust the send period for an autorepeat message.
*
*                 DS1103_CAN_MSG_ID
*                 to change the identifier of a transmit or request message.
*
*                 DS1103_CAN_MSG_DSMCR_BUFFER
*                 to install a multi-client able ring buffer for a message.
*
*                 DS1103_CAN_MSG_LEN
*                 to change the length of a message.
*
*  value_p        Pointer to the value.
*
* EXAMPLE         Set the last four bits for canMsg to "don't care":
*
*                 UInt32 value = 0xFFFFFFF0; 
*
*                 ds1103_can_msg_set( canMsg, DS1103_CAN_MSG_MASK, &value );
* 
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/

__INLINE Int32 ds1103_can_msg_set( ds1103_canMsg* canMsg,
                                   const UInt32 type,
                                   const void* value_p)

{

  UInt32 dincount = 0;
  UInt32 parms[ DEF1103_CAN_MSG_SET_PCNT ];
  UInt32 period;

  if( NULL == canMsg )
  {
    return DS1103_CAN_TYPE_ERROR;  
  } 

  parms[DEF1103_CAN_MSG_SET_MSGNO_LOC] = canMsg->msg_no;
  parms[DEF1103_CAN_MSG_SET_TYPE_LOC]  = type;
    
  switch(type)
  {

    case DS1103_CAN_MSG_MASK:
       if ( DS1103_CAN_RX == canMsg->type )      
       {
         parms[DEF1103_CAN_MSG_SET_VALUE_L_LOC] = *(UInt32*)value_p & 0xFFFF;
         parms[DEF1103_CAN_MSG_SET_VALUE_H_LOC] = (*(UInt32*)value_p) >> 16;
       }
       else
       {
         return DS1103_CAN_TYPE_ERROR;
       }
     break;

    case DS1103_CAN_MSG_PERIOD:
       if( DS1103_CAN_TX == canMsg->type || DS1103_CAN_RQTX == canMsg->type )     
       {
         period = ds1103_can_time2count( *(Float32*)value_p );
         parms[DEF1103_CAN_MSG_SET_VALUE_L_LOC] = period & 0xFFFF;
         parms[DEF1103_CAN_MSG_SET_VALUE_H_LOC] = period >> 16;
       }
       else
       {
         return DS1103_CAN_TYPE_ERROR;
       }
     break;

    case DS1103_CAN_MSG_ID:
       if( DS1103_CAN_TX == canMsg->type || DS1103_CAN_RQTX == canMsg->type )     
       {
         parms[DEF1103_CAN_MSG_SET_VALUE_L_LOC] = *(long*)value_p & 0xFFFF;
         parms[DEF1103_CAN_MSG_SET_VALUE_H_LOC] = (*(long*)value_p) >> 16;
       }
       else
       {
         return DS1103_CAN_TYPE_ERROR;
       }
     break;
    
    case DS1103_CAN_MSG_QUEUE:

      if( canMsg->inform & DS1103_CAN_DATA_INFO )
      {
        dincount  = 5;  
      }

      if( canMsg->inform & DS1103_CAN_MSG_INFO )
      {
        dincount += 3;
      }

      if( canMsg->inform & DS1103_CAN_TIMECOUNT_INFO )
      {
        dincount += 3;
      }
      if( canMsg->inform & DS1103_CAN_DELAYCOUNT_INFO )
      {
        dincount += 2;
      }
      
      dslist_queue_free( canMsg->msgQueue );
      
      canMsg->msgQueue = dslist_queue_init( DSLIST_OVERRUN, *(long*)value_p, dincount * 4 );  

      return DS1103_CAN_NO_ERROR;

    case DS1103_CAN_MSG_DSMCR_BUFFER:

      if( canMsg->inform & DS1103_CAN_DATA_INFO )
      {
        dincount  = 5;  
      }
      if( canMsg->inform & DS1103_CAN_MSG_INFO )
      {
        dincount += 3;
      }
      if( canMsg->inform & DS1103_CAN_TIMECOUNT_INFO )
      {
        dincount += 3;
      }
      if( canMsg->inform & DS1103_CAN_DELAYCOUNT_INFO )
      {
        dincount += 2;
      }
     
      dsmcr_buffer( dincount * sizeof(UInt32), *(UInt32*)value_p, &canMsg->dsmcr_buffer);      

      return DS1103_CAN_NO_ERROR;

    case DS1103_CAN_MSG_LEN:
      {
         UInt32 len;

         if((DS1103_CAN_TX == canMsg->type) || (DS1103_CAN_RQTX == canMsg->type) || (DS1103_CAN_RM == canMsg->type))     
         {
           // Range check
           if(*(UInt32*)value_p <= 8)
              len = *(UInt32*)value_p;
           else
             len = 8;

           parms[DEF1103_CAN_MSG_SET_VALUE_L_LOC] = len;
         }
         else
         {
           return DS1103_CAN_TYPE_ERROR;
         }
      }
     break;

    default:
       return DS1103_CAN_TYPE_ERROR;
  }

  return dsmcom_var_send ( ds1103_can_task_list,
                           canMsg->queue, DEF1103_CAN_MSG_SET,
                           DEF1103_CAN_MSG_SET_PCNT, (unsigned long *) parms );

}





/******************************************************************************
*
* FUNCTION
*       Reads received data.
*
* SYNTAX
*       Int32 ds1103_can_msg_read ( ds1103_canMsg* canMsg )
*
* DESCRIPTION
*       This function reads the received data the number of the received
*       data bytes and the timestamps.
*
*       NOTE:
*       This function updates all message queues on the master for all
*       messages working on the same slave DPM queue. 
*
*       The function ds1103_can_msg_read copies the CAN messages from
*       the slave DPM into the master sided CAN message queues.
*
* PARAMETERS
*   canMsg      Pointer to the CAN message object.
*
* RETURNS       DS1103_CAN_NO_ERROR
*               One message was read.
*
*               DS1103_CAN_NO_DATA
*               No message was available.
*
*               DS1103_CAN_DATA_LOST
*               At least one message was overwritten in the queue.
*
******************************************************************************/

__INLINE Int32 ds1103_can_msg_read ( ds1103_canMsg* canMsg )
{

  UInt32* tmp;
  UInt32 x = 0;
  UInt32 timecount, wrapcount, deltacount, delaycount;

  rtlib_int_status_t int_status;  

  if(canMsg == NULL)
  {
    return DS1103_CAN_NO_DATA;
  }
   
  ds1103_can_msg_update_all(canMsg->queue);

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  tmp = dslist_queue_read_ptr(canMsg->msgQueue);

  if( NULL == tmp )
  {
    canMsg->processed = DS1103_CAN_NOT_PROCESSED;
    RTLIB_INT_RESTORE(int_status);
    return DS1103_CAN_NO_DATA;
  }


  if(canMsg->inform & DS1103_CAN_MSG_INFO)
  {
    canMsg->identifier = tmp[x++] & 0xFFFF;
    canMsg->identifier = canMsg->identifier | (tmp[x++] << 16);
    canMsg->format = tmp[x++];
  };

  if (canMsg->inform & DS1103_CAN_TIMECOUNT_INFO)
  {
    timecount  = tmp[x++] & 0x0000FFFF;
    timecount  = timecount | ((tmp[x++] << 16) & 0xFFFF0000);
    wrapcount  = tmp[x++];
  
    if( timecount >= canMsg->timecount )
    {
      deltacount = timecount - canMsg->timecount;
    }
    else 
    {
      deltacount = timecount + (0x7FFFFFFF - canMsg->timecount);
    }
      
    canMsg->timestamp = ds1103_can_time_convert (timecount, wrapcount);
    canMsg->deltatime = ds1103_can_time_convert (deltacount, 0);
    canMsg->timecount = timecount;
    canMsg->wrapcount = wrapcount;
  }

  if(canMsg->inform & DS1103_CAN_PROCESSED_INFO)
  {
    x++;
  }; 

  if ( canMsg->inform & DS1103_CAN_DELAYCOUNT_INFO )
  {
    delaycount = tmp[x++] & 0x0000FFFF;
    delaycount = delaycount | ((tmp[x++] << 16) & 0xFFFF0000);
    canMsg->delaytime = ds1103_can_time_convert ( delaycount, 0);
  }
    
  if ( canMsg->inform & DS1103_CAN_DATA_INFO )
  {
    canMsg->datalen = tmp[x++];
    ds1103_can_unpack_block_16in8 ( &tmp[x], canMsg->data, canMsg->datalen );
  }

  canMsg->processed  = DS1103_CAN_PROCESSED;

  if( DS1103_CAN_DATA_LOST == canMsg->error )
  {
    canMsg->error = DS1103_CAN_NO_ERROR;
    RTLIB_INT_RESTORE( int_status );
    return( DS1103_CAN_DATA_LOST );
  }
  else
  {
    RTLIB_INT_RESTORE( int_status );
    return( DS1103_CAN_NO_ERROR );
  }

}


/******************************************************************************
*
* DESCRIPTION
*      Use this function to read the data length, the data and the other
*      information (timestamps etc.) from the local buffer (message queue).
*      Before reading from local buffer the message contents must be copied with
*      ds1103_can_msg_update_all or ds1103_can_msg_copy_all_to_mem
*      from DPMem to local buffer.
*      
*      The return value provides information on whether or not the data is
*      new. If not, the existing parameter values remain unchanged. 
*
*      NOTE
*      The information being returned depends on the previously
*      specified parameter inform in the register function corresponding
*      to the message. 
*
*
* PARAMETERS
*      canMsg   pointer to the CAN message object
*
* RETURNS
*      error code; the following symbols are predefined: 
*
*      DS1103_CAN_NO_ERROR
*      One message was read.
*
*      DS1103_CAN_NO_DATA
*      No message was available.
*
*      DS1103_CAN_DATA_LOST
*      At least one message was overwritten in the queue.
*
******************************************************************************/

__INLINE Int32 ds1103_can_msg_read_from_mem ( ds1103_canMsg* canMsg )
{

  UInt32* tmp;
  UInt32 x = 0;
  UInt32 timecount, wrapcount, deltacount, delaycount;

  rtlib_int_status_t int_status;  

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if(canMsg == NULL)
  {
    RTLIB_INT_RESTORE( int_status );
    return DS1103_CAN_NO_DATA;
  }
   
  tmp = dslist_queue_read_ptr(canMsg->msgQueue);

  if( NULL == tmp )
  {
    canMsg->processed = DS1103_CAN_NOT_PROCESSED;
    RTLIB_INT_RESTORE(int_status);
    return DS1103_CAN_NO_DATA;
  }


  if(canMsg->inform & DS1103_CAN_MSG_INFO)
  {
    canMsg->identifier = tmp[x++] & 0xFFFF;
    canMsg->identifier = canMsg->identifier | (tmp[x++] << 16);
    canMsg->format = tmp[x++];
  };

  if (canMsg->inform & DS1103_CAN_TIMECOUNT_INFO)
  {
    timecount  = tmp[x++] & 0x0000FFFF;
    timecount  = timecount | ((tmp[x++] << 16) & 0xFFFF0000);
    wrapcount  = tmp[x++];
  
    if( timecount >= canMsg->timecount )
    {
      deltacount = timecount - canMsg->timecount;
    }
    else 
    {
      deltacount = timecount + (0x7FFFFFFF - canMsg->timecount);
    }
      
    canMsg->timestamp = ds1103_can_time_convert (timecount, wrapcount);
    canMsg->deltatime = ds1103_can_time_convert (deltacount, 0);
    canMsg->timecount = timecount;
    canMsg->wrapcount = wrapcount;
  }

  if(canMsg->inform & DS1103_CAN_PROCESSED_INFO)
  {
    x++;
  }; 

  if ( canMsg->inform & DS1103_CAN_DELAYCOUNT_INFO )
  {
    delaycount = tmp[x++] & 0x0000FFFF;
    delaycount = delaycount | ((tmp[x++] << 16) & 0xFFFF0000);
    canMsg->delaytime = ds1103_can_time_convert ( delaycount, 0);
  }
    
  if ( canMsg->inform & DS1103_CAN_DATA_INFO )
  {
    canMsg->datalen = tmp[x++];
    ds1103_can_unpack_block_16in8 ( &tmp[x], canMsg->data,  canMsg->datalen );
  }

  canMsg->processed  = DS1103_CAN_PROCESSED;

  if( DS1103_CAN_DATA_LOST == canMsg->error )
  {
    canMsg->error = DS1103_CAN_NO_ERROR;
    RTLIB_INT_RESTORE( int_status );
    return( DS1103_CAN_DATA_LOST );
  }
  else
  {
    RTLIB_INT_RESTORE( int_status );
    return( DS1103_CAN_NO_ERROR );
  }

}


/******************************************************************************
*
* DESCRIPTION
*      This function copies the contents of all communication queues
*      (DPMem) to the local buffers.
*
* PARAMETERS
*
* RETURNS
*
******************************************************************************/

__INLINE void ds1103_can_msg_copy_all_to_mem(void)
{
   UInt32 i;

   for(i = 0; i < 6 ; i++ )
   {
     ds1103_can_msg_update_all(i);
   }
}



/******************************************************************************
*
* DESCRIPTION
*      Use this function to read the data length, the data and the status
*      information from a multi-client able ring-buffer which was installed
*      with ds1103_can_msg_set before. Only registered clients are able
*      to read from a ring-buffer (refer to ds1103_can_dsmcr_client_create).
*      Before reading from local buffer the message contents must be copied
*      with ds1103_can_msg_update_all or ds1103_can_msg_copy_all_to_mem
*      from DPMem to local buffer.
*    
*      The return value provides information on whether or not the data is
*      new. If not, the existing parameter values remain unchanged. 
*
*      NOTE
*      The status information being returned depends on the previously
*      specified parameter inform in the register function corresponding
*      to the message. 
*
*
* PARAMETERS
*      clientMsg      pointer to the ds1103_canMsg structure of the client
*
*
* RETURNS
*      error code; the following symbols are predefined: 
*
*      DS1103_CAN_NO_ERROR
*      One message was read.
*
*      DS1103_CAN_NO_DATA
*      No message was available.
*
*      DS1103_CAN_DSMCR_OVERRUN_FUNCTION
*      DS1103_CAN_DSMCR_OVERRUN_ERROR
*      DS1103_CAN_DSMCR_OVERRUN_IGNORE
*      DS1103_CAN_DSMCR_OVERRUN_OVERWRITE
*      Ring-buffer overrun: error code depends on overrun policy
*      of the client (refer to ds1103_can_dsmcr_client_create)
*
******************************************************************************/

__INLINE Int32 ds1103_can_msg_read_from_dsmcr ( ds1103_canMsg* clientMsg )
{
    UInt32 tmp[DEF1103_CAN_MSG_STATUS_READ_DINCNT];
    UInt32 x = 0;
    UInt32 timecount, wrapcount, deltacount, delaycount;
    UInt32 error;
    rtlib_int_status_t int_status;  

    RTLIB_INT_SAVE_AND_DISABLE(int_status);
    
    if((NULL == clientMsg) || 
       ((UInt32)DS1103_CAN_MSG_DSMCR_NOT_INITIALIZED == clientMsg->dsmcr_buffer))
    {
      RTLIB_INT_RESTORE(int_status);
      return DS1103_CAN_NO_DATA;
    }

    error = dsmcr_read(clientMsg->dsmcr_buffer, clientMsg->clientNumber, (void*)tmp);

    if( DSMCR_BUFFER_EMPTY == error )
    {
      clientMsg->processed = DS1103_CAN_NOT_PROCESSED;

      RTLIB_INT_RESTORE(int_status);
      return DS1103_CAN_NO_DATA;
    }

    if( clientMsg->inform & DS1103_CAN_MSG_INFO )
    {
      clientMsg->identifier = tmp[x++] & 0xFFFF;
      clientMsg->identifier = clientMsg->identifier | (tmp[x++] << 16);
      clientMsg->format = tmp[x++];
    }  

    if ( clientMsg->inform & DS1103_CAN_TIMECOUNT_INFO )
    {
      timecount  = tmp[x++] & 0x0000FFFF;
      timecount  = timecount | ((tmp[x++] << 16) & 0xFFFF0000);
      wrapcount  = tmp[x++];

      if( timecount >= clientMsg->timecount )
      {
        deltacount = timecount - clientMsg->timecount;
      }
      else
      {
        deltacount = timecount + (0x7FFFFFFF - clientMsg->timecount);
      }
      
      clientMsg->timestamp = ds1103_can_time_convert ( timecount, wrapcount);
      clientMsg->deltatime = (Float32)ds1103_can_time_convert ( deltacount, 0);
      clientMsg->timecount = timecount;
      clientMsg->wrapcount = wrapcount;
    }

    if ( clientMsg->inform & DS1103_CAN_DELAYCOUNT_INFO )
    {
      delaycount  = tmp[x++] & 0x0000FFFF;
      delaycount  = delaycount  | ((tmp[x++] << 16) & 0xFFFF0000);
      clientMsg->delaytime = (Float32)ds1103_can_time_convert( delaycount, 0 );
    }
    
    if ( clientMsg->inform & DS1103_CAN_DATA_INFO )
    {
      clientMsg->datalen = tmp[x++];
      ds1103_can_unpack_block_16in8 ( &tmp[x], clientMsg->data, clientMsg->datalen ); 
    }

    clientMsg->processed  = DS1103_CAN_PROCESSED;

    /* canMsg->error was previous set from ds1103_can_msg_update_all(),
       ds1103_can_tx_msg_write_to_dsmcr() or ds1103_can_tx_msg_read_loopback() */ 
    
    if( DSMCR_NO_ERROR != clientMsg->error )
    {
      error = clientMsg->error;
      clientMsg->error = DSMCR_NO_ERROR;

      RTLIB_INT_RESTORE( int_status );
      return( (Int32)error );
    }
    else
    {
      RTLIB_INT_RESTORE( int_status );
      return( DS1103_CAN_NO_ERROR );
    }

}



/******************************************************************************
*
* DESCRIPTION
*      Use this function to write the contents of a TX-message together
*      with message data to the ring-buffer of a RX-message.
*
*      The return value provides information on whether or not the data is
*      new. If not, the existing parameter values remain unchanged.
*
*      NOTE
*      The status information being returned depends on the previously
*      specified parameter inform in the register function corresponding
*      to the message. 
*      It is necessary that the message contents of the TX-message are compatible
*      to the entry of the RX-message. This depends on the inform-parameter,
*      which is assigned during registering of the messages.
*      If the inform-parameter is not equal than the entry is not
*      written to the ring-buffer of the RX-message.
*
*
* PARAMETERS
*      txMsg     pointer to the TX-message structure 
*
*      data_len  length of the message data. Valid range is 0 ... 8 bytes. 
*
*      msg_data  pointer to message data 
*                     
*      rxMsg     pointer to the RX-message structure 
*
* RETURNS
*      error code; the following symbols are predefined: 
*
*      DS1103_CAN_NO_ERROR
*      One message was read.
*
*      DS1103_CAN_NO_DATA
*      No message was available.
*
*      DS1103_CAN_DSMCR_OVERRUN_FUNCTION
*      DS1103_CAN_DSMCR_OVERRUN_ERROR
*      DS1103_CAN_DSMCR_OVERRUN_IGNORE
*      DS1103_CAN_DSMCR_OVERRUN_OVERWRITE
*      Ring-buffer overrun: error code depends on overrun policy
*      of the client (refer to ds1103_can_dsmcr_client_create)
*
******************************************************************************/

__INLINE Int32 ds1103_can_tx_msg_write_to_dsmcr ( const ds1103_canMsg* txMsg,
                                               const UInt32 data_len,
                                               const UInt32* msg_data,
                                               ds1103_canMsg* rxMsg )
{
    UInt32 tmp[DEF1103_CAN_MSG_STATUS_READ_DINCNT];
    UInt32 len;
    UInt32 x = 0;
    UInt32 noentry = 0;
   
    rtlib_int_status_t int_status;  
 
    RTLIB_INT_SAVE_AND_DISABLE(int_status);
    
    if( (NULL == txMsg) ||
        ((UInt32)DS1103_CAN_MSG_DSMCR_NOT_INITIALIZED == rxMsg->dsmcr_buffer) ||
        (NULL == rxMsg) )
    {
      RTLIB_INT_RESTORE(int_status);
      return DS1103_CAN_NO_DATA;
    }

    if(rxMsg->inform & DS1103_CAN_MSG_INFO)
    {
      if(txMsg->inform & DS1103_CAN_MSG_INFO)
      {
        tmp[x++] = txMsg->identifier & 0xFFFF;
        tmp[x++] = txMsg->identifier >> 16;
        tmp[x++] = txMsg->format;
      }
      else 
      {
        noentry++;
      }
    }

    if(rxMsg->inform & DS1103_CAN_TIMECOUNT_INFO)
    {
      if(txMsg->inform & DS1103_CAN_TIMECOUNT_INFO)
      {
        tmp[x++] = txMsg->timecount & 0xFFFF;
        tmp[x++] = txMsg->timecount >> 16;
        tmp[x++] = txMsg->wrapcount;
      }
      else 
      {
        noentry++;
      }
    }

    if ( rxMsg->inform & DS1103_CAN_DATA_INFO )
    {
      tmp[x++] = data_len;
      len = (UInt32) ds1103_can_len_arr[data_len];
      ds1103_can_pack_block_8in16(msg_data, &tmp[x], len);  
    }

    if(!noentry)
    {
      rxMsg->error = dsmcr_write(rxMsg->dsmcr_buffer, (void*)tmp);
    } 
    else
    {
      rxMsg->error = DS1103_CAN_NO_DATA;
    }

    for(x=0; x<rxMsg->no_of_clients; x++)
    {
      rxMsg->clientMsgs[x]->error = dsmcr_client_last_write_state(rxMsg->dsmcr_buffer, 
                                    rxMsg->clientMsgs[x]->clientNumber);
    }

    RTLIB_INT_RESTORE( int_status );

    return(rxMsg->error);

}



/******************************************************************************
*
* DESCRIPTION
*      Use this function to read the message entry of a TX-message
*      from a multi-client able ring-buffer, which was installed
*      with ds1103_can_msg_set before, and to write the contents together
*      with message data to the ring-buffer of a RX-message.
*      Only registered clients are able to read from a ring-buffer
*      (refer to ds1103_can_dsmcr_client_create).
*      Before reading from local buffer the message contents must be copied
*      with ds1103_can_msg_update_all or ds1103_can_msg_copy_all_to_mem
*      from DPMem to local buffer.
*
*      The return value provides information on whether or not the data is
*      new. If not, the existing parameter values remain unchanged.
*
*      NOTE
*      The status information being returned depends on the previously
*      specified parameter inform in the register function corresponding
*      to the message. 
*      It is necessary that the message entry of the TX-message is compatible
*      to the entry of the RX-message. This depends on the inform-parameter,
*      which is assigned during registering of the messages.
*      If the inform-parameter is not equal than the entry is not
*      written to the ring-buffer of the RX-message.
*
*
* PARAMETERS
*      txclientMsg  pointer to the TX-message structure of the client
*
*      data_len  length of the message data. Valid range is 0 ... 8 bytes. 
*
*      msg_data  pointer to message data 
*                     
*      rxMsg     pointer to the RX-message structure 
*
* RETURNS
*      error code; the following symbols are predefined: 
*
*      DS1103_CAN_NO_ERROR
*      One message was read.
*
*      DS1103_CAN_NO_DATA
*      No message was available.
*
*      DS1103_CAN_DSMCR_OVERRUN_FUNCTION
*      DS1103_CAN_DSMCR_OVERRUN_ERROR
*      DS1103_CAN_DSMCR_OVERRUN_IGNORE
*      DS1103_CAN_DSMCR_OVERRUN_OVERWRITE
*      Ring-buffer overrun: error code depends on overrun policy
*      of the client (refer to ds1103_can_dsmcr_client_create)
*
******************************************************************************/

__INLINE Int32 ds1103_can_tx_msg_read_loopback ( ds1103_canMsg* txclientMsg,
                                                 const UInt32 data_len,
                                                 const UInt32* msg_data, 
                                                 ds1103_canMsg* rxMsg )

{
    UInt32 tmp[DEF1103_CAN_MSG_STATUS_READ_DINCNT];
    UInt32 rxtmp[DEF1103_CAN_MSG_STATUS_READ_DINCNT];
    UInt32 len;
    UInt32 x = 0;
    UInt32 y = 0;
    UInt32 timecount, wrapcount, deltacount, delaycount;
    UInt32 error;
    UInt32 noentry = 0;
    rtlib_int_status_t int_status;  


    RTLIB_INT_SAVE_AND_DISABLE(int_status);

    if( (NULL == txclientMsg) ||
        ((UInt32)DS1103_CAN_MSG_DSMCR_NOT_INITIALIZED == txclientMsg->dsmcr_buffer) || 
        (NULL == rxMsg) )
    {
      RTLIB_INT_RESTORE(int_status);
      return DS1103_CAN_NO_DATA;
    }

    if((UInt32)DS1103_CAN_MSG_DSMCR_NOT_INITIALIZED == rxMsg->dsmcr_buffer) 
      noentry++;

    error = dsmcr_read(txclientMsg->dsmcr_buffer, txclientMsg->clientNumber, (void*)tmp);

    if( DSMCR_BUFFER_EMPTY == error )
    {
      txclientMsg->processed = DS1103_CAN_NOT_PROCESSED;
      RTLIB_INT_RESTORE(int_status);
      return DS1103_CAN_NO_DATA;
    }

    if( txclientMsg->inform & DS1103_CAN_MSG_INFO )
    {
      txclientMsg->identifier = tmp[x] & 0x1FFFFFFF;
      txclientMsg->format     = (tmp[x] & 0x20000000) >> 29;
  
      txclientMsg->identifier = tmp[x++] & 0xFFFF;
      txclientMsg->identifier = txclientMsg->identifier | (tmp[x++] << 16);
      txclientMsg->format = tmp[x];

      if( rxMsg->inform & DS1103_CAN_MSG_INFO )
      {
        rxtmp[y++] = tmp[x-2];
        rxtmp[y++] = tmp[x-1];
        rxtmp[y++] = tmp[x];
      }

      x++;
    }
    else if( rxMsg->inform & DS1103_CAN_MSG_INFO ) 
    {
      noentry++;
    }

    if (txclientMsg->inform & DS1103_CAN_TIMECOUNT_INFO)
    {
      timecount  = tmp[x++] & 0x0000FFFF;
      timecount  = timecount | ((tmp[x++] << 16) & 0xFFFF0000);
      wrapcount  = tmp[x];

      if( timecount >= txclientMsg->timecount )
      {
        deltacount = timecount - txclientMsg->timecount;
      }
      else
      {
        deltacount = timecount + (0x7FFFFFFF - txclientMsg->timecount);
      }
      
      txclientMsg->timestamp = ds1103_can_time_convert ( timecount, wrapcount);
      txclientMsg->deltatime = (Float32)ds1103_can_time_convert ( deltacount, 0);
      txclientMsg->timecount = timecount;
      txclientMsg->wrapcount = wrapcount;

      if( rxMsg->inform & DS1103_CAN_TIMECOUNT_INFO )
      {
        rxtmp[y++] = tmp[x-2];
        rxtmp[y++] = tmp[x-1];
        rxtmp[y++] = wrapcount;
      }

      x++;

    }
    else if( rxMsg->inform & DS1103_CAN_TIMECOUNT_INFO ) 
    {
      noentry++;
    }

    if ( txclientMsg->inform & DS1103_CAN_DELAYCOUNT_INFO )
    {
      delaycount  = tmp[x++] & 0x0000FFFF;
      delaycount  = delaycount  | ((tmp[x++] << 16) & 0xFFFF0000);
      txclientMsg->delaytime = (Float32)ds1103_can_time_convert ( delaycount, 0);
    }

    if ( rxMsg->inform & DS1103_CAN_DATA_INFO )
    {
      rxtmp[y++] = data_len;
      len = (UInt32) ds1103_can_len_arr[data_len];
      ds1103_can_pack_block_8in16 (msg_data, &rxtmp[y], len);  
    }

    txclientMsg->processed  = DS1103_CAN_PROCESSED;

    if(!noentry)
    {
      rxMsg->error = dsmcr_write(rxMsg->dsmcr_buffer, (void*)rxtmp);
    } 
    else
    {
      rxMsg->error = DS1103_CAN_NO_DATA;
    }

    for(x=0; x<rxMsg->no_of_clients; x++)
    {
      rxMsg->clientMsgs[x]->error = dsmcr_client_last_write_state(rxMsg->dsmcr_buffer,
                                    rxMsg->clientMsgs[x]->clientNumber);
    }

    /* canMsg->error was previous set from ds1103_can_msg_update_all() */ 
    if( DSMCR_NO_ERROR != txclientMsg->error )
    {
      error = txclientMsg->error;
      txclientMsg->error = DSMCR_NO_ERROR;

      RTLIB_INT_RESTORE( int_status );
      return( (Int32)error );
    }
    else
    {
      RTLIB_INT_RESTORE( int_status );
      return( DS1103_CAN_NO_ERROR );
    }

}


/******************************************************************************
*
* DESCRIPTION
*      Returns the number of actually stored messages in the queue.
*
*      NOTE:
*      This function shows only the count of the messages in the
*      queue  on the master. The count of messages on the slave DPM
*      ( dual ported memory ) is not considered. 
*      The function ds1103_can_msg_read copies the CAN messages from
*      the slave DPM into the master sided CAN message queues.
*
* PARAMETERS
*      canMsg   pointer to the ds1103_canMsg structure 
*
* RETURNS         The number of messages in the queue.
*
******************************************************************************/

__INLINE Int32 ds1103_can_msg_queue_level( ds1103_canMsg* canMsg )
{
  return dslist_queue_level( canMsg->msgQueue );
}


/******************************************************************************
*
* FUNCTION
*   Triggers a message.
*
* SYNTAX
*   Int32 ds1103_can_msg_trigger (const ds1103_canMsg* canMsg)
*
* DESCRIPTION
*   This function triggers a tx- or rqtx-messages (acyclic messages).
*
* PARAMETERS
*   canMsg            handle to message structure
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_msg_trigger (const ds1103_canMsg* canMsg,
                                       const Float32 delay)
{
  UInt32 parms[ DEF1103_CAN_MSG_TRIGGER_PCNT ];
  UInt32 opcode;
   
  switch(canMsg->type)
  {
    case DS1103_CAN_TX:
      opcode = DEF1103_CAN_MSG_TRIGGER;
      break;
    case DS1103_CAN_RQTX:
      opcode = DEF1103_CAN_RQTX_MSG_TRIGGER  ;
      break;
    default:
      return DS1103_CAN_TYPE_ERROR;
  }
  
  parms[DEF1103_CAN_MSG_TRIGGER_IDX_LOC]    = canMsg->msg_no ;
  parms[DEF1103_CAN_MSG_TRIGGER_DELAYL_LOC] = ds1103_can_time2count(delay) & 0xFFFF;
  parms[DEF1103_CAN_MSG_TRIGGER_DELAYH_LOC] = ds1103_can_time2count(delay) >> 16;

  return dsmcom_send (ds1103_can_task_list, canMsg->queue, opcode, 
                      DEF1103_CAN_MSG_TRIGGER_PCNT, (unsigned long*)parms );
}

/******************************************************************************
*
* FUNCTION
*   Deactivates a message.
*
* SYNTAX
*   Int32 ds1103_can_msg_sleep (const ds1103_canMsg* canMsg)
*
* DESCRIPTION
*   These functions stops sending a message.
*
* PARAMETERS
*   canMsg        handle to message structure
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_msg_sleep (const ds1103_canMsg* canMsg)
{
 
  unsigned long parms[ DEF1103_CAN_DATA_DIRECT_PCNT ];
  UInt32 opcode;
  
  switch(canMsg->type)
  {
    case DS1103_CAN_TX:
      opcode = DEF1103_CAN_TX_MSG_SLEEP;
      break;
    case DS1103_CAN_RX:
      opcode = DEF1103_CAN_RX_MSG_SLEEP;
      break;
    case DS1103_CAN_RM:
      opcode = DEF1103_CAN_RM_MSG_SLEEP;
      break;
    case DS1103_CAN_RQTX:
      opcode = DEF1103_CAN_RQTX_MSG_SLEEP;
      break;
    case DS1103_CAN_RQRX:
      opcode = DEF1103_CAN_RQRX_MSG_SLEEP;
      break;
    default:
    return DS1103_CAN_TYPE_ERROR;
  }

  parms[ DEF1103_CAN_DATA_DIRECT_IDX_LOC ] = canMsg->msg_no;

  return ( dsmcom_send ( ds1103_can_task_list, canMsg->queue, opcode, 
                         DEF1103_CAN_DATA_DIRECT_PCNT, parms ) );
}

/******************************************************************************
*
* FUNCTION
*   Wakes up a sleeping message.
*
* SYNTAX
*   Int32 ds1103_can_msg_wakeup (ds1103_canMsg* canMsg)
*
* DESCRIPTION
*   These functions reactivates messages which are deactivated before.
*
* PARAMETERS
*   canMsg             structure handle to the corresponding message
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_msg_wakeup ( const ds1103_canMsg* canMsg )
{
  unsigned long parms[ DEF1103_CAN_DATA_DIRECT_PCNT ];
  UInt32 opcode;
    
  switch(canMsg->type)
  {
    case DS1103_CAN_TX:
      opcode = DEF1103_CAN_TX_MSG_WAKEUP;
      break;
    case DS1103_CAN_RX:
      opcode = DEF1103_CAN_RX_MSG_WAKEUP;
      break;
    case DS1103_CAN_RM:
      opcode = DEF1103_CAN_RM_MSG_WAKEUP;
      break;
    case DS1103_CAN_RQTX:
      opcode = DEF1103_CAN_RQTX_MSG_WAKEUP;
      break;
    case DS1103_CAN_RQRX:
      opcode = DEF1103_CAN_RQRX_MSG_WAKEUP;
      break;
    default:
      return DS1103_CAN_TYPE_ERROR;
  }

  parms[ DEF1103_CAN_DATA_DIRECT_IDX_LOC ] = canMsg->msg_no;

  return (dsmcom_send (ds1103_can_task_list, canMsg->queue, opcode, 
                       DEF1103_CAN_DATA_DIRECT_PCNT, parms) );
}

/******************************************************************************
*
* FUNCTION
*   Reads the processed flag and the timestamp.
*
* SYNTAX
*   void ds1103_can_msg_processed_register (ds1103_canMsg* canMsg)
*   Int32 ds1103_can_msg_processed_request ( const ds1103_canMsg* canMsg )
*   Int32 ds1103_can_msg_processed_read (ds1103_canMsg* canMsg,
*                                        Float64* timestamp,
*                                        UInt32*  processed)
*
* DESCRIPTION
*   This function reads the processed flag and the timestamp of a message.
*
* PARAMETERS
*   canMsg        handle to message structure
*   timestamp     timestamp of a message
*   processed     message state flag
*
* RETURNS
*   void / errorcode
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

void ds1103_can_msg_processed_register (ds1103_canMsg* canMsg)
{
  UInt32 parms[ DEF1103_CAN_PROCESSED_INFO_REG_PCNT ];
  UInt32 opcode; 
  Int32 error;
  ds1103_canService* procService;

  rtlib_int_status_t int_status;  

  if(canMsg==NULL)
  {
    return;
  }
        
  if(canMsg->type == DS1103_CAN_RQRX)
  {
    opcode = DEF1103_CAN_RQRX_MSG_PROC_REG;
  }
  else
  {
    opcode = DEF1103_CAN_MSG_PROC_REG;
  }

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  procService = (ds1103_canService*)malloc ( sizeof(ds1103_canService) );

  RTLIB_INT_RESTORE(int_status);
  
  if(procService==NULL)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }
 
  canMsg->msgService = procService;
 
  procService->queue  = TASK6;
  procService->index  = DS1103_CAN_AUTO_INDEX;
 
  parms[ DEF1103_CAN_PROCESSED_INFO_MSGNO_LOC ] = canMsg->msg_no;

  DS1103_CAN_TRY ( dsmcom_register ( ds1103_can_task_list, TASK6, 
                                    (int*)&procService->index, opcode,
                                    DEF1103_CAN_PROCESSED_INFO_DINCNT, 
                                    DEF1103_CAN_PROCESSED_INFO_REG_DOUTCNT,
                                    DEF1103_CAN_PROCESSED_INFO_REG_PCNT, 
                                    (unsigned long*) parms ) );
  
  ds1103_can_max_idx[TASK6]++;
  
  return;

Error:
  free(procService); 
  ds1103_can_error_handler (error, "ds1103_can_msg_processed_register");
  return;
}

#endif /* _INLINE */

/*****************************************************************************/
__INLINE Int32 ds1103_can_msg_processed_request ( const ds1103_canMsg* canMsg )
{
  if(canMsg==NULL)
  {
    return DS1103_CAN_NO_DATA;
  }
  else if(canMsg->msgService==NULL)
  { 
    return DS1103_CAN_NO_DATA;
  }
  else
  { 
    return dsmcom_execute ( ds1103_can_task_list,
                           (canMsg->msgService)->queue,
                           (canMsg->msgService)->index );
  }
}

/*****************************************************************************/
__INLINE Int32 ds1103_can_msg_processed_read (ds1103_canMsg* canMsg,
                                              Float64* timestamp,
                                              UInt32*  processed)
{ 
  UInt32 tmp[ DEF1103_CAN_PROCESSED_INFO_DINCNT ];
  UInt32 timecount, wrapcount;
   Int32 error;
     
  if(canMsg == NULL)
  {
    return DS1103_CAN_NO_DATA;
  }
  else if(canMsg->msgService==NULL)
  {
    return DS1103_CAN_NO_DATA;
  }
  
  DS1103_CAN_TRY( dsmcom_data_read_varlen ( ds1103_can_task_list,
                                            (canMsg->msgService)->queue,
                                            (canMsg->msgService)->index, 
                                            (unsigned long*) tmp ));

  timecount  = tmp[ DEF1103_CAN_PROCESSED_INFO_READ_TIMECL_LOC ] & 0x0000FFFF;
  timecount  = timecount | ((tmp[DEF1103_CAN_PROCESSED_INFO_READ_TIMECH_LOC] << 16) & 0xFFFF0000);
  wrapcount  = tmp[ DEF1103_CAN_PROCESSED_INFO_READ_WRAPC_LOC ];

  *processed = tmp[ DEF1103_CAN_PROCESSED_INFO_READ_PROCESS_LOC ];
  *timestamp = ds1103_can_time_convert ( timecount, wrapcount); 
  
Error:
  return (error);
}

/******************************************************************************
*
* FUNCTION
*   ds1103_can_msg_clear
*
* DESCRIPTION
*   This function clears the received data.
*   Additionally the same data is cleared of all ring buffer clients
*   of the message (DSMCR-buffer).
*
* PARAMETERS
*   canMsg        : pointer to the CAN message object
*
* RETURNS
*   
*
* REMARKS
*
******************************************************************************/
__INLINE void ds1103_can_msg_clear(ds1103_canMsg* canMsg)
{
  UInt32 i;

  canMsg->data[0]    = 0;
  canMsg->data[1]    = 0;
  canMsg->data[2]    = 0;
  canMsg->data[3]    = 0;
  canMsg->data[4]    = 0;
  canMsg->data[5]    = 0;
  canMsg->data[6]    = 0;
  canMsg->data[7]    = 0;
  canMsg->datalen    = 0;
  canMsg->timestamp  = 0;
  canMsg->deltatime  = 0;
  canMsg->timecount  = 0;
  canMsg->wrapcount  = 0;
  canMsg->delaytime  = 0;
  canMsg->processed  = DS1103_CAN_NOT_PROCESSED;

  /* clear contents of DSMCR-clients */
  for(i=0; i<canMsg->no_of_clients; i++)
  {
    canMsg->clientMsgs[i]->data[0] = 0;
    canMsg->clientMsgs[i]->data[1] = 0;
    canMsg->clientMsgs[i]->data[2] = 0;
    canMsg->clientMsgs[i]->data[3] = 0;
    canMsg->clientMsgs[i]->data[4] = 0;
    canMsg->clientMsgs[i]->data[5] = 0;
    canMsg->clientMsgs[i]->data[6] = 0;
    canMsg->clientMsgs[i]->data[7] = 0;
    canMsg->clientMsgs[i]->datalen = 0;
    canMsg->clientMsgs[i]->timestamp = 0;
    canMsg->clientMsgs[i]->deltatime = 0;
    canMsg->clientMsgs[i]->timecount = 0;
    canMsg->clientMsgs[i]->wrapcount = 0;
    canMsg->clientMsgs[i]->delaytime = 0;
    canMsg->clientMsgs[i]->processed = DS1103_CAN_NOT_PROCESSED;
  }

}

/******************************************************************************
*
* FUNCTION
*   Reads a service information.
*
* SYNTAX
*   ds1103_canService* ds1103_can_service_register (const UInt32 service_type)
*   Int32 ds1103_can_service_request (const ds1103_canService* service)
*   Int32 ds1103_can_service_read (ds1103_canService* service)
*
* DESCRIPTION
*   Depending on the registrated service type a service information is
*   returned.
*
* PARAMETERS
*   service_type : Service to be installed. You can combine the predefined
*                  symbols with the logical operator OR.
*                  The following symbols are predefined:
*
*                  NOTE
*
*                  Start the CAN channel with the enabled status interrupt
*                  to use the following predefined services
*                  (refer to ds1103_can_channel_start): 
*
*                  DS1103_CAN_SERVICE_TX_OK            
*                  Number of successfully sent TX/RM/RQTX messages. 
*
*                  DS1103_CAN_SERVICE_RX_OK            
*                  Number of successfully received RX/RQRX messages. 
*
*                  DS1103_CAN_SERVICE_CRC_ERR          
*                  Number of CRC errors. 
*
*                  DS1103_CAN_SERVICE_ACK_ERR          
*                  Number of acknowledge errors. 
*
*                  DS1103_CAN_SERVICE_FORM_ERR         
*                  Number of format errors. 
*
*                  DS1103_CAN_SERVICE_BIT1_ERR         
*                  Number of Bit1 errors. 
*
*                  DS1103_CAN_SERVICE_BIT0_ERR         
*                  Number of Bit0 errors. 
*
*                  DS1103_CAN_SERVICE_STUFFBIT_ERR     
*                  Number of stuff bit errors. 
*
*
*                  NOTE
*                  It is not necessary to start the CAN channel with the 
*                  enabled status interrupt if you are using only the following
*                  predefined services (refer to ds1103_can_channel_start):
*
*
*                  DS1103_CAN_SERVICE_BUSSTATUS        
*                  Status of the CAN controller returned in the member busstatus 
*                  of the structure ds1103_can_canService.
*
*                  the following symbols are predefined: 
* 
*                         DS1103_CAN_BUSOFF_STATE
*                         The CAN channel disconnects itself from the CAN bus.
*                         Use ds1103_can_channel_BOff_return to recover from 
*                         the bus off state. 
* 
*                         DS1103_CAN_WARN_STATE
*                         The CAN controller is still active.
*                         The CAN controller recovers from this state
*                         automatically.
* 
*                         DS1103_CAN_ACTIVE_STATE
*                         The CAN controller is active.
*
*                  DS1103_CAN_SERVICE_STDMASK          
*                  Status of the global standard mask register. 
*
*                  DS1103_CAN_SERVICE_EXTMASK
*                  Status of the global extended mask register.
*
*                  DS1103_CAN_SERVICE_MSG_MASK15          
*                  Status of the message 15 mask register. 
*
*                  DS1103_CAN_SERVICE_RX_LOST          
*                  Number of lost RX messages. 
*                  The RX lost counter will be incremented when a received
*                  message is overwritten in the receive mailbox before the
*                  message has been read.
*
*                  DS1103_CAN_SERVICE_DATA_LOST        
*                  Number of data lost errors. 
*                  The data lost counter will be incremented when the data
*                  of a message is overwritten before the data has been
*                  written to the communication queue.
*
*                  DS1103_CAN_SERVICE_MAILBOX_ERR      
*                  Number of mailbox errors. If a message to be sent can
*                  not be assigned to a mailbox, the mailbox error counter
*                  will be increased by one.
*
*                  DS1103_CAN_SERVICE_MAILBOX_ID(a)
*                  Status of the mailbox ID. The structure member data0
*                  contains the identifier of the corresponding mailbox.
*                  The structure member data1 contains the format information
*                  whether data0 contains standard or extended message
*                  identifier. 
*
*                  DS1103_CAN_SERVICE_TXQUEUE_OVERFLOW_COUNT
*                  Reads the transmit-queue overflow counter of
*                  a channel. For every message format (STD and EXT)
*                  a 16 Bit counter counts the occuring overflows of
*                  the corresponding transmit-queue.
*
*   service             handle to service structure
*
* RETURNS
*   service structure / errorcode
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

ds1103_canService* ds1103_can_service_register (const UInt32 service_type)
{
  unsigned long parms[DEF1103_CAN_MSGID_READ_PCNT];
  Int32 error;  

  ds1103_canService* canSer;

  rtlib_int_status_t  int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  canSer = (ds1103_canService*) malloc (sizeof(ds1103_canService));

  RTLIB_INT_RESTORE(int_status);

  if(canSer==NULL)
  {
   error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }

  canSer->index  = ds1103_canCh->index;
  canSer->queue  = TASK6;
  canSer->type   = service_type;
  canSer->busstatus    = 0;
  canSer->stdmask      = 0;
  canSer->extmask      = 0;
  canSer->msg_mask15   = 0;
  canSer->tx_ok        = 0;
  canSer->rx_ok        = 0;
  canSer->crc_err      = 0;
  canSer->ack_err      = 0;
  canSer->form_err     = 0;
  canSer->stuffbit_err = 0;
  canSer->bit1_err     = 0;
  canSer->bit0_err     = 0;
  canSer->rx_lost      = 0;
  canSer->data_lost    = 0;
  canSer->version      = 0;
  canSer->mailbox_err  = 0;
  canSer->data0        = 0;
  canSer->data1        = 0;
  canSer->txqueue_overflowcnt_std = 0;
  canSer->txqueue_overflowcnt_ext = 0;

  if( DS1103_CAN_SERVICE_MB_ID & canSer->type )
  {
    canSer->type = DS1103_CAN_SERVICE_MB_ID;      

    parms[ DEF1103_CAN_MSGID_READ_MBNO_LOC ]    = (service_type >> 8) & 0xF;

    DS1103_CAN_TRY( dsmcom_register( ds1103_can_task_list,
                  TASK6, (int*)&canSer->index, 
                  DEF1103_CAN_MSGID_READ,
                  DEF1103_CAN_MSGID_READ_DINCNT,
                  DEF1103_CAN_MSGID_READ_DOUTCNT,
                  DEF1103_CAN_MSGID_READ_PCNT, parms ));

  }
  else
  {
    parms[DEF1103_CAN_SERV_MULTI_INFORM_L_LOC] = service_type & 0xFFFF;
    parms[DEF1103_CAN_SERV_MULTI_INFORM_H_LOC] = service_type >> 16;

    DS1103_CAN_TRY( dsmcom_register( ds1103_can_task_list,
                  TASK6, (int*)&canSer->index, 
                  DEF1103_CAN_SERV_MULTI,
                  DEF1103_CAN_SERV_MULTI_DINCNT,
                  DEF1103_CAN_SERV_MULTI_DOUTCNT,
                  DEF1103_CAN_SERV_MULTI_PCNT,
                  parms ));
  }

  (ds1103_can_max_idx[TASK6])++;

  return (canSer);

Error:
  free (canSer); 
  RTLIB_INT_RESTORE(int_status);
  ds1103_can_error_handler(error, "ds1103_can_service_register");
  return NULL;
}

#endif /* _INLINE */


/******************************************************************************/
__INLINE Int32 ds1103_can_service_request (const ds1103_canService* service)
{
  if(service==NULL)
  {
    return DS1103_CAN_NO_DATA;
  } 
  else
  {
    return dsmcom_execute (ds1103_can_task_list, TASK6, service->index);
  }
}


/******************************************************************************
*
* DESCRIPTION
*      Use this function to read the service information from CAN1103. 
*
*      Prior to this, this information must have been requested by the master
*      calling the function ds1103_can_service_request that asks for the 
*      service information from CANTp1. 
*
*      Use the returned handle from the function ds1103_can_service_register. 
*
* PARAMETERS
*      service   Pointer to the updated ds1103_can_canService structure.
*                The following data will be updated if available: 
*
*                busstatus; stdmask; extmask; msg_mask15; tx_ok;
*                rx_ok; crc_err; ack_err; form_err; stuffbit_err;
*                bit1_err; bit0_err; rx_lost; data_lost; mailbox_err;
*                data0; data1; txqueue_overflowcnt_std;
*                txqueue_overflowcnt_ext
*
* RETURNS         
*      error code; the following symbols are predefined: 
*
*      DS1103_CAN_NO_ERROR
*      The function has been performed without error.
*
*      DS1103_CAN_NO_DATA
*      No data was updated.
*
*      DS1103_CAN_DATA_LOST
*      The input data of a previous request for the specified
*      function has been overwritten.
*
******************************************************************************/

__INLINE Int32 ds1103_can_service_read (ds1103_canService* service)
{

  UInt32 i = 0;
  UInt32 data[DEF1103_CAN_SERV_MULTI_DINCNT]; 
  Int32 error;


  if(service == NULL)
  {
    error = DS1103_CAN_NO_DATA; goto Error;
  }

  DS1103_CAN_TRY (dsmcom_data_read_varlen (ds1103_can_task_list,
                           TASK6, service->index, (unsigned long *)data ));
 
  service->data0 = data[DEF1103_CAN_MSGID_READ_IDENTL_LOC] | 
                   (data[DEF1103_CAN_MSGID_READ_IDENTH_LOC] << 16);

  if( DS1103_CAN_SERVICE_MB_ID & service->type )
  {
    service->data1 = data[DEF1103_CAN_MSGID_READ_FORMAT_LOC];
    return error;
  }
  
  if( DS1103_CAN_SERVICE_BUSSTATUS & service->type)
  {
    if(data[i] & DS1103_CAN_BUSOFF)
    {
      service->busstatus = DS1103_CAN_BUSOFF_STATE;
      service->data0 = DS1103_CAN_BUSOFF_STATE;
    }
    else if(data[i] & DS1103_CAN_WARN)
    {
      service->busstatus = DS1103_CAN_WARN_STATE;
      service->data0 = DS1103_CAN_WARN_STATE;
    }
    else
    {
      service->busstatus = DS1103_CAN_ACTIVE_STATE;
      service->data0 = DS1103_CAN_ACTIVE_STATE;
    }

    i++;
  }

  if(DS1103_CAN_SERVICE_STDMASK & service->type)
  { 
    service->stdmask      = data[i++];
    service->stdmask      = service->stdmask | (data[i++] << 16);
  } 

  if(DS1103_CAN_SERVICE_EXTMASK & service->type)
  { 
    service->extmask      = data[i++];
    service->extmask      = service->extmask | (data[i++] << 16);
  } 

  if(DS1103_CAN_SERVICE_MSG_MASK15 & service->type)
  {
    service->msg_mask15   = data[i++];
    service->msg_mask15   = service->msg_mask15 | (data[i++] << 16);
  } 

  if(DS1103_CAN_SERVICE_TX_OK & service->type)		     
  { 
    service->tx_ok        = data[i++];
    service->tx_ok        = service->tx_ok | (data[i++] << 16);
  }

  if(DS1103_CAN_SERVICE_RX_OK & service->type)
  {
    service->rx_ok        = data[i++];
    service->rx_ok        = service->rx_ok | (data[i++] << 16);
  }
  
  if(DS1103_CAN_SERVICE_CRC_ERR & service->type)	 
  {
    service->crc_err      = data[i++];
    service->crc_err      = service->crc_err | (data[i++] << 16);
  }

  if(DS1103_CAN_SERVICE_ACK_ERR & service->type)
  {
    service->ack_err      = data[i++];
    service->ack_err      = service->ack_err | (data[i++] << 16);
  }

  if(DS1103_CAN_SERVICE_FORM_ERR & service->type)
  {  
    service->form_err     = data[i++];
    service->form_err     = service->form_err | (data[i++] << 16);
  }

  if(DS1103_CAN_SERVICE_STUFFBIT_ERR & service->type)
  {  
    service->stuffbit_err = data[i++];
    service->stuffbit_err = service->stuffbit_err | (data[i++] << 16);
  }

  if(DS1103_CAN_SERVICE_BIT1_ERR & service->type)
  { 
    service->bit1_err     = data[i++];
    service->bit1_err     = service->bit1_err | (data[i++] << 16);
  }

  if(DS1103_CAN_SERVICE_BIT0_ERR & service->type)
  {  
    service->bit0_err     = data[i++];
    service->bit0_err     = service->bit0_err | (data[i++] << 16);
  }

  if(DS1103_CAN_SERVICE_RX_LOST & service->type)
  {  
    service->rx_lost      = data[i++];
    service->rx_lost      = service->rx_lost | (data[i++] << 16);
  }

  if(DS1103_CAN_SERVICE_DATA_LOST & service->type)
  {
    service->data_lost    = data[i++];
    service->data_lost    = service->data_lost | (data[i++] << 16);
  } 

  if(DS1103_CAN_SERVICE_VERSION & service->type)
  {
    service->version      = data[i++];
    service->data0        = service->version;
  }

  if(DS1103_CAN_SERVICE_MAILBOX_ERR & service->type)
  {  
    service->mailbox_err  = data[i++];
    service->mailbox_err  = service->mailbox_err | (data[i++] << 16);
  }

  if( DEF1103_CAN_SVC_TXQUEUE_OVERFLOW_COUNT & service->type)
  {
    service->txqueue_overflowcnt_std = data[i++];
    service->txqueue_overflowcnt_ext = data[i++];
  } 

  return error;

Error:
  return error;

}



/******************************************************************************
*
* FUNCTION
*   Read CAN controller error status.
*
* SYNTAX
*   Int32 ds1103_can_error_read ( const Int32 queue )
*
* DESCRIPTION
*   The CAN controller error status for the specified communication channel
*   is read from the communication memory.
*
* PARAMETERS
*   queue             task ID of communication channel
*
* RETURNS
*   slave_error       slave error number
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_error_read ( const Int32 queue )
{
  unsigned long slave_error ;

  unsigned long int_handle;                           /* interrupt status */
  
  RTLIB_INT_SAVE_AND_DISABLE(int_handle);

  dsmcom_slave_error_read ( ds1103_can_task_list, queue, (
                            unsigned long*) &slave_error );

  if (slave_error != DS1103_CAN_NO_ERROR)
  {
   dsmcom_slave_error_clear (ds1103_can_task_list, queue );
  }

  RTLIB_INT_RESTORE(int_handle);
  
  switch (slave_error)
  {
    case DSMCOM_NO_ERROR:            return (DS1103_CAN_NO_ERROR);
    case DSMCOM_SLV_ALLOC_ERROR:     return (DS1103_CAN_SLAVE_ALLOC_ERROR);
    case DSMCOM_SLV_BUFFER_OVERFLOW: return (DS1103_CAN_SLAVE_BUFFER_OVERFLOW);
    case DSMCOM_SLV_INIT_ACK:        return (DS1103_CAN_INIT_ACK);
    default:                         return (DS1103_CAN_NO_ERROR);
  }
}

/******************************************************************************
*
* FUNCTION
*   Clear all data of the command-table.
*
* SYNTAX
*   void ds1103_can_all_data_clear (void)
*
* DESCRIPTION
*   This function clears all data of the command-table.
*
* PARAMETERS
*
* RETURNS
*    void
*
* REMARKS
*
******************************************************************************/
__INLINE void ds1103_can_all_data_clear (void)
{
  UInt32 queue;
  UInt32 i, id_x, data_cnt;
  UInt32 data_buf[40];
  ds1103_canMsg* canMsg;
 
  for(queue=0; queue < DEF1103_CAN_BUFFER_NUM; queue++)
  {
    if(ds1103_can_max_idx[queue])
    {
      dsmcom_data_read_varlen( ds1103_can_task_list, queue, 
                               ds1103_can_max_idx[queue], 
                               (unsigned long*) data_buf );
    }
    
    for (id_x = 1; id_x < ( ds1103_can_max_idx[queue] + 1 ); id_x++ )
    {
      data_cnt = ds1103_can_task_list[queue]->cmdtbl_entry[ id_x ]->data_in_count;
     
      for ( i = 0; i < data_cnt; i++ )
      {
        ( ds1103_can_task_list[queue]->cmdtbl_entry[ id_x ]->data_in[ i ] ) = 0;
      }
    
      if ( ds1103_can_msg_map[queue] != NULL )
      {
        canMsg = (ds1103_can_msg_map[queue])[id_x];  

        if ( canMsg != NULL )
        {
          ds1103_can_msg_clear(canMsg);

          if((UInt32)DS1103_CAN_MSG_DSMCR_NOT_INITIALIZED == canMsg->dsmcr_buffer)
            dslist_queue_clear_data( canMsg->msgQueue );
          else
            dsmcr_clear(canMsg->dsmcr_buffer);
        }
      }
    }
  }
}

/******************************************************************************
*
* FUNCTION
*   Sets the timer of the CAN controller.
*
* SYNTAX
*   void ds1103_can_timecount_set(Float64 time)
*
*
* DESCRIPTION
*   This function sets the timer of the CAN controller and allows the
*   synchronisation of PPC and CAN controller.
*
*
* PARAMETERS
*   time    time in seconds
*
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
__INLINE void ds1103_can_timecount_set(const Float64 time)
{
  UInt32 timecount;
  UInt32 wrapcount;
  UInt32 int_handle;

  volatile UInt16* sync_data;
  
  RTLIB_INT_SAVE_AND_DISABLE(int_handle);

  sync_data = (volatile UInt16*)DEF1103_CAN_TIMERSYNC_ADDR;
  
  wrapcount = (UInt32) (time/(DS1103_CAN_WRAPCOUNT * ds1103_can_timerticks));

  timecount = (UInt32) ((time - ((Float64)wrapcount * DS1103_CAN_WRAPCOUNT * 
                         ds1103_can_timerticks) ) / ds1103_can_timerticks );

  sync_data[DEF1103_CAN_TIMERSYNC_TIMECLO_LOC] = (UInt16)timecount;
  sync_data[DEF1103_CAN_TIMERSYNC_TIMECHI_LOC] = (UInt16)(timecount>>16);
  sync_data[DEF1103_CAN_TIMERSYNC_WRAPC_LOC] = wrapcount;

  dssint_interrupt(dssint_slvmc_sender, (unsigned int) DEF1103_CAN_TIMERSYN_SUBINT);

  RTLIB_INT_RESTORE(int_handle);
}

/******************************************************************************
*
* FUNCTION:
*   Handler for bufferwarn interrupts.
*
* SYNTAX:
*   void ds1103_can_bufferwarn_handler (void)
*
* DESCRIPTION:
*   This function can be used as an interrupt handler for the bufferoverflow-
*   warning interrupt (slave -> master ). If an interrupt occurs, the function
*   reads the buffer to avoid an overflow.
*
* PARAMETERS:
*   -
*
* RETURNS:
*   void
*
* NOTE:
*
******************************************************************************/

#ifndef _INLINE
void ds1103_can_bufferwarn_handler (void)
{
  UInt32 task_nr;

  dsmcom_cmdtbl_header_t  *cmdtbl_p;

  rtlib_int_status_t int_status;
  
  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  task_nr = (UInt32) DPM_READ(ds1103_can_task_list[0], 
                 ds1103_can_task_list[0]->slave_error_addr, 0);

  dsmcom_slave_error_clear (ds1103_can_task_list,
                            DEF1103_CAN_BUFFERWARN_QUEUE );

  RTLIB_INT_RESTORE(int_status);

  if ( task_nr < 6 )
  {
    ds1103_can_msg_update_all(task_nr);
  }
  else if ( task_nr == 6 )
  {
    cmdtbl_p = ds1103_can_task_list[task_nr];
    dsmcom_vardata_update (cmdtbl_p);
  }
}
#endif

/******************************************************************************
*
* DESCRIPTION
*   This function installs a sub-interrupt handler. At function call the 
*   sub-interrupt number is passed to the installed sub-interrupt handler.
*      
* PARAMETERS
*   module
*   subint_handler
*
* RETURNS
*
******************************************************************************/
#ifndef _INLINE

ds1103_can_subint_handler_t ds1103_can_subint_handler_install( 
                                          ds1103_can_subint_handler_t subint_handler)
{
  ds1103_can_subint_handler_t dummy_handler;
  
  dummy_handler = ds1103_can_subint_handler;

  ds1103_can_subint_handler = subint_handler;

  return (dummy_handler);
}

#endif


/******************************************************************************
*
* DESCRIPTION
*       The function creates a new client for the multi-client ring
*       buffer of the specified message (must be initialized with
*       ds1103_can_msg_set first). To read a message from the ring buffer
*       the function ds1103_can_msg_read_from_dsmcr must be used. The
*       function ds1103_can_tx_msg_read_loopback allows to read from the
*       ring buffer of a TX-message and to write the information entries
*       to the ring buffer of a RX-message. The client message, which is
*       allocated by this function, is a copy of the original message and must
*       be used when calling ds1103_can_msg_read_from_dsmcr or
*       ds1103_can_tx_msg_read_loopback
*
*
* PARAMETERS
*
*       canMsg     Pointer to the ds1103_canMsg structure of a TX-message
*                  with a multi-client able ring buffer
*
*       overrun_policy   DS1103_CAN_DSMCR_OVERRUN_FUNCTION
*                          call a client specific function to handle
*                          the overrun
*                        DS1103_CAN_DSMCR_OVERRUN_ERROR
*                          stop execution and display an error message
*                        DS1103_CAN_DSMCR_OVERRUN_IGNORE
*                          omit this entry and ignore the overrun
*                        DS1103_CAN_DSMCR_OVERRUN_OVERWRITE
*                          overwrite the oldest buffer entry with this entry
*
*       p_overrun_fcn    overrun function pointer for overrun policy
*                        DS1103_CAN_DSMCR_OVERRUN_FUNCTION,
*                        NULL for other overrun policies
*
*
*       clientMsg        Address of the pointer to the client message, which
*                        is allocated by this function. The client message
*                        is a copy of the original message (where canMsg
*                        points to).
*
*
* RETURNS 
*       DS1103_CAN_NO_ERROR  The client was created successfully.
*
******************************************************************************/

#ifndef _INLINE

Int32 ds1103_can_dsmcr_client_create(ds1103_canMsg* canMsg,
                                  const UInt32 overrun_policy,
                                  dsmcr_t_p_overrun_fcn p_overrun_fcn,
                                  ds1103_canMsg** clientMsg)
{

  rtlib_int_status_t   int_status;
  Int32 error;


  RTLIB_INT_SAVE_AND_DISABLE(int_status);
    
  *clientMsg = (ds1103_canMsg*) malloc (sizeof(ds1103_canMsg));

  if(*clientMsg == NULL)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }
  
  (*clientMsg)->timestamp = 0.0;
  (*clientMsg)->deltatime = 0.0;
  (*clientMsg)->delaytime = 0.0;
  (*clientMsg)->processed = 0;
  (*clientMsg)->datalen = 0;
  (*clientMsg)->data[0] = 0;
  (*clientMsg)->data[1] = 0;
  (*clientMsg)->data[2] = 0;
  (*clientMsg)->data[3] = 0;
  (*clientMsg)->data[4] = 0;
  (*clientMsg)->data[5] = 0;
  (*clientMsg)->data[6] = 0;
  (*clientMsg)->data[7] = 0;
  (*clientMsg)->identifier = canMsg->identifier;
  (*clientMsg)->format = canMsg->format;
  (*clientMsg)->queue = canMsg->queue;
  (*clientMsg)->index = canMsg->index;
  (*clientMsg)->msg_no = canMsg->msg_no;
  (*clientMsg)->type = canMsg->type;
  (*clientMsg)->inform = canMsg->inform;
  (*clientMsg)->timecount = 0;
  (*clientMsg)->wrapcount = 0;
  (*clientMsg)->error = 0;
  (*clientMsg)->isTxqueued = canMsg->isTxqueued;
  (*clientMsg)->msgQueue = canMsg->msgQueue;
  (*clientMsg)->canChannel = canMsg->canChannel;
  (*clientMsg)->msgService = canMsg->msgService;
  (*clientMsg)->dsmcr_buffer = canMsg->dsmcr_buffer;
  (*clientMsg)->no_of_clients = 0;
  (*clientMsg)->clientMsgs = NULL;

  canMsg->clientMsgs = (ds1103_canMsg**)realloc((void*)canMsg->clientMsgs,
                        (canMsg->no_of_clients + 1) * sizeof(ds1103_canMsg*));

  if(NULL == canMsg->clientMsgs)
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }

  (*clientMsg)->rootMsg = canMsg;

  error = dsmcr_client((*clientMsg)->dsmcr_buffer, overrun_policy,
                       p_overrun_fcn, &(*clientMsg)->clientNumber);

  canMsg->clientMsgs[canMsg->no_of_clients] = (*clientMsg);
  canMsg->no_of_clients++;

  RTLIB_INT_RESTORE(int_status);

  return (error);

  Error:
     ds1103_can_error_handler (error, "ds1103_can_dsmcr_client_create");
     RTLIB_INT_RESTORE(int_status);
     return error;

}

#endif

  
/******************************************************************************
*
* FUNCTION
*
* SYNTAX
*   Int32 ds1103_can_get_subint (void)
*
* DESCRIPTION
*   Function is called repetitively within an interrupt handler and processes
*   the sub-interrupt information copied to the receiver data structure
*   by dssint_acknowledge.
*
* PARAMETERS
*
*
* RETURNS
*   DS1103_CAN_NO_SUBINT if there is no pending sub-interrupt
*   number of highest priority pending sub-interrupt
*
* REMARKS
*
******************************************************************************/
__INLINE Int32 ds1103_can_subint_get (void)
{
  return dssint_decode(dssint_slvmc_receiver);
}

/******************************************************************************
*
* FUNCTION
*   Acknowledge the interrupt.
*
* SYNTAX
*   void ds1103_can_sint_acknowledge(void)
*
* DESCRIPTION
*   The function acknowledges the interrupt (hardware and software).
*      
* PARAMETERS
*   
*
* RETURNS
*    errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE void ds1103_can_subint_acknowledge (void)
{
  dssint_acknowledge(dssint_slvmc_receiver);
}

/******************************************************************************
*
* FUNCTION
*   Interrupt service handler for CAN and Serial Interface.
*
*   The interrupt service handler for the salve processor is installed by the
*   communication init. 
*
* PARAMETERS: none
*
* RETURNS: void
*
******************************************************************************/

#ifndef _INLINE

void ds1103_can_int_handler( void )
{
  Int32  subint;
  UInt32 is_subint = 0;

  rtlib_int_status_t int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  dssint_acknowledge( dssint_slvmc_receiver );
  
  do {
       subint = dssint_decode( dssint_slvmc_receiver );

       switch (subint)
       {
        case DS1103_CAN_SUBINT_BUFFERWARN:
          ds1103_can_bufferwarn_handler();
        break;

        case DS1103_CAN_NO_SUBINT:
          if (is_subint)
          {
           (*ds1103_can_subint_handler)( NULL, subint );
          }
        break;
        
        default:
          is_subint = 1;
          (*ds1103_can_subint_handler)( NULL, subint );
        break;
      };
  } while( subint != DS1103_CAN_NO_SUBINT );

  RTLIB_INT_RESTORE( int_status );
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Data RAM read access.
*
* SYNTAX
*   void ds1103_can_mem_read_register (const Int32 queue, Int32 *index,
*                  const UInt32 addr, const UInt32 len )
*   Int32 ds1103_can_mem_read_request (const Int32 queue, const Int32 index )
*   Int32 ds1103_can_mem_read ( const  Int32 queue, const  Int32 index,
*                               UInt32 *data )
*
* DESCRIPTION
*   A block of 16 bit data is read from the slave adress range.
*
* PARAMETERS
*   queue               task ID of communication channel
*   index               function index (DS1103_CAN_AUTO_INDEX: assign new index)
*   addr                source address in data RAM
*   data                destination data buffer
*   len                 length of data block to be read
*
* RETURNS
*   void / errorcode
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

void ds1103_can_mem_read_register (const Int32 queue,
                                         Int32 *index, 
                                   const UInt32 addr,
                                   const UInt32 len )
{
  UInt32 parms[ DEF1103_CAN_MEM_READ_PCNT ];
   Int32 error;

  parms[ DEF1103_CAN_MEM_READ_ADDR_LOC ] = addr;
  parms[ DEF1103_CAN_MEM_READ_CNT_LOC ]  = len;
  
  DS1103_CAN_TRY ( dsmcom_register ( ds1103_can_task_list, TASK6, (int*)index,
                                     DEF1103_CAN_MEM_READ, len,
                                     DEF1103_CAN_MEM_READ_DOUTCNT, 
                                     DEF1103_CAN_MEM_READ_PCNT, 
                                     (unsigned long*)parms) );

  ds1103_can_max_idx[TASK6]++;
  return;

Error:
  ds1103_can_error_handler(error, "ds1103_can_mem_read_register");
  return;
}
#endif /* _INLINE */

/******************************************************************************/
__INLINE Int32 ds1103_can_mem_read_request (const Int32 queue, 
                                            const Int32 index )
{
 return dsmcom_execute ( ds1103_can_task_list, TASK6, index );
}

/******************************************************************************/
__INLINE Int32 ds1103_can_mem_read ( const  Int32 queue, 
                                     const  Int32 index,
                                           UInt32 *data )
{
 return dsmcom_data_read_varlen (ds1103_can_task_list, TASK6, index,
                                 (unsigned long*) data );
}

/******************************************************************************
*
* FUNCTION
*   Data RAM write access.
*
* SYNTAX
*   void ds1103_can_mem_write_register (const Int32 queue, Int32 *index,
*                                       const UInt32 addr, const UInt32 len)
*   Int32 ds1103_can_mem_write (const Int32 queue, const Int32 index,
*                               const UInt32 *data)
*
* DESCRIPTION
*   A block of 16 bit data is written to the CAN controller RAM.
*
* PARAMETERS
*   queue               task ID of communication channel
*   index               function index (DS1103_CAN_AUTO_INDEX: assign new index)
*   addr                destination address in RAM
*   data                source data buffer
*   len                 length of data block to be written
*
* RETURNS
*   void / errorcode
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE
void ds1103_can_mem_write_register (const Int32 queue,
                                          Int32 *index,
                                    const UInt32 addr,
                                    const UInt32 len)
{
  UInt32 parms[ DEF1103_CAN_MEM_WRITE_PCNT ];
   Int32 error;

  parms[ DEF1103_CAN_MEM_WRITE_ADDR_LOC ] = addr;
  parms[ DEF1103_CAN_MEM_WRITE_CNT_LOC ]  = len;
  
  DS1103_CAN_TRY ( dsmcom_register ( ds1103_can_task_list, queue, (int*)index, 
                                     DEF1103_CAN_MEM_WRITE, 
                                     DEF1103_CAN_MEM_WRITE_DINCNT, len, 
                                     DEF1103_CAN_MEM_WRITE_PCNT, 
                                     (unsigned long*)parms) );
  
  ds1103_can_max_idx[queue]++;
  return;

Error:
  ds1103_can_error_handler (error, "ds1103_can_mem_write_register");
  return;
}

#endif /* _INLINE */

/******************************************************************************/
__INLINE Int32 ds1103_can_mem_write (const Int32 queue,
                                     const Int32 index,
                                     const UInt32 *data)
{
  return dsmcom_execute_data (ds1103_can_task_list, queue, index, 
                              (unsigned long*)data);
}

/******************************************************************************
*
* FUNCTION
*   Service function for subinterrupt test.
*
* SYNTAX
*   Int32 ds1103_can_subint_request(const UInt32 subint)
*
* DESCRIPTION
*   The function requests the subinterrupt (slave->master).
*      
* PARAMETERS
*   subint                  number of the requested subinterrupt
*
* RETURNS
*    errorcode
*
* REMARKS
*   Only for test purposes
*
******************************************************************************/
__INLINE Int32 ds1103_can_subint_request (const UInt32 subint)
{
  Int32 error;
  
  if ( (subint >= 0) && (subint < SINT1103_SINT_MEM_WIDTH) )
  {
    return dsmcom_send ( ds1103_can_task_list, TASK0,  DEF1103_CAN_SUBINT_REQUEST, 
                         DEF1103_CAN_DATA_DIRECT_PCNT, (unsigned long*) &subint );
  }
  else
  {
    error = DS1103_CAN_SUBINT_OUT_OF_RANGE; goto Error;
  }
  
Error:
  ds1103_can_error_handler(error, "ds1103_can_subint_request");
  return (error);
}


/******************************************************************************
*
* FUNCTION
*   Performs the reading of the TX-queue level.
*
* DESCRIPTION
*   Reads the actual fill level of the TX-queue and returns the value.  
*
* PARAMETERS
*
*   canMsg        : pointer to the CAN message object
*
* RETURNS
*   Fill level of TX-queue in number of messages
*
* REMARKS
*
******************************************************************************/

__INLINE UInt32 ds1103_can_msg_txqueue_level_read (const ds1103_canMsg* canMsg)
{
  UInt16 level;
  UInt32 format;

  format = !!canMsg->format;

  do  
  { 
    level = *((volatile UInt16*) (DEF1103_CAN_TXQUEUE_INFO_ADDR + (format << 1)));
  }
  while (level != *((volatile UInt16*) (DEF1103_CAN_TXQUEUE_INFO_ADDR + (format << 1))));

  return (UInt32) level;
}


/******************************************************************************
*  not exported functions !
*
******************************************************************************/

__INLINE Int32 ds1103_can_msg_update_all(const UInt32 queue )
{
    UInt32 tmpData[ DEF1103_CAN_MSG_STATUS_READ_DINCNT ];
    int index;
    ds1103_canMsg* newMsg;
    Int32 error;
    rtlib_int_status_t int_status;

    do
    {
      RTLIB_INT_SAVE_AND_DISABLE(int_status);
      
      error = dsmcom_get_entry_varlen( ds1103_can_task_list,
                                        queue, DEF1103_CAN_MSG_STATUS_READ_DINCNT, 
                                        (unsigned long*)tmpData, &index ); 
       if(DSMCOM_NO_ERROR == error)
       {
          newMsg = (ds1103_can_msg_map[queue])[index];

          if( NULL != newMsg )
          {
            if (NULL != newMsg->pDsCanMsgCb)
            {
              newMsg->pDsCanMsgCb(newMsg, tmpData);;
            }
            else if((UInt32)DS1103_CAN_MSG_DSMCR_NOT_INITIALIZED == newMsg->dsmcr_buffer)
            {
              if( DSLIST_ERROR == dslist_queue_write( newMsg->msgQueue, tmpData ) )
              {
                newMsg->error = DS1103_CAN_DATA_LOST;
              }       
            }
            else
            {
              UInt32 i;
            
              newMsg->error = dsmcr_write(newMsg->dsmcr_buffer, (void*)tmpData);
              for(i=0; i<newMsg->no_of_clients; i++)
              {
                newMsg->clientMsgs[i]->error = dsmcr_client_last_write_state(newMsg->dsmcr_buffer,
                                               newMsg->clientMsgs[i]->clientNumber);
              }
            } 
          }
       }
 
       RTLIB_INT_RESTORE(int_status);

    }while(DSMCOM_NO_ERROR == error);

    return DS1103_CAN_NO_ERROR;
}


#ifndef _INLINE

Int32 ds1103_can_check_timing(const UInt32 type,
                             const Float32 start_time,
                             const Float32 repetition_time,
                                   Float32* timeout )
{
  Float32 frame_time;
   UInt32 baudrate;
 
  baudrate = ds1103_can_calculate_baudrate(ds1103_canCh->btr0,
                                           ds1103_canCh->btr1,
                                           ds1103_canCh->frequency);
  /* 136 bits in a CAN EXT FRAME */
  frame_time = 136 / (Float32) baudrate;

  if(*timeout == DS1103_CAN_TIMEOUT_NORMAL)
  {
    switch(type)
    {
      case DS1103_CAN_TX:
        *timeout = DS1103_CAN_TIMEOUT_NORM_MPY * frame_time;   
      break;
        case DS1103_CAN_RQTX:
        *timeout = 5 * DS1103_CAN_TIMEOUT_NORM_MPY * frame_time;   
      break;
    }
  } 
 
  /* check the timeout */
  if( frame_time * DS1103_CAN_TIMEOUT_MIN > (*timeout) )
  {
    return DS1103_CAN_TIMEOUT_LOW_ERROR; 
  }
 
  if( *timeout > DS1103_CAN_MAX_TIMEOUT )
  {
    return DS1103_CAN_TIMEOUT_HIGH_ERROR; 
  }
 
  /* check the starttime */
  if( start_time > DS1103_CAN_MAX_STARTTIME )
  {
    return DS1103_CAN_STARTTIME_ERROR;
  }

  /* check the repetition time */
  if( repetition_time > DS1103_CAN_MAX_REP_TIME )
  {
    return DS1103_CAN_REP_TIME_HIGH_ERROR;
  }

  if( frame_time * DS1103_CAN_MIN_REP_TIME > repetition_time
      && repetition_time != DS1103_CAN_TRIGGER_MSG )
  {
    return DS1103_CAN_REP_TIME_LOW_ERROR;
  }
 
  return DS1103_CAN_NO_ERROR;
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   void ds1103_can_callback_dummy_fcn( const Int32 subint_no ) 
*
* DESCRIPTION
*   The dummy callback function is installed in the communication init.
*
* PARAMETERS:
*   subint_no               Number of the corresponding sub-interrupt
*
* RETURNS: void
*
******************************************************************************/

#ifndef _INLINE

void ds1103_can_callback_dummy_fcn( void* subint_data, Int32 subint )
{ 

}


#endif /* _INLINE */

/******************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_check_subint(const Int32 subinterrupt)
{
  /* check, if subinterrupt is legal */
  if (!(subinterrupt < DEF1103_CAN_SUBINT_BUFFERWARN) &&
      (subinterrupt >= DS1103_CAN_NO_SUBINT))
  {
    return DS1103_CAN_SUBINT_OUT_OF_RANGE;
  }
  
  /* check, if subinterrupt == busoff_intno */
  if ((DS1103_CAN_BUSOFF_INT == ds1103_can_subints[subinterrupt]) && 
      (subinterrupt != DS1103_CAN_NO_SUBINT) )
  {
    return DS1103_CAN_ILLEGAL_INT_NO;
  }

  return DS1103_CAN_NO_ERROR;
}

#endif

/******************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_check_format(const UInt32 format)
{
  if( (format != DS1103_CAN_STD) && (format != DS1103_CAN_EXT) ) 
  {
    return DS1103_CAN_ILLEGAL_FORMAT;
  }
 
  return DS1103_CAN_NO_ERROR;
}

#endif

/******************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_check_all_id (void)
{
  UInt32 queue, index, error;
  ds1103_canMsg* msg;
 
  /* search a tx message */
  for ( queue = 0; queue < DEF1103_CAN_BUFFER_NUM ; queue++)
  {
    if(NULL != ds1103_can_msg_map[queue])
    {
      for ( index = 1; index < ds1103_can_max_idx[queue] ; index++)
      {
          msg = (ds1103_can_msg_map[queue])[index];

          if (msg != NULL)
          {
            /* is the message not a rx message and on the same channel
               and from the same format */
              if ( (msg->type   != DS1103_CAN_RX             ) &&
                 (msg->format != ds1103_canCh->mb15_format ) )
              {
                error = ds1103_can_check_txrm_id (msg);

                if ( error > DS1103_CAN_NO_ERROR )
                {
                  return error;
                }
              }
          }
      }
    }
 }
 return DS1103_CAN_NO_ERROR; 
}

#endif

/******************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_check_txrm_id (const ds1103_canMsg* canMsg)
{
  UInt32 id, no, value;

  for (no = 0; no < ds1103_can_rm_msg_no; no++ )
  {
    id = (UInt32) ds1103_can_rm_id_list[no];  

    value = ( (id ^ (canMsg->identifier | (canMsg->format << 31) )) &
              ds1103_can_glob_mask);

    if ( (value == 0) && (id != (canMsg->identifier | (canMsg->format << 31) ) ) )
    {
      return DS1103_CAN_ILLEGAL_ID; 
    }
  }
  return DS1103_CAN_NO_ERROR; 
}

#endif

/******************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_check_ident (const ds1103_canMsg* canMsg )
{
  Int32 error = DS1103_CAN_NO_ERROR;
 
  if( canMsg->format == DS1103_CAN_STD)
  {
    if ( canMsg->identifier & ~0x7FF )
    {
      error = DS1103_CAN_ILLEGAL_ID; 
    }
  }
  else
  {
    if ( canMsg->identifier & ~0x1FFFFFFF )
    {
      error = DS1103_CAN_ILLEGAL_ID; 
    }
  }

  if (error)
  {
    msg_info_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_ID,
                     DS1103_CAN_ILLEGAL_ID_INFO_TXT);   
 
    return error;
  }
 
  if ( ds1103_canCh->mb15_format != canMsg->format )
  {
    switch(canMsg->type)
    {
      case DS1103_CAN_RX:
        if (ds1103_can_glob_no)
        {
          ds1103_can_glob_mask &= (~(ds1103_can_glob_id ^ canMsg->identifier));
          ds1103_can_glob_id   &= canMsg->identifier;
        }
        else
        {
          ds1103_can_glob_no = 1;
          ds1103_can_glob_id = canMsg->identifier;   
        }

        error = ds1103_can_check_all_id ();
        break;
    
      case DS1103_CAN_RM:
        error = ds1103_can_check_txrm_id (canMsg );
        break;

    case DS1103_CAN_RQTX:
        error = ds1103_can_check_txrm_id (canMsg);
        break;

      case DS1103_CAN_TX:
        error = ds1103_can_check_txrm_id (canMsg);
        break;
    }
  }  

  if ( error != DS1103_CAN_NO_ERROR)
  {
    msg_info_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_ID,
                     DS1103_CAN_CONFLICT_ID_INFO_TXT);   
  }

  return error;
}

#endif /* _INLINE */

/******************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_check_version (const UInt32 fw_version)
{
  UInt32 ds1103_can_fw_version_expected = (REQ_DS1103_CAN_FIRMWARE_VER_MIR <<  7) |
                                          (REQ_DS1103_CAN_FIRMWARE_VER_MAR << 11);

  /* check dSPACE firmware */
  if ( (fw_version & DS1103_CAN_FIRMWARE_ORIGIN) &&
       (fw_version < ds1103_can_fw_version_expected) )
  {
    return DS1103_CAN_SLAVE_FIRMWARE_WRONG;
  }

  return DS1103_CAN_NO_ERROR;
}

#endif /* _INLINE */

/******************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_register (ds1103_canMsg* canMsg,
                                  Int32* index,
                           const UInt32 op_code,
                           const UInt32 doutcnt,
                           const UInt32 reg_pcnt,
                           const UInt32* parms )
{
  UInt32 dincount = 0;
   Int32 error;

  if( canMsg->msg_no >= DS1103_CAN_MAX_NO_OF_MESSAGES )
  {
    error = DS1103_CAN_TOO_MUCH_MSG; goto Error;
  }
 
  if ( canMsg->inform & DS1103_CAN_DATA_INFO )
  {
    dincount += 5;  
  }

  if( canMsg->inform & DS1103_CAN_MSG_INFO)
  {
    dincount += 3;  
  } 

  if( canMsg->inform & DS1103_CAN_PROCESSED_INFO)
  {
    dincount += 1;
  }

  if ( canMsg->inform & DS1103_CAN_TIMECOUNT_INFO )
  {
    dincount += 3;
  }
  if ( canMsg->inform & DS1103_CAN_DELAYCOUNT_INFO )
  {
    dincount += 2;
  }

  (ds1103_can_max_idx[canMsg->queue])++;

  DS1103_CAN_TRY (dsmcom_register(ds1103_can_task_list, canMsg->queue,
                                  index, op_code, dincount, doutcnt,
                                  reg_pcnt, (unsigned long*) parms));
  
  /* request execution of msg_register */
  DS1103_CAN_TRY (dsmcom_execute(ds1103_can_task_list, canMsg->queue, 
                                 (*index)));

  ds1103_can_msg_map[canMsg->queue] =
  (ds1103_canMsg**) realloc (ds1103_can_msg_map[canMsg->queue],
                            (ds1103_can_max_idx[canMsg->queue]+1)*sizeof(ds1103_canMsg*));

  if( ds1103_can_msg_map[canMsg->queue]==NULL )
  {
    error = DS1103_CAN_ALLOC_ERROR; goto Error;
  }

  (ds1103_can_msg_map[canMsg->queue])[*index] = (ds1103_canMsg*)canMsg;

  canMsg->error = DS1103_CAN_NO_ERROR;
  canMsg->msgQueue = dslist_queue_init ( DSLIST_OVERRUN, 1, dincount * 4 );  

 /* init. dsmcr buffer handle */
  canMsg->dsmcr_buffer = (UInt32) DS1103_CAN_MSG_DSMCR_NOT_INITIALIZED;

  return DS1103_CAN_NO_ERROR;

Error:
  return (error);
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION:
*
* SYNTAX:
*   Int16 ds1103_can_clock_calc( Float32 *freq, UInt8 *vco_div,
*                                UInt8 *ref_div)
*
* DESCRIPTION: 
*   This function calculates the exactly frequency of the CAN controller
*   clock and the values for PLL circuit.
*
* PARAMETERS: 
*   Float32* freq: -> desired frequency
*                  <- calculated frequency
*   UInt8* vco_div: calculated vco-divisor
*   UInt8* ref_div: calculated reference-divisor
*
* RETURNS: 
*   errorcode
*
* NOTE: 
*   This function is used by ds1103_can_channel_init and
*   ds1103_can_channel_init_advanced.
*
*   OBSOLETE: use ds1103_av9110_clock_calc_can
*
******************************************************************************/
#ifndef _INLINE

Int16 ds1103_can_clock_calc( Float32 *freq, UInt8 *vco_div, UInt8 *ref_div)
{
       return(ds1103_av9110_clock_calc_can(*freq, freq,
                         vco_div, ref_div));
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION:
*
* SYNTAX:
*   void ds1103_can_clock_write(UInt8 vco_div, UInt8 ref_div)
*
*
* DESCRIPTION: 
*   This function writes the parameters to the PLL circuit.
*
* PARAMETERS: 
*   UInt8 vco_div: vco-divisor
*   UInt8 ref_div: reference-divisor
*
* RETURNS: 
*   void
*
* NOTE: 
*   This function is used by ds1103_can_channel_init and
*   ds1103_can_channel_init_advanced. 
*
*   OBSOLETE: use ds1103_av9110_clock_write_can
*
******************************************************************************/
#ifndef _INLINE

void ds1103_can_clock_write(UInt8 vco_div, UInt8 ref_div)
{
ds1103_av9110_clock_write_can(vco_div,ref_div);
return;
}
 
#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Converts time in seconds to CAN controller timecount.
*
* SYNTAX
*   UInt32 ds1103_can_time2count (const Float32 time)
*
* DESCRIPTION
*   This function converts the time in seconds to timecount.
*
* PARAMETERS
*   time         time in secounds
*
* RETURNS
*   timecount
*
* REMARKS
*
******************************************************************************/
__INLINE UInt32 ds1103_can_time2count (const Float32 time)
{
  return ((UInt32)(time / (Float32) ds1103_can_timerticks));
}

/******************************************************************************
*
* FUNCTION
*   Error handling function.
*
* SYNTAX
*   void ds1103_can_error_handler (const Int32 error)
*
* DESCRIPTION
*   This function contains the error-handling for errors.
*
* PARAMETERS
*   error       error number
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

void ds1103_can_error_handler (const Int32 error, const char* function)
{
  char* pfunction = NULL;
  rtlib_int_status_t  int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  pfunction = (char*)malloc(MSG_STRING_LENGTH + 128);

  if(NULL == pfunction)
  {
    msg_error_printf(MSG_SM_DS1103MCLIB, DSMCOM_ALLOC_ERROR,
                     "ds1103_can_error_handler: Memory allocation failed.");
    exit(1);
  }

  strcpy(pfunction, function);

  switch (error)
  {
    case DSMCOM_NO_ERROR:
      break;
    case DSMCOM_ALLOC_ERROR:
      strcat(pfunction, DS1103_CAN_ALLOC_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ALLOC_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_ALLOC_ERROR:
      strcat(pfunction, DS1103_CAN_ALLOC_ERROR_TXT);
       msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ALLOC_ERROR, pfunction);
       free(pfunction);
       exit(1);
    case DSMCOM_ILLEGAL_TASK_ID:
      strcat(pfunction, DS1103_CAN_ILLEGAL_QUEUE_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_QUEUE, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_ILLEGAL_QUEUE:
      strcat(pfunction, DS1103_CAN_ILLEGAL_QUEUE_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_QUEUE, pfunction);
      free(pfunction);
      exit(1);
    case DSMCOM_ILLEGAL_INDEX:
      strcat(pfunction, DS1103_CAN_ILLEGAL_INDEX_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_INDEX, pfunction);
      free(pfunction);
      exit(1);
    case DSMCOM_BUFFER_OVERFLOW :
      strcat(pfunction, DS1103_CAN_REG_BUFFER_OVERFLOW_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_REG_BUFFER_OVERFLOW, pfunction);
      free(pfunction);
      exit(1);
    case DSMCOM_TIMEOUT:
      strcat(pfunction, DS1103_CAN_TIMEOUT_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_TIMEOUT, pfunction);
      free(pfunction);
      exit(1);
    case DSMCOM_SLV_ALLOC_ERROR:
      strcat(pfunction, DS1103_CAN_REG_SLAVE_ALLOC_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_REG_SLAVE_ALLOC_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DSMCOM_SLV_BUFFER_OVERFLOW :
      strcat(pfunction, DS1103_CAN_REG_SLAVE_BUFFER_OVERFLOW_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_REG_SLAVE_BUFFER_OVERFLOW, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_SLAVE_FIRMWARE_WRONG:
      strcat(pfunction, DS1103_CAN_SLAVE_FIRMWARE_WRONG_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_SLAVE_FIRMWARE_WRONG, pfunction,
      REQ_DS1103_CAN_FIRMWARE_VER_MAR, REQ_DS1103_CAN_FIRMWARE_VER_MIR);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_SLAVE_FIRMWARE_WRONG, DS1103_CAN_SLAVE_FIRMWARE_WRONG2_TXT);
      free(pfunction);
      exit(1);
    case DS1103_CAN_TIMEOUT_OR_WRONG_FIRMWARE:
      strcat(pfunction, DS1103_CAN_TIMEOUT_OR_WRONG_FIRMWARE_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_TIMEOUT_OR_WRONG_FIRMWARE, pfunction);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_SLAVE_FIRMWARE_WRONG, DS1103_CAN_SLAVE_FIRMWARE_WRONG2_TXT);
      free(pfunction);
      exit(1);

   /* ds1103_can_channel_init */
    case DS1103_CAN_BAUDRATE_L_ERROR:
      strcat(pfunction, DS1103_CAN_BAUDRATE_L_ERROR_TXT);
      msg_error_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_BAUDRATE_L_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_BAUDRATE_H_ERROR:
      strcat(pfunction, DS1103_CAN_BAUDRATE_H_ERROR_TXT);
      msg_error_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_BAUDRATE_H_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_ILLEGAL_BAUDRATE:
      strcat(pfunction, DS1103_CAN_ILLEGAL_BAUDRATE_TXT);
      msg_error_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_BAUDRATE, pfunction);
      free(pfunction);
      exit(1);

   /* ds1103_can_channel_init_advanced */
    case DS1103_CAN_MIN_OSCCLOCK_ERROR:
      strcat(pfunction, DS1103_CAN_MIN_OSCCLOCK_ERROR_TXT);
      msg_error_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_MIN_OSCCLOCK_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_MAX_OSCCLOCK_ERROR:
      strcat(pfunction, DS1103_CAN_MAX_OSCCLOCK_ERROR_TXT);
      msg_error_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_MAX_OSCCLOCK_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_PLLFREQ_ERROR:
      strcat(pfunction, DS1103_CAN_PLLFREQ_ERROR_TXT);
      msg_error_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_PLLFREQ_ERROR, pfunction);
      free(pfunction);
      exit(1);

    /* ds1103_can_msg_rm_register */
    case DS1103_CAN_NO_RM_MAILBOX_FREE:
      strcat(pfunction, DS1103_CAN_NO_RM_MAILBOX_FREE_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_NO_RM_MAILBOX_FREE, pfunction);
      free(pfunction);
      exit(1);

    /* ds1103_can_msg_rqrx_register */
    case DS1103_CAN_RQTX_NOT_REG:
      strcat(pfunction, DS1103_CAN_RQTX_NOT_REG_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_RQTX_NOT_REG, pfunction);
      free(pfunction);
      exit(1);

    /* ds1103_check_timing */
    case DS1103_CAN_STARTTIME_ERROR:
      strcat(pfunction, DS1103_CAN_STARTTIME_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_STARTTIME_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_REP_TIME_HIGH_ERROR:
      strcat(pfunction, DS1103_CAN_REP_TIME_HIGH_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_REP_TIME_HIGH_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_REP_TIME_LOW_ERROR:
      strcat(pfunction, DS1103_CAN_REP_TIME_LOW_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_REP_TIME_LOW_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_TIMEOUT_HIGH_ERROR:
      strcat(pfunction, DS1103_CAN_TIMEOUT_HIGH_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_TIMEOUT_HIGH_ERROR, pfunction);
      free(pfunction);
      exit(1);
    case DS1103_CAN_TIMEOUT_LOW_ERROR:
      strcat(pfunction, DS1103_CAN_TIMEOUT_LOW_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_TIMEOUT_LOW_ERROR, pfunction);
      free(pfunction);
      exit(1);

    /* ds1103_check_subint */
    case DS1103_CAN_SUBINT_OUT_OF_RANGE:
      strcat(pfunction, DS1103_CAN_SUBINT_OUT_OF_RANGE_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_SUBINT_OUT_OF_RANGE, pfunction);
      free(pfunction);
      exit(1);
    case  DS1103_CAN_ILLEGAL_INT_NO:
      strcat(pfunction, DS1103_CAN_ILLEGAL_INT_NO_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_INT_NO, pfunction);
      free(pfunction);
      exit(1);

    /* ds1103_check_format */
    case DS1103_CAN_ILLEGAL_FORMAT:
      strcat(pfunction, DS1103_CAN_ILLEGAL_FORMAT_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_FORMAT, pfunction);
      free(pfunction);
      exit(1);

     /* ds1103_check_id */
    case DS1103_CAN_ILLEGAL_ID:
      strcat(pfunction, DS1103_CAN_ILLEGAL_ID_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_ILLEGAL_ID, pfunction);
      free(pfunction);
      exit(1);

     /* ds1103_can_register */
    case DS1103_CAN_TOO_MUCH_MSG:
      strcat(pfunction, DS1103_CAN_TOO_MUCH_MSG_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_TOO_MUCH_MSG, pfunction);
      free(pfunction);
      exit(1);

    case DS1103_CAN_CHANNEL_NOT_INIT_ERROR:
      strcat(pfunction, DS1103_CAN_CHANNEL_NOT_INIT_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_CHANNEL_NOT_INIT_ERROR, pfunction);
      free(pfunction);
      exit(1);

    /* ds1103_can_msg_txqueue_init */
    case DS1103_CAN_TXQUEUE_INIT_MSG_NOT_REG_ERROR:
      strcat(pfunction, DS1103_CAN_TXQUEUE_INIT_MSG_NOT_REG_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_TXQUEUE_INIT_MSG_NOT_REG_ERROR, pfunction);
      break;

    /* ds1103_can_msg_txqueue_init */
    case DS1103_CAN_TXQUEUE_INIT_MSG_TYPE_ERROR:
      strcat(pfunction, DS1103_CAN_TXQUEUE_INIT_MSG_TYPE_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_TXQUEUE_INIT_MSG_TYPE_ERROR, pfunction);
      break;

    /* ds1103_can_msg_send_id_queued */
    case DS1103_CAN_SEND_ID_QUEUED_INIT_ERROR:
      strcat(pfunction, DS1103_CAN_SEND_ID_QUEUED_INIT_ERROR_TXT);
      msg_error_printf (MSG_SM_DS1103MCLIB, DS1103_CAN_SEND_ID_QUEUED_INIT_ERROR, pfunction);
      break;

    default:
      strcat(pfunction, ": Undefined Error");
      msg_error_printf(MSG_SM_DS1103MCLIB, error, pfunction);
      free(pfunction);
      exit(1);
      break;
  }

  free(pfunction);
  RTLIB_INT_RESTORE(int_status);
  return;

}
#endif /* _INLINE */

/******************************************************************************/
#ifndef _INLINE

UInt32 ds1103_can_calculate_baudrate(const  UInt32 BTR0,
                                     const  UInt32 BTR1,
                                     const  UInt32 clock)
{
  UInt32 TSEG1, TSEG2, brp, dsc;

  TSEG1 =  0x0F & BTR1;
  TSEG2 = (0x70 & BTR1) >> 4;

  brp   =  0x3F & BTR0;

  if (clock > 10000000) { 
    dsc = 1;
  }
  else if (clock > 8000000) { 
    dsc = 0;
  }
  else { 
    dsc = 0;    
  }
 
  return (clock /( (dsc+1) * (brp+1) * (3+TSEG1+TSEG2) ) );
}

#endif /* _INLINE */

/****************************************************************************
* FUNCTION:
*   Calculation of bit-timing parameters clock.
*
* SYNTAX:  
*   Int32 ds1103_can_baudrate_calc (Int32 baudrate, UInt32 *BTR0,
*                                   UInt32 *BTR1, UInt32 *clock)
*
* DESCRIPTION:   
*   This function calculates the bit-timing parameters and the
*   required CAN controller clock for the desired baudrate.
*
*
* Parameter:
*    baudrate:       The desired CAN baudrate
*    BTR0:           The calculated value for the bit-timing0 register
*    BTR1:           The calculated value for the bit-timing1 register
*    clock:          The calculated C164 clock frequency.
*                    Should be close to 20MHz.
*  Returns:
*    DS1103_CAN_NO_ERROR:    If the bit-timing for the desired baudrate could be
*                            calculated.
*
*    DS1103_CAN_ILLEGAL_BAUDRATE:
*                            If the function failed to find a solution for the
*                            desired baudrate.
*
* NOTE: 
*    This function is used by ds1103_can_channel_init.
*
*****************************************************************************/
#ifndef _INLINE

Int32 ds1103_can_baudrate_calculate_inv (const Int32 baudrate,
                                               UInt32 *BTR0,
                                               UInt32 *BTR1,
                                               UInt32 *clock)
{
  UInt32 ctrlCfgBTR0 = 0;
  UInt32 ctrlCfgBTR1 = 0;

  Float64 f_EXTCLK_min = DS1103_CAN_MINCLOCK;
  Float64 f_EXTCLK_max = DS1103_CAN_MAXCLOCK;
  Float64 f_EXTCLK;

  Float64 k_EXTCLK_SCLK = 2;

  Float64 f_SCLK;
  Float64 f_SCLK_estim;

  Float64  r_TSEG1_min;
  Float64  r_TSEG1_max;
  Float64  r_TSEG1;
  Float64  r_TSEG2_min;
  Float64  r_TSEG2_max;
  Float64  r_TSEG2;
  Float64  n_TSEG1_min;
  Float64  n_TSEG1_max;
  Float64  n_TSEG1;

  UInt32  n_TSEG2_min;
  UInt32  n_TSEG2_max;
  UInt32  n_TSEG2;

  Float64  n_TSEG_1_2;

  Float64  r_SJW_min;
  Float64  r_SJW_max;
  UInt32   r_SJW;

  UInt32   n_SJW_min;
  UInt32   n_SJW_max;
  UInt32   n_SJW;

  Float64  r_BRP_min;
  Float64  r_BRP_max;
  Float64  r_BRP;

  Float64  n_BRP_min;
  Float64  n_BRP_max;
  Float64  n_BRP = 0;

  Float64  n_NBT_min;
  Float64  n_NBT_max;
  Float64  n_NBT;

  Float64  n_SYNC = 1;
  Float64  n_MUL_min;
  Float64  n_MUL_max;
  Float64  n_MUL;
  Float64  n_MUL_p;

  Float64  f_EXTCLK_estim;

  Float64  r_SMPL = 0;  

  UInt32* cfgSJW;
  UInt32* cfgSP; 
  Float64* cfgBTR0;
  Float64* cfgBTR1;


  UInt32 idx=0;
  Int32 selectionMatrix[5][5] = { {-1, -1, -1, -1, -1}, { -1, 1, 1, 1, 1}, { -1, 2, 1, 1, -1},
                                  {-1, 2, 2, -1, -1}, {-1, 3, -1, -1, -1} };

  UInt32 minSJWvalue;
  UInt32 maxSPvalue;
  UInt16 i;
  UInt32 numMaxSP;
  UInt32 defCombIndex;

  UInt16 solutionFound = 0;

  rtlib_int_status_t  int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  cfgSJW = (UInt32*)malloc(60*sizeof(UInt32));
  if(cfgSJW == NULL)
  {
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_ALLOC_ERROR);
  }

  cfgSP = (UInt32*)malloc(60*sizeof(UInt32));
  if(cfgSP == NULL)
  {
    free(cfgSJW);
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_ALLOC_ERROR);
  }

  cfgBTR0 = (Float64*)malloc(60*sizeof(Float64));
  if(cfgBTR0 == NULL)
  {
    free(cfgSJW);
    free(cfgSP);
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_ALLOC_ERROR);
  }

  cfgBTR1 = (Float64*)malloc(60*sizeof(Float64));
  if(cfgBTR1 == NULL)
  {
    free(cfgSJW);
    free(cfgSP);
    free(cfgBTR0);
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_ALLOC_ERROR);
  }

  RTLIB_INT_RESTORE(int_status);


  r_TSEG1_min = 2;
  r_TSEG1_max = 15;
  n_TSEG1_min = r_TSEG1_min + 1;
  n_TSEG1_max = r_TSEG1_max + 1;

  r_TSEG2_min = 1;
  r_TSEG2_max = 7;
  n_TSEG2_min = (UInt32) (r_TSEG2_min + 1);
  n_TSEG2_max = (UInt32) (r_TSEG2_max + 1);

  r_SJW_min = 0;
  r_SJW_max = 3;
  n_SJW_min =  (UInt32) (r_SJW_min + 1);
  n_SJW_max =  (UInt32) (r_SJW_max + 1);

  r_BRP_min = 0;
  r_BRP_max = 63;
  n_BRP_min = r_BRP_min + 1;
  n_BRP_max = r_BRP_max + 1;

  n_NBT_min = n_TSEG1_min + n_TSEG2_min + n_SYNC;
  n_NBT_max = n_TSEG1_max + n_TSEG2_max + n_SYNC;

  n_MUL_min = n_NBT_min * n_BRP_min;
  n_MUL_max = n_NBT_max * n_BRP_max;

  f_EXTCLK_estim = k_EXTCLK_SCLK * n_MUL_max * baudrate;
  if( f_EXTCLK_estim > f_EXTCLK_max )
    f_EXTCLK_estim = f_EXTCLK_max;
  
  f_SCLK_estim = f_EXTCLK_estim / k_EXTCLK_SCLK;
  n_MUL = f_SCLK_estim / baudrate;  
  
  if( n_MUL < n_MUL_min )
    n_MUL = n_MUL_min;
  
  n_MUL_p = ceil(n_MUL);  

  n_NBT = n_MUL_p / n_BRP_min;

  if( (n_NBT >= n_NBT_min) && (n_NBT <= n_NBT_max) )
    n_BRP = n_BRP_min;
  else
  {
    solutionFound = 0;
    while( solutionFound == 0 )
    {
      n_NBT = n_NBT_max;
      
      while( (fmod(n_MUL_p, n_NBT) != 0) && (n_NBT > n_NBT_min) ) 
      {
        n_NBT = n_NBT - 1;
      }; 

      if( fmod(n_MUL_p, n_NBT) == 0 )
      {
        n_BRP = n_MUL_p / n_NBT;
        if( n_BRP <= n_BRP_max )
          solutionFound = 1;
        else
          n_MUL_p = n_MUL_p + 1;
      }
      else
        n_MUL_p = n_MUL_p + 1;
    } /*while*/
  } /*if*/

  f_SCLK   = ceil((n_MUL / n_MUL_p) * f_SCLK_estim);
  f_EXTCLK = k_EXTCLK_SCLK * f_SCLK;

  if( (f_EXTCLK > f_EXTCLK_max) || (f_EXTCLK < f_EXTCLK_min) )
  {
    free(cfgSJW);
    free(cfgSP); 
    free(cfgBTR0);
    free(cfgBTR1);
    return DS1103_CAN_ILLEGAL_BAUDRATE;
  };

  *clock = (UInt32)f_EXTCLK;

  n_TSEG_1_2 = n_NBT - n_SYNC; 

  for(n_TSEG2 = n_TSEG2_min; n_TSEG2 <= n_TSEG2_max; n_TSEG2++)
  {
    n_TSEG1 = n_TSEG_1_2 - n_TSEG2;
    if( (n_TSEG1 <= n_TSEG1_max) && (n_TSEG1 >= n_TSEG1_min) )
    {
      for( n_SJW = n_SJW_min; n_SJW <= n_SJW_max; n_SJW++ )
      {
        if( (n_TSEG2 >= n_SJW) && (n_TSEG1 > n_SJW) )
        {
          {
            idx++;
            r_BRP = n_BRP - 1;
            r_SJW = n_SJW - 1;
            r_TSEG1 = n_TSEG1 - 1;
            r_TSEG2 = n_TSEG2 - 1;
            cfgSJW[idx]   = r_SJW;
            cfgSP[idx]    = (UInt32)ceil(100*(n_SYNC + n_TSEG1)/n_NBT);
            cfgBTR0[idx]  = ( r_SJW * pow(2,6) ) + r_BRP;
            cfgBTR1[idx]  = ( r_SMPL * pow(2,7) ) + (r_TSEG2 * pow(2,4) ) + r_TSEG1;
          }
        } /*if*/
      } /*for*/
    } /*if*/
  } /*for*/

  if(idx >= 1) 
  {
    minSJWvalue = cfgSJW[1];
    maxSPvalue = cfgSP[1];
    numMaxSP = 1;
    for(i=2; i<=idx; i++)
    {
      if(cfgSP[i] != maxSPvalue)
      {
        numMaxSP = i - 1;
        break;
      }
    }

    defCombIndex = selectionMatrix[numMaxSP][minSJWvalue + 1];
    ctrlCfgBTR0  = (UInt32)cfgBTR0[defCombIndex];
    ctrlCfgBTR1  = (UInt32)cfgBTR1[defCombIndex];
  } /*if*/

  *BTR0 = (UInt32)ctrlCfgBTR0;
  *BTR1 = (UInt32)ctrlCfgBTR1;

  RTLIB_FREE_PROT(cfgSJW);
  RTLIB_FREE_PROT(cfgSP);
  RTLIB_FREE_PROT(cfgBTR0);
  RTLIB_FREE_PROT(cfgBTR1);
  
  return DS1103_CAN_NO_ERROR;
}

#endif


/*****************************************************************************/

#ifndef _INLINE
  
void ds1103_can_print_version(UInt32* fw_rev)
{

  char fw_version[2] = { '\0', '\0' };


  if (fw_rev[0] & (DS1103_CAN_FIRMWARE_MAINT | DS1103_CAN_FIRMWARE_VERSION))
  {
    switch (fw_rev[0] & DS1103_CAN_FIRMWARE_VERSION)
    {
      case DS1103_CAN_FIRMWARE_ALPHA:
        fw_version[0] = 'a';
        break;

      case DS1103_CAN_FIRMWARE_BETA:
        fw_version[0] = 'b';
 				break;

      default:
        fw_version[0] = '\0';
    }

    if (fw_rev[0] & DS1103_CAN_FIRMWARE_ORIGIN)
    {
      msg_info_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_VERSION_INFO,
        "dSPACE firmware rev. %d.%d.%s%d detected.",
        (fw_rev[0] & DS1103_CAN_FIRMWARE_MAJOR) >> 11,
        (fw_rev[0] & DS1103_CAN_FIRMWARE_MINOR) >> 7,
         fw_version,
        (fw_rev[0] & DS1103_CAN_FIRMWARE_MAINT) >> 3);
    }
    else
    {
       
          msg_info_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_VERSION_INFO,
          "User firmware %d.%d.%d based on dSPACE firmware rev. %d.%d.%s%d detected.",
            fw_rev[1], fw_rev[2], fw_rev[3],
           (fw_rev[0] & DS1103_CAN_FIRMWARE_MAJOR) >> 11,
           (fw_rev[0] & DS1103_CAN_FIRMWARE_MINOR) >> 7,
            fw_version,
           (fw_rev[0] & DS1103_CAN_FIRMWARE_MAINT) >> 3);
    }

  }
  else
  {
    if (fw_rev[0] & DS1103_CAN_FIRMWARE_ORIGIN)
    {
      msg_info_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_VERSION_INFO,
        "dSPACE firmware rev. %d.%d detected.",
        (fw_rev[0] & DS1103_CAN_FIRMWARE_MAJOR) >> 11,
        (fw_rev[0] & DS1103_CAN_FIRMWARE_MINOR) >> 7);

    }
    else
    {

       msg_info_printf(MSG_SM_DS1103MCLIB, DS1103_CAN_VERSION_INFO,
         "User firmware %d.%d.%d based on dSPACE firmware rev. %d.%d detected.",
           fw_rev[1], fw_rev[2], fw_rev[3],
          (fw_rev[0] & DS1103_CAN_FIRMWARE_MAJOR) >> 11,
          (fw_rev[0] & DS1103_CAN_FIRMWARE_MINOR) >> 7);
    }
  }

}

#endif


/*****************************************************************************/
__INLINE void ds1103_can_unpack_block_16in8 ( UInt32 *packed_data,
                                              UInt32 *unpacked_data,
                                              const UInt32 size )
{
  UInt32 x;
  UInt32 len;

  len = (UInt32) ds1103_can_len_arr[size];

  switch( len ) 
  {
    case 0 :

       for(x = 0; x < 8; x++)
       {
         unpacked_data[ x ] = 0;
       }
      
       break;
    
    case 1 :

        packed_data[0] = packed_data[0] & (0xFFFF >> (8 * (2 - size)));
        unpacked_data[0] = ( packed_data[0] & 0x000000FF );
        unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;

        for(x = 2; x < 8; x++)
        {
          unpacked_data[ x ] = 0;
        }

        break;

    case 2 :

        packed_data[1] = packed_data[1] & (0xFFFF >> (8 * (4 - size)));
        unpacked_data[0] = ( packed_data[0] & 0x000000FF );
        unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
        unpacked_data[2] = ( packed_data[1] & 0x000000FF );
        unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;

        for(x = 4; x < 8; x++)
        {
          unpacked_data[ x ] = 0;
        }

        break;

    case 3 :

        packed_data[2] = packed_data[2] & (0xFFFF >> (8 * (6 - size)));
        unpacked_data[0] = ( packed_data[0] & 0x000000FF );
        unpacked_data[1] = ( packed_data[0] & 0x0000FF00 ) >> 8;
        unpacked_data[2] = ( packed_data[1] & 0x000000FF );
        unpacked_data[3] = ( packed_data[1] & 0x0000FF00 ) >> 8;
        unpacked_data[4] = ( packed_data[2] & 0x000000FF );
        unpacked_data[5] = ( packed_data[2] & 0x0000FF00 ) >> 8;
        unpacked_data[6] = 0;
        unpacked_data[7] = 0;

        break;

    case 4 :

        packed_data[3] = packed_data[3] & (0xFFFF >> (8 * (8 - size)));

        for(x = 0; x < len; x++)
        {
          unpacked_data[x * 2]       = ( packed_data[ x ] & 0x000000FF );
          unpacked_data[(x * 2) + 1] = ( packed_data[ x ] & 0x0000FF00 ) >> 8; 
        }

        break;

	default :
       len = 4;
       for(x = 0; x < len; x++)
	     {
        unpacked_data[ x * 2 ]        = ( packed_data[ x ] & 0x000000FF );
        unpacked_data[ (x * 2) + 1 ]  = ( packed_data[ x ] & 0x0000FF00 ) >> 8;
	     }
      
      break;
  }
}

/******************************************************************************/
__INLINE void ds1103_can_pack_block_8in16 (const UInt32 *unpacked_data,
                                                 UInt32 *packed_data,
                                           const UInt32 len )
{
  UInt32 i;
  UInt32 data1, data2;

  for ( i = 0; i < len; i++)
  {
    data1 = *unpacked_data++;
    data1 &= 0x000000ff;

    data2 = *unpacked_data++;
    data2 = data2<<8;
    data2 &= 0x0000ff00;
    data1 |= data2;

    *packed_data++ = data1;
  }
}

/******************************************************************************
*
* FUNCTION
*   Converts timecounts to time in seconds.
*
* FUNCTION
*   Float64 ds1103_can_time_convert (const UInt32 timecount, UInt32 wrapcount)
*
* DESCRIPTION
*   This function converts the timecount to time in seconds.
*
* PARAMETERS
*   timecount     timecount, deltacount or delaycount
*   wrapcount     wrapcount, if 1. parameter = timecount
*                         0, if 1. parameter = deltacount or delaycount
*
* RETURNS
*   time in seconds
*
* REMARKS
*
******************************************************************************/
__INLINE Float64 ds1103_can_time_convert (const UInt32 timecount, UInt32 wrapcount)
{
  return ( ( (Float64)wrapcount  * (Float64)DS1103_CAN_WRAPCOUNT +
             (Float64)timecount) * ds1103_can_timerticks);
}

#undef __INLINE
