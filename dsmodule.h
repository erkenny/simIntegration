/******************************************************************************
*  FILE:
*    DSMODULE.H
*
*  DESCRIPTION:
*    dSPACE module ID's and module specific data types
*
*  REMARKS:
*
*  AUTHOR(S):  G. Theidel, H.-G. Limberg
*
*  Copyright (c) 1999 dSPACE GmbH, GERMANY
*
* $RCSfile: dsmodule.h $ $Revision: 1.44 $ $Date: 2009/02/19 16:42:11GMT+01:00 $
*
******************************************************************************/


#ifndef __DSMODULE_H__
#define __DSMODULE_H__


/* ---- DSSVCM Root Entry -------------------------------------------------- */

#define  VCM_MID_DSVCM        0x03E9
#define  VCM_TXT_DSVCM        "Version Control Module"


/* ---- Hardware ----------------------------------------------------------- */

#define  VCM_MID_BOARD1005       0x03ED
#define  VCM_TXT_BOARD1005       "DS1005 Processor Board"

#define  VCM_MID_BOARD1006       0x03EE
#define  VCM_TXT_BOARD1006       "DS1006 Processor Board"


#define  VCM_MID_GL910           0x0400
#if defined (_DS1005)
#define  VCM_TXT_GL910           "DS910 Gigalink Module"
#elif defined (_DS1006)
#define  VCM_TXT_GL910           "DS911 Gigalink Module"
#endif

#define  VCM_MID_BOARD1103       0x0401
#define  VCM_TXT_BOARD1103       "DS1103 Controller Board"

#define  VCM_MID_BOARD1104       0x0402
#define  VCM_TXT_BOARD1104       "DS1104 Controller Board"

#define  VCM_MID_BOARD1401       0x0579
#define  VCM_TXT_BOARD1401       "DS1401 MABX Base Board"


/* ID 1501 - 1599 (0x05DD - 0x063F) reserved for MABX IO boards */
#define  VCM_MID_IO_BOARD        0x05DD                        /* def. 1501 */
#define  VCM_TXT_IO_BOARD        "DSXXXX MABX I/O Board"

/* ID 1600 - 1610 (0x0640 - 0x064A) reserved for Memory Emulators */

/* ---- Firmware ----------------------------------------------------------- */


#define  VCM_MID_FIRMWARE1005    0x1000
#define  VCM_TXT_FIRMWARE1005    "DS1005 Firmware"

#define  VCM_MID_PHSSCAN         0x1001
#define  VCM_TXT_PHSSCAN         "PHS Bus Scanner"

#define  VCM_MID_GLSCAN          0x1002
#define  VCM_TXT_GLSCAN          "Gigalink Scanner"

#define  VCM_MID_BOOTCMDSERV     0x1003
#define  VCM_TXT_BOOTCMDSERV     "Boot Command Server"

#define  VCM_MID_FIRMWARE1004    0x1010
#define  VCM_TXT_FIRMWARE1004    "DS1004 Firmware"

#define  VCM_MID_PALCODE1004     0x1011
#define  VCM_TXT_PALCODE1004     "DS1004 PAL Code"

#define  VCM_MID_FIRMWARE1401    0x1020
#define  VCM_TXT_FIRMWARE1401    "DS1401 Boot Firmware"

#define  VCM_MID_IOMDSCAN        0x1021
#define  VCM_TXT_IOMDSCAN        "DS1401 I/O Module Scanner"

#define  VCM_MID_FIRMWARE1104    0x1030
#define  VCM_TXT_FIRMWARE1104    "DS1104 Firmware"

#define  VCM_MID_FIRMWARE1006    0x1040
#define  VCM_TXT_FIRMWARE1006    "DS1006 Firmware"

/* ID 4145 - 4155 (0x1031 - 0x103B) is reserved for Memory Emulators */

/* ---- Real-Time Libraries ------------------------------------------------ */

#define  VCM_MID_RTLIB1003       0x1100
#define  VCM_TXT_RTLIB1003       "RTLib1003"

#define  VCM_MID_RTLIB1004       0x1101
#define  VCM_TXT_RTLIB1004       "RTLib1004"

#define  VCM_MID_RTLIB1005       0x1102
#define  VCM_TXT_RTLIB1005       "RTLib1005"

#define  VCM_MID_RTLIB1102       0x1103
#define  VCM_TXT_RTLIB1102       "RTLib1102"

#define  VCM_MID_RTLIB1103       0x1104
#define  VCM_TXT_RTLIB1103       "RTLib1103"

#define  VCM_MID_RTLIB1401       0x1105
#define  VCM_TXT_RTLIB1401       "RTLib1401"

#define  VCM_MID_RTLIB1104       0x1106
#define  VCM_TXT_RTLIB1104       "RTLib1104"

#define  VCM_MID_RTLIB1006       0x1107
#define  VCM_TXT_RTLIB1006       "RTLib1006"

#define  VCM_MID_RTLIB2510       0x1108
#define  VCM_TXT_RTLIB2510       "RTLib2510"


/* ---- Common Real-Time Library Modules ----------------------------------- */

#define  VCM_MID_TIMESTAMP       0x1110
#define  VCM_TXT_TIMESTAMP       "Time Stamping Module"

#define  VCM_MID_MESSAGE         0x1111
#define  VCM_TXT_MESSAGE         "Message Module"

#define  VCM_MID_DSSINT          0x1112
#define  VCM_TXT_DSSINT          "Sub-Interrupt Handling Module"

#define  VCM_MID_HOSTSVC         0x1113
#define  VCM_TXT_HOSTSVC         "Host Services Module"

#define  VCM_MID_PHSINT          0x1114
#define  VCM_TXT_PHSINT          "PHS Bus Interrupt Module"

#define  VCM_MID_DSMCOM          0x1115
#define  VCM_TXT_DSMCOM          "Master-Slv Communication Module"

#define  VCM_MID_DSSTIMUL        0x1116
#define  VCM_TXT_DSSTIMUL        "Stimulus Engine Module"

#define  VCM_MID_RMSERVICE       0x1117
#define  VCM_TXT_RMSERVICE       "Motion Service Module"

#define  VCM_MID_MPMODULES       0x1118
#define  VCM_TXT_MPMODULES       "Multiprocessing Modules"

#define  VCM_MID_ELOG            0x1119
#define  VCM_TXT_ELOG            "Event Logging Module"

#define  VCM_MID_MASTSERV        0x111A
#define  VCM_TXT_MASTSERV        "Master Command Server Module"

#define  VCM_MID_MEM_MNG         0x1120
#define  VCM_TXT_MEM_MNG         "Memory Management Module"

#define  VCM_MID_DSSTD           0x1121
#define  VCM_TXT_DSSTD           "Standard API"

