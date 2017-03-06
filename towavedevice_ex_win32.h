/* $Revision: 1.5.4.3 $ 
 *
 * towavedevice_ex_win32.h
 * Runtime library header file for Windows "To Wave Device" block.
 * Link:  (no import library needed)
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 */

#ifndef TOWAVEDEVICE_EX_H
#define TOWAVEDEVICE_EX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "towavedevice_win32.h"

int exMWDSP_Wao_GetErrorCode(void);

const char* exMWDSP_Wao_GetErrorMessage(void);

void* exMWDSP_Wao_Create(double rate, unsigned short bits, unsigned int chans, unsigned int inputBufSize, int dType,
			double bufSizeInSeconds, unsigned int internalBufSize, double initialDelay, unsigned int whichDevice, 
			unsigned short useTheWaveMapper);

void exMWDSP_Wao_Start(void* obj);

void exMWDSP_Wao_Update(const void * obj, const void* signal);

void exMWDSP_Wao_Terminate(const void* obj);

void exMWDSP_Wao_GetFunctionPtrs(MWDSP_Wao_FunctionPtrStruct* ptrs);

#ifdef __cplusplus
}
#endif

#endif /* TOWAVEDEVICE_EX_H */
