/******************************************************************************
*
* MODULE
*   TMS320F240 slave DSP access functions for DS1103 controller board
*
*   Version for PPC master CPU
*
* FILE
*   Slvdsp1103.c
*
* RELATED FILES
*   Slvdsp1103.h, Dsmcom.h, Dscomdef.h, Def1103.h, Ds1103slvlib_msg.h
*
* DESCRIPTION
*   DS1103 slave DSP access functions for:
*   - ADC input
*   - digital I/O
*   - simple PWM generation
*   - 3-phase PWM generation
*   - 3-phase space vector PWM generation
*   - variable frequency square-wave generation
*   - frequency measurement
*   - PWM measurement
*   - SCI communication
*   - SPI communication
*   - user function interface
*
* AUTHOR(S)
*   H.-J. Miks, M. Heier
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Slvdsp1103.c $ $Revision: 1.5 $ $Date: 2007/08/15 08:25:54GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#include <dstypes.h>                      /* platform independent data types */
#include <ds1103.h>                            /* DS1103 address definitions */
#include <cfg1103.h>                    /* configuration section definitions */
#include <dsstd.h>                              /* DS1103 standard utilities */
#include <tmr1103.h>                             /* DS1103 timer definitions */
#include <dsmsg.h>                             /* message module definitions */
#include <dscomdef.h>   /* common definitions for master-slave communication */
#include <def1103.h>           /* definitions for master-slave communication */
#include <dsmcom.h>                        /* master communication functions */
#include <slvdsp1103.h>                               /* function prototypes */
#include <slvdsp1103_msg.h>                      /* error codes and messages */
#include <tic1103.h>             /*DS1103 on-board executiontime-measurement */
#include <dsvcm.h>

#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif


/******************************************************************************
* constant and macro definitions
******************************************************************************/

/* constants for ADC input functions */

#define SLVDSP1103_ADC_READ_SCALE 1.52587891e-5        /* ADC scaling factor */

/* constants for PWM generation functions */

#define SLVDSP1103_CPU_CLK      20.0e6          /* slave DSP clock frequency */

/* constants for D/F frequency generation functions */

#define SLVDSP1103_D2F_CH1_MSK  0x01                   /* D/F channel 1 mask */
#define SLVDSP1103_D2F_CH2_MSK  0x02                   /* D/F channel 2 mask */
#define SLVDSP1103_D2F_CH3_MSK  0x04                   /* D/F channel 3 mask */
#define SLVDSP1103_D2F_CH4_MSK  0x08                   /* D/F channel 4 mask */

/* constants for SCI communication functions */

#define DS1103_SETUP             0x10000088
#define SLVDSP1103_SCI_CLK_SRC   10.0e6           /* UART clock source in Hz */
#define SLVDSP1103_SPI_CLK_SRC   10.0e6


/******************************************************************************
* global objects
******************************************************************************/

extern Float64 master_time;

#ifndef _INLINE

dsmcom_cmdtbl_header_t **slvdsp1103_handle;
UInt16 slvdsp1103_adc_sel_mask = 0xFCFC;           /* ADC pin selection mask */
UInt16 slvdsp1103_pwm_sel_mask = 0;                /* PWM pin selection mask */
Float64 slvdsp1103_pwm_period;                          /* simple PWM period */
UInt16 slvdsp1103_pwm_mode;                               /* simple PWM mode */
Float64 slvdsp1103_pwm3_period;                        /* 3-phase PWM period */
Int32 slvdsp1103_pwm3sv_period_scaled;            /* space vector PWM period */
UInt16 slvdsp1103_pwm3sv_prescale;      /* space vector PWM clock prescaling */
UInt16 slvdsp1103_d2f_sel_mask = 0;                /* D/F pin selection mask */
UInt16 slvdsp1103_d2f_prescale;                      /* D/F clock prescaling */
Float64 slvdsp1103_f2d_fmin[5] =             /* F/D minimum frequency limits */
  { 0.0, 0.0, 0.0, 0.0, 0.0 };

#else

extern dsmcom_cmdtbl_header_t **slvdsp1103_handle;
extern UInt16 slvdsp1103_adc_sel_mask;
extern UInt16 slvdsp1103_pwm_sel_mask;
extern Float64 slvdsp1103_pwm_period;
extern UInt16 slvdsp1103_pwm_mode;
extern Float64 slvdsp1103_pwm3_period;
extern Int32 slvdsp1103_pwm3sv_period_scaled;
extern UInt16 slvdsp1103_pwm3sv_prescale;
extern UInt16 slvdsp1103_d2f_sel_mask;
extern UInt16 slvdsp1103_d2f_prescale;
extern Float64 slvdsp1103_f2d_fmin[];

#endif /* _INLINE */

/******************************************************************************
  version information
******************************************************************************/

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#pragma asm

.equ  _ds1103_slvdsp_software_ver_mar, DS1103_SLVDSP_SOFTWARE_VER_MAR
.equ  _ds1103_slvdsp_software_ver_mir, DS1103_SLVDSP_SOFTWARE_VER_MIR
.equ  _ds1103_slvdsp_software_ver_mai, DS1103_SLVDSP_SOFTWARE_VER_MAI
.equ  _ds1103_slvdsp_software_ver_spb, DS1103_SLVDSP_SOFTWARE_VER_SPB
.equ  _ds1103_slvdsp_software_ver_spn, DS1103_SLVDSP_SOFTWARE_VER_SPN
.equ  _ds1103_slvdsp_software_ver_plv, DS1103_SLVDSP_SOFTWARE_VER_PLV

.equ  _req_ds1103_slvdsp_firmware_ver_mar, REQ_DS1103_SLVDSP_FIRMWARE_VER_MAR
.equ  _req_ds1103_slvdsp_firmware_ver_mir, REQ_DS1103_SLVDSP_FIRMWARE_VER_MIR
.equ  _req_ds1103_slvdsp_firmware_ver_mai, REQ_DS1103_SLVDSP_FIRMWARE_VER_MAI
.equ  _req_ds1103_slvdsp_firmware_ver_spb, REQ_DS1103_SLVDSP_FIRMWARE_VER_SPB
.equ  _req_ds1103_slvdsp_firmware_ver_spn, REQ_DS1103_SLVDSP_FIRMWARE_VER_SPN
.equ  _req_ds1103_slvdsp_firmware_ver_plv, REQ_DS1103_SLVDSP_FIRMWARE_VER_PLV

#pragma endasm
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Error function
*
* SYNTAX
*   void ds1103_slave_dsp_communication_error (Int16 error)
*
* DESCRIPTION
*
* PARAMETERS
*   error               error number
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_communication_error (Int16 error)
{
  switch (error)
  {
    case DSMCOM_ALLOC_ERROR:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_ALLOC_ERROR);
      exit(1);
      break;

    case DSMCOM_ILLEGAL_TASK_ID:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_ILLEGAL_TASK_ID);
      exit(1);
      break;

    case DSMCOM_ILLEGAL_INDEX:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_ILLEGAL_INDEX);
      exit(1);
      break;

    case DSMCOM_BUFFER_OVERFLOW:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_BUFFER_OVERFLOW);
      exit(1);
      break;

    case DSMCOM_NO_DATA:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_NO_DATA);
      exit(1);
      break;

    case DSMCOM_DATA_LOST:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_DATA_LOST);
      exit(1);
      break;

    case DSMCOM_TIMEOUT:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_TIMEOUT);
      exit(1);
      break;

    case DSMCOM_SLV_ALLOC_ERROR:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_SLV_ALLOC_ERROR);
      exit(1);
      break;

    case DSMCOM_SLV_BUFFER_OVERFLOW:
      SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_SLV_BUFFER_OVERFLOW);
      exit(1);
      break;

    default:
      msg_error_set(MSG_SM_DS1103SLVLIB, error,
        SLVDSP1103_MSG_UNKNOWN_ERROR_TXT);
      exit(1);
      break;
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   Basic initialization of PPC-slave communication
*
* SYNTAX
*   void ds1103_slave_dsp_communication_init (void)
*
* DESCRIPTION
*   The command table and communication buffers are initialized.
*
* PARAMETERS
*   void
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_communication_init (void)
{
  static UInt16 init_done = 0;                        /* initialization flag */
  volatile UInt16 *rst = (UInt16 *) 0x10000040;            /* reset register */
  Int16 error;
  UInt16 fw_rev[4];
  char fw_version[2] = { '\0', '\0' };
  Float64 time;
  ts_timestamp_type start_time, end_time;
  
  UInt16 prgcountl, prgcounth = 0;
  /* VCM support */
  UInt32 special_build = VCM_VERSION_RELEASE;
  vcm_version_type temp_vs;
  vcm_module_descriptor_type *fw_vcm_module_ptr = (void*)0;
  vcm_module_descriptor_type *user_fw_vcm_module_ptr = (void*)0;
  vcm_module_descriptor_type *slvdsp1103_vcm_module_ptr = (void*)0;
  vcm_module_descriptor_type* additional_mem_ptr;
  common_fw_vcm_entry_t* common_fw_vcm_entry_ptr;



  if (init_done)                     /* check if initialiaztion already done */
  {
    return;
  }

  init_done = 1;

  *rst |= SLVDSP1103_RSTSLV;                               /* stop slave DSP */

  /* initialize master-slave communication */
  error = dsmcom_init(&slvdsp1103_handle, DS1103_SLAVE_DSP_DP_RAM_START,
    DPM_TARGET_DIRECT, DEF1103_SLVDSP_BUFFER_NUM, DEF1103_SLVDSP_BUFFER_SIZE,
    DEF1103_MST_ADDR_INC, DEF1103_SLVDSP_ADDR_INC, DPM_ACCESS_DIRECT,
    DPM_ACCESS_DIRECT);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  /* clear DPMEM firmware locations */
  *(volatile UInt16 *)DEF1103_SLVDSP_FW_BURNC_LO_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_FW_BURNC_HI_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_USER_MAJ_FW_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_USER_MIN_FW_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_USER_SUB_FW_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_DSPACE_FW_ADDR   = 0xAAA6;

  *rst &= ~SLVDSP1103_RSTSLV;                             /* start slave DSP */

  ds1103_cfg_section->__cfg_slave_dsp_status = SLVDSP1103_RUNNING;

  /* wait for slave DSP writing firmware revision to DPMEM */
  ts_timestamp_read(&start_time);
  do
  {
    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
    if ( time > DSCOMDEF_TIMEOUT)
    {
      msg_error_printf(MSG_SM_DS1103SLVLIB,
        SLVDSP1103_MSG_TIMEOUT_OR_WRONG_FIRMWARE,
        SLVDSP1103_MSG_TIMEOUT_OR_WRONG_FIRMWARE_TXT);
      exit(1);                              /* timeout, if no slave response */
    }
  }
  while (*(volatile UInt16 *)DEF1103_SLVDSP_DSPACE_FW_ADDR == 0xAAA6);

  /* read firmware rev. and user firmware rev. from DPMEM */
  fw_rev[0] = *(volatile UInt16 *)DEF1103_SLVDSP_DSPACE_FW_ADDR;
  fw_rev[1] = *(volatile UInt16 *)DEF1103_SLVDSP_USER_MAJ_FW_ADDR;
  fw_rev[2] = *(volatile UInt16 *)DEF1103_SLVDSP_USER_MIN_FW_ADDR;
  fw_rev[3] = *(volatile UInt16 *)DEF1103_SLVDSP_USER_SUB_FW_ADDR;

  /* generate firmware revision info message */
  if (fw_rev[0] & (SLVDSP1103_FW_MAINT | SLVDSP1103_FW_VERSION))
  {
    switch (fw_rev[0] & SLVDSP1103_FW_VERSION)
    {
      case SLVDSP1103_FW_ALPHA:
        fw_version[0] = 'a';
        special_build = VCM_VERSION_ALPHA;
        break;

      case SLVDSP1103_FW_BETA:
        fw_version[0] = 'b';
        special_build = VCM_VERSION_BETA;
        break;

      default:
        fw_version[0] = '\0';
    }

    if (fw_rev[0] & SLVDSP1103_FW_ORIGIN)
    {
      msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_SLV_FIRMWARE_REV,
        "dSPACE firmware rev. %d.%d.%s%d detected.",
        (fw_rev[0] & SLVDSP1103_FW_MAJOR) >> 11,
        (fw_rev[0] & SLVDSP1103_FW_MINOR) >> 7,
        fw_version,
        (fw_rev[0] & SLVDSP1103_FW_MAINT) >> 3);
    }
    else
    {
      msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_SLV_FIRMWARE_REV,
        "User firmware rev. %d.%d.%d based on dSPACE firmware rev. %d.%d.%s%d detected.",
        fw_rev[1], fw_rev[2], fw_rev[3],
        (fw_rev[0] & SLVDSP1103_FW_MAJOR) >> 11,
        (fw_rev[0] & SLVDSP1103_FW_MINOR) >> 7,
        fw_version,
        (fw_rev[0] & SLVDSP1103_FW_MAINT) >> 3);
    }

  }
  else
  {
    if (fw_rev[0] & SLVDSP1103_FW_ORIGIN)
    {
      msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_SLV_FIRMWARE_REV,
        "dSPACE firmware rev. %d.%d detected.",
        (fw_rev[0] & SLVDSP1103_FW_MAJOR) >> 11,
        (fw_rev[0] & SLVDSP1103_FW_MINOR) >> 7);

    }
    else
    {
      msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_SLV_FIRMWARE_REV,
        "User firmware rev. %d.%d.%d based on dSPACE firmware rev. %d.%d detected.",
        fw_rev[1], fw_rev[2], fw_rev[3],
        (fw_rev[0] & SLVDSP1103_FW_MAJOR) >> 11,
        (fw_rev[0] & SLVDSP1103_FW_MINOR) >> 7);
    }
  }

  /* VCM support ***********************************************************/

  temp_vs.vs.mar=(fw_rev[0] & SLVDSP1103_FW_MAJOR) >> 11;
  temp_vs.vs.mir=(fw_rev[0] & SLVDSP1103_FW_MINOR) >>  7;
  temp_vs.vs.spb=special_build;
  temp_vs.vs.plv=0;
  if (special_build == VCM_VERSION_RELEASE)
  {
    temp_vs.vs.mai = (fw_rev[0] & SLVDSP1103_FW_MAINT) >> 3;
    temp_vs.vs.spn = 0;
  }
  else
  {
    temp_vs.vs.mai = 0;
    temp_vs.vs.spn = (fw_rev[0] & SLVDSP1103_FW_MAINT) >> 3;
  }

  /* enter version number of software module slvdsp1103 */
  slvdsp1103_vcm_module_ptr = vcm_module_register(VCM_MID_SLVDSP1103,
              vcm_module_find(VCM_MID_RTLIB, NULL), VCM_TXT_SLVDSP1103,
               DS1103_SLVDSP_SOFTWARE_VER_MAR, DS1103_SLVDSP_SOFTWARE_VER_MIR,
               DS1103_SLVDSP_SOFTWARE_VER_MAI, DS1103_SLVDSP_SOFTWARE_VER_SPB,
               DS1103_SLVDSP_SOFTWARE_VER_SPN, DS1103_SLVDSP_SOFTWARE_VER_PLV,
               0, 0);

  /* enter firmware versions */
  if (fw_rev[0] & SLVDSP1103_FW_ORIGIN)
  {
    /* enter version number of slave DSP firmware */
    fw_vcm_module_ptr = vcm_module_register(VCM_MID_SLVDSP1103_FW,
                  0,
                  VCM_TXT_SLVDSP1103_FW,
                  temp_vs.vs.mar,
                  temp_vs.vs.mir,
                  temp_vs.vs.mai,
                  temp_vs.vs.spb,
                  temp_vs.vs.spn,
                  0, 0, 0);

     additional_mem_ptr = fw_vcm_module_ptr;
  }
  else
  {
    /* enter version number of user slave DSP firmware */
    user_fw_vcm_module_ptr = vcm_module_register(VCM_MID_USER_SLVDSP1103_FW,
                  0,
                  VCM_TXT_USER_SLVDSP1103_FW,
                  fw_rev[1],
                  fw_rev[2],
                  fw_rev[3],
                  0, 0, 0, 0, 0);

    /* enter version number of slave DSP firmware (based on...) */
    fw_vcm_module_ptr = vcm_module_register(VCM_MID_BASED_ON_FW,
                  user_fw_vcm_module_ptr,
                  VCM_TXT_BASED_ON_FW,
                  temp_vs.vs.mar,
                  temp_vs.vs.mir,
                  temp_vs.vs.mai,
                  temp_vs.vs.spb,
                  temp_vs.vs.spn,
                  0, 0, 0);

     additional_mem_ptr = user_fw_vcm_module_ptr;

  }

  /* enter required  version number of slave DSP firmware */
  temp_vs.vs.mar = REQ_DS1103_SLVDSP_FIRMWARE_VER_MAR;
  temp_vs.vs.mir = REQ_DS1103_SLVDSP_FIRMWARE_VER_MIR;
  temp_vs.vs.mai = REQ_DS1103_SLVDSP_FIRMWARE_VER_MAI;
  temp_vs.vs.spb = REQ_DS1103_SLVDSP_FIRMWARE_VER_SPB;
  temp_vs.vs.plv = REQ_DS1103_SLVDSP_FIRMWARE_VER_PLV;
  temp_vs.vs.spn = REQ_DS1103_SLVDSP_FIRMWARE_VER_SPN;

  /* read flash programming count number from dpmem, prgcounth is set to zero! */
  prgcountl = *((volatile UInt16*) DEF1103_SLVDSP_FW_BURNC_LO_ADDR);

  /* the additional memory block of the firmware */
  common_fw_vcm_entry_ptr = (common_fw_vcm_entry_t *) vcm_cfg_malloc(sizeof(common_fw_vcm_entry_t));
  common_fw_vcm_entry_ptr->fw_burn_count = prgcountl | (prgcounth << 16);
  common_fw_vcm_entry_ptr->fw_origin = fw_rev[0] & SLVDSP1103_FW_ORIGIN;
  common_fw_vcm_entry_ptr->fw_location = 0;
  common_fw_vcm_entry_ptr->req_fw_ver.version.high = temp_vs.version.high;
  common_fw_vcm_entry_ptr->req_fw_ver.version.low = temp_vs.version.low;
  vcm_memory_ptr_set(additional_mem_ptr, common_fw_vcm_entry_ptr, sizeof(common_fw_vcm_entry_t));

  if(user_fw_vcm_module_ptr)
    vcm_module_status_set(user_fw_vcm_module_ptr, VCM_STATUS_INITIALIZED);

  vcm_module_status_set(slvdsp1103_vcm_module_ptr, VCM_STATUS_INITIALIZED);
  vcm_module_status_set(fw_vcm_module_ptr, VCM_STATUS_INITIALIZED);


  /* VCM support ***********************************************************/

  /* check current firmware version (dSPACE only) */

  if ((fw_rev[0] & SLVDSP1103_FW_ORIGIN) &&
      (fw_rev[0] & SLVDSP1103_FW_CHECK) < SLVDSP1103_FW_REQ)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_FW_REV_UP_ERROR,
      SLVDSP1103_MSG_FW_REV_UP_ERROR_TXT_1, SLVDSP1103_FW_REQ_TXT);
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_FW_REV_UP_ERROR,
      SLVDSP1103_MSG_FW_REV_UP_ERROR_TXT_2);
    exit(1);
  }

  /* clear DPMEM firmware locations */
  *(volatile UInt16 *)DEF1103_SLVDSP_FW_BURNC_LO_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_FW_BURNC_HI_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_USER_MAJ_FW_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_USER_MIN_FW_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_USER_SUB_FW_ADDR = 0;
  *(volatile UInt16 *)DEF1103_SLVDSP_DSPACE_FW_ADDR   = 0;

  /* check for slave DSP initialization acknowledge */
  
  error = dsmcom_slave_acknowledge_check(slvdsp1103_handle, 0);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }


  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_COMM_INIT_INFO);
  }
}


