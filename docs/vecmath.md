vecmath.h
=========

Library: [vecmath.h](../vecmath.h)


vecmath.h is a vector/matrix math library for C/C++, with some additional 
helpers specifically for 3D math.

It's designed to support most of the functionality you would expect from
shader languages like GLSL and, primarily, HLSL, with matching behavior.
Other parts of the library have been modelled after the D3DX parts of the
DirectX SDK.

The goal of vecmath.h is to be a complete and comprehensive vector math
library. It makes no use of SIMD intrinsics or similar, but instead just
implements each function in the most straightforward way. It uses no complex
macro acrobatics to shorten the implementations, and no templates or the 
like. Many compilers do a decent job optimizing the functions, but if you 
need maximum speed, you are probably best off doing a custom SIMD intrinsics
implementation for your specific use case - but this also involves structuring
your data to allow for maximum parallelization.

Care has been taken to keep the API consistent and predictable, and most of
the functions should be easy to guess the name of, and it should be obvious
what they do in most cases, as standard terminology is used.

> vecmath.h is primarily designed to be a C library, but some care have been
> taken to make sure it works well when used from C++ too. It makes no claim
> to be using idiomatic "Modern C++" practices, but is instead taking a 
> pragmatic approach, to leverage some C++ features when using the library
> from C++ code. This documentation points out all C++ specific behavior in
> blocks like this one.

> [!NOTE]  
> In C++ all functions and types are wrapped in `namespace vecmath`.


Examples
--------

Here is a sample of how code using the vecmath.h library can look (this is
taken from a software renderer doing per-pixel lighting, as you would 
typically see in a GPU shader written in HLSL or GLSL):

	vec3_t lighting( vec3_t albedo, vec3_t l, vec3_t n, vec3_t view, float spec, float gloss, float ambocc, vec3_t env ) {
		vec3_t sky = vec3( 0.95f, 0.95f, 0.99f );
		vec3_t ground= vec3( 0.3f, 0.2f, 0.1f );
		vec3_t lightcol= vec3( 0.9f, 0.85f, 0.8f );

		vec3_t r = normalize( 2.0f *  dot(n, l) * n - l); 
		float sn = pow( 2.0f, 8.0f * gloss );
		float s = pow( saturate( dot( r, view ) ), sn ) * ( ( sn + 2.0f ) / ( 2 * VECMATH_PI ) );

		float diffuse = saturate( dot( n, l ) );
		float diffuse_wrapped = saturate( dot( n, l ) * 0.5f + 0.5f );
		vec3_t hemisphere = lerp( ground, sky, n.y * 0.5f + 0.5f ) * 0.5f * ambocc;
		vec3_t directional = lightcol * ( diffuse * 0.75f + diffuse_wrapped * 0.5f );
		vec3_t specular = s * albedo * ambocc;

		vec3_t color = albedo * ( directional + hemisphere ) + specular * 2.0f; 
		color += env * spec;
		return color;
	}

The example above is in C, but it is also using the vecmath.h option to use the
clang extension for vector types, allowing operators `+`, `-`, `*`, `/` to be 
used in C. It is also using the option to define aliases without prefixes for 
nicer vecmath function names.

Without the clang vector type extension, and without stripping prefixes, the same
code would look like this:

	vec3_t lighting( vec3_t albedo, vec3_t l, vec3_t n, vec3_t view, float spec, float gloss, float ambocc, vec3_t env ) {
		vec3_t sky = vec3( 0.95f, 0.95f, 0.99f );
		vec3_t ground= vec3( 0.3f, 0.2f, 0.1f );
		vec3_t lightcol= vec3( 0.9f, 0.85f, 0.8f );

		vec3_t r = vec3_normalize( vec3_fmul( 2.0f, vec3_sub( vec3_fmul( vec3_dot(n, l), n ), l ) ) ); 
		float sn = vecmath_pow( 2.0f, 8.0f * gloss );
		float s = vecmath_pow( vecmath_saturate( vec3_dot( r, view ) ), sn ) * ( ( sn + 2.0f ) / ( 2 * VECMATH_PI ) );

		float diffuse = vecmath_saturate( vec3_dot( n, l ) );
		float diffuse_wrapped = vecmath_saturate( vec3_dot( n, l ) * 0.5f + 0.5f );
		vec3_t hemisphere = vec3_mulf( vec3_lerp( ground, sky, n.y * 0.5f + 0.5f ), 0.5f * ambocc );
		vec3_t directional = vec3_mulf( lightcol, diffuse * 0.75f + diffuse_wrapped * 0.5f );
		vec3_t specular = vec3_fmul( s, vec3_mulf( albedo, ambocc ) );

		vec3_t color = vec3_add( vec3_mul( albedo, vec3_add( directional, hemisphere ) ), vec3_mulf( specular, 2.0f ) ); 
		color = vec3_add( color, vec3_mulf( env, spec ) );
		return color;
	}

Which is admittedly not quite as nice, but still reasonable. Note that both
versions can be compiled with C++ as well.


Customization
-------------

There are a couple of aspects of vecmath.h that can be customized through 
compile-time defines.

### Standard math functions

By default, vecmath.h will include the standard C <math.h> header, and use the
standard math functions defined there. However, you can override this to use
your own equivalents by defining one or more of the following macros:

