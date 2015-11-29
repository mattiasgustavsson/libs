/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

strpool.h - v1.3 - Highly efficient string pool for C/C++.

Do this:
	#define STRPOOL_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef strpool_h
#define strpool_h

#ifndef STRPOOL_U32
	#define STRPOOL_U32 unsigned int
#endif
#ifndef STRPOOL_U64
	#define STRPOOL_U64 unsigned long long
#endif

typedef struct strpool_t strpool_t;

typedef struct strpool_config_t
	{
	void* memctx;
	int ignore_case;
	int counter_bits;
	int index_bits;
	int entry_capacity;
	int block_capacity;
	int block_size;
	int min_data_size;
	} strpool_config_t;

extern strpool_config_t const strpool_default_config;

void strpool_init( strpool_t* pool, strpool_config_t const* config );
void strpool_term( strpool_t* pool );

void strpool_defrag( strpool_t* pool );

STRPOOL_U64 strpool_inject( strpool_t* pool, char const* string, int length );
void strpool_discard( strpool_t* pool, STRPOOL_U64 handle );

int strpool_incref( strpool_t* pool, STRPOOL_U64 handle );
int strpool_decref( strpool_t* pool, STRPOOL_U64 handle );
int strpool_getref( strpool_t* pool, STRPOOL_U64 handle );

int strpool_isvalid( strpool_t const* pool, STRPOOL_U64 handle );

char const* strpool_cstr( strpool_t const* pool, STRPOOL_U64 handle );
int strpool_length( strpool_t const* pool, STRPOOL_U64 handle );

char* strpool_collate( strpool_t const* pool, int* count );
	

#endif /* strpool_h */


/*

Examples:

------------------------------------------------------------------------------

	#define  STRPOOL_IMPLEMENTATION
	#include "strpool.h"

	#include <stdio.h>

	int main()
		{
		strpool_config_t conf = strpool_default_config;
		//conf.ignore_case = true;

		strpool_t pool;
		strpool_init( &pool, &conf );

		STRPOOL_U64 str_a = strpool_inject( &pool, "This is a test string", strlen( "This is a test string" ) );
		STRPOOL_U64 str_b = strpool_inject( &pool, "THIS IS A TEST STRING", strlen( "THIS IS A TEST STRING" ) );
	
		printf( "%s\n", strpool_cstr( &pool, str_a ) );
		printf( "%s\n", strpool_cstr( &pool, str_b ) );
		printf( "%s\n", str_a == str_b ? "Strings are the same" : "Strings are different" );
	
		strpool_term( &pool );
		return 0;
		}
*/


/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifndef strpool_impl
#define strpool_impl

typedef struct strpool_hash_slot_t
	{
	STRPOOL_U32 hask_key;
	int entry_index;
	int base_count;
	} strpool_hash_slot_t;


typedef struct strpool_entry_t
	{
	int hash_slot;
	int handle_index;
	char* data;
	int size;
	int length;
	int refcount;
	} strpool_entry_t;


typedef struct strpool_handle_t
	{
	int entry_index;
	int counter;
	} strpool_handle_t;


typedef struct strpool_block_t
	{
	int capacity;
	char* data;
	char* tail;
	int free_list;
	} strpool_block_t;


typedef struct strpool_free_block_t
	{
	int size;
	int next;
	} strpool_free_block_t;


struct strpool_t
	{
	void* memctx;
	int ignore_case;
	int counter_shift;
	STRPOOL_U64 counter_mask;
	STRPOOL_U64 index_mask;

	int initial_entry_capacity;
	int initial_block_capacity;
	int block_size;
	int min_data_size;

	strpool_hash_slot_t* hash_table;
	int hash_capacity;

	strpool_entry_t* entries;
	int entry_capacity;
	int entry_count;

	strpool_handle_t* handles;
	int handle_capacity;
	int handle_count;
	int handle_freelist_head;
	int handle_freelist_tail;

	strpool_block_t* blocks;
	int block_capacity;
	int block_count;
	int current_block;
	};

#endif /* strpool_impl */


