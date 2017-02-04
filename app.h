/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

app.h - v0.2 - Small cross-platform base framework for graphical apps.

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

int app_run( int (*app_proc)( app_t*, void* ), void* user_data, void* memctx, void* logctx, void* fatalctx );

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

void app_sound_buffer_size( app_t* app, int sample_pairs_count );
int app_sound_position( app_t* app );
void app_sound_write( app_t* app, int sample_pairs_offset, int sample_pairs_count, APP_S16 const* sample_pairs );
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
	APP_KEY_OEM_CLEAR, } app_key_t;

typedef enum app_input_type_t { APP_INPUT_KEY_DOWN, APP_INPUT_KEY_UP, APP_INPUT_DOUBLE_CLICK, APP_INPUT_CHAR, 
	APP_INPUT_MOUSE_MOVE, APP_INPUT_MOUSE_DELTA, APP_INPUT_SCROLL_WHEEL, APP_INPUT_TABLET, } app_input_type_t;

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

Example:

	#define  APP_IMPLEMENTATION
	#define  APP_WINDOWS
	#include "app.h"

	#include <stdlib.h> // for rand and __argc/__argv
	#include <string.h> // for memset

	int app_proc( app_t* app, void* )
		{
		APP_U32 canvas[ 320 * 200 ]; // a place for us to draw stuff
		memset( canvas, 0xC0, sizeof( canvas ) ); // clear to grey
		app_screenmode( app, APP_SCREENMODE_WINDOW );

		// keep running until the user close the window
		while( app_yield( app ) != APP_STATE_EXIT_REQUESTED )
			{
			// plot a random pixel on the canvas
			int x = rand() % 320;
			int y = rand() % 200;
			int color = rand() | ( rand() << 16 );
			canvas[ x + y * 320 ] = color;

			// display the canvas
			app_present( app, canvas, 320, 200, 0xffffffff, 0xff000000 );
			}
		return 0;
		}

	int main( int argc, char** argv )
		{
		(void) argc, argv;
		return app_run( app_proc, 0, 0, 0, 0 );
		}

	// pass-through so the program will build with either /SUBSYSTEM:WINDOWS or /SUBSYSTEN:CONSOLE
	extern "C" int __stdcall WinMain( struct HINSTANCE__*, struct HINSTANCE__*, char*, int ) { return main( __argc, __argv ); }

*/


/*
----------------------
	IMPLEMENTATION
----------------------
*/
#ifdef APP_IMPLEMENTATION
#undef APP_IMPLEMENTATION


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    OPENGL CODE - Shared between platform implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef APP_NULL

#if defined( APP_WINDOWS )
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
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
#else
    #error Undefined platform. Define APP_WINDOWS or APP_NULL.
#endif

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
#define APP_GL_TEXTURE_MIN_FILTER 0x2801
#define APP_GL_TEXTURE_MAG_FILTER 0x2800
#define APP_GL_NEAREST 0x2600
#define APP_GL_LINEAR 0x2601
#define APP_GL_STATIC_DRAW 0x88e4
#define APP_GL_RGBA 0x1908
#define APP_GL_UNSIGNED_BYTE 0x1401
#define APP_GL_COLOR_BUFFER_BIT 0x00004000
#define APP_GL_TRIANGLE_FAN 0x0006

