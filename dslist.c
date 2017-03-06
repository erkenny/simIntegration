/******************************************************************************
*
* FILE:
*   dslist.c
*
* RELATED FILES:
*   dslist.h
*
* DESCRIPTION:
*
* Implementation of a double linked list with variable sizeed data-items.
*
* HISTORY:
*   15.03.2000  Rev 1.0  OJ
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dslist.c $ $Revision: 1.11 $ $Date: 2009/07/09 11:40:37GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSLIST.pj $
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <dsstd.h>
#include <dslist.h>

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

dsList* dslist_init_internal( const int mode, const size_t itemsize );

dsList* dslist_init_items_internal( const int mode, const int itemcount, const size_t itemsize );

struct dsListNode* dslist_node_malloc( const size_t itemsize );

__INLINE int dslist_check_list( dsList* list );

__INLINE int dslist_check_queue( dsList* list );

/******************************************************************************
*
* DESCRIPTION:      This function creates an empty double linked list
*                   for variable sized data-items.
*
*                   The list uses a dummy head and a dummy tail node to
*                   reference the first and last node.
*
*                              dummy head      dummy tail
*
*                          next .------.  next .------.  next
*                         ----> | Node | ----> | Node | ----> NULL
*                    NULL <---- |      | <---- |      | <----
*                          prev '------'  prev '------'  prev
*
*
* PARAMETERS:
*
*   mode            DSLIST_LIST,  creates a double linked list
*                   Please use this macro to initialize your list.
*                   In the future there will be more options available
*                   to initialize your list.
*
* RETURNS:          dsList*  Pointer to the allocated list.
*
******************************************************************************/

#ifndef _INLINE

dsList* dslist_init( const int mode )
{
  /* the overrun mode dosn't exist for a list */

  int temp = mode & ~DSLIST_OVERRUN;

  return dslist_init_internal( temp, 0 );
}

#endif

/******************************************************************************
*
* DESCRIPTION:    Validates the list. Especially for debugging purposes.
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:        DSLIST_ERROR10  10  invalid last node pointer
*                 DSLIST_ERROR11  11  invalid current node pointer
*                 DSLIST_ERROR12  12  invalid last node pointer
*                 DSLIST_ERROR13  13  invalid last node pointer: Not in list,
*                 DSLIST_ERROR14  14  invalid last node pointer: NULL
*                 DSLIST_ERROR15  15  invalid current node pointer: Not in list,
*                                             or link error before current node
*                 DSLIST_ERROR16  16  NULL current node pointer
*                 DSLIST_ERROR17  17  NULL first node pointer or error
*                                             in head node
*                 DSLIST_ERROR18  18  node not found in list
*                 DSLIST_ERROR19  19  node == NULL
*                 DSLIST_ERROR20  20  invalid list->mode
*                 DSLIST_ERROR21  21  NULL == list->current
*                 DSLIST_ERROR22  22  invalid list->level or list->listsize
*                 DSLIST_ERROR23  23  NULL == list, invalid list
*
******************************************************************************/

#ifndef _INLINE

int dslist_check( dsList* list )
{
    if( NULL == list )
    {
      return DSLIST_ERROR23; /* the list is NULL */
    }

    if(  list->level > list->listsize)
    {
      return DSLIST_ERROR22;   /* error in list->level or list->listsize */
    }

    /* Validate current pointer */

    if( NULL == list->current )
    {
      return DSLIST_ERROR21;
    }

    switch( list->mode )
    {
      case DSLIST_LIST:
        return dslist_check_list( list );
      case DSLIST_QUEUE:
        return dslist_check_queue( list );
      default:
        return DSLIST_ERROR20; /* unknown list->mode */
    }
}

#endif

/******************************************************************************
*
* DESCRIPTION:   Removes and frees all items from the list.
*                The Result is an empty list.
*
* PARAMETERS:
*   list         Pointer to the list.
*
* RETURNS:       void
*
******************************************************************************/

#ifndef _INLINE

void dslist_clear( dsList* list )
{
    struct dsListNode* Tmp;
    struct dsListNode* Old;

    rtlib_int_status_t int_status;

    RTLIB_INT_SAVE_AND_DISABLE(int_status);

    Tmp = list->first->next;

    while( (NULL != Tmp->next)  )  /* last node has a NULL next pointer */
    {
      Old = Tmp;
      Tmp = Old->next;
      dslist_node_free( Old );
    }

    rtlib_free( list );

    RTLIB_INT_RESTORE(int_status);

    return;
}

#endif

/******************************************************************************
*
* DESCRIPTION:   Clears only the data of each item in the list.
*
* PARAMETERS:
*   list         Pointer to the list.
*
* RETURNS:       void
*
******************************************************************************/

#ifndef _INLINE

void dslist_clear_data( dsList* list )
{
  unsigned int i, n;

  struct dsListNode* node;

  if( NULL != list )
  {
    if( NULL == list->first->next->next ) /* empty list ? */
    {
      return;
    }

    /* note: current->next will never be NULL in a non-empty list */

    node = list->first;

    /* clear all items including the dummy head and dummy tail node */

    for ( i = 0; i < (list->listsize + 2 ); i++)
    {
      if ( NULL == node )
      {
        return;
      }

      for ( n = 0; n < node->itemsize; n++)
      {
        ((unsigned char*)(node->data))[n] = 0;
      }

      node = node->next;
    }

    /* reset the counter for the queue level */

    list->level = 0;

  }

  return;
}

