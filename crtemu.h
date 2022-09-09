/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

crtemu.h - v0.1 - Cathode ray tube emulation shader for C/C++.

Do this:
    #define CRTEMU_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/


#ifndef crtemu_h
#define crtemu_h

#ifndef CRTEMU_U32
    #define CRTEMU_U32 unsigned int
#endif
#ifndef CRTEMU_U64
    #define CRTEMU_U64 unsigned long long
#endif

typedef struct crtemu_t crtemu_t;

crtemu_t* crtemu_create( void* memctx );

void crtemu_destroy( crtemu_t* crtemu );

typedef struct crtemu_config_t
    {
    float curvature, scanlines, shadow_mask, separation, ghosting, noise, flicker, vignette, distortion, aspect_lock,
        hpos, vpos, hsize, vsize, contrast, brightness, saturation, blur, degauss; // range -1.0f to 1.0f, default=0.0f
    } crtemu_config_t;

void crtemu_config(crtemu_t* crtemu, crtemu_config_t const* config);

void crtemu_frame( crtemu_t* crtemu, CRTEMU_U32* frame_abgr, int frame_width, int frame_height );

void crtemu_present( crtemu_t* crtemu, CRTEMU_U64 time_us, CRTEMU_U32 const* pixels_xbgr, int width, int height,
    CRTEMU_U32 mod_xbgr, CRTEMU_U32 border_xbgr );

void crtemu_coordinates_window_to_bitmap( crtemu_t* crtemu, int width, int height, int* x, int* y );
void crtemu_coordinates_bitmap_to_window( crtemu_t* crtemu, int width, int height, int* x, int* y );

#endif /* crtemu_h */

/*
----------------------
    IMPLEMENTATION
----------------------
*/
#ifdef CRTEMU_IMPLEMENTATION
#undef CRTEMU_IMPLEMENTATION

#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include <stddef.h>
#include <string.h>

#ifndef CRTEMU_MALLOC
    #include <stdlib.h>
    #if defined(__cplusplus)
        #define CRTEMU_MALLOC( ctx, size ) ( ::malloc( size ) )
        #define CRTEMU_FREE( ctx, ptr ) ( ::free( ptr ) )
    #else
        #define CRTEMU_MALLOC( ctx, size ) ( malloc( size ) )
        #define CRTEMU_FREE( ctx, ptr ) ( free( ptr ) )
    #endif
#endif

#ifdef CRTEMU_REPORT_SHADER_ERRORS
    #ifndef CRTEMU_REPORT_ERROR
        #define _CRT_NONSTDC_NO_DEPRECATE
        #define _CRT_SECURE_NO_WARNINGS
        #include <stdio.h>
        #define CRTEMU_REPORT_ERROR( str ) printf( "%s", str )
    #endif
#endif

#ifndef _WIN32
    #define CRTEMU_SDL
#endif

#ifdef __wasm__
    #define CRTEMU_WEBGL
#endif

#ifndef CRTEMU_SDL

    #ifdef __cplusplus
    extern "C" {
    #endif

        __declspec(dllimport) struct HINSTANCE__* __stdcall LoadLibraryA( char const* lpLibFileName );
        __declspec(dllimport) int __stdcall FreeLibrary( struct HINSTANCE__* hModule );
        #if defined(_WIN64)
            typedef __int64 (__stdcall* CRTEMU_PROC)( void );
            __declspec(dllimport) CRTEMU_PROC __stdcall GetProcAddress( struct HINSTANCE__* hModule, char const* lpLibFileName );
        #else
            typedef __int32 (__stdcall* CRTEMU_PROC)( void );
            __declspec(dllimport) CRTEMU_PROC __stdcall GetProcAddress( struct HINSTANCE__* hModule, char const* lpLibFileName );
        #endif

    #ifdef __cplusplus
        }
    #endif

    #define CRTEMU_GLCALLTYPE __stdcall
    typedef unsigned int CRTEMU_GLuint;
    typedef int CRTEMU_GLsizei;
    typedef unsigned int CRTEMU_GLenum;
    typedef int CRTEMU_GLint;
    typedef float CRTEMU_GLfloat;
    typedef char CRTEMU_GLchar;
    typedef unsigned char CRTEMU_GLboolean;
    typedef size_t CRTEMU_GLsizeiptr;
    typedef unsigned int CRTEMU_GLbitfield;

    #define CRTEMU_GL_FLOAT 0x1406
    #define CRTEMU_GL_FALSE 0
    #define CRTEMU_GL_FRAGMENT_SHADER 0x8b30
    #define CRTEMU_GL_VERTEX_SHADER 0x8b31
    #define CRTEMU_GL_COMPILE_STATUS 0x8b81
    #define CRTEMU_GL_LINK_STATUS 0x8b82
    #define CRTEMU_GL_INFO_LOG_LENGTH 0x8b84
    #define CRTEMU_GL_ARRAY_BUFFER 0x8892
    #define CRTEMU_GL_TEXTURE_2D 0x0de1
    #define CRTEMU_GL_TEXTURE0 0x84c0
    #define CRTEMU_GL_TEXTURE1 0x84c1
    #define CRTEMU_GL_TEXTURE2 0x84c2
    #define CRTEMU_GL_TEXTURE3 0x84c3
    #define CRTEMU_GL_TEXTURE_MIN_FILTER 0x2801
    #define CRTEMU_GL_TEXTURE_MAG_FILTER 0x2800
    #define CRTEMU_GL_NEAREST 0x2600
    #define CRTEMU_GL_LINEAR 0x2601
    #define CRTEMU_GL_STATIC_DRAW 0x88e4
    #define CRTEMU_GL_RGBA 0x1908
    #define CRTEMU_GL_UNSIGNED_BYTE 0x1401
    #define CRTEMU_GL_COLOR_BUFFER_BIT 0x00004000
    #define CRTEMU_GL_TRIANGLE_FAN 0x0006
    #define CRTEMU_GL_FRAMEBUFFER 0x8d40
    #define CRTEMU_GL_VIEWPORT 0x0ba2
    #define CRTEMU_GL_RGB 0x1907
    #define CRTEMU_GL_COLOR_ATTACHMENT0 0x8ce0
    #define CRTEMU_GL_TEXTURE_WRAP_S 0x2802
    #define CRTEMU_GL_TEXTURE_WRAP_T 0x2803
    #define CRTEMU_GL_CLAMP_TO_BORDER 0x812D
    #define CRTEMU_GL_TEXTURE_BORDER_COLOR 0x1004

#else

    #ifndef CRTEMU_WEBGL
        #include <GL/glew.h>
        #include "SDL_opengl.h"
    #else
        #include <wajic_gl.h>
    #endif
    #define CRTEMU_GLCALLTYPE GLAPIENTRY

    typedef GLuint CRTEMU_GLuint;
    typedef GLsizei CRTEMU_GLsizei;
    typedef GLenum CRTEMU_GLenum;
    typedef GLint CRTEMU_GLint;
    typedef GLfloat CRTEMU_GLfloat;
    typedef GLchar CRTEMU_GLchar;
    typedef GLboolean CRTEMU_GLboolean;
    typedef GLsizeiptr CRTEMU_GLsizeiptr;
    typedef GLbitfield CRTEMU_GLbitfield;

     #define CRTEMU_GL_FLOAT GL_FLOAT
     #define CRTEMU_GL_FALSE GL_FALSE
     #define CRTEMU_GL_FRAGMENT_SHADER GL_FRAGMENT_SHADER
     #define CRTEMU_GL_VERTEX_SHADER GL_VERTEX_SHADER
     #define CRTEMU_GL_COMPILE_STATUS GL_COMPILE_STATUS
     #define CRTEMU_GL_LINK_STATUS GL_LINK_STATUS
     #define CRTEMU_GL_INFO_LOG_LENGTH GL_INFO_LOG_LENGTH
     #define CRTEMU_GL_ARRAY_BUFFER GL_ARRAY_BUFFER
     #define CRTEMU_GL_TEXTURE_2D GL_TEXTURE_2D
     #define CRTEMU_GL_TEXTURE0 GL_TEXTURE0
     #define CRTEMU_GL_TEXTURE1 GL_TEXTURE1
     #define CRTEMU_GL_TEXTURE2 GL_TEXTURE2
     #define CRTEMU_GL_TEXTURE3 GL_TEXTURE3
     #define CRTEMU_GL_TEXTURE_MIN_FILTER GL_TEXTURE_MIN_FILTER
     #define CRTEMU_GL_TEXTURE_MAG_FILTER GL_TEXTURE_MAG_FILTER
     #define CRTEMU_GL_NEAREST GL_NEAREST
     #define CRTEMU_GL_LINEAR GL_LINEAR
     #define CRTEMU_GL_STATIC_DRAW GL_STATIC_DRAW
     #define CRTEMU_GL_RGBA GL_RGBA
     #define CRTEMU_GL_UNSIGNED_BYTE GL_UNSIGNED_BYTE
     #define CRTEMU_GL_COLOR_BUFFER_BIT GL_COLOR_BUFFER_BIT
     #define CRTEMU_GL_TRIANGLE_FAN GL_TRIANGLE_FAN
     #define CRTEMU_GL_FRAMEBUFFER GL_FRAMEBUFFER
     #define CRTEMU_GL_VIEWPORT GL_VIEWPORT
     #define CRTEMU_GL_RGB GL_RGB
     #define CRTEMU_GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0
     #define CRTEMU_GL_TEXTURE_WRAP_S GL_TEXTURE_WRAP_S
     #define CRTEMU_GL_TEXTURE_WRAP_T GL_TEXTURE_WRAP_T
     #ifndef CRTEMU_WEBGL
         #define CRTEMU_GL_CLAMP_TO_BORDER GL_CLAMP_TO_BORDER
         #define CRTEMU_GL_TEXTURE_BORDER_COLOR GL_TEXTURE_BORDER_COLOR
     #else
         // WebGL does not support GL_CLAMP_TO_BORDER, we have to emulate
         // this behavior with code in the fragment shader
         #define CRTEMU_GL_CLAMP_TO_BORDER GL_CLAMP_TO_EDGE
     #endif
