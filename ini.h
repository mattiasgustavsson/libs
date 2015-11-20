/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

ini.h - v1.0 - Simple ini-file reader for C/C++.

Do this:
	#define INI_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef ini_h
#define ini_h

#define INI_GLOBAL_SECTION ( 0 )
#define INI_NOT_FOUND ( -1 )

typedef struct ini_t ini_t;

ini_t* ini_create();
ini_t* ini_load( char const* data );

ini_t* ini_create_memctx( void* memctx );
ini_t* ini_load_memctx( char const* data, void* memctx );

int ini_save( ini_t const* ini, char* data, int size );
void ini_destroy( ini_t* ini );

int ini_section_count( ini_t const* ini );
char const* ini_section_name( ini_t const* ini, int section );

int ini_property_count( ini_t const* ini, int section );
char const* ini_property_name( ini_t const* ini, int section, int property );
char const* ini_property_value( ini_t const* ini, int section, int property );

int ini_find_section( ini_t const* ini, char const* name );
int ini_find_property( ini_t const* ini, int section, char const* name );
int ini_find_section_l( ini_t const* ini, char const* name, int name_length );
int ini_find_property_l( ini_t const* ini, int section, char const* name, int name_length );

int ini_section_add( ini_t* ini, char const* name );
int ini_section_add_l( ini_t* ini, char const* name, int length );
void ini_property_add( ini_t* ini, int section, char const* name, char const* value );
void ini_property_add_l( ini_t* ini, int section, char const* name, int name_length, char const* value, int value_length );
void ini_section_remove( ini_t* ini, int section );
void ini_property_remove( ini_t* ini, int section, int property );

void ini_section_name_set( ini_t* ini, int section, char const* name );
void ini_section_name_set_l( ini_t* ini, int section, char const* name, int length );
void ini_property_name_set( ini_t* ini, int section, int property, char const* name );
void ini_property_name_set_l( ini_t* ini, int section, int property, char const* name, int length );
void ini_property_value_set( ini_t* ini, int section, int property, char const* value );
void ini_property_value_set_l( ini_t* ini, int section, int property, char const* value, int length  );

#endif /* ini_h */


/*

Examples:

------------------------------------------------------------------------------

	// Loading an ini file and retrieving values
	#define INI_IMPLEMENTATION
	#include "ini.h"

	#include <stdio.h>
	#include <malloc.h>

	int main()
		{
		FILE* fp = fopen( "test.ini", "r" );
		fseek( fp, 0, SEEK_END );
		int size = ftell( fp );
		fseek( fp, 0, SEEK_SET );
		char* data = (char*) malloc( size + 1 );
		fread( data, 1, size, fp );
		data[ size ] = '\0';
		fclose( fp );

		ini_t* ini = ini_load( data );
		free( data );
		int second_index = ini_find_property( ini, INI_GLOBAL_SECTION, "SecondSetting" );
		char const* second = ini_property_value( ini, INI_GLOBAL_SECTION, second_index );
		printf( "%s=%s\n", "SecondSetting", second );
		int section = ini_find_section( ini, "MySection" );
		int third_index = ini_find_property( ini, section, "ThirdSetting" );
		char const* third = ini_property_value( ini, section, third_index );
		printf( "%s=%s\n", "ThirdSetting", third );
		ini_destroy( ini );

		return 0;
		}

------------------------------------------------------------------------------

	// Creating a new ini file
	#define INI_IMPLEMENTATION
	#include "ini.h"

	#include <stdio.h>
	#include <malloc.h>

	int main()
		{		
		ini_t* ini = ini_create();
		ini_property_add( ini, INI_GLOBAL_SECTION, "FirstSetting", "Test" );
		ini_property_add( ini, INI_GLOBAL_SECTION, "SecondSetting", "2" );
		int section = ini_section_add( ini, "MySection" );
		ini_property_add( ini, section, "ThirdSetting", "Three" );

		int size = ini_save( ini, 0, 0 );
		char* data = (char*) malloc( size );
		size = ini_save( ini, data, size );
		ini_destroy( ini );

		FILE* fp = fopen( "test.ini", "w" );
		fwrite( data, 1, size, fp );
		fclose( fp );
		free( data );

		return 0;
		}
*/


