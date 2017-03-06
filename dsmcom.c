/******************************************************************************
*
* MODULE
*   General master-slave communication functions (hardware independent)
*
*   Version with static communication buffers
*   Version for master CPU
*
* FILE
*   dsmcom.c
*
* RELATED FILES
*   dsmcom.h, Comdef.h
*
* DESCRIPTION
*   Functions for command table controlled master-slave communication.
*   - communication initialization
*   - command registration
*   - command execution
*   - return data retrieval
*
* REMARKS
*
* AUTHOR(S)
*   H.-J. Miks, H.-G. Limberg, V. Lang
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsmcom.c $ $Revision: 1.6 $ $Date: 2006/09/01 10:58:01GMT+01:00 $
******************************************************************************/

#include <dstypes.h>                      /* platform independent data types */
#include <dscomdef.h>     /* common definitions for master-slave communication */
#include <dsmcom.h>                      /* master communication functions */
#include <stdlib.h>
#include <math.h>
#include <dsmsg.h>
#include <dsstd.h>
#include <dsprintf.h>
#include <dsts.h>

#if defined (_DS1103)
  #include <dsvcm.h>
	#include <dpm1103.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
  #define SPI_MODULE_NB 1
#elif defined (_DS1104)
  #include <dsvcm.h>
	#include <dpm1104.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
  #define SPI_MODULE_NB 1
#elif defined (_DS1401)
  #define _SW_ARBITRATION
  #include <dpm1401.h>
  #include <ds1401.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
  #define SPI_MODULE_NB DS1401_IMBUS_MODULE_NB
#elif defined (_DS1603)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1005)
  #undef VCM_AVAILABLE
  #define VCM_AVAILABLE
  #include <dsvcm.h>
  #define _SW_ARBITRATION
  #include <dpm1005.h>
  #include <dsphs.h>
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
  #define SPI_MODULE_NB PHS_NUM_SLOTS
#elif defined (_DS1006)
  #undef VCM_AVAILABLE
  #define VCM_AVAILABLE
  #include <dsvcm.h>
  #define _SW_ARBITRATION
  #include <dpm1006.h>
  #include <dsphs.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #define SPI_MODULE_NB PHS_NUM_SLOTS
#elif defined (_DS1003)
  #include <dsvcm.h>
  #define _SW_ARBITRATION
  #include <dpm1003.h>
  #include <dsphs.h>
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
  #define SPI_MODULE_NB PHS_NUM_SLOTS
#else
  #error ERROR: No target defined.
#endif






#if defined (_SW_ARBITRATION)
  static unsigned long compare;  /* for software arbitration */
#endif
/******************************************************************************
* constant and macro definitions
******************************************************************************/

/* first non-reserved command index */

#define DSMCOM_FIRST_INDEX      1

/* increment buffer read pointer */

#define DSMCOM_READ_PTR_INC()                                     \
{                                                                 \
  cmdtbl_p->master_read_ptr = (cmdtbl_p->master_read_ptr + 1) &   \
    (cmdtbl_p->rx_buffer_size - 1);                               \
}

/* increment buffer write pointer */

#define DSMCOM_WRITE_PTR_INC()                                    \
{                                                                 \
  cmdtbl_p->master_write_ptr = (cmdtbl_p->master_write_ptr + 1) & \
    (cmdtbl_p->tx_buffer_size - 1);                               \
}

/* increment FIFO buffer read pointer */
#define DSMCOM_FIFO_READ_PTR_INC()                                \
{                                                             \
  fifo_p->master_read_ptr = (fifo_p->master_read_ptr + 1) &   \
    (fifo_p->rx_buffer_size - 1);                             \
}

/* increment FIFO buffer write pointer */
#define DSMCOM_FIFO_WRITE_PTR_INC()                               \
{                                                             \
  fifo_p->master_write_ptr = (fifo_p->master_write_ptr + 1) & \
    (fifo_p->tx_buffer_size - 1);                             \
}

/******************************************************************************
* function prototypes
******************************************************************************/

/* local functions */

static int dsmcom_cmdtbl_write (dsmcom_cmdtbl_header_t *handle, int *index,
  unsigned int data_in_count, unsigned data_out_count);

static int dsmcom_data_update (dsmcom_cmdtbl_header_t *handle);

/******************************************************************************
* program global variables
******************************************************************************/

/******************************************************************************
  version information
******************************************************************************/

#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 || defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401