#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   Slave DSP firmware revision read
*
* SYNTAX
*   Int16 ds1103_slave_dsp_firmware_rev_read (Int16 task_id, UInt32 *revision)
*
* DESCRIPTION
*   The revision number of the slave DSP firmware is read.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   revision            firmware revision
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/

#ifndef _INLINE

Int16 ds1103_slave_dsp_firmware_rev_read (Int16 task_id, UInt32 *revision)
{
  static int index = -1;
  unsigned long dummy;
  Int16 error;

  error = dsmcom_register(slvdsp1103_handle, task_id, &index,
    DSPDEF_FWREV_READ, DSPDEF_FWREV_READ_DINCNT, DSPDEF_FWREV_READ_DOUTCNT,
    DSPDEF_FWREV_READ_PCNT, (unsigned long *) &dummy);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  if (error == DSMCOM_NO_ERROR)
  {
    do
    {
      error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
        (unsigned long *) revision);
    }
    while (error == DSMCOM_NO_DATA);
  }

  /* check for firmware revision 1.0.0 */
  if (revision[0] == 0x1000)
    revision[0] = (revision[0] >> 1) | 0x0001;      /* convert to new format */

  return(error);
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Slave DSP user firmware revision read
*
* SYNTAX
*   Int16 ds1103_slave_dsp_user_firmware_rev_read (Int16 task_id, UInt32 *revision)
*
* DESCRIPTION
*   The revision number of the slave DSP user firmware is read as specified in
*   the module usrdsp.c of the slave DSP.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   revision            firmware revision
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/

#ifndef _INLINE

Int16 ds1103_slave_dsp_user_firmware_rev_read (Int16 task_id, UInt32 *revision)
{
  static int index = -1;
  unsigned long dummy;
  Int16 error;

  error = dsmcom_register(slvdsp1103_handle, task_id, &index,
    DSPDEF_USRFWREV_READ, DSPDEF_USRFWREV_READ_DINCNT, DSPDEF_USRFWREV_READ_DOUTCNT,
    DSPDEF_USRFWREV_READ_PCNT, (unsigned long *) &dummy);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  if (error == DSMCOM_NO_ERROR)
  {
    do
    {
      error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
        (unsigned long *) revision);
    }
    while (error == DSMCOM_NO_DATA);
  }

  return(error);
}

#endif /* _INLINE */