#endif

/******************************************************************************
*
* DESCRIPTION:   Frees the memory allocated prior with the function
                 dslist_init(...)
*
* PARAMETERS:
*   list         Pointer to the list to be freed.
*
* RETURNS:       void
*
******************************************************************************/

#ifndef _INLINE

void dslist_free( dsList* list )
{
    struct dsListNode* Tmp;
    struct dsListNode* Old;

    rtlib_int_status_t int_status;

    if( NULL != list)
    {
      RTLIB_INT_SAVE_AND_DISABLE(int_status);

      Tmp = list->first;

      while( (NULL != Tmp)  )  /* last node has a NULL next pointer */
      {
        Old = Tmp;
        Tmp = Old->next;
        dslist_node_free( Old );
      }

      rtlib_free( list );

      RTLIB_INT_RESTORE(int_status);
    }

    return;
}

#endif

/******************************************************************************
*
* DESCRIPTION:    Returns the number of items in the list.
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:        The number of items in the list.
*
******************************************************************************/

__INLINE int dslist_size( dsList* list )
{
  return ( (int) list->listsize );
}

/******************************************************************************
*
* DESCRIPTION:    Returns the number of data bytes in the current item.
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:        Number of data bytes in the current item.
*
******************************************************************************/

__INLINE int dslist_item_size( dsList* list )
{
  return ( (int) list->current->itemsize );
}

/******************************************************************************
*
* DESCRIPTION:    Inserts a new item as FIRST node. The current node
*                 is unchanged.
*
*                 Simplified Example:
*                 ---------------------
*                 list = { a, b, c, d }     (old)
*
*                 dslist_item_prepend_from( list, e, ...
*
*                 list = { e, a, b, c, d }  (new)
*
*
*                    The new first node is
*                    inserted here  ---+
*                                      |
*                                      |
*                                      v
*            next .------.  next    .------.     next .------. next
*           ----> | Node | -------> | Node | -------> | Node | ---->
*      NULL <---- |      | <------- |      | <------- |      | <----
*            prev '------'  prev    '------'     prev '------' prev
*
*
*                  dummy head    first node (new)     first node (old)
*
* PARAMETERS:
*
*   list          Pointer to the list.
*
*   source        Pointer to the source data buffer.
*
*   size          Size of the data buffer in bytes.
*
* RETURNS:        DSLIST_NO_ERROR
*                 DSLIST_ALLOC_ERROR
*
******************************************************************************/

__INLINE int dslist_item_prepend_from( dsList* list, const void* source, const size_t size )
{
    struct dsListNode* New;

    New = dslist_node_malloc( size );

    if( NULL == New )
    {
      return DSLIST_ALLOC_ERROR;
    }

    New->itemsize = size;

    memcpy( New->data, source, size );

    return dslist_node_prepend_from( list, New );
}

/******************************************************************************
*
* DESCRIPTION:    Inserts the data at the END of the list. The current node
*                 is unchanged.
*
*                 Simplified Example:
*                 ---------------------
*                 list = { a, b, c, d }    (old)
*
*                 dslist_item_append_from( list, e, ...
*
*                 list = { a, b, c, d, e } (new)
*
*
*                    The new last node is
*                    inserted here  ---+
*                                      :
*                                      :
*                                      v
*            next .------.  next   .------.  next   .------. next
*           ----> | Node | ------> | Node | ------> | Node | ---->
*      NULL <---- |      | <------ |      | <------ |      | <----
*            prev '------'  prev   '------'  prev   '------' prev
*
*
*            last node (old)     last node (new)    dummy tail
*
*
* PARAMETERS:
*
*   list          Pointer to the list.
*
*   source        Pointer to the source data buffer.
*
*   size          Size of the data buffer in bytes.
*
* RETURNS:        DSLIST_NO_ERROR
*                 DSLIST_ALLOC_ERROR
*
******************************************************************************/

__INLINE int dslist_item_append_from( dsList* list, const void* source, const size_t size )
{
    struct dsListNode* New ;

    New = dslist_node_malloc( size );

    if( NULL == New )
    {
      return DSLIST_ALLOC_ERROR;
    }

    New->itemsize = size;

    memcpy( New->data, source, size );

    return dslist_node_append_from( list, New );
}

/******************************************************************************
*
* DESCRIPTION:    Inserts a new item BEFORE current node and
*                 makes the new node to the current node.
*
*                 Simplified Example:
*                 ---------------------
*
*                 list->current = c;
*
*                 list = { a, b, c, d }     (old)
*
*                 dslist_item_insert_from( list, e, ...
*
*                 list->current = e;
*
*                 list = { a, b, e, c, d }  (new)
*
*
*                    .------.  list->current
*                    | list |-----------------------+
*                    |      |    :                  |
*                    '------'    :                  |
*                                : list->current    |
*                                : (new)            |
*                                :                  |
*                                v                  |
*                             .------.  next        | list->current
*                       ----> | Node | ---->        | (old)
*                       <---- |      | <----        |
*  <-                    prev '------'              |                ->
*  to first node                 :                  |       to last node
*                                :                  v
*      next .------. next        v               .------.  next
*     ----> | Node | ------------------------->  | Node | ---->
*     <---- |      | <-------------------------  |      | <----
*      prev '------'                        prev '------'  prev
*
*

* PARAMETERS:
*
*   list          Pointer to the list.
*
*   source        Pointer to the source data buffer.
*
*   size          Size of the data buffer in bytes.
*
* RETURNS:        DSLIST_NO_ERROR
*                 DSLIST_ALLOC_ERROR
*
******************************************************************************/

