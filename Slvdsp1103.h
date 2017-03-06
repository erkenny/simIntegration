/******************************************************************************
*
* MODULE
*   TMS320F240 slave DSP access functions for DS1103 controller board
*
*   Version for PPC master CPU
*
* FILE
*   Slvdsp1103.h
*
* RELATED FILES
*   Slvdsp1103.c, Dsmcom.h, Dscomdef.h, Def1103.h
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
* $RCSfile: Slvdsp1103.h $ $Revision: 1.4 $ $Date: 2004/01/30 10:48:48GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#ifndef __SLVDSP1103_H__
#define __SLVDSP1103_H__

#include <dstypes.h>                      /* platform independent data types */
#include <dscomdef.h>   /* common definitions for master-slave communication */
#include <def1103.h>           /* definitions for master-slave communication */
#include <dsmcom.h>                        /* master communication functions */

/******************************************************************************
* constant and macro definitions
******************************************************************************/
/* $DSVERSION: DS1103_SLVDSP_SOFTWARE - DS1103 SLVDSP software */
#define DS1103_SLVDSP_SOFTWARE_VER_MAR 3
#define DS1103_SLVDSP_SOFTWARE_VER_MIR 5
#define DS1103_SLVDSP_SOFTWARE_VER_MAI 1
#define DS1103_SLVDSP_SOFTWARE_VER_SPB VCM_VERSION_RELEASE
#define DS1103_SLVDSP_SOFTWARE_VER_SPN 0
#define DS1103_SLVDSP_SOFTWARE_VER_PLV 0

/* $DSVERSION: REQ_DS1103_SLVDSP_FIRMWARE - Required DS1103 SLVDSP Firmware */
#define REQ_DS1103_SLVDSP_FIRMWARE_VER_MAR 3
#define REQ_DS1103_SLVDSP_FIRMWARE_VER_MIR 4
#define REQ_DS1103_SLVDSP_FIRMWARE_VER_MAI 0
#define REQ_DS1103_SLVDSP_FIRMWARE_VER_SPB VCM_VERSION_RELEASE
#define REQ_DS1103_SLVDSP_FIRMWARE_VER_SPN 0
#define REQ_DS1103_SLVDSP_FIRMWARE_VER_PLV 0

/* comment out to suppress range check */

#define DS1103_RANGE_CHECK

/* constants for slave DSP control */

#define SLVDSP1103_RSTSLV       0x0010                /* slave DSP reset bit */
#define SLVDSP1103_RUNNING      1                        /* slave DSP status */


/* error codes */

/* from dsmcom.h */
#define SLVDSP1103_NO_ERROR             DSMCOM_NO_ERROR                 /* 0 */
#define SLVDSP1103_ALLOC_ERROR          DSMCOM_ALLOC_ERROR              /* 1 */
#define SLVDSP1103_ILLEGAL_TASK_ID      DSMCOM_ILLEGAL_TASK_ID          /* 2 */
#define SLVDSP1103_ILLEGAL_INDEX        DSMCOM_ILLEGAL_INDEX            /* 3 */
#define SLVDSP1103_BUFFER_OVERFLOW      DSMCOM_BUFFER_OVERFLOW          /* 4 */
#define SLVDSP1103_NO_DATA              DSMCOM_NO_DATA                  /* 5 */
#define SLVDSP1103_DATA_LOST            DSMCOM_DATA_LOST                /* 6 */
#define SLVDSP1103_TIMEOUT              DSMCOM_TIMEOUT                  /* 7 */

/* code for errors caused by the slave */
#define SLVDSP1103_SLV_ALLOC_ERROR      DSMCOM_SLV_ALLOC_ERROR          /* 8 */
#define SLVDSP1103_SLV_BUFFER_OVERFLOW  DSMCOM_SLV_BUFFER_OVERFLOW      /* 9 */
#define SLVDSP1103_SLV_INIT_ACK         DSMCOM_SLV_INIT_ACK            /* 10 */
#define SLVDSP1103_SLV_UNDEFINED        DSMCOM_SLV_UNDEFINED           /* 11 */
#define SLVDSP1103_SLV_ILLEGAL_USR_IDX  DSMCOM_SLV_ILLEGAL_USR_IDX     /* 12 */

