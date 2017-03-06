/* $Revision: 1.4.4.4 $ 
 *
 * towavedevice_win32.h
 * Runtime library header file for Windows "To Wave Device" block.
 * Link:  toolbox/dspblks/lib/win32/towavedevice.lib
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 */

#ifndef TOWAVEDEVICE_H
#define TOWAVEDEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Exported function types
 */

typedef int	    (*MWDSP_WAO_GETERRCODE_FUNC) (void);
typedef const char* (*MWDSP_WAO_GETERRMSG_FUNC) (void);

typedef void* (*MWDSP_WAO_CREATE_FUNC)		(double, unsigned short, unsigned int, unsigned int, int, double, unsigned int,
						 double, unsigned int, unsigned short);
typedef void  (*MWDSP_WAO_START_FUNC)		(const void*);
typedef void  (*MWDSP_WAO_UPDATE_FUNC)		(const void*, const void*);
typedef int  (*MWDSP_WAO_TERMINATE_FUNC)	(const void*);

typedef struct
{
	MWDSP_WAO_GETERRCODE_FUNC getErrorCode;
	MWDSP_WAO_GETERRMSG_FUNC  getErrorMessage;
	MWDSP_WAO_CREATE_FUNC     createFcn;
	MWDSP_WAO_START_FUNC      startFcn;
	MWDSP_WAO_UPDATE_FUNC     updateFcn;
	MWDSP_WAO_TERMINATE_FUNC  terminateFcn;
} MWDSP_Wao_FunctionPtrStruct;
#define MWDSP_WAO_NUM_FUNCTIONS 6

#define MWDSP_WAO_LIB_NAME	    "towavedevice.dll"

#define MWDSP_WAO_GETERRCODE_NAME  "MWDSP_Wao_GetErrorCode"
#define MWDSP_WAO_GETERRMSG_NAME   "MWDSP_Wao_GetErrorMessage"

#define MWDSP_WAO_CREATE_NAME	    "MWDSP_Wao_Create"
#define MWDSP_WAO_START_NAME	    "MWDSP_Wao_Start"
#define MWDSP_WAO_UPDATE_NAME	    "MWDSP_Wao_Update"
#define MWDSP_WAO_TERMINATE_NAME    "MWDSP_Wao_Terminate"

/*
 * Error message function prototypes
 */

int MWDSP_Wao_GetErrorCode(void);
const char* MWDSP_Wao_GetErrorMessage(void);

/*
 * The public interface
 */

void* MWDSP_Wao_Create(double rate, unsigned short bits, unsigned int chans, unsigned int inputBufSize, int dType,
		       double bufSizeInSeconds, unsigned int internalBufSize, double initialDelay, unsigned int whichDevice, 
			unsigned short useTheWaveMapper);

void MWDSP_Wao_Start(const void* obj);

void MWDSP_Wao_Update(const void * obj, const void* signal);

/* returns the number of instances of the "block" after this one is destroyed */
int MWDSP_Wao_Terminate(const void* obj);

#ifdef __cplusplus
}
#endif

#endif /* TOWAVEDEVICE_H */
