/** <!------------------------------------------------------------------------->
*
*  @file DsJ1939Error.h
*
*  @brief Definitions/macros for DsJ1939 error handling.
*
*  @author
*    AlexanderSt
*
*  @description
*    This file contains all error codes and their associated error messages
*    which are used by the DsJ1939 driver.
*
*  @copyright
*    Copyright 2009, dSPACE GmbH. All rights reserved.
*
*  @version
*    $RCSfile: DsJ1939Error.h $ $Revision: 1.10 $ $Date: 2009/07/27 09:55:39GMT+01:00 $
*    $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DsJ1939.pj $
*
*   <hr><br>
*<!-------------------------------------------------------------------------->*/
#ifndef DSJ1939ERROR_H_
#define DSJ1939ERROR_H_


#include <DSMSG.H>


/** J1939 component ID (TBD: define in DSMSG.H) */
#define MSG_SM_DSJ1939                          46


/**
 * @name DsJ1939 error codes
 */

/** No error occurred. */
#define DSJ1939_NO_ERROR                        (DsTError)(0x00)
/** No error occurred. */
#define DSJ1939_NO_ERROR_TXT                    "" /* This condition emits no error/warning/info */

/** At least one function argument was a NULL pointer. */
#define DSJ1939_ERROR_NULL_POINTER              (DsTError)(0x01)
/** At least one function argument was a NULL pointer. */
#define DSJ1939_ERROR_NULL_POINTER_TXT          "%s: Argument %s provided to %s() was a NULL pointer."

/** The selected object (channel, node) does already exist. */
#define DSJ1939_INFO_OBJECT_REUSED              (DsTError)(0x02)
/** The selected object (channel, node) does already exist. */
#define DSJ1939_INFO_OBJECT_REUSED_TXT          "" /* This condition emits no error/warning/info */

/** A memory allocation error occurred. */
#define DSJ1939_ERROR_MEMORY_ALLOCATION         (DsTError)(0x03)
/** A memory allocation error occurred. */
#define DSJ1939_ERROR_MEMORY_ALLOCATION_TXT     "%s: A memory allocation error occurred in %s()."

/** The selected J1939 address is not valid in this context. */
#define DSJ1939_WARNING_INVALID_ADDRESS         (DsTError)(0x04)
/** The selected J1939 address is not valid in this context. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT1    "%s: The selected J1939 address %u passed to %s() is not allowed in this context."
/** A packet passed to the driver was invalid since transmission of message using
 *  the NULL address is only allowed for some special network management PGNs. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT2    "%s: A CMDT packet transmitted by a node using the NULL address (%u) has been detected in %s()."
/** A CMDT packet passed to the driver was invalid since transmission to the global
 *  address of non-BAM packets is not allowed. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT3    "%s: A CMDT packet directed to the global address (%u) has been detected in %s()."
/** The driver invoked an application callback to resolve an address conflict. The callback
 *  provided the conflicting address back. The driver will use the NULL address instead. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT4    "%s: The conflicting address %u has been passed to %s() to resolve an address conflict."
/** The driver invoked an application callback to resolve an address conflict. The callback
 *  provided the global address back. The driver will use the NULL address instead. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT5    "%s: The global address address %u has been passed to %s() to resolve an address conflict."
/** An address claimed PDU passed to the driver was directed to a non-global address. The J1939
 *  standard advises to direct this kind of PDU always to the global address. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT6    "%s: An address claimed PDU directed to a non-global address (%u) has been detected in %s()."
/** A node simulated by the driver was commanded with the global address. This is not valid
 *  and the node will ignore this command. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT7    "%s: A commanded address message commanding the global address (%u) has been detected in %s()."
/** A packet passed to the driver was originated by a node using the global address. This
 *  is not allowed for any kind of PGN. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT8    "%s: A PDU transmitted by a node using the global address (%u) has been detected in %s()."
/** A packet passed to the driver was directed to the NULL address. This is not allowed
 *  for any kind of PGN. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT9    "%s: A PDU directed to the NULL address (%u) has been detected in %s()."
/** The requested PGN can not be transmitted using a node that has not claimed an address.
 *  Only a few network mangement related PGNs are allowed to be transmitted in this case. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT10   "%s: A source node using the NULL address (%u) is not allowed to execute the requested operation %s()."
/** A PDU1 message can not be directed to a node that has not successfully claimed an address. */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT11   "%s: A destination node using the NULL address (%u) is not allowed to execute the requested operation %s()."
/** A PDU2 message can not be directed to a non-global destination */
#define DSJ1939_WARNING_INVALID_ADDRESS_TXT12   "%s: A destination node using a non-global address (%u) is not allowed to execute the requested operation %s()."