DS_VERSION_SYMBOL(_dsmcom_ver_mar, DSMCOM_VER_MAR);
DS_VERSION_SYMBOL(_dsmcom_ver_mir, DSMCOM_VER_MIR);
DS_VERSION_SYMBOL(_dsmcom_ver_mai, DSMCOM_VER_MAI);
DS_VERSION_SYMBOL(_dsmcom_ver_spb, DSMCOM_VER_SPB);
DS_VERSION_SYMBOL(_dsmcom_ver_spn, DSMCOM_VER_SPN);
DS_VERSION_SYMBOL(_dsmcom_ver_plv, DSMCOM_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    dsmcom_vcm_register(void)
*
*  DESCRIPTION
*    Registers the dsmcom module in VCM.
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/
#if defined VCM_AVAILABLE
#ifndef _INLINE
void dsmcom_vcm_register(void)
{
  vcm_module_descriptor_type *dsmcom_version_ptr;


  dsmcom_version_ptr = vcm_module_register(VCM_MID_DSMCOM,
                                        	 vcm_module_find(VCM_MID_RTLIB, NULL),
                                           VCM_TXT_DSMCOM,
                                           DSMCOM_VER_MAR, DSMCOM_VER_MIR, DSMCOM_VER_MAI,
                                           DSMCOM_VER_SPB, DSMCOM_VER_SPN, DSMCOM_VER_PLV,
                                           0, 0);

  vcm_module_status_set(dsmcom_version_ptr,VCM_STATUS_INITIALIZED);
}
#endif
#endif


/******************************************************************************
*
* FUNCTION
*   Basic initialization of master-slave communication
*
* SYNTAX
* int dsmcom_init (dsmcom_cmdtbl_header_t ***handle,
*                  unsigned long buffer_base, long target,
*                  unsigned int channel_count, unsigned int buffer_size,
*                  unsigned int master_address_increment,
*                  unsigned int slave_address_increment,
*                  dpm_write_fcn_t write_fcn, dpm_read_fcn_t  read_fcn)
*
* DESCRIPTION
*   The command table and communication buffers are initialized.
*
* PARAMETERS
*   handle                       communication handle for a certain slave CPU
*   buffer_base                  base address of communication memory
*   target                       describes the access to the target memory
*   channel_count                count of communication channels
*   buffer_size                  size of the transmit and receive buffers
*                  amount the address is incremented to access the next dpm word
*   master_address_increment     - form the master side
*   slave_address_increment      - from the slave side
*   write_fcn                    write function for dpm access
*   read_fcn                     read function for dpm access
*
* RETURNS
*   errorcode
*
* REMARKS
*   Slave CPU acknowledge must be checked by calling routine. Slave CPU must
*   perform it's initialization after this function has been executed on the
*   master CPU.
*
******************************************************************************/

#ifndef _INLINE

int dsmcom_init (dsmcom_cmdtbl_header_t ***handle,
  unsigned long buffer_base, long target, unsigned int channel_count, unsigned int buffer_size,
  unsigned int master_address_increment, unsigned int slave_address_increment,
  dpm_write_fcn_t write_fcn, dpm_read_fcn_t  read_fcn)
{
  dsmcom_cmdtbl_header_t *cmdtbl_p;
  unsigned int offs;
  unsigned int i;

  /* allocate and initialize task list */

  *handle = (dsmcom_cmdtbl_header_t **) malloc(channel_count * sizeof(dsmcom_cmdtbl_header_t *));
  if (*handle == NULL)
  {
    return(DSMCOM_ALLOC_ERROR);
  }

  for (i = 0; i < channel_count; i++)
  {
    /* allocate master communication data structure */

    cmdtbl_p = (dsmcom_cmdtbl_header_t *) malloc(sizeof(dsmcom_cmdtbl_header_t));
    if (cmdtbl_p == NULL)
    {
      return(DSMCOM_ALLOC_ERROR);
    }

    /* initialize master communication data structure */
    cmdtbl_p->dpm_mask = 0xFFFFFFFF;

    cmdtbl_p->target = target;
    cmdtbl_p->write_fcn      = write_fcn;
    cmdtbl_p->read_fcn       = read_fcn;
    cmdtbl_p->channel_count = channel_count;

    cmdtbl_p->tx_buffer_base = buffer_base +
      master_address_increment * buffer_size * i;
    cmdtbl_p->tx_buffer_size = buffer_size;
    cmdtbl_p->rx_buffer_base = buffer_base +
      master_address_increment * buffer_size * (channel_count + i);
    cmdtbl_p->rx_buffer_size = buffer_size;
    cmdtbl_p->master_read_ptr_addr = buffer_base +
      master_address_increment * (channel_count * buffer_size * 2 + i);
    cmdtbl_p->slave_read_ptr_addr = buffer_base +
      master_address_increment * (channel_count * (buffer_size * 2 + 1) + i);
    cmdtbl_p->slave_error_addr = buffer_base +
      master_address_increment * (channel_count * (buffer_size * 2 + 2) + i);
    cmdtbl_p->slave_address_increment = slave_address_increment;
    cmdtbl_p->master_write_ptr = 0;
    cmdtbl_p->master_read_ptr = 0;
    cmdtbl_p->cmdtbl_entry_count = DSMCOM_FIRST_INDEX;
    cmdtbl_p->cmdtbl_size = DSCOMDEF_CMDTBL_INC;
    cmdtbl_p->cmdtbl_entry = (dsmcom_cmdtbl_t **)
      malloc(cmdtbl_p->cmdtbl_size * sizeof(dsmcom_cmdtbl_t *));
    if (cmdtbl_p->cmdtbl_entry == NULL)
    {
      return(DSMCOM_ALLOC_ERROR);
    }

    /* clear transmit buffer */

    for (offs = 0; offs < buffer_size; offs++)
    {
      DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, offs,
        DSCOMDEF_NO_COMMAND);
    }

    /* clear receive buffer */

    for (offs = 0; offs < buffer_size; offs++)
    {
      DPM_WRITE(cmdtbl_p, cmdtbl_p->rx_buffer_base, offs,
        DSCOMDEF_NO_COMMAND);
    }

    /* initialize master read pointer in communication memory */

    DPM_WRITE(cmdtbl_p,cmdtbl_p->master_read_ptr_addr, 0, cmdtbl_p->master_read_ptr);

    /* clear slave error flag in communication memory */

    DPM_WRITE(cmdtbl_p,cmdtbl_p->slave_error_addr, 0, 0);

    /* enter communication data structure pointer in task list */

    (*handle)[i] = cmdtbl_p;
  }
  return(DSMCOM_NO_ERROR);
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   Transmit buffer overflow check
*
* SYNTAX
*   unsigned long dsmcom_buffer_space (dsmcom_cmdtbl_header_t *cmdtbl_p)
*
* DESCRIPTION
*   Evaluates available space in transmit buffer.
*
* PARAMETERS
*   cmdtbl_p            pointer to communication data structure
*
* RETURNS
*   available buffer space (in buffer units)
*
* REMARKS
*
******************************************************************************/

__INLINE unsigned long dsmcom_buffer_space (dsmcom_cmdtbl_header_t *cmdtbl_p)
{
  unsigned long read_ptr;

  /* read slave read pointer from communication memory */
  read_ptr = DPM_READ(cmdtbl_p,cmdtbl_p->slave_read_ptr_addr, 0);
#if defined (_SW_ARBITRATION)
  do   /* software arbitration */
  {
    compare = read_ptr;
    read_ptr = DPM_READ(cmdtbl_p,cmdtbl_p->slave_read_ptr_addr, 0);
  } while(compare - read_ptr);
#endif
  /* calculate available buffer space */

  return((read_ptr / cmdtbl_p->slave_address_increment - cmdtbl_p->master_write_ptr - 1) &
    (cmdtbl_p->tx_buffer_size - 1));
}


/******************************************************************************
*
* FUNCTION
*   Write entry to master command table
*
* SYNTAX
*   int dsmcom_cmdtbl_write (dsmcom_cmdtbl_header_t *cmdtbl_p, int *index,
*     unsigned int data_in_count, unsigned int data_out_count)
*
* DESCRIPTION
*   An entry is added to or replaced in the master command table.
*
* PARAMETERS
*   cmdtbl_p            pointer to communication data structure
*   index               command table index
*   data_in_count       number of data words returned by slave function
*   data_out_count      number of data words to be send to the slave
*
* RETURNS
*   errorcode
*
* REMARKS
*   A new table entry is created if index = DSCOMDEF_AUTO_INDEX, an existing
*   entry is replaced otherwise.
*
******************************************************************************/

__INLINE int dsmcom_cmdtbl_write (dsmcom_cmdtbl_header_t *cmdtbl_p, int *index,
  unsigned int data_in_count, unsigned int data_out_count)
{
  dsmcom_cmdtbl_t **entry_ptr;
  unsigned long *data_in_ptr;
  unsigned int i;
  unsigned long int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  if (*index == DSCOMDEF_AUTO_INDEX)
  {
    /* create new command table entry */

    if (cmdtbl_p->cmdtbl_entry_count >= cmdtbl_p->cmdtbl_size)
    {
      /* increase command table */

      cmdtbl_p->cmdtbl_size += DSCOMDEF_CMDTBL_INC;
      entry_ptr = (dsmcom_cmdtbl_t **) realloc(cmdtbl_p->cmdtbl_entry,
        cmdtbl_p->cmdtbl_size * sizeof(dsmcom_cmdtbl_t *));
      if (entry_ptr == NULL)
      {
        RTLIB_INT_RESTORE(int_status);
        return(DSMCOM_ALLOC_ERROR);
      }
      cmdtbl_p->cmdtbl_entry = entry_ptr;
    }

    /* allocate new command table entry structure */

    cmdtbl_p->cmdtbl_entry[cmdtbl_p->cmdtbl_entry_count] =
      (dsmcom_cmdtbl_t *) malloc(sizeof(dsmcom_cmdtbl_t));
    if (cmdtbl_p->cmdtbl_entry[cmdtbl_p->cmdtbl_entry_count] == NULL)
    {
      RTLIB_INT_RESTORE(int_status);
      return(DSMCOM_ALLOC_ERROR);
    }

    /* allocate temporary data receive buffer */

    if (data_in_count > 0)
    {
      data_in_ptr = (unsigned long *) malloc(sizeof(unsigned long) *
        data_in_count);
      if (data_in_ptr == NULL)
      {
        RTLIB_INT_RESTORE(int_status);
        return(DSMCOM_ALLOC_ERROR);
      }
      cmdtbl_p->cmdtbl_entry[cmdtbl_p->cmdtbl_entry_count]->data_in = data_in_ptr;
    }

    *index = cmdtbl_p->cmdtbl_entry_count;
    (cmdtbl_p->cmdtbl_entry_count)++;
  }
  else
  {
    /* overwrite existing command table index */

    if (*index >= (int)cmdtbl_p->cmdtbl_entry_count)
    {
      RTLIB_INT_RESTORE(int_status);
      return(DSMCOM_ILLEGAL_INDEX);
    }

    /* adjust temporary data receive buffer */

    if (data_in_count > 0)
    {
      data_in_ptr = (unsigned long *)
        realloc(cmdtbl_p->cmdtbl_entry[*index]->data_in,
        sizeof(unsigned long) * data_in_count);
      if (data_in_ptr == NULL)
      {
        RTLIB_INT_RESTORE(int_status);
        return(DSMCOM_ALLOC_ERROR);
      }
      cmdtbl_p->cmdtbl_entry[*index]->data_in = data_in_ptr;
    }
  }

  cmdtbl_p->cmdtbl_entry[*index]->rx_count = 0;
  cmdtbl_p->cmdtbl_entry[*index]->data_in_count = data_in_count;
  cmdtbl_p->cmdtbl_entry[*index]->data_out_count = data_out_count;
  for (i = 0; i < data_in_count; i++)
  {
    cmdtbl_p->cmdtbl_entry[*index]->data_in[i] = 0;
  }
  RTLIB_INT_RESTORE(int_status);
  return(DSMCOM_NO_ERROR);
}


/******************************************************************************
*
* FUNCTION
*   Send command to slave
*
* SYNTAX
*   int dsmcom_send (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
*                    unsigned long cmd, unsigned int data_count,unsigned long *data)
*
* DESCRIPTION
*   A command and corresponding data is written into the send buffer for
*   direct execution.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*   cmd                 command OP-code
*   data_count          number of following data words
*   data                data to be send
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/

__INLINE int dsmcom_send (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  unsigned long cmd, unsigned int data_count, unsigned long *data)
{
  dsmcom_cmdtbl_header_t *cmdtbl_p;
  unsigned long int_status;                           /* interrupt status */
  unsigned long command;
  unsigned int cmd_offs;
  unsigned int i;

  cmdtbl_p = handle[task_id];

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check for sufficient buffer space */

  if (dsmcom_buffer_space(cmdtbl_p) < data_count + 2)
  {
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_BUFFER_OVERFLOW);
  }

  /* save current buffer position where to write command later */

  cmd_offs = cmdtbl_p->master_write_ptr;

  /* write data to transmit buffer */

  for (i = 0; i < data_count; i++)
  {
    DSMCOM_WRITE_PTR_INC();
    DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmdtbl_p->master_write_ptr, *data++);
  }

  /* write trailing NO_COMMAND to transmit buffer */

  DSMCOM_WRITE_PTR_INC();
  DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmdtbl_p->master_write_ptr, DSCOMDEF_NO_COMMAND);

  /* write command to transmit buffer */

  command = (DSCOMDEF_MODE_DIRECT << (DSCOMDEF_CMD_WIDTH-1) ) | cmd;

  DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmd_offs, (unsigned long) command);

  RTLIB_INT_RESTORE(int_status);

  return(DSMCOM_NO_ERROR);
}


/******************************************************************************
*
* FUNCTION
*   Register a command for indirect execution
*
* SYNTAX
*   int dsmcom_register (dsmcom_cmdtbl_header_t **handle,
*                        unsigned int task_id, int *index, unsigned long cmd,
*                        unsigned int data_in_count, unsigned int data_out_count,
*                        unsigned int parm_count, unsigned long *parms)
*
* DESCRIPTION
*   A command entry is created in the master command table. A command is send
*   to the slave to create the corresponding entry in the slave command table.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*   index               command table index
*   cmd                 command OP-code
*   data_in_count       number of data words returned by slave function
*   data_out_count      number of data words to be send to the slave
*   parm_count          number of parameters to be send to the slave
*   parms               parameters
*
* RETURNS
*   errorcode
*
* REMARKS
*   A new table entry is created if index = DSCOMDEF_AUTO_INDEX, an existing
*   entry is replaced otherwise.
*
******************************************************************************/

