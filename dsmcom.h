/******************************************************************************
*
* MODULE
*   General master-slave communication functions (hardware independent)
*
*   Version with static communication buffers
*   Version for master CPU
*
* FILE
*   dsmcom_s.h
*
* RELATED FILES
*   dsmcom.c, Comdef.h
*
* DESCRIPTION
*   Functions for command table controlled master-slave communication.
*   - communication initialization
*   - command registration
*   - command execution
*   - return data retrieval
*
* AUTHOR(S)
*   H.-J. Miks, H.-G. Limberg, V. Lang
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsmcom.h $ $Revision: 1.5 $ $Date: 2006/07/24 16:53:05GMT+01:00 $
******************************************************************************/

#ifndef __DSMCOM_S_H__
#define __DSMCOM_S_H__

#include <dstypes.h>                      /* platform independent data types */
#include <dscomdef.h>     /* common definitions for master-slave communication */
#include <dsvcm.h>

#if defined (_DS1103)
  #include <dpm1103.h>
  #include <tmr1103.h>
  #define dpm_timer_read() (unsigned long)ds1103_timebase_low_read()
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1104)
  #include <dpm1104.h>
  #include <tmr1104.h>
  #define dpm_timer_read() (unsigned long)ds1104_timebase_low_read()
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1401)
  #include <dpm1401.h>
  #include <tmr1401.h>
  #define dpm_timer_read() (unsigned long)ds1401_timebase_low_read()
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1603)
  #include <ds1603dpm.h>
  #define dpm_timer_read() (unsigned long)TS_TIMEBASE_READ_LOW()
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1003)
  #include <dpm1003.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif defined (_DS1005)
  #include <dpm1005.h>
  #include <tmr1005.h>
  #define dpm_timer_read() (unsigned long)ds1005_timebase_low_read()
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1006)
  #include <dpm1006.h>
  #include <tmr1006.h>
  #define dpm_timer_read() (unsigned long)ds1006_timebase_low_read()
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#else
  #error ERROR: No target defined.
#endif
/* ---- DSMCOM version -------------------------------------------------- */

/* $DSVERSION: DSMCOM - Master Slave Communication Module */

#define DSMCOM_VER_MAR 1
#define DSMCOM_VER_MIR 3
#define DSMCOM_VER_MAI 1
#define DSMCOM_VER_SPB 0x0F /* VCM_VERSION_RELEASE */
#define DSMCOM_VER_SPN 0
#define DSMCOM_VER_PLV 0

/******************************************************************************
* constant definitions
******************************************************************************/

/* error codes */

#define DSMCOM_NO_ERROR          0                                /* no error */
#define DSMCOM_ALLOC_ERROR       1                 /* memory allocation error */
#define DSMCOM_ILLEGAL_TASK_ID   2                    /* task ID out of range */
#define DSMCOM_ILLEGAL_INDEX     3              /* command index out of range */
#define DSMCOM_BUFFER_OVERFLOW   4               /* communication buffer full */
#define DSMCOM_NO_DATA           5                /* no return data available */
#define DSMCOM_DATA_LOST         6                        /* return data lost */
#define DSMCOM_TIMEOUT           7                /* slave CPU not responding */

/* code for errors caused by the slave */
#define DSMCOM_SLV_ALLOC_ERROR       8
#define DSMCOM_SLV_BUFFER_OVERFLOW   9
/* special error code for slave initialization acknowledge */
#define DSMCOM_SLV_INIT_ACK         10
/* the error flag is undefined */
#define DSMCOM_SLV_UNDEFINED        11
#define DSMCOM_SLV_ILLEGAL_USR_IDX  12

/* wrong index in the result queue */
#define DSMCOM_WRONG_INDEX          13

/* wrong datalen in the result queue */
#define DSMCOM_WRONG_DATALEN        14

#define DSMCOM_SLV_DATA_LOST        15
#define DSMCOM_SLV_FIFO_OVERFLOW    16
#define DSMCOM_FIRST_NO_DATA        17

#define DSMCOM_SEMA_ACCESS_ERROR   200

/* definitions for firmware version evaluation */
#define DSMCOM_FW_MAJOR     0xF800
#define DSMCOM_FW_MINOR     0x0780
#define DSMCOM_FW_MAINT     0x0078
#define DSMCOM_FW_VERSION   0x0006
#define DSMCOM_FW_ORIGIN    0x0001
#define DSMCOM_FW_FINAL     0x0000
#define DSMCOM_FW_BETA      0x0002
#define DSMCOM_FW_ALPHA     0x0004
#define DSMCOM_FW_STRLEN    10
#define DSMCOM_FW_DSPACE    0
#define DSMCOM_FW_USER      1

/*--- FIFO ------------------------------------------------------------------*/

#define DSMCOM_FIFO_CLEAR            0x0000                    /* no command */

/*--- FIFO semaphore for Master Read Pointer --------------------------------*/

#define DSMCOM_MST_SEMA_ENABLE           1   
#define DSMCOM_SLV_SEMA_ENABLE           2

/******************************************************************************
* type definitions
******************************************************************************/

/* command table data structure */

typedef struct
{
  unsigned int  rx_count;
  unsigned int  data_in_count;
  unsigned long *data_in;
  unsigned int  data_out_count;
} dsmcom_cmdtbl_t;