/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifdef INI_IMPLEMENTATION

#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>

#define INITIAL_CAPACITY ( 256 )


#ifndef ini_malloc
	#include <malloc.h>
	#define ini_malloc( ctx, size ) ( ::malloc( size ) )
	#define ini_realloc( ctx, ptr, size ) ( ::realloc( ptr, size ) )
	#define ini_free( ctx, ptr ) ( ::free( ptr ) )
#endif



struct ini_t
	{

	struct section_t
		{
		char name[ 32 ];
		char* name_large;
		};
	section_t* sections;
	int section_capacity;
	int section_count;

	struct property_t
		{
		int section;
		char name[ 32 ];
		char* name_large;
		char value[ 64 ];
		char* value_large;
		};
	property_t* properties;
	int property_capacity;
	int property_count;

	void* memctx;
	};


static int property_index( ini_t const* const ini, int const section, int const property )
	{
	int i;
	int p;

	if( ini && section >= 0 && section < ini->section_count )
		{
		p = 0;
		for( i = 0; i < ini->property_count; ++i )
			{
			if( ini->properties[ i ].section == section )
				{
				if( p == property )
					return i;
				++p;
				}
			}
		}

	return INI_NOT_FOUND;
	}


ini_t* ini_create_memctx( void* const memctx )
	{
	ini_t* ini;

	ini = (ini_t*) ini_malloc( memctx, sizeof( ini_t ) );
	ini->memctx = memctx;
	ini->sections = (ini_t::section_t*) ini_malloc( ini->memctx, INITIAL_CAPACITY * sizeof( ini->sections[ 0 ] ) );
	ini->section_capacity = INITIAL_CAPACITY;
	ini->section_count = 1; /* global section */
	ini->sections[ 0 ].name[ 0 ] = '\0'; 
	ini->sections[ 0 ].name_large = 0;
	ini->properties = (ini_t::property_t*) ini_malloc( ini->memctx, INITIAL_CAPACITY * sizeof( ini->properties[ 0 ] ) );
	ini->property_capacity = INITIAL_CAPACITY;
	ini->property_count = 0;
	return ini;
	}


ini_t* ini_create( void )
	{
	return ini_create_memctx( (void*) 0 );
	}


ini_t* ini_load_memctx( char const* const data, void* const memctx )
	{
	ini_t* ini;
	char const* ptr;
	int s;
	char const* start;
	char const* start2;
	int l;

	ini = ini_create_memctx( memctx );

	ptr = data;
	if( ptr )
		{
		s = 0;
		while( *ptr )
			{
			/* trim leading whitespace */
			while( *ptr && *ptr <=' ' )
				++ptr;
			
			/* done? */
			if( !*ptr ) break;

			/* comment */
			else if( *ptr == ';' )
				{
				while( *ptr && *ptr !='\n' )
					++ptr;
				}
			/* section */
			else if( *ptr == '[' )
				{
				++ptr;
				start = ptr;
				while( *ptr && *ptr !=']' && *ptr != '\n' )
					++ptr;

				if( *ptr == ']' )
					{
					s = ini_section_add_l( ini, start, (int)( ptr - start) );
					++ptr;
					}
				}
			/* property */
			else
				{
				start = ptr;
				while( *ptr && *ptr !='=' && *ptr != '\n' )
					++ptr;

				if( *ptr == '=' )
					{
					l = (int)( ptr - start);
					++ptr;
					while( *ptr && *ptr <= ' ' && *ptr != '\n' ) 
						ptr++;
					start2 = ptr;
					while( *ptr && *ptr != '\n' )
						++ptr;
					while( *(--ptr) <= ' ' ) ; ptr++;
					ini_property_add_l( ini, s, start, l, start2, (int)( ptr - start2) );
					}
				}
			}
		}	

	return ini;
	}


ini_t* ini_load( char const* const data )
	{
	return ini_load_memctx( data, (void*) 0 );
	}


