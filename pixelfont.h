/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

pixelfont.h - v0.1 - Custom pixel font format builder and renderer.

Do this:
    #define PIXELFONT_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.

If you want to generate pixelfonts through the pixelfont_builder api, do this:
    #define PIXELFONT_BUILDER_IMPLEMENTATION
before include the file to create the implementation.

*/

#ifndef pixelfont_h
#define pixelfont_h

#ifndef PIXELFONT_I8
    #define PIXELFONT_I8 signed char
#endif

#ifndef PIXELFONT_U8
    #define PIXELFONT_U8 unsigned char
#endif

#ifndef PIXELFONT_U16
    #define PIXELFONT_U16 unsigned short
#endif

#ifndef PIXELFONT_U32
    #define PIXELFONT_U32 unsigned int
#endif

typedef struct pixelfont_t
    {
    PIXELFONT_U32 size_in_bytes;
    PIXELFONT_U8 height;
    PIXELFONT_U8 line_spacing;
    PIXELFONT_U8 baseline;
    PIXELFONT_U32 offsets[ 256 ];
    PIXELFONT_U8 glyphs[ 1 ]; // "open" array - ok to access out of bounds (use size_in_bytes to determine the end)
    } pixelfont_t;


typedef enum pixelfont_align_t
    {
    PIXELFONT_ALIGN_LEFT,
    PIXELFONT_ALIGN_RIGHT,
    PIXELFONT_ALIGN_CENTER,
    } pixelfont_align_t;


typedef enum pixelfont_bold_t
    {
    PIXELFONT_BOLD_OFF,
    PIXELFONT_BOLD_ON,
    } pixelfont_bold_t;


typedef enum pixelfont_italic_t
    {
    PIXELFONT_ITALIC_OFF,
    PIXELFONT_ITALIC_ON,
    } pixelfont_italic_t;


typedef enum pixelfont_underline_t
    {
    PIXELFONT_UNDERLINE_OFF,
    PIXELFONT_UNDERLINE_ON,
    } pixelfont_underline_t;


typedef struct pixelfont_bounds_t
    {
    int width;
    int height;
    } pixelfont_bounds_t;



typedef struct pixelfont_builder_t pixelfont_builder_t;

pixelfont_builder_t* pixelfont_builder_create( int height, int baseline, int line_spacing, void* memctx );
void pixelfont_builder_destroy( pixelfont_builder_t* builder );

void pixelfont_builder_glyph( pixelfont_builder_t* builder, int glyph, int width, PIXELFONT_U8* pixels, int lead, int trail );
void pixelfont_builder_kerning( pixelfont_builder_t* builder, int glyph, int follower, int adjust );

pixelfont_t* pixelfont_builder_font( pixelfont_builder_t* builder );

#endif /* pixelfont_h */


// The following will be defined each time the file is included. By defining PIXELFONT_COLOR and PIXELFONT_FUNC_NAME
// it is possible to create declarations for different pixel formats. By defining the same ones, and the additional
// PIXELFONT_PIXEL_FUNC before including the implementation part, the corresponding definition can be created.

#ifndef PIXELFONT_COLOR
    #define PIXELFONT_COLOR PIXELFONT_U8
#endif

#ifndef PIXELFONT_FUNC_NAME
    #define PIXELFONT_FUNC_NAME pixelfont_blit
#endif

void PIXELFONT_FUNC_NAME( pixelfont_t const* font, int x, int y, char const* text, PIXELFONT_COLOR color,
    PIXELFONT_COLOR* target, int width, int height, pixelfont_align_t align, int wrap_width, int hspacing,
    int vspacing, int limit, pixelfont_bold_t bold, pixelfont_italic_t italic, pixelfont_underline_t underline,
    pixelfont_bounds_t* bounds );


/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef PIXELFONT_IMPLEMENTATION
#undef PIXELFONT_IMPLEMENTATION

#ifndef PIXELFONT_PIXEL_FUNC
    #define PIXELFONT_PIXEL_FUNC( dst, fnt, col ) *(dst) = (col);
#endif