`VECMATH_ABS`, `VECMATH_ACOS`, `VECMATH_ASIN`, `VECMATH_ATAN`, `VECMATH_ATAN2`, 
`VECMATH_CEIL`, `VECMATH_COS`, `VECMATH_COSH`, `VECMATH_EXP`, `VECMATH_EXP2`, 
`VECMATH_FLOOR`, `VECMATH_FMOD`, `VECMATH_FRAC`, `VECMATH_LOG`, `VECMATH_LOG10`, 
`VECMATH_POW`, `VECMATH_SIN`, `VECMATH_SINH`, `VECMATH_SQRT`, `VECMATH_LOG2`, 
`VECMATH_ROUND`, `VECMATH_TAN`, `VECMATH_TANH`, `VECMATH_TRUNC` 

If you redefine every single one of these, vecmath.h will no longer include
the <math.h> header at all.


### Clang vector type extensions

The clang compiler has an extension named `ext_vector_type`, which allows for
defining a vector type of a specific size, and then be able to use operators
(+, -, *, /) to perform element-wise arithmetic.

You can enable this by defining `VECMATH_EXT_VECTOR_TYPE` before including
vecmath.h. It will only be enabled for compilers supporting it. If the compiler
does not support it, the `VECMATH_EXT_VECTOR_TYPE` define will be ignored.

Note that any usage code leveraging this feature, will of course only compile
in clang.

> In C++, operator overloading is used to achieve the same functionality, and
> there is no need to toggle it with any define.


Types
-----

There are three vector types:

* `vec2_t` two component vector, x/y
* `vec3_t` three component vector, x/y/z
* `vec4_t` four component vector, x/y/z/w

There are nine matrix types. A matrix is made up of a number of rows, where 
each row is a vector type representing the columns of the matrix.

* `mat22_t` matrix with two rows (x/y) of vec2_t
* `mat23_t` matrix with two rows (x/y) of vec3_t
* `mat24_t` matrix with two rows (x/y) of vec4_t

- `mat32_t` matrix with three rows (x/y/z) of vec2_t
- `mat33_t` matrix with three rows (x/y/z) of vec3_t
- `mat34_t` matrix with three rows (x/y/z) of vec4_t

* `mat42_t` matrix with four rows (x/y/z/w) of vec2_t
* `mat43_t` matrix with four rows (x/y/z/w) of vec3_t
* `mat44_t` matrix with four rows (x/y/z/w) of vec4_t

Matrices are *row-major*, which means if you need to pass them on to an API
expecting column-major matrices (such as OpenGL) you need to transpose them
first.

> In C++, there are also class-equivalents of the types, without the `_t` suffix,
> which supports overloaded constructors and operators for a more idiomatic C++
> feel. These are named vec2, vec3, vec4, mat22, mat23, mat24, mat32, mat33, 
> mat34, mat42, mat43 and mat44.

Note that all of these are simple structs of floats, with no padding, meaning
arrays are also just made up of tightly packed floats. Be aware though, that if
you define `VECMATH_EXT_VECTOR_TYPE`, the clang extension to get operator 
overloading, padding of vector types might be introduced. No part of vecmath.h
itself relies on elements being tightly packed, but if your code does and you
enable `VECMATH_EXT_VECTOR_TYPE`, be aware that tight packing of elements is no
longer guaranteed (typically, vec2_t/vec3_t/vec4_t will all be 16 bytes in size).

For all of the types, both vector and matrices, there are a large number of 
functions, operators and constructors defined, following a unified naming 
convention and behavior.


Constructors
------------

All types can be constructed from their elements:

	vec2_t vec2( float x, float y ) 
	vec3_t vec3( float x, float y, float z )
	vec4_t vec4( float x, float y, float z, float w )

	mat22_t mat22( vec2_t x, vec2_t y )
	mat23_t mat23( vec3_t x, vec3_t y )
	mat24_t mat24( vec4_t x, vec4_t y )

	mat32_t mat32( vec2_t x, vec2_t y, vec2_t z ) 
	mat33_t mat33( vec3_t x, vec3_t y, vec3_t z )
	mat34_t mat34( vec4_t x, vec4_t y, vec4_t z )

	mat42_t mat42( vec2_t x, vec2_t y, vec2_t z, vec2_t w )
	mat43_t mat43( vec3_t x, vec3_t y, vec3_t z, vec3_t w )
	mat44_t mat44( vec4_t x, vec4_t y, vec4_t z, vec4_t w )

Note that for vectors, each element is a float, while for matrices, each
element is a vector.

> In C++, this is implemented as a constructor on the vec2, vec3, vec4, mat22, 
> mat23, mat24, mat32, mat33, mat34, mat42, mat43 and mat44 classes, but the
> syntax for constructing is the same.

Both vectors and matrices can also be initialized with a single float value, which
will be propagated to every element of the vector or the matrices, and in the case
of matrices, to every column of every row.

	vec2_t vec2f( float v )
	vec3_t vec3f( float v )
	vec4_t vec4f( float v )

	mat22_t mat22f( float v )
	mat23_t mat23f( float v )
	mat24_t mat24f( float v )
			
	mat32_t mat32f( float v )
	mat33_t mat33f( float v )
	mat34_t mat34f( float v )
			
	mat42_t mat42f( float v )
	mat43_t mat43f( float v )
	mat44_t mat44f( float v )

All of these have the suffix `f` for float.

