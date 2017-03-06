/***************************************************************************
*
*  MODULE
*    DSMCR - Multi Client Ringbuffer
*
*  FILE
*    dsmcr.c
*
*  RELATED FILES
*    dsmcr.h
*
*  DESCRIPTION
*    dSPACE multi client ringbuffer function API
*
*  REMARKS
*
*  AUTHOR(S)
*    T. Woelfer
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: dsmcr.c $ $Revision: 1.9 $ $Date: 2008/12/12 09:25:41GMT+01:00 $
***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <dsstd.h>
#include <dsvcm.h>
#include <dsmsg.h>
#include <dsmcr.h>

/* $DSVERSION: DSMCR - Multi Client Ringbuffer */
#define DSMCR_VER_MAR 1
#define DSMCR_VER_MIR 2
#define DSMCR_VER_MAI 0
#define DSMCR_VER_SPB VCM_VERSION_RELEASE
#define DSMCR_VER_SPN 0
#define DSMCR_VER_PLV 0

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 || defined _DS1005 || defined _DS1103 || defined _DS1104 || defined _DS1401 

DS_VERSION_SYMBOL(_dsmcr_ver_mar, DSMCR_VER_MAR);
DS_VERSION_SYMBOL(_dsmcr_ver_mir, DSMCR_VER_MIR);
DS_VERSION_SYMBOL(_dsmcr_ver_mai, DSMCR_VER_MAI);
DS_VERSION_SYMBOL(_dsmcr_ver_spb, DSMCR_VER_SPB);
DS_VERSION_SYMBOL(_dsmcr_ver_spn, DSMCR_VER_SPN);
DS_VERSION_SYMBOL(_dsmcr_ver_plv, DSMCR_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

/***************************************************************************
  constant, macro and type definitions
***************************************************************************/


/***************************************************************************
  object declarations
***************************************************************************/

#ifndef _INLINE
dsmcr_t_buffer_descriptor dsmcr_buffers;
#else
extern dsmcr_t_buffer_descriptor dsmcr_buffers;
#endif

/***************************************************************************
  function prototypes
***************************************************************************/


/***************************************************************************
  function declarations
***************************************************************************/

#ifdef _INLINE
#  if defined (_DS1005) || defined (_DS1103) || defined (_DS1104) || defined (_DS1401) || defined (_DS1603)
#    define __INLINE static
#  elif defined (_DS1003) || defined (_DS1004) || defined (_DS1102) || defined (_DS1006)
#    define __INLINE static inline
#  else
#    error dsmcr.c: no processor board defined (e.g. _DS1005)
#  endif
#else
#  define __INLINE
#endif


#ifndef _INLINE

/*******************************************************************************
*
*  FUNCTION
*    dsmcr_init
*
*
*  SYNTAX
*    UInt32 dsmcr_init() 
*
*  DESCRIPTION
*    multi client ring buffer initialization function
*
*  PARAMETERS
*
*  RETURNS
*    DSMCR_NO_ERROR:            the module was initialized successfully
*    DSMCR_ALREADY_INITIALIZED: the module has already been initialized
*
*  REMARKS
*
*******************************************************************************/

UInt32 dsmcr_init ()
{
#ifdef VCM_AVAILABLE
  vcm_module_descriptor_type  *dsmcr_module_ptr;

  /* check for subsequent initialization requests */
  if (VCM_STATUS_INITIALIZED ==
      vcm_module_status_get(vcm_module_find(VCM_MID_DSMCR, NULL)))
    return DSMCR_ALREADY_INITIALIZED;

  /* register the module at the VCM */
  dsmcr_module_ptr = vcm_module_register(VCM_MID_DSMCR,
                                         vcm_module_find(VCM_MID_RTLIB, NULL),
                                         VCM_TXT_DSMCR,
                                         DSMCR_VER_MAR, DSMCR_VER_MIR, DSMCR_VER_MAI,
                                         DSMCR_VER_SPB, DSMCR_VER_SPN, DSMCR_VER_PLV,
                                         0, 0);

  if (!dsmcr_module_ptr)
  {
    msg_error_set(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "dsmcr_init(): " MSG_MEM_ALLOC_ERROR_TXT);
    exit(1);
  }
  #endif

  /* initialize the buffer descriptor structure */
  dsmcr_buffers.number = 0;
  dsmcr_buffers.p_buffers = NULL;

  #ifdef VCM_AVAILABLE
  /* set the module status to initialized */
  vcm_module_status_set(dsmcr_module_ptr, VCM_STATUS_INITIALIZED);
  #endif

  return DSMCR_NO_ERROR;
}


/*******************************************************************************
*
*  FUNCTION
*    dsmcr_buffer
*
*
*  SYNTAX
*    UInt32 dsmcr_buffer (UInt32  element_size,
*                         UInt32  number_of_elements,
*                         UInt32 *p_buffer_index) 
*
*  DESCRIPTION
*    This function creates a new ringbuffer with number_of_elements data
*    elements of element_size bytes.
*
*  PARAMETERS
*    element_size:       size in bytes for one ringbuffer element
*    number_of_elements: number of data elements of the ringbuffer to create
*    p_buffer_index:     pointer to a buffer index, the function returns the
*                        index of the created buffer here
*
*  RETURNS
*    DSMCR_NO_ERROR:     the buffer was created successfully
*
*  REMARKS
*
*******************************************************************************/

UInt32 dsmcr_buffer (UInt32  element_size,
                     UInt32  number_of_elements,
                     UInt32 *p_buffer_index)
{
  /* check for DSMCR initialization */
#if defined VCM_AVAILABLE
  if (VCM_STATUS_INITIALIZED !=
      vcm_module_status_get(vcm_module_find(VCM_MID_DSMCR, NULL)))
  {
    msg_error_set(MSG_SM_RTLIB, DSMCR_MSG_INITIALIZATION_MISSING,
                  "dsmcr_buffer(): " DSMCR_MSG_INITIALIZATION_MISSING_TXT);
    exit(1);
  }
#endif
  /* check for maximum number of buffers */
  if (dsmcr_buffers.number == DSMCR_INVALID_BUFFER_INDEX)
  {
    msg_error_set(MSG_SM_RTLIB, DSMCR_MSG_MAX_BUFFERS, DSMCR_MSG_MAX_BUFFERS_TXT);
    exit(1);
  }

  /* create or extend the buffers description data array */
  dsmcr_buffers.number++;
  dsmcr_buffers.p_buffers = 
    (dsmcr_t_buffer_data *) realloc(dsmcr_buffers.p_buffers,
                                    sizeof(dsmcr_t_buffer_data) * dsmcr_buffers.number);

  /* check for allocation success */
  if (!dsmcr_buffers.p_buffers)
  {
    msg_error_set(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "dsmcr_buffer(): " MSG_MEM_ALLOC_ERROR_TXT);
    exit(1);
  }

  /* initialize this buffers data */
  dsmcr_buffers.p_buffers[dsmcr_buffers.number - 1].write_index = 0;
  dsmcr_buffers.p_buffers[dsmcr_buffers.number - 1].number_of_clients = 0;
  dsmcr_buffers.p_buffers[dsmcr_buffers.number - 1].p_clients = NULL;

  dsmcr_buffers.p_buffers[dsmcr_buffers.number - 1].element_size = element_size;
  dsmcr_buffers.p_buffers[dsmcr_buffers.number - 1].number_of_elements = number_of_elements;

  /* allocate the buffer memory */
  dsmcr_buffers.p_buffers[dsmcr_buffers.number - 1].p_buffer = malloc(element_size * (number_of_elements + 1));

  /* check for allocation success */
  if (!dsmcr_buffers.p_buffers[dsmcr_buffers.number - 1].p_buffer)
  {
    msg_error_set(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "dsmcr_buffer(): " MSG_MEM_ALLOC_ERROR_TXT);
    exit(1);
  }

  /* set the buffer index */
  *p_buffer_index = dsmcr_buffers.number - 1;

  return DSMCR_NO_ERROR;
}


/*******************************************************************************
*
*  FUNCTION
*    dsmcr_client
*
*
*  SYNTAX
*    UInt32 dsmcr_client (UInt32                 buffer_index,
*                         UInt32                 overrun_policy,
*                         dsmcr_t_p_overrun_fcn  p_overrun_fcn,
*                         UInt32                *p_client_index) 
*
*  DESCRIPTION
*    The function creates a new client for the buffer specified by buffer_index.
*
*  PARAMETERS
*    buffer_index:    index to the buffer, to which the new client shall be
*                     added
*    overrun_policy:  the overrun policy, can be one of the following four
*                     constants: DSMCR_OVERRUN_FUNCTION
*                                DSMCR_OVERRUN_ERROR
*                                DSMCR_OVERRUN_IGNORE
*                                DSMCR_OVERRUN_OVERWRITE
*
*    p_overrun_fcn:  the overrun function pointer, set to NULL if no overrun
*                    function is specified
*    p_client_index: pointer to a memory location where the client index is
*                    returned
*
*  RETURNS
*    DSMCR_NO_ERROR: the client was created successfully
*
*  REMARKS
*
*******************************************************************************/

UInt32 dsmcr_client (UInt32                 buffer_index,
                     UInt32                 overrun_policy,
                     dsmcr_t_p_overrun_fcn  p_overrun_fcn,
                     UInt32                *p_client_index)
{
#if defined VCM_AVAILABLE
  /* check for DSMCR initialization */
  if (VCM_STATUS_INITIALIZED !=
      vcm_module_status_get(vcm_module_find(VCM_MID_DSMCR, NULL)))
  {
    msg_error_set(MSG_SM_RTLIB, DSMCR_MSG_INITIALIZATION_MISSING,
                  "dsmcr_client(): " DSMCR_MSG_INITIALIZATION_MISSING_TXT);
    exit(1);
  }
#endif
  /* check for correct buffer index */
  if (buffer_index >= dsmcr_buffers.number)
  {
    msg_error_printf(MSG_SM_RTLIB, DSMCR_MSG_INVALID_BUFFER_INDEX,
                     "dsmcr_client(): " DSMCR_MSG_INVALID_BUFFER_INDEX_TXT, buffer_index);
    exit(1);
  }

  /* check for maximum number of clients */
  if (dsmcr_buffers.p_buffers[buffer_index].number_of_clients == DSMCR_INVALID_CLIENT_INDEX)
  {
    msg_error_printf(MSG_SM_RTLIB, DSMCR_MSG_MAX_CLIENTS, DSMCR_MSG_MAX_CLIENTS_TXT, buffer_index);
    exit(1);
  }

  /* create or extend the client data array */
  dsmcr_buffers.p_buffers[buffer_index].number_of_clients++;
  dsmcr_buffers.p_buffers[buffer_index].p_clients = 
    (dsmcr_t_client_data *) realloc(dsmcr_buffers.p_buffers[buffer_index].p_clients,
                                    sizeof(dsmcr_t_client_data) * dsmcr_buffers.p_buffers[buffer_index].number_of_clients);


  /* check for allocation success */
  if (!dsmcr_buffers.p_buffers[buffer_index].p_clients)
  {
    msg_error_set(MSG_SM_RTLIB, MSG_MEM_ALLOC_ERROR, "dsmcr_client(): " MSG_MEM_ALLOC_ERROR_TXT);
    exit(1);
  }

  /* catch illegal overrun policies, the overrun policy defaults to DSMCR_OVERRUN_ERROR */
  if ((overrun_policy < 1) || (overrun_policy > 4))
    overrun_policy = DSMCR_OVERRUN_ERROR;

  /* initialize the client data */
  dsmcr_buffers.p_buffers[buffer_index].p_clients[dsmcr_buffers.p_buffers[buffer_index].number_of_clients - 1].read_index = 
    dsmcr_buffers.p_buffers[buffer_index].write_index;
  dsmcr_buffers.p_buffers[buffer_index].p_clients[dsmcr_buffers.p_buffers[buffer_index].number_of_clients - 1].overrun_policy = overrun_policy;
  dsmcr_buffers.p_buffers[buffer_index].p_clients[dsmcr_buffers.p_buffers[buffer_index].number_of_clients - 1].p_overrun_fcn = p_overrun_fcn;
  dsmcr_buffers.p_buffers[buffer_index].p_clients[dsmcr_buffers.p_buffers[buffer_index].number_of_clients - 1].last_write_state = DSMCR_NO_ERROR;
  dsmcr_buffers.p_buffers[buffer_index].p_clients[dsmcr_buffers.p_buffers[buffer_index].number_of_clients - 1].is_connected = 1;


  /* set the client index */
  *p_client_index = dsmcr_buffers.p_buffers[buffer_index].number_of_clients - 1;
  
  return DSMCR_NO_ERROR;
}


/*******************************************************************************
*
*  FUNCTION
*    dsmcr_clear
*
*
*  SYNTAX
*    UInt32 dsmcr_clear(UInt32 buffer_index) 
*
*  DESCRIPTION
*    clear a specific buffer for all clients
*
*  PARAMETERS
*    buffer_index: index to the buffer, to which the new client shall be
*                  added
*
*  RETURNS
*    DSMCR_NO_ERROR: the buffer was cleared successfully
*
*  REMARKS
*
*******************************************************************************/

UInt32 dsmcr_clear (UInt32 buffer_index)
{
  rtlib_int_status_t int_status;
  UInt32 client_index;
#if defined VCM_AVAILABLE
  /* check for DSMCR initialization */
  if (VCM_STATUS_INITIALIZED !=
      vcm_module_status_get(vcm_module_find(VCM_MID_DSMCR, NULL)))
  {
    msg_error_set(MSG_SM_RTLIB, DSMCR_MSG_INITIALIZATION_MISSING,
                  "dsmcr_clear(): " DSMCR_MSG_INITIALIZATION_MISSING_TXT);
    exit(1);
  }
#endif
  /* check for correct buffer index */
  if (buffer_index >= dsmcr_buffers.number)
  {
    msg_error_printf(MSG_SM_RTLIB, DSMCR_MSG_INVALID_BUFFER_INDEX,
                     "dsmcr_clear(): " DSMCR_MSG_INVALID_BUFFER_INDEX_TXT, buffer_index);
    exit(1);
  }

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* iterate over all clients and set the read index to the write index */
  for (client_index = 0; client_index < dsmcr_buffers.p_buffers[buffer_index].number_of_clients; client_index++)
  {
    dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index =
      dsmcr_buffers.p_buffers[buffer_index].write_index;
  }

  RTLIB_INT_RESTORE(int_status);

  return DSMCR_NO_ERROR;
}


#endif /* !_INLINE */

/*******************************************************************************
*
*  FUNCTION
*    dsmcr_write
*
*
*  SYNTAX
*    UInt32 dsmcr_write (UInt32  buffer_index,
*                        void   *p_data) 
*
*  DESCRIPTION
*    The function writes a data element beginning from the memory location
*    referenced by p_data to the buffer specified by buffer_index.
*
*  PARAMETERS
*    buffer_index: index to the buffer to which the data element shall be
*                  written
*    p_data:       pointer reference to the data element which shall be written
*                  to the buffer
*
*  RETURNS
*    DSMCR_NO_ERROR:          the data element was stored successfully
*    DSMCR_OVERRUN_FUNCTION:  an overrun occurred while trying to store the data
*                 _ERROR      element, the overrun was handled as specified by
*                 _IGNORE     the return value
*                 _OVERWRITE 
*
*  REMARKS
*
*******************************************************************************/

__INLINE UInt32 dsmcr_write (UInt32  buffer_index,
                             void   *p_data)
{
  rtlib_int_status_t int_status;
  UInt32 advanced_write_index;
  UInt32 client_index;
  UInt32 highest_overrun_policy = 0;
  UInt32 returned_overrun_policy;
  UInt32 highest_returned_overrun_policy = 0;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check all clients for overruns */
  advanced_write_index = dsmcr_buffers.p_buffers[buffer_index].write_index + 1;

  /* check for index wrap around */
  if (advanced_write_index == (dsmcr_buffers.p_buffers[buffer_index].number_of_elements + 1))
  {
    advanced_write_index = 0;
  }

  for (client_index = 0; client_index < dsmcr_buffers.p_buffers[buffer_index].number_of_clients; client_index++)
  {
    /* check if the client is connected, if so then... */
    /* check for an overrun at this specific client */
    /* an overrun occurs when the advanced write index equals the read index */
    if ((dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].is_connected) &&
        (advanced_write_index == dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index))
    {
      /* Enter the last write state for this client: overrun policy. */
      dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].last_write_state = dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].overrun_policy;

      /* remember the highest priority overrun policy */
      if (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].overrun_policy > highest_overrun_policy)
      {
        highest_overrun_policy = dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].overrun_policy;
      }

      /* call the overrunfcuntion immediately if defined */
      if (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].overrun_policy == DSMCR_OVERRUN_FUNCTION)
      {
        /* call the overrun function if one is specified, otherwise switch back to DSMCR_OVERRUN_ERROR */
        if (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].p_overrun_fcn)
        {
          returned_overrun_policy = dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].p_overrun_fcn(buffer_index, client_index, p_data);

          /* catch illegal overrun policies, the overrun policy defaults to DSMCR_OVERRUN_ERROR */
          if ((returned_overrun_policy < 1) || (returned_overrun_policy > 3))
          {
            returned_overrun_policy = DSMCR_OVERRUN_ERROR;
          }

          /* remember the highest returned overrun policy */
          if (returned_overrun_policy > highest_returned_overrun_policy)
          {
            highest_returned_overrun_policy = returned_overrun_policy;
          }
        }
        else
        {
          /* if no overrun function is specified, switch back to DSMCR_OVERRUN_ERROR */
          highest_overrun_policy = DSMCR_OVERRUN_ERROR;
        }
      }
    }
    else
    {
      /* Enter the last write state for this client: no overrun occured. */
      dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].last_write_state = DSMCR_NO_ERROR;
    }
  }

  if (highest_returned_overrun_policy)
  {
    highest_overrun_policy = highest_returned_overrun_policy;
  }

  /* handle the overrun */
  switch (highest_overrun_policy)
  {
    case    0:
      /* transfer the data element into the current write position */
      memcpy((void *) (((UInt32) dsmcr_buffers.p_buffers[buffer_index].p_buffer) + (dsmcr_buffers.p_buffers[buffer_index].write_index * dsmcr_buffers.p_buffers[buffer_index].element_size)),
             p_data,
             dsmcr_buffers.p_buffers[buffer_index].element_size);
      /* advance the write index */
      dsmcr_buffers.p_buffers[buffer_index].write_index = advanced_write_index;
      RTLIB_INT_RESTORE(int_status);
      return DSMCR_NO_ERROR;

    case    DSMCR_OVERRUN_ERROR:
      RTLIB_INT_RESTORE(int_status);
      for (client_index = 0; client_index < dsmcr_buffers.p_buffers[buffer_index].number_of_clients; client_index++)
      {
        if (advanced_write_index == dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index)
        {
          msg_error_printf(MSG_SM_RTLIB, DSMCR_MSG_OVERRUN, DSMCR_MSG_OVERRUN_TXT, client_index, buffer_index);
        }
      }
      exit(1);
      break;

    case    DSMCR_OVERRUN_IGNORE:
      RTLIB_INT_RESTORE(int_status);
      return DSMCR_OVERRUN_IGNORE;

    case    DSMCR_OVERRUN_OVERWRITE:
      /* transfer the data element into the current write position */
      memcpy((void *) (((UInt32) dsmcr_buffers.p_buffers[buffer_index].p_buffer) + (dsmcr_buffers.p_buffers[buffer_index].write_index * dsmcr_buffers.p_buffers[buffer_index].element_size)),
             p_data,
             dsmcr_buffers.p_buffers[buffer_index].element_size);
      /* advance the read indices of all clients where an overrun occurred */
      for (client_index = 0; client_index < dsmcr_buffers.p_buffers[buffer_index].number_of_clients; client_index++)
      {
        /* check for overrun */
        if (advanced_write_index == dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index)
        {
          /* advance the read index */
          dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index++;

          /* check for index wrap around */
          if (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index ==
              (dsmcr_buffers.p_buffers[buffer_index].number_of_elements + 1))
          {
            dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index = 0;
          }
        }
      }
      
      /* advance the write index */
      dsmcr_buffers.p_buffers[buffer_index].write_index = advanced_write_index;
      RTLIB_INT_RESTORE(int_status);

      return DSMCR_OVERRUN_OVERWRITE;
  }

  RTLIB_INT_RESTORE(int_status);

  return DSMCR_NO_ERROR;
}