#ifndef _INLINE

int dsmcom_register (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  int *index, unsigned long cmd, unsigned int data_in_count, unsigned int data_out_count,
  unsigned int parm_count, unsigned long *parms)
{
  dsmcom_cmdtbl_header_t *cmdtbl_p;
  unsigned long int_status;                           /* interrupt status */
  unsigned long *tmp;
  int i, error;

  cmdtbl_p = handle[task_id];
  if (task_id >= handle[task_id]->channel_count)
  {
    return(DSMCOM_ILLEGAL_TASK_ID);
  }

  /* create entry in master command table */

  error = dsmcom_cmdtbl_write(cmdtbl_p, (int*)index, data_in_count, data_out_count);
  if (error != DSMCOM_NO_ERROR)
  {
    return(error);
  }

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* request creation of corresponding entry in slave command table */

  tmp = (unsigned long *) malloc(sizeof(unsigned long) *
    (DSCOMDEF_CMD_REG_PCNT + parm_count));
  if (tmp == NULL)
  {
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_ALLOC_ERROR);
  }
  tmp[DSCOMDEF_CMD_REG_IDX_LOC] = (unsigned long) *index;
  tmp[DSCOMDEF_CMD_REG_CMD_LOC] = (unsigned long) cmd;
  tmp[DSCOMDEF_CMD_REG_PCNT_LOC] = (unsigned long) parm_count;
  for (i = 0; i < parm_count; i++)
  {
    tmp[DSCOMDEF_CMD_REG_PARM_LOC+i] = parms[i];
  }



  error = dsmcom_send (handle, task_id, DSCOMDEF_CMD_REG,
    DSCOMDEF_CMD_REG_PCNT + parm_count, tmp);

  free(tmp);

  if (error != DSMCOM_NO_ERROR)
  {
    RTLIB_INT_RESTORE(int_status);
    return(error);
  }

  /* check for slave DSP initialization acknowledge */

  error = dsmcom_slave_acknowledge_check(handle, task_id);

  RTLIB_INT_RESTORE(int_status);

  if (error != DSMCOM_NO_ERROR)
  {
    return(error);
  }

  return(DSMCOM_NO_ERROR);
}

#endif /* _INLINE */


/******************************************************************************
*
* FUNCTION
*   Request execution of slave command from command table
*
* SYNTAX
*   int dsmcom_execute (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
*     int index)
*
* DESCRIPTION
*   An index is send to the slave to request execution of the corresponding
*   command from the command table.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*   index               command table index
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/

__INLINE int dsmcom_execute (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  int index)
{
  dsmcom_cmdtbl_header_t *cmdtbl_p;
  unsigned long int_status;                           /* interrupt status */
  unsigned int cmd_offs;

  cmdtbl_p = handle[task_id];

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check for sufficient buffer space */

  if (dsmcom_buffer_space(cmdtbl_p) < 2)
  {
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_BUFFER_OVERFLOW);
  }

  /* write index with trailing NO_COMMAND to transmit buffer */

  cmd_offs = cmdtbl_p->master_write_ptr;
  DSMCOM_WRITE_PTR_INC();
  DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmdtbl_p->master_write_ptr, DSCOMDEF_NO_COMMAND);
  DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmd_offs, (unsigned long) index);

  RTLIB_INT_RESTORE(int_status);

  return(DSMCOM_NO_ERROR);
}


/******************************************************************************
*
* FUNCTION
*   Request execution of slave command from command table with output data
*
* SYNTAX
*   int dsmcom_execute_data (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
*     int index, unsigned long *data)
*
* DESCRIPTION
*   An index is send to the slave to request execution of the corresponding
*   command from the command table. Output data is send to the slave.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*   index               command table index
*   data                data to be send to the slave
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/

__INLINE int dsmcom_execute_data (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id, int index, unsigned long *data)
{
  dsmcom_cmdtbl_header_t *cmdtbl_p;
  unsigned long int_status;                           /* interrupt status */
  unsigned long command;
  unsigned int cmd_offs;
  unsigned int i;

  cmdtbl_p = handle[task_id];

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check for sufficient buffer space */

  if (dsmcom_buffer_space(cmdtbl_p) <
    cmdtbl_p->cmdtbl_entry[index]->data_out_count + 2)
  {
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_BUFFER_OVERFLOW);
  }

  /* save current buffer position where to write command later */

  cmd_offs = cmdtbl_p->master_write_ptr;

  /* write data to transmit buffer */

  for (i = 0; i < cmdtbl_p->cmdtbl_entry[index]->data_out_count; i++)
  {
    DSMCOM_WRITE_PTR_INC();
    DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmdtbl_p->master_write_ptr, *data++);
  }

  /* write trailing NO_COMMAND to transmit buffer */

  DSMCOM_WRITE_PTR_INC();
  DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmdtbl_p->master_write_ptr, DSCOMDEF_NO_COMMAND);

  /* write command to transmit buffer */

  command = (DSCOMDEF_MODE_INDIRECT << (DSCOMDEF_CMD_WIDTH-1) ) | index;
  DPM_WRITE(cmdtbl_p,cmdtbl_p->tx_buffer_base, cmd_offs, (unsigned long) command);

  RTLIB_INT_RESTORE(int_status);

  return(DSMCOM_NO_ERROR);
}


/******************************************************************************
*
* FUNCTION
*   Update return data in command table
*
* SYNTAX
*   int dsmcom_data_update (dsmcom_cmdtbl_header_t *cmdtbl_p)
*
* DESCRIPTION
*   The receive buffer is checked for new data which is copied into the
*   command table, if present.
*
* PARAMETERS
*   cmdtbl_p            pointer to communication data structure
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

__INLINE int dsmcom_data_update (dsmcom_cmdtbl_header_t *cmdtbl_p)
{
  unsigned long int_status;                           /* interrupt status */
  unsigned long command, index;
  unsigned int data_count;
  unsigned int i;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check data receive buffer for new data */

  while ((command = (unsigned long) DPM_READ(cmdtbl_p, cmdtbl_p->rx_buffer_base,
    cmdtbl_p->master_read_ptr)) != DSCOMDEF_NO_COMMAND)
  {
#if defined (_SW_ARBITRATION)
    command = (unsigned long) DPM_READ(cmdtbl_p, cmdtbl_p->rx_buffer_base, cmdtbl_p->master_read_ptr);
#endif
    /* get index from command (blank out the mode bit) */
    index = command & ~( 1 << (DSCOMDEF_CMD_WIDTH-1) );


    if (index > cmdtbl_p->cmdtbl_entry_count)
    {
      RTLIB_INT_RESTORE(int_status);
      return(DSMCOM_WRONG_INDEX);
    }

    data_count = cmdtbl_p->cmdtbl_entry[index]->data_in_count;

    /* copy data from data receive buffer into temporary buffer */

    for (i = 0; i < data_count; i++)
    {
      DSMCOM_READ_PTR_INC();
      cmdtbl_p->cmdtbl_entry[index]->data_in[i] = DPM_READ(cmdtbl_p,
        cmdtbl_p->rx_buffer_base, cmdtbl_p->master_read_ptr );
    }
    DSMCOM_READ_PTR_INC();

    /* increment data receive counter */

    (cmdtbl_p->cmdtbl_entry[index]->rx_count)++;

    /* copy read pointer to communication memory */

    DPM_WRITE(cmdtbl_p, cmdtbl_p->master_read_ptr_addr, 0,(unsigned long) cmdtbl_p->master_read_ptr);
  }
  RTLIB_INT_RESTORE(int_status);
  return(DSMCOM_NO_ERROR);
}


/******************************************************************************
*
* FUNCTION
*   Read return data from command table
*
* SYNTAX
*   int dsmcom_data_read (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
*     int index, unsigned long *data)
*
* DESCRIPTION
*   Return data is read from the temporary data buffer, if available.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*   index               index in command table
*   data                data buffer
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/

__INLINE int dsmcom_data_read (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  int index, unsigned long *data)
{
  dsmcom_cmdtbl_header_t *cmdtbl_p;
  unsigned int i;
  int error;

   cmdtbl_p = handle[task_id];

  /* copy all data from data receive buffer into temporary buffers */

  if ((error = dsmcom_data_update(cmdtbl_p)) != DSMCOM_NO_ERROR)
    return(error);

  /* copy data from temporary buffer to destination address */

  for (i = 0; i < cmdtbl_p->cmdtbl_entry[index]->data_in_count; i++)
  {
    data[i] = cmdtbl_p->cmdtbl_entry[index]->data_in[i];
  }

  /* check data receive counter */

  if (cmdtbl_p->cmdtbl_entry[index]->rx_count == 0)
  {
    return(DSMCOM_NO_DATA);
  }
  else if (cmdtbl_p->cmdtbl_entry[index]->rx_count == 1)
  {
    cmdtbl_p->cmdtbl_entry[index]->rx_count = 0;
    return(DSMCOM_NO_ERROR);
  }
  else
  {
    cmdtbl_p->cmdtbl_entry[index]->rx_count = 0;
    return(DSMCOM_DATA_LOST);
  }
}


