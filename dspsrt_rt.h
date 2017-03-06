/*
 *  dspsrt_rt.h
 *
 *  Copyright 1995-2006 The MathWorks, Inc.
 *  $Revision: 1.8.4.9 $ $Date: 2006/06/27 22:48:02 $
 */

#ifndef dspsrt_rt_h
#define dspsrt_rt_h

#include "dsp_rt.h"

#ifdef DSPQSRT_EXPORTS
#define DSPQSRT_EXPORT EXPORT_FCN
#else
#define DSPQSRT_EXPORT MWDSP_IDECL
#endif

/* do not inline recursive quick sort functions */
#ifdef MWDSP_INLINE_DSPRTLIB
#define DSPQSRTNOINLINE_EXPORT extern
#else
#define DSPQSRTNOINLINE_EXPORT DSPQSRT_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function naming glossary
 * ---------------------------
 *
 * MWDSP = MathWorks DSP Blockset
 *
 * Data types - (describe inputs to functions, not outputs)
 * R   = real single-precision
 * D   = real double-precision
 * S08 = int8_T
 * U08 = uint8_T
 * S16 = int16_T
 * U16 = uint16_T
 * S32 = int32_T
 * U32 = uint32_T
 */

/* Function naming convention
 * --------------------------
 *
 *    1) MWDSP_ is a prefix used with all Mathworks DSP runtime library
 *       functions.
 *    2) The second field (Sort) indicates that this function is implementing the
 *       Sort algorithm.
 *    3) The third field [Qk | Ins] indicates quick sort or insertion sort algorithm.
 *    4) The fourth field [Val | Idx] indicates sort by value or by index.
 *    5) The last field enumerates the data type from the above list.
 *       The data types of the input and output are the same.
 *
 *    Examples:
 *       MWDSP_Sort_Qk_Val_D is the Sort run time function for
 *       quick sort by value algorithm for double precision inputs.
 */

/* Quick Sort by index */
#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Idx_D(
    const real_T *dataArray,
    uint32_T *idxArray,
    int_T i, int_T j );
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Idx_R(
    const real32_T *dataArray,
    uint32_T *idxArray,
    int_T i, int_T j );