#ifdef STRPOOL_IMPLEMENTATION


#ifndef STRPOOL_ASSERT
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <assert.h>
	#define STRPOOL_ASSERT( x ) assert( x )
#endif

#ifndef STRPOOL_MEMSET
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define STRPOOL_MEMSET( ptr, val, cnt ) ( memset( ptr, val, cnt ) )
#endif 

#ifndef STRPOOL_MEMCPY
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define STRPOOL_MEMCPY( dst, src, cnt ) ( memcpy( dst, src, cnt ) )
#endif 

#ifndef STRPOOL_MEMCMP
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define STRPOOL_MEMCMP( pr1, pr2, cnt ) ( memcmp( pr1, pr2, cnt ) )
#endif 

#ifndef STRPOOL_STRNICMP
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define STRPOOL_STRNICMP( s1, s2, len ) ( strnicmp( s1, s2, len ) )
#endif 

#ifndef STRPOOL_MALLOC
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <stdlib.h>
	#define STRPOOL_MALLOC( ctx, size ) ( malloc( size ) )
	#define STRPOOL_REALLOC( ctx, ptr, size ) ( realloc( (ptr), (size) ) )
	#define STRPOOL_FREE( ctx, ptr ) ( free( ptr ) )
#endif


strpool_config_t const strpool_default_config = 
	{ 
	/* memctx	      = */ 0,
	/* ignore_case    = */ 0,
	/* counter_bits   = */ 32,
	/* index_bits     = */ 32,
	/* entry_capacity = */ 4096, 
	/* block_capacity = */ 32, 
	/* block_size     = */ 256 * 1024, 
	/* min_data_size  = */ 16,
	};



STRPOOL_U64 strpool_inject( strpool_t* pool, char const* string, int length );
int strpool_incref( strpool_t* pool, STRPOOL_U64 handle );


static STRPOOL_U32 strpool_pow2ceil( STRPOOL_U32 v )
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


static int add_block( strpool_t* pool, int size )
	{
	if( pool->block_count >= pool->block_capacity )	
		{
		pool->block_capacity *= 2;
		pool->blocks = (strpool_block_t*) STRPOOL_REALLOC( pool->memctx, pool->blocks, pool->block_capacity * sizeof( *pool->blocks ) );
		STRPOOL_ASSERT( pool->blocks );
		}
	pool->blocks[ pool->block_count ].capacity = size;
	pool->blocks[ pool->block_count ].data = (char*) STRPOOL_MALLOC( pool->memctx, size );
	STRPOOL_ASSERT( pool->blocks[ pool->block_count ].data );
	pool->blocks[ pool->block_count ].tail = pool->blocks[ pool->block_count ].data;
	pool->blocks[ pool->block_count ].free_list = -1;
	return pool->block_count++;
	}


void strpool_init( strpool_t* pool, strpool_config_t const* config )
	{
	if( !config ) config = &strpool_default_config;

	pool->memctx = config->memctx;
	pool->ignore_case = config->ignore_case;

	STRPOOL_ASSERT( config->counter_bits + config->index_bits <= 64 );
	pool->counter_shift = config->index_bits;
	pool->counter_mask = ( 1ULL << (STRPOOL_U64) config->counter_bits ) - 1;
	pool->index_mask = ( 1ULL << (STRPOOL_U64) config->index_bits ) - 1;

	pool->initial_entry_capacity = strpool_pow2ceil( config->entry_capacity > 1 ? config->entry_capacity : 2 );
	pool->initial_block_capacity = strpool_pow2ceil( config->block_capacity > 1 ? config->block_capacity : 2 );
	pool->block_size = strpool_pow2ceil( config->block_size > 256 ? config->block_size : 256 );
	pool->min_data_size = strpool_pow2ceil( config->min_data_size > 8 ? config->min_data_size : 8 );

	pool->hash_capacity = pool->initial_entry_capacity * 2;
	pool->entry_capacity = pool->initial_entry_capacity;
	pool->handle_capacity = pool->initial_entry_capacity;
	pool->block_capacity = pool->initial_block_capacity;	

	pool->handle_freelist_head = -1;
	pool->handle_freelist_tail = -1;
	pool->block_count = 0;
	pool->handle_count = 0;
	pool->entry_count = 0;
	
	pool->hash_table = (strpool_hash_slot_t*) STRPOOL_MALLOC( pool->memctx, pool->hash_capacity * sizeof( *pool->hash_table ) );
	STRPOOL_ASSERT( pool->hash_table );
	STRPOOL_MEMSET( pool->hash_table, 0, pool->hash_capacity * sizeof( *pool->hash_table ) );
	pool->entries = (strpool_entry_t*) STRPOOL_MALLOC( pool->memctx, pool->entry_capacity * sizeof( *pool->entries ) );
	STRPOOL_ASSERT( pool->entries );
	pool->handles = (strpool_handle_t*) STRPOOL_MALLOC( pool->memctx, pool->handle_capacity * sizeof( *pool->handles ) );
	STRPOOL_ASSERT( pool->handles );
	pool->blocks = (strpool_block_t*) STRPOOL_MALLOC( pool->memctx, pool->block_capacity * sizeof( *pool->blocks ) );
	STRPOOL_ASSERT( pool->blocks );

	pool->current_block = add_block( pool, pool->block_size );
	}