/******************************************************************************
*
* FUNCTION
*   Read slave error status
*
* SYNTAX
*   void dsmcom_slave_error_read (dsmcom_cmdtbl_header_t **handle,
*     unsigned int task_id, unsigned long *slave_error)
*
* DESCRIPTION
*   The slave error status for the specified communication channel is read
*   from the communication memory.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*   slave_error         slave error number
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

__INLINE void dsmcom_slave_error_read (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id, unsigned long *slave_error)
{
  unsigned long error;
  unsigned long int_status;
  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  error = DPM_READ(handle[task_id], handle[task_id]->slave_error_addr, 0);
#if defined (_SW_ARBITRATION)
  do   /* software arbitration */
  {
    compare = error;
    error = DPM_READ(handle[task_id], handle[task_id]->slave_error_addr, 0);
  } while(compare - error);
#endif
  switch (error)
  {
    case DSCOMDEF_NO_ERROR        : *slave_error = DSMCOM_NO_ERROR;
                                  break;
    case DSCOMDEF_ALLOC_ERROR     : *slave_error = DSMCOM_SLV_ALLOC_ERROR;
                                  break;
    case DSCOMDEF_BUFFER_OVERFLOW : *slave_error = DSMCOM_SLV_BUFFER_OVERFLOW;
                                  break;
    case DSCOMDEF_INIT_ACK        : *slave_error = DSMCOM_SLV_INIT_ACK;
                                  break;
    case DSCOMDEF_ILLEGAL_USR_IDX : *slave_error = DSMCOM_SLV_ILLEGAL_USR_IDX;
                                  break;
    default                       : *slave_error = DSMCOM_SLV_UNDEFINED;
                                  break;
  }
  RTLIB_INT_RESTORE(int_status);
}


/******************************************************************************
*
* FUNCTION
*   Clear slave error status
*
* SYNTAX
*   void dsmcom_slave_error_clear (dsmcom_cmdtbl_header_t **handle,
*     unsigned int task_id)
*
* DESCRIPTION
*   The slave error status for the specified communication channel is set to
*   0 in the communication memory.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/

__INLINE void dsmcom_slave_error_clear (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id)
{
  unsigned long int_status;
  RTLIB_INT_SAVE_AND_DISABLE(int_status);
  DPM_WRITE(handle[task_id],handle[task_id]->slave_error_addr, 0, 0);
  RTLIB_INT_RESTORE(int_status);
}

/******************************************************************************
*
* FUNCTION
*   Slave DSP acknowledge check
*
* SYNTAX
*   int dsmcom_slave_acknowledge_check (dsmcom_cmdtbl_header_t **handle,
*     unsigned int task_id)
*
* DESCRIPTION
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/

__INLINE int dsmcom_slave_acknowledge_check (dsmcom_cmdtbl_header_t **handle,
  unsigned int task_id)
{
  dsfloat time;
  ts_timestamp_type start_timer, end_timer;
  int error;

  ts_timestamp_read(&start_timer);
  do
  {
    error = (int) DPM_READ(handle[task_id], handle[task_id]->slave_error_addr, 0);
#if defined (_SW_ARBITRATION)
    do   /* software arbitration */
    {
      compare = error;
      error = DPM_READ(handle[task_id], handle[task_id]->slave_error_addr, 0);
    } while(compare - error);
#endif
    if (error == DSCOMDEF_ALLOC_ERROR)
    {
      return(DSMCOM_SLV_ALLOC_ERROR);
    }
    if (error == DSCOMDEF_BUFFER_OVERFLOW)
    {
      return(DSMCOM_SLV_BUFFER_OVERFLOW);
    }

    ts_timestamp_read(&end_timer);
    time = ts_timestamp_interval(&start_timer, &end_timer);
    if (time > DSCOMDEF_TIMEOUT)
    {
      return(DSMCOM_TIMEOUT);
    }
  }
  while (error != DSCOMDEF_INIT_ACK);

  /* clear slave error flag in communication memory */

  DPM_WRITE(handle[task_id], handle[task_id]->slave_error_addr, 0, 0);

  return(DSMCOM_NO_ERROR);
}

/******************************************************************************
*
* FUNCTION
*   Send command to slave
*
* SYNTAX
*   Int16 dsmcom_var_send (Int16 task_id, UInt16 cmd, UInt16 data_count,
*     comdef_data_t *data)
*
* DESCRIPTION
*   A command, the number of data and corresponding data is written into the
*   send buffer for direct execution.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*   cmd                 command OP-code
*   data_count          number of following data words
*   data                data to be send
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE
int dsmcom_var_send (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  unsigned long cmd, unsigned int data_count, unsigned long *data)
{
  dsmcom_cmdtbl_header_t *cmdtbl_p;
  unsigned long int_status;                           /* interrupt status */
  unsigned long command;
  unsigned int cmd_offs;
  unsigned int i;

  cmdtbl_p = handle[task_id];

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check for sufficient buffer space */

  if (dsmcom_buffer_space(cmdtbl_p) < data_count + 3)
  {
    RTLIB_INT_RESTORE(int_status);
    return(DSMCOM_BUFFER_OVERFLOW);
  }

  /* save current buffer position where to write command later */

  cmd_offs = cmdtbl_p->master_write_ptr;

  /* write data to transmit buffer */


    DSMCOM_WRITE_PTR_INC();
    DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmdtbl_p->master_write_ptr, data_count);


  for (i = 0; i < data_count; i++)
  {
    DSMCOM_WRITE_PTR_INC();
    DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmdtbl_p->master_write_ptr, *data++);
  }

  /* write trailing NO_COMMAND to transmit buffer */

  DSMCOM_WRITE_PTR_INC();
  DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmdtbl_p->master_write_ptr, DSCOMDEF_NO_COMMAND);

  /* write command to transmit buffer */
  command = (DSCOMDEF_MODE_DIRECT << (DSCOMDEF_CMD_WIDTH-1) ) | cmd;

  DPM_WRITE(cmdtbl_p, cmdtbl_p->tx_buffer_base, cmd_offs, command);

  RTLIB_INT_RESTORE(int_status);

  return(DSMCOM_NO_ERROR);
}

#endif
/******************************************************************************
*
* FUNCTION
*   Update return data in command table
*
* SYNTAX
*   int dsmcom_vardata_update (dsmcom_mc_cmdtbl_header_t *cmdtbl_p)
*
* DESCRIPTION
*   The receive buffer is checked for new data of variable number which
*   is copied into the command table, if present.
*
* PARAMETERS
*   cmdtbl_p            pointer to communication data structure
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE
int dsmcom_vardata_update (dsmcom_cmdtbl_header_t *cmdtbl_p)
{
  unsigned long int_status;                           /* interrupt status */
  unsigned long command, index;
  unsigned int  data_count;
  unsigned int i;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check data receive buffer for new data */
 while ((command = (unsigned long)DPM_READ(cmdtbl_p, cmdtbl_p->rx_buffer_base,
    cmdtbl_p->master_read_ptr )) != DSCOMDEF_NO_COMMAND)
  {
#if defined (_SW_ARBITRATION)
    command = (unsigned long)DPM_READ(cmdtbl_p, cmdtbl_p->rx_buffer_base, cmdtbl_p->master_read_ptr );
#endif
    index = command & ~( 1 << (DSCOMDEF_CMD_WIDTH-1) );

    if (index > cmdtbl_p->cmdtbl_entry_count)
    {
      RTLIB_INT_RESTORE(int_status);
      return(DSMCOM_WRONG_INDEX);
    }

    DSMCOM_READ_PTR_INC();
    data_count = DPM_READ(cmdtbl_p, cmdtbl_p->rx_buffer_base,
                 cmdtbl_p->master_read_ptr );


    /* copy data from data receive buffer into temporary buffer */

    for (i = 0; i < data_count; i++)
    {
      DSMCOM_READ_PTR_INC();
      cmdtbl_p->cmdtbl_entry[index]->data_in[i] = DPM_READ(cmdtbl_p,
        cmdtbl_p->rx_buffer_base, cmdtbl_p->master_read_ptr );

    }
    DSMCOM_READ_PTR_INC();

    /* increment data receive counter */
    (cmdtbl_p->cmdtbl_entry[index]->rx_count)++;

    /* copy read pointer to communication memory */
    DPM_WRITE(cmdtbl_p, cmdtbl_p->master_read_ptr_addr, 0,
      cmdtbl_p->master_read_ptr);

  }
  RTLIB_INT_RESTORE(int_status);
  return(DSMCOM_NO_ERROR);
}
#endif

/******************************************************************************
*
* FUNCTION
*   Read return data from command table
*
* SYNTAX
*   Int16 dsmcom_data_read_varlen (dsmcom_mc_cmdtbl_header_t **handle, Int16 task_id,
*     Int16 index, comdef_data_t *data)
*
* DESCRIPTION
*   Return data of variable length is read from the temporary data buffer, if available.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of communication channel
*   index               index in command table
*   data                data buffer
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE
int dsmcom_data_read_varlen (dsmcom_cmdtbl_header_t **handle, unsigned int task_id,
  int index, unsigned long *data)
{
  dsmcom_cmdtbl_header_t *cmdtbl_p;
  unsigned int i;
  int error;

   cmdtbl_p = handle[task_id];

  /* copy all data from data receive buffer into temporary buffers */

  if ((error = dsmcom_vardata_update(cmdtbl_p)) != DSMCOM_NO_ERROR)
    return(error);

  /* copy data from temporary buffer to destination address */

  for (i = 0; i < cmdtbl_p->cmdtbl_entry[index]->data_in_count; i++)
  {
    data[i] = cmdtbl_p->cmdtbl_entry[index]->data_in[i];
  }

  /* check data receive counter */

  if (cmdtbl_p->cmdtbl_entry[index]->rx_count == 0)
  {
    return(DSMCOM_NO_DATA);
  }
  else if (cmdtbl_p->cmdtbl_entry[index]->rx_count == 1)
  {
    cmdtbl_p->cmdtbl_entry[index]->rx_count = 0;
    return(DSMCOM_NO_ERROR);
  }
  else
  {
    cmdtbl_p->cmdtbl_entry[index]->rx_count = 0;
    return(DSMCOM_DATA_LOST);
  }
}

