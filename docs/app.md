app.h
=====

Library: [app.h](../app.h)


Example
-------

Here's a basic sample program which starts a windowed app and plots random pixels.

    #define  APP_IMPLEMENTATION
    #if defined( _WIN32 )
        #define APP_WINDOWS
    #elif defined( __wasm__ )
        #define APP_WASM
    #else
        #define APP_SDL
    #endif
    #include "app.h"

    #include <stdlib.h> // for rand, __argc/__argv, EXIT_SUCCESS
    #include <string.h> // for memset

    int app_proc( app_t* app, void* user_data ) {
        APP_U32 canvas[ 320 * 200 ]; // a place for us to draw stuff
        memset( canvas, 0xC0, sizeof( canvas ) ); // clear to grey
        app_screenmode( app, APP_SCREENMODE_WINDOW );

        // keep running until the user close the window
        while( app_yield( app ) != APP_STATE_EXIT_REQUESTED ) {
            // plot a random pixel on the canvas
            int x = rand() % 320;
            int y = rand() % 200;
            APP_U32 color = rand() | ( (APP_U32) rand() << 16 );
            canvas[ x + y * 320 ] = color;

            // display the canvas
            app_present( app, canvas, 320, 200, 0xffffff, 0x000000 );
        }
    	return EXIT_SUCCESS; 
    }

    int main( int argc, char** argv ) {
        return app_run( app_proc, NULL, NULL, NULL, NULL );
    }

    // pass-through so the program will build with either /SUBSYSTEM:WINDOWS or /SUBSYSTEM:CONSOLE
    #ifdef __cplusplus
        extern "C" 
    #endif
    int __stdcall WinMain( struct HINSTANCE__* a, struct HINSTANCE__* b, char* c, int d ) { 
        return main( __argc, __argv ); 
    }




API Documentation
-----------------

app.h is a single-header library, and does not need any .lib files or other binaries, or any build scripts. To use it,
you just include app.h to get the API declarations. To get the definitions, you must include app.h from *one* single
C or C++ file, and #define the symbol `APP_IMPLEMENTATION` before you do.

As app.h is a cross platform library, you must also define which platform you are running on, like this for Windows:

    #define APP_IMPLEMENTATION
    #define APP_WINDOWS
    #include "app.h"

like this for web assembly (browsers):
    #define APP_IMPLEMENTATION
    #define APP_WASM
    #include "app.h"

or like this for other platforms (osx, linux):
    #define APP_IMPLEMENTATION
    #define APP_SDL
    #include "app.h"

### Customization

There are a few different things in app.h which are configurable by #defines. Most of the API use the `int` data type,
for integer values where the exact size is not important. However, for some functions, it specifically makes use of 
data types with specific bit width and signedness. These have default definitions combining basic C datatypes and the
`unsigned` modifier, but can be redefined before including app.h. This is useful if you, for example, use the types 
from `<stdint.h>` in the rest of your program, and you want app.h to use the same types. app.h makes use of these
data types, shown here with their default definitions:

    #define APP_U8 unsigned char
    #define APP_S16 short
    #define APP_U16 unsigned short
    #define APP_U32 unsigned int
    #define APP_U64 unsigned long long

As an example, if you wanted to redefine them to use the types from `<stdint.h>` you would do:

    #include <stdint.h>
    #define APP_U8 uint8_t
    #define APP_S16 int16_t
    #define APP_U16 uint16_t
    #define APP_U32 uint32_t
    #define APP_U64 uint64_t
    #include "app.h"

Note that when customizing the data types, you need to use the same definition in every place where you include app.h,
as they affect the declarations as well as the definitions.

The rest of the customizations only affect the implementation, so will only need to be defined in the file where you
have the #define APP_IMPLEMENTATION.


#### Custom memory allocators

Even though app.h attempts to minimize the memory use and number of allocations, it still needs to make *some* use of
dynamic allocation by calling `malloc`. Programs might want to keep track of allocations done, or use custom defined
pools to allocate memory from. app.h allows for specifying custom memory allocation functions for `malloc` and `free`.
This is done with the following code:

    #define APP_IMPLEMENTATION
    #define APP_MALLOC( ctx, size ) ( my_custom_malloc( ctx, size ) )
    #define APP_FREE( ctx, ptr ) ( my_custom_free( ctx, ptr ) )
    #include "app.h"

