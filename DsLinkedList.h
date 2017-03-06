/** <!------------------------------------------------------------------------->
*
*  @internal
*
*  @file DsLinkedList.h
*
*  @brief Functions for list handling.
*
*  @author
*    AlexanderSt
*
*  @description
*    This module provides a circularly linked list implementation.
*    For performance reasons no error checks are implemented. It is intended to
*    be used internally only.
*
*  @note
*    The module is inspired by the linux kernels list implementation.
*
*  @copyright
*    Copyright 2009, dSPACE GmbH. All rights reserved.
*
*  @version
*    $RCSfile: DsLinkedList.h $ $Revision: 1.2 $ $Date: 2009/05/19 14:30:46GMT+01:00 $
*    $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DsLinkedList.pj $
*
*   <hr><br>
*<!-------------------------------------------------------------------------->*/
#ifndef DSLINKEDLIST_H_
#define DSLINKEDLIST_H_


#include <stdlib.h>
#include <Dstypes.h>


#if defined (_DS2510) || defined (_DS1005) || defined (_DS1006) || defined (_DS1103) || defined (_DS1401)
    #define __INLINE inline
#elif defined (_DSHOST)
    #define __INLINE __inline
#else
    #error DsLinkedList.h: No known hardware platform defined!
#endif


/** Type definition for list head */
typedef struct DsSLinkedList_Head DsSLinkedList_Head;
/** Type definition for list element */
typedef struct DsSLinkedList_Head DsSLinkedList_Entry;


/** @internal
 *  Base structure for list head(s) or element(s) */
struct DsSLinkedList_Head
{
    DsSLinkedList_Head* pNext;      /**< Pointer to next list element or itself */
    DsSLinkedList_Head* pPrev;      /**< Pointer to next list element or itself */
};


/** Macro for initialization of a lists head at its declaration / definition */
#define DSLINKEDLIST_INIT_DEF(Name)     {&(Name), &(Name)}

/** Macro for initialization of a lists head at other places */
#define DSLINKEDLIST_INIT(pName)        {(pName)->pNext = (pName); (pName)->pPrev = (pName);}

/** Macro that checks for empty list */
#define DSLINKEDLIST_IS_EMPTY(pHead)    ((pHead)->pNext == (pHead))

/** Macro that allows "for each" iteration through list */
#define DSLINKEDLIST_FOR_EACH(pIter, pHead, Type)                           \
            for ((pIter) = (Type*)(pHead)->pNext;                           \
                 (pIter) != ((Type*)(pHead));                               \
                 (pIter) = (Type*)((DsSLinkedList_Entry*)(pIter))->pNext)

/** Macro that allows "for each" iteration through list
 *  (safe against removal of elements during iteration) */
#define DSLINKEDLIST_FOR_EACH_SAVE(pIter, pSave, pHead, Type)               \
            for ((pIter) = (Type*)(pHead)->pNext,                           \
                 (pSave) = (Type*)((DsSLinkedList_Entry*)(pIter))->pNext;   \
                 (pIter) != ((Type*)(pHead));                               \
                 (pIter) = (pSave),                                         \
                 (pSave) = (Type*)((DsSLinkedList_Entry*)(pIter))->pNext)


/** INTERNAL function that inserts element at defined position in list */
static __INLINE void DsLinkedList_insertInternal(DsSLinkedList_Entry* pEntry, DsSLinkedList_Entry* pPrev, DsSLinkedList_Entry* pNext)
{
    pNext->pPrev  = pEntry;
    pEntry->pNext = pNext;
    pEntry->pPrev = pPrev;
    pPrev->pNext  = pEntry;
}

/** PUBLIC function that inserts elements at the beginning of a list */
static __INLINE void DsLinkedList_insertFirst(DsSLinkedList_Entry* pEntry, DsSLinkedList_Head* pHead)
{
    DsLinkedList_insertInternal(pEntry, pHead, pHead->pNext);
}

/** PUBLIC function that inserts elements at the end of a list */
static __INLINE void DsLinkedList_insertLast(DsSLinkedList_Entry* pEntry, DsSLinkedList_Head* pHead)
{
    DsLinkedList_insertInternal(pEntry, pHead->pPrev, pHead);
}

/** INTERNAL function that removes an element from its list */
static __INLINE void DsLinkedList_deleteInternal(DsSLinkedList_Entry* pPrev, DsSLinkedList_Entry* pNext)
{
    pNext->pPrev = pPrev;
    pPrev->pNext = pNext;
}