#endif

#ifndef _INLINE

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    dsmcom_get_entry_varlen (dsmcom_cmdtbl_header_t **handle,
*                             unsigned int task_id,
*                             unsigned int datalen,
*                             unsigned long* data_in,
*                             int* index)
*
*  DESCRIPTION
*    The function can be used to read out data from only one DPM queue
*    (specified by task_id) directly into a user buffer bypassing the
*    default command table of the dsmcom module (it is not touched by this
*    function).
*
*  PARAMETERS
*    handle  : communication handle for a certain slave CPU
*    task_id : task ID of communication channel
*    datalen : maximum count of words
*    data_in : location where the data will be stored
*    index   : index of the command
*
*  RETURNS
*    DSMCOM_NO_ERROR        no error
*    DSMCOM_WRONG_DATALEN   the data count in DPM is greater  than datalen
*
*  REMARKS
*
*******************************************************************************/

int dsmcom_get_entry_varlen (dsmcom_cmdtbl_header_t **handle,
                                    unsigned int task_id,
                                    unsigned int datalen,
                                    unsigned long* data_in,
                                    int* index)
{
  unsigned long int_status;                           /* interrupt status */
  unsigned int  data_count;
  unsigned int i;
  int retval;
  unsigned long command;
  dsmcom_cmdtbl_header_t *cmdtbl_p;

  cmdtbl_p = handle[task_id];

  RTLIB_INT_SAVE_AND_DISABLE( int_status );

  /* check data receive buffer for new data */
  if( (command = DPM_READ(cmdtbl_p, cmdtbl_p->rx_buffer_base, cmdtbl_p->master_read_ptr)) != DSCOMDEF_NO_COMMAND)
  {
    #if defined (_SW_ARBITRATION)
    command = DPM_READ(cmdtbl_p, cmdtbl_p->rx_buffer_base, cmdtbl_p->master_read_ptr );
    #endif

    *index = (int)command & ~( 1 << (DSCOMDEF_CMD_WIDTH-1) );

    if( *index > cmdtbl_p->cmdtbl_entry_count )
    {
      RTLIB_INT_RESTORE( int_status );
      return( DSMCOM_WRONG_INDEX );
    }

    DSMCOM_READ_PTR_INC();

    data_count = DPM_READ( cmdtbl_p, cmdtbl_p->rx_buffer_base, cmdtbl_p->master_read_ptr );

    /* data_count range check */
    if( data_count > datalen )
    {
      RTLIB_INT_RESTORE( int_status );
      return( DSMCOM_WRONG_DATALEN );
    }

    /* copy data from data receive buffer into temporary buffer */
    for( i = 0; i < data_count; i++ )
    {
      DSMCOM_READ_PTR_INC();
      data_in[i] = DPM_READ( cmdtbl_p, cmdtbl_p->rx_buffer_base, cmdtbl_p->master_read_ptr );
    }

    DSMCOM_READ_PTR_INC();

    /* copy read pointer to communication memory */
    DPM_WRITE( cmdtbl_p, cmdtbl_p->master_read_ptr_addr, 0,
               cmdtbl_p->master_read_ptr );

    retval = DSMCOM_NO_ERROR;

  }
  else
  {
    retval = DSMCOM_NO_DATA;
  }

  RTLIB_INT_RESTORE( int_status );

  return( retval );
}
#endif


#ifndef _INLINE
/*******************************************************************************
*
*  FUNCTION
*
*  SYNTAX
*    dsmcom_fw_version_print(char *vs_str, UInt16 vs_value)
*
*  DESCRIPTION
*    Prints the slave firmware revision number into a given character buffer
*
*  PARAMETERS
*    vs_str   : character buffer with dimension DSMCOM_FW_STRLEN (10),
*               must be provided by user
*    vs_value : 16 bit slave firmware revision value
*
*  RETURNS
*    DSMCOM_FW_DSPACE (0): dSPACE firmware
*    DSMCOM_FW_USER   (1): user firmware
*
*  REMARKS
*
*******************************************************************************/
int dsmcom_fw_version_print(char *vs_str, UInt16 vs_value)
{
  char spb_str[2] = {'\0', '\0'};

  /* if alpha or beta version, or maintenance nr. of release version != 0 */
  if( vs_value & (DSMCOM_FW_MAINT | DSMCOM_FW_VERSION) )
  {
    switch(vs_value & DSMCOM_FW_VERSION)
    {
      case DSMCOM_FW_BETA  : spb_str[0] = 'b';
                             break;

      case DSMCOM_FW_ALPHA : spb_str[0] = 'a';
                             break;

      default              : spb_str[0] = '\0';
                             break;
    }

    sprintf(vs_str, "%u.%u.%s%u", (UInt16)((vs_value & DSMCOM_FW_MAJOR) >> 11),
                                  (UInt16)((vs_value & DSMCOM_FW_MINOR) >> 7),
                                  spb_str,
                                  (UInt16)((vs_value & DSMCOM_FW_MAINT) >> 3));
  }
  else /* release version, maintenance nr. = 0 */
  {
    sprintf(vs_str, "%u.%u", (UInt16)((vs_value & DSMCOM_FW_MAJOR) >> 11),
                             (UInt16)((vs_value & DSMCOM_FW_MINOR) >> 7));
  }

  if(vs_value & DSMCOM_FW_ORIGIN) /* if dSPACE firmware */
    return DSMCOM_FW_DSPACE;
  else /* user firmware */
    return DSMCOM_FW_USER;
}
#endif


#ifndef _INLINE
/*******************************************************************************
*
*  FUNCTION
*
*  SYNTAX
*    dsmcom_fw_register_vcm(UInt16 vs_value, UInt32 module_id, char *module_txt,
*                           UInt8 req_mar, UInt8 req_mir, UInt8 req_mai,
*                           UInt8 req_spb, UInt8 req_spn, UInt8 req_plv,
*                           UInt32 location, UInt32 burn_count)
*
*  DESCRIPTION
*    This function performs a VCM module entry for a slave firmware
*
*  PARAMETERS
*    vs_value   : 16 bit slave firmware revision value
*    module_id  : VCM module id for firmware entry
*    module_txt : VCM module text for firmware entry
*    req_mar    : major release number of required firmware
*    req_mir    : minor release number of required firmware
*    req_mai    : maintenance release number of required firmware
*    req_spb    : special build of required firmware
*    req_spn    : special build number of required firmware
*    req_plv    : patch level of required firmware
*    location   : firmware location
*    burn_count : firmware burncount
*
*  RETURNS
*    DSMCOM_ALLOC_ERROR: memory allocation error
*    DSMCOM_NO_ERROR   : VCM entry succeeded
*
*  REMARKS
*
*******************************************************************************/
#if defined VCM_AVAILABLE
int dsmcom_fw_register_vcm(UInt16 vs_value, UInt32 module_id, char *module_txt,
                           UInt8 req_mar, UInt8 req_mir, UInt8 req_mai,
                           UInt8 req_spb, UInt8 req_spn, UInt8 req_plv,
                           UInt32 location, UInt32 burn_count)
{
  vcm_version_type            temp_vs;
  vcm_module_descriptor_type  *fw_vcm_module_ptr = NULL;
  vcm_module_descriptor_type  *additional_mem_ptr;
  common_fw_vcm_entry_t*      common_fw_vcm_entry_ptr;


  /* initialize structure for VCM entry */
  switch(vs_value & DSMCOM_FW_VERSION)
  {
    case DSMCOM_FW_BETA  : temp_vs.vs.mar = (vs_value & DSMCOM_FW_MAJOR) >> 11;
                           temp_vs.vs.mir = (vs_value & DSMCOM_FW_MINOR) >> 7;
                           temp_vs.vs.mai = 0;
                           temp_vs.vs.spb = VCM_VERSION_BETA;
                           temp_vs.vs.spn = (vs_value & DSMCOM_FW_MAINT) >> 3;
                           temp_vs.vs.plv = 0;
                           break;

    case DSMCOM_FW_ALPHA : temp_vs.vs.mar = (vs_value & DSMCOM_FW_MAJOR) >> 11;
                           temp_vs.vs.mir = (vs_value & DSMCOM_FW_MINOR) >> 7;
                           temp_vs.vs.mai = 0;
                           temp_vs.vs.spb = VCM_VERSION_ALPHA;
                           temp_vs.vs.spn = (vs_value & DSMCOM_FW_MAINT) >> 3;
                           temp_vs.vs.plv = 0;
                           break;

    default              : temp_vs.vs.mar = (vs_value & DSMCOM_FW_MAJOR) >> 11;
                           temp_vs.vs.mir = (vs_value & DSMCOM_FW_MINOR) >> 7;
                           temp_vs.vs.mai = (vs_value & DSMCOM_FW_MAINT) >> 3;
                           temp_vs.vs.spb = VCM_VERSION_RELEASE;
                           temp_vs.vs.spn = 0;
                           temp_vs.vs.plv = 0;
                           break;
  }


  /* make VCM entry for slave-DSP dSPACE firmware  */
  fw_vcm_module_ptr = vcm_module_register(module_id,
                                          0,
                                          module_txt,
                                          temp_vs.vs.mar,
                                          temp_vs.vs.mir,
                                          temp_vs.vs.mai,
                                          temp_vs.vs.spb,
                                          temp_vs.vs.spn,
                                          0, 0, 0);

  additional_mem_ptr = fw_vcm_module_ptr;

	/* enter required  version number of slave DSP firmware */
  temp_vs.vs.mar = req_mar;
  temp_vs.vs.mir = req_mir;
  temp_vs.vs.mai = req_mai;
  temp_vs.vs.spb = req_spb;
  temp_vs.vs.plv = req_plv;
  temp_vs.vs.spn = req_spn;

  /* the additional memory block of the firmware */
  common_fw_vcm_entry_ptr = (common_fw_vcm_entry_t *) vcm_cfg_malloc(sizeof(common_fw_vcm_entry_t));
  if(common_fw_vcm_entry_ptr == NULL)
    return DSMCOM_ALLOC_ERROR;

  common_fw_vcm_entry_ptr->fw_burn_count           = burn_count;
  common_fw_vcm_entry_ptr->fw_origin               = DSMCOM_FW_ORIGIN;
  common_fw_vcm_entry_ptr->fw_location             = location;
  common_fw_vcm_entry_ptr->req_fw_ver.version.high = temp_vs.version.high;
  common_fw_vcm_entry_ptr->req_fw_ver.version.low  = temp_vs.version.low;

  vcm_memory_ptr_set(additional_mem_ptr, common_fw_vcm_entry_ptr, sizeof(common_fw_vcm_entry_t));

  vcm_module_status_set(fw_vcm_module_ptr, VCM_STATUS_INITIALIZED);

  return DSMCOM_NO_ERROR;
}
#endif
#endif