#endif


struct crtemu_t
    {
    void* memctx;
    crtemu_config_t config;

    CRTEMU_GLuint vertexbuffer;
    CRTEMU_GLuint backbuffer;

    CRTEMU_GLuint accumulatetexture_a;
    CRTEMU_GLuint accumulatetexture_b;
    CRTEMU_GLuint accumulatebuffer_a;
    CRTEMU_GLuint accumulatebuffer_b;

    CRTEMU_GLuint blurtexture_a;
    CRTEMU_GLuint blurtexture_b;
    CRTEMU_GLuint blurbuffer_a;
    CRTEMU_GLuint blurbuffer_b;

    CRTEMU_GLuint frametexture;
    float use_frame;

    CRTEMU_GLuint crt_shader;
    CRTEMU_GLuint blur_shader;
    CRTEMU_GLuint accumulate_shader;
    CRTEMU_GLuint blend_shader;
    CRTEMU_GLuint copy_shader;

    int last_present_width;
    int last_present_height;


    #ifndef CRTEMU_SDL
        struct HINSTANCE__* gl_dll;
        CRTEMU_PROC (CRTEMU_GLCALLTYPE *wglGetProcAddress) (char const* );
    #endif

    void (CRTEMU_GLCALLTYPE* TexParameterfv) (CRTEMU_GLenum target, CRTEMU_GLenum pname, CRTEMU_GLfloat const* params);
    void (CRTEMU_GLCALLTYPE* DeleteFramebuffers) (CRTEMU_GLsizei n, CRTEMU_GLuint const* framebuffers);
    void (CRTEMU_GLCALLTYPE* GetIntegerv) (CRTEMU_GLenum pname, CRTEMU_GLint *data);
    void (CRTEMU_GLCALLTYPE* GenFramebuffers) (CRTEMU_GLsizei n, CRTEMU_GLuint *framebuffers);
    void (CRTEMU_GLCALLTYPE* BindFramebuffer) (CRTEMU_GLenum target, CRTEMU_GLuint framebuffer);
    void (CRTEMU_GLCALLTYPE* Uniform1f) (CRTEMU_GLint location, CRTEMU_GLfloat v0);
    void (CRTEMU_GLCALLTYPE* Uniform2f) (CRTEMU_GLint location, CRTEMU_GLfloat v0, CRTEMU_GLfloat v1);
    void (CRTEMU_GLCALLTYPE* FramebufferTexture2D) (CRTEMU_GLenum target, CRTEMU_GLenum attachment, CRTEMU_GLenum textarget, CRTEMU_GLuint texture, CRTEMU_GLint level);
    CRTEMU_GLuint (CRTEMU_GLCALLTYPE* CreateShader) (CRTEMU_GLenum type);
    void (CRTEMU_GLCALLTYPE* ShaderSource) (CRTEMU_GLuint shader, CRTEMU_GLsizei count, CRTEMU_GLchar const* const* string, CRTEMU_GLint const* length);
    void (CRTEMU_GLCALLTYPE* CompileShader) (CRTEMU_GLuint shader);
    void (CRTEMU_GLCALLTYPE* GetShaderiv) (CRTEMU_GLuint shader, CRTEMU_GLenum pname, CRTEMU_GLint *params);
    CRTEMU_GLuint (CRTEMU_GLCALLTYPE* CreateProgram) (void);
    void (CRTEMU_GLCALLTYPE* AttachShader) (CRTEMU_GLuint program, CRTEMU_GLuint shader);
    void (CRTEMU_GLCALLTYPE* BindAttribLocation) (CRTEMU_GLuint program, CRTEMU_GLuint index, CRTEMU_GLchar const* name);
    void (CRTEMU_GLCALLTYPE* LinkProgram) (CRTEMU_GLuint program);
    void (CRTEMU_GLCALLTYPE* GetProgramiv) (CRTEMU_GLuint program, CRTEMU_GLenum pname, CRTEMU_GLint *params);
    void (CRTEMU_GLCALLTYPE* GenBuffers) (CRTEMU_GLsizei n, CRTEMU_GLuint *buffers);
    void (CRTEMU_GLCALLTYPE* BindBuffer) (CRTEMU_GLenum target, CRTEMU_GLuint buffer);
    void (CRTEMU_GLCALLTYPE* EnableVertexAttribArray) (CRTEMU_GLuint index);
    void (CRTEMU_GLCALLTYPE* VertexAttribPointer) (CRTEMU_GLuint index, CRTEMU_GLint size, CRTEMU_GLenum type, CRTEMU_GLboolean normalized, CRTEMU_GLsizei stride, void const* pointer);
    void (CRTEMU_GLCALLTYPE* GenTextures) (CRTEMU_GLsizei n, CRTEMU_GLuint* textures);
    void (CRTEMU_GLCALLTYPE* Enable) (CRTEMU_GLenum cap);
    void (CRTEMU_GLCALLTYPE* ActiveTexture) (CRTEMU_GLenum texture);
    void (CRTEMU_GLCALLTYPE* BindTexture) (CRTEMU_GLenum target, CRTEMU_GLuint texture);
    void (CRTEMU_GLCALLTYPE* TexParameteri) (CRTEMU_GLenum target, CRTEMU_GLenum pname, CRTEMU_GLint param);
    void (CRTEMU_GLCALLTYPE* DeleteBuffers) (CRTEMU_GLsizei n, CRTEMU_GLuint const* buffers);
    void (CRTEMU_GLCALLTYPE* DeleteTextures) (CRTEMU_GLsizei n, CRTEMU_GLuint const* textures);
    void (CRTEMU_GLCALLTYPE* BufferData) (CRTEMU_GLenum target, CRTEMU_GLsizeiptr size, void const *data, CRTEMU_GLenum usage);
    void (CRTEMU_GLCALLTYPE* UseProgram) (CRTEMU_GLuint program);
    void (CRTEMU_GLCALLTYPE* Uniform1i) (CRTEMU_GLint location, CRTEMU_GLint v0);
    void (CRTEMU_GLCALLTYPE* Uniform3f) (CRTEMU_GLint location, CRTEMU_GLfloat v0, CRTEMU_GLfloat v1, CRTEMU_GLfloat v2);
    CRTEMU_GLint (CRTEMU_GLCALLTYPE* GetUniformLocation) (CRTEMU_GLuint program, CRTEMU_GLchar const* name);
    void (CRTEMU_GLCALLTYPE* TexImage2D) (CRTEMU_GLenum target, CRTEMU_GLint level, CRTEMU_GLint internalformat, CRTEMU_GLsizei width, CRTEMU_GLsizei height, CRTEMU_GLint border, CRTEMU_GLenum format, CRTEMU_GLenum type, void const* pixels);
    void (CRTEMU_GLCALLTYPE* ClearColor) (CRTEMU_GLfloat red, CRTEMU_GLfloat green, CRTEMU_GLfloat blue, CRTEMU_GLfloat alpha);
    void (CRTEMU_GLCALLTYPE* Clear) (CRTEMU_GLbitfield mask);
    void (CRTEMU_GLCALLTYPE* DrawArrays) (CRTEMU_GLenum mode, CRTEMU_GLint first, CRTEMU_GLsizei count);
    void (CRTEMU_GLCALLTYPE* Viewport) (CRTEMU_GLint x, CRTEMU_GLint y, CRTEMU_GLsizei width, CRTEMU_GLsizei height);
    void (CRTEMU_GLCALLTYPE* DeleteShader) (CRTEMU_GLuint shader);
    void (CRTEMU_GLCALLTYPE* DeleteProgram) (CRTEMU_GLuint program);
    #ifdef CRTEMU_REPORT_SHADER_ERRORS
        void (CRTEMU_GLCALLTYPE* GetShaderInfoLog) (CRTEMU_GLuint shader, CRTEMU_GLsizei bufSize, CRTEMU_GLsizei *length, CRTEMU_GLchar *infoLog);
    #endif
    };


