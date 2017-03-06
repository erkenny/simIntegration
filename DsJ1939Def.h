/** <!------------------------------------------------------------------------->
*
*  @file DsJ1939Def.h
*
*  @brief DsJ1939 specific public definitions.
*
*  @author
*    AlexanderSt
*
*  @description
*    This file provides DsJ1939 specific public definitions that are used by
*    the application and the driver (interface). Structures and enumerations
*    used by multiple DsJ1939 modules are declared in this file.
*
*  @copyright
*    Copyright 2009, dSPACE GmbH. All rights reserved.
*
*  @version
*    $RCSfile: DsJ1939Def.h $ $Revision: 1.6 $ $Date: 2009/07/01 09:17:03GMT+01:00 $
*    $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DsJ1939.pj $
*
*   <hr><br>
*<!-------------------------------------------------------------------------->*/
#ifndef DSJ1939DEF_H_
#define DSJ1939DEF_H_


#include <Dstypes.h>
#include <dsstd.h>


/** @internal
 *  Enables fast-packet support specified by NMEA2000
 *  Fast-packet mode in a nutshell:
 *  - Every packets has its PGN inside the CAN identifier, as for normal 
 *    non-CMDT PDUs 
 *  - One can't tell from looking at a PDU whether it is fast-packet or not
 *    (defined in database or corresponding standard)
 *  - First PDU contains:
 *      - Byte 0: 3 bit sequence counter in bits 5 to 7, remaining bits 0
 *      - Byte 1: Data length code (total number of data bytes, 0 to 223)
 *      - Bytes 2 to 7: data bytes
 *  - Up to 31 subsequent PDUs:
 *      - Byte 0: 3 bit sequence counter in bits 5 to 7, 5 bit frame counter
 *                (0 to 31) in other bits
 *      - Bytes 1 to 7: data bytes
 *  - No restrictions for timing compared to BAM */
/* #define DSJ1939_FAST_PACKET_SUPPORT */

/** Indicates invalid or unused PGN
 *
 *  @see
 *  - AssociatedPgn element in #DsSJ1939_Msg structure */
#define DSJ1939_PGN_INVALID 0xFFFFFFFFUL


/** A valid handle of this type is required to create DsJ1939 network nodes or
 *  to work on a physical bus channel.
 *
 *  @see
 *  - #DsJ1939Ch_create()
 *  - #DsJ1939Ch_destroy() */
typedef struct DsSJ1939Ch* DsTJ1939Ch;

/** A valid handle of this type is required to work with DsJ1939 network nodes.
 *
 *  @see
 *  - #DsJ1939Node_create()
 *  - #DsJ1939Node_destroy() */
typedef struct DsSJ1939Node* DsTJ1939Node;


/** DsJ1939 message status:
 *  @note:
 *  - If the message status does not equal #DSJ1939_MSG_STATUS_OK the data
 *    field of the #DsSJ1939_Msg structure is not necessarily valid.
 *
 *  @see
 *  - #DsSJ1939_Msg */
enum DsEJ1939_MsgStatus
{
    DSJ1939_MSG_STATUS_OK = 1,              /**< Message ok (no error) */
    DSJ1939_MSG_STATUS_TIMEOUT = 2,         /**< Message error - timeout occurred */
    DSJ1939_MSG_STATUS_INVALID_FIELD = 3,   /**< Message error - a field has an invalid value */
    DSJ1939_MSG_STATUS_INVALID_ADDRESS = 4, /**< Message error - an invalid address was detected */
    DSJ1939_MSG_STATUS_UNEXPECTED_DT = 5,   /**< Message error - an unexpected data transfer PDU was detected */
    DSJ1939_MSG_STATUS_UNEXPECTED_CM = 6,   /**< Message error - an unexpected connection management PDU was detected */
    DSJ1939_MSG_STATUS_NO_RESOURCES = 7,    /**< Message error - not enough resources to handle connection */
    DSJ1939_MSG_STATUS_MULT_CONNECTIONS = 8,/**< Message error - connection already established (RTS/CTS) */
    DSJ1939_MSG_STATUS_TOO_MANY_RETRIES = 9,/**< Message error - (RTS/CTS) too many retries required for connection */
    DSJ1939_MSG_STATUS_INVALID = 10          /**< Might be used by the application to detect whether a PDU was decoded into the
                                             message handle passed to the DsJ1939 driver when calling #DsJ1939Ch_processRx() */
    , DSJ1939_MSG_STATUS_UNEXPECTED_FP  /**< Message error - an unexpected fast-packet PDU was detected */
};
typedef enum DsEJ1939_MsgStatus DsEJ1939_MsgStatus;

