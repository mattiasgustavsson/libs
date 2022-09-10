/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

img.h - v0.1 - Image processing functions for C/C++.

Do this:
    #define IMG_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef img_h
#define img_h

#ifndef IMG_U32
    #define IMG_U32  unsigned int
#endif


typedef struct img_rgba_t {
    float r;
    float g;
    float b;
    float a;
} img_rgba_t;

typedef struct img_t {
    int width;
    int height;
    img_rgba_t* pixels;
} img_t;

typedef struct img_hsva_t {
    float h;
    float s;
    float v;
    float a;
} img_hsva_t;

img_t img_create( int width, int height );
img_t img_from_abgr32( IMG_U32* abgr, int width, int height );
void img_free( img_t* img );

void img_to_argb32( img_t* img, IMG_U32* abgr );
img_rgba_t img_sample_clamp( img_t* img, float u, float v );

img_rgba_t img_rgba_lerp( img_rgba_t a, img_rgba_t b, float s );
img_hsva_t img_rgba_to_hsva( img_rgba_t rgb );
img_rgba_t img_hsva_to_rgba( img_hsva_t hsv );

void img_adjust_brightness( img_t* img, float value );
void img_adjust_contrast( img_t* img, float value );
void img_adjust_saturation( img_t* img, float value );
void img_sharpen( img_t* img, float radius, float blend );


#endif /* img_h */

/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef IMG_IMPLEMENTATION
#undef IMG_IMPLEMENTATION

#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>


#if !defined( IMG_POW ) || !defined( IMG_FLOOR )
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
    #pragma warning( push )
    #pragma warning( disable: 4668 ) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
    #include <math.h>
    #pragma warning( pop )
#endif


#ifndef IMG_POW
    #ifdef __TINYC__
        #define IMG_POW( x, y ) ( (float) pow( (double) x, (double) y ) )
    #else
        #define IMG_POW( x, y ) powf( x, y )
    #endif
#endif

#ifndef IMG_FLOOR
    #ifdef __TINYC__
        #define IMG_FLOOR( x ) ( (float) floor( (double) x ) )
    #else
        #define IMG_FLOOR( x ) floorf( x )
    #endif
#endif


img_t img_create( int width, int height ) {
    img_t img;
    img.width = width;
    img.height = height;
    img.pixels = (img_rgba_t*) malloc( sizeof( img_rgba_t ) * width * height );
    memset( img.pixels, 0, sizeof( img_rgba_t ) * width * height );
    return img;
}


img_t img_from_abgr32( IMG_U32* abgr, int width, int height ) {
    img_t img;
    img.width = width;
    img.height = height;
    img.pixels = (img_rgba_t*) malloc( sizeof( img_rgba_t ) * width * height );
    for( int i = 0; i < width * height; ++i ) {
        IMG_U32 p = abgr[ i ];
        IMG_U32 b = ( p       ) & 0xff;
        IMG_U32 g = ( p >>  8 ) & 0xff;
        IMG_U32 r = ( p >> 16 ) & 0xff;
        IMG_U32 a = ( p >> 24 ) & 0xff;
        img_rgba_t* f = &img.pixels[ i ];
        f->r = ( (float) r ) / 255.0f;
        f->g = ( (float) g ) / 255.0f;
        f->b = ( (float) b ) / 255.0f;
        f->a = ( (float) a ) / 255.0f;
    }
    return img;
}


void img_free( img_t* img ) {
    free( img->pixels );
    img->pixels = NULL;
}


float img_clamp( float x, float low, float high ) {
    return x < low ? low : x > high ? high : x;
}


void img_to_argb32( img_t* img, IMG_U32* abgr ) {
    for( int i = 0; i < img->width * img->height; ++i ) {
        img_rgba_t p = img->pixels[ i ];
        p.r = img_clamp( p.r, 0.0f, 1.0f );
        p.g = img_clamp( p.g, 0.0f, 1.0f );
        p.b = img_clamp( p.b, 0.0f, 1.0f );
        p.a = img_clamp( p.a, 0.0f, 1.0f );
        IMG_U32 r = (IMG_U32)( p.r * 256.0f );
        IMG_U32 g = (IMG_U32)( p.g * 256.0f );
        IMG_U32 b = (IMG_U32)( p.b * 256.0f );
        IMG_U32 a = (IMG_U32)( p.a * 256.0f );
        r = r > 255 ? 255 : r;
        g = g > 255 ? 255 : g;
        b = b > 255 ? 255 : b;
        a = a > 255 ? 255 : a;
        abgr[ i ] = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b );
    }
}



