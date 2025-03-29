/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

app.h - v0.6 - Small cross-platform base framework for graphical apps.

Do this:
    #define APP_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/


#ifndef app_h
#define app_h

#ifndef APP_S16
    #define APP_S16 short
#endif
#ifndef APP_U32
    #define APP_U32 unsigned int
#endif
#ifndef APP_U64
    #define APP_U64 unsigned long long
#endif


typedef struct app_t app_t;
int app_t_size( void );

int app_init( app_t*, void* logctx, void* fatalctx );
void app_term( app_t* );

typedef enum app_state_t { APP_STATE_EXIT_REQUESTED, APP_STATE_NORMAL, } app_state_t;
app_state_t app_yield( app_t* app );
void app_cancel_exit( app_t* app );

void app_title( app_t* app, char const* title );

char const* app_cmdline( app_t* app );
char const* app_filename( app_t* app );
char const* app_userdata( app_t* app );
char const* app_appdata( app_t* app );

APP_U64 app_time_count( app_t* app );
APP_U64 app_time_freq( app_t* app );

typedef enum app_log_level_t { APP_LOG_LEVEL_INFO, APP_LOG_LEVEL_WARNING, APP_LOG_LEVEL_ERROR, } app_log_level_t;
void app_log( app_t* app, app_log_level_t level, char const* message );
void app_fatal_error( app_t* app, char const* message );

void app_pointer( app_t* app, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y );
void app_pointer_default( app_t* app, int* width, int* height, APP_U32* pixels_abgr, int* hotspot_x, int* hotspot_y );

void app_pointer_pos( app_t* app, int x, int y );
int app_pointer_x( app_t* app );
int app_pointer_y( app_t* app );

void app_pointer_limit( app_t* app, int x, int y, int width, int height );
void app_pointer_limit_off( app_t* app );

typedef enum app_interpolation_t { APP_INTERPOLATION_NONE, APP_INTERPOLATION_LINEAR, } app_interpolation_t;
void app_interpolation( app_t* app, app_interpolation_t interpolation );

typedef enum app_screenmode_t { APP_SCREENMODE_WINDOW, APP_SCREENMODE_FULLSCREEN, } app_screenmode_t;
void app_screenmode( app_t* app, app_screenmode_t screenmode );

void app_window_size( app_t* app, int width, int height );
int app_window_width( app_t* app );
int app_window_height( app_t* app );

void app_window_pos( app_t* app, int x, int y );
int app_window_x( app_t* app );
int app_window_y( app_t* app );

typedef struct app_display_t
    {
    char id[ 64 ];
    int x;
    int y;
    int width;
    int height;
    } app_display_t ;

typedef struct app_displays_t { app_display_t* displays; int count; } app_displays_t;
app_displays_t app_displays( app_t* app );

void app_present( app_t* app, APP_U32 const* pixels_xbgr, int width, int height, APP_U32 mod_xbgr, APP_U32 border_xbgr );

void app_sound( app_t* app, int sample_pairs_count,
    void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data ), void* user_data );
void app_sound_volume( app_t* app, float volume );

typedef enum app_key_t { APP_KEY_INVALID, APP_KEY_LBUTTON, APP_KEY_RBUTTON, APP_KEY_CANCEL, APP_KEY_MBUTTON,
    APP_KEY_XBUTTON1, APP_KEY_XBUTTON2, APP_KEY_BACK, APP_KEY_TAB, APP_KEY_CLEAR, APP_KEY_RETURN, APP_KEY_SHIFT,
    APP_KEY_CONTROL, APP_KEY_MENU, APP_KEY_PAUSE, APP_KEY_CAPITAL, APP_KEY_KANA, APP_KEY_HANGUL = APP_KEY_KANA,
    APP_KEY_JUNJA, APP_KEY_FINAL, APP_KEY_HANJA, APP_KEY_KANJI = APP_KEY_HANJA, APP_KEY_ESCAPE, APP_KEY_CONVERT,
    APP_KEY_NONCONVERT, APP_KEY_ACCEPT, APP_KEY_MODECHANGE, APP_KEY_SPACE, APP_KEY_PRIOR, APP_KEY_NEXT, APP_KEY_END,
    APP_KEY_HOME, APP_KEY_LEFT, APP_KEY_UP, APP_KEY_RIGHT, APP_KEY_DOWN, APP_KEY_SELECT, APP_KEY_PRINT, APP_KEY_EXEC,
    APP_KEY_SNAPSHOT, APP_KEY_INSERT, APP_KEY_DELETE, APP_KEY_HELP, APP_KEY_0, APP_KEY_1, APP_KEY_2, APP_KEY_3,
    APP_KEY_4, APP_KEY_5, APP_KEY_6, APP_KEY_7, APP_KEY_8, APP_KEY_9, APP_KEY_A, APP_KEY_B, APP_KEY_C, APP_KEY_D,
    APP_KEY_E, APP_KEY_F, APP_KEY_G, APP_KEY_H, APP_KEY_I, APP_KEY_J, APP_KEY_K, APP_KEY_L, APP_KEY_M, APP_KEY_N,
    APP_KEY_O, APP_KEY_P, APP_KEY_Q, APP_KEY_R, APP_KEY_S, APP_KEY_T, APP_KEY_U, APP_KEY_V, APP_KEY_W, APP_KEY_X,
    APP_KEY_Y, APP_KEY_Z, APP_KEY_LWIN, APP_KEY_RWIN, APP_KEY_APPS, APP_KEY_SLEEP, APP_KEY_NUMPAD0, APP_KEY_NUMPAD1,
    APP_KEY_NUMPAD2, APP_KEY_NUMPAD3, APP_KEY_NUMPAD4, APP_KEY_NUMPAD5, APP_KEY_NUMPAD6, APP_KEY_NUMPAD7,
    APP_KEY_NUMPAD8, APP_KEY_NUMPAD9, APP_KEY_MULTIPLY, APP_KEY_ADD, APP_KEY_SEPARATOR, APP_KEY_SUBTRACT,
    APP_KEY_DECIMAL, APP_KEY_DIVIDE, APP_KEY_F1, APP_KEY_F2, APP_KEY_F3, APP_KEY_F4, APP_KEY_F5, APP_KEY_F6, APP_KEY_F7,
    APP_KEY_F8, APP_KEY_F9, APP_KEY_F10, APP_KEY_F11, APP_KEY_F12, APP_KEY_F13, APP_KEY_F14, APP_KEY_F15, APP_KEY_F16,
    APP_KEY_F17, APP_KEY_F18, APP_KEY_F19, APP_KEY_F20, APP_KEY_F21, APP_KEY_F22, APP_KEY_F23, APP_KEY_F24,
    APP_KEY_NUMLOCK, APP_KEY_SCROLL, APP_KEY_LSHIFT, APP_KEY_RSHIFT, APP_KEY_LCONTROL, APP_KEY_RCONTROL, APP_KEY_LMENU,
    APP_KEY_RMENU, APP_KEY_BROWSER_BACK, APP_KEY_BROWSER_FORWARD, APP_KEY_BROWSER_REFRESH, APP_KEY_BROWSER_STOP,
    APP_KEY_BROWSER_SEARCH, APP_KEY_BROWSER_FAVORITES, APP_KEY_BROWSER_HOME, APP_KEY_VOLUME_MUTE, APP_KEY_VOLUME_DOWN,
    APP_KEY_VOLUME_UP, APP_KEY_MEDIA_NEXT_TRACK, APP_KEY_MEDIA_PREV_TRACK, APP_KEY_MEDIA_STOP, APP_KEY_MEDIA_PLAY_PAUSE,
    APP_KEY_LAUNCH_MAIL, APP_KEY_LAUNCH_MEDIA_SELECT, APP_KEY_LAUNCH_APP1, APP_KEY_LAUNCH_APP2, APP_KEY_OEM_1,
    APP_KEY_OEM_PLUS, APP_KEY_OEM_COMMA, APP_KEY_OEM_MINUS, APP_KEY_OEM_PERIOD, APP_KEY_OEM_2, APP_KEY_OEM_3,
    APP_KEY_OEM_4, APP_KEY_OEM_5, APP_KEY_OEM_6, APP_KEY_OEM_7, APP_KEY_OEM_8, APP_KEY_OEM_102, APP_KEY_PROCESSKEY,
    APP_KEY_ATTN, APP_KEY_CRSEL, APP_KEY_EXSEL, APP_KEY_EREOF, APP_KEY_PLAY, APP_KEY_ZOOM, APP_KEY_NONAME, APP_KEY_PA1,
    APP_KEY_OEM_CLEAR, APP_KEYCOUNT } app_key_t;

typedef enum app_input_type_t { APP_INPUT_KEY_DOWN, APP_INPUT_KEY_UP, APP_INPUT_DOUBLE_CLICK, APP_INPUT_CHAR,
    APP_INPUT_MOUSE_MOVE, APP_INPUT_MOUSE_DELTA, APP_INPUT_SCROLL_WHEEL, APP_INPUT_TABLET } app_input_type_t;

typedef enum app_pressed_t { APP_NOT_PRESSED, APP_PRESSED, } app_pressed_t;

typedef struct app_input_event_t
    {
    app_input_type_t type;
    union data_t
        {
        app_key_t key;
        char char_code;
        struct { int x; int y; } mouse_pos;
        struct { float x; float y; } mouse_delta;
        float wheel_delta;
        struct { int x; int y; float pressure; app_pressed_t tip; app_pressed_t lower; app_pressed_t upper; } tablet;
        } data;
    } app_input_event_t;

typedef struct app_input_t { app_input_event_t* events; int count; } app_input_t;
app_input_t app_input( app_t* app );

void app_coordinates_window_to_bitmap( app_t* app, int width, int height, int* x, int* y );
void app_coordinates_bitmap_to_window( app_t* app, int width, int height, int* x, int* y );

#endif /* app_h */


/**

app.h
=====

Small cross-platform base framework for graphical apps.


Example
-------

Here's a basic sample program which starts a windowed app and plots random pixels.

    #define  APP_IMPLEMENTATION
    #define  APP_WINDOWS
    #include "app.h"

    #include <stdlib.h> // for rand and __argc/__argv
    #include <string.h> // for memset

    int main( int argc, char** argv ) {
        (void) argc, argv;

        app_t app = {0};
        if( app_init( &app, NULL, NULL ) ) {
            return EXIT_FAILURE;
        }

        APP_U32 canvas[ 320 * 200 ]; // a place for us to draw stuff
        memset( canvas, 0xC0, sizeof( canvas ) ); // clear to grey
        app_screenmode( &app, APP_SCREENMODE_WINDOW );

        // keep running until the user close the window
        while( app_yield( &app ) != APP_STATE_EXIT_REQUESTED ) {
            // plot a random pixel on the canvas
            int x = rand() % 320;
            int y = rand() % 200;
            APP_U32 color = rand() | ( (APP_U32) rand() << 16 );
            canvas[ x + y * 320 ] = color;

            // display the canvas
            app_present( &app, canvas, 320, 200, 0xffffff, 0x000000 );
        }
        return EXIT_SUCCESS;
    }

    // pass-through so the program will build with either /SUBSYSTEM:WINDOWS or /SUBSYSTEN:CONSOLE
    extern "C" int __stdcall WinMain( struct HINSTANCE__*, struct HINSTANCE__*, char*, int ) { return main( __argc, __argv ); }



API Documentation
-----------------

app.h is a single-header library, and does not need any .lib files or other binaries, or any build scripts. To use it,
you just include app.h to get the API declarations. To get the definitions, you must include app.h from *one* single
C or C++ file, and #define the symbol `APP_IMPLEMENTATION` before you do.

As app.h is a cross platform library, you must also define which platform you are running on, like this for Windows:

    #define APP_IMPLEMENTATION
    #define APP_WINDOWS
    #include "app.h"

Or like this for other platforms:
    #define APP_IMPLEMENTATION
    #define APP_SDL
    #include "app.h"

### Customization

There are a few different things in app.h which are configurable by #defines. Most of the API use the `int` data type,
for integer values where the exact size is not important. However, for some functions, it specifically makes use of 16,
32 and 64 bit data types. These default to using `short`, `unsigned int` and `unsigned long long` by default, but can be
redefined by #defining APP_S16, APP_U32 and APP_U64 respectively, before including app.h. This is useful if you, for
example, use the types from `<stdint.h>` in the rest of your program, and you want app.h to use compatible types. In
this case, you would include app.h using the following code:

    #define APP_S16 int16_t
    #define APP_U32 uint32_t
    #define APP_U64 uint64_t
    #include "app.h"

Note that when customizing the data types, you need to use the same definition in every place where you include app.h,
as they affect the declarations as well as the definitions.

The rest of the customizations only affect the implementation, so will only need to be defined in the file where you
have the #define APP_IMPLEMENTATION.


#### Custom logging function

There's a bunch of things being logged when app.h runs. It will log an informational entry with the date and time for
when the app is started and stopped, it will log warnings when non-essential initialization fails, and it will log
error messages when things go wrong. By default, logging is done by a simple printf to stdout. As some applications may
need a different behavior, such as writing out a log file, it is possible to override the default logging behavior
through defines like this:

    #define APP_IMPLEMENTATION
    #define APP_LOG( ctx, level, message ) ( my_log_func( ctx, level, message ) )
    #include "app.h"

where `my_log_func` is your own logging function. Just like for the memory allocators, the `ctx` parameter is optional,
and is just a `void*` value which is passed through. But in the case of logging, it will be passed through as the value
off the `logctx` parameter passed into `app_init`. The `level` parameter specifies the severity level of the logging,
and can be used to direct different types of messages to different logging systems, or filter out messages of certain
severity level, e.g. supressing informational messages.


#### Custom fatal error function

As the app.h library works on the lowest level of your program, interfacing directly with the operating system, there
might occur errors which it can not recover from. In these cases, a *fatal error* will be reported. By default, when a
fatal error happens, app.h will print a message to stdout, show a messagebox to the user, and force exit the program.

It is possible to change this behaviour using the following define:

    #define APP_IMPLEMENTATION
    #define APP_FATAL_ERROR( ctx, message ) ( my_custom_fatal_error_func( ctx, message ) )
    #include "app.h"

where `my_custom_fatal_error_func` is your own error reporting function. The `ctx` parameter fills the same purpose as
for the allocator and logging functions, but here it is the `fatalctx` parameter to `app_init` which is passed through.


app_init
--------

    int app_init( app_t*, void* logctx, void* fatalctx );

When app_init is called, it will perform all the initialization needed by app.h for a given `app_t*` instance, returning `EXIT_SUCCESS` if it was initialized and `EXIT_FAILURE` otherwise.

* logctx - pointer to user defined data to be passed through to custom APP_LOG calls. May be NULL.
* fatalctx - pointer to user defined data to be passed through to custom APP_FATAL_ERROR calls. May be NULL.


app_t_size
----------

    int app_t_size( );

Returns the size of `app_t` to allow for dynamic allocation of it.


app_term
--------

    void app_term( app_t* );

Terminates the app instance. After `app_term` returns, the parameter `app_t*` value is no longer valid for use in any API calls.


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


app_cancel_exit
---------------

    void app_cancel_exit( app_t* app )

Used to reset the `APP_STATE_EXIT_REQUESTED` state. See `app_yield` for details.


app_title
---------

    void app_title( app_t* app, char const* title )

Sets the name of the application, which is displayed in the task switcher and in the title bar of the window.


app_cmdline
-----------

    char const* app_cmdline( app_t* app )

Returns the command line string used to launch the executable. This can be parsed to get command line arguments.


app_filename
------------

    char const* app_filename( app_t* app )

Returns the full filename and path of the executable. The first part of `app_cmdline` usually contains the name of the
executable, but not necessarily the full path, depending on how it was launched. `app_filename`, however, always returns
the full path.


app_userdata
------------

    char const* app_userdata( app_t* app )

Returns the full path to a directory where a users personal files can be stored. Depending on the access rights of the
user, it may or may not be possible to write data to the same location as the executable, and instead it must be stored
in a specific area designated by the operating system. `app_userdata` returns the path to the root if that directory.
A typical use for this is to store the users savegame files, by creating a subfolder corresponding to your app, and save
the data there.


app_appdata
-----------

    char const* app_appdata( app_t* app )

Returns the full path to a directory where application specific files can be stored. Similar to the location returned by
`app_userdata`, but suitable for application data shared between users. Typical use for this is to store the result of
cached calculations or temporary files.


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


app_log
-------

    void app_log( app_t* app, app_log_level_t level, char const* message )

app.h will do logging on certain events, e.q when the app starts and ends or when something goes wrong. As the logging
can be customized (see section on customization), it might be desirable for the program to do its own logging the same
way as app.h does it. By calling `app_log`, logging will be done the same way as it is done inside app.h, whether custom
logging or default logging is being used.


app_fatal_error
---------------

    void app_fatal_error( app_t* app, char const* message )

Same as with app_log, but for reporting fatal errors, `app_fatal_error` will report an error the same way as is done
internally in app.h, whether custom or default fatal error reporting is being used.


app_pointer
-----------

    void app_pointer( app_t* app, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y )

Sets the appearence current mouse pointer. `app_pointer` is called with the following parameters:

* width, height - the horizontal and vertical dimensions of the mouse pointer bitmap.
* pixels_abgr - width x height number of pixels making up the pointer bitmap, each pixel being a 32-bit unsigned integer
    where the highest 8 bits are the alpha channel, and the following 8-bit groups are blue, green and red channels.
* hotspot_x, hotspot_y - offset into the bitmap of the pointer origin, the center point it will be drawn at.


app_pointer_default
-------------------

    void app_pointer_default( app_t* app, int* width, int* height, APP_U32* pixels_abgr, int* hotspot_x, int* hotspot_y )

Retrieves the width, height, pixel data and hotspot for the default mouse pointer. Useful for restoring the default
pointer after using `app_pointer`, or for doing software rendered pointers. Called with the following parameters:

* width, height - pointers to integer values that are to receive the width and height of the pointer. May be NULL.
* pixels_abgr - width x height number of pixels to receive the pointer bitmap. May be NULL
* hotspot_x, hotspot_y - pointers to integer values that are to receive the hotspot coordinates. May be NULL.

A typical pattern for calling `app_pointer_default` is to first call it with `pixels_abgr` as NULL, to query the bitmaps
dimensions, and then call it again after preparing a large enough memory area.


app_pointer_pos
---------------

    void app_pointer_pos( app_t* app, int x, int y )

Set the position of the mouse pointer, in window coordinates. The function `app_coordinates_bitmap_to_window` can be
used to convert between the coordinate system of the currently displayed bitmap and that of the window.


app_pointer_limit
-----------------

    void app_pointer_limit( app_t* app, int x, int y, int width, int height )

Locks the mouse pointer movements to stay within the specified area, in window coordinates. The function
`app_coordinates_bitmap_to_window` can be used to convert between the coordinate system of the currently displayed
bitmap and that of the window.


app_pointer_limit_off
---------------------

    void app_pointer_limit_off( app_t* app )

Turns of the mouse pointer movement restriction, allowing the pointer to be moved freely again.


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
parameter (in which case the rest of the parameters are ignored). When doing this, it is up to your program to perform
drawing using opengl calls. In this case `app_present` will work as a call to SwapBuffers only. Note that glSetViewPort
will be automatically called whenever the window is resized.


app_sound_buffer_size
---------------------

    void app_sound_buffer_size( app_t* app, int sample_pairs_count )

The api for playing sound samples is just as minimal as that for drawing. app.h provides a single, looping sound stream,
and it is up to your program to handle sound formats, voices and mixing. By calling `app_sound_buffer_size`, a sound
stream of the specified size is initialized, and playback is started. If a `sample_pairs_count` of 0 is given, sound
playback will be stopped.

The sound buffer is in 44100hz, signed 16-bit stereo format, and the `sample_pairs_count` specified how many left/right
pairs of 16-bit samples the buffer will contain. As an example, to specify a 1 second stream buffer, you would give the
value 44100 for the `sample_pairs_count` parameter, which would internally create a sound buffer of 176,400 bytes, from
44100 samples x 2 channels x 2 bytes per sample. However, since the bits per sample and number of channels are fixed,
the exact byte size of the sound buffer is not important in the app.h API.


app_sound_position
------------------

    int app_sound_position( app_t* app )

Returns the current playback position of the sound stream, given in the number of sample pairs from the start of the
buffer. Typical use of a streaming sound buffer is to fill the buffer with data, wait for the playback position to get
passed the mid point of the buffer, and then write the next bit of data over the part that has already been played, and
so on.


app_sound_write
---------------

    void app_sound_write( app_t* app, int sample_pairs_offset, int sample_pairs_count, APP_S16 const* sample_pairs )

Writes sample data to the sound buffer. It takes the following parameters:

* sample_pairs_offset - the offset into the buffer of where to start writing, in number of sample pairs from the start.
* sample_pairs_count - the number of sample pairs to write. Must be less than the buffer size.
* sample_pairs - an array of sound samples, as signed 16-bit integers. Must be at least `sample_pairs_count` in length.

The `sample_pairs` parameter can be NULL, in which case the corresponding part of the buffer is cleared.


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
* APP_INPUT_TABLET - use the `tablet` field of the `data` union, which contains details about the pen used with a
    graphical tablet, if connected and installed. The `x` and `y` fields are the horizontal and vertical positions of
    the pen on the tablet, scaled to the coordinate system of the window. The function `app_coordinates_window_to_bitmap`
    can be used to convert between the coordinate system of the window and that of the currently displayed bitmap. The
    `pressure` field is the current pressure of the pen against the tablet, in normalized 0.0f to 1.0f range, inclusive,
    where 0.0f means no pressure and 1.0f means full pressure. The `tip` field is set to `APP_PRESSED` if the tip of the
    pen is touching the tablet at all, and to `APP_NOT_PRESSED` otherwise. The `upper` and `lower` fields indicate the
    current state of the buttons on the side of the pen. The "eraser" part of the pen is not currently supported.


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

*/

/*
----------------------
    IMPLEMENTATION
----------------------
*/
#ifdef APP_IMPLEMENTATION
#undef APP_IMPLEMENTATION

#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    OPENGL CODE - Shared between platform implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef APP_NULL

#if defined( APP_WINDOWS )

    #define _CRT_NONSTDC_NO_DEPRECATE
    #ifndef _CRT_SECURE_NO_WARNINGS
        #define _CRT_SECURE_NO_WARNINGS
    #endif
    #include <stddef.h>
    #define APP_GLCALLTYPE __stdcall
    typedef unsigned int APP_GLuint;
    typedef int APP_GLsizei;
    typedef unsigned int APP_GLenum;
    typedef int APP_GLint;
    typedef float APP_GLfloat;
    typedef char APP_GLchar;
    typedef unsigned char APP_GLboolean;
    typedef size_t APP_GLsizeiptr;
    typedef unsigned int APP_GLbitfield;

    #define APP_GL_FLOAT 0x1406
    #define APP_GL_FALSE 0
    #define APP_GL_FRAGMENT_SHADER 0x8b30
    #define APP_GL_VERTEX_SHADER 0x8b31
    #define APP_GL_COMPILE_STATUS 0x8b81
    #define APP_GL_LINK_STATUS 0x8b82
    #define APP_GL_INFO_LOG_LENGTH 0x8b84
    #define APP_GL_ARRAY_BUFFER 0x8892
    #define APP_GL_TEXTURE_2D 0x0de1
    #define APP_GL_TEXTURE0 0x84c0
    #define APP_GL_CLAMP 0x2900
    #define APP_GL_TEXTURE_WRAP_S 0x2802
    #define APP_GL_TEXTURE_WRAP_T 0x2803
    #define APP_GL_TEXTURE_MIN_FILTER 0x2801
    #define APP_GL_TEXTURE_MAG_FILTER 0x2800
    #define APP_GL_NEAREST 0x2600
    #define APP_GL_LINEAR 0x2601
    #define APP_GL_STATIC_DRAW 0x88e4
    #define APP_GL_RGBA 0x1908
    #define APP_GL_UNSIGNED_BYTE 0x1401
    #define APP_GL_COLOR_BUFFER_BIT 0x00004000
    #define APP_GL_TRIANGLE_FAN 0x0006

#elif defined( APP_SDL ) || defined( APP_WASM )

    #if defined( APP_WASM )
        #include <wajic_gl.h>
        #define WA_CORO_IMPLEMENT_NANOSLEEP
        #include <wajic_coro.h>
    #else
        #include <GL/glew.h>
        #include "SDL_opengl.h"
    #endif
    #define APP_GLCALLTYPE GLAPIENTRY
    typedef GLuint APP_GLuint;
    typedef GLsizei APP_GLsizei;
    typedef GLenum APP_GLenum;
    typedef GLint APP_GLint;
    typedef GLfloat APP_GLfloat;
    typedef GLchar APP_GLchar;
    typedef GLboolean APP_GLboolean;
    typedef GLsizeiptr APP_GLsizeiptr;
    typedef GLbitfield APP_GLbitfield;

    #define APP_GL_FLOAT GL_FLOAT
    #define APP_GL_FALSE GL_FALSE
    #define APP_GL_FRAGMENT_SHADER GL_FRAGMENT_SHADER
    #define APP_GL_VERTEX_SHADER GL_VERTEX_SHADER
    #define APP_GL_COMPILE_STATUS GL_COMPILE_STATUS
    #define APP_GL_LINK_STATUS GL_LINK_STATUS
    #define APP_GL_INFO_LOG_LENGTH GL_INFO_LOG_LENGTH
    #define APP_GL_ARRAY_BUFFER GL_ARRAY_BUFFER
    #define APP_GL_TEXTURE_2D GL_TEXTURE_2D
    #define APP_GL_TEXTURE0 GL_TEXTURE0
    #if defined( APP_WASM )
        #define APP_GL_CLAMP GL_CLAMP_TO_EDGE
    #else
        #define APP_GL_CLAMP GL_CLAMP
    #endif
    #define APP_GL_TEXTURE_WRAP_S GL_TEXTURE_WRAP_S
    #define APP_GL_TEXTURE_WRAP_T GL_TEXTURE_WRAP_T
    #define APP_GL_TEXTURE_MIN_FILTER GL_TEXTURE_MIN_FILTER
    #define APP_GL_TEXTURE_MAG_FILTER GL_TEXTURE_MAG_FILTER
    #define APP_GL_NEAREST GL_NEAREST
    #define APP_GL_LINEAR GL_LINEAR
    #define APP_GL_STATIC_DRAW GL_STATIC_DRAW
    #define APP_GL_RGBA GL_RGBA
    #define APP_GL_UNSIGNED_BYTE GL_UNSIGNED_BYTE
    #define APP_GL_COLOR_BUFFER_BIT GL_COLOR_BUFFER_BIT
    #define APP_GL_TRIANGLE_FAN GL_TRIANGLE_FAN

#else

    #error Undefined platform. Define APP_WINDOWS, APP_SDL, APP_WASM or APP_NULL.
    #define APP_GLCALLTYPE
    typedef int APP_GLuint;
    typedef int APP_GLsizei;
    typedef int APP_GLenum;
    typedef int APP_GLint;
    typedef int APP_GLfloat;
    typedef int APP_GLchar;
    typedef int APP_GLboolean;
    typedef int APP_GLsizeiptr;
    typedef int APP_GLbitfield;

#endif


#ifdef APP_REPORT_SHADER_ERRORS
    #include <string.h>
#endif