__INLINE int dslist_item_insert_from( dsList* list, const void* source, const size_t size )
{
    struct dsListNode* New ;

    New = dslist_node_malloc( size );

    if( NULL == New )
    {
      return DSLIST_ALLOC_ERROR;
    }

    New->itemsize = size;

    memcpy( New->data, source, size );

    return dslist_node_insert_from( list, New );
}

/******************************************************************************
*
* DESCRIPTION:    Inserts a new item at its sorted position in
*                 the presorted list.
*
*                 The sort order depends on the compare function.
*                 All items must be inserted with
*                 dslist_item_insert_from_sorted() to maintain the
*                 sorting order.
*
*                 The inserted item becomes the current list item.
*
*
*                 list = { a, b, d, e }     (old)
*
*                 dslist_item_insert_from_sorted( list, c, ...
*
*                 list->current = c;
*
*                 list = { a, b, c, d, e }  (new)
*
*
* PARAMETERS:
*
*   list          Pointer to the presorted list.
*
*   source        Pointer to the source data buffer.
*
*   size          Size of the data buffer in bytes.
*
*   compare       Pointer to the compare function.
*
* RETURNS:        DSLIST_NO_ERROR     (inserted the new item)
*                 DSLIST_ERROR        (compare == 0) || (source == 0)
*                 DSLIST_ALLOC_ERROR  (memory allocation error)
*
******************************************************************************/

__INLINE int dslist_item_insert_from_sorted( dsList* list, const void* source,
                              const size_t size, dsListCompareFunc compare )
{
    int error;

    if( NULL == compare )
    {
      return (DSLIST_ERROR);
    }

    if( NULL == source )
    {
      return (DSLIST_ERROR);
    }

    if( NULL == list->first->next->next ) /* empty list ? */
    {
      return dslist_item_insert_from( list, source, size );
    }
    else
    {
      dslist_first( list );
    }

    while( 0 != (error = (*compare)( source, list->current->data ))
             && (NULL != list->current->next->next) )
    {
      list->current = list->current->next;
    }

    if ( 0 != error )
    {
      return dslist_item_append_from( list, source, size );
    }
    else
    {
      return dslist_item_insert_from( list, source, size );
    }
}

/******************************************************************************
*
* DESCRIPTION:    Finds the first occurrence of key in the List
*                 using the compare function.
*
*                 If the item is found, the list sets the current item to
*                 to the found item.
*
*                 If the item is not found, the list sets the current item to
*                 the last item.
*
* PARAMETERS:
*
*   list          Pointer to the list.
*
*   key           Pointer to the search key.
*
*   compare       Pointer to the compare function
*
* RETURNS:        0 == equal == found.
*                 non-zero == not found.
*
******************************************************************************/

__INLINE int dslist_item_find( dsList* list, void* key, dsListCompareFunc compare)
{
    int error;

    if( NULL == list->first->next->next ) /* empty list ? */
    {
      return (DSLIST_ERROR);
    }

    if( NULL == compare )
    {
      return (DSLIST_ERROR);
    }

    dslist_first( list );

    while( 0 != (error = (*compare)( key, list->current->data ))
             && (NULL != list->current->next->next) )
    {
      list->current = list->current->next;
    }

    return (error);
}

/******************************************************************************
*
* DESCRIPTION:    Finds the next occurrence of key in the List
*                 using the compare function, starting from the current
*                 list item.
*
*                 If the item is found, the list sets the current item to
*                 to the found item.
*
*                 If the item is not found, the list sets the current item to
*                 the last item.
*
* PARAMETERS:
*
*   list          Pointer to the list.
*
*   key           Pointer to the search key.
*
*   compare       Pointer to the compare function
*
* RETURNS:        0 == equal == found.
*                 non-zero == not found.
*
******************************************************************************/

__INLINE int dslist_item_find_next( dsList* list, void* key, dsListCompareFunc compare)
{
    int error;

    if( NULL == list->first->next->next ) /* empty list ? */
    {
      return (DSLIST_ERROR);
    }

    if( NULL == compare )
    {
      return (DSLIST_ERROR);
    }

    while( 0 != (error = (*compare)( key, list->current->data ))
             && (NULL != list->current->next->next) )
    {
      list->current = list->current->next;
    }

    return (error);
}

/******************************************************************************
*
* DESCRIPTION:    Removes the current list item. The new current node
*                 is the next node after the old current node.
*
*
*                    .------.  list->current
*                    | list |------------------------+
*                    |      |    :                   |
*                    '------'    :                   |
*                                :                   |
*                                :                   |
*                 list->current  :                   |
*                 (old)          :                   |
*                                :                   |
*                             .------.  next         | list->current
*                       ----> | Node | ---->         | (new)
*                       <---- |      | <----         |
*                        prev '------'               |
*                                                    |
*                             deleted node           |
*   <- to first node                                 |      to last node ->
*                                                    |
*                                                    v
*      next .------. next                         .------.  next
*     ----> | Node | ---- - - - - - - - - - --->  | Node | ---->
*     <---- |      | <--- - - - - - - - - - ----  |      | <----
*      prev '------'                        prev  '------'  prev
*
*
* PARAMETERS:
*
*   list          Pointer to the list.
*
* RETURNS:        void
*
******************************************************************************/