/** DsJ1939 message or frame direction:
 *  @note
 *  - Single frame PDUs: The Dir field of the #DsSJ1939_Msg structure indicates
 *    whether the PDU has been transmitted on this channel (received via loop-
 *    back) or whether it was transmitted by an external node.
 *  - CMDT PDUs: The Dir field of the #DsSJ1939_Msg structure indicates the
 *    flow of data direction (DT PDUs). */
enum DsEJ1939_Direction
{
    DSJ1939_DIR_TRANSMIT = 1,               /**< Message or frame was received using internal loopback */
    DSJ1939_DIR_RECEIVE = 2                 /**< Message or frame was received from other channel */
};
typedef enum DsEJ1939_Direction DsEJ1939_Direction;

/** DSJ1939 transmit frame status: this is the returned status of a connected
 *  transmit callback function.
 *
 *  @see
 *  - #DsTJ1939Ch_TxCbFunc */
enum DsEJ1939_TxState
{
    DSJ1939_TX_FAILED = 1,                  /**< Frame transmission failed */
    DSJ1939_TX_OK = 2                       /**< Frame transmission was successful */
};
typedef enum DsEJ1939_TxState DsEJ1939_TxState;

/** DsJ1939 message mode: this is to distinguish NMEA 2000 fast-packet messages
 *  from normal J1939 / ISO11783 messages
 *  
 *  @see
 *  - #DsSJ1939_Msg */
enum DsEJ1939_MsgMode
{
    DSJ1939_MSG_MODE_NORMAL = 1,            /**< Normal message mode (J1939) */
    DSJ1939_MSG_MODE_FAST_PACKET = 2        /**< Fast packet mode (NMEA 2000) */
};
typedef enum DsEJ1939_MsgMode DsEJ1939_MsgMode;

/** DsJ1939 modes
 *
 *  @see
 *  - #DsJ1939Node_setAddressClaimingMode()
 *  - #DsJ1939Node_setReqForAddressClaimedMode()
 *  - #DsJ1939Node_setCommandedAddressMode() */
enum DsEJ1939_Mode
{
    DSJ1939_MODE_ENABLED = 1,               /**< Mode is enabled */
    DSJ1939_MODE_DISABLED = 2               /**< Mode is disabled */
};
typedef enum DsEJ1939_Mode DsEJ1939_Mode;


/** DsJ1939 CAN frame structure
 *  @note
 *   - Depending whether a frame has to be transmitted or has been received
 *     different fields of this structure are relevant.
 *       - Rx: all
 *       - Tx: all but Dir and TimeStamp
 *   - This structure intentionally has no format field, since the J1939
 *     driver is only able to handle extended frames.
 *
 *  @see
 *  - #DsJ1939Ch_processRx() */
struct DsSJ1939_Frame
{
    Float64 TimeStamp;                  /**< TimeStamp */
    UInt32  Id;                         /**< Identifier: 0x0 to 0x1FFFFFFF */
    UInt32  Dlc;                        /**< Data length code: 0 to 8 */
    DsEJ1939_Direction Dir;             /**< Loopback information */
    UInt8   Data[8];                    /**< Frame data bytes */
};
typedef struct DsSJ1939_Frame DsSJ1939_Frame;