struct app_internal_opengl_t
    {
    APP_GLuint (APP_GLCALLTYPE* glCreateShader) (APP_GLenum type);
    void (APP_GLCALLTYPE* glShaderSource) (APP_GLuint shader, APP_GLsizei count, APP_GLchar const* const* string, APP_GLint const* length);
    void (APP_GLCALLTYPE* glCompileShader) (APP_GLuint shader);
    void (APP_GLCALLTYPE* glGetShaderiv) (APP_GLuint shader, APP_GLenum pname, APP_GLint *params);
    APP_GLuint (APP_GLCALLTYPE* glCreateProgram) (void);
    void (APP_GLCALLTYPE* glAttachShader) (APP_GLuint program, APP_GLuint shader);
    void (APP_GLCALLTYPE* glBindAttribLocation) (APP_GLuint program, APP_GLuint index, APP_GLchar const* name);
    void (APP_GLCALLTYPE* glLinkProgram) (APP_GLuint program);
    void (APP_GLCALLTYPE* glGetProgramiv) (APP_GLuint program, APP_GLenum pname, APP_GLint *params);
    void (APP_GLCALLTYPE* glGenBuffers) (APP_GLsizei n, APP_GLuint *buffers);
    void (APP_GLCALLTYPE* glBindBuffer) (APP_GLenum target, APP_GLuint buffer);
    void (APP_GLCALLTYPE* glEnableVertexAttribArray) (APP_GLuint index);
    void (APP_GLCALLTYPE* glVertexAttribPointer) (APP_GLuint index, APP_GLint size, APP_GLenum type, APP_GLboolean normalized, APP_GLsizei stride, void const* pointer);
    void (APP_GLCALLTYPE* glGenTextures) (APP_GLsizei n, APP_GLuint* textures);
    void (APP_GLCALLTYPE* glEnable) (APP_GLenum cap);
    void (APP_GLCALLTYPE* glActiveTexture) (APP_GLenum texture);
    void (APP_GLCALLTYPE* glBindTexture) (APP_GLenum target, APP_GLuint texture);
    void (APP_GLCALLTYPE* glTexParameteri) (APP_GLenum target, APP_GLenum pname, APP_GLint param);
    void (APP_GLCALLTYPE* glDeleteBuffers) (APP_GLsizei n, APP_GLuint const* buffers);
    void (APP_GLCALLTYPE* glDeleteTextures) (APP_GLsizei n, APP_GLuint const* textures);
    void (APP_GLCALLTYPE* glBufferData) (APP_GLenum target, APP_GLsizeiptr size, void const *data, APP_GLenum usage);
    void (APP_GLCALLTYPE* glUseProgram) (APP_GLuint program);
    void (APP_GLCALLTYPE* glUniform1i) (APP_GLint location, APP_GLint v0);
    void (APP_GLCALLTYPE* glUniform3f) (APP_GLint location, APP_GLfloat v0, APP_GLfloat v1, APP_GLfloat v2);
    APP_GLint (APP_GLCALLTYPE* glGetUniformLocation) (APP_GLuint program, APP_GLchar const* name);
    void (APP_GLCALLTYPE* glTexImage2D) (APP_GLenum target, APP_GLint level, APP_GLint internalformat, APP_GLsizei width, APP_GLsizei height, APP_GLint border, APP_GLenum format, APP_GLenum type, void const* pixels);
    void (APP_GLCALLTYPE* glClearColor) (APP_GLfloat red, APP_GLfloat green, APP_GLfloat blue, APP_GLfloat alpha);
    void (APP_GLCALLTYPE* glClear) (APP_GLbitfield mask);
    void (APP_GLCALLTYPE* glDrawArrays) (APP_GLenum mode, APP_GLint first, APP_GLsizei count);
    void (APP_GLCALLTYPE* glViewport) (APP_GLint x, APP_GLint y, APP_GLsizei width, APP_GLsizei height);
    void (APP_GLCALLTYPE* glDeleteShader) (APP_GLuint shader);
    void (APP_GLCALLTYPE* glDeleteProgram) (APP_GLuint program);
    #ifdef APP_REPORT_SHADER_ERRORS
        void (APP_GLCALLTYPE* glGetShaderInfoLog) (APP_GLuint shader, APP_GLsizei bufSize, APP_GLsizei *length, APP_GLchar *infoLog);
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


    #define STR( x ) #x

	char const* vs_source = 
    STR(
		attribute vec4 pos;
		varying vec2 uv;	

		void main( void )
			{
			gl_Position = vec4( pos.xy, 0.0, 1.0 );
            uv = pos.zw;
			}
	);

	char const* fs_source = 
	STR (
		varying vec2 uv;

		uniform sampler2D texture;
		uniform vec3 modulate;

        void main(void)
			{
			gl_FragColor= texture2D( texture, uv ) * vec4( modulate, 1.0 );
			}			
    );

    #undef STR
    
	#ifdef APP_REPORT_SHADER_ERRORS
		char error_message[ 1024 ]; 
	#endif

    APP_GLuint vs = gl->glCreateShader( APP_GL_VERTEX_SHADER );
	gl->glShaderSource( vs, 1, (char const**) &vs_source, NULL );
	gl->glCompileShader( vs );
	APP_GLint vs_compiled;
	gl->glGetShaderiv( vs, APP_GL_COMPILE_STATUS, &vs_compiled );
	if( !vs_compiled )
		{
		#ifdef APP_REPORT_SHADER_ERRORS		
			char const* prefix = "Vertex Shader Error: ";
			strcpy( error_message, prefix );
			int len = 0, written = 0;
			gl->glGetShaderiv( vs, APP_GL_INFO_LOG_LENGTH, &len );
			gl->glGetShaderInfoLog( vs, (APP_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written, 
				error_message + strlen( prefix ) );		
			app_fatal_error( app, error_message );
		#endif
		return 0;
		}
	
	APP_GLuint fs = gl->glCreateShader( APP_GL_FRAGMENT_SHADER );
	gl->glShaderSource( fs, 1, (char const**) &fs_source, NULL );
	gl->glCompileShader( fs );
	APP_GLint fs_compiled;
	gl->glGetShaderiv( fs, APP_GL_COMPILE_STATUS, &fs_compiled );
	if( !fs_compiled )
		{
		#ifdef APP_REPORT_SHADER_ERRORS		
			char const* prefix = "Fragment Shader Error: ";
			strcpy( error_message, prefix );
			int len = 0, written = 0;
			gl->glGetShaderiv( vs, APP_GL_INFO_LOG_LENGTH, &len );
			gl->glGetShaderInfoLog( fs, (APP_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written, 
				error_message + strlen( prefix ) );		
			app_fatal_error( app, error_message );
		#endif
		return 0;
		}


	APP_GLuint prg = gl->glCreateProgram();
	gl->glAttachShader( prg, fs );
	gl->glAttachShader( prg, vs );
	gl->glBindAttribLocation( prg, 0, "pos" );
	gl->glLinkProgram( prg );

	APP_GLint linked;
	gl->glGetProgramiv( prg, APP_GL_LINK_STATUS, &linked );
	if( !linked )
		{
		#ifdef APP_REPORT_SHADER_ERRORS
			char const* prefix = "Shader Link Error: ";
			strcpy( error_message, prefix );
			int len = 0, written = 0;
			gl->glGetShaderiv( vs, APP_GL_INFO_LOG_LENGTH, &len );
			gl->glGetShaderInfoLog( prg, (APP_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written, 
				error_message + strlen( prefix ) );		
			app_fatal_error( app, error_message );
		#endif
		return 0;
		}

	gl->shader = prg;
    gl->glDeleteShader( fs );
    gl->glDeleteShader( vs );

	gl->glGenBuffers( 1, &gl->vertexbuffer );
	gl->glBindBuffer( APP_GL_ARRAY_BUFFER, gl->vertexbuffer );
	gl->glEnableVertexAttribArray( 0 );
	gl->glVertexAttribPointer( 0, 4, APP_GL_FLOAT, APP_GL_FALSE, 4 * sizeof( APP_GLfloat ), 0 );

    gl->glGenTextures( 1, &gl->texture ); 
    gl->glEnable( APP_GL_TEXTURE_2D ); 
    gl->glActiveTexture( APP_GL_TEXTURE0 );
    gl->glBindTexture( APP_GL_TEXTURE_2D, gl->texture );
    gl->glTexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MIN_FILTER, APP_GL_NEAREST );
    gl->glTexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MAG_FILTER, APP_GL_NEAREST );

    return 1;
    }


static int app_internal_opengl_term( struct app_internal_opengl_t* gl )
    {
    gl->glDeleteProgram( gl->shader );
    gl->glDeleteBuffers( 1, &gl->vertexbuffer); 
    gl->glDeleteTextures( 1, &gl->texture ); 
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

    gl->glBindBuffer( APP_GL_ARRAY_BUFFER, gl->vertexbuffer );
	gl->glBufferData( APP_GL_ARRAY_BUFFER, 4 * 4 * sizeof( APP_GLfloat ), vertices, APP_GL_STATIC_DRAW );
	gl->glVertexAttribPointer( 0, 4, APP_GL_FLOAT, APP_GL_FALSE, 4 * sizeof( APP_GLfloat ), 0 );

	float mod_r = ( ( mod_xbgr >> 16 ) & 0xff ) / 255.0f;
	float mod_g = ( ( mod_xbgr >> 8  ) & 0xff ) / 255.0f;
	float mod_b = ( ( mod_xbgr       ) & 0xff ) / 255.0f;

	gl->glUseProgram( gl->shader );
	gl->glUniform1i( gl->glGetUniformLocation( gl->shader, "texture" ), 0 );
	gl->glUniform3f( gl->glGetUniformLocation( gl->shader, "modulate" ), mod_r, mod_g, mod_b );

	gl->glActiveTexture( APP_GL_TEXTURE0 );
    gl->glBindTexture( APP_GL_TEXTURE_2D, gl->texture );
    gl->glTexImage2D( APP_GL_TEXTURE_2D, 0, APP_GL_RGBA, width, height, 0, APP_GL_RGBA, APP_GL_UNSIGNED_BYTE, pixels_xbgr ); 
    
    if( gl->interpolation == APP_INTERPOLATION_LINEAR )
        {
        gl->glTexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MIN_FILTER, APP_GL_LINEAR );
        gl->glTexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MAG_FILTER, APP_GL_LINEAR );
        }
    else
        {
        gl->glTexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MIN_FILTER, APP_GL_NEAREST );
        gl->glTexParameteri( APP_GL_TEXTURE_2D, APP_GL_TEXTURE_MAG_FILTER, APP_GL_NEAREST );
        }

	float r = ( ( border_xbgr >> 16 ) & 0xff ) / 255.0f;
	float g = ( ( border_xbgr >> 8  ) & 0xff ) / 255.0f;
	float b = ( ( border_xbgr       ) & 0xff ) / 255.0f;
	gl->glClearColor( r, g, b, 1.0f );
	gl->glClear( APP_GL_COLOR_BUFFER_BIT );
	gl->glDrawArrays( APP_GL_TRIANGLE_FAN, 0, 4 );

    return 1;
    }