__INLINE void dslist_item_delete( dsList* list )
{
    dslist_node_free( dslist_node_take( list ) );

    return;
}

/******************************************************************************
*
* DESCRIPTION:    Inserts a node as FIRST node. The current node
*                 is unchanged.
*
*
*                    The node is
*                    inserted here  ---+
*                                      |
*                                      |
*                                      v
*            next .------.  next    .------.     next .------. next
*           ----> | Node | -------> | Node | -------> | Node | ---->
*      NULL <---- |      | <------- |      | <------- |      | <----
*            prev '------'  prev    '------'     prev '------' prev
*
*
*                  dummy head    first node (new)     first node (old)
*
* PARAMETERS:
*
*   list          Pointer to the list.
*
*   node          Pointer to the node to prepend.
*
* RETURNS:        DSLIST_NO_ERROR
*                 DSLIST_ERROR
*
******************************************************************************/

__INLINE int dslist_node_prepend_from( dsList* list, struct dsListNode* node )
{
    if( NULL == node )
    {
      return DSLIST_ERROR;
    }

    node->prev = list->first;
    node->next = list->first->next;

    node->next->prev  = node ;
    list->first->next = node;

    list->listsize++;
    list->level++;

    /* Prevent ->current from pointing at the dummy tail */
    if( NULL == list->current->next )
    {
      list->current = node;
    }

    return DSLIST_NO_ERROR;
}

/******************************************************************************
*
* DESCRIPTION:    Inserts the node at the END of the list. The current node
*                 is unchanged.
*
*
*                    The new last node is
*                    inserted here  ---+
*                                      :
*                                      :
*                                      v
*            next .------.  next   .------.  next   .------. next
*           ----> | Node | ------> | Node | ------> | Node | ---->
*      NULL <---- |      | <------ |      | <------ |      | <----
*            prev '------'  prev   '------'  prev   '------' prev
*
*
*            last node (old)     last node (new)    dummy tail
*
*
* PARAMETERS:
*
*   list          Pointer to the list.
*
*   node          Pointer to the node to append.
*
* RETURNS:        DSLIST_NO_ERROR
*                 DSLIST_ERROR
*
******************************************************************************/

__INLINE int dslist_node_append_from( dsList* list, struct dsListNode* node )
{
    if( NULL == node )
    {
      return DSLIST_ERROR;
    }

   /*
    *  Test if the node is already inserted.
    */
    if( node->prev || node->next )
    {
      return DSLIST_ERROR;
    }

    node->next     = list->last;
    node->prev     = list->last->prev;

    list->last->prev = node;
    node->prev->next = node;

    list->listsize++;
    list->level++;

    if( list->listsize == 1 )
    {
      list->current = node;
    }

    /* Prevent .current from pointing at the dummy tail
       (New is the only normal node...) */
    if( NULL == list->current->next )
    {
     list->current = node;
    }

    return (DSLIST_NO_ERROR);
}


/******************************************************************************
*
* DESCRIPTION:    Inserts a node BEFORE the current node and
*                 makes the inserted node to the current node.
*
*
*                    .------.  list->current
*                    | list |-----------------------+
*                    |      |    :                  |
*                    '------'    :                  |
*                                : list->current    |
*                                : (new)            |
*                                :                  |
*                                v                  |
*                             .------.  next        | list->current
*                       ----> | Node | ---->        | (old)
*                       <---- |      | <----        |
*  <-                    prev '------'              |                ->
*  to first node                 :                  |       to last node
*                                :                  v
*      next .------. next        v               .------.  next
*     ----> | Node | ------------------------->  | Node | ---->
*     <---- |      | <-------------------------  |      | <----
*      prev '------'                        prev '------'  prev
*
*

* PARAMETERS:
*
*   list          Pointer to the list.
*
*   node          Pointer to the node to insert.
*
* RETURNS:        DSLIST_NO_ERROR
*                 DSLIST_ERROR
*
******************************************************************************/

__INLINE int dslist_node_insert_from( dsList* list, struct dsListNode* node )
{
    if( NULL == node )
    {
      return DSLIST_ERROR;
    }

    node->next     = list->current;
    node->prev     = list->current->prev;

    list->current->prev = node;
    node->prev->next    = node;
    list->current       = node;

    list->listsize++;
    list->level++;

    return DSLIST_NO_ERROR;
}


/******************************************************************************
*
* DESCRIPTION:    Takes the current node out from a list without deleting the
*                 node. The new current node is the next node after the
*                 removed node.
*
*
*                    .------.  list->current
*                    | list |------------------------+
*                    |      |    :                   |
*                    '------'    :                   |
*                                :                   |
*                                :                   |
*                 list->current  :                   |
*                 (old)          :                   |
*                                :                   |
*                             .------.  next         | list->current
*                       ----> | Node | ---->         | (new)
*                       <---- |      | <----         |
*                        prev '------'               |
*                                                    |
*                             removed node           |
*   <- to first node                                 |      to last node ->
*                                                    |
*                                                    v
*      next .------. next                         .------.  next
*     ----> | Node | ---- - - - - - - - - - --->  | Node | ---->
*     <---- |      | <--- - - - - - - - - - ----  |      | <----
*      prev '------'                        prev  '------'  prev
*
*
* PARAMETERS:
*
*   list          Pointer to the list.
*
* RETURNS:        Pointer to the removed node.
*
******************************************************************************/

