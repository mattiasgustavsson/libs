/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

palettize.h - v0.1 - Median-cut palette generation and remapping for C/C++.

Do this:
	#define PALETTIZE_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef palettize_h
#define palettize_h


#ifndef PALETTIZE_U32
	#define PALETTIZE_U32 unsigned int
#endif

#ifndef PALETTIZE_U16
	#define PALETTIZE_U16 unsigned short
#endif

#ifndef PALETTIZE_U8
	#define PALETTIZE_U8 unsigned char
#endif


int palettize_generate_palette_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, 
    PALETTIZE_U32* palette, int palette_size, void* memctx );

void palettize_remap_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, 
    PALETTIZE_U32 const* palette, int palette_size, PALETTIZE_U8* output );


int palettize_generate_palette_rgb16( PALETTIZE_U16 const* rgb, int width, int height, 
    PALETTIZE_U16* palette, int palette_size, void* memctx );

void palettize_remap_rgb16( PALETTIZE_U16 const* rgb, int width, int height, 
    PALETTIZE_U16 const* palette, int palette_size, PALETTIZE_U8* output );


int palettize_generate_rgb16_palette_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, 
    PALETTIZE_U16* palette, int palette_size, void* memctx );

void palettize_remap_xbgr32_pal16( PALETTIZE_U32 const* xbgr, int width, int height, 
    PALETTIZE_U16 const* palette, int palette_size, PALETTIZE_U8* output );


#endif /* palettize_h */

/**

Example
=======

An example which loads a bitmap, generate a palette from it, and remap the image to that palette:

    #define PALETTIZE_IMPLEMENTATION
    #include "palettize.h

    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"

    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image_write.h"

	int main( int argc, char** argv )
		{
		(void) argc, argv;
    
        int w, h, c;
        PALETTIZE_U32* img = (PALETTIZE_U32*) stbi_load( "test.png", &w, &h, &c, 4 );
    
        PALETTIZE_U32 pal[ 256 ];
        int pal_count = palettize_generate_palette_xbgr32( img, w, h, pal, 256, 0 );
    
        PALETTIZE_U8* palimg = (PALETTIZE_U8*) malloc( w * h );
        palettize_remap_xbgr32( img, w, h, pal, pal_count, palimg );
    
        for( int i = 0; i < w * h; ++i ) img[ i ] = pal[ palimg[ i ] ];
        stbi_write_png( "palettized.png", w, h, 4, img, w * 4 );
        free( palimg );
        stbi_image_free( img );
		}

	// pass-through so the program will build with either /SUBSYSTEM:WINDOWS or /SUBSYSTEN:CONSOLE
	extern "C" int __stdcall WinMain( struct HINSTANCE__*, struct HINSTANCE__*, char*, int ) { return main( __argc, __argv ); }

**/



/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifdef PALETTIZE_IMPLEMENTATION
#undef PALETTIZE_IMPLEMENTATION


#ifndef PALETTIZE_MALLOC
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
	#include <stdlib.h>
	#if defined(__cplusplus)
		#define PALETTIZE_MALLOC( ctx, size ) ( ::malloc( size ) )
		#define PALETTIZE_FREE( ctx, ptr ) ( ::free( ptr ) )
	#else
		#define PALETTIZE_MALLOC( ctx, size ) ( malloc( size ) )
		#define PALETTIZE_FREE( ctx, ptr ) ( free( ptr ) )
	#endif
#endif


typedef struct palettize_internal_rgb_t { PALETTIZE_U8 rgb[ 3 ]; } palettize_internal_rgb_t;


typedef struct palettize_internal_bucket_t
    {
    palettize_internal_rgb_t* points;
    int points_count;
    palettize_internal_rgb_t min_val;
    palettize_internal_rgb_t max_val;
    } palettize_internal_bucket_t;