struct app_internal_opengl_t
    {

    APP_GLuint (APP_GLCALLTYPE* CreateShader) (APP_GLenum type);
    void (APP_GLCALLTYPE* ShaderSource) (APP_GLuint shader, APP_GLsizei count, APP_GLchar const* const* string, APP_GLint const* length);
    void (APP_GLCALLTYPE* CompileShader) (APP_GLuint shader);
    void (APP_GLCALLTYPE* GetShaderiv) (APP_GLuint shader, APP_GLenum pname, APP_GLint *params);
    APP_GLuint (APP_GLCALLTYPE* CreateProgram) (void);
    void (APP_GLCALLTYPE* AttachShader) (APP_GLuint program, APP_GLuint shader);
    void (APP_GLCALLTYPE* BindAttribLocation) (APP_GLuint program, APP_GLuint index, APP_GLchar const* name);
    void (APP_GLCALLTYPE* LinkProgram) (APP_GLuint program);
    void (APP_GLCALLTYPE* GetProgramiv) (APP_GLuint program, APP_GLenum pname, APP_GLint *params);
    void (APP_GLCALLTYPE* GenBuffers) (APP_GLsizei n, APP_GLuint *buffers);
    void (APP_GLCALLTYPE* BindBuffer) (APP_GLenum target, APP_GLuint buffer);
    void (APP_GLCALLTYPE* EnableVertexAttribArray) (APP_GLuint index);
    void (APP_GLCALLTYPE* VertexAttribPointer) (APP_GLuint index, APP_GLint size, APP_GLenum type, APP_GLboolean normalized, APP_GLsizei stride, void const* pointer);
    void (APP_GLCALLTYPE* GenTextures) (APP_GLsizei n, APP_GLuint* textures);
    void (APP_GLCALLTYPE* Enable) (APP_GLenum cap);
    void (APP_GLCALLTYPE* ActiveTexture) (APP_GLenum texture);
    void (APP_GLCALLTYPE* BindTexture) (APP_GLenum target, APP_GLuint texture);
    void (APP_GLCALLTYPE* TexParameteri) (APP_GLenum target, APP_GLenum pname, APP_GLint param);
    void (APP_GLCALLTYPE* DeleteBuffers) (APP_GLsizei n, APP_GLuint const* buffers);
    void (APP_GLCALLTYPE* DeleteTextures) (APP_GLsizei n, APP_GLuint const* textures);
    void (APP_GLCALLTYPE* BufferData) (APP_GLenum target, APP_GLsizeiptr size, void const *data, APP_GLenum usage);
    void (APP_GLCALLTYPE* UseProgram) (APP_GLuint program);
    void (APP_GLCALLTYPE* Uniform1i) (APP_GLint location, APP_GLint v0);
    void (APP_GLCALLTYPE* Uniform3f) (APP_GLint location, APP_GLfloat v0, APP_GLfloat v1, APP_GLfloat v2);
    APP_GLint (APP_GLCALLTYPE* GetUniformLocation) (APP_GLuint program, APP_GLchar const* name);
    void (APP_GLCALLTYPE* TexImage2D) (APP_GLenum target, APP_GLint level, APP_GLint internalformat, APP_GLsizei width, APP_GLsizei height, APP_GLint border, APP_GLenum format, APP_GLenum type, void const* pixels);
    void (APP_GLCALLTYPE* ClearColor) (APP_GLfloat red, APP_GLfloat green, APP_GLfloat blue, APP_GLfloat alpha);
    void (APP_GLCALLTYPE* Clear) (APP_GLbitfield mask);
    void (APP_GLCALLTYPE* DrawArrays) (APP_GLenum mode, APP_GLint first, APP_GLsizei count);
    void (APP_GLCALLTYPE* Viewport) (APP_GLint x, APP_GLint y, APP_GLsizei width, APP_GLsizei height);
    void (APP_GLCALLTYPE* DeleteShader) (APP_GLuint shader);
    void (APP_GLCALLTYPE* DeleteProgram) (APP_GLuint program);
    #ifdef APP_REPORT_SHADER_ERRORS
        void (APP_GLCALLTYPE* GetShaderInfoLog) (APP_GLuint shader, APP_GLsizei bufSize, APP_GLsizei *length, APP_GLchar *infoLog);
    #endif

    app_interpolation_t interpolation;
    int window_width;
    int window_height;

    APP_GLuint vertexbuffer;
    APP_GLuint texture;
    APP_GLuint shader;
    };


static int app_internal_opengl_init( app_t* app, struct app_internal_opengl_t* gl, app_interpolation_t interpolation,
    int window_width, int window_height )
    {
    (void) app;
    gl->interpolation = interpolation;
    gl->window_width = window_width;
    gl->window_height = window_height;

    char const* vs_source =
    #ifdef APP_WASM
        "precision highp float;\n"
    #else
        "#version 120\n"
    #endif
        "attribute vec4 pos;"
        "varying vec2 uv;"
        ""
        "void main( void )"
        "    {"
        "    gl_Position = vec4( pos.xy, 0.0, 1.0 );"
        "    uv = pos.zw;"
        "    }"
        ;

    char const* fs_source =
    #ifdef APP_WASM
        "precision highp float;\n"
    #else
        "#version 120\n"
    #endif
        "varying vec2 uv;"
        ""
        "uniform sampler2D texture;"
        "uniform vec3 modulate;"
        ""
        "void main(void)"
        "    {"
        "    gl_FragColor = texture2D( texture, uv ) * vec4( modulate, 1.0 );"
        "    }"
        ;

    #ifdef APP_REPORT_SHADER_ERRORS
        char error_message[ 1024 ];
    #endif

    APP_GLuint vs = gl->CreateShader( APP_GL_VERTEX_SHADER );
    gl->ShaderSource( vs, 1, (char const**) &vs_source, NULL );
    gl->CompileShader( vs );
    APP_GLint vs_compiled;
    gl->GetShaderiv( vs, APP_GL_COMPILE_STATUS, &vs_compiled );
    if( !vs_compiled )
        {
        #ifdef APP_REPORT_SHADER_ERRORS
            char const* prefix = "Vertex Shader Error: ";
            memcpy( error_message, prefix, strlen( prefix ) + 1 );
            int len = 0, written = 0;
            gl->GetShaderiv( vs, APP_GL_INFO_LOG_LENGTH, &len );
            gl->GetShaderInfoLog( vs, (APP_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written,
                error_message + strlen( prefix ) );
            app_fatal_error( app, error_message );
        #endif
        return 0;
        }

    APP_GLuint fs = gl->CreateShader( APP_GL_FRAGMENT_SHADER );
    gl->ShaderSource( fs, 1, (char const**) &fs_source, NULL );
    gl->CompileShader( fs );
    APP_GLint fs_compiled;
    gl->GetShaderiv( fs, APP_GL_COMPILE_STATUS, &fs_compiled );
    if( !fs_compiled )
        {
        #ifdef APP_REPORT_SHADER_ERRORS
            char const* prefix = "Fragment Shader Error: ";
            memcpy( error_message, prefix, strlen( prefix ) + 1 );
            int len = 0, written = 0;
            gl->GetShaderiv( vs, APP_GL_INFO_LOG_LENGTH, &len );
            gl->GetShaderInfoLog( fs, (APP_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written,
                error_message + strlen( prefix ) );
            app_fatal_error( app, error_message );
        #endif
        return 0;
        }


    APP_GLuint prg = gl->CreateProgram();
    gl->AttachShader( prg, fs );
    gl->AttachShader( prg, vs );
    gl->BindAttribLocation( prg, 0, "pos" );
    gl->LinkProgram( prg );

    APP_GLint linked;
    gl->GetProgramiv( prg, APP_GL_LINK_STATUS, &linked );
    if( !linked )
        {
        #ifdef APP_REPORT_SHADER_ERRORS
            char const* prefix = "Shader Link Error: ";
            memcpy( error_message, prefix, strlen( prefix ) + 1 );
            int len = 0, written = 0;
            gl->GetShaderiv( vs, APP_GL_INFO_LOG_LENGTH, &len );
            gl->GetShaderInfoLog( prg, (APP_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written,
                error_message + strlen( prefix ) );
            app_fatal_error( app, error_message );
        #endif
        return 0;
        }

    gl->shader = prg;
    gl->DeleteShader( fs );
    gl->DeleteShader( vs );

    gl->GenBuffers( 1, &gl->vertexbuffer );
    gl->BindBuffer( APP_GL_ARRAY_BUFFER, gl->vertexbuffer );
    gl->EnableVertexAttribArray( 0 );
    gl->VertexAttribPointer( 0, 4, APP_GL_FLOAT, APP_GL_FALSE, 4 * sizeof( APP_GLfloat ), 0 );

    gl->GenTextures( 1, &gl->texture );
    #ifndef APP_WASM
        // This enable call is not necessary when using fragment shaders, avoid logged warnings in WebGL
        gl->Enable( APP_GL_TEXTURE_2D );
    #endif
    gl->ActiveTexture( APP_GL_TEXTURE0 );
    gl->BindTexture( APP_GL_TEXTURE_2D, gl->texture );
    gl->TexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MIN_FILTER, APP_GL_NEAREST );
    gl->TexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MAG_FILTER, APP_GL_NEAREST );
    gl->TexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_WRAP_S, APP_GL_CLAMP );
    gl->TexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_WRAP_T, APP_GL_CLAMP );

    return 1;
    }


static int app_internal_opengl_term( struct app_internal_opengl_t* gl )
    {
    gl->DeleteProgram( gl->shader );
    gl->DeleteBuffers( 1, &gl->vertexbuffer);
    gl->DeleteTextures( 1, &gl->texture );
    return 1;
    }


static int app_internal_opengl_present( struct app_internal_opengl_t* gl, APP_U32 const* pixels_xbgr, int width,
    int height, APP_U32 mod_xbgr, APP_U32 border_xbgr )
    {
    float x1 = 0.0f, y1 = 0.0f, x2 = (float) gl->window_width, y2 = (float) gl->window_height;

    if( gl->interpolation == APP_INTERPOLATION_LINEAR )
        {
        float hscale = gl->window_width / (float) width;
        float vscale = gl->window_height / (float) height;
        float pixel_scale = hscale < vscale ? hscale : vscale;

        float hborder = ( gl->window_width - pixel_scale * width ) / 2.0f;
        float vborder = ( gl->window_height - pixel_scale * height ) / 2.0f;
        x1 = hborder;
        y1 = vborder;
        x2 = x1 + pixel_scale * width;
        y2 = y1 + pixel_scale * height;
        }
    else
        {
        int hscale = gl->window_width / width;
        int vscale = gl->window_height / height;
        int pixel_scale = pixel_scale = hscale < vscale ? hscale : vscale;
        pixel_scale = pixel_scale < 1 ? 1 : pixel_scale;

        int hborder = ( gl->window_width - pixel_scale * width ) / 2;
        int vborder = ( gl->window_height - pixel_scale * height ) / 2;
        x1 = (float) hborder;
        y1 = (float) vborder;
        x2 = x1 + (float) ( pixel_scale * width );
        y2 = y1 + (float) ( pixel_scale * height );
        }

    x1 = ( x1 / gl->window_width ) * 2.0f - 1.0f;
    x2 = ( x2 / gl->window_width ) * 2.0f - 1.0f;
    y1 = ( y1 / gl->window_height ) * 2.0f - 1.0f;
    y2 = ( y2 / gl->window_height ) * 2.0f - 1.0f;

    APP_GLfloat vertices[ 16 ];
    vertices[  0 ] = x1;
    vertices[  1 ] = y1;
    vertices[  2 ] = 0.0f;
    vertices[  3 ] = 1.0f;

    vertices[  4 ] = x2;
    vertices[  5 ] = y1;
    vertices[  6 ] = 1.0f;
    vertices[  7 ] = 1.0f;

    vertices[  8 ] = x2;
    vertices[  9 ] = y2;
    vertices[ 10 ] = 1.0f;
    vertices[ 11 ] = 0.0f;

    vertices[ 12 ] = x1;
    vertices[ 13 ] = y2;
    vertices[ 14 ] = 0.0f;
    vertices[ 15 ] = 0.0f;

    gl->BindBuffer( APP_GL_ARRAY_BUFFER, gl->vertexbuffer );
    gl->BufferData( APP_GL_ARRAY_BUFFER, 4 * 4 * sizeof( APP_GLfloat ), vertices, APP_GL_STATIC_DRAW );
    gl->VertexAttribPointer( 0, 4, APP_GL_FLOAT, APP_GL_FALSE, 4 * sizeof( APP_GLfloat ), 0 );

    float mod_r = ( ( mod_xbgr >> 16 ) & 0xff ) / 255.0f;
    float mod_g = ( ( mod_xbgr >> 8  ) & 0xff ) / 255.0f;
    float mod_b = ( ( mod_xbgr       ) & 0xff ) / 255.0f;

    gl->UseProgram( gl->shader );
    gl->Uniform1i( gl->GetUniformLocation( gl->shader, "texture" ), 0 );
    gl->Uniform3f( gl->GetUniformLocation( gl->shader, "modulate" ), mod_r, mod_g, mod_b );

    gl->ActiveTexture( APP_GL_TEXTURE0 );
    gl->BindTexture( APP_GL_TEXTURE_2D, gl->texture );
    gl->TexImage2D( APP_GL_TEXTURE_2D, 0, APP_GL_RGBA, width, height, 0, APP_GL_RGBA, APP_GL_UNSIGNED_BYTE, pixels_xbgr );

    if( gl->interpolation == APP_INTERPOLATION_LINEAR )
        {
        gl->TexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MIN_FILTER, APP_GL_LINEAR );
        gl->TexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MAG_FILTER, APP_GL_LINEAR );
        }
    else
        {
        gl->TexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MIN_FILTER, APP_GL_NEAREST );
        gl->TexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MAG_FILTER, APP_GL_NEAREST );
        }

    float b = ( ( border_xbgr >> 16 ) & 0xff ) / 255.0f;
    float g = ( ( border_xbgr >> 8  ) & 0xff ) / 255.0f;
    float r = ( ( border_xbgr       ) & 0xff ) / 255.0f;
    gl->ClearColor( r, g, b, 1.0f );
    gl->Clear( APP_GL_COLOR_BUFFER_BIT );
    gl->DrawArrays( APP_GL_TRIANGLE_FAN, 0, 4 );

    return 1;
    }


static void app_internal_opengl_resize( struct app_internal_opengl_t* gl, int width, int height )
    {
    gl->Viewport( 0, 0, width, height );
    gl->window_width = width;
    gl->window_height = height;
    }


static void app_internal_opengl_interpolation( struct app_internal_opengl_t* gl, app_interpolation_t interpolation )
    {
    gl->interpolation = interpolation;
    }


#endif // #ifndef APP_NULL


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    NULL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined( APP_NULL )


struct app_t { void* dummy; };
int app_t_size( ) { return sizeof( app_t ); }
int app_init( app_t*, void* logctx, void* fatalctx ) { return EXIT_SUCCESS; }
void app_term( app_t* ) { }
app_state_t app_yield( app_t* app ) { return APP_STATE_NORMAL; }
void app_cancel_exit( app_t* app ) { }
void app_title( app_t* app, char const* title ) { }
char const* app_cmdline( app_t* app ) { return ""; }
char const* app_filename( app_t* app ) { return ""; }
char const* app_userdata( app_t* app ) { return ""; }
char const* app_appdata( app_t* app ) { return ""; }
APP_U64 app_time_count( app_t* app ) { return 0ULL; }
APP_U64 app_time_freq( app_t* app ) { return 0ULL; }
void app_log( app_t* app, app_log_level_t level, char const* message ) { }
void app_fatal_error( app_t* app, char const* message ) { }
void app_pointer( app_t* app, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y ) { }
void app_pointer_default( app_t* app, int* width, int* height, APP_U32* pixels_abgr, int* hotspot_x, int* hotspot_y ) { }
void app_pointer_pos( app_t* app, int x, int y ) { }
int app_pointer_x( app_t* app ) { return 0; }
int app_pointer_y( app_t* app ) { return 0;}
void app_pointer_limit( app_t* app, int x, int y, int width, int height ) { }
void app_pointer_limit_off( app_t* app ) { }
void app_interpolation( app_t* app, app_interpolation_t interpolation ) { }
void app_screenmode( app_t* app, app_screenmode_t screenmode ) { }
void app_window_size( app_t* app, int width, int height ) { }
int app_window_width( app_t* app ) { return 0; }
int app_window_height( app_t* app ) { return 0; }
void app_window_pos( app_t* app, int x, int y ) { }
int app_window_x( app_t* app ) { return 0; }
int app_window_y( app_t* app ) { return 0; }
app_displays_t app_displays( app_t* app ) { app_displays_t ret = { 0 }; return ret; }
void app_present( app_t* app, APP_U32 const* pixels_xbgr, int width, int height, APP_U32 mod_xbgr, APP_U32 border_xbgr ) { }
void app_sound( app_t* app, int sample_pairs_count, void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data ), void* user_data ) { }
void app_sound_volume( app_t* app, float volume ) { }
app_input_t app_input( app_t* app ) { app_input_t ret = { 0 }; return ret; }
void app_coordinates_window_to_bitmap( app_t* app, int width, int height, int* x, int* y ) { }
void app_coordinates_bitmap_to_window( app_t* app, int width, int height, int* x, int* y );


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    WINDOWS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#elif defined( APP_WINDOWS )

#define _CRT_NONSTDC_NO_DEPRECATE
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0601
#undef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601// requires Windows 7 minimum
#endif
// 0x0400=Windows NT 4.0, 0x0500=Windows 2000, 0x0501=Windows XP, 0x0502=Windows Server 2003, 0x0600=Windows Vista,
// 0x0601=Windows 7, 0x0602=Windows 8, 0x0603=Windows 8.1, 0x0A00=Windows 10,
#define _WINSOCKAPI_
#pragma warning( push )
#pragma warning( disable: 4619 ) // #pragma warning: there is no warning number 'nnnn'
#pragma warning( disable: 4668 ) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#pragma warning( disable: 4768 ) // __declspec attributes before linkage specification are ignored
#pragma warning( disable: 4255 ) // 'function' : no function prototype given: converting '()' to '(void)'
#pragma warning( disable: 4917 ) // 'declarator' : a GUID can only be associated with a class, interface or namespace
#define _NTDDSCM_H_ /* Fixes the error of mismatched pragma warning push/pop in Windows SDK 10.0.17763.0 */
#include <windows.h>
//#include <shlobj.h>
#pragma warning( pop )
#ifndef __TINYC__
#pragma comment( lib, "user32.lib" )
#pragma comment( lib, "gdi32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "shell32.lib" )
#else
#pragma comment( lib, "user32")
#pragma comment( lib, "gdi32")
#endif

#include <time.h>
#include <stdio.h>

#pragma warning( push )
#pragma warning( disable: 4668 ) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#include <math.h>
#pragma warning( pop )

#ifndef APP_LOG
    #if defined(__cplusplus)
        #define APP_LOG( ctx, level, message ) ::printf( "%s\n", message )
    #else
        #define APP_LOG( ctx, level, message ) printf( "%s\n", message )
    #endif
#endif

#ifndef APP_FATAL_ERROR
    #if defined(__cplusplus)
        #define APP_FATAL_ERROR( ctx, message ) { ::printf( "FATAL ERROR: %s\n", message ); \
            ::MessageBoxA( 0, message, "Fatal Error!", MB_OK | MB_ICONSTOP ); ::_flushall(); ::_exit( 0xff ); }
    #else
        #define APP_FATAL_ERROR( ctx, message ) { printf( "FATAL ERROR: %s\n", message ); \
            MessageBoxA( 0, message, "Fatal Error!", MB_OK | MB_ICONSTOP ); _flushall(); _exit( 0xff ); }
    #endif
#endif


#ifndef APP_WINDOWED_WS_STYLE
    #define APP_WINDOWED_WS_STYLE WS_OVERLAPPEDWINDOW
#endif

#ifndef APP_WINDOWED_WS_EX_STYLE
    #define APP_WINDOWED_WS_EX_STYLE 0
#endif


typedef struct APP_LOGCONTEXTA
    {
    char lcName[ 40 ]; UINT lcOptions; UINT lcStatus; UINT lcLocks; UINT lcMsgBase; UINT lcDevice; UINT lcPktRate;
    DWORD lcPktData; DWORD lcPktMode; DWORD lcMoveMask; DWORD lcBtnDnMask; DWORD lcBtnUpMask; LONG lcInOrgX;
    LONG lcInOrgY; LONG lcInOrgZ; LONG lcInExtX; LONG lcInExtY; LONG lcInExtZ; LONG lcOutOrgX; LONG lcOutOrgY;
    LONG lcOutOrgZ; LONG lcOutExtX; LONG lcOutExtY; LONG lcOutExtZ; DWORD lcSensX; DWORD lcSensY; DWORD lcSensZ;
    BOOL lcSysMode; int lcSysOrgX; int lcSysOrgY; int lcSysExtX; int lcSysExtY; DWORD lcSysSensX; DWORD lcSysSensY;
    } APP_LOGCONTEXTA;
typedef struct APP_AXIS { LONG axMin; LONG axMax; UINT axUnits; DWORD axResolution; } APP_AXIS;
typedef struct APP_PACKET { DWORD pkButtons; LONG pkX; LONG pkY; UINT pkNormalPressure; } APP_PACKET;
DECLARE_HANDLE( APP_HMGR );
DECLARE_HANDLE( APP_HCTX );
#define APP_WTI_DEVICES 100
#define APP_WTI_DDCTXS 400 /* 1.1 */
#define APP_CXO_MESSAGES 0x0004
#define APP_DVC_NPRESSURE 15
#define APP_PK_BUTTONS 0x0040 // button information
#define APP_PK_X 0x0080 // x axis
#define APP_PK_Y 0x0100 // y axis
#define APP_PK_NORMAL_PRESSURE 0x0400 // normal or tip pressure
#define APP_PACKETDATA APP_PK_X | APP_PK_Y | APP_PK_BUTTONS | APP_PK_NORMAL_PRESSURE
#define APP_PACKETMODE 0
#define APP_WT_PACKET 0x7FF0



////// DSOUND DEFINITIONS ////////

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef struct _DSOUND_WAVEFORMATEX
    {
    WORD        wFormatTag;
    WORD        nChannels;
    DWORD       nSamplesPerSec;
    DWORD       nAvgBytesPerSec;
    WORD        nBlockAlign;
    WORD        wBitsPerSample;
    WORD        cbSize;
    } DSOUND_WAVEFORMATEX;

typedef struct _DSBUFFERDESC
    {
    DWORD           dwSize;
    DWORD           dwFlags;
    DWORD           dwBufferBytes;
    DWORD           dwReserved;
    DSOUND_WAVEFORMATEX*  lpwfxFormat;
    } DSBUFFERDESC;

typedef struct _DSBPOSITIONNOTIFY
    {
    DWORD           dwOffset;
    HANDLE          hEventNotify;
    } DSBPOSITIONNOTIFY;


typedef struct _DSCAPS DSCAPS;
typedef struct _DSBCAPS DSBCAPS;
typedef struct _DSEFFECTDESC DSEFFECTDESC;
struct IDirectSound8;

typedef struct IDirectSoundBuffer8 { struct IDirectSoundBuffer8Vtbl* lpVtbl; } IDirectSoundBuffer8;
typedef struct IDirectSoundBuffer8Vtbl IDirectSoundBuffer8Vtbl;

struct IDirectSoundBuffer8Vtbl
{
    // IUnknown methods
    HRESULT (STDMETHODCALLTYPE *QueryInterface)       (IDirectSoundBuffer8*, REFIID, LPVOID*);
    ULONG (STDMETHODCALLTYPE *AddRef)        (IDirectSoundBuffer8*);
    ULONG (STDMETHODCALLTYPE *Release)       (IDirectSoundBuffer8*);

    // IDirectSoundBuffer methods
    HRESULT (STDMETHODCALLTYPE *GetCaps)              (IDirectSoundBuffer8*, DSBCAPS* pDSBufferCaps);
    HRESULT (STDMETHODCALLTYPE *GetCurrentPosition)   (IDirectSoundBuffer8*, LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor);
    HRESULT (STDMETHODCALLTYPE *GetFormat)            (IDirectSoundBuffer8*, DSOUND_WAVEFORMATEX* pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten);
    HRESULT (STDMETHODCALLTYPE *GetVolume)            (IDirectSoundBuffer8*, LPLONG plVolume);
    HRESULT (STDMETHODCALLTYPE *GetPan)               (IDirectSoundBuffer8*, LPLONG plPan);
    HRESULT (STDMETHODCALLTYPE *GetFrequency)         (IDirectSoundBuffer8*, LPDWORD pdwFrequency);
    HRESULT (STDMETHODCALLTYPE *GetStatus)            (IDirectSoundBuffer8*, LPDWORD pdwStatus);
    HRESULT (STDMETHODCALLTYPE *Initialize)           (IDirectSoundBuffer8*, struct IDirectSound8* pDirectSound, DSBUFFERDESC* pcDSBufferDesc);
    HRESULT (STDMETHODCALLTYPE *Lock)                 (IDirectSoundBuffer8*, DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *Play)                 (IDirectSoundBuffer8*, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *SetCurrentPosition)   (IDirectSoundBuffer8*, DWORD dwNewPosition);
    HRESULT (STDMETHODCALLTYPE *SetFormat)            (IDirectSoundBuffer8*, DSOUND_WAVEFORMATEX* pcfxFormat);
    HRESULT (STDMETHODCALLTYPE *SetVolume)            (IDirectSoundBuffer8*, LONG lVolume);
    HRESULT (STDMETHODCALLTYPE *SetPan)               (IDirectSoundBuffer8*, LONG lPan);
    HRESULT (STDMETHODCALLTYPE *SetFrequency)         (IDirectSoundBuffer8*, DWORD dwFrequency);
    HRESULT (STDMETHODCALLTYPE *Stop)                 (IDirectSoundBuffer8*);
    HRESULT (STDMETHODCALLTYPE *Unlock)               (IDirectSoundBuffer8*, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2);
    HRESULT (STDMETHODCALLTYPE *Restore)              (IDirectSoundBuffer8*);

    // IDirectSoundBuffer8 methods
    HRESULT (STDMETHODCALLTYPE *SetFX)                (IDirectSoundBuffer8*, DWORD dwEffectsCount, DSEFFECTDESC* pDSFXDesc, LPDWORD pdwResultCodes);
    HRESULT (STDMETHODCALLTYPE *AcquireResources)     (IDirectSoundBuffer8*, DWORD dwFlags, DWORD dwEffectsCount, LPDWORD pdwResultCodes);
    HRESULT (STDMETHODCALLTYPE *GetObjectInPath)      (IDirectSoundBuffer8*, REFGUID rguidObject, DWORD dwIndex, REFGUID rguidInterface, LPVOID *ppObject);
};

#define IDirectSoundBuffer8_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectSoundBuffer8_Lock(p,a,b,c,d,e,f,g)        (p)->lpVtbl->Lock(p,a,b,c,d,e,f,g)
#define IDirectSoundBuffer8_Unlock(p,a,b,c,d)            (p)->lpVtbl->Unlock(p,a,b,c,d)
#define IDirectSoundBuffer8_Restore(p)                   (p)->lpVtbl->Restore(p)
#define IDirectSoundBuffer8_GetCurrentPosition(p,a,b)    (p)->lpVtbl->GetCurrentPosition(p,a,b)
#define IDirectSoundBuffer8_Play(p,a,b,c)                (p)->lpVtbl->Play(p,a,b,c)
#define IDirectSoundBuffer8_SetVolume(p,a)               (p)->lpVtbl->SetVolume(p,a)
#define IDirectSoundBuffer8_Release(p)                  (p)->lpVtbl->Release(p)


