/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

buffer.h - v0.1 - Memory buffer with read/write operations, for C/C++.

Do this:
    #define BUFFER_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef buffer_h
#define buffer_h

// If you want buffer to swap endianness on read/write, do this before include: #define BUFFER_BIG_ENDIAN

#ifndef BUFFER_I8_T
    #define BUFFER_I8_T char
#endif

#ifndef BUFFER_I16_T
    #define BUFFER_I16_T short
#endif

#ifndef BUFFER_I32_T
    #define BUFFER_I32_T int
#endif

#ifndef BUFFER_I64_T
    #define BUFFER_I64_T long long
#endif

#ifndef BUFFER_U8_T
    #define BUFFER_U8_T unsigned char
#endif

#ifndef BUFFER_U16_T
    #define BUFFER_U16_T unsigned short
#endif

#ifndef BUFFER_U32_T
    #define BUFFER_U32_T unsigned int
#endif

#ifndef BUFFER_U64_T
    #define BUFFER_U64_T unsigned long long
#endif


typedef struct buffer_t buffer_t;

buffer_t* buffer_create( void );
buffer_t* buffer_load( const char* filename );
buffer_t* buffer_map( void* data, size_t size );
void buffer_destroy( buffer_t* buffer );
bool buffer_save( buffer_t* buffer, char const* filename );
void buffer_resize( buffer_t* buffer, size_t size );
size_t buffer_position( buffer_t* buffer );
size_t buffer_position_set( buffer_t* buffer, size_t position );
size_t buffer_size( buffer_t* buffer );
void* buffer_data( buffer_t* buffer );

int buffer_read_char( buffer_t* buffer, char* value, int count );
int buffer_read_i8( buffer_t* buffer, BUFFER_I8_T* value, int count );
int buffer_read_i16( buffer_t* buffer, BUFFER_I16_T* value, int count );
int buffer_read_i32( buffer_t* buffer, BUFFER_I32_T* value, int count );
int buffer_read_i64( buffer_t* buffer, BUFFER_I64_T* value, int count );
int buffer_read_u8( buffer_t* buffer, BUFFER_U8_T* value, int count );
int buffer_read_u16( buffer_t* buffer, BUFFER_U16_T* value, int count );
int buffer_read_u32( buffer_t* buffer, BUFFER_U32_T* value, int count );
int buffer_read_u64( buffer_t* buffer, BUFFER_U64_T* value, int count );
int buffer_read_float( buffer_t* buffer, float* value, int count );
int buffer_read_double( buffer_t* buffer, double* value, int count );
int buffer_read_bool( buffer_t* buffer, bool* value, int count );

int buffer_write_char( buffer_t* buffer, char const* value, int count );
int buffer_write_i8( buffer_t* buffer, BUFFER_I8_T const* value, int count );
int buffer_write_i16( buffer_t* buffer, BUFFER_I16_T const* value, int count );
int buffer_write_i32( buffer_t* buffer, BUFFER_I32_T const* value, int count );
int buffer_write_i64( buffer_t* buffer, BUFFER_I64_T const* value, int count );
int buffer_write_u8( buffer_t* buffer, BUFFER_U8_T const* value, int count );
int buffer_write_u16( buffer_t* buffer, BUFFER_U16_T const* value, int count );
int buffer_write_u32( buffer_t* buffer, BUFFER_U32_T const* value, int count );
int buffer_write_u64( buffer_t* buffer, BUFFER_U64_T const* value, int count );
int buffer_write_float( buffer_t* buffer, float const* value, int count );
int buffer_write_double( buffer_t* buffer, double const* value, int count );
int buffer_write_bool( buffer_t* buffer, bool const* value, int count );

#endif /* buffer_h */


#ifdef BUFFER_IMPLEMENTATION
#undef BUFFER_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static BUFFER_U32_T buffer_pow2ceil( BUFFER_U32_T v ) {
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    v += ( v == 0 );
    return v;
}


struct buffer_t {
    size_t capacity;
    size_t size;
    size_t position;
    void* data;
    int is_mapped;
};


buffer_t* buffer_create( void ) {
    buffer_t* buffer = (buffer_t*) malloc( sizeof( buffer_t) );
    buffer->capacity = 4096;
    buffer->size = 0;
    buffer->position = 0;
    buffer->data = malloc( buffer->capacity );
    buffer->is_mapped = 0;
    return buffer;
}


buffer_t* buffer_load( const char* filename ) {
    FILE* fp = fopen( filename, "rb" );
    if( !fp ) {
        return NULL;
    }
    fseek( fp, 0, SEEK_END );
    int size = (int) ftell( fp );
    fseek( fp, 0, SEEK_SET );
    if( size == 0 ) {
        fclose( fp );
        return NULL;
    }

    int pow2size = (int)buffer_pow2ceil( (BUFFER_U32_T) size );
    void* data = malloc( pow2size );
    int read = (int) fread( data, 1, size, fp );
    fclose( fp );
    if( read != size ) {
        free( data );
        return NULL;
    }

    buffer_t* buffer = (buffer_t*) malloc( sizeof( buffer_t) );
    buffer->capacity = pow2size;
    buffer->size = size;
    buffer->position = 0;
    buffer->data = data;
    buffer->is_mapped = 0;
    return buffer;
}