#define  VCM_MID_DSSINT_WB       0x1122
#define  VCM_TXT_DSSINT_WB       "Word Based Sub-Interrupt Module"

#define  VCM_MID_DSFLASH         0x1123
#define  VCM_TXT_DSFLASH         "Flash Administration Module"

#define  VCM_MID_DSNVDATA        0x1124
#define  VCM_TXT_DSNVDATA        "Non-volatile Data Module"

#define  VCM_MID_DSNVDATA_STRUCT 0x1125
#define  VCM_TXT_DSNVDATA_STRUCT "Dataset Flash Structure"

#define  VCM_MID_DSFI2RE         0x1126
#define  VCM_TXT_DSFI2RE         "DsFI2RE Module"

#define  VCM_MID_DSMCR           0x1127
#define  VCM_TXT_DSMCR           "Multi Client Ringbuffer"

#define  VCM_MID_DSRTAE          0x1128
#define  VCM_TXT_DSRTAE          "Real-Time Automation Engine"

#define  VCM_MID_DSSER           0x1129
#define  VCM_TXT_DSSER           "Serial Communication Module"

#define  VCM_MID_DSFR            0x1130
#define  VCM_TXT_DSFR            "FlexRay Communication Module"

#define  VCM_MID_DSLIN           0x1131
#define  VCM_TXT_DSLIN           "LIN Communication Module"

#define  VCM_MID_DSBYPASS        0x1132
#define  VCM_TXT_DSBYPASS        "Service Based Bypassing Module"

#define  VCM_MID_MALLOC_LIB      0x1133
#define  VCM_TXT_MALLOC_LIB      "Malloc Library"

#define  VCM_MID_DSDAQ           0x1134
#define  VCM_TXT_DSDAQ           "Data Acquisition Module"

#define  VCM_MID_DSPHS           0x1135
#define  VCM_TXT_DSPHS           "PHS Bus Access Module"

/* ---- Sub Modules from Real-Time Libraries  -------------------------------- */

#define  VCM_MID_DBG1004         0x1202
#define  VCM_TXT_DBG1004         "DS1004 Debug Handler"

#define  VCM_MID_IPI1005         0x1203
#define  VCM_TXT_IPI1005         "DS1005 IPI Module"

#define  VCM_MID_FLREC1401       0x1204
#define  VCM_TXT_FLREC1401       "Flight Recorder"

#define  VCM_MID_GLSOFTWARE      0x1205
#define  VCM_TXT_GLSOFTWARE      "Gigalink Software"

#define  VCM_MID_MP1005          0x1206
#define  VCM_TXT_MP1005          "DS1005 Multiprocessing Module"

#define  VCM_MID_MP1005_OPT      0x1207
#define  VCM_TXT_MP1005_OPT      "DS1005 Optional MP Members"

#define  VCM_MID_DSCCP           0x1208
#define  VCM_TXT_DSCCP           "CAN Calibration Protocol Module"

#define  VCM_MID_DSGENECU        0x1209
#define  VCM_TXT_DSGENECU        "Generic ECU Interface Module"

#define  VCM_MID_IPI1006         0x120a
#define  VCM_TXT_IPI1006         "DS1006 IPI Module"

#define  VCM_MID_MP1006          0x120b
#define  VCM_TXT_MP1006          "DS1006 Multiprocessing Module"

#define  VCM_MID_MP1006_OPT      0x120c
#define  VCM_TXT_MP1006_OPT      "DS1006 Optional MP Members"

#define  VCM_MID_PPCEXC          0x120d
#define  VCM_TXT_PPCEXC          "PPC Exception Handling Module"

#define  VCM_MID_ETH1006         0x120e
#define  VCM_TXT_ETH1006         "DS1006 Ethernet Module"

#define  VCM_MID_DAQ_RPC         0x120f
#define  VCM_TXT_DAQ_RPC         "RPC Channel"

/* ---- FI2RE  --------------------------------------------------- */

#define  VCM_MID_DSFI2RE_PROT    0x1401
#define  VCM_TXT_DSFI2RE_PROT    "Available Protocol"

#define  VCM_MID_FI2RE           0x1402
#define  VCM_TXT_FI2RE           "FI2RE at %d. "

#define  VCM_MID_FI2RE_SW        0x1403
#define  VCM_TXT_FI2RE_SW        "Software"

#define  VCM_MID_FI2RE_SW_VS     0x1404
#define  VCM_TXT_FI2RE_SW_VS     "MPC-555 Software"

#define  VCM_MID_FI2RE_PROT      0x1405
#define  VCM_TXT_FI2RE_PROT      "MABX Protocol"

#define  VCM_MID_FI2RE_MPV       0x1406
#define  VCM_TXT_FI2RE_MPV       "Custom Variante: %d"  

#define  VCM_MID_FI2RE_HW        0x1407
#define  VCM_TXT_FI2RE_HW        "Hardware"

#define  VCM_MID_FI2RE_MPC_MOD   0x1408
#define  VCM_TXT_FI2RE_MPC_MOD   "MPC-555 Module"

#define  VCM_MID_FI2RE_MPC_PN    0x1409
#define  VCM_TXT_FI2RE_MPC_PN    "Part Number: %d"

#define  VCM_MID_FI2RE_MPC_MN    0x1410
#define  VCM_TXT_FI2RE_MPC_MN    "Mask Number: %d"

#define  VCM_MID_FI2RE_MPC_CR    0x1411
#define  VCM_TXT_FI2RE_MPC_CR    "Clock Rate: %d MHz"

#define  VCM_MID_FI2RE_MPC_RS    0x1412
#define  VCM_TXT_FI2RE_MPC_RS    "RAM Size: %d KByte"
 
#define  VCM_MID_FI2RE_FLASH     0x1413
#define  VCM_TXT_FI2RE_FLASH     "Flash Memory"

#define  VCM_MID_FI2RE_FLASH_MC  0x1414
#define  VCM_TXT_FI2RE_FLASH_MC  "Manufacture Code: %d"

#define  VCM_MID_FI2RE_FLASH_TP  0x1415
#define  VCM_TXT_FI2RE_FLASH_TP  "Flash Type: %d"

#define  VCM_MID_FI2RE_FLASH_SZ  0x1416
#define  VCM_TXT_FI2RE_FLASH_SZ  "Flash Size: %d KByte"

#define  VCM_MID_FI2RE_CAMCRANK_VS         0x1417
#define  VCM_TXT_FI2RE_CAMCRANK_VS         "Cam-/CrankShaft Module Board VS"

#define  VCM_MID_FI2RE_CAMCRANK_MACH_VS    0x1418
#define  VCM_TXT_FI2RE_CAMCRANK_MACH_VS    "Cam-/CrankShaft Module MACH VS"
 
