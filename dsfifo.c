/******************************************************************************
*
* FILE:
*   dsfifo.c
*
* RELATED FILES:
*   dsfifo.h
*
* DESCRIPTION:
*
* HISTORY: OJ initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsfifo.c $ $Revision: 1.2 $ $Date: 2003/08/07 12:12:26GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSFIFO.pj $
******************************************************************************/

#include <stdlib.h>
#include <Dsmsg.h>
#include <dsstd.h>
#include <math.h>
#include <dsfifo.h>

#if defined (_DS1103)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1104)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1401)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1005)
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE 
  #endif
#elif defined (_DS1006)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE 
  #endif
#elif defined (_DS1003)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#else
  #error ERROR: No target defined.
#endif


/******************************************************************************
  function declarations
******************************************************************************/
/******************************************************************************
*
* DESCRIPTION:      This function allocates a new fifo object. 
*
* PARAMETERS:
*
*   size            Size of the FIFO in byte.
*                   The size must be power of two. 
*
*   mode            DSFIFO_NORMAL_MODE or 1..(size-1).
*
*                   In the normal mode the fifo is overflow protected. 
*                   In the other mode the the contents of the fifo can be
*                   overwritten in blocks of the size of mode.
*
* RETURNS:          dsfifo_t*  Pointer to the allocated fifo object.
*
******************************************************************************/

#ifndef _INLINE
dsfifo_t* dsfifo_init( const unsigned int size, const unsigned int mode )
{
 unsigned int sizen = size , i;

 dsfifo_t* fifo = (dsfifo_t*) malloc(sizeof( dsfifo_t ));

 for (i = 0; sizen != 0;)
 {
   sizen = sizen >> 1; i++;
 }
 sizen = 1 << (i-1);
 
 if ( fifo != NULL )
 {
   fifo->wr_p   = 0;
   fifo->rd_p   = 0;
   fifo->size   = sizen;
   fifo->mode   = mode;
   fifo->error  = DSFIFO_NO_ERROR;
   fifo->buffer = (unsigned char*) malloc(sizeof(unsigned char) * size);

   if ( fifo->buffer == NULL )
   {
     free(fifo);
     return(NULL);
   }
 }

 dsfifo_clear(fifo);

 return(fifo);
} 
#endif

/******************************************************************************
*
* DESCRIPTION:   Clears FIFO buffer und resets the read and write pointer.
*
* PARAMETERS:
*   fifo         Pointer to the fifo object.
*
* RETURNS:       void
*
******************************************************************************/

#ifndef _INLINE
void dsfifo_clear( dsfifo_t* fifo )
{
 unsigned int i;

 if ( fifo != NULL )
 {
   fifo->wr_p       = 0;
   fifo->rd_p       = 0;
   fifo->error      = 0;

   for ( i = 0; i < fifo->size ; i++)
   {
    fifo->buffer[i] = 0;
   }
 }
}
#endif

/******************************************************************************
*
* DESCRIPTION:   Frees the memory allocated prior with the dsfifo_init(...).
*
* PARAMETERS:
*   fifo         Pointer to the fifo object to be freed.
*
* RETURNS:       void
*
******************************************************************************/

#ifndef _INLINE
void dsfifo_free( dsfifo_t* fifo )
{
 if ( fifo != NULL )
 {
   if ( fifo->buffer != NULL )
   {
    free (fifo->buffer);
   }
   
  free (fifo);
 } 
}
#endif


/******************************************************************************
*
* DESCRIPTION:    Returns the number of bytes in the FIFO.
*
* PARAMETERS:
*   fifo          Pointer to the fifo object.
*
* RETURNS:        Returns the number of bytes in the FIFO.
*
******************************************************************************/

__INLINE unsigned int dsfifo_in_count_get( dsfifo_t* fifo )
{
 return ((fifo->wr_p - fifo->rd_p) & ( fifo->size - 1));
}

