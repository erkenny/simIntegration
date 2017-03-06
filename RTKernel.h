/******************************************************************************
  FILE:
    RTKernel.h

  DESCRIPTION:
    Platform independent definitions, variables and prototypes

  REMARKS:

  AUTHOR(S): R. Leinfellner

  Copyright (c) 1997 dSPACE GmbH, GERMANY

  $RCSfile: RTKernel.h $ $Revision: 1.14 $ $Date: 2006/07/31 10:20:05GMT+01:00 $
 *****************************************************************************/

#ifndef __RTKERNEL_H__
#define __RTKERNEL_H__

/* ---- error codes -------------------------------------------------------- */

#define RTK_NO_ERROR                 0
#define RTK_BIND_SERVICE_ERROR       1
#define RTK_BIND_MAPPING_TABLE_ERROR 2
#define RTK_BIND_HARDWARE_INIT_ERROR 3
#define RTK_INVALID_MT_ENTRY         4
#define RTK_FLAG_INIT_ERROR          5
#define RTK_SYNC_INIT_ERROR          6
#define RTK_SBIND_KERNEL_ERROR       7
#define RTK_SBIND_NEWSERV_ERROR      8

/* ---- other defines ------------------------------------------------------ */

/* priority definitions */
#define RTK_BACKGROUND_PRIORITY   128

/* mapping table entry mode/status bit positions */
#define RTK_STATUS_USED           1       /* status: entry is bound          */
#define RTK_STATUS_NORMAL_ENABLED 2       /* mode: enabled in normal mode    */
#define RTK_STATUS_IDLE_ENABLED   4       /* mode: enabled in idle mode      */
#define RTK_STATUS_ENABLED        8       /* status: subint currently enabled*/
#define RTK_STATUS_HW_TRIGGERED  16       /* mode: hardware triggered entry  */

#define RTK_STATUS_NEW            0       /* initial mode/status             */

/* Subinterrupt modes */
#define RTK_SM_REAL_TIME          1       /* enable subint in normal mode    */
#define RTK_SM_IDLE               2       /* enable subint in idle mode      */

#define RTK_NO_SINT              -1       /* no slave sub-interrupt          */

#define RTK_MAX_TASK_NAME_LENGTH  32      /* maximal task name string length */

/* Infovariables Identifier */
#define RTK_TASK_INFO_TURNAROUND_TIME   1
#define RTK_TASK_INFO_TASK_CALLS        2

/* ---- definitions -------------------------------------------------------- */

/* schedule a task per software */
#define rtk_schedule_task(service, subentry_no)                               \
{                                                                             \
  rtlib_int_status_t int_status;                                              \
                                                                              \
  RTK_INT_SAVE_AND_DISABLE(int_status);                                     \
  rtk_scheduler(rtk_task_mapping_table[rtk_handler_descriptor_table[          \
                                service].first_subentry + subentry_no].task); \
  RTK_INT_RESTORE(int_status);                                              \
} 

/* ---- compatibility ------------------------------------------------------ */

#define rtk_current_priority rtk_current_task->priority

/* ---- hardware specific includes ----------------------------------------- */

#if defined(_DS1003)
# include <rtk1003.h>

#elif defined(_DS1004) || defined(__alpha__)
# include <rtk1004.h>

#elif defined(_DS1005)
# include <rtk1005.h>

#elif defined(_DS1102)
# include <rtk1102.h>

#elif defined(_DS1103)
# include <rtk1103.h>

#elif defined(_DS1104)
# include <rtk1104.h>

#elif defined(_DS1201)
# error RTKERNEL.H: RTKernel Vs 1.3 does not support the DS1201!

#elif defined(_DS1401)
# include <rtk1401.h>

#elif defined(_DS1006)
# include <rtk1006.h>

#elif defined(_DS1602)
# include <rtk1602.h>

#elif defined(_DS1603)
# include <rtk1603.h>