static void app_internal_opengl_resize( struct app_internal_opengl_t* gl, int width, int height )
    {
    gl->glViewport( 0, 0, width, height );    
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


struct app_t { };
int app_run( int (*app_proc)( app_t*, void* ), void* user_data, void* memctx, void* logctx, void* fatalctx ) 
    { app_t app; return app_proc( &app, user_data ); }
app_state_t app_yield( app_t* app ) { return APP_STATE_EXIT_REQUESTED; }
void app_cancel_exit( app_t* app ) { }
void app_title( app_t* app, char const* title ) { }
char const* app_cmdline( app_t* app ) { return 0; }
char const* app_filename( app_t* app ) { return 0; }
char const* app_userdata( app_t* app ) { return 0; }
char const* app_appdata( app_t* app ) { return 0; }
APP_U64 app_time_count( app_t* app ) { return 0; }
APP_U64 app_time_freq( app_t* app ) { return 0; }
void app_log( app_t* app, app_log_level_t level, char const* message ) { }
void app_fatal_error( app_t* app, char const* message ) { }
void app_pointer( app_t* app, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y ) { }
void app_pointer_default( app_t* app, int* width, int* height, APP_U32* pixels_abgr, int* hotspot_x, int* hotspot_y ) { }
void app_pointer_pos( app_t* app, int x, int y ) { }
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
app_displays_t app_displays( app_t* app ) { app_displays_t x = { 0 }; return x; }
void app_present( app_t* app, APP_U32 const* pixels_xbgr, int width, int height, APP_U32 mod_xbgr, APP_U32 border_xbgr ) { }
void app_sound_buffer_size( app_t* app, int sample_pairs_count ) { }
int app_sound_position( app_t* app ) { return 0; }
void app_sound_write( app_t* app, int sample_pairs_offset, int sample_pairs_count, APP_S16 const* sample_pairs ) { }
void app_sound_volume( app_t* app, float volume ) { }
app_input_t app_input( app_t* app ) { app_input_t x = { 0 }; return x; }
void app_coordinates_window_to_bitmap( app_t* app, int width, int height, int* x, int* y ) { }
void app_coordinates_bitmap_to_window( app_t* app, int width, int height, int* x, int* y ) { }



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    WINDOWS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#elif defined( APP_WINDOWS )

#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // requires Windows XP minimum
// 0x0400=Windows NT 4.0, 0x0500=Windows 2000, 0x0501=Windows XP, 0x0502=Windows Server 2003, 0x0600=Windows Vista, 
// 0x0601=Windows 7, 0x0602=Windows 8, 0x0603=Windows 8.1, 0x0A00=Windows 10, 
#define _WINSOCKAPI_
#pragma warning( push )
#pragma warning( disable: 4668 )
#pragma warning( disable: 4255 )
#include <windows.h>
#pragma warning( pop )
#pragma warning( push )
#pragma warning( disable: 4917 )
#include <shlobj.h>
#pragma warning( pop )
#pragma comment( lib, "user32.lib" )
#pragma comment( lib, "gdi32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "shell32.lib" )
#include <dsound.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#ifndef APP_MALLOC
	#include <malloc.h>
	#if defined(__cplusplus)
		#define APP_MALLOC( ctx, size ) ( ::malloc( size ) )
		#define APP_FREE( ctx, ptr ) ( ::free( ptr ) )
	#else
		#define APP_MALLOC( ctx, size ) ( malloc( size ) )
		#define APP_FREE( ctx, ptr ) ( free( ptr ) )
	#endif
#endif

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

struct app_t
    {
	void* memctx;
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

	HMODULE dsound_dll;
    IDirectSound* dsound;
    IDirectSoundBuffer* dsoundbuf;	
    int sample_pairs_count;
    int sound_level;

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
            break;
		case WM_SYSKEYDOWN: 
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam ); 
            app_internal_add_input_event( app, &input_event ); 
            break;
		case WM_KEYDOWN: 
            input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam ); 
            app_internal_add_input_event( app, &input_event ); 
            break;
		case WM_SYSKEYUP: 
            input_event.type = APP_INPUT_KEY_UP; input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam ); 
            app_internal_add_input_event( app, &input_event ); 
            break;
		case WM_KEYUP: 
            input_event.type = APP_INPUT_KEY_UP; input_event.data.key = app_internal_vkcode_to_appkey( app, (int) wparam ); 
            app_internal_add_input_event( app, &input_event ); 
            break;        

		case WM_MOUSEWHEEL: input_event.type = APP_INPUT_KEY_DOWN; input_event.data.key = APP_KEY_LBUTTON;
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
			RAWINPUT raw;
			UINT size = sizeof( raw );
			GetRawInputData( (HRAWINPUT) lparam, RID_INPUT, &raw, &size, sizeof( RAWINPUTHEADER ) );    
			if( raw.header.dwType == RIM_TYPEMOUSE ) 
				{
				float dx = (float) raw.data.mouse.lLastX;
				float dy = (float) raw.data.mouse.lLastY;

				float const microsoft_mouse_dpi_constant = 400.0f; // Apparently, most mice are meant to run at 400 DPI. This might be wrong.
				dx /= microsoft_mouse_dpi_constant;
				dy /= microsoft_mouse_dpi_constant;

				if( app->input_count > 0 && app->input_events[ app->input_count - 1 ].type == APP_INPUT_MOUSE_DELTA )
					{
					app_input_event_t* event = &app->input_events[ app->input_count - 1 ];
					event->data.mouse_delta.x += dx;					
					event->data.mouse_delta.y += dy;					
                    }
				else
					{
					input_event.type = APP_INPUT_SCROLL_WHEEL;
					input_event.data.mouse_delta.x = dx;					
					input_event.data.mouse_delta.y = dy;					
    				app_internal_add_input_event( app, &input_event ); 
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
        mi.cbSize = sizeof( mi );
        BOOL res = GetMonitorInfoA( hmonitor, &mi );
        if( res && strlen( mi.szDevice ) >= sizeof( display->id ) ) res = FALSE;
        strcpy( display->id, res ? mi.szDevice : "" ) ;
    #else
        MONITORINFOEXA mi;
        mi.cbSize = sizeof( mi );
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

int app_run( int (*app_proc)( app_t*, void* ), void* user_data, void* memctx, void* logctx, void* fatalctx )
    {
    int result = 0xff;

    // Init app instance
    app_t* app = (app_t*) APP_MALLOC( memctx, sizeof( app_t ) );
    memset( app, 0, sizeof( *app ) );
    app->memctx = memctx;
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
	TIMECAPS tc;
	if( timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == TIMERR_NOERROR ) 
		timeBeginPeriod( tc.wPeriodMin );

	// Get instance handle
	app->hinstance = GetModuleHandle( NULL );

    // Retrieve the path of our executable
    GetModuleFileNameA( 0, app->exe_path, sizeof( app->exe_path ) );

    // Retrieve user data path
    SHGetFolderPathA( NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, app->userdata_path ); 

    // Retrieve app data path
    SHGetFolderPathA( NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, app->appdata_path ); 

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
	AdjustWindowRect( &winrect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE );

	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW,  
        (WNDPROC) app_internal_wndproc, 0, 0, 0, 0, 0, 0, 0, TEXT( "app_wc" ), 0 };
	wc.hInstance = app->hinstance; wc.hIcon = app->icon; wc.hCursor = app->current_pointer; 
	wc.hbrBackground = (HBRUSH) GetStockObject( BLACK_BRUSH ); wc.hIconSm = app->icon;
	RegisterClassEx( &wc );
	app->hwnd = CreateWindowEx( 0, wc.lpszClassName, 0, WS_OVERLAPPEDWINDOW, app->windowed_x, app->windowed_y, 
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

    // Windows specific OpenGL initialization
	app->gl_dll = LoadLibraryA( "opengl32.dll" );
    if( !app->gl_dll ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to load opengl32.dll" ); goto init_failed; }
	app->wglGetProcAddress = (PROC(APP_GLCALLTYPE*)(LPCSTR)) (uintptr_t) GetProcAddress( app->gl_dll, "wglGetProcAddress" );
    if( !app->gl_dll ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to find wglGetProcAddress" ); goto init_failed; }
	app->wglCreateContext = (HGLRC(APP_GLCALLTYPE*)(HDC)) (uintptr_t) GetProcAddress( app->gl_dll, "wglCreateContext" );
    if( !app->gl_dll ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to find wglCreateContext" ); goto init_failed; }
	app->wglDeleteContext = (BOOL(APP_GLCALLTYPE*)(HGLRC)) (uintptr_t) GetProcAddress( app->gl_dll, "wglDeleteContext" );
    if( !app->gl_dll ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to find wglDeleteContext" ); goto init_failed; }
	app->wglMakeCurrent = (BOOL(APP_GLCALLTYPE*)(HDC, HGLRC)) (uintptr_t) GetProcAddress( app->gl_dll, "wglMakeCurrent" );
    if( !app->gl_dll ) { app_log( app, APP_LOG_LEVEL_ERROR, "Failed to find wglMakeCurrent" ); goto init_failed; }
    
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
    app->gl.glCreateShader = ( APP_GLuint (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) GetProcAddress( app->gl_dll, "glCreateShader" );
    app->gl.glShaderSource = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLsizei, APP_GLchar const* const*, APP_GLint const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glShaderSource" );
    app->gl.glCompileShader = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glCompileShader" );
    app->gl.glGetShaderiv = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLenum, APP_GLint*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGetShaderiv" );
    app->gl.glCreateProgram = ( APP_GLuint (APP_GLCALLTYPE*) (void) ) (uintptr_t) GetProcAddress( app->gl_dll, "glCreateProgram" );
    app->gl.glAttachShader = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glAttachShader" );
    app->gl.glBindAttribLocation = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLuint, APP_GLchar const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glBindAttribLocation" );
    app->gl.glLinkProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glLinkProgram" );
    app->gl.glGetProgramiv = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLenum, APP_GLint*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGetProgramiv" );
    app->gl.glGenBuffers = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGenBuffers" );
    app->gl.glBindBuffer = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glBindBuffer" );
    app->gl.glEnableVertexAttribArray = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glEnableVertexAttribArray" );
    app->gl.glVertexAttribPointer = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLint, APP_GLenum, APP_GLboolean, APP_GLsizei, void const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glVertexAttribPointer" );
    app->gl.glGenTextures = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGenTextures" );
    app->gl.glEnable = ( void (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) GetProcAddress( app->gl_dll, "glEnable" );
    app->gl.glActiveTexture = ( void (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) GetProcAddress( app->gl_dll, "glActiveTexture" );
    app->gl.glBindTexture = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glBindTexture" );
    app->gl.glTexParameteri = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLenum, APP_GLint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glTexParameteri" );
    app->gl.glDeleteBuffers = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDeleteBuffers" );
    app->gl.glDeleteTextures = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDeleteTextures" );
    app->gl.glBufferData = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLsizeiptr, void const *, APP_GLenum) ) (uintptr_t) GetProcAddress( app->gl_dll, "glBufferData" );
    app->gl.glUseProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glUseProgram" );
    app->gl.glUniform1i = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glUniform1i" );
    app->gl.glUniform3f = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLfloat, APP_GLfloat, APP_GLfloat) ) (uintptr_t) GetProcAddress( app->gl_dll, "glUniform3f" );
    app->gl.glGetUniformLocation = ( APP_GLint (APP_GLCALLTYPE*) (APP_GLuint, APP_GLchar const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGetUniformLocation" );
    app->gl.glTexImage2D = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLint, APP_GLint, APP_GLsizei, APP_GLsizei, APP_GLint, APP_GLenum, APP_GLenum, void const*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glTexImage2D" );
    app->gl.glClearColor = ( void (APP_GLCALLTYPE*) (APP_GLfloat, APP_GLfloat, APP_GLfloat, APP_GLfloat) ) (uintptr_t) GetProcAddress( app->gl_dll, "glClearColor" );
    app->gl.glClear = ( void (APP_GLCALLTYPE*) (APP_GLbitfield) ) (uintptr_t) GetProcAddress( app->gl_dll, "glClear" );
    app->gl.glDrawArrays = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLint, APP_GLsizei) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDrawArrays" );
    app->gl.glViewport = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLint, APP_GLsizei, APP_GLsizei) ) (uintptr_t) GetProcAddress( app->gl_dll, "glViewport" );
    app->gl.glDeleteShader = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDeleteShader" );
    app->gl.glDeleteProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) GetProcAddress( app->gl_dll, "glDeleteProgram" );
    #ifdef APP_REPORT_SHADER_ERRORS
        app->gl.glGetShaderInfoLog = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLsizei, APP_GLsizei*, APP_GLchar*) ) (uintptr_t) GetProcAddress( app->gl_dll, "glGetShaderInfoLog" );
    #endif

    // Any opengl functions which didn't bind, try binding them using wglGetProcAddrss
    if( !app->gl.glCreateShader ) app->gl.glCreateShader = ( APP_GLuint (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) app->wglGetProcAddress( "glCreateShader" );
    if( !app->gl.glShaderSource ) app->gl.glShaderSource = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLsizei, APP_GLchar const* const*, APP_GLint const*) ) (uintptr_t) app->wglGetProcAddress( "glShaderSource" );
    if( !app->gl.glCompileShader ) app->gl.glCompileShader = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glCompileShader" );
    if( !app->gl.glGetShaderiv ) app->gl.glGetShaderiv = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLenum, APP_GLint*) ) (uintptr_t) app->wglGetProcAddress( "glGetShaderiv" );
    if( !app->gl.glCreateProgram ) app->gl.glCreateProgram = ( APP_GLuint (APP_GLCALLTYPE*) (void) ) (uintptr_t) app->wglGetProcAddress( "glCreateProgram" );
    if( !app->gl.glAttachShader ) app->gl.glAttachShader = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glAttachShader" );
    if( !app->gl.glBindAttribLocation ) app->gl.glBindAttribLocation = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLuint, APP_GLchar const*) ) (uintptr_t) app->wglGetProcAddress( "glBindAttribLocation" );
    if( !app->gl.glLinkProgram ) app->gl.glLinkProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glLinkProgram" );
    if( !app->gl.glGetProgramiv ) app->gl.glGetProgramiv = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLenum, APP_GLint*) ) (uintptr_t) app->wglGetProcAddress( "glGetProgramiv" );
    if( !app->gl.glGenBuffers ) app->gl.glGenBuffers = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint*) ) (uintptr_t) app->wglGetProcAddress( "glGenBuffers" );
    if( !app->gl.glBindBuffer ) app->gl.glBindBuffer = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glBindBuffer" );
    if( !app->gl.glEnableVertexAttribArray ) app->gl.glEnableVertexAttribArray = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glEnableVertexAttribArray" );
    if( !app->gl.glVertexAttribPointer ) app->gl.glVertexAttribPointer = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLint, APP_GLenum, APP_GLboolean, APP_GLsizei, void const*) ) (uintptr_t) app->wglGetProcAddress( "glVertexAttribPointer" );
    if( !app->gl.glGenTextures ) app->gl.glGenTextures = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint*) ) (uintptr_t) app->wglGetProcAddress( "glGenTextures" );
    if( !app->gl.glEnable ) app->gl.glEnable = ( void (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) app->wglGetProcAddress( "glEnable" );
    if( !app->gl.glActiveTexture ) app->gl.glActiveTexture = ( void (APP_GLCALLTYPE*) (APP_GLenum) ) (uintptr_t) app->wglGetProcAddress( "glActiveTexture" );
    if( !app->gl.glBindTexture ) app->gl.glBindTexture = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glBindTexture" );
    if( !app->gl.glTexParameteri ) app->gl.glTexParameteri = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLenum, APP_GLint) ) (uintptr_t) app->wglGetProcAddress( "glTexParameteri" );
    if( !app->gl.glDeleteBuffers ) app->gl.glDeleteBuffers = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint const*) ) (uintptr_t) app->wglGetProcAddress( "glDeleteBuffers" );
    if( !app->gl.glDeleteTextures ) app->gl.glDeleteTextures = ( void (APP_GLCALLTYPE*) (APP_GLsizei, APP_GLuint const*) ) (uintptr_t) app->wglGetProcAddress( "glDeleteTextures" );
    if( !app->gl.glBufferData ) app->gl.glBufferData = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLsizeiptr, void const *, APP_GLenum) ) (uintptr_t) app->wglGetProcAddress( "glBufferData" );
    if( !app->gl.glUseProgram ) app->gl.glUseProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glUseProgram" );
    if( !app->gl.glUniform1i ) app->gl.glUniform1i = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLint) ) (uintptr_t) app->wglGetProcAddress( "glUniform1i" );
    if( !app->gl.glUniform3f ) app->gl.glUniform3f = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLfloat, APP_GLfloat, APP_GLfloat) ) (uintptr_t) app->wglGetProcAddress( "glUniform3f" );
    if( !app->gl.glGetUniformLocation ) app->gl.glGetUniformLocation = ( APP_GLint (APP_GLCALLTYPE*) (APP_GLuint, APP_GLchar const*) ) (uintptr_t) app->wglGetProcAddress( "glGetUniformLocation" );
    if( !app->gl.glTexImage2D ) app->gl.glTexImage2D = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLint, APP_GLint, APP_GLsizei, APP_GLsizei, APP_GLint, APP_GLenum, APP_GLenum, void const*) ) (uintptr_t) app->wglGetProcAddress( "glTexImage2D" );
    if( !app->gl.glClearColor ) app->gl.glClearColor = ( void (APP_GLCALLTYPE*) (APP_GLfloat, APP_GLfloat, APP_GLfloat, APP_GLfloat) ) (uintptr_t) app->wglGetProcAddress( "glClearColor" );
    if( !app->gl.glClear ) app->gl.glClear = ( void (APP_GLCALLTYPE*) (APP_GLbitfield) ) (uintptr_t) app->wglGetProcAddress( "glClear" );
    if( !app->gl.glDrawArrays ) app->gl.glDrawArrays = ( void (APP_GLCALLTYPE*) (APP_GLenum, APP_GLint, APP_GLsizei) ) (uintptr_t) app->wglGetProcAddress( "glDrawArrays" );
    if( !app->gl.glViewport ) app->gl.glViewport = ( void (APP_GLCALLTYPE*) (APP_GLint, APP_GLint, APP_GLsizei, APP_GLsizei) ) (uintptr_t) app->wglGetProcAddress( "glViewport" );
    if( !app->gl.glDeleteShader ) app->gl.glDeleteShader = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glDeleteShader" );
    if( !app->gl.glDeleteProgram ) app->gl.glDeleteProgram = ( void (APP_GLCALLTYPE*) (APP_GLuint) ) (uintptr_t) app->wglGetProcAddress( "glDeleteProgram" );
    #ifdef APP_REPORT_SHADER_ERRORS
        if( !app->gl.glGetShaderInfoLog ) app->gl.glGetShaderInfoLog = ( void (APP_GLCALLTYPE*) (APP_GLuint, APP_GLsizei, APP_GLsizei*, APP_GLchar*) ) (uintptr_t) app->wglGetProcAddress( "glGetShaderInfoLog" );
    #endif

    // Report error if any gl function was not found.
    if( !app->gl.glCreateShader ) { app_fatal_error( app, "Could not find function glCreateShader." ); goto init_failed; }
    if( !app->gl.glShaderSource ) { app_fatal_error( app, "Could not find function glShaderSource." ); goto init_failed; }
    if( !app->gl.glCompileShader ) { app_fatal_error( app, "Could not find function glCompileShader." ); goto init_failed; }
    if( !app->gl.glGetShaderiv ) { app_fatal_error( app, "Could not find function glGetShaderiv." ); goto init_failed; }
    if( !app->gl.glCreateProgram ) { app_fatal_error( app, "Could not find function glCreateProgram." ); goto init_failed; }
    if( !app->gl.glAttachShader ) { app_fatal_error( app, "Could not find function glAttachShader." ); goto init_failed; }
    if( !app->gl.glBindAttribLocation ) { app_fatal_error( app, "Could not find function glBindAttribLocation." ); goto init_failed; }
    if( !app->gl.glLinkProgram ) { app_fatal_error( app, "Could not find function glLinkProgram." ); goto init_failed; }
    if( !app->gl.glGetProgramiv ) { app_fatal_error( app, "Could not find function glGetProgramiv." ); goto init_failed; }
    if( !app->gl.glGenBuffers ) { app_fatal_error( app, "Could not find function glGenBuffers." ); goto init_failed; }
    if( !app->gl.glBindBuffer ) { app_fatal_error( app, "Could not find function glBindBuffer." ); goto init_failed; }
    if( !app->gl.glEnableVertexAttribArray ) { app_fatal_error( app, "Could not find function glEnableVertexAttribArray." ); goto init_failed; }
    if( !app->gl.glVertexAttribPointer ) { app_fatal_error( app, "Could not find function glVertexAttribPointer." ); goto init_failed; }
    if( !app->gl.glGenTextures ) { app_fatal_error( app, "Could not find function glGenTextures." ); goto init_failed; }
    if( !app->gl.glEnable ) { app_fatal_error( app, "Could not find function glEnable." ); goto init_failed; }
    if( !app->gl.glActiveTexture ) { app_fatal_error( app, "Could not find function glActiveTexture." ); goto init_failed; }
    if( !app->gl.glBindTexture ) { app_fatal_error( app, "Could not find function glBindTexture." ); goto init_failed; }
    if( !app->gl.glTexParameteri ) { app_fatal_error( app, "Could not find function glTexParameteri." ); goto init_failed; }
    if( !app->gl.glDeleteBuffers ) { app_fatal_error( app, "Could not find function glDeleteBuffers." ); goto init_failed; }
    if( !app->gl.glDeleteTextures ) { app_fatal_error( app, "Could not find function glDeleteTextures." ); goto init_failed; }
    if( !app->gl.glBufferData ) { app_fatal_error( app, "Could not find function glBufferData." ); goto init_failed; }
    if( !app->gl.glUseProgram ) { app_fatal_error( app, "Could not find function glUseProgram." ); goto init_failed; }
    if( !app->gl.glUniform1i ) { app_fatal_error( app, "Could not find function glUniform1i." ); goto init_failed; }
    if( !app->gl.glUniform3f ) { app_fatal_error( app, "Could not find function glUniform3f." ); goto init_failed; }
    if( !app->gl.glGetUniformLocation ) { app_fatal_error( app, "Could not find function glGetUniformLocation." ); goto init_failed; }
    if( !app->gl.glTexImage2D ) { app_fatal_error( app, "Could not find function glTexImage2D." ); goto init_failed; }
    if( !app->gl.glClearColor ) { app_fatal_error( app, "Could not find function glClearColor." ); goto init_failed; }
    if( !app->gl.glClear ) { app_fatal_error( app, "Could not find function glClear." ); goto init_failed; }
    if( !app->gl.glDrawArrays ) { app_fatal_error( app, "Could not find function glDrawArrays." ); goto init_failed; }
    if( !app->gl.glViewport ) { app_fatal_error( app, "Could not find function glViewport." ); goto init_failed; }
    if( !app->gl.glDeleteShader ) { app_fatal_error( app, "Could not find function glDeleteShader." ); goto init_failed; }
    if( !app->gl.glDeleteProgram ) { app_fatal_error( app, "Could not find function glDeleteProgram." ); goto init_failed; }
    #ifdef APP_REPORT_SHADER_ERRORS
        if( !app->gl.glGetShaderInfoLog ) { app_fatal_error( app, "Could not find function glGetShaderInfoLog." ); goto init_failed; }
    #endif

    // Platform independent OpenGL initialization
    int width = app->screenmode == APP_SCREENMODE_FULLSCREEN ? app->fullscreen_width : app->windowed_w;
    int height = app->screenmode == APP_SCREENMODE_FULLSCREEN ? app->fullscreen_height: app->windowed_h;
    if( !app_internal_opengl_init( app, &app->gl, app->interpolation, width, height ) )
        { 
        app_log( app, APP_LOG_LEVEL_ERROR, "Failed to initialize OpenGL" ); 
        goto init_failed; 
        }

    app->dsound_dll = LoadLibraryA( "dsound.dll" );
    if( !app->dsound_dll ) app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't load dsound.dll. Sound disabled." );

	if( app->dsound_dll )
		{
		HRESULT (WINAPI *DirectSoundCreate)(LPCGUID,LPDIRECTSOUND*,LPUNKNOWN) = ( HRESULT (WINAPI*)(LPCGUID,LPDIRECTSOUND*,LPUNKNOWN) ) 
			(uintptr_t) GetProcAddress( (HMODULE) app->dsound_dll, "DirectSoundCreate" ); 
		if( !DirectSoundCreate ) 
			{ 
			app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't find DirectSoundCreate. Sound disabled." ); 
			FreeLibrary( app->dsound_dll );
			app->dsound_dll = 0;
			}
		if( DirectSoundCreate )
			{
			HRESULT hr = DirectSoundCreate( NULL, &app->dsound, NULL );
			if( FAILED( hr ) || !app->dsound ) 
				{ 
				app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't create DirectSound object. Sound disabled." ); 
				DirectSoundCreate = 0; 
				FreeLibrary( app->dsound_dll );
				app->dsound_dll = 0;
				}	
			else
				{
				hr = IDirectSound_SetCooperativeLevel( app->dsound, app->hwnd, DSSCL_NORMAL);
				if( FAILED( hr ) ) 
					{ 
					app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't set cooperative level for DirectSound object. Sound disabled." ); 
					IDirectSound_Release( app->dsound );
					app->dsound = 0;
					DirectSoundCreate = 0; 
					FreeLibrary( app->dsound_dll );
					app->dsound_dll = 0;
					}
				}
			}
		}

    USHORT const USAGE_PAGE_GENERIC = ((USHORT) 0x01);
    USHORT const USAGE_GENERIC_MOUSE = ((USHORT) 0x02);

    RAWINPUTDEVICE rid[ 1 ];
    rid[ 0 ].usUsagePage = USAGE_PAGE_GENERIC; 
    rid[ 0 ].usUsage = USAGE_GENERIC_MOUSE; 
    rid[ 0 ].dwFlags = RIDEV_INPUTSINK;   
    rid[ 0 ].hwndTarget = app->hwnd;
    RegisterRawInputDevices( rid, 1, sizeof( *rid ) );

    if( !app_internal_tablet_init( app ) ) app_log( app, APP_LOG_LEVEL_WARNING, "WinTab initialization failed - tablet not available" );

    result = app_proc( app, user_data );