__INLINE struct dsListNode* dslist_node_take( dsList* list )
{
    struct dsListNode* Old = list->current ;

    if( list->level == 0 )
    {
      return NULL;  /* don't delete dummy tail node (list is empty) */
    }

    /* adjust links */

    Old->prev->next = Old->next ;
    Old->next->prev = Old->prev ;

    list->listsize--;
    list->level--;

    /* adjust current node pointer
       prevent it from pointing to the dummy tail node */

    if( NULL != Old->next->next )
    {
      list->current = Old->next ;
    }
    else
    {
      list->current = Old->prev ;
    }

    Old->prev = NULL;
    Old->next = NULL;

    return (Old);
}

__INLINE void dslist_node_remove( dsList* list, dsListNode* node )
{
  if( node != list->current )
  {
    /*
    *  Remove only when in a list.
    */
    if( node->prev && node->next )
    {
      node->prev->next = node->next ;
      node->next->prev = node->prev ;

      node->prev = NULL;
      node->next = NULL;

      list->listsize--;
      list->level--;
    }
    return;
  }
  else
  {
    dslist_node_take( list );

    return;
  }
}


/******************************************************************************
*
* DESCRIPTION:    Frees a node, previous taken from a list with the function
*                 dslist_node_take().
*
*                 NOTE:
*                 Use this function only if the node to be deleted is
*                 not a valid node in a valid list, otherwise your Application
*                 may crash.
*
* PARAMETERS:
*
*   node          Pointer to the node to be freed.
*
* RETURNS:        void
*
******************************************************************************/

__INLINE void dslist_node_free( struct dsListNode* node )
{
   rtlib_int_status_t int_status;

   RTLIB_INT_SAVE_AND_DISABLE(int_status);

   if ( NULL != node )
   {
     if ( NULL != node->data )
     {
       rtlib_free( node->data );
     }
     rtlib_free( node );
   }

   RTLIB_INT_RESTORE(int_status);
}


/******************************************************************************
*
* DESCRIPTION:    Changes the current node pointer to the first node.
*
*                 Simplified Example:
*                 ---------------------
*
*                 list = { a, b, c, d }
*
*                 dslist_first( list );
*
*                 list->current = a;
*
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:        DSLIST_NO_ERROR   (changed the current node pointer)
*                 DSLIST_ERROR      (can occur if the list is empty)
*
******************************************************************************/

__INLINE int dslist_first( dsList* list )
{
  list->current = list->first->next ;

  return (NULL == list->first->next->next);
}

/******************************************************************************
*
* DESCRIPTION:    Changes the current node pointer to the last node.
*
*                 Simplified Example:
*                 ---------------------
*
*                 list = { a, b, c, d }
*
*                 dslist_last( list );
*
*                 list->current = d;
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:        DSLIST_NO_ERROR    (changed the current node pointer)
*                 DSLIST_ERROR       (can occur if the list is empty)
*
******************************************************************************/

__INLINE int dslist_last( dsList* list )
{
  list->current = list->last->prev;

  return (NULL == list->last->prev->prev);
}

/******************************************************************************
*
* DESCRIPTION:    Changes the current node pointer to the next node.
*
*                 Simplified Example:
*                 ---------------------
*
*                 list = { a, b, c, d }
*
*                 list->current = b;
*
*                 dslist_next( list );
*
*                 list->current = c;
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:        DSLIST_NO_ERROR    (changed the current node pointer)
*                 DSLIST_ERROR       (you are already at the end of the list)
*
******************************************************************************/

__INLINE int dslist_next( dsList* list )
{
  if( NULL == list->current->next         /* empty list ? */
   || NULL == list->current->next->next ) /* at end ?*/
  {
    return (DSLIST_ERROR); /* do not allow the current node pointer */
  }                        /* to point at the dummy tail node ...   */

  list->current = list->current->next ;
  return (DSLIST_NO_ERROR);
}

/******************************************************************************
*
* DESCRIPTION:    Changes the current node pointer to the previous node.
*
*                 Simplified Example:
*                 ---------------------
*
*                 list = { a, b, c, d }
*
*                 list->current = c;
*
*                 dslist_prev( list );
*
*                 list->current = b;
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:        DSLIST_NO_ERROR     (changed the current node pointer)
*                 DSLIST_ERROR        (you are at the begin of the list)
******************************************************************************/

__INLINE int dslist_prev( dsList* list )
{
  if( NULL == list->current->prev         /* empty list ? */
   || NULL == list->current->prev->prev ) /* begin ? */
  {
    return (DSLIST_ERROR);   /* do not allow the current node pointer */
  }                          /* to point at the dummy head node ...   */

  list->current = list->current->prev ;
  return (DSLIST_NO_ERROR);
}

