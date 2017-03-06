/* $Revision: 1.1.6.1 $ 
 *
 * tommfile_ex.h
 * Runtime library header file for "To Multimedia File" block.
 *
 *  Copyright 1995-2007 The MathWorks, Inc.
 */

#ifndef TOMMFILE_EX_H
#define TOMMFILE_EX_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "tommfile_shared.h"
    
/* helper functions */
    MMAudioInfo* exMWDSP_Wmmfo_getAudioInfoPtr(double sampleRate, int numBits, int numChannels,
                                               int frameSize, AudioDataType dataType, 
                                               const char* audioCompressor);
    MMVideoInfo* exMWDSP_Wmmfo_getVideoInfoPtr(double frameRate, int videoWidth, int videoHeight, 
                                               VideoDataType dataType, 
                                               VideoFrameOrientation orientation, 
                                               const char* videoCompressor);
    
    int exMWDSP_Wmmfo_GetErrorCode(void);
    const char* exMWDSP_Wmmfo_GetErrorMessage(void);
    
    void* exMWDSP_Wmmfo_Create(const char* fName, MMAudioInfo* aInfo, MMVideoInfo* vInfo);
    
    void exMWDSP_Wmmfo_AudioOutputs(void* obj, const void* signal);
    
    void exMWDSP_Wmmfo_VideoOutputs(void* obj, const void* signal);
    
    void exMWDSP_Wmmfo_VideoRGBOutputs(void* obj,
                                       const void* red, const void* green, const void* blue);
    
    int exMWDSP_Wmmfo_Terminate(void* obj);
    
    void exMWDSP_Wmmfo_GetFunctionPtrs(MWDSP_Wmmfo_FunctionPtrStruct* ptrs);
    
#ifdef __cplusplus
}
#endif

#endif /* TOMMFILE_EX_H */
