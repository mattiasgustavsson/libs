/*
------------------------------------------------------------------------------
		  Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

frametimer.h - v1.0 - Framerate timer and frame rate limiter, for C/C++

Do this:
	#define FRAMETIMER_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef frametimer_h
#define frametimer_h

typedef struct frametimer_t frametimer_t;

frametimer_t* frametimer_create( void* memctx );
void frametimer_destroy( frametimer_t* frametimer );

void frametimer_lock_rate( frametimer_t* frametimer, int fps );

float frametimer_update( frametimer_t* frametimer );

float frametimer_delta_time( frametimer_t* frametimer );
int frametimer_frame_counter( frametimer_t* frametimer );

#endif /* frametimer_h */



/**

frametimer.h
============

Framerate timer and frame rate limiter functionality.


Example
-------

Here's a sample program which creates a frame timer, limits update to 60 frames
per second, and retrieves delta time on each loop iteration.

	#define FRAMETIMER_IMPLEMENTATION
	#include "frametimer.h"

	int main() {
		frametimer_t* frametimer = frametimer_create( NULL );
		frametimer_lock_rate( frametimer, 60 );

		int is_running = 1;
		while( is_running ) {
			float dt = frametimer_update( frametimer );

			// ...

		}

		frametimer_destroy( frametimer );
		return 0;
	}


API Documentation
-----------------

frametimer.h is a small library for measuring the time between frame updates in
a game loop or similar application, and optionally limit the frame rate to a
specified number of frames per seconds.

It is using high precision timers and wait functions to accomplish this.


### Customization

There are a few different things in frametimer.h which are configurable by
#defines. The customizations only affect the implementation, so will only need
to be defined in the file where you have the #define FRAMETIMER_IMPLEMENTATION.


#### Custom memory allocators

To store the internal data structures, frametimer.h needs to do dynamic allocation
by calling `malloc`. Programs might want to keep track of allocations done, or use
custom defined pools to allocate memory from. frametimer.h allows for specifying
custom memory allocation functions for `malloc` and `free`.
This is done with the following code:

	#define FRAMETIMER_IMPLEMENTATION
	#define FRAMETIMER_MALLOC( ctx, size ) ( my_custom_malloc( ctx, size ) )
	#define FRAMETIMER_FREE( ctx, ptr ) ( my_custom_free( ctx, ptr ) )
	#include "frametimer.h"

where `my_custom_malloc` and `my_custom_free` are your own memory
allocation/deallocation functions. The `ctx` parameter is an optional parameter of
type `void*`. When `frametimer_create` is called, you can pass in a `memctx`
parameter, which can be a pointer to anything you like, and which will be passed
through as the `ctx` parameter to every `FRAMETIMER_MALLOC`/`FRAMETIMER_FREE` call.
For example, if you are doing memory tracking, you can pass a pointer to your
tracking data as `memctx`, and in your custom allocation/deallocation function, you
can cast the `ctx` param back to the right type, and update the tracking data.

If no custom allocator is defined, frametimer.h will default to `malloc` and `free`
from the C runtime library.


#### Custom unsigned 64-bit integer type

frametimer.h allows for specifying the exact type of 64-bit unsigned integer to be used
internally. By default, it is defined as `unsigned long long`, but as this is not a
standard type on all compilers, you can redefine it by #defining FRAMETIMER_U64 before
including the implementation for frametimer.h. This is useful if you, for example, use
the types from `<stdint.h>` in the rest of your program, and you want frametimer.h to
use compatible types. In this case, you would include frametimer.h using the following
code:

	#define FRAMETIMER_IMPLEMENTATION
	#define FRAMETIMER_U64 uint64_t
	#include "frametimer.h"


frametimer_create
-----------------

	frametimer_t* frametimer_create( void* memctx );

Creates a new frametimer instance. When no longer needed, it should be cleaned up by
calling `frametimer_destroy`. `memctx` is a pointer to user defined data which will be
passed through to the custom FRAMETIMER_MALLOC/FRAMETIMER_FREE calls. It can be NULL if
no user defined data is needed, or if no custom allocators are defined. If the allocation
fails when creating the frametimer, `frametimer_create` returns NULL.


frametimer_destroy
------------------

	void frametimer_destroy( frametimer_t* frametimer )

Destroys a `frametimer_t` instance created by calling `frametimer_create`, releasing the
resources allocated by it. No further API calls are valid on a `frametimer_t` instance
after calling `frametimer_destroy` on it.


frametimer_lock_rate
--------------------

	void frametimer_lock_rate( frametimer_t* frametimer, int fps );

Using frametimer, you can lock the update rate to a fixed number of frames per second.
In this case, calls to `frametimer_update` will measure the time elapsed since last
update, using high precision timers, and if it is less than the time between frames for
the given fps, `frametimer_update` will (non-busy) wait until the expected amount of time
has elapsed. The implementation is using a high quality, high resolution wait mechanic
for this, and (on windows) sets the highest timer resolution possible, ensuring the wait
does not "oversleep".

Passing an `fps` parameter of 0 (or less), turns off the frame rate lock and reverts to
the default behaviour, which is free running frame rate. The update rate is capped at the
lower end to 5 fps, to protect against time steps that are too large to be practical, and
which can cause problems in game update code.


frametimer_update
-----------------

	float frametimer_update( frametimer_t* frametimer );

`frametimer_update` should be called once per frame update in your main loop.

If running at a locked rate (`frametimer_lock_rate` having been called with an `fps`
param greater than 0), `frametimer_update` will measure the time elapsed since last
update, using high precision timers, and if it is less than the time between frames for
the given fps, `frametimer_update` will (non-busy) wait until the expected amount of time
has elapsed. The implementation is using a high quality, high resolution wait mechanic
for this, and (on windows) sets the highest timer resolution possible, ensuring the wait
does not "oversleep".

When running at an unrestricted rate, `frametimer_update` will measure the time elapsed
since last update, using high precision timers.

In both cases, `frametimer_update` will return a floating point delta time value, giving
the number of seconds elapsed since last frame. This value is guaranteed to never be
negative, and it is capped to never be greater than 0.2 seconds, equivalent to running
at 5 frames per second, to protect against time steps that are too large to be practical,
and which can cause problems in game update code.


frametimer_delta_time
---------------------

	float frametimer_delta_time( frametimer_t* frametimer );

Returns the delta time for the last frame update. It returns the exact same value that
was returned from the last call to `frametimer_update`.


frametimer_frame_counter
------------------------

	int frametimer_frame_counter( frametimer_t* frametimer );

Returns the total number of times that `frametimer_update` has been called.


*/


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
	#if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0600
		#undef _WIN32_WINNT
		#define _WIN32_WINNT 0x0601 // requires Windows 7 minimum
	#endif
	// 0x0400=Windows NT 4.0, 0x0500=Windows 2000, 0x0501=Windows XP, 0x0502=Windows Server 2003, 0x0600=Windows Vista,
	// 0x0601=Windows 7, 0x0602=Windows 8, 0x0603=Windows 8.1, 0x0A00=Windows 10,
	#define _WINSOCKAPI_
	#pragma warning( push )
	#pragma warning( disable: 4619 )
	#pragma warning( disable: 4668 ) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
	#pragma warning( disable: 4768 ) // __declspec attributes before linkage specification are ignored
	#pragma warning( disable: 4255 ) // 'function' : no function prototype given: converting '()' to '(void)'
	#define _NTDDSCM_H_ /* Fixes the error of mismatched pragma warning push/pop in Windows SDK 10.0.17763.0 */
	#include <windows.h>
	#pragma warning( pop )
	#ifndef __TINYC__
		#pragma comment(lib, "winmm.lib")
	#else
		typedef struct timecaps_tag { UINT wPeriodMin; UINT wPeriodMax; } TIMECAPS, *PTIMECAPS, NEAR *NPTIMECAPS, FAR *LPTIMECAPS;
		typedef UINT MMRESULT;
		#define TIMERR_NOERROR (0)
		typedef LONG NTSTATUS;
	#endif
	typedef NTSTATUS (WINAPI *NtQueryTimerResolution_t)( PULONG MinimumResolution, PULONG MaximumResolution, PULONG CurrentResolution );
	typedef NTSTATUS (WINAPI *NtSetTimerResolution_t)( ULONG DesiredResolution, BOOLEAN SetResolution, PULONG actualresolution );
	typedef NTSTATUS (WINAPI *RtlGetVersion_t)( PRTL_OSVERSIONINFOW );
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