static void palettize_internal_bucket_init( palettize_internal_bucket_t* bucket, palettize_internal_rgb_t* points, 
    int points_count )
    {
    bucket->points = points;
    bucket->points_count = points_count;

    for( int j = 0; j < 3; ++j ) bucket->min_val.rgb[ j ] = bucket->max_val.rgb[ j ] = bucket->points[ 0 ].rgb[ j ];

    for( int i = 1; i < bucket->points_count; ++i )
        {
        for( int j = 0; j < 3; ++j )
            {
            bucket->min_val.rgb[ j ] = bucket->min_val.rgb[ j ] < bucket->points[ i ].rgb[ j ] ? 
                bucket->min_val.rgb[ j ] : bucket->points[ i ].rgb[ j ];
            bucket->max_val.rgb[ j ] = bucket->max_val.rgb[ j ] > bucket->points[ i ].rgb[ j ] ? 
                bucket->max_val.rgb[ j ] : bucket->points[ i ].rgb[ j ];
            }
        }
    }


static int palettize_internal_bucket_largest_range( palettize_internal_bucket_t const* bucket )
    {
    int m = bucket->max_val.rgb[ 0 ] - bucket->min_val.rgb[ 0 ];
    int max_index = 0;
    for( int i = 1; i < 3; ++i )
        {
        int diff = bucket->max_val.rgb[ i ] - bucket->min_val.rgb[ i ];
        if( diff > m ) { m = diff; max_index = i; }
        }
    return max_index;
    }


static int palettize_internal_bucket_compare( palettize_internal_bucket_t const* a, 
    palettize_internal_bucket_t const* b ) 
    {
    int i = palettize_internal_bucket_largest_range( a );
    int j = palettize_internal_bucket_largest_range( b );
    int ra = a->max_val.rgb[ i ] - a->min_val.rgb[ i ];
    int rb = b->max_val.rgb[ j ] - b->min_val.rgb[ j ];
    return ra < rb ? -1 : ra > rb ? 1 : 0; 
    }


typedef struct palettize_internal_priority_queue_t
    {
    palettize_internal_bucket_t* elements;
    int capacity;
    int count;
    } palettize_internal_priority_queue_t;


static void priority_queue_push( palettize_internal_priority_queue_t* queue, palettize_internal_bucket_t const* value )
    {
    PALDITHER_ASSERT( queue->count < queue->capacity, "Priority queue overflow" );
    if( queue->count >= queue->capacity ) return;
    queue->elements[ queue->count++ ] = *value;
	int index = queue->count;
	while( index > 1 && 
        palettize_internal_bucket_compare( &queue->elements[ index - 1 ], &queue->elements[ index / 2 - 1 ] ) > 0 )
		{
		palettize_internal_bucket_t temp = queue->elements[ index / 2 - 1 ];
		queue->elements[ index / 2 - 1 ] = queue->elements[ index - 1 ];
		queue->elements[ index - 1 ] = temp;
		index /=2;
		}
    }


static void priority_queue_pop( palettize_internal_priority_queue_t* queue )
    {
	if( queue->count == 0 ) return;
    queue->elements[ 0 ] = queue->elements[ --queue->count ];
	int v = 1;
	int u = 0;
	while( u != v)
		{
		u = v;		
		if( 2 * u + 1 <= queue->count ) 
			{
			if( palettize_internal_bucket_compare( &queue->elements[ u - 1 ], &queue->elements[ 2 * u - 1 ] ) <= 0 ) 
                v = 2 * u;
			if( palettize_internal_bucket_compare( &queue->elements[ v - 1 ], &queue->elements[ 2 * u + 1 - 1 ] ) <= 0 ) 
                v = 2 * u + 1;
			}
		else if ( 2 * u <= queue->count )
			{			
			if( palettize_internal_bucket_compare( &queue->elements[ u - 1 ], &queue->elements[ 2 * u - 1 ] ) <= 0 ) 
                v = 2 * u;
			}

		if( u != v )
			{
			palettize_internal_bucket_t temp = queue->elements[ u - 1 ];
			queue->elements[ u - 1 ] = queue->elements[ v - 1 ];
			queue->elements[ v - 1 ] = temp;
			}
		}
	}

	
