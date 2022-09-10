/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

palrle.h - v0.1 - Run-length encoding of palettized bitmaps, for C/C++.

Do this:
    #define PALRLE_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef palrle_h
#define palrle_h

#ifndef PALRLE_U8
    #define PALRLE_U8 unsigned char
#endif
#ifndef PALRLE_U16
    #define PALRLE_U16 unsigned short
#endif
#ifndef PALRLE_U32
    #define PALRLE_U32 unsigned int
#endif

typedef struct palrle_data_t {
    PALRLE_U32 size;
    PALRLE_U16 width;
    PALRLE_U16 height;
    PALRLE_U16 xoffset;
    PALRLE_U16 yoffset;
    PALRLE_U16 hpitch;
    PALRLE_U16 vpitch;
    PALRLE_U16 palette_count;
    PALRLE_U8 data[ 1 ]; // "open" array
} palrle_data_t;

palrle_data_t* palrle_encode( PALRLE_U8* pixels, int width, int height, PALRLE_U32* palette, int palette_count, void* memctx );
palrle_data_t* palrle_encode_mask( PALRLE_U8* pixels, PALRLE_U8* mask, int width, int height, PALRLE_U32* palette, int palette_count, void* memctx );
//palrle_data_t* palrle_encode_transparency_index( PALRLE_U8* pixels, PALRLE_U8 transparency_index, int width, int height, PALRLE_U32* palette, int palette_count, void* memctx );

void palrle_decode( palrle_data_t* rle_data, PALRLE_U8* pixels, PALRLE_U8* mask );

void palrle_blit( palrle_data_t* rle_data, int x, int y, PALRLE_U8* pixels, int width, int height );

void palrle_free( palrle_data_t* rle_data, void* memctx );


#endif /* palrle_h */

/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef PALRLE_IMPLEMENTATION
#undef PALRLE_IMPLEMENTATION

#ifndef PALRLE_MALLOC
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #define PALRLE_MALLOC( ctx, size ) ( malloc( size ) )
    #define PALRLE_FREE( ctx, ptr ) ( free( ptr ) )
#endif