#else
# error rtkernel.h: no processor board defined (_DSxxxx)!
#endif

/* ---- declarations of kernel data types ---------------------------------- */

#include <rtkdtype.h>

/* ---- other RTKernel modules --------------------------------------------- */

#ifdef RTK_KM_FLAGS
# include <rtkflags.h>
#endif

#ifdef RTK_KM_IT
# include <rtkit.h>
#endif

#ifdef RTK_KM_DBG
# include <rtkdbg.h>
#endif

#ifdef RTK_KM_DS2004
# include <rtk2004.h>
#endif

#ifdef RTK_KM_DS4302
# include <rtk4302.h>
#endif

#ifdef RTK_KM_DS2202
# include <rtk2202.h>
#endif

#ifdef RTK_KM_DS2210
# include <rtk2210.h>
#endif

#ifdef RTK_KM_DS2211
# include <rtk2211.h>
#endif

#ifdef RTK_KM_DS4002
# include <rtk4002.h>
#endif

#ifdef RTK_KM_DS5001
# include <rtk5001.h>
#endif

#ifdef RTK_KM_DS5101
# include <rtk5101.h>
#endif

#ifdef RTK_KM_DS4120
# include <rtk4120.h>
#endif

#ifdef RTK_KM_DS4121
# include <rtk4121.h>
#endif

#ifdef RTK_KM_DS4201S
# include <rtk4201s.h>
#endif

#ifdef RTK_KM_DS4330
# include <rtk4330.h>
#endif

#ifdef RTK_KM_AB
# include <rtkab.h>
#endif

/* ---- system variables --------------------------------------------------- */

/* current executing priority in the system */
extern volatile rtk_p_task_control_block rtk_current_task;

/* linked list of TCBs */
extern volatile rtk_p_task_control_block rtk_task_list;

/* end of ready queue for each priority */
extern volatile rtk_p_task_control_block * rtk_end_of_ready_queue;

/* array for mapping interrupts to tasks */
extern volatile rtk_p_task_mapping_table_entry rtk_task_mapping_table;

/* array for handler specific data */
extern volatile rtk_p_handler_descriptor rtk_handler_descriptor_table;

/* array for hardware interrupt specific data */
extern volatile rtk_p_interrupt_descriptor rtk_interrupt_descriptor_table;

/* idle Structure */
extern volatile rtk_p_idle_struct rtk_idle_data;

/* number of services */
extern volatile int rtk_num_services;

/* number of services */
extern volatile int rtk_new_service;

/* overrun flag */
extern volatile int rtk_overrun_flag;

/* system timer time per tic */
extern volatile dsfloat rtk_sys_time_per_tic;

/* pointer to highest priority registered task */
extern volatile rtk_p_task_control_block rtk_hp_registered_task;

/* kernel status */
extern volatile rtk_p_status_type rtk_status;

/* pointer to a function. It will be called before entering the background */
extern rtk_BG_hook_fcn_type rtk_background_enter_hook_fcn_pointer;

/* pointer to a function. It will be called after leaving the background */
extern rtk_BG_hook_fcn_type rtk_background_leave_hook_fcn_pointer;

/* ---- task manager functions --------------------------------------------- */

/* initialize interrupt services */
extern int rtk_initialize();

/* reset the scheduling parameters of all tasks */
extern void rtk_reset_scheduling();

/* create task with a given task function */
extern rtk_p_task_control_block rtk_create_task(rtk_task_fcn_type  fcn, 
                                                int                priority, 
                                                rtk_ovc_check_type ovc_check, 
                                                rtk_ovc_fcn_type   ovc_fcn, 
                                                int                ovc_cnt, 
                                                long               user_data);
/* disable a task */
extern int rtk_suspend_task(int service, int subentry);

/* enable a task */
extern int rtk_activate_task(int service, int subentry);