typedef struct IDirectSound8 { struct IDirectSound8Vtbl* lpVtbl; } IDirectSound8;
typedef struct IDirectSound8Vtbl IDirectSound8Vtbl;

struct IDirectSound8Vtbl
{
    // IUnknown methods
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IDirectSound8*, REFIID, LPVOID*);
    ULONG (STDMETHODCALLTYPE *AddRef)        (IDirectSound8*);
    ULONG (STDMETHODCALLTYPE *Release)       (IDirectSound8*);

    // IDirectSound methods
    HRESULT (STDMETHODCALLTYPE *CreateSoundBuffer)    (IDirectSound8*, DSBUFFERDESC* pcDSBufferDesc, struct IDirectSoundBuffer8** ppDSBuffer, void* pUnkOuter);
    HRESULT (STDMETHODCALLTYPE *GetCaps)              (IDirectSound8*, DSCAPS* pDSCaps);
    HRESULT (STDMETHODCALLTYPE *DuplicateSoundBuffer) (IDirectSound8*, struct IDirectSoundBuffer8* pDSBufferOriginal, struct IDirectSoundBuffer8* *ppDSBufferDuplicate);
    HRESULT (STDMETHODCALLTYPE *SetCooperativeLevel)  (IDirectSound8*, HWND hwnd, DWORD dwLevel);
    HRESULT (STDMETHODCALLTYPE *Compact)              (IDirectSound8*);
    HRESULT (STDMETHODCALLTYPE *GetSpeakerConfig)     (IDirectSound8*, LPDWORD pdwSpeakerConfig);
    HRESULT (STDMETHODCALLTYPE *SetSpeakerConfig)     (IDirectSound8*, DWORD dwSpeakerConfig);
    HRESULT (STDMETHODCALLTYPE *Initialize)           (IDirectSound8*, LPCGUID pcGuidDevice);

    // IDirectSound8 methods
    HRESULT (STDMETHODCALLTYPE *VerifyCertification)  (IDirectSound8*, LPDWORD pdwCertified);
};

#define IDirectSound8_Release(p)                  (p)->lpVtbl->Release(p)
#define IDirectSound8_CreateSoundBuffer(p,a,b,c)  (p)->lpVtbl->CreateSoundBuffer(p,a,b,c)
#define IDirectSound8_SetCooperativeLevel(p,a,b)  (p)->lpVtbl->SetCooperativeLevel(p,a,b)


typedef struct IDirectSoundNotify { struct IDirectSoundNotifyVtbl* lpVtbl; } IDirectSoundNotify;
typedef struct IDirectSoundNotifyVtbl IDirectSoundNotifyVtbl;

struct IDirectSoundNotifyVtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IDirectSoundNotify*, REFIID, LPVOID*);
    ULONG (STDMETHODCALLTYPE *AddRef)        (IDirectSoundNotify*);
    ULONG (STDMETHODCALLTYPE *Release)       (IDirectSoundNotify*);
    HRESULT (STDMETHODCALLTYPE *SetNotificationPositions) (IDirectSoundNotify*, DWORD dwPositionNotifies, DSBPOSITIONNOTIFY* pcPositionNotifies);
};

#define IDirectSoundNotify_Release(p)                       (p)->lpVtbl->Release(p)
#define IDirectSoundNotify_SetNotificationPositions(p,a,b)  (p)->lpVtbl->SetNotificationPositions(p,a,b)


#define DS_OK                           S_OK
#define DSERR_BUFFERLOST                MAKE_HRESULT(1, 0x878, 150)

#define DSSCL_NORMAL                0x00000001
#define DSBCAPS_CTRLVOLUME          0x00000080
#define DSBCAPS_CTRLPOSITIONNOTIFY  0x00000100
#define DSBCAPS_GLOBALFOCUS         0x00008000
#define DSBCAPS_GETCURRENTPOSITION2 0x00010000
#define DSBPLAY_LOOPING             0x00000001
#define DSBVOLUME_MIN               -10000


#ifdef __cplusplus
};
#endif // __cplusplus


///// END DSOUND DEFINITIONS //////


struct app_t
    {
    void* logctx;
    void* fatalctx;
    app_interpolation_t interpolation;
    app_screenmode_t screenmode;

    BOOL initialized;
    BOOL closed;

    char exe_path[ 260 ];
    char userdata_path[ 260 ];
    char appdata_path[ 260 ];
    char const* cmdline;

    HINSTANCE hinstance;
    HWND hwnd;
    LRESULT (CALLBACK *user_wndproc)( app_t*, HWND, UINT, WPARAM, LPARAM );

    HDC hdc;
    HICON icon;
    BOOL has_focus;
    BOOL is_minimized;

    struct app_internal_opengl_t gl;
    HMODULE gl_dll;
    HGLRC gl_context;
    PROC (APP_GLCALLTYPE* wglGetProcAddress) (LPCSTR);
    HGLRC (APP_GLCALLTYPE* wglCreateContext) (HDC);
    BOOL (APP_GLCALLTYPE* wglDeleteContext) (HGLRC);
    BOOL (APP_GLCALLTYPE* wglMakeCurrent) (HDC, HGLRC);
    BOOL (APP_GLCALLTYPE* wglSwapIntervalEXT) (int);

    UINT (WINAPI *GetRawInputDataPtr)( HRAWINPUT, UINT, LPVOID, PUINT, UINT );

    HANDLE sound_notifications[ 2 ];
    HMODULE dsound_dll;
    struct IDirectSound8* dsound;
    struct IDirectSoundBuffer8* dsoundbuf;
    HANDLE sound_thread_handle;
    volatile LONG exit_sound_thread;
    int sample_pairs_count;
    int sound_level;
    void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data );
    void* sound_user_data;

    HCURSOR current_pointer;

    BOOL clip_cursor;
    RECT clip_rect;

    app_input_event_t input_events[ 1024 ];
    int input_count;

    int windowed_x;
    int windowed_y;
    int windowed_h;
    int windowed_w;
    int fullscreen_width;
    int fullscreen_height;

    int display_count;
    app_display_t displays[ 16 ];
    HMONITOR displays_hmonitor[ 16 ];

    struct
        {
        HMODULE wintab_dll;
        APP_HCTX context;
        int max_pressure;

        UINT (WINAPI *WTInfo)( UINT, UINT, LPVOID );
        APP_HCTX (WINAPI *WTOpen)( HWND, APP_LOGCONTEXTA*, BOOL );
        BOOL (WINAPI *WTClose)( APP_HCTX );
        BOOL (WINAPI *WTEnable)( APP_HCTX, BOOL );
        BOOL (WINAPI *WTPacket)( APP_HCTX, UINT, LPVOID );
        } tablet;
    };


static app_key_t app_internal_vkcode_to_appkey( app_t* app, int vkcode )
    {
    int map[ 256 * 2 ] = { APP_KEY_INVALID, 0x00, APP_KEY_LBUTTON, 0x01, APP_KEY_RBUTTON, 0x02, APP_KEY_CANCEL, 0x03, APP_KEY_MBUTTON, 0x04,
        APP_KEY_XBUTTON1, 0x05, APP_KEY_XBUTTON2, 0x06, -1, 0x07, APP_KEY_BACK, 0x08, APP_KEY_TAB, 0x09, -1, 0x0A, -1, 0x0B, APP_KEY_CLEAR, 0x0C,
        APP_KEY_RETURN, 0x0D, -1, 0x0E, -1, 0x0F, APP_KEY_SHIFT, 0x10, APP_KEY_CONTROL, 0x11, APP_KEY_MENU, 0x12, APP_KEY_PAUSE, 0x13,
        APP_KEY_CAPITAL, 0x14, APP_KEY_KANA, 0x15, -1, 0x16, APP_KEY_JUNJA, 0x17, APP_KEY_FINAL, 0x18, APP_KEY_HANJA, 0x19, -1, 0x1A,
        APP_KEY_ESCAPE, 0x1B, APP_KEY_CONVERT, 0x1C, APP_KEY_NONCONVERT, 0x1D, APP_KEY_ACCEPT, 0x1E, APP_KEY_MODECHANGE, 0x1F, APP_KEY_SPACE, 0x20,
        APP_KEY_PRIOR, 0x21, APP_KEY_NEXT, 0x22, APP_KEY_END, 0x23, APP_KEY_HOME, 0x24, APP_KEY_LEFT, 0x25, APP_KEY_UP, 0x26, APP_KEY_RIGHT, 0x27,
        APP_KEY_DOWN, 0x28, APP_KEY_SELECT, 0x29, APP_KEY_PRINT, 0x2A, APP_KEY_EXEC, 0x2B, APP_KEY_SNAPSHOT, 0x2C, APP_KEY_INSERT, 0x2D,
        APP_KEY_DELETE, 0x2E, APP_KEY_HELP, 0x2F, APP_KEY_0, 0x30, APP_KEY_1, 0x31, APP_KEY_2, 0x32, APP_KEY_3, 0x33, APP_KEY_4, 0x34,
        APP_KEY_5, 0x35, APP_KEY_6, 0x36, APP_KEY_7, 0x37, APP_KEY_8, 0x38, APP_KEY_9, 0x39, -1, 0x3A, -1, 0x3B, -1, 0x3C, -1, 0x3D, -1, 0x3E,
        -1, 0x3F, -1, 0x40, APP_KEY_A, 0x41, APP_KEY_B, 0x42, APP_KEY_C, 0x43, APP_KEY_D, 0x44, APP_KEY_E, 0x45, APP_KEY_F, 0x46, APP_KEY_G, 0x47,
        APP_KEY_H, 0x48, APP_KEY_I, 0x49, APP_KEY_J, 0x4A, APP_KEY_K, 0x4B, APP_KEY_L, 0x4C, APP_KEY_M, 0x4D, APP_KEY_N, 0x4E, APP_KEY_O, 0x4F,
        APP_KEY_P, 0x50, APP_KEY_Q, 0x51, APP_KEY_R, 0x52, APP_KEY_S, 0x53, APP_KEY_T, 0x54, APP_KEY_U, 0x55, APP_KEY_V, 0x56, APP_KEY_W, 0x57,
        APP_KEY_X, 0x58, APP_KEY_Y, 0x59, APP_KEY_Z, 0x5A, APP_KEY_LWIN, 0x5B, APP_KEY_RWIN, 0x5C, APP_KEY_APPS, 0x5D, -1, 0x5E, APP_KEY_SLEEP, 0x5F,
        APP_KEY_NUMPAD0, 0x60, APP_KEY_NUMPAD1, 0x61, APP_KEY_NUMPAD2, 0x62, APP_KEY_NUMPAD3, 0x63, APP_KEY_NUMPAD4, 0x64, APP_KEY_NUMPAD5, 0x65,
        APP_KEY_NUMPAD6, 0x66, APP_KEY_NUMPAD7, 0x67, APP_KEY_NUMPAD8, 0x68, APP_KEY_NUMPAD9, 0x69, APP_KEY_MULTIPLY, 0x6A, APP_KEY_ADD, 0x6B,
        APP_KEY_SEPARATOR, 0x6C, APP_KEY_SUBTRACT, 0x6D, APP_KEY_DECIMAL, 0x6E, APP_KEY_DIVIDE, 0x6F, APP_KEY_F1, 0x70, APP_KEY_F2, 0x71,
        APP_KEY_F3, 0x72, APP_KEY_F4, 0x73, APP_KEY_F5, 0x74, APP_KEY_F6, 0x75, APP_KEY_F7, 0x76, APP_KEY_F8, 0x77, APP_KEY_F9, 0x78,
        APP_KEY_F10, 0x79, APP_KEY_F11, 0x7A, APP_KEY_F12, 0x7B, APP_KEY_F13, 0x7C, APP_KEY_F14, 0x7D, APP_KEY_F15, 0x7E, APP_KEY_F16, 0x7F,
        APP_KEY_F17, 0x80, APP_KEY_F18, 0x81, APP_KEY_F19, 0x82, APP_KEY_F20, 0x83, APP_KEY_F21, 0x84, APP_KEY_F22, 0x85, APP_KEY_F23, 0x86,
        APP_KEY_F24, 0x87, -1, 0x88, -1, 0x89, -1, 0x8A, -1, 0x8B, -1, 0x8C, -1, 0x8D, -1, 0x8E, -1, 0x8F, APP_KEY_NUMLOCK, 0x90,
        APP_KEY_SCROLL, 0x91, -1, 0x92, -1, 0x93, -1, 0x94, -1, 0x95, -1, 0x96, -1, 0x97, -1, 0x98, -1, 0x99, -1, 0x9A, -1, 0x9B, -1, 0x9C, -1, 0x9D,
        -1, 0x9E, -1, 0x9F, APP_KEY_LSHIFT, 0xA0, APP_KEY_RSHIFT, 0xA1, APP_KEY_LCONTROL, 0xA2, APP_KEY_RCONTROL, 0xA3, APP_KEY_LMENU, 0xA4,
        APP_KEY_RMENU, 0xA5, APP_KEY_BROWSER_BACK, 0xA6, APP_KEY_BROWSER_FORWARD, 0xA7, APP_KEY_BROWSER_REFRESH, 0xA8, APP_KEY_BROWSER_STOP, 0xA9,
        APP_KEY_BROWSER_SEARCH, 0xAA, APP_KEY_BROWSER_FAVORITES, 0xAB, APP_KEY_BROWSER_HOME, 0xAC, APP_KEY_VOLUME_MUTE, 0xAD,
        APP_KEY_VOLUME_DOWN, 0xAE, APP_KEY_VOLUME_UP, 0xAF, APP_KEY_MEDIA_NEXT_TRACK, 0xB0, APP_KEY_MEDIA_PREV_TRACK, 0xB1, APP_KEY_MEDIA_STOP, 0xB2,
        APP_KEY_MEDIA_PLAY_PAUSE, 0xB3, APP_KEY_LAUNCH_MAIL, 0xB4, APP_KEY_LAUNCH_MEDIA_SELECT, 0xB5, APP_KEY_LAUNCH_APP1, 0xB6,
        APP_KEY_LAUNCH_APP2, 0xB7, -1, 0xB8, -1, 0xB9, APP_KEY_OEM_1, 0xBA, APP_KEY_OEM_PLUS, 0xBB, APP_KEY_OEM_COMMA, 0xBC, APP_KEY_OEM_MINUS, 0xBD,
        APP_KEY_OEM_PERIOD, 0xBE, APP_KEY_OEM_2, 0xBF, APP_KEY_OEM_3, 0xC0, -1, 0xC1, -1, 0xC2, -1, 0xC3, -1, 0xC4, -1, 0xC5, -1, 0xC6, -1, 0xC7,
        -1, 0xC8, -1, 0xC9, -1, 0xCA, -1, 0xCB, -1, 0xCC, -1, 0xCD, -1, 0xCE, -1, 0xCF, -1, 0xD0, -1, 0xD1, -1, 0xD2, -1, 0xD3, -1, 0xD4, -1, 0xD5,
        -1, 0xD6, -1, 0xD7, -1, 0xD8, -1, 0xD9, -1, 0xDA, APP_KEY_OEM_4, 0xDB, APP_KEY_OEM_5, 0xDC, APP_KEY_OEM_6, 0xDD, APP_KEY_OEM_7, 0xDE,
        APP_KEY_OEM_8, 0xDF, -1, 0xE0, -1, 0xE1, APP_KEY_OEM_102, 0xE2, -1, 0xE3, -1, 0xE4, APP_KEY_PROCESSKEY, 0xE5, -1, 0xE6, -1, 0xE7, -1, 0xE8,
        -1, 0xE9, -1, 0xEA, -1, 0xEB, -1, 0xEC, -1, 0xED, -1, 0xEE, -1, 0xEF, -1, 0xF0, -1, 0xF1, -1, 0xF2, -1, 0xF3, -1, 0xF4, -1, 0xF5,
        APP_KEY_ATTN, 0xF6, APP_KEY_CRSEL, 0xF7, APP_KEY_EXSEL, 0xF8, APP_KEY_EREOF, 0xF9, APP_KEY_PLAY, 0xFA, APP_KEY_ZOOM, 0xFB,
        APP_KEY_NONAME, 0xFC, APP_KEY_PA1, 0xFD, APP_KEY_OEM_CLEAR, 0xFE, -1, 0xFF, };
    if( vkcode < 0 || vkcode >= sizeof( map ) / ( 2 * sizeof( *map ) ) ) return APP_KEY_INVALID;
    if( map[ vkcode * 2 + 1 ] != vkcode )
        {
        app_log( app, APP_LOG_LEVEL_ERROR, "Keymap definition error" );
        return APP_KEY_INVALID;
        }
    return (app_key_t) map[ vkcode * 2 ];
    }


static void app_internal_add_input_event( app_t* app, app_input_event_t* event )
    {
    if( app->has_focus )
        {
        if( app->input_count < sizeof( app->input_events ) / sizeof( *app->input_events ) )
            app->input_events[ app->input_count++ ] = *event;
        }
    }


static RECT app_internal_rect( int left, int top, int right, int bottom )
    {
    RECT r; r.left = left; r.top = top; r.right = right; r.bottom = bottom; return r;
    }


static BOOL app_internal_tablet_init( app_t* app )
    {
    app->tablet.wintab_dll = LoadLibraryA( "Wintab32.dll" );
    if( !app->tablet.wintab_dll ) return FALSE;

    app->tablet.WTInfo = ( UINT (WINAPI*)( UINT, UINT, LPVOID ) )
        (uintptr_t ) GetProcAddress( app->tablet.wintab_dll, "WTInfoA" );
    app->tablet.WTOpen = ( APP_HCTX (WINAPI*)( HWND, APP_LOGCONTEXTA*, BOOL ) )
        (uintptr_t ) GetProcAddress( app->tablet.wintab_dll, "WTOpenA" );
    app->tablet.WTClose = ( BOOL (WINAPI*)( APP_HCTX ) )
        (uintptr_t ) GetProcAddress( app->tablet.wintab_dll, "WTClose" );
    app->tablet.WTEnable = ( BOOL (WINAPI*)( APP_HCTX, BOOL ) )
        (uintptr_t ) GetProcAddress( app->tablet.wintab_dll, "WTEnable" );
    app->tablet.WTPacket = ( BOOL (WINAPI*)( APP_HCTX, UINT, LPVOID ) )
        (uintptr_t ) GetProcAddress( app->tablet.wintab_dll, "WTPacket" );

    if( !app->tablet.WTInfo( 0 ,0, NULL ) ) return FALSE; // checks if tablet is present

    APP_LOGCONTEXTA log_context;
    memset( &log_context, 0, sizeof( log_context ) );
    app->tablet.WTInfo( APP_WTI_DDCTXS, 0, &log_context );

    APP_AXIS pressure;
    memset( &pressure, 0, sizeof( pressure ) );
    app->tablet.WTInfo( APP_WTI_DEVICES, APP_DVC_NPRESSURE, &pressure );
    app->tablet.max_pressure = pressure.axMax;

    log_context.lcPktData = APP_PACKETDATA;
    log_context.lcOptions |= APP_CXO_MESSAGES;
    log_context.lcPktMode = APP_PACKETMODE;
    log_context.lcMoveMask = APP_PACKETDATA;
    log_context.lcBtnUpMask = log_context.lcBtnDnMask;
    log_context.lcOutOrgX = 0;
    log_context.lcOutOrgY = 0;
    log_context.lcOutExtX = GetSystemMetrics( SM_CXSCREEN) ;
    log_context.lcOutExtY = -GetSystemMetrics( SM_CYSCREEN );

    app->tablet.context = app->tablet.WTOpen( app->hwnd, &log_context, FALSE );
    if( !app->tablet.context ) return FALSE;
    return TRUE;
}


static BOOL app_internal_tablet_term( app_t* app )
    {
    if( app->tablet.context ) app->tablet.WTClose( app->tablet.context );
    if( app->tablet.wintab_dll ) FreeLibrary( app->tablet.wintab_dll );
    return TRUE;
    }



static LRESULT CALLBACK app_internal_wndproc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
    app_t* app = (app_t*)(uintptr_t) GetWindowLongPtr( hwnd, GWLP_USERDATA );
    if( !app ) return DefWindowProc( hwnd, message, wparam, lparam);

    app_input_event_t input_event;

    switch( message )
        {
        case WM_CHAR:
            input_event.type = APP_INPUT_CHAR; input_event.data.char_code = (char) wparam;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_LBUTTONDOWN:
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = APP_KEY_LBUTTON;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_LBUTTONUP:
            input_event.type = APP_INPUT_KEY_UP; input_event.data.key = APP_KEY_LBUTTON;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_LBUTTONDBLCLK:
            input_event.type = APP_INPUT_DOUBLE_CLICK; input_event.data.key = APP_KEY_LBUTTON;
            app_internal_add_input_event( app, &input_event );
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = APP_KEY_LBUTTON;
            app_internal_add_input_event(app, &input_event);
            break;
        case WM_RBUTTONDOWN:
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = APP_KEY_RBUTTON;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_RBUTTONUP:
            input_event.type = APP_INPUT_KEY_UP; input_event.data.key = APP_KEY_RBUTTON;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_RBUTTONDBLCLK:
            input_event.type = APP_INPUT_DOUBLE_CLICK; input_event.data.key = APP_KEY_RBUTTON;
            app_internal_add_input_event( app, &input_event );
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = APP_KEY_RBUTTON;
            app_internal_add_input_event(app, &input_event);
            break;
        case WM_MBUTTONDOWN:
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = APP_KEY_MBUTTON;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_MBUTTONUP:
            input_event.type = APP_INPUT_KEY_UP; input_event.data.key = APP_KEY_MBUTTON;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_MBUTTONDBLCLK:
            input_event.type = APP_INPUT_DOUBLE_CLICK; input_event.data.key = APP_KEY_MBUTTON;
            app_internal_add_input_event( app, &input_event );
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = APP_KEY_MBUTTON;
            app_internal_add_input_event(app, &input_event);
            break;
        case WM_XBUTTONDOWN:
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = HIWORD( wparam ) == 1 ? APP_KEY_XBUTTON1 :APP_KEY_XBUTTON2;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_XBUTTONUP:
            input_event.type = APP_INPUT_KEY_UP; input_event.data.key = HIWORD( wparam ) == 1 ? APP_KEY_XBUTTON1 :APP_KEY_XBUTTON2;
            app_internal_add_input_event( app, &input_event );
            break;
        case WM_XBUTTONDBLCLK:
            input_event.type = APP_INPUT_DOUBLE_CLICK; input_event.data.key = HIWORD( wparam ) == 1 ? APP_KEY_XBUTTON1 :APP_KEY_XBUTTON2;
            app_internal_add_input_event( app, &input_event );
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = HIWORD( wparam ) == 1 ? APP_KEY_XBUTTON1 :APP_KEY_XBUTTON2;
            app_internal_add_input_event(app, &input_event);
            break;
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            {
            input_event.type = APP_INPUT_KEY_DOWN;
            WPARAM vkcode = wparam;
            UINT scancode = (UINT)( ( lparam & 0x00ff0000 ) >> 16 );
            int extended  = ( lparam & 0x01000000 ) != 0;
            UINT const maptype = 3; //MAPVK_VSC_TO_VK_EX
            switch( vkcode )
                {
                case VK_SHIFT:
                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
            app_internal_add_input_event( app, &input_event );

                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) MapVirtualKey( scancode, maptype ) );
                    app_internal_add_input_event( app, &input_event );
            break;
                case VK_CONTROL:
                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
            app_internal_add_input_event( app, &input_event );

                    input_event.data.key = app_internal_vkcode_to_appkey( app, extended ? VK_RCONTROL : VK_LCONTROL );
                    app_internal_add_input_event( app, &input_event );
            break;
                case VK_MENU:
                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
                    app_internal_add_input_event( app, &input_event );

                    input_event.data.key = app_internal_vkcode_to_appkey( app, extended ? VK_RMENU : VK_LMENU );
                    app_internal_add_input_event( app, &input_event );
                    break;
                default:
                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
                    app_internal_add_input_event( app, &input_event );
                    break;
                }
            } break;

        case WM_HOTKEY:
            {
            input_event.type = APP_INPUT_KEY_DOWN;
            input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
            if( app->input_count < sizeof( app->input_events ) / sizeof( *app->input_events ) )
                app->input_events[ app->input_count++ ] = input_event;
            input_event.type = APP_INPUT_KEY_UP;
            if( app->input_count < sizeof( app->input_events ) / sizeof( *app->input_events ) )
                app->input_events[ app->input_count++ ] = input_event;
            } break;

        case WM_SYSKEYUP:
        case WM_KEYUP:
            {
            input_event.type = APP_INPUT_KEY_UP;
            WPARAM vkcode = wparam;
            UINT scancode = (UINT)( ( lparam & 0x00ff0000 ) >> 16 );
            int extended  = ( lparam & 0x01000000 ) != 0;
            UINT const maptype = 3; //MAPVK_VSC_TO_VK_EX
            switch( vkcode )
                {
                case VK_SHIFT:
                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
                    app_internal_add_input_event( app, &input_event );

                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) MapVirtualKey( scancode, maptype ) );
                    app_internal_add_input_event( app, &input_event );
                    break;
                case VK_CONTROL:
                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
                    app_internal_add_input_event( app, &input_event );

                    input_event.data.key = app_internal_vkcode_to_appkey( app, extended ? VK_RCONTROL : VK_LCONTROL );
                    app_internal_add_input_event( app, &input_event );
                    break;
                case VK_MENU:
                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
                    app_internal_add_input_event( app, &input_event );

                    input_event.data.key = app_internal_vkcode_to_appkey( app, extended ? VK_RMENU : VK_LMENU );
            app_internal_add_input_event( app, &input_event );
            break;
                default:
                    input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam );
            app_internal_add_input_event( app, &input_event );
            break;
                }
            } break;

        /*
        case WM_GESTURE:
            printf( "Gesture\n" );
            if( app->has_focus )
                {
                GESTUREINFO gesture_info = { sizeof( gesture_info ) };
                if( GetGestureInfo( (HGESTUREINFO) lparam, &gesture_info ) )
                    {
                    printf( "Gesture Info\n" );
                    static int prev = 0;
                    if( gesture_info.dwID == GID_PAN )
                        {
                        if( gesture_info.dwFlags & GF_BEGIN )
                            {
                            prev = gesture_info.ptsLocation.y;
                            }
                        else
                            {
                            int dist = gesture_info.ptsLocation.y - prev;
                            prev = gesture_info.ptsLocation.y;
                            float wheel_delta = (float)( dist / 200.0f );
                            printf( "Pan/Inertia: %d\n", dist );
                            if( app->input_count > 0 && app->input_events[ app->input_count - 1 ].type == APP_INPUT_SCROLL_WHEEL )
                                {
                                app_input_event_t* event = &app->input_events[ app->input_count - 1 ];
                                event->data.wheel_delta += wheel_delta;
                                }
                            else
                                {
                                input_event.type = APP_INPUT_SCROLL_WHEEL;
                                input_event.data.wheel_delta = wheel_delta;
                                app_internal_add_input_event( app, &input_event );
                                }
                            }
                        }
                    }
                }
            break;
        */
        case WM_MOUSEWHEEL:
            if( app->has_focus )
                {
                float const microsoft_mouse_wheel_constant = 120.0f;
                float wheel_delta = ( (float) GET_WHEEL_DELTA_WPARAM( wparam ) ) / microsoft_mouse_wheel_constant;
                if( app->input_count > 0 && app->input_events[ app->input_count - 1 ].type == APP_INPUT_SCROLL_WHEEL )
                    {
                    app_input_event_t* event = &app->input_events[ app->input_count - 1 ];
                    event->data.wheel_delta += wheel_delta;
                    }
                else
                    {
                    input_event.type = APP_INPUT_SCROLL_WHEEL;
                    input_event.data.wheel_delta = wheel_delta;
                    app_internal_add_input_event( app, &input_event );
                    }
                }
            break;

        case WM_MOUSEMOVE:
            if( app->has_focus )
                {
                POINT p;
                GetCursorPos( &p );
                ScreenToClient( app->hwnd, &p );
                int mouse_x = p.x;
                int mouse_y = p.y;

                input_event.type = APP_INPUT_MOUSE_MOVE;
                input_event.data.mouse_pos.x = mouse_x;
                input_event.data.mouse_pos.y = mouse_y;
                app_internal_add_input_event( app, &input_event );
                }
            break;

        case WM_INPUT:
            {
            if( app->GetRawInputDataPtr )
                {
                RAWINPUT raw;
                UINT size = sizeof( raw );
                app->GetRawInputDataPtr( (HRAWINPUT) lparam, RID_INPUT, &raw, &size, sizeof( RAWINPUTHEADER ) );
                if( raw.header.dwType == RIM_TYPEMOUSE )
                    {
                    if( ( raw.data.mouse.usFlags & 1 ) == MOUSE_MOVE_RELATIVE)
                        {
                        float dx = (float) raw.data.mouse.lLastX;
                        float dy = (float) raw.data.mouse.lLastY;
                        input_event.type = APP_INPUT_MOUSE_DELTA;
                        input_event.data.mouse_delta.x = dx;
                        input_event.data.mouse_delta.y = dy;
                        app_internal_add_input_event( app, &input_event );
                        }
                    }
                }
            break;
            }

        case APP_WT_PACKET:
            {
            APP_PACKET packet;
            memset( &packet, 0, sizeof( packet ) );
            if( (APP_HCTX) lparam == app->tablet.context &&
                app->tablet.WTPacket( app->tablet.context, (UINT) wparam, &packet ) )
                {
                POINT p;
                p.x = packet.pkX;
                p.y = packet.pkY;
                ScreenToClient( app->hwnd, &p );
                int pen_x = p.x;
                int pen_y = p.y;

                input_event.type = APP_INPUT_TABLET;
                input_event.data.tablet.x = pen_x;
                input_event.data.tablet.y = pen_y;
                input_event.data.tablet.pressure = (float) packet.pkNormalPressure / (float) app->tablet.max_pressure;
                input_event.data.tablet.tip = ( packet.pkButtons & 1 ) ? APP_PRESSED : APP_NOT_PRESSED;
                input_event.data.tablet.lower = ( packet.pkButtons & 2 ) ? APP_PRESSED : APP_NOT_PRESSED;
                input_event.data.tablet.upper = ( packet.pkButtons & 4 ) ? APP_PRESSED : APP_NOT_PRESSED;
                app_internal_add_input_event( app, &input_event );
                }
            } break;

        case WM_SETCURSOR:
            if( LOWORD( lparam ) == HTCLIENT )
                {
                SetCursor( app->current_pointer );
                return 0;
                }
            break;


        case WM_WINDOWPOSCHANGED:
            {
            if( app->screenmode == APP_SCREENMODE_FULLSCREEN )
                {
                RECT wr, cr;
                GetWindowRect( app->hwnd, &wr );
                GetClientRect( app->hwnd, &cr );
                if( wr.right - wr.left == cr.right - cr.left && wr.bottom - wr.top == cr.bottom - cr.top )
                    {
                    if( cr.right - cr.left != app->fullscreen_width || cr.bottom - cr.top != app->fullscreen_height )
                        app_screenmode( app, APP_SCREENMODE_WINDOW );
                    }
                }

            if( app->clip_cursor )
                {
                RECT r = app->clip_rect;
                ClientToScreen( app->hwnd, (POINT*)&r );
                ClientToScreen( app->hwnd, ( (POINT*)&r ) + 1 );
                ClipCursor( &r );
                }

            } break;

        case WM_SIZE:
            {
            if( wparam == SIZE_MAXIMIZED )
                {
                WINDOWPLACEMENT placement;
                placement.length = sizeof( placement );
                GetWindowPlacement( app->hwnd, &placement );
                app->windowed_x = placement.rcNormalPosition.left;
                app->windowed_y = placement.rcNormalPosition.top;
                app->windowed_w = placement.rcNormalPosition.right - placement.rcNormalPosition.left;
                app->windowed_h = placement.rcNormalPosition.bottom - placement.rcNormalPosition.top;
                }

            RECT r;
            GetClientRect( app->hwnd, &r );
            app_internal_opengl_resize( &app->gl, r.right - r.left, r.bottom - r.top );
            } break;


        case WM_ACTIVATEAPP:
            app->has_focus = (BOOL) wparam;
            if( app->has_focus )
                {
                app->is_minimized = FALSE;
                if( app->clip_cursor )
                    {
                    RECT r = app->clip_rect;
                    ClientToScreen( app->hwnd, (POINT*)&r );
                    ClientToScreen( app->hwnd, ( (POINT*)&r ) + 1 );
                    ClipCursor( &r );
                    }
                }
            else
                {
                ClipCursor( NULL );
                }

            break;

        case WM_SYSCOMMAND:
            if( ( wparam & 0xFFF0 ) == SC_MINIMIZE ) app->is_minimized = TRUE;
            break;

        case WM_CLOSE:
            app->closed = TRUE;
            return 0;
            break;

        }

    if( app->user_wndproc )
        return app->user_wndproc( app, hwnd, message, wparam, lparam );

    return DefWindowProc( hwnd, message, wparam, lparam);
    }