palrle_data_t* palrle_encode( PALRLE_U8* pixels, int width, int height, PALRLE_U32* palette, int palette_count, void* memctx ) {
    (void) memctx;

    int xmin = 0;
    int xmax = width - 1;
    int ymin = 0;
    int ymax = height - 1;
    int hpitch = xmax - xmin + 1;
    int vpitch = ymax - ymin + 1;

    // Bitmap is completely empty, so just store the palette and dimensions
    if( hpitch <= 0 || vpitch <= 0 ) {
        size_t size = sizeof( palrle_data_t ) - sizeof( PALRLE_U8 ) + sizeof( PALRLE_U32 ) * palette_count;
        palrle_data_t* data = (palrle_data_t*) PALRLE_MALLOC( memctx, size );
        data->size = (PALRLE_U32) size;
        data->width = (PALRLE_U16) width;
        data->height = (PALRLE_U16) height;
        data->xoffset = 0;
        data->yoffset = 0;
        data->hpitch = 0;
        data->vpitch = 0;
        data->palette_count = (PALRLE_U16) palette_count;
        if( palette ) memcpy( data->data, palette, sizeof( PALRLE_U32 ) * palette_count );
        return data;
    }

    palrle_data_t* data = (palrle_data_t*) PALRLE_MALLOC( memctx,
        sizeof( palrle_data_t ) + // size for the struct itself
        sizeof( PALRLE_U32 ) * palette_count + // size for storing palette entries
        sizeof( PALRLE_U32 ) * vpitch + // size for storing the offset for each row
        hpitch * vpitch * 2 ); // assume worst case - we should never need more than twice the number of pixels
    memset( data, 0, sizeof( palrle_data_t ) + sizeof( PALRLE_U32 ) * palette_count + sizeof( PALRLE_U32 ) * vpitch +
        hpitch * vpitch * 2 );


    data->size = (PALRLE_U32) ( sizeof( palrle_data_t ) - sizeof( PALRLE_U8 ) );
    data->width = (PALRLE_U16) width;
    data->height = (PALRLE_U16) height;
    data->xoffset = (PALRLE_U16) xmin;
    data->yoffset = (PALRLE_U16) ymin;
    data->hpitch = (PALRLE_U16) hpitch;
    data->vpitch = (PALRLE_U16) vpitch;
    data->palette_count = (PALRLE_U16) palette_count;
    if( palette ) memcpy( data->data, palette, sizeof( PALRLE_U32 ) * palette_count );

    int row_offset = (int)( sizeof( PALRLE_U32 ) * palette_count );
    int rle_offset = (int)( row_offset + sizeof( PALRLE_U32 ) * vpitch );
    for( int y = 0; y < vpitch; ++y ) {
        *( (PALRLE_U32*)&data->data[ row_offset ] ) = (PALRLE_U32) rle_offset;
        row_offset += sizeof( PALRLE_U32 );
        int x = 0;
        while( x < hpitch ) {
            // add empty pixel count
            data->data[ rle_offset++ ] = (PALRLE_U8) 0;

            // add non-empty pixels
            int color = pixels[ x + xmin + ( y + ymin ) * width ];
            int count = 0;
            int tx = x;
            while( tx < hpitch ) {
                if( pixels[ tx + xmin + ( y + ymin ) * width ] != color ) break;
                if( count >= 127 ) break;
                ++count;
                ++tx;
            }

            if( count == 0 ) {
                data->data[ rle_offset++ ] = 0;
                continue;
            }

            if( count > 2 ) { // store as a run
                data->data[ rle_offset++ ] = (PALRLE_U8) count; // number of pixels
                data->data[ rle_offset++ ] = (PALRLE_U8) color; // of palette index `color`
                x = tx;
            } else { // store as unique values
                PALRLE_U8* uniques_out = &data->data[ rle_offset++ ];
                int uniques = 1;
                data->data[ rle_offset++ ] = (PALRLE_U8) color;
                ++x;

                // add pixexls until we encounter a run
                while( x < hpitch ) {
                    // check the length of the next run
                    color = pixels[ x + xmin + ( y + ymin ) * width ];
                    count = 0;
                    tx = x;
                    while( tx < hpitch ) {
                        if( pixels[ tx + xmin + ( y + ymin ) * width ] != color ) break;
                        if( count > 2 ) break; // No need to keep counting, we know this is a run
                        ++count;
                        ++tx;
                    }
                    if( count > 2 || count == 0 ) {
                        break;
                    } else {
                        if( uniques >= 128 ) break;
                        ++uniques;
                        data->data[ rle_offset++ ] = (PALRLE_U8) color;
                        ++x;
                    }
                }
                *uniques_out = (PALRLE_U8)( (char) -uniques );
            }
        }
    }
    data->size += rle_offset;

    return data;
}