> In C++, there are also constructor overloads for all classes, allowing for 
> passing a single float, without needing the suffix `f`. Do note that using
> those will of course make it so your code no longer compiles in C. Using 
> the constructor functions with the `f` suffix works in both C and C++.

Additionally, for vec3_t and vec4_t, it is possible to construct them from a
vector with one less component plus a single float, or (in the case of vec4_t)
from two vec2_t.

	vec3_t vec3v2f( vec2_t v, float f )
	vec3_t vec3fv2( float f, vec2_t v )

	vec4_t vec4v3f( vec3_t v, float f )
	vec4_t vec4fv3( float f, vec3_t v ) 
	vec4_t vec4v2( vec2_t a, vec2_t b )

These have various suffixes to indicate which parameters are used - `v2f` for
a two-element vector followed by a single float, or `fv3` for a single float
followed by a three-element vector, etc.

> In C++, there are also constructor overloads for all of these, so you can use
> them without needing the suffixes. Do note that using them will of course make
> it so your code no longer compiles in C. Using the constructor functions with
> the suffixes works in both C and C++.


Accessing elements
------------------

Each element can of course be accessed using `.x`, `.y`, `.z` and `.w`. For 
matrices, individual cells can be accessed using `.x.x`, `.x.y` etc, as would be
expected.

It is also possible to access elements by index. To get the value by index,
the functions are type name with suffix `_get`:

	float vec2_get( vec2_t vec, int index ) 
	float vec3_get( vec3_t vec, int index ) 
	float vec4_get( vec4_t vec, int index ) 

	vec2_t mat22_get( mat22_t m, int row )
	vec3_t mat23_get( mat23_t m, int row )
	vec4_t mat24_get( mat24_t m, int row )

	vec2_t mat32_get( mat32_t m, int row )
	vec3_t mat33_get( mat33_t m, int row )
	vec4_t mat34_get( mat34_t m, int row )

	vec2_t mat42_get( mat42_t m, int row )
	vec3_t mat43_get( mat43_t m, int row )
	vec4_t mat44_get( mat44_t m, int row )

To set a value by index, the functions are type name with suffix `_set`:

	void vec2_set( vec2_t* vec, int index, float f )
	void vec3_set( vec3_t* vec, int index, float f )
	void vec4_set( vec4_t* vec, int index, float f )

	void mat22_set( mat22_t* m, int row, vec2_t v )
	void mat23_set( mat23_t* m, int row, vec3_t v )
	void mat24_set( mat24_t* m, int row, vec4_t v )

	void mat32_set( mat32_t* m, int row, vec2_t v )
	void mat33_set( mat33_t* m, int row, vec3_t v )
	void mat34_set( mat34_t* m, int row, vec4_t v )

	void mat42_set( mat42_t* m, int row, vec2_t v )
	void mat43_set( mat43_t* m, int row, vec3_t v )
	void mat44_set( mat44_t* m, int row, vec4_t v )

> [!CAUTION]
> Do note that accessing elements by index, either using `*_get` or `*_set`,
> do NOT do any range checking on the `index` or `row` parameter. Passing
> an index outside of the valid range for the type you are accessing, will not
> return a valid result, and is likely to cause an access violation.

> In C++, there are also subscript operators, `operator[]` defined for all of
> the classes, allowing you to access elements, for setting or getting, using
> a syntax like `v[0]=2` or `m[3][2]=v[2]` ( the `3` indicates row 3, the `2`
> indicates column 2) etc. These are also NOT bounds checked, so use with care.


Operators
---------

A number of operators are defined, as functions, for all of the types, 
performing element-wise negation, equality-test, addition, multiplication and 
division, with variants supporting element-wise add/sub/mul/div between two of 
the same type as well as any type with a single float.

The operator functions are defined in this general form, where `###` is the 
type name (vec2, vec3, vec4, mat22, mat23, mat24, mat32, mat33, mat34, mat42, 
mat43 or mat44):

	###_t ###_neg( ###_t v )
	int ###_eq( ###_t a, ###_t b )
	###_t ###_add( ###_t a, ###_t b ) 
	###_t ###_sub( ###_t a, ###_t b ) 
	###_t ###_mul( ###_t a, ###_t b ) 
	###_t ###_div( ###_t a, ###_t b ) 
	###_t ###_addf( ###_t a, float s ) 
	###_t ###_subf( ###_t a, float s ) 
	###_t ###_mulf( ###_t a, float s ) 
	###_t ###_divf( ###_t a, float s ) 

For example, for vec2_t the following operators are defined:

	vec2_t vec2_neg( vec2_t v )
	int vec2_eq( vec2_t a, vec2_t b )
	vec2_t vec2_add( vec2_t a, vec2_t b )
	vec2_t vec2_sub( vec2_t a, vec2_t b )
	vec2_t vec2_mul( vec2_t a, vec2_t b )
	vec2_t vec2_div( vec2_t a, vec2_t b )
	vec2_t vec2_addf( vec2_t a, float s )
	vec2_t vec2_subf( vec2_t a, float s )
	vec2_t vec2_mulf( vec2_t a, float s )
	vec2_t vec2_divf( vec2_t a, float s )

Note that for matrix types, the `###_mul` operator is named `###_mul_elem`,
so as to not cause confusion with matrix multiplication functions like
`mat44_mul_mat44` etc (which does full row-by-column multiplication). 