/******************************************************************************
*
* DESCRIPTION:   Writes the data from source to the current item in the list.
*
* PARAMETERS:
*
*   list         Pointer to the list.
*
*   source       Pointer to the source data buffer.
*
* RETURNS:       Size of the copied data in bytes.
*
******************************************************************************/

__INLINE int dslist_write( dsList* list, const void* source )
{
  if( NULL != source )
  {
    memcpy( list->current->data, source,
            list->current->itemsize );
  }

  return ( (int) list->current->itemsize );
}

/******************************************************************************
*
* DESCRIPTION:   Reads the data of the current item from the list
*                to destination.
*
* PARAMETERS:
*
*   list         Pointer to the list object.
*
*   destination  Pointer to the destination buffer.
*
* RETURNS:       Size of the copied data in bytes.
*
******************************************************************************/
__INLINE int dslist_read( dsList* list, void* destination )
{
  if( NULL != destination )
  {
    memcpy( destination, list->current->data,
            list->current->itemsize );
  }

  return ( (int) list->current->itemsize );
}

/******************************************************************************
*
* DESCRIPTION:   Returns a pointer to the data of the current item in the list.
*                This functions is used for optimizations.
*                Use this function only if you know what you do !
*
* PARAMETERS:
*
*   list         Pointer to the list.
*
* RETURNS:       Pointer to the item data.
*
******************************************************************************/

__INLINE void* dslist_read_ptr( dsList* list )
{
  return (list->current->data);
}


__INLINE dsListNode* dslist_current_get( dsList* list )
{
  if( list->listsize )
  {
    return list->current;
  }
  else
  {
    return NULL;
  }
}

/******************************************************************************
*
* DESCRIPTION:      This function creates a new double linked queue with
*                   itemcount items.
*                   All items have the same fixed data size.
*
*                   In the mode DSLIST_BLOCKED the queue is overflow
*                   protected.
*                   In the mode DSLIST_OVERRUN the items of the queue can be
*                   overwritten.
*
*
*                   .------.  next   .------.  next   .------. next
*           +-----> | Node | ------> | Node | ------> | Node | -------+
*           | +---- |      | <------ |      | <------ |      | <----+ |
*           | |     '------'  prev   '------'  prev   '------'      | |
*           | |                                                     | |
*           | |                                                     | |
*           | |                                                     | |
*           | |     .------.  prev   .------.    prev .------.      | |
*           | +---> | Node | ------> | Node | ------> | Node | -----+ |
*           +------ |      | <------ |      | <------ |      | <------+
*                   '------'         '------'         '------'
*
*
* PARAMETERS:
*
*   mode            DSLIST_BLOCKED or DSLIST_OVERRUN
*
*   itemcount       Number of items in the list.
*
*   itemsize        Size of an item in bytes.
*
* RETURNS:          dsList*  Pointer to the allocated queue.
*
******************************************************************************/

#ifndef _INLINE

dsList* dslist_queue_init ( const int mode, const int itemcount, const size_t itemsize )
{
  int temp = mode | DSLIST_QUEUE; /* allow only the queue modes */

  return dslist_init_items_internal( temp, itemcount, itemsize );
}

#endif

/******************************************************************************
*
* DESCRIPTION:    Returns the number of actually stored items in the queue.
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:        The number of items in the list.
*
******************************************************************************/

__INLINE int dslist_queue_level( dsList* list )
{
  if( NULL != list )
  {
    return ( (int) list->level );
  }
  else
  {
    return (0);
  }
}

/******************************************************************************
*
* DESCRIPTION:   Frees the memory allocated prior with the function
                 dslist_queue_init(...)
*
* PARAMETERS:
*   list         Pointer to the list to be freed.
*
* RETURNS:       void
*
******************************************************************************/

#ifndef _INLINE

void dslist_queue_free( dsList* list )
{
    if( NULL != list )
    {
      list->first->prev->next = NULL;

      dslist_free( list );
    }

    return;
}

#endif


/******************************************************************************
*
* DESCRIPTION:   Writes the data from source to the end of the queue.
*
*                NOTE:
*                The read and write functions for the queue are moving the
*                last and first node pointer to achieve a FIFO behaviour !
*
* PARAMETERS:
*
*   list         Pointer to the list.
*
*   source       Pointer to the source data buffer.
*
* RETURNS:       DSLIST_NO_ERROR
*                DSLIST_ERROR     (queue overrun)
*
******************************************************************************/

__INLINE int dslist_queue_write( dsList* list, const void* source )
{
   /* is queue filled ? */

   if( NULL != list )
   {
     if (list->listsize == 1)
     {
        if (list->level)
        {
            if (!list->blocked)
            {
                dslist_write(list, source);
            }

            return DSLIST_ERROR;
        }
        else
        {
            dslist_write(list, source);

            list->level++;

            return DSLIST_NO_ERROR;
        }
     }


     if (list->level == list->listsize)
     {
       if( !list->blocked )
       {
         list->current = list->first;

         dslist_write( list, source );

         list->first = list->first->next;

         /* move the last node to the next node */

         list->last = list->last->next;
       }

       return (DSLIST_ERROR);
     }
     else
     {
       list->current = list->first;

       dslist_write( list, source );

       list->first = list->first->next;

       list->level++;

       return (DSLIST_NO_ERROR);
     }
  }
  return (DSLIST_NO_ERROR);
}