palrle_data_t* palrle_encode_mask( PALRLE_U8* pixels, PALRLE_U8* mask, int width, int height, PALRLE_U32* palette, int palette_count, void* memctx ) {
    (void) memctx;
    // Crop to smallest non-empty region
    int xmin = width;
    int xmax = 0;
    int ymin = height;
    int ymax = 0;
    for( int y = 0; y < height; ++y ) {
        for( int x = 0; x < width; ++x ) {
            if( mask[ x + y * width ] ) {
                if( x < xmin ) xmin = x;
                if( y < ymin ) ymin = y;
                if( x > xmax ) xmax = x;
                if( y > ymax ) ymax = y;
            }
        }
    }
    int hpitch = xmax - xmin + 1;
    int vpitch = ymax - ymin + 1;

    // Bitmap is completely empty, so just store the palette and dimensions
    if( hpitch <= 0 || vpitch <= 0 ) {
        size_t size = sizeof( palrle_data_t ) - sizeof( PALRLE_U8 ) + sizeof( PALRLE_U32 ) * palette_count;
        palrle_data_t* data = (palrle_data_t*) PALRLE_MALLOC( memctx, size );
        data->size = (PALRLE_U32) size;
        data->width = (PALRLE_U16) width;
        data->height = (PALRLE_U16) height;
        data->xoffset = 0;
        data->yoffset = 0;
        data->hpitch = 0;
        data->vpitch = 0;
        data->palette_count = (PALRLE_U16) palette_count;
        if( palette ) memcpy( data->data, palette, sizeof( PALRLE_U32 ) * palette_count );
        return data;
    }

    palrle_data_t* data = (palrle_data_t*) PALRLE_MALLOC( memctx,
        sizeof( palrle_data_t ) + // size for the struct itself
        sizeof( PALRLE_U32 ) * palette_count + // size for storing palette entries
        sizeof( PALRLE_U32 ) * vpitch + // size for storing the offset for each row
        hpitch * vpitch * 2 ); // assume worst case - we should never need more than twice the number of pixels
    memset( data, 0, sizeof( palrle_data_t ) + sizeof( PALRLE_U32 ) * palette_count + sizeof( PALRLE_U32 ) * vpitch +
        hpitch * vpitch * 2 );


    data->size = (PALRLE_U32) ( sizeof( palrle_data_t ) - sizeof( PALRLE_U8 ) );
    data->width = (PALRLE_U16) width;
    data->height = (PALRLE_U16) height;
    data->xoffset = (PALRLE_U16) xmin;
    data->yoffset = (PALRLE_U16) ymin;
    data->hpitch = (PALRLE_U16) hpitch;
    data->vpitch = (PALRLE_U16) vpitch;
    data->palette_count = (PALRLE_U16) palette_count;
    if( palette ) memcpy( data->data, palette, sizeof( PALRLE_U32 ) * palette_count );

    int row_offset = (int)( sizeof( PALRLE_U32 ) * palette_count );
    int rle_offset = (int)( row_offset + sizeof( PALRLE_U32 ) * vpitch );
    for( int y = 0; y < vpitch; ++y ) {
        *( (PALRLE_U32*)&data->data[ row_offset ] ) = (PALRLE_U32) rle_offset;
        row_offset += sizeof( PALRLE_U32 );
        int x = 0;
        while( x < hpitch ) {
            // add empty pixel count
            int empty = 0;
            while( x < hpitch ) {
                if( mask[ x + xmin + ( y + ymin ) * width ] != 0 ) break;
                if( empty >= 255 ) {
                    data->data[ rle_offset++ ] = 255; // 255 empty pixels
                    data->data[ rle_offset++ ] = 0; // 0 non-empty pixels
                    empty = 0;
                }
                ++empty;
                ++x;
            }
            data->data[ rle_offset++ ] = (PALRLE_U8) empty;

            // add non-empty pixels
            int color = pixels[ x + xmin + ( y + ymin ) * width ];
            int count = 0;
            int tx = x;
            while( tx < hpitch ) {
                if( mask[ tx + xmin + ( y + ymin ) * width ] == 0 ) break;
                if( pixels[ tx + xmin + ( y + ymin ) * width ] != color ) break;
                if( count >= 127 ) break;
                ++count;
                ++tx;
            }

            if( count == 0 ) {
                data->data[ rle_offset++ ] = 0;
                continue;
            }

            if( count > 2 ) { // store as a run
                data->data[ rle_offset++ ] = (PALRLE_U8) count; // number of pixels
                data->data[ rle_offset++ ] = (PALRLE_U8) color; // of palette index `color`
                x = tx;
            } else { // store as unique values
                PALRLE_U8* uniques_out = &data->data[ rle_offset++ ];
                int uniques = 1;
                data->data[ rle_offset++ ] = (PALRLE_U8) color;
                ++x;

                // add pixexls until we encounter a run
                while( x < hpitch ) {
                    // check the length of the next run
                    color = pixels[ x + xmin + ( y + ymin ) * width ];
                    count = 0;
                    tx = x;
                    while( tx < hpitch ) {
                        if( mask[ tx + xmin + ( y + ymin ) * width ] == 0 ) break;
                        if( pixels[ tx + xmin + ( y + ymin ) * width ] != color ) break;
                        if( count > 2 ) break; // No need to keep counting, we know this is a run
                        ++count;
                        ++tx;
                    }
                    if( count > 2 || count == 0 ) {
                        break;
                    } else {
                        if( uniques >= 128 ) break;
                        ++uniques;
                        data->data[ rle_offset++ ] = (PALRLE_U8) color;
                        ++x;
                    }
                }
                *uniques_out = (PALRLE_U8)( (char) -uniques );
            }
        }
    }
    data->size += rle_offset;

    return data;
}