> [!WARNING]  
> The `###_mul_elem` functions (`mat22_mul_elem`, `mat44_mul_elem` etc) does
> NOT perform standard matrix multiplication - they perform element-wise
> multiplication, also known as Hadamard product. A separate set of functions,
> named on the form `###_mul_###`, for example `mat44_mul_mat44`, is provided
> for performing standard matrix multiplication between matrices of compatible
> dimensions (see the section on "Matrix Multiplication").

> For C++, all of these, except for the `###_mul_elem` functions, are also 
> provided as C++ operator overloads. The multiplication operator `operator*`
> is mapped to the matrix multiplication functions, like `mat44_mul_mat44`
> rather than to the element-wise `###_mul_elem` functions. For vectors, 
> multiplication operator maps to the element-wise `###_mul` functions.
>
> For multiplication of vector with matrix, or matrix with vector, the vector
> is treated as a 1xN or Nx1 matrix, and full matrix multiplication is performed.


Element-wise functions
----------------------

There is a large number of functions defined for every type, that perform 
element-wise calculations. Here are their declarations for the vect2_t
type, but the naming is consistent for all other types (both vector and 
matrix types):

	vec2_t vec2_abs( vec2_t m ) 
	vec2_t vec2_acos( vec2_t m ) 
	int vec2_all( vec2_t m ) 
	int vec2_any( vec2_t m ) 
	vec2_t vec2_asin( vec2_t m ) 
	vec2_t vec2_atan( vec2_t m ) 
	vec2_t vec2_atan2( vec2_t y, vec2_t x ) 
	vec2_t vec2_ceil( vec2_t m ) 
	vec2_t vec2_clamp( vec2_t m, vec2_t min_v, vec2_t max_v ) 
	vec2_t vec2_cos( vec2_t m ) 
	vec2_t vec2_cosh( vec2_t m ) 
	vec2_t vec2_degrees( vec2_t m ) 
	vec2_t vec2_exp( vec2_t m ) 
	vec2_t vec2_exp2( vec2_t m ) 
	vec2_t vec2_floor( vec2_t m ) 
	vec2_t vec2_fmod( vec2_t a, vec2_t b ) 
	vec2_t vec2_frac( vec2_t m ) 
	vec2_t vec2_lerp( vec2_t a, vec2_t b, float s ) 
	vec2_t vec2_log( vec2_t m ) 
	vec2_t vec2_log2( vec2_t m ) 
	vec2_t vec2_log10( vec2_t m ) 
	vec2_t vec2_max( vec2_t a, vec2_t b ) 
	vec2_t vec2_min( vec2_t a, vec2_t b ) 
	vec2_t vec2_pow( vec2_t a, vec2_t b ) 
	vec2_t vec2_radians( vec2_t m ) 
	vec2_t vec2_rcp( vec2_t m ) 
	vec2_t vec2_round( vec2_t m ) 
	vec2_t vec2_rsqrt( vec2_t m ) 
	vec2_t vec2_saturate( vec2_t m ) 
	vec2_t vec2_sign( vec2_t m ) 
	vec2_t vec2_sin( vec2_t m ) 
	vec2_t vec2_sinh( vec2_t m ) 
	vec2_t vec2_smoothstep( vec2_t min_v, vec2_t max_v, vec2_t m ) 
	vec2_t vec2_smootherstep( vec2_t min_v, vec2_t max_v, vec2_t m ) 
	vec2_t vec2_sqrt( vec2_t m ) 
	vec2_t vec2_step( vec2_t a, vec2_t b ) 
	vec2_t vec2_tan( vec2_t m ) 
	vec2_t vec2_tanh( vec2_t m ) 
	vec2_t vec2_trunc( vec2_t m ) 

These all perform, per element, the standard mathematical calculation suggested
by each function's name. The behavior is consistent with that of HLSL (or GLSL,
but is primarily modeled after HLSL).

In addition, the following are only defined for vector types (and are defined for
vector types of all three sizes), but not for matrix types:

	float vec2_distancesq( vec2_t a, vec2_t b ) 
	float vec2_distance( vec2_t a, vec2_t b ) 
	float vec2_dot( vec2_t a, vec2_t b ) 
	float vec2_lengthsq( vec2_t v ) 
	float vec2_length( vec2_t v ) 
	vec2_t vec2_normalize( vec2_t v ) 
	vec2_t vec2_reflect( vec2_t i, vec2_t n ) 
	vec2_t vec2_refract( vec2_t i, vec2_t n, float r ) 

and again, they work the same as in HLSL.

Finally, a cross-product function is only defined for vec3_t:

	vec3_t vec3_cross( vec3_t a, vec3_t b )


