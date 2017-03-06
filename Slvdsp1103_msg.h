/******************************************************************************
*
* MODULE
*   Error and info messages for TMS320F240 slave DSP access on
*   DS1103 controller board
*
*   Version for PPC master CPU
*
* FILE
*   Ds1103slvlib_msg.h
*
* RELATED FILES
*   Slvdsp1103.c
*
* DESCRIPTION
*
* AUTHOR(S)
*   H.-J. Miks
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Slvdsp1103_msg.h $ $Revision: 1.2 $ $Date: 2003/08/07 14:54:06GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#ifndef __DS1103SLVLIB_MSG_H__
#define __DS1103SLVLIB_MSG_H__


/* macros for easy error and info message setting */

#define SLVDSP1103_MSG_ERROR_SET(err) \
  msg_error_set(MSG_SM_DS1103SLVLIB, err, err##_TXT)

#define SLVDSP1103_MSG_INFO_SET(info) \
  msg_info_set(MSG_SM_DS1103SLVLIB, info, info##_TXT)


/* general master error messages */

#define SLVDSP1103_MSG_ALLOC_ERROR          100
#define SLVDSP1103_MSG_ALLOC_ERROR_TXT      "Not enough memory."

#define SLVDSP1103_MSG_ILLEGAL_TASK_ID      101
#define SLVDSP1103_MSG_ILLEGAL_TASK_ID_TXT  "Illegal communication task ID."

#define SLVDSP1103_MSG_ILLEGAL_INDEX        102
#define SLVDSP1103_MSG_ILLEGAL_INDEX_TXT    "Illegal communication function index."

#define SLVDSP1103_MSG_BUFFER_OVERFLOW      103
#define SLVDSP1103_MSG_BUFFER_OVERFLOW_TXT  "Master->slave communication buffer overflow."

#define SLVDSP1103_MSG_NO_DATA              104
#define SLVDSP1103_MSG_NO_DATA_TXT          "No new data available."

#define SLVDSP1103_MSG_DATA_LOST            105
#define SLVDSP1103_MSG_DATA_LOST_TXT        "Previous data lost."

#define SLVDSP1103_MSG_TIMEOUT              106
#define SLVDSP1103_MSG_TIMEOUT_TXT          "Slave not responding."

#define SLVDSP1103_MSG_FW_REV_UP_ERROR       107
#define SLVDSP1103_MSG_FW_REV_UP_ERROR_TXT_1 "Firmware rev. %d.%d or higher required."
#define SLVDSP1103_MSG_FW_REV_UP_ERROR_TXT_2    \
"Refer to <DSPACE_ROOT>\\Ds1103\\SlaveDSP\\Firmware\\DS1103SLVDSP_FW%d_%d_x_Prg.txt.",   \
REQ_DS1103_SLVDSP_FIRMWARE_VER_MAR, REQ_DS1103_SLVDSP_FIRMWARE_VER_MIR

#define SLVDSP1103_MSG_UNKNOWN_ERROR_TXT    "Unknown error detected."

#define SLVDSP1103_MSG_SLV_SUBINT_RANGE_ERROR      108
#define SLVDSP1103_MSG_SLV_SUBINT_RANGE_ERROR_TXT  "Subint number is out of range."

#define SLVDSP1103_MSG_TIMEOUT_OR_WRONG_FIRMWARE      109
#define SLVDSP1103_MSG_TIMEOUT_OR_WRONG_FIRMWARE_TXT  ": Slave not responding or wrong firmware."


/* general slave DSP error messages */

#define SLVDSP1103_MSG_SLV_UNKNOWN_ERROR       200
#define SLVDSP1103_MSG_SLV_UNKNOWN_ERROR_TXT   "Slave: Unknown error detected."

#define SLVDSP1103_MSG_SLV_ALLOC_ERROR         201
#define SLVDSP1103_MSG_SLV_ALLOC_ERROR_TXT     "Slave: Not enough memory."

#define SLVDSP1103_MSG_SLV_BUFFER_OVERFLOW     202
#define SLVDSP1103_MSG_SLV_BUFFER_OVERFLOW_TXT "Slave->master communication buffer overflow."


/* initialization and range error messages */

#define SLVDSP1103_MSG_ADC_INIT_ERROR       300
#define SLVDSP1103_MSG_ADC_INIT_ERROR_TXT   "_adc_read_register(): channel %d not initialized."
#define SLVDSP1103_MSG_ADC_RANGE_ERROR      301
#define SLVDSP1103_MSG_ADC_RANGE_ERROR_TXT  "_adc_read_register(): parameter 'channel' out of range."

#define SLVDSP1103_MSG_ADC2_INIT_ERROR      310
#define SLVDSP1103_MSG_ADC2_INIT_ERROR_TXT  "_adc2_read_register(): channel %d not initialized."
#define SLVDSP1103_MSG_ADC2_RANGE_ERROR     311
#define SLVDSP1103_MSG_ADC2_RANGE_ERROR_TXT "_adc2_read_register(): parameter '%s' out of range."

#define SLVDSP1103_MSG_PIO_RANGE_ERROR1     320
#define SLVDSP1103_MSG_PIO_RANGE_ERROR1_TXT "_bit_io_init(): parameter 'channel' out of range."
#define SLVDSP1103_MSG_PIO_RANGE_ERROR2     321
#define SLVDSP1103_MSG_PIO_RANGE_ERROR2_TXT "_bit_io_read_register(): parameter 'channel' out of range."
#define SLVDSP1103_MSG_PIO_RANGE_ERROR3     322
#define SLVDSP1103_MSG_PIO_RANGE_ERROR3_TXT "_bit_io_write_register(): parameter 'channel' out of range."
#define SLVDSP1103_MSG_PIO_RANGE_ERROR4     323
#define SLVDSP1103_MSG_PIO_RANGE_ERROR4_TXT "_bit_io_set_register(): parameter 'channel' out of range."
#define SLVDSP1103_MSG_PIO_RANGE_ERROR5     324
#define SLVDSP1103_MSG_PIO_RANGE_ERROR5_TXT "_bit_io_clear_register(): parameter 'channel' out of range."

#define SLVDSP1103_MSG_PWM_INIT_ERROR       330
#define SLVDSP1103_MSG_PWM_INIT_ERROR_TXT   "_pwm_duty_write_register(): channel %d not initialized."
#define SLVDSP1103_MSG_PWM_RANGE_ERROR1     331
#define SLVDSP1103_MSG_PWM_RANGE_ERROR1_TXT "_pwm_init(): parameter '%s' out of range."
#define SLVDSP1103_MSG_PWM_RANGE_ERROR2     332
#define SLVDSP1103_MSG_PWM_RANGE_ERROR2_TXT "_pwm_duty_write_register(): parameter 'channel' out of range."

#define SLVDSP1103_MSG_PWM3_RANGE_ERROR     340
#define SLVDSP1103_MSG_PWM3_RANGE_ERROR_TXT "_pwm3_init(): parameter '%s' out of range."

#define SLVDSP1103_MSG_PWM3SV_RANGE_ERROR     350
#define SLVDSP1103_MSG_PWM3SV_RANGE_ERROR_TXT "_pwm3sv_init(): parameter '%s' out of range."

#define SLVDSP1103_MSG_D2F_INIT_ERROR       360
#define SLVDSP1103_MSG_D2F_INIT_ERROR_TXT   "_d2f_write_register(): channel %d not initialized."
#define SLVDSP1103_MSG_D2F_RANGE_ERROR      361
#define SLVDSP1103_MSG_D2F_RANGE_ERROR_TXT  "_d2f_init(): parameter 'range' out of range."

#define SLVDSP1103_MSG_F2D_RANGE_ERROR      370
#define SLVDSP1103_MSG_F2D_RANGE_ERROR_TXT  "_f2d_init(): parameter '%s' out of range."

/* info messages */

#define SLVDSP1103_MSG_SLV_FIRMWARE_REV     500

#define SLVDSP1103_MSG_SLV_INIT_ACK         501
#define SLVDSP1103_MSG_SLV_INIT_ACK_TXT     "Slave: Initialization acknowledged."

#define SLVDSP1103_MSG_COMM_INIT_INFO       502
#define SLVDSP1103_MSG_COMM_INIT_INFO_TXT   "Master-slave communication initialized."

#define SLVDSP1103_MSG_ADC_INIT_INFO        510
#define SLVDSP1103_MSG_ADC_INIT_INFO_TXT    "ADC channels initialized."
#define SLVDSP1103_MSG_ADC_REG_INFO         511
#define SLVDSP1103_MSG_ADC_REG_INFO_TXT     "ADC read function registered for channel %d."

#define SLVDSP1103_MSG_ADC2_REG_INFO        520
#define SLVDSP1103_MSG_ADC2_REG_INFO_TXT    "2-channel ADC read function registered for channels %d, %d."

#define SLVDSP1103_MSG_PIO_INIT_INFO          530
#define SLVDSP1103_MSG_PIO_INIT_INFO_TXT      "Digital I/O port %d initialized."
#define SLVDSP1103_MSG_PIO_READ_REG_INFO      531
#define SLVDSP1103_MSG_PIO_READ_REG_INFO_TXT  "Digital I/O read function registered for port %d."
#define SLVDSP1103_MSG_PIO_WRITE_REG_INFO     532
#define SLVDSP1103_MSG_PIO_WRITE_REG_INFO_TXT "Digital I/O write function registered for port %d."
#define SLVDSP1103_MSG_PIO_SET_REG_INFO       533
#define SLVDSP1103_MSG_PIO_SET_REG_INFO_TXT   "Digital I/O set function registered for port %d."
#define SLVDSP1103_MSG_PIO_CLR_REG_INFO       534
#define SLVDSP1103_MSG_PIO_CLR_REG_INFO_TXT   "Digital I/O clear function registered for port %d."

#define SLVDSP1103_MSG_PWM_INIT_INFO           540
#define SLVDSP1103_MSG_PWM_INIT_INFO_TXT       "PWM channels initialized."
#define SLVDSP1103_MSG_PWM_WRITE_REG_INFO      541
#define SLVDSP1103_MSG_PWM_WRITE_REG_INFO_TXT  "PWM duty cycle write function registered for channel %d."

#define SLVDSP1103_MSG_PWM3_INIT_INFO            550
#define SLVDSP1103_MSG_PWM3_INIT_INFO_TXT        "PWM3 unit initialized."
#define SLVDSP1103_MSG_PWM3_WRITE_REG_INFO       551
#define SLVDSP1103_MSG_PWM3_WRITE_REG_INFO_TXT   "PWM3 duty cycle write function registered."

#define SLVDSP1103_MSG_PWM3SV_INIT_INFO           560
#define SLVDSP1103_MSG_PWM3SV_INIT_INFO_TXT       "PWM3SV unit initialized."
#define SLVDSP1103_MSG_PWM3SV_WRITE_REG_INFO      561
#define SLVDSP1103_MSG_PWM3SV_WRITE_REG_INFO_TXT  "PWM3SV duty cycle write function registered."

#define SLVDSP1103_MSG_D2F_INIT_INFO           570
#define SLVDSP1103_MSG_D2F_INIT_INFO_TXT       "D/F square wave generation initialized."
#define SLVDSP1103_MSG_D2F_WRITE_REG_INFO      571
#define SLVDSP1103_MSG_D2F_WRITE_REG_INFO_TXT  "D/F write function registered for channel %d."

#define SLVDSP1103_MSG_F2D_INIT_INFO           580
#define SLVDSP1103_MSG_F2D_INIT_INFO_TXT       "F/D frequency measurement initialized."
#define SLVDSP1103_MSG_F2D_READ_REG_INFO       581
#define SLVDSP1103_MSG_F2D_READ_REG_INFO_TXT   "F/D read function registered for channel %d."

#define SLVDSP1103_MSG_PWM2D_INIT_INFO         590
#define SLVDSP1103_MSG_PWM2D_INIT_INFO_TXT     "PWM measurement initialized."
#define SLVDSP1103_MSG_PWM2D_READ_REG_INFO     591
#define SLVDSP1103_MSG_PWM2D_READ_REG_INFO_TXT "PWM measurement read function registered for channel %d."

#define SLVDSP1103_MSG_SCI_INIT_INFO              600
#define SLVDSP1103_MSG_SCI_INIT_INFO_TXT          "SCI communication initialized."
#define SLVDSP1103_MSG_SCI_BYTE_REC_REG_INFO      601
#define SLVDSP1103_MSG_SCI_BYTE_REC_REG_INFO_TXT  "SCI byte receive function registered."
#define SLVDSP1103_MSG_SCI_BYTE_SEND_REG_INFO     602
#define SLVDSP1103_MSG_SCI_BYTE_SEND_REG_INFO_TXT "SCI byte send function registered."

#define SLVDSP1103_MSG_SPI_INIT_INFO              610
#define SLVDSP1103_MSG_SPI_INIT_INFO_TXT          "SPI communication initialized."
#define SLVDSP1103_MSG_SPI_BYTE_REC_REG_INFO      611
#define SLVDSP1103_MSG_SPI_BYTE_REC_REG_INFO_TXT  "SPI byte receive function registered."
#define SLVDSP1103_MSG_SPI_BYTE_SEND_REG_INFO     612
#define SLVDSP1103_MSG_SPI_BYTE_SEND_REG_INFO_TXT "SPI byte send function registered."

#define SLVDSP1103_MSG_USRFCT_REG_INFO      700
#define SLVDSP1103_MSG_USRFCT_REG_INFO_TXT  "User function %d registered."


#endif /* __DS1103SLVLIB_MSG_H__ */
