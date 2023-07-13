/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

cstr.h - v1.0 - String interning and manipulation library for C/C++.

Do this:
    #define CSTR_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef cstr_h
#define cstr_h

#ifndef CSTR_U32
    #define CSTR_U32 unsigned int
#endif

#ifndef CSTR_SIZE_T
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <stddef.h>
    #define CSTR_SIZE_T size_t
#endif

#ifndef CSTR_BOOL_T
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdbool.h>
    #define CSTR_BOOL_T bool
#endif


#ifndef CSTR_VA_LIST_T
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdarg.h>
    #define CSTR_VA_LIST_T va_list
#endif


#ifndef CSTR_NO_GLOBAL_API

void cstr_reset( void );

struct cstr_restore_point_t;
struct cstr_restore_point_t* cstr_restore_point( void );
void cstr_rollback( struct cstr_restore_point_t* restore_point );

char const* cstr( char const* str );
char const* cstr_n( char const* str, CSTR_SIZE_T n );

CSTR_BOOL_T cstr_is_interned( char const* str );

CSTR_SIZE_T cstr_len( char const* str );

char const* cstr_cat( char const* a, char const* b );

char const* cstr_vformat( char const* format, CSTR_VA_LIST_T args );
char const* cstr_format( char const* format, ... );

char const* cstr_trim( char const* str );
char const* cstr_ltrim( char const* str );
char const* cstr_rtrim( char const* str );

char const* cstr_left( char const* str, size_t n );
char const* cstr_right( char const* str, size_t n );
char const* cstr_mid( char const* str, size_t start, size_t n );

char const* cstr_upper( char const* str );
char const* cstr_lower( char const* str );

char const* cstr_lpad( char const* str, char padding, CSTR_SIZE_T total_max_length );
char const* cstr_rpad( char const* str, char padding, CSTR_SIZE_T total_max_length );

char const* cstr_join( char const* a, char const* b, char const* separator );

char const* cstr_replace( char const* str, char const* find, char const* replacement );

char const* cstr_insert( char const* str, int position, char const* insertion );
char const* cstr_remove( char const* str, int start, int length );

char const* cstr_int( int i );
char const* cstr_float( float f );

CSTR_BOOL_T cstr_starts( char const* str, char const* start );
CSTR_BOOL_T cstr_ends( char const* str, char const* end );

CSTR_BOOL_T cstr_is_equal( char const* a, char const* b );
int cstr_compare( char const* a, char const* b );
int cstr_compare_nocase( char const* a, char const* b );

int cstr_find( char const* str, char const* find, int start );
int cstr_rfind( char const* str, char const* find, int start );

CSTR_U32 cstr_hash( char const* str );

struct cstr_tokenizer_t { void* internal; };
struct cstr_tokenizer_t cstr_tokenizer( char const* str );
char const* cstr_tokenize( struct cstr_tokenizer_t* tokenizer, char const* separators );

char* cstr_temp_buffer( size_t capacity );

#endif /* CSTR_NO_GLOBAL_API */


#ifdef CSTR_INSTANCE_API

struct cstri_t;
struct cstri_t* cstri_create( void* memctx );
void cstri_destroy( struct cstri_t* cstri );

void cstri_reset( struct cstri_t* cstri );

struct cstr_restore_point_t* cstri_restore_point( struct cstri_t* cstri );
void cstri_rollback( struct cstri_t* cstri, struct cstr_restore_point_t* restore_point );

char const* cstri( struct cstri_t* cstri, char const* str );
char const* cstri_n( struct cstri_t* cstri, char const* str, CSTR_SIZE_T n );

CSTR_BOOL_T cstri_is_interned( struct cstri_t* cstri, char const* str );

CSTR_SIZE_T cstri_len( struct cstri_t* cstri, char const* str );

char const* cstri_cat( struct cstri_t* cstri, char const* a, char const* b );

char const* cstri_vformat( struct cstri_t* cstri, char const* format, CSTR_VA_LIST_T args );
char const* cstri_format( struct cstri_t* cstri, char const* format, ... );

char const* cstri_trim( struct cstri_t* cstri, char const* str );
char const* cstri_ltrim( struct cstri_t* cstri, char const* str );
char const* cstri_rtrim( struct cstri_t* cstri, char const* str );

char const* cstri_left( struct cstri_t* cstri, char const* str, size_t n );
char const* cstri_right( struct cstri_t* cstri, char const* str, size_t n );
char const* cstri_mid( struct cstri_t* cstri, char const* str, size_t start, size_t n );

char const* cstri_upper( struct cstri_t* cstri, char const* str );
char const* cstri_lower( struct cstri_t* cstri, char const* str );

char const* cstri_lpad( struct cstri_t* cstri, char const* str, char padding, CSTR_SIZE_T total_max_length );
char const* cstri_rpad( struct cstri_t* cstri, char const* str, char padding, CSTR_SIZE_T total_max_length );

char const* cstri_join( struct cstri_t* cstri, char const* a, char const* b, char const* separator );

char const* cstri_replace( struct cstri_t* cstri, char const* str, char const* find, char const* replacement );

char const* cstri_insert( struct cstri_t* cstri, char const* str, int position, char const* insertion );
char const* cstri_remove( struct cstri_t* cstri, char const* str, int start, int length );

char const* cstri_int( struct cstri_t* cstri, int i );
char const* cstri_float( struct cstri_t* cstri, float f );

CSTR_BOOL_T cstri_starts( struct cstri_t* cstri, char const* str, char const* start );
CSTR_BOOL_T cstri_ends( struct cstri_t* cstri, char const* str, char const* end );

CSTR_BOOL_T cstri_is_equal( struct cstri_t* cstri, char const* a, char const* b );
int cstri_compare( struct cstri_t* cstri, char const* a, char const* b );
int cstri_compare_nocase( struct cstri_t* cstri, char const* a, char const* b );

int cstri_find( struct cstri_t* cstri, char const* str, char const* find, int start );
int cstri_rfind( struct cstri_t* cstri, char const* str, char const* find, int start );

CSTR_U32 cstri_hash( struct cstri_t* cstri, char const* str );

struct cstr_tokenizer_t cstri_tokenizer( struct cstri_t* cstri, char const* str );
char const* cstri_tokenize( struct cstri_t* cstri, struct cstr_tokenizer_t* tokenizer, char const* separators );

char* cstri_temp_buffer( struct cstri_t* cstri, size_t capacity );

#endif /* CSTR_INSTANCE_API */




#endif /* cstr_h */


/**

cstr.h
======

*/


// If we are running tests on windows
#if defined( CSTR_RUN_TESTS ) && defined( _WIN32 ) && !defined( __TINYC__ )
    // To get file names/line numbers with meory leak detection, we need to include crtdbg.h before all other files
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif


/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef CSTR_IMPLEMENTATION
#undef CSTR_IMPLEMENTATION

#ifndef CSTR_DEFAULT_BLOCK_SIZE 
    #define CSTR_DEFAULT_BLOCK_SIZE 0x400000 /* 4 MB */
#endif

#ifndef CSTR_MUTEX_LOCK
    #define CSTR_MUTEX_LOCK() 
    #define CSTR_MUTEX_UNLOCK() 
#endif

#ifndef CSTR_VA_START
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdarg.h>
    #define CSTR_VA_START va_start
    #define CSTR_VA_END va_end
#endif

#ifndef CSTR_ASSERT
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <assert.h>
    #define CSTR_ASSERT( expression, message ) assert( ( expression ) && ( message ) )
#endif

#ifndef CSTR_ISSPACE
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <ctype.h>
    #define CSTR_ISSPACE( c ) ( isspace( c ) )
#endif 

#ifndef CSTR_TOUPPER
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <ctype.h>
    #define CSTR_TOUPPER( c ) ( toupper( c ) )
#endif 

#ifndef CSTR_TOLOWER
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <ctype.h>
    #define CSTR_TOLOWER( c ) ( tolower( c ) )
#endif 