/*******************************************************************************
*
*  FUNCTION
*    dsmcr_read
*
*
*  SYNTAX
*    UInt32 dsmcr_read (UInt32  buffer_index,
*                       UInt32  client_index,
*                       void   *p_data) 
*
*  DESCRIPTION
*    The function reads a data element from the buffer referenced by
*    buffer_index as the client specified by client_index.
*
*  PARAMETERS
*    buffer_index: index of the buffer, to which the data shall be written
*    client_index: client index by which the data element shall be read
*    p_data:       pointer reference to the data element which shall be read
*                  from the buffer
*
*  RETURNS
*    DSMCR_NO_ERROR:     the data element was read successfully
*    DSMCR_BUFFER_EMPTY: no new data where available, the data element remains
*                        unchanged
*
*  REMARKS
*
*******************************************************************************/

__INLINE UInt32 dsmcr_read (UInt32  buffer_index,
                            UInt32  client_index,
                            void   *p_data)
{
  rtlib_int_status_t int_status;

  RTLIB_INT_SAVE_AND_DISABLE(int_status);

  /* check clients connection state, if not connected return DSMCR_NO_ERROR */
  if (!(dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].is_connected))
  {
    RTLIB_INT_RESTORE(int_status);

    return DSMCR_NO_ERROR;

  }

      /* check for new events */
  if (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index !=
      dsmcr_buffers.p_buffers[buffer_index].write_index)
  {
    /* transfer the data element */
    memcpy(p_data,
           (void *) (((UInt32) dsmcr_buffers.p_buffers[buffer_index].p_buffer) + (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index * dsmcr_buffers.p_buffers[buffer_index].element_size)),
           dsmcr_buffers.p_buffers[buffer_index].element_size);

    /* advance the read pointer */
    dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index++;

    /* check for index wrap around */
    if (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index ==
        (dsmcr_buffers.p_buffers[buffer_index].number_of_elements + 1))
    {
      dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index = 0;
    }
  }
  else
  {
    RTLIB_INT_RESTORE(int_status);

    return DSMCR_BUFFER_EMPTY;
  }

  RTLIB_INT_RESTORE(int_status);

  return DSMCR_NO_ERROR;
}