where `my_custom_malloc` and `my_custom_free` are your own memory allocation/deallocation functions. The `ctx` parameter
is an optional parameter of type `void*`. When `app_run` is called, you can pass in a `memctx` parameter, which can be a
pointer to anything you like, and which will be passed through as the `ctx` parameter to every APP_MALLOC/APP_FREE call.
For example, if you are doing memory tracking, you can pass a pointer to your tracking data as `memctx`, and in your
custom allocation/deallocation function, you can cast the `ctx` param back to the right type, and access the tracking
data.

If no custom allocator is defined, app.h will default to `malloc` and `free` from the C runtime library.


#### Custom logging function

During successful operation, app.h will not log anything, but anytime something goes wrong, it will log an error or
warning message. By default, logging is done by a simple printf to stdout (or in the case of wasm, a `console.log`). 
As some applications may need a different behavior, such as writing out a log file, it is possible to override the 
default logging behavior through a define like this:

    #define APP_IMPLEMENTATION
    #define APP_LOG( ctx, message ) ( my_log_func( ctx, message ) )
    #include "app.h"

where `my_log_func` is your own logging function. Just like for the memory allocators, the `ctx` parameter is optional,
and is just a `void*` value which is passed through. But in the case of logging, it will be passed through as the value
off the `logctx` parameter passed into `app_run`. 


app_run
-------

    int app_run( int (*app_proc)( app_t*, void* ), void* user_data, void* memctx, void* logctx )

Creates a new app instance, calls the given app_proc and waits for it to return. Then it destroys the app instance.

* app_proc - function pointer to the user defined starting point of the app. The parameters to that function are:
    app_t* a pointer to the app instance. This is an opaque type, and it is passed to all other functions in the API.
    void* pointer to the user defined data that was passed as the `user_data` parameter to `app_run`.
* user_data - pointer to user defined data which will be passed through to app_proc. May be NULL.
* memctx - pointer to user defined data which will be passed through to custom APP_MALLOC/APP_FREE calls. May be NULL.
* logctx - pointer to user defined data to be passed through to custom APP_LOG calls. May be NULL.
When app_run is called, it will perform all the initialization needed by app.h, and create an `app_t*` instance. It will
then call the user-specified `app_proc`, and wait for it to return. The `app_t*` instance will be passed to `app_proc`,
and can be used to call other functions in the API. When returning from `app_proc`, a return value is specified, and
`app_run` will perform termination and cleanup, and destroy the `app_t*` instance, and then return the same value it got
from the `app_proc`. After `app_run` returns, the `app_t*` value is no longer valid for use in any API calls.


app_yield
---------

    app_state_t app_yield( app_t* app )

Allows for app.h and the operating system to perform internal house keeping and updates. It should be called on each
iteration of your main loop.

The return value can be either `APP_STATE_NORMAL` or `APP_STATE_EXIT_REQUESTED`. `APP_STATE_EXIT_REQUESTED` means that
the user have requested the app to terminate, e.g. by pressing the *close* button on the window, and the user defined
`app_proc` needs to handle this, by either returning (to signal that the app should terminate) or by calling
`app_cancel_exit` to ignore the request. A typical pattern is to display a message to the user to confirm that the app
should exit. In the case of `APP_STATE_NORMAL`, there is no need to do anything.

Note: for wasm builds, only `APP_STATE_NORMAL` is supported, and it will never return `APP_STATE_EXIT_REQUESTED`


app_cancel_exit
---------------

    void app_cancel_exit( app_t* app )

Used to reset the `APP_STATE_EXIT_REQUESTED` state. See `app_yield` for details. 


app_title
---------

    void app_title( app_t* app, char const* title )

Sets the name of the application, which is displayed in the task switcher and in the title bar of the window or browser.


app_time_count
--------------

    APP_U64 app_time_count( app_t* app )

Returns the current value of the high precision clock. The epoch is undefined, and the resolution can vary between
systems. Use `app_time_freq` to convert to seconds. Typical use is to make two calls to `app_time_count` and calculate
the difference, to measure the time elapsed between the two calls.


app_time_freq
-------------

    APP_U64 app_time_freq( app_t* app )

Returns the number of clock ticks per second of the high precision clock. An example use case could be:

    APP_U64 current_count = app_time_count( app );
    APP_U64 delta_count = current_count - previous_count;
    double delta_time = ( (double) delta_count ) / ( (double) app_time_freq( app ) );
    previous_count = current_count;

to measure the time between two iterations through your main loop.


app_pointer
-----------

    void app_pointer( app_t* app, app_pointer_t pointer )