/******************************************************************************
*
* DESCRIPTION:   Reads the front item from the queue to destination.
*
*                NOTE:
*                The read and write functions for the queue are moving the
*                last and first node pointer to achieve a FIFO behaviour !
*
* PARAMETERS:
*
*   list         Pointer to the list.
*
*   destination  Pointer to the destination buffer.
*
* RETURNS:       DSLIST_NO_ERROR  (Read one item from the queue.)
*                DSLIST_ERROR     (No data in queue.)
*
******************************************************************************/

__INLINE int dslist_queue_read( dsList* list, void* destination )
{
   /* if queue not empty */

   rtlib_int_status_t int_status;

   if( NULL == list )
   {
        return (DSLIST_ERROR);
   }

    RTLIB_INT_SAVE_AND_DISABLE(int_status);

    if (list->listsize == 1 && list->level)
    {
        dslist_read( list, destination );

        list->level--;

        RTLIB_INT_RESTORE(int_status);

        return (DSLIST_NO_ERROR);
    }

    if (list->level)
    {
        list->current = list->last->next;

        dslist_read( list, destination );

        list->last = list->last->next;

        list->level--;

        RTLIB_INT_RESTORE(int_status);

        return (DSLIST_NO_ERROR);
    }

    RTLIB_INT_RESTORE(int_status);

    return (DSLIST_ERROR);
 }

/******************************************************************************
*
* DESCRIPTION:   Returns a pointer to the data of the first item in the queue.
*                This functions is used for optimizations.
*
*                NOTE:
*                The interrupts are not disabled in this function!
*                The data which the returned pointer point to can change if a
*                write process to the  queue interrupts your actions onto
*                the pointer.
*
*                NOTE:
*                The read and write functions for the queue are moving the
*                last and first node pointer to achieve a FIFO behaviour !
*
* PARAMETERS:
*   list         Pointer to the list object.
*
* RETURNS:       pointer to data or NULL if no data available.
*
******************************************************************************/

__INLINE void* dslist_queue_read_ptr( dsList* list )
{
   if( list == 0)
   {
      return NULL;
   }
    /* if queue not empty */

    if (list->listsize == 1 && list->level)
    {
        list->level--;

        return dslist_read_ptr(list);
    }

    if (list->level)
    {
        void* ptr;

        list->current = list->last->next;

        ptr = dslist_read_ptr( list );

        list->last = list->last->next;

        list->level--;

        return (ptr);
    }

   return NULL;
}


/******************************************************************************

! INTERNAL USED FUNCTIONS ! INTERNAL USED FUNCTIONS ! INTERNAL USED FUNCTIONS !

******************************************************************************/
/******************************************************************************
*
* DESCRIPTION:      This function allocates a new list.
*
* PARAMETERS:
*
*   size            Size of the FIFO in byte.
*                   The size must be power of two.
*
*   mode
*
*                   In the normal mode the list is overflow protected.
*                   In the other mode the the contents of the list can be
*                   overwritten in blocks of the size of mode.
*
* RETURNS:          dsList*  Pointer to the allocated list object.
*
******************************************************************************/

#ifndef _INLINE

dsList* dslist_init_internal( const int mode, const size_t itemsize )
{
 dsList* list;
 struct dsListNode* last;
 struct dsListNode* first;

 /* create dummy head node */

 first = dslist_node_malloc( (unsigned) itemsize );

 if( NULL == first )
 {
   return NULL;
 }

 /* create dummy tail node */

 last  = dslist_node_malloc( (unsigned) itemsize );

 if( NULL == last )
 {
   dslist_node_free( first );

   return NULL;
 }

 /* create list entry point */

 list = (dsList*)rtlib_malloc(sizeof(dsList) );

 if( NULL == list )
 {
   dslist_node_free( first );
   dslist_node_free( last );

   return NULL;
 }

 /* extract the list mode */
 list->mode     = mode & 0x7F;
 list->blocked  = !(DSLIST_OVERRUN == (mode & 0x80));

 list->level    = 0;
 list->current  = last;
 list->first    = first;
 list->last     = last;

 switch( mode & 0x7F )
 {
   case DSLIST_QUEUE:
      list->listsize = 1;
      first->next    = last;
      first->prev    = last;
      last->next     = first;
      last->prev     = first;
    break;
   case DSLIST_LIST:
   default:           /* with no given mode assume list mode */
      list->listsize = 0;
      first->next    = last;
      first->prev    = NULL;
      last->next     = NULL;
      last->prev     = first;
    break;
 }

 return( list );
}
#endif

/******************************************************************************
*
* DESCRIPTION:      This function creates a new list with itemcount items.
*                   All items have the same data size of itemsize.
*
* PARAMETERS:
*
*   mode            DSLIST_LIST,  create a double linked list
*                   DSLIST_QUEUE, create a double linked queue
*
*   itemcount       Number of items in the list.
*
*   itemcount       Size of an item in byte.
*
* RETURNS:          dsList*  Pointer to the allocated list.
*
******************************************************************************/

#ifndef _INLINE