static void palettize_internal_nth_element(palettize_internal_rgb_t* first, palettize_internal_rgb_t* nth, 
    palettize_internal_rgb_t* last, int index) 
    {
    while( last - first > 3 ) 
        {
        palettize_internal_rgb_t const* a = first; 
        palettize_internal_rgb_t const* b = first + ( last - first ) / 2;
        palettize_internal_rgb_t const* c = last - 1;
        palettize_internal_rgb_t const* pivot;
        if( a->rgb[ index ] < b->rgb[ index ] )
            pivot = b->rgb[ index ] < c->rgb[ index ] ? b : a->rgb[ index ] < c->rgb[ index ] ? c : a;
        else 
            pivot = a->rgb[ index ] < c->rgb[ index ]  ? a : b->rgb[ index ] < c->rgb[ index ] ? c : b;
   
        palettize_internal_rgb_t* part_first = first;
        palettize_internal_rgb_t* part_last = last;
        palettize_internal_rgb_t* cut;
        for( ; ; ) 
            {
            while( part_first->rgb[ index ] < pivot->rgb[ index ] ) ++part_first;
            --part_last;
            while( pivot->rgb[ index ] < part_last->rgb[ index ] ) --part_last;
            if( part_first >= part_last ) { cut = part_first; break; }  
            palettize_internal_rgb_t tmp = *part_first; *part_first = *part_last; *part_last = tmp;
            ++part_first;
            }  
        if( cut <= nth ) first = cut; else  last = cut;
        }

    if( first == last ) return;
    for( palettize_internal_rgb_t* i = first + 1; i != last; ++i ) 
        {
        if( i->rgb[ index ] < first->rgb[ index ] ) 
            {
            palettize_internal_rgb_t* copy_last = i;
            palettize_internal_rgb_t* copy_result = i + 1;
            while( first != copy_last ) *( --copy_result ) = *( --copy_last );
            *first = *i;
            }
        else 
            {
            palettize_internal_rgb_t* ins_last = i;
            palettize_internal_rgb_t* next = ins_last;
            --next;  
            while( i->rgb[ index ] < next->rgb[ index ] ) { *ins_last = *next; ins_last = next; --next; }
            *ins_last = *i;
            }
        }
    }


static int palettize_internal_median_cut(palettize_internal_rgb_t* image, int points_count, 
    palettize_internal_bucket_t* work_mem, int desired_size,  palettize_internal_rgb_t* output )
    {
    palettize_internal_priority_queue_t bucket_queue;
    bucket_queue.count = 0;
    bucket_queue.capacity = desired_size;
    bucket_queue.elements = work_mem;

    palettize_internal_bucket_t initial_bucket;
    palettize_internal_bucket_init( &initial_bucket, image, points_count );

    priority_queue_push( &bucket_queue, &initial_bucket );
    while( bucket_queue.count > 0 && bucket_queue.elements[ 0 ].points_count > 1 )
        {
        if( bucket_queue.count >= desired_size ) break;
        palettize_internal_bucket_t largest_range_bucket = bucket_queue.elements[ 0 ];

        priority_queue_pop( &bucket_queue );
        palettize_internal_rgb_t * begin = largest_range_bucket.points;
	    palettize_internal_rgb_t * median = largest_range_bucket.points + ( largest_range_bucket.points_count + 1 ) / 2;
	    palettize_internal_rgb_t * end = largest_range_bucket.points + largest_range_bucket.points_count;

	    switch( palettize_internal_bucket_largest_range( &largest_range_bucket ) )
	        {
	        case 0: palettize_internal_nth_element( begin, median, end, 0 ); break;
	        case 1: palettize_internal_nth_element( begin, median, end, 1 ); break;
	        case 2: palettize_internal_nth_element( begin, median, end, 2 ); break;
	        }

	    palettize_internal_bucket_t bucket1;
        palettize_internal_bucket_init( &bucket1, begin, (int)( median-begin ) );
        priority_queue_push( &bucket_queue, &bucket1);

        palettize_internal_bucket_t bucket2;
        palettize_internal_bucket_init( &bucket2, median, (int)( end-median ) );
        priority_queue_push( &bucket_queue, &bucket2);
        }

    int result = 0;
    while( bucket_queue.count > 0 )
        {
        palettize_internal_bucket_t bucket = bucket_queue.elements[ 0 ];
        priority_queue_pop( &bucket_queue );

        int sum[ 3 ] = { 0, 0, 0 };
        for( int i = 0; i < bucket.points_count; ++i )
            for( int j = 0; j < 3; ++j )
                sum[ j ] += bucket.points[ i ].rgb[ j ];

        if( bucket.points_count )
            {
        palettize_internal_rgb_t average;
        for( int j = 0; j < 3; ++j ) average.rgb[ j ] = (PALETTIZE_U8) ( sum[ j ] / bucket.points_count );

        output[ result++ ] = average;
        }
        }

    return result;
    }