void PIXELFONT_FUNC_NAME( pixelfont_t const* font, int x, int y, char const* text, PIXELFONT_COLOR color,
    PIXELFONT_COLOR* target, int width, int height, pixelfont_align_t align, int wrap_width, int hspacing,
    int vspacing,  int limit, pixelfont_bold_t bold, pixelfont_italic_t italic, pixelfont_underline_t underline,
    pixelfont_bounds_t* bounds )
    {
    int xp = x;
    int yp = y;
    int max_x = x;
    int last_x_on_line = xp;
    int count = 0;
    char const* str = text;
    while( *str )
        {
        int line_char_count = 0;
        int line_width = 0;
        int last_space_char_count = 0;
        int last_space_width = 0;
        char const* tstr = str;
        while( *tstr != '\n' && *tstr != '\0' && ( wrap_width <= 0 || line_width <= wrap_width  ) )
            {
            if( *tstr <= ' ' || ( tstr > str && tstr[-1] == '-' ) )
                {
                last_space_char_count = line_char_count;
                last_space_width = line_width;
                }
            PIXELFONT_U8 const* g = font->glyphs + font->offsets[ (uint8_t)( *tstr ) ];
            line_width += (PIXELFONT_I8) *g++;
            int w = *g++;
            g += font->height * w;
            line_width += (PIXELFONT_I8) *g++;
            line_width += hspacing + ( bold ? 1 : 0 );
            ++tstr;
            int kern = *g++;
            for( int k = 0; k < kern; ++k )
                if( *g++ == *tstr ) { line_width += (PIXELFONT_I8) *g++; break; } else ++g;
            ++line_char_count;
            }

        int skip_space = 0;
        if( wrap_width > 0 && line_width > wrap_width )
            {
            if( last_space_char_count > 0 ) line_char_count = last_space_char_count;
            line_width = last_space_width;
            skip_space = 1;
            }

        if( wrap_width > 0 )
            {
            if( align == PIXELFONT_ALIGN_RIGHT ) x += wrap_width - line_width;
            if( align == PIXELFONT_ALIGN_CENTER ) x += ( wrap_width - line_width ) / 2;
            }
        else
            {
            if( align == PIXELFONT_ALIGN_RIGHT ) x -= line_width;
            if( align == PIXELFONT_ALIGN_CENTER ) x -= line_width / 2;
            }

        for( int c = 0; c < line_char_count; ++c )
            {
            PIXELFONT_U8 const* g = font->glyphs + font->offsets[ (uint8_t)( *str ) ];
            x += (PIXELFONT_I8) *g++;
            int w = *g++;
            int h = font->height;
            for( int iy = y; iy < y + h; ++iy )
                {
                int xs = x + ( italic ? ( h - ( iy - y ) ) / 2 - 1 : 0 );
                for( int ix = xs; ix < xs + w; ++ix )
                    {
                    int p = *g++;
                    if( p && target )
                        if( limit < 0 || count < limit )
                            if( ix >= 0 && iy >= 0 && ix < width && iy < height )
                                {
                                last_x_on_line = ix >= last_x_on_line ? ix + ( bold ? 1 : 0 ) : last_x_on_line;
                                PIXELFONT_PIXEL_FUNC( ( &target[ ix + iy * width ] ), (PIXELFONT_U8)p, color );
                                if( bold && ix + 1 < width )
                                    PIXELFONT_PIXEL_FUNC( ( &target[ ix + 1 + iy * width ] ), (PIXELFONT_U8)p, color );
                                }
                    }
                }

            x += (PIXELFONT_I8) *g++;
            x += hspacing + ( bold ? 1 : 0 );
            ++str;
            ++count;

            int kern = *g++;
            for( int k = 0; k < kern; ++k )
                if( *g++ == *str ) { x += (PIXELFONT_I8) *g++; break; } else ++g;

            }

            if( underline && target && y + font->baseline + 1 >= 0 && y + font->baseline + 1 < height && last_x_on_line > xp )
                for( int ix = xp; ix <= last_x_on_line; ++ix )
                    if( ix >= 0 && ix < width )
                        PIXELFONT_PIXEL_FUNC( ( &target[ ix + ( y + font->baseline + 1 ) * width ] ), (PIXELFONT_U8)255, color );
            last_x_on_line = xp;
            max_x = x > max_x ? x : max_x;
            x = xp;
            if( *str ) {
                y += font->line_spacing + vspacing;
            } else {
                y += font->height;
            }
            if( *str == '\n' ) ++str;
            if( *str && skip_space && *str <= ' ' ) ++str;
        }

    if( bounds )
        {
        bounds->width = wrap_width > 0 ? wrap_width : ( max_x - xp );
        bounds->height = y - yp;
        }
    }


#undef PIXELFONT_COLOR
#undef PIXELFONT_FUNC_NAME

#endif /* PIXELFONT_IMPLEMENTATION */



#ifdef PIXELFONT_BUILDER_IMPLEMENTATION
#undef PIXELFONT_BUILDER_IMPLEMENTATION


#ifndef PIXELFONT_MALLOC
    #undef _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_NONSTDC_NO_DEPRECATE
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #define PIXELFONT_MALLOC( ctx, size ) ( malloc( size ) )
    #define PIXELFONT_FREE( ctx, ptr ) ( free( ptr ) )
