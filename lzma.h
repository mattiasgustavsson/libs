/*
 * This is a modified version of of Igor Pavlov's LzmaLib. Basically, I 
 * merged it into a single-file header-only library, made a few changes
 * to make the code compile in both C and C++. The original code is 
 * public domain, and I place my version into the public domain as well.
 *
 *                                                  /Mattias Gustavsson
 * Do this:
 *		#define LZMA_IMPLEMENTATION
 * before including this file in *one* C/C++ file to get the implementation
 *
 */
 
#ifndef LZMA_H
#define LZMA_H

#include <stddef.h>

/* LzmaLib.h -- LZMA library interface
2013-01-18 : Igor Pavlov : Public domain */


#define LZMA_PROPS_SIZE 5


#define SZ_OK 0

#define SZ_ERROR_DATA 1
#define SZ_ERROR_MEM 2
#define SZ_ERROR_CRC 3
#define SZ_ERROR_UNSUPPORTED 4
#define SZ_ERROR_PARAM 5
#define SZ_ERROR_INPUT_EOF 6
#define SZ_ERROR_OUTPUT_EOF 7
#define SZ_ERROR_READ 8
#define SZ_ERROR_WRITE 9
#define SZ_ERROR_PROGRESS 10
#define SZ_ERROR_FAIL 11

#define SZ_ERROR_ARCHIVE 16
#define SZ_ERROR_NO_ARCHIVE 17


/*
RAM requirements for LZMA:
  for compression:   (dictSize * 11.5 + 6 MB) + state_size
  for decompression: dictSize + state_size
    state_size = (4 + (1.5 << (lc + lp))) KB
    by default (lc=3, lp=0), state_size = 16 KB.

LZMA properties (5 bytes) format
    Offset Size  Description
      0     1    lc, lp and pb in encoded form.
      1     4    dictSize (little endian).
*/

/*
LzmaCompress
------------

outPropsSize -
     In:  the pointer to the size of outProps buffer; *outPropsSize = LZMA_PROPS_SIZE = 5.
     Out: the pointer to the size of written properties in outProps buffer; *outPropsSize = LZMA_PROPS_SIZE = 5.

  LZMA Encoder will use defult values for any parameter, if it is
  -1  for any from: level, loc, lp, pb, fb
   0  for dictSize

level - compression level: 0 <= level <= 9;

  level dictSize algo  fb
    0:    16 KB   0    32
    1:    64 KB   0    32
    2:   256 KB   0    32
    3:     1 MB   0    32
    4:     4 MB   0    32
    5:    16 MB   1    32
    6:    32 MB   1    32
    7+:   64 MB   1    64

  The default value for "level" is 5.

  algo = 0 means fast method
  algo = 1 means normal method

dictSize - The dictionary size in bytes. The maximum value is
        128 MB = (1 << 27) bytes for 32-bit version
          1 GB = (1 << 30) bytes for 64-bit version
     The default value is 16 MB = (1 << 24) bytes.
     It's recommended to use the dictionary that is larger than 4 KB and
     that can be calculated as (1 << N) or (3 << N) sizes.

lc - The number of literal context bits (high bits of previous literal).
     It can be in the range from 0 to 8. The default value is 3.
     Sometimes lc=4 gives the gain for big files.

lp - The number of literal pos bits (low bits of current position for literals).
     It can be in the range from 0 to 4. The default value is 0.
     The lp switch is intended for periodical data when the period is equal to 2^lp.
     For example, for 32-bit (4 bytes) periodical data you can use lp=2. Often it's
     better to set lc=0, if you change lp switch.

pb - The number of pos bits (low bits of current position).
     It can be in the range from 0 to 4. The default value is 2.
     The pb switch is intended for periodical data when the period is equal 2^pb.

fb - Word size (the number of fast bytes).
     It can be in the range from 5 to 273. The default value is 32.
     Usually, a big number gives a little bit better compression ratio and
     slower compression process.


Out:
  destLen  - processed output size
Returns:
  SZ_OK               - OK
  SZ_ERROR_MEM        - Memory allocation error
  SZ_ERROR_PARAM      - Incorrect paramater
  SZ_ERROR_OUTPUT_EOF - output buffer overflow
*/

int LzmaCompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen,
  unsigned char *outProps, size_t *outPropsSize, /* *outPropsSize must be = 5 */
  int level,      /* 0 <= level <= 9, default = 5 */
  unsigned dictSize,  /* default = (1 << 24) */
  int lc,        /* 0 <= lc <= 8, default = 3  */
  int lp,        /* 0 <= lp <= 4, default = 0  */
  int pb,        /* 0 <= pb <= 4, default = 2  */
  int fb,        /* 5 <= fb <= 273, default = 32 */
  int (*progress)( size_t inSize, size_t outSize, void* userData ),
  void* userData
);

/*
LzmaUncompress
--------------
In:
  dest     - output data
  destLen  - output data size
  src      - input data
  srcLen   - input data size
Out:
  destLen  - processed output size
  srcLen   - processed input size
Returns:
  SZ_OK                - OK
  SZ_ERROR_DATA        - Data error
  SZ_ERROR_MEM         - Memory allocation arror
  SZ_ERROR_UNSUPPORTED - Unsupported properties
  SZ_ERROR_INPUT_EOF   - it needs more bytes in input buffer (src)
*/

int LzmaUncompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t *srcLen,
  const unsigned char *props, size_t propsSize);


#endif /* LZMA_H */





#ifdef LZMA_IMPLEMENTATION

#define Int32 signed int
#define Int64 signed long long
#define Byte unsigned char
#define UInt16 unsigned short
#define UInt32 unsigned int
#define UInt64 unsigned long long

#define UNUSED_VAR(x) (void)x;





/* 7zTypes.h -- Basic types
2018-08-04 : Igor Pavlov : Public domain */


typedef int SRes;


#ifndef RINOK
#define RINOK(x) { int __result__ = (x); if (__result__ != 0) return __result__; }
#endif

typedef int BoolInt;
/* typedef BoolInt Bool; */
#define True 1
#define False 0


typedef struct ISeqInStream ISeqInStream;
struct ISeqInStream
{
  SRes (*Read)(const ISeqInStream *p, void *buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
       (output(*size) < input(*size)) is allowed */
};
#define ISeqInStream_Read(p, buf, size) (p)->Read(p, buf, size)

/* it can return SZ_ERROR_INPUT_EOF */
SRes SeqInStream_Read(const ISeqInStream *stream, void *buf, size_t size);
SRes SeqInStream_Read2(const ISeqInStream *stream, void *buf, size_t size, SRes errorType);
SRes SeqInStream_ReadByte(const ISeqInStream *stream, Byte *buf);


typedef struct ISeqOutStream ISeqOutStream;
struct ISeqOutStream
{
  size_t (*Write)(const ISeqOutStream *p, const void *buf, size_t size);
    /* Returns: result - the number of actually written bytes.
       (result < size) means error */
};
#define ISeqOutStream_Write(p, buf, size) (p)->Write(p, buf, size)


typedef struct ICompressProgress ICompressProgress;

struct ICompressProgress
{
  SRes (*Progress)(const ICompressProgress *p, UInt64 inSize, UInt64 outSize);
    /* Returns: result. (result != SZ_OK) means break.
       Value (UInt64)(Int64)-1 for size means unknown value. */
};
#define ICompressProgress_Progress(p, inSize, outSize) (p)->Progress(p, inSize, outSize)


typedef struct ISzAlloc ISzAlloc;
typedef const ISzAlloc * ISzAllocPtr;

struct ISzAlloc
{
  void *(*Alloc)(ISzAllocPtr p, size_t size);
  void (*Free)(ISzAllocPtr p, void *address); /* address can be 0 */
};

#define ISzAlloc_Alloc(p, size) (p)->Alloc(p, size)
#define ISzAlloc_Free(p, a) (p)->Free(p, a)

/* deprecated */
#define IAlloc_Alloc(p, size) ISzAlloc_Alloc(p, size)
#define IAlloc_Free(p, a) ISzAlloc_Free(p, a)


#ifndef MY_offsetof
  #ifdef offsetof
    #define MY_offsetof(type, m) offsetof(type, m)
    /*
    #define MY_offsetof(type, m) FIELD_OFFSET(type, m)
    */
  #else
    #define MY_offsetof(type, m) ((size_t)&(((type *)0)->m))
  #endif
#endif



#ifndef MY_container_of

/*
#define MY_container_of(ptr, type, m) container_of(ptr, type, m)
#define MY_container_of(ptr, type, m) CONTAINING_RECORD(ptr, type, m)
#define MY_container_of(ptr, type, m) ((type *)((char *)(ptr) - offsetof(type, m)))
#define MY_container_of(ptr, type, m) (&((type *)0)->m == (ptr), ((type *)(((char *)(ptr)) - MY_offsetof(type, m))))
*/

/*
  GCC shows warning: "perhaps the 'offsetof' macro was used incorrectly"
    GCC 3.4.4 : classes with constructor
    GCC 4.8.1 : classes with non-public variable members"
*/

#define MY_container_of(ptr, type, m) ((type *)((char *)(1 ? (ptr) : &((type *)0)->m) - MY_offsetof(type, m)))


#endif

#define CONTAINER_FROM_VTBL_SIMPLE(ptr, type, m) ((type *)(ptr))

/*
#define CONTAINER_FROM_VTBL(ptr, type, m) CONTAINER_FROM_VTBL_SIMPLE(ptr, type, m)
*/
#define CONTAINER_FROM_VTBL(ptr, type, m) MY_container_of(ptr, type, m)

#define CONTAINER_FROM_VTBL_CLS(ptr, type, m) CONTAINER_FROM_VTBL_SIMPLE(ptr, type, m)
/*
#define CONTAINER_FROM_VTBL_CLS(ptr, type, m) CONTAINER_FROM_VTBL(ptr, type, m)
*/



#ifdef _MSC_VER

#if _MSC_VER >= 1300
#define MY_NO_INLINE __declspec(noinline)
#else
#define MY_NO_INLINE
#endif

#define MY_FORCE_INLINE __forceinline

#define MY_CDECL __cdecl
#define MY_FAST_CALL __fastcall

#else

#define MY_NO_INLINE
#define MY_FORCE_INLINE
#define MY_CDECL
#define MY_FAST_CALL

/* inline keyword : for C++ / C99 */

/* GCC, clang: */
/*
#if defined (__GNUC__) && (__GNUC__ >= 4)
#define MY_FORCE_INLINE __attribute__((always_inline))
#define MY_NO_INLINE __attribute__((noinline))
#endif
*/

#endif





/* Alloc.h -- Memory allocation functions
2018-02-19 : Igor Pavlov : Public domain */

void *MyAlloc(size_t size);
void MyFree(void *address);

#ifdef _WIN32

void SetLargePageSize( void );

void *MidAlloc(size_t size);
void MidFree(void *address);
void *BigAlloc(size_t size);
void BigFree(void *address);

#else

#define MidAlloc(size) MyAlloc(size)
#define MidFree(address) MyFree(address)
#define BigAlloc(size) MyAlloc(size)
#define BigFree(address) MyFree(address)

#endif

extern const ISzAlloc g_Alloc;
extern const ISzAlloc g_BigAlloc;
extern const ISzAlloc g_MidAlloc;
extern const ISzAlloc g_AlignedAlloc;


typedef struct
{
  ISzAlloc vt;
  ISzAllocPtr baseAlloc;
  unsigned numAlignBits; /* ((1 << numAlignBits) >= sizeof(void *)) */
  size_t offset;         /* (offset == (k * sizeof(void *)) && offset < (1 << numAlignBits) */
} CAlignOffsetAlloc;

void AlignOffsetAlloc_CreateVTable(CAlignOffsetAlloc *p);





/*  LzmaEnc.h -- LZMA Encoder
2017-07-27 : Igor Pavlov : Public domain */

typedef struct _CLzmaEncProps
{
  int level;       /* 0 <= level <= 9 */
  UInt32 dictSize; /* (1 << 12) <= dictSize <= (1 << 27) for 32-bit version
                      (1 << 12) <= dictSize <= (3 << 29) for 64-bit version
                      default = (1 << 24) */
  int lc;          /* 0 <= lc <= 8, default = 3 */
  int lp;          /* 0 <= lp <= 4, default = 0 */
  int pb;          /* 0 <= pb <= 4, default = 2 */
  int algo;        /* 0 - fast, 1 - normal, default = 1 */
  int fb;          /* 5 <= fb <= 273, default = 32 */
  int btMode;      /* 0 - hashChain Mode, 1 - binTree mode - normal, default = 1 */
  int numHashBytes; /* 2, 3 or 4, default = 4 */
  UInt32 mc;       /* 1 <= mc <= (1 << 30), default = 32 */
  unsigned writeEndMark;  /* 0 - do not write EOPM, 1 - write EOPM, default = 0 */

  UInt64 reduceSize; /* estimated size of data that will be compressed. default = (UInt64)(Int64)-1.
                        Encoder uses this value to reduce dictionary size */
} CLzmaEncProps;

void LzmaEncProps_Init(CLzmaEncProps *p);
void LzmaEncProps_Normalize(CLzmaEncProps *p);
UInt32 LzmaEncProps_GetDictSize(const CLzmaEncProps *props2);


/* ---------- CLzmaEncHandle Interface ---------- */

/* LzmaEnc* functions can return the following exit codes:
SRes:
  SZ_OK           - OK
  SZ_ERROR_MEM    - Memory allocation error
  SZ_ERROR_PARAM  - Incorrect paramater in props
  SZ_ERROR_WRITE  - ISeqOutStream write callback error
  SZ_ERROR_OUTPUT_EOF - output buffer overflow - version with (Byte *) output
  SZ_ERROR_PROGRESS - some break from progress callback
*/

typedef void * CLzmaEncHandle;

CLzmaEncHandle LzmaEnc_Create(ISzAllocPtr alloc);
void LzmaEnc_Destroy(CLzmaEncHandle p, ISzAllocPtr alloc, ISzAllocPtr allocBig);

SRes LzmaEnc_SetProps(CLzmaEncHandle p, const CLzmaEncProps *props);
void LzmaEnc_SetDataSize(CLzmaEncHandle p, UInt64 expectedDataSiize);
SRes LzmaEnc_WriteProperties(CLzmaEncHandle p, Byte *properties, size_t *size);
unsigned LzmaEnc_IsWriteEndMark(CLzmaEncHandle p);

SRes LzmaEnc_Encode(CLzmaEncHandle p, ISeqOutStream *outStream, ISeqInStream *inStream,
    ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig);
SRes LzmaEnc_MemEncode(CLzmaEncHandle p, Byte *dest, size_t *destLen, const Byte *src, size_t srcLen,
    int writeEndMark, ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig);


/* ---------- One Call Interface ---------- */

SRes LzmaEncode(Byte *dest, size_t *destLen, const Byte *src, size_t srcLen,
    const CLzmaEncProps *props, Byte *propsEncoded, size_t *propsSize, int writeEndMark,
    ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig);





/* LzmaDec.h -- LZMA Decoder
2018-04-21 : Igor Pavlov : Public domain */

/* #define _LZMA_PROB32 */
/* _LZMA_PROB32 can increase the speed on some CPUs,
   but memory usage for CLzmaDec::probs will be doubled in that case */

typedef
#ifdef _LZMA_PROB32
  UInt32
#else
  UInt16
#endif
  CLzmaProb;


/* ---------- LZMA Properties ---------- */

#define LZMA_PROPS_SIZE 5

typedef struct _CLzmaProps
{
  Byte lc;
  Byte lp;
  Byte pb;
  Byte _pad_;
  UInt32 dicSize;
} CLzmaProps;

/* LzmaProps_Decode - decodes properties
Returns:
  SZ_OK
  SZ_ERROR_UNSUPPORTED - Unsupported properties
*/

SRes LzmaProps_Decode(CLzmaProps *p, const Byte *data, unsigned size);


/* ---------- LZMA Decoder state ---------- */

/* LZMA_REQUIRED_INPUT_MAX = number of required input bytes for worst case.
   Num bits = log2((2^11 / 31) ^ 22) + 26 < 134 + 26 = 160; */

#define LZMA_REQUIRED_INPUT_MAX 20

typedef struct
{
  /* Don't change this structure. ASM code can use it. */
  CLzmaProps prop;
  CLzmaProb *probs;
  CLzmaProb *probs_1664;
  Byte *dic;
  size_t dicBufSize;
  size_t dicPos;
  const Byte *buf;
  UInt32 range;
  UInt32 code;
  UInt32 processedPos;
  UInt32 checkDicSize;
  UInt32 reps[4];
  UInt32 state;
  UInt32 remainLen;

  UInt32 numProbs;
  unsigned tempBufSize;
  Byte tempBuf[LZMA_REQUIRED_INPUT_MAX];
} CLzmaDec;

#define LzmaDec_Construct(p) { (p)->dic = NULL; (p)->probs = NULL; }

void LzmaDec_Init(CLzmaDec *p);

/* There are two types of LZMA streams:
     - Stream with end mark. That end mark adds about 6 bytes to compressed size.
     - Stream without end mark. You must know exact uncompressed size to decompress such stream. */

typedef enum
{
  LZMA_FINISH_ANY,   /* finish at any point */
  LZMA_FINISH_END    /* block must be finished at the end */
} ELzmaFinishMode;

/* ELzmaFinishMode has meaning only if the decoding reaches output limit !!!

   You must use LZMA_FINISH_END, when you know that current output buffer
   covers last bytes of block. In other cases you must use LZMA_FINISH_ANY.

   If LZMA decoder sees end marker before reaching output limit, it returns SZ_OK,
   and output value of destLen will be less than output buffer size limit.
   You can check status result also.

   You can use multiple checks to test data integrity after full decompression:
     1) Check Result and "status" variable.
     2) Check that output(destLen) = uncompressedSize, if you know real uncompressedSize.
     3) Check that output(srcLen) = compressedSize, if you know real compressedSize.
        You must use correct finish mode in that case. */

typedef enum
{
  LZMA_STATUS_NOT_SPECIFIED,               /* use main error code instead */
  LZMA_STATUS_FINISHED_WITH_MARK,          /* stream was finished with end mark. */
  LZMA_STATUS_NOT_FINISHED,                /* stream was not finished */
  LZMA_STATUS_NEEDS_MORE_INPUT,            /* you must provide more input bytes */
  LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK  /* there is probability that stream was finished without end mark */
} ELzmaStatus;

/* ELzmaStatus is used only as output value for function call */


/* ---------- Interfaces ---------- */

/* There are 3 levels of interfaces:
     1) Dictionary Interface
     2) Buffer Interface
     3) One Call Interface
   You can select any of these interfaces, but don't mix functions from different
   groups for same object. */


/* There are two variants to allocate state for Dictionary Interface:
     1) LzmaDec_Allocate / LzmaDec_Free
     2) LzmaDec_AllocateProbs / LzmaDec_FreeProbs
   You can use variant 2, if you set dictionary buffer manually.
   For Buffer Interface you must always use variant 1.

LzmaDec_Allocate* can return:
  SZ_OK
  SZ_ERROR_MEM         - Memory allocation error
  SZ_ERROR_UNSUPPORTED - Unsupported properties
*/

SRes LzmaDec_AllocateProbs(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAllocPtr alloc);
void LzmaDec_FreeProbs(CLzmaDec *p, ISzAllocPtr alloc);

SRes LzmaDec_Allocate(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAllocPtr alloc);
void LzmaDec_Free(CLzmaDec *p, ISzAllocPtr alloc);

/* ---------- Dictionary Interface ---------- */

/* You can use it, if you want to eliminate the overhead for data copying from
   dictionary to some other external buffer.
   You must work with CLzmaDec variables directly in this interface.

   STEPS:
     LzmaDec_Construct()
     LzmaDec_Allocate()
     for (each new stream)
     {
       LzmaDec_Init()
       while (it needs more decompression)
       {
         LzmaDec_DecodeToDic()
         use data from CLzmaDec::dic and update CLzmaDec::dicPos
       }
     }
     LzmaDec_Free()
*/

/* LzmaDec_DecodeToDic

   The decoding to internal dictionary buffer (CLzmaDec::dic).
   You must manually update CLzmaDec::dicPos, if it reaches CLzmaDec::dicBufSize !!!

finishMode:
  It has meaning only if the decoding reaches output limit (dicLimit).
  LZMA_FINISH_ANY - Decode just dicLimit bytes.
  LZMA_FINISH_END - Stream must be finished after dicLimit.

Returns:
  SZ_OK
    status:
      LZMA_STATUS_FINISHED_WITH_MARK
      LZMA_STATUS_NOT_FINISHED
      LZMA_STATUS_NEEDS_MORE_INPUT
      LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK
  SZ_ERROR_DATA - Data error
*/

SRes LzmaDec_DecodeToDic(CLzmaDec *p, size_t dicLimit,
    const Byte *src, size_t *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status);


/* ---------- Buffer Interface ---------- */

/* It's zlib-like interface.
   See LzmaDec_DecodeToDic description for information about STEPS and return results,
   but you must use LzmaDec_DecodeToBuf instead of LzmaDec_DecodeToDic and you don't need
   to work with CLzmaDec variables manually.

finishMode:
  It has meaning only if the decoding reaches output limit (*destLen).
  LZMA_FINISH_ANY - Decode just destLen bytes.
  LZMA_FINISH_END - Stream must be finished after (*destLen).
*/

SRes LzmaDec_DecodeToBuf(CLzmaDec *p, Byte *dest, size_t *destLen,
    const Byte *src, size_t *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status);


/* ---------- One Call Interface ---------- */

/* LzmaDecode

finishMode:
  It has meaning only if the decoding reaches output limit (*destLen).
  LZMA_FINISH_ANY - Decode just destLen bytes.
  LZMA_FINISH_END - Stream must be finished after (*destLen).

Returns:
  SZ_OK
    status:
      LZMA_STATUS_FINISHED_WITH_MARK
      LZMA_STATUS_NOT_FINISHED
      LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK
  SZ_ERROR_DATA - Data error
  SZ_ERROR_MEM  - Memory allocation error
  SZ_ERROR_UNSUPPORTED - Unsupported properties
  SZ_ERROR_INPUT_EOF - It needs more bytes in input buffer (src).
*/

SRes LzmaDecode(Byte *dest, size_t *destLen, const Byte *src, size_t *srcLen,
    const Byte *propData, unsigned propSize, ELzmaFinishMode finishMode,
    ELzmaStatus *status, ISzAllocPtr alloc);





/* LzmaLib.c -- LZMA library wrapper
2015-06-13 : Igor Pavlov : Public domain */

//#include "Alloc.h"
//#include "LzmaDec.h"
//#include "LzmaEnc.h"
//#include "LzmaLib.h"

struct WrappedProgress
{
 ICompressProgress intProgress;
 int (*extProgress)( size_t inSize, size_t outSize, void* userData );
 void* userData;
};


SRes compressProgress(const ICompressProgress *p, UInt64 inSize, UInt64 outSize)
{
    struct WrappedProgress* progress = (struct WrappedProgress*) p;
    if( progress->extProgress( (size_t) inSize, (size_t) outSize, progress->userData ) == 0 )
        return SZ_OK;
    else
        return SZ_ERROR_FAIL;
}


int LzmaCompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen,
  unsigned char *outProps, size_t *outPropsSize,
  int level, /* 0 <= level <= 9, default = 5 */
  unsigned dictSize, /* use (1 << N) or (3 << N). 4 KB < dictSize <= 128 MB */
  int lc, /* 0 <= lc <= 8, default = 3  */
  int lp, /* 0 <= lp <= 4, default = 0  */
  int pb, /* 0 <= pb <= 4, default = 2  */
  int fb,  /* 5 <= fb <= 273, default = 32 */
  int (*progress)( size_t inSize, size_t outSize, void* userData ),
  void* userData
)
{
  CLzmaEncProps props;
  LzmaEncProps_Init(&props);
  props.level = level;
  props.dictSize = dictSize;
  props.lc = lc;
  props.lp = lp;
  props.pb = pb;
  props.fb = fb;

  struct WrappedProgress wrappedProgress;
  wrappedProgress.intProgress.Progress = compressProgress;
  wrappedProgress.extProgress = progress;
  wrappedProgress.userData = userData;
  return LzmaEncode(dest, destLen, src, srcLen, &props, outProps, outPropsSize, 0, progress ? &wrappedProgress.intProgress : NULL, &g_Alloc, &g_Alloc);
}


int LzmaUncompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t *srcLen,
  const unsigned char *props, size_t propsSize)
{
  ELzmaStatus status;
  return LzmaDecode(dest, destLen, src, srcLen, props, (unsigned)propsSize, LZMA_FINISH_ANY, &status, &g_Alloc);
}





/* Alloc.c -- Memory allocation functions
2018-04-27 : Igor Pavlov : Public domain */


#include <stdio.h>
/*
#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#pragma warning( disable: 4668 ) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#pragma warning( disable: 4768 ) // __declspec attributes before linkage specification are ignored
#pragma warning( disable: 4255 ) // 'function' : no function prototype given: converting '()' to '(void)'
#define _NTDDSCM_H_
#include <windows.h>
#pragma warning( pop )
*/
#include <stdlib.h>

//#include "Alloc.h"

/* #define _SZ_ALLOC_DEBUG */

/* use _SZ_ALLOC_DEBUG to debug alloc/free operations */
#ifdef _SZ_ALLOC_DEBUG

#include <stdio.h>
int g_allocCount = 0;
int g_allocCountMid = 0;
int g_allocCountBig = 0;