init_failed:
    if( !app_internal_tablet_term( app ) ) app_log( app, APP_LOG_LEVEL_WARNING, "WinTab termination failed" );
    if( app->dsoundbuf ) IDirectSoundBuffer_Release( app->dsoundbuf );
	if( app->dsound ) IDirectSound_Release( app->dsound );
	if( app->dsound_dll ) FreeLibrary( app->dsound_dll );
    if( !app_internal_opengl_term( &app->gl ) ) app_log( app, APP_LOG_LEVEL_WARNING, "Failed to terminate OpenGL" ); 
	if( app->gl_context ) app->wglMakeCurrent( 0, 0 );
    if( app->gl_context ) app->wglDeleteContext( app->gl_context ); 	
	if( app->gl_dll ) FreeLibrary( app->gl_dll );
    if( app->icon ) DestroyIcon( app->icon );
    if( app->current_pointer ) DestroyIcon( app->current_pointer );
    if( app->hdc ) ReleaseDC( app->hwnd, app->hdc );
    if( app->hwnd ) DestroyWindow( app->hwnd );
    UnregisterClass( TEXT( "app_wc" ), app->hinstance );

	if( timeGetDevCaps( &tc, sizeof( TIMECAPS ) ) == TIMERR_NOERROR ) 
		timeEndPeriod( tc.wPeriodMin );

    t = time( NULL );
    struct tm* end = localtime( &t );
	sprintf( msg, "Application terminated %02d:%02d:%02d %04d-%02d-%02d.", 
		end->tm_hour, end->tm_min, end->tm_sec, end->tm_year + 1900, end->tm_mon + 1, end->tm_mday );
	app_log( app, APP_LOG_LEVEL_INFO, msg );

    APP_FREE( memctx, app );
    return result;
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
		ShowWindow( app->hwnd, SW_SHOWNORMAL );			
		SetActiveWindow( app->hwnd );
		BringWindowToTop( app->hwnd );
		SwitchToThisWindow( app->hwnd, TRUE );
		if( app->tablet.context ) app->tablet.WTEnable( app->tablet.context, TRUE );
		app->initialized = TRUE;
		}

	MSG	msg;
	while( PeekMessage( &msg, app->hwnd, 0,0, PM_REMOVE ) )
		{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
		}

	if( app->has_focus )
		SwitchToThread();	//  yield to any thread on same processor
	else
		Sleep( 4 ); // Pause for a bit... we're not in a rush here
	
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
    (void) app, level, message;
    APP_LOG( app->logctx, level, message );
    }