int palettize_generate_palette_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, 
    PALETTIZE_U32* palette, int palette_size, void* memctx )
	{
    (void) memctx;
	int pixel_count = width * height;

    int count = 0;
    for( int i = 0; i < pixel_count; ++i ) 
        {
        if( ( xbgr[ i ] >> 24 ) == 0 ) continue;
        int found = 0;
        for( int j = 0; j < count; ++j )
            {
            if( ( palette[ j ] & 0x00ffffff ) == ( xbgr[ i ] & 0x00ffffff ) )
                {
                found = 1;
                break;
                }
            }
        if( !found ) 
            {
            if( count >= palette_size ) goto skip;
            palette[ count++ ] = xbgr[ i ];
            }
        }

    return count;

skip: ;
    size_t malloc_size =  sizeof( palettize_internal_rgb_t ) * ( pixel_count + palette_size );
    malloc_size += sizeof( palettize_internal_bucket_t ) * palette_size;
	palettize_internal_rgb_t* data = (palettize_internal_rgb_t*) PALETTIZE_MALLOC( memctx, malloc_size );

    int data_size = 0;
	for( int p = 0; p < pixel_count; ++p )
		{
		PALETTIZE_U32 color = *xbgr++;
		PALETTIZE_U8 a = (PALETTIZE_U8 ) ( ( color & 0xff000000 ) >> 24 );
		if( a > 0 )
			{
			PALETTIZE_U8 b = (PALETTIZE_U8) ( ( color & 0x00ff0000 ) >> 16 );
			PALETTIZE_U8 g = (PALETTIZE_U8) ( ( color & 0x0000ff00 ) >> 8  );
			PALETTIZE_U8 r = (PALETTIZE_U8) ( ( color & 0x000000ff )       );
			data[ data_size ].rgb[ 0 ] = r;
			data[ data_size ].rgb[ 1 ] = g;
			data[ data_size ].rgb[ 2 ] = b;
			++data_size;
			}
		}
	palettize_internal_rgb_t* result = data + pixel_count;
    palettize_internal_bucket_t* work_mem = (palettize_internal_bucket_t*) ( data + pixel_count + palette_size );
	int result_count = palettize_internal_median_cut( data, data_size, work_mem, palette_size, result );
	
    count = 0;
	for( int i = 0; i < result_count; ++i )
		{
		palettize_internal_rgb_t p = result[ i ];
		PALETTIZE_U32 c = 0xff000000 | ( p.rgb[ 2 ] << 16 ) | ( p.rgb[ 1 ] << 8 ) | p.rgb[ 0 ];
		int found = 0;
		for( int j = 0; j < count; ++j ) if( palette[ j ] == c ) { found = 1; break; }
		if( !found ) palette[ count++ ] = c; 
        if( count >= palette_size ) break;
		}

	PALETTIZE_FREE( memctx, data );
	return count;
	}


