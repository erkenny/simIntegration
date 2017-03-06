/***************************************************************************
*
*  MODULE
*    DSMCR - Multi Client Ringbuffer
*
*  FILE
*    dsmcr.h
*
*  RELATED FILES
*    dsmcr.c
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
*  $RCSfile: dsmcr.h $ $Revision: 1.5 $ $Date: 2008/12/08 16:44:25GMT+01:00 $
***************************************************************************/
#ifndef __DSMCR_H__
#define __DSMCR_H__

/***************************************************************************
  constant, macro and type definitions
***************************************************************************/

#define DSMCR_NO_ERROR 0

#define DSMCR_ALREADY_INITIALIZED 1

#define DSMCR_BUFFER_EMPTY 1

#define DSMCR_INVALID_BUFFER_INDEX 0xFFFFFFFF
#define DSMCR_INVALID_CLIENT_INDEX 0xFFFFFFFF

#define DSMCR_OVERRUN_FUNCTION  4
#define DSMCR_OVERRUN_ERROR     3
#define DSMCR_OVERRUN_IGNORE    2
#define DSMCR_OVERRUN_OVERWRITE 1

/* DSMCR message error codes and messages */
#define DSMCR_MSG_INITIALIZATION_MISSING     210
#define DSMCR_MSG_INITIALIZATION_MISSING_TXT "DSMCR initialization missing."

#define DSMCR_MSG_INVALID_BUFFER_INDEX       211
#define DSMCR_MSG_INVALID_BUFFER_INDEX_TXT   "Invalid buffer index (%d)."

#define DSMCR_MSG_MAX_BUFFERS                212
#define DSMCR_MSG_MAX_BUFFERS_TXT            "dsmcr_buffer(): Maximum number of buffers reached."

#define DSMCR_MSG_MAX_CLIENTS                213
#define DSMCR_MSG_MAX_CLIENTS_TXT            "dsmcr_client(): Maximum number of clients reached (%d)."

#define DSMCR_MSG_OVERRUN                    214
#define DSMCR_MSG_OVERRUN_TXT                "dsmcr_write(): Overrun for client %d, buffer %d"

/* type definitions */
typedef UInt32 (*dsmcr_t_p_overrun_fcn)
                 (UInt32  buffer_index,
                  UInt32  client_index,
                  void   *p_data);

typedef struct{
  UInt32                read_index;
  int                   overrun_policy;
  dsmcr_t_p_overrun_fcn p_overrun_fcn;
  UInt32                last_write_state;
  int                   is_connected;
} dsmcr_t_client_data;

typedef struct{
  void                *p_buffer;
  UInt32               write_index;
  UInt32               element_size;
  UInt32               number_of_elements;
  UInt32               number_of_clients;
  dsmcr_t_client_data *p_clients;
} dsmcr_t_buffer_data;

typedef struct{
  UInt32               number;
  dsmcr_t_buffer_data *p_buffers;
} dsmcr_t_buffer_descriptor;


/***************************************************************************
  object declarations
***************************************************************************/


/***************************************************************************
  function prototypes
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

UInt32 dsmcr_init ();

UInt32 dsmcr_buffer (UInt32  element_size,
                     UInt32  number_of_elements,
                     UInt32 *p_buffer_index);

UInt32 dsmcr_client (UInt32                 buffer_index,
                     UInt32                 overrun_policy,
                     dsmcr_t_p_overrun_fcn  p_overrun_fcn,
                     UInt32                *p_client_number);

UInt32 dsmcr_clear (UInt32 buffer_index);

__INLINE UInt32 dsmcr_write (UInt32  buffer_index,
                             void   *p_data);

__INLINE UInt32 dsmcr_read (UInt32  buffer_index,
                            UInt32  client_number,
                            void   *p_data);

__INLINE UInt32 dsmcr_client_last_write_state (UInt32  buffer_index,
                                               UInt32  client_index);

__INLINE UInt32 dsmcr_client_connect (UInt32 buffer_index,
                                      UInt32 client_index);

__INLINE UInt32 dsmcr_client_disconnect (UInt32 buffer_index,
                                         UInt32 client_index);


#undef __INLINE
/***************************************************************************
  inclusion of source file(s) for inline expansion
***************************************************************************/

#ifdef _INLINE
#include <dsmcr.c>
#endif

#endif /* __DSMCR_H__ */