static CRTEMU_GLuint crtemu_internal_build_shader( crtemu_t* crtemu, char const* vs_source, char const* fs_source )
    {
    #ifdef CRTEMU_REPORT_SHADER_ERRORS
        char error_message[ 1024 ];
    #endif

    CRTEMU_GLuint vs = crtemu->CreateShader( CRTEMU_GL_VERTEX_SHADER );
    crtemu->ShaderSource( vs, 1, (char const**) &vs_source, NULL );
    crtemu->CompileShader( vs );
    CRTEMU_GLint vs_compiled;
    crtemu->GetShaderiv( vs, CRTEMU_GL_COMPILE_STATUS, &vs_compiled );
    if( !vs_compiled )
        {
        #ifdef CRTEMU_REPORT_SHADER_ERRORS
            char const* prefix = "Vertex Shader Error: ";
            strcpy( error_message, prefix );
            int len = 0, written = 0;
            crtemu->GetShaderiv( vs, CRTEMU_GL_INFO_LOG_LENGTH, &len );
            crtemu->GetShaderInfoLog( vs, (CRTEMU_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written,
                error_message + strlen( prefix ) );
            CRTEMU_REPORT_ERROR( error_message );
        #endif
        return 0;
        }

    CRTEMU_GLuint fs = crtemu->CreateShader( CRTEMU_GL_FRAGMENT_SHADER );
    crtemu->ShaderSource( fs, 1, (char const**) &fs_source, NULL );
    crtemu->CompileShader( fs );
    CRTEMU_GLint fs_compiled;
    crtemu->GetShaderiv( fs, CRTEMU_GL_COMPILE_STATUS, &fs_compiled );
    if( !fs_compiled )
        {
        #ifdef CRTEMU_REPORT_SHADER_ERRORS
            char const* prefix = "Fragment Shader Error: ";
            strcpy( error_message, prefix );
            int len = 0, written = 0;
            crtemu->GetShaderiv( vs, CRTEMU_GL_INFO_LOG_LENGTH, &len );
            crtemu->GetShaderInfoLog( fs, (CRTEMU_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written,
                error_message + strlen( prefix ) );
            CRTEMU_REPORT_ERROR( error_message );
        #endif
        return 0;
        }


    CRTEMU_GLuint prg = crtemu->CreateProgram();
    crtemu->AttachShader( prg, fs );
    crtemu->AttachShader( prg, vs );
    crtemu->BindAttribLocation( prg, 0, "pos" );
    crtemu->LinkProgram( prg );

    CRTEMU_GLint linked;
    crtemu->GetProgramiv( prg, CRTEMU_GL_LINK_STATUS, &linked );
    if( !linked )
        {
        #ifdef CRTEMU_REPORT_SHADER_ERRORS
            char const* prefix = "Shader Link Error: ";
            strcpy( error_message, prefix );
            int len = 0, written = 0;
            crtemu->GetShaderiv( vs, CRTEMU_GL_INFO_LOG_LENGTH, &len );
            crtemu->GetShaderInfoLog( prg, (CRTEMU_GLsizei)( sizeof( error_message ) - strlen( prefix ) ), &written,
                error_message + strlen( prefix ) );
            CRTEMU_REPORT_ERROR( error_message );
        #endif
        return 0;
        }

    return prg;
    }


crtemu_t* crtemu_create( void* memctx )
    {

    char const* vs_source =
        #ifdef CRTEMU_WEBGL
            "precision highp float;\n\n"
        #else
            "#version 120\n\n"
        #endif
        ""
        "attribute vec4 pos;"
        "varying vec2 uv;"
        ""
        "void main( void )"
        "    {"
        "    gl_Position = vec4( pos.xy, 0.0, 1.0 );"
        "    uv = pos.zw;"
        "    }";

    char const* crt_fs_source =
        #ifdef CRTEMU_WEBGL
            "precision highp float;\n\n"
        #else
            "#version 120\n\n"
        #endif
        "\n"
        "varying vec2 uv;\n"
        "\n"
        "uniform vec3 modulate;\n"
        "uniform vec2 resolution;\n"
        "uniform vec2 size;\n"
        "uniform float time;\n"
        "uniform sampler2D backbuffer;\n"
        "uniform sampler2D blurbuffer;\n"
        "uniform sampler2D frametexture;\n"
        "uniform float use_frame;\n"
        "\n"
        #ifdef CRTEMU_WEBGL
            // WebGL does not support GL_CLAMP_TO_BORDER so we overwrite texture2D
            // with this function which emulates the clamp-to-border behavior
            "vec4 texture2Dborder(sampler2D samp, vec2 tc)\n"
            "    {\n"
            "    float borderdist = .502-max(abs(.5-tc.x), abs(.5-tc.y));\n"
            "    float borderfade = clamp(borderdist * 400.0, 0.0, 1.0);\n"
            "    return texture2D( samp, tc ) * borderfade;\n"
            "    }\n"
            "#define texture2D texture2Dborder\n"
        #endif
        "vec3 tsample( sampler2D samp, vec2 tc, float offs, vec2 resolution )\n"
        "    {\n"
        "    tc = tc * vec2(1.025, 0.92) + vec2(-0.0125, 0.04);\n"
        "    vec3 s = pow( abs( texture2D( samp, vec2( tc.x, 1.0-tc.y ) ).rgb), vec3( 2.2 ) );\n"
        "    return s*vec3(1.25);\n"
        "    }"
        "\n"
        "vec3 filmic( vec3 LinearColor )\n"
        "    {\n"
        "    vec3 x = max( vec3(0.0), LinearColor-vec3(0.004));\n"
        "    return (x*(6.2*x+0.5))/(x*(6.2*x+1.7)+0.06);\n"
        "    }\n"
        "\n"
        "vec2 curve( vec2 uv )\n"
        "    {\n"
        "    uv = (uv - 0.5) * 2.0;\n"
        "    uv *= 1.1;  \n"
        "    uv.x *= 1.0 + pow((abs(uv.y) / 5.0), 2.0);\n"
        "    uv.y *= 1.0 + pow((abs(uv.x) / 4.0), 2.0);\n"
        "    uv  = (uv / 2.0) + 0.5;\n"
        "    uv =  uv *0.92 + 0.04;\n"
        "    return uv;\n"
        "    }\n"
        "\n"
        "float rand(vec2 co)\n"
        "    {\n"
        "    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);\n"
        "    }\n"
        "    \n"
        "void main(void)\n"
        "    {\n"
        "    /* Curve */\n"
        "    vec2 curved_uv = mix( curve( uv ), uv, 0.4 );\n"
        "    float scale = 0.04;\n"
        "    vec2 scuv = curved_uv*(1.0-scale)+scale/2.0+vec2(0.003, -0.001);\n"
        "\n"
        "    /* Main color, Bleed */\n"
        "    vec3 col;\n"
        "    float x =  sin(0.1*time+curved_uv.y*13.0)*sin(0.23*time+curved_uv.y*19.0)*sin(0.3+0.11*time+curved_uv.y*23.0)*0.0012;\n"
        "    float o =sin(gl_FragCoord.y*1.5)/resolution.x;\n"
        "    x=x*0.25+o*0.25;\n"
        "    col.r = tsample(backbuffer,vec2(x+scuv.x+0.0009,scuv.y+0.0009),resolution.y/800.0, resolution ).x+0.02;\n"
        "    col.g = tsample(backbuffer,vec2(x+scuv.x+0.0000,scuv.y-0.0011),resolution.y/800.0, resolution ).y+0.02;\n"
        "    col.b = tsample(backbuffer,vec2(x+scuv.x-0.0015,scuv.y+0.0000),resolution.y/800.0, resolution ).z+0.02;\n"
        "    float i = clamp(col.r*0.299 + col.g*0.587 + col.b*0.114, 0.0, 1.0 );        \n"
        "    i = pow( 1.0 - pow(i,2.0), 1.0 );\n"
        "    i = (1.0-i) * 0.85 + 0.15;  \n"
        "\n"
        "    /* Ghosting */\n"
        "    float ghs = 0.15;\n"
        "    vec3 r = tsample(blurbuffer, vec2(x-0.014*1.0, -0.027)*0.85+0.007*vec2( 0.35*sin(1.0/7.0 + 15.0*curved_uv.y + 0.9*time), \n"
        "        0.35*sin( 2.0/7.0 + 10.0*curved_uv.y + 1.37*time) )+vec2(scuv.x+0.001,scuv.y+0.001),\n"
        "        5.5+1.3*sin( 3.0/9.0 + 31.0*curved_uv.x + 1.70*time),resolution).xyz*vec3(0.5,0.25,0.25);\n"
        "    vec3 g = tsample(blurbuffer, vec2(x-0.019*1.0, -0.020)*0.85+0.007*vec2( 0.35*cos(1.0/9.0 + 15.0*curved_uv.y + 0.5*time), \n"
        "        0.35*sin( 2.0/9.0 + 10.0*curved_uv.y + 1.50*time) )+vec2(scuv.x+0.000,scuv.y-0.002),\n"
        "        5.4+1.3*sin( 3.0/3.0 + 71.0*curved_uv.x + 1.90*time),resolution).xyz*vec3(0.25,0.5,0.25);\n"
        "    vec3 b = tsample(blurbuffer, vec2(x-0.017*1.0, -0.003)*0.85+0.007*vec2( 0.35*sin(2.0/3.0 + 15.0*curved_uv.y + 0.7*time), \n"
        "        0.35*cos( 2.0/3.0 + 10.0*curved_uv.y + 1.63*time) )+vec2(scuv.x-0.002,scuv.y+0.000),\n"
        "        5.3+1.3*sin( 3.0/7.0 + 91.0*curved_uv.x + 1.65*time),resolution).xyz*vec3(0.25,0.25,0.5);\n"
        "\n"
        "    col += vec3(ghs*(1.0-0.299))*pow(clamp(vec3(3.0)*r,vec3(0.0),vec3(1.0)),vec3(2.0))*vec3(i);\n"
        "    col += vec3(ghs*(1.0-0.587))*pow(clamp(vec3(3.0)*g,vec3(0.0),vec3(1.0)),vec3(2.0))*vec3(i);\n"
        "    col += vec3(ghs*(1.0-0.114))*pow(clamp(vec3(3.0)*b,vec3(0.0),vec3(1.0)),vec3(2.0))*vec3(i);\n"
        "\n"
        "    /* Level adjustment (curves) */\n"
        "    col *= vec3(0.95,1.05,0.95);\n"
        "    col = clamp(col*1.3 + 0.75*col*col + 1.25*col*col*col*col*col,vec3(0.0),vec3(10.0));\n"
        "\n"
        "    /* Vignette */\n"
        "    float vig = (0.1 + 1.0*16.0*curved_uv.x*curved_uv.y*(1.0-curved_uv.x)*(1.0-curved_uv.y));\n"
        "    vig = 1.3*pow(vig,0.5);\n"
        "    col *= vig;\n"
        "\n"
        "    /* Scanlines */\n"
        "    float scans = clamp( 0.35+0.18*sin(4.0*time+curved_uv.y*size.y*1.5), 0.0, 1.0);\n"
        "    float s = pow(scans,0.9);\n"
        "    col = col * vec3(s);\n"
        "\n"
        "    /* Vertical lines (shadow mask) */\n"
        "    col*=1.0-0.23*(clamp((mod(gl_FragCoord.xy.x, 3.0))/2.0,0.0,1.0));\n"
        "\n"
        "    /* Tone map */\n"
        "    col = filmic( col );\n"
        "\n"
        "    /* Noise */\n"
        "    /*vec2 seed = floor(curved_uv*resolution.xy*vec2(0.5))/resolution.xy;*/\n"
        "    vec2 seed = curved_uv*resolution.xy;;\n"
        "    /* seed = curved_uv; */\n"
        "    col -= 0.015*pow(vec3(rand( seed +time ), rand( seed +time*2.0 ), rand( seed +time * 3.0 ) ), vec3(1.5) );\n"
        "\n"
        "    /* Flicker */\n"
        "    col *= (1.0-0.004*(sin(50.0*time+curved_uv.y*2.0)*0.5+0.5));\n"
        "\n"
        "    /* Clamp */\n"
        "    if (curved_uv.x < 0.0 || curved_uv.x > 1.0)\n"
        "        col *= 0.0;\n"
        "    if (curved_uv.y < 0.0 || curved_uv.y > 1.0)\n"
        "        col *= 0.0;\n"
        "    col *= modulate;\n"
        "    /* Frame */\n"
        "    vec2 fuv=vec2( uv.x, 1.0 - uv.y);\n"
        "    vec4 f=texture2D(frametexture,fuv);\n"
        "    vec3 fr = mix( max( col, 0.0), f.xyz, f.w);\n"
        "    col = mix( col, fr, vec3( use_frame ) );\n"
        "    \n"
        "    gl_FragColor = vec4( col, 1.0 );\n"
        "    }\n"
        "\n";

    char const* blur_fs_source =
        #ifdef CRTEMU_WEBGL
            "precision highp float;\n\n"
        #else
            "#version 120\n\n"
        #endif
        ""
        "varying vec2 uv;"
        ""
        "uniform vec2 blur;"
        "uniform sampler2D texture;"
        ""
        "void main( void )"
        "    {"
        "    vec4 sum = texture2D( texture, uv ) * 0.2270270270;"
        "    sum += texture2D(texture, vec2( uv.x - 4.0 * blur.x, uv.y - 4.0 * blur.y ) ) * 0.0162162162;"
        "    sum += texture2D(texture, vec2( uv.x - 3.0 * blur.x, uv.y - 3.0 * blur.y ) ) * 0.0540540541;"
        "    sum += texture2D(texture, vec2( uv.x - 2.0 * blur.x, uv.y - 2.0 * blur.y ) ) * 0.1216216216;"
        "    sum += texture2D(texture, vec2( uv.x - 1.0 * blur.x, uv.y - 1.0 * blur.y ) ) * 0.1945945946;"
        "    sum += texture2D(texture, vec2( uv.x + 1.0 * blur.x, uv.y + 1.0 * blur.y ) ) * 0.1945945946;"
        "    sum += texture2D(texture, vec2( uv.x + 2.0 * blur.x, uv.y + 2.0 * blur.y ) ) * 0.1216216216;"
        "    sum += texture2D(texture, vec2( uv.x + 3.0 * blur.x, uv.y + 3.0 * blur.y ) ) * 0.0540540541;"
        "    sum += texture2D(texture, vec2( uv.x + 4.0 * blur.x, uv.y + 4.0 * blur.y ) ) * 0.0162162162;"
        "    gl_FragColor = sum;"
        "    }   "
        "";


    char const* accumulate_fs_source =
        #ifdef CRTEMU_WEBGL
            "precision highp float;\n\n"
        #else
            "#version 120\n\n"
        #endif
        ""
        "varying vec2 uv;"
        ""
        "uniform sampler2D tex0;"
        "uniform sampler2D tex1;"
        "uniform float modulate;"
        ""
        "void main( void )"
        "    {"
        "    vec4 a = texture2D( tex0, uv ) * vec4( modulate );"
        "    vec4 b = texture2D( tex1, uv );"
        ""
        "    gl_FragColor = max( a, b * 0.96 );"
        "    }   "
        "";

    char const* blend_fs_source =
        #ifdef CRTEMU_WEBGL
            "precision highp float;\n\n"
        #else
            "#version 120\n\n"
        #endif
        ""
        "varying vec2 uv;"
        ""
        "uniform sampler2D tex0;"
        "uniform sampler2D tex1;"
        "uniform float modulate;"
        ""
        "void main( void )"
        "    {"
        "    vec4 a = texture2D( tex0, uv ) * vec4( modulate );"
        "    vec4 b = texture2D( tex1, uv );"
        ""
        "    gl_FragColor = max( a, b * 0.32 );"
        "    }   "
        "";

    char const* copy_fs_source =
        #ifdef CRTEMU_WEBGL
            "precision highp float;\n\n"
        #else
            "#version 120\n\n"
        #endif
        ""
        "varying vec2 uv;"
        ""
        "uniform sampler2D tex0;"
        ""
        "void main( void )"
        "    {"
        "    gl_FragColor = texture2D( tex0, uv );"
        "    }   "
        "";

    crtemu_t* crtemu = (crtemu_t*) CRTEMU_MALLOC( memctx, sizeof( crtemu_t ) );
    memset( crtemu, 0, sizeof( crtemu_t ) );
    crtemu->memctx = memctx;

    crtemu->config.curvature = 0.0f;
    crtemu->config.scanlines = 0.0f;
    crtemu->config.shadow_mask = 0.0f;
    crtemu->config.separation = 0.0f;
    crtemu->config.ghosting = 0.0f;
    crtemu->config.noise = 0.0f;
    crtemu->config.flicker = 0.0f;
    crtemu->config.vignette = 0.0f;
    crtemu->config.distortion = 0.0f;
    crtemu->config.aspect_lock = 0.0f;
    crtemu->config.hpos = 0.0f;
    crtemu->config.vpos = 0.0f;
    crtemu->config.hsize = 0.0f;
    crtemu->config.vsize = 0.0f;
    crtemu->config.contrast = 0.0f;
    crtemu->config.brightness = 0.0f;
    crtemu->config.saturation = 0.0f;
    crtemu->config.blur = 0.0f;
    crtemu->config.degauss = 0.0f;

    crtemu->use_frame = 0.0f;

    crtemu->last_present_width = 0;
    crtemu->last_present_height = 0;

    #ifndef CRTEMU_SDL

        crtemu->gl_dll = LoadLibraryA( "opengl32.dll" );
        if( !crtemu->gl_dll ) goto failed;

        crtemu->wglGetProcAddress = (CRTEMU_PROC (CRTEMU_GLCALLTYPE*)(char const*)) (uintptr_t) GetProcAddress( crtemu->gl_dll, "wglGetProcAddress" );
        if( !crtemu->gl_dll ) goto failed;

        // Attempt to bind opengl functions using GetProcAddress
        crtemu->TexParameterfv = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLenum, CRTEMU_GLfloat const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glTexParameterfv" );
        crtemu->DeleteFramebuffers = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glDeleteFramebuffers" );
        crtemu->GetIntegerv = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLint *) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glGetIntegerv" );
        crtemu->GenFramebuffers = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint *) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glGenFramebuffers" );
        crtemu->BindFramebuffer = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glBindFramebuffer" );
        crtemu->Uniform1f = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLfloat) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glUniform1f" );
        crtemu->Uniform2f = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLfloat, CRTEMU_GLfloat) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glUniform2f" );
        crtemu->FramebufferTexture2D = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLenum, CRTEMU_GLenum, CRTEMU_GLuint, CRTEMU_GLint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glFramebufferTexture2D" );
        crtemu->CreateShader = ( CRTEMU_GLuint (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glCreateShader" );
        crtemu->ShaderSource = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLsizei, CRTEMU_GLchar const* const*, CRTEMU_GLint const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glShaderSource" );
        crtemu->CompileShader = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glCompileShader" );
        crtemu->GetShaderiv = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLenum, CRTEMU_GLint*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glGetShaderiv" );
        crtemu->CreateProgram = ( CRTEMU_GLuint (CRTEMU_GLCALLTYPE*) (void) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glCreateProgram" );
        crtemu->AttachShader = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glAttachShader" );
        crtemu->BindAttribLocation = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLuint, CRTEMU_GLchar const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glBindAttribLocation" );
        crtemu->LinkProgram = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glLinkProgram" );
        crtemu->GetProgramiv = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLenum, CRTEMU_GLint*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glGetProgramiv" );
        crtemu->GenBuffers = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glGenBuffers" );
        crtemu->BindBuffer = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glBindBuffer" );
        crtemu->EnableVertexAttribArray = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glEnableVertexAttribArray" );
        crtemu->VertexAttribPointer = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLint, CRTEMU_GLenum, CRTEMU_GLboolean, CRTEMU_GLsizei, void const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glVertexAttribPointer" );
        crtemu->GenTextures = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glGenTextures" );
        crtemu->Enable = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glEnable" );
        crtemu->ActiveTexture = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glActiveTexture" );
        crtemu->BindTexture = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glBindTexture" );
        crtemu->TexParameteri = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLenum, CRTEMU_GLint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glTexParameteri" );
        crtemu->DeleteBuffers = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glDeleteBuffers" );
        crtemu->DeleteTextures = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glDeleteTextures" );
        crtemu->BufferData = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLsizeiptr, void const *, CRTEMU_GLenum) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glBufferData" );
        crtemu->UseProgram = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glUseProgram" );
        crtemu->Uniform1i = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glUniform1i" );
        crtemu->Uniform3f = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLfloat, CRTEMU_GLfloat, CRTEMU_GLfloat) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glUniform3f" );
        crtemu->GetUniformLocation = ( CRTEMU_GLint (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLchar const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glGetUniformLocation" );
        crtemu->TexImage2D = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLint, CRTEMU_GLint, CRTEMU_GLsizei, CRTEMU_GLsizei, CRTEMU_GLint, CRTEMU_GLenum, CRTEMU_GLenum, void const*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glTexImage2D" );
        crtemu->ClearColor = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLfloat, CRTEMU_GLfloat, CRTEMU_GLfloat, CRTEMU_GLfloat) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glClearColor" );
        crtemu->Clear = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLbitfield) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glClear" );
        crtemu->DrawArrays = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLint, CRTEMU_GLsizei) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glDrawArrays" );
        crtemu->Viewport = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLint, CRTEMU_GLsizei, CRTEMU_GLsizei) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glViewport" );
        crtemu->DeleteShader = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glDeleteShader" );
        crtemu->DeleteProgram = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glDeleteProgram" );
        #ifdef CRTEMU_REPORT_SHADER_ERRORS
            crtemu->GetShaderInfoLog = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLsizei, CRTEMU_GLsizei*, CRTEMU_GLchar*) ) (uintptr_t) GetProcAddress( crtemu->gl_dll, "glGetShaderInfoLog" );
        #endif

        // Any opengl functions which didn't bind, try binding them using wglGetProcAddrss
        if( !crtemu->TexParameterfv ) crtemu->TexParameterfv = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLenum, CRTEMU_GLfloat const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glTexParameterfv" );
        if( !crtemu->DeleteFramebuffers ) crtemu->DeleteFramebuffers = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glDeleteFramebuffers" );
        if( !crtemu->GetIntegerv ) crtemu->GetIntegerv = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLint *) ) (uintptr_t) crtemu->wglGetProcAddress( "glGetIntegerv" );
        if( !crtemu->GenFramebuffers ) crtemu->GenFramebuffers = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint *) ) (uintptr_t) crtemu->wglGetProcAddress( "glGenFramebuffers" );
        if( !crtemu->BindFramebuffer ) crtemu->BindFramebuffer = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glBindFramebuffer" );
        if( !crtemu->Uniform1f ) crtemu->Uniform1f = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLfloat) ) (uintptr_t) crtemu->wglGetProcAddress( "glUniform1f" );
        if( !crtemu->Uniform2f ) crtemu->Uniform2f = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLfloat, CRTEMU_GLfloat) ) (uintptr_t) crtemu->wglGetProcAddress( "glUniform2f" );
        if( !crtemu->FramebufferTexture2D ) crtemu->FramebufferTexture2D = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLenum, CRTEMU_GLenum, CRTEMU_GLuint, CRTEMU_GLint) ) (uintptr_t) crtemu->wglGetProcAddress( "glFramebufferTexture2D" );
        if( !crtemu->CreateShader ) crtemu->CreateShader = ( CRTEMU_GLuint (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum) ) (uintptr_t) crtemu->wglGetProcAddress( "glCreateShader" );
        if( !crtemu->ShaderSource ) crtemu->ShaderSource = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLsizei, CRTEMU_GLchar const* const*, CRTEMU_GLint const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glShaderSource" );
        if( !crtemu->CompileShader ) crtemu->CompileShader = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glCompileShader" );
        if( !crtemu->GetShaderiv ) crtemu->GetShaderiv = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLenum, CRTEMU_GLint*) ) (uintptr_t) crtemu->wglGetProcAddress( "glGetShaderiv" );
        if( !crtemu->CreateProgram ) crtemu->CreateProgram = ( CRTEMU_GLuint (CRTEMU_GLCALLTYPE*) (void) ) (uintptr_t) crtemu->wglGetProcAddress( "glCreateProgram" );
        if( !crtemu->AttachShader ) crtemu->AttachShader = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glAttachShader" );
        if( !crtemu->BindAttribLocation ) crtemu->BindAttribLocation = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLuint, CRTEMU_GLchar const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glBindAttribLocation" );
        if( !crtemu->LinkProgram ) crtemu->LinkProgram = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glLinkProgram" );
        if( !crtemu->GetProgramiv ) crtemu->GetProgramiv = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLenum, CRTEMU_GLint*) ) (uintptr_t) crtemu->wglGetProcAddress( "glGetProgramiv" );
        if( !crtemu->GenBuffers ) crtemu->GenBuffers = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint*) ) (uintptr_t) crtemu->wglGetProcAddress( "glGenBuffers" );
        if( !crtemu->BindBuffer ) crtemu->BindBuffer = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glBindBuffer" );
        if( !crtemu->EnableVertexAttribArray ) crtemu->EnableVertexAttribArray = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glEnableVertexAttribArray" );
        if( !crtemu->VertexAttribPointer ) crtemu->VertexAttribPointer = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLint, CRTEMU_GLenum, CRTEMU_GLboolean, CRTEMU_GLsizei, void const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glVertexAttribPointer" );
        if( !crtemu->GenTextures ) crtemu->GenTextures = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint*) ) (uintptr_t) crtemu->wglGetProcAddress( "glGenTextures" );
        if( !crtemu->Enable ) crtemu->Enable = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum) ) (uintptr_t) crtemu->wglGetProcAddress( "glEnable" );
        if( !crtemu->ActiveTexture ) crtemu->ActiveTexture = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum) ) (uintptr_t) crtemu->wglGetProcAddress( "glActiveTexture" );
        if( !crtemu->BindTexture ) crtemu->BindTexture = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glBindTexture" );
        if( !crtemu->TexParameteri ) crtemu->TexParameteri = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLenum, CRTEMU_GLint) ) (uintptr_t) crtemu->wglGetProcAddress( "glTexParameteri" );
        if( !crtemu->DeleteBuffers ) crtemu->DeleteBuffers = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glDeleteBuffers" );
        if( !crtemu->DeleteTextures ) crtemu->DeleteTextures = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLsizei, CRTEMU_GLuint const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glDeleteTextures" );
        if( !crtemu->BufferData ) crtemu->BufferData = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLsizeiptr, void const *, CRTEMU_GLenum) ) (uintptr_t) crtemu->wglGetProcAddress( "glBufferData" );
        if( !crtemu->UseProgram ) crtemu->UseProgram = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glUseProgram" );
        if( !crtemu->Uniform1i ) crtemu->Uniform1i = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLint) ) (uintptr_t) crtemu->wglGetProcAddress( "glUniform1i" );
        if( !crtemu->Uniform3f ) crtemu->Uniform3f = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLfloat, CRTEMU_GLfloat, CRTEMU_GLfloat) ) (uintptr_t) crtemu->wglGetProcAddress( "glUniform3f" );
        if( !crtemu->GetUniformLocation ) crtemu->GetUniformLocation = ( CRTEMU_GLint (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLchar const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glGetUniformLocation" );
        if( !crtemu->TexImage2D ) crtemu->TexImage2D = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLint, CRTEMU_GLint, CRTEMU_GLsizei, CRTEMU_GLsizei, CRTEMU_GLint, CRTEMU_GLenum, CRTEMU_GLenum, void const*) ) (uintptr_t) crtemu->wglGetProcAddress( "glTexImage2D" );
        if( !crtemu->ClearColor ) crtemu->ClearColor = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLfloat, CRTEMU_GLfloat, CRTEMU_GLfloat, CRTEMU_GLfloat) ) (uintptr_t) crtemu->wglGetProcAddress( "glClearColor" );
        if( !crtemu->Clear ) crtemu->Clear = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLbitfield) ) (uintptr_t) crtemu->wglGetProcAddress( "glClear" );
        if( !crtemu->DrawArrays ) crtemu->DrawArrays = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLenum, CRTEMU_GLint, CRTEMU_GLsizei) ) (uintptr_t) crtemu->wglGetProcAddress( "glDrawArrays" );
        if( !crtemu->Viewport ) crtemu->Viewport = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLint, CRTEMU_GLint, CRTEMU_GLsizei, CRTEMU_GLsizei) ) (uintptr_t) crtemu->wglGetProcAddress( "glViewport" );
        if( !crtemu->DeleteShader ) crtemu->DeleteShader = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glDeleteShader" );
        if( !crtemu->DeleteProgram ) crtemu->DeleteProgram = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint) ) (uintptr_t) crtemu->wglGetProcAddress( "glDeleteProgram" );
        #ifdef CRTEMU_REPORT_SHADER_ERRORS
            if( !crtemu->GetShaderInfoLog ) crtemu->GetShaderInfoLog = ( void (CRTEMU_GLCALLTYPE*) (CRTEMU_GLuint, CRTEMU_GLsizei, CRTEMU_GLsizei*, CRTEMU_GLchar*) ) (uintptr_t) crtemu->wglGetProcAddress( "glGetShaderInfoLog" );
        #endif

    #else

         crtemu->TexParameterfv = glTexParameterfv;
         crtemu->DeleteFramebuffers = glDeleteFramebuffers;
         crtemu->GetIntegerv = glGetIntegerv;
         crtemu->GenFramebuffers = glGenFramebuffers;
         crtemu->BindFramebuffer = glBindFramebuffer;
         crtemu->Uniform1f = glUniform1f;
         crtemu->Uniform2f = glUniform2f;
         crtemu->FramebufferTexture2D = glFramebufferTexture2D;
         crtemu->CreateShader = glCreateShader;
         crtemu->ShaderSource = glShaderSource;
         crtemu->CompileShader = glCompileShader;
         crtemu->GetShaderiv = glGetShaderiv;
         crtemu->CreateProgram = glCreateProgram;
         crtemu->AttachShader = glAttachShader;
         crtemu->BindAttribLocation = glBindAttribLocation;
         crtemu->LinkProgram = glLinkProgram;
         crtemu->GetProgramiv = glGetProgramiv;
         crtemu->GenBuffers = glGenBuffers;
         crtemu->BindBuffer = glBindBuffer;
         crtemu->EnableVertexAttribArray = glEnableVertexAttribArray;
         crtemu->VertexAttribPointer = glVertexAttribPointer;
         crtemu->GenTextures = glGenTextures;
         crtemu->Enable = glEnable;
         crtemu->ActiveTexture = glActiveTexture;
         crtemu->BindTexture = glBindTexture;
         crtemu->TexParameteri = glTexParameteri;
         crtemu->DeleteBuffers = glDeleteBuffers;
         crtemu->DeleteTextures = glDeleteTextures;
         crtemu->BufferData = glBufferData;
         crtemu->UseProgram = glUseProgram;
         crtemu->Uniform1i = glUniform1i;
         crtemu->Uniform3f = glUniform3f;
         crtemu->GetUniformLocation = glGetUniformLocation;
         crtemu->TexImage2D = glTexImage2D;
         crtemu->ClearColor = glClearColor;
         crtemu->Clear = glClear;
         crtemu->DrawArrays = glDrawArrays;
         crtemu->Viewport = glViewport;
         crtemu->DeleteShader = glDeleteShader;
         crtemu->DeleteProgram = glDeleteProgram;
         #ifdef CRTEMU_REPORT_SHADER_ERRORS
            crtemu->GetShaderInfoLog = glGetShaderInfoLog;
         #endif

    #endif

    // Report error if any gl function was not found.
    if( !crtemu->TexParameterfv ) goto failed;
    if( !crtemu->DeleteFramebuffers ) goto failed;
    if( !crtemu->GetIntegerv ) goto failed;
    if( !crtemu->GenFramebuffers ) goto failed;
    if( !crtemu->BindFramebuffer ) goto failed;
    if( !crtemu->Uniform1f ) goto failed;
    if( !crtemu->Uniform2f ) goto failed;
    if( !crtemu->FramebufferTexture2D ) goto failed;
    if( !crtemu->CreateShader ) goto failed;
    if( !crtemu->ShaderSource ) goto failed;
    if( !crtemu->CompileShader ) goto failed;
    if( !crtemu->GetShaderiv ) goto failed;
    if( !crtemu->CreateProgram ) goto failed;
    if( !crtemu->AttachShader ) goto failed;
    if( !crtemu->BindAttribLocation ) goto failed;
    if( !crtemu->LinkProgram ) goto failed;
    if( !crtemu->GetProgramiv ) goto failed;
    if( !crtemu->GenBuffers ) goto failed;
    if( !crtemu->BindBuffer ) goto failed;
    if( !crtemu->EnableVertexAttribArray ) goto failed;
    if( !crtemu->VertexAttribPointer ) goto failed;
    if( !crtemu->GenTextures ) goto failed;
    if( !crtemu->Enable ) goto failed;
    if( !crtemu->ActiveTexture ) goto failed;
    if( !crtemu->BindTexture ) goto failed;
    if( !crtemu->TexParameteri ) goto failed;
    if( !crtemu->DeleteBuffers ) goto failed;
    if( !crtemu->DeleteTextures ) goto failed;
    if( !crtemu->BufferData ) goto failed;
    if( !crtemu->UseProgram ) goto failed;
    if( !crtemu->Uniform1i ) goto failed;
    if( !crtemu->Uniform3f ) goto failed;
    if( !crtemu->GetUniformLocation ) goto failed;
    if( !crtemu->TexImage2D ) goto failed;
    if( !crtemu->ClearColor ) goto failed;
    if( !crtemu->Clear ) goto failed;
    if( !crtemu->DrawArrays ) goto failed;
    if( !crtemu->Viewport ) goto failed;
    if( !crtemu->DeleteShader ) goto failed;
    if( !crtemu->DeleteProgram ) goto failed;
    #ifdef CRTEMU_REPORT_SHADER_ERRORS
        if( !crtemu->GetShaderInfoLog ) goto failed;
    #endif

    crtemu->crt_shader = crtemu_internal_build_shader( crtemu, vs_source, crt_fs_source );
    if( crtemu->crt_shader == 0 ) goto failed;

    crtemu->blur_shader = crtemu_internal_build_shader( crtemu, vs_source, blur_fs_source );
    if( crtemu->blur_shader == 0 ) goto failed;

    crtemu->accumulate_shader = crtemu_internal_build_shader( crtemu, vs_source, accumulate_fs_source );
    if( crtemu->accumulate_shader == 0 ) goto failed;

    crtemu->blend_shader = crtemu_internal_build_shader( crtemu, vs_source, blend_fs_source );
    if( crtemu->blend_shader == 0 ) goto failed;

    crtemu->copy_shader = crtemu_internal_build_shader( crtemu, vs_source, copy_fs_source );
    if( crtemu->copy_shader == 0 ) goto failed;


    crtemu->GenTextures( 1, &crtemu->accumulatetexture_a );
    crtemu->GenFramebuffers( 1, &crtemu->accumulatebuffer_a );

    crtemu->GenTextures( 1, &crtemu->accumulatetexture_b );
    crtemu->GenFramebuffers( 1, &crtemu->accumulatebuffer_b );

    crtemu->GenTextures( 1, &crtemu->blurtexture_a );
    crtemu->GenFramebuffers( 1, &crtemu->blurbuffer_a );

    crtemu->GenTextures( 1, &crtemu->blurtexture_b );
    crtemu->GenFramebuffers( 1, &crtemu->blurbuffer_b );

    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );

    crtemu->GenTextures( 1, &crtemu->frametexture );
    #ifndef CRTEMU_WEBGL
        // This enable call is not necessary when using fragment shaders, avoid logged warnings in WebGL
        crtemu->Enable( CRTEMU_GL_TEXTURE_2D );
    #endif
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE2 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->frametexture );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );

    crtemu->GenTextures( 1, &crtemu->backbuffer );
    #ifndef CRTEMU_WEBGL
        // This enable call is not necessary when using fragment shaders, avoid logged warnings in WebGL
        crtemu->Enable( CRTEMU_GL_TEXTURE_2D );
    #endif
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->backbuffer );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_NEAREST );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_NEAREST );

    crtemu->GenBuffers( 1, &crtemu->vertexbuffer );
    crtemu->BindBuffer( CRTEMU_GL_ARRAY_BUFFER, crtemu->vertexbuffer );
    crtemu->EnableVertexAttribArray( 0 );
    crtemu->VertexAttribPointer( 0, 4, CRTEMU_GL_FLOAT, CRTEMU_GL_FALSE, 4 * sizeof( CRTEMU_GLfloat ), 0 );

    #ifdef CRTEMU_WEBGL
        // Avoid WebGL error "TEXTURE_2D at unit 0 is incomplete: Non-power-of-two textures must have a wrap mode of CLAMP_TO_EDGE."
        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_a );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_b );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->blurtexture_a );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->blurtexture_b );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->frametexture );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->backbuffer );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    #endif

    return crtemu;