#ifndef _INLINE
/*******************************************************************************
*
*  FUNCTION
*
*  SYNTAX
*    dsmcom_user_fw_register_vcm(UInt16 vs_value, UInt32 usr_mod_id, char *usr_mod_txt,
*                                UInt8 usr_maj, UInt8 usr_min, UInt8 usr_sub,
*                                UInt8 req_mar, UInt8 req_mir, UInt8 req_mai,
*                                UInt8 req_spb, UInt8 req_spn, UInt8 req_plv,
*                                UInt32 location, UInt32 burn_count)
*
*  DESCRIPTION
*    This function performs a VCM module entry for a user slave firmware
*
*  PARAMETERS
*    vs_value   : 16 bit slave firmware revision value
*    module_id  : VCM module id for firmware entry
*    module_txt : VCM module text for firmware entry
*    usr_maj    : major release number of user firmware
*    usr_maj    : minor release number of user firmware
*    usr_maj    : sub minor release number of user firmware
*    req_mar    : major release number of required firmware
*    req_mir    : minor release number of required firmware
*    req_mai    : maintenance release number of required firmware
*    req_spb    : special build of required firmware
*    req_spn    : special build number of required firmware
*    req_plv    : patch level of required firmware
*    location   : firmware location
*    burn_count : firmware burncount
*
*  RETURNS
*    DSMCOM_ALLOC_ERROR: memory allocation error
*    DSMCOM_NO_ERROR   : VCM entry succeeded
*
*  REMARKS
*
*******************************************************************************/
#if defined VCM_AVAILABLE
int dsmcom_user_fw_register_vcm(UInt16 vs_value, UInt32 usr_mod_id, char *usr_mod_txt,
                                UInt8 usr_maj, UInt8 usr_min, UInt8 usr_sub,
                                UInt8 req_mar, UInt8 req_mir, UInt8 req_mai,
                                UInt8 req_spb, UInt8 req_spn, UInt8 req_plv,
                                UInt32 location, UInt32 burn_count)
{
  vcm_version_type            temp_vs;
  vcm_module_descriptor_type  *fw_vcm_module_ptr = NULL;
  vcm_module_descriptor_type  *user_fw_vcm_module_ptr = NULL;
  vcm_module_descriptor_type  *additional_mem_ptr;
  common_fw_vcm_entry_t*      common_fw_vcm_entry_ptr;

  /* initialize structure for VCM entry */
  switch(vs_value & DSMCOM_FW_VERSION)
  {
    case DSMCOM_FW_BETA  : temp_vs.vs.mar = (vs_value & DSMCOM_FW_MAJOR) >> 11;
                           temp_vs.vs.mir = (vs_value & DSMCOM_FW_MINOR) >> 7;
                           temp_vs.vs.mai = 0;
                           temp_vs.vs.spb = VCM_VERSION_BETA;
                           temp_vs.vs.spn = (vs_value & DSMCOM_FW_MAINT) >> 3;
                           temp_vs.vs.plv = 0;
                           break;

    case DSMCOM_FW_ALPHA : temp_vs.vs.mar = (vs_value & DSMCOM_FW_MAJOR) >> 11;
                           temp_vs.vs.mir = (vs_value & DSMCOM_FW_MINOR) >> 7;
                           temp_vs.vs.mai = 0;
                           temp_vs.vs.spb = VCM_VERSION_ALPHA;
                           temp_vs.vs.spn = (vs_value & DSMCOM_FW_MAINT) >> 3;
                           temp_vs.vs.plv = 0;
                           break;

    default              : temp_vs.vs.mar = (vs_value & DSMCOM_FW_MAJOR) >> 11;
                           temp_vs.vs.mir = (vs_value & DSMCOM_FW_MINOR) >> 7;
                           temp_vs.vs.mai = (vs_value & DSMCOM_FW_MAINT) >> 3;
                           temp_vs.vs.spb = VCM_VERSION_RELEASE;
                           temp_vs.vs.spn = 0;
                           temp_vs.vs.plv = 0;
                           break;
  }

  /* make VCM entry for slave-DSP user firmware  */
  user_fw_vcm_module_ptr = vcm_module_register(usr_mod_id,
                                               0,
                                               usr_mod_txt,
                                               usr_maj,
                                               usr_min,
                                               usr_sub,
                                               0, 0, 0, 0, 0);

  /* make VCM entry for slave-DSP parent of user firmware  */
  fw_vcm_module_ptr = vcm_module_register(VCM_MID_BASED_ON_FW,
                                          user_fw_vcm_module_ptr,
                                          VCM_TXT_BASED_ON_FW,
                                          temp_vs.vs.mar,
                                          temp_vs.vs.mir,
                                          temp_vs.vs.mai,
                                          temp_vs.vs.spb,
                                          temp_vs.vs.spn,
                                          0, 0, 0);

  additional_mem_ptr = user_fw_vcm_module_ptr;

	/* enter required  version number of slave DSP firmware */
  temp_vs.vs.mar = req_mar;
  temp_vs.vs.mir = req_mir;
  temp_vs.vs.mai = req_mai;
  temp_vs.vs.spb = req_spb;
  temp_vs.vs.plv = req_plv;
  temp_vs.vs.spn = req_spn;

  /* the additional memory block of the firmware */
  common_fw_vcm_entry_ptr = (common_fw_vcm_entry_t *) vcm_cfg_malloc(sizeof(common_fw_vcm_entry_t));
  if(common_fw_vcm_entry_ptr == NULL)
    return DSMCOM_ALLOC_ERROR;

  common_fw_vcm_entry_ptr->fw_burn_count           = burn_count;
  common_fw_vcm_entry_ptr->fw_origin               = 0;
  common_fw_vcm_entry_ptr->fw_location             = location;
  common_fw_vcm_entry_ptr->req_fw_ver.version.high = temp_vs.version.high;
  common_fw_vcm_entry_ptr->req_fw_ver.version.low  = temp_vs.version.low;

  vcm_memory_ptr_set(additional_mem_ptr, common_fw_vcm_entry_ptr, sizeof(common_fw_vcm_entry_t));

  vcm_module_status_set(user_fw_vcm_module_ptr, VCM_STATUS_INITIALIZED);
  vcm_module_status_set(fw_vcm_module_ptr, VCM_STATUS_INITIALIZED);

  return DSMCOM_NO_ERROR;
}
#endif
#endif

/*- FIFO functions ----------------------------------------------------------*/
#if defined _DS1005 || defined _DS1006 || defined _DS1103 || defined _DS1104 || defined _DS1401

/******************************************************************************
*
* FUNCTION
*   Basic initialization of FIFO's for data exchange
*
* SYNTAX
*   int dsmcom_fifo_init (dsmcom_fifo_header_t ***handle,
*                          unsigned long buffer_base, long target,
*                          unsigned int channel_count, unsigned int buffer_size,
*                          unsigned int master_address_increment,
*                          unsigned int slave_address_increment,
*                          dpm_write_fcn_t write_fcn, dpm_read_fcn_t  read_fcn)
*
* DESCRIPTION
*   The function initializes the FIFO buffer.
*
* PARAMETERS
*   handle                       communication handle for a certain slave CPU
*   buffer_base                  base address of communication memory
*   target                       describes the access to the target memory
*   channel_count                count of communication channels
*   buffer_size                  size of the transmit and receive buffers
*                  amount the address is incremented to access the next dpm word
*   master_address_increment     - form the master side
*   slave_address_increment      - from the slave side
*   write_fcn                    write function for dpm access
*   read_fcn                     read function for dpm access
*
* RETURNS
*   errorcode
*
* REMARKS
*   Slave CPU acknowledge must be checked by calling routine dsmcom_fifo_ack.
*   Slave CPU must perform it's initialization after this function has been 
*   executed on the master CPU.
*
******************************************************************************/
#ifndef _INLINE