#define CONVERT_INT_TO_STR(charType, tempSize) \
  unsigned char temp[tempSize]; unsigned i = 0; \
  while (val >= 10) { temp[i++] = (unsigned char)('0' + (unsigned)(val % 10)); val /= 10; } \
  *s++ = (charType)('0' + (unsigned)val); \
  while (i != 0) { i--; *s++ = temp[i]; } \
  *s = 0;

static void ConvertUInt64ToString(UInt64 val, char *s)
{
  CONVERT_INT_TO_STR(char, 24);
}

#define GET_HEX_CHAR(t) ((char)(((t < 10) ? ('0' + t) : ('A' + (t - 10)))))

static void ConvertUInt64ToHex(UInt64 val, char *s)
{
  UInt64 v = val;
  unsigned i;
  for (i = 1;; i++)
  {
    v >>= 4;
    if (v == 0)
      break;
  }
  s[i] = 0;
  do
  {
    unsigned t = (unsigned)(val & 0xF);
    val >>= 4;
    s[--i] = GET_HEX_CHAR(t);
  }
  while (i);
}

#define DEBUG_OUT_STREAM stderr

static void Print(const char *s)
{
  fputs(s, DEBUG_OUT_STREAM);
}

static void PrintAligned(const char *s, size_t align)
{
  size_t len = strlen(s);
  for(;;)
  {
    fputc(' ', DEBUG_OUT_STREAM);
    if (len >= align)
      break;
    ++len;
  }
  Print(s);
}

static void PrintLn()
{
  Print("\n");
}

static void PrintHex(UInt64 v, size_t align)
{
  char s[32];
  ConvertUInt64ToHex(v, s);
  PrintAligned(s, align);
}

static void PrintDec(UInt64 v, size_t align)
{
  char s[32];
  ConvertUInt64ToString(v, s);
  PrintAligned(s, align);
}

static void PrintAddr(void *p)
{
  PrintHex((UInt64)(size_t)(ptrdiff_t)p, 12);
}


#define PRINT_ALLOC(name, cnt, size, ptr) \
    Print(name " "); \
    PrintDec(cnt++, 10); \
    PrintHex(size, 10); \
    PrintAddr(ptr); \
    PrintLn();

#define PRINT_FREE(name, cnt, ptr) if (ptr) { \
    Print(name " "); \
    PrintDec(--cnt, 10); \
    PrintAddr(ptr); \
    PrintLn(); }

#else

#define PRINT_ALLOC(name, cnt, size, ptr)
#define PRINT_FREE(name, cnt, ptr)
#define Print(s)
#define PrintLn()
#define PrintHex(v, align)
#define PrintDec(v, align)
#define PrintAddr(p)

#endif



void *MyAlloc(size_t size)
{
  if (size == 0)
    return NULL;
  #ifdef _SZ_ALLOC_DEBUG
  {
    void *p = malloc(size);
    PRINT_ALLOC("Alloc    ", g_allocCount, size, p);
    return p;
  }
  #else
  return malloc(size);
  #endif
}

void MyFree(void *address)
{
  PRINT_FREE("Free    ", g_allocCount, address);

  free(address);
}

#ifdef _WIN32

void *MidAlloc(size_t size)
{
  if (size == 0)
    return NULL;

  PRINT_ALLOC("Alloc-Mid", g_allocCountMid, size, NULL);

  return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
}

void MidFree(void *address)
{
  PRINT_FREE("Free-Mid", g_allocCountMid, address);

  if (!address)
    return;
  VirtualFree(address, 0, MEM_RELEASE);
}

#ifndef MEM_LARGE_PAGES
#undef _7ZIP_LARGE_PAGES
#endif

#ifdef _7ZIP_LARGE_PAGES
SIZE_T g_LargePageSize = 0;
typedef SIZE_T (WINAPI *GetLargePageMinimumP)();
#endif

void SetLargePageSize( void )
{
  #ifdef _7ZIP_LARGE_PAGES
  SIZE_T size;
  GetLargePageMinimumP largePageMinimum = (GetLargePageMinimumP)
        GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetLargePageMinimum");
  if (!largePageMinimum)
    return;
  size = largePageMinimum();
  if (size == 0 || (size & (size - 1)) != 0)
    return;
  g_LargePageSize = size;
  #endif
}


void *BigAlloc(size_t size)
{
  if (size == 0)
    return NULL;

  PRINT_ALLOC("Alloc-Big", g_allocCountBig, size, NULL);

  #ifdef _7ZIP_LARGE_PAGES
  {
    SIZE_T ps = g_LargePageSize;
    if (ps != 0 && ps <= (1 << 30) && size > (ps / 2))
    {
      size_t size2;
      ps--;
      size2 = (size + ps) & ~ps;
      if (size2 >= size)
      {
        void *res = VirtualAlloc(NULL, size2, MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
        if (res)
          return res;
      }
    }
  }
  #endif

  return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
}

void BigFree(void *address)
{
  PRINT_FREE("Free-Big", g_allocCountBig, address);

  if (!address)
    return;
  VirtualFree(address, 0, MEM_RELEASE);
}

#endif


static void *SzAlloc(ISzAllocPtr p, size_t size) { UNUSED_VAR(p); return MyAlloc(size); }
static void SzFree(ISzAllocPtr p, void *address) { UNUSED_VAR(p); MyFree(address); }
const ISzAlloc g_Alloc = { SzAlloc, SzFree };

static void *SzMidAlloc(ISzAllocPtr p, size_t size) { UNUSED_VAR(p); return MidAlloc(size); }
static void SzMidFree(ISzAllocPtr p, void *address) { UNUSED_VAR(p); MidFree(address); }
const ISzAlloc g_MidAlloc = { SzMidAlloc, SzMidFree };

static void *SzBigAlloc(ISzAllocPtr p, size_t size) { UNUSED_VAR(p); return BigAlloc(size); }
static void SzBigFree(ISzAllocPtr p, void *address) { UNUSED_VAR(p); BigFree(address); }
const ISzAlloc g_BigAlloc = { SzBigAlloc, SzBigFree };


/*
  uintptr_t : <stdint.h> C99 (optional)
            : unsupported in VS6
*/

#ifdef _WIN32
  typedef UINT_PTR UIntPtr;
#else
  /*
  typedef uintptr_t UIntPtr;
  */
  typedef ptrdiff_t UIntPtr;
#endif


#define ADJUST_ALLOC_SIZE 0
/*
#define ADJUST_ALLOC_SIZE (sizeof(void *) - 1)
*/
/*
  Use (ADJUST_ALLOC_SIZE = (sizeof(void *) - 1)), if
     MyAlloc() can return address that is NOT multiple of sizeof(void *).
*/


/*
#define MY_ALIGN_PTR_DOWN(p, align) ((void *)((char *)(p) - ((size_t)(UIntPtr)(p) & ((align) - 1))))
*/
#define MY_ALIGN_PTR_DOWN(p, align) ((void *)((((UIntPtr)(p)) & ~((UIntPtr)(align) - 1))))

#define MY_ALIGN_PTR_UP_PLUS(p, align) MY_ALIGN_PTR_DOWN(((char *)(p) + (align) + ADJUST_ALLOC_SIZE), align)


#if ( defined( _POSIX_C_SOURCE ) && _POSIX_C_SOURCE >= 200112L) && !defined(_WIN32)
  #define USE_posix_memalign
#endif

/*
  This posix_memalign() is for test purposes only.
  We also need special Free() function instead of free(),
  if this posix_memalign() is used.
*/

/*
static int posix_memalign(void **ptr, size_t align, size_t size)
{
  size_t newSize = size + align;
  void *p;
  void *pAligned;
  *ptr = NULL;
  if (newSize < size)
    return 12; // ENOMEM
  p = MyAlloc(newSize);
  if (!p)
    return 12; // ENOMEM
  pAligned = MY_ALIGN_PTR_UP_PLUS(p, align);
  ((void **)pAligned)[-1] = p;
  *ptr = pAligned;
  return 0;
}
*/

/*
  ALLOC_ALIGN_SIZE >= sizeof(void *)
  ALLOC_ALIGN_SIZE >= cache_line_size
*/

#define ALLOC_ALIGN_SIZE ((size_t)1 << 7)

static void *SzAlignedAlloc(ISzAllocPtr pp, size_t size)
{
  #ifndef USE_posix_memalign

  void *p;
  void *pAligned;
  size_t newSize;
  UNUSED_VAR(pp);

  /* also we can allocate additional dummy ALLOC_ALIGN_SIZE bytes after aligned
     block to prevent cache line sharing with another allocated blocks */

  newSize = size + ALLOC_ALIGN_SIZE * 1 + ADJUST_ALLOC_SIZE;
  if (newSize < size)
    return NULL;

  p = MyAlloc(newSize);

  if (!p)
    return NULL;
  pAligned = MY_ALIGN_PTR_UP_PLUS(p, ALLOC_ALIGN_SIZE);

  Print(" size="); PrintHex(size, 8);
  Print(" a_size="); PrintHex(newSize, 8);
  Print(" ptr="); PrintAddr(p);
  Print(" a_ptr="); PrintAddr(pAligned);
  PrintLn();

  ((void **)pAligned)[-1] = p;

  return pAligned;

  #else

  void *p;
  UNUSED_VAR(pp);
  if (posix_memalign(&p, ALLOC_ALIGN_SIZE, size))
    return NULL;

  Print(" posix_memalign="); PrintAddr(p);
  PrintLn();

  return p;

  #endif
}


static void SzAlignedFree(ISzAllocPtr pp, void *address)
{
  UNUSED_VAR(pp);
  #ifndef USE_posix_memalign
  if (address)
    MyFree(((void **)address)[-1]);
  #else
  free(address);
  #endif
}


const ISzAlloc g_AlignedAlloc = { SzAlignedAlloc, SzAlignedFree };



#define MY_ALIGN_PTR_DOWN_1(p) MY_ALIGN_PTR_DOWN(p, sizeof(void *))

/* we align ptr to support cases where CAlignOffsetAlloc::offset is not multiply of sizeof(void *) */
#define REAL_BLOCK_PTR_VAR(p) ((void **)MY_ALIGN_PTR_DOWN_1(p))[-1]
/*
#define REAL_BLOCK_PTR_VAR(p) ((void **)(p))[-1]
*/

static void *AlignOffsetAlloc_Alloc(ISzAllocPtr pp, size_t size)
{
  CAlignOffsetAlloc *p = CONTAINER_FROM_VTBL(pp, CAlignOffsetAlloc, vt);
  void *adr;
  void *pAligned;
  size_t newSize;
  size_t extra;
  size_t alignSize = (size_t)1 << p->numAlignBits;

  if (alignSize < sizeof(void *))
    alignSize = sizeof(void *);

  if (p->offset >= alignSize)
    return NULL;

  /* also we can allocate additional dummy ALLOC_ALIGN_SIZE bytes after aligned
     block to prevent cache line sharing with another allocated blocks */
  extra = p->offset & (sizeof(void *) - 1);
  newSize = size + alignSize + extra + ADJUST_ALLOC_SIZE;
  if (newSize < size)
    return NULL;

  adr = ISzAlloc_Alloc(p->baseAlloc, newSize);

  if (!adr)
    return NULL;

  pAligned = (char *)MY_ALIGN_PTR_DOWN((char *)adr +
      alignSize - p->offset + extra + ADJUST_ALLOC_SIZE, alignSize) + p->offset;

  PrintLn();
  Print("- Aligned: ");
  Print(" size="); PrintHex(size, 8);
  Print(" a_size="); PrintHex(newSize, 8);
  Print(" ptr="); PrintAddr(adr);
  Print(" a_ptr="); PrintAddr(pAligned);
  PrintLn();

  REAL_BLOCK_PTR_VAR(pAligned) = adr;

  return pAligned;
}


static void AlignOffsetAlloc_Free(ISzAllocPtr pp, void *address)
{
  if (address)
  {
    CAlignOffsetAlloc *p = CONTAINER_FROM_VTBL(pp, CAlignOffsetAlloc, vt);
    PrintLn();
    Print("- Aligned Free: ");
    PrintLn();
    ISzAlloc_Free(p->baseAlloc, REAL_BLOCK_PTR_VAR(address));
  }
}


void AlignOffsetAlloc_CreateVTable(CAlignOffsetAlloc *p)
{
  p->vt.Alloc = AlignOffsetAlloc_Alloc;
  p->vt.Free = AlignOffsetAlloc_Free;
}





/* LzFind.h -- Match finder for LZ algorithms
2017-06-10 : Igor Pavlov : Public domain */

typedef UInt32 CLzRef;

typedef struct _CMatchFinder
{
  Byte *buffer;
  UInt32 pos;
  UInt32 posLimit;
  UInt32 streamPos;
  UInt32 lenLimit;

  UInt32 cyclicBufferPos;
  UInt32 cyclicBufferSize; /* it must be = (historySize + 1) */

  Byte streamEndWasReached;
  Byte btMode;
  Byte bigHash;
  Byte directInput;

  UInt32 matchMaxLen;
  CLzRef *hash;
  CLzRef *son;
  UInt32 hashMask;
  UInt32 cutValue;

  Byte *bufferBase;
  ISeqInStream *stream;

  UInt32 blockSize;
  UInt32 keepSizeBefore;
  UInt32 keepSizeAfter;

  UInt32 numHashBytes;
  size_t directInputRem;
  UInt32 historySize;
  UInt32 fixedHashSize;
  UInt32 hashSizeSum;
  SRes result;
  UInt32 crc[256];
  size_t numRefs;

  UInt64 expectedDataSize;
} CMatchFinder;

#define Inline_MatchFinder_GetPointerToCurrentPos(p) ((p)->buffer)

#define Inline_MatchFinder_GetNumAvailableBytes(p) ((p)->streamPos - (p)->pos)

#define Inline_MatchFinder_IsFinishedOK(p) \
    ((p)->streamEndWasReached \
        && (p)->streamPos == (p)->pos \
        && (!(p)->directInput || (p)->directInputRem == 0))

int MatchFinder_NeedMove(CMatchFinder *p);
Byte *MatchFinder_GetPointerToCurrentPos(CMatchFinder *p);
void MatchFinder_MoveBlock(CMatchFinder *p);
void MatchFinder_ReadIfRequired(CMatchFinder *p);

void MatchFinder_Construct(CMatchFinder *p);

/* Conditions:
     historySize <= 3 GB
     keepAddBufferBefore + matchMaxLen + keepAddBufferAfter < 511MB
*/
int MatchFinder_Create(CMatchFinder *p, UInt32 historySize,
    UInt32 keepAddBufferBefore, UInt32 matchMaxLen, UInt32 keepAddBufferAfter,
    ISzAllocPtr alloc);
void MatchFinder_Free(CMatchFinder *p, ISzAllocPtr alloc);
void MatchFinder_Normalize3(UInt32 subValue, CLzRef *items, size_t numItems);
void MatchFinder_ReduceOffsets(CMatchFinder *p, UInt32 subValue);

UInt32 * GetMatchesSpec1(UInt32 lenLimit, UInt32 curMatch, UInt32 pos, const Byte *buffer, CLzRef *son,
    UInt32 _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 _cutValue,
    UInt32 *distances, UInt32 maxLen);

/*
Conditions:
  Mf_GetNumAvailableBytes_Func must be called before each Mf_GetMatchLen_Func.
  Mf_GetPointerToCurrentPos_Func's result must be used only before any other function
*/

typedef void (*Mf_Init_Func)(void *object);
typedef UInt32 (*Mf_GetNumAvailableBytes_Func)(void *object);
typedef const Byte * (*Mf_GetPointerToCurrentPos_Func)(void *object);
typedef UInt32 (*Mf_GetMatches_Func)(void *object, UInt32 *distances);
typedef void (*Mf_Skip_Func)(void *object, UInt32);

typedef struct _IMatchFinder
{
  Mf_Init_Func Init;
  Mf_GetNumAvailableBytes_Func GetNumAvailableBytes;
  Mf_GetPointerToCurrentPos_Func GetPointerToCurrentPos;
  Mf_GetMatches_Func GetMatches;
  Mf_Skip_Func Skip;
} IMatchFinder;

void MatchFinder_CreateVTable(CMatchFinder *p, IMatchFinder *vTable);

void MatchFinder_Init_LowHash(CMatchFinder *p);
void MatchFinder_Init_HighHash(CMatchFinder *p);
void MatchFinder_Init_3(CMatchFinder *p, int readData);
void MatchFinder_Init(CMatchFinder *p);

UInt32 Bt3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances);
UInt32 Hc3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances);

void Bt3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num);
void Hc3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num);





/* LzmaEnc.c -- LZMA Encoder
2019-01-10: Igor Pavlov : Public domain */

#include <string.h>

/* #define SHOW_STAT */
/* #define SHOW_STAT2 */

#if defined(SHOW_STAT) || defined(SHOW_STAT2)
#include <stdio.h>
#endif


#ifdef SHOW_STAT
static unsigned g_STAT_OFFSET = 0;
#endif

#define kLzmaMaxHistorySize ((UInt32)3 << 29)
/* #define kLzmaMaxHistorySize ((UInt32)7 << 29) */

#define kNumTopBits 24
#define kTopValue ((UInt32)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5
#define kProbInitValue (kBitModelTotal >> 1)

#define kNumMoveReducingBits 4
#define kNumBitPriceShiftBits 4
#define kBitPrice (1 << kNumBitPriceShiftBits)

#define REP_LEN_COUNT 64

void LzmaEncProps_Init(CLzmaEncProps *p)
{
  p->level = 5;
  p->dictSize = p->mc = 0;
  p->reduceSize = (UInt64)(Int64)-1;
  p->lc = p->lp = p->pb = p->algo = p->fb = p->btMode = p->numHashBytes = -1;
  p->writeEndMark = 0;
}

void LzmaEncProps_Normalize(CLzmaEncProps *p)
{
  int level = p->level;
  if (level < 0) level = 5;
  p->level = level;

  if (p->dictSize == 0) p->dictSize = (level <= 5 ? (1 << (level * 2 + 14)) : (level <= 7 ? (1 << 25) : (1 << 26)));
  if (p->dictSize > p->reduceSize)
  {
    unsigned i;
    UInt32 reduceSize = (UInt32)p->reduceSize;
    for (i = 11; i <= 30; i++)
    {
      if (reduceSize <= ((UInt32)2 << i)) { p->dictSize = ((UInt32)2 << i); break; }
      if (reduceSize <= ((UInt32)3 << i)) { p->dictSize = ((UInt32)3 << i); break; }
    }
  }

  if (p->lc < 0) p->lc = 3;
  if (p->lp < 0) p->lp = 0;
  if (p->pb < 0) p->pb = 2;

  if (p->algo < 0) p->algo = (level < 5 ? 0 : 1);
  if (p->fb < 0) p->fb = (level < 7 ? 32 : 64);
  if (p->btMode < 0) p->btMode = (p->algo == 0 ? 0 : 1);
  if (p->numHashBytes < 0) p->numHashBytes = 4;
  if (p->mc == 0) p->mc = (16 + (p->fb >> 1)) >> (p->btMode ? 0 : 1);
}

UInt32 LzmaEncProps_GetDictSize(const CLzmaEncProps *props2)
{
  CLzmaEncProps props = *props2;
  LzmaEncProps_Normalize(&props);
  return props.dictSize;
}

#if (_MSC_VER >= 1400)
/* BSR code is fast for some new CPUs */
/* #define LZMA_LOG_BSR */
#endif

#ifdef LZMA_LOG_BSR

#define kDicLogSizeMaxCompress 32

#define BSR2_RET(pos, res) { unsigned long zz; _BitScanReverse(&zz, (pos)); res = (zz + zz) + ((pos >> (zz - 1)) & 1); }

static unsigned GetPosSlot1(UInt32 pos)
{
  unsigned res;
  BSR2_RET(pos, res);
  return res;
}
#define GetPosSlot2(pos, res) { BSR2_RET(pos, res); }
#define GetPosSlot(pos, res) { if (pos < 2) res = pos; else BSR2_RET(pos, res); }

#else

#define kNumLogBits (9 + sizeof(size_t) / 2)
/* #define kNumLogBits (11 + sizeof(size_t) / 8 * 3) */

#define kDicLogSizeMaxCompress ((kNumLogBits - 1) * 2 + 7)

static void LzmaEnc_FastPosInit(Byte *g_FastPos)
{
  unsigned slot;
  g_FastPos[0] = 0;
  g_FastPos[1] = 1;
  g_FastPos += 2;

  for (slot = 2; slot < kNumLogBits * 2; slot++)
  {
    size_t k = ((size_t)1 << ((slot >> 1) - 1));
    size_t j;
    for (j = 0; j < k; j++)
      g_FastPos[j] = (Byte)slot;
    g_FastPos += k;
  }
}

/* we can use ((limit - pos) >> 31) only if (pos < ((UInt32)1 << 31)) */
/*
#define BSR2_RET(pos, res) { unsigned zz = 6 + ((kNumLogBits - 1) & \
  (0 - (((((UInt32)1 << (kNumLogBits + 6)) - 1) - pos) >> 31))); \
  res = p->g_FastPos[pos >> zz] + (zz * 2); }
*/

/*
#define BSR2_RET(pos, res) { unsigned zz = 6 + ((kNumLogBits - 1) & \
  (0 - (((((UInt32)1 << (kNumLogBits)) - 1) - (pos >> 6)) >> 31))); \
  res = p->g_FastPos[pos >> zz] + (zz * 2); }
*/

#define BSR2_RET(pos, res) { unsigned zz = (pos < (1 << (kNumLogBits + 6))) ? 6 : 6 + kNumLogBits - 1; \
  res = p->g_FastPos[pos >> zz] + (zz * 2); }

/*
#define BSR2_RET(pos, res) { res = (pos < (1 << (kNumLogBits + 6))) ? \
  p->g_FastPos[pos >> 6] + 12 : \
  p->g_FastPos[pos >> (6 + kNumLogBits - 1)] + (6 + (kNumLogBits - 1)) * 2; }
*/

#define GetPosSlot1(pos) p->g_FastPos[pos]
#define GetPosSlot2(pos, res) { BSR2_RET(pos, res); }
#define GetPosSlot(pos, res) { if (pos < kNumFullDistances) res = p->g_FastPos[pos & (kNumFullDistances - 1)]; else BSR2_RET(pos, res); }

#endif


#define LZMA_NUM_REPS 4

typedef UInt16 CState;
typedef UInt16 CExtra;

typedef struct
{
  UInt32 price;
  CState state;
  CExtra extra;
      // 0   : normal
      // 1   : LIT : MATCH
      // > 1 : MATCH (extra-1) : LIT : REP0 (len)
  UInt32 len;
  UInt32 dist;
  UInt32 reps[LZMA_NUM_REPS];
} COptimal;


// 18.06
#define kNumOpts (1 << 11)
#define kPackReserve (kNumOpts * 8)
// #define kNumOpts (1 << 12)
// #define kPackReserve (1 + kNumOpts * 2)

#define kNumLenToPosStates 4
#define kNumPosSlotBits 6
#define kDicLogSizeMin 0
#define kDicLogSizeMax 32
#define kDistTableSizeMax (kDicLogSizeMax * 2)

#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)
#define kAlignMask (kAlignTableSize - 1)

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

/*
typedef
#ifdef _LZMA_PROB32
  UInt32
#else
  UInt16
#endif
  CLzmaProb;
*/

#define LZMA_PB_MAX 4
#define LZMA_LC_MAX 8
#define LZMA_LP_MAX 4

#define LZMA_NUM_PB_STATES_MAX (1 << LZMA_PB_MAX)

#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)
#define kLenNumSymbolsTotal (kLenNumLowSymbols * 2 + kLenNumHighSymbols)

#define LZMA_MATCH_LEN_MIN 2
#define LZMA_MATCH_LEN_MAX (LZMA_MATCH_LEN_MIN + kLenNumSymbolsTotal - 1)

#define kNumStates 12


typedef struct
{
  CLzmaProb low[LZMA_NUM_PB_STATES_MAX << (kLenNumLowBits + 1)];
  CLzmaProb high[kLenNumHighSymbols];
} CLenEnc;


typedef struct
{
  unsigned tableSize;
  UInt32 prices[LZMA_NUM_PB_STATES_MAX][kLenNumSymbolsTotal];
  // UInt32 prices1[LZMA_NUM_PB_STATES_MAX][kLenNumLowSymbols * 2];
  // UInt32 prices2[kLenNumSymbolsTotal];
} CLenPriceEnc;

#define GET_PRICE_LEN(p, posState, len) \
    ((p)->prices[posState][(size_t)(len) - LZMA_MATCH_LEN_MIN])

/*
#define GET_PRICE_LEN(p, posState, len) \
    ((p)->prices2[(size_t)(len) - 2] + ((p)->prices1[posState][((len) - 2) & (kLenNumLowSymbols * 2 - 1)] & (((len) - 2 - kLenNumLowSymbols * 2) >> 9)))
*/

typedef struct
{
  UInt32 range;
  unsigned cache;
  UInt64 low;
  UInt64 cacheSize;
  Byte *buf;
  Byte *bufLim;
  Byte *bufBase;
  ISeqOutStream *outStream;
  UInt64 processed;
  SRes res;
} CRangeEnc;


typedef struct
{
  CLzmaProb *litProbs;

  unsigned state;
  UInt32 reps[LZMA_NUM_REPS];

  CLzmaProb posAlignEncoder[1 << kNumAlignBits];
  CLzmaProb isRep[kNumStates];
  CLzmaProb isRepG0[kNumStates];
  CLzmaProb isRepG1[kNumStates];
  CLzmaProb isRepG2[kNumStates];
  CLzmaProb isMatch[kNumStates][LZMA_NUM_PB_STATES_MAX];
  CLzmaProb isRep0Long[kNumStates][LZMA_NUM_PB_STATES_MAX];

  CLzmaProb posSlotEncoder[kNumLenToPosStates][1 << kNumPosSlotBits];
  CLzmaProb posEncoders[kNumFullDistances];

  CLenEnc lenProbs;
  CLenEnc repLenProbs;

} CSaveState;


