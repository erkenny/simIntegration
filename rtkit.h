/***************************************************************************
*
*  MODULE
*    RTKIT
*
*  FILE
*    rtkit.h
*
*  RELATED FILES
*    rtkit.c, rtkitXXXX.h, rtkitXXXXdef.h
*
*  DESCRIPTION
*    RTK interval timer module
*
*  REMARKS
*
*  AUTHOR(S)
*    T. Woelfer
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: rtkit.h $ $Revision: 1.4 $ $Date: 2005/11/10 08:36:51GMT+01:00 $
***************************************************************************/
#ifndef __RTKIT_H__
#define __RTKIT_H__

#if !(defined(_DS1005) || defined(_DS1103) || defined(_DS1401) || defined(_DS1006) ||defined(_DS1603) ) 
# error rtkit.h: interval timer module not supported!
#endif

#include <dstypes.h>
#if defined (_DS1005)
#include <rtkit1005.h>
#elif defined (_DS1006)
#include <rtkit1006.h>
#elif defined (_DS1103)
#include <rtkit1103.h>
#elif defined (_DS1104)
#include <rtkit1104.h>
#elif defined (_DS1401)
#include <rtkit1401.h>
#elif defined (_DS1603)
#include <rtkit1603.h>
#endif
#include <rtkernel.h>


/***************************************************************************
  constant, macro and type definitions
***************************************************************************/

#define RTK_IT_INIT_ERROR 1

#define RTK_IT_TT_INVALID_ROUND    1
#define RTK_IT_TT_EVENT_FAILED     2

/* use the internal roundcounter when registering time tables */
#define RTK_IT_TT_CURRENTROUND 0

/*---------------------------------------------------------------------------*/

typedef dsfloat rtk_it_time_type;

typedef UInt32 rtk_it_tics_type;

/* data structures for TimeTables */

/* data structure for making event specifications */
typedef struct
{
  int service;                 /* service of the task      */
  int subentry;                /* sub-entry                */
  rtk_it_time_type interval;   /* the start delay interval */
  rtk_it_time_type deadline;   /* task deadline time       */
  rtk_timestamp_type ts_interval;
  rtk_timestamp_type ts_deadline;
} rtk_it_tt_spec_type;

/* the round header */
typedef struct
{
  int num_events;                 /* number of events for this round */
  rtk_it_tt_spec_type *p_specs;   /* pointer to the event spec array */
} rtk_it_tt_round_type;

/* the TimeTable header */
typedef struct
{
  int num_rounds;                 /* num of rounds for this TaskTable     */
  int current_round;              /* active round for the next tt trigger */
  rtk_it_tt_round_type *p_rounds; /* pointer to the rounds array          */
} rtk_it_tt_type;
typedef rtk_it_tt_type * rtk_it_p_tt_type;

typedef struct rtk_it_event_struct rtk_it_event_type;
typedef struct rtk_it_event_struct *rtk_it_p_event_type;

struct rtk_it_event_struct
{
  int                service;      /* service of the task            */
  int                subentry;     /* sub-entry                      */
  rtk_it_tics_type   tics_abs;     /* absolute counter tics          */
  rtk_timestamp_type time_abs;     /* absolute time as a time stamp  */
  rtk_it_event_type  *p_next;      /* pointer to next queue entry    */
  rtk_it_event_type  *p_deadline;  /* pointer to a deadline event    */
  int                status;       /* contains the active and
                                      deadline bits */
  rtk_it_tt_type     *p_tt;        /* pointer to a time table which 
                                      triggered the event */
};

/* cancel deadlines */
#define RTK_IT_DEADLINE_CANCEL(task)                             \
  if (task->p_deadline)                                          \
  {                                                              \
    rtk_it_event_cancel((rtk_it_p_event_type) task->p_deadline); \
  }

#define RTK_DEADLINE_CANCEL(task) RTK_IT_DEADLINE_CANCEL(task)

/***************************************************************************
  function prototypes
***************************************************************************/

int rtk_it_initialize();

void rtk_it_queue_reset();

void rtk_it_event_cancel(rtk_it_p_event_type it_event);

rtk_it_time_type rtk_it_event_status(rtk_it_p_event_type it_event);

rtk_it_p_event_type rtk_it_task_register_abs(int service,
                                             int subentry,
                                             int subsubint,
                                             ts_timestamp_type *ts_p_event,
                                             ts_timestamp_type *ts_p_deadline,
                                             dsfloat sample_time);

rtk_it_p_event_type rtk_it_task_register_rel(int service,
                                             int subentry,
                                             int subsubint,
                                             rtk_it_time_type interval,
                                             rtk_it_time_type deadline,
                                             dsfloat sample_time,
                                             ts_timestamp_type *ts_p_ref);

rtk_it_p_event_type rtk_it_task_register_tics_abs(int service,
                                                  int subentry,
                                                  int mapping_pos,
                                                  ts_timestamp_type *ts_p_start,
                                                  rtk_it_tics_type tics_start,
                                                  rtk_it_tics_type event_tics,
                                                  int event_type);

int rtk_it_tt_register_rel(rtk_it_tt_type    *p_time_table,
                           int                round_no,
                           dsfloat            sample_time,
                           ts_timestamp_type *ts_p_ref);

rtk_it_tt_type* rtk_it_tt_entry_add(int service,
                                    int subentry,
                                    int subsubint,
                                    rtk_it_time_type interval,
                                    rtk_it_time_type deadline,
                                    rtk_it_p_tt_type p_time_table,
                                    int round);

void rtk_it_tt_print(rtk_it_tt_type *p_time_table);

void rtk_it_tt_delete(rtk_it_tt_type *p_time_table);

int rtk_it_tt_round_get(rtk_it_tt_type *p_time_table);

void rtk_it_tt_cancel(rtk_it_tt_type *p_time_table);

void rtk_it_dlv_fcn_set(rtk_p_task_control_block p_task,
                        rtk_dlv_fcn_type         p_dlv_fcn);



#endif /* __RTKIT_H__ */