For completeness, all of these functions are also implemented in a version for
single floats, using a `vecmath_` prefix:

	float vecmath_abs( float v ) 
	float vecmath_acos( float v ) 
	int vecmath_all( float v ) 
	int vecmath_any( float v ) 
	float vecmath_asin( float v ) 
	float vecmath_atan( float v ) 
	float vecmath_atan2( float y, float x ) 
	float vecmath_ceil( float v ) 
	float vecmath_clamp( float v, float min_v, float max_v ) 
	float vecmath_cos( float v ) 
	float vecmath_cosh( float v ) 
	float vecmath_degrees( float v ) 
	float vecmath_distancesq( float a, float b ) 
	float vecmath_distance( float a, float b ) 
	float vecmath_dot( float a, float b ) 
	float vecmath_exp( float v ) 
	float vecmath_exp2( float v ) 
	float vecmath_floor( float v ) 
	float vecmath_fmod( float a, float b ) 
	float vecmath_frac( float v ) 
	float vecmath_lengthsq( float v ) 
	float vecmath_length( float v ) 
	float vecmath_lerp( float a, float b, float s ) 
	float vecmath_log( float v ) 
	float vecmath_log2( float v ) 
	float vecmath_log10( float v ) 
	float vecmath_max( float a, float b ) 
	float vecmath_min( float a, float b ) 
	float vecmath_normalize( float v ) 
	float vecmath_pow( float a, float b ) 
	float vecmath_radians( float v ) 
	float vecmath_rcp( float v ) 
	float vecmath_reflect( float i, float n ) 
	float vecmath_refract( float i, float n, float r ) 
	float vecmath_round( float v ) 
	float vecmath_rsqrt( float v ) 
	float vecmath_saturate( float v ) 
	float vecmath_sign( float v ) 
	float vecmath_sin( float v ) 
	float vecmath_sinh( float v ) 
	float vecmath_smoothstep( float min_v, float max_v, float v ) 
	float vecmath_smootherstep( float min_v, float max_v, float v ) 
	float vecmath_sqrt( float v ) 
	float vecmath_step( float edge, float x ) 
	float vecmath_tan( float v ) 
	float vecmath_tanh( float v ) 
	float vecmath_trunc( float v ) 


Transpose, determinant, inverse and identity matrices
-----------------------------------------------------

Transposing matrices are supported for all matrix types:

	mat22_t mat22_transpose( mat22_t m )
	mat32_t mat23_transpose( mat23_t m )
	mat23_t mat32_transpose( mat32_t m )
	mat33_t mat33_transpose( mat33_t m )
	mat42_t mat24_transpose( mat24_t m )
	mat43_t mat34_transpose( mat34_t m )
	mat24_t mat42_transpose( mat42_t m )
	mat34_t mat43_transpose( mat43_t m )
	mat44_t mat44_transpose( mat44_t m )

Calculating the determinant is supported for square matrices only:

	float mat22_determinant( mat22_t m)
	float mat33_determinant( mat33_t m)
	float mat44_determinant( mat44_t m)

As is computing the inverse:

	int mat22_inverse( mat22_t* out_matrix, float* out_determinant, mat22_t m ) 
	int mat33_inverse( mat33_t* out_matrix, float* out_determinant, mat33_t m ) 
	int mat44_inverse( mat44_t* out_matrix, float* out_determinant, mat44_t m ) 

Note that the inverse functions return 0 if the matrix cannot be inverted, and 
non-zero if inversion was successful. Both the `out_matrix` and the 
`out_determinant` parameters are optional and may be NULL. If non-NULL, 
`out_determinant` will be set, and if non-NULL and inversion is successful, 
`out_matrix` will be set.

There are helper functions to construct the identity matrix for each of the 
square matrix sizes:

	mat22_t mat22_identity( void ) 
	mat33_t mat33_identity( void ) 
	mat44_t mat44_identity( void ) 

and also functions to test if a matrix is exactly identity:

	int mat22_is_identity( mat22_t m ) 
	int mat33_is_identity( mat33_t m ) 
	int mat44_is_identity( mat44_t m ) 

returning non-zero if the matrix is identity, as compared exactly (no 
floating point epsilon is used).


Matrix multiplications
----------------------

Matrix multiplication is supported between all vectors and matrices, in any
valid combination of dimensions, all named on the form `###_mul_###` where 
`###` is the type name of the first and second argument. The return type
depends on the dimensions of the types involved.