void strpool_term( strpool_t* pool )
	{
#if 0
	// Debug statistics
	printf( "\n\n" );
	printf( "Handles: %d/%d\n", pool->handle_count, pool->handle_capacity );
	printf( "Entries: %d/%d\n", pool->entry_count, pool->entry_capacity );
	printf( "Hashtable: %d/%d\n", pool->entry_count, pool->hash_capacity );
	printf( "Blocks: %d/%d\n", pool->block_count, pool->block_capacity );
	for( int i = 0; i < pool->block_count; ++i )
		{
		printf( "\n" );
		printf( "BLOCK: %d\n", i );
		printf( "Capacity: %d\n", pool->blocks[ i ].capacity );
		printf( "Free: [ %d ]", pool->blocks[ i ].capacity - ( pool->blocks[ i ].tail - pool->blocks[ i ].data ) );
		int fl = pool->blocks[ i ].free_list;
		int count = 0;
		int size = 0;
		int total = 0;
		while( fl >= 0 )
			{
			strpool_free_block_t* free_entry = (strpool_free_block_t*) ( pool->blocks[ i ].data + fl );
			total += free_entry->size;
			if( size == 0 ) { size = free_entry->size; }
			if( size != free_entry->size )
				{
				printf( ", %dx%d", count, size );
				count = 1;
				size = free_entry->size;
				}
			else
				{
				++count;
				}
			fl = free_entry->next;
			}
		if( size != 0 ) printf( ", %dx%d", count, size );
		printf( ", { %d }\n", total );
		}
	printf( "\n\n" );
#endif

	for( int i = 0; i < pool->block_count; ++i )
		{
		STRPOOL_FREE( pool->memctx, pool->blocks[ i ].data );
		}
	STRPOOL_FREE( pool->memctx, pool->blocks );			
	STRPOOL_FREE( pool->memctx, pool->handles );			
	STRPOOL_FREE( pool->memctx, pool->entries );			
	STRPOOL_FREE( pool->memctx, pool->hash_table );			
	}


