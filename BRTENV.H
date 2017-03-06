/******************************************************************************
*
* FILE:
*   Brtenv.h
*
* RELATED FILES:
*   Brtenv.c
*
* DESCRIPTION:
*    Basic Realtime Environment for the Ds1103:
*  - includes all necessary header-files,
*  - declaration of global variables,
*  - definitions of macros.
*
* HISTORY: RK initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: BRTENV.H $ $Revision: 1.2 $ $Date: 2003/08/07 14:53:45GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/

#ifndef __BRTENV__
#define __BRTENV__

#include <Dstypes.h>                      /* platform independent data types */
#include <Ds1103.h>                              /* DS1103 board definitions */
#include <Init1103.h>                         /* DS1103 board initialisation */
#include <Cfg1103.h>                        /* config section type defintion */
#include <Int1103.h>                                    /* interrupt support */
#include <Dsmem.h>                                      /* memory management */
#include <Tmr1103.h>                              /* general timer functions */
#include <Io1103.h>                                 /* peripheral I/O access */
#include <Ser1103.h>                                   /* serial port access */
#include <dsser1103.h>                 /* serial port access, board specific */
#include <dsser.h>                     /* serial port access, generic module */
#include <Slvdsp1103.h>                                  /* slave DSP access */
#include <can1103.h>                                     /* slave MC access  */
#include <Dsmsg.h>                                         /* message module */
#include <ds1103_msg.h>
#include <Hostsvc.h>                                        /* host services */
#include <Tic1103.h>                                     /* time measurement */
#include <Info1103.h>                                          /* Info modul */
#include <Exc1103.h>                                   /* Exception Handling */
#include <Pm1103.h>                                   /* Performance Monitor */
#include <def1103.h>                                 /* subinterrupt support */
#include <can1103_msg.h>                            /* CAN message submodule */
#include <dsts.h>                                    /* time stamping module */
#include <dsstd.h>                            /* dSPACE standard definitions */
#include <dsvcm.h>                  /* Version and Config section Management */
#include <PpcStack.h>                                /* stack control module */
#include <dselog.h>                                  /* Event Logging module */
#include <dsmcr.h>                                /* multi client ringbuffer */

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/




/******************************************************************************
  object declarations
******************************************************************************/



/******************************************************************************
  function prototypes
******************************************************************************/


#endif /*__BRTENV__*/

