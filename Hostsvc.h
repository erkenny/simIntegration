/******************************************************************************
*
* MODULE
*   Host services header
*
*
* FILE
*   hostsvc.h
*
* RELATED FILES
*   hostsvc.c
*
* DESCRIPTION
*   This file defines the interface to the host services
*
* AUTHOR(S)
*   R. Kraft, R. Leinfellner, T. Woelfer, V. Lang
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Hostsvc.h $ $Revision: 1.2 $ $Date: 2003/08/07 12:13:35GMT+01:00 $
******************************************************************************/


#ifndef __HOSTSVC_H_
#define __HOSTSVC_H_

#include <hostserv_msg.h>

/* ------------------------------------------------------------------------- */
/*     hardware specific includes                                            */
/* ------------------------------------------------------------------------- */

#if defined(_DS1103)
#include <dsts.h>
#include <hsvc1103.h>

#elif defined(_DS1401)
#include <dsts.h>
#include <hsvc1401.h>

#elif defined(_DS1005)
#include <dsts.h>
#include <hsvc1005.h>

#elif defined(_DS1006)
#include <dsts.h>
#include <hsvc1006.h>

#elif defined(_DS1104)
#include <dsts.h>
#include <hsvc1104.h>

#endif

/* definitions for trigger dispatching (connections array) */
#define HSVC_LINK_NONE  -1
#define HSVC_LINK0       0
#define HSVC_LINK1       1
#define HSVC_LINK2       2
#define HSVC_LINK3       3

/* link description defines (hsvc_links array) */
#define HSVC_LINK_UNUSED 0
#define HSVC_LINK_TREE   1
#define HSVC_LINK_MASTER 2

/* master link defines */
#define HSVC_ML_UNINITIALIZED -1
#define HSVC_ML_MASTER        -2

/* length of the trigger descriptor array */
#define HSVC_TRIGGER_DESCRIPTOR_LENGTH 32

/* ------------------------------------------------------------------------- */
/*     data structures                                                       */
/* ------------------------------------------------------------------------- */

/* ---- instance list entry ------------------------------------------------ */

typedef struct
{
  UInt32 ServiceTablePointer;           /* pointer to service table          */
  UInt32 SuccessorIndex;                /* index of next entry in inst.list  */
  UInt32 OwnersHostAppIndex;            /* host appl. index of owner program */
} t_IndexListEntry;

/* ---- host service structure --------------------------------------------- */

typedef struct
{
  UInt32 ServiceAvailability;          /* bits for available service numbers */
  UInt32 MaxTasks;                     /* length of task table               */
  UInt32 MaxInstances;                 /* length of instance list            */
  UInt32 TaskTable[HSVC_MAX_SVC_TASKS];     /* index of first entry in each list */
  UInt32 InstListBegin[HSVC_MAX_SVC_TASKS]; /* index of first entry in each list */
  UInt32 InstListCount[HSVC_MAX_SVC_TASKS]; /* number of entries in each list    */
} t_HostServiceStruct;

/* ---- typedefs for trigger dispatching ----------------------------------- */

typedef UInt32 hsvc_dispatch_addr_type;
typedef UInt32 hsvc_trigger_type;

typedef struct
{
  hsvc_trigger_type trigger;
  UInt32 status;
  hsvc_trigger_type changes;
} hsvc_trigger_array_type;

typedef struct
{
  UInt32 proc_id;
  UInt32 num_proc;
  UInt32 array_len;
  UInt32 reserved;
  UInt32 array[HSVC_TRIGGER_DESCRIPTOR_LENGTH];
} hsvc_trigger_descriptor_type;

/* ------------------------------------------------------------------------- */
/*     global variables                                                      */
/* ------------------------------------------------------------------------- */

/* number of cpus which are actually part of the tree */
extern UInt32 hsvc_num_existing_cpus;

/* ---- host services ------------------------------------------------------ */

/* host service structure */
extern t_HostServiceStruct HostServiceStruct;

/* ---- trigger dispatching ------------------------------------------------ */

#ifdef HSVC_MP_SERVICES

/* trigger array */
extern hsvc_trigger_array_type *hsvc_trigger_array;

/* trigger descriptor */
extern hsvc_trigger_descriptor_type *hsvc_trigger_descriptor;

/* dispatch array */
extern hsvc_dispatch_addr_type *hsvc_dispatch_array;

/* local trigger pointer */
extern hsvc_trigger_type *hsvc_trigger_ptr;

/* number of used gigalinks */
extern UInt32 hsvc_links_used;

/* link directed to the master processor */
extern Int32 hsvc_master_link;

/* link directed to the master */
extern Int32 hsvc_links[HSVC_NUM_LINKS];

#endif

/* ------------------------------------------------------------------------- */
/*     function prototypes                                                   */
/* ------------------------------------------------------------------------- */

/* master command server */
extern void master_cmd_server();

/* service interpreter */
extern void hsvc_service_interpreter(UInt16 hostsvc_no, ts_timestamp_ptr_type ts);
extern void hsvc_call_svc_fcn(UInt32 **ip, ts_timestamp_ptr_type ts);

/* host services initialization */
extern void hsvc_init_hostsvc(void);

/* initialize the trigger dispatcher */
extern void hsvc_init_trigger_dispatching(UInt32 num_proc,
                                          UInt32 proc_id,
                                          Int32  *connections);

/* trigger dispatching functions */
extern void hsvc_remote_trigger_dispatch(void);

extern void hsvc_local_trigger_dispatch(void);


#endif /*__HOSTSVC_H_*/

