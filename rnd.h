/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

rnd.h - v1.0 - Pseudo-random number generators for C/C++.

Do this:
    #define RND_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.

Additional Contributors
  Jonatan Hedborg: unsigned int to normalized float conversion
*/

#ifndef rnd_h
#define rnd_h

#ifndef RND_U32
    #define RND_U32 unsigned int
#endif
#ifndef RND_U64
    #define RND_U64 unsigned long long
#endif

typedef struct rnd_pcg_t { RND_U64 state[ 2 ]; } rnd_pcg_t;
void rnd_pcg_seed( rnd_pcg_t* pcg, RND_U32 seed );
RND_U32 rnd_pcg_next( rnd_pcg_t* pcg );
float rnd_pcg_nextf( rnd_pcg_t* pcg );
int rnd_pcg_range( rnd_pcg_t* pcg, int min, int max );

typedef struct rnd_well_t { RND_U32 state[ 17 ]; } rnd_well_t;
void rnd_well_seed( rnd_well_t* well, RND_U32 seed );
RND_U32 rnd_well_next( rnd_well_t* well );
float rnd_well_nextf( rnd_well_t* well );
int rnd_well_range( rnd_well_t* well, int min, int max );

typedef struct rnd_gamerand_t { RND_U32 state[ 2 ]; } rnd_gamerand_t;
void rnd_gamerand_seed( rnd_gamerand_t* gamerand, RND_U32 seed );
RND_U32 rnd_gamerand_next( rnd_gamerand_t* gamerand );
float rnd_gamerand_nextf( rnd_gamerand_t* gamerand );
int rnd_gamerand_range( rnd_gamerand_t* gamerand, int min, int max );

typedef struct rnd_xorshift_t { RND_U64 state[ 2 ]; } rnd_xorshift_t;
void rnd_xorshift_seed( rnd_xorshift_t* xorshift, RND_U64 seed );
RND_U64 rnd_xorshift_next( rnd_xorshift_t* xorshift );
float rnd_xorshift_nextf( rnd_xorshift_t* xorshift );
int rnd_xorshift_range( rnd_xorshift_t* xorshift, int min, int max );

#endif /* rnd_h */