buffer_t* buffer_map( void* data, size_t size ) {
    if( !data ) {
        return NULL;
    }
    buffer_t* buffer = (buffer_t*) malloc( sizeof( buffer_t) );
    buffer->capacity = size;
    buffer->size = size;
    buffer->position = 0;
    buffer->data = data;
    buffer->is_mapped = 1;
    return buffer;
}


void buffer_destroy( buffer_t* buffer ) {
    if( !buffer->is_mapped ) {
        free( buffer->data );
    }
    free( buffer );
}


bool buffer_save( buffer_t* buffer, char const* filename ) {
    FILE* fp = fopen( filename, "wb" );
    if( !fp ) {
        return false;
    }
    size_t written = fwrite( buffer->data, 1, buffer->size, fp );
    fclose( fp );
    return written == buffer->size;
}


void buffer_resize( buffer_t* buffer, size_t size ) {
    if( buffer->is_mapped ) {
        return;
    }
    if( size > buffer->capacity ) {
        while( size > buffer->capacity ) {
            buffer->capacity *= 2;
        }
        buffer->data = realloc( buffer->data, buffer->capacity );
    }
    buffer->size = size;
    if( buffer->position > size ) {
        buffer->position = size;
    }
}


size_t buffer_position( buffer_t* buffer ) {
    size_t result = buffer->position;
    return result;
}


size_t buffer_position_set( buffer_t* buffer, size_t position ) {
    buffer->position = position > buffer->size ? buffer->size : position;
    size_t result = buffer->position;
    return result;
}


size_t buffer_size( buffer_t* buffer ) {
    size_t result = buffer->size;
    return result;
}


void* buffer_data( buffer_t* buffer ) {
    return buffer->data;
}

#ifndef BUFFER_BIG_ENDIAN
    #define BUFFER_READ_IMPL \
        { \
            int result = 0; \
            for( int i = 0; i < count; ++i ) { \
                if( buffer->position + sizeof( *value ) > buffer->size ) { \
                    return result; \
                } \
                memcpy( &value[ i ], (void*)( ( (uintptr_t) buffer->data ) + buffer->position ), sizeof( *value ) ); \
                buffer->position += sizeof( *value ); \
                ++result; \
            } \
            return result; \
        }
#else
    #define BUFFER_READ_IMPL \
        { \
            int result = 0; \
            for( int i = 0; i < count; ++i ) { \
                if( buffer->position + sizeof( *value ) > buffer->size ) { \
                    return result; \
                } \
                void* x = &value[ i ]; \
                memcpy( x, (void*)( ( (uintptr_t) buffer->data ) + buffer->position ), sizeof( *value ) ); \
                if( sizeof(*value) == 2 ) \
                    *(BUFFER_U16_T*)(x) = \
                    ((((*(BUFFER_U16_T*)(x)) & 0xFF00) >> 8) | \
                     (((*(BUFFER_U16_T*)(x)) & 0x00FF) << 8)); \
                else if( sizeof(*value) == 4 ) \
                    *(BUFFER_U32_T*)(x) = \
                    ((((*(BUFFER_U32_T*)(x)) & 0xFF000000) >> 24) | \
                     (((*(BUFFER_U32_T*)(x)) & 0x00FF0000) >> 8) | \
                     (((*(BUFFER_U32_T*)(x)) & 0x0000FF00) << 8) | \
                     (((*(BUFFER_U32_T*)(x)) & 0x000000FF) << 24) ); \
                else if( sizeof(*value) == 8 ) \
                    *(BUFFER_U64_T*)(x) = \
                    ((((*(BUFFER_U64_T*)(x)) & 0xFF00000000000000) >> 56 ) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x00FF000000000000) >> 40 ) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x0000FF0000000000) >> 24 ) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x000000FF00000000) >> 8 ) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x00000000FF000000) << 8) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x0000000000FF0000) << 24) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x000000000000FF00) << 40) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x00000000000000FF) << 56)); \
                buffer->position += sizeof( *value ); \
                ++result; \
            } \
            return result; \
        }
#endif