failed:
    if( crtemu->accumulatetexture_a ) crtemu->DeleteTextures( 1, &crtemu->accumulatetexture_a );
    if( crtemu->accumulatebuffer_a ) crtemu->DeleteFramebuffers( 1, &crtemu->accumulatebuffer_a );
    if( crtemu->accumulatetexture_b ) crtemu->DeleteTextures( 1, &crtemu->accumulatetexture_b );
    if( crtemu->accumulatebuffer_b ) crtemu->DeleteFramebuffers( 1, &crtemu->accumulatebuffer_b );
    if( crtemu->blurtexture_a ) crtemu->DeleteTextures( 1, &crtemu->blurtexture_a );
    if( crtemu->blurbuffer_a ) crtemu->DeleteFramebuffers( 1, &crtemu->blurbuffer_a );
    if( crtemu->blurtexture_b ) crtemu->DeleteTextures( 1, &crtemu->blurtexture_b );
    if( crtemu->blurbuffer_b ) crtemu->DeleteFramebuffers( 1, &crtemu->blurbuffer_b );
    if( crtemu->frametexture ) crtemu->DeleteTextures( 1, &crtemu->frametexture );
    if( crtemu->backbuffer ) crtemu->DeleteTextures( 1, &crtemu->backbuffer );
    if( crtemu->vertexbuffer ) crtemu->DeleteBuffers( 1, &crtemu->vertexbuffer );

    #ifndef CRTEMU_SDL
        if( crtemu->gl_dll ) FreeLibrary( crtemu->gl_dll );
    #endif
    CRTEMU_FREE( crtemu->memctx, crtemu );
    return 0;
    }