void strpool_defrag( strpool_t* pool )
	{
	int data_size = 0;
	int count = 0;
	for( int i = 0; i < pool->entry_count; ++i )
		{
		strpool_entry_t* entry = &pool->entries[ i ];
		if( entry->refcount > 0 )
			{
			data_size += entry->size;
			++count;
			}
		}

	int data_capacity = data_size < pool->block_size ? pool->block_size : strpool_pow2ceil( data_size );

	int hash_capacity = count + count / 2;
	hash_capacity = hash_capacity < ( pool->initial_entry_capacity * 2 ) ? ( pool->initial_entry_capacity * 2 ) : strpool_pow2ceil( hash_capacity );
	strpool_hash_slot_t* hash_table = (strpool_hash_slot_t*) STRPOOL_MALLOC( pool->memctx, hash_capacity * sizeof( *hash_table ) );
	STRPOOL_ASSERT( hash_table );
	STRPOOL_MEMSET( hash_table, 0, hash_capacity * sizeof( *hash_table ) );

	char* data = (char*) STRPOOL_MALLOC( pool->memctx, data_capacity );
	STRPOOL_ASSERT( data );
	int capacity = count < pool->initial_entry_capacity ? pool->initial_entry_capacity : strpool_pow2ceil( count );
	strpool_entry_t* entries = (strpool_entry_t*) STRPOOL_MALLOC( pool->memctx, capacity * sizeof( *entries ) );
	STRPOOL_ASSERT( entries );
	int index = 0;
	char* tail = data;
	for( int i = 0; i < pool->entry_count; ++i )
		{
		strpool_entry_t* entry = &pool->entries[ i ];
		if( entry->refcount > 0 )
			{
			entries[ index ] = *entry;

			STRPOOL_U32 hash = pool->hash_table[ entry->hash_slot ].hask_key;
			int base_slot = hash & ( hash_capacity - 1 );
			int slot = base_slot;
			while( hash_table[ slot ].hask_key )
				{
				slot = (slot + 1 ) & ( hash_capacity - 1 );
				}
			STRPOOL_ASSERT( hash );
			hash_table[ slot ].hask_key = hash;
			hash_table[ slot ].entry_index = index;
			++hash_table[ base_slot ].base_count;

			entries[ index ].hash_slot = slot;
			entries[ index ].data = tail;
			entries[ index ].handle_index = entry->handle_index;
			pool->handles[ entry->handle_index ].entry_index = index;
			STRPOOL_MEMCPY( tail, entry->data, entries->length + 1 + sizeof( STRPOOL_U32 ) );
			tail += entry->size;
			++index;
			}
		}


	STRPOOL_FREE( pool->memctx, pool->hash_table );
	STRPOOL_FREE( pool->memctx, pool->entries );
	for( int i = 0; i < pool->block_count; ++i )
		{
		STRPOOL_FREE( pool->memctx, pool->blocks[ i ].data );
		}

	if( pool->block_capacity != pool->initial_block_capacity )
		{
		STRPOOL_FREE( pool->memctx, pool->blocks );
		pool->blocks = (strpool_block_t*) STRPOOL_MALLOC( pool->memctx, pool->initial_block_capacity * sizeof( *pool->blocks ) );
		STRPOOL_ASSERT( pool->blocks );
		}
	pool->block_capacity = pool->initial_block_capacity;
	pool->block_count = 1;
	pool->current_block = 0;
	pool->blocks[ 0 ].capacity = data_capacity;
	pool->blocks[ 0 ].data = data;
	pool->blocks[ 0 ].tail = tail;
	pool->blocks[ 0 ].free_list = -1;
	
	pool->hash_table = hash_table;
	pool->hash_capacity = hash_capacity;

	pool->entries = entries;
	pool->entry_capacity = capacity;
	pool->entry_count = count;
	}


static STRPOOL_U64 make_handle( int index, int counter, STRPOOL_U64 index_mask, int counter_shift, STRPOOL_U64 counter_mask )
	{
	STRPOOL_U64 i = (STRPOOL_U64) ( index + 1 );
	STRPOOL_U64 c = (STRPOOL_U64) counter;
	return ( ( c & counter_mask ) << counter_shift ) | ( i & index_mask );
	}


static int counter_from_handle( STRPOOL_U64 handle, int counter_shift, STRPOOL_U64 counter_mask  )
	{
	return (int) ( ( handle >> counter_shift ) & counter_mask ) ;
	}
	

static int index_from_handle( STRPOOL_U64 handle, STRPOOL_U64 index_mask )
	{
	return ( (int) ( handle & index_mask ) ) - 1;
	}


static strpool_entry_t* get_entry( strpool_t const* pool, STRPOOL_U64 handle )
	{
	int index = index_from_handle( handle, pool->index_mask );
	int counter = counter_from_handle( handle, pool->counter_shift, pool->counter_mask );

	if( index >= 0 && index < pool->handle_count && counter == (int) ( pool->handles[ index ].counter & pool->counter_mask ) )
		{
		return &pool->entries[ pool->handles[ index ].entry_index ];
		}

	return 0;
	}


