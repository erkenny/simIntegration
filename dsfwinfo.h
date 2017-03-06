/******************************************************************************
*
* FILE: DSFWINFO.H
*
*
* DESCRIPTION:
*  This board independent header file contains definitions and declarations 
*  of state variables for firmware applications which are loaded by the hardware
*  manager (HWM).
*
* HISTORY: 10.01.2000  RK initial version
*    
*
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsfwinfo.h $ $Revision: 1.3 $ $Date: 2003/08/07 11:54:41GMT+01:00 $
*
******************************************************************************/
 

#ifndef __DSFWINFO_H__
#define __DSFWINFO_H__


#include <Dstypes.h>



/* ---- state variables -------------------------------------------- */

#ifndef _DSHOST
/* firmware application state variable */
volatile UInt32 hwm_fw_appl_state = 0; 

/* firmware destination address */
volatile UInt32 hwm_fw_dest_addr = 0;
#endif

/* code changed by sho on 12.01.00 for ControlDesk 1.2.1 */
#ifdef _DS1003
#define HWM_FW_APPL_STATE_NAME          "_hwm_fw_appl_state"
#define HWM_FW_DEST_ADDR_NAME           "_hwm_fw_dest_addr"  
#else
#define HWM_FW_APPL_STATE_NAME          "hwm_fw_appl_state"
#define HWM_FW_DEST_ADDR_NAME           "hwm_fw_dest_addr"
#endif
/* end of code changed by sho on 12.01.00 for ControlDesk 1.2.1 */



/* ---- version variables ------------------------------------------ */
/* code changed by obi on 12.04.00 for ControlDesk 2.0 */
#ifdef _DS1003
#define DSFW_MAR                        "__dsfw_mar"
#define DSFW_MIR                        "__dsfw_mir"
#define DSFW_MAI                        "__dsfw_mai"
#define DSFW_SPB                        "__dsfw_spb"
#define DSFW_SPN                        "__dsfw_spn"
#define DSFW_PLV                        "__dsfw_plv"
#define DSFW_ORG                        "__dsfw_org"
#define DSFW_TAR                        "__dsfw_tar"
#define DSFW_USER_MAR                   "__dsfw_user_mar"
#define DSFW_USER_MIR                   "__dsfw_user_mir"
#define DSFW_USER_SUBMIR                "__dsfw_user_submir"
#else
#define DSFW_MAR                        "_dsfw_mar"
#define DSFW_MIR                        "_dsfw_mir"
#define DSFW_MAI                        "_dsfw_mai"
#define DSFW_SPB                        "_dsfw_spb"
#define DSFW_SPN                        "_dsfw_spn"
#define DSFW_PLV                        "_dsfw_plv"
#define DSFW_ORG                        "_dsfw_org"
#define DSFW_TAR                        "_dsfw_tar"
#define DSFW_USER_MAR                   "_dsfw_user_mar"
#define DSFW_USER_MIR                   "_dsfw_user_mir"
#define DSFW_USER_SUBMIR                "_dsfw_user_submir"
#endif
/* end of code changed by obi on 12.04.00 for ControlDesk 2.0 */



/* ---- state definitions -------------------------------------------- */

/* defines for hwm_fw_appl_state */
#define DS_FW_APPL_STATE_BUSY                 0x0001
#define DS_FW_APPL_STATE_READY                0x0002
#define DS_FW_APPL_STATE_COMMON_ERROR         0x0100


/* ---- other definitions -------------------------------------------- */

/* addr. timeout: 30s */
#define DS_FW_APPL_DEST_ADDR_TIMEOUT              30



#endif /*__DSFWINFO_H__*/