typedef UInt32 CProbPrice;


typedef struct
{
  void *matchFinderObj;
  IMatchFinder matchFinder;

  unsigned optCur;
  unsigned optEnd;

  unsigned longestMatchLen;
  unsigned numPairs;
  UInt32 numAvail;

  unsigned state;
  unsigned numFastBytes;
  unsigned additionalOffset;
  UInt32 reps[LZMA_NUM_REPS];
  unsigned lpMask, pbMask;
  CLzmaProb *litProbs;
  CRangeEnc rc;

  UInt32 backRes;

  unsigned lc, lp, pb;
  unsigned lclp;

  BoolInt fastMode;
  BoolInt writeEndMark;
  BoolInt finished;
  BoolInt needInit;
  // BoolInt _maxMode;

  UInt64 nowPos64;

  unsigned matchPriceCount;
  // unsigned alignPriceCount;
  int repLenEncCounter;

  unsigned distTableSize;

  UInt32 dictSize;
  SRes result;

  CMatchFinder matchFinderBase;

  CProbPrice ProbPrices[kBitModelTotal >> kNumMoveReducingBits];

  UInt32 matches[LZMA_MATCH_LEN_MAX * 2 + 2 + 1];

  UInt32 alignPrices[kAlignTableSize];
  UInt32 posSlotPrices[kNumLenToPosStates][kDistTableSizeMax];
  UInt32 distancesPrices[kNumLenToPosStates][kNumFullDistances];

  CLzmaProb posAlignEncoder[1 << kNumAlignBits];
  CLzmaProb isRep[kNumStates];
  CLzmaProb isRepG0[kNumStates];
  CLzmaProb isRepG1[kNumStates];
  CLzmaProb isRepG2[kNumStates];
  CLzmaProb isMatch[kNumStates][LZMA_NUM_PB_STATES_MAX];
  CLzmaProb isRep0Long[kNumStates][LZMA_NUM_PB_STATES_MAX];
  CLzmaProb posSlotEncoder[kNumLenToPosStates][1 << kNumPosSlotBits];
  CLzmaProb posEncoders[kNumFullDistances];

  CLenEnc lenProbs;
  CLenEnc repLenProbs;

  #ifndef LZMA_LOG_BSR
  Byte g_FastPos[1 << kNumLogBits];
  #endif

  CLenPriceEnc lenEnc;
  CLenPriceEnc repLenEnc;

  COptimal opt[kNumOpts];

  CSaveState saveState;

} CLzmaEnc;



#define COPY_ARR(dest, src, arr) memcpy(dest->arr, src->arr, sizeof(src->arr));

void LzmaEnc_SaveState(CLzmaEncHandle pp)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  CSaveState *dest = &p->saveState;

  dest->state = p->state;

  dest->lenProbs = p->lenProbs;
  dest->repLenProbs = p->repLenProbs;

  COPY_ARR(dest, p, reps);

  COPY_ARR(dest, p, posAlignEncoder);
  COPY_ARR(dest, p, isRep);
  COPY_ARR(dest, p, isRepG0);
  COPY_ARR(dest, p, isRepG1);
  COPY_ARR(dest, p, isRepG2);
  COPY_ARR(dest, p, isMatch);
  COPY_ARR(dest, p, isRep0Long);
  COPY_ARR(dest, p, posSlotEncoder);
  COPY_ARR(dest, p, posEncoders);

  memcpy(dest->litProbs, p->litProbs, ((UInt32)0x300 << p->lclp) * sizeof(CLzmaProb));
}


void LzmaEnc_RestoreState(CLzmaEncHandle pp)
{
  CLzmaEnc *dest = (CLzmaEnc *)pp;
  const CSaveState *p = &dest->saveState;

  dest->state = p->state;

  dest->lenProbs = p->lenProbs;
  dest->repLenProbs = p->repLenProbs;

  COPY_ARR(dest, p, reps);

  COPY_ARR(dest, p, posAlignEncoder);
  COPY_ARR(dest, p, isRep);
  COPY_ARR(dest, p, isRepG0);
  COPY_ARR(dest, p, isRepG1);
  COPY_ARR(dest, p, isRepG2);
  COPY_ARR(dest, p, isMatch);
  COPY_ARR(dest, p, isRep0Long);
  COPY_ARR(dest, p, posSlotEncoder);
  COPY_ARR(dest, p, posEncoders);

  memcpy(dest->litProbs, p->litProbs, ((UInt32)0x300 << dest->lclp) * sizeof(CLzmaProb));
}



SRes LzmaEnc_SetProps(CLzmaEncHandle pp, const CLzmaEncProps *props2)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  CLzmaEncProps props = *props2;
  LzmaEncProps_Normalize(&props);

  if (props.lc > LZMA_LC_MAX
      || props.lp > LZMA_LP_MAX
      || props.pb > LZMA_PB_MAX
      || props.dictSize > ((UInt64)1 << kDicLogSizeMaxCompress)
      || props.dictSize > kLzmaMaxHistorySize)
    return SZ_ERROR_PARAM;

  p->dictSize = props.dictSize;
  {
    unsigned fb = props.fb;
    if (fb < 5)
      fb = 5;
    if (fb > LZMA_MATCH_LEN_MAX)
      fb = LZMA_MATCH_LEN_MAX;
    p->numFastBytes = fb;
  }
  p->lc = props.lc;
  p->lp = props.lp;
  p->pb = props.pb;
  p->fastMode = (props.algo == 0);
  // p->_maxMode = True;
  p->matchFinderBase.btMode = (Byte)(props.btMode ? 1 : 0);
  {
    unsigned numHashBytes = 4;
    if (props.btMode)
    {
      if (props.numHashBytes < 2)
        numHashBytes = 2;
      else if (props.numHashBytes < 4)
        numHashBytes = props.numHashBytes;
    }
    p->matchFinderBase.numHashBytes = numHashBytes;
  }

  p->matchFinderBase.cutValue = props.mc;

  p->writeEndMark = props.writeEndMark;

  return SZ_OK;
}


void LzmaEnc_SetDataSize(CLzmaEncHandle pp, UInt64 expectedDataSiize)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  p->matchFinderBase.expectedDataSize = expectedDataSiize;
}


#define kState_Start 0
#define kState_LitAfterMatch 4
#define kState_LitAfterRep   5
#define kState_MatchAfterLit 7
#define kState_RepAfterLit   8

static const Byte kLiteralNextStates[kNumStates] = {0, 0, 0, 0, 1, 2, 3, 4,  5,  6,   4, 5};
static const Byte kMatchNextStates[kNumStates]   = {7, 7, 7, 7, 7, 7, 7, 10, 10, 10, 10, 10};
static const Byte kRepNextStates[kNumStates]     = {8, 8, 8, 8, 8, 8, 8, 11, 11, 11, 11, 11};
static const Byte kShortRepNextStates[kNumStates]= {9, 9, 9, 9, 9, 9, 9, 11, 11, 11, 11, 11};

#define IsLitState(s) ((s) < 7)
#define GetLenToPosState2(len) (((len) < kNumLenToPosStates - 1) ? (len) : kNumLenToPosStates - 1)
#define GetLenToPosState(len) (((len) < kNumLenToPosStates + 1) ? (len) - 2 : kNumLenToPosStates - 1)

#define kInfinityPrice (1 << 30)

static void RangeEnc_Construct(CRangeEnc *p)
{
  p->outStream = NULL;
  p->bufBase = NULL;
}

#define RangeEnc_GetProcessed(p)       ((p)->processed + ((p)->buf - (p)->bufBase) + (p)->cacheSize)
#define RangeEnc_GetProcessed_size_t(p) ((size_t)(p)->processed + ((p)->buf - (p)->bufBase) + (size_t)(p)->cacheSize)

#define RC_BUF_SIZE (1 << 16)

static int RangeEnc_Alloc(CRangeEnc *p, ISzAllocPtr alloc)
{
  if (!p->bufBase)
  {
    p->bufBase = (Byte *)ISzAlloc_Alloc(alloc, RC_BUF_SIZE);
    if (!p->bufBase)
      return 0;
    p->bufLim = p->bufBase + RC_BUF_SIZE;
  }
  return 1;
}

static void RangeEnc_Free(CRangeEnc *p, ISzAllocPtr alloc)
{
  ISzAlloc_Free(alloc, p->bufBase);
  p->bufBase = 0;
}

static void RangeEnc_Init(CRangeEnc *p)
{
  /* Stream.Init(); */
  p->range = 0xFFFFFFFF;
  p->cache = 0;
  p->low = 0;
  p->cacheSize = 0;

  p->buf = p->bufBase;

  p->processed = 0;
  p->res = SZ_OK;
}

MY_NO_INLINE static void RangeEnc_FlushStream(CRangeEnc *p)
{
  size_t num;
  if (p->res != SZ_OK)
    return;
  num = p->buf - p->bufBase;
  if (num != ISeqOutStream_Write(p->outStream, p->bufBase, num))
    p->res = SZ_ERROR_WRITE;
  p->processed += num;
  p->buf = p->bufBase;
}

MY_NO_INLINE static void MY_FAST_CALL RangeEnc_ShiftLow(CRangeEnc *p)
{
  UInt32 low = (UInt32)p->low;
  unsigned high = (unsigned)(p->low >> 32);
  p->low = (UInt32)(low << 8);
  if (low < (UInt32)0xFF000000 || high != 0)
  {
    {
      Byte *buf = p->buf;
      *buf++ = (Byte)(p->cache + high);
      p->cache = (unsigned)(low >> 24);
      p->buf = buf;
      if (buf == p->bufLim)
        RangeEnc_FlushStream(p);
      if (p->cacheSize == 0)
        return;
    }
    high += 0xFF;
    for (;;)
    {
      Byte *buf = p->buf;
      *buf++ = (Byte)(high);
      p->buf = buf;
      if (buf == p->bufLim)
        RangeEnc_FlushStream(p);
      if (--p->cacheSize == 0)
        return;
    }
  }
  p->cacheSize++;
}

static void RangeEnc_FlushData(CRangeEnc *p)
{
  int i;
  for (i = 0; i < 5; i++)
    RangeEnc_ShiftLow(p);
}

#define RC_NORM(p) if (range < kTopValue) { range <<= 8; RangeEnc_ShiftLow(p); }

#define RC_BIT_PRE(p, prob) \
  ttt = *(prob); \
  newBound = (range >> kNumBitModelTotalBits) * ttt;

// #define _LZMA_ENC_USE_BRANCH

#ifdef _LZMA_ENC_USE_BRANCH

#define RC_BIT(p, prob, bit) { \
  RC_BIT_PRE(p, prob) \
  if (bit == 0) { range = newBound; ttt += (kBitModelTotal - ttt) >> kNumMoveBits; } \
  else { (p)->low += newBound; range -= newBound; ttt -= ttt >> kNumMoveBits; } \
  *(prob) = (CLzmaProb)ttt; \
  RC_NORM(p) \
  }

#else

#define RC_BIT(p, prob, bit) { \
  UInt32 mask; \
  RC_BIT_PRE(p, prob) \
  mask = 0 - (UInt32)bit; \
  range &= mask; \
  mask &= newBound; \
  range -= mask; \
  (p)->low += mask; \
  mask = (UInt32)bit - 1; \
  range += newBound & mask; \
  mask &= (kBitModelTotal - ((1 << kNumMoveBits) - 1)); \
  mask += ((1 << kNumMoveBits) - 1); \
  ttt += (Int32)(mask - ttt) >> kNumMoveBits; \
  *(prob) = (CLzmaProb)ttt; \
  RC_NORM(p) \
  }

#endif




#define RC_BIT_0_BASE(p, prob) \
  range = newBound; *(prob) = (CLzmaProb)(ttt + ((kBitModelTotal - ttt) >> kNumMoveBits));

#define RC_BIT_1_BASE(p, prob) \
  range -= newBound; (p)->low += newBound; *(prob) = (CLzmaProb)(ttt - (ttt >> kNumMoveBits)); \

#define RC_BIT_0(p, prob) \
  RC_BIT_0_BASE(p, prob) \
  RC_NORM(p)

#define RC_BIT_1(p, prob) \
  RC_BIT_1_BASE(p, prob) \
  RC_NORM(p)

static void RangeEnc_EncodeBit_0(CRangeEnc *p, CLzmaProb *prob)
{
  UInt32 range, ttt, newBound;
  range = p->range;
  RC_BIT_PRE(p, prob)
  RC_BIT_0(p, prob)
  p->range = range;
}

static void LitEnc_Encode(CRangeEnc *p, CLzmaProb *probs, UInt32 sym)
{
  UInt32 range = p->range;
  sym |= 0x100;
  do
  {
    UInt32 ttt, newBound;
    // RangeEnc_EncodeBit(p, probs + (sym >> 8), (sym >> 7) & 1);
    CLzmaProb *prob = probs + (sym >> 8);
    UInt32 bit = (sym >> 7) & 1;
    sym <<= 1;
    RC_BIT(p, prob, bit);
  }
  while (sym < 0x10000);
  p->range = range;
}

static void LitEnc_EncodeMatched(CRangeEnc *p, CLzmaProb *probs, UInt32 sym, UInt32 matchByte)
{
  UInt32 range = p->range;
  UInt32 offs = 0x100;
  sym |= 0x100;
  do
  {
    UInt32 ttt, newBound;
    CLzmaProb *prob;
    UInt32 bit;
    matchByte <<= 1;
    // RangeEnc_EncodeBit(p, probs + (offs + (matchByte & offs) + (sym >> 8)), (sym >> 7) & 1);
    prob = probs + (offs + (matchByte & offs) + (sym >> 8));
    bit = (sym >> 7) & 1;
    sym <<= 1;
    offs &= ~(matchByte ^ sym);
    RC_BIT(p, prob, bit);
  }
  while (sym < 0x10000);
  p->range = range;
}



static void LzmaEnc_InitPriceTables(CProbPrice *ProbPrices)
{
  UInt32 i;
  for (i = 0; i < (kBitModelTotal >> kNumMoveReducingBits); i++)
  {
    const unsigned kCyclesBits = kNumBitPriceShiftBits;
    UInt32 w = (i << kNumMoveReducingBits) + (1 << (kNumMoveReducingBits - 1));
    unsigned bitCount = 0;
    unsigned j;
    for (j = 0; j < kCyclesBits; j++)
    {
      w = w * w;
      bitCount <<= 1;
      while (w >= ((UInt32)1 << 16))
      {
        w >>= 1;
        bitCount++;
      }
    }
    ProbPrices[i] = (CProbPrice)((kNumBitModelTotalBits << kCyclesBits) - 15 - bitCount);
    // printf("\n%3d: %5d", i, ProbPrices[i]);
  }
}


#define GET_PRICE(prob, bit) \
  p->ProbPrices[((prob) ^ (unsigned)(((-(int)(bit))) & (kBitModelTotal - 1))) >> kNumMoveReducingBits];

#define GET_PRICEa(prob, bit) \
     ProbPrices[((prob) ^ (unsigned)((-((int)(bit))) & (kBitModelTotal - 1))) >> kNumMoveReducingBits];

#define GET_PRICE_0(prob) p->ProbPrices[(prob) >> kNumMoveReducingBits]
#define GET_PRICE_1(prob) p->ProbPrices[((prob) ^ (kBitModelTotal - 1)) >> kNumMoveReducingBits]

#define GET_PRICEa_0(prob) ProbPrices[(prob) >> kNumMoveReducingBits]
#define GET_PRICEa_1(prob) ProbPrices[((prob) ^ (kBitModelTotal - 1)) >> kNumMoveReducingBits]


static UInt32 LitEnc_GetPrice(const CLzmaProb *probs, UInt32 sym, const CProbPrice *ProbPrices)
{
  UInt32 price = 0;
  sym |= 0x100;
  do
  {
    unsigned bit = sym & 1;
    sym >>= 1;
    price += GET_PRICEa(probs[sym], bit);
  }
  while (sym >= 2);
  return price;
}


static UInt32 LitEnc_Matched_GetPrice(const CLzmaProb *probs, UInt32 sym, UInt32 matchByte, const CProbPrice *ProbPrices)
{
  UInt32 price = 0;
  UInt32 offs = 0x100;
  sym |= 0x100;
  do
  {
    matchByte <<= 1;
    price += GET_PRICEa(probs[offs + (matchByte & offs) + (sym >> 8)], (sym >> 7) & 1);
    sym <<= 1;
    offs &= ~(matchByte ^ sym);
  }
  while (sym < 0x10000);
  return price;
}


static void RcTree_ReverseEncode(CRangeEnc *rc, CLzmaProb *probs, unsigned numBits, unsigned sym)
{
  UInt32 range = rc->range;
  unsigned m = 1;
  do
  {
    UInt32 ttt, newBound;
    unsigned bit = sym & 1;
    // RangeEnc_EncodeBit(rc, probs + m, bit);
    sym >>= 1;
    RC_BIT(rc, probs + m, bit);
    m = (m << 1) | bit;
  }
  while (--numBits);
  rc->range = range;
}



static void LenEnc_Init(CLenEnc *p)
{
  unsigned i;
  for (i = 0; i < (LZMA_NUM_PB_STATES_MAX << (kLenNumLowBits + 1)); i++)
    p->low[i] = kProbInitValue;
  for (i = 0; i < kLenNumHighSymbols; i++)
    p->high[i] = kProbInitValue;
}

static void LenEnc_Encode(CLenEnc *p, CRangeEnc *rc, unsigned sym, unsigned posState)
{
  UInt32 range, ttt, newBound;
  CLzmaProb *probs = p->low;
  range = rc->range;
  RC_BIT_PRE(rc, probs);
  if (sym >= kLenNumLowSymbols)
  {
    RC_BIT_1(rc, probs);
    probs += kLenNumLowSymbols;
    RC_BIT_PRE(rc, probs);
    if (sym >= kLenNumLowSymbols * 2)
    {
      RC_BIT_1(rc, probs);
      rc->range = range;
      // RcTree_Encode(rc, p->high, kLenNumHighBits, sym - kLenNumLowSymbols * 2);
      LitEnc_Encode(rc, p->high, sym - kLenNumLowSymbols * 2);
      return;
    }
    sym -= kLenNumLowSymbols;
  }

  // RcTree_Encode(rc, probs + (posState << kLenNumLowBits), kLenNumLowBits, sym);
  {
    unsigned m;
    unsigned bit;
    RC_BIT_0(rc, probs);
    probs += (posState << (1 + kLenNumLowBits));
    bit = (sym >> 2)    ; RC_BIT(rc, probs + 1, bit); m = (1 << 1) + bit;
    bit = (sym >> 1) & 1; RC_BIT(rc, probs + m, bit); m = (m << 1) + bit;
    bit =  sym       & 1; RC_BIT(rc, probs + m, bit);
    rc->range = range;
  }
}

static void SetPrices_3(const CLzmaProb *probs, UInt32 startPrice, UInt32 *prices, const CProbPrice *ProbPrices)
{
  unsigned i;
  for (i = 0; i < 8; i += 2)
  {
    UInt32 price = startPrice;
    UInt32 prob;
    price += GET_PRICEa(probs[1           ], (i >> 2));
    price += GET_PRICEa(probs[2 + (i >> 2)], (i >> 1) & 1);
    prob = probs[4 + (i >> 1)];
    prices[i    ] = price + GET_PRICEa_0(prob);
    prices[i + 1] = price + GET_PRICEa_1(prob);
  }
}


MY_NO_INLINE static void MY_FAST_CALL LenPriceEnc_UpdateTables(
    CLenPriceEnc *p,
    unsigned numPosStates,
    const CLenEnc *enc,
    const CProbPrice *ProbPrices)
{
  UInt32 b;

  {
    unsigned prob = enc->low[0];
    UInt32 a, c;
    unsigned posState;
    b = GET_PRICEa_1(prob);
    a = GET_PRICEa_0(prob);
    c = b + GET_PRICEa_0(enc->low[kLenNumLowSymbols]);
    for (posState = 0; posState < numPosStates; posState++)
    {
      UInt32 *prices = p->prices[posState];
      const CLzmaProb *probs = enc->low + (posState << (1 + kLenNumLowBits));
      SetPrices_3(probs, a, prices, ProbPrices);
      SetPrices_3(probs + kLenNumLowSymbols, c, prices + kLenNumLowSymbols, ProbPrices);
    }
  }

  /*
  {
    unsigned i;
    UInt32 b;
    a = GET_PRICEa_0(enc->low[0]);
    for (i = 0; i < kLenNumLowSymbols; i++)
      p->prices2[i] = a;
    a = GET_PRICEa_1(enc->low[0]);
    b = a + GET_PRICEa_0(enc->low[kLenNumLowSymbols]);
    for (i = kLenNumLowSymbols; i < kLenNumLowSymbols * 2; i++)
      p->prices2[i] = b;
    a += GET_PRICEa_1(enc->low[kLenNumLowSymbols]);
  }
  */

  // p->counter = numSymbols;
  // p->counter = 64;

  {
    unsigned i = p->tableSize;

    if (i > kLenNumLowSymbols * 2)
    {
      const CLzmaProb *probs = enc->high;
      UInt32 *prices = p->prices[0] + kLenNumLowSymbols * 2;
      i -= kLenNumLowSymbols * 2 - 1;
      i >>= 1;
      b += GET_PRICEa_1(enc->low[kLenNumLowSymbols]);
      do
      {
        /*
        p->prices2[i] = a +
        // RcTree_GetPrice(enc->high, kLenNumHighBits, i - kLenNumLowSymbols * 2, ProbPrices);
        LitEnc_GetPrice(probs, i - kLenNumLowSymbols * 2, ProbPrices);
        */
        // UInt32 price = a + RcTree_GetPrice(probs, kLenNumHighBits - 1, sym, ProbPrices);
        unsigned sym = --i + (1 << (kLenNumHighBits - 1));
        UInt32 price = b;
        do
        {
          unsigned bit = sym & 1;
          sym >>= 1;
          price += GET_PRICEa(probs[sym], bit);
        }
        while (sym >= 2);

        {
          unsigned prob = probs[(size_t)i + (1 << (kLenNumHighBits - 1))];
          prices[(size_t)i * 2    ] = price + GET_PRICEa_0(prob);
          prices[(size_t)i * 2 + 1] = price + GET_PRICEa_1(prob);
        }
      }
      while (i);

      {
        unsigned posState;
        size_t num = (p->tableSize - kLenNumLowSymbols * 2) * sizeof(p->prices[0][0]);
        for (posState = 1; posState < numPosStates; posState++)
          memcpy(p->prices[posState] + kLenNumLowSymbols * 2, p->prices[0] + kLenNumLowSymbols * 2, num);
      }
    }
  }
}

/*
  #ifdef SHOW_STAT
  g_STAT_OFFSET += num;
  printf("\n MovePos %u", num);
  #endif
*/

#define MOVE_POS(p, num) { \
    p->additionalOffset += (num); \
    p->matchFinder.Skip(p->matchFinderObj, (UInt32)(num)); }


static unsigned ReadMatchDistances(CLzmaEnc *p, unsigned *numPairsRes)
{
  unsigned numPairs;

  p->additionalOffset++;
  p->numAvail = p->matchFinder.GetNumAvailableBytes(p->matchFinderObj);
  numPairs = p->matchFinder.GetMatches(p->matchFinderObj, p->matches);
  *numPairsRes = numPairs;

  #ifdef SHOW_STAT
  printf("\n i = %u numPairs = %u    ", g_STAT_OFFSET, numPairs / 2);
  g_STAT_OFFSET++;
  {
    unsigned i;
    for (i = 0; i < numPairs; i += 2)
      printf("%2u %6u   | ", p->matches[i], p->matches[i + 1]);
  }
  #endif

  if (numPairs == 0)
    return 0;
  {
    unsigned len = p->matches[(size_t)numPairs - 2];
    if (len != p->numFastBytes)
      return len;
    {
      UInt32 numAvail = p->numAvail;
      if (numAvail > LZMA_MATCH_LEN_MAX)
        numAvail = LZMA_MATCH_LEN_MAX;
      {
        const Byte *p1 = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;
        const Byte *p2 = p1 + len;
        ptrdiff_t dif = (ptrdiff_t)-1 - p->matches[(size_t)numPairs - 1];
        const Byte *lim = p1 + numAvail;
        for (; p2 != lim && *p2 == p2[dif]; p2++)
        {}
        return (unsigned)(p2 - p1);
      }
    }
  }
}

#define MARK_LIT ((UInt32)(Int32)-1)

#define MakeAs_Lit(p)       { (p)->dist = MARK_LIT; (p)->extra = 0; }
#define MakeAs_ShortRep(p)  { (p)->dist = 0; (p)->extra = 0; }
#define IsShortRep(p)       ((p)->dist == 0)


#define GetPrice_ShortRep(p, state, posState) \
  ( GET_PRICE_0(p->isRepG0[state]) + GET_PRICE_0(p->isRep0Long[state][posState]))

#define GetPrice_Rep_0(p, state, posState) ( \
    GET_PRICE_1(p->isMatch[state][posState]) \
  + GET_PRICE_1(p->isRep0Long[state][posState])) \
  + GET_PRICE_1(p->isRep[state]) \
  + GET_PRICE_0(p->isRepG0[state])