/* define the overrun handling for a task */
extern void rtk_setup_overrun_handling(rtk_p_task_control_block task, 
                                       rtk_ovc_check_type       ovc_check, 
                                       rtk_ovc_fcn_type         ovc_fcn, 
                                       int                      ovc_cnt);

extern int rtk_set_task_type(int service,
                             int subentry,
                             int subsubint,
                             rtk_task_type task_type,
                             rtk_p_task_control_block baserate_task,
                             dsfloat sample_rate_offset,
                             int step_multiple);

extern void rtk_reset_tasks();

void rtk_task_name_set(rtk_p_task_control_block  task,
                       char *name);

extern int rtk_task_scheduling_policy_set(int service,
                                          int subentry,
                                          int subsubint,
                                          rtk_scheduling_policy_type scheduling_policy);

extern int rtk_task_scheduling_policy_get(int service,
                                          int subentry,
                                          int subsubint,
                                          rtk_scheduling_policy_type *scheduling_policy_ptr);

/* ---- scheduler functions ------------------------------------------------ */

/* default overrun function when calling Create_Task */
extern void rtk_default_overrun_fcn(rtk_p_task_control_block task);
void rtk_background_enter_hook_fcn_set(rtk_BG_hook_fcn_type);
void rtk_background_leave_hook_fcn_set(rtk_BG_hook_fcn_type);

/* ---- interrupt manager functions ---------------------------------------- */

/* define a system service */
extern int rtk_define_service(int                  service, 
                              int                  int_vector, 
                              rtk_int_handler_type idle_handler, 
                              rtk_sys_fcn_type     enable_fcn, 
                              rtk_sys_fcn_type     disable_fcn,
                              rtk_init_fcn_type    init_fcn );

/* define the mode of a subinterrupt */
extern int rtk_define_subint_mode(int service,
                                  int subentry,
                                  int mode);

extern int rtk_define_subint_mode_si(int service,
                                     int subentry,
                                     int subsubint,
                                     int mode);

/* define the idle subinterrupt */
extern int rtk_define_idle_service(int service, int subentry);
extern int rtk_define_idle_interrupt(int service, int subentry, int subsubint);

/* define an interrupt source: bind it to a task and/or define its target processor */
extern int rtk_bind_interrupt(int                      service, 
                              int                      subentry, 
                              rtk_p_task_control_block task, 
                              dsfloat                  sample_time, 
                              rtk_target_type          target,
                              int                      log_int_nr, 
                              rtk_hook_fcn_type        hook_fcn);

/* define a sub-interrupt source: bind it to a task and/or define its target processor */
extern int rtk_bind_subinterrupt(int                      service, 
                                 int                      subentry, 
                                 int                      slave_subint_nr,
                                 rtk_p_task_control_block task, 
                                 dsfloat                  sample_time, 
                                 rtk_target_type          target, 
                                 int                      log_int_nr, 
                                 rtk_hook_fcn_type        hook_fcn);

/* enable services */
extern void rtk_enable_services(void);

/* disable services */
extern void rtk_disable_services(void);

/* enable subint */
void rtk_subint_enable(int service,
                       int subentry,
                       int subsubint);

/* disable subint */
void rtk_subint_disable(int service,
                        int subentry,
                        int subsubint);


/* dispatch a sub-interrupt */
void rtk_dispatch_int(int service, int subentry);

/* install an own interrupt handler for a service */
extern rtk_int_handler_type rtk_install_service_vector(int service, 
                                                       rtk_int_handler_type handler);
/* get the interrupt vector of a service */
extern rtk_int_handler_type rtk_get_service_vector(int service);

/* trigger an interrupt by software */
extern void rtk_trigger_interrupt(int service, int subentry);

/* install the defined idle service */
extern void rtk_enter_idle_mode();

/* uninstall the idle service */
extern void rtk_leave_idle_mode();

/* idle function (provided by the user) */
extern void rtk_idle_function();