Shows or hides the mouse pointer. `APP_POINTER_HIDE` is used to hide the pointer, and `APP_POINTER_SHOW` is used to
show the pointer (the default). The pointer will be hidden when over the client area of the window or browser, but
not when over the title bar or resize regions.


app_pointer_pos
---------------

    void app_pointer_pos( app_t* app, int x, int y )

Set the position of the mouse pointer, in window coordinates. The function `app_coordinates_bitmap_to_window` can be
used to convert between the coordinate system of the currently displayed bitmap and that of the window.

Note: Setting the pointer position is not supported for wasm/browser builds


app_interpolation
-----------------

    void app_interpolation( app_t* app, app_interpolation_t interpolation )

app.h supports two different modes of displaying a bitmap. When using `APP_INTERPOLATION_LINEAR`, the bitmap will be
drawn with bilinear interpolations, stretching it to fill the window (maintaining aspect ratio), giving it a smooth, if
somwhat blurry, look. With `APP_INTERPOLATION_NONE`, scaling will only be done on whole pixel ratios, using no
interpolation, which is particularly suitable to maintain the clean, precise look of pixel art.
`APP_INTERPOLATION_LINEAR` is the default setting.


app_screenmode
--------------

    void app_screenmode( app_t* app, app_screenmode_t screenmode )

Switch between windowed mode and fullscreen mode. `APP_SCREENMODE_WINDOW` is used to select windowed mode, and
`APP_SCREENMODE_FULLSCREEN` is used to switch to fullscreen mode. `APP_SCREENMODE_FULLSCREEN` is the default. Note that
the app.h fullscreenmode is of the "borderless windowed" type, meaning that fullscreen mode just means that the window
is set to cover the entire screen, and its borders are hidden. It does not imply any exclusive locking of GPU resources.
When switching from windowed to fullscreen mode on a multi-display system, the app will go fullscreen on the display
that the window is currently on.


app_window_size
---------------

    void app_window_size( app_t* app, int width, int height )

Sets the size of the window. If currently in `APP_SCREENMODE_FULLSCREEN` screen mode, the setting will not take effect
until switching to `APP_SCREENMODE_WINDOW`. `width` and `height` specifies the size of the windows client area, not
counting borders, title bar or decorations.

Note: Setting the window size is not supported for wasm/browser builds


app_window_width/app_window_height
----------------------------------

    int app_window_width( app_t* app )
    int app_window_height( app_t* app )

Returns the current dimensions of the window (which might have been resized by the user). Regardless of whether the app
is currently in fullscreen or windowed mode, `app_window_width` and `app_window_height` returns the dimension the window
*would* have in windowed mode. Width and height specifies the size of the windows client area, not counting borders,
title bar or decorations.


app_window_pos
--------------

    void app_window_pos( app_t* app, int x, int y )

Sets the position of the top left corner of the window. If currently in `APP_SCREENMODE_FULLSCREEN` screen mode, the
setting will not take effect until switching to `APP_SCREENMODE_WINDOW`.

Note: Setting the window position is not supported for wasm/browser builds


app_window_x/app_window_y
-------------------------

    int app_window_x( app_t* app )
    int app_window_y( app_t* app )

Returns the current position of the windows top left corner. Regardless of whether the app is currently in fullscreen or
windowed mode, `app_window_x` and `app_window_y` returns the position the window *would* have in windowed mode.


app_displays
------------

    app_displays_t app_displays( app_t* app )

Returns a list of all displays connected to the system. For each display, the following fields are reported:
* id - a platform specific string used to identify the display. Useful for saving which display was in use.
* x, y - position of the top left corner of the display, relative to the primary dispay which is always at 0,0.
* width, height - size of the display, in pixels.

Note: For wasm/browser builds, only the display that the browser window is currently located on will be reported,
and only the width/height values contain valid information.


app_present
-----------

    void app_present( app_t* app, APP_U32 const* pixels_xbgr, int width, int height, APP_U32 mod_xbgr, APP_U32 border_xbgr )

app.h provides a very minimal API for drawing - the only thing you can really do, is provide it with a bitmap for it to
display on the screen. It is then up to the rest of your program to implement code for drawing shapes or sprites onto
that bitmap. When all your drawing is done, you call `app_present` passing it the bitmap, and it will be displayed on
the screen. `app_present` takes the following parameters:
* pixels_xbgr - width x height number of pixels making up the bitmap to be presented, each pixel being a 32-bit unsigned
    integer where the highest 8 bits are not used, and the following 8-bit groups are blue, green and red channels. This
    parameter may be NULL, in which case no bitmap is drawn, to allow for custom rendering. See below for details.