/******************************************************************************
*
* FUNCTION
*   Slave DSP error status read
*
* SYNTAX
*   void ds1103_slave_dsp_error_read (Int16 task_id, UInt16 *slave_error)
*
* DESCRIPTION
*   The slave error status for the specified communication channel is read
*   from the communication memory.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   slave_error         slave error number
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

__INLINE void ds1103_slave_dsp_error_read (Int16 task_id, UInt32 *slave_error)
{
  dsmcom_slave_error_read(slvdsp1103_handle, task_id,
    (unsigned long *) slave_error);
}


/******************************************************************************
*
* FUNCTION
*   ADC initialization
*
* SYNTAX
*   void ds1103_slave_dsp_adc_init (Int16 task_id, UInt16 sel_mask)
*
* DESCRIPTION
*   The channels specified by 'sel_mask' are initialized for ADC input.
*   This is required for ADC channels 1, 2, 9, and 10 only. All other channels
*   are available by default.
*
*   pin assignment:     channel | pin
*                       --------|--------
*                          1    | SADCH1
*                          .    |  .
*                          .    |  .
*                         16    | SADCH16
*
* PARAMETERS
*   task_id             task ID of communication channel
*   sel_mask            channel selection mask (combination of
*                         SLVDSP1103_ADC_CH1_MSK, SLVDSP1103_ADC_CH2_MSK,
*                         SLVDSP1103_ADC_CH9_MSK, SLVDSP1103_ADC_CH10_MSK)
*
* RETURNS
*   void
*
* REMARKS
*   Digital I/O port 1 bits 0..3 conflict with ADC input channels 1, 2, 10,
*   and 9, respectively.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_adc_init (Int16 task_id, UInt16 sel_mask)
{
  unsigned long parms[DSPDEF_ADC_INIT_PCNT];
  Int16 error;

  parms[DSPDEF_ADC_INIT_MSK_LOC] = (sel_mask & SLVDSP1103_ADC_CH1_MSK)
    | (sel_mask & SLVDSP1103_ADC_CH2_MSK)
    | (sel_mask & SLVDSP1103_ADC_CH9_MSK) >> 5
    | (sel_mask & SLVDSP1103_ADC_CH10_MSK) >> 7;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_ADC_INIT,
    DSPDEF_ADC_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  slvdsp1103_adc_sel_mask |= sel_mask;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_ADC_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   ADC input
*
* SYNTAX
*   void ds1103_slave_dsp_adc_read_register (Int16 task_id, Int16 *index,
*     UInt16 channel, UInt16 int_enable)
*   Int16 ds1103_slave_dsp_adc_read_request (Int16 task_id, Int32 index)
*   Int16 ds1103_slave_dsp_adc_read (Int16 task_id, Int32 index,
*     Float64 *value)
*   Int16 ds1103_slave_dsp_adc_read_new (Int16 task_id, Int32 index,
*     Float64 *value)
*
* DESCRIPTION
*   An input value is read from the specified ADC channel. The result is
*   scaled to the range 0..1.0.
*   The function ds1103_slave_dsp_adc_read() returns the result from the
*   previous conversion if the current conversion has not yet been finished.
*   The function ds1103_slave_dsp_adc_read_new() waits until the result
*   of the current conversion is available.
*   If enabled, a PPC interrupt will be generated on EOC with the sub-interrupt
*   number of the selected ADC channel.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             ADC channel (1..16)
*   int_enable          interrupt enable (SLVDSP1103_INT_DISABLE,
*                         SLVDSP1103_INT_ENABLE)
*   value               ADC input value (0..1.0)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   If one of the channels 1, 2, 9, and 10 are used, ADC input must have been
*   initialized by using the function ds1103_slave_dsp_adc_init().
*   Digital I/O port 1 bits 0..3 conflict with ADC input channels 1, 2, 10,
*   and 9, respectively.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_adc_read_register (Int16 task_id, Int16 *index,
  UInt16 channel, UInt16 int_enable)
{
  unsigned long parms[DSPDEF_ADC_READ_PCNT];
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  if (channel < 1 || channel > 16)
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_ADC_RANGE_ERROR);
    exit(1);
  }

  if ((slvdsp1103_adc_sel_mask & 1 << (channel-1)) == 0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_ADC_INIT_ERROR,
      SLVDSP1103_MSG_ADC_INIT_ERROR_TXT, channel);
    exit(1);
  }

  parms[DSPDEF_ADC_READ_CH_LOC] = channel;
  parms[DSPDEF_ADC_READ_INT_LOC] = int_enable;
  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_ADC_READ,
    DSPDEF_ADC_READ_DINCNT, DSPDEF_ADC_READ_DOUTCNT,
    DSPDEF_ADC_READ_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_ADC_REG_INFO,
      SLVDSP1103_MSG_ADC_REG_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_adc_read_request (Int16 task_id, Int32 index)
{
  Int16 error;

  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_adc_read (Int16 task_id, Int32 index,
  Float64 *value)
{
  unsigned long tmp;
  Int16 error;

  error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
    (unsigned long *) &tmp);
  *value = (Float64) tmp * SLVDSP1103_ADC_READ_SCALE;

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_adc_read_new (Int16 task_id, Int32 index,
  Float64 *value)
{
  unsigned long tmp;
  Int16 error;

  do
  {
    error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
      (unsigned long *) &tmp);
  }
  while (error == SLVDSP1103_NO_DATA);

  *value = (Float64) tmp * SLVDSP1103_ADC_READ_SCALE;

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   2-channel ADC input
*
* SYNTAX
*   void ds1103_slave_dsp_adc2_read_register (Int16 task_id, Int16 *index,
*     UInt16 channel1, UInt16 channel2, UInt16 int_enable)
*   Int16 ds1103_slave_dsp_adc2_read_request (Int16 task_id, Int32 index)
*   Int16 ds1103_slave_dsp_adc2_read (Int16 task_id, Int32 index,
*     Float64 *value1, Float64 *value2)
*   Int16 ds1103_slave_dsp_adc2_read_new (Int16 task_id, Int32 index,
*     Float64 *value1, Float64 *value2)
*
* DESCRIPTION
*   Two input values are read from the specified ADC channels. The results are
*   scaled to the range 0..1.0.
*   The function ds1103_slave_dsp_adc2_read() returns the results from the
*   previous conversions if the current conversions have not yet been finished.
*   The function ds1103_slave_dsp_adc2_read_new() waits until the results
*   of the current conversions are available.
*   If enabled, a PPC interrupt will be generated on EOC with the sub-interrupt
*   number of the selected ADC1 channel.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel1            ADC1 channel (1..8)
*   channel2            ADC2 channel (9..16)
*   int_enable          interrupt enable (SLVDSP1103_INT_DISABLE,
*                         SLVDSP1103_INT_ENABLE)
*   value1              ADC1 input value (0..1.0)
*   value2              ADC2 input value (0..1.0)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   If one of the channels 1, 2, 9, and 10 are used, ADC input must have been
*   initialized by using the function ds1103_slave_dsp_adc_init().
*   Digital I/O port 1 bits 0..3 conflict with ADC input channels 1, 2, 10,
*   and 9, respectively.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_adc2_read_register (Int16 task_id, Int16 *index,
  UInt16 channel1, UInt16 channel2, UInt16 int_enable)
{
  unsigned long parms[DSPDEF_ADC2_READ_PCNT];
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  if (channel1 < 1 || channel1 > 8)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_ADC2_RANGE_ERROR,
      SLVDSP1103_MSG_ADC2_RANGE_ERROR_TXT, "channel1");
    exit(1);
  }

  if  (channel2 < 9 || channel2 > 16)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_ADC2_RANGE_ERROR,
      SLVDSP1103_MSG_ADC2_RANGE_ERROR_TXT, "channel2");
    exit(1);
  }

  if ((slvdsp1103_adc_sel_mask & 1 << (channel1-1)) == 0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_ADC2_INIT_ERROR,
      SLVDSP1103_MSG_ADC2_INIT_ERROR_TXT, channel1);
    exit(1);
  }

  if ((slvdsp1103_adc_sel_mask & 1 << (channel2-1)) == 0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_ADC2_INIT_ERROR,
      SLVDSP1103_MSG_ADC2_INIT_ERROR_TXT, channel2);
    exit(1);
  }

  parms[DSPDEF_ADC2_READ_CH1_LOC] = channel1;
  parms[DSPDEF_ADC2_READ_CH2_LOC] = channel2;
  parms[DSPDEF_ADC2_READ_INT_LOC] = int_enable;
  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_ADC2_READ,
    DSPDEF_ADC2_READ_DINCNT, DSPDEF_ADC2_READ_DOUTCNT,
    DSPDEF_ADC2_READ_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_ADC2_REG_INFO,
      SLVDSP1103_MSG_ADC2_REG_INFO_TXT, channel1, channel2);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_adc2_read_request (Int16 task_id, Int32 index)
{
  Int16 error;

  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_adc2_read (Int16 task_id, Int32 index,
  Float64 *value1, Float64 *value2)
{
  unsigned long tmp[2];
  Int16 error;

  error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
    (unsigned long *) tmp);
  *value1 = (Float64) tmp[0] * SLVDSP1103_ADC_READ_SCALE;
  *value2 = (Float64) tmp[1] * SLVDSP1103_ADC_READ_SCALE;

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_adc2_read_new (Int16 task_id, Int32 index,
  Float64 *value1, Float64 *value2)
{
  unsigned long tmp[2];
  Int16 error;

  do
  {
    error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
      (unsigned long *) tmp);
  }
  while (error == SLVDSP1103_NO_DATA);
  *value1 = (Float64) tmp[0] * SLVDSP1103_ADC_READ_SCALE;
  *value2 = (Float64) tmp[1] * SLVDSP1103_ADC_READ_SCALE;

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Digital I/O port initialization
*
* SYNTAX
*   void ds1103_slave_dsp_bit_io_init (Int16 task_id, UInt16 channel,
*     UInt8 sel_mask, UInt8 dir_mask)
*
* DESCRIPTION
*   The specified digital I/O port is initialized bitwise for input or output
*   corresponding to the parameter 'dir_mask'. The pins specified by 'sel_mask'
*   are selected as digital I/O pins.
*
*   pins assignment:   bit | port 1  | port 2  | port 3
*                      ----|---------|---------|--------
*                       0  | SADCH1  | SPWM7   | SADCOSC
*                       1  | SADCH2  | SPWM8   |
*                       2  | SADCH10 | SPWM9   | SXF
*                       3  | SADCH9  | ST1PWM  | SBIO
*                       4  |         | ST2PWM  | SCAP1
*                       5  |         | ST3PWM  | SCAP2
*                       6  |         | STMRDIR | SCAP3
*                       7  |         | STMRCLK | SCAP4
*
*   Bits 4..7 of port 1 and bit 1 of port 3 are not available.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   channel             port channel number (1..3)
*   sel_mask            pin selection bitmask (0: deselected, 1: selected)
*   dir_mask            direction bitmask (0: input, 1: output)
*
* RETURNS
*   void
*
* REMARKS
*   Digital I/O port 1 bits 0..3 conflict with ADC input channels 1, 2, 10, 9.
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*   Digital I/O port 3 bits 4..7 conflict with F/D and PWM/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_bit_io_init (Int16 task_id, UInt16 channel,
  UInt8 sel_mask, UInt8 dir_mask)
{
  unsigned long parms[DSPDEF_PIO_INIT_PCNT];
  Int16 error;

  if (channel < 1 || channel > 3)
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_PIO_RANGE_ERROR1);
    exit(1);
  }

  parms[DSPDEF_PIO_INIT_CH_LOC] = channel;
  parms[DSPDEF_PIO_INIT_SEL_MSK_LOC] = (UInt16) sel_mask;
  parms[DSPDEF_PIO_INIT_DIR_MSK_LOC] = (UInt16) dir_mask;
  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PIO_INIT,
    DSPDEF_PIO_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PIO_INIT_INFO,
      SLVDSP1103_MSG_PIO_INIT_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   Digital I/O port input
*
* SYNTAX
*   void ds1103_slave_dsp_bit_io_read_register (Int16 task_id, Int16 *index,
*     UInt16 channel)
*   Int16 ds1103_slave_dsp_bit_io_read_request (Int16 task_id, Int32 index)
*   UInt16 ds1103_slave_dsp_bit_io_read (Int16 task_id, Int32 index,
*     UInt8 *value)
*   UInt16 ds1103_slave_dsp_bit_io_read_new (Int16 task_id, Int32 index,
*     UInt8 *value)
*
* DESCRIPTION
*   One byte is read from the specified digital I/O port.
*   The function ds1103_slave_dsp_bit_io_read() may return the result from the
*   previous request if the new result has not yet been received.
*   The function ds1103_slave_dsp_bit_io_read_new() waits until the requested
*   result is available.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             port channel number (1..3)
*   value               input value
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   The respective digital I/O port must have been initialized by using the
*   function ds1103_slave_dsp_bit_io_init().
*   Digital I/O port 1 bits 0..3 conflict with ADC input channels 1, 2, 10, 9.
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*   Digital I/O port 3 bits 4..7 conflict with F/D and PWM/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_bit_io_read_register (Int16 task_id, Int16 *index,
  UInt32 channel)
{
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  if (channel < 1 || channel > 3)
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_PIO_RANGE_ERROR2);
    exit(1);
  }

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_PIO_READ,
    DSPDEF_PIO_READ_DINCNT, DSPDEF_PIO_READ_DOUTCNT,
    DSPDEF_PIO_READ_PCNT, (unsigned long *) &channel);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PIO_READ_REG_INFO,
      SLVDSP1103_MSG_PIO_READ_REG_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_bit_io_read_request (Int16 task_id,
  Int32 index)
{
  Int16 error;

  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_bit_io_read (Int16 task_id, Int32 index,
  UInt8 *value)
{
  unsigned long tmp;
  Int16 error;

  error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
    (unsigned long *) &tmp);
  *value = (UInt8) tmp;

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_bit_io_read_new (Int16 task_id, Int32 index,
  UInt8 *value)
{
  unsigned long tmp;
  Int16 error;

  do
  {
    error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
      (unsigned long *) &tmp);
  }
  while (error == SLVDSP1103_NO_DATA);
  *value = (UInt8) tmp;

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Digital I/O port output
*
* SYNTAX
*   void ds1103_slave_dsp_bit_io_write_register (Int16 task_id, Int16 *index,
*     UInt16 channel)
*   Int16 ds1103_slave_dsp_bit_io_write (Int16 task_id, Int32 index,
*     UInt8 value)
*
* DESCRIPTION
*   One byte is written to the specified digital I/O port.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             port channel number (1..3)
*   value               output value
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   The respective digital I/O port must have been initialized by using the
*   function ds1103_slave_dsp_bit_io_init().
*   Digital I/O port 1 bits 0..3 conflict with ADC input channels 1, 2, 10, 9.
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*   Digital I/O port 3 bits 4..7 conflict with F/D and PWM/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_bit_io_write_register (Int16 task_id, Int16 *index,
  UInt32 channel)
{
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  if (channel < 1 || channel > 3)
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_PIO_RANGE_ERROR3);
    exit(1);
  }

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_PIO_WRITE,
    DSPDEF_PIO_WRITE_DINCNT, DSPDEF_PIO_WRITE_DOUTCNT,
    DSPDEF_PIO_WRITE_PCNT, (unsigned long *) &channel);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PIO_WRITE_REG_INFO,
      SLVDSP1103_MSG_PIO_WRITE_REG_INFO_TXT, channel);
  }
}

#endif


__INLINE Int16 ds1103_slave_dsp_bit_io_write (Int16 task_id, Int32 index,
  UInt8 value)
{
  unsigned long tmp = (UInt16) value;
  Int16 error;

  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) &tmp);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Bitwise digital I/O port set
*
* SYNTAX
*   void ds1103_slave_dsp_bit_io_set_register (Int16 task_id, Int16 *index,
*     UInt16 channel)
*   Int16 ds1103_slave_dsp_bit_io_set (Int16 task_id, Int32 index,
*     UInt8 mask)
*
* DESCRIPTION
*   The bits specified by 'mask' are set at the specified digital I/O port.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             port channel number (1..3)
*   mask                output bitmask (0: no action, 1: set bit)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   The respective digital I/O port must have been initialized by using the
*   function ds1103_slave_dsp_bit_io_init().
*   Digital I/O port 1 bits 0..3 conflict with ADC input channels 1, 2, 10, 9.
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*   Digital I/O port 3 bits 4..7 conflict with F/D and PWM/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_bit_io_set_register (Int16 task_id, Int16 *index,
  UInt32 channel)
{
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  if (channel < 1 || channel > 3)
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_PIO_RANGE_ERROR4);
    exit(1);
  }

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_PIO_SET,
    DSPDEF_PIO_SET_DINCNT, DSPDEF_PIO_SET_DOUTCNT,
    DSPDEF_PIO_SET_PCNT, (unsigned long *) &channel);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PIO_SET_REG_INFO,
      SLVDSP1103_MSG_PIO_SET_REG_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_bit_io_set (Int16 task_id, Int32 index,
  UInt8 mask)
{
  unsigned long tmp = (UInt16) mask;
  Int16 error;

  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) &tmp);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Bitwise digital I/O port clear
*
* SYNTAX
*   void ds1103_slave_dsp_bit_io_clear_register (Int16 task_id, Int16 *index,
*     UInt16 channel)
*   Int16 ds1103_slave_dsp_bit_io_clear (Int16 task_id, Int32 index,
*     UInt8 mask)
*
* DESCRIPTION
*   The bits specified by 'mask' are cleared at the specified digital I/O port.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             port channel number (1..3)
*   mask                output bitmask (0: no action, 1: clear bit)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   The respective digital I/O port must have been initialized by using the
*   function ds1103_slave_dsp_bit_io_init().
*   Digital I/O port 1 bits 0..3 conflict with ADC input channels 1, 2, 10, 9.
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*   Digital I/O port 3 bits 4..7 conflict with F/D and PWM/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_bit_io_clear_register (Int16 task_id, Int16 *index,
  UInt32 channel)
{
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  if (channel < 1 || channel > 3)
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_PIO_RANGE_ERROR5);
    exit(1);
  }

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_PIO_CLR,
    DSPDEF_PIO_CLR_DINCNT, DSPDEF_PIO_CLR_DOUTCNT,
    DSPDEF_PIO_CLR_PCNT, (unsigned long *) &channel);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PIO_CLR_REG_INFO,
      SLVDSP1103_MSG_PIO_CLR_REG_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_bit_io_clear (Int16 task_id, Int32 index,
  UInt8 mask)
{
  unsigned long tmp = (UInt16) mask;
  Int16 error;

  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) &tmp);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Simple PWM initialization
*
* SYNTAX
*   void ds1103_slave_dsp_pwm_init (Int16 task_id, Float64 period,
*     Float64 duty, UInt16 mode, UInt16 pol, UInt16 mask)
*
* DESCRIPTION
*   The channels specified by 'mask' are initialized for simple PWM generation.
*
*   pin assignment:     channel | pin
*                       --------|-------
*                          1    | ST2PWM
*                          2    | SPWM7
*                          3    | SPWM8
*                          4    | SPWM8
*
*   ranges:             PWM mode   | period min | period max
*                       -----------|------------|-----------
*                       asymmetric | 100 ns     | 400 ms
*                       symmetric  | 200 ns     | 800 ms
*
*   resolution:         period (asym/sym)  | res (asym/sym)
*                       -------------------|---------------
*                       <   3.2 /   6.4 ms |  50 /  100 ns
*                       <   6.4 /  12.8 ms | 100 /  200 ns
*                       <  12.8 /  25.6 ms | 200 /  400 ns
*                       <  25.6 /  51.2 ms | 400 /  800 ns
*                       <  51.2 / 102.4 ms | 0.8 /  1.6 us
*                       < 102.4 / 204.8 ms | 1.6 /  3.2 us
*                       < 204.8 / 409.6 ms | 3.2 /  6.4 us
*                       < 409.6 / 819.2 ms | 6.4 / 12.8 us
*
* PARAMETERS
*   task_id             task ID of communication channel
*   period              common PWM period (in s)
*   duty                common initial duty cycle (0..1.0)
*   mode                PWM generation mode (SLVDSP1103_PWM_MODE_ASYM/_SYM)
*   pol                 PWM output polarity (SLVDSP1103_PWM_POL_LOW/_HIGH)
*   mask                channel mask (combination of
*                         SLVDSP1103_PWM_CH1_MSK, SLVDSP1103_PWM_CH2_MSK,
*                         SLVDSP1103_PWM_CH3_MSK, SLVDSP1103_PWM_CH4_MSK)
*
* RETURNS
*   void
*
* REMARKS
*   All PWM channels must use the same PWM period and mode.
*   The output polarity can be assigned individually by calling this function
*   repeatedly for different channels.
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_pwm_init (Int16 task_id, Float64 period, Float64 duty,
  UInt16 mode, UInt16 pol, UInt16 mask)
{
  UInt16 period_scaled, prescale = 0;
  Int32 duty_scaled;
  unsigned long parms[DSPDEF_PWM_INIT_PCNT];
  Int16 error;

  if (mode == SLVDSP1103_PWM_MODE_SYM)
  {
    /* PWM input clock prescaling */

    if (period < 6.4e-3)
      prescale = 0x00;
    else if (period < 12.8e-3)
      prescale = 0x01;
    else if (period < 25.6e-3)
      prescale = 0x02;
    else if (period < 51.2e-3)
      prescale = 0x03;
    else if (period < 102.4e-3)
      prescale = 0x04;
    else if (period < 204.8e-3)
      prescale = 0x05;
    else if (period < 409.6e-3)
      prescale = 0x06;
    else if (period < 819.2e-3)
      prescale = 0x07;
    else
    {
      msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM_RANGE_ERROR1,
        SLVDSP1103_MSG_PWM_RANGE_ERROR1_TXT, "period");
      exit(1);
    }

    /* PWM period */

    period_scaled = (UInt16) (period * SLVDSP1103_CPU_CLK /
      (Float64) (1 << prescale) / 2);
  }
  else
  {
    /* PWM input clock prescaling */

    if (period < 3.2e-3)
      prescale = 0x00;
    else if (period < 6.4e-3)
      prescale = 0x01;
    else if (period < 12.8e-3)
      prescale = 0x02;
    else if (period < 25.6e-3)
      prescale = 0x03;
    else if (period < 51.2e-3)
      prescale = 0x04;
    else if (period < 102.4e-3)
      prescale = 0x05;
    else if (period < 204.8e-3)
      prescale = 0x06;
    else if (period < 409.6e-3)
      prescale = 0x07;
    else
    {
      msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM_RANGE_ERROR1,
        SLVDSP1103_MSG_PWM_RANGE_ERROR1_TXT, "period");
      exit(1);
    }

    /* PWM period */

    period_scaled = (UInt16) (period * SLVDSP1103_CPU_CLK /
      (Float64) (1 << prescale)) - 1;
  }

  slvdsp1103_pwm_period = (Float64) period_scaled;
  slvdsp1103_pwm_mode = mode;

  /* PWM duty cycle */

  if (mode == SLVDSP1103_PWM_MODE_SYM)
  {
    duty_scaled = (Int32) ((Float64) period_scaled * (1.0 - duty));
  }
  else
  {
    duty_scaled = (Int32) ((Float64) period_scaled * duty);
  }

  /* duty cycle range check */

  if (duty_scaled < 0 || duty_scaled > period_scaled)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM_RANGE_ERROR1,
      SLVDSP1103_MSG_PWM_RANGE_ERROR1_TXT, "duty");
    exit(1);
  }

  /* setup parameters and send command to slave */

  parms[DSPDEF_PWM_INIT_PRD_LOC] = (unsigned long) period_scaled;
  parms[DSPDEF_PWM_INIT_DTY_LOC] = (unsigned long) duty_scaled;
  parms[DSPDEF_PWM_INIT_SCL_LOC] = (unsigned long) prescale;
  parms[DSPDEF_PWM_INIT_MOD_LOC] = (unsigned long) mode;
  parms[DSPDEF_PWM_INIT_POL_LOC] = (unsigned long) pol;
  parms[DSPDEF_PWM_INIT_MSK_LOC] = (unsigned long) mask;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM_INIT,
    DSPDEF_PWM_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  slvdsp1103_pwm_sel_mask |= mask;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_PWM_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   Simple PWM duty cycle update