MY_FORCE_INLINE
static UInt32 GetPrice_PureRep(const CLzmaEnc *p, unsigned repIndex, size_t state, size_t posState)
{
  UInt32 price;
  UInt32 prob = p->isRepG0[state];
  if (repIndex == 0)
  {
    price = GET_PRICE_0(prob);
    price += GET_PRICE_1(p->isRep0Long[state][posState]);
  }
  else
  {
    price = GET_PRICE_1(prob);
    prob = p->isRepG1[state];
    if (repIndex == 1)
      price += GET_PRICE_0(prob);
    else
    {
      price += GET_PRICE_1(prob);
      price += GET_PRICE(p->isRepG2[state], repIndex - 2);
    }
  }
  return price;
}


static unsigned Backward(CLzmaEnc *p, unsigned cur)
{
  unsigned wr = cur + 1;
  p->optEnd = wr;

  for (;;)
  {
    UInt32 dist = p->opt[cur].dist;
    unsigned len = (unsigned)p->opt[cur].len;
    unsigned extra = (unsigned)p->opt[cur].extra;
    cur -= len;

    if (extra)
    {
      wr--;
      p->opt[wr].len = (UInt32)len;
      cur -= extra;
      len = extra;
      if (extra == 1)
      {
        p->opt[wr].dist = dist;
        dist = MARK_LIT;
      }
      else
      {
        p->opt[wr].dist = 0;
        len--;
        wr--;
        p->opt[wr].dist = MARK_LIT;
        p->opt[wr].len = 1;
      }
    }

    if (cur == 0)
    {
      p->backRes = dist;
      p->optCur = wr;
      return len;
    }

    wr--;
    p->opt[wr].dist = dist;
    p->opt[wr].len = (UInt32)len;
  }
}



#define LIT_PROBS(pos, prevByte) \
  (p->litProbs + (UInt32)3 * (((((pos) << 8) + (prevByte)) & p->lpMask) << p->lc))


static unsigned GetOptimum(CLzmaEnc *p, UInt32 position)
{
  unsigned last, cur;
  UInt32 reps[LZMA_NUM_REPS];
  unsigned repLens[LZMA_NUM_REPS];
  UInt32 *matches;

  {
    UInt32 numAvail;
    unsigned numPairs, mainLen, repMaxIndex, i, posState;
    UInt32 matchPrice, repMatchPrice;
    const Byte *data;
    Byte curByte, matchByte;

    p->optCur = p->optEnd = 0;

    if (p->additionalOffset == 0)
      mainLen = ReadMatchDistances(p, &numPairs);
    else
    {
      mainLen = p->longestMatchLen;
      numPairs = p->numPairs;
    }

    numAvail = p->numAvail;
    if (numAvail < 2)
    {
      p->backRes = MARK_LIT;
      return 1;
    }
    if (numAvail > LZMA_MATCH_LEN_MAX)
      numAvail = LZMA_MATCH_LEN_MAX;

    data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;
    repMaxIndex = 0;

    for (i = 0; i < LZMA_NUM_REPS; i++)
    {
      unsigned len;
      const Byte *data2;
      reps[i] = p->reps[i];
      data2 = data - reps[i];
      if (data[0] != data2[0] || data[1] != data2[1])
      {
        repLens[i] = 0;
        continue;
      }
      for (len = 2; len < numAvail && data[len] == data2[len]; len++)
      {}
      repLens[i] = len;
      if (len > repLens[repMaxIndex])
        repMaxIndex = i;
    }

    if (repLens[repMaxIndex] >= p->numFastBytes)
    {
      unsigned len;
      p->backRes = (UInt32)repMaxIndex;
      len = repLens[repMaxIndex];
      MOVE_POS(p, len - 1)
      return len;
    }

    matches = p->matches;

    if (mainLen >= p->numFastBytes)
    {
      p->backRes = matches[(size_t)numPairs - 1] + LZMA_NUM_REPS;
      MOVE_POS(p, mainLen - 1)
      return mainLen;
    }

    curByte = *data;
    matchByte = *(data - reps[0]);

    last = repLens[repMaxIndex];
    if (last <= mainLen)
      last = mainLen;

    if (last < 2 && curByte != matchByte)
    {
      p->backRes = MARK_LIT;
      return 1;
    }

    p->opt[0].state = (CState)p->state;

    posState = (position & p->pbMask);

    {
      const CLzmaProb *probs = LIT_PROBS(position, *(data - 1));
      p->opt[1].price = GET_PRICE_0(p->isMatch[p->state][posState]) +
        (!IsLitState(p->state) ?
          LitEnc_Matched_GetPrice(probs, curByte, matchByte, p->ProbPrices) :
          LitEnc_GetPrice(probs, curByte, p->ProbPrices));
    }

    MakeAs_Lit(&p->opt[1]);

    matchPrice = GET_PRICE_1(p->isMatch[p->state][posState]);
    repMatchPrice = matchPrice + GET_PRICE_1(p->isRep[p->state]);

    // 18.06
    if (matchByte == curByte && repLens[0] == 0)
    {
      UInt32 shortRepPrice = repMatchPrice + GetPrice_ShortRep(p, p->state, posState);
      if (shortRepPrice < p->opt[1].price)
      {
        p->opt[1].price = shortRepPrice;
        MakeAs_ShortRep(&p->opt[1]);
      }
      if (last < 2)
      {
        p->backRes = p->opt[1].dist;
        return 1;
      }
    }

    p->opt[1].len = 1;

    p->opt[0].reps[0] = reps[0];
    p->opt[0].reps[1] = reps[1];
    p->opt[0].reps[2] = reps[2];
    p->opt[0].reps[3] = reps[3];

    // ---------- REP ----------

    for (i = 0; i < LZMA_NUM_REPS; i++)
    {
      unsigned repLen = repLens[i];
      UInt32 price;
      if (repLen < 2)
        continue;
      price = repMatchPrice + GetPrice_PureRep(p, i, p->state, posState);
      do
      {
        UInt32 price2 = price + GET_PRICE_LEN(&p->repLenEnc, posState, repLen);
        COptimal *opt = &p->opt[repLen];
        if (price2 < opt->price)
        {
          opt->price = price2;
          opt->len = (UInt32)repLen;
          opt->dist = (UInt32)i;
          opt->extra = 0;
        }
      }
      while (--repLen >= 2);
    }


    // ---------- MATCH ----------
    {
      unsigned len = repLens[0] + 1;
      if (len <= mainLen)
      {
        unsigned offs = 0;
        UInt32 normalMatchPrice = matchPrice + GET_PRICE_0(p->isRep[p->state]);

        if (len < 2)
          len = 2;
        else
          while (len > matches[offs])
            offs += 2;

        for (; ; len++)
        {
          COptimal *opt;
          UInt32 dist = matches[(size_t)offs + 1];
          UInt32 price = normalMatchPrice + GET_PRICE_LEN(&p->lenEnc, posState, len);
          unsigned lenToPosState = GetLenToPosState(len);

          if (dist < kNumFullDistances)
            price += p->distancesPrices[lenToPosState][dist & (kNumFullDistances - 1)];
          else
          {
            unsigned slot;
            GetPosSlot2(dist, slot);
            price += p->alignPrices[dist & kAlignMask];
            price += p->posSlotPrices[lenToPosState][slot];
          }

          opt = &p->opt[len];

          if (price < opt->price)
          {
            opt->price = price;
            opt->len = (UInt32)len;
            opt->dist = dist + LZMA_NUM_REPS;
            opt->extra = 0;
          }

          if (len == matches[offs])
          {
            offs += 2;
            if (offs == numPairs)
              break;
          }
        }
      }
    }


    cur = 0;

    #ifdef SHOW_STAT2
    /* if (position >= 0) */
    {
      unsigned i;
      printf("\n pos = %4X", position);
      for (i = cur; i <= last; i++)
      printf("\nprice[%4X] = %u", position - cur + i, p->opt[i].price);
    }
    #endif
  }



  // ---------- Optimal Parsing ----------

  for (;;)
  {
    unsigned numAvail;
    UInt32 numAvailFull;
    unsigned newLen, numPairs, prev, state, posState, startLen;
    UInt32 litPrice, matchPrice, repMatchPrice;
    BoolInt nextIsLit;
    Byte curByte, matchByte;
    const Byte *data;
    COptimal *curOpt, *nextOpt;

    if (++cur == last)
      break;

    // 18.06
    if (cur >= kNumOpts - 64)
    {
      unsigned j, best;
      UInt32 price = p->opt[cur].price;
      best = cur;
      for (j = cur + 1; j <= last; j++)
      {
        UInt32 price2 = p->opt[j].price;
        if (price >= price2)
        {
          price = price2;
          best = j;
        }
      }
      {
        unsigned delta = best - cur;
        if (delta != 0)
        {
          MOVE_POS(p, delta);
        }
      }
      cur = best;
      break;
    }

    newLen = ReadMatchDistances(p, &numPairs);

    if (newLen >= p->numFastBytes)
    {
      p->numPairs = numPairs;
      p->longestMatchLen = newLen;
      break;
    }

    curOpt = &p->opt[cur];

    position++;

    // we need that check here, if skip_items in p->opt are possible
    /*
    if (curOpt->price >= kInfinityPrice)
      continue;
    */

    prev = cur - curOpt->len;

    if (curOpt->len == 1)
    {
      state = (unsigned)p->opt[prev].state;
      if (IsShortRep(curOpt))
        state = kShortRepNextStates[state];
      else
        state = kLiteralNextStates[state];
    }
    else
    {
      const COptimal *prevOpt;
      UInt32 b0;
      UInt32 dist = curOpt->dist;

      if (curOpt->extra)
      {
        prev -= (unsigned)curOpt->extra;
        state = kState_RepAfterLit;
        if (curOpt->extra == 1)
          state = (dist < LZMA_NUM_REPS ? kState_RepAfterLit : kState_MatchAfterLit);
      }
      else
      {
        state = (unsigned)p->opt[prev].state;
        if (dist < LZMA_NUM_REPS)
          state = kRepNextStates[state];
        else
          state = kMatchNextStates[state];
      }

      prevOpt = &p->opt[prev];
      b0 = prevOpt->reps[0];

      if (dist < LZMA_NUM_REPS)
      {
        if (dist == 0)
        {
          reps[0] = b0;
          reps[1] = prevOpt->reps[1];
          reps[2] = prevOpt->reps[2];
          reps[3] = prevOpt->reps[3];
        }
        else
        {
          reps[1] = b0;
          b0 = prevOpt->reps[1];
          if (dist == 1)
          {
            reps[0] = b0;
            reps[2] = prevOpt->reps[2];
            reps[3] = prevOpt->reps[3];
          }
          else
          {
            reps[2] = b0;
            reps[0] = prevOpt->reps[dist];
            reps[3] = prevOpt->reps[dist ^ 1];
          }
        }
      }
      else
      {
        reps[0] = (dist - LZMA_NUM_REPS + 1);
        reps[1] = b0;
        reps[2] = prevOpt->reps[1];
        reps[3] = prevOpt->reps[2];
      }
    }

    curOpt->state = (CState)state;
    curOpt->reps[0] = reps[0];
    curOpt->reps[1] = reps[1];
    curOpt->reps[2] = reps[2];
    curOpt->reps[3] = reps[3];

    data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;
    curByte = *data;
    matchByte = *(data - reps[0]);

    posState = (position & p->pbMask);

    /*
    The order of Price checks:
       <  LIT
       <= SHORT_REP
       <  LIT : REP_0
       <  REP    [ : LIT : REP_0 ]
       <  MATCH  [ : LIT : REP_0 ]
    */

    {
      UInt32 curPrice = curOpt->price;
      unsigned prob = p->isMatch[state][posState];
      matchPrice = curPrice + GET_PRICE_1(prob);
      litPrice = curPrice + GET_PRICE_0(prob);
    }

    nextOpt = &p->opt[(size_t)cur + 1];
    nextIsLit = False;

    // here we can allow skip_items in p->opt, if we don't check (nextOpt->price < kInfinityPrice)
    // 18.new.06
    if ((nextOpt->price < kInfinityPrice
        // && !IsLitState(state)
        && matchByte == curByte)
        || litPrice > nextOpt->price
        )
      litPrice = 0;
    else
    {
      const CLzmaProb *probs = LIT_PROBS(position, *(data - 1));
      litPrice += (!IsLitState(state) ?
          LitEnc_Matched_GetPrice(probs, curByte, matchByte, p->ProbPrices) :
          LitEnc_GetPrice(probs, curByte, p->ProbPrices));

      if (litPrice < nextOpt->price)
      {
        nextOpt->price = litPrice;
        nextOpt->len = 1;
        MakeAs_Lit(nextOpt);
        nextIsLit = True;
      }
    }

    repMatchPrice = matchPrice + GET_PRICE_1(p->isRep[state]);

    numAvailFull = p->numAvail;
    {
      unsigned temp = kNumOpts - 1 - cur;
      if (numAvailFull > temp)
        numAvailFull = (UInt32)temp;
    }

    // 18.06
    // ---------- SHORT_REP ----------
    if (IsLitState(state)) // 18.new
    if (matchByte == curByte)
    if (repMatchPrice < nextOpt->price) // 18.new
    // if (numAvailFull < 2 || data[1] != *(data - reps[0] + 1))
    if (
        // nextOpt->price >= kInfinityPrice ||
        nextOpt->len < 2   // we can check nextOpt->len, if skip items are not allowed in p->opt
        || (nextOpt->dist != 0
            // && nextOpt->extra <= 1 // 17.old
            )
        )
    {
      UInt32 shortRepPrice = repMatchPrice + GetPrice_ShortRep(p, state, posState);
      // if (shortRepPrice <= nextOpt->price) // 17.old
      if (shortRepPrice < nextOpt->price)  // 18.new
      {
        nextOpt->price = shortRepPrice;
        nextOpt->len = 1;
        MakeAs_ShortRep(nextOpt);
        nextIsLit = False;
      }
    }

    if (numAvailFull < 2)
      continue;
    numAvail = (numAvailFull <= p->numFastBytes ? numAvailFull : p->numFastBytes);

    // numAvail <= p->numFastBytes

    // ---------- LIT : REP_0 ----------

    if (!nextIsLit
        && litPrice != 0 // 18.new
        && matchByte != curByte
        && numAvailFull > 2)
    {
      const Byte *data2 = data - reps[0];
      if (data[1] == data2[1] && data[2] == data2[2])
      {
        unsigned len;
        unsigned limit = p->numFastBytes + 1;
        if (limit > numAvailFull)
          limit = numAvailFull;
        for (len = 3; len < limit && data[len] == data2[len]; len++)
        {}

        {
          unsigned state2 = kLiteralNextStates[state];
          unsigned posState2 = (position + 1) & p->pbMask;
          UInt32 price = litPrice + GetPrice_Rep_0(p, state2, posState2);
          {
            unsigned offset = cur + len;

            if (last < offset)
              last = offset;

            // do
            {
              UInt32 price2;
              COptimal *opt;
              len--;
              // price2 = price + GetPrice_Len_Rep_0(p, len, state2, posState2);
              price2 = price + GET_PRICE_LEN(&p->repLenEnc, posState2, len);

              opt = &p->opt[offset];
              // offset--;
              if (price2 < opt->price)
              {
                opt->price = price2;
                opt->len = (UInt32)len;
                opt->dist = 0;
                opt->extra = 1;
              }
            }
            // while (len >= 3);
          }
        }
      }
    }

    startLen = 2; /* speed optimization */

    {
      // ---------- REP ----------
      unsigned repIndex = 0; // 17.old
      // unsigned repIndex = IsLitState(state) ? 0 : 1; // 18.notused
      for (; repIndex < LZMA_NUM_REPS; repIndex++)
      {
        unsigned len;
        UInt32 price;
        const Byte *data2 = data - reps[repIndex];
        if (data[0] != data2[0] || data[1] != data2[1])
          continue;

        for (len = 2; len < numAvail && data[len] == data2[len]; len++)
        {}

        // if (len < startLen) continue; // 18.new: speed optimization

        {
          unsigned offset = cur + len;
          if (last < offset)
            last = offset;
        }
        {
          unsigned len2 = len;
          price = repMatchPrice + GetPrice_PureRep(p, repIndex, state, posState);
          do
          {
            UInt32 price2 = price + GET_PRICE_LEN(&p->repLenEnc, posState, len2);
            COptimal *opt = &p->opt[cur + len2];
            if (price2 < opt->price)
            {
              opt->price = price2;
              opt->len = (UInt32)len2;
              opt->dist = (UInt32)repIndex;
              opt->extra = 0;
            }
          }
          while (--len2 >= 2);
        }

        if (repIndex == 0) startLen = len + 1;  // 17.old
        // startLen = len + 1; // 18.new

        /* if (_maxMode) */
        {
          // ---------- REP : LIT : REP_0 ----------
          // numFastBytes + 1 + numFastBytes

          unsigned len2 = len + 1;
          unsigned limit = len2 + p->numFastBytes;
          if (limit > numAvailFull)
            limit = numAvailFull;

          len2 += 2;
          if (len2 <= limit)
          if (data[len2 - 2] == data2[len2 - 2])
          if (data[len2 - 1] == data2[len2 - 1])
          {
            unsigned state2 = kRepNextStates[state];
            unsigned posState2 = (position + len) & p->pbMask;
            price += GET_PRICE_LEN(&p->repLenEnc, posState, len)
                + GET_PRICE_0(p->isMatch[state2][posState2])
                + LitEnc_Matched_GetPrice(LIT_PROBS(position + len, data[(size_t)len - 1]),
                    data[len], data2[len], p->ProbPrices);

            // state2 = kLiteralNextStates[state2];
            state2 = kState_LitAfterRep;
            posState2 = (posState2 + 1) & p->pbMask;


            price += GetPrice_Rep_0(p, state2, posState2);

          for (; len2 < limit && data[len2] == data2[len2]; len2++)
          {}

          len2 -= len;
          // if (len2 >= 3)
          {
            {
              unsigned offset = cur + len + len2;

              if (last < offset)
                last = offset;
              // do
              {
                UInt32 price2;
                COptimal *opt;
                len2--;
                // price2 = price + GetPrice_Len_Rep_0(p, len2, state2, posState2);
                price2 = price + GET_PRICE_LEN(&p->repLenEnc, posState2, len2);

                opt = &p->opt[offset];
                // offset--;
                if (price2 < opt->price)
                {
                  opt->price = price2;
                  opt->len = (UInt32)len2;
                  opt->extra = (CExtra)(len + 1);
                  opt->dist = (UInt32)repIndex;
                }
              }
              // while (len2 >= 3);
            }
          }
          }
        }
      }
    }


    // ---------- MATCH ----------
    /* for (unsigned len = 2; len <= newLen; len++) */
    if (newLen > numAvail)
    {
      newLen = numAvail;
      for (numPairs = 0; newLen > matches[numPairs]; numPairs += 2);
      matches[numPairs] = (UInt32)newLen;
      numPairs += 2;
    }

    // startLen = 2; /* speed optimization */

    if (newLen >= startLen)
    {
      UInt32 normalMatchPrice = matchPrice + GET_PRICE_0(p->isRep[state]);
      UInt32 dist;
      unsigned offs, posSlot, len;

      {
        unsigned offset = cur + newLen;
        if (last < offset)
          last = offset;
      }

      offs = 0;
      while (startLen > matches[offs])
        offs += 2;
      dist = matches[(size_t)offs + 1];

      // if (dist >= kNumFullDistances)
      GetPosSlot2(dist, posSlot);

      for (len = /*2*/ startLen; ; len++)
      {
        UInt32 price = normalMatchPrice + GET_PRICE_LEN(&p->lenEnc, posState, len);
        {
          COptimal *opt;
          unsigned lenNorm = len - 2;
          lenNorm = GetLenToPosState2(lenNorm);
          if (dist < kNumFullDistances)
            price += p->distancesPrices[lenNorm][dist & (kNumFullDistances - 1)];
          else
            price += p->posSlotPrices[lenNorm][posSlot] + p->alignPrices[dist & kAlignMask];

          opt = &p->opt[cur + len];
          if (price < opt->price)
          {
            opt->price = price;
            opt->len = (UInt32)len;
            opt->dist = dist + LZMA_NUM_REPS;
            opt->extra = 0;
          }
        }

        if (len == matches[offs])
        {
          // if (p->_maxMode) {
          // MATCH : LIT : REP_0

          const Byte *data2 = data - dist - 1;
          unsigned len2 = len + 1;
          unsigned limit = len2 + p->numFastBytes;
          if (limit > numAvailFull)
            limit = numAvailFull;

          len2 += 2;
          if (len2 <= limit)
          if (data[len2 - 2] == data2[len2 - 2])
          if (data[len2 - 1] == data2[len2 - 1])
          {
          for (; len2 < limit && data[len2] == data2[len2]; len2++)
          {}

          len2 -= len;

          // if (len2 >= 3)
          {
            unsigned state2 = kMatchNextStates[state];
            unsigned posState2 = (position + len) & p->pbMask;
            unsigned offset;
            price += GET_PRICE_0(p->isMatch[state2][posState2]);
            price += LitEnc_Matched_GetPrice(LIT_PROBS(position + len, data[(size_t)len - 1]),
                    data[len], data2[len], p->ProbPrices);

            // state2 = kLiteralNextStates[state2];
            state2 = kState_LitAfterMatch;

            posState2 = (posState2 + 1) & p->pbMask;
            price += GetPrice_Rep_0(p, state2, posState2);

            offset = cur + len + len2;

            if (last < offset)
              last = offset;
            // do
            {
              UInt32 price2;
              COptimal *opt;
              len2--;
              // price2 = price + GetPrice_Len_Rep_0(p, len2, state2, posState2);
              price2 = price + GET_PRICE_LEN(&p->repLenEnc, posState2, len2);
              opt = &p->opt[offset];
              // offset--;
              if (price2 < opt->price)
              {
                opt->price = price2;
                opt->len = (UInt32)len2;
                opt->extra = (CExtra)(len + 1);
                opt->dist = dist + LZMA_NUM_REPS;
              }
            }
            // while (len2 >= 3);
          }

          }

          offs += 2;
          if (offs == numPairs)
            break;
          dist = matches[(size_t)offs + 1];
          // if (dist >= kNumFullDistances)
            GetPosSlot2(dist, posSlot);
        }
      }
    }
  }

  do
    p->opt[last].price = kInfinityPrice;
  while (--last);

  return Backward(p, cur);
}



#define ChangePair(smallDist, bigDist) (((bigDist) >> 7) > (smallDist))



static unsigned GetOptimumFast(CLzmaEnc *p)
{
  UInt32 numAvail, mainDist;
  unsigned mainLen, numPairs, repIndex, repLen, i;
  const Byte *data;

  if (p->additionalOffset == 0)
    mainLen = ReadMatchDistances(p, &numPairs);
  else
  {
    mainLen = p->longestMatchLen;
    numPairs = p->numPairs;
  }

  numAvail = p->numAvail;
  p->backRes = MARK_LIT;
  if (numAvail < 2)
    return 1;
  // if (mainLen < 2 && p->state == 0) return 1; // 18.06.notused
  if (numAvail > LZMA_MATCH_LEN_MAX)
    numAvail = LZMA_MATCH_LEN_MAX;
  data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;
  repLen = repIndex = 0;

  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    unsigned len;
    const Byte *data2 = data - p->reps[i];
    if (data[0] != data2[0] || data[1] != data2[1])
      continue;
    for (len = 2; len < numAvail && data[len] == data2[len]; len++)
    {}
    if (len >= p->numFastBytes)
    {
      p->backRes = (UInt32)i;
      MOVE_POS(p, len - 1)
      return len;
    }
    if (len > repLen)
    {
      repIndex = i;
      repLen = len;
    }
  }

  if (mainLen >= p->numFastBytes)
  {
    p->backRes = p->matches[(size_t)numPairs - 1] + LZMA_NUM_REPS;
    MOVE_POS(p, mainLen - 1)
    return mainLen;
  }

  mainDist = 0; /* for GCC */

  if (mainLen >= 2)
  {
    mainDist = p->matches[(size_t)numPairs - 1];
    while (numPairs > 2)
    {
      UInt32 dist2;
      if (mainLen != p->matches[(size_t)numPairs - 4] + 1)
        break;
      dist2 = p->matches[(size_t)numPairs - 3];
      if (!ChangePair(dist2, mainDist))
        break;
      numPairs -= 2;
      mainLen--;
      mainDist = dist2;
    }
    if (mainLen == 2 && mainDist >= 0x80)
      mainLen = 1;
  }

  if (repLen >= 2)
    if (    repLen + 1 >= mainLen
        || (repLen + 2 >= mainLen && mainDist >= (1 << 9))
        || (repLen + 3 >= mainLen && mainDist >= (1 << 15)))
  {
    p->backRes = (UInt32)repIndex;
    MOVE_POS(p, repLen - 1)
    return repLen;
  }

  if (mainLen < 2 || numAvail <= 2)
    return 1;

  {
    unsigned len1 = ReadMatchDistances(p, &p->numPairs);
    p->longestMatchLen = len1;

    if (len1 >= 2)
    {
      UInt32 newDist = p->matches[(size_t)p->numPairs - 1];
      if (   (len1 >= mainLen && newDist < mainDist)
          || (len1 == mainLen + 1 && !ChangePair(mainDist, newDist))
          || (len1 >  mainLen + 1)
          || (len1 + 1 >= mainLen && mainLen >= 3 && ChangePair(newDist, mainDist)))
        return 1;
    }
  }

  data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - 1;

  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    unsigned len, limit;
    const Byte *data2 = data - p->reps[i];
    if (data[0] != data2[0] || data[1] != data2[1])
      continue;
    limit = mainLen - 1;
    for (len = 2;; len++)
    {
      if (len >= limit)
        return 1;
      if (data[len] != data2[len])
        break;
    }
  }

  p->backRes = mainDist + LZMA_NUM_REPS;
  if (mainLen != 2)
  {
    MOVE_POS(p, mainLen - 2)
  }
  return mainLen;
}




