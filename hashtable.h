/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

hashtable.h - v2.0 - Cache efficient hash table implementation for C/C++.

Do this:
    #define HASHTABLE_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef hashtable_h
#define hashtable_h

#ifndef HASHTABLE_U32
    #define HASHTABLE_U32 unsigned int
#endif

typedef struct hashtable_t hashtable_t;

void hashtable_init( hashtable_t* table, int key_size, int item_size, int initial_capacity, void* memctx );
void hashtable_term( hashtable_t* table );

void hashtable_insert( hashtable_t* table, HASHTABLE_U32 hash, void const* key, void const* item );
void hashtable_remove( hashtable_t* table, HASHTABLE_U32 hash, void const* key );
void hashtable_clear( hashtable_t* table );

void* hashtable_find( hashtable_t const* table, HASHTABLE_U32 hash, void const* key );

int hashtable_count( hashtable_t const* table );
void* hashtable_items( hashtable_t const* table );
void const* hashtable_keys( hashtable_t const* table );

void hashtable_swap( hashtable_t* table, int index_a, int index_b );


#endif /* hashtable_h */

/*

uint32_t hash_u32( uint32_t key ) {
    key = ~key + ( key << 15 );
    key = key ^ ( key >> 12 );
    key = key + ( key << 2 );
    key = key ^ ( key >> 4 );
    key = (key + ( key << 3 ) ) + ( key << 11 );
    key = key ^ ( key >> 16);
    return key;
}


uint32_t hash_u64( uint64_t key ) {
    key = ( ~key ) + ( key << 18 );
    key = key ^ ( key >> 31 );
    key = ( key + ( key << 2 ) ) + ( key << 4 );
    key = key ^ ( key >> 11 );
    key = key + ( key << 6 );
    key = key ^ ( key >> 22 );  
    return (uint32_t) key;
}


uint32_t hash_str( char const* key ) {
    uint32_t hash = 5381u; 
    for( char const* s = key; *s != ’\0’; ++s ) {
        hash = ( ( hash << 5u ) + hash ) ^ (*s);
    }
    return hash;
}


uint32_t murmur_hash( const void * key, int len, uint32_t seed ) {
	uint32_t const m = 0x5bd1e995;
	int const r = 24;
	uint32_t h = seed ^ len;
	
    uint8_t const* data = (uint8_t const*) key;
	while( len >= 4 ) {
		#ifdef PLATFORM_BIG_ENDIAN
			uint32_t k = ( data[0] ) + ( data[1] << 8 ) + ( data[2] << 16 ) + ( data[3] << 24 );
		#else
			uint32_t k = *(uint32_t *)data;
		#endif

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

    switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

*/