void palettize_remap_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, 
    PALETTIZE_U32 const* palette, int palette_size, PALETTIZE_U8* output )
	{
    for( int y = 0; y < height;  ++y )
        {
		for( int x = 0; x < width; ++x )
            {
            PALETTIZE_U32 color = *xbgr++;
            int best_index = 0;
            int mindist_sq = 255 * 255 + 255 * 255 + 255 * 255 + 1;
            for( int i = 0; i < palette_size; ++i ) 
		        {
		        PALETTIZE_U8 cr = ( (PALETTIZE_U8) ( ( color & 0x00ff0000 ) >> 16 ) );
		        PALETTIZE_U8 cg = ( (PALETTIZE_U8) ( ( color & 0x0000ff00 ) >> 8  ) );
		        PALETTIZE_U8 cb = ( (PALETTIZE_U8) ( ( color & 0x000000ff )       )  );
		        PALETTIZE_U8 pr = ( (PALETTIZE_U8) ( ( palette[ i ] & 0x00ff0000 ) >> 16 ) );
		        PALETTIZE_U8 pg = ( (PALETTIZE_U8) ( ( palette[ i ] & 0x0000ff00 ) >> 8  ) );
		        PALETTIZE_U8 pb = ( (PALETTIZE_U8) ( ( palette[ i ] & 0x000000ff )       ) );
                int dr = ( (int) cr ) - ( (int) pr );
                int dg = ( (int) cg ) - ( (int) pg );
                int db = ( (int) cb ) - ( (int) pb );
                int dist_sq = dr * dr + dg * dg + db * db;
                if( dist_sq < mindist_sq ) { mindist_sq = dist_sq; best_index = i; }
		        }
			*output++ = (PALETTIZE_U8) best_index;
            }
        }
    }


static PALETTIZE_U16 palettize_internal_rgb32_to_rgb16( PALETTIZE_U32 color )
	{
	return ((PALETTIZE_U16) ( ((color & 0x00f80000)>>8) | ((color & 0x0000fc00)>>5) | ((color & 0x000000f8)>>3) ));
	}


static PALETTIZE_U32 palettize_internal_rgb16_to_rgb32( PALETTIZE_U16 color )
	{
	return (((PALETTIZE_U32)color & 0xf800)<<8) | (((PALETTIZE_U32)color & 0x07e0)<<5) | 
        (((PALETTIZE_U32)color & 0x001f)<<3) | (0xff000000);
	}


int palettize_generate_palette_rgb16( PALETTIZE_U16 const* rgb, int width, int height, 
    PALETTIZE_U16* palette, int palette_size, void* memctx )
	{
    (void) memctx;
	int pixel_count = width * height;

    int count = 0;
    for( int i = 0; i < pixel_count; ++i ) 
        {
        int found = 0;
        for( int j = 0; j < count; ++j )
            {
            if( ( palette[ j ] ) == ( rgb[ i ] & 0x00ffffff ) )
                {
                found = 1;
                break;
                }
            }
        if( !found ) 
            {
            if( count >= palette_size ) goto skip;
            palette[ count++ ] = rgb[ i ];
            }
        }

    return count;

skip: ;
    size_t malloc_size =  sizeof( palettize_internal_rgb_t ) * ( pixel_count + palette_size );
    malloc_size += sizeof( palettize_internal_bucket_t ) * palette_size;
	palettize_internal_rgb_t* data = (palettize_internal_rgb_t*) PALETTIZE_MALLOC( memctx, malloc_size );

    int data_size = 0;
	for( int p = 0; p < pixel_count; ++p )
		{
		PALETTIZE_U32 color = palettize_internal_rgb16_to_rgb32( *rgb++ );
		PALETTIZE_U8 a = (PALETTIZE_U8) ( ( color & 0xff000000 ) >> 24 );
		if( a > 0 )
			{
			PALETTIZE_U8 b = (PALETTIZE_U8) ( ( color & 0x00ff0000 ) >> 16 );
			PALETTIZE_U8 g = (PALETTIZE_U8) ( ( color & 0x0000ff00 ) >> 8  );
			PALETTIZE_U8 r = (PALETTIZE_U8) ( ( color & 0x000000ff )       );
			data[ data_size ].rgb[ 0 ] = r;
			data[ data_size ].rgb[ 1 ] = g;
			data[ data_size ].rgb[ 2 ] = b;
			++data_size;
			}
		}

	palettize_internal_rgb_t* result = data + pixel_count;
    palettize_internal_bucket_t* work_mem = (palettize_internal_bucket_t*) ( data + pixel_count + palette_size );
	int result_count = palettize_internal_median_cut( data, data_size, work_mem, palette_size, result );

    count = 0;
	for( int i = 0; i < result_count; ++i )
		{
		palettize_internal_rgb_t p = result[ i ];
		PALETTIZE_U16 c = palettize_internal_rgb32_to_rgb16( 
			(PALETTIZE_U32)( ( p.rgb[ 2 ] << 16 ) | ( p.rgb[ 1 ] << 8 ) | p.rgb[ 0 ] ) );
		int found = 0;
		for( int j = 0; j < count; ++j ) if( palette[ j ] == c ) { found = 1; break; }
		if( !found ) palette[ count++ ] = c;
        if( count >= palette_size ) break;
		}

	PALETTIZE_FREE( memctx, data );
	return count;
	}


