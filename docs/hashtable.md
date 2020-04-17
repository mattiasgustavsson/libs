hashtable.h
===========

Library: [hashtable.h](../hashtable.h)


Example
=======

```cpp
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
```


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

