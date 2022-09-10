/*
------------------------------------------------------------------------------
		  Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

paldither.h - v0.1 - Convert true-color image to custom palette, with dither.

Do this:
	#define PALDITHER_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef paldither_h
#define paldither_h

#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#include <stddef.h>

#ifndef PALDITHER_U32
	#define PALDITHER_U32 unsigned int
#endif

#ifndef PALDITHER_U8
	#define PALDITHER_U8 unsigned char
#endif


typedef enum paldither_type_t
	{
	PALDITHER_TYPE_DEFAULT,
	PALDITHER_TYPE_BAYER,
	PALDITHER_TYPE_NONE,
	} paldither_type_t;


typedef struct paldither_palette_t
	{
	void* memctx;
	int color_count;
	PALDITHER_U32 colortable[ 256 ];
	} paldither_palette_t;

paldither_palette_t* paldither_palette_create( PALDITHER_U32 const* xbgr, int count, size_t* palette_size, void* memctx );
paldither_palette_t* paldither_palette_create_from_data( void const* data, size_t size, void* memctx );

void paldither_palette_destroy( paldither_palette_t* palette );

void paldither_palettize( PALDITHER_U32* abgr, int width, int height, paldither_palette_t const* palette, 
	paldither_type_t dither_type, PALDITHER_U8* output );

#endif /* paldither_h */


/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifdef PALDITHER_IMPLEMENTATION
#undef PALDITHER_IMPLEMENTATION

#ifndef PALDITHER_MALLOC
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <stdlib.h>
	#if defined(__cplusplus)
		#define PALDITHER_MALLOC( ctx, size ) ( ::malloc( size ) )
		#define PALDITHER_FREE( ctx, ptr ) ( ::free( ptr ) )
	#else
		#define PALDITHER_MALLOC( ctx, size ) ( malloc( size ) )
		#define PALDITHER_FREE( ctx, ptr ) ( free( ptr ) )
	#endif
#endif

#ifndef PALDITHER_ASSERT
	#undef _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#undef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#include <assert.h>
	#define PALDITHER_ASSERT( expression, message ) assert( ( expression ) && ( message ) )
#endif

#include <stdlib.h>


typedef struct paldither_mix_t
	{
	int first;
	int second;
	int d;
	unsigned char ratio;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char l;
	} paldither_mix_t;


static void paldither_internal_mix( void* memctx, PALDITHER_U32 const* xbgr, int count, int mix_levels,
	paldither_mix_t** out_mix, int* out_mix_count )
	{
	(void) memctx;

	int mix_count = ( ( count / 2 ) * ( count + 1 ) ) * mix_levels;
	paldither_mix_t* mix = (paldither_mix_t*) PALDITHER_MALLOC( memctx, mix_count * sizeof( paldither_mix_t ) );

	int c = 0;
	for( int i = 0; i < count; ++i )
		{
		PALDITHER_U32 fcolor = xbgr[ i ] & 0x00ffffff;
		int fr = (int)( ( fcolor & 0x000000ff ) );
		int fg = (int)( ( fcolor & 0x0000ff00 ) >> 8 );
		int fb = (int)( ( fcolor & 0x00ff0000 ) >> 16 );                
		int fl = (int)( ( 54 * fr + 183 * fg + 19 * fb + 127 ) >> 8 );
		PALDITHER_ASSERT( fl <= 0xff && fl >= 0, "Value out of range" );          

		for( int j = i + 1; j < count; ++j )	
			{
			PALDITHER_U32 scolor = xbgr[ j ] & 0x00ffffff;
			int sr = (int)( ( scolor & 0x000000ff ) );
			int sg = (int)( ( scolor & 0x0000ff00 ) >> 8 );
			int sb = (int)( ( scolor & 0x00ff0000 ) >> 16 );                
			int sl = (int)( ( 54 * sr + 183 * sg + 19 * sb + 127 ) >> 8 );
			PALDITHER_ASSERT( sl <= 0xff && sl >= 0, "Value out of range" );                   

			int dr = fr - sr;
			int dg = fg - sg;
			int db = fb - sb;
			int dl = fl - sl;
			int d = ( ( ( ( dr*dr + dg*dg + db*db ) >> 1 ) + dl*dl ) + 127 ) >> 8;
	   
			for( int k = 0; k < mix_levels; ++k )	
				{
				int r = fr;
				int g = fg;
				int b = fb;
				int l = fl;
				if( mix_levels > 1 )
					{
					int s = mix_levels - 1;
					int ik = s - k;                    
					r = ( r * ik + sr * k + ( mix_levels / 2 ) ) / s;
					g = ( g * ik + sg * k + ( mix_levels / 2 ) ) / s;
					b = ( b * ik + sb * k + ( mix_levels / 2 ) ) / s;
					l = ( 54 * r + 183 * g + 19 * b  + 127 ) >> 8;
					PALDITHER_ASSERT( r <= 0xff && g <= 0xff && b <= 0xff && r >= 0 && g >= 0 && b >= 0 && l <= 0xff && l >= 0, "Value out of range" );                   
					}

				mix[ c ].first = i;
				mix[ c ].second = j;
				mix[ c ].ratio = (unsigned char) k;
				mix[ c ].r = (unsigned char) r;
				mix[ c ].g = (unsigned char) g;
				mix[ c ].b = (unsigned char) b;
				mix[ c ].l = (unsigned char) l;
				mix[ c ].d = d;
				++c;
				PALDITHER_ASSERT( mix_count >= c, "Out of range" );
				}
			}
		}

	*out_mix = mix;
	*out_mix_count = c;
	}