#define  VCM_MID_FI2RE_INJ_M1_VS           0x1419
#define  VCM_TXT_FI2RE_INJ_M1_VS           "Injection Module 1 Board VS"

#define  VCM_MID_FI2RE_INJ_M1_MACH_VS      0x1420
#define  VCM_TXT_FI2RE_INJ_M1_MACH_VS      "MACH VS"

#define  VCM_MID_FI2RE_INJ_M2_VS           0x1421
#define  VCM_TXT_FI2RE_INJ_M2_VS           "Injection Module 2 Board VS"

#define  VCM_MID_FI2RE_INJ_M2_MACH_VS      0x1422
#define  VCM_TXT_FI2RE_INJ_M2_MACH_VS      "MACH VS"

#define  VCM_MID_FI2RE_INJ_M3_VS           0x1423
#define  VCM_TXT_FI2RE_INJ_M3_VS           "Injection Module 3 Board VS"

#define  VCM_MID_FI2RE_INJ_M3_MACH_VS      0x1424
#define  VCM_TXT_FI2RE_INJ_M3_MACH_VS      "MACH VS"

#define  VCM_MID_FI2RE_IO_MOD_VS           0x1425
#define  VCM_TXT_FI2RE_IO_MOD_VS           "I/O-Module Board VS"

#define  VCM_MID_FI2RE_IO_MOD_MACH_VS      0x1426
#define  VCM_TXT_FI2RE_IO_MOD_MACH_VS      "MACH VS"

#define  VCM_MID_FI2RE_HW_POD_PAL_VS       0x1427
#define  VCM_TXT_FI2RE_HW_POD_PAL_VS       "POD PAL VS"

/* ---- Rapid Pro ---------------------------------------------------------- */

#define  VCM_MID_RAPIDPRO                  0x1500  
#define  VCM_TXT_RAPIDPRO                  "RapidPro at %d. "

#define  VCM_MID_RP_TOPOLOGY_ID            0x1501
#define  VCM_TXT_RP_TOPOLOGY_ID            "TopologyID: 0x%X"

#define  VCM_MID_RP_RPCU_BOARD             0x1502
#define  VCM_TXT_RP_RPCU_BOARD             "Control Unit"

#define  VCM_MID_RP_RPPU_BOARD             0x1503
#define  VCM_TXT_RP_RPPU_BOARD             "Power Unit"

#define  VCM_MID_RP_RPSCU_BOARD            0x1504
#define  VCM_TXT_RP_RPSCU_BOARD            "SC Unit"

#define  VCM_MID_RP_BOARD_LAYER            0x1505
#define  VCM_TXT_RP_BOARD_LAYER            "Layer: %d"

#define  VCM_MID_RP_BOARD_VS               0x1506
#define  VCM_TXT_RP_BOARD_VS               "Board Version: %d.%d"

#define  VCM_MID_RP_RPCU_BFW565            0x1507
#define  VCM_TXT_RP_RPCU_BFW565            "MPC565 Bfw Version: %d.%d%s%d"

#define  VCM_MID_RP_RPCU_APPL565           0x1508
#define  VCM_TXT_RP_RPCU_APPL565           "MPC565 Appl Version:"

#define  VCM_MID_RP_COM_MOD_TYPE           0x1509
#define  VCM_TXT_RP_COM_MOD_TYPE           "Com Module Type: %d"

#define  VCM_MID_RP_PS_MOD_TYPE            0x150A
#define  VCM_TXT_RP_PS_MOD_TYPE            "Module Type: %d"

#define  VCM_MID_RP_SC_MOD_TYPE            0x150B
#define  VCM_TXT_RP_SC_MOD_TYPE            "Module Type: %d"

#define  VCM_MID_RP_SER_MOD_TYPE           0x150C
#define  VCM_TXT_RP_SER_MOD_TYPE           "Serial Module Type: %d"

#define  VCM_MID_RP_CAN_MOD_TYPE           0x150D
#define  VCM_TXT_RP_CAN_MOD_TYPE           "CAN Module Type: %d"

#define  VCM_MID_RP_MC_MOD_TYPE            0x150E
#define  VCM_TXT_RP_MC_MOD_TYPE            "Processor Module Type: %d"

#define  VCM_MID_RP_MOD_SLOT               0x150F
#define  VCM_TXT_RP_MOD_SLOT               "Slot Number: %d"

#define  VCM_MID_RP_MOD_VS                 0x1510
#define  VCM_TXT_RP_MOD_VS                 "Version: %d.%d"

#define  VCM_MID_RP_MC_MOD_CLOCK           0x1511
#define  VCM_TXT_RP_MC_MOD_CLOCK           "Clock: %.0lf MHz"

#define  VCM_MID_RP_MST_SW_VS              0x1512
#define  VCM_TXT_RP_MST_SW_VS              "RapidPro Module"

#define  VCM_MID_RP_MST_PROT_VS            0x1513
#define  VCM_TXT_RP_MST_PROT_VS            "Master Protocol Version"

#define  VCM_MID_RP_SLV_PROT_VS            0x1514
#define  VCM_TXT_RP_SLV_PROT_VS            "Slave Protocol Version"

#define  VCM_MID_RP_TRX_MOD_TYPE           0x1515
#define  VCM_TXT_RP_TRX_MOD_TYPE           "Transceiver Module Type: %d"

#define  VCM_MID_RP_BOARD_IOPLD_VS         0x1516
#define  VCM_TXT_RP_BOARD_IOPLD_VS         "I/O PLD Version: %d.%d"

#define  VCM_MID_RP_MC_MOD_IOPLD_VS        0x1517
#define  VCM_TXT_RP_MC_MOD_IOPLD_VS        "I/O PLD Version: %d.%d"

#define  VCM_MID_RP_MC_MOD_SYSPLD_VS       0x1518
#define  VCM_TXT_RP_MC_MOD_SYSPLD_VS       "System PLD Version: %d.%d"

#define  VCM_MID_RP_SC_MOD_PLD_VS          0x1519
#define  VCM_TXT_RP_SC_MOD_PLD_VS          "PLD Version: %d.%d"

#define  VCM_MID_RP_PS_MOD_PLD_VS          0x151A
#define  VCM_TXT_RP_PS_MOD_PLD_VS          "PLD Version: %d.%d"

#define  VCM_MID_RP_COM_MOD_PLD_VS         0x151B
#define  VCM_TXT_RP_COM_MOD_PLD_VS         "PLD Version: %d.%d"

#define  VCM_MID_RP_ENG_MST_PROT_VS        0x151C
#define  VCM_TXT_RP_ENG_MST_PROT_VS        "Eng Master Protocol Version"

#define  VCM_MID_RP_ENG_SLV_PROT_VS        0x151D
#define  VCM_TXT_RP_ENG_SLV_PROT_VS        "Eng Slave Protocol Version"