*
* SYNTAX
*   void ds1103_slave_dsp_pwm_duty_write_register (Int16 task_id, Int16 *index,
*     UInt16 channel)
*   Int16 ds1103_slave_dsp_pwm_duty_write (Int16 task_id, Int32 index,
*     Float64 duty)
*
* DESCRIPTION
*   The duty cycle of the specified PWM channel is updated to a new value.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             PWM channel number (1..4)
*   duty                duty cycle value (0..1.0)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   PWM generation must have been initialized by using the function
*   ds1103_slave_dsp_pwm_init().
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_pwm_duty_write_register (Int16 task_id, Int16 *index,
  UInt32 channel)
{
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  if (channel < 1 || channel > 4)
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_PWM_RANGE_ERROR2);
    exit(1);
  }

  if ((slvdsp1103_pwm_sel_mask & 1 << (channel-1)) == 0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM_INIT_ERROR,
      SLVDSP1103_MSG_PWM_INIT_ERROR_TXT, channel);
    exit(1);
  }

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_PWM_WRITE,
    DSPDEF_PWM_WRITE_DINCNT, DSPDEF_PWM_WRITE_DOUTCNT,
    DSPDEF_PWM_WRITE_PCNT, (unsigned long *) &channel);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM_WRITE_REG_INFO,
      SLVDSP1103_MSG_PWM_WRITE_REG_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_pwm_duty_write (Int16 task_id, Int32 index,
  Float64 duty)
{
  unsigned long parm;
  Int16 error;

#ifdef DS1103_RANGE_CHECK

  Int32 duty_scaled;

  if (slvdsp1103_pwm_mode == SLVDSP1103_PWM_MODE_SYM)
  {
    duty_scaled = (Int32) (slvdsp1103_pwm_period * (1.0 - duty));
  }
  else
  {
    duty_scaled = (Int32) (slvdsp1103_pwm_period * duty);
  }

  /* duty cycle range check */

  if (duty_scaled < 0)
  {
    parm = 0;
  }
  else if (duty_scaled > slvdsp1103_pwm_period)
  {
    parm = (unsigned long) slvdsp1103_pwm_period;
  }
  else
  {
    parm = (unsigned long) duty_scaled;
  }

#else

  if (slvdsp1103_pwm_mode == SLVDSP1103_PWM_MODE_SYM)
  {
    parm = (unsigned long) (slvdsp1103_pwm_period * (1.0 - duty));
  }
  else
  {
    parm = (unsigned long) (slvdsp1103_pwm_period * duty);
  }

#endif /* DS1103_RANGE_CHECK */

  /* send update command to slave */

  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) &parm);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Simple PWM start and stop
*
* SYNTAX
*   Int16 ds1103_slave_dsp_pwm_start (Int16 task_id, UInt32 mask)
*   Int16 ds1103_slave_dsp_pwm_stop (Int16 task_id, UInt32 mask)
*
* DESCRIPTION
*   The PWM channels selected by 'mask' are started or stopped, respectively.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   mask                PWM channel mask (combination of
*                         SLVDSP1103_PWM_CH1_MSK, SLVDSP1103_PWM_CH2_MSK,
*                         SLVDSP1103_PWM_CH3_MSK, SLVDSP1103_PWM_CH4_MSK)
*
* RETURNS
*   errorcode
*
* REMARKS
*   PWM generation must have been initialized by using the function
*   ds1103_slave_dsp_pwm_init().
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*
******************************************************************************/

__INLINE Int16 ds1103_slave_dsp_pwm_start (Int16 task_id, UInt32 mask)
{
  Int16 error;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM_START,
    DSPDEF_PWM_START_PCNT, (unsigned long *) &mask);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_pwm_stop (Int16 task_id, UInt32 mask)
{
  Int16 error;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM_STOP,
    DSPDEF_PWM_STOP_PCNT, (unsigned long *) &mask);

  return(error);
}

/******************************************************************************
*
* FUNCTION
*   Sets simple PWM outputs.
*
* SYNTAX
*   Int16 ds1103_slave_dsp_pwm_output_set (Int16 task_id, UInt32 mask,
*     UInt32 state)
*
* DESCRIPTION
*   The PWM output of the selected channels are set as specified by the
*   parameter 'state'.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   mask                PWM channel mask (combination of
*                         SLVDSP1103_PWM_CH1_MSK, SLVDSP1103_PWM_CH2_MSK,
*                         SLVDSP1103_PWM_CH3_MSK, SLVDSP1103_PWM_CH4_MSK)
*   state               state of the PMW outputs
*                         SLVDSP1103_PWM_TTL_LOW,
*                         SLVDSP1103_PWM_TTL_HIGH,
*                         SLVDSP1103_PWM_ACTIVE_LOW,
*                         SLVDSP1103_PWM_ACTIVE_HIGH
*
* RETURNS
*   errorcode
*
* REMARKS
*   PWM generation must have been initialized by using the function
*   ds1103_slave_dsp_pwm_init().
*   Digital I/O port 2 bits 0, 1, 2, 4 conflict with simple PWM channels 1..4.
*
******************************************************************************/

__INLINE Int16 ds1103_slave_dsp_pwm_output_set (Int16 task_id, UInt32 mask,
  UInt32 state)
{
  Int16 error;
  unsigned long parms[DSPDEF_PWM_OUT_SET_PCNT];

  parms[DSPDEF_PWM_OUT_SET_MSK_LOC] = (unsigned long) mask;
  parms[DSPDEF_PWM_OUT_SET_OS_LOC]  = (unsigned long) state;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM_OUT_SET,
    DSPDEF_PWM_OUT_SET_PCNT, (unsigned long *) parms);

  return(error);
}

/******************************************************************************
*
* FUNCTION
*   3-phase PWM initialization
*
* SYNTAX
*   void ds1103_slave_dsp_pwm3_init (Int16 task_id, Float64 period,
*     Float64 duty1, Float64 duty2, Float64 duty3, Float64 dead_band,
*     Float64 sync_pos)
*
* DESCRIPTION
*   The 3-phase PWM generation is initialized.
*
*   pin assignment:     signal  | non-inverted | inverted
*                       --------|--------------|---------
*                       phase1  | SPWM1        | SPWM2
*                       phase2  | SPWM3        | SPWM4
*                       phase3  | SPWM5        | SPWM6
*
*   ranges:             period min | period max
*                       -----------|-----------
*                       200 ns     | 800 ms
*
*   resolution:         period     | resolution
*                       -----------|-----------
*                       <   6.4 ms |  100 ns
*                       <  12.8 ms |  200 ns
*                       <  25.6 ms |  400 ns
*                       <  51.2 ms |  800 ns
*                       < 102.4 ms |  1.6 us
*                       < 204.8 ms |  3.2 us
*                       < 409.6 ms |  6.4 us
*                       < 819.2 ms | 12.8 us
*
* PARAMETERS
*   task_id             task ID of communication channel
*   period              PWM period (0..800ms)
*   duty1               initial duty cycle of PWM phase 1 (0..1.0)
*   duty2               initial duty cycle of PWM phase 2 (0..1.0)
*   duty3               initial duty cycle of PWM phase 3 (0..1.0)
*   dead_band           dead-band period (0..100us)
*   sync_pos            sync interrupt position (0..1.0)
*                       SLVDSP1103_PWM3_SYNC_OFF
*                       SLVDSP1103_PWM3_SYNC_LEFT
*                       SLVDSP1103_PWM3_SYNC_CENT
*
*
* RETURNS
*   void
*
* REMARKS
*   D/F square wave generation channels 1..3 conflict with PWM3 generation.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_pwm3_init (Int16 task_id, Float64 period, Float64 duty1,
  Float64 duty2, Float64 duty3, Float64 dead_band, Float64 sync_pos)
{
  UInt16 period_scaled, prescale = 0;
  Int32  duty1_scaled, duty2_scaled, duty3_scaled;
  UInt16 period_db_scaled, prescale_db = 0;
  Int32  syncpos_scaled = 0;
  unsigned long parms[DSPDEF_PWM3_INIT_PCNT];
  Int16 error;


  /* PWM input clock prescaling */
  
  if (period < 6.4e-3)
    prescale = 0x00;
  else if (period < 12.8e-3)
    prescale = 0x01;
  else if (period < 25.6e-3)
    prescale = 0x02;
  else if (period < 51.2e-3)
    prescale = 0x03;
  else if (period < 102.4e-3)
    prescale = 0x04;
  else if (period < 204.8e-3)
    prescale = 0x05;
  else if (period < 409.6e-3)
    prescale = 0x06;
  else if (period < 819.2e-3)
    prescale = 0x07;
  else
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM3_RANGE_ERROR,
      SLVDSP1103_MSG_PWM3_RANGE_ERROR_TXT, "period");
    exit(1);
  }

  /* PWM period */

  period_scaled = (UInt16) (period * SLVDSP1103_CPU_CLK /
    (Float64) (1 << prescale) / 2);
  slvdsp1103_pwm3_period = (Float64) period_scaled;

  /* PWM duty cycles */

  duty1_scaled = (Int32) ((Float64) period_scaled * (1.0 - duty1));
  duty2_scaled = (Int32) ((Float64) period_scaled * (1.0 - duty2));
  duty3_scaled = (Int32) ((Float64) period_scaled * (1.0 - duty3));

  /* duty cycle range check */

  if (duty1_scaled < 0 || duty1_scaled > period_scaled)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM3_RANGE_ERROR,
      SLVDSP1103_MSG_PWM3_RANGE_ERROR_TXT, "duty1");
    exit(1);
  }

  if (duty2_scaled < 0 || duty2_scaled > period_scaled)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM3_RANGE_ERROR,
      SLVDSP1103_MSG_PWM3_RANGE_ERROR_TXT, "duty2");
    exit(1);
  }

  if (duty3_scaled < 0 || duty3_scaled > period_scaled)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM3_RANGE_ERROR,
      SLVDSP1103_MSG_PWM3_RANGE_ERROR_TXT, "duty3");
    exit(1);
  }

  /* dead-band clock prescaling */

  if (dead_band < 12.6e-6)
    prescale_db = 0x00;
  else if (dead_band < 25.2e-6)
    prescale_db = 0x01;
  else if (dead_band < 50.4e-6)
    prescale_db = 0x02;
  else if (dead_band < 100.8e-6)
    prescale_db = 0x03;
  else
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM3_RANGE_ERROR,
      SLVDSP1103_MSG_PWM3_RANGE_ERROR_TXT, "dead_band");
    exit(1);
  }

  /* dead-band period */

  period_db_scaled = (UInt16) (dead_band * SLVDSP1103_CPU_CLK /
    (Float64) (1 << prescale_db));

  /* PWM sync. interrupt position */

  /* check predefined symbol, which is mapped for compatibility */
  if (sync_pos == SLVDSP1103_PWM3_SYNC_CENT)
    sync_pos = 0.5;

  if ((sync_pos <= 0) || (sync_pos > 1))
    syncpos_scaled = 0;                           /* disable sync. interrupt */
  else
    /* calculate scaled sync. interrupt position */
    syncpos_scaled = (Int32) ((Float64) period_scaled * sync_pos);

  /* add offset, required for version identification on slave DSP */
  /* the values 0, 1 and 2 are reserved for older predefined symbols */
  syncpos_scaled += 10;

  /* setup parameters and send command to slave */

  parms[DSPDEF_PWM3_INIT_PRD_LOC]     = (unsigned long) period_scaled;
  parms[DSPDEF_PWM3_INIT_DTY1_LOC]    = (unsigned long) duty1_scaled;
  parms[DSPDEF_PWM3_INIT_DTY2_LOC]    = (unsigned long) duty2_scaled;
  parms[DSPDEF_PWM3_INIT_DTY3_LOC]    = (unsigned long) duty3_scaled;
  parms[DSPDEF_PWM3_INIT_SCL_LOC]     = (unsigned long) prescale;
  parms[DSPDEF_PWM3_INIT_PRDDB_LOC]   = (unsigned long) period_db_scaled;
  parms[DSPDEF_PWM3_INIT_SCLDB_LOC]   = (unsigned long) prescale_db;
  parms[DSPDEF_PWM3_INIT_SYNCPOS_LOC] = (unsigned long) syncpos_scaled;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM3_INIT,
    DSPDEF_PWM3_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_PWM3_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   3-phase PWM duty cycle update
