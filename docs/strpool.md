strpool.h
=========

Library: [strpool.h](../strpool.h)


Example
=======

```cpp
#define  STRPOOL_IMPLEMENTATION
#include "strpool.h"

#include <stdio.h> // for printf
#include <string.h> // for strlen

int main( int argc, char** argv )
	{
	(void) argc, argv;

	strpool_config_t conf = strpool_default_config;
	//conf.ignore_case = true;

	strpool_t pool;
	strpool_init( &pool, &conf );

	STRPOOL_U64 str_a = strpool_inject( &pool, "This is a test string", (int) strlen( "This is a test string" ) );
	STRPOOL_U64 str_b = strpool_inject( &pool, "THIS IS A TEST STRING", (int) strlen( "THIS IS A TEST STRING" ) );

	printf( "%s\n", strpool_cstr( &pool, str_a ) );
	printf( "%s\n", strpool_cstr( &pool, str_b ) );
	printf( "%s\n", str_a == str_b ? "Strings are the same" : "Strings are different" );

	strpool_term( &pool );
	return 0;
	}
```


API Documentation
=================

strpool.h is a system for string interning, where each string is stored only once. It makes comparing strings very fast,
as it will just be uint64 comparison rather than looping over strings and comparing character by character. strpool.h is
also optimized for fast creation of strings, and with an efficient memory allocation scheme.

strpool.h is a single-header library, and does not need any .lib files or other binaries, or any build scripts. To use 
it, you just include strpool.h to get the API declarations. To get the definitions, you must include strpool.h from 
*one* single C or C++ file, and #define the symbol `STRPOOL_IMPLEMENTATION` before you do. 


Customization
-------------
There are a few different things in strpool.h which are configurable by #defines. Most of the API use the `int` data 
type, for integer values where the exact size is not important. However, for some functions, it specifically makes use 
of 32 and 64 bit data types. These default to using `unsigned int` and `unsigned long long` by default, but can be
redefined by #defining STRPOOL_U32 and STRPOOL_U64 respectively, before including strpool.h. This is useful if you, for 
example, use the types from `<stdint.h>` in the rest of your program, and you want strpool.h to use compatible types. In 
this case, you would include strpool.h using the following code:

    #define STRPOOL_U32 uint32_t
    #define STRPOOL_U64 uint64_t
    #include "strpool.h"

Note that when customizing the data types, you need to use the same definition in every place where you include 
strpool.h, as they affect the declarations as well as the definitions.

The rest of the customizations only affect the implementation, so will only need to be defined in the file where you
have the #define STRPOOL_IMPLEMENTATION.

Note that if all customizations are utilized, strpool.h will include no external files whatsoever, which might be useful
if you need full control over what code is being built.


### Custom memory allocators

To store strings and the internal structures (entry list, hashtable etc) strpool.h needs to dodynamic allocation by 
calling `malloc`. Programs might want to keep track of allocations done, or use custom defined pools to allocate memory 
from. strpool.h allows for specifying custom memory allocation functions for `malloc` and `free`.
This is done with the following code:

    #define STRPOOL_IMPLEMENTATION
    #define STRPOOL_MALLOC( ctx, size ) ( my_custom_malloc( ctx, size ) )
    #define STRPOOL_FREE( ctx, ptr ) ( my_custom_free( ctx, ptr ) )
    #include "strpool.h"

where `my_custom_malloc` and `my_custom_free` are your own memory allocation/deallocation functions. The `ctx` parameter
is an optional parameter of type `void*`. When `strpool_init` is called, you can set the `memctx` field of the `config`
parameter, to a pointer to anything you like, and which will be passed through as the `ctx` parameter to every 
`STRPOOL_MALLOC`/`STRPOOL_FREE` call. For example, if you are doing memory tracking, you can pass a pointer to your 
tracking data as `memctx`, and in your custom allocation/deallocation function, you can cast the `ctx` param back to the 
right type, and access the tracking data.

If no custom allocator is defined, strpool.h will default to `malloc` and `free` from the C runtime library.


### Custom assert

strpool.h makes use of asserts to report usage errors and failed allocation errors. By default, it makes use of the C 
runtime library `assert` macro, which only executes in debug builds. However, it allows for substituting with your own
assert function or macro using the following code:

    #define STRPOOL_IMPLEMENTATION
    #define STRPOOL_ASSERT( condition ) ( my_custom_assert( condition ) )
    #include "strpool.h"