/** DsJ1939 transmit message structure
 *  @note
 *    - Depending whether the message is to be transmitted or has been
 *      received different fields of this structure are relevant. (A 'x' in the
 *      following table marks relevant fields, a '-' fields to be ignored.
 *    - For multi-packet messages (Dlc > 8) the pData field of erroneously
 *      received messages is set to NULL by the driver to indicate invalid
 *      data and to prevent any application attempts to use the data.
 *    - The data page and reserved (extended data page) bit have no dedicated
 *      fields in the structure. They are considered to be part of the PGN.
 *    - Note that for received messages NULL == DestNode is only a necessary
 *      condition to detect whether it is non-destination specific, since the
 *      driver might simply not know the DestNode of the message. It is less
 *      error prone to check the DestAddress field for this matter.
 *
 *  <table align="center">
 *      <tr>    <td></td>               <td>Transmission</td>    <td>Reception</td>
 *      <tr>    <td>TimeStamp</td>      <td>-</td>               <td>x</td>           </tr>
 *      <tr>    <td>Pgn</td>            <td>x</td>               <td>x</td>           </tr>
 *      <tr>    <td>Dlc</td>            <td>x</td>               <td>x</td>           </tr>
 *      <tr>    <td>pData</td>          <td>x</td>               <td>x/-</td>         </tr>
 *      <tr>    <td>AssociatedPgn</td>  <td>-</td>               <td>x</td>           </tr>
 *      <tr>    <td>Status</td>         <td>-</td>               <td>x</td>           </tr>
 *      <tr>    <td>Dir</td>            <td>-</td>               <td>x</td>           </tr>
 *      <tr>    <td>SrcNode</td>        <td>x</td>               <td>x/-</td>         </tr>
 *      <tr>    <td>DestNode</td>       <td>x</td>               <td>x/-</td>         </tr>
 *      <tr>    <td>Priority</td>       <td>x</td>               <td>x</td>           </tr>
 *      <tr>    <td>SrcAddress</td>     <td>-</td>               <td>x</td>           </tr>
 *      <tr>    <td>DestAddress</td>    <td>-</td>               <td>x</td>           </tr>
 *  </table>
 *
 *  @see
 *  - #DsJ1939Node_transmit()
 *  - #DsTJ1939Ch_RxCbFunc */
struct DsSJ1939_Msg
{
    Float64 TimeStamp;                  /**< TimeStamp in seconds */
    UInt32  Pgn;                        /**< Parameter group number, 18 bit: EDP, DP, PDUF, PDUS */
    UInt32  Dlc;                        /**< Number of relevant bytes, data length code */
    UInt8*  pData;                      /**< Pointer to a location providing Dlc data bytes */
    UInt32  AssociatedPgn;              /**< Used for CMDT connections to indicate PGN of DT/CM PDUs,
                                             for single frame PDUs AssociatedPgn == #DSJ1939_PGN_INVALID */
    DsEJ1939_MsgStatus Status;          /**< Message status */
    DsEJ1939_Direction Dir;             /**< Message direction (rx or tx / loop-back) */
    DsTJ1939Node SrcNode;               /**< Source node (transmitter) for rx message if known
                                             by the driver, otherwise NULL */
    DsTJ1939Node DestNode;              /**< Destination node (receiver) for P2P rx message if known
                                             by the driver, otherwise NULL */
    UInt8   Priority;                   /**< Priority: 0 to 7 */
    UInt8   SrcAddress;                 /**< Source address */
    UInt8   DestAddress;                /**< Destination address */
    DsEJ1939_MsgMode Mode;              /**< Used to distinguish J1939 messages from NMEA 2000 messages */
    UInt8   SequenceCntr;               /**< 3 bit sequence counter (bit 5:7, bit 0:4 = 00000);
                                             counter is used to distinguish separate fast-packet messages
                                             of the same PGN */
};
typedef struct DsSJ1939_Msg DsSJ1939_Msg;


#endif /* DSJ1939DEF_H_ */
