/******************************************************************************
  FILE:
    RTKIF.C

  DESCRIPTION:
    RTKernel inline functions

  REMARKS:

  AUTHOR(S): R. Leinfellner

  Copyright (c) 1997-98 dSPACE GmbH, GERMANY

  $RCSfile: rtkif.c $ $Revision: 1.9 $ $Date: 2006/02/15 14:42:59GMT+01:00 $
******************************************************************************/

#ifndef __RTKIF_C__
#define __RTKIF_C__

#include <limits.h>

#if defined(_DS1003)
# ifdef _INLINE 
#  define __INLINE static inline 
# else
#  define __INLINE
# endif
# include <rtkif1003.c>

#elif defined(_DS1004) || defined(__alpha__)
# ifdef _INLINE 
#  define __INLINE static inline 
# else
#  define __INLINE
# endif
# include <rtkif1004.c>

#elif defined(_DS1005)
# ifdef _INLINE 
#  define __INLINE static 
# else
#  define __INLINE
# endif
# include <rtkif1005.c>

#elif defined(_DS1006)
# ifdef _INLINE 
#  define __INLINE static inline
# else
#  define __INLINE
# endif
# include <rtkif1006.c>

#elif defined(_DS1102)
# ifdef _INLINE 
#  define __INLINE static inline 
# else
#  define __INLINE
# endif
# include <rtkif1102.c>

#elif defined(_DS1103)
# ifdef _INLINE 
#  define __INLINE static 
# else
#  define __INLINE
# endif
# include <rtkif1103.c>

#elif defined(_DS1104)
# ifdef _INLINE 
#  define __INLINE static 
# else
#  define __INLINE
# endif
# include <rtkif1104.c>

#elif defined(_DS1401)
# ifdef _INLINE 
#  define __INLINE static 
# else
#  define __INLINE
# endif
# include <rtkif1401.c>

#elif defined(_DS1602)
# ifdef _INLINE 
#  define __INLINE static 
# else
#  define __INLINE
# endif
# include <rtkif1602.c>

#elif defined(_DS1603)
# ifdef _INLINE 
#  define __INLINE static inline
# else
#  define __INLINE
# endif
# include <rtkif1603.c>

#elif defined(_DS1201)
# error RTKIF.C: This RTKernel version does not support the DS1201!

#else
# error RTKIF.C: No processor board defined (e.g. _DS1003)!
#endif

/* ------------------------------------------------------------------------- */
/*   inline functions                                                        */
/* ------------------------------------------------------------------------- */

/* Get Task Pointer */
__INLINE rtk_p_task_control_block rtk_get_task_pointer (int service,
                                                        int subentry,
                                                        int subsubint)
{
  if (subsubint == -1)
  {
    return rtk_task_mapping_table[rtk_handler_descriptor_table[service].first_subentry + 
                                  subentry].task; 
  }
  else
  {
    return rtk_task_mapping_table[rtk_handler_descriptor_table[
                                    rtk_task_mapping_table[
                                      rtk_handler_descriptor_table[service].first_subentry + subentry].hook_service
                                    ].first_subentry
                                    + subsubint
                                 ].task; 
  }
}

/* ------------------------------------------------------------------------- */

/* Get Turnaround Time */
__INLINE dsfloat rtk_get_turnaround_time(int service, int subentry)
{
  return rtk_task_mapping_table[rtk_handler_descriptor_table[service].first_subentry + 
                                subentry].task->turnaround_time;
}

/* ------------------------------------------------------------------------- */

/* Convert a service/subhandler information into a mapping table position */
__INLINE int rtk_get_mapping_pos(int service, int subentry)
{
  if ((service < rtk_num_services) && 
      (subentry < rtk_handler_descriptor_table[service].num_subentries))
    return rtk_handler_descriptor_table[service].first_subentry + subentry;
  else
    return -1;
}

/* ------------------------------------------------------------------------- */