static void paldither_internal_list( void* memctx, paldither_mix_t* mix, int mix_count,
	int out_map[ 16 * 16 * 16 ], int** out_list, int* out_list_count )
	{
	(void) memctx;
	int cube[ 17 * 17 * 17 ];	
	for( int r = 0; r < 17; ++r )
		{
		for( int g = 0; g < 17; ++g )
			{
			for( int b = 0; b < 17; ++b )
				{
				int best_diff = 0x7FFFFFFF;
				int best_mix = 0;
				paldither_mix_t const* m = mix;
				int l = ( 54 * (r * 16) + 183 * (g * 16) + 19 * (b * 16) + 127 ) >> 8;
				for( int i = 0; i < mix_count; ++i, ++m )
					{
					int dr = r * 16 - m->r;
					int dg = g * 16 - m->g;
					int db = b * 16 - m->b;
					int dl = l - m->l;
					int d = ( ( ( dr*dr + dg*dg + db*db ) >> 1 ) + dl*dl) + ( m->d * 3 );
					if( i == 0 || d < best_diff ) { best_diff = d; best_mix = i; }
					}
				cube[ r * 17 * 17 + g * 17 + b ] = best_mix;
				}
			}
		}

	int map[ 16 * 16 * 16 ];
	int list_capacity = mix_count * 256;
	int* list = (int*) PALDITHER_MALLOC( memctx, list_capacity * sizeof( int ) );
	int list_count = 0;
	
	int* mapptr = map;
	for( int r = 0; r < 16; ++r )
		{
		for( int g = 0; g < 16; ++g )
			{
			for( int b = 0; b < 16; ++b )
				{
				paldither_mix_t const* m = mix;
				*mapptr++ = list_count;
				if( list_count == list_capacity ) 
					{ 
					list_capacity *= 2; 
					int* new_list = (int*) PALDITHER_MALLOC( memctx, list_capacity * sizeof( int ) ); 
					memcpy( new_list, list, list_count * sizeof( int ) );
					PALDITHER_FREE( memctx, list );
					list = new_list; 
					}
				int count_index = list_count++;
				list[ count_index ] = 0;
				for( int i = 0; i < mix_count; ++i, ++m )
					{
					paldither_mix_t const* best_mix = &mix[ cube[ ( r ) * 17 * 17 + ( g ) * 17 + ( b ) ] ];
					paldither_mix_t const* best_mix2 = &mix[ cube[ ( r + 1 ) * 17 * 17 + ( g + 1 ) * 17 + ( b + 1 ) ] ];
					int pass = 				
					    ( m->r >= best_mix->r && m->r <= best_mix2->r )
					 && ( m->g >= best_mix->g && m->g <= best_mix2->g )
					 && ( m->b >= best_mix->b && m->b <= best_mix2->b );
					if( pass ) 
						{
						list[ count_index ]++;
						if( list_count == list_capacity ) 
							{ 
							list_capacity *= 2; 
							int* new_list = (int*) PALDITHER_MALLOC( memctx, list_capacity * sizeof( int ) ); 
							memcpy( new_list, list, list_count * sizeof( int ) );
							PALDITHER_FREE( memctx, list );
							list = new_list; 
							}
						list[ list_count++ ] = i;
						}
					
					}
				}
			}
		}

	memcpy( out_map, map, sizeof( map ) );
	*out_list = list;
	*out_list_count = list_count;
	}