void palettize_remap_rgb16( PALETTIZE_U16 const* rgb, int width, int height, 
    PALETTIZE_U16 const* palette, int palette_size, PALETTIZE_U8* output )
	{
    for( int y = 0; y < height;  ++y )
        {
		for( int x = 0; x < width; ++x )
            {
            PALETTIZE_U32 color = palettize_internal_rgb16_to_rgb32( *rgb++ );
	        int best_index = 0;
            int mindist_sq = 255 * 255 + 255 * 255 + 255 * 255 + 1;
            for( int i = 0; i < palette_size; ++i ) 
		        {
		        PALETTIZE_U8 cr = ( (PALETTIZE_U8) ( ( color & 0x00ff0000 ) >> 16 ) );
		        PALETTIZE_U8 cg = ( (PALETTIZE_U8) ( ( color & 0x0000ff00 ) >> 8  ) );
		        PALETTIZE_U8 cb = ( (PALETTIZE_U8) ( ( color & 0x000000ff )       ) );
                PALETTIZE_U32 pal = palettize_internal_rgb16_to_rgb32( palette[ i ] );
		        PALETTIZE_U8 pr = ( (PALETTIZE_U8) ( ( pal & 0x00ff0000 ) >> 16 ) );
		        PALETTIZE_U8 pg = ( (PALETTIZE_U8) ( ( pal & 0x0000ff00 ) >> 8  ) );
		        PALETTIZE_U8 pb = ( (PALETTIZE_U8) ( ( pal & 0x000000ff )       ) );
                int dr = ( (int) cr) - ( (int) pr );
                int dg = ( (int) cg) - ( (int) pg );
                int db = ( (int) cb) - ( (int) pb );
                int dist_sq = dr * dr + dg * dg + db * db;
                if( dist_sq < mindist_sq )  { mindist_sq = dist_sq; best_index = i; }
		        }
			*output++ = (PALETTIZE_U8) best_index;            
            }
        }
	}


