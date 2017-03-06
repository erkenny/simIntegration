/******************************************************************************
*
* MODULE
*   DS1103 Error and info messages
*
*   Version for PPC master CPU
*
* FILE
*   ds1103_msg.h
*
* RELATED FILES
*
*
* DESCRIPTION
*
* AUTHOR(S)
*   M. Heier
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: DS1103_msg.h $ $Revision: 1.3 $ $Date: 2003/08/18 14:27:06GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#ifndef __DS1103_MSG_H__
#define __DS1103_MSG_H__

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

/* macros for easy error and info message setting */

#define DS1103_MSG_ERROR_SET(err) \
  msg_error_set(MSG_SM_RTLIB, err, err##_TXT)

#define DS1103_MSG_INFO_SET(info) \
  msg_info_set(MSG_SM_RTLIB, info, info##_TXT)

/* ------------------ msg-error-codes --------------------------------------- */


#define DS1103_NO_ERROR                 0

#define DS1103_SUBINT_MODULE_ERROR      100
#define DS1103_SUBINT_MODULE_ERROR_TXT  "Subinterrupt module initialization error."

#define DS1103_WRONG_BOARD_REV              110
#define DS1103_WRONG_BOARD_REV_TXT       "%s requires board revision %s."


/* ----------------- info-messages ------------------------------------------ */

/* module Init1103.c */

#define DS1103_INIT                     200
#define DS1103_INIT_TXT                 "Ds1103 initialization completed."

#define DS1103_MSG_INIT                 201
#define DS1103_MSG_INIT_TXT             "Message module initialized."

#define DS1103_HOSTSVC_INIT             202
#define DS1103_HOSTSVC_INIT_TXT         "Host services initialized."

#define DS1103_SUBINT_INIT              203
#define DS1103_SUBINT_INIT_TXT          "Subinterrupt module initialized."

#define DS1103_ADC_MUX_INIT             204
#define DS1103_ADC_MUX_INIT_TXT         "ADC multiplexers initialized."

#define DS1103_DAC_INIT                 205
#define DS1103_DAC_INIT_TXT             "DAC channels initialized."

#define DS1103_INC_ENCODER_INIT         206
#define DS1103_INC_ENCODER_INIT_TXT     "Incremental encoder interfaces initialized."

#define DS1103_DIG_BIO_INIT             207
#define DS1103_DIG_BIO_INIT_TXT         "Digital bit I/O initialized."

#define DS1103_UART_INIT                208
#define DS1103_UART_INIT_TXT            "Serial interface initialized."

#define DS1103_DSTS_INIT                209
#define DS1103_DSTS_INIT_TXT            "Time stamping module initialized."

#define DS1103_ELOG_INIT                210
#define DS1103_ELOG_INIT_TXT            "Event Logging module initialized."

#define DS1103_INIT_LOCK                211
#define DS1103_INIT_LOCK_TXT            "Only the first of multiple calls was actually performed."

#define DS1103_DSMCR_INIT               212
#define DS1103_DSMCR_INIT_TXT           "Multi Client Ringbuffer module initialized."

/* module Io1103.c */

#define DS1103_ADC_CH_POLL_START        300
#define DS1103_ADC_CH_POLL_START_TXT    "_adc_read_ch(): busy flag polling started."

#define DS1103_ADC_CH_POLL_END          301
#define DS1103_ADC_CH_POLL_END_TXT      "_adc_read_ch(): busy flag polling finished."

#define DS1103_ADC_CONV_POLL_START      302
#define DS1103_ADC_CONV_POLL_START_TXT  "_adc_read_conv(): busy flag polling started."

#define DS1103_ADC_CONV_POLL_END        303
#define DS1103_ADC_CONV_POLL_END_TXT    "_adc_read_conv(): busy flag polling finished."

#define DS1103_ADC_READ2_POLL_START      304
#define DS1103_ADC_READ2_POLL_START_TXT  "_adc_read2(): busy flag polling started."

#define DS1103_ADC_READ2_POLL_END        305
#define DS1103_ADC_READ2_POLL_END_TXT    "_adc_read2(): busy flag polling finished."

#define DS1103_ADC_READ_ALL_12BIT_POLL_START      306
#define DS1103_ADC_READ_ALL_12BIT_POLL_START_TXT  "_adc_read_all(): 12 Bit busy flag polling started."

#define DS1103_ADC_READ_ALL_12BIT_POLL_END        307
#define DS1103_ADC_READ_ALL_12BIT_POLL_END_TXT    "_adc_read_all(): 12 Bit busy flag polling finished."

#define DS1103_ADC_READ_ALL_16BIT_POLL_START      308
#define DS1103_ADC_READ_ALL_16BIT_POLL_START_TXT  "_adc_read_all(): 16 Bit busy flag polling started."

#define DS1103_ADC_READ_ALL_16BIT_POLL_END        309
#define DS1103_ADC_READ_ALL_16BIT_POLL_END_TXT    "_adc_read_all(): 16 Bit busy flag polling finished."

#define DS1103_CPU_TEMP_POLL_START       310
#define DS1103_CPU_TEMP_POLL_START_TXT   "_cpu_temperature(): rdy/ack polling started."

#define DS1103_CPU_TEMP_POLL_END         311
#define DS1103_CPU_TEMP_POLL_END_TXT     "_cpu_temperature(): rdy/ack polling finished."

/* module ser1103.c */

#define DS1103_UART_TX_EMPTY_POLL_START       400
#define DS1103_UART_TX_EMPTY_POLL_START_TXT   "Serial interface (UART): tx_empty polling started."

#define DS1103_UART_TX_EMPTY_POLL_END         401
#define DS1103_UART_TX_EMPTY_POLL_END_TXT     "Serial interface (UART): tx_empty polling finished."

#define DS1103_UART_REC_READ_POLL_START      402
#define DS1103_UART_REC_READ_POLL_START_TXT  "Serial interface (UART): receiver read polling started."

#define DS1103_UART_REC_READ_POLL_END        403
#define DS1103_UART_REC_READ_POLL_END_TXT    "Serial interface (UART): receiver read polling finished."



#endif /* __DS1103_MSG_H__ */

















