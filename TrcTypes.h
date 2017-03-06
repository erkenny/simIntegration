#ifndef __TRCTYPES_H_9B2DFDD0_64C9_11d1_9812_00609777269C_INCLUDED
#define __TRCTYPES_H_9B2DFDD0_64C9_11d1_9812_00609777269C_INCLUDED

/* TM 21.08.96
   The definition of the TrcObjType bases on the following rules:
   - (TrcObjType & 1) must be a pointer type.
   - (TrcObjType < IntType) must be a floating point type.
*/
typedef enum
{
  /* floating point DSP data types */
  FltType = 0,
  FltPtrType,

  /* new floating point data types for IEEE-hardware */
  FltIeee32Type,
  FltIeee32PtrType,
  FltIeee64Type,
  FltIeee64PtrType,

  /* integer DSP data types */
  IntType,
  IntPtrType,

  /* new integer data types */
  Int8Type,
  Int8PtrType,
  Int16Type,
  Int16PtrType,

  /* new 64-bit integer data types */
  Int64Type,
  Int64PtrType,

  /* new unsigned integer data types */
  UInt8Type,
  UInt8PtrType,
  UInt16Type,
  UInt16PtrType,
  UInt32Type,
  UInt32PtrType,
  UInt64Type,
  UInt64PtrType,

  /* new string data types */
  StringType,
  StringPtrType,

  /* new structure data types */
  StructType,
  StructPtrType

} TrcObjType;

/* code changed by tmi on 23.02.00 */
typedef struct _TrcSampPeriodRec
{
    double  SamplPeriod;        /* Keyword: value: <val>        */
    char *  Alias;              /* Keyword: alias: <val>        */
    double  Increment;          /* Keyword: increment: <val>    */
    char *  Unit;               /* Keyword: unit: <val>         */
} TrcSampPeriodRec, *TrcSampPeriodPtr;

typedef struct _TrcKeywRec
{
  TrcSampPeriodPtr* SamplPeriods;       /* Keyword: sampling_period[x] = <val>  */
/* end of code changed by tmi on 23.02.00 */
  int               SamplPeriodOccured; /* Flag   : sampling_period used ?      */

  char *            Application;        /* Keyword: _application "pathname"     */
  int               ApplicationOccured; /* Flag   : _application used ?         */

  char *            GenName;            /* Keyword: _genname "productname"      */
  char *            GenVersion;         /* Keyword: _genversion "productvs"     */
  char *            GenDate;            /* Keyword: _gendate "date time"        */

  char *            Description;        /* Keyword: _description "text"         */
  char *            Author;             /* Keyword: _author "auhtorname"        */
  char *            Model;              /* Keyword: _model "model(file)name"    */
  char *            ModelID;            /* Keyword: _modelid "datetime/rnd-nr"  */

  /* to be augmented */

} TrcKeywRec, * TrcKeywPtr;

/* Errors returned by procedure 'TRCPARS_Read' */
typedef enum
{
  PARSE_NO_ERROR = 0,            /* no error                  */
  PARSE_TRC_ERROR,               /* syntax or semantic error  */
  PARSE_TRC_NOT_FOUND,           /* .trc file does not exist  */
  PARSE_CANNOT_OPEN_TRC_FILE,    /* no access to .trc file    */
  PARSE_LINK_MAP_ERROR,          /* not used for .trc file    */
  PARSE_LINK_MAP_NOT_FOUND,      /* not used for .trc file    */
  PARSE_CANNOT_OPEN_LINK_MAPFILE,/* not used for .trc file    */
  PARSE_VARIABLES_OBSOLETE,      /* not used for .trc file    */
  PARSE_CANNOT_CREATE_ERR_FILE,  /* .err could not be created */
  PARSE_MEMORY_FULL,             /* memory allocation error   */
  PARSE_TOO_MANY_VARIABLES,      /* over 16000 elems (16-bit) */
  PARSE_FATAL_ERROR				 /* unrecoverable err, e.g. token to long */

} TrcParseError;

/* set mode for .errfile format */
typedef enum _ErrMsgMode
{
  ERRMSG_VERBOSE,
  ERRMSG_SHORT

} ErrMsgMode;

#define OBJ_NAME_1_LEN	128
#define OBJ_NAME_2_LEN	128
#define OBJ_UNIT_LEN	32
#define OBJ_FMT_LEN	    32