int ini_save( ini_t const* const ini, char* const data, int const size )
	{
	int s;
	int p;
	int i;
	int l;
	char* n;
	int pos;

	if( ini )
		{
		pos = 0;
		for( s = 0; s < ini->section_count; ++s )
			{
			n = ini->sections[ s ].name_large ? ini->sections[ s ].name_large : ini->sections[ s ].name;
			l = (int) strlen( n );
			if( l > 0 )
				{
				if( data && pos < size ) data[ pos ] = '[';
				++pos;
				for( i = 0; i < l; ++i )
					{
					if( data && pos < size ) data[ pos ] = n[ i ];
					++pos;
					}
				if( data && pos < size ) data[ pos ] = ']';
				++pos;
				if( data && pos < size ) data[ pos ] = '\n';
				++pos;
				}

			for( p = 0; p < ini->property_count; ++p )
				{
				if( ini->properties[ p ].section == s )
					{
					n = ini->properties[ p ].name_large ? ini->properties[ p ].name_large : ini->properties[ p ].name;
					l = (int) strlen( n );
					for( i = 0; i < l; ++i )
						{
						if( data && pos < size ) data[ pos ] = n[ i ];
						++pos;
						}
					if( data && pos < size ) data[ pos ] = '=';
					++pos;
					n = ini->properties[ p ].value_large ? ini->properties[ p ].value_large : ini->properties[ p ].value;
					l = (int) strlen( n );
					for( i = 0; i < l; ++i )
						{
						if( data && pos < size ) data[ pos ] = n[ i ];
						++pos;
						}
					if( data && pos < size ) data[ pos ] = '\n';
					++pos;
					}
				}

			if( pos > 0 )
				{
				if( data && pos < size ) data[ pos ] = '\n';
				++pos;
				}
			}

		if( data && pos < size ) data[ pos ] = '\0';
		++pos;

		return pos;
		}

	return 0;
	}


void ini_destroy( ini_t* const ini )
	{
	int i;

	if( ini )
		{
		for( i = 0; i < ini->property_count; ++i )
			{
			if( ini->properties[ i ].value_large ) ini_free( ini->memctx, ini->properties[ i ].value_large );
			if( ini->properties[ i ].name_large ) ini_free( ini->memctx, ini->properties[ i ].name_large );
			}
		for( i = 0; i < ini->section_count; ++i )
			if( ini->sections[ i ].name_large ) ini_free( ini->memctx, ini->sections[ i ].name_large );
		ini_free( ini->memctx, ini->properties );
		ini_free( ini->memctx, ini->sections );
		ini_free( ini->memctx, ini );
		}
	}


int ini_section_count( ini_t const* const ini )
	{
	if( ini )
		return ini->section_count;

	return 0;
	}


char const* ini_section_name( ini_t const* const ini, int const section )
	{
	if( ini && section >= 0 && section < ini->section_count )
		return ini->sections[ section ].name_large ? ini->sections[ section ].name_large : ini->sections[ section ].name;

	return 0;
	}


int ini_property_count( ini_t const* const ini, int const section )
	{
	int i;
	int count;

	if( ini )
		{
		count = 0;
		for( i = 0; i < ini->property_count; ++i )
			{
			if( ini->properties[ i ].section == section )
				++count;
			}
		return count;
		}

	return 0;
	}


char const* ini_property_name( ini_t const* const ini, int const section, int const property )
	{
	int p;

	if( ini && section >= 0 && section < ini->section_count )
		{
		p = property_index( ini, section, property );
		if( p != INI_NOT_FOUND )
			return ini->properties[ p ].name_large ? ini->properties[ p ].name_large : ini->properties[ p ].name;
		}

	return 0;
	}


char const* ini_property_value( ini_t const* const ini, int const section, int const property )
	{
	int p;

	if( ini && section >= 0 && section < ini->section_count )
		{
		p = property_index( ini, section, property );
		if( p != INI_NOT_FOUND )
			return ini->properties[ p ].value_large ? ini->properties[ p ].value_large : ini->properties[ p ].value;
		}

	return 0;
	}


int ini_find_section_l( ini_t const* const ini, char const* const name, int const name_length )
	{
	int i;

	if( ini && name )
		{
		for( i = 0; i < ini->section_count; ++i )
			{
			char const* const other = ini->sections[ i ].name_large ? ini->sections[ i ].name_large : ini->sections[ i ].name;
			if( (int) strlen( other ) == name_length && stricmp( name, other ) == 0 )
				return i;
			}
		}

	return INI_NOT_FOUND;
	}


