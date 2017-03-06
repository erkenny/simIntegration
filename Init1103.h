/******************************************************************************
*
* MODULE
*   This module contains the functions for initializing the DS1103.
*
*
* FILE
*   Init1103.h
*
* RELATED FILES
*   Init1103.c
*
* DESCRIPTION
*   - ds1103_init_register inits the I/O and some other registers.
*   - ds1103_board_init calls several init-functions for I/O and
*     services.
*
* AUTHOR(S)
*   R. Kraft
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Init1103.h $ $Revision: 1.34 $ $Date: 2009/06/22 17:21:37GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/


#ifndef __Init1103__
#define __Init1103__

#include <dsvcm.h>


/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/

/* $DSVERSION: RTLIB1103 - DS1103 Realtime Library */
#define RTLIB1103_VER_MAR 3
#define RTLIB1103_VER_MIR 3
#define RTLIB1103_VER_MAI 0
#define RTLIB1103_VER_SPB VCM_VERSION_RELEASE
#define RTLIB1103_VER_SPN 0
#define RTLIB1103_VER_PLV 0

 extern void ds1103_init_register(void);
 extern void ds1103_init(void);
 extern void ds1103_slv_boot_finished(void);
 extern int  sint1103_init(void);

 /* global variable for fast board differentiation between Boards Rev > 1.7*/
 extern volatile UInt16 ds1103_advanced_io;

 /* global variables for subinterrupt handling */
 #include <dssint.h>
 extern dssint_receiver_type *dssint_slvmc_receiver;
 extern dssint_sender_type   *dssint_slvmc_sender;
 extern dssint_receiver_type *dssint_slvdsp_fw_receiver;
 extern dssint_receiver_type *dssint_slvdsp_usr_receiver;
 extern dssint_receiver_type *dssint_slvmc_receiver;
 extern dssint_sender_type   *dssint_slvdsp_sender;
 extern dssint_sender_type   *dssint_slvmc_sender;


#endif /*__Init1103__*/