#define  VCM_MID_RP_RPCU_CUST_APPL565      0x151E
#define  VCM_TXT_RP_RPCU_CUST_APPL565      "MPC565 Custom Appl Version:"

#define  VCM_MID_RP_RPCU_USER_APPL565      0x151F
#define  VCM_TXT_RP_RPCU_USER_APPL565      "MPC565 User Appl Version:"

#define  VCM_MID_RP_RPCU_BASE_APPL565      0x1520
#define  VCM_TXT_RP_RPCU_BASE_APPL565      "based on MPC565 Appl Version:"

#define  VCM_MID_RP_CUST_SUPPORT1_VS       0x1521
#define  VCM_TXT_RP_CUST_SUPPORT1_VS       "PE016%d Customer Support"

#define  VCM_MID_RP_CUST_SUPPORT2_VS       0x1522
#define  VCM_TXT_RP_CUST_SUPPORT2_VS       "PE0160%d Customer Support"

#define  VCM_MID_RP_RPCU_BASE_SLV_PRO_VS   0x1523
#define  VCM_TXT_RP_RPCU_BASE_SLV_PRO_VS   "based on Slave Protocol Version:"

#define  VCM_MID_RP_RPCU_BASE_MST_PRO_VS   0x1524
#define  VCM_TXT_RP_RPCU_BASE_MST_PRO_VS   "based on Master Protocol Version:"

#define  VCM_MID_RP_MST_CUSTOM_SW_VS       0x1525
#define  VCM_TXT_RP_MST_CUSTOM_SW_VS       "RapidPro Custom Module"

/* ---- RealtimeTesting ------<0x1600 to 0x1640> ----------------------------- */

#define  VCM_MID_DSRTTESTING              0x1600
#define  VCM_TXT_DSRTTESTING              "Real-Time Testing Module"

#define  VCM_MID_DSRTSEQUENCE             0x1601
#define  VCM_TXT_DSRTSEQUENCE 	          "RTSequence XXX"

#define  VCM_MID_RTPYTHON                 0x1602
#define  VCM_TXT_RTPYTHON                 "RTPythoninterpreter"


/*
 * this block of IDs (from FIRST_HMQ to LAST_HMQ) is reserved for
 * various types of host message queues
 */
#define  VCM_MID_DSRT_FIRST_HMQ           0x1610
#define  VCM_MID_DSRT_LAST_HMQ            0x1640

#define  VCM_MID_DSRT_HMQ_GLOBAL_CNTL     VCM_MID_DSRT_FIRST_HMQ
#define  VCM_TXT_DSRT_HMQ_GLOBAL_CNTL     "Global Control"

#define  VCM_MID_DSRT_HMQ_STDOUT          0x1611
#define  VCM_TXT_DSRT_HMQ_STDOUT          "Standard Out"

#define  VCM_MID_DSRT_HMQ_BACKANNOTATION  0x1612
#define  VCM_TXT_DSRT_HMQ_BACKANNOTATION  "Backannotation"

#define  VCM_MID_DSRT_HMQ_SCRIPT_CONTROL  0x1613
#define  VCM_TXT_DSRT_HMQ_SCRIPT_CONTROL  "Script Control"

#define  VCM_MID_DSRT_HMQ_HOSTCALLS       0x1614
#define  VCM_TXT_DSRT_HMQ_HOSTCALLS       "HostCalls"

/* 0x1641 reserved */
#define  VCM_MID_HMQ_SCK_FIFO             0x1641
#define  VCM_TXT_HMQ_SCK_FIFO             "SCK Host Message Queue"


/* --- RTIBypass --<0x1700 to 0x17FF>--------------------------------------- */

/*#########################################################################
 * The range of VCM MID's from <<0x1700 to 0x17FF>> is reserved for the 
 * RTIBypass RTLib modules. This MID range is handled and defined by the 
 * plugin based RTIBypass RTLib modules itself and will not get defined
 * within the dsmodule.h! Do not define VCM MID's within this range into
 * this module to avoid double ID definitions!
 *########################################################################*/

/* ---- Real-Time Kernel --------------------------------------------------- */

#define  VCM_MID_RTKERNEL        0x2000
#define  VCM_TXT_RTKERNEL        "Real-Time Kernel"

#define  VCM_MID_RTK1003         0x2001
#define  VCM_TXT_RTK1003         "RTK1003"

#define  VCM_MID_RTK1004         0x2002
#define  VCM_TXT_RTK1004         "RTK1004"

#define  VCM_MID_RTK1005         0x2003
#define  VCM_TXT_RTK1005         "RTK1005"

#define  VCM_MID_RTK1102         0x2004
#define  VCM_TXT_RTK1102         "RTK1102"

#define  VCM_MID_RTK1103         0x2005
#define  VCM_TXT_RTK1103         "RTK1103"

#define  VCM_MID_RTK1401         0x2006
#define  VCM_TXT_RTK1401         "RTK1401"

#define  VCM_MID_RTK1104         0x2007
#define  VCM_TXT_RTK1104         "RTK1104"

#define  VCM_MID_RTK1006         0x2008
#define  VCM_TXT_RTK1006         "RTK1006"


/* ---- I/O Board Software ------------------------------------------------- */

#define  VCM_MID_IOBOARDS        0x3000
#define  VCM_TXT_IOBOARDS        "I/O Board Modules"

#define  VCM_MID_DS2001          0x3001
#define  VCM_TXT_DS2001          "DS2001 High-Speed A/D"

#define  VCM_MID_DS2002          0x3002
#define  VCM_TXT_DS2002          "DS2002 Multi-Channel A/D"

#define  VCM_MID_DS2003          0x3003
#define  VCM_TXT_DS2003          "DS2003 Multi-Channel A/D"

#define  VCM_MID_DS2101          0x3004
#define  VCM_TXT_DS2101          "DS2101 5-Channel D/A"

#define  VCM_MID_DS2102          0x3005
#define  VCM_TXT_DS2102          "DS2102 High-Resolution D/A"

#define  VCM_MID_DS2103          0x3006
#define  VCM_TXT_DS2103          "DS2103 Multi-Channel D/A"

#define  VCM_MID_DS2201          0x3007
#define  VCM_TXT_DS2201          "DS2201 Multi I/O Board"

#define  VCM_MID_DS2210          0x3008
#define  VCM_TXT_DS2210          "DS2210 Engine HIL Board"

#define  VCM_MID_DS2301          0x3009
#define  VCM_TXT_DS2301          "DS2301 DDS Board"

#define  VCM_MID_DS2302          0x300A
#define  VCM_TXT_DS2302          "DS2302 DDS Board"

#define  VCM_MID_DS2401          0x300B
#define  VCM_TXT_DS2401          "DS2401 Resistor Board"