*
* SYNTAX
*   void ds1103_slave_dsp_pwm3_duty_write_register (Int16 task_id,
*     Int16 *index)
*   Int16 ds1103_slave_dsp_pwm3_duty_write (Int16 task_id, Int32 index,
*     Float64 duty1, Float64 duty2, Float64 duty3)
*
* DESCRIPTION
*   The duty cycles of the 3-phase PWM are updated to new values.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   duty1               duty cycle value of phase 1 (0..1.0)
*   duty2               duty cycle value of phase 2 (0..1.0)
*   duty3               duty cycle value of phase 3 (0..1.0)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   3-phase PWM generation must have been initialized by using the function
*   ds1103_slave_dsp_pwm3_init().
*   D/F square wave generation channels 1..3 conflict with PWM3 generation.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_pwm3_duty_write_register (Int16 task_id, Int16 *index)
{
  unsigned long dummy;
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_PWM3_WRITE,
    DSPDEF_PWM3_WRITE_DINCNT, DSPDEF_PWM3_WRITE_DOUTCNT,
    DSPDEF_PWM3_WRITE_PCNT, (unsigned long *) &dummy);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_PWM3_WRITE_REG_INFO);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_pwm3_duty_write (Int16 task_id, Int32 index,
  Float64 duty1, Float64 duty2, Float64 duty3)
{
  unsigned long parms[DSPDEF_PWM3_WRITE_DOUTCNT];
  Int16 error;

#ifdef DS1103_RANGE_CHECK

  Int32 duty_scaled;

  /* duty cycle scaling and range check */

  duty_scaled = (Int32) (slvdsp1103_pwm3_period * (1.0 - duty1));
  if (duty_scaled < 0)
  {
    parms[0] = 0;
  }
  else if (duty_scaled > slvdsp1103_pwm3_period)
  {
    parms[0] = (unsigned long) slvdsp1103_pwm3_period;
  }
  else
  {
    parms[0] = (unsigned long) duty_scaled;
  }

  duty_scaled = (Int32) (slvdsp1103_pwm3_period * (1.0 - duty2));
  if (duty_scaled < 0)
  {
    parms[1] = 0;
  }
  else if (duty_scaled > slvdsp1103_pwm3_period)
  {
    parms[1] = (unsigned long) slvdsp1103_pwm3_period;
  }
  else
  {
    parms[1] = (unsigned long) duty_scaled;
  }

  duty_scaled = (Int32) (slvdsp1103_pwm3_period * (1.0 - duty3));
  if (duty_scaled < 0)
  {
    parms[2] = 0;
  }
  else if (duty_scaled > slvdsp1103_pwm3_period)
  {
    parms[2] = (unsigned long) slvdsp1103_pwm3_period;
  }
  else
  {
    parms[2] = (unsigned long) duty_scaled;
  }

#else

  parms[0] = (unsigned long) (slvdsp1103_pwm3_period * (1.0 - duty1));
  parms[1] = (unsigned long) (slvdsp1103_pwm3_period * (1.0 - duty2));
  parms[2] = (unsigned long) (slvdsp1103_pwm3_period * (1.0 - duty3));

#endif /* DS1103_RANGE_CHECK */

  /* send update command to slave */

  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) parms);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   3-phase space vector PWM initialization
*
* SYNTAX
*   void ds1103_slave_dsp_pwm3sv_init (Int16 task_id, Float64 period,
*     UInt16 sector, Float64 t1, Float64 t2, Float64 dead_band,
*     Float64 sync_pos)
*
* DESCRIPTION
*   The space vector PWM generation is initialized.
*
*   pin assignment:     signal  | non-inverted | inverted
*                       --------|--------------|---------
*                       phase1  | SPWM1        | SPWM2
*                       phase2  | SPWM3        | SPWM4
*                       phase3  | SPWM5        | SPWM6
*
*   ranges:             period min | period max
*                       -----------|-----------
*                       200 ns     | 800 ms
*
*   resolution:         period     | resolution
*                       -----------|-----------
*                       <   6.4 ms |  100 ns
*                       <  12.8 ms |  200 ns
*                       <  25.6 ms |  400 ns
*                       <  51.2 ms |  800 ns
*                       < 102.4 ms |  1.6 us
*                       < 204.8 ms |  3.2 us
*                       < 409.6 ms |  6.4 us
*                       < 819.2 ms | 12.8 us
*
* PARAMETERS
*   task_id             task ID of communication channel
*   period              PWM period (0..800ms)
*   sector              initial sector (1..6)
*   t1                  initial on-time t1
*   t2                  initial on-time t2
*   dead_band           dead-band period (0..100us)
*   sync_pos            sync interrupt position (0..1.0)
*                       SLVDSP1103_PWM3_SYNC_OFF
*                       SLVDSP1103_PWM3_SYNC_LEFT
*                       SLVDSP1103_PWM3_SYNC_CENT
*
*
* RETURNS
*   void
*
* REMARKS
*   D/F square wave generation channels 1..3 conflict with PWM3 generation.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_pwm3sv_init (Int16 task_id, Float64 period,
  UInt16 sector, Float64 t1, Float64 t2, Float64 dead_band, Float64 sync_pos)
{
  UInt16 prescale = 0;
  Int32 period_scaled, t1_scaled, t1_t2_scaled;
  UInt16 period_db_scaled, prescale_db = 0;
  Int32  syncpos_scaled = 0;
  unsigned long parms[DSPDEF_PWM3SV_INIT_PCNT];
  Int16 error;


  /* PWM input clock prescaling */

  if (period < 6.4e-3)
    prescale = 0x00;
  else if (period < 12.8e-3)
    prescale = 0x01;
  else if (period < 25.6e-3)
    prescale = 0x02;
  else if (period < 51.2e-3)
    prescale = 0x03;
  else if (period < 102.4e-3)
    prescale = 0x04;
  else if (period < 204.8e-3)
    prescale = 0x05;
  else if (period < 409.6e-3)
    prescale = 0x06;
  else if (period < 819.2e-3)
    prescale = 0x07;
  else
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM3SV_RANGE_ERROR,
      SLVDSP1103_MSG_PWM3SV_RANGE_ERROR_TXT, "period");
    exit(1);
  }

  /* PWM period and duty cycle scaling */

  period_scaled = (Int32) (period * SLVDSP1103_CPU_CLK /
    (Float64) (1 << prescale) / 2);

  t1_scaled = (Int32) (t1 * SLVDSP1103_CPU_CLK /                     /* t1/2 */
    (Float64) (1 << prescale) / 2);

  t1_t2_scaled = (Int32) ((t1 + t2) * SLVDSP1103_CPU_CLK /      /* t1/2+t2/2 */
    (Float64) (1 << prescale) / 2);

  /* workaround for F240 basic space vector switching problem */

  if (t1_scaled == 0)
  {
    t1_scaled = 1;
    t1_t2_scaled += 1;
  }

  /* duty cycle range check */

  if (t1_scaled < 0 || t1_t2_scaled - t1_scaled < 0 ||
    t1_t2_scaled > period_scaled)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM3SV_RANGE_ERROR,
      SLVDSP1103_MSG_PWM3SV_RANGE_ERROR_TXT, "t1 and/or t2");
    exit(1);
  }

  /* dead-band clock prescaling */

  if (dead_band < 12.6e-6)
    prescale_db = 0x00;
  else if (dead_band < 25.2e-6)
    prescale_db = 0x01;
  else if (dead_band < 50.4e-6)
    prescale_db = 0x02;
  else if (dead_band < 100.8e-6)
    prescale_db = 0x03;
  else
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM3SV_RANGE_ERROR,
      SLVDSP1103_MSG_PWM3SV_RANGE_ERROR_TXT, "dead_band");
    exit(1);
  }

  /* dead-band period scaling */

  period_db_scaled = (UInt16) (dead_band * SLVDSP1103_CPU_CLK /
    (Float64) (1 << prescale_db));

  /* save period and prescale factor */

  slvdsp1103_pwm3sv_period_scaled = period_scaled;
  slvdsp1103_pwm3sv_prescale = prescale;

  /* PWM sync. interrupt position */

  /* check predefined symbol, which is mapped for compatibility */
  if (sync_pos == SLVDSP1103_PWM3_SYNC_CENT)
    sync_pos = 0.5;

  if ((sync_pos <= 0) || (sync_pos > 1))
    syncpos_scaled = 0;                           /* disable sync. interrupt */
  else
    /* calculate scaled sync. interrupt position */
    syncpos_scaled = (Int32) ((Float64) period_scaled * sync_pos);

  /* add offset, required for version identification on slave DSP */
  /* the values 0, 1 and 2 are reserved for older predefined symbols */
  syncpos_scaled += 10;

  /* setup parameters and send command to slave */

  parms[DSPDEF_PWM3SV_INIT_PRD_LOC]     = (unsigned long) period_scaled;
  parms[DSPDEF_PWM3SV_INIT_T1_LOC]      = (unsigned long) t1_scaled;
  parms[DSPDEF_PWM3SV_INIT_T12_LOC]     = (unsigned long) t1_t2_scaled;
  parms[DSPDEF_PWM3SV_INIT_SEC_LOC]     = (unsigned long) sector;
  parms[DSPDEF_PWM3SV_INIT_SCL_LOC]     = (unsigned long) prescale;
  parms[DSPDEF_PWM3SV_INIT_PRDDB_LOC]   = (unsigned long) period_db_scaled;
  parms[DSPDEF_PWM3SV_INIT_SCLDB_LOC]   = (unsigned long) prescale_db;
  parms[DSPDEF_PWM3SV_INIT_SYNCPOS_LOC] = (unsigned long) syncpos_scaled;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM3SV_INIT,
    DSPDEF_PWM3SV_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_PWM3SV_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   3-phase space vector PWM duty cycle update
*
* SYNTAX
*   void ds1103_slave_dsp_pwm3sv_duty_write_register (Int16 task_id,
*     Int16 *index)
*   Int16 ds1103_slave_dsp_pwm3sv_duty_write (Int16 task_id, Int32 index,
*     UInt16 sector, Float64 t1, Float64 t2)
*
* DESCRIPTION
*   The duty cycles of the 3-phase space vector PWM are updated to new values.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   sector              sector (1..6)
*   t1                  time period t1
*   t2                  time period t2
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   3-phase space vector PWM generation must have been initialized by using
*   the function ds1103_slave_dsp_pwm3sv_init().
*   D/F square wave generation channels 1..3 conflict with PWM3 generation.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_pwm3sv_duty_write_register (Int16 task_id, Int16 *index)
{
  unsigned long dummy;
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32,
    DSPDEF_PWM3SV_WRITE, DSPDEF_PWM3SV_WRITE_DINCNT,
    DSPDEF_PWM3SV_WRITE_DOUTCNT, DSPDEF_PWM3SV_WRITE_PCNT,
    (unsigned long *) &dummy);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_PWM3SV_WRITE_REG_INFO);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_pwm3sv_duty_write (Int16 task_id, Int32 index,
  UInt16 sector, Float64 t1, Float64 t2)
{
  unsigned long parms[DSPDEF_PWM3SV_WRITE_DOUTCNT];
  Int32 t1_scaled, t1_t2_scaled;
  Int16 error;

  /* duty cycle scaling */

  t1_scaled = (Int32) (t1 * SLVDSP1103_CPU_CLK /                     /* t1/2 */
    (Float64) (1 << slvdsp1103_pwm3sv_prescale) / 2);

  t1_t2_scaled = (Int32) ((t1 + t2) * SLVDSP1103_CPU_CLK /      /* t1/2+t2/2 */
    (Float64) (1 << slvdsp1103_pwm3sv_prescale) / 2);

  /* workaround for F240 basic space vector switching problem */

  if (t1_scaled == 0)
  {
    t1_scaled = 1;
    t1_t2_scaled += 1;
  }

#ifdef DS1103_RANGE_CHECK
  /* duty cycle range check */

  if (t1_scaled < 0 || t1_t2_scaled - t1_scaled < 0 ||
    t1_t2_scaled > slvdsp1103_pwm3sv_period_scaled)
  {
    return(SLVDSP1103_RANGE_ERROR);
  }
#endif

  parms[0] = (unsigned long) t1_scaled;
  parms[1] = (unsigned long) t1_t2_scaled;
  parms[2] = sector;

  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) parms);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   3-phase PWM start and stop (standard or space vector PWM)
