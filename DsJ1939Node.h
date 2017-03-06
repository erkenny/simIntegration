/** <!------------------------------------------------------------------------->
*
*  @file DsJ1939Node.h
*
*  @brief Functions for DsJ1939 network node handling.
*
*  @author
*    AlexanderSt
*
*  @description
*    This module provides J1939 network node specific functions. Any
*    communication on a J1939 network takes places between nodes (if not
*    directed to global address). A node is characterized by its 64 bit J1939
*    NAME. The network management behavior as well as some CMDT related
*    parameters can be configured per node.
*
*  @copyright
*    Copyright 2009, dSPACE GmbH. All rights reserved.
*
*  @version
*    $RCSfile: DsJ1939Node.h $ $Revision: 1.11 $ $Date: 2009/07/27 09:12:08GMT+01:00 $
*    $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DsJ1939.pj $
*
*   <hr><br>
*<!-------------------------------------------------------------------------->*/
#ifndef DSJ1939NODE_H_
#define DSJ1939NODE_H_


#include <DsJ1939Def.h>
#include <DsJ1939Error.h>


/** J1939 node NAME structure */
struct DsSJ1939Node_Name
{
    UInt8 ArbitraryAddressCapable;          /**< 1 bit: Arbitrary Address Capable */
    UInt8 IndustryGroup;                    /**< 3 bit: Industry Group */
    UInt8 VehicleSystemInstance;            /**< 4 bit: Vehicle System Instance */
    UInt8 VehicleSystem;                    /**< 7 bit: Vehicle System */
    UInt8 Reserved;                         /**< 1 bit: Reserved */
    UInt8 Function;                         /**< 8 bit: Function */
    UInt8 FunctionInstance;                 /**< 5 bit: Function Instance */
    UInt8 ECUInstance;                      /**< 3 bit: ECU Instance */
    UInt16 ManufacturerCode;                /**< 11 bit: Manufacturer Code */
    UInt32 IdentityNumber;                  /**< 21 bit: Identity Number */
};
typedef struct DsSJ1939Node_Name DsSJ1939Node_Name;


/** DsJ1939 network node states enumeration, examples:
 *  - Successful address claim for node of type #DSJ1939_NODE_TYPE_SIMULATED:
 *      - #DSJ1939_NODE_STATE_INIT
 *      - #DSJ1939_NODE_STATE_ADDRESS_CLAIMING
 *      - #DSJ1939_NODE_STATE_ADDRESS_CLAIMED (The transition delay for the
 *        last two states can be defined by the application
 *        (#DsJ1939Node_setAddressClaimingContTime()).
 *  - Successful claim for node of type #DSJ1939_NODE_TYPE_EXTERNAL:
 *      - #DSJ1939_NODE_STATE_ADDRESS_UNVERIFIED
 *      - #DSJ1939_NODE_STATE_ADDRESS_CLAIMED (State changes on reception of
 *        the address claimed message of the node).
 *  - Unsuccessful attempt of external node to claim address:
 *      - #DSJ1939_NODE_STATE_ADDRESS_UNVERIFIED
 *      - #DSJ1939_NODE_STATE_ADDRESS_CLAIM_FAILED (immediately on reception of
 *                                                  cannot claim)<br>
 *    Or alternatively:
 *      - #DSJ1939_NODE_STATE_ADDRESS_UNVERIFIED
 *      - #DSJ1939_NODE_STATE_ADDRESS_CLAIMED (immediately on reception of
 *                                             address claim)
 *      - #DSJ1939_NODE_STATE_ADDRESS_CLAIM_FAILED (immediately on reception of
 *                                                  cannot claim)
 *  - A simulated node directly moves from #DSJ1939_NODE_STATE_INIT to
 *    #DSJ1939_NODE_STATE_ADDRESS_CLAIMING_DISABLED if it is configured to do
 *    no address claiming (#DsJ1939Node_setAddressClaimingMode()).
 *  - An external node remains in state #DSJ1939_NODE_STATE_ADDRESS_UNVERIFIED as
 *    long as no address claim or cannot claim of it is received on the channel.
 *
 *  @see
 *  - #DsJ1939Node_getState() */
