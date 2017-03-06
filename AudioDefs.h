/*
 * AudioDefs.h - Definitions for audio I/O
 *
 *  Copyright 1995-2004 The MathWorks, Inc.
 *  $Revision: 1.1.6.5 $  $Date: 2008/11/18 01:38:33 $
 */

#ifndef AUDIODEFS_H
#define AUDIODEFS_H


/* these follow Simulink data types, in simstruc_types.h */
typedef enum
{
    AudioDataType_Double = 0,	/* double */
    AudioDataType_Single,		/* float */
    AudioDataType_Int8,			/* char */
    AudioDataType_Uint8,		/* unsigned char */
    AudioDataType_Int16,		/* short */
    AudioDataType_Uint16,		/* unsigned short */
    AudioDataType_Int32,		/* long */
    AudioDataType_Uint32,		/* unsigned long */
    AudioDataType_Boolean		/* bool */
} AudioDataType;


typedef struct
{
    unsigned char isValid;       /* zero if the file has no audio */
    double sampleRate;	         /* audio sample rate */
    int numBits;                 /* audio bit depth */
    int numChannels;             /* number of audio channels (typically 1 or two) */
    int frameSize;               /* number of audio samples per frame */
    AudioDataType dataType;      /* data type of audio samples to be passed in */
    const char* audioCompressor; /* typically set to NULL for none */
} MMAudioInfo;


#endif	/* AUDIODEFS_H */