#endif


#ifndef PIXELFONT_MEMCPY
    #undef _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_NONSTDC_NO_DEPRECATE
    #undef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define PIXELFONT_MEMCPY( dst, src, cnt ) ( memcpy( dst, src, cnt ) )
#endif


typedef struct pixelfont_builder_glyph_t
    {
    int lead;
    int trail;
    int width;
    PIXELFONT_U8* pixels;
    } pixelfont_builder_glyph_t;


typedef struct pixelfont_builder_kerning_t
    {
    int glyph;
    int follower;
    int adjust;
    } pixelfont_builder_kerning_t;


struct pixelfont_builder_t
    {
    void* memctx;
    pixelfont_t* font;
    int height;
    int baseline;
    int line_spacing;

    pixelfont_builder_glyph_t glyphs[ 256 ];

    int kernings_capacity;
    int kernings_count;
    pixelfont_builder_kerning_t* kernings;
    };


pixelfont_builder_t* pixelfont_builder_create( int height, int baseline, int line_spacing, void* memctx )
    {
    pixelfont_builder_t* builder = (pixelfont_builder_t*) PIXELFONT_MALLOC( memctx, sizeof( pixelfont_builder_t ) ) ;
    memset( builder, 0, sizeof( *builder ) );
    builder->memctx = memctx;
    builder->font = 0;
    builder->height = height < 0 ? 0 : height > 255 ? 255 : height;
    builder->baseline = baseline < 0 ? 0 : baseline > 255 ? 255 : baseline;
    builder->line_spacing = line_spacing < 0 ? 0 : line_spacing > 255 ? 255 : line_spacing;
    builder->kernings_capacity = 0;
    builder->kernings_count = 0;
    builder->kernings = 0;
    return builder;
    }


void pixelfont_builder_destroy( pixelfont_builder_t* builder )
    {
    for( int i = 0; i < sizeof( builder->glyphs ) / sizeof( *builder->glyphs ); ++i )
        if( builder->glyphs[ i ].pixels ) PIXELFONT_FREE( builder->memctx, builder->glyphs[ i ].pixels );

    if( builder->kernings ) PIXELFONT_FREE( builder->memctx, builder->kernings );
    if( builder->font ) PIXELFONT_FREE( builder->memctx, builder->font );
    PIXELFONT_FREE( builder->memctx, builder );
    }


void pixelfont_builder_glyph( pixelfont_builder_t* builder, int glyph, int width, PIXELFONT_U8* pixels, int lead, int trail )
    {
    if( glyph < 0 || glyph > 255 ) return;

    if( builder->glyphs[ glyph ].pixels ) PIXELFONT_FREE( builder->memctx, builder->glyphs[ glyph ].pixels );
    builder->glyphs[ glyph ].pixels = 0;
    builder->glyphs[ glyph ].lead = 0;
    builder->glyphs[ glyph ].trail = 0;
    builder->glyphs[ glyph ].width = 0;

    if( pixels && width > 0 )
        {
        builder->glyphs[ glyph ].pixels = (PIXELFONT_U8*) PIXELFONT_MALLOC( builder->memctx, width * builder->height * sizeof( PIXELFONT_U8 ) );
        PIXELFONT_MEMCPY( builder->glyphs[ glyph ].pixels, pixels, width * builder->height * sizeof( PIXELFONT_U8 ) );
        builder->glyphs[ glyph ].lead = lead < -127 ? -127 : lead > 127 ? 127 : lead;
        builder->glyphs[ glyph ].trail = trail < -127 ? -127 : trail > 127 ? 127 : trail;
        builder->glyphs[ glyph ].width = width < 0 ? 0 : width > 255 ? 255 : width;
        }
    else if( lead || trail )
        {
        builder->glyphs[ glyph ].pixels = (PIXELFONT_U8*) PIXELFONT_MALLOC( builder->memctx, 1 );
        builder->glyphs[ glyph ].lead = lead < -127 ? -127 : lead > 127 ? 127 : lead;
        builder->glyphs[ glyph ].trail = trail < -127 ? -127 : trail > 127 ? 127 : trail;
        builder->glyphs[ glyph ].width = 0;
        }
    }