typedef struct
{
  unsigned int         master_write_ptr;
  unsigned int         master_read_ptr;
  unsigned long        master_read_ptr_addr;
  unsigned long        slave_read_ptr_addr;
  unsigned long        slave_error_addr;
  unsigned int         slave_address_increment;
  unsigned long        tx_buffer_base;
  unsigned long        rx_buffer_base;
  unsigned int         tx_buffer_size;
  unsigned int         rx_buffer_size;
  long                 target;
  dpm_write_fcn_t      write_fcn;
  dpm_read_fcn_t       read_fcn;
  unsigned int         channel_count;
  unsigned int         cmdtbl_size;
  unsigned int         cmdtbl_entry_count;
  dsmcom_cmdtbl_t      **cmdtbl_entry;
  unsigned long        dpm_mask;
} dsmcom_cmdtbl_header_t;

typedef struct
{
  unsigned int         master_write_ptr;
  unsigned int         master_read_ptr;
  unsigned long        slave_error_ack;
  unsigned long        master_write_ptr_addr;
  unsigned long        master_read_ptr_addr;
  unsigned long        slave_write_ptr_addr;
  unsigned long        slave_read_ptr_addr;
  unsigned long        slave_error_addr;
  unsigned long        slave_error_ack_addr;
  unsigned long        semaphore_addr;
  unsigned int         slave_address_increment;
  unsigned long        tx_buffer_base;
  unsigned long        rx_buffer_base;
  unsigned int         tx_buffer_size;
  unsigned int         rx_buffer_size;
  long                 target;
  dpm_write_fcn_t      write_fcn;
  dpm_read_fcn_t       read_fcn;
  unsigned int         channel_count;
} dsmcom_fifo_header_t;

/******************************************************************************
* function prototypes
******************************************************************************/

void dsmcom_vcm_register(void);

int dsmcom_init (dsmcom_cmdtbl_header_t ***handle,
  unsigned long buffer_base, long target, unsigned int channel_count, unsigned int buffer_size,
  unsigned int master_address_increment, unsigned int slave_address_increment,
  dpm_write_fcn_t write_fcn, dpm_read_fcn_t  read_fcn);

__INLINE unsigned long dsmcom_buffer_space (dsmcom_cmdtbl_header_t *cmdtbl_p);

int dsmcom_register (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  int *index, unsigned long cmd, unsigned int data_in_count, unsigned int data_out_count,
  unsigned int param_count, unsigned long *param);

__INLINE int dsmcom_send (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  unsigned long cmd, unsigned int data_count, unsigned long *data);

__INLINE int dsmcom_execute (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id, int index);

__INLINE int dsmcom_execute_data (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id, int index, unsigned long *data);

__INLINE int dsmcom_data_read(dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id, int index, unsigned long *data);

__INLINE void dsmcom_slave_error_read (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id, unsigned long *slave_error);
__INLINE void dsmcom_slave_error_clear (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id);
__INLINE int dsmcom_slave_acknowledge_check (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id);

int dsmcom_var_send (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  unsigned long cmd, unsigned int data_count, unsigned long *data);

int dsmcom_data_read_varlen (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  int index, unsigned long *data);

int dsmcom_vardata_update (dsmcom_cmdtbl_header_t *cmdtbl_p);

int dsmcom_get_entry_varlen (dsmcom_cmdtbl_header_t **handle,
                                    unsigned int task_id,
                                    unsigned int datalen,
                                    unsigned long* data_in,
                                    int* index);

int dsmcom_fw_version_print(char *vs_str, UInt16 vs_value);

int dsmcom_fw_register_vcm(UInt16 vs_value, UInt32 module_id, char *module_txt,
                           UInt8 req_mar, UInt8 req_mir, UInt8 req_mai,
                           UInt8 req_spb, UInt8 req_spn, UInt8 req_plv,
                           UInt32 location, UInt32 burn_count);

int dsmcom_user_fw_register_vcm(UInt16 vs_value, UInt32 usr_mod_id, char *usr_mod_txt,
                                UInt8 usr_maj, UInt8 usr_min, UInt8 usr_sub,
                                UInt8 req_mar, UInt8 req_mir, UInt8 req_mai,
                                UInt8 req_spb, UInt8 req_spn, UInt8 req_plv,
                                UInt32 location, UInt32 burn_count);

/*--- FIFO ------------------------------------------------------------------*/
#if defined _DS1005 || defined _DS1006 || defined _DS1103 || defined _DS1104 || defined _DS1401

Int16 dsmcom_fifo_init ( dsmcom_fifo_header_t ***handle,
                         UInt32 buffer_base, UInt32 target, 
                         UInt16 channel_count, UInt16 buffer_size,
                         UInt16 master_address_increment, 
                         UInt16 slave_address_increment,
                         dpm_write_fcn_t write_fcn, dpm_read_fcn_t  read_fcn );

__INLINE void dsmcom_fifo_error_read (dsmcom_fifo_header_t **handle,
                                      UInt32               *fifo_error);

__INLINE void dsmcom_fifo_error_clear (dsmcom_fifo_header_t **handle,
                                       UInt32               fifo_error);

__INLINE void dsmcom_fifo_write ( dsmcom_fifo_header_t **handle, 
                                  const UInt16         task_id, 
                                  const UInt16         datalen, 
                                  const UInt16         *data);

__INLINE void dsmcom_fifo_read ( dsmcom_fifo_header_t **handle, 
                                  const UInt16        task_id,
                                  const UInt16        datalen,
                                  UInt16              *data);

__INLINE UInt32 dsmcom_fifo_receive_count_get ( dsmcom_fifo_header_t **handle,
                                                const UInt16         task_id );

__INLINE UInt32 dsmcom_fifo_transmit_space_get( dsmcom_fifo_header_t **handle, 
                                                const UInt16         task_id );

#endif /* ifdef _DSXXXX */
#undef __INLINE


/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
#include <dsmcom.c>
#endif


#endif /* __DSMCOM_S_H__ */

