/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

frametimer.h - v0.1 - Framerate timer functionality.

Do this:
    #define FRAMETIMER_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef frametimer_h
#define frametimer_h

typedef struct frametimer_t frametimer_t;

frametimer_t* frametimer_create( void* memxtx );

void frametimer_destroy( frametimer_t* frametimer );

void frametimer_lock_rate( frametimer_t* frametimer, int fps );

float frametimer_update( frametimer_t* frametimer );

float frametimer_delta_time( frametimer_t* frametimer );

int frametimer_frame_counter( frametimer_t* frametimer );

#endif /* frametimer_h */

/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef FRAMETIMER_IMPLEMENTATION
#undef FRAMETIMER_IMPLEMENTATION

#define _CRT_NONSTDC_NO_DEPRECATE
#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef _WIN32
    #if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0501
        #undef _WIN32_WINNT
        #define _WIN32_WINNT 0x0501 // requires Windows XP minimum
    #endif
    // 0x0400=Windows NT 4.0, 0x0500=Windows 2000, 0x0501=Windows XP, 0x0502=Windows Server 2003, 0x0600=Windows Vista,
    // 0x0601=Windows 7, 0x0602=Windows 8, 0x0603=Windows 8.1, 0x0A00=Windows 10,
    #define _WINSOCKAPI_
    #pragma warning( push )
    #pragma warning( disable: 4619 )
    #pragma warning( disable: 4668 ) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
    #pragma warning( disable: 4768 ) // __declspec attributes before linkage specification are ignored
    #pragma warning( disable: 4255 ) // 'function' : no function prototype given: converting '()' to '(void)'
    #include <windows.h>
    #pragma warning( pop )
    #ifndef __TINYC__
        #pragma comment(lib, "winmm.lib")
    #else
        typedef struct timecaps_tag { UINT wPeriodMin; UINT wPeriodMax; } TIMECAPS, *PTIMECAPS, NEAR *NPTIMECAPS, FAR *LPTIMECAPS;
        typedef UINT MMRESULT;
        #define TIMERR_NOERROR (0)
        static MMRESULT (*timeGetDevCaps)( LPTIMECAPS ptc, UINT cbtc );
        static MMRESULT (*timeBeginPeriod)( UINT uPeriod );
        static MMRESULT (*timeEndPeriod)( UINT uPeriod );
    #endif
#elif defined( __APPLE__ )
    #include <mach/mach_time.h>
#elif defined( __wasm__ )
    #define WA_CORO_IMPLEMENT_NANOSLEEP
    #include <wajic_coro.h>
#else
    #include <time.h>
#endif

#ifndef FRAMETIMER_MALLOC
    #include <stdlib.h>
    #if defined(__cplusplus)
        #define FRAMETIMER_MALLOC( ctx, size ) ( ::malloc( size ) )
        #define FRAMETIMER_FREE( ctx, ptr ) ( ::free( ptr ) )
    #else
        #define FRAMETIMER_MALLOC( ctx, size ) ( malloc( size ) )
        #define FRAMETIMER_FREE( ctx, ptr ) ( free( ptr ) )
    #endif
#endif

#ifndef FRAMETIMER_U64
    #define FRAMETIMER_U64 unsigned long long
#endif

struct frametimer_t
    {
    FRAMETIMER_U64 clock_freq;
    FRAMETIMER_U64 prev_clock;
    void* memctx;
    float delta_time;
    int initialized;
    int frame_counter;
    int frame_rate_lock;
    #ifdef _WIN32
        HANDLE waitable_timer;
    #endif
    };


frametimer_t* frametimer_create( void* memctx )
    {
    frametimer_t* frametimer = (frametimer_t*) FRAMETIMER_MALLOC( memctx, sizeof( frametimer_t ) );
    #ifdef _WIN32
        #ifdef __TINYC__
            HMODULE winmm = LoadLibrary( "winmm" );
            timeGetDevCaps = GetProcAddress( winmm, "timeGetDevCaps");
            timeBeginPeriod = GetProcAddress( winmm, "timeBeginPeriod");
            timeEndPeriod = GetProcAddress( winmm, "timeEndPeriod");
        #endif
        TIMECAPS tc;
        if( timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == TIMERR_NOERROR )
            timeBeginPeriod( tc.wPeriodMin );
        frametimer->waitable_timer = CreateWaitableTimer(NULL, TRUE, NULL);
    #endif

    frametimer->memctx = memctx;
    frametimer->initialized = 0;

    #ifdef _WIN32
        LARGE_INTEGER f;
        QueryPerformanceFrequency( &f );
        frametimer->clock_freq = (FRAMETIMER_U64) f.QuadPart;
    #else
        frametimer->clock_freq = 1000000000ull;
    #endif

    frametimer->prev_clock = 0;
    frametimer->delta_time = 0.0f;
    frametimer->frame_counter = 0;
    frametimer->frame_rate_lock = 0;
    return frametimer;
    }


