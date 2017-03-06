/* $Revision: 1.1.6.4 $ 
 *
 * frommmfile_ex_win32.h
 * Runtime library header file for Windows "From Multimedia File" block.
 * Link:  (no import library needed)
 *
 *  Copyright 1995-2007 The MathWorks, Inc.
 */

#ifndef FROMMMFILE_EX_H
#define FROMMMFILE_EX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "frommmfile_win32.h"


    MMAudioInfo* exMWDSP_getAudioInfoPtr(double sampleRate, int numBits, 
                                         int numChannels,
                                         int frameSize, 
                                         AudioDataType dataType, 
                                         char* audioCompressor);
    MMVideoInfo* exMWDSP_getVideoInfoPtr(double frameRate,
                                         double frameRateComputed,
                                         int videoWidth,
                                         int videoHeight, 
                                         VideoDataType dataType, 
                                         VideoFrameOrientation orientation, 
                                         char* videoCompressor);
    
    int exMWDSP_Wmmfi_GetErrorCode(void);
    const char* exMWDSP_Wmmfi_GetErrorMessage(void);
    
    void* exMWDSP_Wmmfi_Create(const char* fName, MMAudioInfo* aInfo, 
                               MMVideoInfo* vInfo);
    
    void exMWDSP_Wmmfi_AudioOutputs(void* obj, void* signal);
    
    void exMWDSP_Wmmfi_AudioFlush(void* obj);
    
    void exMWDSP_Wmmfi_VideoOutputs(void* obj, void* signal);
    
    void exMWDSP_Wmmfi_VideoRGBOutputs(void* obj, void* red, void* green, 
                                       void* blue);

    int  exMWDSP_Wmmfi_Rewind(void* obj);
    
    int  exMWDSP_Wmmfi_Terminate(void* obj);
    
    void exMWDSP_Wmmfi_SetNumRepeats(void* obj, unsigned long rpts);
    
    void exMWDSP_Wmmfi_LoopIndefinitely(void* obj, unsigned short loopInf);
    
    void exMWDSP_Wmmfi_VideoFlush(void* obj);
    
    void exMWDSP_Wmmfi_GetFunctionPtrs(MWDSP_Wmmfi_FunctionPtrStruct* ptrs);
    
    void exMWDSP_Wmmfi_VideoIntensityOutputs(void* obj, void* signal);
    
    unsigned short exMWDSP_Wmmfi_IsAtEndOfFile(void* obj);
    
#ifdef __cplusplus
}
#endif

#endif /* FROMMMFILE_EX_H */