static BOOL CALLBACK app_internal_monitorenumproc( HMONITOR hmonitor, HDC dc, LPRECT rect, LPARAM data )
    {
    (void) dc;
    app_t* app = (app_t*) data;

    if( app->display_count >= sizeof( app->displays ) / sizeof( *app->displays ) ) return FALSE;
    app->displays_hmonitor[ app->display_count ] = hmonitor;
    app_display_t* display = &app->displays[ app->display_count++ ];

    display->x = rect->left;
    display->y = rect->top;
    display->width = rect->right - rect->left;
    display->height = rect->bottom - rect->top;

    #ifdef __cplusplus
        MONITORINFOEXA mi;
        memset( &mi, 0, sizeof( MONITORINFOEXA ) );
        mi.cbSize = sizeof( MONITORINFOEXA );
        BOOL res = GetMonitorInfoA( hmonitor, &mi );
        if( res && strlen( mi.szDevice ) >= sizeof( display->id ) ) res = FALSE;
        strcpy( display->id, res ? mi.szDevice : "" ) ;
    #else
        MONITORINFOEXA mi;
        memset( &mi, 0, sizeof( MONITORINFOEXA ) );
        BOOL res = GetMonitorInfoA( hmonitor, (LPMONITORINFO)&mi );
        if( res && strlen( mi.szDevice ) >= sizeof( display->id ) ) res = FALSE;
        strcpy( display->id, res ? mi.szDevice : "" ) ;
    #endif

    return TRUE;
    }


static void app_internal_app_default_cursor( app_t* app )
    {
    APP_U32 pointer_pixels[ 256 * 256 ];
    int pointer_width, pointer_height, pointer_hotspot_x, pointer_hotspot_y;
    app_pointer_default( app, &pointer_width, &pointer_height, pointer_pixels, &pointer_hotspot_x, &pointer_hotspot_y );
    app_pointer( app, pointer_width, pointer_height, pointer_pixels, pointer_hotspot_x, pointer_hotspot_y );
    }


#pragma warning( push )
#pragma warning( disable: 4533 ) // initialization of 'wc' is skipped by 'goto init_failed'

int app_t_size( ) { return sizeof( app_t ); }

int app_init( app_t* app, void* logctx, void* fatalctx )
    {
    //SetProcessDPIAware();
    app->logctx = logctx;
    app->fatalctx = fatalctx;
    app->interpolation = APP_INTERPOLATION_LINEAR;
    app->screenmode = APP_SCREENMODE_FULLSCREEN;

    // Log start message
    char msg[ 64 ];
    time_t t = time( NULL );
    struct tm* start = localtime( &t );
    sprintf( msg, "Application started %02d:%02d:%02d %04d-%02d-%02d.",
        start->tm_hour, start->tm_min, start->tm_sec, start->tm_year + 1900, start->tm_mon + 1, start->tm_mday );
    app_log( app, APP_LOG_LEVEL_INFO, msg );

    // Increase timing precision
    #ifndef __TINYC__
    TIMECAPS tc;
    if( timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == TIMERR_NOERROR )
        timeBeginPeriod( tc.wPeriodMin );
    #endif

    // Get instance handle
    app->hinstance = GetModuleHandle( NULL );

    // Retrieve the path of our executable
    GetModuleFileNameA( 0, app->exe_path, sizeof( app->exe_path ) );

    HMODULE shell32 = LoadLibraryA( "shell32.dll" );
    if( shell32 )
        {
        HRESULT (__stdcall *SHGetFolderPathAPtr)(HWND, int, HANDLE, DWORD, LPSTR ) =
            (HRESULT (__stdcall*)(HWND, int, HANDLE, DWORD, LPSTR ) ) (uintptr_t)
                GetProcAddress( shell32, "SHGetFolderPathA" );

        if( SHGetFolderPathAPtr )
            {
            #define APP_CSIDL_PERSONAL 0x0005 // My Documents
            #define APP_CSIDL_COMMON_APPDATA 0x0023 // All Users\Application Data
            #define APP_CSIDL_FLAG_CREATE 0x8000

            // Retrieve user data path
            SHGetFolderPathAPtr( NULL, APP_CSIDL_PERSONAL | APP_CSIDL_FLAG_CREATE, NULL, 0, app->userdata_path );

            // Retrieve app data path
            SHGetFolderPathAPtr( NULL, APP_CSIDL_COMMON_APPDATA | APP_CSIDL_FLAG_CREATE, NULL, 0, app->appdata_path );

            #undef APP_CSIDL_PERSONAL
            #undef APP_CSIDL_COMMON_APPDATA
            #undef APP_CSIDL_FLAG_CREATE
        }

        FreeLibrary( shell32 );
        }

    // Get command line string
    app->cmdline = GetCommandLineA();

    // Load a default Arrow cursor
    app_internal_app_default_cursor( app );

    // Load first icon in the exe and use as app icon
    app->icon = LoadIconA( app->hinstance , MAKEINTRESOURCEA( 1 ) );

    // List all displays
    app->display_count = 0;
    EnumDisplayMonitors( NULL, NULL, app_internal_monitorenumproc, (LPARAM) app );
    if( app->display_count <= 0 ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to get display info" ); goto init_failed;  }

    // Setup the main application window
    app->windowed_w = app->displays[ 0 ].width - app->displays[ 0 ].width / 6;
    app->windowed_h = app->displays[ 0 ].height - app->displays[ 0 ].height / 6;
    app->windowed_x = ( app->displays[ 0 ].width - app->windowed_w ) /  2;
    app->windowed_y = ( app->displays[ 0 ].height - app->windowed_h ) / 2;

    app->fullscreen_width = app->displays[ 0 ].width;
    app->fullscreen_height = app->displays[ 0 ].height;

    RECT winrect = app_internal_rect( app->windowed_x, app->windowed_y,
        app->windowed_x + app->windowed_w, app->windowed_y + app->windowed_h );
    AdjustWindowRect( &winrect, APP_WINDOWED_WS_STYLE | WS_VISIBLE, FALSE );

    WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_DBLCLKS | CS_OWNDC ,
        (WNDPROC) app_internal_wndproc, 0, 0, 0, 0, 0, 0, 0, TEXT( "app_wc" ), 0 };
    wc.hInstance = app->hinstance; wc.hIcon = app->icon; wc.hCursor = app->current_pointer;
    wc.hbrBackground = (HBRUSH) GetStockObject( BLACK_BRUSH ); wc.hIconSm = app->icon;
    RegisterClassEx( &wc );
    app->hwnd = CreateWindowEx( APP_WINDOWED_WS_EX_STYLE, wc.lpszClassName, 0, APP_WINDOWED_WS_STYLE, app->windowed_x, app->windowed_y,
        winrect.right - winrect.left, winrect.bottom - winrect.top, (HWND) 0, (HMENU) 0, app->hinstance, 0 );
    if( !app->hwnd ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to create window." ); goto init_failed; }
    app->hdc = GetDC( app->hwnd );
    app->has_focus = TRUE;
    app->is_minimized = FALSE;

    // Store app pointer with window
    #pragma warning( push )
    #pragma warning( disable: 4244 ) // conversion from 'LONG_PTR' to 'LONG', possible loss of data
    SetWindowLongPtr( app->hwnd, GWLP_USERDATA, (LONG_PTR) app );
    #pragma warning( pop )


    #ifdef APP_ENABLE_MEDIA_KEYS
        int const APP_MOD_NOREPEAT = 0x4000;
        RegisterHotKey( app->hwnd, VK_VOLUME_MUTE, APP_MOD_NOREPEAT, VK_VOLUME_MUTE );
        RegisterHotKey( app->hwnd, VK_VOLUME_DOWN, APP_MOD_NOREPEAT, VK_VOLUME_DOWN );
        RegisterHotKey( app->hwnd, VK_VOLUME_UP, APP_MOD_NOREPEAT, VK_VOLUME_UP );
        RegisterHotKey( app->hwnd, VK_MEDIA_NEXT_TRACK, APP_MOD_NOREPEAT, VK_MEDIA_NEXT_TRACK );
        RegisterHotKey( app->hwnd, VK_MEDIA_PREV_TRACK, APP_MOD_NOREPEAT, VK_MEDIA_PREV_TRACK );
        RegisterHotKey( app->hwnd, VK_MEDIA_STOP, APP_MOD_NOREPEAT, VK_MEDIA_STOP );
        RegisterHotKey( app->hwnd, VK_MEDIA_PLAY_PAUSE, APP_MOD_NOREPEAT, VK_MEDIA_PLAY_PAUSE );
    #endif

    ShowWindow( app->hwnd, SW_HIDE );
    // Windows specific OpenGL initialization
    app->gl_dll = LoadLibraryA( "opengl32.dll" );
    if( !app->gl_dll ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to load opengl32.dll" ); goto init_failed; }
    app->wglGetProcAddress = (PROC(APP_GLCALLTYPE*)(LPCSTR)) (uintptr_t) GetProcAddress( app->gl_dll, "wglGetProcAddress" );
    if( !app->wglGetProcAddress ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to find wglGetProcAddress" ); goto init_failed; }
    app->wglCreateContext = (HGLRC(APP_GLCALLTYPE*)(HDC)) (uintptr_t) GetProcAddress( app->gl_dll, "wglCreateContext" );
    if( !app->wglCreateContext ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to find wglCreateContext" ); goto init_failed; }
    app->wglDeleteContext = (BOOL(APP_GLCALLTYPE*)(HGLRC)) (uintptr_t) GetProcAddress( app->gl_dll, "wglDeleteContext" );
    if( !app->wglDeleteContext ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to find wglDeleteContext" ); goto init_failed; }
    app->wglMakeCurrent = (BOOL(APP_GLCALLTYPE*)(HDC, HGLRC)) (uintptr_t) GetProcAddress( app->gl_dll, "wglMakeCurrent" );
    if( !app->wglMakeCurrent ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to find wglMakeCurrent" ); goto init_failed; }

    PIXELFORMATDESCRIPTOR pfd;
    memset( &pfd, 0, sizeof( pfd ) );
    pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    BOOL res = SetPixelFormat( app->hdc, ChoosePixelFormat( app->hdc, &pfd ), &pfd );
    if( !res ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to set pixel format" ); goto init_failed; }

    app->gl_context = app->wglCreateContext( app->hdc );
    if( !app->gl_context ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to create OpenGL context" ); goto init_failed; }
    res = app->wglMakeCurrent( app->hdc, app->gl_context );
    if( !res ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to activate OpenGl Context" ); goto init_failed; }

    app->wglSwapIntervalEXT = (BOOL (APP_GLCALLTYPE*)(int)) (uintptr_t) app->wglGetProcAddress( "wglSwapIntervalEXT" );
    if( app->wglSwapIntervalEXT ) app->wglSwapIntervalEXT( 1 );

    // Attempt to bind opengl functions using GetProcAddress
    app->gl.CreateShader = ( APP_GLuint (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) GetProcAddress( app->gl_dll, "glCreateShader" );
    app->gl.ShaderSource = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLsizei, APP_GLchar const* const*, APP_GLint const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glShaderSource" );
    app->gl.CompileShader = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glCompileShader" );
    app->gl.GetShaderiv = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLenum, APP_GLint*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGetShaderiv" );
    app->gl.CreateProgram = ( APP_GLuint (APP_GLCALLTYPE*) (void) ) (uintptr_t) GetProcAddress( app->gl_dll, "glCreateProgram" );
    app->gl.AttachShader = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glAttachShader" );
    app->gl.BindAttribLocation = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLuint, APP_GLchar const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glBindAttribLocation" );
    app->gl.LinkProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glLinkProgram" );
    app->gl.GetProgramiv = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLenum, APP_GLint*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGetProgramiv" );
    app->gl.GenBuffers = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGenBuffers" );
    app->gl.BindBuffer = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glBindBuffer" );
    app->gl.EnableVertexAttribArray = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glEnableVertexAttribArray" );
    app->gl.VertexAttribPointer = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLint, APP_GLenum, APP_GLboolean, APP_GLsizei, void const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glVertexAttribPointer" );
    app->gl.GenTextures = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGenTextures" );
    app->gl.Enable = ( void (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) GetProcAddress( app->gl_dll, "glEnable" );
    app->gl.ActiveTexture = ( void (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) GetProcAddress( app->gl_dll, "glActiveTexture" );
    app->gl.BindTexture = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glBindTexture" );
    app->gl.TexParameteri = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLenum, APP_GLint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glTexParameteri" );
    app->gl.DeleteBuffers = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDeleteBuffers" );
    app->gl.DeleteTextures = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDeleteTextures" );
    app->gl.BufferData = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLsizeiptr, void const *, APP_GLenum) ) (uintptr_t) GetProcAddress( app->gl_dll, "glBufferData" );
    app->gl.UseProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glUseProgram" );
    app->gl.Uniform1i = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glUniform1i" );
    app->gl.Uniform3f = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLfloat, APP_GLfloat, APP_GLfloat) ) (uintptr_t) GetProcAddress( app->gl_dll, "glUniform3f" );
    app->gl.GetUniformLocation = ( APP_GLint (APP_GLCALLTYPE*) (APP_GLuint, APP_GLchar const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGetUniformLocation" );
    app->gl.TexImage2D = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLint, APP_GLint, APP_GLsizei, APP_GLsizei, APP_GLint, APP_GLenum, APP_GLenum, void const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glTexImage2D" );
    app->gl.ClearColor = ( void (APP_GLCALLTYPE*) (APP_GLfloat, APP_GLfloat, APP_GLfloat, APP_GLfloat) ) (uintptr_t) GetProcAddress( app->gl_dll, "glClearColor" );
    app->gl.Clear = ( void (APP_GLCALLTYPE*) (APP_GLbitfield) ) (uintptr_t) GetProcAddress( app->gl_dll, "glClear" );
    app->gl.DrawArrays = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLint, APP_GLsizei) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDrawArrays" );
    app->gl.Viewport = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLint, APP_GLsizei, APP_GLsizei) ) (uintptr_t) GetProcAddress( app->gl_dll, "glViewport" );
    app->gl.DeleteShader = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDeleteShader" );
    app->gl.DeleteProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDeleteProgram" );
    #ifdef APP_REPORT_SHADER_ERRORS
        app->gl.GetShaderInfoLog = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLsizei, APP_GLsizei*, APP_GLchar*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGetShaderInfoLog" );
    #endif

    // Any opengl functions which didn't bind, try binding them using wglGetProcAddrss
    if( !app->gl.CreateShader ) app->gl.CreateShader = ( APP_GLuint (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) app->wglGetProcAddress( "glCreateShader" );
    if( !app->gl.ShaderSource ) app->gl.ShaderSource = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLsizei, APP_GLchar const* const*, APP_GLint const*) ) (uintptr_t) app->wglGetProcAddress( "glShaderSource" );
    if( !app->gl.CompileShader ) app->gl.CompileShader = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glCompileShader" );
    if( !app->gl.GetShaderiv ) app->gl.GetShaderiv = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLenum, APP_GLint*) ) (uintptr_t) app->wglGetProcAddress( "glGetShaderiv" );
    if( !app->gl.CreateProgram ) app->gl.CreateProgram = ( APP_GLuint (APP_GLCALLTYPE*) (void) ) (uintptr_t) app->wglGetProcAddress( "glCreateProgram" );
    if( !app->gl.AttachShader ) app->gl.AttachShader = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glAttachShader" );
    if( !app->gl.BindAttribLocation ) app->gl.BindAttribLocation = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLuint, APP_GLchar const*) ) (uintptr_t) app->wglGetProcAddress( "glBindAttribLocation" );
    if( !app->gl.LinkProgram ) app->gl.LinkProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glLinkProgram" );
    if( !app->gl.GetProgramiv ) app->gl.GetProgramiv = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLenum, APP_GLint*) ) (uintptr_t) app->wglGetProcAddress( "glGetProgramiv" );
    if( !app->gl.GenBuffers ) app->gl.GenBuffers = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint*) ) (uintptr_t) app->wglGetProcAddress( "glGenBuffers" );
    if( !app->gl.BindBuffer ) app->gl.BindBuffer = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glBindBuffer" );
    if( !app->gl.EnableVertexAttribArray ) app->gl.EnableVertexAttribArray = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glEnableVertexAttribArray" );
    if( !app->gl.VertexAttribPointer ) app->gl.VertexAttribPointer = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLint, APP_GLenum, APP_GLboolean, APP_GLsizei, void const*) ) (uintptr_t) app->wglGetProcAddress( "glVertexAttribPointer" );
    if( !app->gl.GenTextures ) app->gl.GenTextures = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint*) ) (uintptr_t) app->wglGetProcAddress( "glGenTextures" );
    if( !app->gl.Enable ) app->gl.Enable = ( void (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) app->wglGetProcAddress( "glEnable" );
    if( !app->gl.ActiveTexture ) app->gl.ActiveTexture = ( void (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) app->wglGetProcAddress( "glActiveTexture" );
    if( !app->gl.BindTexture ) app->gl.BindTexture = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glBindTexture" );
    if( !app->gl.TexParameteri ) app->gl.TexParameteri = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLenum, APP_GLint) ) (uintptr_t) app->wglGetProcAddress( "glTexParameteri" );
    if( !app->gl.DeleteBuffers ) app->gl.DeleteBuffers = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint const*) ) (uintptr_t) app->wglGetProcAddress( "glDeleteBuffers" );
    if( !app->gl.DeleteTextures ) app->gl.DeleteTextures = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint const*) ) (uintptr_t) app->wglGetProcAddress( "glDeleteTextures" );
    if( !app->gl.BufferData ) app->gl.BufferData = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLsizeiptr, void const *, APP_GLenum) ) (uintptr_t) app->wglGetProcAddress( "glBufferData" );
    if( !app->gl.UseProgram ) app->gl.UseProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glUseProgram" );
    if( !app->gl.Uniform1i ) app->gl.Uniform1i = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLint) ) (uintptr_t) app->wglGetProcAddress( "glUniform1i" );
    if( !app->gl.Uniform3f ) app->gl.Uniform3f = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLfloat, APP_GLfloat, APP_GLfloat) ) (uintptr_t) app->wglGetProcAddress( "glUniform3f" );
    if( !app->gl.GetUniformLocation ) app->gl.GetUniformLocation = ( APP_GLint (APP_GLCALLTYPE*) (APP_GLuint, APP_GLchar const*) ) (uintptr_t) app->wglGetProcAddress( "glGetUniformLocation" );
    if( !app->gl.TexImage2D ) app->gl.TexImage2D = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLint, APP_GLint, APP_GLsizei, APP_GLsizei, APP_GLint, APP_GLenum, APP_GLenum, void const*) ) (uintptr_t) app->wglGetProcAddress( "glTexImage2D" );
    if( !app->gl.ClearColor ) app->gl.ClearColor = ( void (APP_GLCALLTYPE*) (APP_GLfloat, APP_GLfloat, APP_GLfloat, APP_GLfloat) ) (uintptr_t) app->wglGetProcAddress( "glClearColor" );
    if( !app->gl.Clear ) app->gl.Clear = ( void (APP_GLCALLTYPE*) (APP_GLbitfield) ) (uintptr_t) app->wglGetProcAddress( "glClear" );
    if( !app->gl.DrawArrays ) app->gl.DrawArrays = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLint, APP_GLsizei) ) (uintptr_t) app->wglGetProcAddress( "glDrawArrays" );
    if( !app->gl.Viewport ) app->gl.Viewport = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLint, APP_GLsizei, APP_GLsizei) ) (uintptr_t) app->wglGetProcAddress( "glViewport" );
    if( !app->gl.DeleteShader ) app->gl.DeleteShader = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glDeleteShader" );
    if( !app->gl.DeleteProgram ) app->gl.DeleteProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glDeleteProgram" );
    #ifdef APP_REPORT_SHADER_ERRORS
        if( !app->gl.GetShaderInfoLog ) app->gl.GetShaderInfoLog = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLsizei, APP_GLsizei*, APP_GLchar*) ) (uintptr_t) app->wglGetProcAddress( "glGetShaderInfoLog" );
    #endif

    // Report error if any gl function was not found.
    if( !app->gl.CreateShader ) { app_fatal_error( app, "Could not find function CreateShader." ); goto init_failed; }
    if( !app->gl.ShaderSource ) { app_fatal_error( app, "Could not find function ShaderSource." ); goto init_failed; }
    if( !app->gl.CompileShader ) { app_fatal_error( app, "Could not find function CompileShader." ); goto init_failed; }
    if( !app->gl.GetShaderiv ) { app_fatal_error( app, "Could not find function GetShaderiv." ); goto init_failed; }
    if( !app->gl.CreateProgram ) { app_fatal_error( app, "Could not find function CreateProgram." ); goto init_failed; }
    if( !app->gl.AttachShader ) { app_fatal_error( app, "Could not find function AttachShader." ); goto init_failed; }
    if( !app->gl.BindAttribLocation ) { app_fatal_error( app, "Could not find function BindAttribLocation." ); goto init_failed; }
    if( !app->gl.LinkProgram ) { app_fatal_error( app, "Could not find function LinkProgram." ); goto init_failed; }
    if( !app->gl.GetProgramiv ) { app_fatal_error( app, "Could not find function GetProgramiv." ); goto init_failed; }
    if( !app->gl.GenBuffers ) { app_fatal_error( app, "Could not find function GenBuffers." ); goto init_failed; }
    if( !app->gl.BindBuffer ) { app_fatal_error( app, "Could not find function BindBuffer." ); goto init_failed; }
    if( !app->gl.EnableVertexAttribArray ) { app_fatal_error( app, "Could not find function EnableVertexAttribArray." ); goto init_failed; }
    if( !app->gl.VertexAttribPointer ) { app_fatal_error( app, "Could not find function VertexAttribPointer." ); goto init_failed; }
    if( !app->gl.GenTextures ) { app_fatal_error( app, "Could not find function GenTextures." ); goto init_failed; }
    if( !app->gl.Enable ) { app_fatal_error( app, "Could not find function Enable." ); goto init_failed; }
    if( !app->gl.ActiveTexture ) { app_fatal_error( app, "Could not find function ActiveTexture." ); goto init_failed; }
    if( !app->gl.BindTexture ) { app_fatal_error( app, "Could not find function BindTexture." ); goto init_failed; }
    if( !app->gl.TexParameteri ) { app_fatal_error( app, "Could not find function TexParameteri." ); goto init_failed; }
    if( !app->gl.DeleteBuffers ) { app_fatal_error( app, "Could not find function DeleteBuffers." ); goto init_failed; }
    if( !app->gl.DeleteTextures ) { app_fatal_error( app, "Could not find function DeleteTextures." ); goto init_failed; }
    if( !app->gl.BufferData ) { app_fatal_error( app, "Could not find function BufferData." ); goto init_failed; }
    if( !app->gl.UseProgram ) { app_fatal_error( app, "Could not find function UseProgram." ); goto init_failed; }
    if( !app->gl.Uniform1i ) { app_fatal_error( app, "Could not find function Uniform1i." ); goto init_failed; }
    if( !app->gl.Uniform3f ) { app_fatal_error( app, "Could not find function Uniform3f." ); goto init_failed; }
    if( !app->gl.GetUniformLocation ) { app_fatal_error( app, "Could not find function GetUniformLocation." ); goto init_failed; }
    if( !app->gl.TexImage2D ) { app_fatal_error( app, "Could not find function TexImage2D." ); goto init_failed; }
    if( !app->gl.ClearColor ) { app_fatal_error( app, "Could not find function ClearColor." ); goto init_failed; }
    if( !app->gl.Clear ) { app_fatal_error( app, "Could not find function Clear." ); goto init_failed; }
    if( !app->gl.DrawArrays ) { app_fatal_error( app, "Could not find function DrawArrays." ); goto init_failed; }
    if( !app->gl.Viewport ) { app_fatal_error( app, "Could not find function Viewport." ); goto init_failed; }
    if( !app->gl.DeleteShader ) { app_fatal_error( app, "Could not find function DeleteShader." ); goto init_failed; }
    if( !app->gl.DeleteProgram ) { app_fatal_error( app, "Could not find function DeleteProgram." ); goto init_failed; }
    #ifdef APP_REPORT_SHADER_ERRORS
        if( !app->gl.GetShaderInfoLog ) { app_fatal_error( app, "Could not find function GetShaderInfoLog." ); goto init_failed; }
    #endif

    // Platform independent OpenGL initialization
    int width = app->screenmode == APP_SCREENMODE_FULLSCREEN ? app->fullscreen_width : app->windowed_w;
    int height = app->screenmode == APP_SCREENMODE_FULLSCREEN ? app->fullscreen_height: app->windowed_h;
    if( !app_internal_opengl_init( app, &app->gl, app->interpolation, width, height ) )
        {
        app_log( app, APP_LOG_LEVEL_ERROR, "Failed to initialize OpenGL" );
        goto init_failed;
        }

    app->sound_notifications[ 0 ] = CreateEventA( NULL, FALSE, FALSE, NULL );
    app->sound_notifications[ 1 ] = CreateEventA( NULL, FALSE, FALSE, NULL );


    app->dsound_dll = LoadLibraryA( "dsound.dll" );
    if( !app->dsound_dll ) app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't load dsound.dll. Sound disabled." );

    if( app->dsound_dll )
        {
        HRESULT (WINAPI *DirectSoundCreate8Ptr)(LPCGUID,struct IDirectSound8**,void*) = ( HRESULT (WINAPI*)(LPCGUID,struct IDirectSound8**,void*) )
            (uintptr_t) GetProcAddress( (HMODULE) app->dsound_dll, "DirectSoundCreate8" );
        if( !DirectSoundCreate8Ptr )
            {
            app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't find DirectSoundCreate. Sound disabled." );
            FreeLibrary( app->dsound_dll );
            app->dsound_dll = 0;
            }
        if( DirectSoundCreate8Ptr )
            {
            HRESULT hr = DirectSoundCreate8Ptr( NULL, &app->dsound, NULL );
            if( FAILED( hr ) || !app->dsound )
                {
                app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't create DirectSound object. Sound disabled." );
                DirectSoundCreate8Ptr = 0;
                FreeLibrary( app->dsound_dll );
                app->dsound_dll = 0;
                }
            else
                {
                hr = IDirectSound8_SetCooperativeLevel( app->dsound, app->hwnd, DSSCL_NORMAL);
                if( FAILED( hr ) )
                    {
                    app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't set cooperative level for DirectSound object. Sound disabled." );
                    IDirectSound8_Release( app->dsound );
                    app->dsound = 0;
                    DirectSoundCreate8Ptr = 0;
                    FreeLibrary( app->dsound_dll );
                    app->dsound_dll = 0;
                    }
                }
            }
        }
    app->sound_thread_handle = INVALID_HANDLE_VALUE;

    HMODULE user32 = LoadLibraryA( "user32.dll" );
    if( user32 )
        {
        BOOL (WINAPI *RegisterRawInputDevicesPtr)( PCRAWINPUTDEVICE, UINT, UINT ) =
            (BOOL (WINAPI*)( PCRAWINPUTDEVICE, UINT, UINT ) )(uintptr_t) GetProcAddress( user32, "RegisterRawInputDevices" );

        app->GetRawInputDataPtr = (UINT (WINAPI*)( HRAWINPUT, UINT, LPVOID, PUINT, UINT))
            (uintptr_t) GetProcAddress( user32, "GetRawInputData" );

        USHORT const USAGE_PAGE_GENERIC = ((USHORT) 0x01);
        USHORT const USAGE_GENERIC_MOUSE = ((USHORT) 0x02);

        RAWINPUTDEVICE rid[ 1 ];
        rid[ 0 ].usUsagePage = USAGE_PAGE_GENERIC;
        rid[ 0 ].usUsage = USAGE_GENERIC_MOUSE;
        rid[ 0 ].dwFlags = RIDEV_INPUTSINK;
        rid[ 0 ].hwndTarget = app->hwnd;
        RegisterRawInputDevicesPtr( rid, 1, sizeof( *rid ) );

        FreeLibrary( user32 );
        }

    if( !app_internal_tablet_init( app ) ) app_log( app, APP_LOG_LEVEL_WARNING, "WinTab initialization failed - tablet not available" );

    return EXIT_SUCCESS;