*
* SYNTAX
*   Int16 ds1103_slave_dsp_pwm3_start (Int16 task_id)
*   Int16 ds1103_slave_dsp_pwm3_stop (Int16 task_id)
*
* DESCRIPTION
*   The 3-phase PWM is started or stopped, respectively.
*
* PARAMETERS
*   task_id             task ID of communication channel
*
* RETURNS
*   errorcode
*
* REMARKS
*   3-phase PWM generation must have been initialized by using the function
*   ds1103_slave_dsp_pwm3_init() or ds1103_slave_dsp_pwm3sv_init().
*   D/F square wave generation channels 1..3 conflict with PWM3 generation.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*
******************************************************************************/

__INLINE Int16 ds1103_slave_dsp_pwm3_start (Int16 task_id)
{
  unsigned long dummy;
  Int16 error;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM3_START,
    DSPDEF_PWM3_START_PCNT, (unsigned long *) &dummy);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_pwm3_stop (Int16 task_id)
{
  unsigned long dummy;
  Int16 error;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM3_STOP,
    DSPDEF_PWM3_STOP_PCNT, (unsigned long *) &dummy);

  return(error);
}

/******************************************************************************
*
* FUNCTION
*   Sets the 3-phase PWM outputs (standard or space vector PWM).
*
* SYNTAX
*   Int16 ds1103_slave_dsp_pwm3_output_set (Int16 task_id, UInt32 int_mode,
*     UInt32 state1, UInt32 state2, UInt32 state3,
*     UInt32 state4, UInt32 state5, UInt32 state6)
*
* DESCRIPTION
*   The 3-phase PWM outputs are set as specified by the corresponding
*   parameter 'state'.
*
* PARAMETERS
*   task_id             task ID of communication channel
*
*   int_mode            specifies the PWM interrupt mode:
*                         SLVDSP1103_PWM3_INT_ON
*                         SLVDSP1103_PWM3_INT_OFF
*
*   state1..6           state of the PMW outputs:
*                         SLVDSP1103_PWM3_TTL_LOW,
*                         SLVDSP1103_PWM3_TTL_HIGH,
*                         SLVDSP1103_PWM3_ACTIVE_LOW,
*                         SLVDSP1103_PWM3_ACTIVE_HIGH
*
*   pin assignment:     parameter  | pin     | signal
*                       -----------|--------------------------------
*                       state1     | SPWM1   | phase1 (non-inverted)
*                       state2     | SPWM2   |        (inverted)
*                       state3     | SPWM3   | phase2 (non-inverted)
*                       state4     | SPWM4   |        (inverted)
*                       state5     | SPWM5   | phase3 (non-inverted)
*                       state6     | SPWM6   |        (inverted)
*
* RETURNS
*   errorcode
*
* REMARKS
*   3-phase PWM generation must have been initialized by using the function
*   ds1103_slave_dsp_pwm3_init() or ds1103_slave_dsp_pwm3sv_init().
*   D/F square wave generation channels 1..3 conflict with PWM3 generation.
*   Digital I/O port 2 bit 3 conflicts with PWM3 sync-interrupt.
*
******************************************************************************/

__INLINE Int16 ds1103_slave_dsp_pwm3_output_set (Int16 task_id, UInt32 int_mode,
  UInt32 state1, UInt32 state2, UInt32 state3, UInt32 state4, UInt32 state5,
  UInt32 state6)

{
  Int16 error;
  unsigned long parms[DSPDEF_PWM3_OUT_SET_PCNT];

  parms[DSPDEF_PWM3_OUT_SET_MSK_LOC] =  state1       |
                                       (state2 << 2) |
                                       (state3 << 4) |
                                       (state4 << 6) |
                                       (state5 << 8) |
                                       (state6 << 10);

  parms[DSPDEF_PWM3_OUT_SET_INT_LOC] =  int_mode;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM3_OUT_SET,
    DSPDEF_PWM3_OUT_SET_PCNT, (unsigned long *) parms);

  return(error);
}

/******************************************************************************
*
* FUNCTION
*   D/F variable frequency square wave generation initialization
*
* SYNTAX
*   void ds1103_slave_dsp_d2f_init (Int16 task_id, UInt16 range, Float64 freq,
*     UInt16 ch4_enable)
*
* DESCRIPTION
*   The D/F square wave generation is initialized for channels 1..3. An
*   optional channel 4 is initialized depending on the parameter 'ch4_enable'.
*
*   pin assignment:     channel | pin
*                       --------|-------
*                          1    | SPWM1
*                          2    | SPWM3
*                          3    | SPWM5
*                          4    | ST2PWM
*
*   ranges (ch 1..3):   range | frequency range   | resolution
*                       ------|-------------------|-----------
*                         1   | 320 Hz .. 35 kHz  |  100 ns
*                         2   | 160 Hz .. 35 kHz  |  200 ns
*                         3   |  80 Hz .. 35 kHz  |  400 ns
*                         4   |  40 Hz .. 35 kHz  |  800 ns
*                         5   |  20 Hz .. 35 kHz  |  1.6 us
*                         6   |  10 Hz .. 35 kHz  |  3.2 us
*                         7   |   5 Hz .. 35 kHz  |  6.4 us
*                         8   | 2.5 Hz .. 35 kHz  | 12.8 us
*
*   The maximum frequency limit is an approximation. The actual limit depends
*   on the number of interrupts, i.e. on the number and frequency of D/F
*   signals being generated.
*
*   ranges (ch 4):      range | frequency range   | resolution
*                       ------|-------------------|-----------
*                         1   | 320 Hz .. 4.8 MHz |  100 ns
*                         2   | 160 Hz .. 2.4 MHz |  200 ns
*                         3   |  80 Hz .. 1.2 MHz |  400 ns
*                         4   |  40 Hz .. 600 kHz |  800 ns
*                         5   |  20 Hz .. 300 kHz |  1.6 us
*                         6   |  10 Hz .. 150 kHz |  3.2 us
*                         7   |   5 Hz ..  75 kHz |  6.4 us
*
*  A frequency value beyond the minimum frequency limits starts the frequency
*  generation with f = 0 (all outputs reset).
*  Range 8 is available only if channel 4 is not used.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   range               frequency range (see table above)
*   freq                initial frequency
*   ch4_enable          D/F channel 4 enable (SLVDSP1103_D2F_CH4_DISABLE,
*                         SLVDSP1103_D2F_CH4_ENABLE)
*
* RETURNS
*   void
*
* REMARKS
*   D/F square wave generation channels 1..3 conflict with PWM3 generation.
*   D/F square wave generation channel 4 conflicts with simple PWM channel 1.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_d2f_init (Int16 task_id, UInt16 range, Float64 freq,
  UInt16 ch4_enable)
{
  Float64 period_half;
  UInt16 period_half_scaled;
  unsigned long parms[DSPDEF_D2F_INIT_PCNT];
  Int16 error;

  if ((!ch4_enable && range >= 1 && range <= 8) ||
      (ch4_enable && range >= 1 && range <= 7))
  {
    slvdsp1103_d2f_prescale = range - 1;
  }
  else
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_D2F_RANGE_ERROR);
    exit(1);
  }

  if (freq <= 0.0)
  {
    period_half_scaled = 0xFFFF;                        /* force D/F disable */
  }
  else
  {
    period_half = 0.5 / freq;
    if (period_half >= 1.6e-3 * (Float64) (1 << slvdsp1103_d2f_prescale))
    {
      period_half_scaled = 0xFFFF;                      /* force D/F disable */
    }
    else
    {
      period_half_scaled = (UInt16) (period_half * SLVDSP1103_CPU_CLK /
        (Float64) (1 << slvdsp1103_d2f_prescale));
    }
  }

  /* setup parameters and send command to slave */

  parms[DSPDEF_D2F_INIT_PRD_LOC] = period_half_scaled;
  parms[DSPDEF_D2F_INIT_SCL_LOC] = slvdsp1103_d2f_prescale;
  parms[DSPDEF_D2F_INIT_CH4_LOC] = ch4_enable;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_D2F_INIT,
    DSPDEF_D2F_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  slvdsp1103_d2f_sel_mask =
    SLVDSP1103_D2F_CH1_MSK |
    SLVDSP1103_D2F_CH2_MSK |
    SLVDSP1103_D2F_CH3_MSK;

  if (ch4_enable == SLVDSP1103_D2F_CH4_ENABLE)
  {
    slvdsp1103_d2f_sel_mask |= SLVDSP1103_D2F_CH4_MSK;
  }

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_D2F_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   D/F frequency update
*
* SYNTAX
*   void ds1103_slave_dsp_d2f_write_register (Int16 task_id, Int16 *index,
*     UInt16 channel)
*   Int16 ds1103_slave_dsp_d2f_write (Int16 task_id, Int32 index, Float64 freq)
*
* DESCRIPTION
*   The output frequency of the specified channel is updated to a new value.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             output channel (1..3, 4 if enabled)
*   freq                frequency value (Hz)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   Frequency generation must have been initialized by using the function
*   ds1103_slave_dsp_d2f_init().
*   Channel 4 can be used only if it has been enabled during initialization.
*   D/F square wave generation channels 1..3 conflict with PWM3 generation.
*   D/F square wave generation channel 4 conflicts with simple PWM channel 1.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_d2f_write_register (Int16 task_id, Int16 *index,
  unsigned long channel)
{
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  if ((slvdsp1103_d2f_sel_mask & 1 << (channel-1)) == 0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_D2F_INIT_ERROR,
       SLVDSP1103_MSG_D2F_INIT_ERROR_TXT, channel);
    exit(1);
  }

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_D2F_WRITE,
    DSPDEF_D2F_WRITE_DINCNT, DSPDEF_D2F_WRITE_DOUTCNT,
    DSPDEF_D2F_WRITE_PCNT, (unsigned long *) &channel);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_D2F_WRITE_REG_INFO,
      SLVDSP1103_MSG_D2F_WRITE_REG_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_d2f_write (Int16 task_id, Int32 index,
  Float64 freq)
{
  Float64 period_half;
  unsigned long period_half_scaled;
  Int16 error;
  /* need to work around a bogus division by zero warning (with MCC 3.3) */
  volatile Float64 divisor = freq;


  if (divisor == 0.0)
  {
    period_half_scaled = 0xFFFF;                        /* force D/F disable */
  }
  else
  {
    period_half = 0.5 / divisor;
    if (period_half >= 1.6e-3 * (Float64) (1 << slvdsp1103_d2f_prescale))
    {
      period_half_scaled = 0xFFFF;                      /* force D/F disable */
    }
    else
    {
      period_half_scaled = (UInt16) (period_half * SLVDSP1103_CPU_CLK /
        (Float64) (1 << slvdsp1103_d2f_prescale));
    }
  }

  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) &period_half_scaled);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   F/D frequency measurement initialization
*
* SYNTAX
*   void ds1103_slave_dsp_f2d_init (Int16 task_id, UInt16 range, UInt16 mask)
*
* DESCRIPTION
*   The capture unit is initialized for F/D frequency measurement.
*
*   pin assignment:     channel | pin
*                       --------|-----
*                          1    | CAP1
*                          2    | CAP2
*                          3    | CAP3
*                          4    | CAP4
*
*   ranges:             channels | frequency range | resolution
*                       ---------|-----------------|-----------
*                          1     | fmin .. 80 kHz  | 50 ns
*                          2     | fmin .. 40 kHz  | 50 ns
*                          3     | fmin .. 25 kHz  | 50 ns
*                          4     | fmin .. 20 kHz  | 50 ns
*
* PARAMETERS
*   task_id             task ID of communication channel
*   fmin1..fmin4        minimum frequency for zero frequency detection
*                       (0.005 Hz .. 150 Hz)
*
* RETURNS
*   void
*
* REMARKS
*   F/D frequency measurement conflicts with PWM measurement.
*   Digital I/O port 3 bits 4..7 conflict with F/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_f2d_init (Int16 task_id, Float64 fmin1, Float64 fmin2,
  Float64 fmin3, Float64 fmin4)
{
  unsigned long parms[DSPDEF_F2D_INIT_PCNT];
  Int16 error;

  if (fmin1 < 0.005 | fmin1 > 150.0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_F2D_RANGE_ERROR,
      SLVDSP1103_MSG_F2D_RANGE_ERROR_TXT, "fmin1");
    exit(1);
  }

  if (fmin2 < 0.005 | fmin2 > 150.0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_F2D_RANGE_ERROR,
      SLVDSP1103_MSG_F2D_RANGE_ERROR_TXT, "fmin2");
    exit(1);
  }

  if (fmin3 < 0.005 | fmin3 > 150.0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_F2D_RANGE_ERROR,
      SLVDSP1103_MSG_F2D_RANGE_ERROR_TXT, "fmin3");
    exit(1);
  }

  if (fmin4 < 0.005 | fmin4 > 150.0)
  {
    msg_error_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_F2D_RANGE_ERROR,
      SLVDSP1103_MSG_F2D_RANGE_ERROR_TXT, "fmin4");
    exit(1);
  }

  slvdsp1103_f2d_fmin[1] = fmin1;
  slvdsp1103_f2d_fmin[2] = fmin2;
  slvdsp1103_f2d_fmin[3] = fmin3;
  slvdsp1103_f2d_fmin[4] = fmin4;

  parms[DSPDEF_F2D_INIT_MAX1_LOC] =
    (UInt16) ((UInt32) (SLVDSP1103_CPU_CLK / fmin1) >> 16);
  parms[DSPDEF_F2D_INIT_MAX2_LOC] =
    (UInt16) ((UInt32) (SLVDSP1103_CPU_CLK / fmin2) >> 16);
  parms[DSPDEF_F2D_INIT_MAX3_LOC] =
    (UInt16) ((UInt32) (SLVDSP1103_CPU_CLK / fmin3) >> 16);
  parms[DSPDEF_F2D_INIT_MAX4_LOC] =
    (UInt16) ((UInt32) (SLVDSP1103_CPU_CLK / fmin4) >> 16);

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_F2D_INIT,
    DSPDEF_F2D_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_F2D_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   F/D frequency measurement