paldither_palette_t* paldither_palette_create( PALDITHER_U32 const* xbgr, int count, size_t* palette_size, void* memctx )
	{	
	(void) memctx;

	int default_mix_count;
	paldither_mix_t* default_mix;
	paldither_internal_mix( memctx, xbgr, count, 11, &default_mix, &default_mix_count );

	int default_map[ 16 * 16 * 16 ];
	int* default_list;
	int default_list_count = 0;
	paldither_internal_list( memctx, default_mix, default_mix_count, default_map, &default_list, &default_list_count );
	
	int bayer_mix_count;
	paldither_mix_t* bayer_mix;
	paldither_internal_mix( memctx, xbgr, count, 17, &bayer_mix, &bayer_mix_count );

	int bayer_map[ 16 * 16 * 16 ];
	int* bayer_list;
	int bayer_list_count = 0;
	paldither_internal_list( memctx, bayer_mix, bayer_mix_count, bayer_map, &bayer_list, &bayer_list_count );

	int nodither_mix_count;
	paldither_mix_t* nodither_mix;
	paldither_internal_mix( memctx, xbgr, count, 1, &nodither_mix, &nodither_mix_count );

	int nodither_map[ 16 * 16 * 16 ];
	int* nodither_list;
	int nodither_list_count = 0;
	paldither_internal_list( memctx, nodither_mix, nodither_mix_count, nodither_map, &nodither_list, &nodither_list_count );

	size_t size = sizeof( paldither_palette_t ) + 
		sizeof( int ) + default_list_count * sizeof( int ) + 
		sizeof( int ) + default_mix_count * sizeof( paldither_mix_t ) + 
		sizeof( default_map ) +
		sizeof( int ) + bayer_list_count * sizeof( int ) + 
		sizeof( int ) + bayer_mix_count * sizeof( paldither_mix_t ) + 
		sizeof( bayer_map ) +
		sizeof( int ) + nodither_list_count * sizeof( int ) + 
		sizeof( int ) + nodither_mix_count * sizeof( paldither_mix_t ) + 
		sizeof( nodither_map );	
	paldither_palette_t* palette = (paldither_palette_t*) PALDITHER_MALLOC( memctx, size );
	palette->memctx = memctx;
	
	palette->color_count = count;
	memcpy( palette->colortable, xbgr, sizeof( *xbgr ) * count );
	
	uintptr_t dest = (uintptr_t)( palette + 1) ;

	*(int*) dest = default_mix_count; dest += sizeof( int );
	memcpy( (void*)dest, default_mix, default_mix_count * sizeof( paldither_mix_t ) );
	dest += default_mix_count * sizeof( paldither_mix_t );

	memcpy( (void*)dest, default_map, sizeof( default_map ) );
	dest += sizeof( default_map );

	*(int*) dest = default_list_count; dest += sizeof( int );
	memcpy( (void*)dest, default_list, default_list_count * sizeof( int ) );
	dest += default_list_count * sizeof( int );

	PALDITHER_FREE( memctx, default_list );
	PALDITHER_FREE( memctx, default_mix );
	
	*(int*) dest = bayer_mix_count; dest += sizeof( int );
	memcpy( (void*)dest, bayer_mix, bayer_mix_count * sizeof( paldither_mix_t ) );
	dest += bayer_mix_count * sizeof( paldither_mix_t );

	memcpy( (void*)dest, bayer_map, sizeof( bayer_map ) );
	dest += sizeof( bayer_map );

	*(int*) dest = bayer_list_count; dest += sizeof( int );
	memcpy( (void*)dest, bayer_list, bayer_list_count * sizeof( int ) );
	dest += bayer_list_count * sizeof( int );

	PALDITHER_FREE( memctx, bayer_list );
	PALDITHER_FREE( memctx, bayer_mix );

	*(int*) dest = nodither_mix_count; dest += sizeof( int );
	memcpy( (void*)dest, nodither_mix, nodither_mix_count * sizeof( paldither_mix_t ) );
	dest += nodither_mix_count * sizeof( paldither_mix_t );

	memcpy( (void*)dest, nodither_map, sizeof( nodither_map ) );
	dest += sizeof( nodither_map );

	*(int*) dest = nodither_list_count; dest += sizeof( int );
	memcpy( (void*)dest, nodither_list, nodither_list_count * sizeof( int ) );
	dest += nodither_list_count * sizeof( int );

	PALDITHER_FREE( memctx, nodither_list );
	PALDITHER_FREE( memctx, nodither_mix );
	
	if( palette_size ) *palette_size = size;
	return palette;
	}

	