init_failed:
    app_term( app );
    return EXIT_FAILURE;
    }

void app_term( app_t* app )
    {
    if ( !app ) { return; }

    if( !app_internal_tablet_term( app ) ) app_log( app, APP_LOG_LEVEL_WARNING, "WinTab termination failed" );
    if( app->sound_thread_handle != INVALID_HANDLE_VALUE )
        {
        InterlockedExchange( &app->exit_sound_thread, 1 );
        WaitForSingleObject( app->sound_thread_handle, INFINITE );
        CloseHandle( app->sound_thread_handle );
        }
    if( app->dsoundbuf ) IDirectSoundBuffer8_Release( app->dsoundbuf );
    if( app->dsound ) IDirectSound8_Release( app->dsound );
    if( app->dsound_dll ) FreeLibrary( app->dsound_dll );
    if( app->sound_notifications[ 0 ] ) CloseHandle( app->sound_notifications[ 0 ] );
    if( app->sound_notifications[ 1 ] ) CloseHandle( app->sound_notifications[ 1 ] );
    if( !app_internal_opengl_term( &app->gl ) ) app_log( app, APP_LOG_LEVEL_WARNING, "Failed to terminate OpenGL" );
    if( app->gl_context ) app->wglMakeCurrent( 0, 0 );
    if( app->gl_context ) app->wglDeleteContext( app->gl_context );
    if( app->gl_dll ) FreeLibrary( app->gl_dll );
    if( app->icon ) DestroyIcon( app->icon );
    if( app->current_pointer ) DestroyIcon( app->current_pointer );
    if( app->hdc ) ReleaseDC( app->hwnd, app->hdc );
    if( app->hwnd ) DestroyWindow( app->hwnd );
    UnregisterClass( TEXT( "app_wc" ), app->hinstance );

    #ifndef __TINYC__
    if( timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == TIMERR_NOERROR )
        timeEndPeriod( tc.wPeriodMin );
    #endif

    t = time( NULL );
    struct tm* end = localtime( &t );
    sprintf( msg, "Application terminated %02d:%02d:%02d %04d-%02d-%02d.",
        end->tm_hour, end->tm_min, end->tm_sec, end->tm_year + 1900, end->tm_mon + 1, end->tm_mday );
    app_log( app, APP_LOG_LEVEL_INFO, msg );
    }

#pragma warning( pop )


app_state_t app_yield( app_t* app )
    {
    if( !app->initialized )
        {
        if( app->screenmode == APP_SCREENMODE_WINDOW )
        {
            app->screenmode = APP_SCREENMODE_FULLSCREEN;
            app_screenmode( app, APP_SCREENMODE_WINDOW );
        }
        else
        {
            app->screenmode = APP_SCREENMODE_WINDOW;
            app_screenmode( app, APP_SCREENMODE_FULLSCREEN );
        }
        STARTUPINFOA startup_info = { sizeof( STARTUPINFOA ) };
        GetStartupInfoA( &startup_info );
        if( startup_info.dwFlags & STARTF_USESHOWWINDOW ) {
            ShowWindow( app->hwnd, startup_info.wShowWindow );
        } else {
            ShowWindow( app->hwnd, SW_SHOWDEFAULT );
        }
        SetActiveWindow( app->hwnd );
        BringWindowToTop( app->hwnd );
        SwitchToThisWindow( app->hwnd, TRUE );
        if( app->tablet.context ) app->tablet.WTEnable( app->tablet.context, TRUE );
        app->initialized = TRUE;
        }

    MSG msg;
    while( PeekMessage( &msg, app->hwnd, 0,0, PM_REMOVE ) )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }

        SwitchToThread();   //  yield to any thread on same processor

    return app->closed == TRUE ? APP_STATE_EXIT_REQUESTED : APP_STATE_NORMAL;
    }


void app_cancel_exit( app_t* app )
    {
    app->closed = FALSE;
    }


void app_title( app_t* app, char const* title )
    {
    #ifdef UNICODE
        int len = (int) strlen (title );
        if( len < 256 )
            {
            WCHAR unistring[ 256 ];
            MultiByteToWideChar( CP_ACP, 0, title, -1, unistring, len + 1 );
            SetWindowText( app->hwnd, unistring );
            }
    #else
        SetWindowText( app->hwnd, title );
    #endif
    }


char const* app_cmdline( app_t* app )
    {
    return app->cmdline;
    }


char const* app_filename( app_t* app )
    {
    return app->exe_path;
    }


char const* app_userdata( app_t* app )
    {
    return app->userdata_path;
    }


char const* app_appdata( app_t* app )
    {
    return app->appdata_path;
    }


APP_U64 app_time_count( app_t* app )
    {
    (void) app;
    LARGE_INTEGER c;
    QueryPerformanceCounter( &c );
    return (APP_U64) c.QuadPart;
    }


APP_U64 app_time_freq( app_t* app )
    {
    (void) app;
    LARGE_INTEGER f;
    QueryPerformanceFrequency( &f );
    return (APP_U64) f.QuadPart;
    }


void app_log( app_t* app, app_log_level_t level, char const* message )
    {
    (void) app, (void) level, (void) message;
    APP_LOG( app->logctx, level, message );
    }


void app_fatal_error( app_t* app, char const* message )
    {
    (void) app, (void) message;
    APP_FATAL_ERROR( app->fatalctx, message );
    }


static HCURSOR app_internal_create_cursor( HWND hwnd, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y )
    {
    int size = width > height ? width : height;
    BITMAPV5HEADER header;
    memset( &header, 0, sizeof( BITMAPV5HEADER ) );
    header.bV5Size = sizeof( BITMAPV5HEADER );
    header.bV5Width = (LONG) size;
    header.bV5Height = -(LONG) size;
    header.bV5Planes = 1;
    header.bV5BitCount = 32;
    header.bV5Compression = BI_BITFIELDS;
    header.bV5RedMask   =  0x00FF0000;
    header.bV5GreenMask =  0x0000FF00;
    header.bV5BlueMask  =  0x000000FF;
    header.bV5AlphaMask =  0xFF000000;

    HDC hdc = GetDC( hwnd );
    void* bits = NULL;
    HBITMAP bitmap = CreateDIBSection( hdc, (BITMAPINFO*)&header, DIB_RGB_COLORS,  (void**) &bits, NULL, (DWORD) 0);
    ReleaseDC( NULL, hdc );

    APP_U32* ptr = (APP_U32*) bits;
    for( int y = 0; y < height; ++y )
        {
        for( int x = 0; x < width; ++x )
        {
            APP_U32 c = pixels_abgr[ x + y * width ];
            APP_U32 a = ( c & 0xff000000 ) >> 24;
            APP_U32 b = ( c & 0x00ff0000 ) >> 16;
            APP_U32 g = ( c & 0x0000ff00 ) >> 8;
            APP_U32 r = ( c & 0x000000ff );
            ptr[ x + y * size ] = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;
            }
        }

    HBITMAP empty_mask = CreateBitmap( size, size, 1, 1, NULL );
    ICONINFO icon_info;
    icon_info.fIcon = FALSE;
    icon_info.xHotspot = (DWORD) hotspot_x;
    icon_info.yHotspot = (DWORD) hotspot_y;
    icon_info.hbmMask = empty_mask;
    icon_info.hbmColor = bitmap;

    HCURSOR cursor = CreateIconIndirect( &icon_info );
    DeleteObject( bitmap );
    DeleteObject( empty_mask );

    return cursor;
    }


void app_pointer( app_t* app, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y )
    {
    if( app->current_pointer ) DestroyIcon( app->current_pointer );
    app->current_pointer = 0;

    if( pixels_abgr )
        app->current_pointer = app_internal_create_cursor( app->hwnd, width, height,
            pixels_abgr, hotspot_x, hotspot_y );
    ShowCursor( FALSE );
    SetCursor( app->current_pointer );
    ShowCursor( TRUE );
    }


static BOOL app_internal_extract_default_windows_cursor( int* width, int* height, APP_U32* pixels_abgr,
    int* hotspot_x, int* hotspot_y )
    {
    HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
    if( !cursor ) return FALSE;

    ICONINFO info;
    if( !GetIconInfo( cursor, &info ) ) { DestroyCursor( cursor ); return FALSE; }
    BOOL bw_cursor = ( info.hbmColor == NULL );

    BITMAP bmpinfo;
    memset( &bmpinfo, 0, sizeof( bmpinfo ) );
    if( bw_cursor && GetObject( info.hbmMask, sizeof( BITMAP ), &bmpinfo ) == 0 )
        {
        DestroyCursor( cursor );
        DeleteObject( info.hbmColor );
        DeleteObject( info.hbmMask );
        return FALSE;
        }
    if( !bw_cursor && GetObject( info.hbmColor, sizeof( BITMAP ), &bmpinfo ) == 0 )
        {
        DestroyCursor( cursor );
        DeleteObject( info.hbmColor );
        DeleteObject( info.hbmMask );
        return FALSE;
        }

    if( bmpinfo.bmWidth > 256 || bmpinfo.bmHeight > 256 )
        {
        DestroyCursor( cursor );
        DeleteObject( info.hbmColor );
        DeleteObject( info.hbmMask );
        return FALSE;
        }
    int pointer_width = bmpinfo.bmWidth;
    int pointer_height = ( bmpinfo.bmHeight >= 0 ? bmpinfo.bmHeight : -bmpinfo.bmHeight ) / ( bw_cursor ? 2 : 1 );

    if( width ) *width = pointer_width;
    if( height ) *height = pointer_height;
    if( hotspot_x ) *hotspot_x = (int) info.xHotspot;
    if( hotspot_y ) *hotspot_y = (int) info.yHotspot;
    if( !pixels_abgr )
        {
        DestroyCursor( cursor );
        DeleteObject( info.hbmColor );
        DeleteObject( info.hbmMask );
        return TRUE;
        }

    BITMAPINFOHEADER bmi;
    bmi.biSize = sizeof( BITMAPINFOHEADER );
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biWidth = bmpinfo.bmWidth;
    bmi.biHeight = -bmpinfo.bmHeight;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;
    HDC hdc = GetDC( NULL );
    if( GetDIBits( hdc, bw_cursor ? info.hbmMask : info.hbmColor, 0, (UINT) bmpinfo.bmHeight, pixels_abgr,
        (BITMAPINFO*) &bmi, DIB_RGB_COLORS ) != bmpinfo.bmHeight )
        {
        DestroyCursor( cursor );
        DeleteObject( info.hbmColor );
        DeleteObject( info.hbmMask );
        ReleaseDC( NULL, hdc );
        return FALSE;
        }
    ReleaseDC( NULL, hdc );

    if( bw_cursor )
        {
        for( int y = 0; y < pointer_height; ++y )
            {
            for( int x = 0; x < pointer_width; ++x )
                {
                APP_U32 c = pixels_abgr[ x + pointer_width * y ];
                APP_U32 m = pixels_abgr[ x + pointer_width * ( pointer_height + y ) ];
                APP_U32 a = 255 - ( c & 0xff );
                APP_U32 g = m & 0xff;
                pixels_abgr[ x + pointer_width * y ] = ( a << 24 ) | ( g << 16 ) | ( g << 8 ) | g;
                }
            }
        }
    else
        {
        for( int y = 0; y < pointer_height; ++y )
            {
            for( int x = 0; x < pointer_width; ++x )
                {
                APP_U32 c = pixels_abgr[ x + pointer_width * y ];
                APP_U32 a = ( c >> 24 ) & 0xff;
                APP_U32 r = ( c >> 16 ) & 0xff;
                APP_U32 g = ( c >>  8 ) & 0xff;
                APP_U32 b = ( c       ) & 0xff;
                pixels_abgr[ x + pointer_width * y ] = ( a << 24 ) | ( b << 16 ) | ( g << 8 ) | r;
                }
            }
        }

    DeleteObject( info.hbmColor );
    DeleteObject( info.hbmMask );
    DestroyCursor( cursor );
    return TRUE;
    }


void app_pointer_default( app_t* app, int* width, int* height, APP_U32* pixels_abgr, int* hotspot_x, int* hotspot_y )
    {
    (void) app;

    APP_U32 default_pointer_data[ 11 * 16 ] =
        {
        0xFF000000,0xFF000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFF000000,
        0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFFFFFFFF,0xFF000000,0x00000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0xFF000000,0x00000000,0x00000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,0x00000000,
        0xFF000000,0x00000000,0x00000000,0x00000000,0x00000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,
        0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFF000000,0x00000000,0x00000000,
        0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0xFF000000,0xFF000000,0xFF000000,0x00000000,0x00000000,
        };

    if( !app_internal_extract_default_windows_cursor( width, height, pixels_abgr, hotspot_x, hotspot_y ) )
        {
        if( width ) *width = 11;
        if( height ) *height = 16;
        if( hotspot_x ) *hotspot_x = 0;
        if( hotspot_y ) *hotspot_y = 0;
        if( pixels_abgr ) memcpy( pixels_abgr, default_pointer_data, sizeof( APP_U32 ) * 11 * 16 );
        }
    }



void app_pointer_pos( app_t* app, int x, int y )
    {
    POINT p;
    p.x = x;
    p.y = y;
    ClientToScreen( app->hwnd, &p );
    SetCursorPos( p.x, p.y );
    }


int app_pointer_x( app_t* app )
    {
    POINT p;
    GetCursorPos( &p );
    ScreenToClient( app->hwnd, &p );
    return (int) p.x;
    }


int app_pointer_y( app_t* app )
    {
    POINT p;
    GetCursorPos( &p );
    ScreenToClient( app->hwnd, &p );
    return (int) p.y;
    }


void app_pointer_limit( app_t* app, int x, int y, int width, int height )
    {
    app->clip_cursor = TRUE;
    app->clip_rect.left= x;
    app->clip_rect.top = y;
    app->clip_rect.right = x + width;
    app->clip_rect.bottom = y + height;

    RECT r = app->clip_rect;
    ClientToScreen( app->hwnd, (POINT*)&r );
    ClientToScreen( app->hwnd, ( (POINT*)&r ) + 1 );
    ClipCursor( &r );
    }


void app_pointer_limit_off( app_t* app )
    {
    app->clip_cursor = FALSE;
    ClipCursor( 0 );
    }


void app_interpolation( app_t* app, app_interpolation_t interpolation )
    {
    if( interpolation == app->interpolation ) return;
    app->interpolation = interpolation;

    POINT p;
    GetCursorPos( &p );
    ScreenToClient( app->hwnd, &p );
    int mouse_x = p.x;
    int mouse_y = p.y;

    app_input_event_t input_event;
    input_event.type = APP_INPUT_MOUSE_MOVE;
    input_event.data.mouse_pos.x = mouse_x;
    input_event.data.mouse_pos.y = mouse_y;
    app_internal_add_input_event( app, &input_event );

    app_internal_opengl_interpolation( &app->gl, interpolation );
    }