int palettize_generate_rgb16_palette_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, 
    PALETTIZE_U16* palette, int palette_size, void* memctx )
    {
    (void) memctx;
	int pixel_count = width * height;

    int count = 0;
    for( int i = 0; i < pixel_count; ++i ) 
        {
        if( ( xbgr[ i ] >> 24 ) == 0 ) continue;
		PALETTIZE_U16 c = palettize_internal_rgb32_to_rgb16( xbgr[ i ] );
        int found = 0;
        for( int j = 0; j < count; ++j )
            {
            if( palette[ j ] == c )
                {
                found = 1;
                break;
                }
            }
        if( !found ) 
            {
            if( count >= palette_size ) goto skip;
            palette[ count++ ] = c;
            }
        }

    return count;

skip: ;
    size_t malloc_size =  sizeof( palettize_internal_rgb_t ) * ( pixel_count + palette_size );
    malloc_size += sizeof( palettize_internal_bucket_t ) * palette_size;
	palettize_internal_rgb_t* data = (palettize_internal_rgb_t*) PALETTIZE_MALLOC( memctx,  malloc_size );

    int data_size = 0;
	for( int p = 0; p < pixel_count; ++p )
		{
		PALETTIZE_U32 color = *xbgr++;
		PALETTIZE_U8 a = (PALETTIZE_U8) ( ( color & 0xff000000 ) >>24 );
		if( a > 0 )
			{
			color = palettize_internal_rgb16_to_rgb32( palettize_internal_rgb32_to_rgb16( color ) );
			PALETTIZE_U8 b = (PALETTIZE_U8) ( ( color & 0x00ff0000 ) >> 16 );
			PALETTIZE_U8 g = (PALETTIZE_U8) ( ( color & 0x0000ff00 ) >> 8  );
			PALETTIZE_U8 r = (PALETTIZE_U8) ( ( color & 0x000000ff )       );
			data[ data_size ].rgb[ 0 ] = r;
			data[ data_size ].rgb[ 1 ] = g;
			data[ data_size ].rgb[ 2 ] = b;
			++data_size;
			}
		}
	palettize_internal_rgb_t* result = data + pixel_count;
    palettize_internal_bucket_t* work_mem = (palettize_internal_bucket_t*) ( data + pixel_count + palette_size );
	int result_count = palettize_internal_median_cut( data, data_size, work_mem, palette_size, result );
	
    count = 0;
	for( int i = 0; i < result_count; ++i )
		{
		palettize_internal_rgb_t p = result[ i ];
		PALETTIZE_U16 c = palettize_internal_rgb32_to_rgb16( 
			(PALETTIZE_U32)( ( p.rgb[ 2 ] << 16 ) | ( p.rgb[ 1 ] << 8 ) | p.rgb[ 0 ] ) );
		int found = 0;
		for( int j= 0; j < count; ++j ) if( palette[ j ] == c ) { found = 1; break; }
		if( !found ) palette[ count++ ] = c;
        if( count >= palette_size ) break;
		}

	PALETTIZE_FREE( memctx, data );
	return count;
    }


void palettize_remap_xbgr32_pal16( PALETTIZE_U32 const* xbgr, int width, int height, 
    PALETTIZE_U16 const* palette, int palette_size, PALETTIZE_U8* output )
    {
    for( int y = 0; y < height;  ++y )
        {
		for( int x = 0; x < width; ++x )
            {
            PALETTIZE_U32 color = *xbgr++;
            int best_index = 0;
            int mindist_sq = 255 * 255 + 255 * 255 + 255 * 255 + 1;
            for( int i = 0; i < palette_size; ++i ) 
		        {
		        PALETTIZE_U8 cr = ( (PALETTIZE_U8) ( ( color & 0x00ff0000 ) >> 16 ) );
		        PALETTIZE_U8 cg = ( (PALETTIZE_U8) ( ( color & 0x0000ff00 ) >> 8  ) );
		        PALETTIZE_U8 cb = ( (PALETTIZE_U8) ( ( color & 0x000000ff )       ) );
                PALETTIZE_U32 pal = palettize_internal_rgb16_to_rgb32( palette[ i ] );
		        PALETTIZE_U8 pr = ( (PALETTIZE_U8) ( ( pal & 0x00ff0000 ) >> 16) );
		        PALETTIZE_U8 pg = ( (PALETTIZE_U8) ( ( pal & 0x0000ff00 ) >> 8 ) );
		        PALETTIZE_U8 pb = ( (PALETTIZE_U8) ( ( pal & 0x000000ff )      ) );
                int dr = ( (int) cr) - ( (int) pr );
                int db = ( (int) cb) - ( (int) pb );
                int dg = ( (int) cg) - ( (int) pg );
                int dist_sq = dr * dr + dg * dg + db * db;
                if( dist_sq < mindist_sq ) { mindist_sq = dist_sq; best_index = i; }
		        }
			*output++ = (PALETTIZE_U8) best_index;
            }
        }
    }

#endif /* PALETTIZE_IMPLEMENTATION */


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