#ifndef CSTR_MEMCPY
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define CSTR_MEMCPY( dst, src, cnt ) ( memcpy( (dst), (src), (cnt) ) )
#endif 

#ifndef CSTR_MEMCMP
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define CSTR_MEMCMP( a, b, cnt ) ( memcmp( (a), (b), (cnt) ) )
#endif 

#ifndef CSTR_MEMSET
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define CSTR_MEMSET( ptr, val, cnt ) ( memset( (ptr), (val), (cnt) ) )
#endif 

#ifndef CSTR_STRLEN
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define CSTR_STRLEN( s ) ( strlen( s ) )        
#endif 

#ifndef CSTR_STRSTR
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #define CSTR_STRSTR( s1, s2 ) ( strstr( (s1), (s2) ) )        
#endif 

#ifndef CSTR_STRNICMP
    #ifdef _WIN32
        #define _CRT_NONSTDC_NO_DEPRECATE 
        #define _CRT_SECURE_NO_WARNINGS
        #include <string.h>
        #define CSTR_STRNICMP( s1, s2, len ) ( strnicmp( (s1), (s2), (len) ) )
    #else
        #include <strings.h>
        #define CSTR_STRNICMP( s1, s2, len ) ( strncasecmp( (s1), (s2), (len) ) )        
    #endif
#endif 

#ifndef CSTR_VSNPRINTF
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdio.h>
    #define CSTR_VSNPRINTF( s, n, fmt, args ) ( vsnprintf( (s), (n), (fmt), (args) ) )
#endif

#ifndef CSTR_MALLOC
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #define CSTR_MALLOC( ctx, size ) ( malloc( size ) )
    #define CSTR_FREE( ctx, ptr ) ( free( ptr ) )
#endif

#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h>


//// instance api

struct cstr_block_t {
    char* head;
    char* tail;
    char* end;
};


struct cstr_slot_t {
    CSTR_U32 hash;
    CSTR_SIZE_T length;
    char const* string;
};


struct cstri_t {
    void* memctx;
    CSTR_SIZE_T blocks_count;
    CSTR_SIZE_T blocks_capacity;
    struct cstr_block_t* blocks;
    CSTR_SIZE_T hash_table_count;
    CSTR_SIZE_T hash_table_capacity;
    struct cstr_slot_t* hash_table;
    CSTR_SIZE_T temp_capacity;
    char* temp_buffer;
};


static CSTR_U32 internal_cstr_hash( char const* str, CSTR_SIZE_T length ) {
	CSTR_U32 m = 0x5bd1e995u;
	CSTR_U32 h = 0x31313137u;
	while( length >= 4 ) {
		CSTR_U32 k = *(CSTR_U32 *)str;
		k *= m;
		k ^= k >> 24;
		k *= m;
		h *= m;
		h ^= k;
		str += 4;
		length -= 4;
	}
    switch( length ) {
        case 3: h ^= str[ 2 ] << 16;
        case 2: h ^= str[ 1 ] << 8;
        case 1: h ^= str[ 0 ];
                h *= m;
	}
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;
	return h;
}


static struct cstr_slot_t* internal_cstr_interned( struct cstri_t* cstri, char const* str ) {
    if( str ) {
        for( CSTR_SIZE_T i = 0; i < cstri->blocks_count; ++i ) {
            if( str >= cstri->blocks[ i ].head + sizeof( CSTR_SIZE_T ) + sizeof( CSTR_U32 ) && str < cstri->blocks[ i ].end ) {
                str -= sizeof( CSTR_U32 );
                CSTR_U32 hash = *(CSTR_U32*) str;
                str -= sizeof( CSTR_SIZE_T );
        	    size_t slot = ( hash & ( cstri->hash_table_capacity - 1 ) );
	            while( cstri->hash_table[ slot ].string ) {
                    if( cstri->hash_table[ slot ].string == str ) {
                        return &cstri->hash_table[ slot ];
                    }
		            slot = ( slot + 1 ) & ( cstri->hash_table_capacity - 1 );
	            }   
                return NULL;
            }
        }
    }
    return NULL;
}


static struct cstr_slot_t* internal_cstr_find_slot( struct cstri_t* cstri, CSTR_U32 hash, char const* str, CSTR_SIZE_T len ) {
	size_t slot = ( hash & ( cstri->hash_table_capacity - 1 ) );
	while( cstri->hash_table[ slot ].string ) {
		if( cstri->hash_table[ slot ].hash == hash && cstri->hash_table[ slot ].length == len ) {
            char const* slot_string = cstri->hash_table[ slot ].string + sizeof( CSTR_SIZE_T ) + sizeof( CSTR_U32 );
			if( CSTR_MEMCMP( slot_string, str, len ) == 0 ) {
                break;
			}
		}
		slot = ( slot + 1 ) & ( cstri->hash_table_capacity - 1 );
	}   
    return &cstri->hash_table[ slot ];
}


static char const* internal_cstr_insert( struct cstri_t* cstri, char const* str, CSTR_SIZE_T n ) {
    struct cstr_slot_t* interned = internal_cstr_interned( cstri, str );
    if( interned && interned->length == n ) {
        return interned->string + sizeof( CSTR_SIZE_T ) + sizeof( CSTR_U32 );
    }

    if( !str ) {
        str = "";
        n = 0;
    }
    CSTR_U32 hash = internal_cstr_hash( str, n );
    struct cstr_slot_t* slot = internal_cstr_find_slot( cstri, hash, str, n );
    if( slot->string ) {
        return slot->string + sizeof( CSTR_SIZE_T ) + sizeof( CSTR_U32 );
    }

    if( cstri->hash_table_count >= cstri->hash_table_capacity / 3 ) {
        size_t old_capacity = cstri->hash_table_capacity;
        struct cstr_slot_t* old_table = cstri->hash_table;

        cstri->hash_table_capacity *= 2;

        cstri->hash_table = (struct cstr_slot_t*) CSTR_MALLOC( cstri->memctx,  
            cstri->hash_table_capacity * sizeof( *cstri->hash_table ) );
        CSTR_MEMSET( cstri->hash_table, 0, cstri->hash_table_capacity * sizeof( *cstri->hash_table ) );

        for( size_t i = 0; i < old_capacity; ++i ) {
            if( old_table[ i ].string ) {
                CSTR_U32 entry_hash = old_table[ i ].hash;
                size_t new_slot = ( entry_hash & cstri->hash_table_capacity - 1 );
                while( cstri->hash_table[ new_slot ].string ) {
                    new_slot = ( new_slot + 1 ) & ( cstri->hash_table_capacity - 1 );
                }
                cstri->hash_table[ new_slot ] = old_table[ i ];
            }               
        }

        CSTR_FREE( cstri->memctx, old_table );
        slot = internal_cstr_find_slot( cstri, hash, str, n );
    }

    CSTR_SIZE_T alloc_len = ( n + 1 + sizeof( CSTR_SIZE_T ) + sizeof( CSTR_U32 ) + 0xf ) & ~(CSTR_SIZE_T)0xf;

    struct cstr_block_t* block = NULL;
    if( cstri->blocks_count > 0 ) {
        struct cstr_block_t* b = &cstri->blocks[ cstri->blocks_count - 1 ];
        if( (CSTR_SIZE_T)( b->end - b->tail ) >= alloc_len ) {
            block = b;
        }
    }
    if( !block ) {
        if( cstri->blocks_count >= cstri->blocks_capacity ) {
            cstri->blocks_capacity *= 2;
            void* new_blocks = CSTR_MALLOC( cstri->memctx, cstri->blocks_capacity * sizeof( struct cstr_block_t ) );
            CSTR_MEMCPY( new_blocks, cstri->blocks, cstri->blocks_count * sizeof( struct cstr_block_t ) );
            CSTR_FREE( cstri->memctx, cstri->blocks );
            cstri->blocks = (struct cstr_block_t*) new_blocks;
        }
        block = &cstri->blocks[ cstri->blocks_count++ ];
        CSTR_SIZE_T size = alloc_len <= CSTR_DEFAULT_BLOCK_SIZE ? CSTR_DEFAULT_BLOCK_SIZE : alloc_len;
        block->head = (char*) CSTR_MALLOC( cstri->memctx, size );
        block->tail = block->head;
        block->end = block->head + size;
    }

    char* item = block->tail;
    block->tail += alloc_len;

    slot->hash = hash;
    slot->length = n;
    slot->string = item;
    ++cstri->hash_table_count;

    *(CSTR_SIZE_T*)item = n;
    item += sizeof( CSTR_SIZE_T );
    *(CSTR_U32*)item = hash;
    item += sizeof( CSTR_U32 );
    CSTR_MEMCPY( item, str, n );
    item[ n ] = '\0';
    return item;
}


