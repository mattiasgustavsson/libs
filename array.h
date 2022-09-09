/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

array.h - v0.1 - Dynamic array library for C/C++.

Do this:
    #define ARRAY_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef array_h
#define array_h

#ifndef ARRAY_BOOL_T
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdbool.h>
    #define ARRAY_BOOL_T bool
#endif

#define array_t( type ) struct { int count; type* items; }
#define array_param_t( type ) void
#define array_create( type ) ARRAY_CAST( (void*)internal_array_create( sizeof( type ), NULL ) )
#define array_create_memctx( type, memctx ) ARRAY_CAST( (void*)internal_array_create( sizeof( type ), (memctx) ) )
#define array_destroy( array ) internal_array_destroy( (struct internal_array_t*) (array) )
#define array_add( array, item ) ARRAY_CAST( internal_array_add( (struct internal_array_t*) (array), (void*) (item) ) )
#define array_remove( array, index ) internal_array_remove( (struct internal_array_t*) (array), (index) )
#define array_remove_ordered( array, index ) internal_array_remove_ordered( (struct internal_array_t*) (array), (index) )
#define array_get( array, index, item ) internal_array_get( (struct internal_array_t*) (array), (index), (void*) (item) )
#define array_set( array, index, item ) internal_array_set( (struct internal_array_t*) (array), (index), (void*) (item) )
#define array_count( array ) internal_array_count( (struct internal_array_t*) (array) )
#define array_sort( array, compare ) internal_array_sort( (struct internal_array_t*) (array), (compare) )
#define array_bsearch( array, key, compare ) internal_array_bsearch( (struct internal_array_t*) (array), (void*) (key), (compare) )
#define array_find( array, item ) internal_array_find( (struct internal_array_t*) (array), (void*) (item) )
#define array_item( array, index ) ARRAY_CAST( internal_array_item( (struct internal_array_t*) (array), (index) ) )


// In C, a void* can be implicitly cast to any other kind of pointer, while in C++ you need an explicit cast. In most
// cases, the explicit cast works for both C and C++, but if we consider the case where we have nested structs, then
// the way you refer to them differs between C and C++ (in C++, `parent_type::nested_type`, in C just `nested_type`).
// In addition, with the automatic cast in C, it is possible to use unnamed nested structs and still dynamically
// allocate arrays of that type - this would be desirable when the code is compiled from C++ as well.
// This VOID_CAST macro allows for automatic cast from void* in C++. In C, it does nothing, but for C++ it uses a
// simple template function to define a cast-to-anything operator.
// Use like this:
//      struct {
//          struct {
//              int x;
//          } *nested;
//      } parent;
//      parent.nested = VOID_CAST( malloc( sizeof( *parent.nested ) * count ) );
//
#ifndef ARRAY_CAST
    #ifdef __cplusplus
        struct array_cast {
            inline array_cast( void* x_ ) : x( x_ ) { }
            inline array_cast( void const* x_ ) : x( (void*) x_ ) { }
            template< typename T > inline operator T() { return (T)x; } // cast to whatever requested
            void* x;
        };
        #define ARRAY_CAST( x ) array_cast( x )
    #else
        #define ARRAY_CAST( x ) x
    #endif
#endif


struct internal_array_t;

struct internal_array_t* internal_array_create( int item_size, void* memctx );
void internal_array_destroy( struct internal_array_t* array );
void* internal_array_add( struct internal_array_t* array, void* item );
void internal_array_remove( struct internal_array_t* array, int index );
void internal_array_remove_ordered( struct internal_array_t* array, int index );
ARRAY_BOOL_T internal_array_get( struct internal_array_t* array, int index, void* item );
ARRAY_BOOL_T internal_array_set( struct internal_array_t* array, int index, void const* item );
int internal_array_count( struct internal_array_t* array );
void internal_array_sort( struct internal_array_t* array, int (*compare)( void const*, void const* ) );
int internal_array_bsearch( struct internal_array_t* array, void* key, int (*compare)( void const*, void const* ) );
int internal_array_find( struct internal_array_t* array, void* item );
void* internal_array_item( struct internal_array_t* array, int index );

#endif /* array_h */


/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef ARRAY_IMPLEMENTATION
#undef ARRAY_IMPLEMENTATION


#ifndef ARRAY_MALLOC
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #define ARRAY_MALLOC( ctx, size ) ( malloc( size ) )
    #define ARRAY_FREE( ctx, ptr ) ( free( ptr ) )
#endif

#ifndef ARRAY_MEMCPY
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define ARRAY_MEMCPY( dst, src, cnt ) ( memcpy( (dst), (src), (cnt) ) )
#endif

#ifndef ARRAY_MEMMOVE
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define ARRAY_MEMMOVE( dst, src, cnt ) ( memcpy( (dst), (src), (cnt) ) )
#endif

