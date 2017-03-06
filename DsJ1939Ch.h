/** <!------------------------------------------------------------------------->
*
*  @file DsJ1939Ch.h
*
*  @brief Functions for DsJ1939 channel handling.
*
*  @author
*    AlexanderSt
*
*  @description
*    This module provides J1939 channel specific functions. Channel handles
*    are required to create nodes. Any communication on a J1939 network takes
*    places between nodes. A J1939 channel is intended to work with a
*    physical CAN channel as its counterpart.
*    The hardware abstraction (CAN interface, timer) has to be realized by
*    users of the DsJ1939 driver, the component is intended to be platform
*    independent.
*
*    The most important interfaces are:
*    - Timer: #DsJ1939Ch_update()
*    - Transmission: #DsJ1939Ch_connectTxCb()
*    - Reception: #DsJ1939Ch_processRx()
*
*    Basically the (most common) mode of operation can be described as follows:
*    The application invokes #DsJ1939Ch_update() within a cyclic timer task to
*    provide the current time to the driver. Within #DsJ1939Ch_update() the
*    driver triggers any required transmissions by invoking a callback
*    functions connected by the application during initialization
*    (DsJ1939Ch_connectTxCb()). The application reads any received CAN frames
*    (loop-back and external) from the CAN hardware and provides it to the
*    driver using DsJ1939Ch_processRx().
*
*  @copyright
*    Copyright 2009, dSPACE GmbH. All rights reserved.
*
*  @version
*    $RCSfile: DsJ1939Ch.h $ $Revision: 1.5 $ $Date: 2009/06/16 14:50:58GMT+01:00 $
*    $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DsJ1939.pj $
*
*   <hr><br>
*<!-------------------------------------------------------------------------->*/
#ifndef DSJ1939CH_H_
#define DSJ1939CH_H_


#include <DsJ1939Def.h>
#include <DsJ1939Error.h>


/** The application may provide a callback function that is invoked by the J1939
 *  driver whenever it has successfully received a PGN.
 *
 *  @see
 *  - #DsJ1939Ch_connectRxMonitorCb() */
typedef void (*DsTJ1939Ch_RxCbFunc) (const DsSJ1939_Msg* pMsg, const void* pArg);

/** The application may provide a callback function that is invoked by the J1939
 *  driver whenever the internal NAME-address table changes. This function can be
 *  used for network management purposes.
 *
 *  @see
 *  - #DsJ1939Ch_connectNmCb() */
typedef void (*DsTJ1939Ch_NmCbFunc) (ULong64 Name, UInt8 Address, const void* pArg);

/** The application has to provide a callback function that is invoked by the
 *  J1939 driver whenever it intends to transmit a frame (required interface if
 *  stack shall transmit on the bus!). The return value has to be used to signal
 *  whether the transmission was successful or not.
 *
 *  @see
 *  - #DsJ1939Ch_connectTxCb() */
typedef DsEJ1939_TxState (*DsTJ1939Ch_TxCbFunc) (const DsSJ1939_Frame* pFrame, const void* pArg);

/** The application may provide a callback function that is invoked by the
 *  J1939 driver whenever it detects an address claiming conflict (ACC) for a
 *  DsJ1939 network node in mode #DSJ1939_NODE_TYPE_SIMULATED (Node).
 *  The callback should return an alternative address or the NULL address. If a
 *  node is not arbitrary address capable (i.e. the AAC bit in its NAME is 0)
 *  this callback does not need to be provided by the application.
 *  In case the application provides the global address or the conflicting
 *  nodes current address to the driver the NULL address is claimed.
 *
 *  @see
 *  - #DsJ1939Ch_connectAccCb() */
typedef UInt8 (*DsTJ1939Ch_AccCbFunc) (DsTJ1939Node Node, const void* pArg);

#ifdef DSJ1939_FAST_PACKET_SUPPORT
/** The application has to provide a callback function to help the stack 
 *  distinguishing normal PDUs from fast-packet PDUs if this feature is
 *  required. The callback is invoked with the Pgn in question as argument. The
 *  stack expects the return code to indicate how it has to interpret the PDU.
 *
 *  @see
 *  - #DsJ1939Ch_connectFpCb() */
typedef DsEJ1939_MsgMode (*DsTJ1939Ch_FpCbFunc) (UInt32 Pgn, const void* pArg);
#endif


#if defined (__cplusplus)
extern "C" {
#endif


DsTError DsJ1939Ch_create(DsTJ1939Ch* pCh, const void* pChId);

DsTError DsJ1939Ch_destroy(DsTJ1939Ch* pCh);

DsTError DsJ1939Ch_start(DsTJ1939Ch Ch);

DsTError DsJ1939Ch_stop(DsTJ1939Ch Ch);

DsTError DsJ1939Ch_reset(DsTJ1939Ch Ch);

DsTError DsJ1939Ch_setDescriptor(DsTJ1939Ch Ch, const char* pDescriptor);

DsTError DsJ1939Ch_setMsgBuffer(DsTJ1939Ch Ch, UInt32 NumElements, UInt32 MaxDlc);

DsTError DsJ1939Ch_setFrameBuffer(DsTJ1939Ch Ch, UInt32 NumElements);

DsTError DsJ1939Ch_apply(DsTJ1939Ch Ch);

DsTError DsJ1939Ch_connectRxMonitorCb(DsTJ1939Ch Ch, DsTJ1939Ch_RxCbFunc pCb, const void* pArg);

DsTError DsJ1939Ch_connectTxCb(DsTJ1939Ch Ch, DsTJ1939Ch_TxCbFunc pCb, const void* pArg);

DsTError DsJ1939Ch_connectNmCb(DsTJ1939Ch Ch, DsTJ1939Ch_NmCbFunc pCb, const void* pArg);

DsTError DsJ1939Ch_connectAccCb(DsTJ1939Ch Ch, DsTJ1939Ch_AccCbFunc pCb, const void* pArg);

DsTError DsJ1939Ch_getUnusedAddress(DsTJ1939Ch Ch, UInt8* pAddress);

DsTError DsJ1939Ch_processRx(DsTJ1939Ch Ch, const DsSJ1939_Frame* pFrame, DsSJ1939_Msg* pMsg);

DsTError DsJ1939Ch_update(DsTJ1939Ch Ch, Float64 Time);

#ifdef DSJ1939_FAST_PACKET_SUPPORT
DsTError DsJ1939Ch_connectFpCb(DsTJ1939Ch Ch, DsTJ1939Ch_FpCbFunc pCb, const void* pArg);
#endif

#if defined (__cplusplus)
}
#endif


#endif /* DSJ1939CH_H_ */