/**

rnd.h
=====

Pseudo-random number generators for C/C++.


Example
-------

A basic example showing how to use the PCG set of random functions.

    #define  RND_IMPLEMENTATION
    #include "rnd.h"

    #include <stdio.h> // for printf
    #include <time.h> // for time
    
    int main( int argc, char** argv ) {

        rnd_pcg_t pcg;
        rnd_pcg_seed( &pcg, 0u ); // initialize generator

        // print a handful of random integers
        // these will be the same on every run, as we 
        // seeded the rng with a fixed value
        for( int i = 0; i < 5; ++i ) {
            RND_U32 n = rnd_pcg_next( &pcg );
            printf( "%08x, ", n );
        }
        printf( "\n" );

        // reseed with a value which is different on each run
        time_t seconds;
        time( &seconds );
        rnd_pcg_seed( &pcg, (RND_U32) seconds ); 

        // print another handful of random integers
        // these will be different on every run
        for( int i = 0; i < 5; ++i ) {
            RND_U32 n = rnd_pcg_next( &pcg );
            printf( "%08x, ", n );
        }
        printf( "\n" );


        // print a handful of random floats
        for( int i = 0; i < 5; ++i ) {
            float f = rnd_pcg_nextf( &pcg );
            printf( "%f, ", f );
        }
        printf( "\n" );

        // print random integers in the range 1 to 6
        for( int i = 0; i < 15; ++i ) {
            int r = rnd_pcg_range( &pcg, 1, 6 );
            printf( "%d, ", r );
        }
        printf( "\n" );

        return 0;
    }


API Documentation
-----------------

rnd.h is a single-header library, and does not need any .lib files or other binaries, or any build scripts. To use it,
you just include rnd.h to get the API declarations. To get the definitions, you must include rnd.h from *one* single C 
or C++ file, and #define the symbol `RND_IMPLEMENTATION` before you do. 

The library is meant for general-purpose use, such as games and similar apps. It is not meant to be used for 
cryptography and similar use cases.


### Customization

rnd.h allows for specifying the exact type of 32 and 64 bit unsigned integers to be used in its API. By default, these
default to `unsigned int` and `unsigned long long`, but can be redefined by #defining RND_U32 and RND_U64 respectively
before including rnd.h. This is useful if you, for example, use the types from `<stdint.h>` in the rest of your program, 
and you want rnd.h to use compatible types. In this case, you would include rnd.h using the following code:

    #define RND_U32 uint32_t
    #define RND_U64 uint64_t
    #include "rnd.h"

Note that when customizing the data type, you need to use the same definition in every place where you include rnd.h, 
as it affect the declarations as well as the definitions.


### The generators

The library includes four different generators: PCG, WELL, GameRand and XorShift. They all have different 
characteristics, and you might want to use them for different things. GameRand is very fast, but does not give a great
distribution or period length. XorShift is the only one returning a 64-bit value. WELL is an improvement of the often
used Mersenne Twister, and has quite a large internal state. PCG is small, fast and has a small state. If you don't
have any specific reason, you may default to using PCG.

All generators expose their internal state, so it is possible to save this state and later restore it, to resume the 
random sequence from the same point.


#### PCG - Permuted Congruential Generator

PCG is a family of simple fast space-efficient statistically good algorithms for random number generation. Unlike many 
general-purpose RNGs, they are also hard to predict.

More information can be found here: 

http://www.pcg-random.org/


#### WELL - Well Equidistributed Long-period Linear

Random number generation, using the WELL algorithm by F. Panneton, P. L'Ecuyer and M. Matsumoto.
More information in the original paper: 

http://www.iro.umontreal.ca/~panneton/WELLRNG.html

This code is originally based on WELL512 C/C++ code written by Chris Lomont (published in Game Programming Gems 7) 
and placed in the public domain. 

http://lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf


#### GameRand

Based on the random number generator by Ian C. Bullard:

http://www.redditmirror.cc/cache/websites/mjolnirstudios.com_7yjlc/mjolnirstudios.com/IanBullard/files/79ffbca75a75720f066d491e9ea935a0-10.html

GameRand is a random number generator based off an "Image of the Day" posted by Stephan Schaem. More information here:

http://www.flipcode.com/archives/07-15-2002.shtml


#### XorShift 

A random number generator of the type LFSR (linear feedback shift registers). This specific implementation uses the
XorShift+ variation, and returns 64-bit random numbers.

More information can be found here: 

https://en.wikipedia.org/wiki/Xorshift



rnd_pcg_seed
------------

    void rnd_pcg_seed( rnd_pcg_t* pcg, RND_U32 seed )

Initialize a PCG generator with the specified seed. The generator is not valid until it's been seeded.


rnd_pcg_next
------------

    RND_U32 rnd_pcg_next( rnd_pcg_t* pcg )

Returns a random number N in the range: 0 <= N <= 0xffffffff, from the specified PCG generator.


rnd_pcg_nextf
-------------

    float rnd_pcg_nextf( rnd_pcg_t* pcg )

Returns a random float X in the range: 0.0f <= X < 1.0f, from the specified PCG generator.


rnd_pcg_range
-------------

    int rnd_pcg_range( rnd_pcg_t* pcg, int min, int max )

Returns a random integer N in the range: min <= N <= max, from the specified PCG generator.


rnd_well_seed
-------------

    void rnd_well_seed( rnd_well_t* well, RND_U32 seed )

Initialize a WELL generator with the specified seed. The generator is not valid until it's been seeded.


rnd_well_next
-------------

    RND_U32 rnd_well_next( rnd_well_t* well )

Returns a random number N in the range: 0 <= N <= 0xffffffff, from the specified WELL generator.


rnd_well_nextf
--------------
    float rnd_well_nextf( rnd_well_t* well )

Returns a random float X in the range: 0.0f <= X < 1.0f, from the specified WELL generator.


rnd_well_range
--------------

    int rnd_well_range( rnd_well_t* well, int min, int max )

Returns a random integer N in the range: min <= N <= max, from the specified WELL generator.


rnd_gamerand_seed
-----------------

    void rnd_gamerand_seed( rnd_gamerand_t* gamerand, RND_U32 seed )

Initialize a GameRand generator with the specified seed. The generator is not valid until it's been seeded.


rnd_gamerand_next
-----------------

    RND_U32 rnd_gamerand_next( rnd_gamerand_t* gamerand )

Returns a random number N in the range: 0 <= N <= 0xffffffff, from the specified GameRand generator.


rnd_gamerand_nextf
------------------

    float rnd_gamerand_nextf( rnd_gamerand_t* gamerand )

Returns a random float X in the range: 0.0f <= X < 1.0f, from the specified GameRand generator.


rnd_gamerand_range
------------------

    int rnd_gamerand_range( rnd_gamerand_t* gamerand, int min, int max )

Returns a random integer N in the range: min <= N <= max, from the specified GameRand generator.


rnd_xorshift_seed
-----------------

    void rnd_xorshift_seed( rnd_xorshift_t* xorshift, RND_U64 seed )

Initialize a XorShift generator with the specified seed. The generator is not valid until it's been seeded.


rnd_xorshift_next
-----------------

    RND_U64 rnd_xorshift_next( rnd_xorshift_t* xorshift )

Returns a random number N in the range: 0 <= N <= 0xffffffffffffffff, from the specified XorShift generator.


rnd_xorshift_nextf
------------------

    float rnd_xorshift_nextf( rnd_xorshift_t* xorshift )

Returns a random float X in the range: 0.0f <= X < 1.0f, from the specified XorShift generator.


rnd_xorshift_range
------------------

    int rnd_xorshift_range( rnd_xorshift_t* xorshift, int min, int max )

Returns a random integer N in the range: min <= N <= max, from the specified XorShift generator.


*/