Note that if you only want the asserts to trigger in debug builds, you must add a check for this in your custom assert.


### Custom C runtime function

The library makes use of four additional functions from the C runtime library, and for full flexibility, it allows you 
to substitute them for your own. Here's an example:

    #define STRPOOL_IMPLEMENTATION
    #define STRPOOL_MEMSET( ptr, val, cnt ) ( my_memset_func( ptr, val, cnt ) )
    #define STRPOOL_MEMCPY( dst, src, cnt ) ( my_memcpy_func( dst, src, cnt ) )
    #define STRPOOL_MEMCMP( pr1, pr2, cnt ) ( my_memcmp_func( pr1, pr2, cnt ) )
    #define STRPOOL_STRNICMP( s1, s2, len ) ( my_strnicmp_func( s1, s2, len ) )
    #include "strpool.h"

If no custom function is defined, strpool.h will default to the C runtime library equivalent.


strpool_init
------------

    void strpool_init( strpool_t* pool, strpool_config_t const* config )

Initializes an instance of the string pool. The `config` parameter allows for controlling the behavior of the pool
instance. It contains the following fields:

* memctx - pointer to user defined data which will be passed through to custom STRPOOL_MALLOC/STRPOOL_FREE calls. May 
    be NULL.
* ignore_case - set to 0 to make strings case sensitive, set to 1 to make strings case insensitive. Default is 0.
* counter_bits - how many bits of the string handle to use for keeping track of handle reuse and invalidation. Default
    is 32. See below for details about the handle bits.
* index_bits - how many bits of the string handle to use for referencing string instances. Default is 32. See below for 
    details about the handle bits.
* entry_capacity - number of string instance entries to pre-allocate space for when pool is initialized. Default 
    is space for 4096 string entries.
* block_capacity - number of string storage block entries to pre-allocate space for when pool is initialized. Default 
    is space for 32 entries of block information - though only a single block will be pre-allocated.
* block_size - size to allocate for each string storage block. A higher value might mean you often have more memory 
    allocated than is actually being used. A lower value means you will be making allocations more often. Default 
    is 256 kilobyte. 
* min_length - minimum space to allocate for each string. A higher value wastes more space, but makes it more likely 
    that recycled storage can be re-used by subsequent requests. Default is a string length of 23 characters.

The function `strpool_inject` returns a 64-bit handle. Using the settings `counter_bits`/`index_bits`, you can control
how many bits of the handle is in use, and how many are used for index vs counter. For example, setting `counter_bits`
to 8 and `index_bits` to 24, you will get a handle which only uses 32 bits in total, can store some 16 million different
strings (2^24 -1), with a reuse counter going up to 255 before wrapping around. In this scenario, the 64-bit handle 
returned from strpool_inject can be safely truncated and stored in a 32-bit variable. Any combination of counter vs 
index bits is possible, but the number of strings which can be stored will be limited by `index_bits`. 