dsList* dslist_init_items_internal( const int mode, const int itemcount, const size_t itemsize )
{
   rtlib_int_status_t int_status;

   unsigned i;

   dsList* list;

   void* source;

   list = dslist_init_internal( mode, itemsize );

   if( NULL == list )
   {
     return NULL;
   }

   RTLIB_INT_SAVE_AND_DISABLE(int_status);

   source = rtlib_malloc((unsigned) itemsize );

   if( NULL == source )
   {
     if( DSLIST_QUEUE == list->mode )
     {
       dslist_queue_free( list );
     }
     else
     {
       dslist_free( list );
     }

     RTLIB_INT_RESTORE(int_status);

     return NULL;
   }

   for( i = list->listsize; i < (unsigned)itemcount; i++ )
   {
     if( DSLIST_ALLOC_ERROR == dslist_item_insert_from( list, source, itemsize ))
     {
       if( DSLIST_QUEUE == list->mode )
       {
         dslist_queue_free( list );
       }
       else
       {
         dslist_free( list );
       }

       rtlib_free( source );

       RTLIB_INT_RESTORE(int_status);

       return NULL;
     }
   }

   rtlib_free( source );

   list->level = 0;

   RTLIB_INT_RESTORE(int_status);

   return list;
}
#endif

/******************************************************************************
*
* DESCRIPTION:
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:
*
******************************************************************************/

__INLINE int dslist_check_node( dsList* list, struct dsListNode* node )
{
  int i = 0;

  struct dsListNode* tmp = node;

  if (NULL == node )
  {
     return DSLIST_ERROR19;
  }

  /* look for node in list,
     checking the prev links along the way  */
  do
  {
    tmp = tmp->next;

    if( (NULL == tmp) || (NULL == tmp->prev) ||
        (tmp  != tmp->prev->next) || ( (unsigned)i > list->listsize ) )
    {
      return DSLIST_ERROR18 ;        /* node not found in list */
    }                                /* or link to/from next node */
                                     /* invalid                   */
    i++;

  }while( tmp != list->current );

  return DSLIST_NO_ERROR;
}

/******************************************************************************
*
* DESCRIPTION:
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:
*
******************************************************************************/

__INLINE int dslist_check_queue( dsList* list )
{
  int error;

  /* look for current in list  */

  error = dslist_check_node( list, list->current );

  if (error) return (error);

  /* look for first in list  */

  error = dslist_check_node( list, list->first );

  if (error) return (error);

  /* look for last in list  */

  error = dslist_check_node( list, list->last );

  if (error) return (error);

  return (DSLIST_NO_ERROR);

}

/******************************************************************************
*
* DESCRIPTION:
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:
*
******************************************************************************/

__INLINE int dslist_check_list( dsList* list )
{
    if( NULL == list->first ||
        NULL == list->first->next ||   /* missing tail ? */
        NULL != list->first->prev )
    {
      return DSLIST_ERROR17;         /* error in head node */
    }

    if( NULL != list->first->next->next ) /* empty list ? */
    {                                                   /* not empty.   */
        struct dsListNode* tmp = list->first;

        if( NULL == list->current->next )
        {
            return DSLIST_ERROR16; /* a NULL next pointer is only valid */
        }                          /* for an empty list.                */

        /* look for current in list,
           checking the prev links along the way  */
        do
        {
          tmp = tmp->next;

          if( NULL == tmp || NULL == tmp->prev || tmp  != tmp->prev->next )
          {
            return DSLIST_ERROR15 ;  /* current not found in list */
          }                         /* or link to/from next node */
                                    /* invalid                   */
        }while( tmp != list->current );

        /* Found current in list. Also without link errors.
           Now look for valid last node pointer in the list,
           checking the prev links along the way
           Note that current itself is never supposed to be equal
           to last (which points to the dummy tail) !
        */
        if( NULL == list->last )
        {
          return DSLIST_ERROR14 ;
        }

        do
        {
          tmp = tmp->next ;

          if( NULL == tmp || NULL == tmp->prev || tmp != tmp->prev->next )
          {
            return DSLIST_ERROR13 ;  /* last not found in list    */
          }                         /* or link to/from prev node */
                                    /* invalid                   */
        }while( tmp != list->last );

        /* Found last in list but is it really a valid last pointer?
           Note: tmp == last */

        if( NULL != tmp->next )
        {
          return DSLIST_ERROR12;
        }

       return DSLIST_NO_ERROR;
    }

    /* first->next->next == NULL  =>  list is empty  */
    if( list->current != list->first->next )
    {
      return DSLIST_ERROR11;
    }

    if( list->last != list->first->next ||
        list->last != list->current->prev->next )
    {
        return DSLIST_ERROR10;
    }

    return DSLIST_NO_ERROR;

}

/******************************************************************************
*
* DESCRIPTION:
*
* PARAMETERS:
*   list          Pointer to the list.
*
* RETURNS:
*
******************************************************************************/

#ifndef _INLINE

struct dsListNode* dslist_node_malloc( const size_t itemsize )
{
 struct dsListNode* node;

 node = (dsListNode*)rtlib_malloc(sizeof(struct dsListNode) );
  
 if( NULL == node )
 {
   return NULL;
 }

 node->next     = 0;
 node->prev     = 0;
 node->itemsize = itemsize;

 if( itemsize )
 {
   node->data = rtlib_malloc(itemsize);

   if( node->data == NULL)
   {
     rtlib_free( node );

     return NULL;
   }
 }
 else
 {
   node->data = NULL;
 }

 return node;
}

#endif

#undef __INLINE