int ini_find_section( ini_t const* const ini, char const* const name )
	{
	if( name )
		return ini_find_section_l( ini, name, (int) strlen( name ) );

	return INI_NOT_FOUND;
	}


int ini_find_property_l( ini_t const* const ini, int const section, char const* const name, int const name_length )
	{
	int i;
	int c;

	if( ini && name && section >= 0 && section < ini->section_count)
		{
		c = 0;
		for( i = 0; i < ini->property_capacity; ++i )
			{
			if( ini->properties[ i ].section == section )
				{
				char const* const other = ini->properties[ i ].name_large ? ini->properties[ i ].name_large : ini->properties[ i ].name;
				if( (int) strlen( other ) == name_length && stricmp( name, other ) == 0 )
					return c;
				++c;
				}
			}
		}

	return INI_NOT_FOUND;
	}


int ini_find_property( ini_t const* const ini, int const section, char const* const name )
	{
	if( name )
		return ini_find_property_l( ini, section, name, (int) strlen( name ) );

	return INI_NOT_FOUND;
	}


int ini_section_add_l( ini_t* const ini, char const* const name, int const length )
	{
	if( ini && name )
		{
		if( ini->section_count >= ini->section_capacity )
			{
			ini->section_capacity *= 2;
			ini->sections = (ini_t::section_t*) ini_realloc( ini->memctx, ini->sections, ini->section_capacity * sizeof( ini->sections[ 0 ] ) );
			}

		ini->sections[ ini->section_count ].name_large = 0;
		if( length + 1 >= sizeof( ini->sections[ 0 ].name ) )
			{
			ini->sections[ ini->section_count ].name_large = (char*) ini_malloc( ini->memctx, length + 1 );
			memcpy( ini->sections[ ini->section_count ].name_large, name, length );
			ini->sections[ ini->section_count ].name_large[ length ] = '\0';
			}
		else
			{
			memcpy( ini->sections[ ini->section_count ].name, name, length );
			ini->sections[ ini->section_count ].name[ length ] = '\0';
			}

		return ini->section_count++;
		}
	return INI_NOT_FOUND;
	}


void ini_property_add_l( ini_t* const ini, int const section, char const* const name, int const name_length, char const* const value, int const value_length )
	{
	if( ini && name && section >= 0 && section < ini->section_count )
		{
		if( ini->property_count >= ini->property_capacity )
			{
			ini->property_capacity *= 2;
			ini->properties = (ini_t::property_t*) ini_realloc( ini->memctx, ini->properties, ini->property_capacity * sizeof( ini->properties[ 0 ] ) );
			}
		
		ini->properties[ ini->property_count ].section = section;
		ini->properties[ ini->property_count ].name_large = 0;
		ini->properties[ ini->property_count ].value_large = 0;

		if( name_length + 1 >= sizeof( ini->properties[ 0 ].name ) )
			{
			ini->properties[ ini->property_count ].name_large = (char*) ini_malloc( ini->memctx, name_length + 1 );
			memcpy( ini->properties[ ini->property_count ].name_large, name, name_length );
			ini->properties[ ini->property_count ].name_large[ name_length ] = '\0';
			}
		else
			{
			memcpy( ini->properties[ ini->property_count ].name, name, name_length );
			ini->properties[ ini->property_count ].name[ name_length ] = '\0';
			}

		if( value_length + 1 >= sizeof( ini->properties[ 0 ].value ) )
			{
			ini->properties[ ini->property_count ].value_large = (char*) ini_malloc( ini->memctx, value_length + 1 );
			memcpy( ini->properties[ ini->property_count ].value_large, value, value_length );
			ini->properties[ ini->property_count ].value_large[ value_length ] = '\0';
			}
		else
			{
			memcpy( ini->properties[ ini->property_count ].value, value, value_length );
			ini->properties[ ini->property_count ].value[ value_length ] = '\0';
			}

		++ini->property_count;
		}
	}


int ini_section_add( ini_t* const ini, char const* const name )
	{
	if( name )
		return ini_section_add_l( ini, name, (int) strlen( name ) );
	
	return INI_NOT_FOUND;
	}