/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef RND_IMPLEMENTATION
#undef RND_IMPLEMENTATION

// Convert a randomized RND_U32 value to a float value x in the range 0.0f <= x < 1.0f. Contributed by Jonatan Hedborg
static float rnd_internal_float_normalized_from_u32( RND_U32 value )
    {
    RND_U32 exponent = 127;
    RND_U32 mantissa = value >> 9;
    RND_U32 result = ( exponent << 23 ) | mantissa;
    float fresult = *(float*)( &result );
    return fresult - 1.0f;
    }


static RND_U32 rnd_internal_murmur3_avalanche32( RND_U32 h )
    {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
    }


static RND_U64 rnd_internal_murmur3_avalanche64( RND_U64 h )
    {
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccd;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53;
    h ^= h >> 33;
    return h;
    }


void rnd_pcg_seed( rnd_pcg_t* pcg, RND_U32 seed )
    {
    RND_U64 value = ( ( (RND_U64) seed ) << 1ULL ) | 1ULL;
    value = rnd_internal_murmur3_avalanche64( value );
    pcg->state[ 0 ] = 0U;
    pcg->state[ 1 ] = ( value << 1ULL ) | 1ULL;
    rnd_pcg_next( pcg );
    pcg->state[ 0 ] += rnd_internal_murmur3_avalanche64( value );
    rnd_pcg_next( pcg );
    }


RND_U32 rnd_pcg_next( rnd_pcg_t* pcg )
    {
    RND_U64 oldstate = pcg->state[ 0 ];
    pcg->state[ 0 ] = oldstate * 0x5851f42d4c957f2dULL + pcg->state[ 1 ];
    RND_U32 xorshifted = (RND_U32)( ( ( oldstate >> 18ULL)  ^ oldstate ) >> 27ULL );
    RND_U32 rot = (RND_U32)( oldstate >> 59ULL );
    return ( xorshifted >> rot ) | ( xorshifted << ( ( -(int) rot ) & 31 ) );
    }


float rnd_pcg_nextf( rnd_pcg_t* pcg )
    {
    return rnd_internal_float_normalized_from_u32( rnd_pcg_next( pcg ) );
    }


int rnd_pcg_range( rnd_pcg_t* pcg, int min, int max )
    {
    int const range = ( max - min ) + 1;
    if( range <= 0 ) return min;
    int const value = (int) ( rnd_pcg_nextf( pcg ) * range );
    return min + value; 
    }


void rnd_well_seed( rnd_well_t* well, RND_U32 seed )
    {
    RND_U32 value = rnd_internal_murmur3_avalanche32( ( seed << 1U ) | 1U );
    well->state[ 16 ] = 0;
    well->state[ 0 ] = value ^ 0xf68a9fc1U;
    for( int i = 1; i < 16; ++i ) 
        well->state[ i ] = ( 0x6c078965U * ( well->state[ i - 1 ] ^ ( well->state[ i - 1 ] >> 30 ) ) + i ); 
    }


