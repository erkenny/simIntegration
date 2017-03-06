/*
 * VideoDefs.h - Definitions for video I/O
 *
 *  Copyright 1995-2007 The MathWorks, Inc.
 *  $Revision: 1.1.6.7 $  $Date: 2008/11/18 01:38:48 $
 */

#ifndef VIDEODEFS_H
#define VIDEODEFS_H

typedef enum
{
	VideoFrame_RowMajor = 0,
	VideoFrame_ColumnMajor
} VideoFrameOrientation;

typedef enum
{
	LineOrder_TopDown = 0,
	LineOrder_BottomUp
} VideoFrameLineOrder;

/* these follow Simulink data types, in simstruc_types.h */
typedef enum
{
	VideoDataType_Double = 0,	/* double */
	VideoDataType_Single,		/* float */
	VideoDataType_Int8,			/* char */
	VideoDataType_Uint8,		/* unsigned char */
	VideoDataType_Int16,		/* short */
	VideoDataType_Uint16,		/* unsigned short */
	VideoDataType_Int32,		/* long */
	VideoDataType_Uint32,		/* unsigned long */
	VideoDataType_Boolean		/* bool */
} VideoDataType;

/* VideoOutputType: used by the to video device component to specify where 
the incoming video signal should go... */

typedef enum
{
	VideoOutput_Window = 0,
	VideoOutput_Device
} VideoOutputType;

typedef struct
{
    unsigned char isValid;    /* Zero if the file has no video */
	double frameRate;		  /* video num frames per second */
	double frameRateComputed; /* video num frames per second, as computed by the video framework.
                               * The reason that this field is necessary is that on Windows, 
                               * this number is less accurate than the frameRate field, yet it is essential
                               * to use this value in methods that determine which frame to emit.
                               * The frameRate field determines the Sample Time on the 
                               * Simulink wire, to be consistent with the UNIX implementation.
                               */
	int videoWidth;			  /* width of video image */
	int videoHeight;		  /* height of video image */
	VideoDataType dataType;	  /* data type of incoming video image pixels */
	VideoFrameOrientation orientation;  /* row- or column-major */
	const char* videoCompressor;        /* typically set to NULL for none */
    int   isOut3D;            /* single output port with a 3-D signal or 3 seperate R,G and B ports. */
} MMVideoInfo;


#endif	/* VIDEODEFS_H */