void crtemu_destroy( crtemu_t* crtemu )
    {
    crtemu->DeleteTextures( 1, &crtemu->accumulatetexture_a );
    crtemu->DeleteFramebuffers( 1, &crtemu->accumulatebuffer_a );
    crtemu->DeleteTextures( 1, &crtemu->accumulatetexture_b );
    crtemu->DeleteFramebuffers( 1, &crtemu->accumulatebuffer_b );
    crtemu->DeleteTextures( 1, &crtemu->blurtexture_a );
    crtemu->DeleteFramebuffers( 1, &crtemu->blurbuffer_a );
    crtemu->DeleteTextures( 1, &crtemu->blurtexture_b );
    crtemu->DeleteFramebuffers( 1, &crtemu->blurbuffer_b );
    crtemu->DeleteTextures( 1, &crtemu->frametexture );
    crtemu->DeleteTextures( 1, &crtemu->backbuffer );
    crtemu->DeleteBuffers( 1, &crtemu->vertexbuffer );
    #ifndef CRTEMU_SDL
        FreeLibrary( crtemu->gl_dll );
    #endif
    CRTEMU_FREE( crtemu->memctx, crtemu );
    }


void crtemu_config(crtemu_t* crtemu, crtemu_config_t const* config)
    {
    crtemu->config = *config;
    }


