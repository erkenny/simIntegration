/* $Revision: 1.5.4.3 $
 *
 * fromwavedevice_ex_win32.h
 * Runtime library header file for Windows "From Wave Device" block.
 * Link:  (no import library needed)
 *
 *  Copyright 1995-2006 The MathWorks, Inc.
 */

#ifndef FROMWAVEDEVICE_EX_H
#define FROMWAVEDEVICE_EX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fromwavedevice_win32.h"

int exMWDSP_Wai_GetErrorCode(void);

const char* exMWDSP_Wai_GetErrorMessage(void);

void* exMWDSP_Wai_Create(double rate, unsigned short bits, int chans, int inputBufSize, int dType,
						double bufSizeInSeconds, unsigned int whichDevice, unsigned short useTheWaveMapper);

void exMWDSP_Wai_Start(void* obj);


void exMWDSP_Wai_Outputs(const void * obj, void* signal);

void exMWDSP_Wai_Terminate(const void* obj);

void exMWDSP_Wai_GetFunctionPtrs(MWDSP_Wai_FunctionPtrStruct* ptrs);

#ifdef __cplusplus
}
#endif

#endif /* FROMWAVEDEVICE_EX_H */
