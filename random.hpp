/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

random.hpp - v1.0 - Pseudo-random number generators and helper functions.

Do this:
	#define RANDOM_IMPLEMENTATION
before you include this file in *one* C++ file to create the implementation.

Additional Contributors
  Jonatan Hedborg: u32 to normalized float conversion
*/

#ifndef random_hpp
#define random_hpp

namespace random {


#ifdef RANDOM_U32
    typedef RANDOM_U32 u32;
#else 
	typedef unsigned int u32;
#endif

#ifdef RANDOM_U64
	typedef RANDOM_U64 u64;
#else
    typedef unsigned long long u64;
#endif

// rng types
struct pcg;
struct well;
struct gamerand;
struct xorshift;

#ifdef RANDOM_DEFAULT_RNG
    typedef RANDOM_DEFAULT_RNG default_rng;
#else
    typedef pcg default_rng;
#endif


// Returns a random number N in the range: 0 <= N <= 0xffffffff
template< typename R > u32 next( R& rng );
template< typename R > u32 next();
inline u32 next();

// Returns a random float X in the range: 0.0f <= X < 1.0f
template< typename R > float nextf( R& rng );
template< typename R > float nextf();
inline float nextf();

// Seeds the rng with the specified seed value
template< typename R > void seed( R& rng, u32 seed );
template< typename R > void seed( u32 value );
inline void seed( u32 seed );

// Returns a random integer N in the range: min <= N <= max
template< typename R > int range( R& rng, int min, int max );
template< typename R > int range( int min, int max );
inline int range( int min, int max );

// Returns a random float X in the range: 0.0f <= X < 1.0f, distributed along a rough approximation of a bell-curve
template< typename R > float bell_curve( R& rng, int iterations = 3 );
template< typename R > float bell_curve( int iterations = 3 );
inline float bell_curve( int iterations = 3 );

// Returns a random integer N in the range: min <= N <= max, distributed along a rough approximation of a bell-curve
template< typename R > int bell_curve( R& rng, int min, int max, int iterations = 3 );
template< typename R > int bell_curve( int min, int max, int iterations = 3 );
inline int bell_curve( int min, int max, int iterations = 3 );

// Randomizes the order of elements in the specified array
template< typename R, typename T > void shuffle( R& rng, T* array, int count );
template< typename R, typename T > void shuffle( T* array, int count );
template< typename T > void shuffle( T* array, int count );

// Returns a random three-dimensional unit vector of type T ( T must have a constructor which takes three floats )
template< typename R, typename T > T unit_vector( R& rng );
template< typename R, typename T > T unit_vector();
template< typename T > T unit_vector();


// Permuted Congruential Generator
struct pcg
	{
	static pcg global;

	pcg();

	void seed( u32 seed );
	u32 next();
	float nextf();

	inline int state_size() { return sizeof( state_ ); }
	inline void* state() { return (void*) state_; }

	pcg const& operator=( pcg const& other );

	private:
        u64 state_[ 2 ];
	};


// Well Equidistributed Long-period Linear
struct well
	{
	static well global;

	well();

	void seed( u32 seed );
	u32 next();
	float nextf();

	inline int state_size() { return sizeof( state_ ); }
	inline void* state() { return (void*) state_; }

	well const& operator=( well const& other );

	private:
		u32 state_[ 17 ];
	};


// Gamerand rng
struct gamerand
	{
	static gamerand global;

	gamerand();

	void seed( u32 seed );
	u32 next();

	float nextf();

	inline int state_size() { return sizeof( state_ ); }
	inline void* state() { return (void*) state_; }

	gamerand const& operator=( gamerand const& );

    private:
		u32 state_[ 2 ];

	};


// Xor shift rng
struct xorshift
	{
	static xorshift global;

	xorshift();

	void seed( u64 seed );
	u64 next();
	float nextf();

	inline int state_size() { return sizeof( state_ ); }
	inline void* state() { return (void*) state_; }

	xorshift const& operator=( xorshift const& );

	private:
		u64 state_[ 2 ];
	};


} /* namespace random */

#endif /* random_hpp */


/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifndef random_impl
#define random_impl

