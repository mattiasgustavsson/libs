/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

speech.h - v0.1 - Very basic text-to-speech synthesizer library for C/C++.

Do this:
    #define SPEECH_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.

------------------------------------------------------------------------------

This file contains modified code based on the TTS code from the SoLoud audio
engine (http://sol.gfxile.net/soloud/), which in turn is based on rsynth by 
Nick Ing-Simmons (et al). I have not included any code which were under the 
SoLoud license (ZLib/LibPNG). I've basically just merged the code into a 
single header library, converted it from C++ to C, and put my own interface 
on top of it instead of the original SoLoud interface.

The original license text can be found at the end of this file, and I place my
changes under the same license.

                        / Mattias Gustavsson ( mattias@mattiasgustavsson.com )
*/

#ifndef speech_h
#define speech_h

short* speech_gen( int* samples_pairs_generated, char const* str, void* memctx );
void speech_free( short* sample_pairs, void* memctx );

#endif /* speech_h */

/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef SPEECH_IMPLEMENTATION
#undef SPEECH_IMPLEMENTATION

#pragma warning( push )
#pragma warning( disable: 4706 ) // assignment within conditional expression

#ifndef SPEECH_MALLOC
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #if defined(__cplusplus)
        #define SPEECH_MALLOC( ctx, size ) ( ::malloc( size ) )
        #define SPEECH_FREE( ctx, ptr ) ( ::free( ptr ) )
    #else
        #define SPEECH_MALLOC( ctx, size ) ( malloc( size ) )
        #define SPEECH_FREE( ctx, ptr ) ( free( ptr ) )
    #endif
#endif


#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define TTS_PI 3.1415926535897932384626433832795f


struct tts_darray_t {
    void* memctx;
    char *mData;
    int mUsed;
    int mAllocated;
    int mAllocChunk;
    char mem[ 1024 ];
};


static void tts_darray_init( struct tts_darray_t* darray, void* memctx ) {
    darray->memctx = memctx;
    darray->mAllocChunk = 1024;
    darray->mUsed = 0;
    darray->mAllocated = sizeof( darray->mem );
    darray->mData = darray->mem;
}


static void tts_darray_clear( struct tts_darray_t* darray ) {
    if( darray->mData != darray->mem ) SPEECH_FREE( darray->memctx, darray->mData );
    darray->mAllocChunk = 1024;
    darray->mUsed = 0;
    darray->mAllocated = sizeof( darray->mem );
    darray->mData = darray->mem;
}


static void tts_darray_term( struct tts_darray_t* darray ) {
    tts_darray_clear( darray );
}


static char* tts_darray_getDataInPos( struct tts_darray_t* darray, int aPosition ) {
    if( aPosition < darray->mAllocated && aPosition < darray->mUsed )
        return darray->mData + aPosition;

    if( aPosition >= darray->mAllocated ) {
        int newsize = darray->mAllocated;

        while( newsize <= aPosition ) {
            newsize += darray->mAllocChunk;
            darray->mAllocChunk *= 2;
        }

        char* newdata = (char*) SPEECH_MALLOC( darray->memctx, newsize );
        if( !newdata ) {
            if( darray->mData != darray->mem ) SPEECH_FREE( memctx, darray->mData);
            darray->mData = NULL;
            darray->mAllocated = darray->mUsed = 0;
            return NULL;
        }
        memcpy( newdata, darray->mData, darray->mAllocated );
        if( darray->mData != darray->mem ) {
            SPEECH_FREE( darray->memctx, darray->mData );
        }
        darray->mData = newdata;
        darray->mAllocated = newsize;           
    }

    if( aPosition >= darray->mUsed ) {
        darray->mUsed = aPosition + 1;
    }

    return darray->mData + aPosition;
}


static void tts_darray_put( struct tts_darray_t* darray, int aData ) {
    char *s = tts_darray_getDataInPos( darray, darray->mUsed );

    *s = (char) aData;
}


static int tts_darray_getSize( struct tts_darray_t* darray ) { 
    return darray->mUsed; 
}


static char* tts_darray_getData( struct tts_darray_t* darray ) { 
    return darray->mData; 
} 

struct tts_resonator_t {
    float mA, mB, mC, mP1, mP2;
};


/* Convert formant freqencies and bandwidth into resonator difference equation coefficents */
static void tts_resonator_initResonator(
    struct tts_resonator_t* resonator,
    int aFrequency,                       /* Frequency of resonator in Hz  */
    int aBandwidth,                      /* Bandwidth of resonator in Hz  */
    int aSamplerate )
{
    float arg = ( -TTS_PI / aSamplerate ) * aBandwidth;
    float r = (float) exp( arg );  
    resonator->mC = -( r * r );             
    arg = ( -2.0f * TTS_PI / aSamplerate ) * aFrequency;
    resonator->mB = r * (float) cos( arg ) * 2.0f;   
    resonator->mA = 1.0f - resonator->mB - resonator->mC;    
}


/* Convert formant freqencies and bandwidth into anti-resonator difference equation constants */
static void tts_resonator_initAntiresonator(
    struct tts_resonator_t* resonator,
    int aFrequency,                       /* Frequency of resonator in Hz  */
    int aBandwidth,                      /* Bandwidth of resonator in Hz  */
    int aSamplerate ) {

    tts_resonator_initResonator( resonator, aFrequency, aBandwidth, aSamplerate ); /* First compute ordinary resonator coefficients */
    /* Now convert to antiresonator coefficients */
    resonator->mA = 1.0f / resonator->mA;             /* a'=  1/a */
    resonator->mB *= -resonator->mA;                  /* b'= -b/a */
    resonator->mC *= -resonator->mA;                  /* c'= -c/a */
}


/* Generic resonator function */
static float tts_resonator_resonate( struct tts_resonator_t* resonator, float input ) {
    float x = resonator->mA * input + resonator->mB * resonator->mP1 + resonator->mC * resonator->mP2;
    resonator->mP2 = resonator->mP1;
    resonator->mP1 = x;
    return x;
}


/* Generic anti-resonator function
    Same as resonator except that a,b,c need to be set with initAntiresonator()
    and we save inputs in p1/p2 rather than outputs.
    There is currently only one of these - "mNasalZero"
*/
/*  Output = (mNasalZero.a * input) + (mNasalZero.b * oldin1) + (mNasalZero.c * oldin2) */

static float tts_resonator_antiresonate( struct tts_resonator_t* resonator, float input ) {
    float x = resonator->mA * input + resonator->mB * resonator->mP1 + resonator->mC * resonator->mP2;
    resonator->mP2 = resonator->mP1;
    resonator->mP1 = input;
    return x;
}


static void tts_resonator_init( struct tts_resonator_t* resonator ) {
    resonator->mA = resonator->mB = resonator->mC = resonator->mP1 = resonator->mP2 = 0;
}


static void tts_resonator_setGain( struct tts_resonator_t* resonator, float aG ) {
    resonator->mA *= aG;
}



#define TTS_CASCADE_PARALLEL      1
#define TTS_ALL_PARALLEL          2
#define TTS_NPAR                 40

struct tts_klatt_frame_t {
    int mF0FundamentalFreq;          // Voicing fund freq in Hz                       
    int mVoicingAmpdb;               // Amp of voicing in dB,            0 to   70    
    int mFormant1Freq;               // First formant freq in Hz,        200 to 1300  
    int mFormant1Bandwidth;          // First formant bw in Hz,          40 to 1000   
    int mFormant2Freq;               // Second formant freq in Hz,       550 to 3000  
    int mFormant2Bandwidth;          // Second formant bw in Hz,         40 to 1000   
    int mFormant3Freq;               // Third formant freq in Hz,        1200 to 4999 
    int mFormant3Bandwidth;          // Third formant bw in Hz,          40 to 1000   
    int mFormant4Freq;               // Fourth formant freq in Hz,       1200 to 4999 
    int mFormant4Bandwidth;          // Fourth formant bw in Hz,         40 to 1000   
    int mFormant5Freq;               // Fifth formant freq in Hz,        1200 to 4999 
    int mFormant5Bandwidth;          // Fifth formant bw in Hz,          40 to 1000   
    int mFormant6Freq;               // Sixth formant freq in Hz,        1200 to 4999 
    int mFormant6Bandwidth;          // Sixth formant bw in Hz,          40 to 2000   
    int mNasalZeroFreq;              // Nasal zero freq in Hz,           248 to  528  
    int mNasalZeroBandwidth;         // Nasal zero bw in Hz,             40 to 1000   
    int mNasalPoleFreq;              // Nasal pole freq in Hz,           248 to  528  
    int mNasalPoleBandwidth;         // Nasal pole bw in Hz,             40 to 1000   
    int mAspirationAmpdb;            // Amp of aspiration in dB,         0 to   70    
    int mNoSamplesInOpenPeriod;      // # of samples in open period,     10 to   65   
    int mVoicingBreathiness;         // Breathiness in voicing,          0 to   80    
    int mVoicingSpectralTiltdb;      // Voicing spectral tilt in dB,     0 to   24    
    int mFricationAmpdb;             // Amp of frication in dB,          0 to   80    
    int mSkewnessOfAlternatePeriods; // Skewness of alternate periods,   0 to   40 in sample#/2
    int mFormant1Ampdb;              // Amp of par 1st formant in dB,    0 to   80  
    int mFormant1ParallelBandwidth;  // Par. 1st formant bw in Hz,       40 to 1000 
    int mFormant2Ampdb;              // Amp of F2 frication in dB,       0 to   80  
    int mFormant2ParallelBandwidth;  // Par. 2nd formant bw in Hz,       40 to 1000 
    int mFormant3Ampdb;              // Amp of F3 frication in dB,       0 to   80  
    int mFormant3ParallelBandwidth;  // Par. 3rd formant bw in Hz,       40 to 1000 
    int mFormant4Ampdb;              // Amp of F4 frication in dB,       0 to   80  
    int mFormant4ParallelBandwidth;  // Par. 4th formant bw in Hz,       40 to 1000 
    int mFormant5Ampdb;              // Amp of F5 frication in dB,       0 to   80  
    int mFormant5ParallelBandwidth;  // Par. 5th formant bw in Hz,       40 to 1000 
    int mFormant6Ampdb;              // Amp of F6 (same as r6pa),        0 to   80  
    int mFormant6ParallelBandwidth;  // Par. 6th formant bw in Hz,       40 to 2000 
    int mParallelNasalPoleAmpdb;     // Amp of par nasal pole in dB,     0 to   80  
    int mBypassFricationAmpdb;       // Amp of bypass fric. in dB,       0 to   80  
    int mPalallelVoicingAmpdb;       // Amp of voicing,  par in dB,      0 to   70  
    int mOverallGaindb;              // Overall gain, 60 dB is unity,    0 to   60  
};


struct tts_slope_t {
    float mValue;                   /* boundary value */
    int mTime;                      /* transition time */
};


static void tts_slope_init( struct tts_slope_t* slope ) {
    slope->mValue = 0.0f;
    slope->mTime = 0;
}


struct tts_interp_t {
    float mSteady;
    float mFixed;
    char  mProportion;
    char  mExtDelay;
    char  mIntDelay;
};



enum tts_eparm_t {
    TTS_ELM_FN, TTS_ELM_F1, TTS_ELM_F2, TTS_ELM_F3, 
    TTS_ELM_B1, TTS_ELM_B2, TTS_ELM_B3, TTS_ELM_AN, 
    TTS_ELM_A1, TTS_ELM_A2, TTS_ELM_A3, TTS_ELM_A4, 
    TTS_ELM_A5, TTS_ELM_A6, TTS_ELM_AB, TTS_ELM_AV, 
    TTS_ELM_AVC, TTS_ELM_ASP, TTS_ELM_AF, 
    TTS_ELM_COUNT
};
 

struct tts_element_t {
      const char *mName; // unused
      const char mRK;
      const char mDU;
      const char mUD;
      unsigned char mFont; // unused
      const char  *mDict; // unused
      const char  *mIpa; // unused
      int   mFeat; // only TTS_ELM_FEATURE_VWL
      struct tts_interp_t mInterpolator[ TTS_ELM_COUNT ];
};


struct tts_klatt_t {
    // resonators
    struct tts_resonator_t mParallelFormant1, mParallelFormant2, mParallelFormant3, 
                       mParallelFormant4, mParallelFormant5, mParallelFormant6,
                       mParallelResoNasalPole, mNasalPole, mNasalZero, 
                       mCritDampedGlotLowPassFilter, mDownSampLowPassFilter, 
                       mOutputLowPassFilter;

    int mF0Flutter;
    int mSampleRate;
    int mNspFr;
    int mF0FundamentalFreq;        // Voicing fund freq in Hz  
    int mVoicingAmpdb;          // Amp of voicing in dB,    0 to   70  
    int mSkewnessOfAlternatePeriods;         // Skewness of alternate periods,0 to   40  
    int mTimeCount;     // used for f0 flutter
    int mNPer;          // Current loc in voicing period   40000 samp/s
    int mT0;            // Fundamental period in output samples times 4 
    int mNOpen;         // Number of samples in open phase of period  
    int mNMod;          // Position in period to begin noise amp. modul 

    // Various amplitude variables used in main loop

    float mAmpVoice;     // mVoicingAmpdb converted to linear gain  
    float mAmpBypas;     // mBypassFricationAmpdb converted to linear gain  
    float mAmpAspir;     // AP converted to linear gain  
    float mAmpFrica;     // mFricationAmpdb converted to linear gain  
    float mAmpBreth;     // ATURB converted to linear gain  

    // State variables of sound sources

    int mSkew;                  // Alternating jitter, in half-period units  
    float mNatglotA;           // Makes waveshape of glottal pulse when open  
    float mNatglotB;           // Makes waveshape of glottal pulse when open  
    float mVWave;               // Ditto, but before multiplication by mVoicingAmpdb  
    float mVLast;               // Previous output of voice  
    float mNLast;               // Previous output of random number generator  
    float mGlotLast;            // Previous value of glotout  
    float mDecay;               // mVoicingSpectralTiltdb converted to exponential time const  
    float mOneMd;               // in voicing one-pole TTS_ELM_FEATURE_LOW-pass filter  

    int mElementCount;
    unsigned char* mElement;
    int mElementIndex;
    struct tts_klatt_frame_t mKlattFramePars;
    struct tts_element_t* mLastElement;
    int mTStress;
    int mNTStress;
    struct tts_slope_t mStressS;
    struct tts_slope_t mStressE;
    float mTop;

    int count; // MG: REINIT AT LONG PAUSE
};


enum tts_elm_feature_t {
    TTS_ELM_FEATURE_ALV = 0x00000001,
    TTS_ELM_FEATURE_APR = 0x00000002,
    TTS_ELM_FEATURE_BCK = 0x00000004,
    TTS_ELM_FEATURE_BLB = 0x00000008,
    TTS_ELM_FEATURE_CNT = 0x00000010,
    TTS_ELM_FEATURE_DNT = 0x00000020,
    TTS_ELM_FEATURE_FNT = 0x00000040,
    TTS_ELM_FEATURE_FRC = 0x00000080,
    TTS_ELM_FEATURE_GLT = 0x00000100,
    TTS_ELM_FEATURE_HGH = 0x00000200,
    TTS_ELM_FEATURE_LAT = 0x00000400,
    TTS_ELM_FEATURE_LBD = 0x00000800,
    TTS_ELM_FEATURE_LBV = 0x00001000,
    TTS_ELM_FEATURE_LMD = 0x00002000,
    TTS_ELM_FEATURE_LOW = 0x00004000,
    TTS_ELM_FEATURE_MDL = 0x00008000,
    TTS_ELM_FEATURE_NAS = 0x00010000,
    TTS_ELM_FEATURE_PAL = 0x00020000,
    TTS_ELM_FEATURE_PLA = 0x00040000,
    TTS_ELM_FEATURE_RND = 0x00080000,
    TTS_ELM_FEATURE_RZD = 0x00100000,
    TTS_ELM_FEATURE_SMH = 0x00200000,
    TTS_ELM_FEATURE_STP = 0x00400000,
    TTS_ELM_FEATURE_UMD = 0x00800000,
    TTS_ELM_FEATURE_UNR = 0x01000000,
    TTS_ELM_FEATURE_VCD = 0x02000000,
    TTS_ELM_FEATURE_VEL = 0x04000000,
    TTS_ELM_FEATURE_VLS = 0x08000000,
    TTS_ELM_FEATURE_VWL = 0x10000000
};


enum tts_elm_t 
{
    TTS_ELM_END = 0,    
    TTS_ELM_Q,  TTS_ELM_P,  TTS_ELM_PY, TTS_ELM_PZ, TTS_ELM_T,  TTS_ELM_TY, 
    TTS_ELM_TZ, TTS_ELM_K,  TTS_ELM_KY, TTS_ELM_KZ, TTS_ELM_B,  TTS_ELM_BY, TTS_ELM_BZ, 
    TTS_ELM_D,  TTS_ELM_DY, TTS_ELM_DZ, TTS_ELM_G,  TTS_ELM_GY, TTS_ELM_GZ, TTS_ELM_M,  
    TTS_ELM_N,  TTS_ELM_NG, TTS_ELM_F,  TTS_ELM_TH, TTS_ELM_S,  TTS_ELM_SH, TTS_ELM_X,
    TTS_ELM_H,  TTS_ELM_V,  TTS_ELM_QQ, TTS_ELM_DH, TTS_ELM_DI, TTS_ELM_Z,  TTS_ELM_ZZ,
    TTS_ELM_ZH, TTS_ELM_CH, TTS_ELM_CI, TTS_ELM_J,  TTS_ELM_JY, TTS_ELM_L,  TTS_ELM_LL,
    TTS_ELM_RX, TTS_ELM_R,  TTS_ELM_W,  TTS_ELM_Y,  TTS_ELM_I,  TTS_ELM_E,  TTS_ELM_AA,
    TTS_ELM_U,  TTS_ELM_O,  TTS_ELM_OO, TTS_ELM_A,  TTS_ELM_EE, TTS_ELM_ER, TTS_ELM_AR,
    TTS_ELM_AW, TTS_ELM_UU, TTS_ELM_AI, TTS_ELM_IE, TTS_ELM_OI, TTS_ELM_OU, TTS_ELM_OV,
    TTS_ELM_OA, TTS_ELM_IA, TTS_ELM_IB, TTS_ELM_AIR,TTS_ELM_OOR,TTS_ELM_OR
};


#define TTS_PHONEME_COUNT 52
#define TTS_AMP_ADJ 14
#define TTS_StressDur(e) (((e->mDU + e->mUD)/2))




struct tts_phoneme_to_elements_t  {
    int mKey;
    char mData[8];
};


/* Order is important - 2 byte phonemes first, otherwise
   the search function will fail*/
static struct tts_phoneme_to_elements_t tts_phoneme_to_elements[TTS_PHONEME_COUNT] = {
    /* mKey, count, 0-7 elements */
/* tS */ 0x5374, 2, TTS_ELM_CH, TTS_ELM_CI, 0, 0, 0, 0, 0,
/* dZ */ 0x5a64, 4, TTS_ELM_J, TTS_ELM_JY, TTS_ELM_QQ, TTS_ELM_JY, 0, 0, 0,
/* rr */ 0x7272, 3, TTS_ELM_R, TTS_ELM_QQ, TTS_ELM_R, 0, 0, 0, 0,
/* eI */ 0x4965, 2, TTS_ELM_AI, TTS_ELM_I, 0, 0, 0, 0, 0,
/* aI */ 0x4961, 2, TTS_ELM_IE, TTS_ELM_I, 0, 0, 0, 0, 0,
/* oI */ 0x496f, 2, TTS_ELM_OI, TTS_ELM_I, 0, 0, 0, 0, 0,
/* aU */ 0x5561, 2, TTS_ELM_OU, TTS_ELM_OV, 0, 0, 0, 0, 0,
/* @U */ 0x5540, 2, TTS_ELM_OA, TTS_ELM_OV, 0, 0, 0, 0, 0,
/* I@ */ 0x4049, 2, TTS_ELM_IA, TTS_ELM_IB, 0, 0, 0, 0, 0,
/* e@ */ 0x4065, 2, TTS_ELM_AIR, TTS_ELM_IB, 0, 0, 0, 0, 0,
/* U@ */ 0x4055, 2, TTS_ELM_OOR, TTS_ELM_IB, 0, 0, 0, 0, 0,
/* O@ */ 0x404f, 2, TTS_ELM_OR, TTS_ELM_IB, 0, 0, 0, 0, 0,
/* oU */ 0x556f, 2, TTS_ELM_OI, TTS_ELM_OV, 0, 0, 0, 0, 0,
/*    */ 0x0020, 1, TTS_ELM_Q, 0, 0, 0, 0, 0, 0,
/* p  */ 0x0070, 3, TTS_ELM_P, TTS_ELM_PY, TTS_ELM_PZ, 0, 0, 0, 0,
/* t  */ 0x0074, 3, TTS_ELM_T, TTS_ELM_TY, TTS_ELM_TZ, 0, 0, 0, 0,
/* k  */ 0x006b, 3, TTS_ELM_K, TTS_ELM_KY, TTS_ELM_KZ, 0, 0, 0, 0,
/* b  */ 0x0062, 3, TTS_ELM_B, TTS_ELM_BY, TTS_ELM_BZ, 0, 0, 0, 0,
/* d  */ 0x0064, 3, TTS_ELM_D, TTS_ELM_DY, TTS_ELM_DZ, 0, 0, 0, 0,
/* g  */ 0x0067, 3, TTS_ELM_G, TTS_ELM_GY, TTS_ELM_GZ, 0, 0, 0, 0,
/* m  */ 0x006d, 1, TTS_ELM_M, 0, 0, 0, 0, 0, 0,
/* n  */ 0x006e, 1, TTS_ELM_N, 0, 0, 0, 0, 0, 0,
/* N  */ 0x004e, 1, TTS_ELM_NG, 0, 0, 0, 0, 0, 0,
/* f  */ 0x0066, 1, TTS_ELM_F, 0, 0, 0, 0, 0, 0,
/* T  */ 0x0054, 1, TTS_ELM_TH, 0, 0, 0, 0, 0, 0,
/* s  */ 0x0073, 1, TTS_ELM_S, 0, 0, 0, 0, 0, 0,
/* S  */ 0x0053, 1, TTS_ELM_SH, 0, 0, 0, 0, 0, 0,
/* h  */ 0x0068, 1, TTS_ELM_H, 0, 0, 0, 0, 0, 0,
/* v  */ 0x0076, 3, TTS_ELM_V, TTS_ELM_QQ, TTS_ELM_V, 0, 0, 0, 0,
/* D  */ 0x0044, 3, TTS_ELM_DH, TTS_ELM_QQ, TTS_ELM_DI, 0, 0, 0, 0,
/* z  */ 0x007a, 3, TTS_ELM_Z, TTS_ELM_QQ, TTS_ELM_ZZ, 0, 0, 0, 0,
/* Z  */ 0x005a, 3, TTS_ELM_ZH, TTS_ELM_QQ, TTS_ELM_ZH, 0, 0, 0, 0,
/* l  */ 0x006c, 1, TTS_ELM_L, 0, 0, 0, 0, 0, 0,
/* r  */ 0x0072, 1, TTS_ELM_R, 0, 0, 0, 0, 0, 0,
/* R  */ 0x0052, 1, TTS_ELM_RX, 0, 0, 0, 0, 0, 0,
/* w  */ 0x0077, 1, TTS_ELM_W, 0, 0, 0, 0, 0, 0,
/* x  */ 0x0078, 1, TTS_ELM_X, 0, 0, 0, 0, 0, 0,
/* %  */ 0x0025, 1, TTS_ELM_QQ, 0, 0, 0, 0, 0, 0,
/* j  */ 0x006a, 1, TTS_ELM_Y, 0, 0, 0, 0, 0, 0,
/* I  */ 0x0049, 1, TTS_ELM_I, 0, 0, 0, 0, 0, 0,
/* e  */ 0x0065, 1, TTS_ELM_E, 0, 0, 0, 0, 0, 0,
/* &  */ 0x0026, 1, TTS_ELM_AA, 0, 0, 0, 0, 0, 0,
/* V  */ 0x0056, 1, TTS_ELM_U, 0, 0, 0, 0, 0, 0,
/* 0  */ 0x0030, 1, TTS_ELM_O, 0, 0, 0, 0, 0, 0,
/* U  */ 0x0055, 1, TTS_ELM_OO, 0, 0, 0, 0, 0, 0,
/* @  */ 0x0040, 1, TTS_ELM_A, 0, 0, 0, 0, 0, 0,
/* i  */ 0x0069, 1, TTS_ELM_EE, 0, 0, 0, 0, 0, 0,
/* 3  */ 0x0033, 1, TTS_ELM_ER, 0, 0, 0, 0, 0, 0,
/* A  */ 0x0041, 1, TTS_ELM_AR, 0, 0, 0, 0, 0, 0,
/* O  */ 0x004f, 1, TTS_ELM_AW, 0, 0, 0, 0, 0, 0,
/* u  */ 0x0075, 1, TTS_ELM_UU, 0, 0, 0, 0, 0, 0,
/* o  */ 0x006f, 1, TTS_ELM_OI, 0, 0, 0, 0, 0, 0,
};


static struct tts_element_t tts_gElement[] = {
/*mName, mRK, mDU, mUD, mFont, mDict, mIpa, mFeat, interpolators*/
/* (mSteady, mFixed, mProportion, mExtDelay, mIntDelay) */
{"END", 31, 5, 5,0x00,NULL,NULL,0,
 {
  {   270,    135,  50,  3,  3}, /* TTS_ELM_FN       0 */
  {   490,      0, 100,  0,  0}, /* TTS_ELM_F1       0 */
  {  1480,      0, 100,  0,  0}, /* TTS_ELM_F2       0 */
  {  2500,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_AN   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_A1   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_A2   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_A3   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_A4   -10.5 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"Q",   29, 6, 6,0x00,NULL,NULL,0,
 {
  {   270,    135,  50,  3,  3}, /* TTS_ELM_FN       0 */
  {   490,      0, 100,  3,  3}, /* TTS_ELM_F1       0 */
  {  1480,      0, 100,  3,  3}, /* TTS_ELM_F2       0 */
  {  2500,      0, 100,  3,  3}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  3,  3}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  3,  3}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  3}, /* TTS_ELM_B3       0 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_AN   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_A1   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_A2   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_A3   -10.5 */
  {   -30,  -10.5, 100,  3,  0}, /* TTS_ELM_A4   -10.5 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"P",   23, 8, 8,0x70,"p","p",TTS_ELM_FEATURE_BLB|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* TTS_ELM_F1      15 */
  {   760,    350,  50,  2,  2}, /* TTS_ELM_F2     -30 */
  {  2500,      0, 100,  0,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"PY",  29, 1, 1,0x70,"p","p",TTS_ELM_FEATURE_BLB|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* TTS_ELM_F1       0 */
  {   760,      0, 100,  0,  0}, /* TTS_ELM_F2       0 */
  {  2500,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {    49,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 43.75,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {  38.5,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"PZ",  23, 2, 2,0x70,"p","p",TTS_ELM_FEATURE_BLB|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* TTS_ELM_F1      15 */
  {   760,    350,  50,  2,  2}, /* TTS_ELM_F2     -30 */
  {  2500,      0, 100,  2,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  2,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  38.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 33.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {    28,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"T",   23, 6, 6,0x74,"t","t",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* TTS_ELM_F1      15 */
  {  1780,    950,  50,  2,  2}, /* TTS_ELM_F2      60 */
  {  2680,   2680,   0,  0,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* TTS_ELM_B2       0 */
  {   150,    150,   0,  0,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"TY",  29, 1, 1,0x74,"t","t",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* TTS_ELM_F1       0 */
  {  1780,      0, 100,  0,  0}, /* TTS_ELM_F2       0 */
  {  2680,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {  38.5,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 50.75,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"TZ",  23, 2, 2,0x74,"t","t",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  1}, /* TTS_ELM_F1      15 */
  {  1780,    950,  50,  2,  1}, /* TTS_ELM_F2      60 */
  {  2680,   2680,   0,  2,  0}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  1}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  1}, /* TTS_ELM_B2       0 */
  {   150,    150,   0,  2,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {    28,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"K",   23, 8, 8,0x6B,"k","k",TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VEL|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  3,  3}, /* TTS_ELM_F1      15 */
  {  1480,   1550,  50,  3,  3}, /* TTS_ELM_F2     810 */
  {  2620,   1580,  50,  3,  3}, /* TTS_ELM_F3     270 */
  {    60,     30,  50,  3,  3}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  3}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  3,  3}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"KY",  29, 1, 1,0x6B,"k","k",TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VEL|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* TTS_ELM_F1       0 */
  {  1480,      0, 100,  0,  0}, /* TTS_ELM_F2       0 */
  {  2620,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 50.75,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 50.75,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"KZ",  23, 4, 4,0x6B,"k","k",TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VEL|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  3,  3}, /* TTS_ELM_F1      15 */
  {  1480,   1550,  50,  3,  3}, /* TTS_ELM_F2     810 */
  {  2620,   1580,  50,  3,  3}, /* TTS_ELM_F3     270 */
  {    60,     30,  50,  3,  3}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  3}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  3,  3}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 19.25,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"B",   26,12,12,0x62,"b","b",TTS_ELM_FEATURE_BLB|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* TTS_ELM_F1      15 */
  {   760,    350,  50,  2,  2}, /* TTS_ELM_F2     -30 */
  {  2500,      0, 100,  0,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"BY",  29, 1, 1,0x62,"b","b",TTS_ELM_FEATURE_BLB|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* TTS_ELM_F1       0 */
  {   760,      0, 100,  0,  0}, /* TTS_ELM_F2       0 */
  {  2500,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {    49,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 43.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {  38.5,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"BZ",  26, 0, 0,0x62,"b","b",TTS_ELM_FEATURE_BLB|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  0}, /* TTS_ELM_F1      15 */
  {   760,    350,  50,  2,  0}, /* TTS_ELM_F2     -30 */
  {  2500,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  0}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"D",   26, 8, 8,0x64,"d","d",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* TTS_ELM_F1      15 */
  {  1780,    950,  50,  2,  2}, /* TTS_ELM_F2      60 */
  {  2680,   2680,   0,  2,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* TTS_ELM_B2       0 */
  {   150,    150,   0,  2,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {  31.5,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"DY",  29, 1, 1,0x64,"d","d",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* TTS_ELM_F1       0 */
  {  1780,      0, 100,  0,  0}, /* TTS_ELM_F2       0 */
  {  2680,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {  38.5,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  38.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {    35,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {  45.5,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"DZ",  26, 1, 1,0x64,"d","d",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  0}, /* TTS_ELM_F1      15 */
  {  1780,    950,  50,  2,  0}, /* TTS_ELM_F2      60 */
  {  2680,   2680,   0,  2,  0}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  0}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  0}, /* TTS_ELM_B2       0 */
  {   150,    150,   0,  2,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {  38.5,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {    28,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {    35,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},


{"G",   26,12,12,0x67,"g","g",TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD|TTS_ELM_FEATURE_VEL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  3,  3}, /* TTS_ELM_F1      15 */
  {  1480,   1550,  50,  3,  3}, /* TTS_ELM_F2     810 */
  {  2620,   1580,  50,  3,  3}, /* TTS_ELM_F3     270 */
  {    60,     30,  50,  3,  3}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  3}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  3,  3}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {    35,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"GY",  29, 1, 1,0x67,"g","g",TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD|TTS_ELM_FEATURE_VEL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,      0, 100,  0,  0}, /* TTS_ELM_F1       0 */
  {  1480,      0, 100,  0,  0}, /* TTS_ELM_F2       0 */
  {  2620,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {    35,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  45.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"GZ",  26, 2, 2,0x67,"g","g",TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD|TTS_ELM_FEATURE_VEL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  3,  2}, /* TTS_ELM_F1      15 */
  {  1480,   1550,  50,  3,  2}, /* TTS_ELM_F2     810 */
  {  2620,   1580,  50,  3,  2}, /* TTS_ELM_F3     270 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {    35,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {    35,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {    14,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"M",   15, 8, 8,0x6D,"m","m",TTS_ELM_FEATURE_BLB|TTS_ELM_FEATURE_NAS,
 {
  {   360,    360,   0,  3,  0}, /* TTS_ELM_FN       0 */
  {   480,    480,   0,  3,  0}, /* TTS_ELM_F1       0 */
  {  1000,    350,  50,  3,  0}, /* TTS_ELM_F2    -150 */
  {  2200,      0, 100,  5,  0}, /* TTS_ELM_F3       0 */
  {    40,     20,  50,  3,  0}, /* TTS_ELM_B1       0 */
  {   175,     87,  50,  3,  0}, /* TTS_ELM_B2    -0.5 */
  {   120,      0, 100,  5,  0}, /* TTS_ELM_B3       0 */
  {    42,     21,  50,  3,  0}, /* TTS_ELM_AN       0 */
  {    26,    -10, 100,  3,  0}, /* TTS_ELM_A1     -10 */
  {    30,    -10, 100,  3,  0}, /* TTS_ELM_A2     -10 */
  {    33,    -10, 100,  3,  0}, /* TTS_ELM_A3     -10 */
  {   -30,    -10, 100,  3,  0}, /* TTS_ELM_A4     -10 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  2,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  2,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  2,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  2,  0}  /* TTS_ELM_AF       0 */
 }
},

{"N",   15, 8, 8,0x6E,"n","n",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_NAS,
 {
  {   450,    450,   0,  3,  0}, /* TTS_ELM_FN       0 */
  {   480,    480,   0,  3,  0}, /* TTS_ELM_F1       0 */
  {  1780,    950,  50,  3,  3}, /* TTS_ELM_F2      60 */
  {  2620,   2680,   0,  3,  0}, /* TTS_ELM_F3      60 */
  {    40,     20,  50,  3,  0}, /* TTS_ELM_B1       0 */
  {   300,    150,  50,  3,  3}, /* TTS_ELM_B2       0 */
  {   260,    130,  50,  3,  0}, /* TTS_ELM_B3       0 */
  {    42,     21,  50,  3,  0}, /* TTS_ELM_AN       0 */
  {    35,    -10, 100,  3,  0}, /* TTS_ELM_A1     -10 */
  {    35,    -10, 100,  3,  0}, /* TTS_ELM_A2     -10 */
  {    35,    -10, 100,  3,  0}, /* TTS_ELM_A3     -10 */
  {    20,    -10, 100,  3,  0}, /* TTS_ELM_A4     -10 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  2,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  2,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  2,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  2,  0}  /* TTS_ELM_AF       0 */
 }
},

{"NG",  15, 8, 8,0x4E,"N","N",TTS_ELM_FEATURE_NAS|TTS_ELM_FEATURE_VEL,
 {
  {   360,    360,   0,  3,  0}, /* TTS_ELM_FN       0 */
  {   480,    480,   0,  3,  0}, /* TTS_ELM_F1       0 */
  {   820,   1550,  50,  5,  3}, /* TTS_ELM_F2    1140 */
  {  2800,   1580,  50,  3,  3}, /* TTS_ELM_F3     180 */
  {   160,     80,   0,  5,  0}, /* TTS_ELM_B1     -80 */
  {   150,     75,  50,  5,  3}, /* TTS_ELM_B2       0 */
  {   100,     50,  50,  3,  0}, /* TTS_ELM_B3       0 */
  {    42,     21,  50,  3,  3}, /* TTS_ELM_AN       0 */
  {    20,      0, 100,  3,  0}, /* TTS_ELM_A1       0 */
  {    30,      0, 100,  3,  0}, /* TTS_ELM_A2       0 */
  {    35,      0, 100,  3,  0}, /* TTS_ELM_A3       0 */
  {     0,      0, 100,  3,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  3,  0}, /* TTS_ELM_AB       0 */
  {    52,     26,  50,  2,  0}, /* TTS_ELM_AV       0 */
  {    56,     28,  50,  2,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  2,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  2,  0}  /* TTS_ELM_AF       0 */
 }
},

{"F",   18,12,12,0x66,"f","f",TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_LBD|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* TTS_ELM_F1     -30 */
  {  1420,    350,  50,  3,  2}, /* TTS_ELM_F2    -360 */
  {  2560,    980,  50,  3,  2}, /* TTS_ELM_F3    -300 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {     0,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {     0,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {    54,     27,  50,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    32,     16,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    54,     30,  50,  0,  0}  /* TTS_ELM_AF       3 */
 }
},

{"TH",  18,15,15,0x54,"T","T",TTS_ELM_FEATURE_DNT|TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* TTS_ELM_F1     -30 */
  {  1780,   1190,  50,  3,  2}, /* TTS_ELM_F2     300 */
  {  2680,   2680,   0,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,    150,   0,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {    28,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 22.75,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"S",   18,12,12,0x73,"s","s",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* TTS_ELM_F1     -30 */
  {  1720,    950,  50,  3,  2}, /* TTS_ELM_F2      90 */
  {  2620,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {   200,    100,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    96,     48,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   220,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {    28,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {    28,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    32,     16,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"SH",  18,12,12,0x53,"S","S",TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_PLA|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* TTS_ELM_F1     -30 */
  {  2200,   1190,  50,  3,  2}, /* TTS_ELM_F2      90 */
  {  2560,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  31.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {    42,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {  31.5,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"X",   18,12,12,0x78,"x","x",TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_VEL|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  3,  3}, /* TTS_ELM_F1      15 */
  {  1480,   1550,  50,  3,  3}, /* TTS_ELM_F2     810 */
  {  2620,   1580,  50,  3,  3}, /* TTS_ELM_F3     270 */
  {    60,     30,  50,  3,  3}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  3}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  3,  3}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 19.25,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"H",    9,10,10,0x68,"h","h",TTS_ELM_FEATURE_APR|TTS_ELM_FEATURE_GLT,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,      0, 100,  0,  7}, /* TTS_ELM_F1       0 */
  {  1480,      0, 100,  0,  7}, /* TTS_ELM_F2       0 */
  {  2500,      0, 100,  0,  7}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  7}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  7}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  7}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  7}, /* TTS_ELM_AN       0 */
  {    35,    -14, 100,  0,  7}, /* TTS_ELM_A1     -14 */
  { 36.75,    -14, 100,  0,  7}, /* TTS_ELM_A2     -14 */
  { 26.25,     -7, 100,  0,  7}, /* TTS_ELM_A3      -7 */
  { 22.75,   -3.5, 100,  0,  7}, /* TTS_ELM_A4    -3.5 */
  {   -30,      0, 100,  0,  7}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  7}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  7}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"V",   20, 4, 4,0x76,"v","v",TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_LBD|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* TTS_ELM_F1      30 */
  {  1420,    350,  50,  3,  2}, /* TTS_ELM_F2    -360 */
  {  2560,    980,  50,  3,  2}, /* TTS_ELM_F3    -300 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 36.75,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 33.25,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"QQ",  30, 0, 0,0x5A,"Z","Z",TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   280,      0, 100,  0,  0}, /* TTS_ELM_F1       0 */
  {  1420,      0, 100,  0,  0}, /* TTS_ELM_F2       0 */
  {  2560,      0, 100,  0,  0}, /* TTS_ELM_F3       0 */
  {    60,      0, 100,  0,  0}, /* TTS_ELM_B1       0 */
  {    90,      0, 100,  0,  0}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  0,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 40.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 36.75,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 33.25,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"DH",  20, 4, 4,0x54,"D","D",TTS_ELM_FEATURE_DNT|TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* TTS_ELM_F1      30 */
  {  1600,   1190,  50,  3,  2}, /* TTS_ELM_F2     390 */
  {  2560,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  31.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {    28,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {    54,     27,  50,  0,  0}, /* TTS_ELM_AB       0 */
  {    36,     18,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    54,     27,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"DI",  20, 4, 4,0x54,"D","D",TTS_ELM_FEATURE_DNT|TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* TTS_ELM_F1      30 */
  {  1600,   1190,  50,  3,  2}, /* TTS_ELM_F2     390 */
  {  2560,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  31.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {    28,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"Z",   20, 4, 4,0x7A,"z","z",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* TTS_ELM_F1      30 */
  {  1720,    950,  50,  3,  2}, /* TTS_ELM_F2      90 */
  {  2560,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 36.75,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    40,     20,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    54,     27,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"ZZ",  20, 4, 4,0x7A,"z","z",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* TTS_ELM_F1      30 */
  {  1720,    950,  50,  3,  2}, /* TTS_ELM_F2      90 */
  {  2560,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {  24.5,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 36.75,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"ZH",  20, 4, 4,0x5A,"Z","Z",TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_PLA|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* TTS_ELM_F1      30 */
  {  2020,   1190,  50,  3,  2}, /* TTS_ELM_F2     180 */
  {  2560,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 36.75,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"CH",  23, 4, 4,0x74,"t","t",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* TTS_ELM_F1      15 */
  {  1780,    950,  50,  2,  2}, /* TTS_ELM_F2      60 */
  {  2680,   2680,   0,  2,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* TTS_ELM_B2       0 */
  {   150,    150,   0,  2,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"CI",  18, 8, 8,0x53,"S","S",TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_PLA|TTS_ELM_FEATURE_VLS,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   400,    170,  50,  3,  2}, /* TTS_ELM_F1     -30 */
  {  2020,   1190,  50,  3,  2}, /* TTS_ELM_F2     180 */
  {  2560,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {  31.5,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {    42,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {  31.5,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {    60,     30,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {    60,     30,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"J",   26, 4, 4,0x64,"d","d",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_STP|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,    110,  50,  2,  2}, /* TTS_ELM_F1      15 */
  {  1780,    950,  50,  2,  2}, /* TTS_ELM_F2      60 */
  {  2680,   2680,   0,  2,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  2,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  2,  2}, /* TTS_ELM_B2       0 */
  {   150,    150,   0,  2,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  {  31.5,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"JY",  20, 3, 3,0x5A,"Z","Z",TTS_ELM_FEATURE_FRC|TTS_ELM_FEATURE_PLA|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   280,    170,  50,  3,  2}, /* TTS_ELM_F1      30 */
  {  2020,   1190,  50,  3,  2}, /* TTS_ELM_F2     180 */
  {  2560,      0, 100,  3,  2}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  3,  2}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  3,  2}, /* TTS_ELM_B2       0 */
  {   150,      0, 100,  3,  2}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 29.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 36.75,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"L",   11, 8, 8,0x6C,"l","l",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_LAT|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   460,    230,  50,  6,  0}, /* TTS_ELM_F1       0 */
  {  1480,    710,  50,  6,  0}, /* TTS_ELM_F2     -30 */
  {  2500,   1220,  50,  6,  0}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  6,  0}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  6,  0}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  6,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 36.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {    21,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"LL",  11, 8, 8,0x6C,"l","l",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_LAT|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   460,    230,  50,  6,  0}, /* TTS_ELM_F1       0 */
  {   940,    470,  50,  6,  0}, /* TTS_ELM_F2       0 */
  {  2500,   1220,  50,  6,  0}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  6,  0}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  6,  0}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  6,  0}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AN       0 */
  { 36.75,      0, 100,  0,  0}, /* TTS_ELM_A1       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A2       0 */
  { 26.25,      0, 100,  0,  0}, /* TTS_ELM_A3       0 */
  {    21,      0, 100,  0,  0}, /* TTS_ELM_A4       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A5       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_A6       0 */
  {   -30,      0, 100,  0,  0}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"RX",  10,10,10,0xD5,"R","<TTS_ELM_FEATURE_RZD>",TTS_ELM_FEATURE_RZD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,      0, 100,  0,  5}, /* TTS_ELM_F1       0 */
  {  1180,      0, 100,  0,  5}, /* TTS_ELM_F2       0 */
  {  1600,   1600,   0,  5,  5}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  0,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {    70,     35,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  {    42,     21,  50,  5,  5}, /* TTS_ELM_A1       0 */
  {    35,   17.5,  50,  5,  5}, /* TTS_ELM_A2       0 */
  {    35,   17.5,  50,  5,  5}, /* TTS_ELM_A3       0 */
  {   -30,      0,  50,  5,  5}, /* TTS_ELM_A4      15 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    50,     25,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"R",   10,11,11,0xA8,"r","r",TTS_ELM_FEATURE_ALV|TTS_ELM_FEATURE_APR,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,      0, 100,  0,  5}, /* TTS_ELM_F1       0 */
  {  1180,    590,  50,  5,  5}, /* TTS_ELM_F2       0 */
  {  1600,    740,  50,  5,  5}, /* TTS_ELM_F3     -60 */
  {    60,      0, 100,  0,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  {    42,     21,  50,  5,  5}, /* TTS_ELM_A1       0 */
  {    35,   17.5,  50,  5,  5}, /* TTS_ELM_A2       0 */
  {    35,   17.5,  50,  5,  5}, /* TTS_ELM_A3       0 */
  {   -30,      0,  50,  5,  5}, /* TTS_ELM_A4      15 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"W",   10, 8, 8,0x77,"w","w",TTS_ELM_FEATURE_APR|TTS_ELM_FEATURE_LBV|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   190,     50,  50,  4,  4}, /* TTS_ELM_F1     -45 */
  {   760,    350,  50,  4,  4}, /* TTS_ELM_F2     -30 */
  {  2020,    980,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 43.75,     21,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  {    28,     14,  50,  4,  4}, /* TTS_ELM_A2       0 */
  {    21,   10.5,  50,  4,  4}, /* TTS_ELM_A3       0 */
  {   -30,      0,  50,  4,  4}, /* TTS_ELM_A4      15 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"Y",   10, 7, 7,0x6A,"j","j",TTS_ELM_FEATURE_APR|TTS_ELM_FEATURE_PAL|TTS_ELM_FEATURE_VCD,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   250,    110,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {  2500,   1190,  50,  4,  4}, /* TTS_ELM_F2     -60 */
  {  2980,   1460,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  { 33.25,   17.5,  50,  4,  4}, /* TTS_ELM_A2   0.875 */
  {  38.5,   17.5,  50,  4,  4}, /* TTS_ELM_A3   -1.75 */
  {  31.5,     14,  50,  4,  4}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"I",    2, 8, 6,0x49,"I","I",TTS_ELM_FEATURE_FNT|TTS_ELM_FEATURE_SMH|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   400,    170,  50,  4,  4}, /* TTS_ELM_F1     -30 */
  {  2080,   1070,  50,  4,  4}, /* TTS_ELM_F2      30 */
  {  2560,   1340,  50,  4,  4}, /* TTS_ELM_F3      60 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  { 36.75,   17.5,  50,  4,  4}, /* TTS_ELM_A2  -0.875 */
  {    35,   17.5,  50,  4,  4}, /* TTS_ELM_A3       0 */
  { 29.75,     14,  50,  4,  4}, /* TTS_ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"E",    2, 8, 4,0x45,"e","E",TTS_ELM_FEATURE_FNT|TTS_ELM_FEATURE_LMD|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   640,    350,  50,  4,  4}, /* TTS_ELM_F1      30 */
  {  2020,   1070,  50,  4,  4}, /* TTS_ELM_F2      60 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  {    42,     21,  50,  4,  4}, /* TTS_ELM_A2       0 */
  {  38.5,   17.5,  50,  4,  4}, /* TTS_ELM_A3   -1.75 */
  {  31.5,     14,  50,  4,  4}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"AA",   2,10, 5,0x51,"&","&",TTS_ELM_FEATURE_FNT|TTS_ELM_FEATURE_LOW|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   790,    410,  50,  4,  4}, /* TTS_ELM_F1      15 */
  {  1780,    950,  50,  4,  4}, /* TTS_ELM_F2      60 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {   130,     65,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  { 47.25,   24.5,  50,  4,  4}, /* TTS_ELM_A2   0.875 */
  {  38.5,   17.5,  50,  4,  4}, /* TTS_ELM_A3   -1.75 */
  {  31.5,     14,  50,  4,  4}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"U",    2, 9, 6,0xC3,"V","V",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_LMD|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   700,    350,  50,  4,  4}, /* TTS_ELM_F1       0 */
  {  1360,    710,  50,  4,  4}, /* TTS_ELM_F2      30 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  { 43.75,     21,  50,  4,  4}, /* TTS_ELM_A2  -0.875 */
  {  31.5,     14,  50,  4,  4}, /* TTS_ELM_A3   -1.75 */
  {  24.5,   10.5,  50,  4,  4}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"O",    2, 9, 6,0x81,"0","A.",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_LOW|TTS_ELM_FEATURE_RND|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   610,    290,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {   880,    470,  50,  4,  4}, /* TTS_ELM_F2      30 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  { 47.25,   24.5,  50,  4,  4}, /* TTS_ELM_A2   0.875 */
  { 22.75,   10.5,  50,  4,  4}, /* TTS_ELM_A3  -0.875 */
  { 15.75,      7,  50,  4,  4}, /* TTS_ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"OO",   2, 6, 4,0x55,"U","U",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_RND|TTS_ELM_FEATURE_SMH|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   370,    170,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {  1000,    470,  50,  4,  4}, /* TTS_ELM_F2     -30 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  {    42,     21,  50,  4,  4}, /* TTS_ELM_A2       0 */
  {    28,     14,  50,  4,  4}, /* TTS_ELM_A3       0 */
  { 22.75,   10.5,  50,  4,  4}, /* TTS_ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"A",    2, 4, 4,0xAB,"@","@",TTS_ELM_FEATURE_CNT|TTS_ELM_FEATURE_MDL|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,    230,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {  1480,    710,  50,  4,  4}, /* TTS_ELM_F2     -30 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A2  -0.875 */
  { 33.25,   17.5,  50,  4,  4}, /* TTS_ELM_A3   0.875 */
  { 26.25,     14,  50,  4,  4}, /* TTS_ELM_A4   0.875 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"EE",   2,11, 7,0x69,"i","i",TTS_ELM_FEATURE_FNT|TTS_ELM_FEATURE_HGH|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   250,    110,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {  2320,   1190,  50,  4,  4}, /* TTS_ELM_F2      30 */
  {  3200,   1580,  50,  4,  4}, /* TTS_ELM_F3     -20 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  { 33.25,   17.5,  50,  4,  4}, /* TTS_ELM_A2   0.875 */
  { 36.75,   17.5,  50,  4,  4}, /* TTS_ELM_A3  -0.875 */
  {  31.5,     14,  50,  4,  4}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"ER",   2,16,16,0xCE,"3","V\"",TTS_ELM_FEATURE_CNT|TTS_ELM_FEATURE_LMD|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   580,    290,  50,  4,  4}, /* TTS_ELM_F1       0 */
  {  1420,    710,  50,  4,  4}, /* TTS_ELM_F2       0 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  {  45.5,     21,  50,  4,  4}, /* TTS_ELM_A2   -1.75 */
  { 33.25,   17.5,  50,  4,  4}, /* TTS_ELM_A3   0.875 */
  { 26.25,     14,  50,  4,  4}, /* TTS_ELM_A4   0.875 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"AR",   2,15,15,0x41,"A","A",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_LOW|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   790,    410,  50,  4,  4}, /* TTS_ELM_F1      15 */
  {   880,    470,  50,  4,  4}, /* TTS_ELM_F2      30 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  {    49,   24.5,  50,  4,  4}, /* TTS_ELM_A2       0 */
  { 29.75,     14,  50,  4,  4}, /* TTS_ELM_A3  -0.875 */
  { 22.75,   10.5,  50,  4,  4}, /* TTS_ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"AW",   2,16,10,0x8D,"O","O",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_LMD|TTS_ELM_FEATURE_RND|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,    230,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {   820,    470,  50,  4,  4}, /* TTS_ELM_F2      60 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  {  45.5,     21,  50,  4,  4}, /* TTS_ELM_A2   -1.75 */
  { 22.75,   10.5,  50,  4,  4}, /* TTS_ELM_A3  -0.875 */
  {  17.5,      7,  50,  4,  4}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"UU",   2,14, 9,0x75,"u","u",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_HGH|TTS_ELM_FEATURE_RND|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   250,    110,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {   880,    470,  50,  4,  4}, /* TTS_ELM_F2      30 */
  {  2200,   1100,  50,  4,  4}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  {  38.5,   17.5,  50,  4,  4}, /* TTS_ELM_A2   -1.75 */
  {  17.5,      7,  50,  4,  4}, /* TTS_ELM_A3   -1.75 */
  {  10.5,    3.5,  50,  4,  4}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"AI",   2, 9, 6,0x45,"e","E",TTS_ELM_FEATURE_FNT|TTS_ELM_FEATURE_LMD|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   640,    290,  50,  5,  5}, /* TTS_ELM_F1     -30 */
  {  1600,    830,  50,  5,  5}, /* TTS_ELM_F2      30 */
  {  2500,   1220,  50,  5,  5}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  {  45.5,     21,  50,  5,  5}, /* TTS_ELM_A2   -1.75 */
  {    35,   17.5,  50,  5,  5}, /* TTS_ELM_A3       0 */
  { 29.75,     14,  50,  5,  5}, /* TTS_ELM_A4  -0.875 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"IE",   2, 9, 6,0x61,"a","a",TTS_ELM_FEATURE_CNT|TTS_ELM_FEATURE_LOW|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   790,    410,  50,  5,  5}, /* TTS_ELM_F1      15 */
  {   880,    470,  50,  5,  5}, /* TTS_ELM_F2      30 */
  {  2500,   1220,  50,  5,  5}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  {    49,   24.5,  50,  5,  5}, /* TTS_ELM_A2       0 */
  { 29.75,     14,  50,  5,  5}, /* TTS_ELM_A3  -0.875 */
  { 22.75,   10.5,  50,  5,  5}, /* TTS_ELM_A4  -0.875 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"OI",   2, 9, 6,0x6F,"o","o",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_RND|TTS_ELM_FEATURE_UMD|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,    230,  50,  5,  5}, /* TTS_ELM_F1     -15 */
  {   820,    350,  50,  5,  5}, /* TTS_ELM_F2     -60 */
  {  2500,   1220,  50,  5,  5}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  {  45.5,     21,  50,  5,  5}, /* TTS_ELM_A2   -1.75 */
  { 22.75,   10.5,  50,  5,  5}, /* TTS_ELM_A3  -0.875 */
  {  17.5,      7,  50,  5,  5}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"OU",   2, 9, 6,0x61,"a","a",TTS_ELM_FEATURE_CNT|TTS_ELM_FEATURE_LOW|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   790,    410,  50,  5,  5}, /* TTS_ELM_F1      15 */
  {  1300,    590,  50,  5,  5}, /* TTS_ELM_F2     -60 */
  {  2500,   1220,  50,  5,  5}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  { 47.25,   24.5,  50,  5,  5}, /* TTS_ELM_A2   0.875 */
  {    35,   17.5,  50,  5,  5}, /* TTS_ELM_A3       0 */
  {    28,     14,  50,  5,  5}, /* TTS_ELM_A4       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"OV",   2, 8, 6,0x55,"U","U",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_RND|TTS_ELM_FEATURE_SMH|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   370,    170,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {  1000,    470,  50,  4,  4}, /* TTS_ELM_F2     -30 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  {    42,     21,  50,  4,  4}, /* TTS_ELM_A2       0 */
  {    28,     14,  50,  4,  4}, /* TTS_ELM_A3       0 */
  { 22.75,   10.5,  50,  4,  4}, /* TTS_ELM_A4  -0.875 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"OA",   2, 9, 6,0xAB,"@","@",TTS_ELM_FEATURE_CNT|TTS_ELM_FEATURE_MDL|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,    230,  50,  5,  5}, /* TTS_ELM_F1     -15 */
  {  1480,    710,  50,  5,  5}, /* TTS_ELM_F2     -30 */
  {  2500,   1220,  50,  5,  5}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A2  -0.875 */
  { 33.25,   17.5,  50,  5,  5}, /* TTS_ELM_A3   0.875 */
  { 26.25,     14,  50,  5,  5}, /* TTS_ELM_A4   0.875 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"IA",   2, 9, 6,0x49,"I","I",TTS_ELM_FEATURE_FNT|TTS_ELM_FEATURE_SMH|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   310,    170,  50,  5,  5}, /* TTS_ELM_F1      15 */
  {  2200,   1070,  50,  5,  5}, /* TTS_ELM_F2     -30 */
  {  2920,   1460,  50,  5,  5}, /* TTS_ELM_F3       0 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  {    35,   17.5,  50,  5,  5}, /* TTS_ELM_A2       0 */
  { 36.75,   17.5,  50,  5,  5}, /* TTS_ELM_A3  -0.875 */
  {  31.5,     14,  50,  5,  5}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"IB",   2, 8, 6,0x51,"@","@",TTS_ELM_FEATURE_FNT|TTS_ELM_FEATURE_LOW|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,    230,  50,  4,  4}, /* TTS_ELM_F1     -15 */
  {  1480,    710,  50,  4,  4}, /* TTS_ELM_F2     -30 */
  {  2500,   1220,  50,  4,  4}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  4,  4}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  4,  4}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  4,  4}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  4,  4}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A1  -0.875 */
  { 50.75,   24.5,  50,  4,  4}, /* TTS_ELM_A2  -0.875 */
  { 33.25,   17.5,  50,  4,  4}, /* TTS_ELM_A3   0.875 */
  { 26.25,     14,  50,  4,  4}, /* TTS_ELM_A4   0.875 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  4,  4}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"AIR",  2, 9, 6,0x45,"e","E",TTS_ELM_FEATURE_FNT|TTS_ELM_FEATURE_LMD|TTS_ELM_FEATURE_UNR|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   640,    350,  50,  5,  5}, /* TTS_ELM_F1      30 */
  {  2020,   1070,  50,  5,  5}, /* TTS_ELM_F2      60 */
  {  2500,   1220,  50,  5,  5}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  {    42,     21,  50,  5,  5}, /* TTS_ELM_A2       0 */
  {  38.5,   17.5,  50,  5,  5}, /* TTS_ELM_A3   -1.75 */
  {  31.5,     14,  50,  5,  5}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"OOR",  2, 9, 6,0x55,"U","U",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_RND|TTS_ELM_FEATURE_SMH|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   370,    170,  50,  5,  5}, /* TTS_ELM_F1     -15 */
  {  1000,    470,  50,  5,  5}, /* TTS_ELM_F2     -30 */
  {  2500,   1220,  50,  5,  5}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  {    42,     21,  50,  5,  5}, /* TTS_ELM_A2       0 */
  {    28,     14,  50,  5,  5}, /* TTS_ELM_A3       0 */
  { 22.75,      7,  50,  5,  5}, /* TTS_ELM_A4  -4.375 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
},

{"OR",   2, 9, 6,0x8D,"O","O",TTS_ELM_FEATURE_BCK|TTS_ELM_FEATURE_LMD|TTS_ELM_FEATURE_RND|TTS_ELM_FEATURE_VWL,
 {
  {   270,    135,  50,  0,  0}, /* TTS_ELM_FN       0 */
  {   490,    230,  50,  5,  5}, /* TTS_ELM_F1     -15 */
  {   820,    470,  50,  5,  5}, /* TTS_ELM_F2      60 */
  {  2500,   1220,  50,  5,  5}, /* TTS_ELM_F3     -30 */
  {    60,     30,  50,  5,  5}, /* TTS_ELM_B1       0 */
  {    90,     45,  50,  5,  5}, /* TTS_ELM_B2       0 */
  {   150,     75,  50,  5,  5}, /* TTS_ELM_B3       0 */
  {   -30,      0, 100,  5,  5}, /* TTS_ELM_AN       0 */
  { 50.75,   24.5,  50,  5,  5}, /* TTS_ELM_A1  -0.875 */
  {  45.5,     21,  50,  5,  5}, /* TTS_ELM_A2   -1.75 */
  { 22.75,   10.5,  50,  5,  5}, /* TTS_ELM_A3  -0.875 */
  {  17.5,      7,  50,  5,  5}, /* TTS_ELM_A4   -1.75 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A5       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_A6       0 */
  {   -30,    -15,  50,  5,  5}, /* TTS_ELM_AB       0 */
  {    62,     31,  50,  0,  0}, /* TTS_ELM_AV       0 */
  {    16,      8,  50,  0,  0}, /* TTS_ELM_AVC      0 */
  {     0,      0,  50,  0,  0}, /* TTS_ELM_ASP      0 */
  {     0,      0,  50,  0,  0}  /* TTS_ELM_AF       0 */
 }
} 

};

static short tts_clip( float input ) {
    int temp = (int)input;
    /* tts_clip on boundaries of 16-bit word */

    if (temp < -32767) {
        //assert?
        temp = -32767;
    } else if (temp > 32767) {
        //assert?
        temp = 32767;
    }

    return (short)(temp);
}

/* Convert from decibels to a linear scale factor */
static float tts_DBtoLIN(int dB) {
    /*
    * Convertion table, db to linear, 87 dB --> 32767
    *                                 86 dB --> 29491 (1 dB down = 0.5**1/6)
    *                                 ...
    *                                 81 dB --> 16384 (6 dB down = 0.5)
    *                                 ...
    *                                  0 dB -->     0
    *
    * The just noticeable difference for a change in intensity of a vowel
    *   is approximately 1 dB.  Thus all amplitudes are quantized to 1 dB
    *   steps.
    */

    static const float amptable[88] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 6.0, 7.0,
        8.0, 9.0, 10.0, 11.0, 13.0,
        14.0, 16.0, 18.0, 20.0, 22.0,
        25.0, 28.0, 32.0, 35.0, 40.0,
        45.0, 51.0, 57.0, 64.0, 71.0,
        80.0, 90.0, 101.0, 114.0, 128.0,
        142.0, 159.0, 179.0, 202.0, 227.0,
        256.0, 284.0, 318.0, 359.0, 405.0,
        455.0, 512.0, 568.0, 638.0, 719.0,
        811.0, 911.0, 1024.0, 1137.0, 1276.0,
        1438.0, 1622.0, 1823.0, 2048.0, 2273.0,
        2552.0, 2875.0, 3244.0, 3645.0, 4096.0,
        4547.0, 5104.0, 5751.0, 6488.0, 7291.0,
        8192.0, 9093.0, 10207.0, 11502.0, 12976.0,
        14582.0, 16384.0, 18350.0, 20644.0, 23429.0,
        26214.0, 29491.0, 32767.0
    };

    // Check limits or argument (can be removed in final product)
    if (dB < 0) {
        dB = 0;
    } else if (dB >= 88) {
        dB = 87;
    }

    return amptable[dB] * 0.001f;
}



static void tts_klatt_frame_init( struct tts_klatt_frame_t* klatt_frame ) {
    klatt_frame->mF0FundamentalFreq = 1330;   
    klatt_frame->mVoicingAmpdb = 60;              
    klatt_frame->mFormant1Freq = 500;  
    klatt_frame->mFormant1Bandwidth = 60;     
    klatt_frame->mFormant2Freq = 1500;            
    klatt_frame->mFormant2Bandwidth = 90;
    klatt_frame->mFormant3Freq = 2800;        
    klatt_frame->mFormant3Bandwidth = 150;        
    klatt_frame->mFormant4Freq = 3250; 
    klatt_frame->mFormant4Bandwidth = 200;    
    klatt_frame->mFormant5Freq = 3700;            
    klatt_frame->mFormant5Bandwidth = 200;  
    klatt_frame->mFormant6Freq = 4990;        
    klatt_frame->mFormant6Bandwidth = 500;        
    klatt_frame->mNasalZeroFreq = 270;  
    klatt_frame->mNasalZeroBandwidth = 100;   
    klatt_frame->mNasalPoleFreq = 270;            
    klatt_frame->mNasalPoleBandwidth = 100; 
    klatt_frame->mAspirationAmpdb = 0;        
    klatt_frame->mNoSamplesInOpenPeriod = 30;     
    klatt_frame->mVoicingBreathiness = 0;      
    klatt_frame->mVoicingSpectralTiltdb = 10; 
    klatt_frame->mFricationAmpdb = 0;             
    klatt_frame->mSkewnessOfAlternatePeriods = 0;   
    klatt_frame->mFormant1Ampdb = 0;          
    klatt_frame->mFormant1ParallelBandwidth = 80; 
    klatt_frame->mFormant2Ampdb = 0;      
    klatt_frame->mFormant2ParallelBandwidth = 200; 
    klatt_frame->mFormant3Ampdb = 0;         
    klatt_frame->mFormant3ParallelBandwidth = 350;
    klatt_frame->mFormant4Ampdb = 0;          
    klatt_frame->mFormant4ParallelBandwidth = 500; 
    klatt_frame->mFormant5Ampdb = 0;      
    klatt_frame->mFormant5ParallelBandwidth = 600; 
    klatt_frame->mFormant6Ampdb = 0;         
    klatt_frame->mFormant6ParallelBandwidth = 800;    
    klatt_frame->mParallelNasalPoleAmpdb = 0; 
    klatt_frame->mBypassFricationAmpdb = 0;       
    klatt_frame->mPalallelVoicingAmpdb = 0;   
    klatt_frame->mOverallGaindb = 62;
};


/*
function FLUTTER

This function adds F0 flutter, as specified in:

"Analysis, synthesis and perception of voice quality variations among
female and male talkers" D.H. Klatt and L.C. Klatt JASA 87(2) February 1990.
Flutter is added by applying a quasi-random element constructed from three
slowly varying sine waves.
*/
static void tts_klatt_flutter( struct tts_klatt_t* klatt, struct tts_klatt_frame_t* pars ) {
    int original_f0 = pars->mF0FundamentalFreq / 10;
    float fla = (float) klatt->mF0Flutter / 50;
    float flb = (float) original_f0 / 100;
    float flc = (float)sin(2 * TTS_PI * 12.7 * klatt->mTimeCount);
    float fld = (float)sin(2 * TTS_PI * 7.1 * klatt->mTimeCount);
    float fle = (float)sin(2 * TTS_PI * 4.7 * klatt->mTimeCount);
    float delta_f0 = fla * flb * (flc + fld + fle) * 10;
    klatt->mF0FundamentalFreq += (int) delta_f0;
}

/* Vwave is the differentiated glottal flow waveform, there is a weak
spectral zero around 800 Hz, magic constants a,b reset pitch-synch
*/

static float tts_klatt_natural_source( struct tts_klatt_t* klatt, int aNper ) {
    // See if glottis open 

    if (aNper < klatt->mNOpen) {
        float lgtemp;
        klatt->mNatglotA -= klatt->mNatglotB;
        klatt->mVWave += klatt->mNatglotA;
        lgtemp = klatt->mVWave * 0.028f;        /* function of samp_rate ? */
        return (lgtemp);
    } else {
        // Glottis closed 
        klatt->mVWave = 0.0;
        return (0.0);
    }
}

/* Reset selected parameters pitch-synchronously */

static void tts_klatt_pitch_synch_par_reset( struct tts_klatt_t* klatt, struct tts_klatt_frame_t* frame, int ns ) {
    /*
    * Constant natglot[] controls shape of glottal pulse as a function
    * of desired duration of open phase N0
    * (Note that N0 is specified in terms of 40,000 samples/sec of speech)
    *
    *    Assume voicing waveform V(t) has form: k1 t**2 - k2 t**3
    *
    *    If the radiation characterivative, a temporal derivative
    *      is folded in, and we go from continuous time to discrete
    *      integers n:  dV/dt = mVWave[n]
    *                         = sum over i=1,2,...,n of { a - (i * b) }
    *                         = a n  -  b/2 n**2
    *
    *      where the  constants a and b control the detailed shape
    *      and amplitude of the voicing waveform over the open
    *      potion of the voicing cycle "mNOpen".
    *
    *    Let integral of dV/dt have no net dc flow --> a = (b * mNOpen) / 3
    *
    *    Let maximum of dUg(n)/dn be constant --> b = gain / (mNOpen * mNOpen)
    *      meaning as mNOpen gets bigger, V has bigger peak proportional to n
    *
    *    Thus, to generate the table below for 40 <= mNOpen <= 263:
    *
    *      natglot[mNOpen - 40] = 1920000 / (mNOpen * mNOpen)
    */
    static const short natglot[ 224 ] = {
        1200, 1142, 1088, 1038, 991, 948, 907, 869, 833, 799,
        768, 738, 710, 683, 658, 634, 612, 590, 570, 551,
        533, 515, 499, 483, 468, 454, 440, 427, 415, 403,
        391, 380, 370, 360, 350, 341, 332, 323, 315, 307,
        300, 292, 285, 278, 272, 265, 259, 253, 247, 242,
        237, 231, 226, 221, 217, 212, 208, 204, 199, 195,
        192, 188, 184, 180, 177, 174, 170, 167, 164, 161,
        158, 155, 153, 150, 147, 145, 142, 140, 137, 135,
        133, 131, 128, 126, 124, 122, 120, 119, 117, 115,
        113, 111, 110, 108, 106, 105, 103, 102, 100, 99,
        97, 96, 95, 93, 92, 91, 90, 88, 87, 86,
        85, 84, 83, 82, 80, 79, 78, 77, 76, 75,
        75, 74, 73, 72, 71, 70, 69, 68, 68, 67,
        66, 65, 64, 64, 63, 62, 61, 61, 60, 59,
        59, 58, 57, 57, 56, 56, 55, 55, 54, 54,
        53, 53, 52, 52, 51, 51, 50, 50, 49, 49,
        48, 48, 47, 47, 46, 46, 45, 45, 44, 44,
        43, 43, 42, 42, 41, 41, 41, 41, 40, 40,
        39, 39, 38, 38, 38, 38, 37, 37, 36, 36,
        36, 36, 35, 35, 35, 35, 34, 34, 33, 33,
        33, 33, 32, 32, 32, 32, 31, 31, 31, 31,
        30, 30, 30, 30, 29, 29, 29, 29, 28, 28,
        28, 28, 27, 27
    };

    if( klatt->mF0FundamentalFreq > 0) {
        klatt->mT0 = (40 * klatt->mSampleRate) / klatt->mF0FundamentalFreq;

        /* Period in samp*4 */
        klatt->mAmpVoice = tts_DBtoLIN(klatt->mVoicingAmpdb);

        /* Duration of period before amplitude modulation */
        klatt->mNMod = klatt->mT0;

        if (klatt->mVoicingAmpdb > 0) {
            klatt->mNMod >>= 1;
        }

        /* Breathiness of voicing waveform */

        klatt->mAmpBreth = tts_DBtoLIN(frame->mVoicingBreathiness) * 0.1f;

        /* Set open phase of glottal period */
        /* where  40 <= open phase <= 263 */

        klatt->mNOpen = 4 * frame->mNoSamplesInOpenPeriod;

        if (klatt->mNOpen >= (klatt->mT0 - 1)) {
            klatt->mNOpen = klatt->mT0 - 2;
        }

        if (klatt->mNOpen < 40) {
            klatt->mNOpen = 40;                  /* F0 max = 1000 Hz */
        }

        /* Reset a & b, which determine shape of "natural" glottal waveform */

        klatt->mNatglotB = natglot[klatt->mNOpen - 40];
        klatt->mNatglotA = (klatt->mNatglotB * klatt->mNOpen) * .333f;

        /* Reset width of "impulsive" glottal pulse */

        int temp;
        float temp1;

        temp = klatt->mSampleRate / klatt->mNOpen;
        tts_resonator_initResonator( &klatt->mCritDampedGlotLowPassFilter, 0L, temp, klatt->mSampleRate);

        /* Make gain at F1 about constant */

        temp1 = klatt->mNOpen * .00833f;
        tts_resonator_setGain( &klatt->mCritDampedGlotLowPassFilter, temp1 * temp1);

        /* Truncate skewness so as not to exceed duration of closed phase
        of glottal period */

        temp = klatt->mT0 - klatt->mNOpen;

        if (klatt->mSkewnessOfAlternatePeriods > temp) {
            klatt->mSkewnessOfAlternatePeriods = temp;
        }

        if (klatt->mSkew >= 0) {
            klatt->mSkew = klatt->mSkewnessOfAlternatePeriods;                /* Reset mSkew to requested mSkewnessOfAlternatePeriods */
        } else {
            klatt->mSkew = -klatt->mSkewnessOfAlternatePeriods;
        }

        /* Add skewness to closed portion of voicing period */

        klatt->mT0 = klatt->mT0 + klatt->mSkew;
        klatt->mSkew = -klatt->mSkew;
    } else {
        klatt->mT0 = 4;                        /* Default for f0 undefined */
        klatt->mAmpVoice = 0.0;
        klatt->mNMod = klatt->mT0;
        klatt->mAmpBreth = 0.0;
        klatt->mNatglotA = 0.0;
        klatt->mNatglotB = 0.0;
    }

    /* Reset these pars pitch synchronously or at update rate if f0=0 */

    if( (klatt->mT0 != 4) || (ns == 0) ) {
        /* Set one-pole TTS_ELM_FEATURE_LOW-pass filter that tilts glottal source */
        klatt->mDecay = (0.033f * frame->mVoicingSpectralTiltdb);  /* Function of samp_rate ? */

        if (klatt->mDecay > 0.0f) {
            klatt->mOneMd = 1.0f - klatt->mDecay;
        } else {
            klatt->mOneMd = 1.0f;
        }
    }
}


/* Get variable parameters from host computer,
initially also get definition of fixed pars
*/

static void tts_klatt_frame_setup( struct tts_klatt_t* klatt, struct tts_klatt_frame_t* frame ) {
    int mOverallGaindb;                       /* Overall gain, 60 dB is unity  0 to   60  */
    float amp_parF1;                 /* mFormant1Ampdb converted to linear gain  */
    float amp_parFN;                 /* mParallelNasalPoleAmpdb converted to linear gain  */
    float amp_parF2;                 /* mFormant2Ampdb converted to linear gain  */
    float amp_parF3;                 /* mFormant3Ampdb converted to linear gain  */
    float amp_parF4;                 /* mFormant4Ampdb converted to linear gain  */
    float amp_parF5;                 /* mFormant5Ampdb converted to linear gain  */
    float amp_parF6;                 /* mFormant6Ampdb converted to linear gain  */

    /* Read  speech frame definition into temp store
       and move some parameters into active use immediately
       (voice-excited ones are updated pitch synchronously
       to avoid waveform glitches).
     */

    klatt->mF0FundamentalFreq = frame->mF0FundamentalFreq;
    klatt->mVoicingAmpdb = frame->mVoicingAmpdb - 7;

    if (klatt->mVoicingAmpdb < 0) klatt->mVoicingAmpdb = 0;

    klatt->mAmpAspir = tts_DBtoLIN(frame->mAspirationAmpdb) * .05f;
    klatt->mAmpFrica = tts_DBtoLIN(frame->mFricationAmpdb) * 0.25f;
    klatt->mSkewnessOfAlternatePeriods = frame->mSkewnessOfAlternatePeriods;

    /* Fudge factors (which comprehend affects of formants on each other?)
       with these in place TTS_ALL_PARALLEL should sound as close as
       possible to TTS_CASCADE_PARALLEL.
       Possible problem feeding in Holmes's amplitudes given this.
    */
    amp_parF1 = tts_DBtoLIN(frame->mFormant1Ampdb) * 0.4f;  /* -7.96 dB */
    amp_parF2 = tts_DBtoLIN(frame->mFormant2Ampdb) * 0.15f; /* -16.5 dB */
    amp_parF3 = tts_DBtoLIN(frame->mFormant3Ampdb) * 0.06f; /* -24.4 dB */
    amp_parF4 = tts_DBtoLIN(frame->mFormant4Ampdb) * 0.04f; /* -28.0 dB */
    amp_parF5 = tts_DBtoLIN(frame->mFormant5Ampdb) * 0.022f;    /* -33.2 dB */
    amp_parF6 = tts_DBtoLIN(frame->mFormant6Ampdb) * 0.03f; /* -30.5 dB */
    amp_parFN = tts_DBtoLIN(frame->mParallelNasalPoleAmpdb) * 0.6f; /* -4.44 dB */
    klatt->mAmpBypas = tts_DBtoLIN(frame->mBypassFricationAmpdb) * 0.05f;  /* -26.0 db */

    // Set coeficients of nasal resonator and zero antiresonator 
    tts_resonator_initResonator( &klatt->mNasalPole, frame->mNasalPoleFreq, frame->mNasalPoleBandwidth, klatt->mSampleRate);

    tts_resonator_initAntiresonator( &klatt->mNasalZero, frame->mNasalZeroFreq, frame->mNasalZeroBandwidth, klatt->mSampleRate);

    // Set coefficients of parallel resonators, and amplitude of outputs 
    tts_resonator_initResonator( &klatt->mParallelFormant1, frame->mFormant1Freq, frame->mFormant1ParallelBandwidth, klatt->mSampleRate);
    tts_resonator_setGain( &klatt->mParallelFormant1, amp_parF1);

    tts_resonator_initResonator( &klatt->mParallelResoNasalPole, frame->mNasalPoleFreq, frame->mNasalPoleBandwidth, klatt->mSampleRate);
    tts_resonator_setGain( &klatt->mParallelResoNasalPole, amp_parFN);

    tts_resonator_initResonator( &klatt->mParallelFormant2, frame->mFormant2Freq, frame->mFormant2ParallelBandwidth, klatt->mSampleRate);
    tts_resonator_setGain( &klatt->mParallelFormant2, amp_parF2);

    tts_resonator_initResonator( &klatt->mParallelFormant3, frame->mFormant3Freq, frame->mFormant3ParallelBandwidth, klatt->mSampleRate);
    tts_resonator_setGain( &klatt->mParallelFormant3, amp_parF3);

    tts_resonator_initResonator( &klatt->mParallelFormant4, frame->mFormant4Freq, frame->mFormant4ParallelBandwidth, klatt->mSampleRate);
    tts_resonator_setGain( &klatt->mParallelFormant4, amp_parF4);

    tts_resonator_initResonator( &klatt->mParallelFormant5, frame->mFormant5Freq, frame->mFormant5ParallelBandwidth, klatt->mSampleRate);
    tts_resonator_setGain( &klatt->mParallelFormant5, amp_parF5);

    tts_resonator_initResonator( &klatt->mParallelFormant6, frame->mFormant6Freq, frame->mFormant6ParallelBandwidth, klatt->mSampleRate);
    tts_resonator_setGain( &klatt->mParallelFormant6, amp_parF6);


    /* fold overall gain into output resonator */
    mOverallGaindb = frame->mOverallGaindb - 3;

    if (mOverallGaindb <= 0)
        mOverallGaindb = 57;

    /* output TTS_ELM_FEATURE_LOW-pass filter - resonator with freq 0 and BW = globals->mSampleRate
    Thus 3db point is globals->mSampleRate/2 i.e. Nyquist limit.
    Only 3db down seems rather mild...
    */
    tts_resonator_initResonator( &klatt->mOutputLowPassFilter, 0L, (int) klatt->mSampleRate, klatt->mSampleRate);
    tts_resonator_setGain( &klatt->mOutputLowPassFilter, tts_DBtoLIN(mOverallGaindb));
}

/*
function PARWAV

CONVERT FRAME OF PARAMETER DATA TO A WAVEFORM CHUNK
Synthesize globals->mNspFr samples of waveform and store in jwave[].
*/

static void tts_klatt_parwave(struct tts_klatt_t* klatt, struct tts_klatt_frame_t* frame, short int *jwave) {
    /* Output of cascade branch, also final output  */

    /* Initialize synthesizer and get specification for current speech
    frame from host microcomputer */

    tts_klatt_frame_setup( klatt, frame );

    if( klatt->mF0Flutter != 0 ) {
        klatt->mTimeCount++;                  /* used for f0 flutter */
        tts_klatt_flutter( klatt, frame );       /* add f0 flutter */
    }

    /* MAIN LOOP, for each output sample of current frame: */

    int ns;
    for( ns = 0; ns < klatt->mNspFr; ns++ ) {
        // TODO: get rid of static var
        static unsigned int seed = 5; /* Fixed staring value */
        float noise;
        int n4;
        float sourc;                   /* Sound source if all-parallel config used  */
        float glotout;                 /* Output of glottal sound source  */
        float par_glotout;             /* Output of parallelglottal sound sourc  */
        float voice = 0;               /* Current sample of voicing waveform  */
        float frics;                   /* Frication sound source  */
        float aspiration;              /* Aspiration sound source  */
        int nrand;                    /* Varible used by random number generator  */

        /* Our own code like rand(), but portable
        whole upper 31 bits of seed random
        assumes 32-bit unsigned arithmetic
        with untested code to handle larger.
        */
        seed = ( seed * 1664525 + 1 ) & 0xFFFFFFFF;
        
        /* Shift top bits of seed up to top of int then back down to LS 14 bits */
        /* Assumes 8 bits per sizeof unit i.e. a "byte" */
        nrand = (((int) seed) << (8 * sizeof(int) - 32)) >> (8 * sizeof(int) - 14);

        /* Tilt down noise spectrum by soft TTS_ELM_FEATURE_LOW-pass filter having
        *    a pole near the origin in the z-plane, i.e.
        *    output = input + (0.75 * lastoutput) */

        noise = nrand + (0.75f * klatt->mNLast);   /* Function of samp_rate ? */

        klatt->mNLast = noise;

        /* Amplitude modulate noise (reduce noise amplitude during
        second half of glottal period) if voicing simultaneously present
        */

        if( klatt->mNPer > klatt->mNMod ) {
            noise *= 0.5f;
        }

        /* Compute frication noise */
        sourc = frics = klatt->mAmpFrica * noise;

        /* Compute voicing waveform : (run glottal source simulation at
        4 times normal sample rate to minimize quantization noise in
        period of female voice)
        */

        for( n4 = 0; n4 < 4; n4++ ) {
            /* use a more-natural-shaped source waveform with excitation
            occurring both upon opening and upon closure, stronest at closure */
            voice = tts_klatt_natural_source(klatt, klatt->mNPer);

            /* Reset period when counter 'mNPer' reaches mT0 */

            if( klatt->mNPer >= klatt->mT0 ) {
                klatt->mNPer = 0;
                tts_klatt_pitch_synch_par_reset( klatt, frame, ns );
            }

            /* Low-pass filter voicing waveform before downsampling from 4*globals->mSampleRate */
            /* to globals->mSampleRate samples/sec.  Resonator f=.09*globals->mSampleRate, bw=.06*globals->mSampleRate  */

            voice = tts_resonator_resonate( &klatt->mDownSampLowPassFilter, voice); /* in=voice, out=voice */

            /* Increment counter that keeps track of 4*globals->mSampleRate samples/sec */
            klatt->mNPer++;
        }

        /* Tilt spectrum of voicing source down by soft TTS_ELM_FEATURE_LOW-pass filtering, amount
        of tilt determined by mVoicingSpectralTiltdb
        */
        voice = (voice * klatt->mOneMd) + (klatt->mVLast * klatt->mDecay);

        klatt->mVLast = voice;

        /* Add breathiness during glottal open phase */
        if( klatt->mNPer < klatt->mNOpen ) {
            /* Amount of breathiness determined by parameter mVoicingBreathiness */
            /* Use nrand rather than noise because noise is TTS_ELM_FEATURE_LOW-passed */
            voice += klatt->mAmpBreth * nrand;
        }

        /* Set amplitude of voicing */
        glotout = klatt->mAmpVoice * voice;

        /* Compute aspiration amplitude and add to voicing source */
        aspiration = klatt->mAmpAspir * noise;

        glotout += aspiration;

        par_glotout = glotout;

        /* NIS - rsynth "hack"
        As Holmes' scheme is weak at nasals and (physically) nasal cavity
        is "back near glottis" feed glottal source through nasal resonators
        Don't think this is quite right, but improves things a bit
        */
        par_glotout = tts_resonator_antiresonate( &klatt->mNasalZero, par_glotout);
        par_glotout = tts_resonator_resonate( &klatt->mNasalPole, par_glotout);
        /* And just use mParallelFormant1 NOT mParallelResoNasalPole */     
        float out = tts_resonator_resonate( &klatt->mParallelFormant1, par_glotout);
        /* Sound sourc for other parallel resonators is frication
        plus first difference of voicing waveform.
        */
        sourc += (par_glotout - klatt->mGlotLast);
        klatt->mGlotLast = par_glotout;

        /* Standard parallel vocal tract
        Formants F6,F5,F4,F3,F2, outputs added with alternating sign
        */
        out = tts_resonator_resonate( &klatt->mParallelFormant6, sourc) - out;
        out = tts_resonator_resonate( &klatt->mParallelFormant5, sourc) - out;
        out = tts_resonator_resonate( &klatt->mParallelFormant4, sourc) - out;
        out = tts_resonator_resonate( &klatt->mParallelFormant3, sourc) - out;
        out = tts_resonator_resonate( &klatt->mParallelFormant2, sourc) - out;
        out = klatt->mAmpBypas * sourc - out;
        out = tts_resonator_resonate( &klatt->mOutputLowPassFilter, out);

        *jwave++ = tts_clip(out); /* Convert back to integer */
    }
}



static char * tts_phoneme_to_element_lookup( char *s, void ** data ) {
    int key8 = *s;
    int key16 = key8 + (s[1] << 8);
    if (s[1] == 0) key16 = -1; // avoid key8==key16
    int i;
    for (i = 0; i < TTS_PHONEME_COUNT; i++) {
        if (tts_phoneme_to_elements[i].mKey == key16) {
            *data = &tts_phoneme_to_elements[i].mData;
            return s+2;
        }
        if (tts_phoneme_to_elements[i].mKey == key8) {
            *data = &tts_phoneme_to_elements[i].mData;
            return s+1;
        }
    }
    // should never happen
    *data = NULL;
    return s+1;
}



static int tts_phone_to_elm( char *aPhoneme, int aCount, struct tts_darray_t *aElement ) {
    int stress = 0;
    char *s = aPhoneme;
    int t = 0;
    char *limit = s + aCount;

    while (s < limit && *s) {
        char *e = NULL;
        s = tts_phoneme_to_element_lookup(s, (void**)&e);

        if (e) {
            int n = *e++;

            while (n-- > 0) {
                int x = *e++;
                struct tts_element_t * p = &tts_gElement[x];
                /* This works because only vowels have mUD != mDU,
                and we set stress just before a vowel
                */
                tts_darray_put( aElement, x );

                if (!(p->mFeat & TTS_ELM_FEATURE_VWL))
                    stress = 0;

                int stressdur = TTS_StressDur(p);

                t += stressdur;

                tts_darray_put( aElement, stressdur);
                tts_darray_put( aElement, stress);
            }
        } else {
            char ch = *s++;

            switch (ch) {
                case '\'':                /* Primary stress */
                    stress = 3;
                    break;
                case ',':                 /* Secondary stress */
                    stress = 2;
                    break;
                case '+':                 /* Tertiary stress */
                    stress = 1;
                    break;
                case '-':                 /* hyphen in input */
                    break;
                default:
//                  fprintf(stderr, "Ignoring %c in '%.*s'\n", ch, aCount, aPhoneme);
                    break;
            }
        }
    }

    return t;
}



/* 'a' is dominant element, 'b' is dominated
    ext is flag to say to use external times from 'a' rather
    than internal i.e. ext != 0 if 'a' is NOT current element.
 */

static void tts_set_trans( struct tts_slope_t* t, struct tts_element_t* a, struct tts_element_t* b, int ext, int e ) {
    (void) e;
    int i;

    for (i = 0; i < TTS_ELM_COUNT; i++) {
        t[i].mTime = ((ext) ? a->mInterpolator[i].mExtDelay : a->mInterpolator[i].mIntDelay);

        if (t[i].mTime) {
            t[i].mValue = a->mInterpolator[i].mFixed + (a->mInterpolator[i].mProportion * b->mInterpolator[i].mSteady) * 0.01f; // mProportion is in scale 0..100, so *0.01.
        } else {
            t[i].mValue = b->mInterpolator[i].mSteady;
        }
    }
}


static float tts_lerp( float a, float b, int t, int d ) {
    if (t <= 0) {
        return a;
    }

    if (t >= d) {
        return b;
    }

    float f = (float)t / (float)d;
    return a + (b - a) * f;
}


static float tts_interpolate( struct tts_slope_t* aStartSlope, struct tts_slope_t* aEndSlope, float aMidValue, int aTime, int aDuration )
{
    int steadyTime = aDuration - (aStartSlope->mTime + aEndSlope->mTime);

    if( steadyTime >= 0 ) {
        // Interpolate to a midpoint, stay there for a while, then tts_interpolate to end

        if( aTime < aStartSlope->mTime ) {
            // tts_interpolate to the first value
            return tts_lerp(aStartSlope->mValue, aMidValue, aTime, aStartSlope->mTime);
        }
        // reached midpoint

        aTime -= aStartSlope->mTime;

        if( aTime <= steadyTime ) {
            // still at steady state
            return aMidValue;  
        }

        // tts_interpolate to the end
        return tts_lerp(aMidValue, aEndSlope->mValue, aTime - steadyTime, aEndSlope->mTime);
    } else {
        // No steady state
        float f = 1.0f - ((float) aTime / (float) aDuration);
        float sp = tts_lerp(aStartSlope->mValue, aMidValue, aTime, aStartSlope->mTime);
        float ep = tts_lerp(aEndSlope->mValue, aMidValue, aDuration - aTime, aEndSlope->mTime);
        return f * sp + ((float) 1.0 - f) * ep;
    }
}



static void tts_klatt_initsynth( struct tts_klatt_t* klatt, int aElementCount,unsigned char *aElement ) {
    klatt->mElement = aElement;
    klatt->mElementCount = aElementCount;
    klatt->mElementIndex = 0;
    klatt->mLastElement = &tts_gElement[0];
    klatt->mTStress = 0;
    klatt->mNTStress = 0;
    klatt->mKlattFramePars.mF0FundamentalFreq = 1330;
    klatt->mTop = 1.1f * klatt->mKlattFramePars.mF0FundamentalFreq;
    klatt->mKlattFramePars.mNasalPoleFreq = (int)klatt->mLastElement->mInterpolator[TTS_ELM_FN].mSteady;
    klatt->mKlattFramePars.mFormant1ParallelBandwidth = klatt->mKlattFramePars.mFormant1Bandwidth = 60;
    klatt->mKlattFramePars.mFormant2ParallelBandwidth = klatt->mKlattFramePars.mFormant2Bandwidth = 90;
    klatt->mKlattFramePars.mFormant3ParallelBandwidth = klatt->mKlattFramePars.mFormant3Bandwidth = 150;
//  klatt->mKlattFramePars.mFormant4ParallelBandwidth = (default)

    // Set stress attack/decay slope 
    klatt->mStressS.mTime = 40;
    klatt->mStressE.mTime = 40;
    klatt->mStressE.mValue = 0.0;
}


int tts_klatt_synth( struct tts_klatt_t* klatt, int aSampleCount, short *aSamplePointer) {
    (void) aSampleCount;

    short *samp = aSamplePointer;

    if( klatt->mElementIndex >= klatt->mElementCount )
        return -1;

    struct tts_element_t * currentElement = &tts_gElement[klatt->mElement[klatt->mElementIndex++]];
    int dur = klatt->mElement[klatt->mElementIndex++];
    klatt->mElementIndex++; // skip stress 

    // Skip zero length elements which are only there to affect
    // boundary values of adjacent elements     

    if (dur > 0) {
        struct tts_element_t* ne = (klatt->mElementIndex < klatt->mElementCount) ? &tts_gElement[klatt->mElement[klatt->mElementIndex]] : &tts_gElement[0];
        struct tts_slope_t start[TTS_ELM_COUNT];
        struct tts_slope_t end[TTS_ELM_COUNT];
        int t;

        if (currentElement->mRK > klatt->mLastElement->mRK) {
            tts_set_trans(start, currentElement, klatt->mLastElement, 0, 's');
            // we dominate last 
        } else {
            tts_set_trans(start, klatt->mLastElement, currentElement, 1, 's');
            // last dominates us 
        }

        if (ne->mRK > currentElement->mRK) {
            tts_set_trans(end, ne, currentElement, 1, 'e');
            // next dominates us 
        } else {
            tts_set_trans(end, currentElement, ne, 0, 'e');
            // we dominate next 
        }

        for (t = 0; t < dur; t++, klatt->mTStress++) {
            float base = klatt->mTop * 0.8f; // 3 * top / 5 
            float tp[TTS_ELM_COUNT];

            if (klatt->mTStress == klatt->mNTStress) {
                int j = klatt->mElementIndex;
                klatt->mStressS = klatt->mStressE;
                klatt->mTStress = 0;
                klatt->mNTStress = dur;

                while (j <= klatt->mElementCount) {
                    struct tts_element_t* e = (j < klatt->mElementCount) ? &tts_gElement[klatt->mElement[j++]] : &tts_gElement[0];
                    int du = (j < klatt->mElementCount) ? klatt->mElement[j++] : 0;
                    int s  = (j < klatt->mElementCount) ? klatt->mElement[j++] : 3;

                    if (s || e->mFeat & TTS_ELM_FEATURE_VWL) {
                        int d = 0;

                        if (s)
                            klatt->mStressE.mValue = (float) s / 3;
                        else
                            klatt->mStressE.mValue = (float) 0.1;

                        do {
                            d += du;
                            e = (j < klatt->mElementCount) ? &tts_gElement[klatt->mElement[j++]] : &tts_gElement[0];
                            du = klatt->mElement[j++];
                        } while ((e->mFeat & TTS_ELM_FEATURE_VWL) && klatt->mElement[j++] == s);

                        klatt->mNTStress += d / 2;

                        break;
                    }

                    klatt->mNTStress += du;
                }
            }

            for (int j = 0; j < TTS_ELM_COUNT; j++) {
                tp[j] = tts_interpolate(&start[j], &end[j], (float) currentElement->mInterpolator[j].mSteady, t, dur);
            }

            // Now call the synth for each frame 

            klatt->mKlattFramePars.mF0FundamentalFreq = (int)(base + (klatt->mTop - base) * tts_interpolate(&klatt->mStressS, &klatt->mStressE, (float) 0, klatt->mTStress, klatt->mNTStress));
            klatt->mKlattFramePars.mVoicingAmpdb = klatt->mKlattFramePars.mPalallelVoicingAmpdb = (int)tp[TTS_ELM_AV];
            klatt->mKlattFramePars.mFricationAmpdb = (int)tp[TTS_ELM_AF];
            klatt->mKlattFramePars.mNasalZeroFreq = (int)tp[TTS_ELM_FN];
            klatt->mKlattFramePars.mAspirationAmpdb = (int)tp[TTS_ELM_ASP];
            klatt->mKlattFramePars.mVoicingBreathiness = (int)tp[TTS_ELM_AVC];
            klatt->mKlattFramePars.mFormant1ParallelBandwidth = klatt->mKlattFramePars.mFormant1Bandwidth = (int)tp[TTS_ELM_B1];
            klatt->mKlattFramePars.mFormant2ParallelBandwidth = klatt->mKlattFramePars.mFormant2Bandwidth = (int)tp[TTS_ELM_B2];
            klatt->mKlattFramePars.mFormant3ParallelBandwidth = klatt->mKlattFramePars.mFormant3Bandwidth = (int)tp[TTS_ELM_B3];
            klatt->mKlattFramePars.mFormant1Freq = (int)tp[TTS_ELM_F1];
            klatt->mKlattFramePars.mFormant2Freq = (int)tp[TTS_ELM_F2];
            klatt->mKlattFramePars.mFormant3Freq = (int)tp[TTS_ELM_F3];

            // TTS_AMP_ADJ + is a kludge to get amplitudes up to klatt-compatible levels
                
                
            //pars.mParallelNasalPoleAmpdb  = TTS_AMP_ADJ + tp[TTS_ELM_AN];
                
            klatt->mKlattFramePars.mBypassFricationAmpdb = TTS_AMP_ADJ + (int)tp[TTS_ELM_AB];
            klatt->mKlattFramePars.mFormant5Ampdb = TTS_AMP_ADJ + (int)tp[TTS_ELM_A5];
            klatt->mKlattFramePars.mFormant6Ampdb = TTS_AMP_ADJ + (int)tp[TTS_ELM_A6];
            klatt->mKlattFramePars.mFormant1Ampdb = TTS_AMP_ADJ + (int)tp[TTS_ELM_A1];
            klatt->mKlattFramePars.mFormant2Ampdb = TTS_AMP_ADJ + (int)tp[TTS_ELM_A2];
            klatt->mKlattFramePars.mFormant3Ampdb = TTS_AMP_ADJ + (int)tp[TTS_ELM_A3];
            klatt->mKlattFramePars.mFormant4Ampdb = TTS_AMP_ADJ + (int)tp[TTS_ELM_A4];

            tts_klatt_parwave( klatt, &klatt->mKlattFramePars, samp);

            samp += klatt->mNspFr;

            // Declination of f0 envelope 0.25Hz / cS 
            klatt->mTop -= 0.5;
        }
    }

    klatt->mLastElement = currentElement;

    /* MG: BEGIN REINIT AT LONG PAUSE */
    if( currentElement->mName[0] == 'Q' && currentElement->mName[1] == '\0' ) {
        ++klatt->count; 
    } else {
        klatt->count = 0;
    }
    if( klatt->count == 3 ) {
        klatt->mTStress = 0;
        klatt->mNTStress = 0;
        klatt->mKlattFramePars.mF0FundamentalFreq = 1330;
        klatt->mTop = 1.1f * klatt->mKlattFramePars.mF0FundamentalFreq;
        klatt->mKlattFramePars.mNasalPoleFreq = (int)klatt->mLastElement->mInterpolator[TTS_ELM_FN].mSteady;
        klatt->mKlattFramePars.mFormant1ParallelBandwidth = klatt->mKlattFramePars.mFormant1Bandwidth = 60;
        klatt->mKlattFramePars.mFormant2ParallelBandwidth = klatt->mKlattFramePars.mFormant2Bandwidth = 90;
        klatt->mKlattFramePars.mFormant3ParallelBandwidth = klatt->mKlattFramePars.mFormant3Bandwidth = 150;
    //  mKlattFramePars.mFormant4ParallelBandwidth = (default)

        // Set stress attack/decay slope 
        klatt->mStressS.mTime = 40;
        klatt->mStressE.mTime = 40;
        klatt->mStressE.mValue = 0.0;
    }
    /* MG: END REINIT AT LONG PAUSE */

    return (int)(samp - aSamplePointer);
}


static void tts_klatt_init( struct tts_klatt_t* klatt ) {
    memset( klatt, 0, sizeof( *klatt ) );
    tts_klatt_frame_init( &klatt->mKlattFramePars );
    klatt->mSampleRate = 11025;
    klatt->mF0Flutter = 0;

    int FLPhz = (950 * klatt->mSampleRate) / 10000;
    int BLPhz = (630 * klatt->mSampleRate) / 10000;
    klatt->mNspFr = (int)(klatt->mSampleRate * 10) / 1000;

    tts_resonator_initResonator( &klatt->mDownSampLowPassFilter, FLPhz, BLPhz, klatt->mSampleRate );

    klatt->mNPer = 0;                        /* LG */
    klatt->mT0 = 0;                          /* LG */

    klatt->mVLast = 0;                       /* Previous output of voice  */
    klatt->mNLast = 0;                       /* Previous output of random number generator  */
    klatt->mGlotLast = 0;                    /* Previous value of glotout  */
    klatt->count = 0;
}



static const char *tts_ASCII[] = {
    "null", "", "", "",
    "", "", "", "",
    "", "", "", "",
    "", "", "", "",
    "", "", "", "",
    "", "", "", "",
    "", "", "", "",
    "", "", "", "",
    "space", "exclamation mark", "double quote", "hash",
    "dollar", "percent", "ampersand", "quote",
    "open parenthesis", "close parenthesis", "asterisk", "plus",
    "comma", "minus", "full stop", "slash",
    "zero", "one", "two", "three",
    "four", "five", "six", "seven",
    "eight", "nine", "colon", "semi colon",
    "less than", "equals", "greater than", "question mark",
#ifndef TTS_ALPHA_IN_DICT
    "at", "ay", "bee", "see",
    "dee", "e", "eff", "gee",
    "aych", "i", "jay", "kay",
    "ell", "em", "en", "ohe",
    "pee", "kju", "are", "es",
    "tee", "you", "vee", "double you",
    "eks", "why", "zed", "open bracket",
#else                             /* TTS_ALPHA_IN_DICT */
    "at", "A", "B", "C",
    "D", "E", "F", "G",
    "H", "I", "J", "K",
    "L", "M", "N", "O",
    "P", "Q", "R", "S",
    "T", "U", "V", "W",
    "X", "Y", "Z", "open bracket",
#endif                            /* TTS_ALPHA_IN_DICT */
    "back slash", "close bracket", "circumflex", "underscore",
#ifndef TTS_ALPHA_IN_DICT
    "back quote", "ay", "bee", "see",
    "dee", "e", "eff", "gee",
    "aych", "i", "jay", "kay",
    "ell", "em", "en", "ohe",
    "pee", "kju", "are", "es",
    "tee", "you", "vee", "double you",
    "eks", "why", "zed", "open brace",
#else                             /* TTS_ALPHA_IN_DICT */
    "back quote", "A", "B", "C",
    "D", "E", "F", "G",
    "H", "I", "J", "K",
    "L", "M", "N", "O",
    "P", "Q", "R", "S",
    "T", "U", "V", "W",
    "X", "Y", "Z", "open brace",
#endif                            /* TTS_ALPHA_IN_DICT */
    "vertical bar", "close brace", "tilde", "delete",
    NULL
};

/* Context definitions */
static const char tts_Anything[] = "";
/* No context requirement */

static const char tts_Nothing[] = " ";
/* Context is beginning or end of word */

static const char tts_Silent[] = "";
/* No phonemes */


#define TTS_LEFT_PART       0
#define TTS_MATCH_PART      1
#define TTS_RIGHT_PART      2
#define TTS_OUT_PART        3

typedef const char *tts_Rule[4];
/* tts_Rule is an array of 4 character pointers */


/*0 = Punctuation */
/*
**      TTS_LEFT_PART       TTS_MATCH_PART      TTS_RIGHT_PART      TTS_OUT_PART
*/


static tts_Rule tts_punct_rules[] = {
    {tts_Anything, " ", tts_Anything, " "},
    {tts_Anything, "-", tts_Anything, ""},
    {".", "'S", tts_Anything, "z"},
    {"#:.E", "'S", tts_Anything, "z"},
    {"#", "'S", tts_Anything, "z"},
    {tts_Anything, "'", tts_Anything, ""},
    {tts_Anything, ",", tts_Anything, " "},
    {tts_Anything, ".", tts_Anything, " "},
    {tts_Anything, "?", tts_Anything, " "},
    {tts_Anything, "!", tts_Anything, " "},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_A_rules[] = {
    {tts_Anything, "A", tts_Nothing, "@"},
    {tts_Nothing, "ARE", tts_Nothing, "0r"},
    {tts_Nothing, "AR", "O", "@r"},
    {tts_Anything, "AR", "#", "er"},
    {"^", "AS", "#", "eIs"},
    {tts_Anything, "A", "WA", "@"},
    {tts_Anything, "AW", tts_Anything, "O"},
    {" :", "ANY", tts_Anything, "eni"},
    {tts_Anything, "A", "^+#", "eI"},
    {"#:", "ALLY", tts_Anything, "@li"},
    {tts_Nothing, "AL", "#", "@l"},
    {tts_Anything, "AGAIN", tts_Anything, "@gen"},
    {"#:", "AG", "E", "IdZ"},
    {tts_Anything, "A", "^+:#", "&"},
    {" :", "A", "^+ ", "eI"},
    {tts_Anything, "A", "^%", "eI"},
    {tts_Nothing, "ARR", tts_Anything, "@r"},
    {tts_Anything, "ARR", tts_Anything, "&r"},
    {" :", "AR", tts_Nothing, "0r"},
    {tts_Anything, "AR", tts_Nothing, "3"},
    {tts_Anything, "AR", tts_Anything, "0r"},
    {tts_Anything, "AIR", tts_Anything, "er"},
    {tts_Anything, "AI", tts_Anything, "eI"},
    {tts_Anything, "AY", tts_Anything, "eI"},
    {tts_Anything, "AU", tts_Anything, "O"},
    {"#:", "AL", tts_Nothing, "@l"},
    {"#:", "ALS", tts_Nothing, "@lz"},
    {tts_Anything, "ALK", tts_Anything, "Ok"},
    {tts_Anything, "AL", "^", "Ol"},
    {" :", "ABLE", tts_Anything, "eIb@l"},
    {tts_Anything, "ABLE", tts_Anything, "@b@l"},
    {tts_Anything, "ANG", "+", "eIndZ"},
    {"^", "A", "^#", "eI"},
    {tts_Anything, "A", tts_Anything, "&"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_B_rules[] = {
    {tts_Nothing, "BE", "^#", "bI"},
    {tts_Anything, "BEING", tts_Anything, "biIN"},
    {tts_Nothing, "BOTH", tts_Nothing, "b@UT"},
    {tts_Nothing, "BUS", "#", "bIz"},
    {tts_Anything, "BUIL", tts_Anything, "bIl"},
    {tts_Anything, "B", tts_Anything, "b"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_C_rules[] = {
    {tts_Nothing, "CH", "^", "k"},
    {"^E", "CH", tts_Anything, "k"},
    {tts_Anything, "CH", tts_Anything, "tS"},
    {" S", "CI", "#", "saI"},
    {tts_Anything, "CI", "A", "S"},
    {tts_Anything, "CI", "O", "S"},
    {tts_Anything, "CI", "EN", "S"},
    {tts_Anything, "C", "+", "s"},
    {tts_Anything, "CK", tts_Anything, "k"},
    {tts_Anything, "COM", "%", "kVm"},
    {tts_Anything, "C", tts_Anything, "k"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_D_rules[] = {
    {"#:", "DED", tts_Nothing, "dId"},
    {".E", "D", tts_Nothing, "d"},
    {"#:^E", "D", tts_Nothing, "t"},
    {tts_Nothing, "DE", "^#", "dI"},
    {tts_Nothing, "DO", tts_Nothing, "mDU"},
    {tts_Nothing, "DOES", tts_Anything, "dVz"},
    {tts_Nothing, "DOING", tts_Anything, "duIN"},
    {tts_Nothing, "DOW", tts_Anything, "daU"},
    {tts_Anything, "DU", "A", "dZu"},
    {tts_Anything, "D", tts_Anything, "d"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_E_rules[] = {
    {"#:", "E", tts_Nothing, ""},
    {"':^", "E", tts_Nothing, ""},
    {" :", "E", tts_Nothing, "i"},
    {"#", "ED", tts_Nothing, "d"},
    {"#:", "E", "D ", ""},
    {tts_Anything, "EV", "ER", "ev"},
    {tts_Anything, "E", "^%", "i"},
    {tts_Anything, "ERI", "#", "iri"},
    {tts_Anything, "ERI", tts_Anything, "erI"},
    {"#:", "ER", "#", "3"},
    {tts_Anything, "ER", "#", "er"},
    {tts_Anything, "ER", tts_Anything, "3"},
    {tts_Nothing, "EVEN", tts_Anything, "iven"},
    {"#:", "E", "W", ""},
    {"T", "EW", tts_Anything, "u"},
    {"S", "EW", tts_Anything, "u"},
    {"R", "EW", tts_Anything, "u"},
    {"D", "EW", tts_Anything, "u"},
    {"L", "EW", tts_Anything, "u"},
    {"Z", "EW", tts_Anything, "u"},
    {"N", "EW", tts_Anything, "u"},
    {"J", "EW", tts_Anything, "u"},
    {"TH", "EW", tts_Anything, "u"},
    {"CH", "EW", tts_Anything, "u"},
    {"SH", "EW", tts_Anything, "u"},
    {tts_Anything, "EW", tts_Anything, "ju"},
    {tts_Anything, "E", "O", "i"},
    {"#:S", "ES", tts_Nothing, "Iz"},
    {"#:C", "ES", tts_Nothing, "Iz"},
    {"#:G", "ES", tts_Nothing, "Iz"},
    {"#:Z", "ES", tts_Nothing, "Iz"},
    {"#:X", "ES", tts_Nothing, "Iz"},
    {"#:J", "ES", tts_Nothing, "Iz"},
    {"#:CH", "ES", tts_Nothing, "Iz"},
    {"#:SH", "ES", tts_Nothing, "Iz"},
    {"#:", "E", "S ", ""},
    {"#:", "ELY", tts_Nothing, "li"},
    {"#:", "EMENT", tts_Anything, "ment"},
    {tts_Anything, "EFUL", tts_Anything, "fUl"},
    {tts_Anything, "EE", tts_Anything, "i"},
    {tts_Anything, "EARN", tts_Anything, "3n"},
    {tts_Nothing, "EAR", "^", "3"},
    {tts_Anything, "EAD", tts_Anything, "ed"},
    {"#:", "EA", tts_Nothing, "i@"},
    {tts_Anything, "EA", "SU", "e"},
    {tts_Anything, "EA", tts_Anything, "i"},
    {tts_Anything, "EIGH", tts_Anything, "eI"},
    {tts_Anything, "EI", tts_Anything, "i"},
    {tts_Nothing, "EYE", tts_Anything, "aI"},
    {tts_Anything, "EY", tts_Anything, "i"},
    {tts_Anything, "EU", tts_Anything, "ju"},
    {tts_Anything, "E", tts_Anything, "e"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_F_rules[] = {
    {tts_Anything, "FUL", tts_Anything, "fUl"},
    {tts_Anything, "F", tts_Anything, "f"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_G_rules[] = {
    {tts_Anything, "GIV", tts_Anything, "gIv"},
    {tts_Nothing, "G", "I^", "g"},
    {tts_Anything, "GE", "T", "ge"},
    {"SU", "GGES", tts_Anything, "gdZes"},
    {tts_Anything, "GG", tts_Anything, "g"},
    {" B#", "G", tts_Anything, "g"},
    {tts_Anything, "G", "+", "dZ"},
    {tts_Anything, "GREAT", tts_Anything, "greIt"},
    {"#", "GH", tts_Anything, ""},
    {tts_Anything, "G", tts_Anything, "g"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_H_rules[] = {
    {tts_Nothing, "HAV", tts_Anything, "h&v"},
    {tts_Nothing, "HERE", tts_Anything, "hir"},
    {tts_Nothing, "HOUR", tts_Anything, "aU3"},
    {tts_Anything, "HOW", tts_Anything, "haU"},
    {tts_Anything, "H", "#", "h"},
    {tts_Anything, "H", tts_Anything, ""},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_I_rules[] = {
    {tts_Nothing, "IAIN", tts_Nothing, "I@n"},
    {tts_Nothing, "ING", tts_Nothing, "IN"},
    {tts_Nothing, "IN", tts_Anything, "In"},
    {tts_Nothing, "I", tts_Nothing, "aI"},
    {tts_Anything, "IN", "D", "aIn"},
    {tts_Anything, "IER", tts_Anything, "i3"},
    {"#:R", "IED", tts_Anything, "id"},
    {tts_Anything, "IED", tts_Nothing, "aId"},
    {tts_Anything, "IEN", tts_Anything, "ien"},
    {tts_Anything, "IE", "T", "aIe"},
    {" :", "I", "%", "aI"},
    {tts_Anything, "I", "%", "i"},
    {tts_Anything, "IE", tts_Anything, "i"},
    {tts_Anything, "I", "^+:#", "I"},
    {tts_Anything, "IR", "#", "aIr"},
    {tts_Anything, "IZ", "%", "aIz"},
    {tts_Anything, "IS", "%", "aIz"},
    {tts_Anything, "I", "D%", "aI"},
    {"+^", "I", "^+", "I"},
    {tts_Anything, "I", "T%", "aI"},
    {"#:^", "I", "^+", "I"},
    {tts_Anything, "I", "^+", "aI"},
    {tts_Anything, "IR", tts_Anything, "3"},
    {tts_Anything, "IGH", tts_Anything, "aI"},
    {tts_Anything, "ILD", tts_Anything, "aIld"},
    {tts_Anything, "IGN", tts_Nothing, "aIn"},
    {tts_Anything, "IGN", "^", "aIn"},
    {tts_Anything, "IGN", "%", "aIn"},
    {tts_Anything, "IQUE", tts_Anything, "ik"},
    {"^", "I", "^#", "aI"},
    {tts_Anything, "I", tts_Anything, "I"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_J_rules[] = {
    {tts_Anything, "J", tts_Anything, "dZ"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_K_rules[] = {
    {tts_Nothing, "K", "N", ""},
    {tts_Anything, "K", tts_Anything, "k"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_L_rules[] = {
    {tts_Anything, "LO", "C#", "l@U"},
    {"L", "L", tts_Anything, ""},
    {"#:^", "L", "%", "@l"},
    {tts_Anything, "LEAD", tts_Anything, "lid"},
    {tts_Anything, "L", tts_Anything, "l"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_M_rules[] = {
    {tts_Anything, "MOV", tts_Anything, "muv"},
    {"#", "MM", "#", "m"},
    {tts_Anything, "M", tts_Anything, "m"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_N_rules[] = {
    {"E", "NG", "+", "ndZ"},
    {tts_Anything, "NG", "R", "Ng"},
    {tts_Anything, "NG", "#", "Ng"},
    {tts_Anything, "NGL", "%", "Ng@l"},
    {tts_Anything, "NG", tts_Anything, "N"},
    {tts_Anything, "NK", tts_Anything, "Nk"},
    {tts_Nothing, "NOW", tts_Nothing, "naU"},
    {"#", "NG", tts_Nothing, "Ng"},
    {tts_Anything, "N", tts_Anything, "n"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_O_rules[] = {
    {tts_Anything, "OF", tts_Nothing, "@v"},
    {tts_Anything, "OROUGH", tts_Anything, "3@U"},
    {"#:", "OR", tts_Nothing, "3"},
    {"#:", "ORS", tts_Nothing, "3z"},
    {tts_Anything, "OR", tts_Anything, "Or"},
    {tts_Nothing, "ONE", tts_Anything, "wVn"},
    {tts_Anything, "OW", tts_Anything, "@U"},
    {tts_Nothing, "OVER", tts_Anything, "@Uv3"},
    {tts_Anything, "OV", tts_Anything, "Vv"},
    {tts_Anything, "O", "^%", "@U"},
    {tts_Anything, "O", "^EN", "@U"},
    {tts_Anything, "O", "^I#", "@U"},
    {tts_Anything, "OL", "D", "@Ul"},
    {tts_Anything, "OUGHT", tts_Anything, "Ot"},
    {tts_Anything, "OUGH", tts_Anything, "Vf"},
    {tts_Nothing, "OU", tts_Anything, "aU"},
    {"H", "OU", "S#", "aU"},
    {tts_Anything, "OUS", tts_Anything, "@s"},
    {tts_Anything, "OUR", tts_Anything, "Or"},
    {tts_Anything, "OULD", tts_Anything, "Ud"},
    {"^", "OU", "^L", "V"},
    {tts_Anything, "OUP", tts_Anything, "up"},
    {tts_Anything, "OU", tts_Anything, "aU"},
    {tts_Anything, "OY", tts_Anything, "oI"},
    {tts_Anything, "OING", tts_Anything, "@UIN"},
    {tts_Anything, "OI", tts_Anything, "oI"},
    {tts_Anything, "OOR", tts_Anything, "Or"},
    {tts_Anything, "OOK", tts_Anything, "Uk"},
    {tts_Anything, "OOD", tts_Anything, "Ud"},
    {tts_Anything, "OO", tts_Anything, "u"},
    {tts_Anything, "O", "E", "@U"},
    {tts_Anything, "O", tts_Nothing, "@U"},
    {tts_Anything, "OA", tts_Anything, "@U"},
    {tts_Nothing, "ONLY", tts_Anything, "@Unli"},
    {tts_Nothing, "ONCE", tts_Anything, "wVns"},
    {tts_Anything, "ON'T", tts_Anything, "@Unt"},
    {"C", "O", "N", "0"},
    {tts_Anything, "O", "NG", "O"},
    {" :^", "O", "N", "V"},
    {"I", "ON", tts_Anything, "@n"},
    {"#:", "ON", tts_Nothing, "@n"},
    {"#^", "ON", tts_Anything, "@n"},
    {tts_Anything, "O", "ST ", "@U"},
    {tts_Anything, "OF", "^", "Of"},
    {tts_Anything, "OTHER", tts_Anything, "VD3"},
    {tts_Anything, "OSS", tts_Nothing, "Os"},
    {"#:^", "OM", tts_Anything, "Vm"},
    {tts_Anything, "O", tts_Anything, "0"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_P_rules[] = {
    {tts_Anything, "PH", tts_Anything, "f"},
    {tts_Anything, "PEOP", tts_Anything, "pip"},
    {tts_Anything, "POW", tts_Anything, "paU"},
    {tts_Anything, "PUT", tts_Nothing, "pUt"},
    {tts_Anything, "P", tts_Anything, "p"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_Q_rules[] = {
    {tts_Anything, "QUAR", tts_Anything, "kwOr"},
    {tts_Anything, "QU", tts_Anything, "kw"},
    {tts_Anything, "Q", tts_Anything, "k"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_R_rules[] = {
    {tts_Nothing, "RE", "^#", "ri"},
    {tts_Anything, "R", tts_Anything, "r"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_S_rules[] = {
    {tts_Anything, "SH", tts_Anything, "S"},
    {"#", "SION", tts_Anything, "Z@n"},
    {tts_Anything, "SOME", tts_Anything, "sVm"},
    {"#", "SUR", "#", "Z3"},
    {tts_Anything, "SUR", "#", "S3"},
    {"#", "SU", "#", "Zu"},
    {"#", "SSU", "#", "Su"},
    {"#", "SED", tts_Nothing, "zd"},
    {"#", "S", "#", "z"},
    {tts_Anything, "SAID", tts_Anything, "sed"},
    {"^", "SION", tts_Anything, "S@n"},
    {tts_Anything, "S", "S", ""},
    {".", "S", tts_Nothing, "z"},
    {"#:.E", "S", tts_Nothing, "z"},
    {"#:^##", "S", tts_Nothing, "z"},
    {"#:^#", "S", tts_Nothing, "s"},
    {"U", "S", tts_Nothing, "s"},
    {" :#", "S", tts_Nothing, "z"},
    {tts_Nothing, "SCH", tts_Anything, "sk"},
    {tts_Anything, "S", "C+", ""},
    {"#", "SM", tts_Anything, "zm"},
    {"#", "SN", "'", "z@n"},
    {tts_Anything, "S", tts_Anything, "s"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_T_rules[] = {
    {tts_Nothing, "THE", tts_Nothing, "D@"},
    {tts_Anything, "TO", tts_Nothing, "tu"},
    {tts_Anything, "THAT", tts_Nothing, "D&t"},
    {tts_Nothing, "THIS", tts_Nothing, "DIs"},
    {tts_Nothing, "THEY", tts_Anything, "DeI"},
    {tts_Nothing, "THERE", tts_Anything, "Der"},
    {tts_Anything, "THER", tts_Anything, "D3"},
    {tts_Anything, "THEIR", tts_Anything, "Der"},
    {tts_Nothing, "THAN", tts_Nothing, "D&n"},
    {tts_Nothing, "THEM", tts_Nothing, "Dem"},
    {tts_Anything, "THESE", tts_Nothing, "Diz"},
    {tts_Nothing, "THEN", tts_Anything, "Den"},
    {tts_Anything, "THROUGH", tts_Anything, "Tru"},
    {tts_Anything, "THOSE", tts_Anything, "D@Uz"},
    {tts_Anything, "THOUGH", tts_Nothing, "D@U"},
    {tts_Nothing, "THUS", tts_Anything, "DVs"},
    {tts_Anything, "TH", tts_Anything, "T"},
    {"#:", "TED", tts_Nothing, "tId"},
    {"S", "TI", "#N", "tS"},
    {tts_Anything, "TI", "O", "S"},
    {tts_Anything, "TI", "A", "S"},
    {tts_Anything, "TIEN", tts_Anything, "S@n"},
    {tts_Anything, "TUR", "#", "tS3"},
    {tts_Anything, "TU", "A", "tSu"},
    {tts_Nothing, "TWO", tts_Anything, "tu"},
    {tts_Anything, "T", tts_Anything, "t"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_U_rules[] = {
    {tts_Nothing, "UN", "I", "jun"},
    {tts_Nothing, "UN", tts_Anything, "Vn"},
    {tts_Nothing, "UPON", tts_Anything, "@pOn"},
    {"T", "UR", "#", "Ur"},
    {"S", "UR", "#", "Ur"},
    {"R", "UR", "#", "Ur"},
    {"D", "UR", "#", "Ur"},
    {"L", "UR", "#", "Ur"},
    {"Z", "UR", "#", "Ur"},
    {"N", "UR", "#", "Ur"},
    {"J", "UR", "#", "Ur"},
    {"TH", "UR", "#", "Ur"},
    {"CH", "UR", "#", "Ur"},
    {"SH", "UR", "#", "Ur"},
    {tts_Anything, "UR", "#", "jUr"},
    {tts_Anything, "UR", tts_Anything, "3"},
    {tts_Anything, "U", "^ ", "V"},
    {tts_Anything, "U", "^^", "V"},
    {tts_Anything, "UY", tts_Anything, "aI"},
    {" G", "U", "#", ""},
    {"G", "U", "%", ""},
    {"G", "U", "#", "w"},
    {"#N", "U", tts_Anything, "ju"},
    {"T", "U", tts_Anything, "u"},
    {"S", "U", tts_Anything, "u"},
    {"R", "U", tts_Anything, "u"},
    {"D", "U", tts_Anything, "u"},
    {"L", "U", tts_Anything, "u"},
    {"Z", "U", tts_Anything, "u"},
    {"N", "U", tts_Anything, "u"},
    {"J", "U", tts_Anything, "u"},
    {"TH", "U", tts_Anything, "u"},
    {"CH", "U", tts_Anything, "u"},
    {"SH", "U", tts_Anything, "u"},
    {tts_Anything, "U", tts_Anything, "ju"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_V_rules[] = {
    {tts_Anything, "VIEW", tts_Anything, "vju"},
    {tts_Anything, "V", tts_Anything, "v"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_W_rules[] = {
    {tts_Nothing, "WERE", tts_Anything, "w3"},
    {tts_Anything, "WA", "S", "w0"},
    {tts_Anything, "WA", "T", "w0"},
    {tts_Anything, "WHERE", tts_Anything, "hwer"},
    {tts_Anything, "WHAT", tts_Anything, "hw0t"},
    {tts_Anything, "WHOL", tts_Anything, "h@Ul"},
    {tts_Anything, "WHO", tts_Anything, "hu"},
    {tts_Anything, "WH", tts_Anything, "hw"},
    {tts_Anything, "WAR", tts_Anything, "wOr"},
    {tts_Anything, "WOR", "^", "w3"},
    {tts_Anything, "WR", tts_Anything, "r"},
    {tts_Anything, "W", tts_Anything, "w"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_X_rules[] = {
    {tts_Anything, "X", tts_Anything, "ks"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_Y_rules[] = {
    {tts_Anything, "YOUNG", tts_Anything, "jVN"},
    {tts_Nothing, "YOU", tts_Anything, "ju"},
    {tts_Nothing, "YES", tts_Anything, "jes"},
    {tts_Nothing, "Y", tts_Anything, "j"},
    {"#:^", "Y", tts_Nothing, "i"},
    {"#:^", "Y", "I", "i"},
    {" :", "Y", tts_Nothing, "aI"},
    {" :", "Y", "#", "aI"},
    {" :", "Y", "^+:#", "I"},
    {" :", "Y", "^#", "aI"},
    {tts_Anything, "Y", tts_Anything, "I"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule tts_Z_rules[] = {
    {tts_Anything, "Z", tts_Anything, "z"},
    {tts_Anything, 0, tts_Anything, tts_Silent},
};

static tts_Rule *tts_Rules[] = {
    tts_punct_rules,
    tts_A_rules, tts_B_rules, tts_C_rules, tts_D_rules, tts_E_rules, tts_F_rules, tts_G_rules,
    tts_H_rules, tts_I_rules, tts_J_rules, tts_K_rules, tts_L_rules, tts_M_rules, tts_N_rules,
    tts_O_rules, tts_P_rules, tts_Q_rules, tts_R_rules, tts_S_rules, tts_T_rules, tts_U_rules,
    tts_V_rules, tts_W_rules, tts_X_rules, tts_Y_rules, tts_Z_rules
};


static const char *tts_Cardinals[] = {
    "zero", "one", "two", "three", "four", 
    "five", "six", "seven", "eight", "nine", 
    "ten", "eleven", "twelve", "thirteen", "fourteen", 
    "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"
};


static const char *tts_Twenties[] = {
    "twenty", "thirty", "forty", "fifty",
    "sixty", "seventy", "eighty", "ninety"
};


static const char *tts_Ordinals[] = {
    "zeroth", "first", "second", "third", "fourth", 
    "fifth", "sixth", "seventh","eighth", "ninth",
    "tenth", "eleventh", "twelfth", "thirteenth", "fourteenth", 
    "fifteenth", "sixteenth", "seventeenth", "eighteenth", "nineteenth"
};


static const char *tts_Ord_twenties[] = {
    "twentieth", "thirtieth", "fortieth", "fiftieth",
    "sixtieth", "seventieth", "eightieth", "ninetieth"
};


static int tts_xlate_string(const char *string, struct tts_darray_t *phone, void* memctx);


/*
** Translate a number to phonemes.  This version is for CARDINAL numbers.
**       Note: this is recursive.
*/
static int tts_xlate_cardinal(int value, struct tts_darray_t* phone, void* memctx)
{
    int nph = 0;

    if (value < 0) {
        nph += tts_xlate_string("minus", phone, memctx);
        value = (-value);

        if (value < 0) {                 /* Overflow!  -32768 */
            nph += tts_xlate_string("a lot", phone, memctx);
            return nph;
        }
    }

    if (value >= 1000000000L) {
        /* Billions */
        nph += tts_xlate_cardinal(value / 1000000000L, phone, memctx);
        nph += tts_xlate_string("billion", phone, memctx);
        value = value % 1000000000;

        if (value == 0)
            return nph;                   /* Even billion */

        if (value < 100)
            nph += tts_xlate_string("and", phone, memctx);

        /* as in THREE BILLION AND FIVE */
    }

    if (value >= 1000000L) {
        /* Millions */
        nph += tts_xlate_cardinal(value / 1000000L, phone, memctx);
        nph += tts_xlate_string("million", phone, memctx);
        value = value % 1000000L;

        if (value == 0)
            return nph;                   /* Even million */

        if (value < 100)
            nph += tts_xlate_string("and", phone, memctx);

        /* as in THREE MILLION AND FIVE */
    }

    /* Thousands 1000..1099 2000..99999 */
    /* 1100 to 1999 is eleven-hunderd to ninteen-hunderd */

    if ((value >= 1000L && value <= 1099L) || value >= 2000L) {
        nph += tts_xlate_cardinal(value / 1000L, phone, memctx);
        nph += tts_xlate_string("thousand", phone, memctx);
        value = value % 1000L;

        if (value == 0)
            return nph;                   /* Even thousand */

        if (value < 100)
            nph += tts_xlate_string("and", phone, memctx);

        /* as in THREE THOUSAND AND FIVE */
    }

    if (value >= 100L) {
        nph += tts_xlate_string(tts_Cardinals[value / 100], phone, memctx);
        nph += tts_xlate_string("hundred", phone, memctx);
        value = value % 100;

        if (value == 0)
            return nph;                   /* Even hundred */
    }

    if (value >= 20) {
        nph += tts_xlate_string(tts_Twenties[(value - 20) / 10], phone, memctx);
        value = value % 10;

        if (value == 0)
            return nph;                   /* Even ten */
    }

    nph += tts_xlate_string(tts_Cardinals[value], phone, memctx);

    return nph;
}

/*
** Translate a number to phonemes.  This version is for ORDINAL numbers.
**       Note: this is recursive.
*/
#if 0
static int tts_xlate_ordinal(int value, darray *phone) {
    int nph = 0;

    if (value < 0) {
        nph += tts_xlate_string("minus", phone);
        value = (-value);

        if (value < 0) {                 /* Overflow!  -32768 */
            nph += tts_xlate_string("a lot", phone);
            return nph;
        }
    }

    if (value >= 1000000000L) {
        /* Billions */
        nph += tts_xlate_cardinal(value / 1000000000L, phone);
        value = value % 1000000000;

        if (value == 0)
        {
            nph += tts_xlate_string("billionth", phone);
            return nph;                  /* Even billion */
        }

        nph += tts_xlate_string("billion", phone);

        if (value < 100)
            nph += tts_xlate_string("and", phone);

        /* as in THREE BILLION AND FIVE */
    }

    if (value >= 1000000L) {
        /* Millions */
        nph += tts_xlate_cardinal(value / 1000000L, phone);
        value = value % 1000000L;

        if (value == 0)
        {
            nph += tts_xlate_string("millionth", phone);
            return nph;                  /* Even million */
        }

        nph += tts_xlate_string("million", phone);

        if (value < 100)
            nph += tts_xlate_string("and", phone);

        /* as in THREE MILLION AND FIVE */
    }

    /* Thousands 1000..1099 2000..99999 */
    /* 1100 to 1999 is eleven-hunderd to ninteen-hunderd */

    if ((value >= 1000L && value <= 1099L) || value >= 2000L) {
        nph += tts_xlate_cardinal(value / 1000L, phone);
        value = value % 1000L;

        if (value == 0)
        {
            nph += tts_xlate_string("thousandth", phone);
            return nph;                  /* Even thousand */
        }

        nph += tts_xlate_string("thousand", phone);

        if (value < 100)
            nph += tts_xlate_string("and", phone);

        /* as in THREE THOUSAND AND FIVE */
    }

    if (value >= 100L) {
        nph += tts_xlate_string(tts_Cardinals[value / 100], phone);
        value = value % 100;

        if (value == 0)
        {
            nph += tts_xlate_string("hundredth", phone);
            return nph;                  /* Even hundred */
        }

        nph += tts_xlate_string("hundred", phone);
    }

    if (value >= 20) {
        if ((value % 10) == 0)
        {
            nph += tts_xlate_string(tts_Ord_twenties[(value - 20) / 10], phone);
            return nph;                  /* Even ten */
        }

        nph += tts_xlate_string(tts_Twenties[(value - 20) / 10], phone);

        value = value % 10;
    }

    nph += tts_xlate_string(tts_Ordinals[value], phone);

    return nph;
}
#endif


static int tts_isvowel(int chr) {
    return (chr == 'A' || chr == 'E' || chr == 'I' ||
        chr == 'O' || chr == 'U');
}

static int tts_isconsonant(int chr) {
    return (isupper(chr) && !tts_isvowel(chr));
}

static int tts_leftmatch(
    const char *pattern,                    /* first char of pattern to match in text */
    const char *context) {                     /* last char of text to be matched */
 
    const char *pat;
    const char *text;
    int count;

    if (*pattern == '\0') {
        /* null string matches any context */
        return 1;
    }

    /* point to last character in pattern string */
    count = (int)strlen(pattern);

    pat = pattern + (count - 1);

    text = context;

    for (; count > 0; pat--, count--) {
        /* First check for simple text or space */
        if (isalpha(*pat) || *pat == '\'' || *pat == ' ') {
            if (*pat != *text) {
                return 0;
            } else {
                text--;
                continue;
            }
        }

        switch (*pat) {
            case '#':                   /* One or more vowels */
                if (!tts_isvowel(*text))
                    return 0;
                text--;
                while (tts_isvowel(*text))
                    text--;
                break;

            case ':':                   /* Zero or more consonants */
                while (tts_isconsonant(*text))
                    text--;
                break;

            case '^':                   /* One consonant */
                if (!tts_isconsonant(*text))
                    return 0;
                text--;
                break;

            case '.':                   /* B, D, V, G, J, L, M, N, R, W, Z */
                if (*text != 'B' && *text != 'D' && *text != 'V'
                    && *text != 'G' && *text != 'J' && *text != 'L'
                    && *text != 'M' && *text != 'N' && *text != 'R'
                    && *text != 'W' && *text != 'Z')
                    return 0;
                text--;
                break;

            case '+':                   /* E, I or Y (front vowel) */
                if (*text != 'E' && *text != 'I' && *text != 'Y')
                    return 0;
                text--;
                break;

            case '%':
            default:
                // fprintf(stderr, "Bad char in left rule: '%c'\n", *pat);
                return 0;
        }
    }

    return 1;
}

static int tts_rightmatch(
    const char *pattern,                    /* first char of pattern to match in text */
    const char *context) {                  /* last char of text to be matched */

    const char *pat;
    const char *text;

    if (*pattern == '\0')
        /* null string matches any context */
        return 1;

    pat = pattern;

    text = context;

    for (pat = pattern; *pat != '\0'; pat++) {
        /* First check for simple text or space */
        if (isalpha(*pat) || *pat == '\'' || *pat == ' ') {
            if (*pat != *text) {
                return 0;
            } else {
                text++;
                continue;
            }
        }

        switch (*pat) {
            case '#':                   /* One or more vowels */
                if (!tts_isvowel(*text))
                    return 0;
                text++;
                while (tts_isvowel(*text))
                    text++;
                break;

            case ':':                   /* Zero or more consonants */
                while (tts_isconsonant(*text))
                    text++;
                break;

            case '^':                   /* One consonant */
                if (!tts_isconsonant(*text))
                    return 0;
                text++;
                break;

            case '.':                   /* B, D, V, G, J, L, M, N, R, W, Z */
                if (*text != 'B' && *text != 'D' && *text != 'V'
                    && *text != 'G' && *text != 'J' && *text != 'L'
                    && *text != 'M' && *text != 'N' && *text != 'R'
                    && *text != 'W' && *text != 'Z')
                    return 0;
                text++;
                break;

            case '+':                   /* E, I or Y (front vowel) */
                if (*text != 'E' && *text != 'I' && *text != 'Y')
                    return 0;
                text++;
                break;

            case '%':                   /* ER, E, ES, ED, ING, ELY (a suffix) */
                if (*text == 'E') {
                    text++;
                    if (*text == 'L') {
                        text++;
                        if (*text == 'Y') {
                            text++;
                            break;
                        } else {
                            text--;               /* Don't gobble L */
                            break;
                        }
                    } else {
                        if (*text == 'R' || *text == 'S' || *text == 'D')
                            text++;
                    }
                    break;
                } else if (*text == 'I') {
                    text++;
                    if (*text == 'N') {
                        text++;
                        if (*text == 'G') {
                            text++;
                            break;
                        }
                    }

                    return 0;
                } else {
                    return 0;
                }

            default:
                // fprintf(stderr, "Bad char in right rule:'%c'\n", *pat);
                return 0;
        }
    }

    return 1;
}



static void tts_phone_cat(struct tts_darray_t *arg, const char *s) {
    char ch;

    while ((ch = *s++))
        tts_darray_put( arg, ch );
}


static int tts_find_rule( struct tts_darray_t* arg, char *word, int index, tts_Rule *rules ) {
    for (;;) {                        /* Search for the rule */
        tts_Rule *rule;
        const char *left,
            *match,
            *right,
            *output;
        int remainder;
        rule = rules++;
        match = (*rule)[1];

        if (match == 0) {
            /* bad symbol! */
            fprintf(stderr, "Error: Can't find rule for: '%c' in \"%s\"\n",
                word[index], word);
            return index + 1;            /* Skip it! */
        }

        for (remainder = index; *match != '\0'; match++, remainder++) {
            if (*match != word[remainder])
                break;
        }

        if (*match != '\0')
            continue;                     /* found missmatch */

        left = (*rule)[0];

        right = (*rule)[2];

        if (!tts_leftmatch(left, &word[index - 1]))
            continue;

        if (!tts_rightmatch(right, &word[remainder]))
            continue;

        output = (*rule)[3];

        tts_phone_cat(arg, output);

        return remainder;
    }
}


static void tts_guess_word( struct tts_darray_t *arg, char *word ) {
    int index;                       /* Current position in word */
    int type;                        /* First letter of match part */
    index = 1;                       /* Skip the initial blank */

    do {
        if (isupper(word[index]))
            type = word[index] - 'A' + 1;
        else
            type = 0;

        index = tts_find_rule(arg, word, index, tts_Rules[type]);
    } while (word[index] != '\0');
}


static int tts_NRL(const char *s, int n, struct tts_darray_t *phone, void* memctx ) {
    (void) memctx;
    int old = tts_darray_getSize( phone );
    char mem[ 64 ];    
    char *word = mem;
    if( n + 3 > sizeof( mem ) ) word = (char *) SPEECH_MALLOC( memctx, n + 3);
    char *d = word;
    *d++ = ' ';

    while (n-- > 0) {
        char ch = *s++;

        if (islower(ch))
            ch = (char) toupper(ch);

        *d++ = ch;
    }

    *d++ = ' '; // kinda unnecessary

    *d = '\0';
    tts_guess_word(phone, word);
    if( word != mem ) SPEECH_FREE( memctx, word );
    return tts_darray_getSize( phone ) - old;
}


static int tts_spell_out(const char *word, int n, struct tts_darray_t *phone, void* memctx)
{
    int nph = 0;

    while (n-- > 0) {
        nph += tts_xlate_string(tts_ASCII[*word++ & 0x7F], phone, memctx);
    }

    return nph;
}

static int tts_suspect_word(const char *s, int n) {
    int i = 0;
    int seen_lower = 0;
    int seen_upper = 0;
    int seen_vowel = 0;
    int last = 0;

    for (i = 0; i < n; i++) {
        char ch = *s++;

        if (i && last != '-' && isupper(ch))
            seen_upper = 1;

        if (islower(ch)) {
            seen_lower = 1;
            ch = (char) toupper(ch);
        }

        if (ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U' || ch == 'Y')
            seen_vowel = 1;

        last = ch;
    }

    return !seen_vowel || (seen_upper && seen_lower) || !seen_lower;
}

static int tts_xlate_word(const char *word, int n, struct tts_darray_t *phone, void* memctx) {
    int nph = 0;

    if (*word != '[') {
        if (tts_suspect_word(word, n)) {
            return tts_spell_out(word, n, phone, memctx);
        } else {
            nph += tts_NRL(word, n, phone, memctx);
        }
    } else {
        if ((++word)[(--n) - 1] == ']')
            n--;

        while (n-- > 0) {
            tts_darray_put( phone, *word++);
            nph++;
        }
    }

    tts_darray_put( phone, ' ');

    return nph + 1;
}


static int tts_xlate_string(const char *string, struct tts_darray_t *phone, void* memctx) {
    int nph = 0;
    const char *s = string;
    char ch;

    while (isspace(ch = *s))
        s++;

    while ((ch = *s)) {
        const char *word = s;

        if (isalpha(ch)) {
            while (isalpha(ch = *s) || ((ch == '\'' || ch == '-' || ch == '.') && isalpha(s[1])))
                s++;

            if (!ch || isspace(ch) || ispunct(ch) || (isdigit(ch) && !tts_suspect_word(word, (int)(s - word)))) {
                nph += tts_xlate_word(word, (int)(s - word), phone, memctx);
            } else {
                while ((ch = *s) && !isspace(ch) && !ispunct(ch))
                    s++;

                nph += tts_spell_out(word, (int)(s - word), phone, memctx);
            }
        } else {
            if (isdigit(ch) || (ch == '-' && isdigit(s[1]))) {
                int sign = (ch == '-') ? -1 : 1;
                int value = 0;

                if (sign < 0)
                    ch = *++s;

                while (isdigit(ch = *s)) {
                    value = value * 10 + ch - '0';
                    s++;
                }

                if (ch == '.' && isdigit(s[1])) {
                    word = ++s;
                    nph += tts_xlate_cardinal(value * sign, phone, memctx);
                    nph += tts_xlate_string("point", phone, memctx);

                    while (isdigit(ch = *s))
                        s++;

                    nph += tts_spell_out(word, (int)(s - word), phone, memctx);
                } else {
                    /* check for ordinals, date, time etc. can go in here */
                    nph += tts_xlate_cardinal(value * sign, phone, memctx);
                }
            } else {
                if (ch == '[' && strchr(s, ']')) {
                    const char *w = s;

                    while (*s && *s++ != ']')
                        /* nothing */
                        ;

                    nph += tts_xlate_word(w, (int)(s - w), phone, memctx);
                } else {
                    if (ispunct(ch)) {
                        switch (ch) {
                            case '!':
                            case '?':
                            case '.': {
                                s++;
                                tts_darray_put( phone, ' ');
                            } break;

                            case '"':                 /* change pitch ? */
                            case ':':
                            case '-':
                            case ';':
                            case ',':
                            case '(':
                            case ')': {
                                s++;
                                tts_darray_put( phone, ' ');
                            } break;

                            case '[': {
                                const char *e = strchr(s, ']');

                                if (e) {
                                    s++;

                                    while (s < e)
                                        tts_darray_put( phone, *s++);

                                    s = e + 1;

                                    break;
                                }
                            }
                            default: {
                                nph += tts_spell_out(word, 1, phone, memctx);
                                s++;
                            } break;
                        }
                    } else {
                        while ((ch = *s) && !isspace(ch))
                            s++;

                        nph += tts_spell_out(word, (int)(s - word), phone, memctx);
                    }

                    while (isspace(ch = *s))
                        s++;
                }
            }
        }
    }

    return nph;
}



short* speech_gen( int* samples_pairs_generated, char const* str, void* memctx ) {
    struct tts_darray_t element;
    tts_darray_init( &element, memctx );

    struct tts_darray_t phone;
    tts_darray_init( &phone, memctx );

    tts_xlate_string( str, &phone, memctx );
    int frames = tts_phone_to_elm( tts_darray_getData( &phone ), tts_darray_getSize( &phone ), &element );

    struct tts_klatt_t synth;
    tts_klatt_init( &synth );
    
    int sample_count = synth.mNspFr * frames;

    if( samples_pairs_generated ) *samples_pairs_generated = sample_count * 4;
    short* sample_pairs = (short*) SPEECH_MALLOC( memctx, sample_count * 8 * sizeof( short ) );   

    short* samples = (short*) sample_pairs;

    tts_klatt_initsynth( &synth, tts_darray_getSize( &element ), (unsigned char*) tts_darray_getData( &element )) ;
    int x = 0;
    int pos = 0;
    while( x >= 0 ) {
        x = tts_klatt_synth( &synth, 0, &samples[ pos ] );
        if( x < 0 ) break;
        pos += x;
    }

    short* in = samples + sample_count;
    short* out = sample_pairs + sample_count * 4 * 2;
    short prev_sample = 0;
    while( out > sample_pairs ) {
        out -= 4 *2;
        --in;
        short sample = *in;
        int o[ 8 ];
        o[0] = ( ((sample + (3 * prev_sample)) >> 2) ) * 2;
        o[1] = ( ((sample + prev_sample) >> 1) ) * 2;
        o[2] = ( (((3 * sample) + prev_sample) >> 2) ) * 2;
        o[3] = ( sample ) * 2;
        o[4] = ( ((sample + (3 * prev_sample)) >> 2) ) * 2;
        o[5] = ( ((sample + prev_sample) >> 1) ) * 2;
        o[6] = ( (((3 * sample) + prev_sample) >> 2) ) * 2;
        o[7] = ( sample ) * 2;
        prev_sample = sample;
        for( int i = 0; i < 8; ++i ) out[ i ] = (short)( o[ i ] > 32767 ? 32767 : o[ i ] < -32767 ? -32767 : o[ i ] );
    }
    
    tts_darray_term( &phone );
    tts_darray_term( &element );
    return sample_pairs;
}


void speech_free( short* sample_pairs, void* memctx ) {
    (void) memctx;
    SPEECH_FREE( memctx, sample_pairs );
}


#pragma warning( pop )

#endif /* SPEECH_IMPLEMENTATION */


/*
----------------------------------------------------------------------------
    LICENSE
----------------------------------------------------------------------------

The speech synth is based on rsynth by the late Nick Ing-Simmons (et al).

He described the legal status as:

    This is a text to speech system produced by integrating various pieces 
    of code and tables of data, which are all (I believe) in the public 
    domain.
    
Since then, the rsynth source code has passed legal checks by several open 
source organizations, so it "should" be pretty safe.

The primary copyright claims seem to have to do with text-to-speech 
dictionary use, which I've removed completely.

I've done some serious refactoring, clean-up and feature removal on the 
source, as all I need is "a" free, simple speech synth, not a "good" speech 
synth. Since I've removed a bunch of stuff, this is probably safer public 
domain release than the original.

(I'm rather surprised there's no good public domain speech synths out 
there; after all, it's 2013..)

I'm placing my changes in public domain as well, or if that's not 
acceptable for you, then CC0:
http://creativecommons.org/publicdomain/zero/1.0/

The SoLoud interface files (soloud_speech.*) are under ZLib/LibPNG license.

-- Jari Komppa 2013
   
----------------------------------------------------------------------------
*/
