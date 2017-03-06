/*
 * dsp_flip_sim - mid-level simulation functions
 *  for the flip block
 *
 *  Copyright 1995-2006 The MathWorks, Inc.
 *  $Revision: 1.3.4.7 $  $Date: 2008/03/17 22:02:28 $
 */

#ifndef DSP_FLIP_SIM_H
#define DSP_FLIP_SIM_H

/* version.h indirectly includes tmwtypes.h
 * after compiler switch automagic
 */
#include "version.h"

/*
 * ----------------------------------------------------------
 * Definitions used locally 
 * ----------------------------------------------------------
 */
/* MAX_BYTES_PER_CPLX_DTYPE is used to create a temp buffer for the inplace
 * algorithm.  The biggest data type supported in SL is currently
 * 128 bits = 16 bytes; 32 bytes for complex. In the inplace algorithms,
 * we assume worst case.
 * Note: that the "BYTE" definition on some target platforms, 
 *       e.g., TI C55x DSP, may be 16 bits instead of 8 bits
 */
#define MAX_BYTES_PER_CPLX_DTYPE 32

typedef struct {
    const void *u;                /* pointer to input array */
    void       *y;                /* pointer to output array */
    int_T       numRows;          /* number of rows in input array */
    int_T       numCols;          /* number of columns in input array */
    int_T       bytesPerElement;  /* number of bytes per element in input array */
} DSPSIM_FlipArgsCache;

/*
 * ====================================================
 * Function Argument Structure Usage
 * ====================================================
 *
 * All members of the DSPSIM_FlipArgsCache argument structure (see above)
 * need to be defined, except in the case of an 'inplace' flip.
 *
 * For an inplace ("IP") flip operation, the input signal (*u) is not used.
 * That is, for IP simulation functions below, the input and output signals
 * share the same memory space (*y), and "*u" is ignored for those functions.
 *
 *   u               = pointer to the input signal
 *   y               = pointer to the output signal
 *   numRows         = number of rows in the input signal
 *   numCols         = number of columns in the input signal
 *   bytesPerElement = size of DType in bytes
 *
 *
 * ====================================================
 * Public Function Usage
 * ====================================================
 *
 * Three factors determine which function should be called: input size,
 * flip direction, and whether or not the flip needs to use an inplace
 * algorithm.
 *
 * Not all combinations map to one of the below functions.  For the rest,
 * either no action needs to be taken, or a simple copy of inputs to outputs
 * is sufficient.
 *
 * The public functions (see below) are named as follows.
 *
 * Format : DSPSIM_Flip{Vector|MatrixCol|MatrixRow}{IP|OP}
 *            -OR-
 *          DSPSIM_FlipCopyInputToOutput
 *            -OR-
 *          DSPSIM_FlipNoOp
 *
 * Vector    = vector input, flip along vector length
 * MatrixCol = matrix input, flip along columns
 * MatrixRow = matrix input, flip along rows
 * IP        = flip is 'inplace'
 * OP        = flip is not 'inplace'
 *
 *
 * The following combinations require calling DSPSIM_FlipCopyInputToOutput:
 *
 *    Column flip, not inplace, scalar input
 *    Column flip, not inplace, row vector input
 *    Row flip,    not inplace, scalar input
 *    Row flip,    not inplace, column vector input
 *
 *
 * The following combinations require no function call (or a call to DSPSIM_FlipNoOp):
 *
 *    Column flip, inplace, scalar input
 *    Column flip, inplace, row vector input
 *    Row flip,    inplace, scalar input
 *    Row flip,    inplace, column vector input
 *
 * Note that an unoriented (1D) vector never has row/column input vs.
 * row/column flip dirirection conflict (there is no distinction
 * between row and column).
 *
 * This code is data type independent.
 */
extern void DSPSIM_FlipNoOp(DSPSIM_FlipArgsCache* args);
extern void DSPSIM_FlipCopyInputToOutput(DSPSIM_FlipArgsCache* args);
extern void DSPSIM_FlipVectorIP(DSPSIM_FlipArgsCache* args);
extern void DSPSIM_FlipVectorOP(DSPSIM_FlipArgsCache* args);
extern void DSPSIM_FlipMatrixRowIP(DSPSIM_FlipArgsCache* args);
extern void DSPSIM_FlipMatrixRowOP(DSPSIM_FlipArgsCache* args);
extern void DSPSIM_FlipMatrixColIP(DSPSIM_FlipArgsCache* args);
extern void DSPSIM_FlipMatrixColOP(DSPSIM_FlipArgsCache* args);

#endif

/* [EOF] dsp_flip_sim.h */