static STRPOOL_U32 find_in_blocks( strpool_t const* pool, char const* string )
	{
	for( int i = 0; i < pool->block_count; ++i )
		{
		strpool_block_t* block = &pool->blocks[ i ];
		if( string >= block->data && string < block->data + block->capacity ) // Check if string comes from pool
			{
			return *( (STRPOOL_U32*)( string - sizeof( STRPOOL_U32 ) ) ); // Hash is stored before it
			}
		}

	return 0;
	}


static STRPOOL_U32 calculate_hash( char const* string, int length, int ignore_case )
	{
	STRPOOL_U32 hash = 5381U; 

	if( ignore_case) 
		{
		for( int i = 0; i < length; ++i )
			{
			char c = string[ i ];
			c = ( c <= 'z' && c >= 'a' ) ? c - ( 'a' - 'A' ) : c;
			hash = ( ( hash << 5U ) + hash) ^ c;
			}
		}
	else
		{
		for( int i = 0; i < length; ++i )
			{
			char c = string[ i ];
			hash = ( ( hash << 5U ) + hash) ^ c;
			}
		}

	hash = ( hash == 0 ) ? 1 : hash; // We can't allow 0-value hash keys, but dupes are ok
	return hash;
	}


static void expand_hash( strpool_t* pool )
	{
	int old_capacity = pool->hash_capacity;
	strpool_hash_slot_t* old_table = pool->hash_table;

	pool->hash_capacity *= 2;

	pool->hash_table = (strpool_hash_slot_t*) STRPOOL_MALLOC( pool->memctx, pool->hash_capacity * sizeof( *pool->hash_table ) );
	STRPOOL_ASSERT( pool->hash_table );
	STRPOOL_MEMSET( pool->hash_table, 0, pool->hash_capacity * sizeof( *pool->hash_table ) );

	for( int i = 0; i < old_capacity; ++i )
		{
		STRPOOL_U32 hash_key = old_table[ i ].hask_key;
		if( hash_key )
			{
			int base_slot = hash_key & ( pool->hash_capacity - 1 );
			int slot = base_slot;
			while( pool->hash_table[ slot ].hask_key )
				{
				slot = ( slot + 1 ) & ( pool->hash_capacity - 1 );
				}		
			STRPOOL_ASSERT( hash_key );
			pool->hash_table[ slot ].hask_key = hash_key;
			pool->hash_table[ slot ].entry_index = old_table[ i ].entry_index;	
			pool->entries[ pool->hash_table[ slot ].entry_index ].hash_slot = slot;	
			++pool->hash_table[ base_slot ].base_count;
			}				
		}

	STRPOOL_FREE( pool->memctx, old_table );
	}


static void expand_entries( strpool_t* pool )
	{
	pool->entry_capacity *= 2;
	pool->entries = (strpool_entry_t*) STRPOOL_REALLOC( pool->memctx, pool->entries, pool->entry_capacity * sizeof( *pool->entries ) );
	STRPOOL_ASSERT( pool->entries );
	}


static void expand_handles( strpool_t* pool )
	{
	pool->handle_capacity *= 2;
	pool->handles = (strpool_handle_t*) STRPOOL_REALLOC( pool->memctx, pool->handles, pool->handle_capacity * sizeof( *pool->handles ) );
	STRPOOL_ASSERT( pool->handles );
	}


