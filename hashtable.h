/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

hashtable.h - v1.1 - Cache efficient hash table implementation for C/C++.

Do this:
    #define HASHTABLE_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef hashtable_h
#define hashtable_h

#ifndef HASHTABLE_U64
    #define HASHTABLE_U64 unsigned long long
#endif

typedef struct hashtable_t hashtable_t;

void hashtable_init( hashtable_t* table, int item_size, int initial_capacity, void* memctx );
void hashtable_term( hashtable_t* table );

void hashtable_insert( hashtable_t* table, HASHTABLE_U64 key, void const* item );
void hashtable_remove( hashtable_t* table, HASHTABLE_U64 key );
void hashtable_clear( hashtable_t* table );

void* hashtable_find( hashtable_t const* table, HASHTABLE_U64 key );

int hashtable_count( hashtable_t const* table );
void* hashtable_items( hashtable_t const* table );
HASHTABLE_U64 const* hashtable_keys( hashtable_t const* table );

void hashtable_swap( hashtable_t* table, int index_a, int index_b );


#endif /* hashtable_h */


/**

Example
=======

    #define HASHTABLE_IMPLEMENTATION
    #include "hashtable.h"

    #include <stdio.h> // for printf

    int main( int argc, char** argv )                                                                                                                          
        {                                                                                                                                                       
        (void) argc, argv;

        // define some example key and value types
        typedef struct key_t { int a, b, c; } key_t;
        typedef struct value_t 
            { 
            char id[ 64 ]; 
            float x, y, z; 
            int n[ 250 ]; 
            } value_t;

        // create a couple of sample keys 
        // (don't bother to fill in the fields for this sample)
        key_t* key_a = (key_t*)malloc( sizeof( key_t ) );
        key_t* key_b = (key_t*)malloc( sizeof( key_t ) );

        hashtable_t table;
        hashtable_init( &table, sizeof( value_t ), 256, 0 );

        {
        // values are copied into the table, not stored by pointer
        // (don't bother to fill in all the fields for this sample)
        value_t value_a = { "Item A" };
        value_t value_b = { "Item B" };
        hashtable_insert( &table, (HASHTABLE_U64)(uintptr_t)key_a, &value_a );
        hashtable_insert( &table, (HASHTABLE_U64)(uintptr_t)key_b, &value_b );
        }

        // find the values by key
        value_t* value_a = (value_t*)hashtable_find( &table, (HASHTABLE_U64)(uintptr_t)key_a );
        printf( "First item: %s\n", value_a->id );   
        value_t* value_b = (value_t*)hashtable_find( &table, (HASHTABLE_U64)(uintptr_t)key_b );
        printf( "Second item: %s\n", value_b->id );

        // remove one of the items
        hashtable_remove( &table, (HASHTABLE_U64)(uintptr_t)key_a );

        // it is possible to enumerate keys and values
        int count = hashtable_count( &table );
        HASHTABLE_U64 const* keys = hashtable_keys( &table );
        value_t* items = (value_t*)hashtable_items( &table );
        printf( "\nEnumeration:\n" );
        for( int i = 0; i < count; ++i )
            printf( "  0x%X : %s\n", (int) keys[ i ], items[ i ].id );

        // cleanup
        hashtable_term( &table );
        free( key_b );
        free( key_a );
        return 0;
        }


API Documentation
=================

hashtable.h is a small library for storing values in a table and access them efficiently by a 64-bit key. It is a 
single-header library, and does not need any .lib files or other binaries, or any build scripts. To use it, you just 
include hashtable.h to get the API declarations. To  get the definitions, you must include hashtable.h from *one* single 
C or C++ file, and #define the symbol `HASHTABLE_IMPLEMENTATION` before you do. 

The key value must be unique per entry, and is hashed for efficient lookup using an internal hashing algorithm. This
library does not support custom key types, so typically pointers or handles are used as key values.

The library is written with efficiency in mind. Data and keys are stored in separate structures, for better cache 
coherency, and hash collisions are resolved with open addressing/linear probing using the next available slot, which is
also good for the cache. 


Customization
-------------
There are a few different things in hashtable.h which are configurable by #defines. Most of the API use the `int` data 
type, for integer values where the exact size is not important. However, for some functions, it specifically makes use 
of 32 and 64 bit data types. These default to using `unsigned int` and `unsigned long long` by default, but can be
redefined by #defining HASHTABLE_U32 and HASHTABLE_U64 respectively, before including hashtable.h. This is useful if 
you, for  example, use the types from `<stdint.h>` in the rest of your program, and you want hashtable.h to use 
compatible types. In this case, you would include hashtable.h using the following code:

    #define HASHTABLE_U32 uint32_t
    #define HASHTABLE_U64 uint64_t
    #include "hashtable.h"

Note that when customizing the data types, you need to use the same definition in every place where you include 
hashtable.h, as they affect the declarations as well as the definitions.

The rest of the customizations only affect the implementation, so will only need to be defined in the file where you
have the #define HASHTABLE_IMPLEMENTATION.

Note that if all customizations are utilized, hashtable.h will include no external files whatsoever, which might be 
useful if you need full control over what code is being built.


### size_t

Internally, the hashtable.h implementation makes use of the standard `size_t` data type. This requires including the
c runtime library header `<stddef.h>`. To allow full configurability, and avoid hashtable.h including stddef.h, you can 
specify which type hashtable.h should use for its size_t, by #defining HASHTABLE_SIZE_T, like this:

    #define HASHTABLE_IMPLEMENTATION
    #define HASHTABLE_SIZE_T uint64_t
    #include "hashtable.h"

If not specified, hashtable.h will by default include stddef.h and use the standard `size_t` type.


### Custom memory allocators

To store the internal data structures, hashtable.h needs to do dynamic allocation by calling `malloc`. Programs might 
want to keep track of allocations done, or use custom defined pools to allocate memory from. hashtable.h allows for 
specifying custom memory allocation functions for `malloc` and `free`. This is done with the following code:

    #define HASHTABLE_IMPLEMENTATION
    #define HASHTABLE_MALLOC( ctx, size ) ( my_custom_malloc( ctx, size ) )
    #define HASHTABLE_FREE( ctx, ptr ) ( my_custom_free( ctx, ptr ) )
    #include "hashtable.h"

where `my_custom_malloc` and `my_custom_free` are your own memory allocation/deallocation functions. The `ctx` parameter
is an optional parameter of type `void*`. When `hashtable_init` is called, you can pass in a `memctx` parameter, which 
can be a pointer to anything you like, and which will be passed through as the `ctx` parameter to every  
`HASHTABLE_MALLOC`/`HASHTABLE_FREE` call. For example, if you are doing memory tracking, you can pass a pointer to your 
tracking data as `memctx`, and in your custom allocation/deallocation function, you can cast the `ctx` param back to the 
right type, and access the tracking data.

If no custom allocator is defined, hashtable.h will default to `malloc` and `free` from the C runtime library.


### Custom assert

hashtable.h makes use of asserts to report usage errors and failed allocation errors. By default, it makes use of the C 
runtime library `assert` macro, which only executes in debug builds. However, it allows for substituting with your own
assert function or macro using the following code:

    #define HASHTABLE_IMPLEMENTATION
    #define HASHTABLE_ASSERT( condition ) ( my_custom_assert( condition ) )
    #include "hashtable.h"

Note that if you only want the asserts to trigger in debug builds, you must add a check for this in your custom assert.


### Custom C runtime functions

The library makes use of two additional functions from the C runtime library, and for full flexibility, it allows you 
to substitute them for your own. Here's an example:

    #define HASHTABLE_IMPLEMENTATION
    #define HASHTABLE_MEMCPY( dst, src, cnt ) ( my_memcpy_func( dst, src, cnt ) )
    #define HASHTABLE_MEMSET( ptr, val, cnt ) ( my_memset_func( ptr, val, cnt ) )
    #include "hashtable.h"

If no custom function is defined, hashtable.h will default to the C runtime library equivalent.


hashtable_init
--------------

    void hashtable_init( hashtable_t* table, int item_size, int initial_capacity, void* memctx )

Initialize a hashtable instance. `item_size` specifies the size, in bytes, of the data type holding a single item stored
in the table. `initial_capacity` is the number of items to allocate storage for initially - capacity will automatically
grow as needed, by reallocating memory.


hashtable_term
--------------
    
    void hashtable_term( hashtable_t* table )

Terminates a hashtable instance, releasing all memory used by it. No further calls to the hashtable API are valid until
the instance is reinitialized by another call to `hashtable_init`.


hashtable_insert
----------------

    void hashtable_insert( hashtable_t* table, HASHTABLE_U64 key, void const* item )

Inserts a data item into the hashtable, associating it with the specified key. The item is copied into the hashtable, 
rather than just storing the `item` pointer, so the `item` pointer can be safely released after the call to 
`hashtable_insert`. The value of `key` must be unique - it is not valid to store two items with the same key value. An
assert is triggered if trying to add a key which already exists, which means that if the default assert is used, it will
only be checked in debug builds - in release builds, it is up to the calling code to ensure this doesn't happen, or the
hashtable will be left in an undefined state.


hashtable_remove
----------------

    void hashtable_remove( hashtable_t* table, HASHTABLE_U64 key )

Removes the item associated with the specified key, and the instance of the key itself, from the hashtable. If the 
specified key could not be found, an assert is triggered.


hashtable_clear
---------------

    void hashtable_clear( hashtable_t* table )

Removes all the items stored in the hashtable, without deallocating any of the memory it has allocated.


hashtable_find
--------------

    void* hashtable_find( hashtable_t const* table, HASHTABLE_U64 key )

Returns a pointer to the item associated with the specified key, or NULL it the key was not found. The lookup is 
designed for efficiency, and for minimizing cache missed.


hashtable_count
---------------

    int hashtable_count( hashtable_t const* table )

Returns the number of items currently held in the table.


hashtable_items
---------------

    void* hashtable_items( hashtable_t const* table )

Returns a pointer to the items currently held in the table. All items are stored in a contiguous memory block, and you
can get to the next item be moving the pointer `item_size` bytes forward, where `item_size` is the same value as passed
to hash_table_init. The easiest way to acces items is to cast the return value to the correct type and just index it as
a normal array. It contains as many items as returned by `hashtable_count`.


hashtable_keys
--------------

    HASHTABLE_U64 const* hashtable_keys( hashtable_t const* table )

Returns a pointer to the keys currently held in the table, in the same order as the items returned from 
`hashtable_items`. Can be indexed as an array with as many elements as returned by `hashtable_count`.


hashtable_swap
--------------

    void hashtable_swap( hashtable_t* table, int index_a, int index_b )

Swaps the specified item/key pairs, and updates the hash lookup for both. Can be used to re-order the contents, as
retrieved by calling `hashtable_items` and `hashtable_keys`, while keeping the hashing intact.

*/

