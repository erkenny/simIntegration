/******************************************************************************
*
* FILE:
*   dslist.h
*
* RELATED FILES:
*   dslist.c
*
* DESCRIPTION:
*
*
* HISTORY:
*   15.03.2000  Rev 1.0  OJ
*
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dslist.h $ $Revision: 1.7 $ $Date: 2009/07/01 16:10:25GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSLIST.pj $
******************************************************************************/

#ifndef __dslist__
#define __dslist__

#include <stdlib.h>
#include <Dstypes.h>

#if defined (_DS1103)
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
#elif defined (_DS1603)
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
#elif defined (_DS2510)
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
#elif defined (_DSHOST)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#else
  #error ERROR: No target defined.
#endif

/******************************************************************************
* constant definitions
******************************************************************************/

/* list mode */

#define DSLIST_LIST        0x00 /* configure the list as double linked list  */
#define DSLIST_QUEUE       0x01 /* configure the list as double linked queue */
#define DSLIST_STACK       0x02 /* not implemented yet */
#define DSLIST_RESERVED0   0x04 /* reserved */
#define DSLIST_RESERVED1   0x08 /* reserved */
#define DSLIST_RESERVED3   0x10 /* reserved */
#define DSLIST_RESERVED4   0x20 /* reserved */
#define DSLIST_RESERVED5   0x40 /* reserved */

/* queue mode */

#define DSLIST_BLOCKED     0x00 /* default value for queues */
#define DSLIST_OVERRUN     0x80 /* when the list is configured as queue this
                                   mode allows a fifo overrun */

/******************************************************************************
  error codes
******************************************************************************/

#define DSLIST_NO_ERROR      0 /* no error */
#define DSLIST_ERROR         1 /* the operation is aborted with no success */
#define DSLIST_ALLOC_ERROR   2 /* memory allocation error */

/* return codes from dslist_check() or dslist_queue_check() */

#define DSLIST_ERROR10      10 /* invalid last node pointer */
#define DSLIST_ERROR11      11 /* invalid current node pointer */
#define DSLIST_ERROR12      12 /* invalid last node pointer */
#define DSLIST_ERROR13      13 /* invalid last node pointer: Not in list,    */
#define DSLIST_ERROR14      14 /* invalid last node pointer: NULL            */
#define DSLIST_ERROR15      15 /* invalid current node pointer: Not in list, */
                               /*      or link error before current node     */
#define DSLIST_ERROR16      16 /* NULL current node pointer                  */
#define DSLIST_ERROR17      17 /* NULL first node pointer                    */
                               /*      or error in head node                 */
#define DSLIST_ERROR18      18 /* node not found in list */
#define DSLIST_ERROR19      19 /* node == NULL */
#define DSLIST_ERROR20      20 /* invalid list->mode */
#define DSLIST_ERROR21      21 /* NULL == list->current */
#define DSLIST_ERROR22      22 /* invalid list->level or list->listsize */
#define DSLIST_ERROR23      23 /* NULL == list, invalid list */

/******************************************************************************
* type definitions
******************************************************************************/

typedef struct dsListNode dsListNode;

struct dsListNode
{
    struct dsListNode* next;    /* points to the next node                   */
    struct dsListNode* prev;    /* points to the previous node               */
    unsigned itemsize;          /* size of data in bytes                     */
    void* data;                 /* pointer to the data                       */
};

typedef struct
{
    struct dsListNode* current;  /* points to the actual current node        */
    struct dsListNode* first;    /* always points to dummy head node         */
    struct dsListNode* last;     /* always points to dummy tail node         */
    unsigned listsize;           /* number of items in the list              */
    unsigned mode;               /* mode of the list: DSLIST_LIST,  ...      */
    unsigned blocked;            /* mode of the queue: overrun or blocked    */
    unsigned level;              /* Count of elements with data in the list. */
}                                /* Used for queues. */
 dsList;

 typedef int (*dsListCompareFunc)( const void *, const void * );

/******************************************************************************
  function prototypes
******************************************************************************/

/* ---- list management and maintenance -------------------------- */

dsList* dslist_init( const int mode );
           /* Creates an empty list. */

int dslist_check( dsList* list );
           /* Use this function for debugging. */

void dslist_clear( dsList* list );
           /* Removes all items from the list. */

void dslist_clear_data( dsList* list );
           /* Clears only the data of each item in the list */

void dslist_free( dsList* list );
           /* Removes all items from the list and destroys the list. */

__INLINE int dslist_size( dsList* list );
           /* Returns the number of items in the list. */

__INLINE int dslist_item_size ( dsList* list );
           /* Returns the number of data bytes in the current item. */