/*******************************************************************************
*
*  FUNCTION
*    dsmcr_client_last_write_state
*
*
*  SYNTAX
*    UInt32 dsmcr_client_last_write_state (UInt32  buffer_index,
*                                          UInt32  client_index) 
*
*  DESCRIPTION
*    The function returns the last write state from the client referenced by
*    buffer_index and client_index.
*
*  PARAMETERS
*    buffer_index: index of the buffer, to which the client belongs
*    client_index: client index of the client to read the state from
*
*  RETURNS
*    The last write state for the specified client, on of:
*    DSMCR_NO_ERROR:          the data element was stored successfully
*    DSMCR_OVERRUN_FUNCTION:  an overrun occurred while trying to store the data
*                 _ERROR      element, the overrun was handled as specified by
*                 _IGNORE     the return value
*                 _OVERWRITE 
*
*  REMARKS
*
*******************************************************************************/

__INLINE UInt32 dsmcr_client_last_write_state (UInt32  buffer_index,
                                               UInt32  client_index)
{
  return dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].last_write_state;
}


/*******************************************************************************
*
*  FUNCTION
*    dsmcr_client_connect
*
*
*  SYNTAX
*    UInt32 dsmcr_client_connect (UInt32  buffer_index,
*                                 UInt32  client_index) 
*
*  DESCRIPTION
*    Connect a client to its associated buffer. This call is not
*    required unless the client was explicitly disconnected from its
*    buffer before. With this call, the regular readData and overrun handling
*    behaviour will be restored. The client is being reset within this call,
*    therefore, a subsequent read data call will return "buffer empty".
*
*  PARAMETERS
*    buffer_index: index of the buffer, to which the client belongs
*    client_index: client index of the client to connect to its buffer
*
*  RETURNS
*    DSMCR_NO_ERROR
*
*  REMARKS
*
*******************************************************************************/