img_rgba_t img_rgba_lerp( img_rgba_t a, img_rgba_t b, float s ) {
    img_rgba_t c;
    c.r = a.r + ( b.r - a.r ) * s;
    c.g = a.g + ( b.g - a.g ) * s;
    c.b = a.b + ( b.b - a.b ) * s;
    c.a = a.a + ( b.a - a.a ) * s;
    return c;
}


img_rgba_t img_sample_clamp( img_t* img, float u, float v ) {
    int maxw = ( img->width - 1 );
    int maxh = ( img->height - 1 );

    float fu = IMG_FLOOR( u );
    float du = u - fu;
    float fv = IMG_FLOOR( v );
    float dv = v - fv;

    int x1 = (int) ( fu );
    int y1 = (int) ( fv );
    int x2 = (int) ( fu + 1.0f );
    int y2 = (int) ( fv + 1.0f );

    x1 = x1 < 0 ? 0 : x1 > maxw ? maxw : x1;
    x2 = x2 < 0 ? 0 : x2 > maxw ? maxw : x2;
    y1 = y1 < 0 ? 0 : y1 > maxh ? maxh : y1;
    y2 = y2 < 0 ? 0 : y2 > maxh ? maxh : y2;

    img_rgba_t c1 = img->pixels[ x1 + y1 * img->width ];
    img_rgba_t c2 = img->pixels[ x2 + y1 * img->width ];
    img_rgba_t c3 = img->pixels[ x1 + y2 * img->width ];
    img_rgba_t c4 = img->pixels[ x2 + y2 * img->width ];

    return img_rgba_lerp( img_rgba_lerp( c1, c2, du ), img_rgba_lerp( c3, c4, du ), dv );
}


void img_adjust_brightness( img_t* img, float value ) {
    if( value == 0.0f ) return;
    for( int i = 0; i < img->width * img->height; ++i ) {
        img_rgba_t* p = &img->pixels[ i ];
        p->r = p->r + value;
        p->g = p->g + value;
        p->b = p->b + value;
    }
}

void img_adjust_contrast( img_t* img, float value ) {
    if( value == 0.0f ) return;
    for( int i = 0; i < img->width * img->height; ++i ) {
        img_rgba_t* p = &img->pixels[ i ];
        p->r = ( p->r - 0.5f ) * value + 0.5f;
        p->g = ( p->g - 0.5f ) * value + 0.5f;
        p->b = ( p->b - 0.5f ) * value + 0.5f;
    }
}

#define img_min( x, y ) ( (x) < (y) ? (x) : (y) )
#define img_max( x, y ) ( (x) > (y) ? (x) : (y) )

img_hsva_t img_rgba_to_hsva( img_rgba_t rgb ) {
    img_hsva_t hsv;
    hsv.a = rgb.a;

    float cmin = img_min( rgb.r, img_min( rgb.g, rgb.b ) ); //Min. value of RGB
    float cmax = img_max( rgb.r, img_max( rgb.g, rgb.b ) ); //Max. value of RGB
    float cdel = cmax - cmin;   //Delta RGB value

    hsv.v = cmax;

    if( cdel == 0 ) { //This is a gray, no chroma...
        hsv.h = 0;  //HSV results from 0 to 1
        hsv.s = 0;
    } else { //Chromatic data...
        hsv.s = cdel / cmax;

        float rdel = ( ( ( cmax - rgb.r ) / 6.0f ) + ( cdel / 2.0f ) ) / cdel;
        float gdel = ( ( ( cmax - rgb.g ) / 6.0f ) + ( cdel / 2.0f ) ) / cdel;
        float bdel = ( ( ( cmax - rgb.b ) / 6.0f ) + ( cdel / 2.0f ) ) / cdel;

        if( rgb.r == cmax ) {
            hsv.h = bdel - gdel;
        } else if( rgb.g == cmax ) {
            hsv.h = ( 1.0f / 3.0f ) + rdel - bdel;
        } else {
            hsv.h = ( 2.0f / 3.0f ) + gdel - rdel;
        }

        if ( hsv.h < 0.0f ) {
            hsv.h += 1.0f;
        }
        if ( hsv.h > 1.0f ) {
            hsv.h -= 1.0f;
        }
    }

    hsv.h = hsv.h;
    hsv.s = hsv.s;
    hsv.v = hsv.v;
    return hsv;
}


