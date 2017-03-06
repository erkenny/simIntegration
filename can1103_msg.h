/******************************************************************************
*
* MODULE
*   C164 slave MC access functions for DS1103 controller board
*
*   Version for PPC master CPU
*
* FILE
*   can1103.h
*
* RELATED FILES
*   can1103.c, dsmcom.h
*
* DESCRIPTION
*   DS1103 slave MC access functions for the CAN bus.
*
*
* AUTHOR(S)
*   A.Gropengieﬂer, O.Jabczinski
*
* Copyright dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: can1103_msg.h $   $Revision: 1.3 $   $Date: 2008/03/14 18:01:42GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#ifndef __CAN1103_MSG_H__
#define __CAN1103_MSG_H__

#include <dsmcom.h>

/******************************************************************************
*  error definitions
******************************************************************************/

#define   DS1103_CAN_TYPE_ERROR                         100

#define   DS1103_CAN_ALLOC_ERROR                        101
#define   DS1103_CAN_ALLOC_ERROR_TXT                    \
": Memory allocation failed."

#define   DS1103_CAN_ILLEGAL_QUEUE                      102
#define   DS1103_CAN_ILLEGAL_QUEUE_TXT                  \
": Illegal communication queue."

#define   DS1103_CAN_ILLEGAL_INDEX                      103
#define   DS1103_CAN_ILLEGAL_INDEX_TXT                  \
": Illegal communication function index."

#define   DS1103_CAN_REG_BUFFER_OVERFLOW                104
#define   DS1103_CAN_REG_BUFFER_OVERFLOW_TXT            \
": Master to slave communication buffer overflow."

#define   DS1103_CAN_TIMEOUT                            106
#define   DS1103_CAN_TIMEOUT_TXT                        \
": Slave not responding."

#define   DS1103_CAN_REG_SLAVE_ALLOC_ERROR              107
#define   DS1103_CAN_REG_SLAVE_ALLOC_ERROR_TXT          \
": Memory allocation error on slave."

#define   DS1103_CAN_REG_SLAVE_BUFFER_OVERFLOW         108
#define   DS1103_CAN_REG_SLAVE_BUFFER_OVERFLOW_TXT      \
": Slave to master communication buffer overflow."

#define   DS1103_CAN_SLAVE_FIRMWARE_WRONG              109
#define   DS1103_CAN_SLAVE_FIRMWARE_WRONG_TXT           \
": Firmware rev. %d.%d or higher required."

#define DS1103_CAN_SLAVE_FIRMWARE_WRONG2_TXT             \
"Refer to <DSPACE_ROOT>\\Ds1103\\CAN\\Firmware\\DS1103CAN_FW%d_%d_x_Prg.txt.",\
REQ_DS1103_CAN_FIRMWARE_VER_MAR, REQ_DS1103_CAN_FIRMWARE_VER_MIR

#define   DS1103_CAN_TIMEOUT_OR_WRONG_FIRMWARE          110
#define   DS1103_CAN_TIMEOUT_OR_WRONG_FIRMWARE_TXT      \
": Slave not responding or wrong firmware."

#define   DS1103_CAN_ILLEGAL_BAUDRATE                   120
#define   DS1103_CAN_ILLEGAL_BAUDRATE_TXT               \
": Illegal baudrate."

#define   DS1103_CAN_BAUDRATE_L_ERROR                   121
#define   DS1103_CAN_BAUDRATE_L_ERROR_TXT               \
": Baudrate too low (min. 10000 baud)!"

#define   DS1103_CAN_BAUDRATE_H_ERROR                   122
#define   DS1103_CAN_BAUDRATE_H_ERROR_TXT               \
": Baudrate too high (max. 1000000 baud)!"

#define   DS1103_CAN_MIN_OSCCLOCK_ERROR                 124
#define   DS1103_CAN_MIN_OSCCLOCK_ERROR_TXT             \
": Oscillator-clock < 5 MHz!"

#define   DS1103_CAN_MAX_OSCCLOCK_ERROR                 125
#define   DS1103_CAN_MAX_OSCCLOCK_ERROR_TXT             \
": Oscillator-clock > 20 MHz!"

#define   DS1103_CAN_ILLEGAL_FORMAT                     140
#define   DS1103_CAN_ILLEGAL_FORMAT_TXT                 \
": format_check: Illegal message format."

#define   DS1103_CAN_SUBINT_OUT_OF_RANGE                141
#define   DS1103_CAN_SUBINT_OUT_OF_RANGE_TXT            \
": subint: Use range 0...14 !"

