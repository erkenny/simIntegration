/**
 * @file HostLib_Audio.h
 * @brief Helper for the AudioDevices.
 * Copyright 2007-2008 The MathWorks, Inc.
 * $Revision: 1.1.6.2 $ $Date: 2008/10/08 17:06:24 $
 */ 


/* Wrap everything in extern C */
#ifdef __cplusplus
extern "C" {
#endif 

extern const char *libName;
  
/*******************************
 * Routines which are defined in the library in question
 *******************************/
typedef void (*pFnLibCreate)(char *err, char *warn, const char *deviceName, int inOut, void **m_device,
                             int numChannels, double sampleRate, int deviceDatatype, int bufferSize, double queueDuration,
                             int frameSize, int signalDatatype);
typedef void (*pFnLibUpdate)(void *device, char *err, const void *src, int signalDatatype, int samplesPerFrame, int *nDroppedSamples);
typedef void (*pFnLibOutputs)(void *device, char *err, void *src, int signalDatatype, int samplesPerFrame, int *nDroppedSamples);

/*******************************
 * Routines which we define to call the functions in the library 
 *******************************/
void LibCreate_Audio(void *hostLib, char *warn, const char *deviceName, int inOut,
               int numChannels, double sampleRate, int deviceDatatype, int bufferSize, double queueDuration,
               int frameSize, int signalDatatype);
void LibUpdate_Audio(void *hostLib, const void *src, int signalDatatype, int samplesPerFrame, int *nDroppedSamples);
void LibOutputs_Audio(void *hostLib, void *src, int signalDatatype, int samplesPerFrame, int *nDroppedSamples);

/* Include HostLib for declarations of LibStart, LibTerminate, CreateHostLibrary, and DestroyHostLibrary. */
#include "HostLib_rtw.h"

#ifdef __cplusplus
} // extern "C"
#endif 