namespace random {

template< typename R > u32 next( R& rng ) { return (u32) rng.next(); }
template< typename R > u32 next() { return next( R::global ); }
inline u32 next() { return next<default_rng>(); }

template< typename R > float nextf( R& rng ) { return rng.nextf(); }
template< typename R > float nextf() { return nextf( R::global ); }
inline float nextf() { return nextf<default_rng>(); }

template< typename R > void seed( R& rng, u32 value ) { rng.seed( value ); }
template< typename R > void seed( u32 value ) { seed( R::global, value ); }
inline void seed( u32 value ) { seed<default_rng>( value ); }


template< typename R > 
int range( R& rng, int min, int max )
	{
	int const range = ( max - min ) + 1;
	if( range <= 0 ) return min;
	int const value = (int) ( rng.nextf() * range );
    return min + value; 
	}


template< typename R > int range( int min, int max ) { return range( R::global, min, max ); }

inline int range( int min, int max ) { return range<default_rng>( min, max ); }


template< typename R > 
float bell_curve( R& rng, int iterations )
	{
	float sum = 0.0f;
	for( int i = 0; i < iterations; ++i )
		sum += rng.nextf();
	return sum / (float) iterations;
	}


template< typename R > float bell_curve( int iterations ) { return bell_curve<R>( R::global, iterations ); }

inline float bell_curve( int iterations ) { return bell_curve<default_rng>( iterations ); }


template< typename R > 
int bell_curve( R& rng, int min, int max, int iterations )
	{
	int const range = ( max - min ) + 1;
	if( range <= 0 ) return min;
	int const value = (int) ( bell_curve( rng, iterations ) * range );
    return min + value; 
	}


template< typename R > int bell_curve( int min, int max, int iterations ) { return bell_curve( R::global, min, max, iterations ); }

inline int bell_curve( int min, int max, int iterations ) { return bell_curve<default_rng>( min, max, iterations ); }


template< typename R, typename T > 
void shuffle( R& rng, T* const array, int count )
	{
	for( int i = 0; i < count; ++i )
		{
		int const r = range( rng, i, count - 1 );
		T const temp = array[ i ];
		array[ i ] = array[ r ];
		array[ r ] = temp;
		}
	}

template< typename R, typename T > void shuffle( T* const array, int count ) { shuffle( R::global, array, count ); }

template< typename T > void shuffle( T* const array, int count ) { shuffle<default_rng>( array, count ); }


template< typename R, typename T > 
T unit_vector( R& rng )
	{
	float const TWO_PI = 6.283185307179586476925286766559f;

	float phi = rng.nextf() * TWO_PI;
    float costheta = rng.nextf() * 2.0f - 1.0f;
    float theta = acosf( costheta );
 
	float x = sinf( theta ) * cosf( phi );
    float y = sinf( theta ) * sinf( phi );
    float z = cosf( theta );

    return T( x, y, z );
	}


template< typename R, typename T > T unit_vector() { return unit_vector<R, T>( R::global ); }

template< typename T > T unit_vector() { return default_rng<default_rng, T>( default_rng::global ); }

} /* namespace random */

#endif random_impl


#ifdef RANDOM_IMPLEMENTATION