#define   DS1103_CAN_ILLEGAL_INT_NO                     142
#define   DS1103_CAN_ILLEGAL_INT_NO_TXT                 \
": subinterrupt number is used for busoff."

#define   DS1103_CAN_ILLEGAL_ID                         143
#define   DS1103_CAN_ILLEGAL_ID_TXT                     \
": Illegal message ID or ID conflict."

#define   DS1103_CAN_TOO_MUCH_MSG                       144
#define   DS1103_CAN_TOO_MUCH_MSG_TXT                   \
": Too much messages (max. 100)!"

#define   DS1103_CAN_STARTTIME_ERROR                    145
#define   DS1103_CAN_STARTTIME_ERROR_TXT                \
": Start time to high (max. 420s)!"

#define   DS1103_CAN_REP_TIME_HIGH_ERROR                146
#define   DS1103_CAN_REP_TIME_HIGH_ERROR_TXT            \
": Repetition time too high (max.100s)!"

#define   DS1103_CAN_REP_TIME_LOW_ERROR                 147
#define   DS1103_CAN_REP_TIME_LOW_ERROR_TXT             \
": Repetition time too low !"

#define   DS1103_CAN_TIMEOUT_HIGH_ERROR                 148
#define   DS1103_CAN_TIMEOUT_HIGH_ERROR_TXT             \
": Timeout time too high (max. 100s)!"

#define   DS1103_CAN_TIMEOUT_LOW_ERROR                  149
#define   DS1103_CAN_TIMEOUT_LOW_ERROR_TXT              \
": Timeout time too low !"

#define   DS1103_CAN_NO_RM_MAILBOX_FREE                 150
#define   DS1103_CAN_NO_RM_MAILBOX_FREE_TXT             \
": No remote mailbox free (max. 10)."

#define   DS1103_CAN_RQTX_NOT_REG                       151
#define   DS1103_CAN_RQTX_NOT_REG_TXT                   \
": RQTX message wasn't registered before!"

#define   DS1103_CAN_CHANNEL_NOT_INIT_ERROR             152
#define   DS1103_CAN_CHANNEL_NOT_INIT_ERROR_TXT         \
": The CAN channel is not initialized."

#define   DS1103_CAN_SEND_ID_QUEUED_INIT_ERROR             153
#define   DS1103_CAN_SEND_ID_QUEUED_INIT_ERROR_TXT   \
": TX queue: Not initialized!"

#define   DS1103_CAN_TXQUEUE_INIT_MSG_NOT_REG_ERROR        154
#define   DS1103_CAN_TXQUEUE_INIT_MSG_NOT_REG_ERROR_TXT    \
": TX message is not registered!" 

#define   DS1103_CAN_TXQUEUE_INIT_MSG_TYPE_ERROR           155
#define   DS1103_CAN_TXQUEUE_INIT_MSG_TYPE_ERROR_TXT    \
": Not a TX message!" 

#define   DS1103_CAN_BAUDRATE_SET_BAUDR_ERROR              156 

#define   DS1103_CAN_CHANNEL_SET_PARAM_ERROR               157

#define   DS1103_CAN_PLLFREQ_ERROR                         201
#define   DS1103_CAN_PLLFREQ_ERROR_TXT                  \
": Illegal PLL frequency!"

/* Warnings */

#define   DS1103_CAN_OSCCLOCK_WARN                         300
#define   DS1103_CAN_OSCCLOCK_WARN_TXT                  \
": Osc.-clock < 10 MHz: Low performance!"

#define   DS1103_CAN_TXQUEUE_INIT_DELAYTIME_WARN           301
#define   DS1103_CAN_TXQUEUE_INIT_DELAYTIME_WARN_TXT   \
"delay time: Too high (max. 10s)! Set to maximum."

#define   DS1103_CAN_BAUDRATE_WARN                         302
#define   DS1103_CAN_BAUDRATE_WARN_TXT         \
"CAN1103: baudrate doesn't match the desired baudrate. New baudrate = %d bit/s"

/* Infos */
#define   DS1103_CAN_ILLEGAL_ID_INFO_TXT                \
"Message ID out of range: 0x%X", canMsg->identifier

#define   DS1103_CAN_CONFLICT_ID_INFO_TXT               \
"Message ID conflict: 0x%X", canMsg->identifier

#define   DS1103_CAN_VERSION_INFO                          250


#endif /*__CAN1103_MSG_H__*/