/* error code for pwm3sv range check */
#define SLVDSP1103_RANGE_ERROR          20

/* slave DSP firmware revision definitions */

#define SLVDSP1103_FW_MAJOR     0xF800
#define SLVDSP1103_FW_MINOR     0x0780
#define SLVDSP1103_FW_MAINT     0x0078
#define SLVDSP1103_FW_VERSION   0x0006
#define SLVDSP1103_FW_ORIGIN    0x0001

#define SLVDSP1103_FW_CHECK     0xFF80      /* check major and minor release */

#define SLVDSP1103_FW_FINAL     0x0000
#define SLVDSP1103_FW_BETA      0x0002
#define SLVDSP1103_FW_ALPHA     0x0004

#define SLVDSP1103_FW_REQ       \
(REQ_DS1103_SLVDSP_FIRMWARE_VER_MAR << 11 |  \
 REQ_DS1103_SLVDSP_FIRMWARE_VER_MIR <<  7)

#define SLVDSP1103_FW_REQ_TXT        \
REQ_DS1103_SLVDSP_FIRMWARE_VER_MAR,  \
REQ_DS1103_SLVDSP_FIRMWARE_VER_MIR,  \
REQ_DS1103_SLVDSP_FIRMWARE_VER_MAI


/* initialization of slave DSP interrupts */

#define ds1103_slave_dsp_int_init(isr_function_name) { \
  ds1103_set_interrupt_vector(DS1103_INT_SLAVE_DSP, \
    (DS1103_Int_Handler_Type) &isr_function_name, SAVE_REGS_ON); \
  ds1103_enable_hardware_int(DS1103_INT_SLAVE_DSP); \
  DS1103_GLOBAL_INTERRUPT_ENABLE(); }

/* initialization of slave DSP PWM3 sync interrupt */

#define ds1103_slave_dsp_pwm3_int_init(isr_function_name) { \
  ds1103_set_interrupt_vector(DS1103_INT_SLAVE_DSP_PWM, \
    (DS1103_Int_Handler_Type) &isr_function_name, SAVE_REGS_ON); \
  ds1103_enable_hardware_int(DS1103_INT_SLAVE_DSP_PWM); \
  DS1103_GLOBAL_INTERRUPT_ENABLE(); }

/* initialization of slave DSP PWM3 inverted sync interrupt */

#define ds1103_slave_dsp_pwm3_inv_int_init(isr_function_name) { \
  ds1103_set_interrupt_vector(DS1103_INT_SLAVE_DSP_PWM_INV, \
    (DS1103_Int_Handler_Type) &isr_function_name, SAVE_REGS_ON); \
  ds1103_enable_hardware_int(DS1103_INT_SLAVE_DSP_PWM_INV); \
  DS1103_GLOBAL_INTERRUPT_ENABLE(); }

/******************************************************************************
* function prototypes
******************************************************************************/

#ifdef _INLINE
#define __INLINE static
#else
#define __INLINE
#endif


/*--- initialization and error handling -------------------------------------*/

void ds1103_slave_dsp_communication_init (void);

Int16 ds1103_slave_dsp_firmware_rev_read (Int16 task_id, UInt32 *revision);

Int16 ds1103_slave_dsp_user_firmware_rev_read (Int16 task_id, UInt32 *revision);

__INLINE void ds1103_slave_dsp_error_read (Int16 task_id, UInt32 *slave_error);

/*--- ADC input -------------------------------------------------------------*/

void ds1103_slave_dsp_adc_init (Int16 task_id, UInt16 sel_mask);

void ds1103_slave_dsp_adc_read_register (Int16 task_id, Int16 *index,
  UInt16 channel, UInt16 int_enable);

__INLINE Int16 ds1103_slave_dsp_adc_read_request (Int16 task_id, Int32 index);

__INLINE Int16 ds1103_slave_dsp_adc_read (Int16 task_id, Int32 index,
  Float64 *value);

__INLINE Int16 ds1103_slave_dsp_adc_read_new (Int16 task_id, Int32 index,
  Float64 *value);

void ds1103_slave_dsp_adc2_read_register (Int16 task_id, Int16 *index,
  UInt16 channel1, UInt16 channel2, UInt16 int_enable);

__INLINE Int16 ds1103_slave_dsp_adc2_read_request (Int16 task_id, Int32 index);