static char* internal_cstr_temp_buffer( struct cstri_t* cstri, size_t capacity ) {
    if( cstri->temp_capacity <= capacity ) {
        CSTR_FREE( cstri->memctx, cstri->temp_buffer );
        while( cstri->temp_capacity <= capacity ) {
            cstri->temp_capacity *= 2;
        }
        cstri->temp_buffer = (char*) CSTR_MALLOC( cstri->memctx, cstri->temp_capacity );
    }
    return cstri->temp_buffer;
}


struct cstri_t* cstri_create( void* memctx ) {
    struct cstri_t* cstri = (struct cstri_t*) CSTR_MALLOC( memctx, sizeof( struct cstri_t ) );
    cstri->memctx = memctx;
    cstri->blocks_count = 0;
    cstri->blocks_capacity = 16;
    cstri->blocks = (struct cstr_block_t*) CSTR_MALLOC( memctx, cstri->blocks_capacity * sizeof( *cstri->blocks ) );
    cstri->hash_table_count = 1024;
    cstri->hash_table_capacity = 1024;
    cstri->hash_table = (struct cstr_slot_t*) CSTR_MALLOC( memctx, cstri->hash_table_capacity * sizeof( *cstri->hash_table ) );
    CSTR_MEMSET( cstri->hash_table, 0, cstri->hash_table_capacity * sizeof( *cstri->hash_table ) );
    cstri->temp_capacity = 1024;
    cstri->temp_buffer = (char*) CSTR_MALLOC( memctx, cstri->temp_capacity );
    return cstri;
}


void cstri_destroy( struct cstri_t* cstri ) {
    CSTR_FREE( cstri->memctx, cstri->temp_buffer );
    CSTR_FREE( cstri->memctx, cstri->hash_table );
    for( CSTR_SIZE_T i = 0; i < cstri->blocks_count; ++i ) {
        CSTR_FREE( cstri->memctx, cstri->blocks[ i ].head );
    }
    CSTR_FREE( cstri->memctx, cstri->blocks );
    CSTR_FREE( cstri->memctx, cstri );
}


void cstri_reset( struct cstri_t* cstri ) {
    for( CSTR_SIZE_T i = 0; i < cstri->blocks_count; ++i ) {
        CSTR_FREE( cstri->memctx, cstri->blocks[ i ].head );
    }
    cstri->blocks_count = 0;
    CSTR_MEMSET( cstri->hash_table, 0, cstri->hash_table_capacity * sizeof( *cstri->hash_table ) );
}


struct cstr_restore_point_t* cstri_restore_point( struct cstri_t* cstri ) {
    return (struct cstr_restore_point_t*)( cstri->blocks_count > 0 ? cstri->blocks[ cstri->blocks_count - 1 ].tail : NULL );
}


void cstri_rollback( struct cstri_t* cstri, struct cstr_restore_point_t* restore_point ) {
    // find the block containing the restore point    
    CSTR_SIZE_T index = cstri->blocks_count;
    for( CSTR_SIZE_T i = 0; i < cstri->blocks_count; ++i ) {
        if( restore_point >= (struct cstr_restore_point_t*)cstri->blocks[ i ].head 
          && restore_point <= (struct cstr_restore_point_t*)cstri->blocks[ i ].tail ) {
            index = i;
            break;
        }
    }

    // ignore invalid restore points
    if( index >= cstri->blocks_count ) {
        return;
    }

    // remove blocks allocated after restore point
    cstri->blocks[ index ].tail = (char*)restore_point;
    for( CSTR_SIZE_T i = index + 1; i < cstri->blocks_count; ++i ) {
        CSTR_FREE( cstri->memctx, cstri->blocks[ i ].head );
    }
    cstri->blocks_count = index + 1;

    // recreate hash table
    CSTR_MEMSET( cstri->hash_table, 0, cstri->hash_table_capacity * sizeof( *cstri->hash_table ) );
    for( CSTR_SIZE_T i = 0; i < cstri->blocks_count; ++i ) {
        struct cstr_block_t* block = &cstri->blocks[ i ];
        char const* ptr = block->head;
        while( ptr + sizeof( CSTR_SIZE_T ) + sizeof( CSTR_U32 ) < block->tail ) {
            CSTR_SIZE_T len = *(CSTR_SIZE_T*)ptr;
            CSTR_U32 hash = *(CSTR_U32*)( ptr + sizeof( CSTR_SIZE_T ) );

            size_t slot = ( hash & ( cstri->hash_table_capacity - 1 ) );
	        while( cstri->hash_table[ slot ].string ) {
		        slot = ( slot + 1 ) & ( cstri->hash_table_capacity - 1 );
	        }   
            cstri->hash_table[ slot ].hash = hash;
            cstri->hash_table[ slot ].length = len;
            cstri->hash_table[ slot ].string = ptr;
            ptr += ( len + 1 + sizeof( CSTR_SIZE_T ) + sizeof( CSTR_U32 ) + 0xf ) & ~(CSTR_SIZE_T)0xf;
        }
    }

}


char const* cstri( struct cstri_t* cstri, char const* str ) {
    return internal_cstr_insert( cstri, str, str ? CSTR_STRLEN( str ) : 0 );
}


char const* cstri_n( struct cstri_t* cstri, char const* str, CSTR_SIZE_T n ) {
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    CSTR_SIZE_T len = 0;
    while( str[ len ] && len < n ) {
        ++len;
    }
    return internal_cstr_insert( cstri, str, len );
}


CSTR_BOOL_T cstri_is_interned( struct cstri_t* cstri, char const* str ) {
    if( !str ) return 0;
    struct cstr_slot_t* slot = internal_cstr_interned( cstri, str );
    return slot != NULL;
}


CSTR_SIZE_T cstri_len( struct cstri_t* cstri, char const* str ) {
    if( !str ) return 0;
    struct cstr_slot_t* slot = internal_cstr_interned( cstri, str );
    if( slot ) {
        return slot->length;
    } else {
        return CSTR_STRLEN( str );
    }
}


char const* cstri_cat( struct cstri_t* cstri, char const* a, char const* b ) {
    CSTR_SIZE_T len_a = cstri_len( cstri, a );
    CSTR_SIZE_T len_b = cstri_len( cstri, b );
    CSTR_SIZE_T len = len_a + len_b;
    char* temp = internal_cstr_temp_buffer( cstri, len );
    CSTR_MEMCPY( temp, a, len_a );
    CSTR_MEMCPY( temp + len_a, b, len_b );
    return internal_cstr_insert( cstri, temp, len );
}