int buffer_read_char( buffer_t* buffer, char* value, int count ) BUFFER_READ_IMPL
int buffer_read_i8( buffer_t* buffer, BUFFER_I8_T* value, int count ) BUFFER_READ_IMPL
int buffer_read_i16( buffer_t* buffer, BUFFER_I16_T* value, int count ) BUFFER_READ_IMPL
int buffer_read_i32( buffer_t* buffer, BUFFER_I32_T* value, int count ) BUFFER_READ_IMPL
int buffer_read_i64( buffer_t* buffer, BUFFER_I64_T* value, int count ) BUFFER_READ_IMPL
int buffer_read_u8( buffer_t* buffer, BUFFER_U8_T* value, int count ) BUFFER_READ_IMPL
int buffer_read_u16( buffer_t* buffer, BUFFER_U16_T* value, int count ) BUFFER_READ_IMPL
int buffer_read_u32( buffer_t* buffer, BUFFER_U32_T* value, int count ) BUFFER_READ_IMPL
int buffer_read_u64( buffer_t* buffer, BUFFER_U64_T* value, int count ) BUFFER_READ_IMPL
int buffer_read_float( buffer_t* buffer, float* value, int count ) BUFFER_READ_IMPL
int buffer_read_double( buffer_t* buffer, double* value, int count ) BUFFER_READ_IMPL
int buffer_read_bool( buffer_t* buffer, bool* value, int count ) BUFFER_READ_IMPL

#undef BUFFER_READ_IMPL


#ifndef BUFFER_BIG_ENDIAN
    #define BUFFER_WRITE_IMPL \
        { \
            int result = 0; \
            for( int i = 0; i < count; ++i ) { \
                if( buffer->position + sizeof( *value ) > buffer->size ) { \
                    if( buffer->is_mapped ) break; \
                    buffer->size = buffer->position + sizeof( *value ); \
                    while( buffer->size > buffer->capacity ) { \
                        buffer->capacity *= 2; \
                    } \
                    buffer->data = realloc( buffer->data, buffer->capacity ); \
                } \
                memcpy( (void*)( ( (uintptr_t) buffer->data ) + buffer->position ), &value[ i ], sizeof( *value ) ); \
                buffer->position += sizeof( *value ); \
                ++result; \
            } \
            return result; \
        }
#else
    #define BUFFER_WRITE_IMPL \
        { \
            int result = 0; \
            for( int i = 0; i < count; ++i ) { \
                if( buffer->position + sizeof( *value ) > buffer->size ) { \
                    if( buffer->is_mapped ) break; \
                    buffer->size = buffer->position + sizeof( *value ); \
                    while( buffer->size > buffer->capacity ) { \
                        buffer->capacity *= 2; \
                    } \
                    buffer->data = realloc( buffer->data, buffer->capacity ); \
                } \
                void* x = (void*)( ( (uintptr_t) buffer->data ) + buffer->position ); \
                memcpy( x, &value[ i ], sizeof( *value ) ); \
                if( sizeof(*value) == 2 ) \
                    *(BUFFER_U16_T*)(x) = \
                    ((((*(BUFFER_U16_T*)(x)) & 0xFF00) >> 8) | \
                     (((*(BUFFER_U16_T*)(x)) & 0x00FF) << 8)); \
                else if( sizeof(*value) == 4 ) \
                    *(BUFFER_U32_T*)(x) = \
                    ((((*(BUFFER_U32_T*)(x)) & 0xFF000000) >> 24) | \
                     (((*(BUFFER_U32_T*)(x)) & 0x00FF0000) >> 8) | \
                     (((*(BUFFER_U32_T*)(x)) & 0x0000FF00) << 8) | \
                     (((*(BUFFER_U32_T*)(x)) & 0x000000FF) << 24) ); \
                else if( sizeof(*value) == 8 ) \
                    *(BUFFER_U64_T*)(x) = \
                    ((((*(BUFFER_U64_T*)(x)) & 0xFF00000000000000) >> 56 ) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x00FF000000000000) >> 40 ) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x0000FF0000000000) >> 24 ) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x000000FF00000000) >> 8 ) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x00000000FF000000) << 8) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x0000000000FF0000) << 24) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x000000000000FF00) << 40) | \
                     (((*(BUFFER_U64_T*)(x)) & 0x00000000000000FF) << 56)); \
                buffer->position += sizeof( *value ); \
                ++result; \
            } \
            return result; \
        }
#endif

int buffer_write_char( buffer_t* buffer, char const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_i8( buffer_t* buffer, BUFFER_I8_T const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_i16( buffer_t* buffer, BUFFER_I16_T const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_i32( buffer_t* buffer, BUFFER_I32_T const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_i64( buffer_t* buffer, BUFFER_I64_T const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_u8( buffer_t* buffer, BUFFER_U8_T const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_u16( buffer_t* buffer, BUFFER_U16_T const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_u32( buffer_t* buffer, BUFFER_U32_T const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_u64( buffer_t* buffer, BUFFER_U64_T const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_float( buffer_t* buffer, float const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_double( buffer_t* buffer, double const* value, int count ) BUFFER_WRITE_IMPL
int buffer_write_bool( buffer_t* buffer, bool const* value, int count ) BUFFER_WRITE_IMPL

#undef BUFFER_WRITE_IMPL

#endif /* BUFFER_IMPLEMENTATION */

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2022 Mattias Gustavsson

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
