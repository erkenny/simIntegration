/* $Revision: 1.1.6.1 $ 
 *
 * dll_utils_win.h
 * Only used by "external mode" (_ex_ in filename) audio/video block headers.
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 */

#ifndef DLL_UTILS_WIN_H
#define DLL_UTILS_WIN_H

typedef int (*MWDSP_windll_getErrCode)(void);
typedef const char* (*MWDSP_windll_getErrMsg)(void);

#define		ERR_NO_ERROR				0
#define		ERR_LOAD_LIBRARY_FAILED		1

int MWDSP_windll_GetErrorCode(void);

const char* MWDSP_windll_GetErrorMessage(void);

void MWDSP_windll_SetErrorFunctionPtrs(MWDSP_windll_getErrCode* errCodeFcn,
												MWDSP_windll_getErrMsg* errMsgFcn);

void MWDSP_windll_SetErrorMessage(void);

void MWDSP_windll_LoadLibraryFailed(const char* libName, 
												MWDSP_windll_getErrCode* errCodeFcn,
												MWDSP_windll_getErrMsg* errMsgFcn);


#endif /* DLL_UTILS_WIN_H */
