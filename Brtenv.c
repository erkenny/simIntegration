/******************************************************************************
*
* FILE:
*   BRTENV.c
*
* RELATED FILES:
*   BRTENV.H
*
* DESCRIPTION:
*    Basic Realtime Environment for the Ds1103:
*  - includes all necessary header-files,
*  - declaration of global variables,
*  - definitions of macros.
*   
*
* HISTORY: RK initial version
*    
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Brtenv.c $ $Revision: 1.2 $ $Date: 2003/08/07 14:53:47GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/
#include <Dstypes.h>
#include <Cfg1103.h>

 volatile UInt16 ds1103_debug = 0;

 UInt32 ds1103_isr_status_timerA;
 UInt32 ds1103_isr_status_timerB;
 UInt32 ds1103_isr_status_decrementer;

 volatile cfg_section_type* ds1103_cfg_section = (volatile cfg_section_type*)DS1103_GLOBAL_RAM_START;