/** The selected data length code exceeds the allowed range for J1939 messages or CAN frames. */
#define DSJ1939_WARNING_INVALID_DLC             (DsTError)(0x05)
/** The selected data length code exceeds the allowed range for J1939 messages. */
#define DSJ1939_WARNING_INVALID_DLC_TXT         "%s: The selected data length code %u passed to %s() exceeds the allowed range (%u to %u)."

/** The selected parameter group number is not supported (undefined bits set or PS != 0 for PDU1 messages. */
#define DSJ1939_WARNING_INVALID_PGN             (DsTError)(0x06)
/** The selected parameter group number is not supported (undefined bits set or PS != 0 for PDU1 messages. */
#define DSJ1939_WARNING_INVALID_PGN_TXT1        "%s: The selected PGN %u passed to %s() is not supported."
/** The selected parameter group number is invalid in this context. PDU2 PGNs shall be
 *  transmitted using BAM instead of RTS/CTS. */
#define DSJ1939_WARNING_INVALID_PGN_TXT2        "%s: The selected PGN %u passed to %s() is invalid (PDU2 in RTS/CTS detected)."
#ifdef DSJ1939_FAST_PACKET_SUPPORT
/** The selected parameter group number is invalid in this context. PDU2 PGNs shall be
 *  transmitted using BAM instead of fast-packet mode. */
#define DSJ1939_WARNING_INVALID_PGN_TXT3        "%s: The selected PGN %u passed to %s() is invalid (PDU2 for fast-packet)."
#endif

/** The selected priority for a J1939 message exceeds the allowed range. */
#define DSJ1939_WARNING_INVALID_PRIORITY        (DsTError)(0x07)
/** The selected priority for a J1939 message exceeds the allowed range. */
#define DSJ1939_WARNING_INVALID_PRIORITY_TXT    "%s: The selected priority %u passed to %s() exceeds the allowed range (0 to 7)."

/** The driver is not able to handle the request because it ran out of internal buffers. */
#define DSJ1939_WARNING_BUFFER_OVERRUN          (DsTError)(0x08)
/** The driver is not able to handle the request because it ran out of internal buffers.
 *  Consider increasing the buffer size using #DsJ1939Ch_setMsgBuffer() and make sure that
 *  the communication scenario does not exceed the physical limits of the CAN bus. */
#define DSJ1939_WARNING_BUFFER_OVERRUN_TXT      "%s: A buffer overrun occurred in %s()."

/** The driver was not able to handle a message because of its size. */
#define DSJ1939_WARNING_BUFFER_SIZE             (DsTError)(0x09)
/** The driver was not able to handle a message because of its size. Consider increasing the limit
 *  using #DsJ1939Ch_setMsgBuffer(). */
#define DSJ1939_WARNING_BUFFER_SIZE_TXT         "%s: Could not handle message in %s() because of its size (%u)."

/** The requested operation is not supported for an external node. */
#define DSJ1939_WARNING_NODE_EXTERN             (DsTError)(0x0A)
/** The requested operation is not supported for an external node. */
#define DSJ1939_WARNING_NODE_EXTERN_TXT         "%s: The requested operation %s() is not supported for an external node."

/** The requested operation can not be performed in the current node state. */
#define DSJ1939_WARNING_INVALID_STATE           (DsTError)(0x0B)
/** The requested operation can not be performed in the current node state. */
#define DSJ1939_WARNING_INVALID_STATE_TXT       "%s: The requested operation %s() is not supported for a node in state %s."

/** The provided time is out of range. */
#define DSJ1939_WARNING_INVALID_TIME            (DsTError)(0x0C)
/** The provided time is out of range. */
#define DSJ1939_WARNING_INVALID_TIME_TXT        "%s: The selected time %lf passed to %s() is either negative or not ascending."

/** The selected CMDT packet number is out of range. */
#define DSJ1939_WARNING_INVALID_PACKET_NUMBER   (DsTError)(0x0D)
/** The selected CMDT packet number is out of range. */
#define DSJ1939_WARNING_INVALID_PACKET_NUMBER_TXT "%s: The selected packet number %u passed to %s() exceeds the allowed range (%u to %u)."