The counter bits need a more detailed description. When a string is added via `strpool_inject`, you get a handle back, 
which is used to reference the string. You might store this handle in various data structures. When removing a string by
calling `strpool_discard`, the index part of that handle will be recycled, and re-used for future `strpool_inject`.
However, when a string is discarded, the counter part of the handle will be increased, so the system knows that any 
handles that are still being kept around in any data structure, does no longer point to a valid string (and thus will 
return a NULL string pointer when queried through `strpool_cstr`). If your use case involves creating a bunch of strings
and just keeping them around until the application terminates, it is fine to specify 0 for `counter_bits`, thereby
effectively disabling the handle validation all together. If you have a case where strings are being repeatedly created 
and removed, but strings are queried very frequently, then you can specify a low number for `counter_bits` (since you 
can invalidate any stored handles as soon as you find out it's been invalidated). If you have a case where handles might
sit inside data structures for a long period of time before you check their validity, you best specify a high value for
`counter_bits`. The default value is 32 bits for index and 32 bits for counter.


strpool_term
------------

    void strpool_term( strpool_t* pool )

Terminates a string pool instance, releasing all memory used by it. No further calls to the strpool API are valid until
the instance is reinitialized by another call to `strpool_init`.


strpool_defrag
--------------

    void strpool_defrag( strpool_t* pool )

As strings are added to and removed from the pool, the memory blocks holding the strings may get fragmented, making them
take up more allocated memory than is actually needed to store the set of strings, should they be packed tightly,
`strpool_defrag` consolidates all strings into a single memory block (which might well be larger than the block size 
specified when the pool was initialized), and recreates the internal hash table. Any additionally allocated memory 
blocks will be deallocated, making the memory used by the pool as little as it can be to fit the current set of strings.
All string handles remain valid after a call to `strpool_defrag`.


strpool_inject
--------------

    STRPOOL_U64 strpool_inject( strpool_t* pool, char const* string, int length )

Adds a string to the pool, and returns a handle for the added string. If the string doesn't exist, it will be stored in
an unused part of an already allocated storage block (or a new block will be allocated if there is no free space large
enough to hold the string), and inserted into the internal hash table. If the string already exists, a handle to the
existing string will be returned. If the `string` parameter is already pointing to a string stored in the pool, there 
are specific optimizations for avoiding to loop over it to calculate the hash for it, with the idea being that you 
should be able to use char* types when you're passing strings around, and use the string pool for storage, without too
much of a performance penalty. `string` does not have to be null terminated, but when it is retrieved from the string 
pool, it will be. If `string` is NULL or length is 0, a handle with a value of 0 will be returned, to signify empty
string.


strpool_discard
---------------

    void strpool_discard( strpool_t* pool, STRPOOL_U64 handle )

Removes a string from the pool. Any handles held for the string will be invalid after this. Memory used for storing the
string will be recycled and used for further `strpool_inject` calls. If `handle` is invalid, `strpool_discard` will do
nothing.


strpool_incref
--------------

    int strpool_incref( strpool_t* pool, STRPOOL_U64 handle )

`strpool.h` supports reference counting of strings. It is optional and not automatic, and does not automatically discard
strings when the reference count reaches 0. To use reference counting, make sure to call `strpool_incref` whenever you
add a string (after having called `strpool_inject`), and to call `strpool_decref` whenever you want to remove a string
(but only call `strpool_discard` if reference count have reached 0). It would be advisable to write wrapper functions
to ensure consistency in this, or if C++ is used, a wrapper class with constructors/destructor. `strpool_incref` returns
the reference count after increasing it. If `handle` is invalid, `strpool_incref` will do nothing, and return 0.


strpool_decref
--------------

    int strpool_decref( strpool_t* pool, STRPOOL_U64 handle )

Decreases the reference count of the specified string by 1, returning the reference count after decrementing. If the
reference count is less than 1, an assert will be triggered. If `handle` is invalid, `strpool_decref` will do nothing, 
and return 0.


strpool_getref
--------------

    int strpool_getref( strpool_t* pool, STRPOOL_U64 handle )

Returns the current reference count for the specified string. If `handle` is invalid, `strpool_getref` will do nothing, 
and return 0.


strpool_isvalid
---------------
    
    int strpool_isvalid( strpool_t const* pool, STRPOOL_U64 handle )

Returns 1 if the specified string handle is valid, and 0 if it is not. 


strpool_cstr
------------

    char const* strpool_cstr( strpool_t const* pool, STRPOOL_U64 handle )

Returns the zero-terminated C string for the specified string handle. The resulting string pointer is only valid as long
as no call is made to `strpool_init`, `strpool_term`, `strpool_defrag` or `strpool_discard`. It is therefor recommended
to never store the C string pointer, and always grab it fresh by another call to `strpool_cstr` when it is needed.
`strpool_cstr` is a very fast function to call - it does little more than an array lookup. If `handle` is invalid, 
`strpool_cstr` returns NULL. 


strpool_length
--------------

    int strpool_length( strpool_t const* pool, STRPOOL_U64 handle )

Returns the length, in characters, of the specified string. The resulting value is only valid as long as no call is made 
to `strpool_init`, `strpool_term`, `strpool_defrag` or `strpool_discard`. It is therefor recommended to never store the 
value, and always grab it fresh by another call to `strpool_length` when it is needed. `strpool_length` is a very fast 
function to call - it does little more than an array lookup. If `handle` is invalid, `strpool_length` returns 0.


strpool_collate
---------------

    char* strpool_collate( strpool_t const* pool, int* count )

Returns a list of all the strings currently stored in the string pool, and stores the number of strings in the int
variable pointed to by `count`. If there are no strings in the string pool, `strpool_collate` returns NULL. The pointer
returned points to the first character of the first string. Strings are zero-terminated, and immediately after the 
termination character, comes the first character of the next string.


strpool_free_collated
---------------------

    void strpool_free_collated( strpool_t const* pool, char* collated_ptr )

Releases the memory returned by `strpool_collate`. 