void app_screenmode( app_t* app, app_screenmode_t screenmode )
    {
    if( screenmode == app->screenmode ) return;
    app->screenmode = screenmode;
    BOOL visible = IsWindowVisible( app->hwnd );
    if( screenmode == APP_SCREENMODE_WINDOW )
        {
        SetWindowLong( app->hwnd, GWL_STYLE, APP_WINDOWED_WS_STYLE | ( visible ? WS_VISIBLE : 0 ) );

        WINDOWPLACEMENT placement;
        placement.length = sizeof( placement );
        GetWindowPlacement( app->hwnd, &placement );
        placement.showCmd = (UINT)( visible ? SW_SHOW : SW_HIDE );

        placement.rcNormalPosition.left = app->windowed_x;
        placement.rcNormalPosition.top = app->windowed_y;
        placement.rcNormalPosition.right = app->windowed_x + app->windowed_w;
        placement.rcNormalPosition.bottom = app->windowed_y + app->windowed_h;
        SetWindowPlacement( app->hwnd, &placement );
        }
    else
        {
        WINDOWPLACEMENT placement;
        placement.length = sizeof( placement );
        GetWindowPlacement( app->hwnd, &placement );

        if( visible )
            {
        if( placement.showCmd != SW_SHOWMAXIMIZED )
            {
            app->windowed_x = placement.rcNormalPosition.left;
            app->windowed_y = placement.rcNormalPosition.top;
            app->windowed_w = placement.rcNormalPosition.right - placement.rcNormalPosition.left;
            app->windowed_h = placement.rcNormalPosition.bottom - placement.rcNormalPosition.top;
            }
        else
            {
            ShowWindow( app->hwnd, SW_RESTORE );
            }
            }

        HMONITOR hmonitor = MonitorFromWindow( app->hwnd, MONITOR_DEFAULTTOPRIMARY );
        int display_index = 0;
        for( int i = 0; i < app->display_count; ++i )
            {
            if( app->displays_hmonitor[ i ] == hmonitor )
                {
                display_index = i;
                break;
                }
            }


        RECT r = app_internal_rect( app->displays[ display_index ].x, app->displays[ display_index ].y,
            app->displays[ display_index ].x + app->displays[ display_index ].width,
            app->displays[ display_index ].y + app->displays[ display_index ].height );
        app->fullscreen_width = r.right - r.left;
        app->fullscreen_height = r.bottom - r.top;
        SetWindowPos( app->hwnd, 0, r.left, r.top, app->fullscreen_width, app->fullscreen_height,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED );

        SetWindowLong( app->hwnd, GWL_STYLE, ( visible ? WS_VISIBLE : 0 ) );
        SetWindowPos( app->hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
        }
    }


void app_window_size( app_t* app, int width, int height )
    {
    RECT r;
    r = app_internal_rect( 0, 0, width, height );
    AdjustWindowRect( &r, APP_WINDOWED_WS_STYLE | WS_VISIBLE, FALSE );

    width = r.right - r.left;
    height = r.bottom - r.top;
    app->windowed_w = width;
    app->windowed_h = height;

    if( app->screenmode == APP_SCREENMODE_WINDOW )
        SetWindowPos( app->hwnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
    }


int app_window_width( app_t* app )
    {
    RECT r;
    GetClientRect( app->hwnd, &r );
    return r.right - r.left;
    }


int app_window_height( app_t* app )
    {
    RECT r;
    GetClientRect( app->hwnd, &r );
    return r.bottom - r.top;
    }


void app_window_pos( app_t* app, int x, int y )
    {
    if( app->screenmode == APP_SCREENMODE_WINDOW )
        {
        WINDOWPLACEMENT placement;
        placement.length = sizeof( placement );
        GetWindowPlacement( app->hwnd, &placement );
        placement.rcNormalPosition.right = x + ( placement.rcNormalPosition.right - placement.rcNormalPosition.left );
        placement.rcNormalPosition.bottom = y + ( placement.rcNormalPosition.bottom - placement.rcNormalPosition.top );
        placement.rcNormalPosition.left = x;
        placement.rcNormalPosition.top = y;
        SetWindowPlacement( app->hwnd, &placement );
        }

        app->windowed_x = x;
        app->windowed_y = y;
        }


int app_window_x( app_t* app )
    {
    if( app->screenmode == APP_SCREENMODE_WINDOW )
        {
        WINDOWPLACEMENT placement;
        placement.length = sizeof( placement );
        GetWindowPlacement( app->hwnd, &placement );
        return placement.rcNormalPosition.left;
        }
    else
        {
        return app->windowed_x;
        }
    }


int app_window_y( app_t* app )
    {
    if( app->screenmode == APP_SCREENMODE_WINDOW )
        {
        WINDOWPLACEMENT placement;
        placement.length = sizeof( placement );
        GetWindowPlacement( app->hwnd, &placement );
        return placement.rcNormalPosition.top;
        }
    else
        {
        return app->windowed_y;
        }
    }


app_displays_t app_displays( app_t* app )
    {
    app_displays_t displays;
    displays.count = app->display_count;
    displays.displays = app->displays;
    return displays;
    }


void app_present( app_t* app, APP_U32 const* pixels_xbgr, int width, int height, APP_U32 mod_xbgr, APP_U32 border_xbgr )
    {
    if( app->is_minimized ) return;
    if( pixels_xbgr ) app_internal_opengl_present( &app->gl, pixels_xbgr, width, height, mod_xbgr, border_xbgr );
    SwapBuffers( app->hdc );
    }


static void app_sound_write( app_t* app, int sample_pairs_offset, int sample_pairs_count )
    {
    int offset = sample_pairs_offset * 2 * ( 16 / 8 );
    int length = sample_pairs_count * 2 * ( 16 / 8 );

    // Obtain memory address of write block. This will be in two parts if the block wraps around.
    LPVOID lpvPtr1;
    DWORD dwBytes1;
    LPVOID lpvPtr2;
    DWORD dwBytes2;
    HRESULT hr = IDirectSoundBuffer8_Lock( app->dsoundbuf, (DWORD) offset, (DWORD) length, &lpvPtr1, &dwBytes1,
        &lpvPtr2, &dwBytes2, 0 );

    // If DSERR_BUFFERLOST is returned, restore and retry lock.
    if( hr == DSERR_BUFFERLOST )
        {
        IDirectSoundBuffer8_Restore( app->dsoundbuf );
        hr = IDirectSoundBuffer8_Lock( app->dsoundbuf, (DWORD) offset, (DWORD) length, &lpvPtr1, &dwBytes1,
            &lpvPtr2, &dwBytes2, 0 );
        }
    if( FAILED( hr) )
        {
        app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't lock sound buffer" );
        IDirectSound8_Release( app->dsound );
        app->dsound = 0;
        return;
        }

    // Write to pointers.
    app->sound_callback( (APP_S16*) lpvPtr1, (int) dwBytes1 / ( 2 * ( 16 / 8 ) ), app->sound_user_data );
    if( lpvPtr2 ) app->sound_callback( (APP_S16*) lpvPtr2, (int) dwBytes2 / ( 2 * ( 16 / 8 ) ), app->sound_user_data );

    // Release the data back to DirectSound.
    hr = IDirectSoundBuffer8_Unlock( app->dsoundbuf, lpvPtr1, dwBytes1, lpvPtr2, dwBytes2 );
    if( FAILED( hr) )
        {
        app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't unlock sound buffer" );
        IDirectSound8_Release( app->dsound );
        app->dsound = 0;
        return;
        }
    }


static DWORD WINAPI app_sound_thread_proc( LPVOID lpThreadParameter )
    {
    app_t* app = (app_t*) lpThreadParameter;
    int mid_point = app->sample_pairs_count / 2;
    int half_size = mid_point;
    int prev_pos = 0;
    while( InterlockedCompareExchange( &app->exit_sound_thread, 0, 0 ) == 0 )
    {
        WaitForMultipleObjectsEx( 2, app->sound_notifications, FALSE, 100, FALSE );
        DWORD position = 0;
        IDirectSoundBuffer8_GetCurrentPosition( app->dsoundbuf, &position, 0 );
        int pos = ( (int) position )/( 2 * ( 16 / 8 ) );

        if( prev_pos >= mid_point && pos < mid_point )
            app_sound_write( app, mid_point, half_size );
        else if( prev_pos < mid_point && pos >= mid_point )
            app_sound_write( app, 0, half_size );

        prev_pos = pos;
        }

    return 0;
    }


void app_sound( app_t* app, int sample_pairs_count, void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data ), void* user_data )
    {
    if( !app->dsound ) return;

    if( !sound_callback || !sample_pairs_count )
        {
        if( app->sound_thread_handle != INVALID_HANDLE_VALUE )
            {
            InterlockedExchange( &app->exit_sound_thread, 1 );
            WaitForSingleObject( app->sound_thread_handle, INFINITE );
            CloseHandle( app->sound_thread_handle );
            app->sound_thread_handle = INVALID_HANDLE_VALUE;
            }
        if( app->dsoundbuf )
            {
            IDirectSoundBuffer8_Release( app->dsoundbuf );
            app->dsoundbuf = NULL;
            }
        app->sample_pairs_count = 0;
        app->sound_callback = NULL;
        app->sound_user_data = NULL;
        return;
        }

    if( app->sample_pairs_count != sample_pairs_count )
        {
    app->sample_pairs_count = sample_pairs_count;

    if( app->dsoundbuf )
        {
            IDirectSoundBuffer8_Release( app->dsoundbuf );
        app->dsoundbuf = 0;
        }

    if( sample_pairs_count > 0 )
        {
        int const channels = 2;
        int const frequency = 44100;
        int const bits_per_sample = 16;

        WORD const DSOUND_WAVE_FORMAT_PCM = 1;
        DSOUND_WAVEFORMATEX format;
        memset( &format, 0, sizeof( DSOUND_WAVEFORMATEX ) );
        format.wFormatTag = DSOUND_WAVE_FORMAT_PCM;
        format.nChannels = (WORD) channels;
        format.nSamplesPerSec = (DWORD) frequency;
        format.nBlockAlign = (WORD) ( ( channels * bits_per_sample ) / 8 );
        format.nAvgBytesPerSec = (DWORD) ( frequency * format.nBlockAlign );
        format.wBitsPerSample = (WORD) bits_per_sample;
        format.cbSize = 0;

        DSBUFFERDESC dsbdesc;
        memset( &dsbdesc, 0, sizeof( DSBUFFERDESC ) );
        dsbdesc.dwSize = sizeof( DSBUFFERDESC );

            dsbdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY ;

        int size = channels * ( bits_per_sample / 8 ) * sample_pairs_count;
        dsbdesc.dwBufferBytes = (DWORD) size;
        dsbdesc.lpwfxFormat = &format;

            struct IDirectSoundBuffer8* soundbuf = NULL;
            HRESULT hr = IDirectSound8_CreateSoundBuffer( app->dsound, &dsbdesc, &soundbuf, NULL );
            if( FAILED( hr ) || !soundbuf )
            {
            app_log( app, APP_LOG_LEVEL_WARNING, "Failed to create sound buffer" );
                IDirectSound8_Release( app->dsound );
            app->dsound = 0;
                app->sample_pairs_count = 0;
                app->sound_callback = NULL;
                app->sound_user_data = NULL;
            return;
            }

            GUID const GUID_IDirectSoundBuffer8 = { 0x6825a449, 0x7524, 0x4d82, { 0x92, 0x0f, 0x50, 0xe3, 0x6a, 0xb3, 0xab, 0x1e } };
            #ifdef __cplusplus
                GUID const& ref_GUID_IDirectSoundBuffer8 = GUID_IDirectSoundBuffer8;
            #else
                GUID const* ref_GUID_IDirectSoundBuffer8 = &GUID_IDirectSoundBuffer8;
            #endif
            hr = IDirectSoundBuffer8_QueryInterface( soundbuf, ref_GUID_IDirectSoundBuffer8, (void**) &app->dsoundbuf );
            IDirectSoundBuffer8_Release( soundbuf );

            if( FAILED( hr ) || !app->dsoundbuf )
    {
                app_log( app, APP_LOG_LEVEL_WARNING, "Failed to create sound buffer" );
                IDirectSound8_Release( app->dsound );
                app->dsound = 0;
                app->sample_pairs_count = 0;
                app->sound_callback = NULL;
                app->sound_user_data = NULL;
                return;
    }

            struct IDirectSoundNotify* notify = NULL;
            GUID const GUID_IDirectSoundNotify8 = { 0xb0210783, 0x89cd, 0x11d0, { 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16 } };
            #ifdef __cplusplus
                GUID const& ref_GUID_IDirectSoundNotify8 = GUID_IDirectSoundNotify8;
            #else
                GUID const* ref_GUID_IDirectSoundNotify8 = &GUID_IDirectSoundNotify8;
            #endif
            hr = IDirectSoundBuffer8_QueryInterface( app->dsoundbuf, ref_GUID_IDirectSoundNotify8, (void**) &notify );
            if( FAILED( hr ) || !notify )
            {
                app_log( app, APP_LOG_LEVEL_WARNING, "Failed to create sound buffer" );
                IDirectSoundBuffer8_Release( app->dsoundbuf );
                IDirectSound8_Release( app->dsound );
                app->dsound = 0;
                app->dsoundbuf = 0;
                app->sample_pairs_count = 0;
                app->sound_callback = NULL;
                app->sound_user_data = NULL;
                return;
                }

            DSBPOSITIONNOTIFY notify_positions[ 2 ];
            notify_positions[ 0 ].dwOffset = 0;
            notify_positions[ 0 ].hEventNotify = app->sound_notifications[ 0 ];
            notify_positions[ 1 ].dwOffset = (DWORD)( size / 2 );
            notify_positions[ 1 ].hEventNotify = app->sound_notifications[ 1 ];

            IDirectSoundNotify_SetNotificationPositions( notify, 2, notify_positions );
            IDirectSoundNotify_Release( notify );

            InterlockedExchange( &app->exit_sound_thread, 0 );
            app->sound_thread_handle = CreateThread( NULL, 0U, app_sound_thread_proc, app, 0, NULL );
            SetThreadPriority( app->sound_thread_handle, THREAD_PRIORITY_HIGHEST );

            IDirectSoundBuffer8_Play( app->dsoundbuf, 0, 0, DSBPLAY_LOOPING );
        }
        }

    app->sound_callback = sound_callback;
    app->sound_user_data = user_data;
        }


void app_sound_volume( app_t* app, float volume )
    {
    if( !app->dsound ) return;
    if( !app->dsoundbuf ) return;

    int level = volume < 0.000015f ? DSBVOLUME_MIN : (int) ( 2000.0f * (float) log10( (double ) volume ) );
    if( app->sound_level == level ) return;
    app->sound_level = level;

    IDirectSoundBuffer8_SetVolume( app->dsoundbuf, level );
    }


app_input_t app_input( app_t* app )
    {
    app_input_t input;
    input.events = app->input_events;
    input.count = app->input_count;
    app->input_count = 0;
    return input;
    }


void app_coordinates_window_to_bitmap( app_t* app, int width, int height, int* x, int* y )
    {
    if( width == 0 || height == 0 ) return;
    RECT r;
    GetClientRect( app->hwnd, &r );
    int window_width = ( app->screenmode == APP_SCREENMODE_FULLSCREEN ) ? app->fullscreen_width : r.right - r.left;
    int window_height = ( app->screenmode == APP_SCREENMODE_FULLSCREEN ) ? app->fullscreen_height : r.bottom - r.top;


    if( app->interpolation == APP_INTERPOLATION_LINEAR )
        {
        float hscale = window_width / (float) width;
        float vscale = window_height / (float) height;
        float pixel_scale = hscale < vscale ? hscale : vscale;
        if( pixel_scale > 0.0f )
            {
            float hborder = ( window_width - pixel_scale * width ) / 2.0f;
            float vborder = ( window_height - pixel_scale * height ) / 2.0f;
            *x -= (int)( hborder );
            *y -= (int)( vborder );
            *x = (int)( *x / pixel_scale );
            *y = (int)( *y / pixel_scale );
            }
        else
            {
            *x = 0;
            *y = 0;
            }
        }
    else
        {
        int hscale = window_width / width;
        int vscale = window_height / height;
        int pixel_scale = pixel_scale = hscale < vscale ? hscale : vscale;
        pixel_scale = pixel_scale < 1 ? 1 : pixel_scale;
        int hborder = ( window_width - pixel_scale * width ) / 2;
        int vborder = ( window_height - pixel_scale * height ) / 2;
        *x -= (int)( hborder );
        *y -= (int)( vborder );
        *x = (int)( *x / pixel_scale );
        *y = (int)( *y / pixel_scale );
        }
    }


void app_coordinates_bitmap_to_window( app_t* app, int width, int height, int* x, int* y )
    {
    RECT r;
    GetClientRect( app->hwnd, &r );
    int window_width = ( app->screenmode == APP_SCREENMODE_FULLSCREEN ) ? app->fullscreen_width : r.right - r.left;
    int window_height = ( app->screenmode == APP_SCREENMODE_FULLSCREEN ) ? app->fullscreen_height : r.bottom - r.top;


    if( app->interpolation == APP_INTERPOLATION_LINEAR )
        {
        float hscale = window_width / (float) width;
        float vscale = window_height / (float) height;
        float pixel_scale = hscale < vscale ? hscale : vscale;
        if( pixel_scale > 0.0f )
            {
            float hborder = ( window_width - pixel_scale * width ) / 2.0f;
            float vborder = ( window_height - pixel_scale * height ) / 2.0f;
            *x = (int)( *x * pixel_scale );
            *y = (int)( *y * pixel_scale );
            *x += (int)( hborder );
            *y += (int)( vborder );
            }
        else
            {
            *x = 0;
            *y = 0;
            }
        }
    else
        {
        int hscale = window_width / width;
        int vscale = window_height / height;
        int pixel_scale = pixel_scale = hscale < vscale ? hscale : vscale;
        pixel_scale = pixel_scale < 1 ? 1 : pixel_scale;
        int hborder = ( window_width - pixel_scale * width ) / 2;
        int vborder = ( window_height - pixel_scale * height ) / 2;
        *x = (int)( *x * pixel_scale );
        *y = (int)( *y * pixel_scale );
        *x += (int)( hborder );
        *y += (int)( vborder );
        }
    }




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    SDL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#elif defined( APP_SDL )

#include <string.h>
#include <stdio.h>

#ifdef __TINYC__
// TCC can't handle GCC's intrinsic syntax. Disable it.
#define SDL_DISABLE_IMMINTRIN_H
#endif

#include "SDL.h"

#ifndef APP_FATAL_ERROR
    #define APP_FATAL_ERROR( ctx, message ) { \
            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Fatal Error!", message, NULL ); exit( 0xff ); }
#endif

struct app_t
    {
    void* logctx;
    void* fatalctx;
    struct app_internal_opengl_t gl;
    int initialized;
    int exit_requested;
    int has_focus;
    app_interpolation_t interpolation;
    app_screenmode_t screenmode;

    SDL_Window* window;
    SDL_Cursor* cursor;

    SDL_AudioDeviceID sound_device;
    void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data );
    void* sound_user_data;
    int volume;

    app_input_event_t input_events[ 1024 ];
    int input_count;

    int display_count;
    app_display_t displays[ 16 ];

    };


int app_t_size( ) { return sizeof( app_t ); }