img_rgba_t img_hsva_to_rgba( img_hsva_t hsv ) {
    img_rgba_t rgb;
    rgb.a = hsv.a;

    if( hsv.s == 0.0f ) { // HSV from 0 to 1
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
    } else {
        float h = hsv.h * 6.0f;
        if( h == 6.0f ) {
            h = 0.0f;   //H must be < 1
        }
        float i = IMG_FLOOR( h );
        float v1 = hsv.v * ( 1.0f - hsv.s );
        float v2 = hsv.v * ( 1.0f - hsv.s * ( h - i ) );
        float v3 = hsv.v * ( 1.0f - hsv.s * ( 1.0f - ( h - i ) ) );

        switch( (int) i ) {
            case 0: { rgb.r = hsv.v; rgb.g = v3   ; rgb.b = v1   ; } break;
            case 1: { rgb.r = v2   ; rgb.g = hsv.v; rgb.b = v1   ; } break;
            case 2: { rgb.r = v1   ; rgb.g = hsv.v; rgb.b = v3   ; } break;
            case 3: { rgb.r = v1   ; rgb.g = v2   ; rgb.b = hsv.v; } break;
            case 4: { rgb.r = v3   ; rgb.g = v1   ; rgb.b = hsv.v; } break;
            default:{ rgb.r = hsv.v; rgb.g = v1   ; rgb.b = v2   ; } break;
        }
    }

    rgb.r = rgb.r;
    rgb.g = rgb.g;
    rgb.b = rgb.b;
    return rgb;
}


void img_adjust_saturation( img_t* img, float value ) {
    if( value == 0.0f ) return;
    for( int i = 0; i < img->width * img->height; ++i ) {
        img_rgba_t* p = &img->pixels[ i ];
        img_hsva_t hsv = img_rgba_to_hsva( *p );
        hsv.s = img_clamp( hsv.s + value, 0.0f, 1.0f );
        *p = img_hsva_to_rgba( hsv );
    }
}


void img_sharpen( img_t* img, float radius, float blend ) {
    float filter[ 9 ] = {
        -1.0f, -1.0f, -1.0f,
        -1.0f,  9.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
    };

    img_rgba_t* result = (img_rgba_t*) malloc( img->width * img->height * sizeof( img_rgba_t ) );

    img_rgba_t* src = img->pixels;
    img_rgba_t* dst = result;
    for( int y = 0; y < img->height; ++y ) {
        for( int x = 0; x < img->width; ++x ) {
            if( src->a > 0.0f ) {
                img_rgba_t acc = { 0.0f, 0.0f, 0.0f, 0.0f };
                for( int fx = -1; fx <= 1; ++fx ) {
                    for( int fy = -1; fy <= 1; ++fy ) {
                        float u = ( (float) x ) + ( (float) fx ) * radius;
                        float v = ( (float) y ) + ( (float) fy ) * radius;
                        img_rgba_t s = img_sample_clamp( img, u, v );
                        if( s.a < 0.0001f ) {
                            s = *src;
                        }
                        acc.r += s.r * ( filter[ ( 1 + fx ) + ( 1 + fy ) * 3 ] );
                        acc.g += s.g * ( filter[ ( 1 + fx ) + ( 1 + fy ) * 3 ] );
                        acc.b += s.b * ( filter[ ( 1 + fx ) + ( 1 + fy ) * 3 ] );
                    }
                }
                *dst = img_rgba_lerp( *src, acc, blend );
                dst->a = src->a;
            } else {
                *dst = *src;
            }
            ++src;
            ++dst;
        }
    }

    free( img->pixels );
    img->pixels = result;
}

#endif /* IMG_IMPLEMENTATION */

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
