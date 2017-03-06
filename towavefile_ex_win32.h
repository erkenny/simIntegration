/* $Revision: 1.5.4.2 $ 
 *
 * towavefile_ex_win32.h
 * Runtime library header file for Windows "To Wave File" block.
 * Link:  (no import library needed)
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 */

#ifndef TOWAVEFILE_EX_H
#define TOWAVEFILE_EX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "towavefile_win32.h"

int exMWDSP_Wafo_GetErrorCode(void);

const char* exMWDSP_Wafo_GetErrorMessage(void);

void* exMWDSP_Wafo_Create(char* outputFilename, unsigned short bits, 
		    int minSampsToWrite, int chans, int inputBufSize,
		    double rate, int dType);

void exMWDSP_Wafo_Outputs(const void * obj, const void* signal);

void exMWDSP_Wafo_Terminate(const void* obj);

void exMWDSP_Wafo_GetFunctionPtrs(MWDSP_Wafo_FunctionPtrStruct* ptrs);

#ifdef __cplusplus
}
#endif

#endif /* TOWAVEFILE_EX_H */