static char* get_data_storage( strpool_t* pool, int size, int* alloc_size )
	{
	if( size < sizeof( strpool_free_block_t ) ) size = sizeof( strpool_free_block_t );
	if( size < pool->min_data_size ) size = pool->min_data_size;
	size = strpool_pow2ceil( size );
	
	// Try to find a large enough free slot in existing blocks
	for( int i = 0; i < pool->block_count; ++i )
		{
		int free_list = pool->blocks[ i ].free_list;
		int prev_list = -1;
		while( free_list >= 0 )
			{
			strpool_free_block_t* free_entry = (strpool_free_block_t*) ( pool->blocks[ i ].data + free_list );
			if( free_entry->size / 2 < size ) 
				{
				// At this point, all remaining slots are too small, so bail out if the current slot is not large enough
				if( free_entry->size < size ) break; 

				if( prev_list < 0 )
					{
					pool->blocks[ i ].free_list = free_entry->next;			
					}
				else
					{
					strpool_free_block_t* prev_entry = (strpool_free_block_t*) ( pool->blocks[ i ].data + prev_list );
					prev_entry->next = free_entry->next;
					}
				*alloc_size = free_entry->size;
				return (char*) free_entry;
				}
			prev_list = free_list;
			free_list = free_entry->next;
			}
		}

	// Use current block, if enough space left
	int offset = (int) ( pool->blocks[ pool->current_block ].tail - pool->blocks[ pool->current_block ].data );
	if( size <= pool->blocks[ pool->current_block ].capacity - offset )
		{
		char* data = pool->blocks[ pool->current_block ].tail;
		pool->blocks[ pool->current_block ].tail += size;
		*alloc_size = size;
		return data;
		}

	// Allocate a new block
	pool->current_block = add_block( pool, size > pool->block_size ? size : pool->block_size );
	char* data = pool->blocks[ pool->current_block ].tail;
	pool->blocks[ pool->current_block ].tail += size;
	*alloc_size = size;
	return data;
	}
	

STRPOOL_U64 strpool_inject( strpool_t* pool, char const* string, int length )
	{
	if( !string || length < 0 )
		{
		return 0;
		}

	STRPOOL_U32 hash = find_in_blocks( pool, string );
	if( !hash )
		{
		hash = calculate_hash( string, length, pool->ignore_case ); // No stored hash, so calculate it from data
		}

	// Return handle to existing string, if it is already in pool
	int base_slot = hash & ( pool->hash_capacity - 1 );
	int base_count = pool->hash_table[ base_slot ].base_count;
	int slot = base_slot;
	int first_free = slot;
	while( base_count > 0 )
		{
		STRPOOL_U32 slot_hash = pool->hash_table[ slot ].hask_key;
		if( slot_hash == 0 && pool->hash_table[ first_free ].hask_key != 0 ) first_free = slot;
		int slot_base = ( slot_hash & ( pool->hash_capacity - 1 ) );
		if( slot_base == base_slot ) 
			{
			STRPOOL_ASSERT( base_count > 0 );
			--base_count;
			if( slot_hash == hash )
				{
				int index = pool->hash_table[ slot ].entry_index;
				strpool_entry_t* entry = &pool->entries[ index ];
				if( entry->length == length && 
					( 
					   ( !pool->ignore_case &&   STRPOOL_MEMCMP( entry->data + sizeof( STRPOOL_U32 ), string, length ) == 0 )
					|| (  pool->ignore_case && STRPOOL_STRNICMP( entry->data + sizeof( STRPOOL_U32 ), string, length ) == 0 ) 
					) 
				  )
					{
					int handle_index = entry->handle_index;
					return make_handle( handle_index, pool->handles[ handle_index ].counter, pool->index_mask, pool->counter_shift, pool->counter_mask );
					}
				}
			}
		slot = ( slot + 1 ) & ( pool->hash_capacity - 1 );
		}	

	// This is a new string, so let's add it

	if( pool->entry_count >= ( pool->hash_capacity  - pool->hash_capacity / 3 ) )
		{
		expand_hash( pool );

		base_slot = hash & ( pool->hash_capacity - 1 );
		slot = base_slot;
		first_free = slot;
		while( base_count )
			{
			STRPOOL_U32 slot_hash = pool->hash_table[ slot ].hask_key;
			if( slot_hash == 0 && pool->hash_table[ first_free ].hask_key != 0 ) first_free = slot;
			int slot_base = ( slot_hash & ( pool->hash_capacity - 1 ) );
			if( slot_base == base_slot ) 
				{
				--base_count;
				}
			slot = ( slot + 1 ) & ( pool->hash_capacity - 1 );
			}		
		}
		
	slot = first_free;
	while( pool->hash_table[ slot ].hask_key )
		{
		slot = ( slot + 1 ) & ( pool->hash_capacity - 1 );
		}

	if( pool->entry_count >= pool->entry_capacity )
		{
		expand_entries( pool );
		}

	STRPOOL_ASSERT( !pool->hash_table[ slot ].hask_key && ( hash & ( (STRPOOL_U32) pool->hash_capacity - 1 ) ) == (STRPOOL_U32) base_slot );
	STRPOOL_ASSERT( hash );
	pool->hash_table[ slot ].hask_key = hash;
	pool->hash_table[ slot ].entry_index = pool->entry_count;
	++pool->hash_table[ base_slot ].base_count;

	int handle_index;

	if( pool->handle_count < pool->handle_capacity )
		{
		handle_index = pool->handle_count;
		pool->handles[ pool->handle_count ].counter = 1;
		++pool->handle_count;			
		}
	else if( pool->handle_freelist_head >= 0 )
		{
		handle_index = pool->handle_freelist_head;
		if( pool->handle_freelist_tail == pool->handle_freelist_head ) pool->handle_freelist_tail = pool->handles[ pool->handle_freelist_head ].entry_index;
		pool->handle_freelist_head = pool->handles[ pool->handle_freelist_head ].entry_index;						
		}
	else
		{
		expand_handles( pool );
		handle_index = pool->handle_count;
		pool->handles[ pool->handle_count ].counter = 1;
		++pool->handle_count;			
		}

	pool->handles[ handle_index ].entry_index = pool->entry_count;
		
	strpool_entry_t& entry = pool->entries[ pool->entry_count ];
	++pool->entry_count;
		
	int data_size = length + 1 + (int) sizeof( STRPOOL_U32 );
	char* data = get_data_storage( pool, data_size, &data_size );
	entry.hash_slot = slot;
	entry.handle_index = handle_index;
	entry.data = data;
	entry.size = data_size;
	entry.length = length;
	entry.refcount = 0;

	*(STRPOOL_U32*)(data) = hash;
	data += sizeof( STRPOOL_U32 );
	STRPOOL_MEMCPY( data, string, length ); 
	data[ length ] = 0; // Ensure trailing zero

	return make_handle( handle_index, pool->handles[ handle_index ].counter, pool->index_mask, pool->counter_shift, pool->counter_mask );
	}