/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifndef hashtable_t_h
#define hashtable_t_h

#ifndef HASHTABLE_U32
    #define HASHTABLE_U32 unsigned int
#endif

struct hashtable_internal_slot_t
    {
    HASHTABLE_U32 key_hash;
    int item_index;
    int base_count;
    };

struct hashtable_t
    {
    void* memctx;
    int count;
    int item_size;

    struct hashtable_internal_slot_t* slots;
    int slot_capacity;

    HASHTABLE_U64* items_key;
    int* items_slot;
    void* items_data;
    int item_capacity;

    void* swap_temp;
    };

#endif /* hashtable_t_h */


#ifdef HASHTABLE_IMPLEMENTATION
#undef HASHTABLE_IMPLEMENTATION

#ifndef HASHTABLE_SIZE_T
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <stddef.h>
    #define HASHTABLE_SIZE_T size_t
#endif

#ifndef HASHTABLE_ASSERT
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <assert.h>
    #define HASHTABLE_ASSERT( x ) assert( x )
#endif

#ifndef HASHTABLE_MEMSET
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define HASHTABLE_MEMSET( ptr, val, cnt ) ( memset( ptr, val, cnt ) )
#endif 

#ifndef HASHTABLE_MEMCPY
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define HASHTABLE_MEMCPY( dst, src, cnt ) ( memcpy( dst, src, cnt ) )
#endif 

