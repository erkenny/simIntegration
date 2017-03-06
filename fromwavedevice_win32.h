/* $Revision: 1.4.4.4 $ 
 *
 * fromwavedevice_win32.h
 * Runtime library header file for Windows "From Wave Device" block.
 * Link:  toolbox/dspblks/lib/win32/fromwavedevice.lib
 *
 *  Copyright 1995-2006 The MathWorks, Inc.
 */

#ifndef FROMWAVEDEVICE_H
#define FROMWAVEDEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Exported function types
 */

typedef void	(*MWDSP_WAI_SETERRCODE_FUNC) (int);
typedef int	    (*MWDSP_WAI_GETERRCODE_FUNC) (void);
typedef const char* (*MWDSP_WAI_GETERRMSG_FUNC) (void);

typedef void* (*MWDSP_WAI_CREATE_FUNC)		(double, unsigned short, int, int, int, double, unsigned int, unsigned short);
typedef void  (*MWDSP_WAI_START_FUNC)		(const void*);
typedef void  (*MWDSP_WAI_OUTPUTS_FUNC)		(const void*, void*);
typedef int  (*MWDSP_WAI_TERMINATE_FUNC)	(const void*);

typedef struct
{
	MWDSP_WAI_SETERRCODE_FUNC setErrorCode;
	MWDSP_WAI_GETERRCODE_FUNC getErrorCode;
	MWDSP_WAI_GETERRMSG_FUNC  getErrorMessage;
	MWDSP_WAI_CREATE_FUNC     createFcn;
	MWDSP_WAI_START_FUNC      startFcn;
	MWDSP_WAI_OUTPUTS_FUNC    outputsFcn;
	MWDSP_WAI_TERMINATE_FUNC  terminateFcn;
} MWDSP_Wai_FunctionPtrStruct;
#define MWDSP_WAI_NUM_FUNCTIONS 7

#define MWDSP_WAI_LIB_NAME	    "fromwavedevice.dll"

#define MWDSP_WAI_SETERRCODE_NAME  "MWDSP_Wai_SetErrorCode"
#define MWDSP_WAI_GETERRCODE_NAME  "MWDSP_Wai_GetErrorCode"
#define MWDSP_WAI_GETERRMSG_NAME   "MWDSP_Wai_GetErrorMessage"

#define MWDSP_WAI_CREATE_NAME	    "MWDSP_Wai_Create"
#define MWDSP_WAI_START_NAME		"MWDSP_Wai_Start"
#define MWDSP_WAI_OUTPUTS_NAME	    "MWDSP_Wai_Outputs"
#define MWDSP_WAI_TERMINATE_NAME   "MWDSP_Wai_Terminate"

/*
 * Error message function prototypes
 */

void MWDSP_Wai_SetErrorCode(int code);
int MWDSP_Wai_GetErrorCode(void);
const char* MWDSP_Wai_GetErrorMessage(void);

/*
 * The public interface
 */

void* MWDSP_Wai_Create(double rate, unsigned short bits, int chans, int inputBufSize, int dType,
						double bufSizeInSeconds, unsigned int whichDevice, unsigned short useTheWaveMapper);

void MWDSP_Wai_Start(const void* obj);

void MWDSP_Wai_Outputs(const void * obj, void* signal);

/* returns the number of instances of the "block" after this one is destroyed */
int MWDSP_Wai_Terminate(const void* obj);

#ifdef __cplusplus
}
#endif

#endif /* FROMWAVEDEVICE_H */
