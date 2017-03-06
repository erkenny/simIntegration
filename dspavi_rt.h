/*
 * dspavi_rt.h Header for avi_rt.c which provides AVI file reading support.
 *
 * Copyright 2005-2006 The MathWorks, Inc.
 * $Revision: 1.1.6.3 $  $Date: 2008/11/18 01:38:57 $
 */

#ifndef DSPAVI_RT_H
#define DSPAVI_RT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dsp_rt.h"
#include "rtwtypes.h"

#ifdef DSPAVI_EXPORTS
#  define DSPAVI_EXPORT EXPORT_FCN
#else
#  define DSPAVI_EXPORT MWDSP_IDECL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Types and defines */
#define AVI_MAX_ERRSTR_LENGTH   1024
/* although we parse up to 8 stream headers, this AVI reader is designed
   to handle only one video and one audio stream; the first found
   video/audio stream will be processed by this AVI reader */
#define AVI_MAX_NUM_STREAMS 8 

/* for parsing dwFlags in main header */
#define AVIF_HASINDEX           0x00000010
#define AVIF_MUSTUSEINDEX       0x00000020
#define AVIF_ISINTERLEAVED      0x00000100
#define AVIF_TRUSTCKTYPE        0x00000800
#define AVIF_WASCAPTUREFILE     0x00010000
#define AVIF_COPYRIGHTED        0x00020000
/* for parsing dwFlags in stream headers */
#define	AVISF_DISABLED          0x00000001
#define	AVISF_VIDEO_PALCHANGES  0x00010000

/* This typedef comes from Microsoft */
typedef struct {
    int32_T dwMicroSecPerFrame;
    int32_T dwMaxBytesPerSec;
    int32_T dwReserved1;
    int32_T dwFlags;
    int32_T dwTotalFrames;
    int32_T dwInitialFrames;
    int32_T dwStreams;
    int32_T dwSuggestedBufferSize;
    int32_T dwWidth;
    int32_T dwHeight;
    int32_T dwReserved[4];
} aviMainHeader_T;

typedef struct {
    boolean_T hasIndex;
    boolean_T mustUseIndex;
    boolean_T isInterleaved;
    boolean_T trustCkType;
    boolean_T wasCapturedFile;
    boolean_T isCopyrighted;
} aviFlags_T;

typedef struct _RECT { 
    int32_T left; 
    int32_T top; 
    int32_T right; 
    int32_T bottom; 
} RECT, *PRECT; 

typedef struct {
    char_T   fccType[5];    /* include space for a null character */
    char_T   fccHandler[5];
    int32_T  dwFlags;
    int32_T  dwPriority;
    int32_T  dwInitialFrames;
    int32_T  dwScale;
    int32_T  dwRate;
    int32_T  dwStart;
    int32_T  dwLength;
    int32_T  dwSuggestedBufferSize;
    int32_T  dwQuality;
    int32_T  dwSampleSize;
    RECT     rcFrame;
    /* this is not part of an MS header but simply interpretation of dwFlags */
    boolean_T isStreamDisabled; /* disabled by default */
    boolean_T videoPalChanges;  /* this avi has updates to colormap */
} aviStreamHeader_T;

/* Audio format tags */
#define AVI_AUDIO_PCM 1

typedef struct waveformat_extended_tag {
    int16_T  wFormatTag;      
    int16_T  nChannels;       
    int32_T  nSamplesPerSec;  
    int32_T  nAvgBytesPerSec; 
    int16_T  nBlockAlign;     
    int16_T  wBitsPerSample;  
    int16_T  cbSize;          
} waveFormatHeader_T;

typedef struct tagBITMAPINFOHEADER {
    int32_T  biSize;          /* was DWORD */
    int32_T  biWidth;         /* was LONG  */
    int32_T  biHeight;        /* was LONG  */
    int16_T  biPlanes;
    int16_T  biBitCount;      /* when > 8 assume truecolor and no colormap */
    int32_T  biCompression;
    int32_T  biSizeImage;
    int32_T  biXPelsPerMeter; /* was LONG */
    int32_T  biYPelsPerMeter; /* was LONG */
    int32_T  biClrUsed;
    int32_T  biClrImportant;
} bitmapFormatHeader_T;

