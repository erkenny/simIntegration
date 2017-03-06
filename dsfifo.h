/******************************************************************************
*
* FILE:
*   dsfifo.h
*
* RELATED FILES:
*   dsfifo.c
*
* DESCRIPTION:
*   Implementation of a simple bytewise FIFO.
*
*   Features: 
*   FIFO size: 2.. END OF MEMORY
*   FIFO mode: overflow or blocked
*
* HISTORY: OJ initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dsfifo.h $ $Revision: 1.2 $ $Date: 2003/08/07 12:12:26GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSFIFO.pj $
******************************************************************************/

#ifndef __dsfifo__
#define __dsfifo__

#include <Dstypes.h>

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
#elif defined (_DS1003)
  #ifdef _INLINE
    #define __INLINE static inline
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
#else
  #error ERROR: No target defined.
#endif

/******************************************************************************
  error codes
******************************************************************************/
#define DSFIFO_NO_ERROR            0
#define DSFIFO_BUFFER_OVERFLOW   202

/******************************************************************************
* constant definitions
******************************************************************************/
#define DSFIFO_NORMAL_MODE         0

/******************************************************************************
* type definitions
******************************************************************************/

typedef struct 
{
/* public  */
  unsigned int      error;

/* private */
  unsigned int      wr_p;
  unsigned int      rd_p;
  unsigned int      size;
  unsigned int      mode;  
  unsigned char*    buffer;                      
} dsfifo_t;

/******************************************************************************
  function prototypes
******************************************************************************/
dsfifo_t*             dsfifo_init ( const unsigned int size, const unsigned int mode );
                 void dsfifo_free         ( dsfifo_t* fifo );
                 void dsfifo_clear        ( dsfifo_t* fifo );
__INLINE unsigned int dsfifo_in_count_get ( dsfifo_t* fifo );
__INLINE unsigned int dsfifo_write        ( dsfifo_t* fifo, const unsigned int count, const unsigned char* data );
__INLINE unsigned int dsfifo_read         ( dsfifo_t* fifo, const unsigned int count,       unsigned char* data );
__INLINE unsigned int dsfifo_read_term    ( dsfifo_t* fifo, const unsigned int count,       unsigned char* data,
                                                                  unsigned char term );
/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/
#undef __INLINE

#ifdef _INLINE
#include <dsfifo.c>
#endif

#endif /* dsfifo.h */