Int16 dsmcom_fifo_init ( dsmcom_fifo_header_t ***handle,
                       UInt32 buffer_base, 
                       UInt32 target, 
                       UInt16 channel_count,
                       UInt16 buffer_size,
                       UInt16 master_address_increment, 
                       UInt16 slave_address_increment,
                       dpm_write_fcn_t write_fcn, 
                       dpm_read_fcn_t  read_fcn )
{
  dsmcom_fifo_header_t *fifo_p;
  unsigned int offs;
  unsigned int i;

  /* allocate and initialize task list */

  *handle = (dsmcom_fifo_header_t **) 
    malloc(sizeof(dsmcom_fifo_header_t *));
  if (*handle == NULL)
  {
    return(DSMCOM_ALLOC_ERROR);
  }

  /* allocate master communication data structure */


  for (i = 0; i < channel_count; i++)
  {
    /* allocate master communication data structure */

    fifo_p = (dsmcom_fifo_header_t *) malloc(sizeof(dsmcom_fifo_header_t));
    if (fifo_p == NULL)
    {
      return(DSMCOM_ALLOC_ERROR);
    }

    /* initialize master communication data structure */
    fifo_p->target         = target;
    fifo_p->write_fcn      = write_fcn;
    fifo_p->read_fcn       = read_fcn;
    fifo_p->channel_count  = channel_count;

    fifo_p->tx_buffer_base = buffer_base + 
      master_address_increment * buffer_size * i;
    fifo_p->tx_buffer_size = buffer_size;
    fifo_p->rx_buffer_base = buffer_base +
      master_address_increment * buffer_size * (channel_count + i);
    fifo_p->rx_buffer_size = buffer_size;
    fifo_p->master_write_ptr_addr = buffer_base +
      master_address_increment * (channel_count * buffer_size * 2 + i);
    fifo_p->master_read_ptr_addr  = buffer_base +
      master_address_increment * (channel_count * (buffer_size * 2 + 1) + i);
    fifo_p->slave_write_ptr_addr  = buffer_base +
      master_address_increment * (channel_count * (buffer_size * 2 + 2) + i);
    fifo_p->slave_read_ptr_addr   = buffer_base +
      master_address_increment * (channel_count * (buffer_size * 2 + 3) + i);
    fifo_p->slave_error_addr      = buffer_base +
      master_address_increment * (channel_count * (buffer_size * 2 + 4) + i);
    fifo_p->slave_error_ack_addr  = buffer_base +
      master_address_increment * (channel_count * (buffer_size * 2 + 5) + i);
    fifo_p->semaphore_addr        = buffer_base +
      master_address_increment * (channel_count * (buffer_size * 2 + 6) + i);
    fifo_p->slave_address_increment = slave_address_increment;
    fifo_p->master_write_ptr = 0;
    fifo_p->master_read_ptr = 0;
    fifo_p->slave_error_ack = 0;

    /* clear transmit buffer */
    for (offs = 0; offs < buffer_size; offs++)
    {
      DPM_WRITE(fifo_p, fifo_p->tx_buffer_base, offs,
        DSMCOM_FIFO_CLEAR);
    }

    /* clear receive buffer */

    for (offs = 0; offs < buffer_size; offs++)
    {
      DPM_WRITE(fifo_p, fifo_p->rx_buffer_base, offs,
        DSMCOM_FIFO_CLEAR);
    }

    /* initialize master write pointer in communication memory */
    DPM_WRITE(fifo_p,fifo_p->master_write_ptr_addr, 0, fifo_p->master_write_ptr);
  
    /* initialize master read pointer in communication memory */
    DPM_WRITE(fifo_p,fifo_p->master_read_ptr_addr, 0, fifo_p->master_read_ptr);

    /* clear slave error flag in communication memory */
    DPM_WRITE(fifo_p,fifo_p->slave_error_addr, 0, 0);

    /* clear master error flag in communication memory */
    DPM_WRITE(fifo_p,fifo_p->slave_error_ack_addr, 0, 0);

    /* clear UART semaphore flag in communication memory */
    DPM_WRITE(fifo_p,fifo_p->semaphore_addr, 0, 0);
  

    /* enter communication data structure pointer in task list */
    (**handle) = fifo_p;
  }
  return(DSMCOM_NO_ERROR);
}

#endif /* _INLINE */




/******************************************************************************
*
* FUNCTION
*   Clears the FIFO.
*
* SYNTAX
*   void dsmcom_fifo_clear ( dsmcom_fifo_header_t **handle,
*                             const unsigned int task_id)
*
* DESCRIPTION
*   The function clear the FIFO and the master read and write pointer.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
#ifndef _INLINE

void dsmcom_fifo_clear ( dsmcom_fifo_header_t **handle,
                         const UInt16 task_id)
{
  UInt16 offs;
  UInt16 buffer_size;

  dsmcom_fifo_header_t *fifo_p;

  fifo_p = handle[task_id];

  fifo_p->master_write_ptr = 0;
  fifo_p->master_read_ptr = 0;
  fifo_p->slave_error_ack = 0;

  buffer_size = fifo_p->tx_buffer_size;

  for (offs = 0; offs < buffer_size; offs++)
  {
    DPM_WRITE(fifo_p, fifo_p->tx_buffer_base, offs,
              DSMCOM_FIFO_CLEAR);
  }

  /* clear receive buffer */

  buffer_size = fifo_p->rx_buffer_size;

  for (offs = 0; offs < buffer_size; offs++)
  {
    DPM_WRITE(fifo_p, fifo_p->rx_buffer_base, offs, DSMCOM_FIFO_CLEAR);
  }

  /* initialize master write pointer in communication memory */
  DPM_WRITE(fifo_p,fifo_p->master_write_ptr_addr, 0, fifo_p->master_write_ptr);
    
  /* initialize master read pointer in communication memory */
  DPM_WRITE(fifo_p,fifo_p->master_read_ptr_addr, 0, fifo_p->master_read_ptr);

  /* clear slave error flag in communication memory */
  DPM_WRITE(fifo_p,fifo_p->slave_error_addr, 0, 0);

  /* clear slave error flag in communication memory */
  DPM_WRITE(fifo_p,fifo_p->slave_error_ack_addr, 0, 0);

  /* clear UART semaphore flag in communication memory */
  DPM_WRITE(fifo_p,fifo_p->semaphore_addr, 0, 0);

}

#endif

/******************************************************************************
*
* FUNCTION
*   Transmit FIFO overflow check
*
* SYNTAX
*   unsigned long dsmcom_fifo_transmit_space_get ( dsmcom_fifo_header_t **handle, 
*                                                   const unsigned int task_id )
*
* DESCRIPTION
*   The function evaluates available space in the transmit FIFO.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*
* RETURNS
*   available FIFO space (in FIFO units)
*
* REMARKS
*
******************************************************************************/
__INLINE UInt32 dsmcom_fifo_transmit_space_get( dsmcom_fifo_header_t **handle, 
                                                const UInt16 task_id )
{
  unsigned long read_ptr;
  dsmcom_fifo_header_t *fifo_p;
  
  fifo_p = handle[task_id];

  /* read slave read pointer from communication memory */
  read_ptr = DPM_READ(fifo_p,fifo_p->slave_read_ptr_addr, 0);

#if defined (_SW_ARBITRATION)
  do   /* software arbitration */
  {
    compare = read_ptr;
    read_ptr = DPM_READ(fifo_p,fifo_p->slave_read_ptr_addr, 0);
  } while(compare - read_ptr);
#endif

  /* calculate available buffer space */
  return((read_ptr / fifo_p->slave_address_increment - fifo_p->master_write_ptr - 1) &
    (fifo_p->tx_buffer_size - 1));
}

/******************************************************************************
*
* FUNCTION
*   Receive FIFO available check
*
* SYNTAX
*   unsigned long dsmcom_fifo_receive_count_get ( dsmcom_fifo_header_t **handle, 
*                                                  const unsigned int task_id )
*
* DESCRIPTION
*   The function evaluates available data in the receive FIFO.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*
* RETURNS
*   available FIFO data (in FIFO units)
*
* REMARKS
*
******************************************************************************/
__INLINE UInt32 dsmcom_fifo_receive_count_get ( dsmcom_fifo_header_t **handle, 
                                                       const UInt16 task_id )
{
  unsigned long write_ptr;
  unsigned long read_ptr;
  dsmcom_fifo_header_t *fifo_p;
  
  fifo_p = handle[task_id];

  /* read slave write pointer from communication memory */
  write_ptr = DPM_READ(fifo_p,fifo_p->slave_write_ptr_addr, 0);

#if defined (_SW_ARBITRATION)
  do   /* software arbitration */
  {
    compare = write_ptr;
    write_ptr = DPM_READ(fifo_p,fifo_p->slave_write_ptr_addr, 0);
  } while(compare - write_ptr);
#endif

  read_ptr = DPM_READ(fifo_p,fifo_p->master_read_ptr_addr, 0);

#if defined (_SW_ARBITRATION)
  do   /* software arbitration */
  {
    compare = read_ptr;
    read_ptr = DPM_READ(fifo_p,fifo_p->master_read_ptr_addr, 0);
  } while(compare - read_ptr);
#endif

  fifo_p->master_read_ptr = read_ptr;
  
  /* calculate available data */
  return((write_ptr / fifo_p->slave_address_increment - read_ptr) &
    (fifo_p->tx_buffer_size - 1));
}
/******************************************************************************
*
* FUNCTION
*   Read FIFO error status
*
* SYNTAX
*   void void dsmcom_fifo_error_read (dsmcom_fifo_header_t **handle,
*                                     Int16                task_id,  
*                                     UInt16               *fifo_error)
*
* DESCRIPTION
*   The FIFO error status is read from the communication memory.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*   fifo_error          fifo error number
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
__INLINE void dsmcom_fifo_error_read (dsmcom_fifo_header_t **handle,
                                      UInt32               *fifo_error)
{
  unsigned long error;
  dsmcom_fifo_header_t *fifo_p = *handle;

  error = DPM_READ(fifo_p, fifo_p->slave_error_addr, 0);
#if defined (_SW_ARBITRATION)
  do   /* software arbitration */
  {
    compare = error;
    error = DPM_READ(fifo_p, fifo_p->slave_error_addr, 0);
  } while(compare - error);
