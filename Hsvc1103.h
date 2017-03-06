/******************************************************************************
*
* MODULE
*   Hardware dependent host services definitions
*
*
* FILE
*   hsvc1103.h
*
* RELATED FILES
*   hostsvc.h
*   hostsvc.c
*
* DESCRIPTION
*   Hardware dependent host services definitions
*
* AUTHOR(S)
*   R. Leinfellner, T. Woelfer
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Hsvc1103.h $ $Revision: 1.3 $ $Date: 2005/07/28 17:00:07GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSHSVC1103.pj $
******************************************************************************/


#ifndef __HSVC1103_H_
#define __HSVC1103_H_

#include <cfg1103.h>

/* ------------------------------------------------------------------------- */
/*     hardware specific definitions                                         */
/* ------------------------------------------------------------------------- */

#define HSVC_MAX_SVC_TASKS          32
#define HSVC_MAX_SVC_INSTANCES      8

/* ------------------------------------------------------------------------- */
/*     host service macro                                                    */
/* ------------------------------------------------------------------------- */

#define host_service(x, y)                                                                                 \
{                                                                                                          \
  HostServiceStruct.ServiceAvailability = HostServiceStruct.ServiceAvailability | (((UInt32)0x1) << (x));  \
  hsvc_service_interpreter((x), (y));                                                                      \
}

/* ------------------------------------------------------------------------- */
/*     external references                                                   */
/* ------------------------------------------------------------------------- */

extern volatile UInt32 * __cfg_CMDQ_START;
extern volatile UInt32 * __cfg_CMDQ_PTR;
extern volatile UInt32 * __cfg_CMDQ_END;
extern volatile UInt32 * __cfg_STP;
extern volatile UInt32 * __cfg_STRP;
extern volatile UInt32 * __cfg_CQP;
extern volatile UInt32 * __cfg_AQP;
extern volatile UInt32 * __cfg_HostService_Struct;


#endif

