/* $Revision: 1.1.6.5 $ 
 *
 * tommfile_win32.h
 * Runtime library header file for Windows "To Multimedia File" block.
 * Link:  toolbox/dspblks/lib/win32/ToMMFile.lib
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 */

#ifndef TOMMFILE_H
#define TOMMFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "VideoDefs.h"
#include "AudioDefs.h"


/*
 * Exported function types
 */

typedef int			(*MWDSP_WMMFO_GETERRCODE_FUNC) (void);
typedef const char*	(*MWDSP_WMMFO_GETERRMSG_FUNC) (void);

typedef void* (*MWDSP_WMMFO_CREATE_FUNC)    (char*, MMAudioInfo*, MMVideoInfo*);
typedef void  (*MWDSP_WMMFO_AUDIO_OUTPUTS_FUNC)   (void*, const void*);
typedef void  (*MWDSP_WMMFO_VIDEO_OUTPUTS_FUNC)   (void*, const void*);
typedef void  (*MWDSP_WMMFO_VIDEOCHANS_OUTPUTS_FUNC) (void*, const void*, const void*, const void*);
typedef int  (*MWDSP_WMMFO_TERMINATE_FUNC) (void*);

typedef struct
{
	MWDSP_WMMFO_GETERRCODE_FUNC          getErrorCode;
	MWDSP_WMMFO_GETERRMSG_FUNC           getErrorMessage;
	MWDSP_WMMFO_CREATE_FUNC              createFcn;
	MWDSP_WMMFO_AUDIO_OUTPUTS_FUNC       audioOutputsFcn;
	MWDSP_WMMFO_VIDEO_OUTPUTS_FUNC       videoOutputsFcn;
	MWDSP_WMMFO_VIDEOCHANS_OUTPUTS_FUNC  videoRGBOutputsFcn;
	MWDSP_WMMFO_TERMINATE_FUNC           terminateFcn;
} MWDSP_Wmmfo_FunctionPtrStruct;
#define MWDSP_WMMFO_NUM_FUNCTIONS 7


#define MWDSP_WMMFO_LIB_NAME			"tommfile.dll"

#define MWDSP_WMMFO_GETERRCODE_NAME  "MWDSP_Wmmfo_GetErrorCode"
#define MWDSP_WMMFO_GETERRMSG_NAME   "MWDSP_Wmmfo_GetErrorMessage"

#define MWDSP_WMMFO_CREATE_NAME				"MWDSP_Wmmfo_Create"
#define MWDSP_WMMFO_AUDIO_OUTPUTS_NAME		"MWDSP_Wmmfo_AudioOutputs"
#define MWDSP_WMMFO_VIDEO_OUTPUTS_NAME		"MWDSP_Wmmfo_VideoOutputs"
#define MWDSP_WMMFO_VIDEOCHANS_OUTPUTS_NAME	"MWDSP_Wmmfo_VideoRGBOutputs"
#define MWDSP_WMMFO_TERMINATE_NAME			"MWDSP_Wmmfo_Terminate"

/*
 * Error message function prototypes
 */

int MWDSP_Wmmfo_GetErrorCode(void);
char* MWDSP_Wmmfo_GetErrorMessage(void);

/*
 * The public interface
 */

void* MWDSP_Wmmfo_Create(char* fName, MMAudioInfo* aInfo, MMVideoInfo* vInfo);

void MWDSP_Wmmfo_AudioOutputs(void* obj, const void* signal);

void MWDSP_Wmmfo_VideoOutputs(void* obj, const void* signal);

void MWDSP_Wmmfo_VideoRGBOutputs(void* obj,
						  const void* red, const void* green, const void* blue);

int MWDSP_Wmmfo_Terminate(void* obj);

#ifdef __cplusplus
}
#endif

#endif /* TOMMFILE_H */
