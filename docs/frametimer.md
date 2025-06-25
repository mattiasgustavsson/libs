frametimer.h
============

Library: [frametimer.h](../frametimer.h)


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