#include <string.h>

struct frametimer_t {
	FRAMETIMER_U64 clock_freq;
	FRAMETIMER_U64 prev_clock;
	void* memctx;
	float delta_time;
	int initialized;
	int frame_counter;
	int frame_rate_lock;
	#ifdef _WIN32
		HANDLE waitable_timer;
		NtSetTimerResolution_t NtSetTimerResolution;
	#endif
};


frametimer_t* frametimer_create( void* memctx ) {
	frametimer_t* frametimer = (frametimer_t*) FRAMETIMER_MALLOC( memctx, sizeof( frametimer_t ) );
	if( !frametimer ) {
		return NULL;
	}
	memset( frametimer, 0, sizeof( *frametimer ) );

	#ifdef _WIN32
		#ifdef __TINYC__
			HMODULE winmm = LoadLibrary( "winmm" );
			MMRESULT (*timeGetDevCaps)( LPTIMECAPS ptc, UINT cbtc ) = GetProcAddress( winmm, "timeGetDevCaps" );
			MMRESULT (*timeBeginPeriod)( UINT uPeriod ) = GetProcAddress( winmm, "timeBeginPeriod" );
		#endif
		TIMECAPS tc;
		if( timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == TIMERR_NOERROR ) {
			timeBeginPeriod( tc.wPeriodMin );
		}

		RtlGetVersion_t RtlGetVersion = NULL;
		HMODULE ntdll = GetModuleHandleA( "ntdll.dll" );
		if( ntdll ) {
			RtlGetVersion = (RtlGetVersion_t) (uintptr_t) GetProcAddress( ntdll, "RtlGetVersion" );
			NtQueryTimerResolution_t NtQueryTimerResolution = (NtQueryTimerResolution_t) (uintptr_t) GetProcAddress( ntdll, "NtQueryTimerResolution" );
			frametimer->NtSetTimerResolution = (NtSetTimerResolution_t) (uintptr_t) GetProcAddress( ntdll, "NtSetTimerResolution" );
			if( NtQueryTimerResolution && frametimer->NtSetTimerResolution ) {
				ULONG minres = 0;
				ULONG maxres = 0;
				ULONG curres = 0;
				if( NtQueryTimerResolution( &maxres, &minres, &curres ) == 0 )	{
					ULONG actualres = 0;
					frametimer->NtSetTimerResolution( minres, TRUE, &actualres );
				}
			}
		}

		#ifndef __TINYC__
			RTL_OSVERSIONINFOW version_info = { sizeof( version_info ) };
			if( RtlGetVersion && RtlGetVersion( &version_info ) == 0 && ( version_info.dwMajorVersion > 10 || ( version_info.dwMajorVersion == 10 && version_info.dwBuildNumber >= 17134 ) ) ) {
				frametimer->waitable_timer = CreateWaitableTimerExW( NULL, NULL, CREATE_WAITABLE_TIMER_MANUAL_RESET | CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS );
			} else {
				frametimer->waitable_timer = CreateWaitableTimerA( NULL, TRUE, NULL );
			}
		#else
			frametimer->waitable_timer = CreateWaitableTimerA( NULL, TRUE, NULL );
		#endif
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


void frametimer_destroy( frametimer_t* frametimer ) {
	#ifdef _WIN32
		CloseHandle( frametimer->waitable_timer );
		if( frametimer->NtSetTimerResolution ) {
			ULONG actualres = 0;
			frametimer->NtSetTimerResolution( 0, FALSE, &actualres );
		}
		#ifdef __TINYC__
			HMODULE winmm = LoadLibrary( "winmm" );
			MMRESULT (*timeGetDevCaps)( LPTIMECAPS ptc, UINT cbtc ) = GetProcAddress( winmm, "timeGetDevCaps" );
			MMRESULT (*timeEndPeriod)( UINT uPeriod ) = GetProcAddress( winmm, "timeEndPeriod" );
		#endif
		TIMECAPS tc;
		if( timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == TIMERR_NOERROR ) {
			timeEndPeriod( tc.wPeriodMin );
		}
	#endif
	FRAMETIMER_FREE( frametimer->memctx, frametimer );
}


void frametimer_lock_rate( frametimer_t* frametimer, int fps ) {
	frametimer->frame_rate_lock = ( fps > 0 && fps < 5 ) ? 5 : fps < 0 ? 0 : fps;
}


float frametimer_update( frametimer_t* frametimer ) {
	if( !frametimer->initialized ) {
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

	if( frametimer->frame_rate_lock > 0 ) {
		FRAMETIMER_U64 delta = 0ULL;
		if( curr_clock > frametimer->prev_clock ) {
			delta = curr_clock - frametimer->prev_clock - 1ULL;
		}
		if( delta < frametimer->clock_freq / frametimer->frame_rate_lock ) {
			FRAMETIMER_U64 wait = ( ( frametimer->clock_freq + frametimer->frame_rate_lock - 1 ) / frametimer->frame_rate_lock ) - delta;
			#ifdef _WIN32
				if( wait > 0 ) {
					LARGE_INTEGER due_time;
					due_time.QuadPart = - (LONGLONG) ( ( 10000000ULL * wait ) / frametimer->clock_freq ) ;
					SetWaitableTimer( frametimer->waitable_timer, &due_time, 0, 0, 0, FALSE );
					WaitForSingleObject( frametimer->waitable_timer, 200 ); // wait long enough for timer to trigger ( 200ms == 5fps )
					CancelWaitableTimer( frametimer->waitable_timer ); // in case we timed out (should never happen)
				}
			#else
				struct timespec t = { 0, 0 };
				t.tv_nsec = wait;
				while( t.tv_nsec > 0 ) {
					struct timespec r = { 0, 0 };
					if( nanosleep( &t, &r ) >= 0 ) break;
					t = r;
				}
			#endif
			curr_clock += wait;
		}
	}

	FRAMETIMER_U64 delta_clock = 0ULL;
	if( curr_clock > frametimer->prev_clock ) {
		delta_clock = curr_clock - frametimer->prev_clock;
	}

	// Cap delta time if it is too high (running at less than 5 fps ) or things will jump
	// like crazy on occasional long stalls.
	if( delta_clock > frametimer->clock_freq / 5ULL ) {
		delta_clock = frametimer->clock_freq / 5ULL; // Cap to 5 fps
	}

	float delta_time = (float) ( ( (double) delta_clock ) / ( (double) frametimer->clock_freq ) );

	frametimer->delta_time = delta_time;
	frametimer->prev_clock = curr_clock;

	return delta_time;
}


float frametimer_delta_time( frametimer_t* frametimer ) {
	return frametimer->delta_time;
}


int frametimer_frame_counter( frametimer_t* frametimer ) {
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