static void WriteEndMarker(CLzmaEnc *p, unsigned posState)
{
  UInt32 range;
  range = p->rc.range;
  {
    UInt32 ttt, newBound;
    CLzmaProb *prob = &p->isMatch[p->state][posState];
    RC_BIT_PRE(&p->rc, prob)
    RC_BIT_1(&p->rc, prob)
    prob = &p->isRep[p->state];
    RC_BIT_PRE(&p->rc, prob)
    RC_BIT_0(&p->rc, prob)
  }
  p->state = kMatchNextStates[p->state];

  p->rc.range = range;
  LenEnc_Encode(&p->lenProbs, &p->rc, 0, posState);
  range = p->rc.range;

  {
    // RcTree_Encode_PosSlot(&p->rc, p->posSlotEncoder[0], (1 << kNumPosSlotBits) - 1);
    CLzmaProb *probs = p->posSlotEncoder[0];
    unsigned m = 1;
    do
    {
      UInt32 ttt, newBound;
      RC_BIT_PRE(p, probs + m)
      RC_BIT_1(&p->rc, probs + m);
      m = (m << 1) + 1;
    }
    while (m < (1 << kNumPosSlotBits));
  }
  {
    // RangeEnc_EncodeDirectBits(&p->rc, ((UInt32)1 << (30 - kNumAlignBits)) - 1, 30 - kNumAlignBits);    UInt32 range = p->range;
    unsigned numBits = 30 - kNumAlignBits;
    do
    {
      range >>= 1;
      p->rc.low += range;
      RC_NORM(&p->rc)
    }
    while (--numBits);
  }

  {
    // RcTree_ReverseEncode(&p->rc, p->posAlignEncoder, kNumAlignBits, kAlignMask);
    CLzmaProb *probs = p->posAlignEncoder;
    unsigned m = 1;
    do
    {
      UInt32 ttt, newBound;
      RC_BIT_PRE(p, probs + m)
      RC_BIT_1(&p->rc, probs + m);
      m = (m << 1) + 1;
    }
    while (m < kAlignTableSize);
  }
  p->rc.range = range;
}


static SRes CheckErrors(CLzmaEnc *p)
{
  if (p->result != SZ_OK)
    return p->result;
  if (p->rc.res != SZ_OK)
    p->result = SZ_ERROR_WRITE;
  if (p->matchFinderBase.result != SZ_OK)
    p->result = SZ_ERROR_READ;
  if (p->result != SZ_OK)
    p->finished = True;
  return p->result;
}


MY_NO_INLINE static SRes Flush(CLzmaEnc *p, UInt32 nowPos)
{
  /* ReleaseMFStream(); */
  p->finished = True;
  if (p->writeEndMark)
    WriteEndMarker(p, nowPos & p->pbMask);
  RangeEnc_FlushData(&p->rc);
  RangeEnc_FlushStream(&p->rc);
  return CheckErrors(p);
}


MY_NO_INLINE static void FillAlignPrices(CLzmaEnc *p)
{
  unsigned i;
  const CProbPrice *ProbPrices = p->ProbPrices;
  const CLzmaProb *probs = p->posAlignEncoder;
  // p->alignPriceCount = 0;
  for (i = 0; i < kAlignTableSize / 2; i++)
  {
    UInt32 price = 0;
    unsigned sym = i;
    unsigned m = 1;
    unsigned bit;
    UInt32 prob;
    bit = sym & 1; sym >>= 1; price += GET_PRICEa(probs[m], bit); m = (m << 1) + bit;
    bit = sym & 1; sym >>= 1; price += GET_PRICEa(probs[m], bit); m = (m << 1) + bit;
    bit = sym & 1; sym >>= 1; price += GET_PRICEa(probs[m], bit); m = (m << 1) + bit;
    prob = probs[m];
    p->alignPrices[i    ] = price + GET_PRICEa_0(prob);
    p->alignPrices[i + 8] = price + GET_PRICEa_1(prob);
    // p->alignPrices[i] = RcTree_ReverseGetPrice(p->posAlignEncoder, kNumAlignBits, i, p->ProbPrices);
  }
}


MY_NO_INLINE static void FillDistancesPrices(CLzmaEnc *p)
{
  // int y; for (y = 0; y < 100; y++) {

  UInt32 tempPrices[kNumFullDistances];
  unsigned i, lps;

  const CProbPrice *ProbPrices = p->ProbPrices;
  p->matchPriceCount = 0;

  for (i = kStartPosModelIndex / 2; i < kNumFullDistances / 2; i++)
  {
    unsigned posSlot = GetPosSlot1(i);
    unsigned footerBits = (posSlot >> 1) - 1;
    unsigned base = ((2 | (posSlot & 1)) << footerBits);
    const CLzmaProb *probs = p->posEncoders + (size_t)base * 2;
    // tempPrices[i] = RcTree_ReverseGetPrice(p->posEncoders + base, footerBits, i - base, p->ProbPrices);
    UInt32 price = 0;
    unsigned m = 1;
    unsigned sym = i;
    unsigned offset = (unsigned)1 << footerBits;
    base += i;

    if (footerBits)
    do
    {
      unsigned bit = sym & 1;
      sym >>= 1;
      price += GET_PRICEa(probs[m], bit);
      m = (m << 1) + bit;
    }
    while (--footerBits);

    {
      unsigned prob = probs[m];
      tempPrices[base         ] = price + GET_PRICEa_0(prob);
      tempPrices[base + offset] = price + GET_PRICEa_1(prob);
    }
  }

  for (lps = 0; lps < kNumLenToPosStates; lps++)
  {
    unsigned slot;
    unsigned distTableSize2 = (p->distTableSize + 1) >> 1;
    UInt32 *posSlotPrices = p->posSlotPrices[lps];
    const CLzmaProb *probs = p->posSlotEncoder[lps];

    for (slot = 0; slot < distTableSize2; slot++)
    {
      // posSlotPrices[slot] = RcTree_GetPrice(encoder, kNumPosSlotBits, slot, p->ProbPrices);
      UInt32 price;
      unsigned bit;
      unsigned sym = slot + (1 << (kNumPosSlotBits - 1));
      unsigned prob;
      bit = sym & 1; sym >>= 1; price  = GET_PRICEa(probs[sym], bit);
      bit = sym & 1; sym >>= 1; price += GET_PRICEa(probs[sym], bit);
      bit = sym & 1; sym >>= 1; price += GET_PRICEa(probs[sym], bit);
      bit = sym & 1; sym >>= 1; price += GET_PRICEa(probs[sym], bit);
      bit = sym & 1; sym >>= 1; price += GET_PRICEa(probs[sym], bit);
      prob = probs[(size_t)slot + (1 << (kNumPosSlotBits - 1))];
      posSlotPrices[(size_t)slot * 2    ] = price + GET_PRICEa_0(prob);
      posSlotPrices[(size_t)slot * 2 + 1] = price + GET_PRICEa_1(prob);
    }

    {
      UInt32 delta = ((UInt32)((kEndPosModelIndex / 2 - 1) - kNumAlignBits) << kNumBitPriceShiftBits);
      for (slot = kEndPosModelIndex / 2; slot < distTableSize2; slot++)
      {
        posSlotPrices[(size_t)slot * 2    ] += delta;
        posSlotPrices[(size_t)slot * 2 + 1] += delta;
        delta += ((UInt32)1 << kNumBitPriceShiftBits);
      }
    }

    {
      UInt32 *dp = p->distancesPrices[lps];

      dp[0] = posSlotPrices[0];
      dp[1] = posSlotPrices[1];
      dp[2] = posSlotPrices[2];
      dp[3] = posSlotPrices[3];

      for (i = 4; i < kNumFullDistances; i += 2)
      {
        UInt32 slotPrice = posSlotPrices[GetPosSlot1(i)];
        dp[i    ] = slotPrice + tempPrices[i];
        dp[i + 1] = slotPrice + tempPrices[i + 1];
      }
    }
  }
  // }
}



void LzmaEnc_Construct(CLzmaEnc *p)
{
  RangeEnc_Construct(&p->rc);
  MatchFinder_Construct(&p->matchFinderBase);

  {
    CLzmaEncProps props;
    LzmaEncProps_Init(&props);
    LzmaEnc_SetProps(p, &props);
  }

  #ifndef LZMA_LOG_BSR
  LzmaEnc_FastPosInit(p->g_FastPos);
  #endif

  LzmaEnc_InitPriceTables(p->ProbPrices);
  p->litProbs = NULL;
  p->saveState.litProbs = NULL;

}

CLzmaEncHandle LzmaEnc_Create(ISzAllocPtr alloc)
{
  void *p;
  p = ISzAlloc_Alloc(alloc, sizeof(CLzmaEnc));
  if (p)
    LzmaEnc_Construct((CLzmaEnc *)p);
  return p;
}

void LzmaEnc_FreeLits(CLzmaEnc *p, ISzAllocPtr alloc)
{
  ISzAlloc_Free(alloc, p->litProbs);
  ISzAlloc_Free(alloc, p->saveState.litProbs);
  p->litProbs = NULL;
  p->saveState.litProbs = NULL;
}

void LzmaEnc_Destruct(CLzmaEnc *p, ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  MatchFinder_Free(&p->matchFinderBase, allocBig);
  LzmaEnc_FreeLits(p, alloc);
  RangeEnc_Free(&p->rc, alloc);
}

void LzmaEnc_Destroy(CLzmaEncHandle p, ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  LzmaEnc_Destruct((CLzmaEnc *)p, alloc, allocBig);
  ISzAlloc_Free(alloc, p);
}


static SRes LzmaEnc_CodeOneBlock(CLzmaEnc *p, UInt32 maxPackSize, UInt32 maxUnpackSize)
{
  UInt32 nowPos32, startPos32;
  if (p->needInit)
  {
    p->matchFinder.Init(p->matchFinderObj);
    p->needInit = 0;
  }

  if (p->finished)
    return p->result;
  RINOK(CheckErrors(p));

  nowPos32 = (UInt32)p->nowPos64;
  startPos32 = nowPos32;

  if (p->nowPos64 == 0)
  {
    unsigned numPairs;
    Byte curByte;
    if (p->matchFinder.GetNumAvailableBytes(p->matchFinderObj) == 0)
      return Flush(p, nowPos32);
    ReadMatchDistances(p, &numPairs);
    RangeEnc_EncodeBit_0(&p->rc, &p->isMatch[kState_Start][0]);
    // p->state = kLiteralNextStates[p->state];
    curByte = *(p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - p->additionalOffset);
    LitEnc_Encode(&p->rc, p->litProbs, curByte);
    p->additionalOffset--;
    nowPos32++;
  }

  if (p->matchFinder.GetNumAvailableBytes(p->matchFinderObj) != 0)

  for (;;)
  {
    UInt32 dist;
    unsigned len, posState;
    UInt32 range, ttt, newBound;
    CLzmaProb *probs;

    if (p->fastMode)
      len = GetOptimumFast(p);
    else
    {
      unsigned oci = p->optCur;
      if (p->optEnd == oci)
        len = GetOptimum(p, nowPos32);
      else
      {
        const COptimal *opt = &p->opt[oci];
        len = opt->len;
        p->backRes = opt->dist;
        p->optCur = oci + 1;
      }
    }

    posState = (unsigned)nowPos32 & p->pbMask;
    range = p->rc.range;
    probs = &p->isMatch[p->state][posState];

    RC_BIT_PRE(&p->rc, probs)

    dist = p->backRes;

    #ifdef SHOW_STAT2
    printf("\n pos = %6X, len = %3u  pos = %6u", nowPos32, len, dist);
    #endif

    if (dist == MARK_LIT)
    {
      Byte curByte;
      const Byte *data;
      unsigned state;

      RC_BIT_0(&p->rc, probs);
      p->rc.range = range;
      data = p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - p->additionalOffset;
      probs = LIT_PROBS(nowPos32, *(data - 1));
      curByte = *data;
      state = p->state;
      p->state = kLiteralNextStates[state];
      if (IsLitState(state))
        LitEnc_Encode(&p->rc, probs, curByte);
      else
        LitEnc_EncodeMatched(&p->rc, probs, curByte, *(data - p->reps[0]));
    }
    else
    {
      RC_BIT_1(&p->rc, probs);
      probs = &p->isRep[p->state];
      RC_BIT_PRE(&p->rc, probs)

      if (dist < LZMA_NUM_REPS)
      {
        RC_BIT_1(&p->rc, probs);
        probs = &p->isRepG0[p->state];
        RC_BIT_PRE(&p->rc, probs)
        if (dist == 0)
        {
          RC_BIT_0(&p->rc, probs);
          probs = &p->isRep0Long[p->state][posState];
          RC_BIT_PRE(&p->rc, probs)
          if (len != 1)
          {
            RC_BIT_1_BASE(&p->rc, probs);
          }
          else
          {
            RC_BIT_0_BASE(&p->rc, probs);
            p->state = kShortRepNextStates[p->state];
          }
        }
        else
        {
          RC_BIT_1(&p->rc, probs);
          probs = &p->isRepG1[p->state];
          RC_BIT_PRE(&p->rc, probs)
          if (dist == 1)
          {
            RC_BIT_0_BASE(&p->rc, probs);
            dist = p->reps[1];
          }
          else
          {
            RC_BIT_1(&p->rc, probs);
            probs = &p->isRepG2[p->state];
            RC_BIT_PRE(&p->rc, probs)
            if (dist == 2)
            {
              RC_BIT_0_BASE(&p->rc, probs);
              dist = p->reps[2];
            }
            else
            {
              RC_BIT_1_BASE(&p->rc, probs);
              dist = p->reps[3];
              p->reps[3] = p->reps[2];
            }
            p->reps[2] = p->reps[1];
          }
          p->reps[1] = p->reps[0];
          p->reps[0] = dist;
        }

        RC_NORM(&p->rc)

        p->rc.range = range;

        if (len != 1)
        {
          LenEnc_Encode(&p->repLenProbs, &p->rc, len - LZMA_MATCH_LEN_MIN, posState);
          --p->repLenEncCounter;
          p->state = kRepNextStates[p->state];
        }
      }
      else
      {
        unsigned posSlot;
        RC_BIT_0(&p->rc, probs);
        p->rc.range = range;
        p->state = kMatchNextStates[p->state];

        LenEnc_Encode(&p->lenProbs, &p->rc, len - LZMA_MATCH_LEN_MIN, posState);
        // --p->lenEnc.counter;

        dist -= LZMA_NUM_REPS;
        p->reps[3] = p->reps[2];
        p->reps[2] = p->reps[1];
        p->reps[1] = p->reps[0];
        p->reps[0] = dist + 1;

        p->matchPriceCount++;
        GetPosSlot(dist, posSlot);
        // RcTree_Encode_PosSlot(&p->rc, p->posSlotEncoder[GetLenToPosState(len)], posSlot);
        {
          UInt32 sym = (UInt32)posSlot + (1 << kNumPosSlotBits);
          range = p->rc.range;
          probs = p->posSlotEncoder[GetLenToPosState(len)];
          do
          {
            CLzmaProb *prob = probs + (sym >> kNumPosSlotBits);
            UInt32 bit = (sym >> (kNumPosSlotBits - 1)) & 1;
            sym <<= 1;
            RC_BIT(&p->rc, prob, bit);
          }
          while (sym < (1 << kNumPosSlotBits * 2));
          p->rc.range = range;
        }

        if (dist >= kStartPosModelIndex)
        {
          unsigned footerBits = ((posSlot >> 1) - 1);

          if (dist < kNumFullDistances)
          {
            unsigned base = ((2 | (posSlot & 1)) << footerBits);
            RcTree_ReverseEncode(&p->rc, p->posEncoders + base, footerBits, (unsigned)(dist /* - base */));
          }
          else
          {
            UInt32 pos2 = (dist | 0xF) << (32 - footerBits);
            range = p->rc.range;
            // RangeEnc_EncodeDirectBits(&p->rc, posReduced >> kNumAlignBits, footerBits - kNumAlignBits);
            /*
            do
            {
              range >>= 1;
              p->rc.low += range & (0 - ((dist >> --footerBits) & 1));
              RC_NORM(&p->rc)
            }
            while (footerBits > kNumAlignBits);
            */
            do
            {
              range >>= 1;
              p->rc.low += range & (0 - (pos2 >> 31));
              pos2 += pos2;
              RC_NORM(&p->rc)
            }
            while (pos2 != 0xF0000000);


            // RcTree_ReverseEncode(&p->rc, p->posAlignEncoder, kNumAlignBits, posReduced & kAlignMask);

            {
              unsigned m = 1;
              unsigned bit;
              bit = dist & 1; dist >>= 1; RC_BIT(&p->rc, p->posAlignEncoder + m, bit); m = (m << 1) + bit;
              bit = dist & 1; dist >>= 1; RC_BIT(&p->rc, p->posAlignEncoder + m, bit); m = (m << 1) + bit;
              bit = dist & 1; dist >>= 1; RC_BIT(&p->rc, p->posAlignEncoder + m, bit); m = (m << 1) + bit;
              bit = dist & 1;             RC_BIT(&p->rc, p->posAlignEncoder + m, bit);
              p->rc.range = range;
              // p->alignPriceCount++;
            }
          }
        }
      }
    }

    nowPos32 += (UInt32)len;
    p->additionalOffset -= len;

    if (p->additionalOffset == 0)
    {
      UInt32 processed;

      if (!p->fastMode)
      {
        /*
        if (p->alignPriceCount >= 16) // kAlignTableSize
          FillAlignPrices(p);
        if (p->matchPriceCount >= 128)
          FillDistancesPrices(p);
        if (p->lenEnc.counter <= 0)
          LenPriceEnc_UpdateTables(&p->lenEnc, 1 << p->pb, &p->lenProbs, p->ProbPrices);
        */
        if (p->matchPriceCount >= 64)
        {
          FillAlignPrices(p);
          // { int y; for (y = 0; y < 100; y++) {
          FillDistancesPrices(p);
          // }}
          LenPriceEnc_UpdateTables(&p->lenEnc, 1 << p->pb, &p->lenProbs, p->ProbPrices);
        }
        if (p->repLenEncCounter <= 0)
        {
          p->repLenEncCounter = REP_LEN_COUNT;
          LenPriceEnc_UpdateTables(&p->repLenEnc, 1 << p->pb, &p->repLenProbs, p->ProbPrices);
        }
      }

      if (p->matchFinder.GetNumAvailableBytes(p->matchFinderObj) == 0)
        break;
      processed = nowPos32 - startPos32;

      if (maxPackSize)
      {
        if (processed + kNumOpts + 300 >= maxUnpackSize
            || RangeEnc_GetProcessed_size_t(&p->rc) + kPackReserve >= maxPackSize)
          break;
      }
      else if (processed >= (1 << 17))
      {
        p->nowPos64 += nowPos32 - startPos32;
        return CheckErrors(p);
      }
    }
  }

  p->nowPos64 += nowPos32 - startPos32;
  return Flush(p, nowPos32);
}



#define kBigHashDicLimit ((UInt32)1 << 24)

static SRes LzmaEnc_Alloc(CLzmaEnc *p, UInt32 keepWindowSize, ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  UInt32 beforeSize = kNumOpts;
  if (!RangeEnc_Alloc(&p->rc, alloc))
    return SZ_ERROR_MEM;

  {
    unsigned lclp = p->lc + p->lp;
    if (!p->litProbs || !p->saveState.litProbs || p->lclp != lclp)
    {
      LzmaEnc_FreeLits(p, alloc);
      p->litProbs = (CLzmaProb *)ISzAlloc_Alloc(alloc, ((UInt32)0x300 << lclp) * sizeof(CLzmaProb));
      p->saveState.litProbs = (CLzmaProb *)ISzAlloc_Alloc(alloc, ((UInt32)0x300 << lclp) * sizeof(CLzmaProb));
      if (!p->litProbs || !p->saveState.litProbs)
      {
        LzmaEnc_FreeLits(p, alloc);
        return SZ_ERROR_MEM;
      }
      p->lclp = lclp;
    }
  }

  p->matchFinderBase.bigHash = (Byte)(p->dictSize > kBigHashDicLimit ? 1 : 0);

  if (beforeSize + p->dictSize < keepWindowSize)
    beforeSize = keepWindowSize - p->dictSize;

  if (!MatchFinder_Create(&p->matchFinderBase, p->dictSize, beforeSize, p->numFastBytes, LZMA_MATCH_LEN_MAX, allocBig))
    return SZ_ERROR_MEM;
  p->matchFinderObj = &p->matchFinderBase;
  MatchFinder_CreateVTable(&p->matchFinderBase, &p->matchFinder);

  return SZ_OK;
}

void LzmaEnc_Init(CLzmaEnc *p)
{
  unsigned i;
  p->state = 0;
  p->reps[0] =
  p->reps[1] =
  p->reps[2] =
  p->reps[3] = 1;

  RangeEnc_Init(&p->rc);

  for (i = 0; i < (1 << kNumAlignBits); i++)
    p->posAlignEncoder[i] = kProbInitValue;

  for (i = 0; i < kNumStates; i++)
  {
    unsigned j;
    for (j = 0; j < LZMA_NUM_PB_STATES_MAX; j++)
    {
      p->isMatch[i][j] = kProbInitValue;
      p->isRep0Long[i][j] = kProbInitValue;
    }
    p->isRep[i] = kProbInitValue;
    p->isRepG0[i] = kProbInitValue;
    p->isRepG1[i] = kProbInitValue;
    p->isRepG2[i] = kProbInitValue;
  }

  {
    for (i = 0; i < kNumLenToPosStates; i++)
    {
      CLzmaProb *probs = p->posSlotEncoder[i];
      unsigned j;
      for (j = 0; j < (1 << kNumPosSlotBits); j++)
        probs[j] = kProbInitValue;
    }
  }
  {
    for (i = 0; i < kNumFullDistances; i++)
      p->posEncoders[i] = kProbInitValue;
  }

  {
    UInt32 num = (UInt32)0x300 << (p->lp + p->lc);
    UInt32 k;
    CLzmaProb *probs = p->litProbs;
    for (k = 0; k < num; k++)
      probs[k] = kProbInitValue;
  }


  LenEnc_Init(&p->lenProbs);
  LenEnc_Init(&p->repLenProbs);

  p->optEnd = 0;
  p->optCur = 0;

  {
    for (i = 0; i < kNumOpts; i++)
      p->opt[i].price = kInfinityPrice;
  }

  p->additionalOffset = 0;

  p->pbMask = (1 << p->pb) - 1;
  p->lpMask = ((UInt32)0x100 << p->lp) - ((unsigned)0x100 >> p->lc);
}


void LzmaEnc_InitPrices(CLzmaEnc *p)
{
  if (!p->fastMode)
  {
    FillDistancesPrices(p);
    FillAlignPrices(p);
  }

  p->lenEnc.tableSize =
  p->repLenEnc.tableSize =
      p->numFastBytes + 1 - LZMA_MATCH_LEN_MIN;

  p->repLenEncCounter = REP_LEN_COUNT;

  LenPriceEnc_UpdateTables(&p->lenEnc, 1 << p->pb, &p->lenProbs, p->ProbPrices);
  LenPriceEnc_UpdateTables(&p->repLenEnc, 1 << p->pb, &p->repLenProbs, p->ProbPrices);
}

static SRes LzmaEnc_AllocAndInit(CLzmaEnc *p, UInt32 keepWindowSize, ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  unsigned i;
  for (i = kEndPosModelIndex / 2; i < kDicLogSizeMax; i++)
    if (p->dictSize <= ((UInt32)1 << i))
      break;
  p->distTableSize = i * 2;

  p->finished = False;
  p->result = SZ_OK;
  RINOK(LzmaEnc_Alloc(p, keepWindowSize, alloc, allocBig));
  LzmaEnc_Init(p);
  LzmaEnc_InitPrices(p);
  p->nowPos64 = 0;
  return SZ_OK;
}

static SRes LzmaEnc_Prepare(CLzmaEncHandle pp, ISeqOutStream *outStream, ISeqInStream *inStream,
    ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  p->matchFinderBase.stream = inStream;
  p->needInit = 1;
  p->rc.outStream = outStream;
  return LzmaEnc_AllocAndInit(p, 0, alloc, allocBig);
}

SRes LzmaEnc_PrepareForLzma2(CLzmaEncHandle pp,
    ISeqInStream *inStream, UInt32 keepWindowSize,
    ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  p->matchFinderBase.stream = inStream;
  p->needInit = 1;
  return LzmaEnc_AllocAndInit(p, keepWindowSize, alloc, allocBig);
}

static void LzmaEnc_SetInputBuf(CLzmaEnc *p, const Byte *src, size_t srcLen)
{
  p->matchFinderBase.directInput = 1;
  p->matchFinderBase.bufferBase = (Byte *)src;
  p->matchFinderBase.directInputRem = srcLen;
}

SRes LzmaEnc_MemPrepare(CLzmaEncHandle pp, const Byte *src, size_t srcLen,
    UInt32 keepWindowSize, ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  LzmaEnc_SetInputBuf(p, src, srcLen);
  p->needInit = 1;

  LzmaEnc_SetDataSize(pp, srcLen);
  return LzmaEnc_AllocAndInit(p, keepWindowSize, alloc, allocBig);
}

void LzmaEnc_Finish(CLzmaEncHandle pp)
{
  UNUSED_VAR(pp);
}


typedef struct
{
  ISeqOutStream vt;
  Byte *data;
  size_t rem;
  BoolInt overflow;
} CLzmaEnc_SeqOutStreamBuf;

