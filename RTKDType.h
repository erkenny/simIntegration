/******************************************************************************
  FILE:
    RTKDTYPE.H

  DESCRIPTION:
    platform independent data type definitions

  REMARKS:

  AUTHOR(S): R. Leinfellner

  Copyright (c) 1997-98 dSPACE GmbH, GERMANY

  $RCSfile: RTKDType.h $ $Revision: 1.5 $ $Date: 2006/02/15 14:42:56GMT+01:00 $
 *****************************************************************************/

#ifndef __RTK_INT_H__
#define __RTK_INT_H__

/* ---- type definitions --------------------------------------------------- */

typedef struct rtk_task_control_block *       rtk_p_task_control_block;
typedef struct rtk_task_mapping_table_entry * rtk_p_task_mapping_table_entry;
typedef struct rtk_handler_descriptor *       rtk_p_handler_descriptor;
typedef struct rtk_interrupt_descriptor *     rtk_p_interrupt_descriptor;
typedef struct rtk_idle_struct *              rtk_p_idle_struct;
typedef struct rtk_status_type *              rtk_p_status_type;

typedef void (*rtk_task_fcn_type)(rtk_p_task_control_block);
typedef void (*rtk_sys_fcn_type)(int, int);
typedef void (*rtk_ovc_fcn_type)(rtk_p_task_control_block);
typedef void (*rtk_dlv_fcn_type)(rtk_p_task_control_block);
typedef void (*rtk_int_handler_type)(void);
typedef void (*rtk_hook_fcn_type)(int);
typedef void (*rtk_BG_hook_fcn_type)(void);
typedef int  (*rtk_init_fcn_type)(int, int, dsfloat);

typedef enum { idle, ready, running, idle_suspended, ready_suspended } rtk_task_state_type;
typedef enum { ovc_count, ovc_fcn, ovc_queue } rtk_ovc_check_type;
typedef enum { rtk_km_real_time, rtk_km_idle } rtk_mode;
typedef enum { schedule_fcfs, schedule_lcfs } rtk_scheduling_policy_type;

typedef int rtk_target_type;
typedef int rtk_service_type;

typedef enum { rtk_tt_periodic, rtk_tt_aperiodic, rtk_tt_inherited} rtk_task_type;

/* hook package definition */

typedef struct  {
                   rtk_hook_fcn_type       hook_fcn; 
                   rtk_hook_fcn_type       idle_hook_fcn;
                   rtk_sys_fcn_type        enable_fcn;
                   rtk_sys_fcn_type        disable_fcn;
                   rtk_init_fcn_type       init_fcn;
                 } rtk_hook_package_type;


/* ---- system data structure definitions ---------------------------------- */

/* Task Control Block (TCB) */
struct rtk_task_control_block
{ 
  int                      task_id;          /* task identification          */
  rtk_task_fcn_type        fcn;              /* task function                */

  int 		               priority;         /* task priority                */
  rtk_task_state_type      state;            /* scheduling state             */

  rtk_p_task_control_block next;             /* task linking to next entry   */
  rtk_p_task_control_block prev;             /* task linking to prev. entry  */

  rtk_ovc_check_type       ovc_type;         /* overrun check behaviour      */
  rtk_ovc_fcn_type         ovc_fcn;          /* overrun function             */
  int                      ovc_repeat;       /* overrun repeat counter       */
  int                      ovc_max;          /* maximal overrun repeat value */
  int                      ovc_counter;      /* number of overruns occured   */
  
  dsfloat                  sample_time;      /* task sample time             */
  unsigned long            sample_rate;      /* task rate                    */
  unsigned long            rate_counter;     /* task rate counter            */

  dsfloat                  turnaround_time;  /* turnaround time              */
  dsfloat*                 p_global_variable_turnaround_time;
  rtk_timestamp_type       tm_ts_trigger_time; /* timestamp task trigger     */

  long                     user_data;        /* data for the user            */

  rtk_p_task_control_block preempted;        /* preempted task               */

  rtk_task_type            tm_task_type;     /* task type (async./ sync.)    */
  rtk_p_task_control_block tm_baserate_task; /* pointer to the baserate task */
  dsfloat                  tm_task_calls;    /* task counter                 */
  dsfloat*                 p_global_variable_task_calls;
  rtk_timestamp_type       tm_ts_absolute_time; /* corrected absolute time   */
  
  dsfloat                  sample_rate_offset;  /* task offset in seconds    */
  char                     *task_name;       /* pointer to a name string     */
  int                      elog_display_id;  /* handle for the tasks display */
  void                     *p_deadline;      /* pointer to the tasks 
                                                deadline event               */
  dsfloat                  tm_task_calls_idle;  /* saved task call value at
                                                   entering idle mode        */
  rtk_dlv_fcn_type         dlv_fcn;          /* deadline violation function  */
  int                      dlv_counter;      /* deadline violation counter   */
  rtk_scheduling_policy_type  scheduling_policy;/* task scheduling policy    */
};

/* Task mapping list */
struct rtk_task_mapping_table_entry 
{
  rtk_p_task_control_block task;          /* link to a task                  */
  rtk_target_type          target;        /* target board(s)                 */
  rtk_hook_fcn_type        hook_fcn;      /* hook function                   */
  int                      log_int_nr;    /* logical interrupt number        */
  int                      status;        /* status information              */
  int                      hook_service;  /* service associated with the hook*/

  int                      sm_multiple;   /* step multiple                   */
  int                      sm_counter;    /* step counter                    */
};

/* Handler descriptor: defines an interrupt service */
struct rtk_handler_descriptor 
{
  int                    initialized;        /* initialisation flag             */
  int                    int_vector;         /* interrupt vector of the service */
  rtk_sys_fcn_type       enable;             /* function to enable the service  */
  rtk_sys_fcn_type       disable;            /* function to disable the service */
  rtk_init_fcn_type      init_fcn;           /* function to init. the service   */
  int                    target_summary;     /* summary of service targets      */
  int                    first_subentry;     /* first MT subentry position      */
  int                    num_subentries;     /* number of mapping table entries */
  rtk_int_handler_type   idle_handler;       /* idle function                   */
  dsfloat                min_sample_time;    /* minimal sample time             */
  int                    enable_counter;     /* counts the enabled sub-ints     */
  int                    package_id;         /* interrupt package               */
  rtk_sint_receiver_type *sint_receiver;     /* pointer to the subint receiver  */
  rtk_sint_receiver_type *sint_usr_receiver; /* pointer to the subint receiver  */
  unsigned long          additional_info;    /* additional info field           */
  dsfloat                reload_sample_time; /* sample time for reload counters */
  rtk_sint_wb_receiver_type *sint_wb_receiver; /* pointer to the subint receiver  */
};

/* Interrupt descriptor: describes a hardware interrupt */
struct rtk_interrupt_descriptor 
{
  int first_subentry;
  int service;
};
  
/* Idle structure */
struct rtk_idle_struct 
{
  int                  idle_service;      /* idle service                       */
  int                  idle_subentry;     /* idle subinterrupt number           */
  rtk_int_handler_type idle_backup;       /* backup field for normal handler    */
  int                  idle_subsubint;    /* idle subsubinterrupt number        */
  rtk_hook_fcn_type    idle_hook_backup;  /* backup field for the hook function */
};

/* status data structure */
struct rtk_status_type
{
  rtk_mode             mode;              /* kernel mode                     */
};


#endif