void frametimer_destroy( frametimer_t* frametimer )
    {
    #ifdef _WIN32
        CloseHandle( frametimer->waitable_timer );
        TIMECAPS tc;
        if( timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == TIMERR_NOERROR )
            timeEndPeriod( tc.wPeriodMin );
    #endif
    FRAMETIMER_FREE( frametimer->memctx, frametimer );
    }


void frametimer_lock_rate( frametimer_t* frametimer, int fps )
    {
    frametimer->frame_rate_lock = ( fps > 0 && fps < 5 ) ? 5 : fps < 0 ? 0 : fps;
    }


float frametimer_update( frametimer_t* frametimer )
    {
    if( !frametimer->initialized )
        {
        #ifdef _WIN32
            LARGE_INTEGER c;
            QueryPerformanceCounter( &c );
            frametimer->prev_clock = (FRAMETIMER_U64) c.QuadPart;
        #elif defined( __APPLE__ )
            frametimer->prev_clock = clock_gettime_nsec_np( CLOCK_UPTIME_RAW );
        #else
            struct timespec t;
            clock_gettime( CLOCK_MONOTONIC_RAW, &t );
            frametimer->prev_clock = (FRAMETIMER_U64)t.tv_sec;
            frametimer->prev_clock *= 1000000000ull;
            frametimer->prev_clock += (FRAMETIMER_U64)t.tv_nsec;
        #endif
        frametimer->initialized = 1;
        }

    ++frametimer->frame_counter;


    FRAMETIMER_U64 curr_clock = 0ULL;
    #ifdef _WIN32
        LARGE_INTEGER c;
        QueryPerformanceCounter( &c );
        curr_clock = (FRAMETIMER_U64) c.QuadPart;
    #elif defined( __APPLE__ )
        curr_clock = (FRAMETIMER_U64) clock_gettime_nsec_np( CLOCK_UPTIME_RAW );
    #else
        struct timespec t;
        clock_gettime( CLOCK_MONOTONIC_RAW, &t );
        curr_clock = (FRAMETIMER_U64)t.tv_sec;
        curr_clock *= 1000000000ull;
        curr_clock += (FRAMETIMER_U64)t.tv_nsec;
    #endif

    if( frametimer->frame_rate_lock > 0 )
        {
        FRAMETIMER_U64 delta = 0ULL;
        if( curr_clock > frametimer->prev_clock )
            delta = curr_clock - frametimer->prev_clock - 1ULL;
        if( delta < frametimer->clock_freq / frametimer->frame_rate_lock )
            {
            FRAMETIMER_U64 wait = ( frametimer->clock_freq / frametimer->frame_rate_lock ) - delta;
            #ifdef _WIN32
                if( wait > 0 )
                    {
                    LARGE_INTEGER due_time;
                    due_time.QuadPart = - (LONGLONG) ( ( 10000000ULL * wait ) / frametimer->clock_freq ) ;

                    SetWaitableTimer( frametimer->waitable_timer, &due_time, 0, 0, 0, FALSE );
                    WaitForSingleObject( frametimer->waitable_timer, 200 ); // wait long enough for timer to trigger ( 200ms == 5fps )
                    CancelWaitableTimer( frametimer->waitable_timer ); // in case we timed out
                    }
            #else
                struct timespec t = { 0, 0 };
                t.tv_nsec = wait;
                while( t.tv_nsec > 0 )
                    {
                    struct timespec r = { 0, 0 };
                    if( nanosleep( &t, &r ) >= 0 ) break;
                    t = r;
                    }
            #endif
            curr_clock += wait;
            }
        }

    FRAMETIMER_U64 delta_clock = 0ULL;
    if( curr_clock > frametimer->prev_clock ) delta_clock = curr_clock - frametimer->prev_clock;

    // Cap delta time if it is too high (running at less than 5 fps ) or things will jump
    // like crazy on occasional long stalls.
    if( delta_clock > frametimer->clock_freq / 5ULL ) delta_clock = frametimer->clock_freq / 5ULL; // Cap to 5 fps

    float delta_time = (float) ( ( (double) delta_clock ) / ( (double) frametimer->clock_freq ) );

    frametimer->delta_time = delta_time;
    frametimer->prev_clock = curr_clock;

    return delta_time;
    }


float frametimer_delta_time( frametimer_t* frametimer )
    {
    return frametimer->delta_time;
    }


int frametimer_frame_counter( frametimer_t* frametimer )
    {
    return frametimer->frame_counter;
    }


#endif /* FRAMETIMER_IMPLEMENTATION */


/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2015 Mattias Gustavsson

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

------------------------------------------------------------------------------

ALTERNATIVE B - Public Domain (www.unlicense.org)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

------------------------------------------------------------------------------
*/