/* ---- Item management --------------------------------------------------*/

/* Functions NOT changing the current node pointer. */

__INLINE int dslist_item_prepend_from( dsList* list, const void* source, const size_t size );
            /* Inserts a new item as FIRST node */

__INLINE int dslist_item_append_from( dsList* list, const void* source, const size_t size );
            /* Inserts the data at the END of the list. */

/* Functions changing current node pointer to the new node. */

__INLINE int dslist_item_insert_from( dsList* list, const void* source, const size_t size );
            /* Inserts a new item BEFORE current node and
               makes the new node to the current node  */

__INLINE int dslist_item_insert_from_sorted( dsList* list, const void* source,
            const size_t size, dsListCompareFunc compare );
            /* Inserts a new item at its sorted position in the presorted list. */

__INLINE int dslist_item_find( dsList* list, void* key, dsListCompareFunc compare);
            /* Finds the first occurrence of key in the List
               using the compare function. */

__INLINE int dslist_item_find_next( dsList* list, void* key, dsListCompareFunc compare);
            /* Finds the next occurrence of key in the List
               using the compare function, starting from the current list item. */

__INLINE void dslist_item_delete( dsList* list );
            /* Removes the current list item. */

/* ---- Node management --------------------------------------------------*/

/* Functions NOT changing the current node pointer. */

__INLINE int dslist_node_prepend_from( dsList* list, struct dsListNode* node );
            /* Inserts the node as FIRST node */

__INLINE int dslist_node_append_from( dsList* list, struct dsListNode* node );
            /* Inserts the node at the END of the list. */

/* Functions changing current node pointer to the new node. */

__INLINE int dslist_node_insert_from( dsList* list, struct dsListNode* node );
            /* Inserts the node BEFORE the current node and
               makes the inserted node to the current node  */

__INLINE struct dsListNode* dslist_node_take( dsList* list );
            /* Takes the current list item from the list without deleting them. */

__INLINE void dslist_node_remove( dsList* list, dsListNode* node );

__INLINE void dslist_node_free( struct dsListNode* node );
            /* Frees a node, previous taken from a list with the
               function dslist_node_take() */

/* ---- current node pointer management ----------------------------------- */

__INLINE int dslist_first( dsList* list );
            /* Changes the current node pointer to the first node */

__INLINE int dslist_last( dsList* list );
            /* Changes the current node pointer to the last node */

__INLINE int dslist_next( dsList* list );
            /* Changes the current node pointer to the next node */

__INLINE int dslist_prev( dsList* list );
            /* Changes the current node pointer to the previous node */

__INLINE dsListNode* dslist_current_get( dsList* list );

/* ---- list data management ------------------------------------------------*/

__INLINE int dslist_write( dsList* list, const void* source );
           /* Writes the data from source to the current item in the list. */

__INLINE int dslist_read( dsList* list, void* destination );
           /* Reads the data of the current item from the list to destination. */

__INLINE void* dslist_read_ptr( dsList* list );
           /* Returns a pointer to the data of the current item in the list. */
           /* This functions is used for optimizations. */
           /* Use this function only if you know what you do ! */


/* ---- queue management and maintenance ----------------------------------- */

dsList* dslist_queue_init ( const int mode, const int itemcount, const size_t itemsize );
            /* Creates an empty queue with fixed datasize. */

#define dslist_queue_check(list)       dslist_check(list)
           /* Use this function for debugging. */

#define dslist_queue_clear_data(list)       dslist_clear_data(list)
           /* Clears the data of each item in the queue */

void dslist_queue_free( dsList* list );
           /* Removes all items from the queue and destroys the queue. */

#define dslist_queue_size(list)        dslist_size_get(list)
           /* Returns the max number of items in the queue. */

__INLINE int dslist_queue_level( dsList* list );
           /* Returns the number of actually stored items in the queue. */

/* ---- queue data management -----------------------------------------------*/
/*  The functions for the queue data management are moving the
    the last and first node pointer to achieve a FIFO behaviour !            */

__INLINE int dslist_queue_write( dsList* list, const void* source );
           /* Writes source to the end of the queue. */

__INLINE int dslist_queue_read( dsList* list, void* destination );
           /* Reads the front item from the queue to destination. */

__INLINE void* dslist_queue_read_ptr( dsList* list );
           /* Returns a pointer to the data of the first item in the queue. */
           /* This functions is used for optimizations. */
           /* Use this function only if you know what you do ! */

/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/
#undef __INLINE

#ifdef _INLINE
#include <dslist.c>
#endif

#endif /* dslist.h */