#endif

  *fifo_error = error ^ fifo_p->slave_error_ack;
 
  return;
}

/******************************************************************************
*
* FUNCTION
*   Clear FIFO error status
*
* SYNTAX
*   void dsmcom_fifo_error_clear (ser_tp1_fifo_header_t **handle,
*                                 Int16                 task_id,
*                                 UInt16                fifo_error)
*
* DESCRIPTION
*   The FIFO error status is cleared in the communication memory.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*
* RETURNS
*   void
*
* REMARKS
*
******************************************************************************/
__INLINE void dsmcom_fifo_error_clear (dsmcom_fifo_header_t **handle,
                                       UInt32               fifo_error)
{
  unsigned long i;
  unsigned long bit_position;
  dsmcom_fifo_header_t *fifo_p = *handle;

  for ( i = 0; i < 16; i++)
  {
    bit_position = 1 << i;

    if (fifo_error & bit_position)
    {
      if (fifo_p->slave_error_ack & bit_position)
      {
        fifo_p->slave_error_ack &= ~bit_position;
      }
      else
      {
        fifo_p->slave_error_ack |= bit_position;
      }
    }
  }
    
  DPM_WRITE(fifo_p, fifo_p->slave_error_ack_addr, 0, fifo_p->slave_error_ack);
}

/******************************************************************************
*
* FUNCTION
*   Transmit data to slave
*
* SYNTAX
*   void dsmcom_fifo_write ( dsmcom_fifo_header_t **handle,
*                            const unsigned int task_id, 
*                            const unsigned int datalen, 
*                            const unsigned int *data)
*
* DESCRIPTION
*   The function writes data into the transmit FIFO.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*   datalen             number of data that is to be written
*   data                data to be written
*
* RETURNS
*   void
*
* REMARKS
*   The availability of space in the FIFO must be guaranteed by the calling 
*   function.
*
******************************************************************************/
__INLINE void dsmcom_fifo_write ( dsmcom_fifo_header_t **handle, 
                                  const UInt16 task_id, 
                                  const UInt16 datalen, 
                                  const UInt16 *data)
{
  dsmcom_fifo_header_t *fifo_p;
  unsigned long int_status;                           /* interrupt status */
  UInt16 i;
  unsigned long dummy;

  fifo_p = handle[task_id];

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* write data to transmit buffer */
  for (i = 0; i < datalen; i++)
  {
    DSMCOM_FIFO_WRITE_PTR_INC();
    dummy =  (*data++);
    DPM_WRITE(fifo_p, fifo_p->tx_buffer_base, fifo_p->master_write_ptr, dummy);
  }

  /* update master write pointer in communication memory */
  DPM_WRITE(fifo_p,fifo_p->master_write_ptr_addr, 0, fifo_p->master_write_ptr);
  
  RTLIB_INT_RESTORE(int_status);
}

/******************************************************************************
*
* FUNCTION
*   Read data from FIFO
*
* SYNTAX
*   int dsmcom_fifo_read (dsmcom_fifo_header_t **handle, 
*                               const unsigned int task_id,
                                const unsigned int datalen,
*                               unsigned int *data)
*
* DESCRIPTION
*   The function reads data from the receive FIFO.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*   datalen             number of data that is to be read
*   data                data to be read
*
* RETURNS
*   void
*
* REMARKS
*   The availability of data must be guaranteed by the calling function.
*
******************************************************************************/
__INLINE void dsmcom_fifo_read ( dsmcom_fifo_header_t **handle, 
                                  const UInt16 task_id,
                                  const UInt16 datalen,
                                  UInt16 *data)
{
  dsmcom_fifo_header_t *fifo_p;
  unsigned long int_status;                           /* interrupt status */
  UInt16 i;
  unsigned long read_ptr;
 
  fifo_p = handle[task_id];

  read_ptr = DPM_READ(fifo_p,fifo_p->master_read_ptr_addr, 0);

#if defined (_SW_ARBITRATION)
  do   /* software arbitration */
  {
    compare = read_ptr;
    read_ptr = DPM_READ(fifo_p,fifo_p->master_read_ptr_addr, 0);
  } while(compare - read_ptr);
#endif

  RTLIB_INT_SAVE_AND_DISABLE(int_status);
  
  fifo_p->master_read_ptr = read_ptr;

  for (i = 0; i < datalen; i++)
  {
    DSMCOM_FIFO_READ_PTR_INC();
    *data++ = DPM_READ(fifo_p, fifo_p->rx_buffer_base, fifo_p->master_read_ptr);
  }

  DPM_WRITE(fifo_p,fifo_p->master_read_ptr_addr, 0, fifo_p->master_read_ptr);

  RTLIB_INT_RESTORE(int_status);
}

/******************************************************************************
*
* FUNCTION
*   Set the semaphore
*
* SYNTAX
*   int dsmcom_fifo_semaphore_set( dsmcom_fifo_header_t **handle, 
                                    const unsigned int task_id )
*
* DESCRIPTION
*   The semaphore for the master read pointer is occupied.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE int dsmcom_fifo_semaphore_set( dsmcom_fifo_header_t **handle,
                                         const UInt16 task_id )
{
//  Float32 start_time;
  unsigned long semaphore;
  dsmcom_fifo_header_t *fifo_p;
  ts_timestamp_type timestamp_start, timestamp_end;
  double time;

  fifo_p = handle[task_id];

  ts_timestamp_read_fast(&timestamp_start);

  do{
    do{
      semaphore = DPM_READ(fifo_p, fifo_p->semaphore_addr, 0);

#if defined (_SW_ARBITRATION)
      do   /* software arbitration */
      {
        compare = semaphore;
        semaphore = DPM_READ(fifo_p, fifo_p->semaphore_addr, 0);
      } while(compare - semaphore);
#endif
      ts_timestamp_read_fast(&timestamp_end);
      time = ts_timestamp_interval(&timestamp_start, &timestamp_end);

      if (time >= (dsfloat) DSCOMDEF_FIFO_TIMEOUT)
      {
        return (DSMCOM_TIMEOUT);
      }
    } while(semaphore == DSMCOM_SLV_SEMA_ENABLE);
  
    DPM_WRITE(fifo_p, fifo_p->semaphore_addr, 0, DSMCOM_MST_SEMA_ENABLE);

    semaphore = DPM_READ(fifo_p, fifo_p->semaphore_addr, 0);
#if defined (_SW_ARBITRATION)
    do   /* software arbitration */
    {
      compare = semaphore;
      semaphore = DPM_READ(fifo_p, fifo_p->semaphore_addr, 0);
    } while(compare - semaphore);
#endif
  }while (semaphore != DSMCOM_MST_SEMA_ENABLE);

  return (DSMCOM_NO_ERROR);
}
  
/******************************************************************************
*
* FUNCTION
*   Clears the semaphore
*
* SYNTAX
*   int dsmcom_fifo_semaphore_clear( dsmcom_fifo_header_t **handle, 
*                                     const unsigned int task_id )
*
* DESCRIPTION
*   The semaphore for the master read pointer is released.
*
* PARAMETERS
*   handle              communication handle for a certain slave CPU
*   task_id             task ID of FIFO channel
*
* RETURNS
*   errorcode
*
* REMARKS
*
******************************************************************************/
__INLINE int dsmcom_fifo_semaphore_clear( dsmcom_fifo_header_t **handle,
                                           const UInt16 task_id )
{  
  unsigned long semaphore;
  dsmcom_fifo_header_t *fifo_p;
  
  fifo_p = handle[task_id];

  semaphore = DPM_READ(fifo_p, fifo_p->semaphore_addr, 0);
#if defined (_SW_ARBITRATION)
  do   /* software arbitration */
  {
    compare = semaphore;
    semaphore = DPM_READ(fifo_p, fifo_p->semaphore_addr, 0);
  } while(compare - semaphore);
#endif

  if (semaphore != DSMCOM_MST_SEMA_ENABLE)
  { 
    return (DSMCOM_SEMA_ACCESS_ERROR);
  }

  DPM_WRITE(fifo_p, fifo_p->semaphore_addr, 0, 0);
  return(DSMCOM_NO_ERROR);
}

#endif /* #if defined _DSXXXX */

#undef __INLINE