/* TrcVariant Data Type */
typedef enum _TrcVarType
{
	TVT_EMPTY = 0,  /* No Value */
	TVT_I2,         /* short */
	TVT_I4,         /* long */
	TVT_R4,         /* float */
	TVT_R8,         /* double */
	TVT_VAR,        /* Another Variant */
	TVT_CHAR,       /* Character or String */
	TVT_PTR,         /* Any Pointer */
    TVT_MATRIX      /* 2D Matrix */
} TRCVARTYPE;

typedef struct _TrcVarRec *TrcVarPtr;

typedef struct _TrcMatrixRec
{
    long    x;
    long    y;
    TrcVarPtr varVal;
} TrcMatrixRec, *TrcMatrixPtr;

typedef struct _TrcVarRec
{
	TRCVARTYPE	type;
	union
    {
	    short		iVal;
	    long		lVal;
	    float		fVal;
	    double	    dVal;
	    TrcVarPtr	varVal;
	    char		*pszVal;
	    void		*pVal;
        TrcMatrixRec    matrixVal;
#if defined(_DS1401) || defined(_DS1103) || defined(_DS1005) || defined(_DS1104)
    } TrcVariant;
#else /*  */
	} ;
#endif /*  */
} TrcVarRec;

typedef struct _TrcSizeRec
{
    long lMin;
    long lMax;
} TrcSizeRec, *TrcSizePtr;

/* base type structure
   the type member identifies the real type
   0 = TrcArrayRec */
typedef struct _TrcTypeRec
{
    short   type;
    char    type_name[OBJ_NAME_1_LEN];
} TrcTypeRec, *TrcTypePtr;

typedef struct _TrcArrayRec
{
    short       type;                           /* must be 0 */
    char        type_name[OBJ_NAME_1_LEN];
    TrcObjType  type_basetype;
    short       type_dim;
    TrcSizePtr  *type_dim_size;
    long        type_flags;
} TrcArrayRec, *TrcArrayPtr;

/* trc flags */
#define TRCF_READONLY   0x00000001  /* syntax-flag */
#define TRCF_HIDDEN     0x00000002  /* syntax-flag, invisible by trc file */
#define TRCF_PARAM      0x00000004  /* syntax-flag */
#define TRCF_MASKED     0x00000008  /* syntax-flag */
#define TRCF_POINTER    0x00000010  /* internal flag */
#define TRCF_LOOKUP     0x00000020  /* internal flag */
#define TRCF_COLLAPSED  0x00000040  /* syntax-flag */
#define TRCF_BLOCK      0x00000080  /* syntax-flag */
#define TRCF_FILTERED   0x00000100  /* internal flag, invisible by filter manager */
#define TRCF_MARKED     0x00000200  /* syntax-flag */
#define TRCF_SELECTED   0x00000400  /* internal flag */
#define TRCF_RESERVED   0x00000800  /* syntax-flag, user-defined usage */
#define TRCF_LABEL      0x00001000  /* syntax-flag */
#define TRCF_SINK       0x00002000  /* syntax-flag */
#define TRCF_DSM        0x00004000  /* syntax-flag */
#define TRCF_XDISC      0x00008000  /* syntax-flag */
#define TRCF_XCONT      0x00010000  /* syntax-flag */
#define TRCF_DERIV      0x00020000  /* syntax-flag */
#define TRCF_SYSTEM     0x00040000  /* syntax-flag */
#define TRCF_OUTPUT     0x00080000  /* syntax-flag */
/* code changed by gs on 22.11.99 */
#define TRCF_WORKSPACE  0x00100000  /* syntax-flag */
/* end of code changed by gs on 22.11.99 */
/* code changed by tmi on 23.10.00 */
#define TRCF_COLORIENT  0x00200000  /* internal flag */
/* end of code changed by tmi on 23.10.00 */
/* code changed by tmi on 06.11.00 */
#define TRCF_VECTOR     0x00400000  /* internal flag */
/* end of code changed by tmi on 06.11.00 */
/* code changed by tmi on 13.12.00 */
#define TRCF_DISABLED   0x00800000  /* internal flag */
/* end of code changed by tmi on 13.12.00 */

#endif /* __TRCTYPES_H_9B2DFDD0_64C9_11d1_9812_00609777269C_INCLUDED */
