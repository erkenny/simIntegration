/*********************************************************
* Header file for RealMotion - Services
*
* Internal structures
*
*********************************************************/
/* $RCSfile: Rmserv.h $ $Revision: 1.3 $ $Date: 2004/05/19 10:35:00GMT+01:00 $ */

#ifndef _RMSERV_H
#define _RMSERV_H
#include "rmproto.h"

/* Definition of local constants used in library routines only */
#define RM_MAX_LENGTH_NAMES   20


/* Type definition of the descriptor structure */
typedef struct
{
  /* type of the Service */
  long ServiceType;

  /* version of the Service */
  long Version;

  /* reserved for expansion */
  long HookPtr;

  /* initialization flag for the services */
  int  InitFlag;

} RM_SERVICE_STRUCT;

/* Type definition of the description structures for each graphics member  */
typedef struct
{
  /* Internal index for each graphics object */
  UInt32   rm_id;

  /* Name of each transformation function. As the characters are read 32 Bit
     oriented by the Host, they are defined as UInt32 */
  UInt32   rm_names[RM_MAX_LENGTH_NAMES + 1];

} RM_DS_MEMBER_NAMES;

/* Type definition of the description structures for each graphics member  */
typedef struct
{
  /* Internal index for each graphics object */
  UInt32   rm_id;

  /* Name of each transformation function. As the characters are read 32 Bit
     oriented by the Host, they are defined as UInt32 */
  char     rm_names[RM_MAX_LENGTH_NAMES + 1];

} RM_MEMBER_NAMES;


typedef union
{
   UInt32 dummy;
   RM_STATE rm_sim_state;
} rm_sim_state_union;

/* data structure for the Ring_buffered mode */
typedef struct
{
  /* signal state of service */
  rm_sim_state_union state;

  /* simulation time stamp */
  Float32 rm_tsim;

  /* pointer to the ringbuffer data */
  RM_MEMBER_DATA member[1];

} RM_RING_BUFFER_ELEMENT;


/* initialization structure for the Ring_buffered mode */
typedef struct
{
  /* Number of RM_RING_BUFFER_DATA elements set in RMSTREAM */
  UInt32   rm_nring_buf_elem;

  /* Downsampling Rate set in RMSTREAM */
  UInt32   rm_downsampling;

  /* Head of the Ring buffer */
  RM_RING_BUFFER_ELEMENT*    rm_top_elem_ptr;

  /* Ring buffer data pointer of the Host */
  RM_RING_BUFFER_ELEMENT*    rm_read_ptr;

  /* Ring buffer data pointer of the DSP */
  RM_RING_BUFFER_ELEMENT*    rm_write_ptr;

  /* ring buffer is full */
  UInt32   rm_overflow;

  /* DT of the DSP application */
  Float32  rm_dt;

  /* counter of the interrupt */
  UInt32   rm_count_isr;

} RM_RING_BUFFER;


/* Type definition of the complete interface descriptor */
typedef struct
{
  /* synchronization with host */
  RM_ACCESS rm_sync_flag;

  /* signal state of service */
   RM_STATE rm_sim_state;

  /* number of members */
   UInt32 rm_num_members;

  /* simulation time stamp */
  Float32 rm_tsim;         

   /* pointer to table of data */
   RM_MEMBER_DATA_PTR rm_model_data_ptr;      

   /* pointer to names list */
   RM_DS_MEMBER_NAMES *rm_names_list_ptr;      

  /* error flag for diagnostics */
  RM_ERROR rm_error_code;

  /* pointer to the ring_buffer struct */
  RM_RING_BUFFER* rm_ring_buffer_ptr;

   /* signals state of snapshot buffer */   
  RM_SERVICE_STATE rm_service_state;

  /* indicates the mode */
  RM_MODE rm_mode_flag;

   /* pointer to table of data */
  RM_MEMBER_DATA_PTR rm_member_data_ptr;      

   /* pointer to snapshot buffer */
   RM_SNAPSHOT_BUFFER_PTR rm_snapshot_buffer_ptr; 

   /* pointer to transformation function  */
   RM_TRANS_FUNCTION_PTR rm_transformation_function;

   /* pointer to snapshot function*/                     
   RM_SNAPSHOT_FUNCTION_PTR rm_snapshot_function;

   /* 64-bit double precision simulation time stamp */
   Float64 rm_tsim64;

   /* tells if simulation supports maneuvers or not */
   RM_MANEUVER_SUPPORT rm_maneuver_supported;

   /* keeps the time when the maneuver is being started */
   Float64 rm_maneuver_start_time;

} RM_INTERFACE_DESCRIPTOR;

#endif      /* _RMSERV_H */                                          