static size_t SeqOutStreamBuf_Write(const ISeqOutStream *pp, const void *data, size_t size)
{
  CLzmaEnc_SeqOutStreamBuf *p = CONTAINER_FROM_VTBL(pp, CLzmaEnc_SeqOutStreamBuf, vt);
  if (p->rem < size)
  {
    size = p->rem;
    p->overflow = True;
  }
  memcpy(p->data, data, size);
  p->rem -= size;
  p->data += size;
  return size;
}


UInt32 LzmaEnc_GetNumAvailableBytes(CLzmaEncHandle pp)
{
  const CLzmaEnc *p = (CLzmaEnc *)pp;
  return p->matchFinder.GetNumAvailableBytes(p->matchFinderObj);
}


const Byte *LzmaEnc_GetCurBuf(CLzmaEncHandle pp)
{
  const CLzmaEnc *p = (CLzmaEnc *)pp;
  return p->matchFinder.GetPointerToCurrentPos(p->matchFinderObj) - p->additionalOffset;
}


SRes LzmaEnc_CodeOneMemBlock(CLzmaEncHandle pp, BoolInt reInit,
    Byte *dest, size_t *destLen, UInt32 desiredPackSize, UInt32 *unpackSize)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  UInt64 nowPos64;
  SRes res;
  CLzmaEnc_SeqOutStreamBuf outStream;

  outStream.vt.Write = SeqOutStreamBuf_Write;
  outStream.data = dest;
  outStream.rem = *destLen;
  outStream.overflow = False;

  p->writeEndMark = False;
  p->finished = False;
  p->result = SZ_OK;

  if (reInit)
    LzmaEnc_Init(p);
  LzmaEnc_InitPrices(p);

  nowPos64 = p->nowPos64;
  RangeEnc_Init(&p->rc);
  p->rc.outStream = &outStream.vt;

  if (desiredPackSize == 0)
    return SZ_ERROR_OUTPUT_EOF;

  res = LzmaEnc_CodeOneBlock(p, desiredPackSize, *unpackSize);

  *unpackSize = (UInt32)(p->nowPos64 - nowPos64);
  *destLen -= outStream.rem;
  if (outStream.overflow)
    return SZ_ERROR_OUTPUT_EOF;

  return res;
}


static SRes LzmaEnc_Encode2(CLzmaEnc *p, ICompressProgress *progress)
{
  SRes res = SZ_OK;

  for (;;)
  {
    res = LzmaEnc_CodeOneBlock(p, 0, 0);
    if (res != SZ_OK || p->finished)
      break;
    if (progress)
    {
      res = ICompressProgress_Progress(progress, p->nowPos64, RangeEnc_GetProcessed(&p->rc));
      if (res != SZ_OK)
      {
        res = SZ_ERROR_PROGRESS;
        break;
      }
    }
  }

  LzmaEnc_Finish(p);

  /*
  if (res == SZ_OK && !Inline_MatchFinder_IsFinishedOK(&p->matchFinderBase))
    res = SZ_ERROR_FAIL;
  }
  */

  return res;
}


SRes LzmaEnc_Encode(CLzmaEncHandle pp, ISeqOutStream *outStream, ISeqInStream *inStream, ICompressProgress *progress,
    ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  RINOK(LzmaEnc_Prepare(pp, outStream, inStream, alloc, allocBig));
  return LzmaEnc_Encode2((CLzmaEnc *)pp, progress);
}


SRes LzmaEnc_WriteProperties(CLzmaEncHandle pp, Byte *props, size_t *size)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  unsigned i;
  UInt32 dictSize = p->dictSize;
  if (*size < LZMA_PROPS_SIZE)
    return SZ_ERROR_PARAM;
  *size = LZMA_PROPS_SIZE;
  props[0] = (Byte)((p->pb * 5 + p->lp) * 9 + p->lc);

  if (dictSize >= ((UInt32)1 << 22))
  {
    UInt32 kDictMask = ((UInt32)1 << 20) - 1;
    if (dictSize < (UInt32)0xFFFFFFFF - kDictMask)
      dictSize = (dictSize + kDictMask) & ~kDictMask;
  }
  else for (i = 11; i <= 30; i++)
  {
    if (dictSize <= ((UInt32)2 << i)) { dictSize = (2 << i); break; }
    if (dictSize <= ((UInt32)3 << i)) { dictSize = (3 << i); break; }
  }

  for (i = 0; i < 4; i++)
    props[1 + i] = (Byte)(dictSize >> (8 * i));
  return SZ_OK;
}


unsigned LzmaEnc_IsWriteEndMark(CLzmaEncHandle pp)
{
  return ((CLzmaEnc *)pp)->writeEndMark;
}


SRes LzmaEnc_MemEncode(CLzmaEncHandle pp, Byte *dest, size_t *destLen, const Byte *src, size_t srcLen,
    int writeEndMark, ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  SRes res;
  CLzmaEnc *p = (CLzmaEnc *)pp;

  CLzmaEnc_SeqOutStreamBuf outStream;

  outStream.vt.Write = SeqOutStreamBuf_Write;
  outStream.data = dest;
  outStream.rem = *destLen;
  outStream.overflow = False;

  p->writeEndMark = writeEndMark;
  p->rc.outStream = &outStream.vt;

  res = LzmaEnc_MemPrepare(pp, src, srcLen, 0, alloc, allocBig);

  if (res == SZ_OK)
  {
    res = LzmaEnc_Encode2(p, progress);
    if (res == SZ_OK && p->nowPos64 != srcLen)
      res = SZ_ERROR_FAIL;
  }

  *destLen -= outStream.rem;
  if (outStream.overflow)
    return SZ_ERROR_OUTPUT_EOF;
  return res;
}


SRes LzmaEncode(Byte *dest, size_t *destLen, const Byte *src, size_t srcLen,
    const CLzmaEncProps *props, Byte *propsEncoded, size_t *propsSize, int writeEndMark,
    ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig)
{
  CLzmaEnc *p = (CLzmaEnc *)LzmaEnc_Create(alloc);
  SRes res;
  if (!p)
    return SZ_ERROR_MEM;

  res = LzmaEnc_SetProps(p, props);
  if (res == SZ_OK)
  {
    res = LzmaEnc_WriteProperties(p, propsEncoded, propsSize);
    if (res == SZ_OK)
      res = LzmaEnc_MemEncode(p, dest, destLen, src, srcLen,
          writeEndMark, progress, alloc, allocBig);
  }

  LzmaEnc_Destroy(p, alloc, allocBig);
  return res;
}





/* LzmaDec.c -- LZMA Decoder
2018-07-04 : Igor Pavlov : Public domain */

#include <string.h>

/* #include "CpuArch.h" */
//#include "LzmaDec.h"

#define kNumTopBits 24
#define kTopValue ((UInt32)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5

#define RC_INIT_SIZE 5

#define NORMALIZE if (range < kTopValue) { range <<= 8; code = (code << 8) | (*buf++); }

#define IF_BIT_0(p) ttt = *(p); NORMALIZE; bound = (range >> kNumBitModelTotalBits) * (UInt32)ttt; if (code < bound)
#define UPDATE_0(p) range = bound; *(p) = (CLzmaProb)(ttt + ((kBitModelTotal - ttt) >> kNumMoveBits));
#define UPDATE_1(p) range -= bound; code -= bound; *(p) = (CLzmaProb)(ttt - (ttt >> kNumMoveBits));
#define GET_BIT2(p, i, A0, A1) IF_BIT_0(p) \
  { UPDATE_0(p); i = (i + i); A0; } else \
  { UPDATE_1(p); i = (i + i) + 1; A1; }

#define TREE_GET_BIT(probs, i) { GET_BIT2(probs + i, i, ;, ;); }

#define REV_BIT(p, i, A0, A1) IF_BIT_0(p + i) \
  { UPDATE_0(p + i); A0; } else \
  { UPDATE_1(p + i); A1; }
#define REV_BIT_VAR(  p, i, m) REV_BIT(p, i, i += m; m += m, m += m; i += m; )
#define REV_BIT_CONST(p, i, m) REV_BIT(p, i, i += m;       , i += m * 2; )
#define REV_BIT_LAST( p, i, m) REV_BIT(p, i, i -= m        , ; )

#define TREE_DECODE(probs, limit, i) \
  { i = 1; do { TREE_GET_BIT(probs, i); } while (i < limit); i -= limit; }

/* #define _LZMA_SIZE_OPT */

#ifdef _LZMA_SIZE_OPT
#define TREE_6_DECODE(probs, i) TREE_DECODE(probs, (1 << 6), i)
#else
#define TREE_6_DECODE(probs, i) \
  { i = 1; \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  i -= 0x40; }
#endif

#define NORMAL_LITER_DEC TREE_GET_BIT(prob, symbol)
#define MATCHED_LITER_DEC \
  matchByte += matchByte; \
  bit = offs; \
  offs &= matchByte; \
  probLit = prob + (offs + bit + symbol); \
  GET_BIT2(probLit, symbol, offs ^= bit; , ;)



#define NORMALIZE_CHECK if (range < kTopValue) { if (buf >= bufLimit) return DUMMY_ERROR; range <<= 8; code = (code << 8) | (*buf++); }

#define IF_BIT_0_CHECK(p) ttt = *(p); NORMALIZE_CHECK; bound = (range >> kNumBitModelTotalBits) * (UInt32)ttt; if (code < bound)
#define UPDATE_0_CHECK range = bound;
#define UPDATE_1_CHECK range -= bound; code -= bound;
#define GET_BIT2_CHECK(p, i, A0, A1) IF_BIT_0_CHECK(p) \
  { UPDATE_0_CHECK; i = (i + i); A0; } else \
  { UPDATE_1_CHECK; i = (i + i) + 1; A1; }
#define GET_BIT_CHECK(p, i) GET_BIT2_CHECK(p, i, ; , ;)
#define TREE_DECODE_CHECK(probs, limit, i) \
  { i = 1; do { GET_BIT_CHECK(probs + i, i) } while (i < limit); i -= limit; }


#define REV_BIT_CHECK(p, i, m) IF_BIT_0_CHECK(p + i) \
  { UPDATE_0_CHECK; i += m; m += m; } else \
  { UPDATE_1_CHECK; m += m; i += m; }


#define kNumPosBitsMax 4
#define kNumPosStatesMax (1 << kNumPosBitsMax)

#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define LenLow 0
#define LenHigh (LenLow + 2 * (kNumPosStatesMax << kLenNumLowBits))
#define kNumLenProbs (LenHigh + kLenNumHighSymbols)

#define LenChoice LenLow
#define LenChoice2 (LenLow + (1 << kLenNumLowBits))

#define kNumStates 12
#define kNumStates2 16
#define kNumLitStates 7

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

#define kNumPosSlotBits 6
#define kNumLenToPosStates 4

#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)

#define kMatchMinLen 2
#define kMatchSpecLenStart (kMatchMinLen + kLenNumLowSymbols * 2 + kLenNumHighSymbols)

/* External ASM code needs same CLzmaProb array layout. So don't change it. */

/* (probs_1664) is faster and better for code size at some platforms */
/*
#ifdef MY_CPU_X86_OR_AMD64
*/
#define kStartOffset 1664
#define GET_PROBS p->probs_1664
/*
#define GET_PROBS p->probs + kStartOffset
#else
#define kStartOffset 0
#define GET_PROBS p->probs
#endif
*/

#define SpecPos (-kStartOffset)
#define IsRep0Long (SpecPos + kNumFullDistances)
#define RepLenCoder (IsRep0Long + (kNumStates2 << kNumPosBitsMax))
#define LenCoder (RepLenCoder + kNumLenProbs)
#define IsMatch (LenCoder + kNumLenProbs)
#define Align (IsMatch + (kNumStates2 << kNumPosBitsMax))
#define IsRep (Align + kAlignTableSize)
#define IsRepG0 (IsRep + kNumStates)
#define IsRepG1 (IsRepG0 + kNumStates)
#define IsRepG2 (IsRepG1 + kNumStates)
#define PosSlot (IsRepG2 + kNumStates)
#define Literal (PosSlot + (kNumLenToPosStates << kNumPosSlotBits))
#define NUM_BASE_PROBS (Literal + kStartOffset)

#if Align != 0 && kStartOffset != 0
  #error Stop_Compiling_Bad_LZMA_kAlign
#endif

#if NUM_BASE_PROBS != 1984
  #error Stop_Compiling_Bad_LZMA_PROBS
#endif


#define LZMA_LIT_SIZE 0x300

#define LzmaProps_GetNumProbs(p) (NUM_BASE_PROBS + ((UInt32)LZMA_LIT_SIZE << ((p)->lc + (p)->lp)))


#define CALC_POS_STATE(processedPos, pbMask) (((processedPos) & (pbMask)) << 4)
#define COMBINED_PS_STATE (posState + state)
#define GET_LEN_STATE (posState)

#define LZMA_DIC_MIN (1 << 12)

/*
p->remainLen : shows status of LZMA decoder:
    < kMatchSpecLenStart : normal remain
    = kMatchSpecLenStart : finished
    = kMatchSpecLenStart + 1 : need init range coder
    = kMatchSpecLenStart + 2 : need init range coder and state
*/

/* ---------- LZMA_DECODE_REAL ---------- */
/*
LzmaDec_DecodeReal_3() can be implemented in external ASM file.
3 - is the code compatibility version of that function for check at link time.
*/

#define LZMA_DECODE_REAL LzmaDec_DecodeReal_3

/*
LZMA_DECODE_REAL()
In:
  RangeCoder is normalized
  if (p->dicPos == limit)
  {
    LzmaDec_TryDummy() was called before to exclude LITERAL and MATCH-REP cases.
    So first symbol can be only MATCH-NON-REP. And if that MATCH-NON-REP symbol
    is not END_OF_PAYALOAD_MARKER, then function returns error code.
  }

Processing:
  first LZMA symbol will be decoded in any case
  All checks for limits are at the end of main loop,
  It will decode new LZMA-symbols while (p->buf < bufLimit && dicPos < limit),
  RangeCoder is still without last normalization when (p->buf < bufLimit) is being checked.

Out:
  RangeCoder is normalized
  Result:
    SZ_OK - OK
    SZ_ERROR_DATA - Error
  p->remainLen:
    < kMatchSpecLenStart : normal remain
    = kMatchSpecLenStart : finished
*/


#ifdef _LZMA_DEC_OPT

int MY_FAST_CALL LZMA_DECODE_REAL(CLzmaDec *p, size_t limit, const Byte *bufLimit);

#else

static
int MY_FAST_CALL LZMA_DECODE_REAL(CLzmaDec *p, size_t limit, const Byte *bufLimit)
{
  CLzmaProb *probs = GET_PROBS;
  unsigned state = (unsigned)p->state;
  UInt32 rep0 = p->reps[0], rep1 = p->reps[1], rep2 = p->reps[2], rep3 = p->reps[3];
  unsigned pbMask = ((unsigned)1 << (p->prop.pb)) - 1;
  unsigned lc = p->prop.lc;
  unsigned lpMask = ((unsigned)0x100 << p->prop.lp) - ((unsigned)0x100 >> lc);

  Byte *dic = p->dic;
  size_t dicBufSize = p->dicBufSize;
  size_t dicPos = p->dicPos;

  UInt32 processedPos = p->processedPos;
  UInt32 checkDicSize = p->checkDicSize;
  unsigned len = 0;

  const Byte *buf = p->buf;
  UInt32 range = p->range;
  UInt32 code = p->code;

  do
  {
    CLzmaProb *prob;
    UInt32 bound;
    unsigned ttt;
    unsigned posState = CALC_POS_STATE(processedPos, pbMask);

    prob = probs + IsMatch + COMBINED_PS_STATE;
    IF_BIT_0(prob)
    {
      unsigned symbol;
      UPDATE_0(prob);
      prob = probs + Literal;
      if (processedPos != 0 || checkDicSize != 0)
        prob += (UInt32)3 * ((((processedPos << 8) + dic[(dicPos == 0 ? dicBufSize : dicPos) - 1]) & lpMask) << lc);
      processedPos++;

      if (state < kNumLitStates)
      {
        state -= (state < 4) ? state : 3;
        symbol = 1;
        #ifdef _LZMA_SIZE_OPT
        do { NORMAL_LITER_DEC } while (symbol < 0x100);
        #else
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        NORMAL_LITER_DEC
        #endif
      }
      else
      {
        unsigned matchByte = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
        unsigned offs = 0x100;
        state -= (state < 10) ? 3 : 6;
        symbol = 1;
        #ifdef _LZMA_SIZE_OPT
        do
        {
          unsigned bit;
          CLzmaProb *probLit;
          MATCHED_LITER_DEC
        }
        while (symbol < 0x100);
        #else
        {
          unsigned bit;
          CLzmaProb *probLit;
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
          MATCHED_LITER_DEC
        }
        #endif
      }

      dic[dicPos++] = (Byte)symbol;
      continue;
    }

    {
      UPDATE_1(prob);
      prob = probs + IsRep + state;
      IF_BIT_0(prob)
      {
        UPDATE_0(prob);
        state += kNumStates;
        prob = probs + LenCoder;
      }
      else
      {
        UPDATE_1(prob);
        /*
        // that case was checked before with kBadRepCode
        if (checkDicSize == 0 && processedPos == 0)
          return SZ_ERROR_DATA;
        */
        prob = probs + IsRepG0 + state;
        IF_BIT_0(prob)
        {
          UPDATE_0(prob);
          prob = probs + IsRep0Long + COMBINED_PS_STATE;
          IF_BIT_0(prob)
          {
            UPDATE_0(prob);
            dic[dicPos] = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
            dicPos++;
            processedPos++;
            state = state < kNumLitStates ? 9 : 11;
            continue;
          }
          UPDATE_1(prob);
        }
        else
        {
          UInt32 distance;
          UPDATE_1(prob);
          prob = probs + IsRepG1 + state;
          IF_BIT_0(prob)
          {
            UPDATE_0(prob);
            distance = rep1;
          }
          else
          {
            UPDATE_1(prob);
            prob = probs + IsRepG2 + state;
            IF_BIT_0(prob)
            {
              UPDATE_0(prob);
              distance = rep2;
            }
            else
            {
              UPDATE_1(prob);
              distance = rep3;
              rep3 = rep2;
            }
            rep2 = rep1;
          }
          rep1 = rep0;
          rep0 = distance;
        }
        state = state < kNumLitStates ? 8 : 11;
        prob = probs + RepLenCoder;
      }

      #ifdef _LZMA_SIZE_OPT
      {
        unsigned lim, offset;
        CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0(probLen)
        {
          UPDATE_0(probLen);
          probLen = prob + LenLow + GET_LEN_STATE;
          offset = 0;
          lim = (1 << kLenNumLowBits);
        }
        else
        {
          UPDATE_1(probLen);
          probLen = prob + LenChoice2;
          IF_BIT_0(probLen)
          {
            UPDATE_0(probLen);
            probLen = prob + LenLow + GET_LEN_STATE + (1 << kLenNumLowBits);
            offset = kLenNumLowSymbols;
            lim = (1 << kLenNumLowBits);
          }
          else
          {
            UPDATE_1(probLen);
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols * 2;
            lim = (1 << kLenNumHighBits);
          }
        }
        TREE_DECODE(probLen, lim, len);
        len += offset;
      }
      #else
      {
        CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0(probLen)
        {
          UPDATE_0(probLen);
          probLen = prob + LenLow + GET_LEN_STATE;
          len = 1;
          TREE_GET_BIT(probLen, len);
          TREE_GET_BIT(probLen, len);
          TREE_GET_BIT(probLen, len);
          len -= 8;
        }
        else
        {
          UPDATE_1(probLen);
          probLen = prob + LenChoice2;
          IF_BIT_0(probLen)
          {
            UPDATE_0(probLen);
            probLen = prob + LenLow + GET_LEN_STATE + (1 << kLenNumLowBits);
            len = 1;
            TREE_GET_BIT(probLen, len);
            TREE_GET_BIT(probLen, len);
            TREE_GET_BIT(probLen, len);
          }
          else
          {
            UPDATE_1(probLen);
            probLen = prob + LenHigh;
            TREE_DECODE(probLen, (1 << kLenNumHighBits), len);
            len += kLenNumLowSymbols * 2;
          }
        }
      }
      #endif

      if (state >= kNumStates)
      {
        UInt32 distance;
        prob = probs + PosSlot +
            ((len < kNumLenToPosStates ? len : kNumLenToPosStates - 1) << kNumPosSlotBits);
        TREE_6_DECODE(prob, distance);
        if (distance >= kStartPosModelIndex)
        {
          unsigned posSlot = (unsigned)distance;
          unsigned numDirectBits = (unsigned)(((distance >> 1) - 1));
          distance = (2 | (distance & 1));
          if (posSlot < kEndPosModelIndex)
          {
            distance <<= numDirectBits;
            prob = probs + SpecPos;
            {
              UInt32 m = 1;
              distance++;
              do
              {
                REV_BIT_VAR(prob, distance, m);
              }
              while (--numDirectBits);
              distance -= m;
            }
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              NORMALIZE
              range >>= 1;

              {
                UInt32 t;
                code -= range;
                t = (0 - ((UInt32)code >> 31)); /* (UInt32)((Int32)code >> 31) */
                distance = (distance << 1) + (t + 1);
                code += range & t;
              }
              /*
              distance <<= 1;
              if (code >= range)
              {
                code -= range;
                distance |= 1;
              }
              */
            }
            while (--numDirectBits);
            prob = probs + Align;
            distance <<= kNumAlignBits;
            {
              unsigned i = 1;
              REV_BIT_CONST(prob, i, 1);
              REV_BIT_CONST(prob, i, 2);
              REV_BIT_CONST(prob, i, 4);
              REV_BIT_LAST (prob, i, 8);
              distance |= i;
            }
            if (distance == (UInt32)0xFFFFFFFF)
            {
              len = kMatchSpecLenStart;
              state -= kNumStates;
              break;
            }
          }
        }

        rep3 = rep2;
        rep2 = rep1;
        rep1 = rep0;
        rep0 = distance + 1;
        state = (state < kNumStates + kNumLitStates) ? kNumLitStates : kNumLitStates + 3;
        if (distance >= (checkDicSize == 0 ? processedPos: checkDicSize))
        {
          p->dicPos = dicPos;
          return SZ_ERROR_DATA;
        }
      }

      len += kMatchMinLen;

      {
        size_t rem;
        unsigned curLen;
        size_t pos;

        if ((rem = limit - dicPos) == 0)
        {
          p->dicPos = dicPos;
          return SZ_ERROR_DATA;
        }

        curLen = ((rem < len) ? (unsigned)rem : len);
        pos = dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0);

        processedPos += (UInt32)curLen;

        len -= curLen;
        if (curLen <= dicBufSize - pos)
        {
          Byte *dest = dic + dicPos;
          ptrdiff_t src = (ptrdiff_t)pos - (ptrdiff_t)dicPos;
          const Byte *lim = dest + curLen;
          dicPos += (size_t)curLen;
          do
            *(dest) = (Byte)*(dest + src);
          while (++dest != lim);
        }
        else
        {
          do
          {
            dic[dicPos++] = dic[pos];
            if (++pos == dicBufSize)
              pos = 0;
          }
          while (--curLen != 0);
        }
      }
    }
  }
  while (dicPos < limit && buf < bufLimit);

  NORMALIZE;

  p->buf = buf;
  p->range = range;
  p->code = code;
  p->remainLen = (UInt32)len;
  p->dicPos = dicPos;
  p->processedPos = processedPos;
  p->reps[0] = rep0;
  p->reps[1] = rep1;
  p->reps[2] = rep2;
  p->reps[3] = rep3;
  p->state = (UInt32)state;

  return SZ_OK;
}
#endif

static void MY_FAST_CALL LzmaDec_WriteRem(CLzmaDec *p, size_t limit)
{
  if (p->remainLen != 0 && p->remainLen < kMatchSpecLenStart)
  {
    Byte *dic = p->dic;
    size_t dicPos = p->dicPos;
    size_t dicBufSize = p->dicBufSize;
    unsigned len = (unsigned)p->remainLen;
    size_t rep0 = p->reps[0]; /* we use size_t to avoid the BUG of VC14 for AMD64 */
    size_t rem = limit - dicPos;
    if (rem < len)
      len = (unsigned)(rem);

    if (p->checkDicSize == 0 && p->prop.dicSize - p->processedPos <= len)
      p->checkDicSize = p->prop.dicSize;

    p->processedPos += (UInt32)len;
    p->remainLen -= (UInt32)len;
    while (len != 0)
    {
      len--;
      dic[dicPos] = dic[dicPos - rep0 + (dicPos < rep0 ? dicBufSize : 0)];
      dicPos++;
    }
    p->dicPos = dicPos;
  }
}


#define kRange0 0xFFFFFFFF
#define kBound0 ((kRange0 >> kNumBitModelTotalBits) << (kNumBitModelTotalBits - 1))
#define kBadRepCode (kBound0 + (((kRange0 - kBound0) >> kNumBitModelTotalBits) << (kNumBitModelTotalBits - 1)))
#if kBadRepCode != (0xC0000000 - 0x400)
  #error Stop_Compiling_Bad_LZMA_Check
#endif

static int MY_FAST_CALL LzmaDec_DecodeReal2(CLzmaDec *p, size_t limit, const Byte *bufLimit)
{
  do
  {
    size_t limit2 = limit;
    if (p->checkDicSize == 0)
    {
      UInt32 rem = p->prop.dicSize - p->processedPos;
      if (limit - p->dicPos > rem)
        limit2 = p->dicPos + rem;

      if (p->processedPos == 0)
        if (p->code >= kBadRepCode)
          return SZ_ERROR_DATA;
    }

    RINOK(LZMA_DECODE_REAL(p, limit2, bufLimit));

    if (p->checkDicSize == 0 && p->processedPos >= p->prop.dicSize)
      p->checkDicSize = p->prop.dicSize;

    LzmaDec_WriteRem(p, limit);
  }
  while (p->dicPos < limit && p->buf < bufLimit && p->remainLen < kMatchSpecLenStart);

  return 0;
}