/**

hashtable.h
===========

Cache efficient hash table implementation for C/C++.


Example
-------

    #define HASHTABLE_IMPLEMENTATION
    #include "hashtable.h"

    #include <stdio.h> // for printf

    int main( int argc, char** argv ) {
        // define some example key and value types
        typedef struct key_t { int a, b, c; } key_t;
        typedef struct value_t { 
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
        for( int i = 0; i < count; ++i ) {
            printf( "  0x%X : %s\n", (int) keys[ i ], items[ i ].id );
        }

        // cleanup
        hashtable_term( &table );
        free( key_b );
        free( key_a );
        return 0;
        }


API Documentation
-----------------

hashtable.h is a small library for storing values in a table and access them efficiently by a 64-bit key. It is a 
single-header library, and does not need any .lib files or other binaries, or any build scripts. To use it, you just 
include hashtable.h to get the API declarations. To  get the definitions, you must include hashtable.h from *one* single 
C or C++ file, and #define the symbol `HASHTABLE_IMPLEMENTATION` before you do. 

The key value must be unique per entry, and is hashed for efficient lookup using an internal hashing algorithm. This
library does not support custom key types, so typically pointers or handles are used as key values.

The library is written with efficiency in mind. Data and keys are stored in separate structures, for better cache 
coherency, and hash collisions are resolved with open addressing/linear probing using the next available slot, which is
also good for the cache. 


### Customization

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


#### size_t

Internally, the hashtable.h implementation makes use of the standard `size_t` data type. This requires including the
c runtime library header `<stddef.h>`. To allow full configurability, and avoid hashtable.h including stddef.h, you can 
specify which type hashtable.h should use for its size_t, by #defining HASHTABLE_SIZE_T, like this:

    #define HASHTABLE_IMPLEMENTATION
    #define HASHTABLE_SIZE_T uint64_t
    #include "hashtable.h"

If not specified, hashtable.h will by default include stddef.h and use the standard `size_t` type.


#### Custom memory allocators

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


#### Custom assert

hashtable.h makes use of asserts to report usage errors and failed allocation errors. By default, it makes use of the C 
runtime library `assert` macro, which only executes in debug builds. However, it allows for substituting with your own
assert function or macro using the following code:

    #define HASHTABLE_IMPLEMENTATION
    #define HASHTABLE_ASSERT( condition ) ( my_custom_assert( condition ) )
    #include "hashtable.h"

Note that if you only want the asserts to trigger in debug builds, you must add a check for this in your custom assert.


#### Custom C runtime functions

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
    int key_size;
    int item_size;

    struct hashtable_internal_slot_t* slots;
    int slot_capacity;
    int prime_index;

    void* items_key;
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

#ifndef HASHTABLE_MEMCPY
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define HASHTABLE_MEMCPY( dst, src, cnt ) ( memcpy( dst, src, cnt ) )
#endif 

#ifndef HASHTABLE_KEYCOPY
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define HASHTABLE_KEYCOPY( dst, src, cnt ) ( memcpy( dst, src, cnt ) )
#endif 

#ifndef HASHTABLE_ITEMCOPY
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define HASHTABLE_ITEMCOPY( dst, src, cnt ) ( memcpy( dst, src, cnt ) )
#endif 

#ifndef HASHTABLE_KEYCMP
    #undef _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define HASHTABLE_KEYCMP( a, b, len ) ( memcmp( a, b, len ) == 0 )
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


static int hashtable_internal_primes[] = { 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381, 32771, 65537, 131071, 
    262147, 524287, 1048573, 2097143, 4194301, 8388617, 16777213, 33554467, 67108859, 134217757, 268435459, 536870909, 
    1073741827, 2147483647 };


void hashtable_init( hashtable_t* table, int key_size, int item_size, int initial_capacity, void* memctx )
    {
    initial_capacity = (int)hashtable_internal_pow2ceil( initial_capacity >=0 ? (HASHTABLE_U32) initial_capacity : 32U );
    int prime_index = 0;
    while( hashtable_internal_primes[ prime_index ] < initial_capacity + initial_capacity / 2 ) ++prime_index;
    
    table->memctx = memctx;
    table->count = 0;
    table->key_size = key_size;
    table->item_size = item_size;
    table->slot_capacity = hashtable_internal_primes[ prime_index ];
    table->prime_index = prime_index;

    if( key_size > 0 )
        {
        int slots_size = (int)( table->slot_capacity * sizeof( *table->slots ) );
        table->slots = (struct hashtable_internal_slot_t*) HASHTABLE_MALLOC( table->memctx, (HASHTABLE_SIZE_T) slots_size );
        HASHTABLE_ASSERT( table->slots );

        for( int i = 0; i < table->slot_capacity; ++i  )
            {
            table->slots[ i ].base_count = 0;
            table->slots[ i ].item_index = -1;
            }
        }
    else
        {
        table->slots = 0;
        }

    table->item_capacity = initial_capacity;
    table->items_key = HASHTABLE_MALLOC( table->memctx, 
        table->item_capacity * ( table->key_size + sizeof( *table->items_slot ) + table->item_size ) + 
        ( table->key_size > table->item_size ? table->key_size : table->item_size ) );
    HASHTABLE_ASSERT( table->items_key );
    table->items_slot = (int*)( ( (uintptr_t) table->items_key ) + table->key_size * table->item_capacity );
    table->items_data = (void*)( table->items_slot + table->item_capacity );
    table->swap_temp = (void*)( ( (uintptr_t) table->items_data ) + table->item_size * table->item_capacity ); 
    }


void hashtable_term( hashtable_t* table )
    {
    HASHTABLE_FREE( table->memctx, table->items_key );
    HASHTABLE_FREE( table->memctx, table->slots );
    }



static int hashtable_internal_find_slot( hashtable_t const* table, HASHTABLE_U32 hash, void const* key )
    {
    HASHTABLE_U32 slot_capacity = (HASHTABLE_U32) table->slot_capacity;
    int const base_slot = (int)( hash % slot_capacity );

    int base_count = table->slots[ base_slot ].base_count;
    int slot = base_slot;
    while( base_count > 0 )
        {
        if( table->slots[ slot ].item_index >= 0 )
            {
            HASHTABLE_U32 slot_hash = table->slots[ slot ].key_hash;
            int slot_base = (int)( slot_hash % slot_capacity );
            if( slot_base == base_slot ) 
                {
                HASHTABLE_ASSERT( base_count > 0 );
                if( slot_hash == hash )
                    {
                    void const* slot_key = (void const*)( ( (uintptr_t) table->items_key ) + table->key_size * table->slots[ slot ].item_index );
                    if( HASHTABLE_KEYCMP( slot_key, key, table->key_size ) )
                        return slot;
                    }
                --base_count;
                }
            }
        slot = (int)( ( slot + 1 ) % slot_capacity );
        }   

    return -1;
    }


static void hashtable_internal_expand_slots( hashtable_t* table )
    {
    if( !table->slots ) return;

    int const old_capacity = table->slot_capacity;
    struct hashtable_internal_slot_t* old_slots = table->slots;

    table->slot_capacity = hashtable_internal_primes[ ++table->prime_index ];
    HASHTABLE_U32 slot_capacity = (HASHTABLE_U32) table->slot_capacity;

    int const size = (int)( table->slot_capacity * sizeof( *table->slots ) );
    table->slots = (struct hashtable_internal_slot_t*) HASHTABLE_MALLOC( table->memctx, (HASHTABLE_SIZE_T) size );
    HASHTABLE_ASSERT( table->slots );
    for( int i = 0; i < table->slot_capacity; ++i  )
        {
        table->slots[ i ].base_count = 0;
        table->slots[ i ].item_index = -1;
        }

    for( int i = 0; i < old_capacity; ++i )
        {
        if( old_slots[ i ].item_index >= 0 )
            {
            HASHTABLE_U32 const hash = old_slots[ i ].key_hash;
            int const base_slot = (int)( hash % slot_capacity );
            int slot = base_slot;
            while( table->slots[ slot ].item_index >= 0 )
                slot = (int)( ( slot + 1 ) % slot_capacity );
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
    
    void* const new_items_key = HASHTABLE_MALLOC( table->memctx, 
        table->item_capacity * ( table->key_size + sizeof( *table->items_slot ) + table->item_size ) + 
        ( table->key_size > table->item_size ? table->key_size : table->item_size ) );
    HASHTABLE_ASSERT( new_items_key );

    int* const new_items_slot = (int*)( ( (uintptr_t) new_items_key ) + table->key_size * table->item_capacity );
    void* const new_items_data = (void*)( new_items_slot + table->item_capacity );
    void* const new_swap_temp = (void*)( ( (uintptr_t) new_items_data ) + table->item_size * table->item_capacity ); 

    HASHTABLE_MEMCPY( new_items_key, table->items_key, table->count * table->key_size );
    HASHTABLE_MEMCPY( new_items_slot, table->items_slot, table->count * sizeof( *table->items_slot ) );
    HASHTABLE_MEMCPY( new_items_data, table->items_data, (HASHTABLE_SIZE_T) table->count * table->item_size );
    
    HASHTABLE_FREE( table->memctx, table->items_key );

    table->items_key = new_items_key;
    table->items_slot = new_items_slot;
    table->items_data = new_items_data;
    table->swap_temp = new_swap_temp;
    }


void hashtable_insert( hashtable_t* table, HASHTABLE_U32 hash, void const* key, void const* item )
    {
    if( !table->slots ) 
        {
        if( table->count >= table->item_capacity )
            hashtable_internal_expand_items( table );

        void* dest_item = (void*)( ( (uintptr_t) table->items_data ) + table->count * table->item_size );
        HASHTABLE_ITEMCOPY( dest_item, item, (HASHTABLE_SIZE_T) table->item_size );
        ++table->count;
        return;
        }

    HASHTABLE_ASSERT( hashtable_internal_find_slot( table, hash, key ) < 0 );

    if( table->count >= ( table->slot_capacity - table->slot_capacity / 3 ) )
        hashtable_internal_expand_slots( table );
        
    HASHTABLE_U32 slot_capacity = (HASHTABLE_U32) table->slot_capacity;
    int const base_slot = (int)( hash % slot_capacity );
    int base_count = table->slots[ base_slot ].base_count;
    int slot = base_slot;
    int first_free = slot;
    while( base_count )
        {
        if( table->slots[ slot ].item_index < 0 && table->slots[ first_free ].item_index >= 0 ) first_free = slot;
        HASHTABLE_U32 const slot_hash = table->slots[ slot ].key_hash;
        int slot_base = (int)( slot_hash % slot_capacity );
        if( slot_base == base_slot ) 
            --base_count;
        slot = (int)( ( slot + 1 ) % slot_capacity );
        }       

    slot = first_free;
    while( table->slots[ slot ].item_index >= 0  )
        slot = (int)( ( slot + 1 ) % slot_capacity );

    if( table->count >= table->item_capacity )
        hashtable_internal_expand_items( table );

    HASHTABLE_ASSERT( table->slots[ slot ].item_index < 0 && (int)( hash % slot_capacity ) == (HASHTABLE_U32) base_slot );
    table->slots[ slot ].key_hash = hash;
    table->slots[ slot ].item_index = table->count;
    ++table->slots[ base_slot ].base_count;

        
    void* dest_item = (void*)( ( (uintptr_t) table->items_data ) + table->count * table->item_size );
    HASHTABLE_ITEMCOPY( dest_item, item, (HASHTABLE_SIZE_T) table->item_size );
    void* dest_key = (void*)( ( (uintptr_t) table->items_key ) + table->count * table->key_size );
    HASHTABLE_KEYCOPY( dest_key, key, (HASHTABLE_SIZE_T) table->key_size );
    table->items_slot[ table->count ] = slot;
    ++table->count;
    } 


void hashtable_remove( hashtable_t* table, HASHTABLE_U32 hash, void const* key )
    {
    if( table->slots )
        {
        int const slot = hashtable_internal_find_slot( table, hash, key );
        HASHTABLE_ASSERT( slot >= 0 );

        HASHTABLE_U32 slot_capacity = (HASHTABLE_U32) table->slot_capacity;
        int const base_slot = (int)( hash % slot_capacity );
        int index = table->slots[ slot ].item_index;
        int last_index = table->count - 1;
        --table->slots[ base_slot ].base_count;
        table->slots[ slot ].item_index = -1;

        if( index != last_index )
            {
            void* dst_key = (void*)( ( (uintptr_t) table->items_key ) + index * table->key_size );
            void* src_key = (void*)( ( (uintptr_t) table->items_key ) + last_index * table->key_size );
            HASHTABLE_KEYCOPY( dst_key, src_key, (HASHTABLE_SIZE_T) table->key_size );
            table->items_slot[ index ] = table->items_slot[ last_index ];
            void* dst_item = (void*)( ( (uintptr_t) table->items_data ) + index * table->item_size );
            void* src_item = (void*)( ( (uintptr_t) table->items_data ) + last_index * table->item_size );
            HASHTABLE_ITEMCOPY( dst_item, src_item, (HASHTABLE_SIZE_T) table->item_size );
            table->slots[ table->items_slot[ last_index ] ].item_index = index;
            }
        }
    --table->count;
    } 


void hashtable_clear( hashtable_t* table )
    {
    table->count = 0;
    if( table->slots )
        {
        for( int i = 0; i < table->slot_capacity; ++i  )
            {
            table->slots[ i ].base_count = 0;
            table->slots[ i ].item_index = -1;
            }  
        }
    }


void* hashtable_find( hashtable_t const* table, HASHTABLE_U32 hash, void const* key )
    {
    int const slot = table->slots ? hashtable_internal_find_slot( table, hash, key ) : -1;
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


void const* hashtable_keys( hashtable_t const* table )
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

    void* key_a = (void*)( ( (uintptr_t) table->items_key ) + index_a * table->key_size );
    void* key_b = (void*)( ( (uintptr_t) table->items_key ) + index_b * table->key_size );
    HASHTABLE_KEYCOPY( table->swap_temp, key_a, table->key_size );
    HASHTABLE_KEYCOPY( key_a, key_b, table->key_size );
    HASHTABLE_KEYCOPY( key_b, table->swap_temp, table->key_size );

    void* item_a = (void*)( ( (uintptr_t) table->items_data ) + index_a * table->item_size );
    void* item_b = (void*)( ( (uintptr_t) table->items_data ) + index_b * table->item_size );
    HASHTABLE_ITEMCOPY( table->swap_temp, item_a, table->item_size );
    HASHTABLE_ITEMCOPY( item_a, item_b, table->item_size );
    HASHTABLE_ITEMCOPY( item_b, table->swap_temp, table->item_size );

    if( table->slots )
        {
        table->slots[ slot_a ].item_index = index_b;
        table->slots[ slot_b ].item_index = index_a;
        }
    }


#endif /* HASHTABLE_IMPLEMENTATION */

/*

contributors:
    Randy Gaul (hashtable_clear, hashtable_swap )

revision history:
    2.0     variable key size, custom hashing
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