typedef struct tagRGBQUAD {
    uint8_T blue;
    uint8_T green;
    uint8_T red;
    uint8_T reserved;
} rgbquad_T;

typedef int32_T idx_T;

typedef enum
{
    AVI_READ_MODE,
    AVI_WRITE_MODE
} rwMode_T;

typedef struct
{
    FILE     *aviFile;
    rwMode_T  rwMode;     /* defines whether we are writing or reading */
    
    /* various positions */
    int32_T moviPos;
    int32_T hdrlPos;
    int32_T idx1Pos;      /* optional index */
    int32_T endOfHdrlPos; /* this location is followed by stream headers */
    int32_T strlPos[AVI_MAX_NUM_STREAMS];

    int32_T firstVideoFramePos;   /* location of the first video chunk */
    int32_T currentVideoFramePos;

    int32_T firstAudioChunkPos;   /* location of the first audio chunk */
    int32_T currentAudioChunkPos;

    /* info about the AVI file */
    size_t    videoFrameSize;   /* constant value for uncompressed video */
    size_t    audioBufferSize;  /* size of actual audio to be written into a single chunk  */
    size_t    audioChunkSize;   /* may include padding if audioBufferSize is odd;
                                   each chunk length must be even (AVI rule)               */
    size_t    fileSize;         /* AVI file size */
    boolean_T hasAudio;
    boolean_T hasVideo;
    boolean_T isVideoTopDown;  /* indicates if first line is stored on top */

    /* line buffer for flipping the frame upside down when a rare 
       case of top-down avi is encountered */
    char_T   *topDownLineBuf;

    /* structures for holing information read from AVI headers */
    aviMainHeader_T      mainHeader;
    aviStreamHeader_T    streamHeader[AVI_MAX_NUM_STREAMS];
    waveFormatHeader_T   waveFormatHeader;
    bitmapFormatHeader_T bitmapFormatHeader;
    aviFlags_T           aviFlags;

    int_T    audioStreamNumber; /* # of the first uncompressed audio stream */
    int_T    videoStreamNumber;
    char_T   audioStreamTag[5]; /* contains string with chunk label (##wb) */
    char_T   videoStreamTag[5]; /* note: 5 to accomodate null char */
    size_t   audioIdxLen;       /* num entries in AVI index chunk */
    size_t   videoIdxLen;
    idx_T   *audioIdx;
    idx_T   *videoIdx;
    
    /* reading statistics */
    size_t videoFramesRead;     /* total number of video frames read    */
    size_t videoFramesWritten;  /* total number of video frames written */
    size_t audioChunksWritten;  /* total number of audio chunks written */
    size_t audioBytesRead;      /* total number of audio chunks */
    size_t audioChunkBytesRead; /* bytes read in current chunk  */

    /* storage for colormap in case of indexed AVI (num vid bits <= 8) */
    rgbquad_T *colormap;
} avi_T;

/* Core methods */
DSPAVI_EXPORT avi_T   *MWDSP_aviOpen(const char_T *fName, const char *mode); /* use to read/write data */
DSPAVI_EXPORT avi_T   *MWDSP_aviOpenToGetInformation(const char_T *fName);   /* use to get info        */
DSPAVI_EXPORT void     MWDSP_aviClose(avi_T *aviObj);
DSPAVI_EXPORT void     MWDSP_aviReadVideoFrame(avi_T *aviObj, char_T *buf);
DSPAVI_EXPORT void     MWDSP_aviReadRGB24VideoFrame(avi_T *aviObj, char_T *buf);
DSPAVI_EXPORT void     MWDSP_aviReadAudio(avi_T *aviObj, char_T *buf, size_t bufSize);
DSPAVI_EXPORT void     MWDSP_aviResetVideo(avi_T *aviObj);
DSPAVI_EXPORT void     MWDSP_aviResetAudio(avi_T *aviObj);
DSPAVI_EXPORT void     MWDSP_aviSetVideoInfo(avi_T *aviObj, int32_T width, 
                                             int32_T height, real_T frameRate); /* write mode */
DSPAVI_EXPORT void     MWDSP_aviSetAudioInfo(avi_T *aviObj, real_T sampleRate,
                                             int_T numBits, int_T numChannels,
                                             int32_T chunkSize); /* write mode */