namespace random {

// Convert a randomized u32 value to a float value x in the range 0.0f <= x < 1.0f. Contributed by Jonatan Hedborg
static float float_normalized_from_u32( u32 value )
	{
    u32 exponent = 127;
    u32 mantissa = value >> 9;
    u32 result = ( exponent << 23 ) | mantissa;
    float fresult = *(float*)( &result );
    return fresult - 1.0f;
	}


static u32 murmur3_avalanche( u32 h )
	{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
	}

static u64 murmur3_avalanche( u64 h )
	{
	h ^= h >> 33;
	h *= 0xff51afd7ed558ccd;
	h ^= h >> 33;
	h *= 0xc4ceb9fe1a85ec53;
	h ^= h >> 33;
	return h;
	}

/*
PCG

PCG is a family of simple fast space-efficient statistically good algorithms for random number generation. 
Unlike many general-purpose RNGs, they are also hard to predict.

http://www.pcg-random.org/
*/

pcg pcg::global;

void pcg::seed( u32 value )
	{
    u64 s64 = ( ( (u64) value ) << 1ULL ) | 1ULL;
    s64 = murmur3_avalanche( s64 );
    state_[ 0 ] = 0U;
    state_[ 1 ] = ( s64 << 1ULL ) | 1ULL;
    next();
    state_[ 0 ] += murmur3_avalanche( s64 );
    next();
	}


u32 pcg::next()
	{
    u64 oldstate = state_[ 0 ];
    state_[ 0 ] = oldstate * 0x5851f42d4c957f2dULL + state_[ 1 ];
    u32 xorshifted = (u32)( ((oldstate >> 18ULL) ^ oldstate) >> 27ULL );
    u32 rot = (u32)( oldstate >> 59ULL );
    return (xorshifted >> rot) | (xorshifted << ((-(int)rot) & 31));
	}


float pcg::nextf()
	{
    return float_normalized_from_u32( next() );
	}


pcg::pcg()
	{
	state_[ 0 ] = 0x09ccbd8655572fb8ULL;
	state_[ 1 ] = 0x68ad79f869859659ULL;
	}


pcg const& pcg::operator=( pcg const& other ) 
	{ 
	for( int i = 0; i < sizeof( state_ ) / sizeof( *state_ ); ++i ) 
		state_[ i ] = other.state_[ i ]; 
	return *this; 
	}



/*
WELL

Random number generation, using the WELL algorithm by F. Panneton, P. L'Ecuyer and M. Matsumoto.
More information in the original paper: http://www.iro.umontreal.ca/~panneton/WELLRNG.html

This code is originally based on WELL512 C/C++ code written by Chris Lomont (published in Game Programming Gems 7) 
and placed in the public domain. http://lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf
*/

well well::global;

void well::seed( u32 value )
	{
	value = murmur3_avalanche( ( value << 1U ) | 1U );
	state_[ 16 ] = 0;
    state_[ 0 ] = value ^ 0xf68a9fc1U;
    for( int i = 1; i < 16; ++i ) 
        state_[ i ] = ( 0x6c078965U * ( state_[ i - 1 ] ^ ( state_[ i - 1 ] >> 30 ) ) + i ); 
	}


u32 well::next()
	{
    u32 a = state_[ state_[ 16 ] ];
    u32 c = state_[ ( state_[ 16 ] + 13 ) & 15 ];
    u32 b = a ^ c ^ ( a << 16 ) ^ ( c << 15 );
    c = state_[ ( state_[ 16 ] + 9 ) & 15 ];
    c ^= ( c >> 11 );
    a = state_[ state_[ 16 ] ] = b ^ c;
    u32 d = a ^ ( ( a << 5 ) & 0xda442d24U );
    state_[ 16 ] = (state_[ 16 ] + 15 ) & 15;
    a = state_[ state_[ 16 ] ];
    state_[ state_[ 16 ] ] = a ^ b ^ d ^ ( a << 2 ) ^ ( b << 18 ) ^ ( c << 28 );
    return state_[ state_[ 16 ] ];
	}


float well::nextf()
	{
    return float_normalized_from_u32( next() );
	}


well::well()
	{
	u32 const defaults[ 17 ] = 
		{
		0xa7c4b776U, 0x6af574c5U, 0x6faff556U, 0xa4185a56U, 0xae3e9728U, 0x17bd1d97U, 0x15937b99U, 0x5d83a464U, 
		0x72ace8e1U, 0xa4f7c069U, 0x12a52d41U, 0x6929a3b0U, 0xebde4de1U, 0x7f90ac37U, 0x3db6d75cU, 0xe5e7335bU, 
		0x00000000U, 
		};

	for( int i = 0; i < 17; ++i )
		state_[ i ] = defaults[ i ];
	}


well const& well::operator=( well const& other ) 
	{ 
	for( int i = 0; i < sizeof( state_ ) / sizeof( *state_ ); ++i ) 
		state_[ i ] = other.state_[ i ]; 
	return *this; 
	}


/*
GameRand

Based on the random number generator by Ian C. Bullard:
http://www.redditmirror.cc/cache/websites/mjolnirstudios.com_7yjlc/mjolnirstudios.com/IanBullard/files/79ffbca75a75720f066d491e9ea935a0-10.html

GameRand is a random number generator based off an "Image of the Day" posted by Stephan Schaem
http://www.flipcode.com/archives/07-15-2002.shtml
http://stackoverflow.com/questions/1046714/what-is-a-good-random-number-generator-for-a-game
*/

gamerand gamerand::global;


void gamerand::seed( u32 value )
	{
	value = murmur3_avalanche( ( value << 1U ) | 1U );
    state_[ 0 ] = value;
    state_[ 1 ] = value ^ 0x49616e42U;
	}


u32 gamerand::next()
	{
    state_[ 0 ] = ( state_[ 0 ] << 16 ) + ( state_[ 0 ] >> 16 );
    state_[ 0 ] += state_[ 1 ];
    state_[ 1 ] += state_[ 0 ];
    return state_[ 0 ];
	}


float gamerand::nextf()
	{
    return float_normalized_from_u32( next() );
	}


gamerand::gamerand()
	{
	state_[ 0 ] = 0x514e28b7U;
	state_[ 1 ] = 0x182f46f5U;
	}


gamerand const& gamerand::operator=( gamerand const& other ) 
	{ 
	for( int i = 0; i < sizeof( state_ ) / sizeof( *state_ ); ++i ) 
		state_[ i ] = other.state_[ i ]; 
	return *this; 
	}


/* 
XorShift+

https://en.wikipedia.org/wiki/Xorshift#Xorshift.2B
*/

xorshift xorshift::global;


void xorshift::seed( u64 value )
	{
	value = murmur3_avalanche( ( value << 1ULL ) | 1ULL );
	state_[ 0 ] = value;
	value = murmur3_avalanche( value );
	state_[ 1 ] = value;
	}


u64 xorshift::next()
	{
	u64 x = state_[ 0 ];
	u64 const y = state_[ 1 ];
	state_[ 0 ] = y;
	x ^= x << 23;
	x ^= x >> 17;
	x ^= y ^ ( y >> 26 );
	state_[ 1 ] = x;
	return x + y;
    }


float xorshift::nextf()
	{
    return float_normalized_from_u32( (u32)( next() >> 32 ) );
	}


xorshift::xorshift()
	{
	state_[ 0 ] = 0xb456bcfc34c2cb2cULL;
	state_[ 1 ] = 0x7d6e4ac38b2b1be2ULL;
	}


xorshift const& xorshift::operator=( xorshift const& other ) 
	{ 
	for( int i = 0; i < sizeof( state_ ) / sizeof( *state_ ); ++i ) 
		state_[ i ] = other.state_[ i ]; 
	return *this; 
	}


} /* namespace random */

#endif /* RANDOM_IMPLEMENTATION */

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