__INLINE UInt32 dsmcr_client_connect (UInt32 buffer_index,
                                      UInt32 client_index)
{
    rtlib_int_status_t int_status;

    /* Nothing to do if called for a client which is already connected. */
    if (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].is_connected == 1)
    {
        return DSMCR_NO_ERROR;        
    }
    
    RTLIB_INT_SAVE_AND_DISABLE(int_status);
    
    /* Reset the client by setting the read index to the write index. */
    dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].read_index =
        dsmcr_buffers.p_buffers[buffer_index].write_index;    
    
    /* Set the connected flag. */
    dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].is_connected = 1;
    
    RTLIB_INT_RESTORE(int_status);
    
    return DSMCR_NO_ERROR;    
}
                      
 /*******************************************************************************
*
*  FUNCTION
*    dsmcr_client_disconnect
*
*
*  SYNTAX
*    UInt32 dsmcr_client_disconnect (UInt32  buffer_index,
*                                    UInt32  client_index) 
*
*  DESCRIPTION
*    Disconnect a client from its associated buffer. A read data call for a 
*    disconnected client will always return "buffer empty". Overruns will not be 
*    handled for disconnected clients.
*
*  PARAMETERS
*    buffer_index: index of the buffer, to which the client belongs
*    client_index: client index of the client to disconnect from its buffer
*
*  RETURNS
*    DSMCR_NO_ERROR
*
*  REMARKS
*
*******************************************************************************/

                     
__INLINE UInt32 dsmcr_client_disconnect (UInt32 buffer_index,
                                         UInt32 client_index)
{
    rtlib_int_status_t int_status;

    /* Nothing to do if called for a client which is already disconnected. */
    if (dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].is_connected == 0)
    {
        return DSMCR_NO_ERROR;
    }
    
    RTLIB_INT_SAVE_AND_DISABLE(int_status);
    
    /* Reset the connected flag. */
    dsmcr_buffers.p_buffers[buffer_index].p_clients[client_index].is_connected = 0;
    
    RTLIB_INT_RESTORE(int_status);
    
    return DSMCR_NO_ERROR;    
}

#undef __INLINE