/*
palrle_data_t* palrle_encode_transparency_index( PALRLE_U8* pixels, PALRLE_U8 transparency_index, int width, int height, PALRLE_U32* palette, int palette_count, void* memctx ) {
    (void) pixels, transparency_index, width, height, memctx;
    return 0;
}
*/



void palrle_decode( palrle_data_t* rle_data, PALRLE_U8* pixels, PALRLE_U8* mask ) {
    memset( pixels, 0, rle_data->width * rle_data->height * sizeof( PALRLE_U8 ) );
    if( mask ) memset( mask, 0, rle_data->width * rle_data->height * sizeof( PALRLE_U8 ) );
    PALRLE_U8* data = &rle_data->data[ sizeof( PALRLE_U32 ) * ( rle_data->palette_count + rle_data->vpitch ) ];
    for( int y = 0; y < rle_data->vpitch; ++y ) {
        int x = 0;
        while( x < rle_data->hpitch ) {
            x += *data++;
            char count = (char)( *data++ );
            if( count > 0 ) {
                PALRLE_U8 color = *data++;
                for( int i = 0; i < count; ++i ) {
                    pixels[ rle_data->xoffset + x + ( rle_data->yoffset + y ) * rle_data->width ] = color;
                    if( mask ) mask[ rle_data->xoffset + x + ( rle_data->yoffset + y ) * rle_data->width ] = 255;
                    ++x;
                }
            } else {
                for( int i = 0; i < -count; ++i ) {
                    pixels[ rle_data->xoffset + x + ( rle_data->yoffset + y ) * rle_data->width ] = *data++;
                    if( mask ) mask[ rle_data->xoffset + x + ( rle_data->yoffset + y ) * rle_data->width ] = 255;
                    ++x;
                }
            }
        }
    }
}


void palrle_blit( palrle_data_t* rle_data, int x, int y, PALRLE_U8* pixels, int width, int height ) {
    PALRLE_U8* data = &rle_data->data[ sizeof( PALRLE_U32 ) * ( rle_data->palette_count + rle_data->vpitch ) ];
    for( int iy = 0; iy < rle_data->vpitch; ++iy ) {
        int ix = 0;
        while( ix < rle_data->hpitch ) {
            ix += *data++;
            char count = (char)( *data++ );
            if( count > 0 ) {
                PALRLE_U8 color = *data++;
                for( int i = 0; i < count; ++i ) {
                    int xp = rle_data->xoffset + ix + x;
                    int yp = rle_data->yoffset + iy + y;
                    if( xp > 0 && yp > 0 && xp < width && yp < height ) pixels[ xp + yp * width ] = color;
                    ++ix;
                }
            } else {
                for( int i = 0; i < -count; ++i ) {
                    PALRLE_U8 color = *data++;
                    int xp = rle_data->xoffset + ix + x;
                    int yp = rle_data->yoffset + iy + y;
                    if( xp > 0 && yp > 0 && xp < width && yp < height ) pixels[ xp + yp * width ] = color;
                    ++ix;
                }
            }
        }
    }
}


void palrle_free( palrle_data_t* rle_data, void* memctx ) {
    (void) rle_data, (void) memctx;
    PALRLE_FREE( memctx, rle_data );
}

#endif /* PALRLE_IMPLEMENTATION */


/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2019 Mattias Gustavsson

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