*
* SYNTAX
*   void ds1103_slave_dsp_f2d_read_register (Int16 task_id, Int16 *index,
*     UInt16 channel, UInt16 int_enable)
*   Int16 ds1103_slave_dsp_f2d_read_request (Int16 task_id, Int32 index)
*   Int16 ds1103_slave_dsp_f2d_read (Int16 task_id, Int32 index, Float64 *freq,
*     UInt16 *status)
*
* DESCRIPTION
*   The current frequency of the specified F/D channel is evaluated.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             input channel (1..4)
*   freq                frequency value (Hz)
*   status              function status (SLVDSP1103_F2D_OLD,
*                         SLVDSP1103_F2D_NEW, SLVDSP1103_F2D_OVERFLOW)
*   int_enable          interrupt enable (SLVDSP1103_INT_DISABLE,
*                         SLVDSP1103_INT_ENABLE)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   Frequency measurement must have been initialized by using the function
*   ds1103_slave_dsp_f2d_init().
*   F/D frequency measurement conflicts with PWM measurement.
*   Digital I/O port 3 bits 4..7 conflict with F/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_f2d_read_register (Int16 task_id, Int16 *index,
  UInt16 channel, UInt16 int_enable)
{
  unsigned long parms[DSPDEF_F2D_READ_PCNT];
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  parms[DSPDEF_F2D_READ_CH_LOC] = channel;
  parms[DSPDEF_F2D_READ_INT_LOC] = int_enable;
  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_F2D_READ,
    DSPDEF_F2D_READ_DINCNT, DSPDEF_F2D_READ_DOUTCNT,
    DSPDEF_F2D_READ_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_F2D_READ_REG_INFO,
      SLVDSP1103_MSG_F2D_READ_REG_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_f2d_read_request (Int16 task_id, Int32 index)
{
  Int16 error;

  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_f2d_read (Int16 task_id, Int32 index,
  Float64 *freq, UInt16 *status)
{
  unsigned long data[DSPDEF_F2D_READ_DINCNT];
  Float64 f_tmp;
  Int16 error;

  error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
    (unsigned long *) data);

  if (data[0] == 0 && data[1] == 0)
  {
    *freq = 0.0;
  }
  else
  {
    f_tmp = SLVDSP1103_CPU_CLK /
      (((Float64) ((UInt32) data[1] << 16) + (Float64) data[0]));

    if (f_tmp < slvdsp1103_f2d_fmin[data[3]])
    {
      *freq = 0.0;
    }
    else
    {
      *freq = f_tmp;
    }
  }
  *status = data[2];

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   PWM measurement initialization
*
* SYNTAX
*   void ds1103_slave_dsp_pwm2d_init (Int16 task_id)
*
* DESCRIPTION
*   The capture unit is initialized for F/D frequency measurement.
*
*   pin assignment:     channel | pin
*                       --------|-----
*                          1    | CAP1
*                          2    | CAP2
*                          3    | CAP3
*                          4    | CAP4
*
*   ranges:             channels | period range    | resolution
*                       ---------|-----------------|-----------
*                          1     |  35 us .. 200 s | 50 ns
*                          2     |  70 us .. 200 s | 50 ns
*                          3     | 100 us .. 200 s | 50 ns
*                          4     | 140 us .. 200 s | 50 ns
*
* PARAMETERS
*   task_id             task ID of communication channel
*
* RETURNS
*   void
*
* REMARKS
*   PWM measurement conflicts with F/D frequency measurement.
*   Digital I/O port 3 bits 4..7 conflict with PWM/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_pwm2d_init (Int16 task_id)
{
  unsigned long dummy;
  Int16 error;

  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_PWM2D_INIT,
    DSPDEF_PWM2D_INIT_PCNT, (unsigned long *) &dummy);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_PWM2D_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   PWM period and duty-cycle measurement
*
* SYNTAX
*   void ds1103_slave_dsp_pwm2d_read_register (Int16 task_id, Int16 *index,
*     UInt16 channel, UInt16 int_enable)
*   Int16 ds1103_slave_dsp_pwm2d_read_request (Int16 task_id, Int32 index)
*   Int16 ds1103_slave_dsp_pwm2d_read (Int16 task_id, Int32 index,
*     Float64 *period, Float64 *duty, UInt16 *status)
*
* DESCRIPTION
*   The current period and duty-cycle of the specified channel is evaluated.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   channel             input channel (1..4)
*   period              PWM period (s)
*   duty                PWM duty-cycle (0..1.0)
*   status              function status (SLVDSP1103_PWM2D_OLD,
*                         SLVDSP1103_PWM2D_NEW)
*   int_enable          interrupt enable (SLVDSP1103_INT_DISABLE,
*                         SLVDSP1103_INT_ENABLE)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   PWM measurement must have been initialized by using the function
*   ds1103_slave_dsp_pwm2d_init().
*   PWM measurement conflicts with F/D frequency measurement.
*   Digital I/O port 3 bits 4..7 conflict with PWM/D channels 1..4.
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_pwm2d_read_register (Int16 task_id, Int16 *index,
  UInt16 channel, UInt16 int_enable)
{
  unsigned long parms[DSPDEF_PWM2D_READ_PCNT];
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  parms[DSPDEF_PWM2D_READ_CH_LOC] = channel;
  parms[DSPDEF_PWM2D_READ_INT_LOC] = int_enable;
  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32, DSPDEF_PWM2D_READ,
    DSPDEF_PWM2D_READ_DINCNT, DSPDEF_PWM2D_READ_DOUTCNT,
    DSPDEF_PWM2D_READ_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_PWM2D_READ_REG_INFO,
      SLVDSP1103_MSG_PWM2D_READ_REG_INFO_TXT, channel);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_pwm2d_read_request (Int16 task_id, Int32 index)
{
  Int16 error;

  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_pwm2d_read (Int16 task_id, Int32 index,
  Float64 *period, Float64 *duty, UInt16 *status)
{
  unsigned long data[DSPDEF_PWM2D_READ_DINCNT];
  Int16 error;

  error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
    (unsigned long *) data);

  if (error == DSMCOM_NO_ERROR)
  {
    *period = ((Float64) ((UInt32) data[1] << 16) + (Float64) data[0]) /
      SLVDSP1103_CPU_CLK;
    *duty = ((Float64) ((UInt32) data[3] << 16) + (Float64) data[2]) /
      SLVDSP1103_CPU_CLK / *period;
    *status = data[4];
  }

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Initialization of the serial communication interface SCI.
*
* SYNTAX
*   void ds1103_slave_dsp_sci_init(Int16 task_id, UInt16 scimode,
*     UInt32 baudrate, UInt16 parity, UInt16 databits, UInt16 stopbits)
*
* DESCRIPTION
*   This function initializes the asynchronous serial communication interface
*   (SCI) of the slave DSP. The following parameters must be specified to
*   adjust the serial interface as desired.
*
* PARAMETERS
*   task_id             task ID of communication channel
*
*   scimode             selects RS232 or RS422 mode of the serial port, use:
*                       SLVDSP1103_SCI_RS442
*                       SLVDSP1103_SCI_RS232
*
*   baudrate            transmission speed in bits per second
*                       (300, 600, 1200, 2400, 4800, 9600 19200,
*                        38400, 115200, 625000)
*
*   parity              parity mode of transmission, use:
*                       SLVDSP1103_SCI_NOPAR    (no parity)
*                       SLVDSP1103_SCI_ODDPAR   (odd parity )
*                       SLVDSP1103_SCI_EVENPAR  (even parity )
*
*   databits            number of databits (1..8)
*
*   stopbits            number of stopbits, use:
*                       SLVDSP1103_SCI_1_STOPBIT  (1 stopbit)
*                       SLVDSP1103_SCI_2_STOPBIT  (2 stopbits)
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_sci_init(Int16 task_id, UInt16 scimode,
  UInt32 baudrate, UInt16 parity, UInt16 databits, UInt16 stopbits)
{
  volatile UInt16* setupptr = (volatile UInt16*)DS1103_SETUP;
  unsigned long parms[DSPDEF_SCI_INIT_PCNT];
  Int16 error;
  Float64 divisor;

  /* set sci mode in setup register */
  if(scimode == SLVDSP1103_SCI_RS232)
    *setupptr = (*setupptr & 0xFFDF);                           /* RS232-mode */
  else
    *setupptr = (*setupptr & 0xFFDF) | 0x20;                    /* RS422-mode */

  /* calculate register value for baudrate setting */
  divisor = ((SLVDSP1103_SCI_CLK_SRC / ((Float64) baudrate * 8)) - 1) + 0.5;

  /* store initialization parameters */
  parms[DSPDEF_SCI_INIT_BAUD_LOC] = (UInt16) divisor;
  parms[DSPDEF_SCI_INIT_PAR_LOC] = (UInt16) parity;
  parms[DSPDEF_SCI_INIT_DATAB_LOC] = (UInt16) databits;
  parms[DSPDEF_SCI_INIT_STOPB_LOC] = (UInt16) stopbits;

  /* write initialization data and corresponding command into send buffer */
  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_SCI_INIT,
    DSPDEF_SCI_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  /* set info message if enabled */
  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_SCI_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   Performs data input via SCI.
*
* SYNTAX
*   void ds1103_slave_dsp_sci_read_register (Int16 task_id, Int16 *index)
*   Int16 ds1103_slave_dsp_sci_read_request (Int16 task_id, Int32 index)
*   Int16 ds1103_slave_dsp_sci_read (Int16 task_id, Int32 index,
*     UInt32 *count, UInt32 *status, UInt32 *data)
*
* DESCRIPTION
*   Received data is read from the asynchronous serial communication interface
*   of the slave DSP. Because up to now it is not realized that the Master
*   recognizes an incoming byte via the SCI, on slave side the received bytes
*   are stored temporarily in a SW-FIFO of 16 byte capacity. Each call of the
*   read function delivers the whole contents of this FIFO. If the FIFO has
*   overflown old data had been overwritten and a status bit is sent with
*   the next function call.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   count               number of received bytes
*   status              stauts of the SW-FIFO, 1 = overflow, 0 = no overflow
*                       since last reading
*   data                received data bytes (up to 15 bytes can be read with
*                       each call)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   Serial communication must have been initialized by using the function
*   ds1103_slave_dsp_sci_init().
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_sci_read_register (Int16 task_id, Int16 *index)
{
  unsigned long dummy;
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  /* register read function */
  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32,
    DSPDEF_SCI_READ, DSPDEF_SCI_READ_DINCNT, DSPDEF_SCI_READ_DOUTCNT,
    DSPDEF_SCI_READ_PCNT, (unsigned long *) &dummy);

  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  /* set info message if enabled */
  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_SCI_BYTE_REC_REG_INFO);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_sci_read_request (Int16 task_id, Int32 index)
{
  Int16 error;

  /* request read function for indirect execution */
  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_sci_read (Int16 task_id, Int32 index,
           UInt32 *count, UInt32 *status, UInt32 *data)
{
  unsigned long tmp[DSPDEF_SCI_READ_DINCNT];
  Int16 error;
  UInt16 i;

  /* read received data from dual port memory */
  error = dsmcom_data_read_varlen(slvdsp1103_handle, task_id, index,
    (unsigned long *) tmp);

  /* evaluate received data */
  *count = (UInt32)tmp[0];                  /* number of received data bytes */
  *status = (UInt32)tmp[1];                /* status if FIFO overflow occurs */
  for (i = 0; i < *count; i++)                   /* copy received data bytes */
    data[i]  = (UInt32)tmp[i + 2];

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Performs data output via SCI, byte by byte.
*
* SYNTAX
*   void ds1103_slave_dsp_sci_write_register (Int16 task_id, Int16 *index)
*   Int16 ds1103_slave_dsp_sci_write (Int16 task_id, Int32 index,
*     UInt16 value)
*
* DESCRIPTION
*   A data byte is sent out via the asynchronous serial communication
*   interface of the slave DSP.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   value               data byte to be sent
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   Serial communication must have been initialized by using the function
*   ds1103_slave_dsp_sci_init().
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_sci_write_register (Int16 task_id, Int16 *index)
{
  unsigned long dummy;
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  /* register write function */
  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32,
    DSPDEF_SCI_WRITE, DSPDEF_SCI_WRITE_DINCNT,
    DSPDEF_SCI_WRITE_DOUTCNT, DSPDEF_SCI_WRITE_PCNT,
    (unsigned long *) &dummy);

  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  /* set info message if enabled */
  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_SCI_BYTE_SEND_REG_INFO);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_sci_write (Int16 task_id, Int32 index,
     UInt16 value)
{
  unsigned long tmp = (unsigned long) value;
  Int16 error;

  /* send necessary data to slave for direct execution */
  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) &tmp);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Initialization of the serial peripheral interface SPI.
*
* SYNTAX
*   void ds1103_slave_dsp_spi_init(Int16 task_id, UInt16 spimode,
*     UInt32 baudrate, UInt16 clk_polarity, UInt16 clk_phase, UInt16 databits)
*
* DESCRIPTION
*   This function initializes the synchronous serial peripheral interface
*   (SPI) of the slave DSP. The following parameters must be specified to
*   adjust the serial interface as desired. Because up to now there is no
*   possibility realized to check on master side when a transmission is
*   completed the application software has to handle the communication between
*   master and slave especially if master and slave should send and receive
*   data simultaneously.
*
* PARAMETERS
*   task_id             task ID of communication channel
*
*   spimode             selects master/slave mode of the serial port, use:
*                       SLVDSP1103_SPI_MASTER
*                       SLVDSP1103_SPI_SLAVE
*
*   baudrate            transmission speed in bits per second
*                       (78125bps .. 2.5Mbps)
*                       Note: the maximum master SPI baudrate would be 2.5Mbps,
*                       however, the slave baudrate has a max. speed of 1.25Mbps
*
*   clk_polarity        polarity of the clock signal, use:
*                       SLVDSP1103_SPI_CLKPOL_RISE
*                       (data is output on the rising edge of the clk signal)
*                       SLVDSP1103_SPI_CLKPOL_FALL
*                       (data is output on the falling edge of the clk signal)
*
*   clk_phase           phase of the clock signal, use:
*                       SLVDSP1103_SPI_CLKPHS_WOD
*                       (data is output without delay)
*                       SLVDSP1103_SPI_CLKPHS_WD
*                       (data is output one half-cycle before the first
*                       falling/rising edge)
*                       Note: set clk_phase with delay to achieve that both
*                             master and slave are able to send and reveive data
*                             simultaneously
*
*   databits            number of databits (1..8)
*                       Note: for characters shorter than eight bits, transmit
*                             data must be written in left justified form, and
*                             received data must be read in right-justified form
*
* RETURNS
*   void
*
* REMARKS
*
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_spi_init(Int16 task_id, UInt16 spimode,
  UInt32 baudrate, UInt16 clk_polarity, UInt16 clk_phase, UInt16 databits)
{
  unsigned long parms[DSPDEF_SPI_INIT_PCNT];
  Int16 error;
  Float64 divisor;

  /* calculate register value for baudrate setting */
  if(baudrate >= SLVDSP1103_SPI_MAXBAUD)      /* no brate > 2.5Mbps possible */
    divisor = 4;
  else
    divisor = ((SLVDSP1103_SPI_CLK_SRC / (Float64) baudrate ) - 1) + 0.5;

  /* store initialization parameters */
  parms[DSPDEF_SPI_INIT_MODE_LOC] = (UInt16) spimode;
  parms[DSPDEF_SPI_INIT_CLKPOL_LOC] = (UInt16) clk_polarity;
  parms[DSPDEF_SPI_INIT_CLKPHS_LOC] = (UInt16) clk_phase;
  parms[DSPDEF_SPI_INIT_BAUD_LOC] = (UInt16) divisor;
  parms[DSPDEF_SPI_INIT_DATAB_LOC] = (UInt16) databits;

  /* write initialization data and corresponding command into send buffer */
  error = dsmcom_send(slvdsp1103_handle, task_id, DSPDEF_SPI_INIT,
    DSPDEF_SPI_INIT_PCNT, (unsigned long *) parms);
  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  /* set info message if enabled */
  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_SPI_INIT_INFO);
  }
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   Performs data input via SPI.
*
* SYNTAX
*   void ds1103_slave_dsp_spi_read_register (Int16 task_id, Int16 *index)
*   Int16 ds1103_slave_dsp_spi_read_request (Int16 task_id, Int32 index)
*   Int16 ds1103_slave_dsp_spi_read (Int16 task_id, Int32 index,
*     UInt32 *count, UInt32 *status, UInt32 *data)
*
* DESCRIPTION
*   Received data is read from the synchronous serial peripheral interface
*   of the slave DSP. Because up to now it is not realized that the Master
*   recognizes an incoming byte via the SPI, on slave side the received bytes
*   are stored temporarily in a SW-FIFO of 16 byte capacity. Each call of the
*   read function delivers the whole contents of this FIFO. If the FIFO has
*   overflown old data had been overwritten and a status bit is sent with
*   the next function call.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   count               number of received bytes
*   status              stauts of the SW-FIFO, 1 = overflow, 0 = no overflow
*                       since last reading
*   data                received data bytes (up to 15 bytes can be read with
*                       each call)
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   Serial communication must have been initialized by using the function
*   ds1103_slave_dsp_spi_init().
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_spi_read_register (Int16 task_id, Int16 *index)
{
  unsigned long dummy;
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  /* register read function */
  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32,
    DSPDEF_SPI_READ, DSPDEF_SPI_READ_DINCNT, DSPDEF_SPI_READ_DOUTCNT,
    DSPDEF_SPI_READ_PCNT, (unsigned long *) &dummy);

  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  /* set info message if enabled */
  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_SPI_BYTE_REC_REG_INFO);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_spi_read_request (Int16 task_id, Int32 index)
{
  Int16 error;

  /* request read function for indirect execution */
  error = dsmcom_execute(slvdsp1103_handle, task_id, index);

  return(error);
}


__INLINE Int16 ds1103_slave_dsp_spi_read (Int16 task_id, Int32 index,
           UInt32 *count, UInt32 *status, UInt32 *data)
{
  unsigned long tmp[DSPDEF_SCI_READ_DINCNT];
  Int16 error;
  UInt16 i;

  /* read received data from dual port memory */
  error = dsmcom_data_read_varlen(slvdsp1103_handle, task_id, index,
    (unsigned long *) tmp);

  /* evaluate received data */
  *count = (UInt32)tmp[0];                  /* number of received data bytes */
  *status = (UInt32)tmp[1];                /* status if FIFO overflow occurs */
  for (i = 0; i < *count; i++)                         /* copy received data */
    data[i]  = (UInt32)tmp[i + 2];

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Performs data output via SPI, byte by byte.
*
* SYNTAX
*   void ds1103_slave_dsp_spi_write_register (Int16 task_id, Int16 *index)
*   Int16 ds1103_slave_dsp_spi_write (Int16 task_id, Int32 index,
*     UInt32 value)
*
* DESCRIPTION
*   A data byte is written to the send buffer of the synchronous serial
*   peripheral interface of the slave DSP. In master mode this will start the
*   transmission procedure.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               function index (-1: assign new index)
*   value               data byte to be sent
*
* RETURNS
*   void / errorcode
*
* REMARKS
*   Serial communication must have been initialized by using the function
*   ds1103_slave_dsp_spi_init().
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_spi_write_register (Int16 task_id, Int16 *index)
{
  unsigned long dummy;
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  /* register write function */
  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32,
    DSPDEF_SPI_WRITE, DSPDEF_SPI_WRITE_DINCNT,
    DSPDEF_SPI_WRITE_DOUTCNT, DSPDEF_SPI_WRITE_PCNT,
    (unsigned long *) &dummy);

  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  /* set info message if enabled */
  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    SLVDSP1103_MSG_INFO_SET(SLVDSP1103_MSG_SPI_BYTE_SEND_REG_INFO);
  }
}