void pixelfont_builder_kerning( pixelfont_builder_t* builder, int glyph, int follower, int adjust )
    {
    adjust = adjust < -127 ? -127 : adjust > 127 ? 127 : adjust;

    for( int i = 0; i < builder->kernings_count; ++i )
        {
        if( builder->kernings[ i ].glyph == glyph && builder->kernings[ i ].follower == follower )
            {
            if( adjust )
                builder->kernings[ i ].adjust = adjust;
            else
                builder->kernings[ i ] = builder->kernings[ --builder->kernings_count ];
            return;
            }
        }

    if( adjust )
        {
        if( !builder->kernings || builder->kernings_count >= builder->kernings_capacity )
            {
            builder->kernings_capacity = builder->kernings_capacity ? builder->kernings_capacity * 2 : 256;
            pixelfont_builder_kerning_t* kernings = (pixelfont_builder_kerning_t*) PIXELFONT_MALLOC( builder->memctx,
                sizeof( pixelfont_builder_kerning_t ) * builder->kernings_capacity );
            if( builder->kernings )
                {
                PIXELFONT_MEMCPY( kernings, builder->kernings, sizeof( pixelfont_builder_kerning_t ) * builder->kernings_count );
                PIXELFONT_FREE( builder->memctx, builder->kernings );
                }
            builder->kernings = kernings;
            }

        builder->kernings[ builder->kernings_count ].glyph = glyph;
        builder->kernings[ builder->kernings_count ].follower = follower;
        builder->kernings[ builder->kernings_count ].adjust = adjust;
        ++builder->kernings_count;
        }
    }


pixelfont_t* pixelfont_builder_font( pixelfont_builder_t* builder )
    {
    if( builder->font ) PIXELFONT_FREE( builder->memctx, builder->font );

    PIXELFONT_U8 kerning_counts[ 256 ];
    memset( kerning_counts, 0, sizeof( kerning_counts ) );
    for( int i = 0; i < builder->kernings_count; ++i )
        ++kerning_counts[ builder->kernings[ i ].glyph ];

    PIXELFONT_U32 offsets[ 256 ];
    memset( offsets, 0, sizeof( offsets ) );
    int current_offset = 1;
    int total_width = 0;
    int glyph_count = 0;
    for( int i = 0; i < sizeof( builder->glyphs ) / sizeof( *builder->glyphs ); ++i )
        {
        if( builder->glyphs[ i ].pixels )
            {
            ++glyph_count;
            total_width += builder->glyphs[ i ].width;
            offsets[ i ] = current_offset;
            current_offset += 1 + kerning_counts[ i ] + 3 + builder->glyphs[ i ].width * builder->height;
            }
        }

    size_t size_in_bytes = sizeof( pixelfont_t ) - sizeof( PIXELFONT_U8 ); // base size excluding final placeholder byte
	size_in_bytes += 1; // skip first slot in array, as we use 0 for "no glyph" marker
    size_in_bytes += glyph_count + 2 * builder->kernings_count; // kerning pair count for each glyph + all kerning data
    size_in_bytes += 3 * glyph_count; // lead, trail and width for each glyph
    size_in_bytes += total_width * builder->height; // pixel data for all glyphs

    pixelfont_t* font = (pixelfont_t*) PIXELFONT_MALLOC( builder->memctx, size_in_bytes );
    memset( font, 0, sizeof( *font ) );
    font->size_in_bytes = (PIXELFONT_U32) size_in_bytes;
    font->height = (PIXELFONT_U8) builder->height;
    font->line_spacing = (PIXELFONT_U8) builder->line_spacing;
    font->baseline = (PIXELFONT_U8) builder->baseline;
    memcpy( font->offsets, offsets, sizeof( font->offsets ) );

    for( int i = 0; i < sizeof( builder->glyphs ) / sizeof( *builder->glyphs ); ++i )
        {
        if( builder->glyphs[ i ].pixels )
            {
            PIXELFONT_U8* src = builder->glyphs[ i ].pixels;
            PIXELFONT_U8* out = font->glyphs + offsets[ i ];
            *out++ = (PIXELFONT_U8) builder->glyphs[ i ].lead;
            *out++ = (PIXELFONT_U8) builder->glyphs[ i ].width;
            for( int y = 0; y < builder->height; ++y )
                for( int x = 0; x < builder->glyphs[ i ].width; ++x )
                    *out++ = *src++;
            *out++ = (PIXELFONT_U8) builder->glyphs[ i ].trail;

            *out++ = kerning_counts[ i ];
            for( int j = 0; j < builder->kernings_count; ++j )
                {
                if( builder->kernings[ j ].glyph == i )
                    {
                    *out++ = (PIXELFONT_U8) builder->kernings[ j ].follower;
                    *out++ = (PIXELFONT_U8) builder->kernings[ j ].adjust;
                    }
                }
            }
        }

    builder->font = font;
    return builder->font;
    }


#endif /* PIXELFONT_BUILDER_IMPLEMENTATION */


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
