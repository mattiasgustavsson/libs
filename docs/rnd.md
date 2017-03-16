rnd.h
=====

Library: [rnd.h](../rnd.h)


Example
=======

A basic example showing how to use the PCG set of random functions.

```cpp
#define  RND_IMPLEMENTATION
#include "rnd.h"

#include <stdio.h> // for printf
#include <time.h> // for time

int main( int argc, char** argv )
	{
	(void) argc, argv;

	rnd_pcg_t pcg;
	rnd_pcg_seed( &pcg, 0u ); // initialize generator

	// print a handful of random integers
	// these will be the same on every run, as we 
	// seeded the rng with a fixed value
	for( int i = 0; i < 5; ++i ) 
		{
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
	for( int i = 0; i < 5; ++i ) 
		{
		RND_U32 n = rnd_pcg_next( &pcg );
		printf( "%08x, ", n );
		}
	printf( "\n" );


	// print a handful of random floats
	for( int i = 0; i < 5; ++i ) 
		{
		float f = rnd_pcg_nextf( &pcg );
		printf( "%f, ", f );
		}
	printf( "\n" );

	// print random integers in the range 1 to 6
	for( int i = 0; i < 15; ++i ) 
		{
		int r = rnd_pcg_range( &pcg, 1, 6 );
		printf( "%d, ", r );
		}
	printf( "\n" );

	return 0;
	}
```


API Documentation
=================

rnd.h is a single-header library, and does not need any .lib files or other binaries, or any build scripts. To use it,
you just include rnd.h to get the API declarations. To get the definitions, you must include rnd.h from *one* single C 
or C++ file, and #define the symbol `RND_IMPLEMENTATION` before you do. 

The library is meant for general-purpose use, such as games and similar apps. It is not meant to be used for 
cryptography and similar use cases.


Customization
-------------
rnd.h allows for specifying the exact type of 32 and 64 bit unsigned integers to be used in its API. By default, these
default to `unsigned int` and `unsigned long long`, but can be redefined by #defining RND_U32 and RND_U64 respectively
before including rnd.h. This is useful if you, for example, use the types from `<stdint.h>` in the rest of your program, 
and you want rnd.h to use compatible types. In this case, you would include rnd.h using the following code:

    #define RND_U32 uint32_t
    #define RND_U64 uint64_t
    #include "rnd.h"

Note that when customizing the data type, you need to use the same definition in every place where you include rnd.h, 
as it affect the declarations as well as the definitions.


The generators
--------------

The library includes four different generators: PCG, WELL, GameRand and XorShift. They all have different 
characteristics, and you might want to use them for different things. GameRand is very fast, but does not give a great
distribution or period length. XorShift is the only one returning a 64-bit value. WELL is an improvement of the often
used Mersenne Twister, and has quite a large internal state. PCG is small, fast and has a small state. If you don't
have any specific reason, you may default to using PCG.

All generators expose their internal state, so it is possible to save this state and later restore it, to resume the 
random sequence from the same point.


### PCG - Permuted Congruential Generator

PCG is a family of simple fast space-efficient statistically good algorithms for random number generation. Unlike many 
general-purpose RNGs, they are also hard to predict.

More information can be found here: http://www.pcg-random.org/


### WELL - Well Equidistributed Long-period Linear

Random number generation, using the WELL algorithm by F. Panneton, P. L'Ecuyer and M. Matsumoto.
More information in the original paper: http://www.iro.umontreal.ca/~panneton/WELLRNG.html

This code is originally based on WELL512 C/C++ code written by Chris Lomont (published in Game Programming Gems 7) 
and placed in the public domain. http://lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf


### GameRand

Based on the random number generator by Ian C. Bullard:
http://www.redditmirror.cc/cache/websites/mjolnirstudios.com_7yjlc/mjolnirstudios.com/IanBullard/files/79ffbca75a75720f066d491e9ea935a0-10.html

GameRand is a random number generator based off an "Image of the Day" posted by Stephan Schaem. More information here:
http://www.flipcode.com/archives/07-15-2002.shtml


### XorShift 

A random number generator of the type LFSR (linear feedback shift registers). This specific implementation uses the
XorShift+ variation, and returns 64-bit random numbers.

More information can be found here: https://en.wikipedia.org/wiki/Xorshift



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