* width, height - the horizontal and vertical dimensions of the bitmap
* mod_xbgr - an rgb color value which will be automatically multiplied with each pixel, component by component, before
    it is displayed. Can be used to for example fade the bitmap to/from black. Set to 0xffffff for no effect.
* border_xbgr - an rgb color value to be used as *border color*. The borders are the areas outside of the bitmap, which
    are visible when the window aspect ratio does not match that of the bitmap, so you get bars above or below it.

Since app.h uses opengl, you can also opt to not pass a bitmap to `app_present`, by passing NULL as the `pixels_xbgr`
parameter (in which case the rest of the parameters are ignored). When doing this, it is up to your program to draw by 
using opengl calls. In this case `app_present` will work as a call to SwapBuffers (or equivalent) only. Note that 
glSetViewPort will be automatically called whenever the window is resized.


app_sound
---------

    void app_sound( app_t* app, int sample_pairs_count, 
        void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data ), 
        void* user_data );

The api for playing sound samples is just as minimal as that for drawing. app.h provides a single, looping sound stream,
and it is up to your program to handle sound formats, voices and mixing. By calling `app_sound`, a sound stream of the 
specified size `sample_pairs_count` is initialized, and playback is started. If a `sample_pairs_count` of 0 is given, or
if a `sound_callback` of `NULL` is specified, sound playback will be stopped.

The sound buffer is in 44100hz, signed 16-bit stereo format, and the `sample_pairs_count` specifies how many left/right
pairs of 16-bit samples the buffer will contain. As an example, to specify a 1 second stream buffer, you would give the
value 44100 for the `sample_pairs_count` parameter, which would internally create a sound buffer of 176,400 bytes, from
44100 samples x 2 channels x 2 bytes per sample. However, since the bits per sample and number of channels are fixed,
the exact byte size of the sound buffer is not important in the app.h API. 

The `sound_callback` function will be called from an internal audio thread whenever new audio data is needed. It is 
passed a pointer to a buffer of `sample_pairs_count` stereo sample pairs (signed 16-bit, interleaved L/R), which your 
function must fill with audio data. The `user_data` pointer is passed through unchanged from the call to `app_sound`, and 
can be used to access application state or audio sources.

Note: For wasm/browser builds, `sample_pairs_count` will be ignored, and will always bee 2205 (unless specified as 0 to
stop the sound stream).


app_sound_volume
----------------

    void app_sound_volume( app_t* app, float volume )

Sets the output volume level of the sound stream, as a normalized linear value in the range 0.0f to 1.0f, inclusive.


app_input
---------

    app_input_t app_input( app_t* app )

Returns a list of input events which occured since the last call to `app_input`. Each input event can be of one of a
list of types, and the `type` field of the `app_input_event_t` struct specifies which type the event is. The `data`
struct is a union of fields, where only one of them is valid, depending on the value of `type`:
* APP_INPUT_KEY_DOWN, APP_INPUT_KEY_UP, APP_INPUT_DOUBLE_CLICK - use the `key` field of the `data` union, which contains
    one of the keyboard key identifiers from the `app_key_t` enumeration. `APP_INPUT_KEY_DOWN` means a key was pressed,
    or that it was held long enough for the key repeat to kick in. `APP_INPUT_KEY_UP` means a key was released, and is
    not sent on key repeats. For both these events, a `key` may also mean a mouse button, as those are listed in the
    `app_key_t` enum. `APP_INPUT_DOUBLE_CLICK` means a mouse button have been double clicked, and is not sent for
    keyboard keys.
* APP_INPUT_CHAR - use the `char_code` field of the `data` union, which contains the ASCII value of the key that was
    pressed. This is used to read text input, and will handle things like upper/lower case, and characters which
    requires multiple keys to be pressed in sequence to generate one input. This means that generally, when a key is
    pressed, you will get both an `APP_INPUT_KEY_DOWN` event and an `APP_INPUT_CHAR` event - just use the one you are
    interested in, and ignore the other.
* APP_INPUT_MOUSE_MOVE - use the `mouse_pos` field of the `data` union, which contains the x and y position of the
    mouse pointer, in window coordinates. The function `app_coordinates_window_to_bitmap` can be used to convert between
    the coordinate system of the window and that of the currently displayed bitmap. The `APP_INPUT_MOUSE_MOVE` event is
    sent whenever the user moves the mouse, as long as the window has focus and the pointer is inside its client area.
