/******************************************************************************
*
* FILE:
*   exc1103.h
*
* RELATED FILES:
*   ppc_exc.h
*
* DESCRIPTION:
*   board dependent exception defineds and macros
*
* HISTORY:
*   23.06.98  Rev 1.0  H.-G. Limberg         initial version for DS1103
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: Exc1103.H $ $Revision: 1.2 $ $Date: 2003/08/07 12:02:09GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/PPCEXC1103.pj $
*
******************************************************************************/
#ifndef __EXC1103_H__
#define __EXC1103_H__

#include <PpcExc.h>


#define DS1103_NUMBER_OF_EXCEPTIONS 12

/* ExcID numbers*/

#define DS1103_EXC_DZE    PPC_EXC_DZE
#define DS1103_EXC_FOV    PPC_EXC_FOV
#define DS1103_EXC_UNF    PPC_EXC_UNF
#define DS1103_EXC_INE    PPC_EXC_INE

#define DS1103_EXC_SNAN   PPC_EXC_SNAN
#define DS1103_EXC_ISI    PPC_EXC_ISI
#define DS1103_EXC_IDI    PPC_EXC_IDI
#define DS1103_EXC_ZDZ    PPC_EXC_ZDZ
#define DS1103_EXC_IMZ    PPC_EXC_IMZ
#define DS1103_EXC_VC     PPC_EXC_VC
#define DS1103_EXC_SQRT   PPC_EXC_SQRT
#define DS1103_EXC_ICON   PPC_EXC_ICON

/* exception modes */

#define DS1103_EM_STOP      PPC_EM_STOP
#define DS1103_EM_CHKERR    PPC_EM_CHKERR
#define DS1103_EM_DUMP      PPC_EM_DUMP
#define DS1103_EM_LAST_EXC  PPC_EM_LAST_EXC

#define DS1103_Exc_Handler_Type   Ppc_Exc_Handler_Type


/* function macros */

#define ds1103_exception_enable(x)             ppc_exception_enable(x)
#define ds1103_exception_disable(x)            ppc_exception_disable(x)
#define ds1103_global_exception_enable         ppc_global_exception_enable
#define ds1103_global_exception_disable        ppc_global_exception_disable
#define ds1103_exception_handler_set(x,y,z)    ppc_exception_handler_set(x,y,z)
#define ds1103_all_exception_handlers_set(x,y) ppc_all_exception_handlers_set(x,y)
#define ds1103_exception_mode_set(x,y)         ppc_exception_mode_set(x,y)
#define ds1103_exception_mode_get(x)           ppc_exception_mode_get(x)
#define ds1103_exception_counter_get(x)        ppc_exception_counter_get(x)
#define ds1103_exception_counter_reset(x)      ppc_exception_counter_reset(x)
#define ds1103_total_exception_count_get       ppc_total_exception_count_get
#define ds1103_exception_flag_get              ppc_exception_flag_get
#define ds1103_exception_flag_reset            ppc_exception_flag_reset


#endif