paldither_palette_t* paldither_palette_create_from_data( void const* data, size_t size, void* memctx )
	{
	paldither_palette_t* palette = (paldither_palette_t*)PALDITHER_MALLOC( memctx, size );
	palette->memctx = memctx;
	memcpy( &palette->color_count, data, size );
	return palette;
	}


void paldither_palette_destroy( paldither_palette_t* palette )
	{
	PALDITHER_FREE( palette->memctx, palette );
	}

	
void paldither_palettize( PALDITHER_U32* abgr, int width, int height, paldither_palette_t const* palette, 
	paldither_type_t dither_type, PALDITHER_U8* output )
	{
	unsigned char default_dither_pattern[ 4 * 4 * 11 ] = 
	    {
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,

	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,

	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,

	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,1,

	    1,0,1,0,
	    0,1,0,0,
	    1,0,1,0,
	    0,0,0,1,

	    1,0,1,0,
	    0,1,0,1,
	    1,0,1,0,
	    0,1,0,1,

	    1,0,1,0,
	    1,1,0,1,
	    1,0,1,0,
	    0,1,1,1,

	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,

	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,

	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,

	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,

	   };

	unsigned char bayer_dither_pattern[ 4 * 4 * 17 ] = 
	    {
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,

		1, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,

		1, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 0,

		1, 0, 1, 0,
		0, 0, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 0,

		1, 0, 1, 0,
		0, 0, 0, 0,
		1, 0, 1, 0,
		0, 0, 0, 0,

		1, 0, 1, 0,
		0, 1, 0, 0,
		1, 0, 1, 0,
		0, 0, 0, 0,

		1, 0, 1, 0,
		0, 1, 0, 0,
		1, 0, 1, 0,
		0, 0, 0, 1,

		1, 0, 1, 0,
		0, 1, 0, 1,
		1, 0, 1, 0,
		0, 0, 0, 1,

		1, 0, 1, 0,
		0, 1, 0, 1,
		1, 0, 1, 0,
		0, 1, 0, 1,

		1, 1, 1, 0,
		0, 1, 0, 1,
		1, 0, 1, 0,
		0, 1, 0, 1,

		1, 1, 1, 0,
		0, 1, 0, 1,
		1, 0, 1, 1,
		0, 1, 0, 1,

		1, 1, 1, 1,
		0, 1, 0, 1,
		1, 0, 1, 1,
		0, 1, 0, 1,

		1, 1, 1, 1,
		0, 1, 0, 1,
		1, 1, 1, 1,
		0, 1, 0, 1,

		1, 1, 1, 1,
		1, 1, 0, 1,
		1, 1, 1, 1,
		0, 1, 0, 1,

		1, 1, 1, 1,
		1, 1, 0, 1,
		1, 1, 1, 1,
		0, 1, 1, 1,

		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,
		0, 1, 1, 1,

		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,
		};

	unsigned char none_dither_pattern[ 4 * 4 * 2 ] = 
	    {
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		};


	uintptr_t ptr = (uintptr_t)( palette + 1 );

	int default_pal_mix_count = *(int*)ptr; ptr += sizeof( int );
	paldither_mix_t const* default_pal_mix = (paldither_mix_t*) ptr; ptr += default_pal_mix_count * sizeof( paldither_mix_t );
	int const* default_pal_map = (int*) ptr; ptr += 16 * 16 * 16 * sizeof( int );
	int default_pal_list_count = *(int*)ptr; ptr += sizeof( int );
	int const* default_pal_list = (int*) ptr; ptr += default_pal_list_count * sizeof( int );   
	
	int bayer_pal_mix_count = *(int*)ptr; ptr += sizeof( int );
	paldither_mix_t const* bayer_pal_mix = (paldither_mix_t*) ptr; ptr += bayer_pal_mix_count * sizeof( paldither_mix_t );
	int const* bayer_pal_map = (int*) ptr; ptr += 16 * 16 * 16 * sizeof( int );
	int bayer_pal_list_count = *(int*)ptr; ptr += sizeof( int );
	int const* bayer_pal_list = (int*) ptr; ptr += bayer_pal_list_count * sizeof( int );

	int nodither_pal_mix_count = *(int*)ptr; ptr += sizeof( int );
	paldither_mix_t const* nodither_pal_mix = (paldither_mix_t*) ptr; ptr += nodither_pal_mix_count * sizeof( paldither_mix_t );
	int const* nodither_pal_map = (int*) ptr; ptr += 16 * 16 * 16 * sizeof( int );
	int nodither_pal_list_count = *(int*)ptr; ptr += sizeof( int );
	int const* nodither_pal_list = (int*) ptr; ptr += nodither_pal_list_count * sizeof( int );
	
	unsigned char* dither_pattern = default_dither_pattern;
	int pal_mix_count = default_pal_mix_count;
	paldither_mix_t const* pal_mix = default_pal_mix;
	int const* pal_map = default_pal_map;
	int const* pal_list = default_pal_list;   

	if( dither_type == PALDITHER_TYPE_BAYER )
		{
		dither_pattern = bayer_dither_pattern;
		pal_mix_count = bayer_pal_mix_count;
	    pal_mix = bayer_pal_mix;
	    pal_map = bayer_pal_map;
	    pal_list = bayer_pal_list;   
		}
	else if( dither_type == PALDITHER_TYPE_NONE )
		{
		dither_pattern = none_dither_pattern;
		pal_mix_count = nodither_pal_mix_count;
	    pal_mix = nodither_pal_mix;
	    pal_map = nodither_pal_map;
	    pal_list = nodither_pal_list;   
		}

	for( int y = 0; y < height; ++y )
		{
		for( int x = 0; x < width; ++x )	
			{
			PALDITHER_U32 color = abgr[ x + y * width ];
			int r = (int)( ( color & 0x000000ff ) );
			int g = (int)( ( color & 0x0000ff00 ) >> 8 );
			int b = (int)( ( color & 0x00ff0000 ) >> 16 );                
			int l = (int)( ( 54 * r + 183 * g + 19 * b + 127 ) >> 8 );
			PALDITHER_ASSERT( l <= 0xff && l >= 0, "Value out of range" );           

			paldither_mix_t const* best_mix = 0;
			int pal_index = pal_map[ ( r >> 4 ) * 16 * 16 + ( g >> 4 ) * 16 + ( b >> 4 ) ];
			int count = pal_list[ pal_index++ ];
			if( count != 0 )
				{
				int best_diff = 0x7FFFFFFF;
				int const* index = &pal_list[ pal_index ];
				for( int i = 0; i < count; ++i, ++index )
					{
					paldither_mix_t const* m = &pal_mix[ *index ];
					int dr = r - m->r;
					int dg = g - m->g;
					int db = b - m->b;
					int dl = l - m->l;
					int d = ( ( ( dr*dr + dg*dg + db*db ) >> 1 ) + dl*dl) + ( m->d * 3 );
					if( i == 0 || d < best_diff ) { best_diff = d; best_mix = m; }
					}
				}
			else
				{
				int best_diff = 0x7FFFFFFF;
				paldither_mix_t const* m = pal_mix;
				for( int i = 0; i < pal_mix_count; ++i, ++m )
					{
					int dr = r - m->r;
					int dg = g - m->g;
					int db = b - m->b;
					int dl = l - m->l;
					int d = ( ( ( dr*dr + dg*dg + db*db ) >> 1 ) + dl*dl) + ( m->d * 3 );
					if( i == 0 || d < best_diff ) { best_diff = d; best_mix = m; }
					}
				}

			int index = dither_pattern[ best_mix->ratio * 4 * 4 + ( x & 3 ) + ( y & 3 ) * 4 ] ? 
				best_mix->second : best_mix->first;
				
			if( output) output[ x + y * width ] = (PALDITHER_U8) index;
			abgr[ x + y * width ] = ( abgr[ x + y * width ] & 0xff000000 ) | ( palette->colortable[ index ] & 0x00ffffff );
			}
		}
	}


#endif /* PALDITHER_IMPLEMENTATION */



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