__INLINE Int16 ds1103_slave_dsp_adc2_read (Int16 task_id, Int32 index,
  Float64 *value1, Float64 *value2);

__INLINE Int16 ds1103_slave_dsp_adc2_read_new (Int16 task_id, Int32 index,
  Float64 *value1, Float64 *value2);

/*--- digital I/O -----------------------------------------------------------*/

void ds1103_slave_dsp_bit_io_init (Int16 task_id, UInt16 channel,
  UInt8 sel_mask, UInt8 dir_mask);

void ds1103_slave_dsp_bit_io_read_register (Int16 task_id, Int16 *index,
  UInt32 channel);

__INLINE Int16 ds1103_slave_dsp_bit_io_read_request (Int16 task_id,
  Int32 index);

__INLINE Int16 ds1103_slave_dsp_bit_io_read (Int16 task_id, Int32 index,
  UInt8 *value);

__INLINE Int16 ds1103_slave_dsp_bit_io_read_new (Int16 task_id, Int32 index,
  UInt8 *value);

void ds1103_slave_dsp_bit_io_write_register (Int16 task_id, Int16 *index,
  UInt32 channel);

__INLINE Int16 ds1103_slave_dsp_bit_io_write (Int16 task_id, Int32 index,
  UInt8 value);

void ds1103_slave_dsp_bit_io_set_register (Int16 task_id, Int16 *index,
  UInt32 channel);

__INLINE Int16 ds1103_slave_dsp_bit_io_set (Int16 task_id, Int32 index,
  UInt8 mask);

void ds1103_slave_dsp_bit_io_clear_register (Int16 task_id, Int16 *index,
  UInt32 channel);

__INLINE Int16 ds1103_slave_dsp_bit_io_clear (Int16 task_id, Int32 index,
  UInt8 mask);

/*--- simple PWM generation -------------------------------------------------*/

void ds1103_slave_dsp_pwm_init (Int16 task_id, Float64 period, Float64 duty,
  UInt16 mode, UInt16 pol, UInt16 mask);

void ds1103_slave_dsp_pwm_duty_write_register (Int16 task_id, Int16 *index,
  UInt32 channel);

__INLINE Int16 ds1103_slave_dsp_pwm_duty_write (Int16 task_id, Int32 index,
  Float64 duty);

__INLINE Int16 ds1103_slave_dsp_pwm_start (Int16 task_id, UInt32 mask);

__INLINE Int16 ds1103_slave_dsp_pwm_stop (Int16 task_id, UInt32 mask);

__INLINE Int16 ds1103_slave_dsp_pwm_output_set (Int16 task_id, UInt32 mask,
  UInt32 state);

/*--- 3-phase PWM generation ------------------------------------------------*/

void ds1103_slave_dsp_pwm3_init (Int16 task_id, Float64 period, Float64 duty1,
  Float64 duty2, Float64 duty3, Float64 dead_band, Float64 sync_pos);

void ds1103_slave_dsp_pwm3_duty_write_register (Int16 task_id, Int16 *index);

__INLINE Int16 ds1103_slave_dsp_pwm3_duty_write (Int16 task_id, Int32 index,
  Float64 duty1, Float64 duty2, Float64 duty3);

void ds1103_slave_dsp_pwm3sv_init (Int16 task_id, Float64 period,
  UInt16 sector, Float64 t1, Float64 t2, Float64 dead_band, Float64 sync_pos);

void ds1103_slave_dsp_pwm3sv_duty_write_register (Int16 task_id, Int16 *index);

__INLINE Int16 ds1103_slave_dsp_pwm3sv_duty_write (Int16 task_id, Int32 index,
  UInt16 sector, Float64 t1, Float64 t2);

__INLINE Int16 ds1103_slave_dsp_pwm3_start (Int16 task_id);

__INLINE Int16 ds1103_slave_dsp_pwm3_stop (Int16 task_id);

__INLINE Int16 ds1103_slave_dsp_pwm3_output_set (Int16 task_id, UInt32 int_mode,
  UInt32 state1, UInt32 state2, UInt32 state3, UInt32 state4, UInt32 state5,
  UInt32 state6);

/*--- variable frequency square-wave generation -----------------------------*/

void ds1103_slave_dsp_d2f_init (Int16 task_id, UInt16 range, Float64 freq,
  UInt16 ch4_enable);