int app_init( app_t* app, void* logctx, void* fatalctx )
    {
    app->logctx = logctx;
    app->fatalctx = fatalctx;
    app->interpolation = APP_INTERPOLATION_LINEAR;
    app->screenmode = APP_SCREENMODE_FULLSCREEN;

    int display_count;
    int glres;

    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
        {
//        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        goto init_failed;
        }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    app->window = SDL_CreateWindow( "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 400, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    if( !app->window )
    {
//        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        goto init_failed;
    }

    app->has_focus = 1;
    app->volume = 256;

    display_count = SDL_GetNumVideoDisplays();
    for( int i = 0; i < display_count; ++i )
        {
        SDL_Rect r;
        SDL_GetDisplayBounds( i, &r );
        app_display_t d;
        sprintf( d.id, "DISPLAY%d", i );
        d.x = r.x;
        d.y = r.y;
        d.width = r.w;
        d.height = r.h;
        app->displays[ i ] = d;
        }
    app->display_count = display_count;

    SDL_GL_CreateContext( app->window );
    glewInit();

    SDL_GL_SetSwapInterval( 1 );

    app->gl.CreateShader = glCreateShader;
    app->gl.ShaderSource = glShaderSource;
    app->gl.CompileShader = glCompileShader;
    app->gl.GetShaderiv = glGetShaderiv;
    app->gl.CreateProgram = glCreateProgram;
    app->gl.AttachShader = glAttachShader;
    app->gl.BindAttribLocation = glBindAttribLocation;
    app->gl.LinkProgram = glLinkProgram;
    app->gl.GetProgramiv = glGetProgramiv;
    app->gl.GenBuffers = glGenBuffers;
    app->gl.BindBuffer = glBindBuffer;
    app->gl.EnableVertexAttribArray = glEnableVertexAttribArray;
    app->gl.VertexAttribPointer = glVertexAttribPointer;
    app->gl.GenTextures = glGenTextures;
    app->gl.Enable = glEnable;
    app->gl.ActiveTexture = glActiveTexture;
    app->gl.BindTexture = glBindTexture;
    app->gl.TexParameteri = glTexParameteri;
    app->gl.DeleteBuffers = glDeleteBuffers;
    app->gl.DeleteTextures = glDeleteTextures;
    app->gl.BufferData = glBufferData;
    app->gl.UseProgram = glUseProgram;
    app->gl.Uniform1i = glUniform1i;
    app->gl.Uniform3f = glUniform3f;
    app->gl.GetUniformLocation = glGetUniformLocation;
    app->gl.TexImage2D = glTexImage2D;
    app->gl.ClearColor = glClearColor;
    app->gl.Clear = glClear;
    app->gl.DrawArrays = glDrawArrays;
    app->gl.Viewport = glViewport;
    app->gl.DeleteShader = glDeleteShader;
    app->gl.DeleteProgram = glDeleteProgram;
    #ifdef APP_REPORT_SHADER_ERRORS
        app->gl.GetShaderInfoLog = glGetShaderInfoLog;
    #endif

    glres = app_internal_opengl_init( app, &app->gl, app->interpolation, 640, 400 );
    if( !glres )
        {
        app_fatal_error( app, "OpenGL init fail" );
        goto init_failed;
        }

    return EXIT_SUCCESS;

init_failed:
    app_term( app );
    return EXIT_FAILURE;
    }

void app_term( app_t* app )
    {
    if ( !app ) { return; }

    if( app->sound_device )
        {
        SDL_PauseAudioDevice( app->sound_device, 1 );
        SDL_CloseAudioDevice( app->sound_device );
        app->sound_device = 0;
        app->sound_callback = NULL;
        app->sound_user_data = NULL;
        }

    if( app->cursor ) SDL_FreeCursor( app->cursor );

    //Destroy window
    SDL_DestroyWindow( app->window );


    //Quit SDL subsystems
    SDL_Quit();
    }


static app_key_t app_internal_scancode_to_appkey( app_t* app, SDL_Scancode scancode )

    {
    int map[ 287 * 2 ] = { APP_KEY_INVALID, SDL_SCANCODE_UNKNOWN, APP_KEY_INVALID, 1, APP_KEY_INVALID, 2, APP_KEY_INVALID, 3, APP_KEY_A, SDL_SCANCODE_A,
        APP_KEY_B, SDL_SCANCODE_B, APP_KEY_C, SDL_SCANCODE_C, APP_KEY_D, SDL_SCANCODE_D, APP_KEY_E, SDL_SCANCODE_E, APP_KEY_F, SDL_SCANCODE_F, APP_KEY_G,
        SDL_SCANCODE_G, APP_KEY_H, SDL_SCANCODE_H, APP_KEY_I, SDL_SCANCODE_I, APP_KEY_J, SDL_SCANCODE_J, APP_KEY_K, SDL_SCANCODE_K, APP_KEY_L,
        SDL_SCANCODE_L, APP_KEY_M, SDL_SCANCODE_M, APP_KEY_N, SDL_SCANCODE_N, APP_KEY_O, SDL_SCANCODE_O, APP_KEY_P, SDL_SCANCODE_P, APP_KEY_Q,
        SDL_SCANCODE_Q, APP_KEY_R, SDL_SCANCODE_R, APP_KEY_S, SDL_SCANCODE_S, APP_KEY_T, SDL_SCANCODE_T, APP_KEY_U, SDL_SCANCODE_U, APP_KEY_V,
        SDL_SCANCODE_V, APP_KEY_W, SDL_SCANCODE_W, APP_KEY_X, SDL_SCANCODE_X, APP_KEY_Y, SDL_SCANCODE_Y, APP_KEY_Z, SDL_SCANCODE_Z, APP_KEY_1,
        SDL_SCANCODE_1, APP_KEY_2, SDL_SCANCODE_2, APP_KEY_3, SDL_SCANCODE_3, APP_KEY_4, SDL_SCANCODE_4, APP_KEY_5, SDL_SCANCODE_5, APP_KEY_6,
        SDL_SCANCODE_6, APP_KEY_7, SDL_SCANCODE_7, APP_KEY_8, SDL_SCANCODE_8, APP_KEY_9, SDL_SCANCODE_9, APP_KEY_0, SDL_SCANCODE_0, APP_KEY_RETURN,
        SDL_SCANCODE_RETURN, APP_KEY_ESCAPE, SDL_SCANCODE_ESCAPE, APP_KEY_BACK, SDL_SCANCODE_BACKSPACE, APP_KEY_TAB, SDL_SCANCODE_TAB, APP_KEY_SPACE,
        SDL_SCANCODE_SPACE, APP_KEY_OEM_MINUS, SDL_SCANCODE_MINUS, APP_KEY_INVALID, SDL_SCANCODE_EQUALS, APP_KEY_OEM_4, SDL_SCANCODE_LEFTBRACKET,
        APP_KEY_OEM_6, SDL_SCANCODE_RIGHTBRACKET, APP_KEY_OEM_5, SDL_SCANCODE_BACKSLASH, APP_KEY_INVALID, SDL_SCANCODE_NONUSHASH, APP_KEY_OEM_1,
        SDL_SCANCODE_SEMICOLON, APP_KEY_OEM_7, SDL_SCANCODE_APOSTROPHE, APP_KEY_INVALID, SDL_SCANCODE_GRAVE, APP_KEY_OEM_COMMA, SDL_SCANCODE_COMMA,
        APP_KEY_OEM_PERIOD, SDL_SCANCODE_PERIOD, APP_KEY_OEM_2, SDL_SCANCODE_SLASH, APP_KEY_CAPITAL, SDL_SCANCODE_CAPSLOCK, APP_KEY_F1, SDL_SCANCODE_F1,
        APP_KEY_F2, SDL_SCANCODE_F2, APP_KEY_F3, SDL_SCANCODE_F3, APP_KEY_F4, SDL_SCANCODE_F4, APP_KEY_F5, SDL_SCANCODE_F5, APP_KEY_F6, SDL_SCANCODE_F6,
        APP_KEY_F7, SDL_SCANCODE_F7, APP_KEY_F8, SDL_SCANCODE_F8, APP_KEY_F9, SDL_SCANCODE_F9, APP_KEY_F10, SDL_SCANCODE_F10, APP_KEY_F11,
        SDL_SCANCODE_F11, APP_KEY_F12, SDL_SCANCODE_F12, APP_KEY_SNAPSHOT, SDL_SCANCODE_PRINTSCREEN, APP_KEY_SCROLL, SDL_SCANCODE_SCROLLLOCK,
        APP_KEY_PAUSE, SDL_SCANCODE_PAUSE, APP_KEY_INSERT, SDL_SCANCODE_INSERT, APP_KEY_HOME, SDL_SCANCODE_HOME, APP_KEY_PRIOR, SDL_SCANCODE_PAGEUP,
        APP_KEY_DELETE, SDL_SCANCODE_DELETE, APP_KEY_END, SDL_SCANCODE_END, APP_KEY_NEXT, SDL_SCANCODE_PAGEDOWN, APP_KEY_RIGHT, SDL_SCANCODE_RIGHT,
        APP_KEY_LEFT, SDL_SCANCODE_LEFT, APP_KEY_DOWN, SDL_SCANCODE_DOWN, APP_KEY_UP, SDL_SCANCODE_UP, APP_KEY_NUMLOCK, SDL_SCANCODE_NUMLOCKCLEAR,
        APP_KEY_DIVIDE, SDL_SCANCODE_KP_DIVIDE, APP_KEY_MULTIPLY, SDL_SCANCODE_KP_MULTIPLY, APP_KEY_SUBTRACT, SDL_SCANCODE_KP_MINUS, APP_KEY_ADD,
        SDL_SCANCODE_KP_PLUS, APP_KEY_RETURN, SDL_SCANCODE_KP_ENTER, APP_KEY_NUMPAD1,SDL_SCANCODE_KP_1, APP_KEY_NUMPAD2,SDL_SCANCODE_KP_2, APP_KEY_NUMPAD3,
        SDL_SCANCODE_KP_3, APP_KEY_NUMPAD4,SDL_SCANCODE_KP_4, APP_KEY_NUMPAD5,SDL_SCANCODE_KP_5, APP_KEY_NUMPAD6,SDL_SCANCODE_KP_6, APP_KEY_NUMPAD7,
        SDL_SCANCODE_KP_7, APP_KEY_NUMPAD8,SDL_SCANCODE_KP_8, APP_KEY_NUMPAD9,SDL_SCANCODE_KP_9, APP_KEY_NUMPAD0,SDL_SCANCODE_KP_0, APP_KEY_DECIMAL,
        SDL_SCANCODE_KP_PERIOD, APP_KEY_INVALID, SDL_SCANCODE_NONUSBACKSLASH, APP_KEY_APPS, SDL_SCANCODE_APPLICATION, APP_KEY_INVALID, SDL_SCANCODE_POWER,
        APP_KEY_RETURN, SDL_SCANCODE_KP_EQUALS, APP_KEY_F13, SDL_SCANCODE_F13, APP_KEY_F14, SDL_SCANCODE_F14, APP_KEY_F15, SDL_SCANCODE_F15, APP_KEY_F16,
        SDL_SCANCODE_F16, APP_KEY_F17, SDL_SCANCODE_F17, APP_KEY_F18, SDL_SCANCODE_F18, APP_KEY_F19, SDL_SCANCODE_F19, APP_KEY_F20, SDL_SCANCODE_F20,
        APP_KEY_F21, SDL_SCANCODE_F21, APP_KEY_F22, SDL_SCANCODE_F22, APP_KEY_F23, SDL_SCANCODE_F23, APP_KEY_F24, SDL_SCANCODE_F24, APP_KEY_EXEC,
        SDL_SCANCODE_EXECUTE, APP_KEY_HELP, SDL_SCANCODE_HELP, APP_KEY_MENU, SDL_SCANCODE_MENU, APP_KEY_SELECT, SDL_SCANCODE_SELECT, APP_KEY_MEDIA_STOP,
        SDL_SCANCODE_STOP, APP_KEY_INVALID, SDL_SCANCODE_AGAIN, APP_KEY_INVALID, SDL_SCANCODE_UNDO, APP_KEY_INVALID, SDL_SCANCODE_CUT, APP_KEY_INVALID,
        SDL_SCANCODE_COPY, APP_KEY_INVALID, SDL_SCANCODE_PASTE, APP_KEY_INVALID, SDL_SCANCODE_FIND, APP_KEY_VOLUME_MUTE, SDL_SCANCODE_MUTE,
        APP_KEY_VOLUME_UP, SDL_SCANCODE_VOLUMEUP, APP_KEY_VOLUME_DOWN, SDL_SCANCODE_VOLUMEDOWN, APP_KEY_INVALID, 130, APP_KEY_INVALID, 131,
        APP_KEY_INVALID, 132, APP_KEY_SEPARATOR, SDL_SCANCODE_KP_COMMA, APP_KEY_INVALID, SDL_SCANCODE_KP_EQUALSAS400, APP_KEY_INVALID,
        SDL_SCANCODE_INTERNATIONAL1, APP_KEY_INVALID, SDL_SCANCODE_INTERNATIONAL2, APP_KEY_INVALID, SDL_SCANCODE_INTERNATIONAL3, APP_KEY_INVALID,
        SDL_SCANCODE_INTERNATIONAL4, APP_KEY_INVALID, SDL_SCANCODE_INTERNATIONAL5, APP_KEY_INVALID, SDL_SCANCODE_INTERNATIONAL6, APP_KEY_INVALID,
        SDL_SCANCODE_INTERNATIONAL7, APP_KEY_INVALID, SDL_SCANCODE_INTERNATIONAL8, APP_KEY_INVALID, SDL_SCANCODE_INTERNATIONAL9, APP_KEY_HANGUL,
        SDL_SCANCODE_LANG1, APP_KEY_HANJA, SDL_SCANCODE_LANG2,  APP_KEY_INVALID, SDL_SCANCODE_LANG3, APP_KEY_INVALID, SDL_SCANCODE_LANG4, APP_KEY_INVALID,
        SDL_SCANCODE_LANG5, APP_KEY_INVALID, SDL_SCANCODE_LANG6, APP_KEY_INVALID, SDL_SCANCODE_LANG7, APP_KEY_INVALID, SDL_SCANCODE_LANG8, APP_KEY_INVALID,
        SDL_SCANCODE_LANG9, APP_KEY_INVALID, SDL_SCANCODE_ALTERASE, APP_KEY_INVALID, SDL_SCANCODE_SYSREQ, APP_KEY_CANCEL, SDL_SCANCODE_CANCEL, APP_KEY_CLEAR,
        SDL_SCANCODE_CLEAR, APP_KEY_PRIOR, SDL_SCANCODE_PRIOR, APP_KEY_RETURN, SDL_SCANCODE_RETURN2, APP_KEY_OEM_COMMA, SDL_SCANCODE_SEPARATOR,
        APP_KEY_INVALID, SDL_SCANCODE_OUT, APP_KEY_INVALID, SDL_SCANCODE_OPER, APP_KEY_INVALID, SDL_SCANCODE_CLEARAGAIN, APP_KEY_CRSEL, SDL_SCANCODE_CRSEL,
        APP_KEY_EXSEL, SDL_SCANCODE_EXSEL, APP_KEY_INVALID, 165, APP_KEY_INVALID, 166, APP_KEY_INVALID, 167, APP_KEY_INVALID, 168, APP_KEY_INVALID, 169,
        APP_KEY_INVALID, 170, APP_KEY_INVALID, 171, APP_KEY_INVALID, 172, APP_KEY_INVALID, 173, APP_KEY_INVALID, 174, APP_KEY_INVALID, 175, APP_KEY_INVALID,
        SDL_SCANCODE_KP_00, APP_KEY_INVALID, SDL_SCANCODE_KP_000, APP_KEY_INVALID, SDL_SCANCODE_THOUSANDSSEPARATOR, APP_KEY_INVALID,
        SDL_SCANCODE_DECIMALSEPARATOR, APP_KEY_INVALID, SDL_SCANCODE_CURRENCYUNIT, APP_KEY_INVALID, SDL_SCANCODE_CURRENCYSUBUNIT, APP_KEY_INVALID,
        SDL_SCANCODE_KP_LEFTPAREN, APP_KEY_INVALID, SDL_SCANCODE_KP_RIGHTPAREN, APP_KEY_INVALID, SDL_SCANCODE_KP_LEFTBRACE, APP_KEY_INVALID,
        SDL_SCANCODE_KP_RIGHTBRACE, APP_KEY_INVALID, SDL_SCANCODE_KP_TAB, APP_KEY_INVALID, SDL_SCANCODE_KP_BACKSPACE, APP_KEY_INVALID, SDL_SCANCODE_KP_A,
        APP_KEY_INVALID, SDL_SCANCODE_KP_B, APP_KEY_INVALID, SDL_SCANCODE_KP_C, APP_KEY_INVALID, SDL_SCANCODE_KP_D, APP_KEY_INVALID, SDL_SCANCODE_KP_E,
        APP_KEY_INVALID, SDL_SCANCODE_KP_F, APP_KEY_INVALID, SDL_SCANCODE_KP_XOR, APP_KEY_INVALID, SDL_SCANCODE_KP_POWER, APP_KEY_INVALID,
        SDL_SCANCODE_KP_PERCENT, APP_KEY_INVALID, SDL_SCANCODE_KP_LESS, APP_KEY_INVALID, SDL_SCANCODE_KP_GREATER, APP_KEY_INVALID, SDL_SCANCODE_KP_AMPERSAND,
        APP_KEY_INVALID, SDL_SCANCODE_KP_DBLAMPERSAND, APP_KEY_INVALID, SDL_SCANCODE_KP_VERTICALBAR, APP_KEY_INVALID, SDL_SCANCODE_KP_DBLVERTICALBAR,
        APP_KEY_INVALID, SDL_SCANCODE_KP_COLON, APP_KEY_INVALID, SDL_SCANCODE_KP_HASH, APP_KEY_INVALID, SDL_SCANCODE_KP_SPACE, APP_KEY_INVALID,
        SDL_SCANCODE_KP_AT, APP_KEY_INVALID, SDL_SCANCODE_KP_EXCLAM, APP_KEY_INVALID, SDL_SCANCODE_KP_MEMSTORE, APP_KEY_INVALID, SDL_SCANCODE_KP_MEMRECALL,
        APP_KEY_INVALID, SDL_SCANCODE_KP_MEMCLEAR, APP_KEY_INVALID, SDL_SCANCODE_KP_MEMADD, APP_KEY_INVALID, SDL_SCANCODE_KP_MEMSUBTRACT, APP_KEY_INVALID,
        SDL_SCANCODE_KP_MEMMULTIPLY, APP_KEY_INVALID, SDL_SCANCODE_KP_MEMDIVIDE, APP_KEY_INVALID, SDL_SCANCODE_KP_PLUSMINUS, APP_KEY_INVALID,
        SDL_SCANCODE_KP_CLEAR, APP_KEY_INVALID, SDL_SCANCODE_KP_CLEARENTRY, APP_KEY_INVALID, SDL_SCANCODE_KP_BINARY, APP_KEY_INVALID, SDL_SCANCODE_KP_OCTAL,
        APP_KEY_INVALID, SDL_SCANCODE_KP_DECIMAL, APP_KEY_INVALID, SDL_SCANCODE_KP_HEXADECIMAL, APP_KEY_INVALID, 222, APP_KEY_INVALID, 223, APP_KEY_LCONTROL,
        SDL_SCANCODE_LCTRL, APP_KEY_LSHIFT, SDL_SCANCODE_LSHIFT, APP_KEY_LMENU, SDL_SCANCODE_LALT, APP_KEY_LWIN, SDL_SCANCODE_LGUI, APP_KEY_RCONTROL,
        SDL_SCANCODE_RCTRL, APP_KEY_RSHIFT, SDL_SCANCODE_RSHIFT, APP_KEY_RMENU, SDL_SCANCODE_RALT, APP_KEY_RWIN, SDL_SCANCODE_RGUI, APP_KEY_INVALID, 232,
        APP_KEY_INVALID, 233, APP_KEY_INVALID, 234, APP_KEY_INVALID, 235, APP_KEY_INVALID, 236, APP_KEY_INVALID, 237, APP_KEY_INVALID, 238, APP_KEY_INVALID,
        239, APP_KEY_INVALID, 240, APP_KEY_INVALID, 241, APP_KEY_INVALID, 242, APP_KEY_INVALID, 243, APP_KEY_INVALID, 244, APP_KEY_INVALID, 245,
        APP_KEY_INVALID, 246, APP_KEY_INVALID, 247, APP_KEY_INVALID, 248, APP_KEY_INVALID, 249, APP_KEY_INVALID, 250, APP_KEY_INVALID, 251, APP_KEY_INVALID,
        252, APP_KEY_INVALID, 253, APP_KEY_INVALID, 254, APP_KEY_INVALID, 255, APP_KEY_INVALID, 256, APP_KEY_MODECHANGE, SDL_SCANCODE_MODE,
        APP_KEY_MEDIA_NEXT_TRACK, SDL_SCANCODE_AUDIONEXT, APP_KEY_MEDIA_PREV_TRACK, SDL_SCANCODE_AUDIOPREV, APP_KEY_MEDIA_PLAY_PAUSE, SDL_SCANCODE_AUDIOSTOP,
        APP_KEY_PLAY, SDL_SCANCODE_AUDIOPLAY, APP_KEY_VOLUME_MUTE, SDL_SCANCODE_AUDIOMUTE, APP_KEY_LAUNCH_MEDIA_SELECT, SDL_SCANCODE_MEDIASELECT,
        APP_KEY_INVALID, SDL_SCANCODE_WWW, APP_KEY_LAUNCH_MAIL, SDL_SCANCODE_MAIL, APP_KEY_INVALID, SDL_SCANCODE_CALCULATOR, APP_KEY_INVALID,
        SDL_SCANCODE_COMPUTER, APP_KEY_BROWSER_SEARCH, SDL_SCANCODE_AC_SEARCH, APP_KEY_BROWSER_HOME, SDL_SCANCODE_AC_HOME, APP_KEY_BROWSER_BACK,
        SDL_SCANCODE_AC_BACK, APP_KEY_BROWSER_FORWARD, SDL_SCANCODE_AC_FORWARD, APP_KEY_BROWSER_STOP, SDL_SCANCODE_AC_STOP, APP_KEY_BROWSER_REFRESH,
        SDL_SCANCODE_AC_REFRESH, APP_KEY_BROWSER_FAVORITES, SDL_SCANCODE_AC_BOOKMARKS, APP_KEY_INVALID, SDL_SCANCODE_BRIGHTNESSDOWN, APP_KEY_INVALID,
        SDL_SCANCODE_BRIGHTNESSUP, APP_KEY_INVALID, SDL_SCANCODE_DISPLAYSWITCH, APP_KEY_INVALID, SDL_SCANCODE_KBDILLUMTOGGLE, APP_KEY_INVALID,
        SDL_SCANCODE_KBDILLUMDOWN, APP_KEY_INVALID, SDL_SCANCODE_KBDILLUMUP, APP_KEY_INVALID, SDL_SCANCODE_EJECT, APP_KEY_SLEEP, SDL_SCANCODE_SLEEP,
        APP_KEY_LAUNCH_APP1, SDL_SCANCODE_APP1, APP_KEY_LAUNCH_APP2, SDL_SCANCODE_APP2, APP_KEY_INVALID, SDL_SCANCODE_AUDIOREWIND, APP_KEY_INVALID,
        SDL_SCANCODE_AUDIOFASTFORWARD, };

    if( scancode < 0 || scancode >= sizeof( map ) / ( 2 * sizeof( *map ) ) ) return APP_KEY_INVALID;
    if( map[ scancode * 2 + 1 ] != scancode )
        {
        app_log( app, APP_LOG_LEVEL_ERROR, "Keymap definition error" );
        return APP_KEY_INVALID;
        }
    return (app_key_t) map[ scancode * 2 ];
    }


static void app_internal_add_input_event( app_t* app, app_input_event_t* event )
    {
    if( app->has_focus )
        {
        if( app->input_count < sizeof( app->input_events ) / sizeof( *app->input_events ) )
            app->input_events[ app->input_count++ ] = *event;
        }
    }


app_state_t app_yield( app_t* app )
    {
    if( !app->initialized )
        {
        app->initialized = 1;
        if( app->screenmode == APP_SCREENMODE_FULLSCREEN ) SDL_SetWindowFullscreen( app->window, SDL_WINDOW_FULLSCREEN_DESKTOP );
        SDL_ShowWindow( app->window );
        int w = app->gl.window_width;
        int h = app->gl.window_height;
        SDL_GL_GetDrawableSize( app->window, &w, &h );
        app_internal_opengl_resize( &app->gl, w, h );
        }

    SDL_Event e;
    while( SDL_PollEvent( &e ) )
        {
        if( e.type == SDL_WINDOWEVENT )
            {
            if( e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
                {
                int w = app->gl.window_width;
                int h = app->gl.window_height;
                SDL_GL_GetDrawableSize( app->window, &w, &h );
                if( w != app->gl.window_width || h != app->gl.window_height )
                    {
                    app_internal_opengl_resize( &app->gl, w, h );
                    }
                }
            else if( e.window.event == SDL_WINDOWEVENT_CLOSE )
                {
                app->exit_requested = 1;
                }
            else if( e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED )
                {
                app->has_focus = 1;
                }
            else if( e.window.event == SDL_WINDOWEVENT_FOCUS_LOST )
                {
                app->has_focus = 0;
                }
            }
        else if( e.type == SDL_KEYDOWN )
            {
            app_input_event_t input_event;
            input_event.type = APP_INPUT_KEY_DOWN;
            input_event.data.key = app_internal_scancode_to_appkey( app, e.key.keysym.scancode );
            app_internal_add_input_event( app, &input_event );
            }
        else if( e.type == SDL_KEYUP )
            {
            app_input_event_t input_event;
            input_event.type = APP_INPUT_KEY_UP;
            input_event.data.key = app_internal_scancode_to_appkey( app, e.key.keysym.scancode );
            app_internal_add_input_event( app, &input_event );
            }
        else if( e.type == SDL_TEXTINPUT )
            {
            app_input_event_t input_event;
            char *c;
            input_event.type = APP_INPUT_CHAR;
            for ( c = e.text.text; *c; c++ )
                {
                input_event.data.char_code = *c;
                app_internal_add_input_event( app, &input_event );
                }
            }
        else if( e.type == SDL_MOUSEMOTION )
            {
            app_input_event_t input_event;
            input_event.type = APP_INPUT_MOUSE_MOVE;
            input_event.data.mouse_pos.x = e.motion.x;
            input_event.data.mouse_pos.y = e.motion.y;
            app_internal_add_input_event( app, &input_event );

            input_event.type = APP_INPUT_MOUSE_DELTA;
            input_event.data.mouse_pos.x = e.motion.xrel;
            input_event.data.mouse_pos.y = e.motion.yrel;
            app_internal_add_input_event( app, &input_event );
            }
        else if( e.type == SDL_MOUSEBUTTONDOWN )
            {
            app_input_event_t input_event;
            input_event.type = APP_INPUT_KEY_DOWN;
            if( e.button.button == SDL_BUTTON_LEFT )
                input_event.data.key = APP_KEY_LBUTTON;
            else if( e.button.button == SDL_BUTTON_RIGHT )
                input_event.data.key = APP_KEY_RBUTTON;
            else if( e.button.button == SDL_BUTTON_MIDDLE )
                input_event.data.key = APP_KEY_MBUTTON;
            else if( e.button.button == SDL_BUTTON_X1 )
                input_event.data.key = APP_KEY_XBUTTON1;
            else if( e.button.button == SDL_BUTTON_X2 )
                input_event.data.key = APP_KEY_XBUTTON2;
            app_internal_add_input_event( app, &input_event );
            }
        else if( e.type == SDL_MOUSEBUTTONUP )
            {
            app_input_event_t input_event;
            input_event.type = APP_INPUT_KEY_UP;
            if( e.button.button == SDL_BUTTON_LEFT )
                input_event.data.key = APP_KEY_LBUTTON;
            else if( e.button.button == SDL_BUTTON_RIGHT )
                input_event.data.key = APP_KEY_RBUTTON;
            else if( e.button.button == SDL_BUTTON_MIDDLE )
                input_event.data.key = APP_KEY_MBUTTON;
            else if( e.button.button == SDL_BUTTON_X1 )
                input_event.data.key = APP_KEY_XBUTTON1;
            else if( e.button.button == SDL_BUTTON_X2 )
                input_event.data.key = APP_KEY_XBUTTON2;
            app_internal_add_input_event( app, &input_event );
            }
        else if( e.type == SDL_MOUSEWHEEL )
            {
            float const microsoft_mouse_wheel_constant = 120.0f;
            float wheel_delta = ( (float) e.wheel.y ) / microsoft_mouse_wheel_constant;
            if( app->input_count > 0 && app->input_events[ app->input_count - 1 ].type == APP_INPUT_SCROLL_WHEEL )
                {
                app_input_event_t* event = &app->input_events[ app->input_count - 1 ];
                event->data.wheel_delta += wheel_delta;
                }
            else
                {
                app_input_event_t input_event;
                input_event.type = APP_INPUT_SCROLL_WHEEL;
                input_event.data.wheel_delta = wheel_delta;
                app_internal_add_input_event( app, &input_event );
                }
            }

        }

    return app->exit_requested ? APP_STATE_EXIT_REQUESTED : APP_STATE_NORMAL;
    }


void app_cancel_exit( app_t* app )
    {
    app->exit_requested = 0;
    }


void app_title( app_t* app, char const* title )
    {
    SDL_SetWindowTitle( app->window, title );
    }


char const* app_cmdline( app_t* app ) { /* NOT IMPLEMENTED */ return NULL; }
char const* app_filename( app_t* app ) { /* NOT IMPLEMENTED */ return NULL; }
char const* app_userdata( app_t* app ) { /* NOT IMPLEMENTED */ return NULL; }
char const* app_appdata( app_t* app ) { /* NOT IMPLEMENTED */ return NULL; }


APP_U64 app_time_count( app_t* app )
    {
    return SDL_GetPerformanceCounter();
    }


APP_U64 app_time_freq( app_t* app )
    {
    return SDL_GetPerformanceFrequency();
    }


void app_log( app_t* app, app_log_level_t level, char const* message ) { /* NOT IMPLEMENTED */ }


void app_fatal_error( app_t* app, char const* message )
    {
    APP_FATAL_ERROR( app->fatalctx, message );
    }


void app_pointer( app_t* app, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y )
    {
    SDL_Surface* surf = SDL_CreateRGBSurfaceFrom( (void*)pixels_abgr, width, height, 32, 4 * width, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );
    if( app->cursor ) SDL_FreeCursor( app->cursor );
    app->cursor = SDL_CreateColorCursor( surf, hotspot_x, hotspot_y );
    SDL_SetCursor( app->cursor );
    SDL_FreeSurface( surf );
    }


void app_pointer_default( app_t* app, int* width, int* height, APP_U32* pixels_abgr, int* hotspot_x, int* hotspot_y ) { /* NOT IMPLEMENTED */ }


void app_pointer_pos( app_t* app, int x, int y )
    {
    SDL_WarpMouseInWindow( app->window, x, y );
    }


int app_pointer_x( app_t* app )
    {
    int x = 0;
    SDL_GetMouseState( &x, NULL );
    return x;
    }


int app_pointer_y( app_t* app )
    {
    int y = 0;
    SDL_GetMouseState( NULL, &y );
    return y;
    }


void app_pointer_limit( app_t* app, int x, int y, int width, int height ) { /* NOT IMPLEMENTED */ }
void app_pointer_limit_off( app_t* app ) { /* NOT IMPLEMENTED */ }

void app_interpolation( app_t* app, app_interpolation_t interpolation )
    {
    if( interpolation == app->interpolation ) return;
    app->interpolation = interpolation;

    int mouse_x;
    int mouse_y;
    SDL_GetMouseState( &mouse_x, &mouse_y );

    app_input_event_t input_event;
    input_event.type = APP_INPUT_MOUSE_MOVE;
    input_event.data.mouse_pos.x = mouse_x;
    input_event.data.mouse_pos.y = mouse_y;
    app_internal_add_input_event( app, &input_event );

    app_internal_opengl_interpolation( &app->gl, interpolation );
    }


void app_screenmode( app_t* app, app_screenmode_t screenmode )
    {
    if( screenmode != app->screenmode )
        {
        app->screenmode = screenmode;
        SDL_SetWindowFullscreen( app->window,
            screenmode == APP_SCREENMODE_FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP  : 0 );
        }
    }


void app_window_size( app_t* app, int width, int height )
    {
    SDL_SetWindowSize( app->window, width, height );
    }


int app_window_width( app_t* app )
    {
    int width = 0;
    SDL_GetWindowSize( app->window, &width, NULL );
    return width;
    }


int app_window_height( app_t* app )
    {
    int height = 0;
    SDL_GetWindowSize( app->window, NULL, &height );
    return height;
    }


void app_window_pos( app_t* app, int x, int y )
    {
    SDL_SetWindowPosition( app->window, x, y );
    }


int app_window_x( app_t* app )
    {
    int x = 0;
    SDL_GetWindowPosition( app->window, &x, NULL );
    return x;
    }


int app_window_y( app_t* app )
    {
    int y = 0;
    SDL_GetWindowPosition( app->window, NULL, &y );
    return y;
    }


app_displays_t app_displays( app_t* app )
    {
    app_displays_t displays;
    displays.count = app->display_count;
    displays.displays = app->displays;
    return displays;
    }


void app_present( app_t* app, APP_U32 const* pixels_xbgr, int width, int height, APP_U32 mod_xbgr, APP_U32 border_xbgr )
    {
    if( pixels_xbgr ) app_internal_opengl_present( &app->gl, pixels_xbgr, width, height, mod_xbgr, border_xbgr );
    SDL_GL_SwapWindow( app->window );
    }


static void app_internal_sdl_sound_callback( void* userdata, Uint8* stream, int len )
    {
    app_t* app = (app_t*) userdata;
    if( app->sound_callback )
        {
        app->sound_callback( (APP_S16*) stream, len / ( 2 * sizeof( APP_S16 ) ), app->sound_user_data );
        if( app->volume < 256 )
            {
            APP_S16* samples = (APP_S16*) stream;
            for( int i = 0; i < len / sizeof( APP_S16 ); ++i )
                {
                int s = (int)(*samples);
                s = ( s * app->volume ) >> 8;
                *samples++ = (APP_S16) s;
                }
            }
        }
    }


void app_sound( app_t* app, int sample_pairs_count, void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data ), void* user_data )
    {
    if( app->sound_device )
        {
        SDL_PauseAudioDevice( app->sound_device, 1 );
        SDL_CloseAudioDevice( app->sound_device );
        app->sound_callback = NULL;
        app->sound_user_data = NULL;
        app->sound_device = 0;
        }
    if( sample_pairs_count > 0 && sound_callback )
        {
        SDL_AudioSpec spec;
        spec.freq = 44100;
        spec.format = AUDIO_S16;
        spec.channels = 2;
        spec.silence = 0;
        spec.samples = sample_pairs_count * 2;
        spec.padding = 0;
        spec.size = 0;
        spec.callback = app_internal_sdl_sound_callback;
        spec.userdata = app;

        app->sound_device = SDL_OpenAudioDevice( NULL, 0, &spec, NULL, 0 );
        if( !app->sound_device ) return;

        app->sound_callback = sound_callback;
        app->sound_user_data = user_data;
        SDL_PauseAudioDevice( app->sound_device, 0 );
        }
    }


void app_sound_volume( app_t* app, float volume )
    {
    int v = (int) ( volume * 256.0f );
    app->volume = v < 0 ? 0 : v > 256 ? 256 : v;
    }


app_input_t app_input( app_t* app )
    {
    app_input_t input;
    input.events = app->input_events;
    input.count = app->input_count;
    app->input_count = 0;
    return input;
    }


void app_coordinates_window_to_bitmap( app_t* app, int width, int height, int* x, int* y )
    {
    if( width == 0 || height == 0 ) return;
    int window_width;
    int window_height;
    SDL_GL_GetDrawableSize( app->window, &window_width, &window_height );


    if( app->interpolation == APP_INTERPOLATION_LINEAR )
        {
        float hscale = window_width / (float) width;
        float vscale = window_height / (float) height;
        float pixel_scale = hscale < vscale ? hscale : vscale;
        if( pixel_scale > 0.0f )
            {
            float hborder = ( window_width - pixel_scale * width ) / 2.0f;
            float vborder = ( window_height - pixel_scale * height ) / 2.0f;
            *x -= (int)( hborder );
            *y -= (int)( vborder );
            *x = (int)( *x / pixel_scale );
            *y = (int)( *y / pixel_scale );
            }
        else
            {
            *x = 0;
            *y = 0;
            }
        }
    else
        {
        int hscale = window_width / width;
        int vscale = window_height / height;
        int pixel_scale = pixel_scale = hscale < vscale ? hscale : vscale;
        pixel_scale = pixel_scale < 1 ? 1 : pixel_scale;
        int hborder = ( window_width - pixel_scale * width ) / 2;
        int vborder = ( window_height - pixel_scale * height ) / 2;
        *x -= (int)( hborder );
        *y -= (int)( vborder );
        *x = (int)( *x / pixel_scale );
        *y = (int)( *y / pixel_scale );
        }
    }


void app_coordinates_bitmap_to_window( app_t* app, int width, int height, int* x, int* y )
    {
    int window_width;
    int window_height;
    SDL_GL_GetDrawableSize( app->window, &window_width, &window_height );

    if( app->interpolation == APP_INTERPOLATION_LINEAR )
        {
        float hscale = window_width / (float) width;
        float vscale = window_height / (float) height;
        float pixel_scale = hscale < vscale ? hscale : vscale;
        if( pixel_scale > 0.0f )
            {
            float hborder = ( window_width - pixel_scale * width ) / 2.0f;
            float vborder = ( window_height - pixel_scale * height ) / 2.0f;
            *x = (int)( *x * pixel_scale );
            *y = (int)( *y * pixel_scale );
            *x += (int)( hborder );
            *y += (int)( vborder );
            }
        else
            {
            *x = 0;
            *y = 0;
            }
        }
    else
        {
        int hscale = window_width / width;
        int vscale = window_height / height;
        int pixel_scale = pixel_scale = hscale < vscale ? hscale : vscale;
        pixel_scale = pixel_scale < 1 ? 1 : pixel_scale;
        int hborder = ( window_width - pixel_scale * width ) / 2;
        int vborder = ( window_height - pixel_scale * height ) / 2;
        *x = (int)( *x * pixel_scale );
        *y = (int)( *y * pixel_scale );
        *x += (int)( hborder );
        *y += (int)( vborder );
        }
    }




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    WEBASSEMBLY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#elif defined( APP_WASM )

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

WAJIC(void, app_js_print, (const char* msg),
    {
    WA.print(MStrGet(msg) + "\n");
    })

#ifndef APP_FATAL_ERROR
    #define APP_FATAL_ERROR( ctx, message ) { app_js_print(message); abort(); }
#endif

struct app_t
    {
    void* logctx;
    void* fatalctx;
    struct app_internal_opengl_t gl;
    int has_focus;
    app_interpolation_t interpolation;

    void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data );
    void* sound_user_data;
    int sound_buffer_size;
    APP_S16 sound_buffer[ 2048 * 2 ];
    int volume;

    app_input_event_t input_events[ 1024 ];
    int input_count;
    int pointer_x;
    int pointer_y;
    };

int app_t_size( ) { return sizeof( app_t ); }


// The javascript event handling keeps a simple buffer of events with 3 ints per event
// The first int is the event id, followed by two arguments (not all events use both arguments)
//   EVENT ID                  ARG 1           ARG 2
//   1 WINDOW_SIZE_CHANGED     w               h
//   2 WINDOW_FOCUS            gained/lost
//   3 KEY                     down/up         scancode
//   4 CHAR                    charcode
//   5 MOUSE_MOTION            x               y
//   6 MOUSE_BUTTON            down/up         buttonnum
//   7 MOUSE_WHEEL             wheel