void crtemu_frame( crtemu_t* crtemu, CRTEMU_U32* frame_abgr, int frame_width, int frame_height )
    {
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE3 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->frametexture );
    crtemu->TexImage2D( CRTEMU_GL_TEXTURE_2D, 0, CRTEMU_GL_RGBA, frame_width, frame_height, 0, CRTEMU_GL_RGBA, CRTEMU_GL_UNSIGNED_BYTE, frame_abgr );
    if( frame_abgr )
        crtemu->use_frame = 1.0f;
    else
        crtemu->use_frame = 0.0f;
    }


static void crtemu_internal_blur( crtemu_t* crtemu, CRTEMU_GLuint source, CRTEMU_GLuint blurbuffer_a, CRTEMU_GLuint blurbuffer_b,
    CRTEMU_GLuint blurtexture_b, float r, int width, int height )
    {
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, blurbuffer_b );
    crtemu->UseProgram( crtemu->blur_shader );
    crtemu->Uniform2f( crtemu->GetUniformLocation( crtemu->blur_shader, "blur" ), r / (float) width, 0 );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->blur_shader, "texture" ), 0 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, source );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );
    crtemu->DrawArrays( CRTEMU_GL_TRIANGLE_FAN, 0, 4 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );

    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, blurbuffer_a );
    crtemu->UseProgram( crtemu->blur_shader );
    crtemu->Uniform2f( crtemu->GetUniformLocation( crtemu->blur_shader, "blur" ), 0, r / (float) height );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->blur_shader, "texture" ), 0 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, blurtexture_b );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );
    crtemu->DrawArrays( CRTEMU_GL_TRIANGLE_FAN, 0, 4 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );
    }