void app_fatal_error( app_t* app, char const* message )
    {
    (void) app, message;
	APP_FATAL_ERROR( app->fatalctx, message );
    }


static HCURSOR app_internal_create_cursor( HWND hwnd, int width, int height, APP_U32* pixels_abgr, int hotspot_x, int hotspot_y )
    {
	BITMAPV5HEADER header;
	memset( &header, 0, sizeof( BITMAPV5HEADER ) );
	header.bV5Size = sizeof( BITMAPV5HEADER );
	header.bV5Width = (LONG) width;
	header.bV5Height = -(LONG) height;
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
			*ptr++ = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;
			}
		}

	HBITMAP empty_mask = CreateBitmap( width, height, 1, 1, NULL );
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

	SetCursor( app->current_pointer );
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

    if( screenmode == APP_SCREENMODE_WINDOW ) 
        {
 		SetWindowLong( app->hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE );

		WINDOWPLACEMENT placement;
        placement.length = sizeof( placement );
        GetWindowPlacement( app->hwnd, &placement );
        
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

		SetWindowLong( app->hwnd, GWL_STYLE, WS_VISIBLE );		
	    SetWindowPos( app->hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED );	    
		}
    }


void app_window_size( app_t* app, int width, int height )
    {
    RECT r;
	r = app_internal_rect( 0, 0, width, height );
	AdjustWindowRect( &r, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE );
    
    width = r.right - r.left;
    height = r.bottom - r.top;
	app->windowed_w = width;
	app->windowed_h = height;

	if( app->screenmode == APP_SCREENMODE_WINDOW ) 
	    SetWindowPos( app->hwnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
    }


int app_window_width( app_t* app )
    {
    int width = app->windowed_w;
    if( app->screenmode == APP_SCREENMODE_WINDOW )
        {
	    WINDOWPLACEMENT placement;
        placement.length = sizeof( placement );
	    GetWindowPlacement( app->hwnd, &placement );
        width = placement.rcNormalPosition.right - placement.rcNormalPosition.left;
        }
    RECT r = app_internal_rect( 0, 0, 0, 0 );
	AdjustWindowRect( &r, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE );
    return width - ( r.right - r.left );
    }


int app_window_height( app_t* app )
    {
    int height = app->windowed_h;
    if( app->screenmode == APP_SCREENMODE_WINDOW )
        {
	    WINDOWPLACEMENT placement;
        placement.length = sizeof( placement );
	    GetWindowPlacement( app->hwnd, &placement );
        height = placement.rcNormalPosition.bottom - placement.rcNormalPosition.top;
        }
    RECT r = app_internal_rect( 0, 0, 0, 0 );
	AdjustWindowRect( &r, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE );
    return height - ( r.bottom - r.top );
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
    else
        {
        app->windowed_x = x;
        app->windowed_y = y;
        }
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


void app_sound_buffer_size( app_t* app, int sample_pairs_count ) 
    { 
	if( !app->dsound ) return;

	if( app->sample_pairs_count == sample_pairs_count ) return;
    app->sample_pairs_count = sample_pairs_count;

    if( app->dsoundbuf ) 
        {
        IDirectSoundBuffer_Release( app->dsoundbuf );
        app->dsoundbuf = 0;
        }

    if( sample_pairs_count > 0 )
        {
        int const channels = 2;
        int const frequency = 44100;
        int const bits_per_sample = 16;

	    WAVEFORMATEX format; 
        memset( &format, 0, sizeof( WAVEFORMATEX ) ); 
	    format.wFormatTag = WAVE_FORMAT_PCM; 
        format.nChannels = (WORD) channels; 
        format.nSamplesPerSec = (DWORD) frequency; 
        format.nBlockAlign = (WORD) ( ( channels * bits_per_sample ) / 8 ); 
        format.nAvgBytesPerSec = (DWORD) ( frequency * format.nBlockAlign ); 
        format.wBitsPerSample = (WORD) bits_per_sample; 
        format.cbSize = 0;

        DSBUFFERDESC dsbdesc; 
        memset( &dsbdesc, 0, sizeof( DSBUFFERDESC ) ); 
        dsbdesc.dwSize = sizeof( DSBUFFERDESC ); 
    
        dsbdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS; 
    
        int size = channels * ( bits_per_sample / 8 ) * sample_pairs_count;
        dsbdesc.dwBufferBytes = (DWORD) size; 
        dsbdesc.lpwfxFormat = &format; 
    
        HRESULT hr = IDirectSound_CreateSoundBuffer( app->dsound, &dsbdesc, &app->dsoundbuf, NULL ); 
	    if( FAILED( hr ) || !app->dsoundbuf ) 
            {
            app_log( app, APP_LOG_LEVEL_WARNING, "Failed to create sound buffer" );	
		    IDirectSound_Release( app->dsound );
		    app->dsound = 0;
            return;
            }

        IDirectSoundBuffer_Play( app->dsoundbuf, 0, 0, DSBPLAY_LOOPING );
        }
    }


int app_sound_position( app_t* app ) 
    { 
	if( !app->dsound ) return 0;
	if( !app->dsoundbuf ) return 0;
	DWORD position = 0;
	IDirectSoundBuffer_GetCurrentPosition( app->dsoundbuf, &position, 0 );
	return ( (int) position ) / ( 2 * ( 16 / 8 ) );
    }


void app_sound_write( app_t* app, int sample_pairs_offset, int sample_pairs_count, APP_S16 const* sample_pairs ) 
    { 
	if( !app->dsound ) return;
    if( !app->dsoundbuf ) return;

    int offset = sample_pairs_offset * 2 * ( 16 / 8 );
    int length = sample_pairs_count * 2 * ( 16 / 8 );

    // Check the range of the length parameter
    if( sample_pairs_count > app->sample_pairs_count ) { app_log( app, APP_LOG_LEVEL_ERROR, "Sound buffer out of range" ); return; }

	// Obtain memory address of write block. This will be in two parts if the block wraps around.
    LPVOID lpvPtr1; 
    DWORD dwBytes1; 
    LPVOID lpvPtr2; 
    DWORD dwBytes2; 
    HRESULT hr = IDirectSoundBuffer_Lock( app->dsoundbuf, (DWORD) offset, (DWORD) length, &lpvPtr1, &dwBytes1, 
        &lpvPtr2, &dwBytes2, 0 ); 
 
    // If DSERR_BUFFERLOST is returned, restore and retry lock. 
    if( hr == DSERR_BUFFERLOST ) 
		{ 
        IDirectSoundBuffer_Restore( app->dsoundbuf ); 
	    hr = IDirectSoundBuffer_Lock( app->dsoundbuf, (DWORD) offset, (DWORD) length, &lpvPtr1, &dwBytes1, 
            &lpvPtr2, &dwBytes2, 0 ); 
		} 
    if( FAILED( hr) ) 
        { 
        app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't lock sound buffer" );
		IDirectSound_Release( app->dsound );
		app->dsound = 0;
        return;
        }

	// Write to pointers.      
	if( sample_pairs ) memcpy( lpvPtr1, sample_pairs, dwBytes1 );      
	else memset( lpvPtr1, 0, dwBytes1 );      
	if( lpvPtr2 ) 
		{
        if( sample_pairs ) memcpy( lpvPtr2, (void*)( ( (uintptr_t) sample_pairs ) + dwBytes1 ), dwBytes2 ); 
		else memset( lpvPtr2, 0, dwBytes2 ); 
		}

	// Release the data back to DirectSound. 
    hr = IDirectSoundBuffer_Unlock( app->dsoundbuf, lpvPtr1, dwBytes1, lpvPtr2, dwBytes2 ); 
    if( FAILED( hr) ) 
        { 
        app_log( app, APP_LOG_LEVEL_WARNING, "Couldn't unlock sound buffer" );
		IDirectSound_Release( app->dsound );
		app->dsound = 0;
        return;
        }
    }


void app_sound_volume( app_t* app, float volume )
    {
	if( !app->dsound ) return;
    if( !app->dsoundbuf ) return;

    int level = volume < 0.000015f ? DSBVOLUME_MIN : (int) ( 2000.0f * log10f( volume ) );
    if( app->sound_level == level ) return;	
    app->sound_level = level;

	IDirectSoundBuffer_SetVolume( app->dsoundbuf, level );
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

#else
    #error Undefined platform. Define APP_WINDOWS or APP_NULL.
#endif 


#endif /* APP_IMPLEMENTATION */

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