WAJIC_WITH_INIT(
(
    var evts = [], evtcursor = 0, dpr = window.devicePixelRatio||1, canvas, aspect_ratio, fullscreen;
    var update_canvas_size = (w)=>
    {
        var h = (w *= dpr)/aspect_ratio|0;
        if (w<32 || h<32 || (w == canvas.width && h == canvas.height) || fullscreen) return;
        canvas.width = w;
        canvas.height = h;
        evts.push(1, canvas.width, canvas.height);
    };
    var alias = (el, a, b, c)=>
    {
        return el[a+c] || el['moz'+b+c] || el['webkit'+b+c] || el['ms'+b+c];
    };
),
void, app_js_setup_canvas, (int* out_width, int* out_height),
{
    canvas = WA.canvas;
    if (!canvas.height) { canvas.width = 1024; canvas.height = 576; }
    MU32[out_width>>2] = canvas.clientWidth;
    MU32[out_height>>2] = canvas.clientHeight;
    aspect_ratio = canvas.width/canvas.height;

    var cancelEvent = (e)=>{ if (e.preventDefault) e.preventDefault(true); else if (e.stopPropagation) e.stopPropagation(true); else e.stopped = true; };
    var documtEvent = (t, f, a)=>{ document.addEventListener(t, f); if (!a) { documtEvent('moz'+t, f, 1); documtEvent('webkit'+t, f, 1); documtEvent('ms'+t, f, 1); } };
    var windowEvent = (t, f)=>{ window.addEventListener(t, f, true); };
    var canvasEvent = (t, f)=>{ canvas.addEventListener(t, f, {capture:true,passive:false}); };
    windowEvent('resize', ()=>{ update_canvas_size(canvas.clientWidth); });
    windowEvent('focus', ()=>{ evts.push(2, 1, 0); });
    windowEvent('blur',  ()=>{ evts.push(2, (fullscreen?1:0), 0); });
    windowEvent('keydown', (e)=>
    {
        evts.push(3, 1, e.keyCode);
        if (e.key.length == 1 && e.key.charCodeAt() < 128 && !e.ctrlKey) evts.push(4, e.key.charCodeAt(), 0);
        cancelEvent(e);
    });
    windowEvent('keyup',   (e)=>
    {
        evts.push(3, 0, e.keyCode);
        cancelEvent(e);
    });
    canvasEvent('mousemove', (e)=>
    {
        evts.push(5,
            (e.offsetX * canvas.width /  canvas.clientWidth )|0,
            (e.offsetY * canvas.height / canvas.clientHeight)|0);
        cancelEvent(e);
    });
    var buttons = 0;
    canvasEvent('mousedown', (e)=>
    {
        var btn = (1<<e.button);
        if (buttons & btn) return;
        buttons |= btn;
        evts.push(6, 1, e.button);
        cancelEvent(e);
    });
    windowEvent('mouseup', (e)=>
    {
        var btn = (1<<e.button);
        if (!(buttons & btn)) return;
        buttons &= ~btn;
        evts.push(6, 0, e.button);
        cancelEvent(e);
    });
    canvasEvent('wheel', (e)=>{ evts.push(7, e.deltaY); cancelEvent(e); });
    documtEvent('fullscreenchange', ()=>
    {
        fullscreen = alias(document,'f','F','ullscreenElement') || alias(document,'f','F','ullScreenElement');
        if (fullscreen)
        {
            canvas.orgS = canvas.style.cssText;
            canvas.orgW = canvas.clientWidth;
            canvas.style.cssText = 'background:black';
            canvas.height = screen.height * dpr;
            canvas.width = screen.width * dpr;
            evts.push(1, canvas.width, canvas.height);
        }
        else if (canvas.orgS)
        {
            canvas.style.cssText = canvas.orgS;
            update_canvas_size(canvas.orgW);
        }
    });
    WA.SetFullscreen = (f)=>
    {
        if (!f == !fullscreen) return;
        var el = (f ? WA.canvas : document);
        var fn = (f ? (alias(el,'r','R','equestFullscreen') || alias(el,'r','R','equestFullScreen')) : (alias(el,'e','E','xitFullscreen') || alias(el,'c','C','ancelFullScreen')));
        if (fn) fn.apply(el, []);
    };
})

WAJIC(void, app_js_screenmode, (int fullscreen),
{
    WA.SetFullscreen(fullscreen);
})

WAJIC(void, app_js_set_aspect_ratio, (int* width, int* height),
{
    var new_aspect_ratio = MU32[width>>2]/MU32[height>>2];
    if (Math.abs(new_aspect_ratio - aspect_ratio) > 0.01)
    {
        aspect_ratio = new_aspect_ratio;
        update_canvas_size(canvas.clientWidth);
    }
    MU32[width>>2] = canvas.width;
    MU32[height>>2] = canvas.height;
})

WAJIC(int, app_js_get_event, (int evt[3]),
{
    if (evtcursor >= evts.length)
    {
        evts.length = evtcursor = 0;
        return 0;
    }
    MU32[(evt>>2)+0] = evts[evtcursor++];
    MU32[(evt>>2)+1] = evts[evtcursor++];
    MU32[(evt>>2)+2] = evts[evtcursor++];
    return 1;
})


int app_init( app_t* app, void* logctx, void* fatalctx )
    {
    app->logctx = logctx;
    app->fatalctx = fatalctx;
    app->interpolation = APP_INTERPOLATION_LINEAR;

    app->gl.CreateShader = glCreateShader;
    app->gl.ShaderSource = glShaderSource;
    app->gl.CompileShader = glCompileShader;
    app->gl.GetShaderiv = glGetShaderiv;
    app->gl.CreateProgram = glCreateProgram;
    app->gl.AttachShader = glAttachShader;
    app->gl.BindAttribLocation = glBindAttribLocation;
    app->gl.LinkProgram = glLinkProgram;
    app->gl.GetProgramiv = glGetProgramiv;
    app->gl.GenBuffers = glGenBuffers;
    app->gl.BindBuffer = glBindBuffer;
    app->gl.EnableVertexAttribArray = glEnableVertexAttribArray;
    app->gl.VertexAttribPointer = glVertexAttribPointer;
    app->gl.GenTextures = glGenTextures;
    app->gl.Enable = glEnable;
    app->gl.ActiveTexture = glActiveTexture;
    app->gl.BindTexture = glBindTexture;
    app->gl.TexParameteri = glTexParameteri;
    app->gl.DeleteBuffers = glDeleteBuffers;
    app->gl.DeleteTextures = glDeleteTextures;
    app->gl.BufferData = glBufferData;
    app->gl.UseProgram = glUseProgram;
    app->gl.Uniform1i = glUniform1i;
    app->gl.Uniform3f = glUniform3f;
    app->gl.GetUniformLocation = glGetUniformLocation;
    app->gl.TexImage2D = glTexImage2D;
    app->gl.ClearColor = glClearColor;
    app->gl.Clear = glClear;
    app->gl.DrawArrays = glDrawArrays;
    app->gl.Viewport = glViewport;
    app->gl.DeleteShader = glDeleteShader;
    app->gl.DeleteProgram = glDeleteProgram;
    #ifdef APP_REPORT_SHADER_ERRORS
        app->gl.GetShaderInfoLog = glGetShaderInfoLog;
    #endif

    app_js_setup_canvas( &app->gl.window_width, &app->gl.window_height );
    glSetupCanvasContext( 1, 0, 0, 0 );
    glViewport( 0, 0, app->gl.window_width, app->gl.window_height );

    app->has_focus = 1;
    app->volume = 256;

    int glres = app_internal_opengl_init( app, &app->gl, app->interpolation, app->gl.window_width, app->gl.window_height );
    if( !glres )
        {
        app_fatal_error( app, "OpenGL init fail" );
        goto init_failed;
        }
    WaCoroInitNew( NULL, NULL, NULL, 0 );

    return EXIT_SUCCESS;

init_failed:
    app_term( app );
    return EXIT_FAILURE;
    }

void app_term( app_t* app )
    {
    }


static void app_internal_add_input_event( app_t* app, app_input_event_t* event )
    {
    if( app->has_focus )
        {
        if( app->input_count < sizeof( app->input_events ) / sizeof( *app->input_events ) )
            app->input_events[ app->input_count++ ] = *event;
        }
    }


static app_key_t app_internal_scancode_to_appkey( app_t* app, int scancode )
    {
    static const app_key_t map[] = {
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_CANCEL,      APP_KEY_INVALID,     APP_KEY_INVALID,   APP_KEY_HELP,         APP_KEY_INVALID,         APP_KEY_BACK,            APP_KEY_TAB,
        APP_KEY_RETURN,         APP_KEY_INVALID,             APP_KEY_CLEAR,        APP_KEY_RETURN,      APP_KEY_RETURN,      APP_KEY_INVALID,   APP_KEY_LSHIFT,       APP_KEY_LCONTROL,        APP_KEY_LMENU,           APP_KEY_PAUSE,
        APP_KEY_CAPITAL,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_INVALID,     APP_KEY_HANJA,     APP_KEY_INVALID,      APP_KEY_ESCAPE,          APP_KEY_INVALID,         APP_KEY_INVALID,
        APP_KEY_INVALID,        APP_KEY_MODECHANGE,          APP_KEY_SPACE,        APP_KEY_PRIOR,       APP_KEY_NEXT,        APP_KEY_END,       APP_KEY_HOME,         APP_KEY_LEFT,            APP_KEY_UP,              APP_KEY_RIGHT,
        APP_KEY_DOWN,           APP_KEY_SELECT,              APP_KEY_SNAPSHOT,     APP_KEY_EXEC,        APP_KEY_SNAPSHOT,    APP_KEY_INSERT,    APP_KEY_DELETE,       APP_KEY_HELP,            APP_KEY_0,               APP_KEY_1,
        APP_KEY_2,              APP_KEY_3,                   APP_KEY_4,            APP_KEY_5,           APP_KEY_6,           APP_KEY_7,         APP_KEY_8,            APP_KEY_9,               APP_KEY_INVALID,         APP_KEY_OEM_1,
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_INVALID,     APP_KEY_A,         APP_KEY_B,            APP_KEY_C,               APP_KEY_D,               APP_KEY_E,
        APP_KEY_F,              APP_KEY_G,                   APP_KEY_H,            APP_KEY_I,           APP_KEY_J,           APP_KEY_K,         APP_KEY_L,            APP_KEY_M,               APP_KEY_N,               APP_KEY_O,
        APP_KEY_P,              APP_KEY_Q,                   APP_KEY_R,            APP_KEY_S,           APP_KEY_T,           APP_KEY_U,         APP_KEY_V,            APP_KEY_W,               APP_KEY_X,               APP_KEY_Y,
        APP_KEY_Z,              APP_KEY_LWIN,                APP_KEY_RWIN,         APP_KEY_APPS,        APP_KEY_INVALID,     APP_KEY_SLEEP,     APP_KEY_NUMPAD0,      APP_KEY_NUMPAD1,         APP_KEY_NUMPAD2,         APP_KEY_NUMPAD3,
        APP_KEY_NUMPAD4,        APP_KEY_NUMPAD5,             APP_KEY_NUMPAD6,      APP_KEY_NUMPAD7,     APP_KEY_NUMPAD8,     APP_KEY_NUMPAD9,   APP_KEY_MULTIPLY,     APP_KEY_ADD,             APP_KEY_OEM_COMMA,       APP_KEY_SUBTRACT,
        APP_KEY_INVALID,        APP_KEY_DIVIDE,              APP_KEY_F1,           APP_KEY_F2,          APP_KEY_F3,          APP_KEY_F4,        APP_KEY_F5,           APP_KEY_F6,              APP_KEY_F7,              APP_KEY_F8,
        APP_KEY_F9,             APP_KEY_F10,                 APP_KEY_F11,          APP_KEY_F12,         APP_KEY_F13,         APP_KEY_F14,       APP_KEY_F15,          APP_KEY_F16,             APP_KEY_F17,             APP_KEY_F18,
        APP_KEY_F19,            APP_KEY_F20,                 APP_KEY_F21,          APP_KEY_F22,         APP_KEY_F23,         APP_KEY_F24,       APP_KEY_INVALID,      APP_KEY_INVALID,         APP_KEY_INVALID,         APP_KEY_INVALID,
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_NUMLOCK,     APP_KEY_SCROLL,    APP_KEY_RETURN,       APP_KEY_INVALID,         APP_KEY_INVALID,         APP_KEY_INVALID,
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_INVALID,     APP_KEY_INVALID,   APP_KEY_INVALID,      APP_KEY_INVALID,         APP_KEY_INVALID,         APP_KEY_INVALID,
        APP_KEY_LSHIFT,         APP_KEY_RSHIFT,              APP_KEY_LCONTROL,     APP_KEY_RCONTROL,    APP_KEY_LMENU,       APP_KEY_RMENU,     APP_KEY_BROWSER_BACK, APP_KEY_BROWSER_FORWARD, APP_KEY_BROWSER_REFRESH, APP_KEY_BROWSER_STOP,
        APP_KEY_BROWSER_SEARCH, APP_KEY_BROWSER_FAVORITES,   APP_KEY_BROWSER_HOME, APP_KEY_VOLUME_MUTE, APP_KEY_VOLUME_DOWN, APP_KEY_VOLUME_UP, APP_KEY_INVALID,      APP_KEY_INVALID,         APP_KEY_RETURN,          APP_KEY_INVALID,
        APP_KEY_LAUNCH_MAIL,    APP_KEY_LAUNCH_MEDIA_SELECT, APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_INVALID,     APP_KEY_INVALID,   APP_KEY_OEM_1,        APP_KEY_INVALID,         APP_KEY_OEM_COMMA,       APP_KEY_OEM_MINUS,
        APP_KEY_OEM_PERIOD,     APP_KEY_OEM_2,               APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_INVALID,     APP_KEY_INVALID,   APP_KEY_INVALID,      APP_KEY_INVALID,         APP_KEY_INVALID,         APP_KEY_INVALID,
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_INVALID,     APP_KEY_INVALID,   APP_KEY_INVALID,      APP_KEY_INVALID,         APP_KEY_INVALID,         APP_KEY_INVALID,
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_INVALID,     APP_KEY_INVALID,   APP_KEY_INVALID,      APP_KEY_INVALID,         APP_KEY_INVALID,         APP_KEY_OEM_4,
        APP_KEY_OEM_5,          APP_KEY_OEM_6,               APP_KEY_OEM_7,        APP_KEY_INVALID,     APP_KEY_LWIN,        APP_KEY_RMENU,     APP_KEY_INVALID,      APP_KEY_INVALID,         APP_KEY_INVALID,         APP_KEY_INVALID,
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_INVALID,     APP_KEY_INVALID,   APP_KEY_INVALID,      APP_KEY_INVALID,         APP_KEY_INVALID,         APP_KEY_INVALID,
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_HANGUL,      APP_KEY_INVALID,   APP_KEY_INVALID,      APP_KEY_CRSEL,           APP_KEY_EXSEL,           APP_KEY_INVALID,
        APP_KEY_INVALID,        APP_KEY_INVALID,             APP_KEY_INVALID,      APP_KEY_INVALID,     APP_KEY_CLEAR,       APP_KEY_INVALID, };

    if( scancode < 0 || scancode >= sizeof( map ) / sizeof( *map ) ) return APP_KEY_INVALID;
    return (app_key_t) map[ scancode ];
    }


app_state_t app_yield( app_t* app )
    {
    int evt[3];
    app_input_event_t input_event;
    app_key_t key;
    while (app_js_get_event( evt ))
    {
        switch (evt[0])
            {
            //         EVENT ID                ARG 1           ARG 2
            case 1: // WINDOW_SIZE_CHANGED     w               h
                if( evt[1] != app->gl.window_width || evt[2] != app->gl.window_height )
                    {
                    app_internal_opengl_resize( &app->gl, evt[1], evt[2] );
                    }
                break;
            case 2: // WINDOW_FOCUS            gained/lost
                app->has_focus = evt[1];
                break;
            case 3: // KEY                     down/up         scancode
                input_event.type = (evt[1] ? APP_INPUT_KEY_DOWN : APP_INPUT_KEY_UP);
                key = app_internal_scancode_to_appkey( app, evt[2] );
                if( key == APP_KEY_LCONTROL || key == APP_KEY_RCONTROL )
                    {
                    input_event.data.key = APP_KEY_CONTROL;
                    app_internal_add_input_event( app, &input_event );
                    }
                else if( key == APP_KEY_LSHIFT || key == APP_KEY_RSHIFT )
                    {
                    input_event.data.key = APP_KEY_SHIFT;
                    app_internal_add_input_event( app, &input_event );
                    }
                else if( key == APP_KEY_LMENU || key == APP_KEY_RMENU )
                    {
                    input_event.data.key = APP_KEY_MENU;
                    app_internal_add_input_event( app, &input_event );
                    }
                input_event.data.key = key;
                app_internal_add_input_event( app, &input_event );
                break;
            case 4: // CHAR                    charcode
                input_event.type = APP_INPUT_CHAR;
                input_event.data.char_code = (char) evt[1];
                app_internal_add_input_event( app, &input_event );
                break;
            case 5: // MOUSE_MOTION            x               y
                input_event.type = APP_INPUT_MOUSE_MOVE;
                app->pointer_x = input_event.data.mouse_pos.x = evt[1];
                app->pointer_y = input_event.data.mouse_pos.y = evt[2];
                app_internal_add_input_event( app, &input_event );
                break;
            case 6: // MOUSE_BUTTON            down/up         buttonnum
                input_event.type = (evt[1] ? APP_INPUT_KEY_DOWN : APP_INPUT_KEY_UP);
                if( evt[2] == 0 )
                    input_event.data.key = APP_KEY_LBUTTON;
                else if( evt[2] == 1 )
                    input_event.data.key = APP_KEY_RBUTTON;
                else if( evt[2] == 2 )
                    input_event.data.key = APP_KEY_MBUTTON;
                else if( evt[2] == 3 )
                    input_event.data.key = APP_KEY_XBUTTON1;
                else if( evt[2] == 4 )
                    input_event.data.key = APP_KEY_XBUTTON2;
                else
                    break;
                app_internal_add_input_event( app, &input_event );
                break;
            case 7: // MOUSE_WHEEL             wheel
                {
                float const microsoft_mouse_wheel_constant = 120.0f;
                float wheel_delta = ( (float) evt[1] ) / microsoft_mouse_wheel_constant;
                if( app->input_count > 0 && app->input_events[ app->input_count - 1 ].type == APP_INPUT_SCROLL_WHEEL )
                    {
                    app_input_event_t* event = &app->input_events[ app->input_count - 1 ];
                    event->data.wheel_delta += wheel_delta;
                    }
                else
                    {
                    input_event.type = APP_INPUT_SCROLL_WHEEL;
                    input_event.data.wheel_delta = wheel_delta;
                    app_internal_add_input_event( app, &input_event );
                    }
                } break;
        }
    }
    return APP_STATE_NORMAL;
    }


void app_cancel_exit( app_t* app ) { /* NOT IMPLEMENTED */ }
void app_title( app_t* app, char const* title ) { /* NOT IMPLEMENTED */ }
char const* app_cmdline( app_t* app ) { /* NOT IMPLEMENTED */ return NULL; }
char const* app_filename( app_t* app ) { /* NOT IMPLEMENTED */ return NULL; }
char const* app_userdata( app_t* app ) { /* NOT IMPLEMENTED */ return NULL; }
char const* app_appdata( app_t* app ) { /* NOT IMPLEMENTED */ return NULL; }


WAJIC_WITH_INIT(
(
    var start_time = Date.now();
),
APP_U32, app_js_get_ticks, (),
{
    return Date.now() - start_time;
})

APP_U64 app_time_count( app_t* app )
    {
    return (APP_U64)app_js_get_ticks()*1000;
    }


APP_U64 app_time_freq( app_t* app )
    {
    return (APP_U64)1000*1000;
    }


void app_log( app_t* app, app_log_level_t level, char const* message )
    {
    printf("[APP] [%d] %s\n", (int)level, message);
    }


void app_fatal_error( app_t* app, char const* message )
    {
    APP_FATAL_ERROR( app->fatalctx, message );
    }


void app_pointer( app_t* app, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y ) { /* NOT IMPLEMENTED */ }
void app_pointer_default( app_t* app, int* width, int* height, APP_U32* pixels_abgr, int* hotspot_x, int* hotspot_y ) { /* NOT IMPLEMENTED */ }
void app_pointer_pos( app_t* app, int x, int y ) { /* NOT IMPLEMENTED */ }


int app_pointer_x( app_t* app )
    {
    return app->pointer_x;
    }


int app_pointer_y( app_t* app )
    {
    return app->pointer_y;
    }


void app_pointer_limit( app_t* app, int x, int y, int width, int height ) { /* NOT IMPLEMENTED */ }
void app_pointer_limit_off( app_t* app ) { /* NOT IMPLEMENTED */ }

void app_interpolation( app_t* app, app_interpolation_t interpolation )
    {
    if( interpolation == app->interpolation ) return;
    app->interpolation = interpolation;

    app_internal_opengl_interpolation( &app->gl, interpolation );
    }


void app_screenmode( app_t* app, app_screenmode_t screenmode )
    {
    app_js_screenmode( (int) ( screenmode == APP_SCREENMODE_FULLSCREEN ) );
    }


void app_window_size( app_t* app, int width, int height )
    {
    // view size is controlled by the browser, we only control the display aspect ratio
    app_js_set_aspect_ratio( &width, &height );
    if( width != app->gl.window_width || height != app->gl.window_height )
        {
        app_internal_opengl_resize( &app->gl, width, height );
        }
    }


int app_window_width( app_t* app )
    {
    return app->gl.window_width;
    }


int app_window_height( app_t* app )
    {
    return app->gl.window_height;
    }


void app_window_pos( app_t* app, int x, int y ) { /* NOT IMPLEMENTED */ }


int app_window_x( app_t* app )
    {
    return 0;
    }


int app_window_y( app_t* app )
    {
    return 0;
    }


app_displays_t app_displays( app_t* app )
    {
    // Fixed display for web
    static app_display_t display;
    display.id[0] = '\0';
    display.x = 0;
    display.y = 0;
    display.width = 1920;
    display.height = 1080;
    app_displays_t displays;
    displays.count = 1;
    displays.displays = &display;
    return displays;
    }


WAJIC_WITH_INIT(
(
    var audio_ctx, audio_done = 0, audio_latency = 2048, audio_bufs = [], audio_bufidx = 0, audio_miss = 0;
    var start_audio = ()=>
    {
        if (audio_ctx.state == 'running') return 1;
        audio_done = audio_ctx.currentTime;
        audio_ctx.resume();
    };
    var set_start_audio_event = (name)=>document.addEventListener(name, start_audio, {once:true});
),
int, app_js_audio_needed, (bool has_focus),
{
    if (!audio_ctx)
    {
        if (audio_ctx === false) return 0;
        try { (audio_ctx = new (alias(window,"","",'AudioContext'))()).createBuffer(1,1,44100).getChannelData(0); } catch (e) { }
        if (!audio_ctx) { audio_ctx = false; WA.print('Warning: WebAudio not supported\n'); return 0; }
        for (var i = 0; i != 10; i++) audio_bufs[i] = audio_ctx.createBuffer(2, 2048, 44100);
        if (!start_audio()) { set_start_audio_event('click'); set_start_audio_event('touchstart'); set_start_audio_event('keydown'); }
    }
    if (!start_audio() && !start_audio()) return 0;
    var ct = audio_ctx.currentTime;
    if (audio_done < ct)
    {
        if (has_focus && (audio_miss += 2) > 7)
        {
            audio_latency += 2048;
            audio_miss = 0;
        }
        audio_done = ct;
    }
    else if (audio_miss > 1) audio_miss--;
    return ((ct - audio_done) * 44100 + .5 + audio_latency * (has_focus ? 1 : 2) + 2047)>>11;
})

WAJIC(int, app_js_audio_push, (APP_S16* sample_pairs, int volume),
{
    sample_pairs = new Int16Array(MU8.buffer).subarray(sample_pairs>>1);
    var buf = audio_bufs[audio_bufidx = ((audio_bufidx + 1) % 10)];
    var left = buf.getChannelData(0), right = buf.getChannelData(1);
    var f = (1 / 32768) * (volume / 255);
    for (var i = 0; i != 2048; i++)
    {
        left[i] = sample_pairs[i*2] * f;
        right[i] = sample_pairs[i*2+1] * f;
    }
    var source = audio_ctx.createBufferSource();
    source.connect(audio_ctx.destination);
    source.buffer = buf;
    source[source.start ? 'start' : 'noteOn'](0.005+audio_done);
    audio_done += 2048/44100;
})


void app_present( app_t* app, APP_U32 const* pixels_xbgr, int width, int height, APP_U32 mod_xbgr, APP_U32 border_xbgr )
    {
    if( pixels_xbgr ) app_internal_opengl_present( &app->gl, pixels_xbgr, width, height, mod_xbgr, border_xbgr );
    for( int needed = app_js_audio_needed(app->has_focus); app->sound_callback && needed--;)
        {
        app->sound_callback(app->sound_buffer, 2048, app->sound_user_data);
        app_js_audio_push(app->sound_buffer, app->volume);
        }
    if( app->has_focus )
        WaCoroWaitAnimFrame();
    else
        WaCoroSleep(50);
    }


void app_sound( app_t* app, int sample_pairs_count, void (*sound_callback)( APP_S16* sample_pairs, int sample_pairs_count, void* user_data ), void* user_data )
    {
    app->sound_callback = sound_callback;
    app->sound_user_data = user_data;
    }


void app_sound_volume( app_t* app, float volume )
    {
    int v = (int) ( volume * 256.0f );
    app->volume = v < 0 ? 0 : v > 256 ? 256 : v;
    }


app_input_t app_input( app_t* app )
    {
    app_input_t input;
    input.events = app->input_events;
    input.count = app->input_count;
    app->input_count = 0;
    return input;
    }


void app_coordinates_window_to_bitmap( app_t* app, int width, int height, int* x, int* y )
    {
    if( width == 0 || height == 0 ) return;
    if( app->interpolation == APP_INTERPOLATION_LINEAR )
        {
        float hscale = app->gl.window_width / (float) width;
        float vscale = app->gl.window_height / (float) height;
        float pixel_scale = hscale < vscale ? hscale : vscale;
        if( pixel_scale > 0.0f )
            {
            float hborder = ( app->gl.window_width - pixel_scale * width ) / 2.0f;
            float vborder = ( app->gl.window_height - pixel_scale * height ) / 2.0f;
            *x -= (int)( hborder );
            *y -= (int)( vborder );
            *x = (int)( *x / pixel_scale );
            *y = (int)( *y / pixel_scale );
            }
        else
            {
            *x = 0;
            *y = 0;
            }
        }
    else
        {
        int hscale = app->gl.window_width / width;
        int vscale = app->gl.window_height / height;
        int pixel_scale = pixel_scale = hscale < vscale ? hscale : vscale;
        pixel_scale = pixel_scale < 1 ? 1 : pixel_scale;
        int hborder = ( app->gl.window_width - pixel_scale * width ) / 2;
        int vborder = ( app->gl.window_height - pixel_scale * height ) / 2;
        *x -= (int)( hborder );
        *y -= (int)( vborder );
        *x = (int)( *x / pixel_scale );
        *y = (int)( *y / pixel_scale );
        }
    }


void app_coordinates_bitmap_to_window( app_t* app, int width, int height, int* x, int* y )
    {
    if( app->interpolation == APP_INTERPOLATION_LINEAR )
        {
        float hscale = app->gl.window_width / (float) width;
        float vscale = app->gl.window_height / (float) height;
        float pixel_scale = hscale < vscale ? hscale : vscale;
        if( pixel_scale > 0.0f )
            {
            float hborder = ( app->gl.window_width - pixel_scale * width ) / 2.0f;
            float vborder = ( app->gl.window_height - pixel_scale * height ) / 2.0f;
            *x = (int)( *x * pixel_scale );
            *y = (int)( *y * pixel_scale );
            *x += (int)( hborder );
            *y += (int)( vborder );
            }
        else
            {
            *x = 0;
            *y = 0;
            }
        }
    else
        {
        int hscale = app->gl.window_width / width;
        int vscale = app->gl.window_height / height;
        int pixel_scale = pixel_scale = hscale < vscale ? hscale : vscale;
        pixel_scale = pixel_scale < 1 ? 1 : pixel_scale;
        int hborder = ( app->gl.window_width - pixel_scale * width ) / 2;
        int vborder = ( app->gl.window_height - pixel_scale * height ) / 2;
        *x = (int)( *x * pixel_scale );
        *y = (int)( *y * pixel_scale );
        *x += (int)( hborder );
        *y += (int)( vborder );
        }
    }


#else
    #error Undefined platform. Define APP_WINDOWS, APP_SDL, APP_WASM or APP_NULL.
#endif


#endif /* APP_IMPLEMENTATION */

/*
revision history:
    0.6     app_init, app_term, app_t_size, removed dynamic allocation
    0.4     pointer x/y, callback for sound, modifier keys fix, gl binding fix, cursor fix
    0.3     added API documentation
    0.2     first publicly released version
*/

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2016 Mattias Gustavsson

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