/** The selected CAN identifier is out of range. */
#define DSJ1939_WARNING_INVALID_CAN_ID          (DsTError)(0x0E)
/** The selected CAN identifier is out of range. */
#define DSJ1939_WARNING_INVALID_CAN_ID_TXT      "%s: The selected CAN identifier 0x%08x passed to %s() exceeds the allowed range (0 to 0x1FFFFFFF)."

/** The selected #DsEJ1939Node_Type is invalid. */
#define DSJ1939_ERROR_INVALID_TYPE              (DsTError)(0x0F)
/** The selected #DsEJ1939Node_Type is invalid. */
#define DSJ1939_ERROR_INVALID_TYPE_TXT          "%s: The selected type %d passed to %s() is not supported."

/** The selected #DsEJ1939_Mode is invalid. */
#define DSJ1939_WARNING_INVALID_MODE            (DsTError)(0x10)
/** The selected #DsEJ1939_Mode is invalid. */
#define DSJ1939_WARNING_INVALID_MODE_TXT        "%s: The selected node mode %d passed to %s() is not supported."

/** The requested connection does already exist. */
#define DSJ1939_INFO_CONN_EXISTS                (DsTError)(0x11)
/** The requested connection does already exist. */
#define DSJ1939_INFO_CONN_EXISTS_TXT            /* This condition emits no error/warning/info */

/** An internal error occurred. */
#define DSJ1939_ERROR_INTERNAL                  (DsTError)(0x12)
/** An internal error occurred. */
#define DSJ1939_ERROR_INTERNAL_TXT              "%s: An internal error occurred in %s()."

/** A J1939 NAME conflict has been detected */
#define DSJ1939_WARNING_NAME_CONFLICT           (DsTError)(0x13)
/** A J1939 NAME conflict has been detected, make sure that all nodes on the network use unique names. */
#define DSJ1939_WARNING_NAME_CONFLICT_TXT       "%s: A J1939 NAME conflict for 0x%08x%08x has been detected in %s()."

/** A timeout has been detected */
#define DSJ1939_WARNING_TIMEOUT                 (DsTError)(0x14)
/** A timeout has been detected, make sure that #DsJ1939Ch_update() is called with a sufficient rate. */
#define DSJ1939_WARNING_TIMEOUT_TXT             "%s: A timeout caused by calling the driver with an insufficient rate has been detected in %s()."

/** Invalid nodes have been detected. */
#define DSJ1939_WARNING_INVALID_NODE            (DsTError)(0x15)
/** Nodes handles of different channels have been mixed within a message object to be transmitted. */
#define DSJ1939_WARNING_INVALID_NODE_TXT1       "%s: Mixing node handles of different channels is not allowed (%s())."
/** The node handles of a message object to be transmitted shall not be identical. */
#define DSJ1939_WARNING_INVALID_NODE_TXT2       "%s: Source and destination node are not allowed to be identical (%s())."

/** The descriptor passed to the driver exceeds the maximum allowed length. */
#define DSJ1939_WARNING_INVALID_DESCRIPTOR_SIZE (DsTError)(0x16)
/** The descriptor passed to the driver exceeds the maximum allowed length. */
#define DSJ1939_WARNING_INVALID_DESCRIPTOR_SIZE_TXT "%s: The selected descriptor passed to %s() exceeds the maximum allowed length (%u)."

/** The selected NAME does already exist. */
#define DSJ1939_WARNING_NAME_EXISTS             (DsTError)(0x17)
/** The selected NAME does already exist. */
#define DSJ1939_WARNING_NAME_EXISTS_TXT         "%s: The selected NAME 0x%08x%08x passed to %s() does already exist."

/** A NAME field is out of range */
#define DSJ1939_WARNING_INVALID_NAME_FIELD      (DsTError)(0x18)
/** A NAME field is out of range */
#define DSJ1939_WARNING_INVALID_NAME_FIELD_TXT  "%s: The NAME field %s passed to %s() exceeds the allowed range (0 to %u)"

/** Invalid message mode has been detected */
#define DSJ1939_WARNING_INVALID_MSG_MODE        (DsTError)(0x19)
/** Invalid message mode has been detected */
#define DSJ1939_WARNING_INVALID_MSG_MODE_TXT    "%s: The message mode %d passed to %s() is not supported."

/** Invalid NAME has been detected */
#define DSJ1939_WARNING_INVALID_NAME            (DsTError)(0x1A)
/** Invalid NAME has been detected */
#define DSJ1939_WARNING_INVALID_NAME_TXT        "%s: The node NAME passed to %s() is invalid."


#endif /* DSJ1939ERROR_H_ */