char const* cstri_vformat( struct cstri_t* cstri, char const* format, CSTR_VA_LIST_T args ) {
    if( !format ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
	int size = CSTR_VSNPRINTF( cstri->temp_buffer, cstri->temp_capacity, format, args );
    if( size < 0 ) {
        return NULL;
    }
    if( (CSTR_SIZE_T) size >= cstri->temp_capacity ) {
        internal_cstr_temp_buffer( cstri, size + 1u );
    	CSTR_VSNPRINTF( cstri->temp_buffer, cstri->temp_capacity, format, args );
    }
    return internal_cstr_insert( cstri, cstri->temp_buffer, (CSTR_SIZE_T)size );
}


char const* cstri_format( struct cstri_t* cstri, char const* format, ... ) {
    if( !format ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
	CSTR_VA_LIST_T args;
    CSTR_VA_START( args, format );
    char const* ret = cstri_vformat( cstri, format, args );
    CSTR_VA_END( args );
    return ret;
}


char const* cstri_trim( struct cstri_t* cstri, char const* str ) { 
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    char const* start = str;
    char const* end = str + cstri_len( cstri, str );
    while( start < end && CSTR_ISSPACE( *start ) ) {
        ++start;
    }
    while( end > start && CSTR_ISSPACE( *( end - 1 ) ) ) {
        --end;
    }
    return internal_cstr_insert( cstri, start, (CSTR_SIZE_T)( end - start ) ); 
}


char const* cstri_ltrim( struct cstri_t* cstri, char const* str ) { 
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    char const* start = str;
    char const* end = str + cstri_len( cstri, str );
    while( start < end && CSTR_ISSPACE( *start ) ) {
        ++start;
    }
    return internal_cstr_insert( cstri, start, (CSTR_SIZE_T)( end - start ) ); 
}


char const* cstri_rtrim( struct cstri_t* cstri, char const* str ) { 
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    char const* start = str;
    char const* end = str + cstri_len( cstri, str );
    while( end > start && CSTR_ISSPACE( *( end - 1 ) ) ) {
        --end;
    }
    return internal_cstr_insert( cstri, start, (CSTR_SIZE_T)( end - start ) ); 
}


char const* cstri_left( struct cstri_t* cstri, char const* str, size_t n ) { 
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    CSTR_SIZE_T len = cstri_len( cstri, str );
    if( len <= n ) {
        return internal_cstr_insert( cstri, str, len );
    }
    return internal_cstr_insert( cstri, str, n ); 
}


char const* cstri_right( struct cstri_t* cstri, char const* str, size_t n ) { 
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    CSTR_SIZE_T len = cstri_len( cstri, str );
    if( len <= n ) {
        return internal_cstr_insert( cstri, str, len );
    }
    return internal_cstr_insert( cstri, str + len - n, n ); 
}


char const* cstri_mid( struct cstri_t* cstri, char const* str, size_t start, size_t n ) { 
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    CSTR_SIZE_T len = cstri_len( cstri, str );
    if( len <= start ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    if( len - start <= n || n <= 0 ) {
        return internal_cstr_insert( cstri, str + start, len - start );
    }
    return internal_cstr_insert( cstri, str + start, n ); 
}


char const* cstri_upper( struct cstri_t* cstri, char const* str ) { 
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    CSTR_SIZE_T len = cstri_len( cstri, str );
    char* temp = internal_cstr_temp_buffer( cstri, len );
    for( CSTR_SIZE_T i = 0; i < len; ++i ) {
        temp[ i ] = (char) CSTR_TOUPPER( str[ i ] );
    }
    return internal_cstr_insert( cstri, temp, len ); 
}


char const* cstri_lower( struct cstri_t* cstri, char const* str ) { 
    if( !str ) {
        return internal_cstr_insert( cstri, "", 0 );
    }
    CSTR_SIZE_T len = cstri_len( cstri, str );
    char* temp = internal_cstr_temp_buffer( cstri, len );
    for( CSTR_SIZE_T i = 0; i < len; ++i ) {
        temp[ i ] = (char) CSTR_TOLOWER( str[ i ] );
    }
    return internal_cstr_insert( cstri, temp, len ); 
}

#pragma warning( push )
#pragma warning( disable: 4100 )

char const* cstri_lpad( struct cstri_t* cstri, char const* str, char padding, CSTR_SIZE_T total_max_length ) {
    return NULL; // todo 
}


char const* cstri_rpad( struct cstri_t* cstri, char const* str, char padding, CSTR_SIZE_T total_max_length ) {
    return NULL; // todo 
}


char const* cstri_join( struct cstri_t* cstri, char const* a, char const* b, char const* separator ) {
    return NULL; // todo 
}


char const* cstri_replace( struct cstri_t* cstri, char const* str, char const* find, char const* replacement ) {
    return NULL; // todo 
}


char const* cstri_insert( struct cstri_t* cstri, char const* str, int position, char const* insertion ) {
    return NULL; // todo 
}


char const* cstri_remove( struct cstri_t* cstri, char const* str, int start, int length ) {
    return NULL; // todo 
}

#pragma warning( pop )

char const* cstri_int( struct cstri_t* cstri, int i ) { 
    return cstri_format( cstri, "%d", i ); 
}


char const* cstri_float( struct cstri_t* cstri, float f ) { 
    return cstri_format( cstri, "%f", f ); 
}


CSTR_BOOL_T cstri_starts( struct cstri_t* cstri, char const* str, char const* start ) { 
    CSTR_SIZE_T str_len = cstri_len( cstri, str );
    CSTR_SIZE_T start_len = cstri_len( cstri, start );
    return str_len >= start_len && CSTR_MEMCMP( str, start, start_len ) == 0;
}


CSTR_BOOL_T cstri_ends( struct cstri_t* cstri, char const* str, char const* end ) { 
    CSTR_SIZE_T str_len = cstri_len( cstri, str );
    CSTR_SIZE_T end_len = cstri_len( cstri, end );
    return str_len >= end_len && CSTR_MEMCMP( str + str_len - end_len, end, end_len ) == 0;
}


CSTR_BOOL_T cstri_is_equal( struct cstri_t* cstri, char const* a, char const* b ) {
    if( a == b ) {
        return 1;
    }
    CSTR_SIZE_T len_a = cstri_len( cstri, a );
    CSTR_SIZE_T len_b = cstri_len( cstri, b );
    if( len_a != len_b ) {
        return 0;
    }
    return CSTR_MEMCMP( a, b, len_a ) == 0;
}


int cstri_compare( struct cstri_t* cstri, char const* a, char const* b ) {
    if( a == b ) {
        return 0;
    }
    CSTR_SIZE_T len_a = cstri_len( cstri, a );
    CSTR_SIZE_T len_b = cstri_len( cstri, b );
    CSTR_SIZE_T min_len = len_a < len_b ? len_a : len_b;
    return CSTR_MEMCMP( a ? a : "", b ? b : "", min_len + 1 );
}

int cstri_compare_nocase( struct cstri_t* cstri, char const* a, char const* b ) {
    if( a == b ) {
        return 0;
    }
    CSTR_SIZE_T len_a = cstri_len( cstri, a );
    CSTR_SIZE_T len_b = cstri_len( cstri, b );
    CSTR_SIZE_T min_len = len_a < len_b ? len_a : len_b;
    return CSTR_STRNICMP( a ? a : "", b ? b : "", min_len + 1 );
}


int cstri_find( struct cstri_t* cstri, char const* str, char const* find, int start ) { 
    if( !str || !find || start < 0) {
        return -1;
    }
    CSTR_SIZE_T len = cstri_len( cstri, str );
    if( (CSTR_SIZE_T)start >= len ) {
        return -1;
    }
    char const* res = CSTR_STRSTR( str + start, find );
    if( !res ) {
        return -1;
    }
    return (int)( res - str );
}


int cstri_rfind( struct cstri_t* cstri, char const* str, char const* find, int start ) { 
    (void) cstri, (void)str, (void)find, (void)start;
    // todo
    return -1;
}


CSTR_U32 cstri_hash( struct cstri_t* cstri, char const* str ) {
    struct cstr_slot_t* slot = internal_cstr_interned( cstri, str );
    if( slot ) {
        return slot->hash;
    } else {
        return internal_cstr_hash( str ? str : "", str ? CSTR_STRLEN( str ) : 0 );
    }
}


struct cstr_tokenizer_t cstri_tokenizer( struct cstri_t* cstri, char const* str ) {
    struct cstr_tokenizer_t tokenizer;
    tokenizer.internal = (void*)internal_cstr_insert( cstri, str, str ? CSTR_STRLEN( str ) : 0 );
    return tokenizer;
}


char const* cstri_tokenize( struct cstri_t* cstri, struct cstr_tokenizer_t* tokenizer, char const* separators ) {
    CSTR_SIZE_T sep_len = cstr_len( separators );
    char const* pos = (char const*) tokenizer->internal;

    // strip leading separators
    while( *pos ) {
        char ch = *pos;
        CSTR_BOOL_T is_sep = 0;
        for( CSTR_SIZE_T i = 0; i < sep_len; ++i ) {
            if( ch == separators[ i ] ) {
                is_sep = 1;
                break;
            }
        }
        if( !is_sep ) break;
        ++pos;
    }

    // find next separator
    char const* token = pos;
    while( *pos ) {
        char ch = *pos;
        CSTR_BOOL_T is_sep = 0;
        for( CSTR_SIZE_T i = 0; i < sep_len; ++i ) {
            if( ch == separators[ i ] ) {
                is_sep = 1;
                break;
            }
        }
        if( is_sep ) break;
        ++pos;
    }

    tokenizer->internal = (void*) pos;
    
    // no more tokens?
    if( *token == '\0' ) {
        return NULL;
    }

    // return the token
    return internal_cstr_insert( cstri, token, (CSTR_SIZE_T)( pos - token ) );
}


char* cstri_temp_buffer( struct cstri_t* cstri, size_t capacity ) {
    return internal_cstr_temp_buffer( cstri, capacity );
}


//// global api

#ifndef CSTR_NO_GLOBAL_API


static struct cstri_t* g_internal_cstr = NULL;


void internal_cstr_cleanup( void ) {
    if( g_internal_cstr ) {
        cstri_destroy( g_internal_cstr );
        g_internal_cstr = NULL;
    }
}


static void internal_cstr_instance( void ) {
    if( !g_internal_cstr ) {
        g_internal_cstr = cstri_create( NULL );
        static int atexit_set = 0;
        if( !atexit_set ) {
            #ifndef __wasm__
            atexit( internal_cstr_cleanup );
            #endif
            atexit_set = 1;
        }
    }
}


void cstr_reset( void ) { 
    CSTR_MUTEX_LOCK();
    internal_cstr_cleanup();
    CSTR_MUTEX_UNLOCK();
}


struct cstr_restore_point_t* cstr_restore_point( void ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    struct cstr_restore_point_t* ret = cstri_restore_point( g_internal_cstr );
    CSTR_MUTEX_UNLOCK();
    return ret;
}


void cstr_rollback( struct cstr_restore_point_t* restore_point ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    cstri_rollback( g_internal_cstr, restore_point );
    CSTR_MUTEX_UNLOCK();
}


char const* cstr( char const* str ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_n( char const* str, CSTR_SIZE_T n ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_n( g_internal_cstr, str, n ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


CSTR_BOOL_T cstr_is_interned( char const* str ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    CSTR_BOOL_T ret = cstri_is_interned( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


CSTR_SIZE_T cstr_len( char const* str ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    CSTR_SIZE_T ret = cstri_len( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_cat( char const* a, char const* b ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_cat( g_internal_cstr, a, b ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_vformat( char const* format, CSTR_VA_LIST_T args ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_vformat( g_internal_cstr, format, args ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_format( char const* format, ... ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
	CSTR_VA_LIST_T args;
    CSTR_VA_START( args, format );
    char const* ret = cstri_vformat( g_internal_cstr, format, args ); 
    CSTR_VA_END( args );
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_trim( char const* str ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_trim( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_ltrim( char const* str ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_ltrim( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_rtrim( char const* str ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_rtrim( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_left( char const* str, size_t n ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_left( g_internal_cstr, str, n ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_right( char const* str, size_t n ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_right( g_internal_cstr, str, n ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_mid( char const* str, size_t start, size_t n ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_mid( g_internal_cstr, str, start, n ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_upper( char const* str ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_upper( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_lower( char const* str ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_lower( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_lpad( char const* str, char padding, CSTR_SIZE_T total_max_length ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_lpad( g_internal_cstr, str, padding, total_max_length ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_rpad( char const* str, char padding, CSTR_SIZE_T total_max_length ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_rpad( g_internal_cstr, str, padding, total_max_length ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_join( char const* a, char const* b, char const* separator ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_join( g_internal_cstr, a, b, separator ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_replace( char const* str, char const* find, char const* replacement ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_replace( g_internal_cstr, str, find, replacement ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_insert( char const* str, int position, char const* insertion ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_insert( g_internal_cstr, str, position,insertion ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_remove( char const* str, int start, int length ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_remove( g_internal_cstr, str, start, length ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_int( int i ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_int( g_internal_cstr, i ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_float( float f ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_float( g_internal_cstr, f ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


CSTR_BOOL_T cstr_starts( char const* str, char const* start ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    CSTR_BOOL_T ret = cstri_starts( g_internal_cstr, str, start ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


CSTR_BOOL_T cstr_ends( char const* str, char const* end ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    CSTR_BOOL_T ret = cstri_ends( g_internal_cstr, str, end ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


CSTR_BOOL_T cstr_is_equal( char const* a, char const* b ) {
    if( a == b ) {
        return 1;
    }
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    int ret = cstri_is_equal( g_internal_cstr, a, b ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


int cstr_compare( char const* a, char const* b ) {
    if( a == b ) {
        return 0;
    }
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    int ret = cstri_compare( g_internal_cstr, a, b ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


int cstr_compare_nocase( char const* a, char const* b ) {
    if( a == b ) {
        return 0;
    }
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    int ret = cstri_compare_nocase( g_internal_cstr, a, b ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


int cstr_find( char const* str, char const* find, int start ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    int ret = cstri_find( g_internal_cstr, str, find, start ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


int cstr_rfind( char const* str, char const* find, int start ) { 
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    int ret = cstri_rfind( g_internal_cstr, str, find, start ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


CSTR_U32 cstr_hash( char const* str ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    CSTR_U32  ret = cstri_hash( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


struct cstr_tokenizer_t cstr_tokenizer( char const* str ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    struct cstr_tokenizer_t ret = cstri_tokenizer( g_internal_cstr, str ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char const* cstr_tokenize( struct cstr_tokenizer_t* tokenizer, char const* separators ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char const* ret = cstri_tokenize( g_internal_cstr, tokenizer, separators ); 
    CSTR_MUTEX_UNLOCK();
    return ret;
}


char* cstr_temp_buffer( size_t capacity ) {
    CSTR_MUTEX_LOCK();
    if( !g_internal_cstr ) internal_cstr_instance();
    char* ret = cstri_temp_buffer( g_internal_cstr, capacity );
    CSTR_MUTEX_UNLOCK();
    return ret;
}


#endif /* CSTR_NO_GLOBAL_API */


#endif /* CSTR_IMPLEMENTATION */


/*
----------------------
    TESTS
----------------------
*/


#ifdef CSTR_RUN_TESTS

#include "testfw.h"

void stress_tests( void ) {

    TESTFW_TEST_BEGIN( "Create very large (16MB) cstr" );
    char* src = (char*) malloc( 16 * 1024 * 1024 + 1);
    TESTFW_EXPECTED( src != NULL );
    memset( src, 'A', 16 * 1024 * 1024 );
    src[ 16 * 1024 * 1024 ] = '\0';
    TESTFW_EXPECTED( strlen( src ) == 16 * 1024 * 1024 );
    char const* str = cstr( src );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != src );
    TESTFW_EXPECTED( strcmp( str, src ) == 0 );
    free( src );
    TESTFW_EXPECTED( strlen( str ) == 16 * 1024 * 1024 );
    TESTFW_TEST_END();


    TESTFW_TEST_BEGIN( "Create many strings (4M), and use restore points to roll back" );

    char const** strings = (char const**) malloc( sizeof( char* ) * 4 * 1000 * 1000 );
    struct cstr_restore_point_t* restore_point_a = NULL;
    struct cstr_restore_point_t* restore_point_b = NULL;
    struct cstr_restore_point_t* restore_point_c = NULL;
    for( CSTR_SIZE_T i = 0; i < 4 * 1000 * 1000; ++i ) {
        if( i == 1 * 1000 * 1000 ) {
            restore_point_a = cstr_restore_point();
        }
        if( i == 2 * 1000 * 1000 ) {
            restore_point_b = cstr_restore_point();
        }
        if( i == 3 * 1000 * 1000 ) {
            restore_point_c = cstr_restore_point();
        }
        char src[ 64 ];
        sprintf( src, "STRING: %x", (int) i );
        char const* str = cstr( src );
        strings[ i ] = str;
        TESTFW_EXPECTED( str != NULL );
        TESTFW_EXPECTED( str != src );
        TESTFW_EXPECTED( strlen( str ) != 0 );
        TESTFW_EXPECTED( strcmp( str, src ) == 0 );
    }

    TESTFW_EXPECTED( restore_point_a != NULL );
    TESTFW_EXPECTED( restore_point_b != NULL );
    TESTFW_EXPECTED( restore_point_c != NULL );

    for( CSTR_SIZE_T i = 0; i < 4 * 1000 * 1000; ++i ) {
        TESTFW_EXPECTED( cstr_is_interned( strings[ i ] ) );
    }

    cstr_rollback( restore_point_c );
    for( CSTR_SIZE_T i = 0; i < 3 * 1000 * 1000; ++i ) {
        TESTFW_EXPECTED( cstr_is_interned( strings[ i ] ) );
    }
    for( CSTR_SIZE_T i = 3 * 1000 * 1000; i < 4 * 1000 * 1000; ++i ) {
        TESTFW_EXPECTED( !cstr_is_interned( strings[ i ] ) );
    }

    // intentionally not rolling back to restore_point_b to test skipping over restore points

    cstr_rollback( restore_point_a );
    for( CSTR_SIZE_T i = 0; i < 1 * 1000 * 1000; ++i ) {
        TESTFW_EXPECTED( cstr_is_interned( strings[ i ] ) );
    }
    for( CSTR_SIZE_T i = 1 * 1000 * 1000; i < 4 * 1000 * 1000; ++i ) {
        TESTFW_EXPECTED( !cstr_is_interned( strings[ i ] ) );
    }
    free( (char*)strings );

    TESTFW_TEST_END();


    cstr_reset();
}


void test_cstr( void ) {
    TESTFW_TEST_BEGIN( "Create cstr from NULL pointer" );
    char const* str = cstr( NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from static string" );
    char const* str = cstr( "Static test string" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != (char*)"Static test string" );
    TESTFW_EXPECTED( strcmp( str, "Static test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from char array" );
    char arr[] = "Char array string";
    char const* str = cstr( arr );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != arr );
    TESTFW_EXPECTED( strcmp( str, "Char array string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from cstr" );
    char const* src = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_is_interned( src ) );
    TESTFW_EXPECTED( src != NULL );
    TESTFW_EXPECTED( strcmp( src, "Test string" ) == 0 );
    char const* str = cstr( src );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str == src );
    TESTFW_EXPECTED( strcmp( str, "Test string" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_n( void ) {
    TESTFW_TEST_BEGIN( "Create cstr from NULL pointer and explicit length" );
    char const* str = cstr_n( NULL, 0 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from static string and explicit length" );
    char const* str = cstr_n( "Static test string", 18 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != (char*)"Static test string" );
    TESTFW_EXPECTED( strcmp( str, "Static test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from char array and explicit length" );
    char arr[] = "Char array string";
    char const* str = cstr_n( arr, 17 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != arr );
    TESTFW_EXPECTED( strcmp( str, "Char array string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from cstr and explicit length" );
    char const* src = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_is_interned( src ) );
    TESTFW_EXPECTED( src != NULL );
    TESTFW_EXPECTED( strcmp( src, "Test string" ) == 0 );
    char const* str = cstr_n( src, 11 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str == src );
    TESTFW_EXPECTED( strcmp( str, "Test string" ) == 0 );
    TESTFW_TEST_END();


    TESTFW_TEST_BEGIN( "Create cstr from NULL pointer and too long explicit length" );
    char const* str = cstr_n( NULL, 20 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from static string and too long explicit length" );
    char const* str = cstr_n( "Static test string", 30 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != (char*)"Static test string" );
    TESTFW_EXPECTED( strcmp( str, "Static test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from char array and too long explicit length" );
    char arr[] = "Char array string";
    char const* str = cstr_n( arr, 30 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != arr );
    TESTFW_EXPECTED( strcmp( str, "Char array string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from cstr and too long explicit length" );
    char const* src = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_is_interned( src ) );
    TESTFW_EXPECTED( src != NULL );
    TESTFW_EXPECTED( strcmp( src, "Test string" ) == 0 );
    char const* str = cstr_n( src, 30 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str == src );
    TESTFW_EXPECTED( strcmp( str, "Test string" ) == 0 );
    TESTFW_TEST_END();


    TESTFW_TEST_BEGIN( "Create cstr from static string and explicit length shorter than string" );
    char const* str = cstr_n( "Static test string", 6 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != (char*)"Static" );
    TESTFW_EXPECTED( strcmp( str, "Static" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from char array and explicit length shorter than string" );
    char arr[] = "Char array string";
    char const* str = cstr_n( arr, 4 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != arr );
    TESTFW_EXPECTED( strcmp( str, "Char" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Create cstr from cstr and explicit length shorter than string" );
    char const* src = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_is_interned( src ) );
    TESTFW_EXPECTED( src != NULL );
    TESTFW_EXPECTED( strcmp( src, "Test string" ) == 0 );
    char const* str = cstr_n( src, 4 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( str != src );
    TESTFW_EXPECTED( strcmp( str, "Test" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_len( void ) {
    TESTFW_TEST_BEGIN( "Get length of a cstr string" );
    char const* str = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( cstr_len( str ) == 11 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Get length of a cstr string containing zero terminator" );
    char const* str = cstr_n( "Test string\0part2", 18 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( cstr_len( str ) == 11 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Get length of a string literal" );
    TESTFW_EXPECTED( cstr_len( "Test string" ) == 11 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Get length of a char array" );
    char str[ 18 ] = "Test string\0part2";
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( cstr_len( str ) == 11 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Get length of a cstr created from char array" );
    char arr[ 18 ] = "Test string\0part2";
    char const* str = cstr_n( arr, 18 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( cstr_len( str ) == 11 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Get length of a NULL string" );
    TESTFW_EXPECTED( cstr_len( NULL ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_cat( void ) {
    TESTFW_TEST_BEGIN( "Concatenate two strings" );
    char const* str = cstr_cat( "Test", "String" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "TestString" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Concatenate a string with a NULL string" );
    char const* str = cstr_cat( "Test", NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "Test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Concatenate a NULL string with a string" );
    char const* str = cstr_cat( NULL, "String" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "String" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Concatenate two NULL strings" );
    char const* str = cstr_cat( NULL, NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_format( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_format with a NULL format string" );
    char const* str = cstr_format( NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_format to format a string" );
    char const* str = cstr_format( "Test format, int: %d, float: %f, str: %s", 7, 1.0f, "Test String" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "Test format, int: 7, float: 1.000000, str: Test String" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_trim( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_trim with leading spaces" );
    char const* str = cstr_trim( "   test" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with trailing spaces" );
    char const* str = cstr_trim( "test   " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with leading and trailing spaces" );
    char const* str = cstr_trim( "   test   " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with various whitespace chars" );
    char const* str = cstr_trim( "\t\r\ntest\t\r\n" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with single leading and trailing space" );
    char const* str = cstr_trim( " test " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with no leading or trailing spaces" );
    char const* str = cstr_trim( "test" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with single non-space char" );
    char const* str = cstr_trim( "t" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with single non-space char and single leading space" );
    char const* str = cstr_trim( " t" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with single non-space char and single trailing space" );
    char const* str = cstr_trim( "t " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with single non-space char and single leading and trailing spaces" );
    char const* str = cstr_trim( " t " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with spaces in the middle of the string" );
    char const* str = cstr_trim( "  test test  " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with a string of all spaces" );
    char const* str = cstr_trim( "     " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_trim with a NULL string" );
    char const* str = cstr_trim( NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_ltrim( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_ltrim with leading spaces" );
    char const* str = cstr_ltrim( "   test" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with trailing spaces" );
    char const* str = cstr_ltrim( "test   " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test   " ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with leading and trailing spaces" );
    char const* str = cstr_ltrim( "   test   " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test   " ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with various whitespace chars" );
    char const* str = cstr_ltrim( "\t\r\ntest\t\r\n" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test\t\r\n" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with single leading and trailing space" );
    char const* str = cstr_ltrim( " test " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test " ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with no leading or trailing spaces" );
    char const* str = cstr_ltrim( "test" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with single non-space char" );
    char const* str = cstr_ltrim( "t" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with single non-space char and single leading space" );
    char const* str = cstr_ltrim( " t" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with single non-space char and single trailing space" );
    char const* str = cstr_ltrim( "t " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t " ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with single non-space char and single leading and trailing spaces" );
    char const* str = cstr_ltrim( " t " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t " ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with spaces in the middle of the string" );
    char const* str = cstr_ltrim( "  test test  " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test test  " ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with a string of all spaces" );
    char const* str = cstr_ltrim( "     " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ltrim with a NULL string" );
    char const* str = cstr_ltrim( NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_rtrim( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_rtrim with leading spaces" );
    char const* str = cstr_rtrim( "   test" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "   test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with trailing spaces" );
    char const* str = cstr_rtrim( "test   " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with leading and trailing spaces" );
    char const* str = cstr_rtrim( "   test   " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "   test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with various whitespace chars" );
    char const* str = cstr_rtrim( "\t\r\ntest\t\r\n" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "\t\r\ntest" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with single leading and trailing space" );
    char const* str = cstr_rtrim( " test " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, " test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with no leading or trailing spaces" );
    char const* str = cstr_rtrim( "test" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with single non-space char" );
    char const* str = cstr_rtrim( "t" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with single non-space char and single leading space" );
    char const* str = cstr_rtrim( " t" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, " t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with single non-space char and single trailing space" );
    char const* str = cstr_rtrim( "t " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with single non-space char and single leading and trailing spaces" );
    char const* str = cstr_rtrim( " t " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, " t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with spaces in the middle of the string" );
    char const* str = cstr_rtrim( "  test test  " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "  test test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with a string of all spaces" );
    char const* str = cstr_rtrim( "     " );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_rtrim with a NULL string" );
    char const* str = cstr_rtrim( NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_left( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_left to get beginning of string" );
    char const* str = cstr_left( "Test string", 4 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "Test" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_left with length beyond end of string" );
    char const* str = cstr_left( "Test string", 20 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "Test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_left with NULL string" );
    char const* str = cstr_left( NULL, 4 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_right( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_right to get end of string" );
    char const* str = cstr_right( "Test string", 6 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_right with length beyond start of string" );
    char const* str = cstr_right( "Test string", 20 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "Test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_right with NULL string" );
    char const* str = cstr_right( NULL, 6 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_mid( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_mid to get middle of string" );
    char const* str = cstr_mid( "Test string", 2, 5 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "st st" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_mid with length beyond end of string" );
    char const* str = cstr_mid( "Test string", 2, 20 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "st string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_mid with start beyond end of string" );
    char const* str = cstr_mid( "Test string", 20, 5 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_mid on NULL string" );
    char const* str = cstr_mid( NULL, 2, 5 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_upper( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_upper with mixed case string" );
    char const* str = cstr_upper( "Test String" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "TEST STRING" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_upper with uppercase string" );
    char const* str = cstr_upper( "TEST STRING" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "TEST STRING" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_upper with lowercase string" );
    char const* str = cstr_upper( "test string" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "TEST STRING" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_upper with NULL string" );
    char const* str = cstr_upper( NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_lower( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_lower with mixed case string" );
    char const* str = cstr_lower( "Test String" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_lower with uppercase string" );
    char const* str = cstr_lower( "TEST STRING" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_lower with lowercase string" );
    char const* str = cstr_lower( "test string" );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_lower with NULL string" );
    char const* str = cstr_lower( NULL );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "" ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_int( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_int with a positive number" );
    char const* str = cstr_int( 42 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "42" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_int with a negative number" );
    char const* str = cstr_int( -100 );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "-100" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_int with a large number" );
    char const* str = cstr_int( 2147483647  );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "2147483647" ) == 0 );
    TESTFW_TEST_END();
}

    
void test_cstr_float( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_float" );
    char const* str = cstr_float( 42.0f );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "42.000000" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_int with a small number" );
    char const* str = cstr_float( 42.43e-6f );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "0.000042" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_int with a really small number" );
    char const* str = cstr_float( 42.43e-25f );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "0.000000" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_int with a really large number" );
    char const* str = cstr_float( 42e15f );
    TESTFW_EXPECTED( cstr_is_interned( str ) );
    TESTFW_EXPECTED( str != NULL );
    TESTFW_EXPECTED( strcmp( str, "41999999856279552.000000" ) == 0 );
    TESTFW_TEST_END();
}

    
void test_cstr_starts( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_starts with matching substring" );
    TESTFW_EXPECTED( cstr_starts( "TestString", "Test" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with non-matching substring" );
    TESTFW_EXPECTED( cstr_starts( "Test", "TestString" ) == false );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with identical strings" );
    TESTFW_EXPECTED( cstr_starts( "Test", "Test" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with first string empty" );
    TESTFW_EXPECTED( cstr_starts( "", "Test" ) == false );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with second string empty" );
    TESTFW_EXPECTED( cstr_starts( "Test", "" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with two empty strings" );
    TESTFW_EXPECTED( cstr_starts( "", "" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with single char second string" );
    TESTFW_EXPECTED( cstr_starts( "TestString", "T" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with second string NULL" );
    TESTFW_EXPECTED( cstr_starts( "TestString", NULL ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with first string NULL" );
    TESTFW_EXPECTED( cstr_starts( NULL, "TestString" ) == false );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_starts with both strings NULL" );
    TESTFW_EXPECTED( cstr_starts( NULL, NULL ) == true );
    TESTFW_TEST_END();
}


void test_cstr_ends( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_ends with matching substring" );
    TESTFW_EXPECTED( cstr_ends( "TestString", "String" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with non-matching substring" );
    TESTFW_EXPECTED( cstr_ends( "String", "TestString" ) == false );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with identical strings" );
    TESTFW_EXPECTED( cstr_ends( "Test", "Test" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with first string empty" );
    TESTFW_EXPECTED( cstr_ends( "", "String" ) == false );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with second string empty" );
    TESTFW_EXPECTED( cstr_ends( "String", "" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with two empty strings" );
    TESTFW_EXPECTED( cstr_ends( "", "" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with single char second string" );
    TESTFW_EXPECTED( cstr_ends( "TestString", "g" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with second string NULL" );
    TESTFW_EXPECTED( cstr_ends( "TestString", NULL ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with first string NULL" );
    TESTFW_EXPECTED( cstr_ends( NULL, "TestString" ) == false );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_ends with both strings NULL" );
    TESTFW_EXPECTED( cstr_ends( NULL, NULL ) == true );
    TESTFW_TEST_END();
}


void test_cstr_is_equal( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_is_equal with two NULL strings" );
    TESTFW_EXPECTED( cstr_is_equal( NULL, NULL ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_is_equal with a NULL string and an empty string" );
    TESTFW_EXPECTED( cstr_is_equal( NULL, "" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_is_equal with two string literals" );
    TESTFW_EXPECTED( cstr_is_equal( "Test string", "Test string" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_is_equal with two cstr strings" );
    char const* a = cstr( "Test string" );
    char const* b = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_is_equal( a, b ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_is_equal with a string literal and a cstr string" );
    char const* str = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_is_equal( str, "Test string" ) == true );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_is_equal with two different string literals" );
    TESTFW_EXPECTED( cstr_is_equal( "Test string A", "Test string B" ) == false );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_is_equal with two different cstr strings" );
    char const* a = cstr( "Test string A" );
    char const* b = cstr( "Test string B" );
    TESTFW_EXPECTED( cstr_is_equal( a, b ) == false );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_is_equal with a string literal and a different cstr string" );
    char const* str = cstr( "Test string A" );
    TESTFW_EXPECTED( cstr_is_equal( str, "Test string B" ) == false );
    TESTFW_TEST_END();
}


void test_cstr_compare( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_compare with two NULL strings" );
    TESTFW_EXPECTED( cstr_compare( NULL, NULL ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare with a NULL string and an empty string" );
    TESTFW_EXPECTED( cstr_compare( NULL, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare with two string literals" );
    TESTFW_EXPECTED( cstr_compare( "Test string", "Test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare with two cstr strings" );
    char const* a = cstr( "Test string" );
    char const* b = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_compare( a, b ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare with a string literal and a cstr string" );
    char const* str = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_compare( str, "Test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare with two different string literals" );
    TESTFW_EXPECTED( cstr_compare( "Test string A", "Test string B" ) < 0 );
    TESTFW_EXPECTED( cstr_compare( "Test string B", "Test string A" ) > 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare with two different cstr strings" );
    char const* a = cstr( "Test string A" );
    char const* b = cstr( "Test string B" );
    TESTFW_EXPECTED( cstr_compare( a, b ) < 0 );
    TESTFW_EXPECTED( cstr_compare( b, a ) > 0 );
    TESTFW_TEST_END();
}


void test_cstr_compare_nocase( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with two NULL strings" );
    TESTFW_EXPECTED( cstr_compare_nocase( NULL, NULL ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with a NULL string and an empty string" );
    TESTFW_EXPECTED( cstr_compare_nocase( NULL, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with two string literals" );
    TESTFW_EXPECTED( cstr_compare_nocase( "Test string", "Test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with two cstr strings" );
    char const* a = cstr( "Test string" );
    char const* b = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_compare_nocase( a, b ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with a string literal and a cstr string" );
    char const* str = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_compare_nocase( str, "Test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with two different string literals" );
    TESTFW_EXPECTED( cstr_compare_nocase( "Test string A", "Test string B" ) < 0 );
    TESTFW_EXPECTED( cstr_compare_nocase( "Test string B", "Test string A" ) > 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with two different cstr strings" );
    char const* a = cstr( "Test string A" );
    char const* b = cstr( "Test string B" );
    TESTFW_EXPECTED( cstr_compare_nocase( a, b ) < 0 );
    TESTFW_EXPECTED( cstr_compare_nocase( b, a ) > 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with two string literals differing only by case" );
    TESTFW_EXPECTED( cstr_compare_nocase( "TEST STRING", "Test string" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_compare_nocase with two cstr strings differing only by case" );
    char const* a = cstr( "TEST STRING" );
    char const* b = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_compare_nocase( a, b ) == 0 );
    TESTFW_TEST_END();
}


void test_cstr_find( void ) {

}


void test_cstr_hash( void ) {
    TESTFW_TEST_BEGIN( "Use cstr_hash with NULL string" );
    TESTFW_EXPECTED( cstr_hash( NULL ) == 0x569ed9aa );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_hash with empty string" );
    TESTFW_EXPECTED( cstr_hash( NULL ) == 0x569ed9aa );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_hash with string literal" );
    TESTFW_EXPECTED( cstr_hash( "Test string" ) == 0x5b8cc1a9 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Use cstr_hash with cstr string" );
    char const* str = cstr( "Test string" );
    TESTFW_EXPECTED( cstr_hash( str ) == 0x5b8cc1a9 );
    TESTFW_TEST_END();
}


void test_cstr_tokenize( void ) {
    TESTFW_TEST_BEGIN( "Can tokenize sample string" );
    
    struct cstr_tokenizer_t tokenizer = cstr_tokenizer( "A string\tof ,,tokens\nand some  more tokens    " );
    TESTFW_EXPECTED( tokenizer.internal != NULL );
    
    char const* token1 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( cstr_is_interned( token1 ) );
    TESTFW_EXPECTED( token1 != NULL );
    TESTFW_EXPECTED( strcmp( token1, "A" ) == 0 );
    
    char const* token2 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( cstr_is_interned( token2 ) );
    TESTFW_EXPECTED( token2 != NULL );
    TESTFW_EXPECTED( strcmp( token2, "string" ) == 0 );
    
    char const* token3 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( cstr_is_interned( token3 ) );
    TESTFW_EXPECTED( token3 != NULL );
    TESTFW_EXPECTED( strcmp( token3, "of" ) == 0 );
    
    char const* token4 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( cstr_is_interned( token4 ) );
    TESTFW_EXPECTED( token4 != NULL );
    TESTFW_EXPECTED( strcmp( token4, "tokens" ) == 0 );
    
    char const* token5 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( cstr_is_interned( token5 ) );
    TESTFW_EXPECTED( token5 != NULL );
    TESTFW_EXPECTED( strcmp( token5, "and" ) == 0 );
    
    char const* token6 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( cstr_is_interned( token6 ) );
    TESTFW_EXPECTED( token6 != NULL );
    TESTFW_EXPECTED( strcmp( token6, "some" ) == 0 );
    
    char const* token7 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( cstr_is_interned( token7 ) );
    TESTFW_EXPECTED( token7 != NULL );
    TESTFW_EXPECTED( strcmp( token7, "more" ) == 0 );
    
    char const* token8 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( cstr_is_interned( token8 ) );
    TESTFW_EXPECTED( token8 != NULL );
    TESTFW_EXPECTED( strcmp( token8, "tokens" ) == 0 );
    
    char const* token9 = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( token9 == NULL );

    TESTFW_TEST_END();


    TESTFW_TEST_BEGIN( "Can tokenize string containing only separators" );  
    struct cstr_tokenizer_t tokenizer = cstr_tokenizer( " \t ,,\n   " );
    TESTFW_EXPECTED( tokenizer.internal != NULL );    
    char const* token = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( token == NULL );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "Can tokenize NULL string" );  
    struct cstr_tokenizer_t tokenizer = cstr_tokenizer( NULL );
    TESTFW_EXPECTED( tokenizer.internal != NULL );    
    char const* token = cstr_tokenize( &tokenizer, " ,\t\n" );
    TESTFW_EXPECTED( token == NULL );
    TESTFW_TEST_END();
}


int main( int argc, char** argv ) {
    (void) argc, argv;

    TESTFW_INIT();
    
    test_cstr();
    test_cstr_n();
    test_cstr_len();
    test_cstr_cat();
    test_cstr_format();
    test_cstr_trim();
    test_cstr_ltrim();
    test_cstr_rtrim();
    test_cstr_left();
    test_cstr_right();
    test_cstr_mid();
    test_cstr_upper();
    test_cstr_lower();
    test_cstr_int();
    test_cstr_float();
    test_cstr_starts();
    test_cstr_ends();
    test_cstr_is_equal();
    test_cstr_compare();
    test_cstr_compare_nocase();
    test_cstr_find();
    test_cstr_hash();
    test_cstr_tokenize();
    stress_tests();

    cstr_reset();
    return TESTFW_SUMMARY();
}


// pass-through so the program will build with either /SUBSYSTEM:WINDOWS or /SUBSYSTEM:CONSOLE
#if defined( _WIN32 ) && !defined( __TINYC__ )
    #ifdef __cplusplus 
        extern "C" int __stdcall WinMain( struct HINSTANCE__*, struct HINSTANCE__*, char*, int ) { 
            return main( __argc, __argv ); 
        }
    #else
        struct HINSTANCE__;
        int __stdcall WinMain( struct HINSTANCE__* a, struct HINSTANCE__* b, char* c, int d ) { 
            (void) a, b, c, d; return main( __argc, __argv ); 
        }
    #endif
#endif

#define TESTFW_IMPLEMENTATION
#include "testfw.h"

#endif /* CSTR_RUN_TESTS */


/*
revision history:
    1.0     first released version  
*/


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