#ifndef HASHTABLE_MALLOC
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #define HASHTABLE_MALLOC( ctx, size ) ( malloc( size ) )
    #define HASHTABLE_FREE( ctx, ptr ) ( free( ptr ) )
#endif


static HASHTABLE_U32 hashtable_internal_pow2ceil( HASHTABLE_U32 v )
    {
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


void hashtable_init( hashtable_t* table, int item_size, int initial_capacity, void* memctx )
    {
    initial_capacity = (int)hashtable_internal_pow2ceil( initial_capacity >=0 ? (HASHTABLE_U32) initial_capacity : 32U );
    table->memctx = memctx;
    table->count = 0;
    table->item_size = item_size;
    table->slot_capacity = (int) hashtable_internal_pow2ceil( (HASHTABLE_U32) ( initial_capacity + initial_capacity / 2 ) );
    int slots_size = (int)( table->slot_capacity * sizeof( *table->slots ) );
    table->slots = (struct hashtable_internal_slot_t*) HASHTABLE_MALLOC( table->memctx, (HASHTABLE_SIZE_T) slots_size );
    HASHTABLE_ASSERT( table->slots );
    HASHTABLE_MEMSET( table->slots, 0, (HASHTABLE_SIZE_T) slots_size );
    table->item_capacity = (int) hashtable_internal_pow2ceil( (HASHTABLE_U32) initial_capacity );
    table->items_key = (HASHTABLE_U64*) HASHTABLE_MALLOC( table->memctx, 
        table->item_capacity * ( sizeof( *table->items_key ) + sizeof( *table->items_slot ) + table->item_size ) + table->item_size );
    HASHTABLE_ASSERT( table->items_key );
    table->items_slot = (int*)( table->items_key + table->item_capacity );
    table->items_data = (void*)( table->items_slot + table->item_capacity );
    table->swap_temp = (void*)( ( (uintptr_t) table->items_data ) + table->item_size * table->item_capacity ); 
    }


void hashtable_term( hashtable_t* table )
    {
    HASHTABLE_FREE( table->memctx, table->items_key );
    HASHTABLE_FREE( table->memctx, table->slots );
    }


// from https://gist.github.com/badboy/6267743
static HASHTABLE_U32 hashtable_internal_calculate_hash( HASHTABLE_U64 key )
    {
    key = ( ~key ) + ( key << 18 );
    key = key ^ ( key >> 31 );
    key = key * 21;
    key = key ^ ( key >> 11 );
    key = key + ( key << 6 );
    key = key ^ ( key >> 22 );  
    HASHTABLE_ASSERT( key );
    return (HASHTABLE_U32) key;
    }


static int hashtable_internal_find_slot( hashtable_t const* table, HASHTABLE_U64 key )
    {
    int const slot_mask = table->slot_capacity - 1;
    HASHTABLE_U32 const hash = hashtable_internal_calculate_hash( key );

    int const base_slot = (int)( hash & (HASHTABLE_U32)slot_mask );
    int base_count = table->slots[ base_slot ].base_count;
    int slot = base_slot;

    while( base_count > 0 )
        {
        HASHTABLE_U32 slot_hash = table->slots[ slot ].key_hash;
        if( slot_hash )
            {
            int slot_base = (int)( slot_hash & (HASHTABLE_U32)slot_mask );
            if( slot_base == base_slot ) 
                {
                HASHTABLE_ASSERT( base_count > 0 );
                --base_count;
                if( slot_hash == hash && table->items_key[ table->slots[ slot ].item_index ] == key )
                    return slot;
                }
            }
        slot = ( slot + 1 ) & slot_mask;
        }   

    return -1;
    }


static void hashtable_internal_expand_slots( hashtable_t* table )
    {
    int const old_capacity = table->slot_capacity;
    struct hashtable_internal_slot_t* old_slots = table->slots;

    table->slot_capacity *= 2;
    int const slot_mask = table->slot_capacity - 1;

    int const size = (int)( table->slot_capacity * sizeof( *table->slots ) );
    table->slots = (struct hashtable_internal_slot_t*) HASHTABLE_MALLOC( table->memctx, (HASHTABLE_SIZE_T) size );
    HASHTABLE_ASSERT( table->slots );
    HASHTABLE_MEMSET( table->slots, 0, (HASHTABLE_SIZE_T) size );

    for( int i = 0; i < old_capacity; ++i )
        {
        HASHTABLE_U32 const hash = old_slots[ i ].key_hash;
        if( hash )
            {
            int const base_slot = (int)( hash & (HASHTABLE_U32)slot_mask );
            int slot = base_slot;
            while( table->slots[ slot ].key_hash )
                slot = ( slot + 1 ) & slot_mask;
            table->slots[ slot ].key_hash = hash;
            int item_index = old_slots[ i ].item_index;
            table->slots[ slot ].item_index = item_index;
            table->items_slot[ item_index ] = slot; 
            ++table->slots[ base_slot ].base_count;
            }               
        }

    HASHTABLE_FREE( table->memctx, old_slots );
    }


static void hashtable_internal_expand_items( hashtable_t* table )
    {
    table->item_capacity *= 2;
     HASHTABLE_U64* const new_items_key = (HASHTABLE_U64*) HASHTABLE_MALLOC( table->memctx, 
         table->item_capacity * ( sizeof( *table->items_key ) + sizeof( *table->items_slot ) + table->item_size ) + table->item_size);
    HASHTABLE_ASSERT( new_items_key );

    int* const new_items_slot = (int*)( new_items_key + table->item_capacity );
    void* const new_items_data = (void*)( new_items_slot + table->item_capacity );
    void* const new_swap_temp = (void*)( ( (uintptr_t) new_items_data ) + table->item_size * table->item_capacity ); 

    HASHTABLE_MEMCPY( new_items_key, table->items_key, table->count * sizeof( *table->items_key ) );
    HASHTABLE_MEMCPY( new_items_slot, table->items_slot, table->count * sizeof( *table->items_key ) );
    HASHTABLE_MEMCPY( new_items_data, table->items_data, (HASHTABLE_SIZE_T) table->count * table->item_size );
    
    HASHTABLE_FREE( table->memctx, table->items_key );

    table->items_key = new_items_key;
    table->items_slot = new_items_slot;
    table->items_data = new_items_data;
    table->swap_temp = new_swap_temp;
    }


void hashtable_insert( hashtable_t* table, HASHTABLE_U64 key, void const* item )
    {
    HASHTABLE_ASSERT( hashtable_internal_find_slot( table, key ) < 0 );

    if( table->count >= ( table->slot_capacity - table->slot_capacity / 3 ) )
        hashtable_internal_expand_slots( table );
        
    int const slot_mask = table->slot_capacity - 1;
    HASHTABLE_U32 const hash = hashtable_internal_calculate_hash( key );

    int const base_slot = (int)( hash & (HASHTABLE_U32)slot_mask );
    int base_count = table->slots[ base_slot ].base_count;
    int slot = base_slot;
    int first_free = slot;
    while( base_count )
        {
        HASHTABLE_U32 const slot_hash = table->slots[ slot ].key_hash;
        if( slot_hash == 0 && table->slots[ first_free ].key_hash != 0 ) first_free = slot;
        int slot_base = (int)( slot_hash & (HASHTABLE_U32)slot_mask );
        if( slot_base == base_slot ) 
            --base_count;
        slot = ( slot + 1 ) & slot_mask;
        }       

    slot = first_free;
    while( table->slots[ slot ].key_hash )
        slot = ( slot + 1 ) & slot_mask;

    if( table->count >= table->item_capacity )
        hashtable_internal_expand_items( table );

    HASHTABLE_ASSERT( !table->slots[ slot ].key_hash && ( hash & (HASHTABLE_U32) slot_mask ) == (HASHTABLE_U32) base_slot );
    HASHTABLE_ASSERT( hash );
    table->slots[ slot ].key_hash = hash;
    table->slots[ slot ].item_index = table->count;
    ++table->slots[ base_slot ].base_count;

        
    void* dest_item = (void*)( ( (uintptr_t) table->items_data ) + table->count * table->item_size );
    memcpy( dest_item, item, (HASHTABLE_SIZE_T) table->item_size );
    table->items_key[ table->count ] = key;
    table->items_slot[ table->count ] = slot;
    ++table->count;
    } 


void hashtable_remove( hashtable_t* table, HASHTABLE_U64 key )
    {
    int const slot = hashtable_internal_find_slot( table, key );
    HASHTABLE_ASSERT( slot >= 0 );

    int const slot_mask = table->slot_capacity - 1;
    HASHTABLE_U32 const hash = table->slots[ slot ].key_hash;
    int const base_slot = (int)( hash & (HASHTABLE_U32) slot_mask );
    HASHTABLE_ASSERT( hash );
    --table->slots[ base_slot ].base_count;
    table->slots[ slot ].key_hash = 0;

    int index = table->slots[ slot ].item_index;
    int last_index = table->count - 1;
    if( index != last_index )
        {
        table->items_key[ index ] = table->items_key[ last_index ];
        table->items_slot[ index ] = table->items_slot[ last_index ];
        void* dst_item = (void*)( ( (uintptr_t) table->items_data ) + index * table->item_size );
        void* src_item = (void*)( ( (uintptr_t) table->items_data ) + last_index * table->item_size );
        HASHTABLE_MEMCPY( dst_item, src_item, (HASHTABLE_SIZE_T) table->item_size );
        table->slots[ table->items_slot[ last_index ] ].item_index = index;
        }
    --table->count;
    } 


void hashtable_clear( hashtable_t* table )
    {
    table->count = 0;
    HASHTABLE_MEMSET( table->slots, 0, table->slot_capacity * sizeof( *table->slots ) );
    }


void* hashtable_find( hashtable_t const* table, HASHTABLE_U64 key )
    {
    int const slot = hashtable_internal_find_slot( table, key );
    if( slot < 0 ) return 0;

    int const index = table->slots[ slot ].item_index;
    void* const item = (void*)( ( (uintptr_t) table->items_data ) + index * table->item_size );
    return item;
    }


int hashtable_count( hashtable_t const* table )
    {
    return table->count;
    }


void* hashtable_items( hashtable_t const* table )
    {
    return table->items_data;
    }


HASHTABLE_U64 const* hashtable_keys( hashtable_t const* table )
    {
    return table->items_key;
    }


void hashtable_swap( hashtable_t* table, int index_a, int index_b )
    {
    if( index_a < 0 || index_a >= table->count || index_b < 0 || index_b >= table->count ) return;

    int slot_a = table->items_slot[ index_a ];
    int slot_b = table->items_slot[ index_b ];

    table->items_slot[ index_a ] = slot_b;
    table->items_slot[ index_b ] = slot_a;

    HASHTABLE_U64 temp_key = table->items_key[ index_a ];
    table->items_key[ index_a ] = table->items_key[ index_b ];
    table->items_key[ index_b ] = temp_key;

    void* item_a = (void*)( ( (uintptr_t) table->items_data ) + index_a * table->item_size );
    void* item_b = (void*)( ( (uintptr_t) table->items_data ) + index_b * table->item_size );
    HASHTABLE_MEMCPY( table->swap_temp, item_a, table->item_size );
    HASHTABLE_MEMCPY( item_a, item_b, table->item_size );
    HASHTABLE_MEMCPY( item_b, table->swap_temp, table->item_size );

    table->slots[ slot_a ].item_index = index_b;
    table->slots[ slot_b ].item_index = index_a;
    }


#endif /* HASHTABLE_IMPLEMENTATION */

/*

contributors:
    Randy Gaul (hashtable_clear, hashtable_swap )

revision history:
    1.1     added hashtable_clear, hashtable_swap
    1.0     first released version  

*/

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