All valid combinations, along with their return types, are as follows:

	float vec2_mul_vec2( vec2_t a, vec2_t b )
	float vec3_mul_vec3( vec3_t a, vec3_t b )
	float vec4_mul_vec4( vec4_t a, vec4_t b )

	vec2_t vec2_mul_mat22( vec2_t a, mat22_t b ) 
	vec3_t vec2_mul_mat23( vec2_t a, mat23_t b ) 
	vec4_t vec2_mul_mat24( vec2_t a, mat24_t b ) 
	vec2_t vec3_mul_mat32( vec3_t a, mat32_t b ) 
	vec3_t vec3_mul_mat33( vec3_t a, mat33_t b ) 
	vec4_t vec3_mul_mat34( vec3_t a, mat34_t b ) 
	vec2_t vec4_mul_mat42( vec4_t a, mat42_t b ) 
	vec3_t vec4_mul_mat43( vec4_t a, mat43_t b ) 
	vec4_t vec4_mul_mat44( vec4_t a, mat44_t b ) 

	vec2_t mat22_mul_vec2( mat22_t a, vec2_t b ) 
	vec3_t mat32_mul_vec2( mat32_t a, vec2_t b ) 
	vec4_t mat42_mul_vec2( mat42_t a, vec2_t b ) 
	vec2_t mat23_mul_vec3( mat23_t a, vec3_t b ) 
	vec3_t mat33_mul_vec3( mat33_t a, vec3_t b ) 
	vec4_t mat43_mul_vec3( mat43_t a, vec3_t b ) 
	vec2_t mat24_mul_vec4( mat24_t a, vec4_t b ) 
	vec3_t mat34_mul_vec4( mat34_t a, vec4_t b ) 
	vec4_t mat44_mul_vec4( mat44_t a, vec4_t b )
 
	mat22_t mat22_mul_mat22( mat22_t a, mat22_t b )
	mat23_t mat22_mul_mat23( mat22_t a, mat23_t b )
	mat24_t mat22_mul_mat24( mat22_t a, mat24_t b )
	mat22_t mat23_mul_mat32( mat23_t a, mat32_t b )
	mat23_t mat23_mul_mat33( mat23_t a, mat33_t b )
	mat24_t mat23_mul_mat34( mat23_t a, mat34_t b )
	mat22_t mat24_mul_mat42( mat24_t a, mat42_t b )
	mat23_t mat24_mul_mat43( mat24_t a, mat43_t b )
	mat24_t mat24_mul_mat44( mat24_t a, mat44_t b )

	mat32_t mat32_mul_mat22( mat32_t a, mat22_t b )
	mat33_t mat32_mul_mat23( mat32_t a, mat23_t b )
	mat34_t mat32_mul_mat24( mat32_t a, mat24_t b )
	mat32_t mat33_mul_mat32( mat33_t a, mat32_t b )
	mat33_t mat33_mul_mat33( mat33_t a, mat33_t b )
	mat34_t mat33_mul_mat34( mat33_t a, mat34_t b )
	mat32_t mat34_mul_mat42( mat34_t a, mat42_t b )
	mat33_t mat34_mul_mat43( mat34_t a, mat43_t b )
	mat34_t mat34_mul_mat44( mat34_t a, mat44_t b )

	mat42_t mat42_mul_mat22( mat42_t a, mat22_t b )
	mat43_t mat42_mul_mat23( mat42_t a, mat23_t b )
	mat44_t mat42_mul_mat24( mat42_t a, mat24_t b )
	mat42_t mat43_mul_mat32( mat43_t a, mat32_t b )
	mat43_t mat43_mul_mat33( mat43_t a, mat33_t b )
	mat44_t mat43_mul_mat34( mat43_t a, mat34_t b )
	mat42_t mat44_mul_mat42( mat44_t a, mat42_t b )
	mat43_t mat44_mul_mat43( mat44_t a, mat43_t b )
	mat44_t mat44_mul_mat44( mat44_t a, mat44_t b )

When a vector is multiplied by another vector, this is equivalent to the dot
product. When a vector is multiplied by a matrix, the vector is treated like a
1xN matrix, or row vector. When a matrix is multiplied by a vector, the 
vector is treated like a Nx1 matrix, or a column vector.


Quaternions
-----------

Quaternions are supported, and as they also have four elements, they are 
represented by the `vec4_t` type.

The following functions operate on quaternions:

	vec4_t quat_normalize( vec4_t q ) 
	vec4_t quat_slerp( vec4_t a, vec4_t b, float t ) 
	vec4_t quat_barycentric( vec4_t q1, vec4_t q2, vec4_t q3, float f, float g ) 
	vec4_t quat_conjugate( vec4_t q ) 
	vec4_t quat_exp( vec4_t q ) 
	vec4_t quat_identity( void ) 
	vec4_t quat_inverse( vec4_t q ) 
	int quat_is_identity( vec4_t q ) 
	vec4_t quat_ln( vec4_t q ) 
	vec4_t quat_mul( vec4_t a, vec4_t b ) 
	vec4_t quat_rotation_axis( vec3_t axis, float angle ) 
	vec4_t quat_rotation_matrix( mat44_t m ) 
	vec4_t quat_rotation_yaw_pitch_roll(float yaw, float pitch, float roll) 
	void quat_squad_setup( vec4_t* out_a, vec4_t* out_b, vec4_t* out_c, vec4_t q0, vec4_t q1, vec4_t q2, vec4_t q3 ) 
	vec4_t quat_squad(vec4_t q1, vec4_t a, vec4_t b, vec4_t c, float t ) 
	void quat_to_axis_angle( vec4_t q, vec3_t* out_axis, float* out_angle ) 
	vec3_t quat_rotate_vector( vec3_t v, vec4_t q ) 
	vec4_t quat_shortest_arc( vec3_t from, vec3_t to ) 
	vec4_t quat_from_mat33( mat33_t m ) 
	vec4_t quat_from_mat44( mat44_t m ) 
	float quat_angle( vec4_t a, vec4_t b ) 
	mat33_t mat33_from_quat( vec4_t q ) 
	mat44_t mat44_from_quat( vec4_t q ) 

The functionality and behavior of all of these are consistent with D3DX, 
XNAMath or DirectXMath, and the support they have for quaternions.

> [!NOTE]  
> As there is nothing in the API preventing you from using four-element 
> vectors and quaternions interchangeably, care should be taken to use
> them in the correct way. For example, using `vec4_mul` to multiply two 
> quaternions will give the wrong result - make sure to use `quat_mul` 
> instead.


Matrix 3D utility functions
---------------------------