* APP_INPUT_MOUSE_DELTA - use the `mouse_delta` field of the `data` union, which contains the horizontal and vertical
    offset which the mouse has been moved by. Ideally, these values should be in normalized -1.0f to 1.0f range, but as
    there is no standardisation on the hardware and os level for this, it is not possible to do, so instead the value
    have been scaled to give roughly normalized -1.0f to 1.0f values on a typical setup. For serious use, sensitivity
    settings and/or user calibration is recommended. The `APP_INPUT_MOUSE_DELTA` event is sent whenever the user moves
    the mouse, regardless of whether the window has focus or whether the pointer is inside the window or not. The
    `APP_INPUT_MOUSE_DELTA` event is a better option for reading relative mouse movements than using the
    `APP_INPUT_MOUSE_MOVE` event together with `app_pointer_pos` to re-center the pointer on every update.
* APP_INPUT_SCROLL_WHEEL - use the `wheel_delta` field of the `data` union, which contains the number of clicks by which
    the scroll wheel on the mouse was turned, where positive values indicate that the wheel have been rotated away from
    the user, and negative values means it has turned towards the user. The `APP_INPUT_SCROLL_WHEEL` is sent every time
    the user turns the scroll wheel, as long as the window has focus.


app_coordinates_window_to_bitmap
--------------------------------

    void app_coordinates_window_to_bitmap( app_t* app, int width, int height, int* x, int* y )

Functions in the `app.h` API expects and returns coordinates in the windows coordinate system, where 0, 0 is the top
left corner of the windows client area (the area inside of the window borders, excluding title bar and decorations), and
width, height is the dimension, in pixels, of the client area. It is often desirable to translate a position given in
window coordinates into a position on the bitmap that is being displayed - taking into account whether the window is in
fullscreen or windowed mode, and how the bitmap is stretched or padded depending on which interpolation mode is being
used: `APP_INTERPOLATION_NONE` or `APP_INTERPOLATION_LINEAR`. `app_coordinates_window_to_bitmap` performs this
translation, and is called with the following parameters:
* width, height - dimensions of the bitmap being presented, the same as the ones passed to `app_present`.
* x, y - pointers to integer values containing the coordinate, in the coordinate system of the window, to be translated.
    When the function returns, their values will have been updated with the corresponding position in the coordinate
    system of the bitmap.


app_coordinates_bitmap_to_window
--------------------------------

    void app_coordinates_bitmap_to_window( app_t* app, int width, int height, int* x, int* y )

This performs the opposite translation to `app_coordinates_window_to_bitmap` - it converts a position given in the
coordinate system of the bitmap into the coordinate system of the window. See `app_coordinates_window_to_bitmap` for
details.


app_gamepad
-----------

    app_gamepad_status_t app_gamepad( app_t* app, int index, app_gamepad_t* gamepad, app_gamepad_vibration_t* vibration )

Used to query the state of a connected gamepad, and/or to set vibration feedback. The `index` parameter specifies which 
gamepad to query, in the range 0 to 3. Both `gamepad` and `vibration` are optional and may be NULL.

* index - the zero-based index of the gamepad to query or set vibration state for, between 0 and 3 inclusively.
* gamepad - pointer to an `app_gamepad_t` struct that will be filled with the current state of the specified gamepad. 
    This parameter is optional, and can be NULL if gamepad state is not to be read.
* vibration - pointer to an `app_gamepad_vibration_t` struct that specifies the vibration state to apply to the
    specified gamepad. This parameter is optional, and can be NULL if no vibration state is to be applied.

The return value is an `app_gamepad_status_t`, which will be either `APP_GAMEPAD_STATUS_CONNECTED` or 
`APP_GAMEPAD_STATUS_NOT_CONNECTED`, indicating whether a gamepad is currently connected at the given index.

The `app_gamepad_t` struct contains:
* `buttons` - bitmask of currently pressed buttons. Use the values in the `app_gamepad_button_t` enum to mask each button 
    value.
* `trigger_left`, `trigger_right` - analog trigger values, 0–255.
* `stick_left_x`, `stick_left_y`, `stick_right_x`, `stick_right_y` - analog stick positions as signed 16-bit values, in 
    the range -32768 to 32767, where a negative value indicates left or down, and a positive value indicates right or up.

The `app_gamepad_vibration_t` struct contains:
* `low_frequency`, `high_frequency` - vibration strengths (0–65535) for the low- and high-frequency motors.
* `duration_ms` - duration of vibration in milliseconds.

Note: If both `gamepad` and `vibration` are NULL, the function simply returns the connection status for the specified 
gamepad, without querying state or setting vibration. 