#endif /* _INLINE */


__INLINE Int16 ds1103_slave_dsp_spi_write (Int16 task_id, Int32 index,
     UInt32 value)
{
  unsigned long tmp = (unsigned long) value;
  Int16 error;

  /* send necessary data to slave for direct execution */
  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) &tmp);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Registers a slave DSP user function in the command table.
*
* SYNTAX
*  void ds1103_slave_dsp_usrfct_register (Int16 task_id, Int16 *index,
*    UInt32 command, UInt16 data_in_count, UInt16 data_out_count,
*    UInt16 param_count, void *parm)
*
* DESCRIPTION
*   This function registers a user defined slave DSP function in the command
*   table for indirect execution.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   *index              address of command table index
*   command             command OP-code
*   data_in_count       number of data words to be received
*   data_out_count      number of data words to be output
*   param_count         number of parameters following
*   *parm               address of parameters
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

#ifndef _INLINE

void ds1103_slave_dsp_usrfct_register (Int16 task_id, Int16 *index,
  UInt32 command, UInt16 data_in_count, UInt16 data_out_count,
  UInt16 param_count, void *parm)
{
  Int16 error;
  Int32 idx32;

  idx32 = (Int32) *index;

  error = dsmcom_register(slvdsp1103_handle, task_id, &idx32,
    command, data_in_count, data_out_count, param_count,
    (unsigned long *) parm);

  if (error != DSMCOM_NO_ERROR)
  {
    ds1103_slave_dsp_communication_error(error);
  }

  *index = (Int16) idx32;

  if (ds1103_debug & DS1103_DEBUG_INIT)
  {
    msg_info_printf(MSG_SM_DS1103SLVLIB, SLVDSP1103_MSG_USRFCT_REG_INFO,
      SLVDSP1103_MSG_USRFCT_REG_INFO_TXT, command);
  }
}

#endif


/******************************************************************************
*
* FUNCTION
*   Requests an execution of a slve DSP user function from the
*   command table.
*
* SYNTAX
*   Int16 ds1103_slave_dsp_usrfct_request (Int16 task_id,
*     Int32 index, void *data_out)
*
* DESCRIPTION
*   This function requests indirect execution of a user defined slave DSP
*   function from the command table. The number of data words specified by the
*   parameter data_out_count during registration of the function, are read from
*   memory where data_out points to and are send to the slave DSP.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               command table index
*   *data out           address of output data
*
* RETURNS
*   errorcode           SLVDSP1103_NO_ERROR
*                       SLVDSP1103_BUFFER_OVERFLOW
*
* REMARKS
*
******************************************************************************/

__INLINE Int16 ds1103_slave_dsp_usrfct_request (Int16 task_id,
    Int32 index, void *data_out)
{
  Int16 error;

  error = dsmcom_execute_data(slvdsp1103_handle, task_id, index,
    (unsigned long *) data_out);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Reads the received slave DSP input data of a user function.
*
* SYNTAX
*   Int16 ds1103_slave_dsp_usrfct_data_read (Int16 task_id,
*     Int32 index, void *data_in)
*
* DESCRIPTION
*   The number of data words specified by the parameter data_in_count during
*   registration of the corresponding user function are read from the
*   communication buffer and saved to memory where data_in points to.
*   Because multiple functions can send data through the same communication
*   buffer, the buffer is flushed with each call of a data read function and
*   input data is buffered in the command table.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   index               command table index
*   *data_in            address of input data
*
* RETURNS
*   errorcode           SLVDSP1103_NO_ERROR
*                       SLVDSP1103_NO_DATA
*                       SLVDSP1103_DATA_LOST
*
* REMARKS
*
******************************************************************************/

__INLINE Int16 ds1103_slave_dsp_usrfct_data_read (Int16 task_id,
    Int32 index, void *data_in)
{
  Int16 error;

  error = dsmcom_data_read(slvdsp1103_handle, task_id, index,
    (unsigned long *) data_in);

  return(error);
}


/******************************************************************************
*
* FUNCTION
*   Requests direct execution of a slave DSP user function.
*
* SYNTAX
*   Int16 ds1103_slave_dsp_usrfct_execute(Int16 task_id,
*     UInt32 command, UInt16 data_out_count, void *data_out)
*
* DESCRIPTION
*   This function requests direct execution of a user defined slave DSP
*   function. The number of data words specified by the parameter
*   data_out_count are read from memory where data_out points to and are send
*   to the slave DSP.
*
* PARAMETERS
*   task_id             task ID of communication channel
*   command             command OP-code
*   data_out_count      number of data words to be output
*   *data_out           address of output data
*
* RETURNS
*   errorcode           SLVDSP1103_NO_ERROR
*                       SLVDSP1103_BUFFER_OVERFOLW
*
* REMARKS
*
******************************************************************************/

__INLINE Int16 ds1103_slave_dsp_usrfct_execute(Int16 task_id,
    UInt32 command, UInt16 data_out_count, void *data_out)
{
  Int16 error;

  error = dsmcom_send(slvdsp1103_handle, task_id, command, data_out_count,
    (unsigned long *) data_out);

  return(error);
}

/******************************************************************************
*
* FUNCTION
*   Acknowledge the interrupt.
*
* SYNTAX
*   void ds1103_slave_dsp_usrfct_subint_acknowledge (void)
*
* DESCRIPTION
*   This function acknowledge an user sub-interrupt which was triggered by
*   the slave DSP. Only those sub-interrupts are acknowledged which are
*   triggered by the slave DSP function 'slvdsp_usrfct_subint_trigger'.
*
* PARAMETERS
*   none
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

__INLINE void ds1103_slave_dsp_usrfct_subint_acknowledge (void)
{
   dssint_acknowledge(dssint_slvdsp_usr_receiver);
}


/******************************************************************************
*
* FUNCTION
*   Decode the pending sub-interrupt number.
*
* SYNTAX
*   Int32 ds1103_slave_dsp_usrfct_subint_get (void)
*
* DESCRIPTION
*   This function must be called repetitively within an interrupt handler. They
*   processes the sub-interrupt information copied to the receiver data
*   structure by 'ds1103_slave_dsp_usrfct_subint_acknowledge.
*   Only those sub-interrupts are decoded which are triggered by the slave DSP
*   function 'slvdsp_usrfct_subint_trigger'.
*
* PARAMETERS
*   none
*
* RETURNS
*   SINT_NO_SUBINT if there is no pending sub-interrupt
*   or the number of highest priority pending sub-interrupt
*
* REMARKS
*
******************************************************************************/

__INLINE Int32 ds1103_slave_dsp_usrfct_subint_get (void)
{
  return dssint_decode(dssint_slvdsp_usr_receiver);
}

/******************************************************************************
*
* FUNCTION
*   Service function for subinterrupt test.
*
* SYNTAX
*   Int32 ds1103_slave_dsp_subint_request(const UInt32 subint)
*
* DESCRIPTION
*   The function requests sub-interrupts on the slave DSP. It can be
*   distinguished between user and firmware sub-interrupts. Legal sub-interrupt
*   numbers are:
*
*   user sub-interrupts     : 0 ... 15 (16 in all)
*                             (max. 32, not supported by current firmware!)
*   firmware sub-interrupts : 0 ... 24 (25 in all)
*                             (max. 32, not supported by current firmware!)
*
*   The request is passed via communication buffer '0', which can not be 
*   changed by a function parameter.
*
* PARAMETERS
*
*   subint                  number of the requested subinterrupt
*                           ( 0  ... 15   user sub-interrupts
*                             32 ... 56   firmware sub-interrupts )
*
* RETURNS
*    errorcode
*
* REMARKS
*   Only for test purposes
*
******************************************************************************/
__INLINE Int32 ds1103_slave_dsp_subint_request (UInt32 subint)
{

  if ( ((subint >= 0) && (subint < SINT1103_DSP2PPC_USR_SINT_NUMBER)) ||
       ((subint >= SINT1103_DSP_FW_TASK_OFFSET) &&
        (subint < (SINT1103_DSP_FW_TASK_OFFSET + SINT1103_DSP2PPC_FW_SINT_NUMBER))) )
  {
    return dsmcom_send(slvdsp1103_handle, 0, DSPDEF_SUBINT_REQUEST,
      DSPDEF_SUBINT_REQUEST_PCNT, (unsigned long *) &subint);
  }
  else
  {
    SLVDSP1103_MSG_ERROR_SET(SLVDSP1103_MSG_SLV_SUBINT_RANGE_ERROR);
    return (DSPDEF_SUBINT_REQUEST_PCNT);
  }

}

#undef __INLINE