void ini_property_add( ini_t* const ini, int const section, char const* const name, char const* const value )
	{
	if( name && value )
		ini_property_add_l( ini, section, name, (int) strlen( name ), value, (int) strlen( value ) );
	}


void ini_section_remove( ini_t* const ini, int const section )
	{
	if( ini && section >= 0 && section < ini->section_count )
		{
		if( ini->sections[ section ].name_large ) ini_free( ini->memctx, ini->sections[ section ].name_large );
		ini->sections[ section ] = ini->sections[ --ini->section_count  ];
		}
	}


void ini_property_remove( ini_t* const ini, int const section, int const property )
	{
	int p;

	if( ini && section >= 0 && section < ini->section_count )
		{
		p = property_index( ini, section, property );
		if( p != INI_NOT_FOUND )
			{
			if( ini->properties[ p ].value_large ) ini_free( ini->memctx, ini->properties[ p ].value_large );
			if( ini->properties[ p ].name_large ) ini_free( ini->memctx, ini->properties[ p ].name_large );
			ini->properties[ p ] = ini->properties[ --ini->property_count  ];
			return;
			}
		}
	}


void ini_section_name_set_l( ini_t* const ini, int const section, char const* const name, int const length )
	{
	if( ini && name && section >= 0 && section < ini->section_count )
		{
		if( ini->sections[ section ].name_large ) ini_free( ini->memctx, ini->sections[ section ].name_large );
		ini->sections[ section ].name_large = 0;
		
		if( length + 1 >= sizeof( ini->sections[ 0 ].name ) )
			{
			ini->sections[ section ].name_large = (char*) ini_malloc( ini->memctx, length + 1 );
			memcpy( ini->sections[ section ].name_large, name, length );
			ini->sections[ section ].name_large[ length ] = '\0';
			}
		else
			{
			memcpy( ini->sections[ section ].name, name, length );
			ini->sections[ section ].name[ length ] = '\0';
			}
		}
	}


void ini_property_name_set_l( ini_t* const ini, int const section, int const property, char const* const name, int const length )
	{
	int p;

	if( ini && name && section >= 0 && section < ini->section_count )
		{
		p = property_index( ini, section, property );
		if( p != INI_NOT_FOUND )
			{
			if( ini->properties[ p ].name_large ) ini_free( ini->memctx, ini->properties[ p ].name_large );
			ini->properties[ ini->property_count ].name_large = 0;

			if( length + 1 >= sizeof( ini->properties[ 0 ].name ) )
				{
				ini->properties[ p ].name_large = (char*) ini_malloc( ini->memctx, length + 1 );
				memcpy( ini->properties[ p ].name_large, name, length );
				ini->properties[ p ].name_large[ length ] = '\0';
				}
			else
				{
				memcpy( ini->properties[ p ].name, name, length );
				ini->properties[ p ].name[ length ] = '\0';
				}
			}
		}
	}


void ini_property_value_set_l( ini_t* const ini, int const section, int const property, char const* const value, int const length )
	{
	int p;

	if( ini && value && section >= 0 && section < ini->section_count )
		{
		p = property_index( ini, section, property );
		if( p != INI_NOT_FOUND )
			{
			if( ini->properties[ p ].value_large ) ini_free( ini->memctx, ini->properties[ p ].value_large );
			ini->properties[ ini->property_count ].value_large = 0;

			if( length + 1 >= sizeof( ini->properties[ 0 ].value ) )
				{
				ini->properties[ p ].value_large = (char*) ini_malloc( ini->memctx, length + 1 );
				memcpy( ini->properties[ p ].name_large, value, length );
				ini->properties[ p ].value_large[ length ] = '\0';
				}
			else
				{
				memcpy( ini->properties[ p ].value, value, length );
				ini->properties[ p ].name[ length ] = '\0';
				}
			}
		}
	}


void ini_property_name_set( ini_t* const ini, int const section, int const property, char const* const name )
	{
	if( name )
		return ini_property_name_set_l( ini, section, property, name, (int) strlen( name ) );
	
	return;
	}


void ini_property_value_set( ini_t* const ini, int const section, int const property, char const* const value )
	{
	if( value )
		return ini_property_value_set_l( ini, section, property, value, (int) strlen( value ) );
	
	return;
	}

#endif /* INI_IMPLEMENTATION */


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