There are a number of utility functions for creating various matrices used in 3d
rendering (transformation and projection):

	mat44_t mat44_look_at_lh( vec3_t eye, vec3_t at, vec3_t up ) 
	mat44_t mat44_look_at_rh( vec3_t eye, vec3_t at, vec3_t up ) 
	mat44_t mat44_ortho_lh( float w, float h, float zn, float zf ) 
	mat44_t mat44_ortho_rh( float w, float h, float zn, float zf ) 
	mat44_t mat44_ortho_off_center_lh( float l, float r, float b, float t, float zn, float zf ) 
	mat44_t mat44_ortho_off_center_rh( float l, float r, float b, float t, float zn, float zf ) 
	mat44_t mat44_perspective_lh( float w, float h, float zn, float zf ) 
	mat44_t mat44_perspective_rh( float w, float h, float zn, float zf ) 
	mat44_t mat44_perspective_off_center_lh( float l, float r, float b, float t, float zn, float zf ) 
	mat44_t mat44_perspective_off_center_rh( float l, float r, float b, float t, float zn, float zf ) 
	mat44_t mat44_perspective_fov_lh( float fovy, float aspect, float zn, float zf ) 
	mat44_t mat44_perspective_fov_rh( float fovy, float aspect, float zn, float zf ) 
	mat44_t mat44_rotation_axis( vec3_t axis, float angle ) 
	mat44_t mat44_rotation_x( float angle ) 
	mat44_t mat44_rotation_y( float angle ) 
	mat44_t mat44_rotation_z( float angle ) 
	mat44_t mat44_rotation_yaw_pitch_roll( float yaw, float pitch, float roll ) 
	mat44_t mat44_scaling( float sx, float sy, float sz ) 
	mat44_t mat44_translation( float tx, float ty, float tz ) 

These all work the same as in DirectX (and most 3d math libraries).

There's also a function to decompose a transformation matrix into separate scale,
rotation and translation components:
	
	int mat44_decompose( vec3_t* out_scale, vec4_t* out_rotation, vec3_t* out_translation, mat44_t m ) 

Again, this works just like in D3DX, and returns a non-zero value if the matrix
could be successfully decomposed, and returns 0 if decomposition failed. If it
failed, `out_scale`, `out_rotation` and `out_translation` are not modified. It
is valid to pass NULL for any of the out parameters if you do not need that 
result value.

Finally, there are transform helpers mirroring the ones in D3DX:

	vec4_t vec2_transform( vec2_t v, mat44_t m ) 
	vec2_t vec2_transform_coord( vec2_t v, mat44_t m ) 
	vec2_t vec2_transform_normal( vec2_t v, mat44_t m ) 
	vec4_t vec3_transform( vec3_t v, mat44_t m ) 
	vec3_t vec3_transform_coord( vec3_t v, mat44_t m ) 
	vec3_t vec3_transform_normal( vec3_t v, mat44_t m ) 
	vec4_t vec4_transform( vec4_t v, mat44_t m ) 


Vector swizzling
----------------

vecmath.h supports swizzling in the form swizzle-on-read, but does not support
swizzle-on-right, meaning you cannot assign to a swizzled target.

Swizzling is supported by defining separate swizzle functions for every single
combination of swizzling, on vec2/vec3 and vec4 types. That's hundreds of 
functions, and we won't list them here, but let's at least look at the ones 
returning vec2_t (the rest follows the same pattern, but with many more 
variations):

	vec2_t vec2_xx( vec2_t v )
	vec2_t vec2_xy( vec2_t v )
	vec2_t vec2_yx( vec2_t v )
	vec2_t vec2_yy( vec2_t v )
	
	vec2_t vec3_xx( vec3_t v )
	vec2_t vec3_xy( vec3_t v )
	vec2_t vec3_xz( vec3_t v )
	vec2_t vec3_yx( vec3_t v )
	vec2_t vec3_yy( vec3_t v )
	vec2_t vec3_yz( vec3_t v )
	vec2_t vec3_zx( vec3_t v )
	vec2_t vec3_zy( vec3_t v )
	vec2_t vec3_zz( vec3_t v )
	
	vec2_t vec4_xx( vec4_t v )
	vec2_t vec4_xy( vec4_t v )
	vec2_t vec4_xz( vec4_t v )
	vec2_t vec4_xw( vec4_t v )
	vec2_t vec4_yx( vec4_t v )
	vec2_t vec4_yy( vec4_t v )
	vec2_t vec4_yz( vec4_t v )
	vec2_t vec4_yw( vec4_t v )
	vec2_t vec4_zx( vec4_t v )
	vec2_t vec4_zy( vec4_t v )
	vec2_t vec4_zz( vec4_t v )
	vec2_t vec4_zw( vec4_t v )
	vec2_t vec4_wx( vec4_t v )
	vec2_t vec4_wy( vec4_t v )
	vec2_t vec4_wz( vec4_t v )
	vec2_t vec4_ww( vec4_t v )

As we can see, the return type is decided by how many elements we have in the
function suffix (`_xx`, `_yx` etc) - all of the two elements in this example,
but variants exist with more elements as well, for other swizzling combinations.

The function prefix (`vec2_`, `vec3_`, `vec4_`) indicates the parameter type
as would be expected. 

> For C++, swizzling is also supported in the form of member functions on the
> `vec2`, `vec3` and `vec4` classes, allowing swizzling in the form `v.xxx()`,
> `v.yyxx()`, `v.xy` etc. Note that the C swizzling functions are also available
> from C++.


C Generics and C++ overloads
----------------------------

All functions in vecmath.h are prefixed with the name of the type it operates
on (`vec2_`, `vec3_`, `vec4_`, `mat22_` etc ). From C11, there is the new
keyword `_Generic`, allowing for defining a macro dispatcher that allows the
use of the same name to invoke different functions depending on the type of
the arguments. 

