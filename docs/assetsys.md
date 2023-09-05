assetsys.h
==========

Library: [assetsys.h](../assetsys.h)


Example
=======

```cpp
#define ASSETSYS_IMPLEMENTATION
#include "libs/assetsys.h"                                                                                                                                                           

#include <stdio.h> // for printf

void list_assets( assetsys_t* assetsys, char const* path, int indent )
	{
	// Print folder names and recursively list assets
	for( int i = 0; i < assetsys_subdir_count( assetsys, path ); ++i )
		{
		char const* subdir_name = assetsys_subdir_name( assetsys, path, i );
		for( int j = 0; j < indent; ++j ) printf( "  " );
		printf( "%s/\n", subdir_name );

		char const* subdir_path = assetsys_subdir_path( assetsys, path, i );
		list_assets( assetsys, subdir_path, indent + 1 );
		}

	// Print file names
	for( int i = 0; i < assetsys_file_count( assetsys, path ); ++i )
		{
		char const* file_name = assetsys_file_name( assetsys, path, i );
		for( int j = 0; j < indent; ++j ) printf( "  " );
		printf( "%s\n", file_name );
		}
	}

int main( int, char** )
	{
	assetsys_t* assetsys = assetsys_create( 0 );

	// Mount current working folder as a virtual "/data" path
	assetsys_mount( assetsys, ".", "/data" );

	// Print all files and subfolders
	list_assets( assetsys, "/", 0 ); // Start at root 

	// Load a file
	assetsys_file_t file;
	assetsys_file( assetsys, "/data/readme.txt", &file );
	int size = assetsys_file_size( assetsys, file );
	char* content = (char*) malloc( size + 1 ); // extra space for '\0'
	assetsys_file_load( assetsys, file, content );
	content[ size ] = '\0'; // zero terminate the text file
	printf( "%s\n", content );
	free( content );

	assetsys_destroy( assetsys );
	}
```


API Documentation
=================

assetsys.h is a system for loading binary assets into your game. It allows you to mount directories and archive files 
(bundles of files; assetsys.h supports using standard zip files for this) assign them a virtual path. You then load 
assets through assetsys using the virtual path (which can stay the same even if the mounts change). assetsys.h is 
case-insensitive regardless of platform, and only accepts forward slash "/" as path separator, to ensure consistent 
behavior. It allows you to mount several paths or archive files to the same virtual path, even if they contain files of 
the same name, and a later mount will take precedence over a previous one. This allows you to, for example, have a 
`data` archive file and an `update` archive file, where `update` contains new versions of only some of the files.

assetsys.h is a single-header library, and does not need any .lib files or other binaries, or any build scripts. To use 
it, you just include assetsys.h to get the API declarations. To get the definitions, you must include assetsys.h from 
*one* single C or C++ file, and #define the symbol `ASSETSYS_IMPLEMENTATION` before you do. 

assetsys.h has a dependency om another single-header library, strpool.h, which is used for efficient storage and 
comparison of the filename and path strings. assetsys.h automatically includes strpool.h, so it must reside in the same
path. It does not specify the STRPOOL_IMPLEMENTATION define, on the assumption that you might be including strpool.h in
some other part of your program. If you are not, you can make assetsys.h include the strpool implemention by doing:

    #define ASSETSYS_IMPLEMENTATION
    #define STRPOOL_IMPLEMENTATION
    #include "assetsys.h"


Customization
-------------
There are a few different things in assetsys.h which are configurable by #defines. Most of the API use the `int` data 
type, for integer values where the exact size is not important. However, for some functions, it specifically makes use 
of an unsigned 64 bit data types. It default to using `unsigned long long`, but can be redefined by #defining 
ASSETSYS_U64, before including assetsys.h. This is useful if you, for example, use the types from `<stdint.h>` in the 
rest of your program, and you want assetsys.h to use compatible types. In this case, you would include assetsys.h using 
the following code:

    #define ASSETSYS_U64 uint64_t
    #include "assetsys.h"

Note that when customizing the data type, you need to use the same definition in every place where you include 
assetsys.h, as they affect the declarations as well as the definitions.

The rest of the customizations only affect the implementation, so will only need to be defined in the file where you
have the #define ASSETSYS_IMPLEMENTATION.


### Custom memory allocators