/* set the step multiple of a defined interrupt */
extern int rtk_step_counter_set( int service, int subentry, int subsubint, int step_multiple);

/* get the step multiple of a defined interrupt */
extern int rtk_step_counter_get( int service, int subentry, int subsubint, int* pSM_Counter_Recipient);

/* ---- dummy functions ---------------------------------------------------- */

/* dummy task function */
extern void rtk_dummy_task_fcn(rtk_p_task_control_block task);

/* dummy overrun function */
extern void rtk_dummy_ovc_fcn(rtk_p_task_control_block);

/* dummy function returning an integer*/
extern void rtk_dummy_fcn(int);

/* ---- common hardware specific functions (in RTKxxxx.C) ------------------ */

/* subentry calculation */
int rtk_get_subentry (int service, int board_base, int int_number);

/* ---- inline functions --------------------------------------------------- */

#ifndef _INLINE

/* return the pointer to a task */
rtk_p_task_control_block rtk_get_task_pointer(int service, int subentry, int subsubint);

/* return the turnaround time of a task */
dsfloat rtk_get_turnaround_time(int service, int subentry);

/* return the mapping table position of a task */
int rtk_get_mapping_pos(int service, int subentry);

/* return the pointer to a mapping table entry of a task */
rtk_p_task_mapping_table_entry rtk_get_mapping_table_entry(int service, int subentry);
rtk_p_task_mapping_table_entry rtk_get_mapping_table_entry_si(int service, int subentry, int subsubint);

/* return the overrun counter of a task */ 
int rtk_get_ovc_counter(int service, int subentry);

/* clear the overrun counter of a task */
int rtk_reset_ovc_counter(int service, int subentry);

/* get an unused service identifier */
int rtk_get_new_service();

/* API functions */
dsfloat rtk_current_task_turnaround_time_get(void);
int rtk_current_task_priority_get(void);
int rtk_current_task_task_id_get(void);
rtk_ovc_check_type     rtk_current_task_ovc_type_get(void);
int rtk_current_task_ovc_counter_get(void);
int rtk_current_task_ovc_repeat_get(void);
int rtk_current_task_ovc_max_get(void);
char* rtk_current_task_task_name_get(void);
dsfloat rtk_current_task_task_calls_get(void);
rtk_timestamp_type*  rtk_current_task_trigger_time_ptr_get(void);
rtk_timestamp_type*  rtk_current_task_absolute_time_ptr_get(void);
int rtk_overrun_flag_get(void);
void rtk_overrun_flag_set(void);
void rtk_overrun_flag_clear(void);
rtk_ovc_check_type rtk_task_ovc_type_get(rtk_p_task_control_block task);
void rtk_task_ovc_type_set(rtk_p_task_control_block task, rtk_ovc_check_type ovc_type);
char* rtk_task_task_name_get(rtk_p_task_control_block task);
dsfloat rtk_task_task_calls_get(rtk_p_task_control_block task);
long rtk_task_user_data_get(rtk_p_task_control_block task);
void rtk_task_user_data_set(rtk_p_task_control_block task, long user_data);
int rtk_int_status_bit_set   (int service, int subentry, int subint, int bitmask);
int rtk_int_status_bit_clear (int service, int subentry, int subint, int bitmask);
int rtk_current_task_dlv_counter_get(void);
void rtk_task_infopointer_set(rtk_p_task_control_block task, int info_id, void* pGlobal_Var);

/* dispatch interrupt and register task */
void rtk_dispatch_int_and_register_task(int service, int subentry);

/* task registering */
rtk_p_task_control_block rtk_register_task(rtk_p_task_control_block task);

/* main scheduler function */
void rtk_scheduler(rtk_p_task_control_block task);

/* rate schduler function */ 
void rtk_rate_scheduler(int service, int first_subentry);

/* read system timer */
unsigned long rtk_read_system_timer();

#else
  #include <rtkif.c>
#endif

/* ------------------------------------------------------------------------- */

#endif
