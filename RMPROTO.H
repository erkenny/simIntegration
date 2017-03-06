/***********************************************
* Header file for RealMotion - Services
*
* defining the maximum number of members
* and the functional interface
*
***********************************************/
/* $RCSfile: RMPROTO.H $ $Revision: 1.2 $ $Date: 2004/05/26 11:57:50GMT+01:00 $ */

/* Maximum number of member elements that can be requested from the server */
#ifndef _RM_PROTO__
#define _RM_PROTO__

#define RM_DIMENSION            3
#define RM_HOMOG_DIMENSION      4
#define RM_OFF_DIAGONAL_INIT    0.0
#define RM_DIAGONAL_INIT        1.0
#define RM_TRANSLATE_INIT       0.0
#define RM_TRUE                 1
#define RM_FALSE                0

/* Definitions of the error states */
#define RM_NO_ERROR               0
#define RM_OUT_OF_RANGE           -1101
#define RM_OUT_OF_MEMORY          -1102
#define RM_ILLEGAL_FUNCTION_PTR   -1103
#define RM_NOT_INITIALIZED        -1104


#ifndef __DSTYPES_H__
  typedef unsigned long int    UInt32;
  typedef float                Float32;
#endif


#ifdef  __cplusplus
extern "C" {
#endif


/* Enum type defining the states of the RealMotion service */
typedef enum
{
  RM_DSP_STOP = 0,
  RM_DSP_PAUSE,
  RM_DSP_RUN
} RM_STATE;

/* Enum type defining the access states of the snapshot-buffer */
typedef enum
{
  RM_INIT = 0,
  RM_FROZEN,
  RM_PROCESSING
} RM_SERVICE_STATE;


/* Enum type defining the access states of the communication buffer */
typedef enum
{
  RM_NO_ACCESS = 0,
  RM_DSP_ACCESS,
  RM_HOST_ACCESS
} RM_ACCESS;


/* Enum type defining the data transfer mode */
typedef enum
{
  RM_NORMAL = 0,
  RM_RING_BUFFERED
} RM_MODE;

/* Enumerator type indicating if maneuvers are supported or not. */
typedef enum
{
  RM_MANEUVER_UNSUPPORTED = 0,
  RM_MANEUVER_SUPPORTED
} RM_MANEUVER_SUPPORT;

/* Type definition of the interface structure between */
/* RealMotion Service and host */
typedef struct
{
  UInt32   rm_id;                                   /* Internal id per member */
  Float32  rm_trans[RM_DIMENSION];                  /* Translation vector     */
  Float32  rm_rot[RM_DIMENSION][RM_DIMENSION];      /* Rotation matrix        */
} RM_MEMBER_DATA;

                                                    /* homogeneous data matrix*/
typedef double RM_HOMOG_MEMBER_DATA[RM_HOMOG_DIMENSION][RM_HOMOG_DIMENSION];


/* Type definition of error return state */
typedef int                     RM_ERROR;
typedef void*                   RM_SNAPSHOT_BUFFER_PTR;
typedef RM_MEMBER_DATA*         RM_MEMBER_DATA_PTR;
typedef RM_HOMOG_MEMBER_DATA*   RM_HOMOG_MEMBER_DATA_PTR;
typedef void                    (*RM_SNAPSHOT_FUNCTION_PTR)
                                (RM_SNAPSHOT_BUFFER_PTR rm_snapshot_buffer_ptr);
typedef void                    (*RM_TRANS_FUNCTION_PTR)
                                (RM_SNAPSHOT_BUFFER_PTR rm_snapshot_buffer,
                                 RM_MEMBER_DATA_PTR rm_member_data_buffer_ptr);

/* Interface definitions */
RM_ERROR rm_init_interface( UInt32                   rm_number_of_elements,
                            RM_TRANS_FUNCTION_PTR    rm_transformation_function,
                            RM_SNAPSHOT_FUNCTION_PTR rm_snapshot_function,
                            UInt32                   buffer_size );

RM_ERROR rm_set_member_name( unsigned int rm_id, char rm_name[] );

RM_ERROR rm_init_member( RM_MEMBER_DATA current_transfer );

void rm_set_homog_member_pos( unsigned int rm_id, RM_HOMOG_MEMBER_DATA_PTR homog_data_ptr );

RM_MEMBER_DATA_PTR rm_get_member_data_ptr( UInt32 current_member_id );

void rm_set_sim_state( UInt32 rm_state );

void rm_service_realmotion( void );

void rm_foreground_service( double tsim );

void rm_snapshot( UInt32 rm_state, Float32 tsim );

void rm_close_interface( void );

void rm_set_maneuver_support( RM_MANEUVER_SUPPORT maneuver_support );

void rm_set_maneuver_start_time( double maneuver_start_time );

#ifdef  __cplusplus
}
#endif

#endif

