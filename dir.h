
/*
------------------------------------------------------------------------------
		  Licensing information can be found at the end of the dir.
------------------------------------------------------------------------------

dir.h - v0.1 - Directory listing functions for C/C++

Do this:
	#define DIR_IMPLEMENTATION
before you include this file in *one* C/C++ dir to create the implementation.
*/

#ifndef dir_h
#define dir_h

typedef struct dir_t dir_t;
typedef struct dir_entry_t dir_entry_t;

dir_t* dir_open( char const* path );
void dir_close( dir_t* dir );
char const* dir_path( dir_t* dir );
dir_entry_t* dir_read( dir_t* dir );

char const* dir_name( dir_entry_t* entry );
int dir_is_file( dir_entry_t* entry );
int dir_is_folder( dir_entry_t* entry );

#endif /* dir_h */

/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifdef DIR_IMPLEMENTATION
#undef DIR_IMPLEMENTATION

#ifndef DIR_MALLOC
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <stdlib.h>
	#if defined(__cplusplus)
		#define DIR_MALLOC( size ) ( ::malloc( size ) )
		#define DIR_FREE( ptr ) ( ::free( ptr ) )
	#else
		#define DIR_MALLOC( size ) ( malloc( size ) )
		#define DIR_FREE( ptr ) ( free( ptr ) )
	#endif
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    WINDOWS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined( DIR_WINDOWS )

#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0501 
	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x0501 // requires Windows XP minimum
#endif
// 0x0400=Windows NT 4.0, 0x0500=Windows 2000, 0x0501=Windows XP, 0x0502=Windows Server 2003, 0x0600=Windows Vista, 
// 0x0601=Windows 7, 0x0602=Windows 8, 0x0603=Windows 8.1, 0x0A00=Windows 10, 
#define _WINSOCKAPI_
#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#pragma warning( disable: 4668 ) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#pragma warning( disable: 4768 ) // __declspec attributes before linkage specification are ignored	
#pragma warning( disable: 4255 ) // 'function' : no function prototype given: converting '()' to '(void)'
#define _NTDDSCM_H_
#include <windows.h>
#pragma warning( pop )

struct dir_entry_t 
	{
	char name[ MAX_PATH ];
	BOOL is_folder;
	};


struct dir_t
	{
    char path[ MAX_PATH ];
	HANDLE handle;
	WIN32_FIND_DATAA data;
	dir_entry_t entry;
	};


dir_t* dir_open( char const* path )
	{
	if( !path ) return 0;

	size_t path_len = strlen( path );
	BOOL trailing_path_separator = path[ path_len - 1 ] == '\\' || path[ path_len - 1 ] == '/';
	const char* string_to_append = "*.*";
	if( path_len + strlen( string_to_append ) + ( trailing_path_separator ? 0 : 1 ) >= MAX_PATH ) return NULL;
	char search_pattern[ MAX_PATH ];
	strcpy( search_pattern, path );
	if( !trailing_path_separator ) strcat( search_pattern, "\\" );
	strcat( search_pattern, string_to_append );

	WIN32_FIND_DATAA data;
	HANDLE handle = FindFirstFileA( search_pattern, &data );
	if( handle == INVALID_HANDLE_VALUE ) return NULL;

	dir_t* dir = (dir_t*) DIR_MALLOC( sizeof( dir_t ) );
    strcpy( dir->path, path );
	dir->handle = handle;
	dir->data = data;

	return dir;
	}


void dir_close( dir_t* dir )
	{
	if( !dir ) return;

	if( dir->handle != INVALID_HANDLE_VALUE ) FindClose( dir->handle );
	DIR_FREE( dir );
	}


char const* dir_path( dir_t* dir )
	{
	if( !dir ) return NULL;
	return dir->path;
	}


dir_entry_t* dir_read( dir_t* dir )
	{
	if( !dir ) return NULL;
	if( dir->handle == INVALID_HANDLE_VALUE ) return NULL;

	strcpy( dir->entry.name, dir->data.cFileName );
	dir->entry.is_folder = ( dir->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;

	BOOL result = FindNextFileA( dir->handle, &dir->data );
	if( !result )
		{
		FindClose( dir->handle );
		dir->handle = INVALID_HANDLE_VALUE;      
		}

	return &dir->entry;    
	}


char const* dir_name( dir_entry_t* entry )
	{
	if( !entry ) return NULL;
	return entry->name;
	}


int dir_is_file( dir_entry_t* entry )
	{
	if( !entry ) return 0;
	return entry->is_folder == FALSE;
	}


int dir_is_folder( dir_entry_t* entry )
	{
	if( !entry ) return 0;
	return entry->is_folder == TRUE;
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    POSIX
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif defined( DIR_POSIX )

#include <dirent.h>


dir_t* dir_open( char const* path )
	{
	DIR* dir = opendir( path );
	return (dir_t*) dir;
	}


void dir_close( dir_t* dir )
	{
	if( !dir ) return;
	closedir( (DIR*) dir );
	}


dir_entry_t* dir_read( dir_t* dir )
	{
	if( !dir ) return NULL;
	return (dir_entry_t*)readdir( (DIR*) dir );
	}


char const* dir_name( dir_entry_t* entry )
	{
	if( !entry ) return NULL;
	return ( (struct dirent*)entry )->d_name;
	}


int dir_is_file( dir_entry_t* entry )
	{
	if( !entry ) return 0;
	return ( (struct dirent*)entry )->d_type == DT_REG;
	}


int dir_is_folder( dir_entry_t* entry )
	{
	if( !entry ) return 0;
	return ( (struct dirent*)entry )->d_type == DT_DIR;
	}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    PLATFORM ERROR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else

	#error Undefined platform. Define DIR_WINDOWS or DIR_POSIX.

#endif 


#endif /* DIR_IMPLEMENTATION */

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2017 Mattias Gustavsson

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