void crtemu_present( crtemu_t* crtemu, CRTEMU_U64 time_us, CRTEMU_U32 const* pixels_xbgr, int width, int height,
    CRTEMU_U32 mod_xbgr, CRTEMU_U32 border_xbgr )
    {
    int viewport[ 4 ];
    crtemu->GetIntegerv( CRTEMU_GL_VIEWPORT, viewport );

    if( width != crtemu->last_present_width || height != crtemu->last_present_height )
        {
        crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );

        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_a );
        crtemu->TexImage2D( CRTEMU_GL_TEXTURE_2D, 0, CRTEMU_GL_RGB, width, height, 0, CRTEMU_GL_RGB, CRTEMU_GL_UNSIGNED_BYTE, 0 );
        crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, crtemu->accumulatebuffer_a );
        crtemu->FramebufferTexture2D( CRTEMU_GL_FRAMEBUFFER, CRTEMU_GL_COLOR_ATTACHMENT0, CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_a, 0 );
        crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );

        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_b );
        crtemu->TexImage2D( CRTEMU_GL_TEXTURE_2D, 0, CRTEMU_GL_RGB, width, height, 0, CRTEMU_GL_RGB, CRTEMU_GL_UNSIGNED_BYTE, 0 );
        crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, crtemu->accumulatebuffer_b );
        crtemu->FramebufferTexture2D( CRTEMU_GL_FRAMEBUFFER, CRTEMU_GL_COLOR_ATTACHMENT0, CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_b, 0 );
        crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );

        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->blurtexture_a );
        crtemu->TexImage2D( CRTEMU_GL_TEXTURE_2D, 0, CRTEMU_GL_RGB, width, height, 0, CRTEMU_GL_RGB, CRTEMU_GL_UNSIGNED_BYTE, 0 );
        crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, crtemu->blurbuffer_a );
        crtemu->FramebufferTexture2D( CRTEMU_GL_FRAMEBUFFER, CRTEMU_GL_COLOR_ATTACHMENT0, CRTEMU_GL_TEXTURE_2D, crtemu->blurtexture_a, 0 );
        crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );

        crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->blurtexture_b );
        crtemu->TexImage2D( CRTEMU_GL_TEXTURE_2D, 0, CRTEMU_GL_RGB, width, height, 0, CRTEMU_GL_RGB, CRTEMU_GL_UNSIGNED_BYTE, 0 );
        crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, crtemu->blurbuffer_b );
        crtemu->FramebufferTexture2D( CRTEMU_GL_FRAMEBUFFER, CRTEMU_GL_COLOR_ATTACHMENT0, CRTEMU_GL_TEXTURE_2D, crtemu->blurtexture_b, 0 );
        crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );
        }


    crtemu->last_present_width = width;
    crtemu->last_present_height = height;

    CRTEMU_GLfloat vertices[] =
        {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
        };
    crtemu->BufferData( CRTEMU_GL_ARRAY_BUFFER, 4 * 4 * sizeof( CRTEMU_GLfloat ), vertices, CRTEMU_GL_STATIC_DRAW );
    crtemu->BindBuffer( CRTEMU_GL_ARRAY_BUFFER, crtemu->vertexbuffer );

    // Copy to backbuffer
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->backbuffer );
    crtemu->TexImage2D( CRTEMU_GL_TEXTURE_2D, 0, CRTEMU_GL_RGBA, width, height, 0, CRTEMU_GL_RGBA, CRTEMU_GL_UNSIGNED_BYTE, pixels_xbgr );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );

    crtemu->Viewport( 0, 0, width, height );

    // Blur the previous accumulation buffer
    crtemu_internal_blur( crtemu, crtemu->accumulatetexture_b, crtemu->blurbuffer_a, crtemu->blurbuffer_b, crtemu->blurtexture_b, 1.0f, width, height );

    // Update accumulation buffer
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, crtemu->accumulatebuffer_a );
    crtemu->UseProgram( crtemu->accumulate_shader );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->accumulate_shader, "tex0" ), 0 );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->accumulate_shader, "tex1" ), 1 );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->accumulate_shader, "modulate" ), 1.0f );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->backbuffer );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE1 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->blurtexture_a );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );
    crtemu->DrawArrays( CRTEMU_GL_TRIANGLE_FAN, 0, 4 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE1 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );


    // Store a copy of the accumulation buffer
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, crtemu->accumulatebuffer_b );
    crtemu->UseProgram( crtemu->copy_shader );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->copy_shader, "tex0" ), 0 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_a );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );
    crtemu->DrawArrays( CRTEMU_GL_TRIANGLE_FAN, 0, 4 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );

    // Blend accumulation and backbuffer
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, crtemu->accumulatebuffer_a );
    crtemu->UseProgram( crtemu->blend_shader );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->blend_shader, "tex0" ), 0 );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->blend_shader, "tex1" ), 1 );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->blend_shader, "modulate" ), 1.0f );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->backbuffer );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE1 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_b );
    crtemu->DrawArrays( CRTEMU_GL_TRIANGLE_FAN, 0, 4 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );
    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE1 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );


    // Add slight blur to backbuffer
    crtemu_internal_blur( crtemu, crtemu->accumulatetexture_a, crtemu->accumulatebuffer_a, crtemu->blurbuffer_b, crtemu->blurtexture_b, 0.17f, width, height );

    // Create fully blurred version of backbuffer
    crtemu_internal_blur( crtemu, crtemu->accumulatetexture_a, crtemu->blurbuffer_a, crtemu->blurbuffer_b, crtemu->blurtexture_b, 1.0f, width, height );


    // Present to screen with CRT shader
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );

    crtemu->Viewport( viewport[ 0 ], viewport[ 1 ], viewport[ 2 ], viewport[ 3 ] );

    int window_width = viewport[ 2 ] - viewport[ 0 ];
    int window_height = viewport[ 3 ] - viewport[ 1 ];

    int aspect_width = (int)( ( window_height * 4 ) / 3 );
    int aspect_height= (int)( ( window_width * 3 ) / 4 );
    int target_width, target_height;
    if( aspect_height <= window_height )
        {
        target_width = window_width;
        target_height = aspect_height;
        }
    else
        {
        target_width = aspect_width;
        target_height = window_height;
        }

    float hscale = target_width / (float) width;
    float vscale = target_height / (float) height;

    float hborder = ( window_width - hscale * width ) / 2.0f;
    float vborder = ( window_height - vscale * height ) / 2.0f;
    float x1 = hborder;
    float y1 = vborder;
    float x2 = x1 + hscale * width;
    float y2 = y1 + vscale * height;

    x1 = ( x1 / window_width ) * 2.0f - 1.0f;
    x2 = ( x2 / window_width ) * 2.0f - 1.0f;
    y1 = ( y1 / window_height ) * 2.0f - 1.0f;
    y2 = ( y2 / window_height ) * 2.0f - 1.0f;

    CRTEMU_GLfloat screen_vertices[] =
        {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        };
    screen_vertices[  0 ] = x1;
    screen_vertices[  1 ] = y1;
    screen_vertices[  4 ] = x2;
    screen_vertices[  5 ] = y1;
    screen_vertices[  8 ] = x2;
    screen_vertices[  9 ] = y2;
    screen_vertices[ 12 ] = x1;
    screen_vertices[ 13 ] = y2;

    crtemu->BufferData( CRTEMU_GL_ARRAY_BUFFER, 4 * 4 * sizeof( CRTEMU_GLfloat ), screen_vertices, CRTEMU_GL_STATIC_DRAW );
    crtemu->BindBuffer( CRTEMU_GL_ARRAY_BUFFER, crtemu->vertexbuffer );

    float r = ( ( border_xbgr >> 16 ) & 0xff ) / 255.0f;
    float g = ( ( border_xbgr >> 8  ) & 0xff ) / 255.0f;
    float b = ( ( border_xbgr       ) & 0xff ) / 255.0f;
    crtemu->ClearColor( r, g, b, 1.0f );
    crtemu->Clear( CRTEMU_GL_COLOR_BUFFER_BIT );

    crtemu->UseProgram( crtemu->crt_shader );

    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->crt_shader, "backbuffer" ), 0 );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->crt_shader, "blurbuffer" ), 1 );
    crtemu->Uniform1i( crtemu->GetUniformLocation( crtemu->crt_shader, "frametexture" ), 2 );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "use_frame" ), crtemu->use_frame );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "time" ), 1.5f * (CRTEMU_GLfloat)( ( (double) time_us ) / 1000000.0 ) );
    crtemu->Uniform2f( crtemu->GetUniformLocation( crtemu->crt_shader, "resolution" ), (float) window_width, (float) window_height );
    crtemu->Uniform2f( crtemu->GetUniformLocation( crtemu->crt_shader, "size" ), (float) target_width, (float) target_height );

    float mod_r = ( ( mod_xbgr >> 16 ) & 0xff ) / 255.0f;
    float mod_g = ( ( mod_xbgr >> 8  ) & 0xff ) / 255.0f;
    float mod_b = ( ( mod_xbgr       ) & 0xff ) / 255.0f;
    crtemu->Uniform3f( crtemu->GetUniformLocation( crtemu->crt_shader, "modulate" ), mod_r, mod_g, mod_b );

    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_curvature" ), crtemu->config.curvature );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_scanlines" ), crtemu->config.scanlines );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_shadow_mask" ), crtemu->config.shadow_mask );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_separation" ), crtemu->config.separation );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_ghosting" ), crtemu->config.ghosting );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_noise" ), crtemu->config.noise );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_flicker" ), crtemu->config.flicker );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_vignette" ), crtemu->config.vignette );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_distortion" ), crtemu->config.distortion );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_aspect_lock" ), crtemu->config.aspect_lock );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_hpos" ), crtemu->config.hpos );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_vpos" ), crtemu->config.vpos );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_hsize" ), crtemu->config.hsize );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_vsize" ), crtemu->config.vsize );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_contrast" ), crtemu->config.contrast );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_brightness" ), crtemu->config.brightness );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_saturation" ), crtemu->config.saturation );
    crtemu->Uniform1f( crtemu->GetUniformLocation( crtemu->crt_shader, "cfg_degauss" ), crtemu->config.degauss );

    float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->accumulatetexture_a );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, CRTEMU_GL_CLAMP_TO_BORDER );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, CRTEMU_GL_CLAMP_TO_BORDER );
    #ifndef CRTEMU_WEBGL
        crtemu->TexParameterfv( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_BORDER_COLOR, color );
    #endif

    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE1 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->blurtexture_a );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, CRTEMU_GL_CLAMP_TO_BORDER );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, CRTEMU_GL_CLAMP_TO_BORDER );
    #ifndef CRTEMU_WEBGL
        crtemu->TexParameterfv( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_BORDER_COLOR, color );
    #endif

    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE3 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, crtemu->frametexture );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MIN_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_MAG_FILTER, CRTEMU_GL_LINEAR );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_S, CRTEMU_GL_CLAMP_TO_BORDER );
    crtemu->TexParameteri( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_WRAP_T, CRTEMU_GL_CLAMP_TO_BORDER );
    #ifndef CRTEMU_WEBGL
        crtemu->TexParameterfv( CRTEMU_GL_TEXTURE_2D, CRTEMU_GL_TEXTURE_BORDER_COLOR, color );
    #endif

    crtemu->DrawArrays( CRTEMU_GL_TRIANGLE_FAN, 0, 4 );

    crtemu->ActiveTexture( CRTEMU_GL_TEXTURE0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 0 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 1 );
    crtemu->BindTexture( CRTEMU_GL_TEXTURE_2D, 2 );
    crtemu->BindFramebuffer( CRTEMU_GL_FRAMEBUFFER, 0 );
    }


