/**
 * @file HostLib_FromMMFile.h
 * @brief Helper for the FromMMFile block.
 * Copyright 2008 The MathWorks, Inc.
 * $Revision: 1.1.6.1 $ $Date: 2008/11/18 01:38:36 $
 */ 


/* Wrap everything in extern C */
#ifdef __cplusplus
extern "C" {
#endif 

extern const char *libName_FromMMFile;

#include "VideoDefs.h"
#include "AudioDefs.h"
  
/*******************************
 * Routines which are defined in the library in question
 *******************************/
typedef void* (*pFnLibCreate_FromMMFile)(char *err, char *warn, const char *fileName,
                                         MMAudioInfo* aInfo, MMVideoInfo* vInfo, 
                                         unsigned int numRepeats, unsigned char loopIndef, 
                                         unsigned char scaledDoubleAudio, unsigned char scaledDoubleVideo);
typedef void (*pFnLibOutputs_FromMMFile)(void *hostLib, char *err, unsigned char *bDone, void *audio, void *R, void *G, void *B);

/*******************************
 * Routines which we define to call the functions in the library 
 *******************************/
void LibCreate_FromMMFile(void *hostLib, char *warn, const char *fileName,
                          void* aInfo, void* vInfo, 
                          unsigned int numRepeats, unsigned char loopIndef, 
                          unsigned char scaledDoubleAudio, unsigned char scaledDoubleVideo);
void LibOutputs_FromMMFile(void *hostLib, void *bDone, void *audio, void *R, void *G, void *B);

/*******************************
 * Routines used to initialize MMAudioInfo and MMVideoInfo structures.
 *******************************/

void createAudioInfo(void *audioInfo, unsigned char isValid, double sampleRate, int numBits,
                     int numChannels, int frameSize,
                     AudioDataType dataType, char* audioCompressor);

void createVideoInfo(void *videoInfo, unsigned char isValid, double frameRate, double frameRateComputed,
                     int videoWidth, int videoHeight,
                     VideoDataType dataType, VideoFrameOrientation orientation,
                     char* videoCompressor, int isOut3D);


/* Include HostLib for declarations of LibStart, LibTerminate, CreateHostLibrary, and DestroyHostLibrary. */
#include "HostLib_rtw.h"


#ifdef __cplusplus
} // extern "C"
#endif 