/** PUBLIC function that removes an element from its list */
static __INLINE void DsLinkedList_delete(DsSLinkedList_Entry* pEntry)
{
    DsLinkedList_deleteInternal(pEntry->pPrev, pEntry->pNext);
    pEntry->pNext = NULL;
    pEntry->pPrev = NULL;
}

/** PUBLIC function that returns first element of a list or NULL for an empty list */
static __INLINE DsSLinkedList_Entry* DsLinkedList_getFirst(const DsSLinkedList_Head* pHead)
{
    return ((pHead != pHead->pNext) ? pHead->pNext : NULL);
}

/** PUBLIC function that returns last element of a list or NULL for an empty list */
static __INLINE DsSLinkedList_Entry* DsLinkedList_getLast(const DsSLinkedList_Head* pHead)
{
    return ((pHead != pHead->pPrev) ? pHead->pPrev : NULL);
}

/** PUBLIC function that returns element next to given element */
static __INLINE DsSLinkedList_Entry* DsLinkedList_getNext(DsSLinkedList_Entry* pEntry, DsSLinkedList_Head* pHead)
{
    return ((pEntry->pNext != pHead) ? pEntry->pNext : pHead->pNext);
}

/** PUBLIC function that returns element previous to given element */
static __INLINE DsSLinkedList_Entry* DsLinkedList_getPrev(DsSLinkedList_Entry* pEntry, DsSLinkedList_Head* pHead)
{
    return ((pEntry->pPrev != pHead) ? pEntry->pPrev : pHead->pPrev);
}

/** PUBLIC function that moves element from one list to the beginning of another list */
static __INLINE void DsLinkedList_moveFirst(DsSLinkedList_Entry* pEntry, DsSLinkedList_Head* pHead)
{
    DsLinkedList_deleteInternal(pEntry->pPrev, pEntry->pNext);
    DsLinkedList_insertInternal(pEntry, pHead, pHead->pNext);
}

/** PUBLIC function that moves element from one list to the end of another list */
static __INLINE void DsLinkedList_moveLast(DsSLinkedList_Entry* pEntry, DsSLinkedList_Head* pHead)
{
    DsLinkedList_deleteInternal(pEntry->pPrev, pEntry->pNext);
    DsLinkedList_insertInternal(pEntry, pHead->pPrev, pHead);
}

/** PUBLIC function that removes element from the beginning of a list */
static __INLINE DsSLinkedList_Entry* DsLinkedList_deleteFirst(DsSLinkedList_Head* pHead)
{
    DsSLinkedList_Entry* pEntry = DsLinkedList_getFirst(pHead);
    DsLinkedList_delete(pEntry);
    return pEntry;
}

/** INTERNAL function that moves the elements of one list to the back of another list */
static __INLINE void DsLinkedList_spliceInternal(DsSLinkedList_Head* pHeadSrc, DsSLinkedList_Head* pHeadDest)
{
    DsSLinkedList_Entry* pFirst = pHeadSrc->pNext;
    DsSLinkedList_Entry* pLast = pHeadSrc->pPrev;
    DsSLinkedList_Entry* pCurr = pHeadDest->pNext;

    pFirst->pPrev = pHeadDest;
    pHeadDest->pNext = pFirst;
    pLast->pNext = pCurr;
    pCurr->pPrev = pLast;
}

/** PUBLIC function that moves the elements of one list to the back of another list */
static __INLINE void DsLinkedList_splice(DsSLinkedList_Head* pHeadSrc, DsSLinkedList_Head* pHeadDest)
{
    if (! DSLINKEDLIST_IS_EMPTY(pHeadSrc))
    {
        DsLinkedList_spliceInternal(pHeadSrc, pHeadDest);
        DSLINKEDLIST_INIT(pHeadSrc);
    }
}

/** PUBLIC function that rotates the chain of elements in a list such that the
 *  given element becomes the last one */
static __INLINE void DsLinkedList_makeLast(DsSLinkedList_Entry* pEntry, DsSLinkedList_Head* pHead)
{
    DsLinkedList_delete(pHead);

    pHead->pNext = pEntry->pNext;
    pHead->pPrev = pEntry;
    pEntry->pNext->pPrev = pHead;
    pEntry->pNext = pHead;
}

/** PUBLIC function that rotates the chain of elements in a list such that the
 *  given element becomes the first one */
static __INLINE void DsLinkedList_makeFirst(DsSLinkedList_Entry* pEntry, DsSLinkedList_Head* pHead)
{
    DsLinkedList_delete(pHead);

    pHead->pNext = pEntry;
    pHead->pPrev = pEntry->pPrev;
    pEntry->pPrev->pNext = pHead;
    pEntry->pPrev = pHead;
}


#endif /* DSLINKEDLIST_H_ */