#define  VCM_MID_DS3001          0x300C
#define  VCM_TXT_DS3001          "DS3001 Incremental Encoder"

#define  VCM_MID_DS3002          0x300D
#define  VCM_TXT_DS3002          "DS3002 Incremental Encoder"

#define  VCM_MID_DS4001          0x300E
#define  VCM_TXT_DS4001          "DS4001 Timing/ Digital I/O"

#define  VCM_MID_DS4002          0x300F
#define  VCM_TXT_DS4002          "DS4002 Timing/ Digital I/O"

#define  VCM_MID_DS4003          0x3010
#define  VCM_TXT_DS4003          "DS4003 Digital I/O"

#define  VCM_MID_DS4101          0x3011
#define  VCM_TXT_DS4101          "DS4101 Parallel Processing"

#define  VCM_MID_DS4110          0x3012
#define  VCM_TXT_DS4110          "DS4110 DRAM Board"

#define  VCM_MID_DS4120          0x3013
#define  VCM_TXT_DS4120          "DS4120 ECU Interface"

#define  VCM_MID_DS4121          0x3014
#define  VCM_TXT_DS4121          "DS4121 Serial ECU Interface"

#define  VCM_MID_DS4201          0x3015
#define  VCM_TXT_DS4201          "DS4201 Prototyping Board"

#define  VCM_MID_DS4201S         0x3016
#define  VCM_TXT_DS4201S         "DS4201S Serial Interface Board"

#define  VCM_MID_DS4301          0x3017
#define  VCM_TXT_DS4301          "DS4301 CAN Interface"

#define  VCM_MID_DS4302          0x3018
#define  VCM_TXT_DS4302          "DS4302 CAN Interface"

#define  VCM_MID_DS4401          0x3019
#define  VCM_TXT_DS4401          "DS4401 MIL-STD-1553 Interface"

#define  VCM_MID_DS4501          0x301A
#define  VCM_TXT_DS4501          "DS4501 IP-Carrier"

#define  VCM_MID_DS5001          0x301B
#define  VCM_TXT_DS5001          "DS5001 Digital Waveform Capture"

#define  VCM_MID_DS5101          0x301C
#define  VCM_TXT_DS5101          "DS5101 Digital Waveform Output"

#define  VCM_MID_DS5201          0x301D
#define  VCM_TXT_DS5201          "DS5201 Customer Board"

#define  VCM_MID_DS4330          0x301E
#define  VCM_TXT_DS4330          "DS4330 LIN Interface"

#define  VCM_MID_DS4503          0x301F
#define  VCM_TXT_DS4503          "SCRAMNet Interface Board"

#define  VCM_MID_DS4502          0x3020
#define  VCM_TXT_DS4502          "Fieldbus Interface Board"

#define  VCM_MID_DS2211          0x3021
#define  VCM_TXT_DS2211          "DS2211 HIL I/O Board"

#define  VCM_MID_DS2202          0x3022
#define  VCM_TXT_DS2202          "DS2202 HIL I/O Board"

#define  VCM_MID_DS2004          0x3023
#define  VCM_TXT_DS2004          "DS2004 High-Speed A/D Board"

#define  VCM_MID_DS5202          0x3024
#define  VCM_TXT_DS5202          "DS5202 FPGA BASE BOARD"

#define  VCM_MID_DS4505          0x3025
#define  VCM_TXT_DS4505          "DS4505 FlexRay Interface Board"

#define  VCM_MID_DS5203          0x3026
#define  VCM_TXT_DS5203          "DS5203 FPGA BOARD"

#define  VCM_MID_DS4004          0x3027
#define  VCM_TXT_DS4004          "DS4004 HIL Digital I/O Board"

#define  VCM_MID_DS802           0x3028
#define  VCM_TXT_DS802           "DS802 PHS Link Board"

/* entries up to 0x30FF are reserved for future I/O board software IDs */

/* ----------------    MABX I/O module     --------------------------------- */

#define  VCM_MID_IO_MD_FIRMWARE  0x03100
#define  VCM_TXT_IO_MD_FIRMWARE  "Firmware"

#define  VCM_MID_IO_MD_USR_FW    0x03101
#define  VCM_TXT_IO_MD_USR_FW    "User Firmware"

#define  VCM_MID_IO_MD_DS_FW     0x03102
#define  VCM_TXT_IO_MD_DS_FW     "based on dSPACE Firmware"

#define  VCM_MID_CUSTOMER_FW     0x03103
#define  VCM_TXT_CUSTOMER_FW     "Customer Support PN¿    FW"

#define  VCM_MID_CAN_TP1_FW      0x03104
#define  VCM_TXT_CAN_TP1_FW      "Firmware"

#define  VCM_MID_CAN_TP1U_FW     0x03104
#define  VCM_TXT_CAN_TP1U_FW     "User Firmware"

#define  VCM_MID_CAN_TP1C_FW     0x03104
#define  VCM_TXT_CAN_TP1C_FW     "Customer Support PN¿    FW"

#define  VCM_MID_CAN1401_TP1     0x03105
#define  VCM_TXT_CAN1401_TP1     "DS1401 CAN_Tp1 Module"

#define  VCM_MID_DIO_TP1_FW      0x03106
#define  VCM_TXT_DIO_TP1_FW      "DIO TP1 Firmware"

#define  VCM_MID_DIO_TP1U_FW     0x03106
#define  VCM_TXT_DIO_TP1U_FW     "DIO TP1 User Firmware"

#define  VCM_MID_DIO_TP1C_FW     0x03106
#define  VCM_TXT_DIO_TP1C_FW     "DIO TP1 Firmware"

#define  VCM_MID_DIO1401_TP1     0x03107
#define  VCM_TXT_DIO1401_TP1     "DS1401 Dio_Tp1 Module"

#define  VCM_MID_DIO_TP2_FW      0x03108
#define  VCM_TXT_DIO_TP2_FW      "DIO TP2 Firmware"

#define  VCM_MID_DIO_TP2U_FW     0x03108
#define  VCM_TXT_DIO_TP2U_FW     "DIO TP2 User Firmware"

#define  VCM_MID_DIO_TP2C_FW     0x03108
#define  VCM_TXT_DIO_TP2C_FW     "DIO TP2 Firmware"

#define  VCM_MID_DIO1401_TP2     0x03109
#define  VCM_TXT_DIO1401_TP2     "DS1401 Dio_Tp2 Module"

#define  VCM_MID_UNKNOWN_TPX     0x03110
#define  VCM_TXT_UNKNOWN_TPX     "Module Ï   Type ¢ Module æ "

#define  VCM_MID_CAN_TPX         0x03111
#define  VCM_TXT_CAN_TPX         "CAN Type ¢ Module æ "

#define  VCM_MID_ECU_TPX         0x03112
#define  VCM_TXT_ECU_TPX         "ECU Type ¢ Module æ "