To store the internal data structures, ini.h needs to do dynamic allocation by calling `malloc`. Programs might want to 
keep track of allocations done, or use custom defined pools to allocate memory from. assetsys.h allows for specifying 
custom memory allocation functions for `malloc` and `free`. This is done with the following code:

    #define ASSETSYS_IMPLEMENTATION
    #define ASSETSYS_MALLOC( ctx, size ) ( my_custom_malloc( ctx, size ) )
    #define ASSETSYS_FREE( ctx, ptr ) ( my_custom_free( ctx, ptr ) )
    #include "assetsys.h"

where `my_custom_malloc` and `my_custom_free` are your own memory allocation/deallocation functions. The `ctx` parameter
is an optional parameter of type `void*`. When `assetsys_init` is called, you can set the `memctx` field of the `config`
parameter, to a pointer to anything you like, and which will be passed through as the `ctx` parameter to every 
`ASSETSYS_MALLOC`/`ASSETSYS_FREE` call. For example, if you are doing memory tracking, you can pass a pointer to your 
tracking data as `memctx`, and in your custom allocation/deallocation function, you can cast the `ctx` param back to the 
right type, and access the tracking data.

If no custom allocator is defined, assetsys.h will default to `malloc` and `free` from the C runtime library.


### Custom assert

assetsys.h makes use of asserts to report usage errors and code errors. By default, it makes use of the C runtime 
library `assert` macro, which only executes in debug builds. However, it allows for substituting with your own assert 
function or macro using the following code:

    #define ASSETSYS_IMPLEMENTATION
    #define ASSETSYS_ASSERT( condition ) ( my_custom_assert( condition ) )
    #include "assetsys.h"

Note that if you only want the asserts to trigger in debug builds, you must add a check for this in your custom assert.


### miniz implementation

assetsys.h makes use of the miniz library for parsing and decompressing zip files. It includes the entire miniz source
code inside assetsys.h, so normally you don't have to worry about it. However, in the case where you might already be
using miniz in some other part of the program, you can tell assetsys.h to not include the implementation for miniz. It 
will still include the miniz definitions, and if you don't include the miniz implementation elsewhere, you will get a 
linker error. To exclude the miniz implementation, simply define `ASSETSYS_NO_MINIZ` before including assetsys.h, like 
this:

    #define ASSETSYS_IMPLEMENTATION
    #define ASSETSYS_NO_MINIZ
    #include "assetsys.h"

    
assetsys_create
---------------

    assetsys_t* assetsys_create( void* memctx )

Creates a new assetsys instance. assetsys.h does not use any global variables, all data it needs is accessed through
the instance created by calling assetsys_create. Different instances can be used safely from different threads, but if
using the same instance from multiple threads, it is up to the user to make sure functions are not called concurrently,
for example by adding a mutex lock around each call.


assetsys_destroy
----------------

    void assetsys_destroy( assetsys_t* sys )

Destroys an assetsys instance, releasing all the resources used by it. 


assetsys_mount
--------------

    assetsys_error_t assetsys_mount( assetsys_t* sys, char const* path, char const* mount_as )