/* Get the mapping table entry of a subinterrupt */
__INLINE rtk_p_task_mapping_table_entry rtk_get_mapping_table_entry(int service, 
                                                                    int subentry)
{
  if ((service < rtk_num_services) && 
      (subentry < rtk_handler_descriptor_table[service].num_subentries))
    return &(rtk_task_mapping_table[rtk_handler_descriptor_table[service].first_subentry + 
                                                                          subentry]);
  else
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Get the mapping table entry of a subinterrupt */
__INLINE rtk_p_task_mapping_table_entry rtk_get_mapping_table_entry_si(int service, 
                                                                       int subentry,
                                                                       int subsubint)
{
  rtk_p_task_mapping_table_entry mte;

  /* check input arguments */
  if ((service < rtk_num_services) && 
      (subentry < rtk_handler_descriptor_table[service].num_subentries))
  {
    /* get the pointer to the mapping table entry for the task referenced by service and subentry */
    mte = rtk_task_mapping_table + rtk_handler_descriptor_table[service].first_subentry + subentry;

    if (subsubint != -1)
    {
      /* check for existing subsubint (= subentry of the hook service) */
      if ((mte->hook_service < rtk_num_services) &&
          (subsubint < rtk_handler_descriptor_table[mte->hook_service].num_subentries))
      {
        return &(rtk_task_mapping_table[rtk_handler_descriptor_table[mte->hook_service].first_subentry + subsubint]);
      }
      else
        return 0;
    }
    else
      return mte;
  }
  else
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Get the overrun counter of a task */
__INLINE int rtk_get_ovc_counter(int service, int subentry)
{
  return rtk_task_mapping_table[rtk_handler_descriptor_table[service].first_subentry + 
                                subentry].task->ovc_counter;
}

/* ------------------------------------------------------------------------- */

/* Reset the overrun counter of a task */
__INLINE int rtk_reset_ovc_counter(int service, int subentry)
{
  int ovc_cnt;
  rtk_p_task_control_block task;

  task = rtk_task_mapping_table[rtk_handler_descriptor_table[service].first_subentry + 
                                                                      subentry].task;
  ovc_cnt = task->ovc_counter;
  task->ovc_counter = 0;

  return ovc_cnt;
}

/* ------------------------------------------------------------------------- */

/* Get an unused service identifier */
__INLINE int rtk_get_new_service()
{
  return (rtk_new_service++);
}

/* ------------------------------------------------------------------------- */

/* Get the hook service of a sub-interrupt */
__INLINE int rtk_get_hook_service(int service, int subentry)
{
    return rtk_task_mapping_table[rtk_handler_descriptor_table[service].first_subentry + 
                                                                          subentry].hook_service;
}

/* ------------------------------------------------------------------------- */

__INLINE dsfloat rtk_current_task_turnaround_time_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->turnaround_time);
  }
  else
  {
    return (0.0f);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_current_task_priority_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->priority);
  }
  else
  {
    return (0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_current_task_task_id_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->task_id);
  }
  else
  {
    return (0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE rtk_ovc_check_type rtk_current_task_ovc_type_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->ovc_type);
  }
  else
  {
    return ((rtk_ovc_check_type)0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_current_task_ovc_counter_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->ovc_counter);
  }
  else
  {
    return (0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_current_task_ovc_repeat_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->ovc_repeat);
  }
  else
  {
    return (0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_current_task_ovc_max_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->ovc_max);
  }
  else
  {
    return (0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE char* rtk_current_task_task_name_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->task_name);
  }
  else
  {
    return ((char*)0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE dsfloat rtk_current_task_task_calls_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->tm_task_calls);
  }
  else
  {
    return (0.0f);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE rtk_timestamp_type*  rtk_current_task_trigger_time_ptr_get(void)
{
  if (rtk_current_task)
  {
    return (&(rtk_current_task->tm_ts_trigger_time));
  }
  else
  {
    return ((rtk_timestamp_type*)0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE rtk_timestamp_type*  rtk_current_task_absolute_time_ptr_get(void)
{
  if (rtk_current_task)
  {
    return (&(rtk_current_task->tm_ts_absolute_time));
  }
  else
  {
    return ((rtk_timestamp_type*)0);
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_overrun_flag_get(void)
{
  return(rtk_overrun_flag);
}

/* ------------------------------------------------------------------------- */

__INLINE void rtk_overrun_flag_set(void)
{
  rtk_overrun_flag=1;
}

/* ------------------------------------------------------------------------- */

__INLINE void rtk_overrun_flag_clear(void)
{
  rtk_overrun_flag=0;
}

/* ------------------------------------------------------------------------- */

__INLINE rtk_ovc_check_type rtk_task_ovc_type_get(rtk_p_task_control_block task)
{
  return(task->ovc_type);
}

/* ------------------------------------------------------------------------- */

__INLINE void rtk_task_ovc_type_set(rtk_p_task_control_block task, rtk_ovc_check_type ovc_type)
{
  task->ovc_type=ovc_type;
}

/* ------------------------------------------------------------------------- */

__INLINE long rtk_task_user_data_get(rtk_p_task_control_block task)
{
  return (task->user_data);
}

/* ------------------------------------------------------------------------- */

__INLINE void rtk_task_user_data_set(rtk_p_task_control_block task, long user_data)
{
  task->user_data=user_data;
}

/* ------------------------------------------------------------------------- */

__INLINE char* rtk_task_task_name_get(rtk_p_task_control_block task)
{
  return(task->task_name);
}

/* ------------------------------------------------------------------------- */

__INLINE dsfloat rtk_task_task_calls_get(rtk_p_task_control_block task)
{
  return(task->tm_task_calls);
}

__INLINE void rtk_task_infopointer_set(rtk_p_task_control_block task, int info_id, void* pGlobal_Var)
{
  switch (info_id)
  {
        case RTK_TASK_INFO_TURNAROUND_TIME:
            task->p_global_variable_turnaround_time = (dsfloat*)pGlobal_Var;
            break;
        
        case RTK_TASK_INFO_TASK_CALLS:
            task->p_global_variable_task_calls = (dsfloat*)pGlobal_Var;
            break;
        
        default: return;
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_int_status_bit_set   (int service, int subentry, int subsubint, int bitmask)
{
  rtk_p_task_mapping_table_entry mte;
  mte = rtk_get_mapping_table_entry_si(service,subentry,subsubint);
  if (mte)
  {
    mte->status &= ~bitmask;
    return RTK_NO_ERROR;
  }
  else
  {
    return RTK_INVALID_MT_ENTRY;
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_int_status_bit_clear (int service, int subentry, int subsubint, int bitmask)
{
  rtk_p_task_mapping_table_entry mte;
  mte = rtk_get_mapping_table_entry_si(service,subentry,subsubint);
  if (mte)
  {
    mte->status &= ~bitmask;
    return RTK_NO_ERROR;
  }
  else
  {
    return RTK_INVALID_MT_ENTRY;
  }
}

/* ------------------------------------------------------------------------- */

__INLINE int rtk_current_task_dlv_counter_get(void)
{
  if (rtk_current_task)
  {
    return (rtk_current_task->dlv_counter);
  }
  else
  {
    return (0);
  }
}

/*---------------------------------------------------------------------------*/

/* if not already defined by the RTKIT modul, define this as empty macro */
#ifndef RTK_DEADLINE_CANCEL
#define RTK_DEADLINE_CANCEL(task)
#endif

/* ------------------------------------------------------------------------- */

#define RTK_CHECK_EQUAL_PRIORITIES(task)                                      \
                                                                              \
  {                                                                           \
    rtk_p_task_control_block curr_eorq;                                       \
    rtk_p_task_control_block borq;                                            \
                                                                              \
    curr_eorq = rtk_end_of_ready_queue[task->priority];                       \
                                                                              \
    if (task->scheduling_policy == schedule_fcfs)                             \
    {                                                                         \
      if (curr_eorq)                                                          \
      {                                                                       \
        /* remove the task from the list */                                   \
        if (task->prev)                                                       \
          task->prev->next = task->next;                                      \
        else                                                                  \
          rtk_task_list = task->next;                                         \
        if (task->next)                                                       \
          task->next->prev = task->prev;                                      \
                                                                              \
        /* re-insert the task at the end of the ready queue */                \
        task->prev = curr_eorq;                                               \
        task->next = curr_eorq->next;                                         \
        if (task->next)                                                       \
          task->next->prev = task;                                            \
        curr_eorq->next = task;                                               \
      }                                                                       \
      rtk_end_of_ready_queue[task->priority] = task;                          \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      if (curr_eorq)                                                          \
      {                                                                       \
        /* search the begin of the ready queue */                             \
        borq = curr_eorq;                                                     \
        while (borq->prev &&                                                  \
               (task->priority == borq->prev->priority) &&                    \
               ((borq->prev->state == ready ||                                \
                 borq->prev->state == running)) &&                            \
               (borq->prev != task))                                          \
        {                                                                     \
          borq = borq->prev;                                                  \
        }                                                                     \
                                                                              \
        /* remove the task from the list */                                   \
        if (task->prev)                                                       \
          task->prev->next = task->next;                                      \
        else                                                                  \
          rtk_task_list = task->next;                                         \
        if (task->next)                                                       \
          task->next->prev = task->prev;                                      \
                                                                              \
        /* re-insert the task at the end of the ready queue */                \
        task->prev = borq->prev;                                              \
        task->next = borq;                                                    \
        borq->prev = task;                                                    \
        if (task->prev)                                                       \
          task->prev->next = task;                                            \
      }                                                                       \
      else                                                                    \
      {                                                                       \
        rtk_end_of_ready_queue[task->priority] = task;                        \
      }                                                                       \
    }                                                                         \
  } 

/* ------------------------------------------------------------------------- */

#ifdef ELOG_AVAILABLE
#define RTK_DO_REGISTER_TASK(task) {                                                   \
                                     task->state = ready;                              \
                                     task->tm_task_calls++;                            \
                                     if (task->p_global_variable_task_calls)           \
                                     {                                                 \
                                                *(task->p_global_variable_task_calls)= \
                                                            task->tm_task_calls;       \
                                     }                                                 \
                                     task->ovc_repeat = 1;                             \
                                     task->preempted = rtk_current_task;               \
                                     RTK_READ_SYSTEM_TIME((task->tm_ts_trigger_time)); \
                                     ELOG_TS_ENTRY_SET32(ELOG_SPEC_TASK_REGISTER,      \
                                                      task->tm_task_calls,             \
                                                      task->task_id,                   \
                                                      &(task->tm_ts_trigger_time));    \
                                     RTK_CALCULATE_ABSOLUTE_TIME();                    \
                                     RTK_CHECK_EQUAL_PRIORITIES(task);                 \
                                   }
#else
#define RTK_DO_REGISTER_TASK(task) {                                                   \
                                     task->state = ready;                              \
                                     task->tm_task_calls++;                            \
                                     if (task->p_global_variable_task_calls)           \
                                     {                                                 \
                                                *(task->p_global_variable_task_calls)= \
                                                            task->tm_task_calls;       \
                                     }                                                 \
                                     task->ovc_repeat = 1;                             \
                                     task->preempted = rtk_current_task;               \
                                     RTK_READ_SYSTEM_TIME((task->tm_ts_trigger_time)); \
                                     RTK_CALCULATE_ABSOLUTE_TIME();                    \
                                     RTK_CHECK_EQUAL_PRIORITIES(task);                 \
                                   }
#endif


/* ------------------------------------------------------------------------- */

#ifdef ELOG_AVAILABLE
#define RTK_HANDLE_OVERRUN() {                                                \
                               ELOG_ENTRY_SET32(ELOG_SPEC_OVERRUN,            \
                                                task->tm_task_calls,          \
                                                task->task_id);               \
                               switch(task->ovc_type)                         \
                               {                                              \
                                 case ovc_fcn:   task->ovc_counter++;         \
                                                 task->ovc_fcn(task);         \
                                                 break;                       \
                                 case ovc_count: task->ovc_counter++;         \
                                                 break;                       \
                                 case ovc_queue: if (task->ovc_repeat++ >     \
                                                               task->ovc_max) \
                                                   task->ovc_fcn(task);       \
                                                 task->ovc_counter++;         \
                                                 break;                       \
                               }                                              \
                             }
#else
#define RTK_HANDLE_OVERRUN() {                                                \
                               switch(task->ovc_type)                         \
                               {                                              \
                                 case ovc_fcn:   task->ovc_counter++;         \
                                                 task->ovc_fcn(task);         \
                                                 break;                       \
                                 case ovc_count: task->ovc_counter++;         \
                                                 break;                       \
                                 case ovc_queue: if (task->ovc_repeat++ >     \
                                                               task->ovc_max) \
                                                   task->ovc_fcn(task);       \
                                                 task->ovc_counter++;         \
                                                 break;                       \
                               }                                              \
                             }
#endif

/* ------------------------------------------------------------------------- */

#define RTK_SET_IDLE_SUSPENDED()                                              \
                                                                              \
  {                                                                           \
    task->state = idle_suspended;                                             \
    if (rtk_end_of_ready_queue[task->priority] == task)                       \
    {                                                                         \
      if ((task->prev) && (task->priority == task->prev->priority))           \
      {                                                                       \
        rtk_end_of_ready_queue[task->priority] = task->prev;                  \
      }                                                                       \
      else                                                                    \
      {                                                                       \
        rtk_end_of_ready_queue[task->priority] = 0;                           \
      }                                                                       \
    }                                                                         \
  }

/* ------------------------------------------------------------------------- */

__INLINE rtk_p_task_control_block rtk_register_task(rtk_p_task_control_block task)
{
  switch (task->state)
  {
    case idle:            if (rtk_hp_registered_task)
                          {
                            if (task->scheduling_policy == schedule_fcfs)
                            {
                              if (task->priority < rtk_hp_registered_task->priority)
                                  rtk_hp_registered_task = task;
                            }
                            else
                            {
                              if (task->priority <= rtk_hp_registered_task->priority)
                                  rtk_hp_registered_task = task;
                            }
                          }
                          else 
                            rtk_hp_registered_task = task;

                          RTK_DO_REGISTER_TASK(task);

                          break;

    case idle_suspended:  break;

    case ready_suspended: RTK_SET_IDLE_SUSPENDED();
                          break;
    case running:
    case ready:           RTK_HANDLE_OVERRUN();
                          break;
  }
  
  return rtk_hp_registered_task;
}

/* ------------------------------------------------------------------------- */

/* Main scheduler function */
__INLINE void rtk_scheduler(rtk_p_task_control_block task)
{  
  rtk_timestamp_type time_stamp;

  if (task != rtk_hp_registered_task)
  { 
    switch (task->state)
    {
      case idle:            RTK_DO_REGISTER_TASK(task) ;
                            break;

      case idle_suspended:  break;

      case ready_suspended: RTK_SET_IDLE_SUSPENDED();
                            break;
  
      case running:
      case ready:           RTK_HANDLE_OVERRUN();
                            return;
    }
  }
  
  /* The background (BG) has only been left if the PC was just before inside the
  BG. This is the fact when no task has the state running. (The scheduler 
  currently handles no task) */
  if ((0 != rtk_background_leave_hook_fcn_pointer) && (0 == rtk_current_task)) 
  {
      rtk_background_leave_hook_fcn_pointer();
  }
  
  rtk_hp_registered_task = 0;               /* clear highest priority task   */

  if (task->scheduling_policy == schedule_fcfs)
  {
    if ((rtk_current_task) && (task->priority >= rtk_current_task->priority))
      return;                                 /* return if task has a lower    */
                                              /* priority than the currently   */
  }
  else
  {
    if ((rtk_current_task) && (task->priority > rtk_current_task->priority))
      return;                                 /* return if task has a lower    */
                                              /* priority than the currently   */
  }

  do {                                      /* process the task list         */

    switch (task->state)                                        
    {
      case ready:   /* modify TCB */
                    task->state = running;

                    /* set current task */
                    rtk_current_task = task;

                    do {
#ifdef ELOG_AVAILABLE
                      ELOG_ENTRY_SET32(ELOG_SPEC_TASK_START, task->tm_task_calls, task->task_id);
#endif
                      global_enable();      /* enable interrupts globally    */
                      task->fcn(task);      /* call the task function        */
                      global_disable();     /* disable interrupts globally   */

                      RTK_READ_SYSTEM_TIME(time_stamp);
#ifdef ELOG_AVAILABLE
                      ELOG_TS_ENTRY_SET32(ELOG_SPEC_TASK_END, task->tm_task_calls, task->task_id, &time_stamp);
#endif
                      RTK_DEADLINE_CANCEL(task);

                      task->turnaround_time = RTK_CALCULATE_TIME(time_stamp, task->tm_ts_trigger_time);
                      if (task->p_global_variable_turnaround_time)
                      {
                          *(task->p_global_variable_turnaround_time) = task->turnaround_time;
                      }
                      
                      task->tm_task_calls++;
                      if (task->p_global_variable_task_calls)
                      {
                          *(task->p_global_variable_task_calls) = task->tm_task_calls;
                      }
                      
                    } while(--task->ovc_repeat);
                    
                    task->tm_task_calls--;
                    if (task->p_global_variable_task_calls)
                    {
                        *(task->p_global_variable_task_calls) = task->tm_task_calls;
                    }
                    

                    /* clear EORQ */
                    if (rtk_end_of_ready_queue[task->priority] == task)
                      rtk_end_of_ready_queue[task->priority] = 0;
                    
                    /* set task state back to idle */
                    task->state = idle;
                    break;

      case running: rtk_current_task = task;
                    return;

      default: break;
    }          
    task = task->next;                      /* get next task list entry      */
  } while (task);                           /* repeat until no task is left  */

  rtk_current_task = 0;
  
  if (0 != rtk_background_enter_hook_fcn_pointer)
  {
      rtk_background_enter_hook_fcn_pointer();
  }
}

/* ------------------------------------------------------------------------- */

__INLINE void rtk_rate_scheduler(int service, int first_subentry)
{
  rtk_p_task_control_block task;
  rtlib_int_status_t status;
  int mapping_pos, n;

  RTK_INT_SAVE_AND_DISABLE(status);
  
  mapping_pos = rtk_handler_descriptor_table[service].first_subentry;

  for(n = first_subentry; n < rtk_handler_descriptor_table[service].num_subentries; n++)
  {
    task = rtk_task_mapping_table[mapping_pos + n].task;
    if (++task->rate_counter == task->sample_rate)
    {
      task->rate_counter = 0;
      rtk_register_task(task);
    }
  }

  if (rtk_hp_registered_task)
    rtk_scheduler(rtk_hp_registered_task);

  RTK_INT_RESTORE(status);
}

/* ------------------------------------------------------------------------- */

__INLINE void rtk_dispatch_int_and_register_task(int service, int subentry)
{
  rtk_p_task_mapping_table_entry mte;

  /* dispatch the sub-interrupt */
  if (rtk_handler_descriptor_table[service].target_summary != C_LOCAL)
    rtk_dispatch_int(service, subentry);

  /* get mapping table entry */
  mte = rtk_task_mapping_table + rtk_handler_descriptor_table[service].first_subentry + subentry;

  /* register task */
  if (mte->task)
    rtk_register_task(mte->task);
}

/* ------------------------------------------------------------------------- */
/*     RTKFlags                                                              */
/* ------------------------------------------------------------------------- */

#ifdef RTK_KM_FLAGS

/* poll flag */
__INLINE int rtk_flag_poll(int flag_id)
{
  return *rtk_flags[flag_id]->flag;
}

#endif

/* ------------------------------------------------------------------------- */

#undef __INLINE

#endif