void strpool_discard( strpool_t* pool, STRPOOL_U64 handle )
	{	
	strpool_entry_t* entry = get_entry( pool, handle );
	if( entry && entry->refcount == 0 )
		{
		int entry_index = pool->handles[ entry->handle_index ].entry_index;

		// recycle string mem
		for( int i = 0; i < pool->block_count; ++i )
			{
			strpool_block_t* block = &pool->blocks[ i ];
			if( entry->data >= block->data && entry->data <= block->tail )
				{
				if( block->free_list < 0 )
					{
					strpool_free_block_t* new_entry = (strpool_free_block_t*) ( entry->data );
					block->free_list = (int) ( entry->data - block->data );
					new_entry->next = -1;
					new_entry->size = entry->size;
					}
				else
					{
					int free_list = block->free_list;
					int prev_list = -1;
					while( free_list >= 0 )
						{
						strpool_free_block_t* free_entry = (strpool_free_block_t*) ( pool->blocks[ i ].data + free_list );
						if( free_entry->size <= entry->size ) 
							{
							strpool_free_block_t* new_entry = (strpool_free_block_t*) ( entry->data );
							if( prev_list < 0 )
								{
								new_entry->next = pool->blocks[ i ].free_list;
								pool->blocks[ i ].free_list = (int) ( entry->data - block->data );			
								}
							else
								{
								strpool_free_block_t* prev_entry = (strpool_free_block_t*) ( pool->blocks[ i ].data + prev_list );
								prev_entry->next = (int) ( entry->data - block->data );
								new_entry->next = free_entry->next;
								}
							new_entry->size = entry->size;
							break;
							}
						prev_list = free_list;
						free_list = free_entry->next;
						}
					}
				break;
				}
			}

		// recycle handle
		if( pool->handle_freelist_tail < 0 )
			{
			STRPOOL_ASSERT( pool->handle_freelist_head < 0 );
			pool->handle_freelist_head = entry->handle_index;
			pool->handle_freelist_tail = entry->handle_index;
			}
		else
			{
			pool->handles[ pool->handle_freelist_tail ].entry_index = entry->handle_index;
			pool->handle_freelist_tail = entry->handle_index;
			}
		++pool->handles[ entry->handle_index ].counter; // invalidate handle via counter
		pool->handles[ entry->handle_index ].entry_index = -1;

		// recycle hash slot
		STRPOOL_U32 hash = pool->hash_table[ entry->hash_slot ].hask_key;
		int base_slot = hash & ( pool->hash_capacity - 1 );
		STRPOOL_ASSERT( hash );
		--pool->hash_table[ base_slot ].base_count;
		pool->hash_table[ entry->hash_slot ].hask_key = 0;

		// recycle entry
		if( entry_index != pool->entry_count - 1 )
			{
			pool->entries[ entry_index ] = pool->entries[ pool->entry_count - 1 ];
			pool->hash_table[ pool->entries[ entry_index ].hash_slot ].entry_index = entry_index;
			pool->handles[ pool->entries[ entry_index ].handle_index ].entry_index = entry_index;
			}
		--pool->entry_count;

		}		

	}