#define  VCM_MID_DIO_TPX         0x03113
#define  VCM_TXT_DIO_TPX         "DIO Type ¢ Module æ "

#define  VCM_MID_ADC_TPX         0x03114
#define  VCM_TXT_ADC_TPX         "ADC Type ¢ Module æ "

#define  VCM_MID_DAC_TPX         0x03115
#define  VCM_TXT_DAC_TPX         "DAC Type ¢ Module æ "

#define  VCM_MID_IP_TPX          0x03116
#define  VCM_TXT_IP_TPX          "IP Type ¢ Module æ "



#define  VCM_MID_ECU_TP1_RP_VS   0x03117
#define  VCM_TXT_ECU_TP1_RP_VS   "RapidPro Control Unit"

#define  VCM_MID_ECU_TP1_RP_BVS  0x03118
#define  VCM_TXT_ECU_TP1_RP_BVS  "Boot Firmware"

#define  VCM_MID_ECU_TP1_RP_AVS  0x03119
#define  VCM_TXT_ECU_TP1_RP_AVS  "Firmware"

#define  VCM_MID_DS1606          0x0311A
#define  VCM_TXT_DS1606          "DS1606 Serial ECU Interface"


/* ----------------    sub modules from i/o boards          ----------------- */

#define  VCM_MID_CAN2210          0x3300
#define  VCM_TXT_CAN2210          "DS2210 CAN module"

#define  VCM_MID_CAN2210_FW       0x3301
#define  VCM_TXT_CAN2210_FW       "DS2210 CAN slave firmware"

#define  VCM_MID_USER_CAN2210_FW  0x3301
#define  VCM_TXT_USER_CAN2210_FW  "DS2210 CAN user firmware"

#define  VCM_MID_CUST_CAN2210_FW  0x3301
#define  VCM_TXT_CUST_CAN2210_FW  "DS2210 CAN custom firmware"

#define  VCM_MID_CAN4302_FW       0x3302
#define  VCM_TXT_CAN4302_FW       "DS4302 CAN slave firmware"

#define  VCM_MID_DS4401_FW        0x3303
#define  VCM_TXT_DS4401_FW        "DS4401 slave firmware"

#define  VCM_MID_LIN4330_FW       0x3304
#define  VCM_TXT_LIN4330_FW       "DS4330 LIN slave firmware"

#define  VCM_MID_CAN2211          0x3305
#define  VCM_TXT_CAN2211          "DS2211 CAN module"

#define  VCM_MID_CAN2211_FW       0x3306
#define  VCM_TXT_CAN2211_FW       "DS2211 CAN slave firmware"

#define  VCM_MID_USER_CAN2211_FW  0x3307
#define  VCM_TXT_USER_CAN2211_FW  "DS2211 CAN user firmware"

#define  VCM_MID_CUST_CAN2211_FW  0x3308
#define  VCM_TXT_CUST_CAN2211_FW  "DS2211 CAN custom firmware"

#define  VCM_MID_CAN2202          0x3309
#define  VCM_TXT_CAN2202          "DS2202 CAN module"

#define  VCM_MID_CAN2202_FW       0x330A
#define  VCM_TXT_CAN2202_FW       "DS2202 CAN slave firmware"

#define  VCM_MID_USER_CAN2202_FW  0x330B
#define  VCM_TXT_USER_CAN2202_FW  "DS2202 CAN user firmware"

#define  VCM_MID_CUST_CAN2202_FW  0x330C
#define  VCM_TXT_CUST_CAN2202_FW  "DS2202 CAN custom firmware"

#define  VCM_MID_BASED_ON_FW      0x33FF
#define  VCM_TXT_BASED_ON_FW      "based on dSPACE Firmware"


/* ----------------    Controller Boards     -- ----------------------------- */

#define  VCM_MID_IOMODULES        0x35FF
#define  VCM_TXT_IOMODULES       "I/O Modules"


/* ----------------    DS1103 I/O module     -------------------------------- */

#define  VCM_MID_CAN1103              0x3500
#define  VCM_TXT_CAN1103              "DS1103 CAN module"

#define  VCM_MID_SLVDSP1103           0x3501
#define  VCM_TXT_SLVDSP1103           "DS1103 slave DSP"

#define  VCM_MID_CAN1103_FW           0x3502
#define  VCM_TXT_CAN1103_FW           "DS1103 CAN slave firmware"

#define  VCM_MID_USER_CAN1103_FW      0x3502
#define  VCM_TXT_USER_CAN1103_FW      "DS1103 CAN user firmware"

#define  VCM_MID_CUST_CAN1103_FW      0x3502
#define  VCM_TXT_CUST_CAN1103_FW      "DS1103 CAN custom firmware"

#define  VCM_MID_SLVDSP1103_FW        0x3505
#define  VCM_TXT_SLVDSP1103_FW        "DS1103 slave DSP firmware"

#define  VCM_MID_USER_SLVDSP1103_FW   0x3505
#define  VCM_TXT_USER_SLVDSP1103_FW   "DS1103 slave DSP user firmware"

#define  VCM_MID_CUST_SLVDSP1103_FW   0x3505
#define  VCM_TXT_CUST_SLVDSP1103_FW   "DS1103 slave DSP custom firmware"


/* ----------------    DS1104 I/O module     ------------------------------- */

#define  VCM_MID_MASTERIO1104         0x3520
#define  VCM_TXT_MASTERIO1104         "DS1104 master I/O"

#define  VCM_MID_SLVDSP1104           0x3521
#define  VCM_TXT_SLVDSP1104           "DS1104 slave DSP"

#define  VCM_MID_SLVDSP1104_FW        0x3522
#define  VCM_TXT_SLVDSP1104_FW        "DS1104 slave DSP firmware"

#define  VCM_MID_USER_SLVDSP1104_FW   0x3522
#define  VCM_TXT_USER_SLVDSP1104_FW   "DS1104 slave DSP user firmware"

#define  VCM_MID_CUST_SLVDSP1104_FW   0x3522
#define  VCM_TXT_CUST_SLVDSP1104_FW   "DS1104 slave DSP custom firmware"


/* ---- I/O Board Hardware ------------------------------------------------- */

#define  VCM_MID_DSXXXX_HW       0x4000
#define  VCM_TXT_DSXXXX_HW       "DSxxxx"

#define  VCM_MID_DS2001_HW       0x4001
#define  VCM_TXT_DS2001_HW       "DS2001"

#define  VCM_MID_DS2002_HW       0x4002
#define  VCM_TXT_DS2002_HW       "DS2002"

#define  VCM_MID_DS2003_HW       0x4003
#define  VCM_TXT_DS2003_HW       "DS2003"

#define  VCM_MID_DS2101_HW       0x4004
#define  VCM_TXT_DS2101_HW       "DS2101"

