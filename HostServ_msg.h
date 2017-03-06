/******************************************************************************
*
* MODULE
*   HostServices
* FILE
*   HostServ_msg.h
*
* RELATED FILES
*   Hostsvc.c
*   Mastserv.c
*   Dsserver.c
*   Slvserv.c
*
* DESCRIPTION
*   This file contains the errorcodes and messages for the host services.
*
* REMARKS
*
* AUTHOR(S)
*   R. Kraft
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: HostServ_msg.h $ $Revision: 1.2 $ $Date: 2003/08/07 12:13:33GMT+01:00 $
******************************************************************************/

#ifndef __HostServ_msg_h__
#define __HostServ_msg_h__


#include <Dsmsg.h>

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

#define HOSTSERV_NO_ERROR                     0

#define HOSTSERV_ILLEGAL_CPU_NO               1000
#define HOSTSERV_ILLEGAL_CPU_NO_MSG           "Illegal CPU number!"

#define HOSTSERV_ALLOC_ERROR                  1001
#define HOSTSERV_ALLOC_ERROR_MSG              "Memory allocation error!"

#define HOSTSERV_OVERFLOW_ERROR               1002
#define HOSTSERV_OVERFLOW_ERROR_MSG           "Memory overflow!"

#define HOSTSERV_INVALID_NUMPROC_ERROR        1003
#define HOSTSERV_INVALID_NUMPROC_ERROR_MSG    "Processor ID %d is invalid in a %d processor system!"

#define HOSTSERV_INIT_TDISP_ERROR             1004
#define HOSTSERV_INIT_TDISP_ERROR_MSG         "hsvc_init_trigger_dispatching(): The connections array has to be symmentrical!"

#define HOSTSERV_MP_TREE_BUILD_ERROR            1005
#define HOSTSERV_MP_TREE_BUILD_ERROR_TXT        "hsvc_init_trigger_dispatching(): The specified MP topology is not connected!"

#define HOSTSERV_MP_CPU_NOT_CONNECTED_INFO      1006
#define HOSTSERV_MP_CPU_NOT_CONNECTED_INFO_TXT  "hsvc_init_trigger_dispatching(): CPU %i ist not connected to the MP system!"


#endif /* __HostServ_msg_h__ */