int strpool_incref( strpool_t* pool, STRPOOL_U64 handle )
	{
	strpool_entry_t* entry = get_entry( pool, handle );
	if( entry )
		{
		++entry->refcount;
		return entry->refcount;
		}
	return 0;
	}


int strpool_decref( strpool_t* pool, STRPOOL_U64 handle )
	{
	strpool_entry_t* entry = get_entry( pool, handle );
	if( entry )
		{
		STRPOOL_ASSERT( entry->refcount > 0 );
		--entry->refcount;
		return entry->refcount;
		}
	return 0;
	}


int strpool_getref( strpool_t* pool, STRPOOL_U64 handle )
	{
	strpool_entry_t* entry = get_entry( pool, handle );
	if( entry )
		{
		return entry->refcount;
		}
	return 0;
	}


int strpool_isvalid( strpool_t const* pool, STRPOOL_U64 handle )
	{
	strpool_entry_t const* entry = get_entry( pool, handle );
	if( entry )
		{
		return 1;
		}

	return 0;
	}


char const* strpool_cstr( strpool_t const* pool, STRPOOL_U64 handle )
	{
	strpool_entry_t const* entry = get_entry( pool, handle );
	if( entry )
		{
		return entry->data + sizeof( STRPOOL_U32 ); // Skip leading hash value
		}

	return 0;
	}


int strpool_length( strpool_t const* pool, STRPOOL_U64 handle )
	{
	strpool_entry_t const* entry = get_entry( pool, handle );
	if( entry )
		{
		return entry->length;
		}

	return 0;
	}


char* strpool_collate( strpool_t const* pool, int* count )
	{
	int size = 0;
	for( int i = 0; i < pool->entry_count; ++i )
		{
		size += pool->entries[ i ].length + 1;
		}

	char* strings = (char*) STRPOOL_MALLOC( pool->memctx, size );
	STRPOOL_ASSERT( strings );
	*count = pool->entry_count;
	char* ptr = strings;
	for( int i = 0; i < pool->entry_count; ++i )
		{
		int len = pool->entries[ i ].length + 1;
		char* src = pool->entries[ i ].data += sizeof( STRPOOL_U32 );
		STRPOOL_MEMCPY( ptr, src, len );
		ptr += len;
		}
	return strings;
	}


#endif /* STRPOOL_IMPLEMENTATION */


/*
revision history:
	1.3		fixed typo in mask bit shift
	1.2		made it possible to override standard library functions
	1.1		added is_valid function to query a handles validity
	1.0		first released version	
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