#define  VCM_MID_DS2102_HW       0x4005
#define  VCM_TXT_DS2102_HW       "DS2102"

#define  VCM_MID_DS2103_HW       0x4006
#define  VCM_TXT_DS2103_HW       "DS2103"

#define  VCM_MID_DS2201_HW       0x4007
#define  VCM_TXT_DS2201_HW       "DS2201"

#define  VCM_MID_DS2210_HW       0x4008
#define  VCM_TXT_DS2210_HW       "DS2210"

#define  VCM_MID_DS2301_HW       0x4009
#define  VCM_TXT_DS2301_HW       "DS2301"

#define  VCM_MID_DS2302_HW       0x400A
#define  VCM_TXT_DS2302_HW       "DS2302"

#define  VCM_MID_DS2401_HW       0x400B
#define  VCM_TXT_DS2401_HW       "DS2401"

#define  VCM_MID_DS3001_HW       0x400C
#define  VCM_TXT_DS3001_HW       "DS3001"

#define  VCM_MID_DS3002_HW       0x400D
#define  VCM_TXT_DS3002_HW       "DS3002"

#define  VCM_MID_DS4001_HW       0x400E
#define  VCM_TXT_DS4001_HW       "DS4001"

#define  VCM_MID_DS4002_HW       0x400F
#define  VCM_TXT_DS4002_HW       "DS4002"

#define  VCM_MID_DS4003_HW       0x4010
#define  VCM_TXT_DS4003_HW       "DS4003"

#define  VCM_MID_DS4101_HW       0x4011
#define  VCM_TXT_DS4101_HW       "DS4101"

#define  VCM_MID_DS4110_HW       0x4012
#define  VCM_TXT_DS4110_HW       "DS4110"

#define  VCM_MID_DS4120_HW       0x4013
#define  VCM_TXT_DS4120_HW       "DS4120"

#define  VCM_MID_DS4121_HW       0x4014
#define  VCM_TXT_DS4121_HW       "DS4121"

#define  VCM_MID_DS4201_HW       0x4015
#define  VCM_TXT_DS4201_HW       "DS4201"

#define  VCM_MID_DS4201S_HW      0x4016
#define  VCM_TXT_DS4201S_HW      "DS4201S"

#define  VCM_MID_DS4301_HW       0x4017
#define  VCM_TXT_DS4301_HW       "DS4301"

#define  VCM_MID_DS4302_HW       0x4018
#define  VCM_TXT_DS4302_HW       "DS4302"

#define  VCM_MID_DS4401_HW       0x4019
#define  VCM_TXT_DS4401_HW       "DS4401"

#define  VCM_MID_DS4501_HW       0x401A
#define  VCM_TXT_DS4501_HW       "DS4501"

#define  VCM_MID_DS5001_HW       0x401B
#define  VCM_TXT_DS5001_HW       "DS5001"

#define  VCM_MID_DS5101_HW       0x401C
#define  VCM_TXT_DS5101_HW       "DS5101"

#define  VCM_MID_DS5201_HW       0x401D
#define  VCM_TXT_DS5201_HW       "DS5201"

#define  VCM_MID_DS4330_HW       0x401E
#define  VCM_TXT_DS4330_HW       "DS4330"

#define  VCM_MID_DS4503_HW       0x401F
#define  VCM_TXT_DS4503_HW       "DS4503"

#define  VCM_MID_DS4502_HW       0x4020
#define  VCM_TXT_DS4502_HW       "DS4502"

#define  VCM_MID_DS2211_HW       0x4021
#define  VCM_TXT_DS2211_HW       "DS2211"

#define  VCM_MID_DS2004_HW       0x4022
#define  VCM_TXT_DS2004_HW       "DS2004"

#define  VCM_MID_DS2202_HW       0x4023
#define  VCM_TXT_DS2202_HW       "DS2202"

#define  VCM_MID_DS5202_HW       0x4024
#define  VCM_TXT_DS5202_HW       "DS5202"

#define  VCM_MID_DS5202_PIGGY_BACK_HW 0x4025
#define  VCM_TXT_DS5202_PIGGY_BACK_HW "Piggy-Back Module"

#define  VCM_MID_DS5202_FPGA_FIRMWARE_VERSION 0x4026
#define  VCM_TXT_DS5202_FPGA_FIRMWARE_VERSION "FPGA Firmware Version"

#define  VCM_MID_DS4504_HW       0x4027
#define  VCM_TXT_DS4504_HW       "DS4504"

#define  VCM_MID_DS4505_HW       0x4028
#define  VCM_TXT_DS4505_HW       "DS4505"

#define VCM_MID_DS4505_COM_MODULE 0x4029
#define VCM_TXT_DS4505_COM_MODULE "COM Module"

#define  VCM_MID_DS5203_HW       0x402A
#define  VCM_TXT_DS5203_HW       "DS5203"

#define  VCM_MID_DS5203_PIGGY_BACK_HW 0x402B
#define  VCM_TXT_DS5203_PIGGY_BACK_HW "Piggy-Back Module"

#define  VCM_MID_DS5203_FPGA_FIRMWARE_VERSION 0x402C
#define  VCM_TXT_DS5203_FPGA_FIRMWARE_VERSION "FPGA Firmware Version"

#define  VCM_MID_DS5203_FPGA_APPLICATION 0x402D
#define  VCM_TXT_DS5203_FPGA_APPLICATION "FPGA Application"

#define  VCM_MID_DS4004_HW       0x402E
#define  VCM_TXT_DS4004_HW       "DS4004"

#define  VCM_MID_DS802_HW        0x402F
#define  VCM_TXT_DS802_HW        "DS802"

/* entries up to 0x40FF are reserved for future I/O board IDs */

/* entries from 0x4A00 up to 0x4CFF are reserved for POD IDs */

/* entries from 0x4D00 up to 0x4EFF are reserved for Memory Emulator modules */

/* ---- Engineering Software ------------------------------------------------- */

#define  VCM_MID_ENG_SW          0x5000
#define  VCM_TXT_ENG_SW          "Engineering Software"

#define  VCM_MID_MABXC_SW        0x5001
#define  VCM_TXT_MABXC_SW        "Customer Support PN¿    FW"

/* entries up to 0x57FF are reserved for engineering software and are defined in
   custom header files */

/* ---- User Software -------------------------------------------------------- */

#define  VCM_MID_USR_SW          0x5800
#define  VCM_TXT_USR_SW          "User Software"

/* entries up to 0x5FFF are reserved for user software and are defined in
   custom header files */


/* ---- RapidPro Software ---------------------------------------------------- */

#define VCM_MID_DSMSC            0x6000
#define VCM_TXT_DSMSC            "Master to Slave Communication"