void crtemu_coordinates_window_to_bitmap( crtemu_t* crtemu, int width, int height, int* x, int* y )
    {
    CRTEMU_GLint viewport[ 4 ];
    crtemu->GetIntegerv( CRTEMU_GL_VIEWPORT, viewport );

    int window_width = viewport[ 2 ] - viewport[ 0 ];
    int window_height = viewport[ 3 ] - viewport[ 1 ];

    int aspect_width = (int)( ( window_height * 4 ) / 3 );
    int aspect_height= (int)( ( window_width * 3 ) / 4 );
    int target_width, target_height;
    if( aspect_height <= window_height )
        {
        target_width = window_width;
        target_height = aspect_height;
        }
    else
        {
        target_width = aspect_width;
        target_height = window_height;
        }

    float hscale = target_width / (float) width;
    float vscale = target_height / (float) height;

    float hborder = ( window_width - hscale * width ) / 2.0f;
    float vborder = ( window_height - vscale * height ) / 2.0f;

    float xp = ( ( *x - hborder ) / hscale ) / (float) width;
    float yp = ( ( *y - vborder ) / vscale ) / (float) height;

    /* TODO: Common params for shader and this */
    float xc = ( xp - 0.5f ) * 2.0f;
    float yc = ( yp - 0.5f ) * 2.0f;
    xc *= 1.1f;
    yc *= 1.1f;
    //xc *= 1.0f + powf( ( fabsf( yc ) / 5.0f ), 2.0f);
    //yc *= 1.0f + powf( ( fabsf( xc ) / 4.0f ), 2.0f);
    float yt = ( yc >= 0.0f ? yc : -yc ) / 5.0f;
    float xt = ( xc >= 0.0f ? xc : -xc ) / 4.0f;
    xc *= 1.0f + ( yt * yt );
    yc *= 1.0f + ( xt * xt );
    xc = ( xc / 2.0f ) + 0.5f;
    yc = ( yc / 2.0f ) + 0.5f;
    xc = xc * 0.92f + 0.04f;
    yc = yc * 0.92f + 0.04f;
    xp = xc * 0.6f + xp * 0.4f;
    yp = yc * 0.6f + yp * 0.4f;

    xp = xp * ( 1.0f - 0.04f ) + 0.04f / 2.0f + 0.003f;
    yp = yp * ( 1.0f - 0.04f ) + 0.04f / 2.0f - 0.001f;

    xp = xp * 1.025f - 0.0125f;
    yp = yp * 0.92f + 0.04f;

    xp *= width;
    yp *= height;

    *x = (int) ( xp );
    *y = (int) ( yp );
    }


void crtemu_coordinates_bitmap_to_window( crtemu_t* crtemu, int width, int height, int* x, int* y )
    {
    (void) crtemu, (void) width, (void) height, (void) x, (void) y; // TODO: implement
    }


#endif /* CRTEMU_IMPLEMENTATION */

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
