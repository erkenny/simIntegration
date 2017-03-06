/* $Revision: 1.1.6.9 $ 
 *
 * frommmfile_win32.h
 * Runtime library header file for "From Multimedia File" block.  _win32 extension
 * is a leftover from this block's original Windows-only implementation
 * On Windows, link against:  toolbox/dspblks/lib/<ARCH>/FromMMFile.lib
 *
 *  Copyright 1995-2007 The MathWorks, Inc.
 */

#ifndef FROMMMFILE_H
#define FROMMMFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "VideoDefs.h"
#include "AudioDefs.h"

/*
 * Exported function types
 */

typedef int	        (*MWDSP_WMMFI_GETERRCODE_FUNC) (void);
typedef const char*	(*MWDSP_WMMFI_GETERRMSG_FUNC) (void);

typedef void* (*MWDSP_WMMFI_CREATE_FUNC)          (const char*, MMAudioInfo*, MMVideoInfo*);
typedef void  (*MWDSP_WMMFI_AUDIO_OUTPUTS_FUNC)   (void*, void*);
typedef void  (*MWDSP_WMMFI_AUDIO_FLUSH_FUNC)     (void*);
typedef void  (*MWDSP_WMMFI_VIDEO_OUTPUTS_FUNC)   (void*, void*);
typedef void  (*MWDSP_WMMFI_VCHANS_OUTPUTS_FUNC)  (void*, void*, void*, void*);
typedef int   (*MWDSP_WMMFI_REWIND_FUNC)          (void*);
typedef int   (*MWDSP_WMMFI_TERMINATE_FUNC)       (void*);
typedef void  (*MWDSP_WMMFI_SETREPEATS_FUNC)      (void*, unsigned long);
typedef void  (*MWDSP_WMMFI_LOOPINDEF_FUNC)       (void*, unsigned short);
typedef void  (*MWDSP_WMMFI_VIDEO_FLUSH_FUNC)     (void*);
typedef void  (*MWDSP_WMMFI_VIDEO_I_OUTPUTS_FCN)  (void*, void*);
typedef unsigned short (*MWDSP_WMMFI_EOF_FCN)     (void*);

typedef struct
{
	MWDSP_WMMFI_GETERRCODE_FUNC     getErrorCode;
	MWDSP_WMMFI_GETERRMSG_FUNC      getErrorMessage;
	MWDSP_WMMFI_CREATE_FUNC         createFcn;
	MWDSP_WMMFI_AUDIO_OUTPUTS_FUNC  audioOutputsFcn;
	MWDSP_WMMFI_AUDIO_FLUSH_FUNC    audioFlush;
	MWDSP_WMMFI_VIDEO_OUTPUTS_FUNC  videoOutputsFcn;
	MWDSP_WMMFI_VCHANS_OUTPUTS_FUNC videoRGBOutputsFcn;
	MWDSP_WMMFI_REWIND_FUNC         rewindFcn;
	MWDSP_WMMFI_TERMINATE_FUNC      terminateFcn;
	MWDSP_WMMFI_SETREPEATS_FUNC     setNumRepeats;
	MWDSP_WMMFI_LOOPINDEF_FUNC      loopIndefinitely;
	MWDSP_WMMFI_VIDEO_FLUSH_FUNC    videoFlush;
	MWDSP_WMMFI_VIDEO_I_OUTPUTS_FCN	videoIntensityOutputsFcn;
	MWDSP_WMMFI_EOF_FCN             isAtEndOfFile;

} MWDSP_Wmmfi_FunctionPtrStruct;
#define MWDSP_WMMFI_NUM_FUNCTIONS (sizeof(MWDSP_Wmmfi_FunctionPtrStruct)/sizeof(void *))

#define MWDSP_WMMFI_LIB_NAME              "frommmfile.dll"

#define MWDSP_WMMFI_GETERRCODE_NAME       "MWDSP_Wmmfi_GetErrorCode"
#define MWDSP_WMMFI_GETERRMSG_NAME        "MWDSP_Wmmfi_GetErrorMessage"

#define MWDSP_WMMFI_CREATE_NAME           "MWDSP_Wmmfi_Create"
#define MWDSP_WMMFI_AUDIO_OUTPUTS_NAME    "MWDSP_Wmmfi_AudioOutputs"
#define MWDSP_WMMFI_AUDIO_FLUSH_NAME      "MWDSP_Wmmfi_AudioFlush"
#define MWDSP_WMMFI_VIDEO_OUTPUTS_NAME    "MWDSP_Wmmfi_VideoOutputs"
#define MWDSP_WMMFI_VCHANS_OUTPUTS_NAME   "MWDSP_Wmmfi_VideoRGBOutputs"
#define MWDSP_WMMFI_REWIND_NAME           "MWDSP_Wmmfi_Rewind"
#define MWDSP_WMMFI_TERMINATE_NAME        "MWDSP_Wmmfi_Terminate"
#define MWDSP_WMMFI_SETREPEATS_NAME       "MWDSP_Wmmfi_SetNumRepeats"
#define MWDSP_WMMFI_LOOPINDEF_NAME        "MWDSP_Wmmfi_LoopIndefinitely"
#define MWDSP_WMMFI_VIDEO_FLUSH_NAME      "MWDSP_Wmmfi_VideoFlush"
#define MWDSP_WMMFI_VIDEO_I_OUTPUTS_NAME  "MWDSP_Wmmfi_VideoIntensityOutputs"
#define MWDSP_WMMFI_EOF_NAME              "MWDSP_Wmmfi_IsAtEndOfFile"

/*
 * Error message function prototypes
 */
int MWDSP_Wmmfi_GetErrorCode(void);
char* MWDSP_Wmmfi_GetErrorMessage(void);


/*
 * The public interface
 */

void* MWDSP_Wmmfi_Create(char* fName, MMAudioInfo* aInfo, MMVideoInfo* vInfo);

void MWDSP_Wmmfi_AudioOutputs(void* obj, void* signal);

void MWDSP_Wmmfi_AudioFlush(void* obj);

void MWDSP_Wmmfi_VideoOutputs(void* obj, void* signal);

void MWDSP_Wmmfi_VideoRGBOutputs(void* obj, void* red, void* green, void* blue);

int MWDSP_Wmmfi_Rewind(void* obj);

int MWDSP_Wmmfi_Terminate(void* obj);

void MWDSP_Wmmfi_SetNumRepeats(void* obj, unsigned long rpts);

void MWDSP_Wmmfi_LoopIndefinitely(void* obj, unsigned short loopInf);

void MWDSP_Wmmfi_VideoFlush(void* obj);

void MWDSP_Wmmfi_VideoIntensityOutputs(void* obj, void* signal);

unsigned short MWDSP_Wmmfi_IsAtEndOfFile(void* obj);

#ifdef __cplusplus
}
#endif

#endif	/* FROMMMFILE_H */