void ds1103_slave_dsp_d2f_write_register (Int16 task_id, Int16 *index,
  unsigned long channel);

__INLINE Int16 ds1103_slave_dsp_d2f_write (Int16 task_id, Int32 index,
  Float64 freq);

/*--- frequency measurement -------------------------------------------------*/

void ds1103_slave_dsp_f2d_init (Int16 task_id, Float64 fmin1, Float64 fmin2,
  Float64 fmin3, Float64 fmin4);

void ds1103_slave_dsp_f2d_read_register (Int16 task_id, Int16 *index,
  UInt16 channel, UInt16 int_enable);

__INLINE Int16 ds1103_slave_dsp_f2d_read_request (Int16 task_id, Int32 index);

__INLINE Int16 ds1103_slave_dsp_f2d_read (Int16 task_id, Int32 index,
  Float64 *freq, UInt16 *status);

/*--- PWM measurement -------------------------------------------------------*/

void ds1103_slave_dsp_pwm2d_init (Int16 task_id);

void ds1103_slave_dsp_pwm2d_read_register (Int16 task_id, Int16 *index,
  UInt16 channel, UInt16 int_enable);

__INLINE Int16 ds1103_slave_dsp_pwm2d_read_request (Int16 task_id,
  int index);

__INLINE Int16 ds1103_slave_dsp_pwm2d_read (Int16 task_id, Int32 index,
  Float64 *period, Float64 *duty, UInt16 *status);

/*--- SCI communication ------------------------------------------------------*/

void ds1103_slave_dsp_sci_init(Int16 task_id, UInt16 scimode, UInt32 baudrate,
  UInt16 parity, UInt16 databits, UInt16 stopbits);

void ds1103_slave_dsp_sci_read_register (Int16 task_id, Int16 *index);

__INLINE Int16 ds1103_slave_dsp_sci_read_request (Int16 task_id, Int32 index);

__INLINE Int16 ds1103_slave_dsp_sci_read (Int16 task_id, Int32 index,
           UInt32 *count, UInt32 *status, UInt32 *data);

void ds1103_slave_dsp_sci_write_register (Int16 task_id, Int16 *index);

__INLINE Int16 ds1103_slave_dsp_sci_write (Int16 task_id, Int32 index,
     UInt16 value);

/*--- SPI communication ------------------------------------------------------*/

void ds1103_slave_dsp_spi_init(Int16 task_id, UInt16 spimode,
  UInt32 baudrate, UInt16 clk_polarity, UInt16 clk_phase, UInt16 databits);

void ds1103_slave_dsp_spi_read_register (Int16 task_id, Int16 *index);

__INLINE Int16 ds1103_slave_dsp_spi_read_request (Int16 task_id, Int32 index);

__INLINE Int16 ds1103_slave_dsp_spi_read (Int16 task_id, Int32 index,
           UInt32 *count, UInt32 *status, UInt32 *data);

void ds1103_slave_dsp_spi_write_register (Int16 task_id, Int16 *index);

__INLINE Int16 ds1103_slave_dsp_spi_write (Int16 task_id, Int32 index,
     UInt32 value);

/*--- user interface ---------------------------------------------------------*/

void ds1103_slave_dsp_usrfct_register (Int16 task_id, Int16 *index,
  UInt32 command, UInt16 data_in_count, UInt16 data_out_count,
  UInt16 param_count, void *parm);

__INLINE Int16 ds1103_slave_dsp_usrfct_request (Int16 task_id,
    Int32 index, void *data_out);

__INLINE Int16 ds1103_slave_dsp_usrfct_data_read (Int16 task_id,
    Int32 index, void *data_in);

__INLINE Int16 ds1103_slave_dsp_usrfct_execute(Int16 task_id,
    UInt32 command, UInt16 data_out_count, void *data_out);

__INLINE void  ds1103_slave_dsp_usrfct_subint_acknowledge (void);

__INLINE Int32 ds1103_slave_dsp_usrfct_subint_get (void);

/* Only for test purposes */
__INLINE Int32 ds1103_slave_dsp_subint_request (UInt32 subint);


#undef __INLINE


/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
#include <slvdsp1103.c>
#endif


#endif /* __SLVDSP1103_H__ */