#ifndef ARRAY_MEMCMP
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define ARRAY_MEMCMP( a, b, cnt ) ( memcmp( (a), (b), (cnt) ) )
#endif

#ifndef ARRAY_QSORT
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #define ARRAY_QSORT( base, num, size, cmp ) ( qsort( (base), (num), (size), (cmp) ) )
#endif

#ifndef ARRAY_BSEARCH
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #define ARRAY_BSEARCH( key, base, num, size, cmp ) ( bsearch( (key), (base), (num), (size), (cmp) ) )
#endif


struct internal_array_t {
    int count;
    void* items;
    void* memctx;
    int item_size;
    int capacity;
};


struct internal_array_t* internal_array_create( int item_size, void* memctx ) {
    struct internal_array_t* array = (struct internal_array_t*) ARRAY_MALLOC( memctx, sizeof( struct internal_array_t ) );
    array->memctx = memctx;
    array->item_size = item_size;
    array->capacity = 256;
    array->count = 0;
    array->items = ARRAY_MALLOC( memctx, (size_t) array->capacity * item_size );
    return array;
}


void internal_array_destroy( struct internal_array_t* array ) {
    ARRAY_FREE( array->memctx, array->items );
    ARRAY_FREE( array->memctx, array );
}


void* internal_array_add( struct internal_array_t* array, void* item ) {
    if( array->count >= array->capacity ) {
        array->capacity *= 2;
        void* items = array->items;
        array->items = ARRAY_MALLOC( array->memctx, (size_t) array->capacity * array->item_size );
        ARRAY_MEMCPY( array->items, items, (size_t)array->count * array->item_size );
        ARRAY_FREE( array->memctx, items );
    }
    ARRAY_MEMCPY( (void*)( ( (uintptr_t) array->items ) + array->count * array->item_size ), item,
        (size_t)array->item_size );
    ++array->count;
    return (void*)( ( (uintptr_t) array->items ) + ( array->count - 1 ) * array->item_size );
}


void internal_array_remove( struct internal_array_t* array, int index ) {
    if( index >= 0 && index < array->count ) {
        --array->count;
        ARRAY_MEMMOVE( (void*)( ( (uintptr_t) array->items ) + index * array->item_size ),
            (void*)( ( (uintptr_t) array->items ) + array->count  * array->item_size ), (size_t) array->item_size );
    }
}

void internal_array_remove_ordered( struct internal_array_t* array, int index ) {
    if( index >= 0 && index < array->count ) {
        --array->count;
        ARRAY_MEMMOVE( (void*)( ( (uintptr_t) array->items ) + index * array->item_size ),
            (void*)( ( (uintptr_t) array->items ) + ( index + 1 ) * array->item_size ),
            (size_t)array->item_size * ( array->count - index ) );
    }
}

ARRAY_BOOL_T internal_array_get( struct internal_array_t* array, int index, void* item ) {
    ARRAY_BOOL_T result = index >= 0 && index < array->count;
    if( result ) {
        ARRAY_MEMCPY( item, (void*)( ( (uintptr_t) array->items ) + index * array->item_size ),
            (size_t) array->item_size );
    }
    return result;
}

ARRAY_BOOL_T internal_array_set( struct internal_array_t* array, int index, void const* item ) {
    ARRAY_BOOL_T result = index >= 0 && index < array->count;
    if( result ) {
        ARRAY_MEMCPY( (void*)( ( (uintptr_t) array->items ) + index * array->item_size ), item,
            (size_t) array->item_size );
    }
    return result;
}

int internal_array_count( struct internal_array_t* array ) {
    int count = array->count;
    return count;
}


void internal_array_sort( struct internal_array_t* array, int (*compare)( void const*, void const* ) ) {
    ARRAY_QSORT( array->items, (size_t) array->count, (size_t) array->item_size, compare );
}


int internal_array_bsearch( struct internal_array_t* array, void* key, int (*compare)( void const*, void const* ) ) {
    void* item = ARRAY_BSEARCH( key, array->items, (size_t) array->count, (size_t) array->item_size, compare );
    int result = -1;
    if( item ) {
        result = (int)( ( ((uintptr_t)item) - ((uintptr_t)array->items) ) / array->item_size );
    }
    return result;
}


int internal_array_find( struct internal_array_t* array, void* item ) {
    for( int i = 0; i < array->count; ++i ) {
        if( ARRAY_MEMCMP( (void*)( ( (uintptr_t) array->items ) + i * array->item_size ), item,
            (size_t) array->item_size) == 0 ) {

            return i;
        }
    }
    return -1;
}


void* internal_array_item( struct internal_array_t* array, int index ) {
    if(  index >= 0 && index < array->count ) {
        return (void*)( ( (uintptr_t) array->items ) + index * array->item_size );
    } else {
        return NULL;
    }
}

#endif /* ARRAY_IMPLEMENTATION */

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
