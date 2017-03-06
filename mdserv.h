/***************************************************************************
*
*  RMSERV
*
*  FILE
*    mdserv.h
*
*  RELATED FILES
*    rmserv.c, rmserv.h, rmproto.h
*
*  DESCRIPTION
*    MotionDesk layer for the MotionService module
*
*  REMARKS
*
*  AUTHOR(S)
*    T. Woelfer
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: mdserv.h $ $Revision: 1.4 $ $Date: 2004/05/26 13:43:30GMT+01:00 $
***************************************************************************/
#ifndef __MDSERV_H__
#define __MDSERV_H__

#include <rmproto.h>

/***************************************************************************
  constant, macro and type definitions
***************************************************************************/

#define MD_NO_ERROR RM_NO_ERROR
#define MD_OUT_OF_RANGE RM_OUT_OF_RANGE
#define MD_OUT_OF_MEMORY RM_OUT_OF_MEMORY
#define MD_NOT_INITIALIZED RM_NOT_INITIALIZED

#define MD_SIM_STOP  RM_DSP_STOP
#define MD_SIM_PAUSE RM_DSP_PAUSE
#define MD_SIM_RUN   RM_DSP_RUN

#define MD_MANEUVER_UNSUPPORTED RM_MANEUVER_UNSUPPORTED
#define MD_MANEUVER_SUPPORTED   RM_MANEUVER_SUPPORTED

#define md_error_type RM_ERROR
#define md_state_type RM_STATE
#define md_homog_member_data_type RM_HOMOG_MEMBER_DATA
#define md_p_homog_member_data_type RM_HOMOG_MEMBER_DATA_PTR
#define md_maneuver_support_type RM_MANEUVER_SUPPORT

/***************************************************************************
  function prototypes
***************************************************************************/

#define md_init_interface(number_of_members) rm_init_interface((number_of_members), 0, 0, 0)
#define md_set_member_name(member_id, member_name) rm_set_member_name((member_id), (member_name))
#define md_set_member_position(member_id, homog_data_ptr) rm_set_homog_member_pos((member_id), (homog_data_ptr))
#define md_set_sim_state(sim_state) rm_set_sim_state(sim_state)
#define md_service(tsim) rm_foreground_service(tsim)
#define md_close_interface() rm_close_interface()
#define md_set_maneuver_support(maneuver_support) rm_set_maneuver_support(maneuver_support)
#define md_set_maneuver_start_time(maneuver_start_time) rm_set_maneuver_start_time(maneuver_start_time)



#endif /* __MDSERV_H__ */