/******************************************************************************
*
* DESCRIPTION:   This function writes max count bytes to the FIFO.
*
* PARAMETERS:
*   fifo         Pointer to the fifo object.
*   count        Max number of bytes to write.
*   data         Pointer to the source buffer.
*
* RETURNS:       Number of written bytes into the fifo.
*
******************************************************************************/

__INLINE unsigned int dsfifo_write( dsfifo_t* fifo, const unsigned int count, 
                                                    const unsigned char* data)
{
 unsigned int avail_space, in_count, i;

 in_count = dsfifo_in_count_get(fifo);   
 
 avail_space = fifo->size - in_count - 1;

 if (avail_space == 0)
 {
   fifo->error = DSFIFO_BUFFER_OVERFLOW;
 }

 if (avail_space >= count )
 {
   avail_space = count;
 }
 /* move read pointer */ 
 else if( fifo->mode > DSFIFO_NORMAL_MODE)
 {
    fifo->rd_p = ((unsigned int)
    (fifo->mode * ceil((double)(count-avail_space)/(double)fifo->mode ))
    +(fifo->rd_p) & (fifo->size - 1) );
  
    fifo->error = DSFIFO_BUFFER_OVERFLOW;
    
    avail_space = count;
 }
 
 /* copy data */
 for (i = 0; i < avail_space; i++)
 {
    fifo->buffer[fifo->wr_p] = (*data++);
    fifo->wr_p = (fifo->size - 1) & (fifo->wr_p + 1);
 }

 return (avail_space);
}

/******************************************************************************
*
* DESCRIPTION:   This function reads max count bytes from the FIFO.
*
* PARAMETERS:
*   fifo         Pointer to the fifo object.
*   count        Max number of bytes to read.
*   data         Pointer to the destination buffer.
*
* RETURNS:       Number of returned bytes from the fifo.
*
******************************************************************************/

__INLINE unsigned int dsfifo_read( dsfifo_t* fifo, const unsigned int count, 
                                                         unsigned char* data)
{
   unsigned int in_count = 0, i;
   rtlib_int_status_t int_status;  

   RTLIB_INT_SAVE_AND_DISABLE(int_status);
   
   in_count = dsfifo_in_count_get(fifo); 

   if (count < in_count )
   {
     in_count = count;
   }
 
   for (i = 0; i < in_count; i++)
   {
     (*data++) = fifo->buffer[fifo->rd_p];
     fifo->rd_p = (fifo->size - 1) & (fifo->rd_p + 1);
   }

   RTLIB_INT_RESTORE(int_status);
 
   return (in_count);
}

/******************************************************************************
*
* DESCRIPTION:   This function reads from the FIFO until count is reached
*                or the terminating character occurs in the FIFO. 
*                When the terminating character is found in the FIFO
*                the terminating character is also stored in the data buffer.
*
* PARAMETERS:
*   fifo         Pointer to the fifo object.
*   count        Max number of bytes to read including the terminating char.
*   data         Pointer to the destination buffer.
*   term         Termininate the function with character e.g: 
*                '\n' New line
*                '\r' Carriage return
*
* RETURNS:       Number of returned bytes from the fifo.
*
******************************************************************************/

__INLINE unsigned int dsfifo_read_term    ( dsfifo_t* fifo, const unsigned int   count,
                                                                  unsigned char* data,
                                                                  unsigned char  term )
{
   unsigned int in_count = 0, i;
   rtlib_int_status_t int_status;  

   RTLIB_INT_SAVE_AND_DISABLE(int_status);
   
   in_count = dsfifo_in_count_get(fifo); 

   if (count < in_count )
   {
     in_count = count;
   }
 
   for (i = 0; i < in_count; i++)
   {
     (*data) = fifo->buffer[fifo->rd_p];
     fifo->rd_p = (fifo->size - 1) & (fifo->rd_p + 1);
     
     if (*data == term)
     {
      RTLIB_INT_RESTORE(int_status);
      return (i+1);
     }
     else
     {
      (*data++);
     }
   }

   RTLIB_INT_RESTORE(int_status);
 
   return (i);
}



#undef __INLINE