enum DsEJ1939Node_State
{
    DSJ1939_NODE_STATE_INIT = 1,                /**< Node not enabled */
    DSJ1939_NODE_STATE_ADDRESS_CLAIMING = 2,    /**< Node about to claim an address */
    DSJ1939_NODE_STATE_ADDRESS_CLAIMED = 3,     /**< Node has successfully claimed an address */
    DSJ1939_NODE_STATE_ADDRESS_CLAIMING_DISABLED = 4,/**< Node enabled but does not claim an address */
    DSJ1939_NODE_STATE_ADDRESS_CLAIM_FAILED = 5,/**< Node could not claim an address */
    DSJ1939_NODE_STATE_ADDRESS_UNVERIFIED = 6   /**< Node address (external node) could not yet be verified */
};
typedef enum DsEJ1939Node_State DsEJ1939Node_State;

/** DsJ1939 network node types, note that some node specific functions do not
 *  work for nodes of type #DSJ1939_NODE_TYPE_EXTERNAL.
 *
 *  @see
 *  - #DsJ1939Node_create() */
enum DsEJ1939Node_Type
{
    DSJ1939_NODE_TYPE_SIMULATED = 1,            /**< Node is to be simulated */
    DSJ1939_NODE_TYPE_EXTERNAL = 2              /**< Node is external */
};
typedef enum DsEJ1939Node_Type DsEJ1939Node_Type;


#if defined (__cplusplus)
extern "C" {
#endif


DsTError DsJ1939Node_create(DsTJ1939Node* pNode, DsTJ1939Ch Ch, ULong64 Name, DsEJ1939Node_Type Type);

DsTError DsJ1939Node_createUnique(DsTJ1939Node* pNode, DsTJ1939Ch Ch, DsEJ1939Node_Type Type);

DsTError DsJ1939Node_destroy(DsTJ1939Node* pNode);

DsTError DsJ1939Node_start(DsTJ1939Node Node);

DsTError DsJ1939Node_stop(DsTJ1939Node Node);

DsTError DsJ1939Node_reset(DsTJ1939Node Node);

DsTError DsJ1939Node_setDescriptor(DsTJ1939Node Node, const char* pDescriptor);

DsTError DsJ1939Node_setCMDTSizeParameters(DsTJ1939Node Node, UInt8 NumPduRts, UInt8 NumPduCts);

DsTError DsJ1939Node_setCMDTTimingParameters(DsTJ1939Node Node, Float64 BamPduSpace, Float64 RtsCtsPduSpace);

DsTError DsJ1939Node_setAddress(DsTJ1939Node Node, UInt8 Address);

DsTError DsJ1939Node_setAddressClaimingMode(DsTJ1939Node Node, DsEJ1939_Mode Mode);

DsTError DsJ1939Node_setReqForAddressClaimedMode(DsTJ1939Node Node, DsEJ1939_Mode Mode);

DsTError DsJ1939Node_setCommandedAddressMode(DsTJ1939Node Node, DsEJ1939_Mode Mode);

DsTError DsJ1939Node_setAddressClaimingContTime(DsTJ1939Node Node, Float64 Time);

DsTError DsJ1939Node_setSilentMode(DsTJ1939Node Node, DsEJ1939_Mode Mode);

DsTError DsJ1939Node_apply(DsTJ1939Node Node);

DsTError DsJ1939Node_getState(DsTJ1939Node Node, DsEJ1939Node_State* pState, UInt8* pAddress);

DsTError DsJ1939Node_transmit(const DsSJ1939_Msg* pMsg);

DsTError DsJ1939Node_convertNameFromStructToLong(const DsSJ1939Node_Name* pNameSrc, ULong64* pNameDest);

DsTError DsJ1939Node_convertNameFromLongToStruct(ULong64 NameSrc, DsSJ1939Node_Name* pNameDest);

#ifdef DSJ1939_FAST_PACKET_SUPPORT
DsTError DsJ1939Node_setFpTimingParameters(DsTJ1939Node Node, Float64 FpPduSpace);
#endif

#if defined (__cplusplus)
}
#endif


#endif /* DSJ1939NODE_H_ */