typedef enum
{
  DUMMY_ERROR, /* unexpected end of input stream */
  DUMMY_LIT,
  DUMMY_MATCH,
  DUMMY_REP
} ELzmaDummy;

static ELzmaDummy LzmaDec_TryDummy(const CLzmaDec *p, const Byte *buf, size_t inSize)
{
  UInt32 range = p->range;
  UInt32 code = p->code;
  const Byte *bufLimit = buf + inSize;
  const CLzmaProb *probs = GET_PROBS;
  unsigned state = (unsigned)p->state;
  ELzmaDummy res;

  {
    const CLzmaProb *prob;
    UInt32 bound;
    unsigned ttt;
    unsigned posState = CALC_POS_STATE(p->processedPos, (1 << p->prop.pb) - 1);

    prob = probs + IsMatch + COMBINED_PS_STATE;
    IF_BIT_0_CHECK(prob)
    {
      UPDATE_0_CHECK

      /* if (bufLimit - buf >= 7) return DUMMY_LIT; */

      prob = probs + Literal;
      if (p->checkDicSize != 0 || p->processedPos != 0)
        prob += ((UInt32)LZMA_LIT_SIZE *
            ((((p->processedPos) & ((1 << (p->prop.lp)) - 1)) << p->prop.lc) +
            (p->dic[(p->dicPos == 0 ? p->dicBufSize : p->dicPos) - 1] >> (8 - p->prop.lc))));

      if (state < kNumLitStates)
      {
        unsigned symbol = 1;
        do { GET_BIT_CHECK(prob + symbol, symbol) } while (symbol < 0x100);
      }
      else
      {
        unsigned matchByte = p->dic[p->dicPos - p->reps[0] +
            (p->dicPos < p->reps[0] ? p->dicBufSize : 0)];
        unsigned offs = 0x100;
        unsigned symbol = 1;
        do
        {
          unsigned bit;
          const CLzmaProb *probLit;
          matchByte += matchByte;
          bit = offs;
          offs &= matchByte;
          probLit = prob + (offs + bit + symbol);
          GET_BIT2_CHECK(probLit, symbol, offs ^= bit; , ; )
        }
        while (symbol < 0x100);
      }
      res = DUMMY_LIT;
    }
    else
    {
      unsigned len;
      UPDATE_1_CHECK;

      prob = probs + IsRep + state;
      IF_BIT_0_CHECK(prob)
      {
        UPDATE_0_CHECK;
        state = 0;
        prob = probs + LenCoder;
        res = DUMMY_MATCH;
      }
      else
      {
        UPDATE_1_CHECK;
        res = DUMMY_REP;
        prob = probs + IsRepG0 + state;
        IF_BIT_0_CHECK(prob)
        {
          UPDATE_0_CHECK;
          prob = probs + IsRep0Long + COMBINED_PS_STATE;
          IF_BIT_0_CHECK(prob)
          {
            UPDATE_0_CHECK;
            NORMALIZE_CHECK;
            return DUMMY_REP;
          }
          else
          {
            UPDATE_1_CHECK;
          }
        }
        else
        {
          UPDATE_1_CHECK;
          prob = probs + IsRepG1 + state;
          IF_BIT_0_CHECK(prob)
          {
            UPDATE_0_CHECK;
          }
          else
          {
            UPDATE_1_CHECK;
            prob = probs + IsRepG2 + state;
            IF_BIT_0_CHECK(prob)
            {
              UPDATE_0_CHECK;
            }
            else
            {
              UPDATE_1_CHECK;
            }
          }
        }
        state = kNumStates;
        prob = probs + RepLenCoder;
      }
      {
        unsigned limit, offset;
        const CLzmaProb *probLen = prob + LenChoice;
        IF_BIT_0_CHECK(probLen)
        {
          UPDATE_0_CHECK;
          probLen = prob + LenLow + GET_LEN_STATE;
          offset = 0;
          limit = 1 << kLenNumLowBits;
        }
        else
        {
          UPDATE_1_CHECK;
          probLen = prob + LenChoice2;
          IF_BIT_0_CHECK(probLen)
          {
            UPDATE_0_CHECK;
            probLen = prob + LenLow + GET_LEN_STATE + (1 << kLenNumLowBits);
            offset = kLenNumLowSymbols;
            limit = 1 << kLenNumLowBits;
          }
          else
          {
            UPDATE_1_CHECK;
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols * 2;
            limit = 1 << kLenNumHighBits;
          }
        }
        TREE_DECODE_CHECK(probLen, limit, len);
        len += offset;
      }

      if (state < 4)
      {
        unsigned posSlot;
        prob = probs + PosSlot +
            ((len < kNumLenToPosStates - 1 ? len : kNumLenToPosStates - 1) <<
            kNumPosSlotBits);
        TREE_DECODE_CHECK(prob, 1 << kNumPosSlotBits, posSlot);
        if (posSlot >= kStartPosModelIndex)
        {
          unsigned numDirectBits = ((posSlot >> 1) - 1);

          /* if (bufLimit - buf >= 8) return DUMMY_MATCH; */

          if (posSlot < kEndPosModelIndex)
          {
            prob = probs + SpecPos + ((2 | (posSlot & 1)) << numDirectBits);
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              NORMALIZE_CHECK
              range >>= 1;
              code -= range & (((code - range) >> 31) - 1);
              /* if (code >= range) code -= range; */
            }
            while (--numDirectBits);
            prob = probs + Align;
            numDirectBits = kNumAlignBits;
          }
          {
            unsigned i = 1;
            unsigned m = 1;
            do
            {
              REV_BIT_CHECK(prob, i, m);
            }
            while (--numDirectBits);
          }
        }
      }
    }
  }
  NORMALIZE_CHECK;
  return res;
}


void LzmaDec_InitDicAndState(CLzmaDec *p, BoolInt initDic, BoolInt initState)
{
  p->remainLen = kMatchSpecLenStart + 1;
  p->tempBufSize = 0;

  if (initDic)
  {
    p->processedPos = 0;
    p->checkDicSize = 0;
    p->remainLen = kMatchSpecLenStart + 2;
  }
  if (initState)
    p->remainLen = kMatchSpecLenStart + 2;
}

void LzmaDec_Init(CLzmaDec *p)
{
  p->dicPos = 0;
  LzmaDec_InitDicAndState(p, True, True);
}


SRes LzmaDec_DecodeToDic(CLzmaDec *p, size_t dicLimit, const Byte *src, size_t *srcLen,
    ELzmaFinishMode finishMode, ELzmaStatus *status)
{
  size_t inSize = *srcLen;
  (*srcLen) = 0;

  *status = LZMA_STATUS_NOT_SPECIFIED;

  if (p->remainLen > kMatchSpecLenStart)
  {
    for (; inSize > 0 && p->tempBufSize < RC_INIT_SIZE; (*srcLen)++, inSize--)
      p->tempBuf[p->tempBufSize++] = *src++;
    if (p->tempBufSize != 0 && p->tempBuf[0] != 0)
      return SZ_ERROR_DATA;
    if (p->tempBufSize < RC_INIT_SIZE)
    {
      *status = LZMA_STATUS_NEEDS_MORE_INPUT;
      return SZ_OK;
    }
    p->code =
        ((UInt32)p->tempBuf[1] << 24)
      | ((UInt32)p->tempBuf[2] << 16)
      | ((UInt32)p->tempBuf[3] << 8)
      | ((UInt32)p->tempBuf[4]);
    p->range = 0xFFFFFFFF;
    p->tempBufSize = 0;

    if (p->remainLen > kMatchSpecLenStart + 1)
    {
      size_t numProbs = LzmaProps_GetNumProbs(&p->prop);
      size_t i;
      CLzmaProb *probs = p->probs;
      for (i = 0; i < numProbs; i++)
        probs[i] = kBitModelTotal >> 1;
      p->reps[0] = p->reps[1] = p->reps[2] = p->reps[3] = 1;
      p->state = 0;
    }

    p->remainLen = 0;
  }

  LzmaDec_WriteRem(p, dicLimit);

  while (p->remainLen != kMatchSpecLenStart)
  {
      int checkEndMarkNow = 0;

      if (p->dicPos >= dicLimit)
      {
        if (p->remainLen == 0 && p->code == 0)
        {
          *status = LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK;
          return SZ_OK;
        }
        if (finishMode == LZMA_FINISH_ANY)
        {
          *status = LZMA_STATUS_NOT_FINISHED;
          return SZ_OK;
        }
        if (p->remainLen != 0)
        {
          *status = LZMA_STATUS_NOT_FINISHED;
          return SZ_ERROR_DATA;
        }
        checkEndMarkNow = 1;
      }

      if (p->tempBufSize == 0)
      {
        size_t processed;
        const Byte *bufLimit;
        if (inSize < LZMA_REQUIRED_INPUT_MAX || checkEndMarkNow)
        {
          int dummyRes = LzmaDec_TryDummy(p, src, inSize);
          if (dummyRes == DUMMY_ERROR)
          {
            memcpy(p->tempBuf, src, inSize);
            p->tempBufSize = (unsigned)inSize;
            (*srcLen) += inSize;
            *status = LZMA_STATUS_NEEDS_MORE_INPUT;
            return SZ_OK;
          }
          if (checkEndMarkNow && dummyRes != DUMMY_MATCH)
          {
            *status = LZMA_STATUS_NOT_FINISHED;
            return SZ_ERROR_DATA;
          }
          bufLimit = src;
        }
        else
          bufLimit = src + inSize - LZMA_REQUIRED_INPUT_MAX;
        p->buf = src;
        if (LzmaDec_DecodeReal2(p, dicLimit, bufLimit) != 0)
          return SZ_ERROR_DATA;
        processed = (size_t)(p->buf - src);
        (*srcLen) += processed;
        src += processed;
        inSize -= processed;
      }
      else
      {
        unsigned rem = p->tempBufSize, lookAhead = 0;
        while (rem < LZMA_REQUIRED_INPUT_MAX && lookAhead < inSize)
          p->tempBuf[rem++] = src[lookAhead++];
        p->tempBufSize = rem;
        if (rem < LZMA_REQUIRED_INPUT_MAX || checkEndMarkNow)
        {
          int dummyRes = LzmaDec_TryDummy(p, p->tempBuf, (size_t)rem);
          if (dummyRes == DUMMY_ERROR)
          {
            (*srcLen) += (size_t)lookAhead;
            *status = LZMA_STATUS_NEEDS_MORE_INPUT;
            return SZ_OK;
          }
          if (checkEndMarkNow && dummyRes != DUMMY_MATCH)
          {
            *status = LZMA_STATUS_NOT_FINISHED;
            return SZ_ERROR_DATA;
          }
        }
        p->buf = p->tempBuf;
        if (LzmaDec_DecodeReal2(p, dicLimit, p->buf) != 0)
          return SZ_ERROR_DATA;

        {
          unsigned kkk = (unsigned)(p->buf - p->tempBuf);
          if (rem < kkk)
            return SZ_ERROR_FAIL; /* some internal error */
          rem -= kkk;
          if (lookAhead < rem)
            return SZ_ERROR_FAIL; /* some internal error */
          lookAhead -= rem;
        }
        (*srcLen) += (size_t)lookAhead;
        src += lookAhead;
        inSize -= (size_t)lookAhead;
        p->tempBufSize = 0;
      }
  }

  if (p->code != 0)
    return SZ_ERROR_DATA;
  *status = LZMA_STATUS_FINISHED_WITH_MARK;
  return SZ_OK;
}


SRes LzmaDec_DecodeToBuf(CLzmaDec *p, Byte *dest, size_t *destLen, const Byte *src, size_t *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status)
{
  size_t outSize = *destLen;
  size_t inSize = *srcLen;
  *srcLen = *destLen = 0;
  for (;;)
  {
    size_t inSizeCur = inSize, outSizeCur, dicPos;
    ELzmaFinishMode curFinishMode;
    SRes res;
    if (p->dicPos == p->dicBufSize)
      p->dicPos = 0;
    dicPos = p->dicPos;
    if (outSize > p->dicBufSize - dicPos)
    {
      outSizeCur = p->dicBufSize;
      curFinishMode = LZMA_FINISH_ANY;
    }
    else
    {
      outSizeCur = dicPos + outSize;
      curFinishMode = finishMode;
    }

    res = LzmaDec_DecodeToDic(p, outSizeCur, src, &inSizeCur, curFinishMode, status);
    src += inSizeCur;
    inSize -= inSizeCur;
    *srcLen += inSizeCur;
    outSizeCur = p->dicPos - dicPos;
    memcpy(dest, p->dic + dicPos, outSizeCur);
    dest += outSizeCur;
    outSize -= outSizeCur;
    *destLen += outSizeCur;
    if (res != 0)
      return res;
    if (outSizeCur == 0 || outSize == 0)
      return SZ_OK;
  }
}

void LzmaDec_FreeProbs(CLzmaDec *p, ISzAllocPtr alloc)
{
  ISzAlloc_Free(alloc, p->probs);
  p->probs = NULL;
}

static void LzmaDec_FreeDict(CLzmaDec *p, ISzAllocPtr alloc)
{
  ISzAlloc_Free(alloc, p->dic);
  p->dic = NULL;
}

void LzmaDec_Free(CLzmaDec *p, ISzAllocPtr alloc)
{
  LzmaDec_FreeProbs(p, alloc);
  LzmaDec_FreeDict(p, alloc);
}

SRes LzmaProps_Decode(CLzmaProps *p, const Byte *data, unsigned size)
{
  UInt32 dicSize;
  Byte d;

  if (size < LZMA_PROPS_SIZE)
    return SZ_ERROR_UNSUPPORTED;
  else
    dicSize = data[1] | ((UInt32)data[2] << 8) | ((UInt32)data[3] << 16) | ((UInt32)data[4] << 24);

  if (dicSize < LZMA_DIC_MIN)
    dicSize = LZMA_DIC_MIN;
  p->dicSize = dicSize;

  d = data[0];
  if (d >= (9 * 5 * 5))
    return SZ_ERROR_UNSUPPORTED;

  p->lc = (Byte)(d % 9);
  d /= 9;
  p->pb = (Byte)(d / 5);
  p->lp = (Byte)(d % 5);

  return SZ_OK;
}

static SRes LzmaDec_AllocateProbs2(CLzmaDec *p, const CLzmaProps *propNew, ISzAllocPtr alloc)
{
  UInt32 numProbs = LzmaProps_GetNumProbs(propNew);
  if (!p->probs || numProbs != p->numProbs)
  {
    LzmaDec_FreeProbs(p, alloc);
    p->probs = (CLzmaProb *)ISzAlloc_Alloc(alloc, numProbs * sizeof(CLzmaProb));
    if (!p->probs)
      return SZ_ERROR_MEM;
    p->probs_1664 = p->probs + 1664;
    p->numProbs = numProbs;
  }
  return SZ_OK;
}

SRes LzmaDec_AllocateProbs(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAllocPtr alloc)
{
  CLzmaProps propNew;
  RINOK(LzmaProps_Decode(&propNew, props, propsSize));
  RINOK(LzmaDec_AllocateProbs2(p, &propNew, alloc));
  p->prop = propNew;
  return SZ_OK;
}

SRes LzmaDec_Allocate(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAllocPtr alloc)
{
  CLzmaProps propNew;
  size_t dicBufSize;
  RINOK(LzmaProps_Decode(&propNew, props, propsSize));
  RINOK(LzmaDec_AllocateProbs2(p, &propNew, alloc));

  {
    UInt32 dictSize = propNew.dicSize;
    size_t mask = ((UInt32)1 << 12) - 1;
         if (dictSize >= ((UInt32)1 << 30)) mask = ((UInt32)1 << 22) - 1;
    else if (dictSize >= ((UInt32)1 << 22)) mask = ((UInt32)1 << 20) - 1;;
    dicBufSize = ((size_t)dictSize + mask) & ~mask;
    if (dicBufSize < dictSize)
      dicBufSize = dictSize;
  }

  if (!p->dic || dicBufSize != p->dicBufSize)
  {
    LzmaDec_FreeDict(p, alloc);
    p->dic = (Byte *)ISzAlloc_Alloc(alloc, dicBufSize);
    if (!p->dic)
    {
      LzmaDec_FreeProbs(p, alloc);
      return SZ_ERROR_MEM;
    }
  }
  p->dicBufSize = dicBufSize;
  p->prop = propNew;
  return SZ_OK;
}

SRes LzmaDecode(Byte *dest, size_t *destLen, const Byte *src, size_t *srcLen,
    const Byte *propData, unsigned propSize, ELzmaFinishMode finishMode,
    ELzmaStatus *status, ISzAllocPtr alloc)
{
  CLzmaDec p;
  SRes res;
  size_t outSize = *destLen, inSize = *srcLen;
  *destLen = *srcLen = 0;
  *status = LZMA_STATUS_NOT_SPECIFIED;
  if (inSize < RC_INIT_SIZE)
    return SZ_ERROR_INPUT_EOF;
  LzmaDec_Construct(&p);
  RINOK(LzmaDec_AllocateProbs(&p, propData, propSize, alloc));
  p.dic = dest;
  p.dicBufSize = outSize;
  LzmaDec_Init(&p);
  *srcLen = inSize;
  res = LzmaDec_DecodeToDic(&p, outSize, src, srcLen, finishMode, status);
  *destLen = p.dicPos;
  if (res == SZ_OK && *status == LZMA_STATUS_NEEDS_MORE_INPUT)
    res = SZ_ERROR_INPUT_EOF;
  LzmaDec_FreeProbs(&p, alloc);
  return res;
}





/* LzHash.h -- HASH functions for LZ algorithms
2015-04-12 : Igor Pavlov : Public domain */

#define kHash2Size (1 << 10)
#define kHash3Size (1 << 16)
#define kHash4Size (1 << 20)

#define kFix3HashSize (kHash2Size)
#define kFix4HashSize (kHash2Size + kHash3Size)
#define kFix5HashSize (kHash2Size + kHash3Size + kHash4Size)

#define HASH2_CALC hv = cur[0] | ((UInt32)cur[1] << 8);

#define HASH3_CALC { \
  UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
  h2 = temp & (kHash2Size - 1); \
  hv = (temp ^ ((UInt32)cur[2] << 8)) & p->hashMask; }

#define HASH4_CALC { \
  UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
  h2 = temp & (kHash2Size - 1); \
  temp ^= ((UInt32)cur[2] << 8); \
  h3 = temp & (kHash3Size - 1); \
  hv = (temp ^ (p->crc[cur[3]] << 5)) & p->hashMask; }

#define HASH5_CALC { \
  UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
  h2 = temp & (kHash2Size - 1); \
  temp ^= ((UInt32)cur[2] << 8); \
  h3 = temp & (kHash3Size - 1); \
  temp ^= (p->crc[cur[3]] << 5); \
  h4 = temp & (kHash4Size - 1); \
  hv = (temp ^ (p->crc[cur[4]] << 3)) & p->hashMask; }

/* #define HASH_ZIP_CALC hv = ((cur[0] | ((UInt32)cur[1] << 8)) ^ p->crc[cur[2]]) & 0xFFFF; */
#define HASH_ZIP_CALC hv = ((cur[2] | ((UInt32)cur[0] << 8)) ^ p->crc[cur[1]]) & 0xFFFF;


#define MT_HASH2_CALC \
  h2 = (p->crc[cur[0]] ^ cur[1]) & (kHash2Size - 1);

#define MT_HASH3_CALC { \
  UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
  h2 = temp & (kHash2Size - 1); \
  h3 = (temp ^ ((UInt32)cur[2] << 8)) & (kHash3Size - 1); }

#define MT_HASH4_CALC { \
  UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
  h2 = temp & (kHash2Size - 1); \
  temp ^= ((UInt32)cur[2] << 8); \
  h3 = temp & (kHash3Size - 1); \
  h4 = (temp ^ (p->crc[cur[3]] << 5)) & (kHash4Size - 1); }





/* LzFind.c -- Match finder for LZ algorithms
2018-07-08 : Igor Pavlov : Public domain */

#include <string.h>

//#include "LzFind.h"
//#include "LzHash.h"

#define kEmptyHashValue 0
#define kMaxValForNormalize ((UInt32)0xFFFFFFFF)
#define kNormalizeStepMin (1 << 10) /* it must be power of 2 */
#define kNormalizeMask (~(UInt32)(kNormalizeStepMin - 1))
#define kMaxHistorySize ((UInt32)7 << 29)

#define kStartMaxLen 3

static void LzInWindow_Free(CMatchFinder *p, ISzAllocPtr alloc)
{
  if (!p->directInput)
  {
    ISzAlloc_Free(alloc, p->bufferBase);
    p->bufferBase = NULL;
  }
}

/* keepSizeBefore + keepSizeAfter + keepSizeReserv must be < 4G) */

static int LzInWindow_Create(CMatchFinder *p, UInt32 keepSizeReserv, ISzAllocPtr alloc)
{
  UInt32 blockSize = p->keepSizeBefore + p->keepSizeAfter + keepSizeReserv;
  if (p->directInput)
  {
    p->blockSize = blockSize;
    return 1;
  }
  if (!p->bufferBase || p->blockSize != blockSize)
  {
    LzInWindow_Free(p, alloc);
    p->blockSize = blockSize;
    p->bufferBase = (Byte *)ISzAlloc_Alloc(alloc, (size_t)blockSize);
  }
  return (p->bufferBase != NULL);
}

Byte *MatchFinder_GetPointerToCurrentPos(CMatchFinder *p) { return p->buffer; }

UInt32 MatchFinder_GetNumAvailableBytes(CMatchFinder *p) { return p->streamPos - p->pos; }

void MatchFinder_ReduceOffsets(CMatchFinder *p, UInt32 subValue)
{
  p->posLimit -= subValue;
  p->pos -= subValue;
  p->streamPos -= subValue;
}

static void MatchFinder_ReadBlock(CMatchFinder *p)
{
  if (p->streamEndWasReached || p->result != SZ_OK)
    return;

  /* We use (p->streamPos - p->pos) value. (p->streamPos < p->pos) is allowed. */

  if (p->directInput)
  {
    UInt32 curSize = 0xFFFFFFFF - (p->streamPos - p->pos);
    if (curSize > p->directInputRem)
      curSize = (UInt32)p->directInputRem;
    p->directInputRem -= curSize;
    p->streamPos += curSize;
    if (p->directInputRem == 0)
      p->streamEndWasReached = 1;
    return;
  }

  for (;;)
  {
    Byte *dest = p->buffer + (p->streamPos - p->pos);
    size_t size = (p->bufferBase + p->blockSize - dest);
    if (size == 0)
      return;

    p->result = ISeqInStream_Read(p->stream, dest, &size);
    if (p->result != SZ_OK)
      return;
    if (size == 0)
    {
      p->streamEndWasReached = 1;
      return;
    }
    p->streamPos += (UInt32)size;
    if (p->streamPos - p->pos > p->keepSizeAfter)
      return;
  }
}

void MatchFinder_MoveBlock(CMatchFinder *p)
{
  memmove(p->bufferBase,
      p->buffer - p->keepSizeBefore,
      (size_t)(p->streamPos - p->pos) + p->keepSizeBefore);
  p->buffer = p->bufferBase + p->keepSizeBefore;
}

int MatchFinder_NeedMove(CMatchFinder *p)
{
  if (p->directInput)
    return 0;
  /* if (p->streamEndWasReached) return 0; */
  return ((size_t)(p->bufferBase + p->blockSize - p->buffer) <= p->keepSizeAfter);
}

void MatchFinder_ReadIfRequired(CMatchFinder *p)
{
  if (p->streamEndWasReached)
    return;
  if (p->keepSizeAfter >= p->streamPos - p->pos)
    MatchFinder_ReadBlock(p);
}

static void MatchFinder_CheckAndMoveAndRead(CMatchFinder *p)
{
  if (MatchFinder_NeedMove(p))
    MatchFinder_MoveBlock(p);
  MatchFinder_ReadBlock(p);
}

static void MatchFinder_SetDefaultSettings(CMatchFinder *p)
{
  p->cutValue = 32;
  p->btMode = 1;
  p->numHashBytes = 4;
  p->bigHash = 0;
}

#define kCrcPoly 0xEDB88320

void MatchFinder_Construct(CMatchFinder *p)
{
  unsigned i;
  p->bufferBase = NULL;
  p->directInput = 0;
  p->hash = NULL;
  p->expectedDataSize = (UInt64)(Int64)-1;
  MatchFinder_SetDefaultSettings(p);

  for (i = 0; i < 256; i++)
  {
    UInt32 r = (UInt32)i;
    unsigned j;
    for (j = 0; j < 8; j++)
      r = (r >> 1) ^ (kCrcPoly & ((UInt32)0 - (r & 1)));
    p->crc[i] = r;
  }
}

static void MatchFinder_FreeThisClassMemory(CMatchFinder *p, ISzAllocPtr alloc)
{
  ISzAlloc_Free(alloc, p->hash);
  p->hash = NULL;
}

void MatchFinder_Free(CMatchFinder *p, ISzAllocPtr alloc)
{
  MatchFinder_FreeThisClassMemory(p, alloc);
  LzInWindow_Free(p, alloc);
}

static CLzRef* AllocRefs(size_t num, ISzAllocPtr alloc)
{
  size_t sizeInBytes = (size_t)num * sizeof(CLzRef);
  if (sizeInBytes / sizeof(CLzRef) != num)
    return NULL;
  return (CLzRef *)ISzAlloc_Alloc(alloc, sizeInBytes);
}