#define VCM_MID_DSSMC            0x6001
#define VCM_TXT_DSSMC            "Slave to Master Communication"


/* --------------------------       rti modules        ----------------------- */

#define  VCM_MID_RTI1003         0x8000
#define  VCM_TXT_RTI1003         "RTI1003"

#define  VCM_MID_RTI1005         0x8002
#define  VCM_TXT_RTI1005         "RTI1005"

#define  VCM_MID_RTI1401         0x8003
#define  VCM_TXT_RTI1401         "RTI1401"

#define  VCM_MID_RTI1103         0x8004
#define  VCM_TXT_RTI1103         "RTI1103"

#define  VCM_MID_RTIMP           0x8005
#define  VCM_TXT_RTIMP           "RTI-MP"

#define  VCM_MID_FFTACC          0x8006
#define  VCM_TXT_FFTACC          "FFT Accelerator"

#define  VCM_MID_RTI_DS4301CAN   0x8007
#define  VCM_TXT_RTI_DS4301CAN   "DS4301 CAN Blockset"

#define  VCM_MID_RTICAN          0x8008
#define  VCM_TXT_RTICAN          "RTI CAN Blockset"

#define  VCM_MID_RTI1104         0x8009
#define  VCM_TXT_RTI1104         "RTI1104"

#define  VCM_MID_RTI1006         0x800a
#define  VCM_TXT_RTI1006         "RTI1006"

#define  VCM_MID_MATLAB          0x8100
#define  VCM_TXT_MATLAB          "MATLAB"

#define  VCM_MID_SIMULINK        0x8101
#define  VCM_TXT_SIMULINK        "Simulink"

#define  VCM_MID_RTW             0x8102
#define  VCM_TXT_RTW             "Real-Time Workshop"

#define  VCM_MID_STATEFLOW       0x8103
#define  VCM_TXT_STATEFLOW       "Stateflow"

#define  VCM_MID_STATEFLOW_CODER 0x8104
#define  VCM_TXT_STATEFLOW_CODER "Stateflow Coder"

#define  VCM_MID_RTIFLEXRAY      0x8105
#define  VCM_TXT_RTIFLEXRAY      "RTI FlexRay Blockset"

#define  VCM_MID_RTIFLEXRAYCONFIG 0x8106
#define  VCM_TXT_RTIFLEXRAYCONFIG "RTI FlexRay Configuration Blockset"


/* ---- defines for module specific extra information   ------------------- */


#define  VCM_GL_CONNECTION_DETECTED     0x0001

#define  VCM_OLD_FW_VERSION             0x00FF


#define  VCM_MID_IO_SW_BEGIN            0x3000
#define  VCM_MID_IO_SW_END              0x30FF
#define  VCM_MID_IO_MABX_BEGIN          0x3110
#define  VCM_MID_IO_MABX_END            0x32FF
#define  VCM_MID_IO_SUB_BEGIN           0x3300
#define  VCM_MID_IO_SUB_END             0x33FF
#define  VCM_MID_IO_HW_BEGIN            0x4000
#define  VCM_MID_IO_HW_END              0x40FF



/******************************************************************************
  C data structure
******************************************************************************/

#ifndef __ASSEMBLER_CODE__   /* some things are only valid for C programmers */

#include <dstypes.h>


#if ( ( (defined _DS1103) || (defined _DS1104) || (defined _DS1005) || (defined _DS1401) ) && ( !(defined _DSHOST)) )

typedef union
{

struct { UInt32 high; UInt32 low; } version;

struct
 {
  unsigned mar :8 ; /* major release */
  unsigned mir :8 ; /* minor release */
  unsigned mai :8 ; /* maintenance number */
  unsigned spb :8 ; /* special build */
  unsigned spn :16 ; /* special build number */
  unsigned plv :16 ; /* patch level */
 } vs;
} vcm_version_type;

#else

typedef union
{

struct { UInt32 high; UInt32 low; } version;

struct
{
  unsigned spb :8 ; /* special build */
  unsigned mai :8 ; /* maintenance number */
  unsigned mir :8 ; /* minor release */
  unsigned mar :8 ; /* major release */
  unsigned plv :16 ; /* patch level */
  unsigned spn :16 ; /* special build number */
 } vs;
} vcm_version_type;

#endif


typedef struct
{
   UInt32  magic_number;
} dsvcm_entry_t;                            /* dsvcm add mem entry type*/


/* typedefs for RTLib1005 vcm entry */
typedef struct
{
   vcm_version_type req_fw_ver;
} rtlib1005_vcm_entry_t;                     /* RTLib1005 vcm entry type */

/* typedefs for RTLib1006 vcm entry */
typedef struct
{
   vcm_version_type req_fw_ver;
} rtlib1006_vcm_entry_t;                     /* RTLib1006 vcm entry type */


/* typedefs for RTLib1104 vcm entry */
typedef struct
{
   vcm_version_type req_fw_ver;
} rtlib1104_vcm_entry_t;                     /* RTLib1104 vcm entry type */


/* typedefs for phs_scantbl */
typedef struct
{
   Int32   board_type;
   UInt32  board_info_offs;
} phs_scantbl_entry_t;                     /* PHS scantable entry type */

typedef struct
{
  struct
  {
    vcm_version_type version;
    UInt32 origin;
  }c167;
}ds2210_slvfw_version_entry_t;

typedef struct
{
  struct
  {
    vcm_version_type version;
    UInt32 origin;
  }c167;
}ds2202_slvfw_version_entry_t;

typedef struct
{
  struct
  {
    vcm_version_type version;
    UInt32 origin;
  }c167;
}ds2211_slvfw_version_entry_t;


typedef struct
{
   struct
   {
     vcm_version_type version;
     UInt32 origin;
   }c32;
}ds4302_slvfw_version_entry_t;

typedef struct
{
  struct
  {
    vcm_version_type version;
    UInt32 origin;
  }c32;
}ds4401_slvfw_version_entry_t;


/* typedefs for gl_scantbl */


typedef struct
{
   UInt32  target_board_snr;
   UInt32  target_gl_no;
   Int32   target_cpu_id;
   UInt32  reserved;
} gl_scantbl_entry_t;                     /* GL scantable entry type */



/* common data structure for additional memory blocks of firmware modules */
typedef struct
{
    UInt32 fw_origin;             /* 0 = User FW, 1 = dSPACE FW  */
    UInt32 fw_location;           /* PHS Address if I/O board or */
                                  /* module place if MABX module */
    UInt32 fw_burn_count;         /* FW burn count */
    vcm_version_type req_fw_ver;  /* required firmware version   */
} common_fw_vcm_entry_t;

#define COMMON_FW_VCM_ENTRY_T_SIZE  40

#endif /* __ASSEMBLER_CODE__ */

#endif /* __DSMODULE_H__ */
