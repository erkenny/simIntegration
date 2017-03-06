/* $Revision: 1.4.4.3 $ 
 *
 * towavefile_win32.h
 * Runtime library header file for Windows "To Wave File" block.
 * Link:  toolbox/dspblks/lib/win32/towavefile.lib
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 */

#ifndef TOWAVEFILE_H
#define TOWAVEFILE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Exported function types
 */

typedef int			(*MWDSP_WAFO_GETERRCODE_FUNC) (void);
typedef const char* (*MWDSP_WAFO_GETERRMSG_FUNC) (void);

typedef void* (*MWDSP_WAFO_CREATE_FUNC)    (char*, unsigned short, int, int, int,double, int);
typedef void  (*MWDSP_WAFO_OUTPUTS_FUNC)   (const void*, const void*);
typedef int  (*MWDSP_WAFO_TERMINATE_FUNC) (const void*);

typedef struct
{
	MWDSP_WAFO_GETERRCODE_FUNC getErrorCode;
	MWDSP_WAFO_GETERRMSG_FUNC  getErrorMessage;
	MWDSP_WAFO_CREATE_FUNC     createFcn;
	MWDSP_WAFO_OUTPUTS_FUNC    outputsFcn;
	MWDSP_WAFO_TERMINATE_FUNC  terminateFcn;
} MWDSP_Wafo_FunctionPtrStruct;
#define MWDSP_WAFO_NUM_FUNCTIONS 5

#define MWDSP_WAFO_LIB_NAME			"towavefile.dll"

#define MWDSP_WAFO_GETERRCODE_NAME  "MWDSP_Wafo_GetErrorCode"
#define MWDSP_WAFO_GETERRMSG_NAME   "MWDSP_Wafo_GetErrorMessage"

#define MWDSP_WAFO_CREATE_NAME	    "MWDSP_Wafo_Create"
#define MWDSP_WAFO_OUTPUTS_NAME	    "MWDSP_Wafo_Outputs"
#define MWDSP_WAFO_TERMINATE_NAME   "MWDSP_Wafo_Terminate"

/*
 * Error message function prototypes
 */

int MWDSP_Wafo_GetErrorCode(void);
const char* MWDSP_Wafo_GetErrorMessage(void);

/*
 * The public interface
 */

void* MWDSP_Wafo_Create(char* outputFilename, unsigned short bits, 
		    int minSampsToWrite, int chans, int inputBufSize,
		    double rate, int dType);

void MWDSP_Wafo_Outputs(const void * obj, const void* signal);

int MWDSP_Wafo_Terminate(const void* obj);	/* returns number of instances of block remaining */

#ifdef __cplusplus
}
#endif

#endif /* TOWAVEFILE_H */