#endif /* !INTEGER_CODE */

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Idx_S32(
    const int32_T *dataArray,
    uint32_T *idxArray,
    int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Idx_U32(
    const uint32_T *dataArray,
    uint32_T *idxArray,
    int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Idx_S16(
    const int16_T *dataArray,
    uint32_T *idxArray,
    int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Idx_U16(
    const uint16_T *dataArray,
    uint32_T *idxArray,
    int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Idx_S08(
    const int8_T *dataArray,
    uint32_T *idxArray,
    int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Idx_U08(
    const uint8_T *dataArray,
    uint32_T *idxArray,
    int_T i, int_T j );

/* Quick Sort by value */
#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Val_D(real_T *dataArray,
                                                int_T i, int_T j );
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Val_R(real32_T *dataArray,
                                                int_T i, int_T j );
#endif /* !INTEGER_CODE */

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Val_S32(int32_T *dataArray,
                                                  int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Val_U32(uint32_T *dataArray,
                                                  int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Val_S16(int16_T *dataArray,
                                                  int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Val_U16(uint16_T *dataArray,
                                                  int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Val_S08(int8_T *dataArray,
                                                  int_T i, int_T j );

DSPQSRTNOINLINE_EXPORT void MWDSP_Sort_Qk_Val_U08(uint8_T *dataArray,
                                                  int_T i, int_T j );

/* Helper macros used in quick sort by index functions */
#define qsortIdxSwap(i,j) {uint32_T t    = *(idxArray+i); \
                           *(idxArray+i) = *(idxArray+j); \
                           *(idxArray+j) = t;}

#define qsortIdxOrder(i,j) {if( *(dataArray+*(idxArray+i)) > \
                            *(dataArray+*(idxArray+j)) ) qsortIdxSwap(i,j)}

#define qsortIdxOrder3(i,j,k) {qsortIdxOrder(i,j); qsortIdxOrder(i,k); qsortIdxOrder(j,k);}


/* Insertion Sort by value */
#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT void MWDSP_Sort_Ins_Val_D(real_T *dataArray, int_T n );
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT void MWDSP_Sort_Ins_Val_R(real32_T *dataArray, int_T n );
#endif /* !INTEGER_CODE */

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Val_S32(int32_T *dataArray, int_T n );

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Val_U32(uint32_T *dataArray, int_T n );

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Val_S16(int16_T *dataArray, int_T n );

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Val_U16(uint16_T *dataArray, int_T n );

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Val_S08(int8_T *dataArray, int_T n );

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Val_U08(uint8_T *dataArray, int_T n );


/* Insertion Sort by index */
#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT void MWDSP_Sort_Ins_Idx_D(const real_T *dataArray, uint32_T *idx, int_T n);
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT void MWDSP_Sort_Ins_Idx_R(const real32_T *dataArray, uint32_T *idx, int_T n);
#endif /* !INTEGER_CODE */

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Idx_S32(const int32_T *dataArray, uint32_T *idx, int_T n);

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Idx_U32(const uint32_T *dataArray, uint32_T *idx, int_T n);

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Idx_S16(const int16_T *dataArray, uint32_T *idx, int_T n);

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Idx_U16(const uint16_T *dataArray, uint32_T *idx, int_T n);

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Idx_S08(const int8_T *dataArray, uint32_T *idx, int_T n);

DSPQSRT_EXPORT void MWDSP_Sort_Ins_Idx_U08(const uint8_T *dataArray, uint32_T *idx, int_T n);


/* XXX The functions are still used by dspblks/v3/dspsrt.c */
#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT void MWDSP_SrtQkRecD(
    const real_T *qid_array,
    int_T *qid_index,
    int_T i, int_T j );
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
#if defined(CREAL_T)
DSPQSRT_EXPORT void MWDSP_SrtQkRecZ(
    const creal_T *qid_array,
    int_T   *qid_index,
    real_T  *sort,
    int_T i, int_T j );
#endif /* CREAL_T */
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT void MWDSP_SrtQkRecR(
    const real32_T *qid_array,
    int_T *qid_index,
    int_T i, int_T j );
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
#if defined(CREAL_T)
DSPQSRT_EXPORT void MWDSP_SrtQkRecC(
    const creal32_T *qid_array,
    int_T *qid_index,
    real32_T *sort,
    int_T i, int_T j );
#endif /* CREAL_T */
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT int_T MWDSP_SrtQidPartitionD(
    const real_T *qid_array,
    int_T *qid_index,
    int_T i,
    int_T j,
    int_T pivot );
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT boolean_T MWDSP_SrtQidFindPivotD(
    const real_T *qid_array,
    int_T *qid_index,
    int_T i,
    int_T j,
    int_T *pivot );
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT int_T MWDSP_SrtQidPartitionR(
     const real32_T *qid_array,
     int_T *qid_index,
     int_T i,
     int_T j,
     int_T pivot );
#endif /* !INTEGER_CODE */

#if !defined(INTEGER_CODE) || !INTEGER_CODE
DSPQSRT_EXPORT boolean_T MWDSP_SrtQidFindPivotR(
     const real32_T *qid_array,
     int_T *qid_index,
     int_T i,
     int_T j,
     int_T *pivot );
#endif /* !INTEGER_CODE */

#define qid_Swap(i,j) {int_T t        = *(qid_index+i); \
                       *(qid_index+i) = *(qid_index+j); \
                       *(qid_index+j) = t;}

#define qid_Order(i,j) {if( *(qid_array+*(qid_index+i)) > \
                            *(qid_array+*(qid_index+j)) ) qid_Swap(i,j)}

#define qid_Order3(i,j,k) {qid_Order(i,j); qid_Order(i,k); qid_Order(j,k);}

#ifdef __cplusplus
}
#endif

/* dsp v3 files */

#ifdef MWDSP_INLINE_DSPRTLIB
# if !defined(INTEGER_CODE) || !INTEGER_CODE
#  include "dspqsrt/sort_ins_idx_d_rt.c"
#  include "dspqsrt/sort_ins_idx_r_rt.c"
#  include "dspqsrt/sort_ins_val_d_rt.c"
#  include "dspqsrt/sort_ins_val_r_rt.c"
# endif /* !INTEGER_CODE */
# include "dspqsrt/sort_ins_idx_s08_rt.c"
# include "dspqsrt/sort_ins_idx_s16_rt.c"
# include "dspqsrt/sort_ins_idx_s32_rt.c"
# include "dspqsrt/sort_ins_idx_u08_rt.c"
# include "dspqsrt/sort_ins_idx_u16_rt.c"
# include "dspqsrt/sort_ins_idx_u32_rt.c"
# include "dspqsrt/sort_ins_val_s08_rt.c"
# include "dspqsrt/sort_ins_val_s16_rt.c"
# include "dspqsrt/sort_ins_val_s32_rt.c"
# include "dspqsrt/sort_ins_val_u08_rt.c"
# include "dspqsrt/sort_ins_val_u16_rt.c"
# include "dspqsrt/sort_ins_val_u32_rt.c"
#endif /* MWDSP_INLINE_DSPRTLIB */

#endif /* dspsrt_rt_h */