Mounts the data source `path`, making all its files accessible through this assetsys instance. The data source can be
either a folder or an archive file (a standard .zip file, with or without compression). `path` must be a relative path,
and must use forward slash `/` as path separator, never backslash, regardless of platform. It must not end with a path 
separator. The string `mount_as` will be prepended to all mounted files, and can be passed as an empty string "" if 
desired. `mount_as` may not contain the characters backslash `\` or colon `:`. It must not end with a path separator.
`assetsys_mount` will return `ASSETSYS_ERROR_INVALID_PARAMETER` if either `path` or `mount_as` is NULL. It will return
`ASSETSYS_ERROR_INVALID_PATH` if the conditions detailed above are not met, or if the file or folder specified by `path`
could not be found. If `path` indicates a file, and it is not a valid archive file, `assetsys_mount` returns
`ASSETSYS_ERROR_FAILED_TO_READ_ZIP`.

If multiple mounts contains the same file and it is accessible through the same full path (whether because of the 
`mount_as` prefix or not), the last mounted data source will be used when loading that file.


assetsys_mount_from_memory
-------------------

    assetsys_error_t assetsys_mount_from_memory( assetsys_t* sys, void const* data, size_t size, char const* mount_as )

Same as `assetsys_mount()`, but takes a data buffer of an archived *.zip* file, along with the size of the file.

assetsys_dismount
-----------------

    assetsys_error_t assetsys_dismount( assetsys_t* sys, char const* path, char const* mounted_as )

Removes a data source which was mounted by calling `assetsys_mount`. `path` and `mounted_as` must be the same as was
used when mounting. If `path` is NULL, `assetsys_dismount` returns `ASSETSYS_ERROR_INVALID_PARAMETER`. If `mounted_as`
is NULL, or no matching mount could be found, it returns `ASSETSYS_ERROR_INVALID_MOUNT`. 


assetsys_file
-------------

    assetsys_error_t assetsys_file( assetsys_t* sys, char const* path, assetsys_file_t* file )

Retrieves a handle for the file specified by `path`. `path` needs to be an absolute path, including the `mount_as` 
prefix specified when the data source was mounted, and matching is case insensitive. The mounts are searched in reverse
order they were added, and if a file with the specified path could not be found, `assetsys_file` returns
`ASSETSYS_ERROR_FILE_NOT_FOUND`. The handle is written to `file`, which must be a pointer to a `assetsys_file_t`
variable declared by the caller. The handle is used in calls to `assetsys_file_load` and `assetsys_file_size`. The 
handle is only valid until any mounts are modified by calling `assetsys_mount` or `assetsys_dismount`.


assetsys_file_load
------------------

    assetsys_error_t assetsys_file_load( assetsys_t* sys, assetsys_file_t file, void* buffer )

Load the data from the file specified by the handle `file` (initialized by calling `assetsys_file`) and writes it into
the memory indicated by `buffer`. This memory buffer must be large enough to fit the entire file. To find out how large
the buffer needs to be, call `assetsys_file_size`. If the file could not be loaded, `assetsys_file_load` returns
`ASSETSYS_ERROR_FAILED_TO_READ_FILE`.


assetsys_file_size
------------------

    int assetsys_file_size( assetsys_t* sys, assetsys_file_t file )

Returns the size, in bytes, of the file specified by the handle `file` (initialized by calling `assetsys_file`). If the
file handle is not valid, `assetsys_file_size` returns 0.


assetsys_file_count
-------------------

    int assetsys_file_count( assetsys_t* sys, char const* path )

Returns the number of files in the directory with the specified path, or 0 if the path is invalid. `path` needs to be an 
absolute path, including the `mount_as` prefix specified when the data source was mounted, and matching is case 
insensitive. `assetsys_file_count` returns the total number of files from all mounts, which fall under the path.


assetsys_file_name
------------------
    
    char const* assetsys_file_name( assetsys_t* sys, char const* path, int index )

Returns the filename and extension (but not the full path) of one of the files in the specified path. `path` needs to be 
an absolute path, including the `mount_as` prefix specified when the data source was mounted, and matching is case 
insensitive. `index` needs to be between 0 and one less than the count returned by calling `assetsys_file_count` with
the same path. If the path is invalid or index is out of range, `assetsys_file_name` returns NULL.


assetsys_file_path
------------------

    char const* assetsys_file_path( assetsys_t* sys, char const* path, int index )

Returns the full path, including filename and extension, of one of the files in the specified path. `path` needs to be 
an absolute path, including the `mount_as` prefix specified when the data source was mounted, and matching is case 
insensitive. `index` needs to be between 0 and one less than the count returned by calling `assetsys_file_count` with
the same path. If the path is invalid or index is out of range, `assetsys_file_path` returns NULL.


assetsys_subdir_count
---------------------

    int assetsys_subdir_count( assetsys_t* sys, char const* path )

Returns the number of subdirectories in the directory with the specified path, or 0 if the path is invalid. `path` needs 
to be an absolute path, including the `mount_as` prefix specified when the data source was mounted, and matching is case 
insensitive. `assetsys_subdir_count` returns the total number of directories from all mounts, which fall under the path.


assetsys_subdir_name
--------------------

    char const* assetsys_subdir_name( assetsys_t* sys, char const* path, int index )

Returns the name (but not the full path) of one of the subdirectories in the specified path. `path` needs to be an 
absolute path, including the `mount_as` prefix specified when the data source was mounted, and matching is case 
insensitive. `index` needs to be between 0 and one less than the count returned by calling `assetsys_subdir_count` with
the same path. If the path is invalid or index is out of range, `assetsys_subdir_name` returns NULL.


assetsys_subdir_path
--------------------

    char const* assetsys_subdir_path( assetsys_t* sys, char const* path, int index )

Returns the name, including the full path, of one of the files in the specified path. `path` needs to be an absolute 
path, including the `mount_as` prefix specified when the data source was mounted, and matching is case insensitive. 
`index` needs to be between 0 and one less than the count returned by calling `assetsys_subdir_count` with the same 
path. If the path is invalid or index is out of range, `assetsys_subdir_path` returns NULL.