In vecmath.h you can define the preprocessor symbol `VECMATH_GENERICS`, and if
you are compiling with C11 or later, a full set of `vm_` macros will be defined
for calling functions without specifying a prefix. It allows you to do things
like `vm_add( a, b )` regardless of which vecmath.h types `a` and `b` are, as
long as they are the same type. 

Unlike most other vm_ functions, which perform element-wise operations, the 
`vm_mul` macro does not wrap the `*_mul_elem` element-wise multiply functions, 
but instead the full matrix multiply `*_mul_*` functions, allowing for 
expressions like `vm_mul( v4, m44 )`, `vm_mul( m44, v44 )`, `vm_mul( m44, m44 )` 
etc.

The full set of generic function names are:

	vm_neg
	vm_eq
	vm_mul
	vm_add
	vm_sub
	vm_div
	vm_abs
	vm_acos
	vm_all
	vm_any
	vm_asin
	vm_atan
	vm_atan2
	vm_ceil
	vm_clamp
	vm_cos
	vm_cosh
	vm_cross
	vm_degrees
	vm_distancesq
	vm_distance
	vm_dot
	vm_exp
	vm_exp2
	vm_floor
	vm_fmod
	vm_frac
	vm_lengthsq
	vm_length
	vm_lerp
	vm_log
	vm_log2
	vm_log10
	vm_max
	vm_min
	vm_normalize
	vm_pow
	vm_radians
	vm_rcp
	vm_reflect
	vm_refract
	vm_round
	vm_rsqrt
	vm_saturate
	vm_sign
	vm_sin
	vm_sinh
	vm_smoothstep
	vm_smootherstep
	vm_sqrt
	vm_step
	vm_tan
	vm_tanh
	vm_trunc
	vm_transpose
	vm_determinant

> In C++, if `VECMATH_GENERICS` is defined, all these `vm_` functions will be
> defined as overloaded functions instead of using the `_Generic` macro for C,
> providing the same functionality as long as `VECMATH_GENERICS` is defined.

### Generics without the prefix

In addition to the `VECMATH_GENERICS` preprocessor symbol, it is also possible
to define the symbol `VECMATH_GENERICS_NO_PREFIX`, to create a set of aliases
for all the `vm_*` macros, that omits the `vm_` prefix, providing more natural
names like `add( a, b )`, `mul( v, m )` etc. 

> [!CAUTION]
> Do note that defining `VECMATH_GENERICS_NO_PREFIX` will redefine names like
> `abs`, `sin`, `cos` etc, which will very much be in direct conflict with
> names from the C standard math library. Use this feature with care, as it can
> very easily lead to some pretty weird compilation errors.

> In C++, when `VECMATH_GENERICS_NO_PREFIX` is defined, all the function 
> overloads are just defined without the prefix in the first place, and instead
> macros are defined to provide the `vm_` prefixed names as aliases. This avoids
> the global redefining of common math.h names, and also places the overloaded
> functions within the `vecmath` namespace, which further avoids collisions. 


Unit tests
----------

vecmath.h contains an extensive test suite, consisting of over 1300 tests, 
checking over 6000 assertions. These tests are implemented at the end of the 
vecmath.h file, guarded by a conditional compile flag so they are ignored when
using the library normally.

To enable the tests, compile vecmath.h as a C or C++ file, and define the 
preprocessor symbol VECMATH_RUN_TESTS.

Using MSVCs cl.exe it would look like this:

	cl -Tc vecmath.h -DVECMATH_RUN_TESTS

and using clang (gcc and tcc use similar syntax):

	clang -xc vecmath.h -DVECMATH_RUN_TESTS

The executable produced can then be run, and it will perform all tests and
print the result.

### DirectX D3D conformance tests

To ensure the correctness of the library, it seemed appropriate to test some
parts of it against a known correct implementation. So vecmath.h implements
a set of tests that compare its computations with those of Microsofts DirectX
library, specifically the D3DX part of DirectX 9. This is a very widely used
and well proven math library implementation, and testing vecmath.h against this
gives a high level of confidence.

To enable the running of D3DX tests, define the preprocessor symbol 
`VECMATH_RUN_D3DX_TESTS` (in addition to `VECMATH_RUN_TESTS`), and make sure to
build with the correct include and library paths set for your installation of
d3d9.


### Using external testfw.h

By default, the tests are written using a handful of simple macros that check
for errors and print results. These macros are defined in the test code portion
of vecmath.h.

However, to get better test coverage (catching things like access violations and
other system exceptions, or memory leaks) and nicer printout of the result (for
example, using multi-colored output), it's possible to use the testfw.h single-
header lib, which can be found at:
https://github.com/mattiasgustavsson/libs/blob/main/testfw.h

To enable the use of testfw.h, the file must be placed in the same directory
as vecmath.h, and the preprocessor symbol VECMATH_USE_EXTERNAL_TESTFW must be
defined at compilation, like so:

	cl -Tc vecmath.h -DVECMATH_RUN_TESTS -DVECMATH_USE_EXTERNAL_TESTFW

if using msvc, or:

	clang -xc vecmath.h -DVECMATH_RUN_TESTS -DVECMATH_USE_EXTERNAL_TESTFW

if using clang (gcc and tcc use similar syntax).