RND_U32 rnd_well_next( rnd_well_t* well )
    {
    RND_U32 a = well->state[ well->state[ 16 ] ];
    RND_U32 c = well->state[ ( well->state[ 16 ] + 13 ) & 15 ];
    RND_U32 b = a ^ c ^ ( a << 16 ) ^ ( c << 15 );
    c = well->state[ ( well->state[ 16 ] + 9 ) & 15 ];
    c ^= ( c >> 11 );
    a = well->state[ well->state[ 16 ] ] = b ^ c;
    RND_U32 d = a ^ ( ( a << 5 ) & 0xda442d24U );
    well->state[ 16 ] = (well->state[ 16 ] + 15 ) & 15;
    a = well->state[ well->state[ 16 ] ];
    well->state[ well->state[ 16 ] ] = a ^ b ^ d ^ ( a << 2 ) ^ ( b << 18 ) ^ ( c << 28 );
    return well->state[ well->state[ 16 ] ];
    }


float rnd_well_nextf( rnd_well_t* well )
    {
    return rnd_internal_float_normalized_from_u32( rnd_well_next( well ) );
    }


int rnd_well_range( rnd_well_t* well, int min, int max )
    {
    int const range = ( max - min ) + 1;
    if( range <= 0 ) return min;
    int const value = (int) ( rnd_well_nextf( well ) * range );
    return min + value; 
    }


void rnd_gamerand_seed( rnd_gamerand_t* gamerand, RND_U32 seed )
    {
    RND_U32 value = rnd_internal_murmur3_avalanche32( ( seed << 1U ) | 1U );
    gamerand->state[ 0 ] = value;
    gamerand->state[ 1 ] = value ^ 0x49616e42U;
    }


RND_U32 rnd_gamerand_next( rnd_gamerand_t* gamerand )
    {
    gamerand->state[ 0 ] = ( gamerand->state[ 0 ] << 16 ) + ( gamerand->state[ 0 ] >> 16 );
    gamerand->state[ 0 ] += gamerand->state[ 1 ];
    gamerand->state[ 1 ] += gamerand->state[ 0 ];
    return gamerand->state[ 0 ];
    }


float rnd_gamerand_nextf( rnd_gamerand_t* gamerand )
    {
    return rnd_internal_float_normalized_from_u32( rnd_gamerand_next( gamerand ) );
    }


int rnd_gamerand_range( rnd_gamerand_t* gamerand, int min, int max )
    {
    int const range = ( max - min ) + 1;
    if( range <= 0 ) return min;
    int const value = (int) ( rnd_gamerand_nextf( gamerand ) * range );
    return min + value; 
    }


void rnd_xorshift_seed( rnd_xorshift_t* xorshift, RND_U64 seed )
    {
    RND_U64 value = rnd_internal_murmur3_avalanche64( ( seed << 1ULL ) | 1ULL );
    xorshift->state[ 0 ] = value;
    value = rnd_internal_murmur3_avalanche64( value );
    xorshift->state[ 1 ] = value;
    }


RND_U64 rnd_xorshift_next( rnd_xorshift_t* xorshift )
    {
    RND_U64 x = xorshift->state[ 0 ];
    RND_U64 const y = xorshift->state[ 1 ];
    xorshift->state[ 0 ] = y;
    x ^= x << 23;
    x ^= x >> 17;
    x ^= y ^ ( y >> 26 );
    xorshift->state[ 1 ] = x;
    return x + y;
    }


float rnd_xorshift_nextf( rnd_xorshift_t* xorshift )
    {
    return rnd_internal_float_normalized_from_u32( (RND_U32)( rnd_xorshift_next( xorshift ) >> 32 ) );
    }


int rnd_xorshift_range( rnd_xorshift_t* xorshift, int min, int max )
    {
    int const range = ( max - min ) + 1;
    if( range <= 0 ) return min;
    int const value = (int) ( rnd_xorshift_next( xorshift ) * range );
    return min + value; 
   }



#endif /* RND_IMPLEMENTATION */

/*
revision history:
    1.0     first publicly released version 
*/

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.
Based on public domain implementation - original licenses can be found next to 
the relevant implementation sections of this file.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2016 Mattias Gustavsson

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