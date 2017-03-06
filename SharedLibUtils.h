/**
 * Use this include file to conditionalize header files
 * for use within a library (BUILD_WITH_EXPORTS) or 
 * outside of the library (i.e. for use by clients of the
 * library).
 */

#include "version.h"  /* defines SUPPORTS_PRAGMA_ONCE, EXPORT, IMPORT, etc */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef SHAREDLIBUTILS_H
#define SHAREDLIBUTILS_H

#ifdef DSP_EXPORTS
/*  We are compiling the library */
#   define DSP_EXPORT    EXPORT
#else
/*  We are clients of the library */
#   define DSP_EXPORT    IMPORT
#endif

#endif