int MatchFinder_Create(CMatchFinder *p, UInt32 historySize,
    UInt32 keepAddBufferBefore, UInt32 matchMaxLen, UInt32 keepAddBufferAfter,
    ISzAllocPtr alloc)
{
  UInt32 sizeReserv;

  if (historySize > kMaxHistorySize)
  {
    MatchFinder_Free(p, alloc);
    return 0;
  }

  sizeReserv = historySize >> 1;
       if (historySize >= ((UInt32)3 << 30)) sizeReserv = historySize >> 3;
  else if (historySize >= ((UInt32)2 << 30)) sizeReserv = historySize >> 2;

  sizeReserv += (keepAddBufferBefore + matchMaxLen + keepAddBufferAfter) / 2 + (1 << 19);

  p->keepSizeBefore = historySize + keepAddBufferBefore + 1;
  p->keepSizeAfter = matchMaxLen + keepAddBufferAfter;

  /* we need one additional byte, since we use MoveBlock after pos++ and before dictionary using */

  if (LzInWindow_Create(p, sizeReserv, alloc))
  {
    UInt32 newCyclicBufferSize = historySize + 1;
    UInt32 hs;
    p->matchMaxLen = matchMaxLen;
    {
      p->fixedHashSize = 0;
      if (p->numHashBytes == 2)
        hs = (1 << 16) - 1;
      else
      {
        hs = historySize;
        if (hs > p->expectedDataSize)
          hs = (UInt32)p->expectedDataSize;
        if (hs != 0)
          hs--;
        hs |= (hs >> 1);
        hs |= (hs >> 2);
        hs |= (hs >> 4);
        hs |= (hs >> 8);
        hs >>= 1;
        hs |= 0xFFFF; /* don't change it! It's required for Deflate */
        if (hs > (1 << 24))
        {
          if (p->numHashBytes == 3)
            hs = (1 << 24) - 1;
          else
            hs >>= 1;
          /* if (bigHash) mode, GetHeads4b() in LzFindMt.c needs (hs >= ((1 << 24) - 1))) */
        }
      }
      p->hashMask = hs;
      hs++;
      if (p->numHashBytes > 2) p->fixedHashSize += kHash2Size;
      if (p->numHashBytes > 3) p->fixedHashSize += kHash3Size;
      if (p->numHashBytes > 4) p->fixedHashSize += kHash4Size;
      hs += p->fixedHashSize;
    }

    {
      size_t newSize;
      size_t numSons;
      p->historySize = historySize;
      p->hashSizeSum = hs;
      p->cyclicBufferSize = newCyclicBufferSize;

      numSons = newCyclicBufferSize;
      if (p->btMode)
        numSons <<= 1;
      newSize = hs + numSons;

      if (p->hash && p->numRefs == newSize)
        return 1;

      MatchFinder_FreeThisClassMemory(p, alloc);
      p->numRefs = newSize;
      p->hash = AllocRefs(newSize, alloc);

      if (p->hash)
      {
        p->son = p->hash + p->hashSizeSum;
        return 1;
      }
    }
  }

  MatchFinder_Free(p, alloc);
  return 0;
}

static void MatchFinder_SetLimits(CMatchFinder *p)
{
  UInt32 limit = kMaxValForNormalize - p->pos;
  UInt32 limit2 = p->cyclicBufferSize - p->cyclicBufferPos;

  if (limit2 < limit)
    limit = limit2;
  limit2 = p->streamPos - p->pos;

  if (limit2 <= p->keepSizeAfter)
  {
    if (limit2 > 0)
      limit2 = 1;
  }
  else
    limit2 -= p->keepSizeAfter;

  if (limit2 < limit)
    limit = limit2;

  {
    UInt32 lenLimit = p->streamPos - p->pos;
    if (lenLimit > p->matchMaxLen)
      lenLimit = p->matchMaxLen;
    p->lenLimit = lenLimit;
  }
  p->posLimit = p->pos + limit;
}


void MatchFinder_Init_LowHash(CMatchFinder *p)
{
  size_t i;
  CLzRef *items = p->hash;
  size_t numItems = p->fixedHashSize;
  for (i = 0; i < numItems; i++)
    items[i] = kEmptyHashValue;
}


void MatchFinder_Init_HighHash(CMatchFinder *p)
{
  size_t i;
  CLzRef *items = p->hash + p->fixedHashSize;
  size_t numItems = (size_t)p->hashMask + 1;
  for (i = 0; i < numItems; i++)
    items[i] = kEmptyHashValue;
}


void MatchFinder_Init_3(CMatchFinder *p, int readData)
{
  p->cyclicBufferPos = 0;
  p->buffer = p->bufferBase;
  p->pos =
  p->streamPos = p->cyclicBufferSize;
  p->result = SZ_OK;
  p->streamEndWasReached = 0;

  if (readData)
    MatchFinder_ReadBlock(p);

  MatchFinder_SetLimits(p);
}


void MatchFinder_Init(CMatchFinder *p)
{
  MatchFinder_Init_HighHash(p);
  MatchFinder_Init_LowHash(p);
  MatchFinder_Init_3(p, True);
}


static UInt32 MatchFinder_GetSubValue(CMatchFinder *p)
{
  return (p->pos - p->historySize - 1) & kNormalizeMask;
}

void MatchFinder_Normalize3(UInt32 subValue, CLzRef *items, size_t numItems)
{
  size_t i;
  for (i = 0; i < numItems; i++)
  {
    UInt32 value = items[i];
    if (value <= subValue)
      value = kEmptyHashValue;
    else
      value -= subValue;
    items[i] = value;
  }
}

static void MatchFinder_Normalize(CMatchFinder *p)
{
  UInt32 subValue = MatchFinder_GetSubValue(p);
  MatchFinder_Normalize3(subValue, p->hash, p->numRefs);
  MatchFinder_ReduceOffsets(p, subValue);
}


MY_NO_INLINE
static void MatchFinder_CheckLimits(CMatchFinder *p)
{
  if (p->pos == kMaxValForNormalize)
    MatchFinder_Normalize(p);
  if (!p->streamEndWasReached && p->keepSizeAfter == p->streamPos - p->pos)
    MatchFinder_CheckAndMoveAndRead(p);
  if (p->cyclicBufferPos == p->cyclicBufferSize)
    p->cyclicBufferPos = 0;
  MatchFinder_SetLimits(p);
}


/*
  (lenLimit > maxLen)
*/
MY_FORCE_INLINE
static UInt32 * Hc_GetMatchesSpec(unsigned lenLimit, UInt32 curMatch, UInt32 pos, const Byte *cur, CLzRef *son,
    UInt32 _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 cutValue,
    UInt32 *distances, unsigned maxLen)
{
  /*
  son[_cyclicBufferPos] = curMatch;
  for (;;)
  {
    UInt32 delta = pos - curMatch;
    if (cutValue-- == 0 || delta >= _cyclicBufferSize)
      return distances;
    {
      const Byte *pb = cur - delta;
      curMatch = son[_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)];
      if (pb[maxLen] == cur[maxLen] && *pb == *cur)
      {
        UInt32 len = 0;
        while (++len != lenLimit)
          if (pb[len] != cur[len])
            break;
        if (maxLen < len)
        {
          maxLen = len;
          *distances++ = len;
          *distances++ = delta - 1;
          if (len == lenLimit)
            return distances;
        }
      }
    }
  }
  */

  const Byte *lim = cur + lenLimit;
  son[_cyclicBufferPos] = curMatch;
  do
  {
    UInt32 delta = pos - curMatch;
    if (delta >= _cyclicBufferSize)
      break;
    {
      ptrdiff_t diff;
      curMatch = son[_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)];
      diff = (ptrdiff_t)0 - delta;
      if (cur[maxLen] == cur[maxLen + diff])
      {
        const Byte *c = cur;
        while (*c == c[diff])
        {
          if (++c == lim)
          {
            distances[0] = (UInt32)(lim - cur);
            distances[1] = delta - 1;
            return distances + 2;
          }
        }
        {
          unsigned len = (unsigned)(c - cur);
          if (maxLen < len)
          {
            maxLen = len;
            distances[0] = (UInt32)len;
            distances[1] = delta - 1;
            distances += 2;
          }
        }
      }
    }
  }
  while (--cutValue);

  return distances;
}


MY_FORCE_INLINE
UInt32 * GetMatchesSpec1(UInt32 lenLimit, UInt32 curMatch, UInt32 pos, const Byte *cur, CLzRef *son,
    UInt32 _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 cutValue,
    UInt32 *distances, UInt32 maxLen)
{
  CLzRef *ptr0 = son + ((size_t)_cyclicBufferPos << 1) + 1;
  CLzRef *ptr1 = son + ((size_t)_cyclicBufferPos << 1);
  unsigned len0 = 0, len1 = 0;
  for (;;)
  {
    UInt32 delta = pos - curMatch;
    if (cutValue-- == 0 || delta >= _cyclicBufferSize)
    {
      *ptr0 = *ptr1 = kEmptyHashValue;
      return distances;
    }
    {
      CLzRef *pair = son + ((size_t)(_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)) << 1);
      const Byte *pb = cur - delta;
      unsigned len = (len0 < len1 ? len0 : len1);
      UInt32 pair0 = pair[0];
      if (pb[len] == cur[len])
      {
        if (++len != lenLimit && pb[len] == cur[len])
          while (++len != lenLimit)
            if (pb[len] != cur[len])
              break;
        if (maxLen < len)
        {
          maxLen = (UInt32)len;
          *distances++ = (UInt32)len;
          *distances++ = delta - 1;
          if (len == lenLimit)
          {
            *ptr1 = pair0;
            *ptr0 = pair[1];
            return distances;
          }
        }
      }
      if (pb[len] < cur[len])
      {
        *ptr1 = curMatch;
        ptr1 = pair + 1;
        curMatch = *ptr1;
        len1 = len;
      }
      else
      {
        *ptr0 = curMatch;
        ptr0 = pair;
        curMatch = *ptr0;
        len0 = len;
      }
    }
  }
}

static void SkipMatchesSpec(UInt32 lenLimit, UInt32 curMatch, UInt32 pos, const Byte *cur, CLzRef *son,
    UInt32 _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 cutValue)
{
  CLzRef *ptr0 = son + ((size_t)_cyclicBufferPos << 1) + 1;
  CLzRef *ptr1 = son + ((size_t)_cyclicBufferPos << 1);
  unsigned len0 = 0, len1 = 0;
  for (;;)
  {
    UInt32 delta = pos - curMatch;
    if (cutValue-- == 0 || delta >= _cyclicBufferSize)
    {
      *ptr0 = *ptr1 = kEmptyHashValue;
      return;
    }
    {
      CLzRef *pair = son + ((size_t)(_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)) << 1);
      const Byte *pb = cur - delta;
      unsigned len = (len0 < len1 ? len0 : len1);
      if (pb[len] == cur[len])
      {
        while (++len != lenLimit)
          if (pb[len] != cur[len])
            break;
        {
          if (len == lenLimit)
          {
            *ptr1 = pair[0];
            *ptr0 = pair[1];
            return;
          }
        }
      }
      if (pb[len] < cur[len])
      {
        *ptr1 = curMatch;
        ptr1 = pair + 1;
        curMatch = *ptr1;
        len1 = len;
      }
      else
      {
        *ptr0 = curMatch;
        ptr0 = pair;
        curMatch = *ptr0;
        len0 = len;
      }
    }
  }
}

#ifdef MOVE_POS
#undef MOVE_POS
#endif

#define MOVE_POS \
  ++p->cyclicBufferPos; \
  p->buffer++; \
  if (++p->pos == p->posLimit) MatchFinder_CheckLimits(p);

#define MOVE_POS_RET MOVE_POS return (UInt32)offset;

static void MatchFinder_MovePos(CMatchFinder *p) { MOVE_POS; }

#define GET_MATCHES_HEADER2(minLen, ret_op) \
  unsigned lenLimit; UInt32 hv; const Byte *cur; UInt32 curMatch; \
  lenLimit = (unsigned)p->lenLimit; { if (lenLimit < minLen) { MatchFinder_MovePos(p); ret_op; }} \
  cur = p->buffer;

#define GET_MATCHES_HEADER(minLen) GET_MATCHES_HEADER2(minLen, return 0)
#define SKIP_HEADER(minLen)        GET_MATCHES_HEADER2(minLen, continue)

#define MF_PARAMS(p) p->pos, p->buffer, p->son, p->cyclicBufferPos, p->cyclicBufferSize, p->cutValue

#define GET_MATCHES_FOOTER(offset, maxLen) \
  offset = (unsigned)(GetMatchesSpec1((UInt32)lenLimit, curMatch, MF_PARAMS(p), \
  distances + offset, (UInt32)maxLen) - distances); MOVE_POS_RET;

#define SKIP_FOOTER \
  SkipMatchesSpec((UInt32)lenLimit, curMatch, MF_PARAMS(p)); MOVE_POS;

#define UPDATE_maxLen { \
    ptrdiff_t diff = (ptrdiff_t)0 - d2; \
    const Byte *c = cur + maxLen; \
    const Byte *lim = cur + lenLimit; \
    for (; c != lim; c++) if (*(c + diff) != *c) break; \
    maxLen = (unsigned)(c - cur); }

static UInt32 Bt2_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  unsigned offset;
  GET_MATCHES_HEADER(2)
  HASH2_CALC;
  curMatch = p->hash[hv];
  p->hash[hv] = p->pos;
  offset = 0;
  GET_MATCHES_FOOTER(offset, 1)
}

UInt32 Bt3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  unsigned offset;
  GET_MATCHES_HEADER(3)
  HASH_ZIP_CALC;
  curMatch = p->hash[hv];
  p->hash[hv] = p->pos;
  offset = 0;
  GET_MATCHES_FOOTER(offset, 2)
}

static UInt32 Bt3_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, d2, pos;
  unsigned maxLen, offset;
  UInt32 *hash;
  GET_MATCHES_HEADER(3)

  HASH3_CALC;

  hash = p->hash;
  pos = p->pos;

  d2 = pos - hash[h2];

  curMatch = (hash + kFix3HashSize)[hv];

  hash[h2] = pos;
  (hash + kFix3HashSize)[hv] = pos;

  maxLen = 2;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    UPDATE_maxLen
    distances[0] = (UInt32)maxLen;
    distances[1] = d2 - 1;
    offset = 2;
    if (maxLen == lenLimit)
    {
      SkipMatchesSpec((UInt32)lenLimit, curMatch, MF_PARAMS(p));
      MOVE_POS_RET;
    }
  }

  GET_MATCHES_FOOTER(offset, maxLen)
}

static UInt32 Bt4_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, h3, d2, d3, pos;
  unsigned maxLen, offset;
  UInt32 *hash;
  GET_MATCHES_HEADER(4)

  HASH4_CALC;

  hash = p->hash;
  pos = p->pos;

  d2 = pos - hash                  [h2];
  d3 = pos - (hash + kFix3HashSize)[h3];

  curMatch = (hash + kFix4HashSize)[hv];

  hash                  [h2] = pos;
  (hash + kFix3HashSize)[h3] = pos;
  (hash + kFix4HashSize)[hv] = pos;

  maxLen = 0;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    maxLen = 2;
    distances[0] = 2;
    distances[1] = d2 - 1;
    offset = 2;
  }

  if (d2 != d3 && d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
  {
    maxLen = 3;
    distances[(size_t)offset + 1] = d3 - 1;
    offset += 2;
    d2 = d3;
  }

  if (offset != 0)
  {
    UPDATE_maxLen
    distances[(size_t)offset - 2] = (UInt32)maxLen;
    if (maxLen == lenLimit)
    {
      SkipMatchesSpec((UInt32)lenLimit, curMatch, MF_PARAMS(p));
      MOVE_POS_RET;
    }
  }

  if (maxLen < 3)
    maxLen = 3;

  GET_MATCHES_FOOTER(offset, maxLen)
}

/*
static UInt32 Bt5_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, h3, h4, d2, d3, d4, maxLen, offset, pos;
  UInt32 *hash;
  GET_MATCHES_HEADER(5)

  HASH5_CALC;

  hash = p->hash;
  pos = p->pos;

  d2 = pos - hash                  [h2];
  d3 = pos - (hash + kFix3HashSize)[h3];
  d4 = pos - (hash + kFix4HashSize)[h4];

  curMatch = (hash + kFix5HashSize)[hv];

  hash                  [h2] = pos;
  (hash + kFix3HashSize)[h3] = pos;
  (hash + kFix4HashSize)[h4] = pos;
  (hash + kFix5HashSize)[hv] = pos;

  maxLen = 0;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    distances[0] = maxLen = 2;
    distances[1] = d2 - 1;
    offset = 2;
    if (*(cur - d2 + 2) == cur[2])
      distances[0] = maxLen = 3;
    else if (d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
    {
      distances[2] = maxLen = 3;
      distances[3] = d3 - 1;
      offset = 4;
      d2 = d3;
    }
  }
  else if (d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
  {
    distances[0] = maxLen = 3;
    distances[1] = d3 - 1;
    offset = 2;
    d2 = d3;
  }

  if (d2 != d4 && d4 < p->cyclicBufferSize
      && *(cur - d4) == *cur
      && *(cur - d4 + 3) == *(cur + 3))
  {
    maxLen = 4;
    distances[(size_t)offset + 1] = d4 - 1;
    offset += 2;
    d2 = d4;
  }

  if (offset != 0)
  {
    UPDATE_maxLen
    distances[(size_t)offset - 2] = maxLen;
    if (maxLen == lenLimit)
    {
      SkipMatchesSpec(lenLimit, curMatch, MF_PARAMS(p));
      MOVE_POS_RET;
    }
  }

  if (maxLen < 4)
    maxLen = 4;

  GET_MATCHES_FOOTER(offset, maxLen)
}
*/

static UInt32 Hc4_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, h3, d2, d3, pos;
  unsigned maxLen, offset;
  UInt32 *hash;
  GET_MATCHES_HEADER(4)

  HASH4_CALC;

  hash = p->hash;
  pos = p->pos;

  d2 = pos - hash                  [h2];
  d3 = pos - (hash + kFix3HashSize)[h3];
  curMatch = (hash + kFix4HashSize)[hv];

  hash                  [h2] = pos;
  (hash + kFix3HashSize)[h3] = pos;
  (hash + kFix4HashSize)[hv] = pos;

  maxLen = 0;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    maxLen = 2;
    distances[0] = 2;
    distances[1] = d2 - 1;
    offset = 2;
  }

  if (d2 != d3 && d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
  {
    maxLen = 3;
    distances[(size_t)offset + 1] = d3 - 1;
    offset += 2;
    d2 = d3;
  }

  if (offset != 0)
  {
    UPDATE_maxLen
    distances[(size_t)offset - 2] = (UInt32)maxLen;
    if (maxLen == lenLimit)
    {
      p->son[p->cyclicBufferPos] = curMatch;
      MOVE_POS_RET;
    }
  }

  if (maxLen < 3)
    maxLen = 3;

  offset = (unsigned)(Hc_GetMatchesSpec(lenLimit, curMatch, MF_PARAMS(p),
      distances + offset, maxLen) - (distances));
  MOVE_POS_RET
}

/*
static UInt32 Hc5_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  UInt32 h2, h3, h4, d2, d3, d4, maxLen, offset, pos
  UInt32 *hash;
  GET_MATCHES_HEADER(5)

  HASH5_CALC;

  hash = p->hash;
  pos = p->pos;

  d2 = pos - hash                  [h2];
  d3 = pos - (hash + kFix3HashSize)[h3];
  d4 = pos - (hash + kFix4HashSize)[h4];

  curMatch = (hash + kFix5HashSize)[hv];

  hash                  [h2] = pos;
  (hash + kFix3HashSize)[h3] = pos;
  (hash + kFix4HashSize)[h4] = pos;
  (hash + kFix5HashSize)[hv] = pos;

  maxLen = 0;
  offset = 0;

  if (d2 < p->cyclicBufferSize && *(cur - d2) == *cur)
  {
    distances[0] = maxLen = 2;
    distances[1] = d2 - 1;
    offset = 2;
    if (*(cur - d2 + 2) == cur[2])
      distances[0] = maxLen = 3;
    else if (d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
    {
      distances[2] = maxLen = 3;
      distances[3] = d3 - 1;
      offset = 4;
      d2 = d3;
    }
  }
  else if (d3 < p->cyclicBufferSize && *(cur - d3) == *cur)
  {
    distances[0] = maxLen = 3;
    distances[1] = d3 - 1;
    offset = 2;
    d2 = d3;
  }

  if (d2 != d4 && d4 < p->cyclicBufferSize
      && *(cur - d4) == *cur
      && *(cur - d4 + 3) == *(cur + 3))
  {
    maxLen = 4;
    distances[(size_t)offset + 1] = d4 - 1;
    offset += 2;
    d2 = d4;
  }

  if (offset != 0)
  {
    UPDATE_maxLen
    distances[(size_t)offset - 2] = maxLen;
    if (maxLen == lenLimit)
    {
      p->son[p->cyclicBufferPos] = curMatch;
      MOVE_POS_RET;
    }
  }

  if (maxLen < 4)
    maxLen = 4;

  offset = (UInt32)(Hc_GetMatchesSpec(lenLimit, curMatch, MF_PARAMS(p),
      distances + offset, maxLen) - (distances));
  MOVE_POS_RET
}
*/

UInt32 Hc3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances)
{
  unsigned offset;
  GET_MATCHES_HEADER(3)
  HASH_ZIP_CALC;
  curMatch = p->hash[hv];
  p->hash[hv] = p->pos;
  offset = (unsigned)(Hc_GetMatchesSpec(lenLimit, curMatch, MF_PARAMS(p),
      distances, 2) - (distances));
  MOVE_POS_RET
}

static void Bt2_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    SKIP_HEADER(2)
    HASH2_CALC;
    curMatch = p->hash[hv];
    p->hash[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

void Bt3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    SKIP_HEADER(3)
    HASH_ZIP_CALC;
    curMatch = p->hash[hv];
    p->hash[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

static void Bt3_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2;
    UInt32 *hash;
    SKIP_HEADER(3)
    HASH3_CALC;
    hash = p->hash;
    curMatch = (hash + kFix3HashSize)[hv];
    hash[h2] =
    (hash + kFix3HashSize)[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

static void Bt4_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2, h3;
    UInt32 *hash;
    SKIP_HEADER(4)
    HASH4_CALC;
    hash = p->hash;
    curMatch = (hash + kFix4HashSize)[hv];
    hash                  [h2] =
    (hash + kFix3HashSize)[h3] =
    (hash + kFix4HashSize)[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

/*
static void Bt5_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2, h3, h4;
    UInt32 *hash;
    SKIP_HEADER(5)
    HASH5_CALC;
    hash = p->hash;
    curMatch = (hash + kFix5HashSize)[hv];
    hash                  [h2] =
    (hash + kFix3HashSize)[h3] =
    (hash + kFix4HashSize)[h4] =
    (hash + kFix5HashSize)[hv] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}
*/

static void Hc4_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2, h3;
    UInt32 *hash;
    SKIP_HEADER(4)
    HASH4_CALC;
    hash = p->hash;
    curMatch = (hash + kFix4HashSize)[hv];
    hash                  [h2] =
    (hash + kFix3HashSize)[h3] =
    (hash + kFix4HashSize)[hv] = p->pos;
    p->son[p->cyclicBufferPos] = curMatch;
    MOVE_POS
  }
  while (--num != 0);
}

/*
static void Hc5_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    UInt32 h2, h3, h4;
    UInt32 *hash;
    SKIP_HEADER(5)
    HASH5_CALC;
    hash = p->hash;
    curMatch = hash + kFix5HashSize)[hv];
    hash                  [h2] =
    (hash + kFix3HashSize)[h3] =
    (hash + kFix4HashSize)[h4] =
    (hash + kFix5HashSize)[hv] = p->pos;
    p->son[p->cyclicBufferPos] = curMatch;
    MOVE_POS
  }
  while (--num != 0);
}
*/

void Hc3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num)
{
  do
  {
    SKIP_HEADER(3)
    HASH_ZIP_CALC;
    curMatch = p->hash[hv];
    p->hash[hv] = p->pos;
    p->son[p->cyclicBufferPos] = curMatch;
    MOVE_POS
  }
  while (--num != 0);
}

void MatchFinder_CreateVTable(CMatchFinder *p, IMatchFinder *vTable)
{
  vTable->Init = (Mf_Init_Func)MatchFinder_Init;
  vTable->GetNumAvailableBytes = (Mf_GetNumAvailableBytes_Func)MatchFinder_GetNumAvailableBytes;
  vTable->GetPointerToCurrentPos = (Mf_GetPointerToCurrentPos_Func)MatchFinder_GetPointerToCurrentPos;
  if (!p->btMode)
  {
    /* if (p->numHashBytes <= 4) */
    {
      vTable->GetMatches = (Mf_GetMatches_Func)Hc4_MatchFinder_GetMatches;
      vTable->Skip = (Mf_Skip_Func)Hc4_MatchFinder_Skip;
    }
    /*
    else
    {
      vTable->GetMatches = (Mf_GetMatches_Func)Hc5_MatchFinder_GetMatches;
      vTable->Skip = (Mf_Skip_Func)Hc5_MatchFinder_Skip;
    }
    */
  }
  else if (p->numHashBytes == 2)
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt2_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt2_MatchFinder_Skip;
  }
  else if (p->numHashBytes == 3)
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt3_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt3_MatchFinder_Skip;
  }
  else /* if (p->numHashBytes == 4) */
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt4_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt4_MatchFinder_Skip;
  }
  /*
  else
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt5_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt5_MatchFinder_Skip;
  }
  */
}



#undef Align

#endif /* LZMA_IMPLEMENTATION */