DSPAVI_EXPORT void     MWDSP_aviWriteVideoFrame(avi_T *aviObj, char_T *buf);
DSPAVI_EXPORT void     MWDSP_aviWriteAudio(avi_T *aviObj, char_T *buf);

/* Simple helper methods */
DSPAVI_EXPORT boolean_T MWDSP_aviIsEndOfVideo(avi_T *aviObj);
DSPAVI_EXPORT boolean_T MWDSP_aviIsEndOfAudio(avi_T *aviObj);
DSPAVI_EXPORT int32_T   MWDSP_aviGetRGB24VideoFrameSize(avi_T *aviObj);
DSPAVI_EXPORT int32_T   MWDSP_aviGetAudioChunkSize(avi_T *aviObj);
DSPAVI_EXPORT boolean_T MWDSP_aviIsAudioCompressed(avi_T *aviObj);
DSPAVI_EXPORT boolean_T MWDSP_aviIsVideoCompressed(avi_T *aviObj);
DSPAVI_EXPORT void      MWDSP_aviByteSwapOnBigEndian(void *sig, 
                                                     int32_T sigSize,
                                                     int32_T bytesPerSample);

/* Error codes and error handling */
DSPAVI_EXPORT int_T     MWDSP_aviGetErrorCode(void);
DSPAVI_EXPORT char_T   *MWDSP_aviGetErrorMsg(void);

typedef enum
{
    AVI_SUCCESS = 0, 
    AVI_NORIFFC_ERR,         /* RIFF 4cc is missing */
    AVI_NOAVIC_ERR,          /* AVI  4cc is missing */
    AVI_FOPEN_ERR,           /* Could not open file */
    AVI_FSEEK_ERR,           /* Seek operation failed */
    AVI_FREAD_ERR,           /* Fread operation failed */
    AVI_FWRITE_ERR,          /* Fwrite operation failed */
    AVI_FRAMES_MISSING_ERR,  /* Number of frames reported in header does not
                                match number of ##db chunks */
    AVI_AUDIO_MISSING_ERR,   /* Audio chunks are missing */
    AVI_VIDEO_MISSING_ERR,   /* Video chunks are missing */
    AVI_AVIH_MISSING_ERR,    /* Main AVI header seems to be missing */
    AVI_STRL_MISSING_ERR,    /* One of expected stream lists is missing */
    AVI_BAD_STRL_LEN_ERR,    /* Encountered unexpected length of strl chunk */
    AVI_STRH_MISSING_ERR,    /* One of expected stream headers is missing */
    AVI_WAVE_FORMAT_ERR,     /* Problem while reading Wave format header */
    AVI_BITMAP_FORMAT_ERR,   /* Problem while reading Bitmap format header */
    AVI_BAD_BMP_HEADER_ERR,  /* Unsupported bitmap header, unexp. length */
    AVI_BAD_MAIN_HEADER_ERR, /* Unsupported main header, unexp. length */
    AVI_BAD_WAVE_HEADER_ERR, /* Unsupported wave header, unexp. length */
    AVI_COLORMAP_READ_ERR,   /* Could not read the colormap */
    AVI_OUTOFMEM_ERR,        /* Out of memory error */
    AVI_TOOMANY_STREAMS_ERR, /* Number of expected streams was exceeded */
    AVI_INTERLEAVED_ERR,     /* Unsupported interleaved AVI */
    AVI_BAD_AUDIO_BITS_ERR,  /* We support only 8 or 16 bits audio */
    AVI_MUST_USE_INDEX_ERR,  /* Unsupported AVI that requires reading index */
    AVI_VIDEO_COMP_ERR,      /* Compressed video streams are unsupported */
    AVI_AUDIO_COMP_ERR,      /* Compressed audio streams are unsupported */
    AVI_RW_MODE_ERR,         /* User of the library specified unsupported R/W mode */
    AVI_UNKNOWN_ERR          /* Unknown problem */
} error_T;

#ifdef __cplusplus
}
#endif

#ifdef MWDSP_INLINE_DSPRTLIB
#  include "dspavi/dspavi_rt.c"
#endif /* MWDSP_INLINE_DSPRTLIB */

#endif /* DSPAVI_RT_H */
