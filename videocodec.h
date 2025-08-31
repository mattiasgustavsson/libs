/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

videocodec.h - v0.1 - Custom mpeg-style video codec for game FMVs.

Do this:
    #define VIDEOCODEC_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.

The file includes two example programs, an encoder commandline app and a basic
player app. They can be copied out of the videocodec.h file and put into 
separate .c files, or they can be built directly from videocodec.h like this:

    clang -O2 -DVIDEOCODEC_BUILD_ENCODER -DVIDEOCODEC_IMPLEMENTATION -xc videocodec.h -o encoder.exe
    clang -O2 -DVIDEOCODEC_BUILD_PLAYER -DVIDEOCODEC_IMPLEMENTATION -xc videocodec.h -o player.exe

    cl -O2 -DVIDEOCODEC_BUILD_ENCODER -DVIDEOCODEC_IMPLEMENTATION -Tc videocodec.h -Fe:encoder.exe
    cl -O2 -DVIDEOCODEC_BUILD_PLAYER -DVIDEOCODEC_IMPLEMENTATION -Tc videocodec.h -Fe:player.exe
*/

#ifndef videocodec_h
#define videocodec_h

#define _CRT_SECURE_NO_WARNINGS
#include <stddef.h>
#include <stdint.h>

typedef struct videocodec_enc_stats_t { uint64_t frames_total; uint64_t frames_i; uint64_t frames_p; 
    uint64_t bytes_raw_total; uint64_t bytes_compressed_total; } videocodec_enc_stats_t;

enum videocodec_quality_t { VIDEOCODEC_QUALITY_MIN, VIDEOCODEC_QUALITY_LOW, 
    VIDEOCODEC_QUALITY_DEFAULT, VIDEOCODEC_QUALITY_HIGH, VIDEOCODEC_QUALITY_MAX };

typedef struct videocodec_enc_t videocodec_enc_t;
videocodec_enc_t* videocodec_enc_create( int width, int height, int fps_n, int fps_d, int sar_n, int sar_d, 
    enum videocodec_quality_t quality, videocodec_enc_stats_t* out_stats );
void videocodec_enc_destroy( videocodec_enc_t* enc );

typedef struct videocodec_enc_frame_t { size_t size; void* data; } videocodec_enc_frame_t; 
videocodec_enc_frame_t videocodec_enc_encode_yuv420( videocodec_enc_t* enc, void const* yuv420 );
videocodec_enc_frame_t videocodec_enc_encode_xbgr( videocodec_enc_t* enc, uint32_t const* xbgr );
videocodec_enc_frame_t videocodec_enc_finalize( videocodec_enc_t* enc );


#define VIDEOCODEC_DEC_HEADER_SIZE 96
typedef struct videocodec_dec_t videocodec_dec_t;

videocodec_dec_t* videocodec_dec_create( uint8_t const data[ VIDEOCODEC_DEC_HEADER_SIZE ] );
void videocodec_dec_destroy( videocodec_dec_t* dec );

int  videocodec_dec_width( videocodec_dec_t* dec );
int  videocodec_dec_height( videocodec_dec_t* dec );
void videocodec_dec_fps( videocodec_dec_t* dec, int* fps_n, int* fps_d );
void videocodec_dec_ar ( videocodec_dec_t* dec, int* ar_n,  int* ar_d  );

size_t videocodec_dec_next_frame( videocodec_dec_t* dec, void const* data, size_t size, uint32_t* out_xbgr );


#endif // videocodec_h

/**

videocodec.h
============

Custom mpeg-style video codec for game FMVs.


videocodec_enc_create
---------------------

    videocodec_enc_t* videocodec_enc_create( int width, int height, int fps_n, int fps_d, int sar_n, int sar_d,
        enum videocodec_quality_t quality, videocodec_enc_stats_t* out_stats )

Creates a new encoder instance for compressing video frames. The video stream is defined by its resolution (`width` x 
`height`), frame rate (expressed as a fraction `fps_n/fps_d`), and sample aspect ratio (`sar_n/sar_d`). The `quality`
parameter selects the compression quality, where higher quality means larger output size, and lower quality means
smaller output size.  

`width` and `height` must be positive and multiples of 8, or the function returns NULL. If `sar_n` or `sar_d` are 
non-positive, they are clamped to 1.  

An optional pointer to a `videocodec_enc_stats_t` structure can be provided to receive statistics about the encoding 
process. The encoder will update this structure in place after every frame is encoded. This structure is owned by the 
caller and must remain valid for the lifetime of the encoder. When no longer needed, the encoder must be destroyed with 
`videocodec_enc_destroy`.


videocodec_enc_destroy
----------------------

    void videocodec_enc_destroy( videocodec_enc_t* enc )

Releases the resources held by an encoder instance previously created with `videocodec_enc_create`. After calling this
function, the `enc` pointer is no longer valid and must not be used in further API calls.


videocodec_enc_encode_yuv420
----------------------------

    videocodec_enc_frame_t videocodec_enc_encode_yuv420( videocodec_enc_t* enc, void const* yuv420 )

Encodes a single raw video frame in planar YUV420 format. The pointer `yuv420` must reference a frame of the dimensions
specified when the encoder was created. The buffer must contain the Y plane followed by the U and V planes.  

The function returns a `videocodec_enc_frame_t` containing the compressed frame data and its size. The returned `data`
pointer is owned by the codec and remains valid until the next call to any encode/finalize function or until 
`videocodec_enc_destroy` is called. On the first frame, this function automatically writes the stream header before the
compressed data.


videocodec_enc_encode_xbgr
--------------------------

    videocodec_enc_frame_t videocodec_enc_encode_xbgr( videocodec_enc_t* enc, uint32_t const* xbgr )

Encodes a single raw video frame in packed 32-bit XBGR format (`0xXXBBGGRR` in memory, i.e. little-endian byte order 
R,G,B,X). The pointer `xbgr` must reference a frame of the dimensions specified when the encoder was created.  

The function returns a `videocodec_enc_frame_t` containing the compressed frame data and its size. The returned `data`
pointer is owned by the codec and remains valid until the next call to any encode/finalize function or until 
`videocodec_enc_destroy` is called. On the first frame, this function automatically writes the stream header before the
compressed data.


videocodec_enc_finalize
-----------------------

    videocodec_enc_frame_t videocodec_enc_finalize( videocodec_enc_t* enc )

Finalizes an encoding session, flushing any remaining data from the encoder and emitting the end-of-stream marker. If 
the stream header has not yet been written, it will be included as part of the returned data. The function returns a 
`videocodec_enc_frame_t` containing the final compressed data needed to complete the stream.  

Once this has been called, the encoding session is considered finished. No further frames should be encoded with the 
same encoder instance. To begin a new stream, create a new encoder with `videocodec_enc_create`.


videocodec_dec_create
---------------------

    videocodec_dec_t* videocodec_dec_create( const uint8_t data[ VIDEOCODEC_DEC_HEADER_SIZE ] )

Creates a new decoder instance from a compressed video stream header. The `data` buffer must contain exactly 
`VIDEOCODEC_DEC_HEADER_SIZE` bytes from the start of the compressed stream. The decoder validates the header 
signature, version, and parameters. If invalid, the function returns NULL.  

When no longer needed, the decoder must be destroyed with `videocodec_dec_destroy`.


videocodec_dec_destroy
----------------------

    void videocodec_dec_destroy( videocodec_dec_t* dec )

Releases the resources held by a decoder instance previously created with `videocodec_dec_create`. After calling this 
function, the `dec` pointer is no longer valid and must not be used in further API calls.


videocodec_dec_width
--------------------

    int videocodec_dec_width( videocodec_dec_t* dec )

Returns the width, in pixels, of the decoded video stream.


videocodec_dec_height
---------------------

    int videocodec_dec_height( videocodec_dec_t* dec )

Returns the height, in pixels, of the decoded video stream.


videocodec_dec_fps
------------------

    void videocodec_dec_fps( videocodec_dec_t* dec, int* fps_n, int* fps_d )

Retrieves the frame rate of the decoded video stream, expressed as a rational fraction `fps_n/fps_d`. 


videocodec_dec_ar
-----------------

    void videocodec_dec_ar( videocodec_dec_t* dec, int* ar_n, int* ar_d )

Retrieves the sample aspect ratio of the decoded video stream, expressed as a rational fraction `ar_n/ar_d`. 


videocodec_dec_next_frame
-------------------------

    size_t videocodec_dec_next_frame( videocodec_dec_t* dec, const void* data, size_t size, uint32_t* out_xbgr )

Decodes the next frame from the compressed bitstream. The parameters `data` and `size` specify the available compressed 
data, and `out_xbgr` must point to a buffer large enough to hold the decoded frame in 32-bit XBGR format.

The return value indicates how many bytes are required to provide the next frame. If you wish to query the number of 
bytes needed without supplying data, pass `NULL` for `data`, zero for `size`, and `NULL` for `out_xbgr`.

A return value of 0 indicates either end-of-stream or that the provided input was insufficient/invalid; in both cases, 
no frame was produced, and the stream can be considered ended.
*/

#ifdef VIDEOCODEC_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>


#if defined( VIDEOCODEC_PACK ) || defined( VIDEOCODEC_UNPACK ) || defined( VIDEOCODEC_PACK_ARENA_SIZE )
    #if !defined( VIDEOCODEC_PACK ) || !defined( VIDEOCODEC_UNPACK ) || !defined( VIDEOCODEC_PACK_ARENA_SIZE )
        #error "You can not #define only some of VIDEOCODEC_PACK, VIDEOCODEC_UNPACK or VIDEOCODEC_PACK_ARENA_SIZE. It's all or none."
    #endif
#else
    int internal_videocodec_inflate( void* out, int cap, void const* in, int size ) {
        uint8_t* dst = ( uint8_t* )out, *ode = dst + cap, *o0 = dst;
        uint8_t const* ip = ( uint8_t const* )in, *ie = ip + size;
        uint64_t bitbuf = 0; int bitcnt = 0;
        static const uint8_t order[ 19 ]= {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
        static const short dbase[ 30 ]= {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
        static const uint8_t dbits[ 30 ]= {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
        static const short lbase[ 29 ]= {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
        static const uint8_t lbits[ 29 ]= {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
        uint8_t ll_lens[ 288 ], dd_lens[ 32 ], cl_lens[ 19 ]; int ll_cnt[ 16 ], dd_cnt[ 16 ], cl_cnt[ 16 ]; int ll_off[ 16 ], dd_off[ 16 ], cl_off[ 16 ];
        uint16_t ll_rev[ 288 ], dd_rev[ 32 ], cl_rev[ 19 ]; uint16_t ll_sym[ 288 ], dd_sym[ 32 ], cl_sym[ 19 ];
        int ll_max = 0, dd_max = 0, cl_max = 0;
        #define INTERNAL_INFLATE_ENSURE( n ) while( bitcnt <( n ) ) { if( ip >= ie ) return( int )( dst - o0 ); bitbuf |= ( uint64_t )( *ip++ ) << bitcnt; bitcnt += 8; }
        #define INTERNAL_INFLATE_GETBITS( n, v ) do { INTERNAL_INFLATE_ENSURE( n );( v ) = ( int )( bitbuf &( ( 1u<<( n ) ) - 1u ) ); bitbuf >>= ( n ); bitcnt -= ( n ); } while( 0 )
        #define INTERNAL_INFLATE_GET1( v ) do { INTERNAL_INFLATE_ENSURE( 1 );( v ) = ( int )( bitbuf & 1u ); bitbuf >>= 1; bitcnt -= 1; } while( 0 )
        #define INTERNAL_INFLATE_BUILD_DECODE( LENS, SYMCNT, CNT, OFF, REV, SYM, MAXL ) do{\
            int _i, _b; uint16_t _code[ 16 ]; uint16_t _next[ 16 ]; MAXL = 0; \
            for( _b = 0;_b<16;_b++ ) { CNT[ _b ]= 0; OFF[ _b ]= 0; _code[ _b ]= 0; _next[ _b ]= 0; } \
            for( _i = 0; _i < ( SYMCNT ); ++_i ) { int L = ( LENS )[ _i ]; if( L<0||L>15 ) L = 0; if( L ) { CNT[ L ]++; if( L>MAXL ) MAXL = L; } } \
            OFF[ 0 ]= 0; for( _b = 1; _b <= 15; ++_b ) OFF[ _b ]= OFF[ _b-1 ]+ CNT[ _b-1 ]; \
            _code[ 0 ]= 0; for( _b = 1; _b <= 15; ++_b ) _code[ _b ]= ( uint16_t )( ( _code[ _b-1 ]+ CNT[ _b-1 ])<<1 ); \
            for( _b = 1; _b <= 15; ++_b ) _next[ _b ]= _code[ _b ]; \
            for( _i = 0; _i < ( SYMCNT ); ++_i ) { int L = ( LENS )[ _i ]; if( L ) { uint16_t c = _next[ L ]++; uint16_t r = 0; int k = L; while( k-- ) { r = ( uint16_t )( ( r<<1 )|( c&1 ) ); c>>= 1; } \
                { int pos = OFF[ L ]++; REV[ pos ]= r; SYM[ pos ]= ( uint16_t )_i; } } } \
            for( _b = 15; _b>= 1; --_b ) { OFF[ _b ]= OFF[ _b-1 ]; } OFF[ 0 ]= 0; \
        }while( 0 )
        #define INTERNAL_INFLATE_BUILD_HUFF_DECODE( SYM, CNT, OFF, REV, SYMS, MAXL ) do{\
            int _len = 0, _code = 0, _found = 0; \
            while( !_found ) { int _bit; INTERNAL_INFLATE_GET1( _bit ); _code |= ( _bit<<_len ); ++_len; if( _len>( MAXL ) ) return( int )( dst - o0 ); \
                { int _s = OFF[ _len ], _c = CNT[ _len ], _k; for( _k = 0; _k<_c; ++_k ) { if( REV[ _s+_k ]== ( unsigned )_code ) {( SYM ) = SYMS[ _s+_k ]; _found = 1; break; } } } \
            } \
        }while( 0 )
        enum {ST_HDR, ST_STO, ST_FIX, ST_DYN, ST_BLK}; int state = ST_HDR, last = 0;
        for( ;; ) {
            if( state == ST_HDR ) {
                int type; INTERNAL_INFLATE_GET1( last ); INTERNAL_INFLATE_GETBITS( 2, type );
                if( type == 0 ) state = ST_STO; else if( type == 1 ) state = ST_FIX; else if( type == 2 ) state = ST_DYN; else return( int )( dst - o0 );
            } else if( state == ST_STO ) {
                int pad = bitcnt & 7; if( pad ) { bitbuf >>= pad; bitcnt -= pad; }
                { int len, nlen; INTERNAL_INFLATE_GETBITS( 16, len ); INTERNAL_INFLATE_GETBITS( 16, nlen );
                 if( ( uint16_t )len != ( uint16_t )( ~( unsigned )nlen ) ) return( int )( dst - o0 );
                 if( ip + len > ie ) return( int )( dst - o0 );
                 if( dst + len > ode ) return -2;
                 memcpy( dst, ip, ( size_t )len ); dst += len; ip += len;
                 if( last ) return( int )( dst - o0 ); state = ST_HDR; }
            } else if( state == ST_FIX ) {
                int i; for( i = 0; i < 288; i++ ) ll_lens[ i ]= 0; for( i = 0; i <= 143; i++ ) ll_lens[ i ]= 8; for( i = 144; i <= 255; i++ ) ll_lens[ i ]= 9; for( i = 256; i <= 279; i++ ) ll_lens[ i ]= 7; for( i = 280; i <= 287; i++ ) ll_lens[ i ]= 8;
                for( i = 0; i < 32; i++ ) dd_lens[ i ]= 5;
                INTERNAL_INFLATE_BUILD_DECODE( ll_lens, 288, ll_cnt, ll_off, ll_rev, ll_sym, ll_max );
                INTERNAL_INFLATE_BUILD_DECODE( dd_lens, 32 , dd_cnt, dd_off, dd_rev, dd_sym, dd_max );
                state = ST_BLK;
            } else if( state == ST_DYN ) {
                int i, nlit, ndist, nlen, npos = 0;
                for( i = 0; i < 19; i++ ) cl_lens[ i ]= 0;
                INTERNAL_INFLATE_GETBITS( 5, nlit ); nlit += 257;
                INTERNAL_INFLATE_GETBITS( 5, ndist ); ndist += 1;
                INTERNAL_INFLATE_GETBITS( 4, nlen ); nlen += 4;
                for( i = 0; i < nlen; i++ ) { int v; INTERNAL_INFLATE_GETBITS( 3, v ); cl_lens[ order[ i ] ]= ( uint8_t )v; }
                INTERNAL_INFLATE_BUILD_DECODE( cl_lens, 19, cl_cnt, cl_off, cl_rev, cl_sym, cl_max );
                for( i = 0; i < 288; i++ ) ll_lens[ i ]= 0; for( i = 0; i < 32; i++ ) dd_lens[ i ]= 0;
                while( npos <( nlit + ndist ) ) {
                    int csym; INTERNAL_INFLATE_BUILD_HUFF_DECODE( csym, cl_cnt, cl_off, cl_rev, cl_sym, cl_max );
                    if( csym <= 15 ) {
                        if( npos < nlit ) ll_lens[ npos++ ]= ( uint8_t )csym; else dd_lens[ npos++ - nlit ]= ( uint8_t )csym;
                    } else if( csym == 16 ) {
                        int rep, prev; if( npos == 0 ) return( int )( dst - o0 );
                        INTERNAL_INFLATE_GETBITS( 2, rep ); rep += 3;
                        prev = ( npos <= nlit ) ?( npos-1 < nlit ? ll_lens[ npos-1 ]: ll_lens[ nlit-1 ]) : dd_lens[ ( npos-1 )-nlit ];
                        while( rep-- ) { if( npos < nlit ) ll_lens[ npos++ ]= ( uint8_t )prev; else dd_lens[ npos++ - nlit ]= ( uint8_t )prev; }
                    } else if( csym == 17 ) {
                        int rep; INTERNAL_INFLATE_GETBITS( 3, rep ); rep += 3; while( rep-- ) { if( npos < nlit ) ll_lens[ npos++ ]= 0; else dd_lens[ npos++ - nlit ]= 0; }
                    } else {
                        int rep; INTERNAL_INFLATE_GETBITS( 7, rep ); rep += 11; while( rep-- ) { if( npos < nlit ) ll_lens[ npos++ ]= 0; else dd_lens[ npos++ - nlit ]= 0; }
                    }
                }
                INTERNAL_INFLATE_BUILD_DECODE( ll_lens, nlit , ll_cnt, ll_off, ll_rev, ll_sym, ll_max );
                INTERNAL_INFLATE_BUILD_DECODE( dd_lens, ndist, dd_cnt, dd_off, dd_rev, dd_sym, dd_max );
                state = ST_BLK;
            } else {
                for( ;; ) {
                    int sym; INTERNAL_INFLATE_BUILD_HUFF_DECODE( sym, ll_cnt, ll_off, ll_rev, ll_sym, ll_max );
                    if( sym < 256 ) {
                        if( dst >= ode ) return -2;
                        *dst++= ( uint8_t )sym;
                    } else if( sym == 256 ) {
                        if( last ) return( int )( dst - o0 );
                        state = ST_HDR; break;
                    } else {
                        int len, dsym, offs;
                        if( sym >= 286 ) return( int )( dst - o0 );
                        sym -= 257;
                        { int eb = lbits[ sym ]; int add = 0; if( eb ) { INTERNAL_INFLATE_GETBITS( eb, add ); } len = lbase[ sym ]+ add; }
                        INTERNAL_INFLATE_BUILD_HUFF_DECODE( dsym, dd_cnt, dd_off, dd_rev, dd_sym, dd_max );
                        if( dsym >= 30 ) return( int )( dst - o0 );
                        { int eb = dbits[ dsym ]; int add = 0; if( eb ) { INTERNAL_INFLATE_GETBITS( eb, add ); } offs = dbase[ dsym ]+ add; }
                        if( offs <= 0 || offs >( int )( dst - o0 ) ) return( int )( dst - o0 );
                        if( dst + len > ode ) return -2;
                        { uint8_t *s = dst - offs, *d = dst, *e = dst + len; if( offs == 1 ) { uint8_t v = *s; while( d < e ) { *d++= v; } } else { while( d < e ) { *d++= *s++; } } dst = e; }
                    }
                }
            }
        }
        #undef INTERNAL_INFLATE_ENSURE
        #undef INTERNAL_INFLATE_GETBITS
        #undef INTERNAL_INFLATE_GET1
        #undef INTERNAL_INFLATE_BUILD_DECODE
        #undef INTERNAL_INFLATE_BUILD_HUFF_DECODE
    }


    #define INTERNAL_VIDEOCODEC_DEFLATE_ARENA_SIZE 1316320

    int internal_videocodec_deflate( void* out_buf, void const* in_buf, int n_bytes, uint8_t arena[ INTERNAL_VIDEOCODEC_DEFLATE_ARENA_SIZE ]) {
      if( n_bytes <= 0 ) return 0;
      if( !out_buf || !in_buf ) { int max_blocks = 1 +( ( n_bytes + 65535 ) / 65536 ); int bound = 5 * max_blocks + n_bytes + 1 + 4 + 8 + 3; return bound; }
      enum { WBITS = 15, WSIZE = 1 << WBITS, WMASK = WSIZE - 1, HBITS = 15, HSIZE = 1 << HBITS, MINM = 4, MAXM = 258, BLKMAX = 256 * 1024, LIT_EOB = 256, MAXBITS = 15, MAX_CHAIN = 1 << 13 };
      static const uint16_t LBASE[ 29 ]= {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
      static const uint8_t  LBITS[ 29 ]= {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
      static const uint16_t DBASE[ 30 ]= {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
      static const uint8_t  DBITS[ 30 ]= {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
      static const uint8_t  PREPERM[ 19 ]= {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
      int32_t *head = ( int32_t* )arena; arena += ( size_t )HSIZE * sizeof( int32_t );
      int32_t *link = ( int32_t* )arena; arena += ( size_t )WSIZE * sizeof( int32_t );
      uint16_t *tok_len  = ( uint16_t* )arena; arena += ( size_t )BLKMAX * sizeof( uint16_t );
      uint16_t *tok_dist = ( uint16_t* )arena; arena += ( size_t )BLKMAX * sizeof( uint16_t );
      int32_t *heap_w = ( int32_t* )arena; arena += ( size_t )700 * sizeof( int32_t );
      int32_t *heap_s = ( int32_t* )arena; arena += ( size_t )700 * sizeof( int32_t );
      for( int i = 0; i < HSIZE;++i ) head[ i ]= -1;  for( int i = 0; i < WSIZE;++i ) link[ i ]= -1;
      unsigned char *op = ( unsigned char* )out_buf; uint32_t bw_acc = 0; int bw_n = 0;
      #define INTERNAL_DEFLATE_BW_PUT( v, nb ) do{ uint32_t _nb = ( nb ); if( _nb ) { bw_acc |= ( ( uint32_t )( v ) << bw_n ); bw_n += ( int )_nb; while( bw_n >= 8 ) { *op++= ( unsigned char )( bw_acc & 255u ); bw_acc >>= 8; bw_n -= 8; } } }while( 0 )
      #define INTERNAL_DEFLATE_BW_ALIGN( ) do{ if( bw_n ) INTERNAL_DEFLATE_BW_PUT( 0, 8 - bw_n ); }while( 0 )
      #define INTERNAL_DEFLATE_BW_U16LE( x ) do{ INTERNAL_DEFLATE_BW_ALIGN( ); *op++= ( unsigned char )( ( x )&255u ); *op++= ( unsigned char )( ( ( x )>>8 )&255u ); }while( 0 )
      uint8_t const* in = ( uint8_t const* )in_buf;
      #define INTERNAL_DEFLATE_RD32_LE( p )( ( uint32_t )( ( p )[ 0 ]|( ( uint32_t )( p )[ 1 ]<<8 ) |( ( uint32_t )( p )[ 2 ]<<16 ) |( ( uint32_t )( p )[ 3 ]<<24 ) ) )
      #define INTERNAL_DEFLATE_HASH4( p )  ( ( uint32_t )( ( INTERNAL_DEFLATE_RD32_LE( p ) * 0x9E377989u ) >>( 32 - HBITS ) ) )
      #define INTERNAL_DEFLATE_LT( a, b )( ( heap_w[ a ]< heap_w[ b ]) ||( heap_w[ a ]== heap_w[ b ]&& heap_s[ a ]< heap_s[ b ]) )
      #define INTERNAL_DEFLATE_SIFTDOWN( n, i ) do{ int _r = ( i ); for( ;; ) { int _l = ( _r<<1 )+1, _m = _r; if( _l<( n ) && INTERNAL_DEFLATE_LT( _l, _m ) ) _m = _l; if( _l+1<( n ) && INTERNAL_DEFLATE_LT( _l+1, _m ) ) _m = _l+1; if( _m == _r ) break; int32_t _tw = heap_w[ _r ], _ts = heap_s[ _r ]; heap_w[ _r ]= heap_w[ _m ]; heap_s[ _r ]= heap_s[ _m ]; heap_w[ _m ]= _tw; heap_s[ _m ]= _ts; _r = _m; } }while( 0 )
      #define INTERNAL_DEFLATE_HEAP_PUSH( n, w, s ) do{ int _c = ( n )++; heap_w[ _c ]= ( w ); heap_s[ _c ]= ( s ); while( _c>0 ) { int _p = ( _c-1 )>>1; if( !INTERNAL_DEFLATE_LT( _c, _p ) ) break; int32_t _tw = heap_w[ _p ], _ts = heap_s[ _p ]; heap_w[ _p ]= heap_w[ _c ]; heap_s[ _p ]= heap_s[ _c ]; heap_w[ _c ]= _tw; heap_s[ _c ]= _ts; _c = _p; } }while( 0 )
      #define INTERNAL_DEFLATE_HEAP_POP( n, ow, os ) do{( ow ) = heap_w[ 0 ];( os ) = heap_s[ 0 ]; --( n ); heap_w[ 0 ]= heap_w[ ( n ) ]; heap_s[ 0 ]= heap_s[ ( n ) ]; INTERNAL_DEFLATE_SIFTDOWN( ( n ), 0 ); }while( 0 )
      #define INTERNAL_DEFLATE_BUILD_LEN( freqArr, NS, MAXB_, outLens ) do{ \
          uint8_t rawL[ NS ]; memset( rawL, 0, sizeof( rawL ) ); int hn = 0; \
          for( int _i = 0; _i < ( NS ); ++_i ) if( ( freqArr )[ _i ]) INTERNAL_DEFLATE_HEAP_PUSH( hn, ( int32_t )( freqArr )[ _i ], _i ); \
          if( hn == 0 ) { memset( ( outLens ), 0, ( NS ) ); } else { \
              if( hn == 1 ) { INTERNAL_DEFLATE_HEAP_PUSH( hn, heap_w[ 0 ], heap_s[ 0 ]== 0?1:0 ); } \
              int ip = 0; int32_t Lc[ 700 ], Rc[ 700 ]; \
              while( hn>= 2 ) { int32_t w1, s1, w2, s2; INTERNAL_DEFLATE_HEAP_POP( hn, w1, s1 ); INTERNAL_DEFLATE_HEAP_POP( hn, w2, s2 ); Lc[ ip ]= s1; Rc[ ip ]= s2; INTERNAL_DEFLATE_HEAP_PUSH( hn, w1+w2, ( NS )+ip ); ip++; } \
              int stkN[ 700 ], stkD[ 700 ], sp = 0; stkN[ sp ]= heap_s[ 0 ]; stkD[ sp ]= 0; sp++; \
              while( sp ) { int n = stkN[ --sp ], d = stkD[ sp ]; if( n<( NS ) ) { if( ( freqArr )[ n ]) rawL[ n ]= ( uint8_t )d; } else { int j = n-( NS ); stkN[ sp ]= Lc[ j ]; stkD[ sp ]= d+1; sp++; stkN[ sp ]= Rc[ j ]; stkD[ sp ]= d+1; sp++; } } \
              uint32_t blc[ 600 ]; memset( blc, 0, sizeof( blc ) ); int overflow = 0; int cnt = 0; \
              for( int _i = 0; _i < ( NS ); ++_i ) { if( ( freqArr )[ _i ]) { cnt++; int d = rawL[ _i ]; if( d>MAXB_ ) { d = MAXB_; overflow++; } blc[ d ]++; } } \
              while( overflow>0 ) { int b = ( MAXB_ )-1; while( b>0 && !blc[ b ]) b--; blc[ b ]--; blc[ b+1 ]+= 2; blc[ MAXB_ ]--; overflow-= 2; } \
              int ord[ 700 ], p = 0; for( int _i = 0; _i < ( NS ); ++_i ) if( ( freqArr )[ _i ]) ord[ p++ ]= _i; \
              for( int a = 1;a<cnt;++a ) { int v = ord[ a ], j = a-1; while( j>= 0 &&( ( freqArr )[ ord[ j ] ]<( freqArr )[ v ]||( ( freqArr )[ ord[ j ] ]== ( freqArr )[ v ]&& ord[ j ]< v ) ) ) { ord[ j+1 ]= ord[ j ]; j--; } ord[ j+1 ]= v; } \
              memset( ( outLens ), 0, ( NS ) ); int posi = 0; for( int b = 1; b <= ( MAXB_ ); ++b ) { int k = ( int )blc[ b ]; while( k-- > 0 && posi < cnt ) {( outLens )[ ord[ posi++ ] ]= ( uint8_t )b; } } \
          } \
      }while( 0 )
      #define INTERNAL_DEFLATE_GEN_CANON_CODES( LEN, NS, MAXB_, OUT ) do{ uint16_t t_[ NS ]; memset( t_, 0, sizeof( t_ ) ); uint16_t code = 0; for( int len = 1;len <= ( MAXB_ );++len ) { for( int sym = 0;sym<( NS );++sym ) { if( ( LEN )[ sym ]== len ) { t_[ sym ]= code++; } } code <<= 1; } for( int c = 0; c<( NS ); ++c ) { if( ( LEN )[ c ]) { unsigned _temp = t_[ c ]; _temp = ( ( _temp&0x5555 )<<1 )|( ( _temp&0xAAAA )>>1 ); _temp = ( ( _temp&0x3333 )<<2 )|( ( _temp&0xCCCC )>>2 ); _temp = ( ( _temp&0x0F0F )<<4 )|( ( _temp&0xF0F0 )>>4 ); _temp = ( ( _temp&0x00FF )<<8 )|( ( _temp&0xFF00 )>>8 ); t_[ c ]= _temp >>( 16-( LEN )[ c ]); } } memcpy( ( OUT ), t_, sizeof( t_ ) ); }while( 0 )
      #define INTERNAL_DEFLATE_FIND_LENGTH_CODE( LEN, LC, LXB, LXV ) do{ if( ( LEN ) == 258 ) {( LC ) = 285; } else {( LC ) = 257; for( int z = 0;z<29;++z ) { int base = LBASE[ z ], eb = LBITS[ z ], maxv = base+( ( 1<<eb )-1 ); if( ( LEN )>= base&&( LEN ) <= maxv ) {( LC ) = 257+z;( LXB ) = eb;( LXV ) = ( LEN )-base;break; } } } }while( 0 )
      #define INTERNAL_DEFLATE_FIND_DISTANCE_CODE( DIST, DC, DXB, DXV ) do{( DC ) = 0; for( int z = 0;z < 30;++z ) { int base = DBASE[ z ], eb = DBITS[ z ], maxv = base+( ( 1<<eb )-1 ); if( ( DIST )>= base&&( DIST ) <= maxv ) {( DC ) = z;( DXB ) = eb;( DXV ) = ( DIST )-base;break; } } }while( 0 )
      #define INTERNAL_DEFLATE_FIND_MATCH( OFF, MATCH, MAX_MATCH ) do{ uint32_t h = INTERNAL_DEFLATE_HASH4( in +( OFF ) ); int c = head[ h ]; int limit = ( ( OFF ) - WSIZE < -1 ) ? -1 :( ( OFF ) - WSIZE ); int chain_len = MAX_CHAIN; \
          while( c > limit ) { if( c + MINM <= blk_end && in[ c +( MATCH ).len ]== in[ ( OFF ) +( MATCH ).len ]&& INTERNAL_DEFLATE_RD32_LE( in + c ) == INTERNAL_DEFLATE_RD32_LE( in +( OFF ) ) ) { int n = MINM; while( n <( MAX_MATCH ) && in[ c + n ]== in[ ( OFF ) + n ]) { n++; } if( n >( MATCH ).len ) {( MATCH ).len = n;( MATCH ).off = ( OFF ) - c; if( n == ( MAX_MATCH ) ) break; } } if( !( --chain_len ) ) break; c = link[ c & WMASK ]; } }while( 0 )
      for(  int pos = 0; pos < n_bytes; ) {
          int blk_end = pos + BLKMAX; if( blk_end > n_bytes ) blk_end = n_bytes; int litlen = 0; int ntok = 0; int i = pos; 
          uint32_t flit[ 286 ]; memset( flit, 0, sizeof( flit ) ); uint32_t fdis[ 30 ];  memset( fdis, 0, sizeof( fdis ) );   
          while(  i < blk_end ) {
              struct { int off; int len; } m = {0, 0};
              int left = blk_end - i; int max_match = ( left > MAXM ) ? MAXM : left; int run = 1;
              if( max_match > MINM ) { INTERNAL_DEFLATE_FIND_MATCH( i, m, max_match ); }
              if( m.len >= MINM ) {
                  const int nice_match = 130;
                  if( m.len + 1 < nice_match ) {
                      struct { int off; int len; } m2 = {0, 0};
                      if( i + 1 < blk_end && i + 1 + MINM <= blk_end ) {
                          int max_match2 = m.len + 1;
                          int remain2 = blk_end -( i + 1 );
                          if( max_match2 > remain2 ) max_match2 = remain2;
                          INTERNAL_DEFLATE_FIND_MATCH( i + 1, m2, max_match2 );
                      }
                      if( m2.len > m.len ) { m.len = 0; }
                  }
              }
              if( m.len >= MINM ) {
                  if( litlen ) { for( int j = i - litlen; j < i; j++ ) { tok_len[ ntok ]= in[ j ]; tok_dist[ ntok ]= 0; ntok++; flit[ in[ j ] ]++; } litlen = 0; }
                  int lc, lxb = 0, lxv = 0; INTERNAL_DEFLATE_FIND_LENGTH_CODE( m.len, lc, lxb, lxv );
                  int dc = 0, dxb = 0, dxv = 0; INTERNAL_DEFLATE_FIND_DISTANCE_CODE( m.off, dc, dxb, dxv );
                  flit[ lc ]++; fdis[ dc ]++;
                  tok_len[ ntok ]= ( uint16_t )m.len; tok_dist[ ntok ]= ( uint16_t )m.off; ntok++;
                  run = m.len;
              } else {
                  litlen++;
              }
              if( blk_end -( i + run ) > MINM ) { while( run-- > 0 ) { if( i+MINM <= blk_end ) { uint32_t _h = INTERNAL_DEFLATE_HASH4( in + i ); link[ i&WMASK ]= head[ _h ]; head[ _h ]= i; } i ++; } } else { i += run; }
          }
          if( litlen ) for( int j = i - litlen; j < i; j++ ) { tok_len[ ntok ]= in[ j ]; tok_dist[ ntok ]= 0; ntok++; flit[ in[ j ] ]++; }
          flit[ LIT_EOB ]++;
          uint8_t ll_len[ 286 ], d_len[ 30 ]; memset( ll_len, 0, sizeof( ll_len ) ); memset( d_len, 0, sizeof( d_len ) );
          INTERNAL_DEFLATE_BUILD_LEN( flit, 286, 15, ll_len );
          INTERNAL_DEFLATE_BUILD_LEN( fdis, 30,  15, d_len );
          int HLIT = 286; while( HLIT>257 && ll_len[ HLIT-1 ]== 0 ) HLIT--;
          int HDIST = 30; while( HDIST>1   && d_len[ HDIST-1 ]== 0 ) HDIST--;
          if( HDIST == 1 && d_len[ 0 ]== 0 ) d_len[ 0 ]= 1;
          uint16_t codeL[ 286 ], codeD[ 30 ];
          INTERNAL_DEFLATE_GEN_CANON_CODES( ll_len, 286, MAXBITS, codeL );
          INTERNAL_DEFLATE_GEN_CANON_CODES( d_len, 30, MAXBITS, codeD );
          #define INTERNAL_DEFLATE_CL_PUSH( S, X ) do{ cl[ cln ].sym = ( uint8_t )( S ); cl[ cln ].xb = ( uint8_t )( X ); cln++; pref[ ( S ) ]++; }while( 0 )
          struct { uint8_t sym; uint8_t xb; } cl[ 700 ]; int cln = 0;
          uint32_t pref[ 19 ]; memset( pref, 0, sizeof( pref ) );
          int tot = HLIT + HDIST; uint8_t cat[ 316 ];
          for( int a = 0;a<HLIT;++a ) cat[ a ]= ll_len[ a ];
          for( int a = 0;a<HDIST;++a ) cat[ HLIT+a ]= d_len[ a ];
          for( int q, p = 0; p<tot; p = q ) {
              uint8_t L = cat[ p ]; q = p+1; while( q<tot && cat[ q ]== L ) q++; int run = q-p;
              if( L == 0 ) {
                  while( run >= 11 ) { int r = run-11; if( r>127 ) r = 127; INTERNAL_DEFLATE_CL_PUSH( 18, r ); run -= 11 + r; }
                  if( run >= 3 ) { int r = run-3; if( r>7 ) r = 7; INTERNAL_DEFLATE_CL_PUSH( 17, r ); run -= 3 + r; }
                  while( run-- ) { INTERNAL_DEFLATE_CL_PUSH( 0, 0 ); }
              } else {
                  INTERNAL_DEFLATE_CL_PUSH( L, 0 ); run--;
                  while( run >= 3 ) { int r = run-3; if( r>3 ) r = 3; INTERNAL_DEFLATE_CL_PUSH( 16, r ); run -= 3 + r; }
                  while( run-- ) { INTERNAL_DEFLATE_CL_PUSH( L, 0 ); }
              }
          }
          uint8_t pre_len[ 19 ]; INTERNAL_DEFLATE_BUILD_LEN( pref, 19, 7, pre_len );
          uint16_t pre_code[ 19 ];
          uint16_t blc_[ 16 ]= {0}; for( int _i = 0; _i < 19; ++_i ) if( pre_len[ _i ]) blc_[ pre_len[ _i ] ]++;
          uint16_t next_[ 16 ]= {0}; uint16_t _c = 0; for( int b = 1;b <= 7; ++b ) { _c = ( uint16_t )( ( _c+blc_[ b-1 ])<<1 ); next_[ b ]= _c; } 
          for( int _i = 0; _i < 19; ++_i ) { uint8_t L = pre_len[ _i ]; if( !L ) { pre_code[ _i ]= 0; } else { uint16_t c = next_[ L ]++, r = 0; for( int t = 0;t<L;++t ) { r = ( uint16_t )( ( r<<1 )|( c&1 ) ); c>>= 1; } pre_code[ _i ]= r; } } 
          int HCLEN = 4; for( int i2 = 19; i2>4;--i2 ) { if( pre_len[ PREPERM[ i2-1 ] ]) { HCLEN = i2; break; } }
          uint64_t dyn_bits = 3 + 5 + 5 + 4 +( uint64_t )HCLEN * 3;
          for( int i2 = 0; i2<cln;++i2 ) { uint8_t s = cl[ i2 ].sym; dyn_bits += pre_len[ s ]+( s == 16?2:s == 17?3:s == 18?7:0 ); }
          for( int s = 0;s<286;++s ) { uint32_t f = flit[ s ]; if( f ) { dyn_bits += ( uint64_t )f *( ( s <= 255||s == LIT_EOB ) ? ll_len[ s ]:( ll_len[ s ]+ LBITS[ s-257 ]) ); } }
          for( int s = 0;s<30;++s ) { uint32_t f = fdis[ s ]; if( f ) dyn_bits += ( uint64_t )f *( d_len[ s ]+ DBITS[ s ]); }
          int remain = ( blk_end - pos ); uint64_t bits = 0; while( remain > 0 ) { int chunk = remain > 65535 ? 65535 : remain; bits += 3; bits += ( ( 8 -( bits & 7u ) ) & 7u ); bits += ( 4 +( uint64_t )chunk ) * 8u; remain -= chunk; } 
          if( bits <= dyn_bits ) {
              int remain = ( blk_end - pos ), off = pos;
              while( remain > 0 ) {
                  int chunk = remain > 65535 ? 65535 : remain;
                  INTERNAL_DEFLATE_BW_PUT( ( blk_end == n_bytes ) &&( remain == chunk ) ? 1u : 0u, 1 ); INTERNAL_DEFLATE_BW_PUT( 0u, 2 );
                  INTERNAL_DEFLATE_BW_ALIGN( ); INTERNAL_DEFLATE_BW_U16LE( ( uint16_t )chunk ); INTERNAL_DEFLATE_BW_U16LE( ( uint16_t )~chunk );
                  memcpy( op, in + off, ( size_t )chunk ); op += chunk;
                  off += chunk; remain -= chunk;
              }
          } else {
              INTERNAL_DEFLATE_BW_PUT( ( blk_end == n_bytes ) ? 1u : 0u, 1 ); INTERNAL_DEFLATE_BW_PUT( 2u, 2 );
              INTERNAL_DEFLATE_BW_PUT( ( uint32_t )( HLIT - 257 ), 5 );
              INTERNAL_DEFLATE_BW_PUT( ( uint32_t )( HDIST - 1 ), 5 );
              INTERNAL_DEFLATE_BW_PUT( ( uint32_t )( HCLEN - 4 ), 4 );
              for( int k = 0;k<HCLEN;++k ) INTERNAL_DEFLATE_BW_PUT( pre_len[ PREPERM[ k ] ], 3 );
              for( int k = 0;k<cln;++k ) {
                  uint8_t s = cl[ k ].sym; INTERNAL_DEFLATE_BW_PUT( pre_code[ s ], pre_len[ s ]);
                  if( s == 16 ) INTERNAL_DEFLATE_BW_PUT( cl[ k ].xb, 2 );
                  else if( s == 17 ) INTERNAL_DEFLATE_BW_PUT( cl[ k ].xb, 3 );
                  else if( s == 18 ) INTERNAL_DEFLATE_BW_PUT( cl[ k ].xb, 7 );
              }
              for( int t = 0;t<ntok;++t ) {
                  if( tok_dist[ t ]== 0 ) {
                      uint16_t lit = tok_len[ t ];
                      INTERNAL_DEFLATE_BW_PUT( codeL[ lit ], ll_len[ lit ]);
                  } else {
                      int L = tok_len[ t ], D = tok_dist[ t ];
                      int lc, lxb = 0, lxv = 0; INTERNAL_DEFLATE_FIND_LENGTH_CODE( L, lc, lxb, lxv );
                      int dc = 0, dxb = 0, dxv = 0; INTERNAL_DEFLATE_FIND_DISTANCE_CODE( D, dc, dxb, dxv );
                      INTERNAL_DEFLATE_BW_PUT( codeL[ lc ], ll_len[ lc ]); if( lxb ) INTERNAL_DEFLATE_BW_PUT( ( uint32_t )lxv, lxb );
                      INTERNAL_DEFLATE_BW_PUT( codeD[ dc ], d_len[ dc ]);   if( dxb ) INTERNAL_DEFLATE_BW_PUT( ( uint32_t )dxv, dxb );
                  }
              }
              INTERNAL_DEFLATE_BW_PUT( codeL[ LIT_EOB ], ll_len[ LIT_EOB ]);
          }
          pos = blk_end;
      }
      INTERNAL_DEFLATE_BW_ALIGN( );
      return( int )( op -( unsigned char* )out_buf );
      #undef INTERNAL_DEFLATE_BUILD_LEN
      #undef INTERNAL_DEFLATE_HEAP_POP
      #undef INTERNAL_DEFLATE_HEAP_PUSH
      #undef INTERNAL_DEFLATE_SIFTDOWN
      #undef INTERNAL_DEFLATE_LT
      #undef INTERNAL_DEFLATE_HASH4
      #undef INTERNAL_DEFLATE_RD32_LE
      #undef INTERNAL_DEFLATE_BW_U16LE
      #undef INTERNAL_DEFLATE_CL_PUSH
      #undef INTERNAL_DEFLATE_BW_PUT
      #undef INTERNAL_DEFLATE_BW_ALIGN
      #undef INTERNAL_DEFLATE_GEN_CANON_CODES
      #undef INTERNAL_DEFLATE_FIND_LENGTH_CODE
      #undef INTERNAL_DEFLATE_FIND_DISTANCE_CODE
      #undef INTERNAL_DEFLATE_FIND_MATCH
    }

    #define VIDEOCODEC_PACK_ARENA_SIZE INTERNAL_VIDEOCODEC_DEFLATE_ARENA_SIZE
    #define VIDEOCODEC_PACK( out, in, size, arena ) internal_videocodec_deflate( out, in, size, arena )
    #define VIDEOCODEC_UNPACK( out, capacity, in, size ) internal_videocodec_inflate( out, capacity, in, size )
#endif


static const uint8_t INTERNAL_VIDEOCODEC_VERSION = 0;


typedef struct internal_videocodec_quality_params_t {
    int q_ac_y_num, q_ac_y_den;
    int q_ac_c_num, q_ac_c_den;
    int q_dc_y_num, q_dc_y_den;
    int q_dc_c_num, q_dc_c_den;
    int q_edge_num, q_edge_den;
    int q_hf_y_num, q_hf_y_den;
    int q_hf_c_num, q_hf_c_den;
    int q_dz_num, q_dz_den;
    int rd_lambda;
    int skip_luma_max, skip_luma_sum;
    int skip_chroma_max, skip_chroma_sum;
    int skip_luma_mean_abs_max, skip_chroma_mean_abs_max;
    int skip_coarse_mv_margin;
} internal_videocodec_quality_params_t;


static inline void internal_videocodec_quality_from_enum( enum videocodec_quality_t lvl, internal_videocodec_quality_params_t* q ) {
    switch( lvl ) {
        case VIDEOCODEC_QUALITY_MIN:
            q->q_dc_y_num = 180; q->q_dc_y_den = 100;
            q->q_dc_c_num = 210; q->q_dc_c_den = 100;
            q->q_ac_y_num = 160; q->q_ac_y_den = 100;
            q->q_ac_c_num = 180; q->q_ac_c_den = 100;
            q->q_dz_num = 80; q->q_dz_den = 40;
            q->rd_lambda = 110;
            q->skip_luma_max = 8; q->skip_luma_sum = 2048;
            q->skip_chroma_max = 9; q->skip_chroma_sum = 512;
            q->skip_luma_mean_abs_max = 2; q->skip_chroma_mean_abs_max = 3;
            q->skip_coarse_mv_margin = 16;
            q->q_edge_num = 6; q->q_edge_den = 5;
            q->q_hf_y_num = 64; q->q_hf_y_den = 40;
            q->q_hf_c_num = 68; q->q_hf_c_den = 40;
            break;
        case VIDEOCODEC_QUALITY_LOW:
            q->q_dc_y_num = 120; q->q_dc_y_den = 100;
            q->q_dc_c_num = 130; q->q_dc_c_den = 100;
            q->q_ac_y_num = 130; q->q_ac_y_den = 100;
            q->q_ac_c_num = 145; q->q_ac_c_den = 100;
            q->q_dz_num = 60; q->q_dz_den = 40;
            q->rd_lambda = 60;
            q->skip_luma_max = 3; q->skip_luma_sum = 384;
            q->skip_chroma_max = 4; q->skip_chroma_sum = 96;
            q->skip_luma_mean_abs_max = 1; q->skip_chroma_mean_abs_max = 1;
            q->skip_coarse_mv_margin = 8;
            q->q_edge_num = 5; q->q_edge_den = 5;
            q->q_hf_y_num = 56; q->q_hf_y_den = 40;
            q->q_hf_c_num = 60; q->q_hf_c_den = 40;
            break;
        case VIDEOCODEC_QUALITY_HIGH:
            q->q_dc_y_num = 100; q->q_dc_y_den = 100;
            q->q_dc_c_num = 100; q->q_dc_c_den = 100;
            q->q_ac_y_num = 70; q->q_ac_y_den = 100;
            q->q_ac_c_num = 75; q->q_ac_c_den = 100;
            q->q_dz_num = 40; q->q_dz_den = 40;
            q->rd_lambda = 28;
            q->skip_luma_max = 1; q->skip_luma_sum = 128;
            q->skip_chroma_max = 2; q->skip_chroma_sum = 32;
            q->skip_luma_mean_abs_max = 0; q->skip_chroma_mean_abs_max = 0;
            q->skip_coarse_mv_margin = 3;
            q->q_edge_num = 3; q->q_edge_den = 5;
            q->q_hf_y_num = 43; q->q_hf_y_den = 40;
            q->q_hf_c_num = 45; q->q_hf_c_den = 40;
            break;
        case VIDEOCODEC_QUALITY_MAX:
            q->q_dc_y_num = 100; q->q_dc_y_den = 100;
            q->q_dc_c_num = 100; q->q_dc_c_den = 100;
            q->q_ac_y_num = 50; q->q_ac_y_den = 100;
            q->q_ac_c_num = 55; q->q_ac_c_den = 100;
            q->q_dz_num = 20; q->q_dz_den = 40;
            q->rd_lambda = 18;
            q->skip_luma_max = 0; q->skip_luma_sum = 64;
            q->skip_chroma_max = 1; q->skip_chroma_sum = 16;
            q->skip_luma_mean_abs_max = 0; q->skip_chroma_mean_abs_max = 0;
            q->skip_coarse_mv_margin = 2;
            q->q_edge_num = 2; q->q_edge_den = 5;
            q->q_hf_y_num = 41; q->q_hf_y_den = 40;
            q->q_hf_c_num = 43; q->q_hf_c_den = 40;
            break;
        case VIDEOCODEC_QUALITY_DEFAULT:
        default:
            q->q_dc_y_num = 100; q->q_dc_y_den = 100;
            q->q_dc_c_num = 100; q->q_dc_c_den = 100;
            q->q_ac_y_num = 103; q->q_ac_y_den = 100;
            q->q_ac_c_num = 109; q->q_ac_c_den = 100;
            q->q_dz_num = 53; q->q_dz_den = 40;
            q->rd_lambda = 42;
            q->skip_luma_max = 2; q->skip_luma_sum = 256;
            q->skip_chroma_max = 3; q->skip_chroma_sum = 64;
            q->skip_luma_mean_abs_max = 0; q->skip_chroma_mean_abs_max = 1;
            q->skip_coarse_mv_margin = 6;
            q->q_edge_num = 4; q->q_edge_den = 5;
            q->q_hf_y_num = 47; q->q_hf_y_den = 40;
            q->q_hf_c_num = 51; q->q_hf_c_den = 40;
            break;
    }
}


static const uint8_t internal_videocodec_zz[ 64 ]={
  0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 
  12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 
  35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 
  58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

static const uint8_t internal_videocodec_QY[ 64 ]={
  8, 16, 19, 22, 26, 27, 29, 34, 16, 16, 22, 24, 27, 29, 34, 37, 
  19, 22, 26, 27, 29, 34, 34, 38, 22, 22, 26, 27, 29, 34, 37, 40, 
  22, 26, 27, 29, 32, 35, 40, 48, 26, 27, 29, 32, 35, 40, 48, 58, 
  26, 27, 29, 34, 38, 46, 56, 69, 27, 29, 35, 38, 46, 56, 69, 83
};


static const uint8_t internal_videocodec_QC[ 64 ]={
  16, 17, 18, 19, 20, 21, 22, 24, 17, 18, 19, 20, 21, 22, 24, 25, 
  18, 19, 20, 21, 22, 24, 25, 27, 19, 20, 21, 22, 24, 25, 27, 28, 
  20, 21, 22, 24, 25, 27, 28, 30, 21, 22, 24, 25, 27, 28, 30, 32, 
  22, 24, 25, 27, 28, 30, 32, 35, 24, 25, 27, 28, 30, 32, 35, 38
};


#define INTERNAL_VIDEOCODEC_COS_SHIFT 14

static const int16_t internal_videocodec_C8[ 8 ][ 8 ] = {
  { 5793, 5793, 5793, 5793, 5793, 5793, 5793, 5793 }, 
  { 8035, 6811, 4551, 1598, -1598, -4551, -6811, -8035 }, 
  { 7568, 3135, -3135, -7568, -7568, -3135, 3135, 7568 }, 
  { 6811, -1598, -8035, -4551, 4551, 8035, 1598, -6811 }, 
  { 5793, -5793, -5793, 5793, 5793, -5793, -5793, 5793 }, 
  { 4551, -8035, 1598, 6811, -6811, -1598, 8035, -4551 }, 
  { 3135, -7568, 7568, -3135, -3135, 7568, -7568, 3135 }, 
  { 1598, -4551, 6811, -8035, 8035, -6811, 4551, -1598 }
};


static inline int internal_videocodec_abs( int a ) { return a < 0 ? -a : a; }
static inline int internal_videocodec_imax( int a, int b ) {return a > b ? a : b; }
static inline int internal_videocodec_imin( int a, int b ) {return a < b ? a : b; }
static inline int internal_videocodec_clampi( int v ) { return v < 0 ? 0 : ( v > 255 ? 255 : v ); }
static inline int internal_videocodec_clampii( int v, int lo, int hi ) { return v < lo ? lo : ( v > hi ? hi : v ); }
static inline int internal_videocodec_floor_div2( int v ) { return ( v >= 0 ) ? ( v >> 1 ) : -( ( -v + 1 ) >> 1 ); }
static inline int internal_videocodec_floor_div4( int v ) { return ( v >= 0 ) ? ( v >> 2 ) : -( ( -v + 3 ) >> 2 ); }


static inline void internal_videocodec_build_window( uint16_t W8[ 64 ] ) {
    const int shift = 8;
    const int W_SOFT = 240;
    const int W_MID = 212;
    const int W_HIGH = 190;
    const int W_XHI = 176;
    for( int v = 0; v < 8; ++v ) {
        for( int u = 0; u < 8; ++u ) {
            int idx = v * 8 + u;
            int s = u + v;
            int w = 256;
            if( u == 0 && v == 0 ) w = 256;
            else if( s <= 2 ) w = 256;
            else if( s <= 4 ) w = W_SOFT;
            else if( s <= 6 ) w = W_MID;
            else {
                w = W_HIGH;
                if( ( u >= 6 || v >= 6 ) ) w = ( w * W_XHI + ( 1 << ( shift - 1 ) ) ) >> shift;
            }
            W8[ idx ] = (uint16_t) w;
        }
    }
}


static inline void internal_videocodec_reshape_quant_one( uint8_t* dst, uint8_t const* src, int chroma, int edge_num, int edge_den, int hf_num, int hf_den ) {
    for( int i = 0; i < 64; i++ ) {
        int u = i & 7, v = i >> 3, s = u + v;
        int q = src[ i ];
        if( i == 0 ) {
            dst[ i ] = (uint8_t) q;
            continue;
        }
        if( s <= 2 ) {
            q = ( q * edge_num + ( edge_den >> 1 ) ) / edge_den;
            if( q < 1 ) q = 1;
        } else if( s >= 8 || u >= 6 || v >= 6 ) {
            q = ( q * hf_num + ( hf_den >> 1 ) ) / hf_den;
            if( q > 255 ) q = 255;
        }
        dst[ i ] = (uint8_t) q;
    }
}


static inline void internal_videocodec_build_quants( uint8_t QYx[ 64 ], uint8_t QCx[ 64 ], internal_videocodec_quality_params_t const* q ) {
    for( int i = 0; i < 64; i++ ) {
        QYx[ i ] = internal_videocodec_QY[ i ];
        QCx[ i ] = internal_videocodec_QC[ i ];
    }
    internal_videocodec_reshape_quant_one( QYx, internal_videocodec_QY, 0, q->q_edge_num, q->q_edge_den, q->q_hf_y_num, q->q_hf_y_den );
    internal_videocodec_reshape_quant_one( QCx, internal_videocodec_QC, 1, q->q_edge_num, q->q_edge_den, q->q_hf_c_num, q->q_hf_c_den );

    int q0y = QYx[ 0 ];
    q0y = ( q0y * q->q_dc_y_num + ( q->q_dc_y_den >> 1 ) ) / q->q_dc_y_den;
    if( q0y < 1 ) q0y = 1;
    else if( q0y > 255 ) q0y = 255;
    QYx[ 0 ] = (uint8_t) q0y;

    int q0c = QCx[ 0 ];
    q0c = ( q0c * q->q_dc_c_num + ( q->q_dc_c_den >> 1 ) ) / q->q_dc_c_den;
    if( q0c < 1 ) q0c = 1;
    else if( q0c > 255 ) q0c = 255;
    QCx[ 0 ] = (uint8_t) q0c;

    for( int i = 1; i < 64; i++ ) {
        int qy = QYx[ i ];
        qy = ( qy * q->q_ac_y_num + ( q->q_ac_y_den >> 1 ) ) / q->q_ac_y_den;
        if( qy < 1 ) qy = 1;
        else if( qy > 255 ) qy = 255;
        QYx[ i ] = (uint8_t) qy;

        int qc = QCx[ i ];
        qc = ( qc * q->q_ac_c_num + ( q->q_ac_c_den >> 1 ) ) / q->q_ac_c_den;
        if( qc < 1 ) qc = 1;
        else if( qc > 255 ) qc = 255;
        QCx[ i ] = (uint8_t) qc;
    }
}


static inline int16_t internal_videocodec_div_rnd_q( int32_t t, int16_t qstep ) {
    int32_t d = ( qstep <= 0 ) ? 1 : (int32_t) qstep;
    return (int16_t) ( ( t >= 0 ) ? ( ( t + ( d >> 1 ) ) / d ) : ( -( ( ( -t ) + ( d >> 1 ) ) / d ) ) );
}


static inline int16_t internal_videocodec_quant_dc_plain( int32_t F, int16_t qstep ) {
    return internal_videocodec_div_rnd_q( F, qstep );
}


static inline int16_t internal_videocodec_quant_ac_deadzone( int32_t F, int16_t qstep, int dz_num, int dz_den ) {
    int32_t a = ( F >= 0 ) ? F : -F;
    int32_t t0 = ( (int32_t) qstep * (int32_t) dz_num + (int32_t) dz_den ) / ( 2 * (int32_t) dz_den );
    if( a <= t0 ) return 0;
    return internal_videocodec_div_rnd_q( F, qstep );
}


static void internal_videocodec_fdct8x8_u8( uint8_t const* src, int stride, int32_t F[ 64 ] ) {
    int32_t tmp[ 64 ];
    for( int y = 0; y < 8; y++ ) {
        int32_t r[ 8 ];
        for( int x = 0; x < 8; x++ ) r[ x ] = (int32_t) src[ y * stride + x ] - 128;
        for( int u = 0; u < 8; u++ ) {
            int64_t s = 0;
            for( int x = 0; x < 8; x++ ) s += (int64_t) internal_videocodec_C8[ u ][ x ] * (int64_t) r[ x ];
            tmp[ y * 8 + u ] = (int32_t) ( ( s + ( (int64_t) 1 << ( INTERNAL_VIDEOCODEC_COS_SHIFT - 1 ) ) ) >> INTERNAL_VIDEOCODEC_COS_SHIFT );
        }
    }
    for( int u = 0; u < 8; u++ ) {
        for( int v = 0; v < 8; v++ ) {
            int64_t s = 0;
            for( int y = 0; y < 8; y++ ) s += (int64_t) internal_videocodec_C8[ v ][ y ] * (int64_t) tmp[ y * 8 + u ];
            F[ v * 8 + u ] = (int32_t) ( ( s + ( (int64_t) 1 << ( INTERNAL_VIDEOCODEC_COS_SHIFT - 1 ) ) ) >> INTERNAL_VIDEOCODEC_COS_SHIFT );
        }
    }
}


static void internal_videocodec_fdct8x8_s16( int16_t const* src, int stride, int32_t F[ 64 ] ) {
    int32_t tmp[ 64 ];
    for( int y = 0; y < 8; y++ ) {
        int32_t r[ 8 ];
        for( int x = 0; x < 8; x++ ) r[ x ] = (int32_t) src[ y * stride + x ];
        for( int u = 0; u < 8; u++ ) {
            int64_t s = 0;
            for( int x = 0; x < 8; x++ ) s += (int64_t) internal_videocodec_C8[ u ][ x ] * (int64_t) r[ x ];
            tmp[ y * 8 + u ] = (int32_t) ( ( s + ( (int64_t) 1 << ( INTERNAL_VIDEOCODEC_COS_SHIFT - 1 ) ) ) >> INTERNAL_VIDEOCODEC_COS_SHIFT );
        }
    }
    for( int u = 0; u < 8; u++ ) {
        for( int v = 0; v < 8; v++ ) {
            int64_t s = 0;
            for( int y = 0; y < 8; y++ ) s += (int64_t) internal_videocodec_C8[ v ][ y ] * (int64_t) tmp[ y * 8 + u ];
            F[ v * 8 + u ] = (int32_t) ( ( s + ( (int64_t) 1 << ( INTERNAL_VIDEOCODEC_COS_SHIFT - 1 ) ) ) >> INTERNAL_VIDEOCODEC_COS_SHIFT );
        }
    }
}


static inline void internal_videocodec_post_weight_F_ctx( int32_t F[ 64 ], uint16_t const* W8 ) {
    const int shift = 8;
    for( int i = 0; i < 64; i++ ) {
        int64_t t = (int64_t) F[ i ] * (int64_t) W8[ i ];
        F[ i ] = (int32_t) ( ( t + ( (int64_t) 1 << ( shift - 1 ) ) ) >> shift );
    }
}


static inline void internal_videocodec_copy_block_from( uint8_t const* src, int w, int h, int sx, int sy, uint8_t* dst ) {
    for( int by = 0; by < 8; ++by ) {
        int yy = internal_videocodec_clampii( sy + by, 0, h - 1 );
        const uint8_t* srow = src + (size_t) yy * w;
        for( int bx = 0; bx < 8; ++bx ) {
            int xx = internal_videocodec_clampii( sx + bx, 0, w - 1 );
            dst[ by * 8 + bx ] = srow[ xx ];
        }
    }
}


static inline void internal_videocodec_store_block( uint8_t* img, int w, int h, int x, int y, uint8_t const* blk ) {
    int bwid = ( x + 8 <= w ) ? 8 : ( w - x ), bhgt = ( y + 8 <= h ) ? 8 : ( h - y );
    for( int by = 0; by < bhgt; ++by ) {
        memcpy( img + (size_t) ( y + by ) * w + x, blk + (size_t) by * 8, (size_t) bwid );
    }
}


static inline uint8_t internal_videocodec_sample_chroma_qpel_px( uint8_t const* img, int w, int h, int x, int y, int dxq, int dyq ) {
    int bx = x + internal_videocodec_floor_div4( dxq );
    int by = y + internal_videocodec_floor_div4( dyq );
    int rx = dxq - 4 * internal_videocodec_floor_div4( dxq );
    int ry = dyq - 4 * internal_videocodec_floor_div4( dyq );
    int x0 = internal_videocodec_clampii( bx, 0, w - 1 );
    int y0 = internal_videocodec_clampii( by, 0, h - 1 );
    int x1 = internal_videocodec_clampii( bx + 1, 0, w - 1 );
    int y1 = internal_videocodec_clampii( by + 1, 0, h - 1 );
    uint8_t p00 = img[ (size_t) y0 * w + x0 ];
    uint8_t p10 = img[ (size_t) y0 * w + x1 ];
    uint8_t p01 = img[ (size_t) y1 * w + x0 ];
    uint8_t p11 = img[ (size_t) y1 * w + x1 ];
    int w00 = ( 4 - rx ) * ( 4 - ry );
    int w10 = ( rx ) * ( 4 - ry );
    int w01 = ( 4 - rx ) * ( ry );
    int w11 = ( rx ) * ( ry );
    int s = w00 * p00 + w10 * p10 + w01 * p01 + w11 * p11;
    return (uint8_t) ( ( s + 8 ) >> 4 );
}


static inline void internal_videocodec_copy_block_from_frac_chroma( uint8_t const* src, int w, int h, int sx, int sy, int dxq, int dyq, uint8_t* dst ) {
    for( int by = 0; by < 8; ++by ) {
        for( int bx = 0; bx < 8; ++bx ) {
            dst[ by * 8 + bx ] = internal_videocodec_sample_chroma_qpel_px( src, w, h, sx + bx, sy + by, dxq, dyq );
        }
    }
}


static inline uint8_t internal_videocodec_sample_luma_hpel_px( uint8_t const* img, int w, int h, int x, int y, int dxh, int dyh ) {
    int bx = x + internal_videocodec_floor_div2( dxh );
    int by = y + internal_videocodec_floor_div2( dyh );
    int fx = ( dxh & 1 ) != 0;
    int fy = ( dyh & 1 ) != 0;
    int x0 = internal_videocodec_clampii( bx, 0, w - 1 );
    int y0 = internal_videocodec_clampii( by, 0, h - 1 );
    int x1 = internal_videocodec_clampii( bx + 1, 0, w - 1 );
    int y1 = internal_videocodec_clampii( by + 1, 0, h - 1 );
    uint8_t p00 = img[ (size_t) y0 * w + x0 ];
    if( !fx && !fy ) return p00;
    if( fx && !fy ) {
        uint8_t p10 = img[ (size_t) y0 * w + x1 ];
        return (uint8_t) ( ( p00 + p10 + 1 ) >> 1 );
    } else if( !fx && fy ) {
        uint8_t p01 = img[ (size_t) y1 * w + x0 ];
        return (uint8_t) ( ( p00 + p01 + 1 ) >> 1 );
    } else {
        uint8_t p10 = img[ (size_t) y0 * w + x1 ];
        uint8_t p01 = img[ (size_t) y1 * w + x0 ];
        uint8_t p11 = img[ (size_t) y1 * w + x1 ];
        int s = (int) p00 + (int) p10 + (int) p01 + (int) p11;
        return (uint8_t) ( ( s + 2 ) >> 2 );
    }
}


static inline void internal_videocodec_copy_block_from_frac_luma( uint8_t const* src, int w, int h, int sx, int sy, int dxh, int dyh, uint8_t* dst ) {
    for( int by = 0; by < 8; ++by ) {
        for( int bx = 0; bx < 8; ++bx ) {
            dst[ by * 8 + bx ] = internal_videocodec_sample_luma_hpel_px( src, w, h, sx + bx, sy + by, dxh, dyh );
        }
    }
}


static inline int internal_videocodec_hadamard4_abs_sum( const int r[ 16 ] ) {
    int a[ 16 ];
    for( int i = 0; i < 4; i++ ) {
        int r0 = r[ i * 4 + 0 ], r1 = r[ i * 4 + 1 ], r2 = r[ i * 4 + 2 ], r3 = r[ i * 4 + 3 ];
        int t0 = r0 + r1, t1 = r0 - r1, t2 = r2 + r3, t3 = r2 - r3;
        a[ i * 4 + 0 ] = t0 + t2;
        a[ i * 4 + 1 ] = t1 + t3;
        a[ i * 4 + 2 ] = t0 - t2;
        a[ i * 4 + 3 ] = t1 - t3;
    }
    int sum = 0;
    for( int j = 0; j < 4; j++ ) {
        int b0 = a[ 0 * 4 + j ] + a[ 1 * 4 + j ], b1 = a[ 0 * 4 + j ] - a[ 1 * 4 + j ];
        int b2 = a[ 2 * 4 + j ] + a[ 3 * 4 + j ], b3 = a[ 2 * 4 + j ] - a[ 3 * 4 + j ];
        int c0 = b0 + b2, c1 = b1 + b3, c2 = b0 - b2, c3 = b1 - b3;
        sum += internal_videocodec_abs( c0 ) + internal_videocodec_abs( c1 ) + internal_videocodec_abs( c2 ) + internal_videocodec_abs( c3 );
    }
    return sum;
}


static int internal_videocodec_satd16x16_luma_hpel( uint8_t const* cur, int w, int h, int x, int y, uint8_t const* ref, int dxh, int dyh, int cutoff ) {
    int sum = 0;
    int r[ 16 ];
    for( int ty = 0; ty < 16; ty += 4 ) {
        for( int tx = 0; tx < 16; tx += 4 ) {
            for( int j = 0; j < 4; j++ ) {
                int yy = internal_videocodec_clampii( y + ty + j, 0, h - 1 );
                const uint8_t* crow = cur + (size_t) yy * w;
                for( int i = 0; i < 4; i++ ) {
                    int xx = internal_videocodec_clampii( x + tx + i, 0, w - 1 );
                    uint8_t p = internal_videocodec_sample_luma_hpel_px( ref, w, h, x + tx + i, y + ty + j, dxh, dyh );
                    r[ j * 4 + i ] = (int) crow[ xx ] - (int) p;
                }
            }
            sum += internal_videocodec_hadamard4_abs_sum( r );
            if( sum >= cutoff ) return sum;
        }
    }
    return sum;
}


typedef struct internal_videocodec_buffer_t {
    uint8_t* buf;
    size_t cap, len;
} internal_videocodec_buffer_t;


static void internal_videocodec_buffer_reset( internal_videocodec_buffer_t* buffer ) {
    buffer->len = 0;
}


static inline void internal_videocodec_buffer_put_u8( internal_videocodec_buffer_t* buffer, uint32_t v ) {
    buffer->buf[ buffer->len++ ] = (uint8_t) v;
}


static inline void internal_videocodec_buffer_put_i16( internal_videocodec_buffer_t* buffer, int32_t v ) {
    int16_t t = (int16_t) v;
    memcpy( buffer->buf + buffer->len, &t, 2 );
    buffer->len += 2;
}


static void internal_videocodec_deblock_plane( uint8_t* img, int w, int h, int is_chroma ) {
    if( w < 16 || h < 16 ) return;

    const int step_cap = is_chroma ? 3 : 6;
    const int edge_floor = is_chroma ? 2 : 1;

    for( int x = 8; x < w; x += 8 ) {
        const int i = x - 1;
        for( int y = 0; y < h; ++y ) {
            uint8_t* row = img + (size_t)y * w;

            int p2 = row[ i - 2 ], p1 = row[ i - 1 ], p0 = row[ i ];
            int q0 = row[ i + 1 ], q1 = row[ i + 2 ], q2 = ( i + 3 < w ) ? row[ i + 3 ] : row[ i + 2 ];

            int g  = internal_videocodec_abs( p0 - q0 );
            int rL = internal_videocodec_imax( internal_videocodec_abs( p2 - p1 ), internal_videocodec_abs( p1 - p0 ) );
            int rR = internal_videocodec_imax( internal_videocodec_abs( q2 - q1 ), internal_videocodec_abs( q1 - q0 ) );
            int flat = internal_videocodec_imax( rL, rR );

            if( g <= edge_floor ) continue;
            if( g <= flat ) continue;

            int a = ( p1 + 3*p0 + 3*q0 + q1 + 4 ) >> 3;  /* cross-edge target */
            int dp = a - p0, dq = a - q0;

            int wgt = g - flat;
            if( wgt < 0 ) wgt = 0;
            if( wgt > 12 ) wgt = 12;

            int step = ( ( wgt + 1 ) >> 1 );            /* 0..6 */
            if( step > step_cap ) step = step_cap;

            if( dp >  step ) dp = step;  else if( dp < -step ) dp = -step;
            if( dq >  step ) dq = step;  else if( dq < -step ) dq = -step;

            int p0n = internal_videocodec_clampi( p0 + dp );
            int q0n = internal_videocodec_clampi( q0 + dq );
            row[ i ]     = (uint8_t)p0n;
            row[ i + 1 ] = (uint8_t)q0n;

            if( !is_chroma ) {
                int flat2 = internal_videocodec_imax( internal_videocodec_abs( p2 - p1 ), internal_videocodec_abs( q2 - q1 ) );
                if( flat2 <= 3 ) {
                    int adj = ( step + 1 ) >> 1;         /* 0..3 */
                    int tL = ( ( p2 + p0n ) >> 1 ) - p1;
                    int tR = ( ( q2 + q0n ) >> 1 ) - q1;
                    if( tL >  adj ) tL =  adj; else if( tL < -adj ) tL = -adj;
                    if( tR >  adj ) tR =  adj; else if( tR < -adj ) tR = -adj;
                    row[ i - 1 ] = (uint8_t)internal_videocodec_clampi( p1 + tL );
                    row[ i + 2 ] = (uint8_t)internal_videocodec_clampi( q1 + tR );
                }
            }
        }
    }

    for( int yb = 8; yb < h; yb += 8 ) {
        const int rP0 = yb - 1, rQ0 = yb, rP1 = yb - 2, rP2 = yb - 3, rQ1 = yb + 1, rQ2 = yb + 2;
        for( int x = 0; x < w; ++x ) {
            int p2 = img[ (size_t)internal_videocodec_imax( rP2, 0 ) * w + x ];
            int p1 = img[ (size_t)rP1 * w + x ];
            int p0 = img[ (size_t)rP0 * w + x ];
            int q0 = img[ (size_t)rQ0 * w + x ];
            int q1 = img[ (size_t)internal_videocodec_imin( rQ1, h - 1 ) * w + x ];
            int q2 = img[ (size_t)internal_videocodec_imin( rQ2, h - 1 ) * w + x ];

            int g  = internal_videocodec_abs( p0 - q0 );
            int rL = internal_videocodec_imax( internal_videocodec_abs( p2 - p1 ), internal_videocodec_abs( p1 - p0 ) );
            int rR = internal_videocodec_imax( internal_videocodec_abs( q2 - q1 ), internal_videocodec_abs( q1 - q0 ) );
            int flat = internal_videocodec_imax( rL, rR );

            if( g <= edge_floor ) continue;
            if( g <= flat ) continue;

            int a = ( p1 + 3*p0 + 3*q0 + q1 + 4 ) >> 3;
            int dp = a - p0, dq = a - q0;

            int wgt = g - flat;
            if( wgt < 0 ) wgt = 0;
            if( wgt > 12 ) wgt = 12;

            int step = ( ( wgt + 1 ) >> 1 );
            if( step > step_cap ) step = step_cap;

            if( dp >  step ) dp = step;  else if( dp < -step ) dp = -step;
            if( dq >  step ) dq = step;  else if( dq < -step ) dq = -step;

            int p0n = internal_videocodec_clampi( p0 + dp );
            int q0n = internal_videocodec_clampi( q0 + dq );
            img[ (size_t)rP0 * w + x ] = (uint8_t)p0n;
            img[ (size_t)rQ0 * w + x ] = (uint8_t)q0n;

            if( !is_chroma ) {
                int flat2 = internal_videocodec_imax( internal_videocodec_abs( p2 - p1 ), internal_videocodec_abs( q2 - q1 ) );
                if( flat2 <= 3 ) {
                    int adj = ( step + 1 ) >> 1;
                    int tL = ( ( p2 + p0n ) >> 1 ) - p1;
                    int tR = ( ( q2 + q0n ) >> 1 ) - q1;
                    if( tL >  adj ) tL =  adj; else if( tL < -adj ) tL = -adj;
                    if( tR >  adj ) tR =  adj; else if( tR < -adj ) tR = -adj;
                    img[ (size_t)rP1 * w + x ] = (uint8_t)internal_videocodec_clampi( p1 + tL );
                    img[ (size_t)internal_videocodec_imin( rQ1, h - 1 ) * w + x ] = (uint8_t)internal_videocodec_clampi( q1 + tR );
                }
            }
        }
    }
}


static void internal_videocodec_dering_luma( uint8_t* Y, int w, int h ) {
    if( w < 3 || h < 3 ) return;
    const int FLAT_TH = 24;
    const int EXT_MARGIN = 1;
    for( int y = 1; y < h - 1; ++y ) {
        uint8_t* row = Y + (size_t) y * w;
        const uint8_t* rowN = row - w;
        const uint8_t* rowS = row + w;
        for( int x = 1; x < w - 1; ++x ) {
            int n = rowN[ x ], s = rowS[ x ], wv = row[ x - 1 ], e = row[ x + 1 ];
            int lo = internal_videocodec_imin( internal_videocodec_imin( n, s ), internal_videocodec_imin( wv, e ) );
            int hi = internal_videocodec_imax( internal_videocodec_imax( n, s ), internal_videocodec_imax( wv, e ) );
            if( hi - lo > FLAT_TH ) continue;
            int avg = ( n + s + wv + e + 2 ) >> 2;
            int px = row[ x ];
            int d = avg - px;
            if( d >= 2 ) d = 1;
            else if( d <= -2 ) d = -1;
            else d = 0;
            if( d != 0 && !( px <= lo + EXT_MARGIN || px >= hi - EXT_MARGIN ) ) d = 0;
            row[ x ] = (uint8_t) ( px + d );
        }
    }
}


static inline int internal_videocodec_rle_len_est( const int16_t zzq[ 64 ] ) {
    int len = 2, run = 0;
    for( int i = 1; i < 64; i++ ) {
        if( zzq[ i ] == 0 ) {
            run++;
            continue;
        }
        while( run > 255 ) {
            len += 3;
            run -= 255;
        }
        len += 3;
        run = 0;
    }
    len += 3;
    return len;
}


static inline void internal_videocodec_rle_write( internal_videocodec_buffer_t* buffer, const int16_t zzq[ 64 ] ) {
    internal_videocodec_buffer_put_i16( buffer, zzq[ 0 ] );
    int run = 0;
    for( int i = 1; i < 64; i++ ) {
        if( zzq[ i ] == 0 ) {
            run++;
            continue;
        }
        while( run > 255 ) {
            internal_videocodec_buffer_put_u8( buffer, 255 );
            internal_videocodec_buffer_put_i16( buffer, 0 );
            run -= 255;
        }
        internal_videocodec_buffer_put_u8( buffer, (uint8_t) run );
        internal_videocodec_buffer_put_i16( buffer, zzq[ i ] );
        run = 0;
    }
    internal_videocodec_buffer_put_u8( buffer, 0 );
    internal_videocodec_buffer_put_i16( buffer, 0 );
}


static inline uint8_t const* internal_videocodec_rle_read_block( uint8_t const* p, int16_t zzq[ 64 ] ) {
    for( int i = 0; i < 64; i++ ) zzq[ i ] = 0;
    memcpy( &zzq[ 0 ], p, 2 );
    p += 2;
    int idx = 1;
    for( ;; ) {
        uint8_t run = *p++;
        int16_t lev;
        memcpy( &lev, p, 2 );
        p += 2;
        if( run == 0 && lev == 0 ) break;
        idx += run;
        if( idx >= 64 ) break;
        zzq[ idx++ ] = lev;
    }
    return p;
}


static void internal_videocodec_down2_box( const uint8_t* src, int w, int h, uint8_t* dst ) {
    int W = w >> 1, H = h >> 1;
    for( int y = 0; y < H; ++y ) {
        int y0 = y * 2, y1 = ( y0 + 1 < h ) ? y0 + 1 : h - 1;
        const uint8_t* r0 = src + (size_t) y0 * w;
        const uint8_t* r1 = src + (size_t) y1 * w;
        for( int x = 0; x < W; x++ ) {
            int x0 = x * 2, x1 = ( x0 + 1 < w ) ? x0 + 1 : w - 1;
            int s = r0[ x0 ] + r0[ x1 ] + r1[ x0 ] + r1[ x1 ];
            dst[ (size_t) y * W + x ] = (uint8_t) ( ( s + 2 ) >> 2 );
        }
    }
}


static inline int internal_videocodec_sad_block_clamped_early( const uint8_t* a, int w, int h, int ax, int ay, const uint8_t* b, int bx, int by, int B, int cutoff ) {
    int s = 0;
    for( int yy = 0; yy < B; ++yy ) {
        int ya = ay + yy;
        if( ya < 0 ) ya = 0;
        else if( ya >= h ) ya = h - 1;
        int yb = by + yy;
        if( yb < 0 ) yb = 0;
        else if( yb >= h ) yb = h - 1;
        const uint8_t* ra = a + (size_t) ya * w;
        const uint8_t* rb = b + (size_t) yb * w;
        for( int xx = 0; xx < B; ++xx ) {
            int xa = ax + xx;
            if( xa < 0 ) xa = 0;
            else if( xa >= w ) xa = w - 1;
            int xb = bx + xx;
            if( xb < 0 ) xb = 0;
            else if( xb >= w ) xb = w - 1;
            int d = (int) ra[ xa ] - (int) rb[ xb ];
            s += ( d < 0 ? -d : d );
        }
        if( s >= cutoff ) return s;
    }
    return s;
}


enum { INTERNAL_VIDEOCODEC_FT_I = 0, INTERNAL_VIDEOCODEC_FT_P = 1 };


static inline uint8_t internal_videocodec_neutral_for_plane( int plane ) {
    return plane == 0 ? 16 : 128;
}


static void internal_videocodec_idct8x8_dequant_to_u8( int16_t const* qcoef, uint8_t const* Q, uint16_t const* W8, uint8_t* dst, int stride ) {
    int32_t F[ 64 ], tmp[ 64 ];
    for( int i = 0; i < 64; i++ ) F[ i ] = (int32_t) qcoef[ i ] * (int32_t) Q[ i ];
    internal_videocodec_post_weight_F_ctx( F, W8 );
    for( int y = 0; y < 8; y++ ) {
        for( int u = 0; u < 8; u++ ) {
            int64_t s = 0;
            for( int v = 0; v < 8; v++ ) s += (int64_t) internal_videocodec_C8[ v ][ y ] * (int64_t) F[ v * 8 + u ];
            tmp[ y * 8 + u ] = (int32_t) ( ( s + ( (int64_t) 1 << ( INTERNAL_VIDEOCODEC_COS_SHIFT - 1 ) ) ) >> INTERNAL_VIDEOCODEC_COS_SHIFT );
        }
    }
    for( int y = 0; y < 8; y++ ) {
        for( int x = 0; x < 8; x++ ) {
            int64_t s = 0;
            for( int u = 0; u < 8; u++ ) s += (int64_t) internal_videocodec_C8[ u ][ x ] * (int64_t) tmp[ y * 8 + u ];
            int32_t v = (int32_t) ( ( s + ( (int64_t) 1 << ( INTERNAL_VIDEOCODEC_COS_SHIFT - 1 ) ) ) >> INTERNAL_VIDEOCODEC_COS_SHIFT );
            dst[ y * stride + x ] = (uint8_t) internal_videocodec_clampi( v + 128 );
        }
    }
}


static void internal_videocodec_idct8x8_dequant_to_s16( int16_t const* qcoef, uint8_t const* Q, uint16_t const* W8, int16_t* dst, int dstride ) {
    int32_t F[ 64 ], tmp[ 64 ];
    for( int i = 0; i < 64; i++ ) F[ i ] = (int32_t) qcoef[ i ] * (int32_t) Q[ i ];
    internal_videocodec_post_weight_F_ctx( F, W8 );
    for( int y = 0; y < 8; y++ ) {
        for( int u = 0; u < 8; u++ ) {
            int64_t s = 0;
            for( int v = 0; v < 8; v++ ) s += (int64_t) internal_videocodec_C8[ v ][ y ] * (int64_t) F[ v * 8 + u ];
            tmp[ y * 8 + u ] = (int32_t) ( ( s + ( (int64_t) 1 << ( INTERNAL_VIDEOCODEC_COS_SHIFT - 1 ) ) ) >> INTERNAL_VIDEOCODEC_COS_SHIFT );
        }
    }
    for( int y = 0; y < 8; y++ ) {
        for( int x = 0; x < 8; x++ ) {
            int64_t s = 0;
            for( int u = 0; u < 8; u++ ) s += (int64_t) internal_videocodec_C8[ u ][ x ] * (int64_t) tmp[ y * 8 + u ];
            int32_t v = (int32_t) ( ( s + ( (int64_t) 1 << ( INTERNAL_VIDEOCODEC_COS_SHIFT - 1 ) ) ) >> INTERNAL_VIDEOCODEC_COS_SHIFT );
            dst[ y * dstride + x ] = (int16_t) v;
        }
    }
}


static inline int internal_videocodec_all_zero_64( const int16_t* zzq ) {
    for( int i = 0; i < 64; i++ )
        if( zzq[ i ] != 0 ) return 0;
    return 1;
}


static inline uint8_t internal_videocodec_make_cbp6( const int16_t Y[ 4 ][ 64 ], const int16_t U[ 64 ], const int16_t V[ 64 ] ) {
    uint8_t cbp = 0;
    for( int i = 0; i < 4; i++ )
        if( !internal_videocodec_all_zero_64( Y[ i ] ) ) cbp |= (uint8_t) ( 1u << i );
    if( !internal_videocodec_all_zero_64( U ) ) cbp |= (uint8_t) ( 1u << 4 );
    if( !internal_videocodec_all_zero_64( V ) ) cbp |= (uint8_t) ( 1u << 5 );
    return cbp;
}


struct videocodec_enc_t {
    int w, h, fps_n, fps_d;
    int sar_n, sar_d;
    internal_videocodec_quality_params_t q;
    uint8_t QYx[ 64 ], QCx[ 64 ];
    uint16_t W8[ 64 ];
    internal_videocodec_buffer_t buffer;
    uint8_t *rY, *rU, *rV;
    uint8_t *refY, *refU, *refV;
    uint8_t *Y2, *R2, *Y4, *R4;
    uint8_t* out;
    size_t out_len;
    size_t out_cap;
    size_t fidx;
    int wrote_header;
    uint8_t *tY, *tU, *tV;
    int mb_w, mb_h, mb_n;
    int cir_K;
    int cir_frame;
    uint16_t* cir_gid;
    int frames_since_last_i;
    int last_cut_sad_perpx;
    int last_cut_hist_l1_mmp;
    videocodec_enc_stats_t stats;
    videocodec_enc_stats_t* user_stats;
    uint8_t pack_arena[ VIDEOCODEC_PACK_ARENA_SIZE ];
};


static inline uint32_t internal_videocodec_hash_xy( uint32_t x, uint32_t y ) {
    return ( x * 73856093u ) ^ ( y * 19349663u ) ^ 0x9e3779b9u;
}


static void internal_videocodec_cir_build_map( videocodec_enc_t* e ) {
    const int K_default = 120;
    e->cir_K = K_default > 0 ? K_default : 1;
    e->cir_frame = 0;
    if( e->cir_K == 1 ) {
        for( int i = 0; i < e->mb_n; i++ ) e->cir_gid[ i ] = 0;
        return;
    }
    int nA = 0;
    for( int my = 0; my < e->mb_h; ++my ) {
        for( int mx = 0; mx < e->mb_w; ++mx ) {
            if( ( ( mx + my ) & 1 ) == 0 ) nA++;
        }
    }
    int K = e->cir_K;
    int KA = (int) ( ( (long long) K * nA + ( e->mb_n / 2 ) ) / e->mb_n );
    if( KA < 1 ) KA = 1;
    if( KA > K - 1 ) KA = K - 1;
    int KB = K - KA;
    for( int my = 0; my < e->mb_h; ++my ) {
        for( int mx = 0; mx < e->mb_w; ++mx ) {
            int idx = my * e->mb_w + mx;
            if( K == 1 ) {
                e->cir_gid[ idx ] = 0;
                continue;
            }
            int parity = ( mx + my ) & 1;
            int base = parity ? KA : 0;
            int Kcol = parity ? KB : KA;
            uint16_t g = (uint16_t) ( base + ( Kcol > 0 ? ( internal_videocodec_hash_xy( (uint32_t) mx, (uint32_t) my ) % (uint32_t) Kcol ) : 0 ) );
            e->cir_gid[ idx ] = g;
        }
    }
}


static void internal_videocodec_encode_block_I( internal_videocodec_buffer_t* buffer, uint8_t const* p, int stride, uint8_t const* Q, uint16_t const* W8, uint8_t* recon, int rstride, internal_videocodec_quality_params_t const* qp ) {
    int32_t F[ 64 ];
    int16_t cq[ 64 ], zzq[ 64 ], rq[ 64 ];
    internal_videocodec_fdct8x8_u8( p, stride, F );
    cq[ 0 ] = internal_videocodec_quant_dc_plain( F[ 0 ], (int16_t) Q[ 0 ] );
    for( int i = 1; i < 64; i++ ) cq[ i ] = internal_videocodec_quant_ac_deadzone( F[ i ], (int16_t) Q[ i ], qp->q_dz_num, qp->q_dz_den );
    for( int i = 0; i < 64; i++ ) zzq[ i ] = cq[ internal_videocodec_zz[ i ] ];
    internal_videocodec_rle_write( buffer, zzq );
    for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
    internal_videocodec_idct8x8_dequant_to_u8( rq, Q, W8, recon, rstride );
}


static void internal_videocodec_encode_plane_I( internal_videocodec_buffer_t* buffer, uint8_t const* src, int w, int h, uint8_t const* Q, uint16_t const* W8, uint8_t* recon, internal_videocodec_quality_params_t const* qp, int plane ) {
    for( int y = 0; y < h; y += 8 ) {
        for( int x = 0; x < w; x += 8 ) {
            int bwid = ( x + 8 <= w ) ? 8 : ( w - x );
            int bhgt = ( y + 8 <= h ) ? 8 : ( h - y );
            if( bwid == 8 && bhgt == 8 ) {
                internal_videocodec_encode_block_I( buffer, src + (size_t) y * w + x, w, Q, W8, recon + (size_t) y * w + x, w, qp );
            } else {
                uint8_t s8[ 64 ], r8[ 64 ];
                uint8_t fill = internal_videocodec_neutral_for_plane( plane );
                for( int by = 0; by < 8; ++by ) {
                    for( int bx = 0; bx < 8; ++bx ) {
                        s8[ by * 8 + bx ] = ( bx < bwid && by < bhgt ) ? src[ (size_t) ( y + by ) * w + ( x + bx ) ] : fill;
                    }
                }
                internal_videocodec_encode_block_I( buffer, s8, 8, Q, W8, r8, 8, qp );
                for( int by = 0; by < bhgt; ++by ) {
                    memcpy( recon + (size_t) ( y + by ) * w + x, r8 + (size_t) by * 8, (size_t) bwid );
                }
            }
        }
    }
}


static void internal_videocodec_trial_I_block( const uint8_t* p, int stride, const uint8_t* Q, const uint16_t* W8, const internal_videocodec_quality_params_t* qp, int16_t out_zzq[ 64 ], uint8_t out_recon[ 64 ], int* out_rle_bytes, int64_t* out_sse, int bwid, int bhgt ) {
    int32_t F[ 64 ];
    int16_t cq[ 64 ], zzq[ 64 ], rq[ 64 ];
    internal_videocodec_fdct8x8_u8( p, stride, F );
    cq[ 0 ] = internal_videocodec_quant_dc_plain( F[ 0 ], (int16_t) Q[ 0 ] );
    for( int i = 1; i < 64; i++ ) cq[ i ] = internal_videocodec_quant_ac_deadzone( F[ i ], (int16_t) Q[ i ], qp->q_dz_num, qp->q_dz_den );
    for( int i = 0; i < 64; i++ ) zzq[ i ] = cq[ internal_videocodec_zz[ i ] ];
    for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
    internal_videocodec_idct8x8_dequant_to_u8( rq, Q, W8, out_recon, 8 );
    *out_rle_bytes = internal_videocodec_rle_len_est( zzq );
    *out_sse = 0;
    for( int by = 0; by < bhgt; ++by ) {
        for( int bx = 0; bx < bwid; ++bx ) {
            int d = (int) p[ by * stride + bx ] - (int) out_recon[ by * 8 + bx ];
            *out_sse += (int64_t) d * (int64_t) d;
        }
    }
    memcpy( out_zzq, zzq, sizeof( int16_t ) * 64 );
}


static void internal_videocodec_trial_P_block( const uint8_t* cur, int cstride, const uint8_t* pred, int pstride, const uint8_t* Q, const uint16_t* W8, const internal_videocodec_quality_params_t* qp, int is_luma, int16_t out_zzq[ 64 ], int16_t out_add16[ 64 ], int* out_rle_bytes, int64_t* out_sse, int bwid, int bhgt ) {
    const int SMALL_TH = is_luma ? 1 : 2;
    int16_t R[ 64 ];
    int maxabs = 0;
    for( int y = 0; y < 8; ++y ) {
        for( int x = 0; x < 8; ++x ) {
            int v = 0;
            if( y < bhgt && x < bwid ) {
                int d = (int) cur[ y * cstride + x ] - (int) pred[ y * pstride + x ];
                v = d;
                int ad = d < 0 ? -d : d;
                if( ad > maxabs ) maxabs = ad;
            }
            R[ y * 8 + x ] = (int16_t) v;
        }
    }
    if( maxabs <= SMALL_TH ) {
        for( int i = 0; i < 64; i++ ) out_zzq[ i ] = 0;
        for( int i = 0; i < 64; i++ ) out_add16[ i ] = 0;
        *out_rle_bytes = 5;
        int64_t sse = 0;
        for( int by = 0; by < bhgt; ++by ) {
            for( int bx = 0; bx < bwid; ++bx ) {
                int idx = by * 8 + bx;
                int d = (int) R[ idx ];
                sse += (int64_t) d * (int64_t) d;
            }
        }
        *out_sse = sse;
        return;
    }
    int32_t F[ 64 ];
    int16_t cq[ 64 ], zzq[ 64 ], rq[ 64 ], add16[ 64 ];
    internal_videocodec_fdct8x8_s16( R, 8, F );
    cq[ 0 ] = internal_videocodec_quant_dc_plain( F[ 0 ], (int16_t) Q[ 0 ] );
    for( int i = 1; i < 64; i++ ) cq[ i ] = internal_videocodec_quant_ac_deadzone( F[ i ], (int16_t) Q[ i ], qp->q_dz_num, qp->q_dz_den );
    {
        int sum_ac = 0;
        for( int i = 1; i < 64; i++ ) {
            int v = cq[ i ];
            sum_ac += ( v < 0 ? -v : v );
            if( sum_ac > 2 ) break;
        }
        int dcabs = cq[ 0 ];
        if( dcabs < 0 ) dcabs = -dcabs;
        if( sum_ac <= 2 && dcabs <= 1 )
            for( int i = 0; i < 64; i++ ) cq[ i ] = 0;
    }
    for( int i = 0; i < 64; i++ ) zzq[ i ] = cq[ internal_videocodec_zz[ i ] ];
    *out_rle_bytes = internal_videocodec_rle_len_est( zzq );
    for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
    internal_videocodec_idct8x8_dequant_to_s16( rq, Q, W8, add16, 8 );
    int64_t sse = 0;
    for( int by = 0; by < bhgt; ++by ) {
        for( int bx = 0; bx < bwid; ++bx ) {
            int idx = by * 8 + bx;
            int d = (int) R[ idx ] - (int) add16[ idx ];
            sse += (int64_t) d * (int64_t) d;
        }
    }
    *out_sse = sse;
    memcpy( out_zzq, zzq, sizeof( int16_t ) * 64 );
    memcpy( out_add16, add16, sizeof( int16_t ) * 64 );
}


static void internal_videocodec_encode_iframe( uint8_t const* Y, uint8_t const* U, uint8_t const* V, int w, int h, internal_videocodec_buffer_t* buffer, 
	uint8_t* rY, uint8_t* rU, uint8_t* rV, uint8_t const* QY, uint8_t const* QC, uint16_t const* W8, internal_videocodec_quality_params_t const* qp ) {
		
    internal_videocodec_buffer_reset( buffer );
    internal_videocodec_buffer_put_u8( buffer, (uint8_t) INTERNAL_VIDEOCODEC_FT_I );
    internal_videocodec_encode_plane_I( buffer, Y, w, h, QY, W8, rY, qp, 0 );
    internal_videocodec_encode_plane_I( buffer, U, w / 2, h / 2, QC, W8, rU, qp, 1 );
    internal_videocodec_encode_plane_I( buffer, V, w / 2, h / 2, QC, W8, rV, qp, 1 );
    internal_videocodec_deblock_plane( rY, w, h, 0 );
    internal_videocodec_deblock_plane( rU, w / 2, h / 2, 1 );
    internal_videocodec_deblock_plane( rV, w / 2, h / 2, 1 );
    internal_videocodec_dering_luma( rY, w, h );
}


static void internal_videocodec_search_best_mv16x16( const uint8_t* Y, int w, int h, int x, int y, const uint8_t* ref, int rad, 
	const uint8_t* Y2, int w2, int h2, const uint8_t* R2, const uint8_t* Y4, int w4, int h4, const uint8_t* R4, int* out_dxh, int* out_dyh ) {
		
    const int BIAS_SAD_4x = 1;
    const int BIAS_SAD_2x = 2;
    const int BIAS_SATD_1x = 6;
    const int FRAC_PEN = 2;
    int cx4 = x >> 2, cy4 = y >> 2;
    int radf4 = rad >> 2;
    if( radf4 < 2 ) radf4 = 2;
    int bx4 = 0, by4 = 0, best4 = INT_MAX;
    for( int dy = -radf4; dy <= radf4; ++dy ) {
        for( int dx = -radf4; dx <= radf4; ++dx ) {
            int s = internal_videocodec_sad_block_clamped_early( Y4, w4, h4, cx4, cy4, R4, cx4 + dx, cy4 + dy, 4, best4 );
            s += BIAS_SAD_4x * ( internal_videocodec_abs( dx ) + internal_videocodec_abs( dy ) );
            if( s < best4 ) {
                best4 = s;
                bx4 = dx;
                by4 = dy;
            }
        }
    }
    int bx2 = bx4 << 1, by2 = by4 << 1;
    int cx2 = x >> 1, cy2 = y >> 1;
    int best2 = internal_videocodec_sad_block_clamped_early( Y2, w2, h2, cx2, cy2, R2, cx2 + bx2, cy2 + by2, 8, INT_MAX );
    best2 += BIAS_SAD_2x * ( internal_videocodec_abs( bx2 ) + internal_videocodec_abs( by2 ) );
    static const int pat[ 8 ][ 2 ] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }, { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 } };
    for( int step = 2; step >= 1; --step ) {
        int improved = 1;
        while( improved ) {
            improved = 0;
            for( int i = 0; i < 8; ++i ) {
                int dx = bx2 + pat[ i ][ 0 ] * step;
                int dy = by2 + pat[ i ][ 1 ] * step;
                int s = internal_videocodec_sad_block_clamped_early( Y2, w2, h2, cx2, cy2, R2, cx2 + dx, cy2 + dy, 8, best2 );
                s += BIAS_SAD_2x * ( internal_videocodec_abs( dx ) + internal_videocodec_abs( dy ) );
                if( s < best2 ) {
                    best2 = s;
                    bx2 = dx;
                    by2 = dy;
                    improved = 1;
                }
            }
        }
    }
    int best_dxh = ( bx2 << 1 );
    int best_dyh = ( by2 << 1 );
    int bestF = internal_videocodec_satd16x16_luma_hpel( Y, w, h, x, y, ref, best_dxh, best_dyh, INT_MAX );
    bestF += BIAS_SATD_1x * ( internal_videocodec_abs( best_dxh ) + internal_videocodec_abs( best_dyh ) );
    bestF += FRAC_PEN * ( ( best_dxh & 1 ) + ( best_dyh & 1 ) );
    for( int iy = -2; iy <= 2; ++iy ) {
        for( int ix = -2; ix <= 2; ++ix ) {
            int dxh = ( bx2 + ix ) << 1;
            int dyh = ( by2 + iy ) << 1;
            int s = internal_videocodec_satd16x16_luma_hpel( Y, w, h, x, y, ref, dxh, dyh, bestF );
            s += BIAS_SATD_1x * ( internal_videocodec_abs( dxh ) + internal_videocodec_abs( dyh ) );
            s += FRAC_PEN * ( ( dxh & 1 ) + ( dyh & 1 ) );
            if( s < bestF ) {
                bestF = s;
                best_dxh = dxh;
                best_dyh = dyh;
            }
        }
    }
    for( int fy = -1; fy <= 1; ++fy ) {
        for( int fx = -1; fx <= 1; ++fx ) {
            int dxh = best_dxh + fx;
            int dyh = best_dyh + fy;
            int s = internal_videocodec_satd16x16_luma_hpel( Y, w, h, x, y, ref, dxh, dyh, bestF );
            s += BIAS_SATD_1x * ( internal_videocodec_abs( dxh ) + internal_videocodec_abs( dyh ) );
            s += FRAC_PEN * ( ( dxh & 1 ) + ( dyh & 1 ) );
            if( s < bestF ) {
                bestF = s;
                best_dxh = dxh;
                best_dyh = dyh;
            }
        }
    }
    *out_dxh = best_dxh;
    *out_dyh = best_dyh;
}


static void internal_videocodec_encode_pframe( uint8_t const* Y, uint8_t const* U, uint8_t const* V, int w, int h, internal_videocodec_buffer_t* buffer, 
	uint8_t* rY, uint8_t* rU, uint8_t* rV, uint8_t* refY, uint8_t* refU, uint8_t* refV, uint8_t* Y2, uint8_t* R2, uint8_t* Y4, uint8_t* R4, uint16_t const* cir_gid, int cir_group, int mb_w, 
	uint8_t const* QY, uint8_t const* QC, uint16_t const* W8, internal_videocodec_quality_params_t const* qp ) {
		
    size_t ysz = (size_t) w * h, csz = (size_t) ( w / 2 ) * ( h / 2 );
    memcpy( refY, rY, ysz );
    memcpy( refU, rU, csz );
    memcpy( refV, rV, csz );
    int w2 = w >> 1, h2 = h >> 1, w4 = w >> 2, h4 = h >> 2;
    internal_videocodec_down2_box( Y, w, h, Y2 );
    internal_videocodec_down2_box( refY, w, h, R2 );
    internal_videocodec_down2_box( Y2, w2, h2, Y4 );
    internal_videocodec_down2_box( R2, w2, h2, R4 );
    internal_videocodec_buffer_reset( buffer );
    internal_videocodec_buffer_put_u8( buffer, (uint8_t) INTERNAL_VIDEOCODEC_FT_P );
    const int RD_LAMBDA_BUMP = (int) ( ( qp->rd_lambda * 11 + 5 ) / 10 );
    const int SEARCH_RAD = 96;
    for( int yb = 0; yb < h; yb += 16 ) {
        for( int xb = 0; xb < w; xb += 16 ) {
            int mbx = xb >> 4, mby = yb >> 4;
            int mbi = mby * mb_w + mbx;
            int force_intra = ( cir_gid && cir_group >= 0 ) ? ( cir_gid[ mbi ] == (uint16_t) cir_group ) : 0;
            if( !force_intra ) {
                int bwid = ( xb + 16 <= w ) ? 16 : ( w - xb );
                int bhgt = ( yb + 16 <= h ) ? 16 : ( h - yb );
                const int N_y = bwid * bhgt;
                int sumY_abs = 0, maxY = 0, sumY_signed = 0;
                for( int y = 0; y < bhgt; y++ ) {
                    const uint8_t* a = Y + (size_t) ( yb + y ) * w + xb;
                    const uint8_t* b = refY + (size_t) ( yb + y ) * w + xb;
                    for( int x = 0; x < bwid; x++ ) {
                        int d = (int) a[ x ] - (int) b[ x ];
                        int ad = d < 0 ? -d : d;
                        sumY_abs += ad;
                        sumY_signed += d;
                        if( ad > maxY ) maxY = ad;
                    }
                }
                int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                int cbw = ( cx + 8 <= cw ) ? 8 : ( cw - cx );
                int cbh = ( cy + 8 <= ch ) ? 8 : ( ch - cy );
                const int N_c = cbw * cbh;
                int sumU_abs = 0, maxU = 0, sumU_signed = 0;
                int sumV_abs = 0, maxV = 0, sumV_signed = 0;
                for( int y = 0; y < cbh; y++ ) {
                    const uint8_t* aU = U + (size_t) ( cy + y ) * cw + cx;
                    const uint8_t* bU = refU + (size_t) ( cy + y ) * cw + cx;
                    const uint8_t* aV = V + (size_t) ( cy + y ) * cw + cx;
                    const uint8_t* bV = refV + (size_t) ( cy + y ) * cw + cx;
                    for( int x = 0; x < cbw; x++ ) {
                        int du = (int) aU[ x ] - (int) bU[ x ];
                        int dv = (int) aV[ x ] - (int) bV[ x ];
                        int au = du < 0 ? -du : du, av = dv < 0 ? -dv : dv;
                        sumU_abs += au;
                        sumV_abs += av;
                        sumU_signed += du;
                        sumV_signed += dv;
                        if( au > maxU ) maxU = au;
                        if( av > maxV ) maxV = av;
                    }
                }
                int amp_ok =
                    ( maxY <= qp->skip_luma_max && sumY_abs <= qp->skip_luma_sum ) &&
                    ( maxU <= qp->skip_chroma_max && sumU_abs <= qp->skip_chroma_sum ) &&
                    ( maxV <= qp->skip_chroma_max && sumV_abs <= qp->skip_chroma_sum );
                int mean_ok =
                    ( internal_videocodec_abs( sumY_signed ) <= qp->skip_luma_mean_abs_max * N_y ) &&
                    ( internal_videocodec_abs( sumU_signed ) <= qp->skip_chroma_mean_abs_max * N_c ) &&
                    ( internal_videocodec_abs( sumV_signed ) <= qp->skip_chroma_mean_abs_max * N_c );
                int mv0_ok = 0;
                if( w4 >= 4 && h4 >= 4 ) {
                    int cx4 = xb >> 2, cy4 = yb >> 2;
                    int best = INT_MAX, s00;
                    s00 = internal_videocodec_sad_block_clamped_early( Y4, w4, h4, cx4, cy4, R4, cx4, cy4, 4, INT_MAX );
                    for( int dy = -1; dy <= 1; ++dy ) {
                        for( int dx = -1; dx <= 1; ++dx ) {
                            int s = internal_videocodec_sad_block_clamped_early( Y4, w4, h4, cx4, cy4, R4, cx4 + dx, cy4 + dy, 4, best );
                            if( s < best ) best = s;
                        }
                    }
                    mv0_ok = ( s00 <= best + qp->skip_coarse_mv_margin );
                }
                if( amp_ok && mean_ok && mv0_ok ) {
                    internal_videocodec_buffer_put_u8( buffer, 0 );
                    for( int by = 0; by < 2; ++by )
                        for( int bx = 0; bx < 2; ++bx ) {
                            int x = xb + bx * 8, y = yb + by * 8;
                            uint8_t blk[ 64 ];
                            internal_videocodec_copy_block_from( refY, w, h, x, y, blk );
                            internal_videocodec_store_block( rY, w, h, x, y, blk );
                        }
                    {
                        int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                        uint8_t blk[ 64 ];
                        internal_videocodec_copy_block_from( refU, cw, ch, cx, cy, blk );
                        internal_videocodec_store_block( rU, cw, ch, cx, cy, blk );
                    }
                    {
                        int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                        uint8_t blk[ 64 ];
                        internal_videocodec_copy_block_from( refV, cw, ch, cx, cy, blk );
                        internal_videocodec_store_block( rV, cw, ch, cx, cy, blk );
                    }
                    continue;
                }
            }
            if( ( cir_gid && cir_group >= 0 ) && ( cir_gid[ ( yb >> 4 ) * mb_w + ( xb >> 4 ) ] == (uint16_t) cir_group ) ) {
                int16_t zzYi[ 4 ][ 64 ], zzUi[ 64 ], zzVi[ 64 ];
                uint8_t recYi[ 4 ][ 64 ], recUi[ 64 ], recVi[ 64 ];
                int64_t dummy_sse;
                int dummy_bytes;
                for( int by = 0; by < 2; ++by ) {
                    for( int bx = 0; bx < 2; ++bx ) {
                        int x = xb + bx * 8, y = yb + by * 8;
                        int bwid = ( x + 8 <= w ) ? 8 : ( w - x ), bhgt = ( y + 8 <= h ) ? 8 : ( h - y );
                        internal_videocodec_trial_I_block( Y + (size_t) y * w + x, w, QY, W8, qp, zzYi[ by * 2 + bx ], recYi[ by * 2 + bx ], &dummy_bytes, &dummy_sse, bwid, bhgt );
                    }
                }
                int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                {
                    int bwid = ( cx + 8 <= cw ) ? 8 : ( cw - cx ), bhgt = ( cy + 8 <= ch ) ? 8 : ( ch - cy );
                    internal_videocodec_trial_I_block( U + (size_t) cy * cw + cx, cw, QC, W8, qp, zzUi, recUi, &dummy_bytes, &dummy_sse, bwid, bhgt );
                }
                {
                    int bwid = ( cx + 8 <= cw ) ? 8 : ( cw - cx ), bhgt = ( cy + 8 <= ch ) ? 8 : ( ch - cy );
                    internal_videocodec_trial_I_block( V + (size_t) cy * cw + cx, cw, QC, W8, qp, zzVi, recVi, &dummy_bytes, &dummy_sse, bwid, bhgt );
                }
                internal_videocodec_buffer_put_u8( buffer, 2 );
                uint8_t cbp = internal_videocodec_make_cbp6( zzYi, zzUi, zzVi );
                internal_videocodec_buffer_put_u8( buffer, cbp );
                for( int i = 0; i < 4; i++ )
                    if( cbp & ( 1u << i ) ) internal_videocodec_rle_write( buffer, zzYi[ i ] );
                if( cbp & ( 1u << 4 ) ) internal_videocodec_rle_write( buffer, zzUi );
                if( cbp & ( 1u << 5 ) ) internal_videocodec_rle_write( buffer, zzVi );
                for( int by = 0; by < 2; ++by )
                    for( int bx = 0; bx < 2; ++bx ) {
                        int x = xb + bx * 8, y = yb + by * 8;
                        internal_videocodec_store_block( rY, w, h, x, y, recYi[ by * 2 + bx ] );
                    }
                int cw2 = w >> 1, ch2 = h >> 1, cx2 = xb >> 1, cy2 = yb >> 1;
                internal_videocodec_store_block( rU, cw2, ch2, cx2, cy2, recUi );
                internal_videocodec_store_block( rV, cw2, ch2, cx2, cy2, recVi );
                continue;
            }
            int dxh = 0, dyh = 0;
            internal_videocodec_search_best_mv16x16( Y, w, h, xb, yb, refY, SEARCH_RAD, Y2, w2, h2, R2, Y4, w4, h4, R4, &dxh, &dyh );
            int8_t dx8 = (int8_t) internal_videocodec_clampii( dxh, -127, 127 );
            int8_t dy8 = (int8_t) internal_videocodec_clampii( dyh, -127, 127 );
            int64_t D_inter = 0, D_intra = 0;
            const int is_interz = ( dx8 == 0 && dy8 == 0 );
            int R_inter = 1 + ( is_interz ? 0 : 2 );
            int R_intra = 1;
            int16_t zzY[ 4 ][ 64 ], addY[ 4 ][ 64 ], zzU[ 64 ], addU[ 64 ], zzV[ 64 ], addV[ 64 ];
            for( int by = 0; by < 2; ++by ) {
                for( int bx = 0; bx < 2; ++bx ) {
                    int x = xb + bx * 8, y = yb + by * 8;
                    int bwid = ( x + 8 <= w ) ? 8 : ( w - x ), bhgt = ( y + 8 <= h ) ? 8 : ( h - y );
                    uint8_t cur[ 64 ], pred[ 64 ];
                    internal_videocodec_copy_block_from( Y, w, h, x, y, cur );
                    internal_videocodec_copy_block_from_frac_luma( refY, w, h, x, y, dx8, dy8, pred );
                    int idx = by * 2 + bx;
                    int bytes = 0;
                    int64_t sse = 0;
                    internal_videocodec_trial_P_block( cur, 8, pred, 8, QY, W8, qp, 1, zzY[ idx ], addY[ idx ], &bytes, &sse, bwid, bhgt );
                    D_inter += sse;
                    R_inter += bytes;
                }
            }
            {
                int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                int bwid = ( cx + 8 <= cw ) ? 8 : ( cw - cx ), bhgt = ( cy + 8 <= ch ) ? 8 : ( ch - cy );
                uint8_t cur[ 64 ], pred[ 64 ];
                int bytes = 0;
                int64_t sse = 0;
                internal_videocodec_copy_block_from( U, cw, ch, cx, cy, cur );
                internal_videocodec_copy_block_from_frac_chroma( refU, cw, ch, cx, cy, dx8, dy8, pred );
                internal_videocodec_trial_P_block( cur, 8, pred, 8, QC, W8, qp, 0, zzU, addU, &bytes, &sse, bwid, bhgt );
                D_inter += sse;
                R_inter += bytes;
            }
            {
                int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                int bwid = ( cx + 8 <= cw ) ? 8 : ( cw - cx ), bhgt = ( cy + 8 <= ch ) ? 8 : ( ch - cy );
                uint8_t cur[ 64 ], pred[ 64 ];
                int bytes = 0;
                int64_t sse = 0;
                internal_videocodec_copy_block_from( V, cw, ch, cx, cy, cur );
                internal_videocodec_copy_block_from_frac_chroma( refV, cw, ch, cx, cy, dx8, dy8, pred );
                internal_videocodec_trial_P_block( cur, 8, pred, 8, QC, W8, qp, 0, zzV, addV, &bytes, &sse, bwid, bhgt );
                D_inter += sse;
                R_inter += bytes;
            }
            int16_t zzYi[ 4 ][ 64 ], zzUi[ 64 ], zzVi[ 64 ];
            uint8_t recYi[ 4 ][ 64 ], recUi[ 64 ], recVi[ 64 ];
            for( int by = 0; by < 2; ++by ) {
                for( int bx = 0; bx < 2; ++bx ) {
                    int x = xb + bx * 8, y = yb + by * 8;
                    int bwid = ( x + 8 <= w ) ? 8 : ( w - x ), bhgt = ( y + 8 <= h ) ? 8 : ( h - y );
                    int bytes = 0;
                    int64_t sse = 0;
                    internal_videocodec_trial_I_block( Y + (size_t) y * w + x, w, QY, W8, qp, zzYi[ by * 2 + bx ], recYi[ by * 2 + bx ], &bytes, &sse, bwid, bhgt );
                    D_intra += sse;
                    R_intra += bytes;
                }
            }
            {
                int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                int bytes = 0;
                int64_t sse = 0;
                int bwid = ( cx + 8 <= cw ) ? 8 : ( cw - cx ), bhgt = ( cy + 8 <= ch ) ? 8 : ( ch - cy );
                internal_videocodec_trial_I_block( U + (size_t) cy * cw + cx, cw, QC, W8, qp, zzUi, recUi, &bytes, &sse, bwid, bhgt );
                D_intra += sse;
                R_intra += bytes;
            }
            {
                int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                int bytes = 0;
                int64_t sse = 0;
                int bwid = ( cx + 8 <= cw ) ? 8 : ( cw - cx ), bhgt = ( cy + 8 <= ch ) ? 8 : ( ch - cy );
                internal_videocodec_trial_I_block( V + (size_t) cy * cw + cx, cw, QC, W8, qp, zzVi, recVi, &bytes, &sse, bwid, bhgt );
                D_intra += sse;
                R_intra += bytes;
            }
            int inter_all_zero = ( dx8 == 0 && dy8 == 0 ) && internal_videocodec_all_zero_64( zzU ) && internal_videocodec_all_zero_64( zzV ) && internal_videocodec_all_zero_64( zzY[ 0 ] ) && 
				internal_videocodec_all_zero_64( zzY[ 1 ] ) && internal_videocodec_all_zero_64( zzY[ 2 ] ) && internal_videocodec_all_zero_64( zzY[ 3 ] );
            if( inter_all_zero ) {
                internal_videocodec_buffer_put_u8( buffer, 0 );
                for( int by = 0; by < 2; ++by )
                    for( int bx = 0; bx < 2; ++bx ) {
                        int x = xb + bx * 8, y = yb + by * 8;
                        uint8_t blk[ 64 ];
                        internal_videocodec_copy_block_from( refY, w, h, x, y, blk );
                        internal_videocodec_store_block( rY, w, h, x, y, blk );
                    }
                int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                {
                    uint8_t blk[ 64 ];
                    internal_videocodec_copy_block_from( refU, cw, ch, cx, cy, blk );
                    internal_videocodec_store_block( rU, cw, ch, cx, cy, blk );
                }
                {
                    uint8_t blk[ 64 ];
                    internal_videocodec_copy_block_from( refV, cw, ch, cx, cy, blk );
                    internal_videocodec_store_block( rV, cw, ch, cx, cy, blk );
                }
                continue;
            }
            int zeros_inter = 0, zeros_intra = 0;
            for( int i = 0; i < 4; i++ ) {
                if( internal_videocodec_all_zero_64( zzY[ i ] ) ) zeros_inter++;
            }
            if( internal_videocodec_all_zero_64( zzU ) ) zeros_inter++;
            if( internal_videocodec_all_zero_64( zzV ) ) zeros_inter++;
            for( int i = 0; i < 4; i++ ) {
                if( internal_videocodec_all_zero_64( zzYi[ i ] ) ) zeros_intra++;
            }
            if( internal_videocodec_all_zero_64( zzUi ) ) zeros_intra++;
            if( internal_videocodec_all_zero_64( zzVi ) ) zeros_intra++;
            int R_inter_cbp = R_inter - 5 * zeros_inter + 1;
            int R_intra_cbp = R_intra - 5 * zeros_intra + 1;
            int64_t C_inter = D_inter + (int64_t) RD_LAMBDA_BUMP * (int64_t) R_inter_cbp;
            int64_t C_intra = D_intra + (int64_t) RD_LAMBDA_BUMP * (int64_t) R_intra_cbp;
            if( C_intra < C_inter ) {
                internal_videocodec_buffer_put_u8( buffer, 2 );
                uint8_t cbp = internal_videocodec_make_cbp6( zzYi, zzUi, zzVi );
                internal_videocodec_buffer_put_u8( buffer, cbp );
                for( int i = 0; i < 4; i++ )
                    if( cbp & ( 1u << i ) ) internal_videocodec_rle_write( buffer, zzYi[ i ] );
                if( cbp & ( 1u << 4 ) ) internal_videocodec_rle_write( buffer, zzUi );
                if( cbp & ( 1u << 5 ) ) internal_videocodec_rle_write( buffer, zzVi );
                for( int by = 0; by < 2; ++by )
                    for( int bx = 0; bx < 2; ++bx ) {
                        int x = xb + bx * 8, y = yb + by * 8;
                        internal_videocodec_store_block( rY, w, h, x, y, recYi[ by * 2 + bx ] );
                    }
                int cw2 = w >> 1, ch2 = h >> 1, cx2 = xb >> 1, cy2 = yb >> 1;
                internal_videocodec_store_block( rU, cw2, ch2, cx2, cy2, recUi );
                internal_videocodec_store_block( rV, cw2, ch2, cx2, cy2, recVi );
            } else {
                const int is_interz2 = ( dx8 == 0 && dy8 == 0 );
                internal_videocodec_buffer_put_u8( buffer, (uint8_t) ( is_interz2 ? 3 : 1 ) );
                if( !is_interz2 ) {
                    internal_videocodec_buffer_put_u8( buffer, (uint8_t) dx8 );
                    internal_videocodec_buffer_put_u8( buffer, (uint8_t) dy8 );
                }
                uint8_t cbp = internal_videocodec_make_cbp6( zzY, zzU, zzV );
                internal_videocodec_buffer_put_u8( buffer, cbp );
                for( int i = 0; i < 4; i++ )
                    if( cbp & ( 1u << i ) ) internal_videocodec_rle_write( buffer, zzY[ i ] );
                if( cbp & ( 1u << 4 ) ) internal_videocodec_rle_write( buffer, zzU );
                if( cbp & ( 1u << 5 ) ) internal_videocodec_rle_write( buffer, zzV );
                for( int by = 0; by < 2; ++by ) {
                    for( int bx = 0; bx < 2; ++bx ) {
                        int x = xb + bx * 8, y = yb + by * 8;
                        uint8_t pred[ 64 ], out8[ 64 ];
                        internal_videocodec_copy_block_from_frac_luma( refY, w, h, x, y, dx8, dy8, pred );
                        int idx = by * 2 + bx;
                        for( int i = 0; i < 64; i++ ) {
                            int v = (int) pred[ i ] + (int) addY[ idx ][ i ];
                            out8[ i ] = (uint8_t) internal_videocodec_clampi( v );
                        }
                        internal_videocodec_store_block( rY, w, h, x, y, out8 );
                    }
                }
                int cw = w >> 1, ch = h >> 1, cx = xb >> 1, cy = yb >> 1;
                {
                    uint8_t pred[ 64 ], out8[ 64 ];
                    internal_videocodec_copy_block_from_frac_chroma( refU, cw, ch, cx, cy, dx8, dy8, pred );
                    for( int i = 0; i < 64; i++ ) {
                        int v = (int) pred[ i ] + (int) addU[ i ];
                        out8[ i ] = (uint8_t) internal_videocodec_clampi( v );
                    }
                    internal_videocodec_store_block( rU, cw, ch, cx, cy, out8 );
                }
                {
                    uint8_t pred[ 64 ], out8[ 64 ];
                    internal_videocodec_copy_block_from_frac_chroma( refV, cw, ch, cx, cy, dx8, dy8, pred );
                    for( int i = 0; i < 64; i++ ) {
                        int v = (int) pred[ i ] + (int) addV[ i ];
                        out8[ i ] = (uint8_t) internal_videocodec_clampi( v );
                    }
                    internal_videocodec_store_block( rV, cw, ch, cx, cy, out8 );
                }
            }
        }
    }
    internal_videocodec_deblock_plane( rY, w, h, 0 );
    internal_videocodec_deblock_plane( rU, w / 2, h / 2, 1 );
    internal_videocodec_deblock_plane( rV, w / 2, h / 2, 1 );
    internal_videocodec_dering_luma( rY, w, h );
}


static int64_t internal_videocodec_sad_plane_shifted( const uint8_t* A, const uint8_t* B, int w, int h, int dx, int dy, int64_t cutoff ) {
    int64_t s = 0;
    for( int y = 0; y < h; ++y ) {
        int yb = y + dy;
        if( yb < 0 ) yb = 0;
        else if( yb >= h ) yb = h - 1;
        const uint8_t* ra = A + (size_t) y * w;
        const uint8_t* rb = B + (size_t) yb * w;
        for( int x = 0; x < w; ++x ) {
            int xb = x + dx;
            if( xb < 0 ) xb = 0;
            else if( xb >= w ) xb = w - 1;
            int d = (int) ra[ x ] - (int) rb[ xb ];
            s += ( d < 0 ? -d : d );
        }
        if( s >= cutoff ) return s;
    }
    return s;
}


static void internal_videocodec_hist32_luma( const uint8_t* img, int w, int h, uint32_t H[ 32 ] ) {
    for( int i = 0; i < 32; i++ ) H[ i ] = 0;
    const int N = w * h;
    int i = 0;
    for( ; i + 4 <= N; i += 4 ) {
        uint8_t a = img[ i + 0 ], b = img[ i + 1 ], c = img[ i + 2 ], d = img[ i + 3 ];
        H[ a >> 3 ]++;
        H[ b >> 3 ]++;
        H[ c >> 3 ]++;
        H[ d >> 3 ]++;
    }
    for( ; i < N; ++i ) H[ img[ i ] >> 3 ]++;
}


static int internal_videocodec_should_emit_iframe( videocodec_enc_t* e, const uint8_t* Y ) {
    if( e->fidx == 0 ) return 1;
    const int MIN_GAP = 10;
    if( e->frames_since_last_i < MIN_GAP ) {
        e->last_cut_sad_perpx = 0;
        e->last_cut_hist_l1_mmp = 0;
        return 0;
    }
    const int w = e->w, h = e->h;
    const int w2 = w >> 1, h2 = h >> 1, w4 = w >> 2, h4 = h >> 2;
    if( w4 < 4 || h4 < 4 ) return 0;
    internal_videocodec_down2_box( Y, w, h, e->Y2 );
    internal_videocodec_down2_box( e->Y2, w2, h2, e->Y4 );
    internal_videocodec_down2_box( e->rY, w, h, e->R2 );
    internal_videocodec_down2_box( e->R2, w2, h2, e->R4 );
    const int RAD4 = 2;
    int64_t best = INT64_MAX;
    for( int dy = -RAD4; dy <= RAD4; ++dy ) {
        for( int dx = -RAD4; dx <= RAD4; ++dx ) {
            int64_t s = internal_videocodec_sad_plane_shifted( e->Y4, e->R4, w4, h4, dx, dy, best );
            if( s < best ) best = s;
        }
    }
    const int64_t N = (int64_t) w4 * (int64_t) h4;
    int sad_perpx = (int) ( ( best + ( N >> 1 ) ) / N );
    uint32_t Hc[ 32 ], Hr[ 32 ];
    internal_videocodec_hist32_luma( e->Y4, w4, h4, Hc );
    internal_videocodec_hist32_luma( e->R4, w4, h4, Hr );
    int64_t diff = 0;
    for( int i = 0; i < 32; i++ ) {
        int64_t d = (int64_t) Hc[ i ] - (int64_t) Hr[ i ];
        diff += ( d < 0 ? -d : d );
    }
    int hist_l1_mmp = (int) ( ( diff * 10000 ) / N );
    e->last_cut_sad_perpx = sad_perpx;
    e->last_cut_hist_l1_mmp = hist_l1_mmp;
    const int CUT_HIST_ONLY = 3500;
    const int CUT_SAD_HI = 26;
    const int CUT_SAD_MID = 18;
    const int CUT_HIST_MID = 1500;
    if( hist_l1_mmp >= CUT_HIST_ONLY ) return 1;
    if( sad_perpx >= CUT_SAD_HI ) return 1;
    if( sad_perpx >= CUT_SAD_MID && hist_l1_mmp >= CUT_HIST_MID ) return 1;
    return 0;
}


static void internal_videocodec_append( videocodec_enc_t* e, const void* src, size_t n ) {
    memcpy( e->out + e->out_len, src, n );
    e->out_len += n;
}


static void internal_videocodec_write_header( videocodec_enc_t* e ) {
    if( e->wrote_header ) return;
    uint8_t signature[ ] = { 'F', 'M', 'V', INTERNAL_VIDEOCODEC_VERSION };
    internal_videocodec_append( e, signature, 4 );
    int32_t hw[ 22 ];
    hw[ 0 ] = e->w;
    hw[ 1 ] = e->h;
    hw[ 2 ] = e->fps_n;
    hw[ 3 ] = e->fps_d;
    hw[ 4 ] = ( e->sar_n > 0 ? e->sar_n : 1 );
    hw[ 5 ] = ( e->sar_d > 0 ? e->sar_d : 1 );
    hw[ 6 ] = e->q.q_ac_y_num;
    hw[ 7 ] = e->q.q_ac_y_den;
    hw[ 8 ] = e->q.q_ac_c_num;
    hw[ 9 ] = e->q.q_ac_c_den;
    hw[ 10 ] = e->q.q_dc_y_num;
    hw[ 11 ] = e->q.q_dc_y_den;
    hw[ 12 ] = e->q.q_dc_c_num;
    hw[ 13 ] = e->q.q_dc_c_den;
    hw[ 14 ] = e->q.q_edge_num;
    hw[ 15 ] = e->q.q_edge_den;
    hw[ 16 ] = e->q.q_hf_y_num;
    hw[ 17 ] = e->q.q_hf_y_den;
    hw[ 18 ] = e->q.q_hf_c_num;
    hw[ 19 ] = e->q.q_hf_c_den;
    hw[ 20 ] = e->q.q_dz_num;
    hw[ 21 ] = e->q.q_dz_den;
    internal_videocodec_append( e, hw, sizeof( hw ) );
    e->wrote_header = 1;
}


static inline void internal_videocodec_compress_and_append_frame( videocodec_enc_t* e ) {
    int raw = (int) e->buffer.len;
    size_t size_pos = e->out_len;
    uint32_t placeholder = 0;
    internal_videocodec_append( e, &placeholder, 4 );
    internal_videocodec_append( e, &raw, 4 );
    int clen = VIDEOCODEC_PACK( e->out + e->out_len, e->buffer.buf, raw, e->pack_arena );
    e->out_len += clen;
    uint32_t actual_size = (uint32_t) ( 4 + clen );
    memcpy( e->out + size_pos, &actual_size, 4 );
    e->stats.frames_total++;
    e->stats.bytes_raw_total += (uint64_t) raw;
    e->stats.bytes_compressed_total += (uint64_t) clen;
    if( e->user_stats ) *e->user_stats = e->stats;
}


static videocodec_enc_frame_t internal_videocodec_encode_from_planes( videocodec_enc_t* e, uint8_t const* Y, uint8_t const* U, uint8_t const* V ) {
    videocodec_enc_frame_t ret;
    ret.size = 0;
    ret.data = NULL;
    e->out_len = 0;
    internal_videocodec_write_header( e );
    if( e->fidx == 0 ) {
        internal_videocodec_buffer_reset( &e->buffer );
        internal_videocodec_encode_iframe( Y, U, V, e->w, e->h, &e->buffer, e->rY, e->rU, e->rV, e->QYx, e->QCx, e->W8, &e->q );
        e->fidx++;
        e->stats.frames_i++;
        internal_videocodec_compress_and_append_frame( e );
        ret.data = e->out;
        ret.size = e->out_len;
        return ret;
    }
    int choose_I = internal_videocodec_should_emit_iframe( e, Y );
    if( choose_I ) {
        internal_videocodec_buffer_reset( &e->buffer );
        internal_videocodec_encode_iframe( Y, U, V, e->w, e->h, &e->buffer, e->rY, e->rU, e->rV, e->QYx, e->QCx, e->W8, &e->q );
        e->stats.frames_i++;
        e->frames_since_last_i = 0;
    } else {
        int group = ( e->cir_K > 0 ) ? ( e->cir_frame % e->cir_K ) : 0;
        internal_videocodec_encode_pframe( Y, U, V, e->w, e->h, &e->buffer, e->rY, e->rU, e->rV, e->refY, e->refU, e->refV, e->Y2, e->R2, e->Y4, e->R4, e->cir_gid, group, e->mb_w, e->QYx, e->QCx, e->W8, &e->q );
        e->stats.frames_p++;
        e->frames_since_last_i++;
        if( e->cir_K > 0 ) e->cir_frame = ( e->cir_frame + 1 ) % e->cir_K;
    }
    e->fidx++;
    internal_videocodec_compress_and_append_frame( e );
    ret.data = e->out;
    ret.size = e->out_len;
    return ret;
}


static inline uint8_t internal_videocodec_rgb_to_y_601( int r, int g, int b ) {
    int y = ( ( 66 * r + 129 * g + 25 * b + 128 ) >> 8 ) + 16;
    return (uint8_t) internal_videocodec_clampi( y );
}


static inline uint8_t internal_videocodec_rgb_to_u_601( int r, int g, int b ) {
    int u = ( ( -38 * r - 74 * g + 112 * b + 128 ) >> 8 ) + 128;
    return (uint8_t) internal_videocodec_clampi( u );
}


static inline uint8_t internal_videocodec_rgb_to_v_601( int r, int g, int b ) {
    int v = ( ( 112 * r - 94 * g - 18 * b + 128 ) >> 8 ) + 128;
    return (uint8_t) internal_videocodec_clampi( v );
}


videocodec_enc_t* videocodec_enc_create( int width, int height, int fps_n, int fps_d, int sar_n, int sar_d, enum videocodec_quality_t quality, videocodec_enc_stats_t* out_stats ) {
    if( ( width & 7 ) || ( height & 7 ) || width <= 0 || height <= 0 || fps_d == 0 ) {
        return NULL;
    }
    
    size_t ysz = (size_t) width * height, csz = (size_t) ( width / 2 ) * ( height / 2 );
    size_t mb_w = ( width + 15 ) >> 4, mb_h = ( height + 15 ) >> 4;
    size_t bytes_per_block = 194;  // DC + 63 AC * 3 + terminator
    size_t blocks_per_mb = 6;      // 4Y + 1U + 1V
    size_t max_encoded_size = mb_w * mb_h * blocks_per_mb * bytes_per_block;
    int max_compressed_bound = VIDEOCODEC_PACK( NULL, NULL, (int) max_encoded_size, NULL );
    size_t max_output_size = 92 + ( 8 + max_compressed_bound );
    
    size_t total = sizeof( videocodec_enc_t ) + max_encoded_size + ysz + csz + csz + ysz + csz + csz + 
                   ( width / 2 ) * ( height / 2 ) + ( width / 2 ) * ( height / 2 ) + 
                   ( width / 4 ) * ( height / 4 ) + ( width / 4 ) * ( height / 4 ) + 
                   ysz + csz + csz + ( ( width + 15 ) >> 4 ) * ( ( height + 15 ) >> 4 ) * sizeof( uint16_t ) + max_output_size;
    
    uint8_t* arena = (uint8_t*) malloc( total );
    if( !arena ) return NULL;
    
    videocodec_enc_t* e = (videocodec_enc_t*) arena;
    memset( e, 0, sizeof( *e ) );
    e->w = width;
    e->h = height;
    e->fps_n = fps_n;
    e->fps_d = fps_d;
    e->sar_n = sar_n > 0 ? sar_n : 1;
    e->sar_d = sar_d > 0 ? sar_d : 1;
    internal_videocodec_quality_from_enum( quality, &e->q );
    internal_videocodec_build_quants( e->QYx, e->QCx, &e->q );
    internal_videocodec_build_window( e->W8 );
    e->mb_w = ( e->w + 15 ) >> 4;
    e->mb_h = ( e->h + 15 ) >> 4;
    e->mb_n = e->mb_w * e->mb_h;
    
    arena += sizeof( videocodec_enc_t );
    e->buffer.buf = arena; arena += max_encoded_size;
    e->rY = arena; arena += ysz;
    e->rU = arena; arena += csz;
    e->rV = arena; arena += csz;
    e->refY = arena; arena += ysz;
    e->refU = arena; arena += csz;
    e->refV = arena; arena += csz;
    e->Y2 = arena; arena += ( e->w / 2 ) * ( e->h / 2 );
    e->R2 = arena; arena += ( e->w / 2 ) * ( e->h / 2 );
    e->Y4 = arena; arena += ( e->w / 4 ) * ( e->h / 4 );
    e->R4 = arena; arena += ( e->w / 4 ) * ( e->h / 4 );
    e->tY = arena; arena += ysz;
    e->tU = arena; arena += csz;
    e->tV = arena; arena += csz;
    e->cir_gid = (uint16_t*) arena; arena += e->mb_n * sizeof( uint16_t );
    e->out = arena;
    
    e->buffer.cap = max_encoded_size;
    e->buffer.len = 0;
    memset( e->rY, 0, ysz );
    memset( e->rU, 128, csz );
    memset( e->rV, 128, csz );
    e->out_len = 0;
    e->out_cap = max_output_size;
    e->fidx = 0;
    e->wrote_header = 0;
    internal_videocodec_cir_build_map( e );
    memset( &e->stats, 0, sizeof( e->stats ) );
    e->user_stats = out_stats;
    if( e->user_stats ) *e->user_stats = e->stats;
    e->frames_since_last_i = 0;
    e->last_cut_sad_perpx = 0;
    e->last_cut_hist_l1_mmp = 0;
    return e;
}


void videocodec_enc_destroy( videocodec_enc_t* e ) {
    free( e );
}


videocodec_enc_frame_t videocodec_enc_encode_yuv420( videocodec_enc_t* e, void const* yuv420 ) {
    const uint8_t* base = (const uint8_t*) yuv420;
    size_t ysz = (size_t) e->w * e->h, csz = (size_t) ( e->w / 2 ) * ( e->h / 2 );
    const uint8_t* Y = base;
    const uint8_t* U = base + ysz;
    const uint8_t* V = base + ysz + csz;
    return internal_videocodec_encode_from_planes( e, Y, U, V );
}


videocodec_enc_frame_t videocodec_enc_encode_xbgr( videocodec_enc_t* e, uint32_t const* xbgr ) {
    const int W = e->w, H = e->h;
    uint8_t const* px = (uint8_t const*) xbgr;
    uint8_t *Y = e->tY, *U = e->tU, *V = e->tV;
    for( int y = 0; y < H; ++y ) {
        const uint8_t* row = px + (size_t) y * W * 4;
        uint8_t* yrow = Y + (size_t) y * W;
        for( int x = 0; x < W; ++x ) {
            int r = row[ x * 4 + 0 ], g = row[ x * 4 + 1 ], b = row[ x * 4 + 2 ];
            yrow[ x ] = internal_videocodec_rgb_to_y_601( r, g, b );
        }
    }
    for( int y = 0; y < H; y += 2 ) {
        const uint8_t* r0 = px + (size_t) y * W * 4;
        const uint8_t* r1 = px + (size_t) internal_videocodec_imin( y + 1, H - 1 ) * W * 4;
        uint8_t* urow = U + (size_t) ( y >> 1 ) * ( W / 2 );
        uint8_t* vrow = V + (size_t) ( y >> 1 ) * ( W / 2 );
        for( int x = 0; x < W; x += 2 ) {
            int x1 = internal_videocodec_imin( x + 1, W - 1 );
            int r00 = r0[ x * 4 + 0 ], g00 = r0[ x * 4 + 1 ], b00 = r0[ x * 4 + 2 ];
            int r01 = r0[ x1 * 4 + 0 ], g01 = r0[ x1 * 4 + 1 ], b01 = r0[ x1 * 4 + 2 ];
            int r10 = r1[ x * 4 + 0 ], g10 = r1[ x * 4 + 1 ], b10 = r1[ x * 4 + 2 ];
            int r11 = r1[ x1 * 4 + 0 ], g11 = r1[ x1 * 4 + 1 ], b11 = r1[ x1 * 4 + 2 ];
            int u = internal_videocodec_rgb_to_u_601( r00, g00, b00 ) + internal_videocodec_rgb_to_u_601( r01, g01, b01 ) + 
				internal_videocodec_rgb_to_u_601( r10, g10, b10 ) + internal_videocodec_rgb_to_u_601( r11, g11, b11 );
            int v = internal_videocodec_rgb_to_v_601( r00, g00, b00 ) + internal_videocodec_rgb_to_v_601( r01, g01, b01 ) + 
				internal_videocodec_rgb_to_v_601( r10, g10, b10 ) + internal_videocodec_rgb_to_v_601( r11, g11, b11 );
            urow[ x >> 1 ] = (uint8_t) internal_videocodec_clampi( ( u + 2 ) >> 2 );
            vrow[ x >> 1 ] = (uint8_t) internal_videocodec_clampi( ( v + 2 ) >> 2 );
        }
    }
    return internal_videocodec_encode_from_planes( e, Y, U, V );
}


videocodec_enc_frame_t videocodec_enc_finalize( videocodec_enc_t* e ) {
    videocodec_enc_frame_t f = { 0, NULL };
    e->out_len = 0;
    if( !e->wrote_header ) {
        internal_videocodec_write_header( e );
    }
    memset( e->out + e->out_len, 0, 4 );
    e->out_len += 4;
    f.data = e->out;
    f.size = e->out_len;
    return f;
}


struct videocodec_dec_t {
    int w, h;
    uint32_t fps_n, fps_d;
    int sar_n, sar_d;
    internal_videocodec_quality_params_t q;
    uint8_t QYx[ 64 ], QCx[ 64 ];
    uint16_t W8[ 64 ];
    uint8_t *Y, *U, *V;
    uint8_t *refY, *refU, *refV;
    uint8_t* zbuf;
    size_t zcap;
    int bytes_needed;
};


static inline const uint8_t* internal_videocodec_dec_plane_I( const uint8_t* p, int w, int h, uint8_t* out, const uint8_t* Q, const uint16_t* W8 ) {
    int16_t zzq[ 64 ], rq[ 64 ];
    uint8_t blk[ 64 ];
    for( int y = 0; y < h; y += 8 ) {
        for( int x = 0; x < w; x += 8 ) {
            int bwid = ( x + 8 <= w ) ? 8 : ( w - x );
            int bhgt = ( y + 8 <= h ) ? 8 : ( h - y );
            p = internal_videocodec_rle_read_block( p, zzq );
            for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
            if( bwid == 8 && bhgt == 8 ) {
                internal_videocodec_idct8x8_dequant_to_u8( rq, Q, W8, out + (size_t) y * w + x, w );
            } else {
                internal_videocodec_idct8x8_dequant_to_u8( rq, Q, W8, blk, 8 );
                for( int by = 0; by < bhgt; ++by ) {
                    memcpy( out + (size_t) ( y + by ) * w + x, blk + (size_t) by * 8, (size_t) bwid );
                }
            }
        }
    }
    return p;
}


videocodec_dec_t* videocodec_dec_create( uint8_t const data[ VIDEOCODEC_DEC_HEADER_SIZE ] ) {
    if( !data ) return NULL;

    uint8_t signature[ ] = { 'F', 'M', 'V', INTERNAL_VIDEOCODEC_VERSION };
    int32_t const* header = (int32_t const*) data;
    if( *header++ != *(int32_t*)signature ) return NULL;

    int w = *header++;
    int h = *header++;
    int fn = *header++;
    int fd = *header++;
    int sn = *header++;
    int sd = *header++;
    if( w == 0 || h == 0 || fd == 0 || sd == 0 ) return NULL;

    size_t ysz = (size_t)w * (size_t)h;
    size_t csz = ((size_t)w >> 1) * ((size_t)h >> 1);
    size_t total = sizeof(videocodec_dec_t) + ysz + csz + csz + ysz + csz + csz;

    videocodec_dec_t* d = (videocodec_dec_t*) malloc( total );
    if( !d ) return NULL;
    memset( d, 0, sizeof(*d) );

    d->w = w; d->h = h;
    d->fps_n = fn; d->fps_d = fd;
    d->sar_n = sn; d->sar_d = sd;

    d->q.q_ac_y_num = *header++; d->q.q_ac_y_den = *header++;
    d->q.q_ac_c_num = *header++; d->q.q_ac_c_den = *header++;
    d->q.q_dc_y_num = *header++; d->q.q_dc_y_den = *header++;
    d->q.q_dc_c_num = *header++; d->q.q_dc_c_den = *header++;
    d->q.q_edge_num = *header++; d->q.q_edge_den = *header++;
    d->q.q_hf_y_num = *header++; d->q.q_hf_y_den = *header++;
    d->q.q_hf_c_num = *header++; d->q.q_hf_c_den = *header++;
    d->q.q_dz_num = *header++; d->q.q_dz_den = *header++;

    d->bytes_needed = (*header++);
    d->bytes_needed += d->bytes_needed ? 4 : 0;

    uint8_t* arena = (uint8_t*)( d + 1 );
    d->Y = arena; arena += ysz;
    d->U = arena; arena += csz;
    d->V = arena; arena += csz;
    d->refY = arena; arena += ysz;
    d->refU = arena; arena += csz;
    d->refV = arena; arena += csz;

    size_t initial_zbuf_size = ysz + csz + csz;  // YUV420 raw frame size
    d->zbuf = (uint8_t*) malloc( initial_zbuf_size );
    if( !d->zbuf ) {
        free( d );
        return NULL;
    }
    d->zcap = initial_zbuf_size;

    internal_videocodec_build_quants( d->QYx, d->QCx, &d->q );
    internal_videocodec_build_window( d->W8 );

    memset( d->Y, 0, ysz );
    memset( d->U, 128, csz );
    memset( d->V, 128, csz );

    return d;
}


void videocodec_dec_destroy( videocodec_dec_t* d ) {
    free( d->zbuf );
    free( d );
}


int videocodec_dec_width( videocodec_dec_t* d ) { 
    return d->w; 
}


int videocodec_dec_height( videocodec_dec_t* d ) { 
    return d->h; 
}


void videocodec_dec_fps( videocodec_dec_t* d, int* fps_n, int* fps_d ) {
    if( fps_n ) *fps_n = d->fps_n;
    if( fps_d ) *fps_d = d->fps_d;
}


void videocodec_dec_ar( videocodec_dec_t* d, int* ar_n, int* ar_d ) {
    if( ar_n ) *ar_n = d->sar_n;
    if( ar_d ) *ar_d = d->sar_d;
}


size_t videocodec_dec_next_frame( videocodec_dec_t* d, void const* data, size_t size, uint32_t* out_xbgr ) {
    if( data == NULL && size == 0 && out_xbgr == NULL ) return (size_t) d->bytes_needed;
    if( !d || !data || !out_xbgr ) return 0;
    if( size < 8 ) return 0;

    uint8_t const* p = (uint8_t const*) data;
    uint32_t raw = *(uint32_t const*) p;
    if( raw == 0 ) return 0;
    uint8_t const* comp = p + 4;
    unsigned clen = (unsigned)( size - 8 );
    if( (size_t) raw > d->zcap ) {
        free( d->zbuf );
        d->zbuf = (uint8_t*) malloc( (size_t) raw );
        if( !d->zbuf ) return 0;
        d->zcap = (size_t) raw;
    }
    int n = VIDEOCODEC_UNPACK( d->zbuf, (int) raw, comp, (int) clen );
    if( n != (int) raw ) return 0;

    uint8_t const* z = d->zbuf;
    uint8_t ftype = *z++;
    if( ftype == INTERNAL_VIDEOCODEC_FT_I ) {
        z = internal_videocodec_dec_plane_I( z, d->w, d->h, d->Y, d->QYx, d->W8 );
        z = internal_videocodec_dec_plane_I( z, d->w / 2, d->h / 2, d->U, d->QCx, d->W8 );
        z = internal_videocodec_dec_plane_I( z, d->w / 2, d->h / 2, d->V, d->QCx, d->W8 );
        internal_videocodec_deblock_plane( d->Y, d->w, d->h, 0 );
        internal_videocodec_deblock_plane( d->U, d->w / 2, d->h / 2, 1 );
        internal_videocodec_deblock_plane( d->V, d->w / 2, d->h / 2, 1 );
        internal_videocodec_dering_luma( d->Y, d->w, d->h );
    } else if( ftype == INTERNAL_VIDEOCODEC_FT_P ) {
        size_t ysz = (size_t) d->w * d->h, csz = (size_t) ( d->w / 2 ) * ( d->h / 2 );
        memcpy( d->refY, d->Y, ysz );
        memcpy( d->refU, d->U, csz );
        memcpy( d->refV, d->V, csz );
        for( int yb = 0; yb < d->h; yb += 16 ) {
            for( int xb = 0; xb < d->w; xb += 16 ) {
                uint8_t mode = *z++;
                if( mode == 0 ) {
                    for( int by = 0; by < 2; ++by )
                        for( int bx = 0; bx < 2; ++bx ) {
                            int x = xb + bx * 8, y = yb + by * 8;
                            uint8_t blk[ 64 ];
                            internal_videocodec_copy_block_from( d->refY, d->w, d->h, x, y, blk );
                            internal_videocodec_store_block( d->Y, d->w, d->h, x, y, blk );
                        }
                    int cw = d->w >> 1, ch = d->h >> 1, cx = xb >> 1, cy = yb >> 1;
                    {
                        uint8_t blk[ 64 ];
                        internal_videocodec_copy_block_from( d->refU, cw, ch, cx, cy, blk );
                        internal_videocodec_store_block( d->U, cw, ch, cx, cy, blk );
                    }
                    {
                        uint8_t blk[ 64 ];
                        internal_videocodec_copy_block_from( d->refV, cw, ch, cx, cy, blk );
                        internal_videocodec_store_block( d->V, cw, ch, cx, cy, blk );
                    }
                } else if( mode == 1 || mode == 3 ) {
                    int8_t dx8 = 0, dy8 = 0;
                    if( mode == 1 ) {
                        dx8 = (int8_t) ( *z++ );
                        dy8 = (int8_t) ( *z++ );
                    }
                    uint8_t cbp = *z++;
                    int16_t zzq[ 64 ], rq[ 64 ], add16[ 64 ];
                    uint8_t pred[ 64 ], out8[ 64 ];
                    for( int by = 0; by < 2; ++by ) {
                        for( int bx = 0; bx < 2; ++bx ) {
                            int x = xb + bx * 8, y = yb + by * 8;
                            int idx = by * 2 + bx;
                            if( cbp & ( 1u << idx ) ) {
                                z = internal_videocodec_rle_read_block( z, zzq );
                                for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
                                internal_videocodec_idct8x8_dequant_to_s16( rq, d->QYx, d->W8, add16, 8 );
                            } else {
                                for( int i = 0; i < 64; i++ ) add16[ i ] = 0;
                            }
                            internal_videocodec_copy_block_from_frac_luma( d->refY, d->w, d->h, x, y, dx8, dy8, pred );
                            for( int i = 0; i < 64; i++ ) {
                                int v = (int) pred[ i ] + (int) add16[ i ];
                                out8[ i ] = (uint8_t) internal_videocodec_clampi( v );
                            }
                            internal_videocodec_store_block( d->Y, d->w, d->h, x, y, out8 );
                        }
                    }
                    int cw = d->w >> 1, ch = d->h >> 1, cx = xb >> 1, cy = yb >> 1;
                    if( cbp & ( 1u << 4 ) ) {
                        z = internal_videocodec_rle_read_block( z, zzq );
                        for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
                        internal_videocodec_idct8x8_dequant_to_s16( rq, d->QCx, d->W8, add16, 8 );
                    } else {
                        for( int i = 0; i < 64; i++ ) add16[ i ] = 0;
                    }
                    internal_videocodec_copy_block_from_frac_chroma( d->refU, cw, ch, cx, cy, dx8, dy8, pred );
                    for( int i = 0; i < 64; i++ ) {
                        int v = (int) pred[ i ] + (int) add16[ i ];
                        out8[ i ] = (uint8_t) internal_videocodec_clampi( v );
                    }
                    internal_videocodec_store_block( d->U, cw, ch, cx, cy, out8 );
                    if( cbp & ( 1u << 5 ) ) {
                        z = internal_videocodec_rle_read_block( z, zzq );
                        for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
                        internal_videocodec_idct8x8_dequant_to_s16( rq, d->QCx, d->W8, add16, 8 );
                    } else {
                        for( int i = 0; i < 64; i++ ) add16[ i ] = 0;
                    }
                    internal_videocodec_copy_block_from_frac_chroma( d->refV, cw, ch, cx, cy, dx8, dy8, pred );
                    for( int i = 0; i < 64; i++ ) {
                        int v = (int) pred[ i ] + (int) add16[ i ];
                        out8[ i ] = (uint8_t) internal_videocodec_clampi( v );
                    }
                    internal_videocodec_store_block( d->V, cw, ch, cx, cy, out8 );
                } else if( mode == 2 ) {
                    uint8_t cbp = *z++;
                    int16_t zzq[ 64 ], rq[ 64 ];
                    uint8_t blk[ 64 ];
                    for( int by = 0; by < 2; ++by ) {
                        for( int bx = 0; bx < 2; ++bx ) {
                            int x = xb + bx * 8, y = yb + by * 8;
                            int idx = by * 2 + bx;
                            if( cbp & ( 1u << idx ) ) {
                                z = internal_videocodec_rle_read_block( z, zzq );
                                for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
                                internal_videocodec_idct8x8_dequant_to_u8( rq, d->QYx, d->W8, blk, 8 );
                            } else {
                                for( int i = 0; i < 64; i++ ) blk[ i ] = 128;
                            }
                            internal_videocodec_store_block( d->Y, d->w, d->h, x, y, blk );
                        }
                    }
                    int cw = d->w >> 1, ch = d->h >> 1, cx = xb >> 1, cy = yb >> 1;
                    if( cbp & ( 1u << 4 ) ) {
                        z = internal_videocodec_rle_read_block( z, zzq );
                        for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
                        internal_videocodec_idct8x8_dequant_to_u8( rq, d->QCx, d->W8, blk, 8 );
                    } else {
                        for( int i = 0; i < 64; i++ ) blk[ i ] = 128;
                    }
                    internal_videocodec_store_block( d->U, cw, ch, cx, cy, blk );
                    if( cbp & ( 1u << 5 ) ) {
                        z = internal_videocodec_rle_read_block( z, zzq );
                        for( int i = 0; i < 64; i++ ) rq[ internal_videocodec_zz[ i ] ] = zzq[ i ];
                        internal_videocodec_idct8x8_dequant_to_u8( rq, d->QCx, d->W8, blk, 8 );
                    } else {
                        for( int i = 0; i < 64; i++ ) blk[ i ] = 128;
                    }
                    internal_videocodec_store_block( d->V, cw, ch, cx, cy, blk );
                } else {
                    return 0;
                }
            }
        }
        internal_videocodec_deblock_plane( d->Y, d->w, d->h, 0 );
        internal_videocodec_deblock_plane( d->U, d->w / 2, d->h / 2, 1 );
        internal_videocodec_deblock_plane( d->V, d->w / 2, d->h / 2, 1 );
        internal_videocodec_dering_luma( d->Y, d->w, d->h );
    } else {
        return 0;
    }

    const int W = d->w, H = d->h, CW = W >> 1;
    for( int y = 0; y < H; ++y ) {
        uint8_t* out = (uint8_t*) out_xbgr + (size_t) y * (size_t) W * 4u;
        const uint8_t* yrow = d->Y + (size_t) y * (size_t) W;
        const uint8_t* urow = d->U + (size_t)( y >> 1 ) * (size_t) CW;
        const uint8_t* vrow = d->V + (size_t)( y >> 1 ) * (size_t) CW;
        for( int x = 0; x < W; ++x ) {
            int Yv = (int) yrow[ x ] - 16; if( Yv < 0 ) Yv = 0;
            int Uv = (int) urow[ x >> 1 ] - 128;
            int Vv = (int) vrow[ x >> 1 ] - 128;
            int C = 298 * Yv;
            int R = ( C + 409 * Vv + 128 ) >> 8; if( R < 0 ) R = 0; else if( R > 255 ) R = 255;
            int G = ( C - 100 * Uv - 208 * Vv + 128 ) >> 8; if( G < 0 ) G = 0; else if( G > 255 ) G = 255;
            int B = ( C + 516 * Uv + 128 ) >> 8; if( B < 0 ) B = 0; else if( B > 255 ) B = 255;
            out[ x * 4 + 0 ] = (uint8_t) R;
            out[ x * 4 + 1 ] = (uint8_t) G;
            out[ x * 4 + 2 ] = (uint8_t) B;
            out[ x * 4 + 3 ] = 255;
        }
    }

    uint32_t next_zsz = *(const uint32_t*)( p + size - 4 );
    d->bytes_needed = (int)( next_zsz + ( next_zsz > 0 ? 4 : 0 ) );
    return (size_t) d->bytes_needed;
}

#endif // VIDEOCODEC_IMPLEMENTATION



#ifdef VIDEOCODEC_BUILD_ENCODER

#define _CRT_SECURE_NO_WARNINGS

#ifndef videocodec_h
#include "videocodec.h"
#endif

#include "dir.h"
#include "stb_image.h"

#include <ctype.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>


static char* out_path_from( const char* in ) {
    size_t n = strlen( in );
    char* o = (char*) malloc( n + 5 );
    if( !o ) {
        return NULL;
    }
    memcpy( o, in, n + 1 );
    char* dot = strrchr( o, '.' );
    if( dot ) {
        strcpy( dot, ".fmv" );
    } else {
        o[n] = '.';
        o[n + 1] = 'f';
        o[n + 2] = 'm';
        o[n + 3] = 'v';
        o[n + 4] = 0;
    }
    return o;
}


static int write_all( FILE* f, const void* data, size_t size ) {
    const uint8_t* p = (const uint8_t*) data;
    while( size ) {
        size_t n = fwrite( p, 1, size, f );
        if( n == 0 ) {
            return -1;
        }
        p += n;
        size -= n;
    }
    return 0;
}


static const char* path_basename( const char* p ) {
    const char* s1 = strrchr( p, '/' );
    const char* s2 = strrchr( p, '\\' );
    const char* s = s1 ? ( s2 ? ( s1 > s2 ? s1 : s2 ) : s1 ) : s2;
    return s ? s + 1 : p;
}


static const char* path_extname( const char* path ) {
    const char* b = path_basename( path );
    const char* d = strrchr( b, '.' );
    return d ? d : "";
}


static char* join_path( const char* a, const char* b ) {
    size_t na = strlen( a ), nb = strlen( b );
    int need_sep = 1;
    if( na > 0 ) {
        char ca = a[na - 1];
        if( ca == '/' || ca == '\\' ) {
            need_sep = 0;
        }
    }
    char* r = (char*) malloc( na + need_sep + nb + 1 );
    if( !r ) {
        return NULL;
    }
    memcpy( r, a, na );
    if( need_sep ) {
        r[na] = '/';
        na++;
    }
    memcpy( r + na, b, nb );
    r[na + nb] = 0;
    return r;
}


static int read_line( FILE* f, char* buf, size_t cap ) {
    if( !fgets( buf, (int) cap, f ) ) {
        return 0;
    }
    size_t n = strlen( buf );
    while( n && ( buf[n - 1] == '\n' || buf[n - 1] == '\r' ) ) {
        buf[--n] = 0;
    }
    return 1;
}


static int parse_y4m_header_file( FILE* f, int* w, int* h, int* fn, int* fd, int* sar_n, int* sar_d, int* is420 ) {
    char line[2048];
    if( !read_line( f, line, sizeof( line ) ) ) {
        return -1;
    }
    if( strncmp( line, "YUV4MPEG2", 9 ) != 0 ) {
        return -1;
    }
    *w = 0;
    *h = 0;
    *fn = 0;
    *fd = 0;
    *sar_n = 1;
    *sar_d = 1;
    *is420 = 0;
    const char* s = line + 9;
    while( *s ) {
        while( *s == ' ' ) {
            s++;
        }
        if( !*s ) {
            break;
        }
        char tag = *s++;
        const char* p = s;
        while( *s && *s != ' ' ) {
            s++;
        }
        if( tag == 'W' ) {
            *w = atoi( p );
        } else if( tag == 'H' ) {
            *h = atoi( p );
        } else if( tag == 'F' ) {
            int n = 0, d = 0;
            const char* q = p;
            while( q < s && *q >= '0' && *q <= '9' ) {
                n = n * 10 + ( *q++ - '0' );
            }
            if( q < s && *q == ':' ) {
                q++;
                while( q < s && *q >= '0' && *q <= '9' ) {
                    d = d * 10 + ( *q++ - '0' );
                }
            }
            *fn = n;
            *fd = d ? d : 1;
        } else if( tag == 'C' ) {
            *is420 = strncmp( p, "420", 3 ) == 0 || strncmp( p, "420jpeg", 7 ) == 0 || strncmp( p, "420mpeg2", 8 ) == 0 || strncmp( p, "420paldv", 8 ) == 0;
        } else if( tag == 'A' ) {
            int n = 0, d = 0;
            const char* q = p;
            while( q < s && *q >= '0' && *q <= '9' ) {
                n = n * 10 + ( *q++ - '0' );
            }
            if( q < s && *q == ':' ) {
                q++;
                while( q < s && *q >= '0' && *q <= '9' ) {
                    d = d * 10 + ( *q++ - '0' );
                }
            }
            if( n > 0 && d > 0 ) {
                *sar_n = n;
                *sar_d = d;
            }
        }
    }
    if( *w <= 0 || *h <= 0 ) {
        return -1;
    }
    if( *fn == 0 || *fd == 0 ) {
        *fn = 30;
        *fd = 1;
    }
    return 0;
}


static void fmt_bytes( char* out, size_t cap, uint64_t bytes ) {
    const char* u[] = { "B", "KB", "MB", "GB", "TB" };
    float v = (float) bytes;
    int i = 0;
    while( v >= 1024.0f && i < 4 ) {
        v /= 1024.0f;
        i++;
    }
    snprintf( out, (int) cap, "%.2f %s", v, u[i] );
}


static void fmt_hms( char* out, size_t cap, float s ) {
    if( s < 0 ) {
        s = 0;
    }
    int h = (int) ( s / 3600.0f );
    int m = (int) ( ( s - h * 3600 ) / 60.0f );
    float r = s - h * 3600 - m * 60;
    if( h > 0 ) {
        snprintf( out, (int) cap, "%d:%02d:%06.3f", h, m, r );
    } else {
        snprintf( out, (int) cap, "%d:%06.3f", m, r );
    }
}


static const char* quality_name( enum videocodec_quality_t q ) {
    switch( q ) {
    case VIDEOCODEC_QUALITY_MIN: return "min";
    case VIDEOCODEC_QUALITY_LOW: return "low";
    case VIDEOCODEC_QUALITY_DEFAULT: return "default";
    case VIDEOCODEC_QUALITY_HIGH: return "high";
    case VIDEOCODEC_QUALITY_MAX: return "max";
    default: return "?";
    }
}

static void videocodec_print_stats( const struct videocodec_enc_stats_t* s,
                             int fps_n,
                             int fps_d,
                             int width,
                             int height,
                             int sar_n,
                             int sar_d,
                             size_t file_bytes,
                             enum videocodec_quality_t qlevel ) {
    if( !s ) {
        printf( "No stats available.\n" );
        return;
    }

    const uint64_t frames = s->frames_total ? s->frames_total : 0;
    const float fps = ( fps_d > 0 ) ? ( (float) fps_n / (float) fps_d ) : 0.0f;
    const float dur_s = ( fps > 0.0f ) ? ( frames / fps ) : 0.0f;

    char size_hr[64];
    fmt_bytes( size_hr, sizeof( size_hr ), (uint64_t) file_bytes );

    float bitrate_mbps = 0.0f;
    if( dur_s > 0.0f ) {
        bitrate_mbps = ( file_bytes * 8.0f ) / ( dur_s * 1e6f );
    }

    const float yuv420_bytes_per_frame = (float) width * (float) height * 1.5f;
    const float raw_bytes = yuv420_bytes_per_frame * (float) frames;
    float space_saved_pct = 0.0f, ratio_vs_raw = 0.0f;
    if( file_bytes > 0 && raw_bytes > 0.0f ) {
        ratio_vs_raw = raw_bytes / (float) file_bytes;
        space_saved_pct = 100.0f * ( 1.0f - ( (float) file_bytes / raw_bytes ) );
        if( space_saved_pct < 0.0f ) {
            space_saved_pct = 0.0f;
        }
        if( space_saved_pct > 100.0f ) {
            space_saved_pct = 100.0f;
        }
    }

    const uint64_t iframes = s->frames_i;
    float avg_gop_frames = ( iframes > 0 ) ? ( (float) frames / (float) iframes ) : 0.0f;
    float avg_gop_seconds = ( fps > 0.0f ) ? ( avg_gop_frames / fps ) : 0.0f;

    const int psar_n = ( sar_n > 0 ) ? sar_n : 1, psar_d = ( sar_d > 0 ) ? sar_d : 1;
    const float dar = ( height > 0 && psar_d > 0 ) ? ( (float) width * psar_n ) / ( (float) height * psar_d ) : 0.0f;

    char dur_hr[64];
    fmt_hms( dur_hr, sizeof( dur_hr ), dur_s );

    printf( "==== Compression summary ====\n" );
    printf( "Resolution:        %dx%d  (SAR %d:%d)  DAR = %.3f:1\n", width, height, psar_n, psar_d, dar );
    if( fps > 0.0f ) {
        float fps_dec = fps;
        printf( "Frame rate:        %d/%d = %.3f fps\n", fps_n, fps_d, fps_dec );
    }
    printf( "Duration:          %s\n", dur_hr );
    printf( "Frames:            %" PRIu64 "  (keyframes: %" PRIu64 ")\n", frames, iframes );
    if( avg_gop_frames > 0.0f ) {
        printf( "Avg keyframe gap:  %.1f frames (= %.2f s)\n", avg_gop_frames, avg_gop_seconds );
    }
    printf( "File size:         %s (%" PRIu64 " bytes)\n", size_hr, (uint64_t) file_bytes );
    if( dur_s > 0.0f ) {
        printf( "Avg bitrate:       %.3f Mbit/s\n", bitrate_mbps );
    }
    if( raw_bytes > 0.0f ) {
        char raw_hr[64];
        fmt_bytes( raw_hr, sizeof( raw_hr ), (uint64_t) ( raw_bytes + 0.5f ) );
        printf( "Vs raw YUV420:     %s raw  |  %.2fx smaller  |  %.1f%% space saved\n",
                raw_hr,
                ratio_vs_raw,
                space_saved_pct );
    }
    printf( "Quality preset:    %s\n", quality_name( qlevel ) );
    printf( "=============================\n" );
}


static enum videocodec_quality_t map_quality_1to5( int q ) {
    if( q <= 1 ) {
        return VIDEOCODEC_QUALITY_MIN;
    }
    if( q == 2 ) {
        return VIDEOCODEC_QUALITY_LOW;
    }
    if( q == 3 ) {
        return VIDEOCODEC_QUALITY_DEFAULT;
    }
    if( q == 4 ) {
        return VIDEOCODEC_QUALITY_HIGH;
    }
    return VIDEOCODEC_QUALITY_MAX;
}


static int parse_quality_arg( const char* s, enum videocodec_quality_t* out ) {
    if( !s || !out ) {
        return 0;
    }
    for( const char* p = s; *p; ++p )
        if( !isdigit( (unsigned char) *p ) ) {
            return 0;
        }
    int v = atoi( s );
    if( v < 1 || v > 5 ) {
        return 0;
    }
    *out = map_quality_1to5( v );
    return 1;
}


static int process_y4m( const char* in_path, enum videocodec_quality_t qlevel ) {
    FILE* in = NULL;
    FILE* out = NULL;
    char* out_path = NULL;
    struct videocodec_enc_t* enc = NULL;
    uint8_t* framebuf = NULL;
    struct videocodec_enc_stats_t stats;

    in = fopen( in_path, "rb" );
    if( !in ) {
        fprintf( stderr, "Cannot open input file '%s'\n", in_path );
        exit( EXIT_FAILURE );
    }

    int w, h, fn, fd, sar_n, sar_d, is420;
    if( parse_y4m_header_file( in, &w, &h, &fn, &fd, &sar_n, &sar_d, &is420 ) != 0 ) {
        fprintf( stderr, "Invalid Y4M header\n" );
        exit( EXIT_FAILURE );
    }
    if( !( is420 && !( w & 7 ) && !( h & 7 ) ) ) {
        fprintf( stderr, "Y4M file must be 4:2:0 format with dimensions that are multiples of 8\n" );
        exit( EXIT_FAILURE );
    }

    out_path = out_path_from( in_path );
    if( !out_path ) {
        fprintf( stderr, "Out of memory\n" );
        exit( EXIT_FAILURE );
    }
    out = fopen( out_path, "wb" );
    if( !out ) {
        fprintf( stderr, "Cannot create output file '%s'\n", out_path );
        exit( EXIT_FAILURE );
    }

    enc = videocodec_enc_create( w, h, fn, fd, sar_n, sar_d, qlevel, &stats );
    if( !enc ) {
        fprintf( stderr, "Failed to initialize encoder\n" );
        exit( EXIT_FAILURE );
    }

    const size_t ysz = (size_t) w * h, csz = (size_t) ( w / 2 ) * ( h / 2 ), fsz = ysz + csz + csz;
    framebuf = (uint8_t*) malloc( fsz );
    if( !framebuf ) {
        fprintf( stderr, "Out of memory\n" );
        exit( EXIT_FAILURE );
    }

    size_t frames = 0;
    char line[256];
    while( read_line( in, line, sizeof( line ) ) ) {
        if( strncmp( line, "FRAME", 5 ) != 0 ) {
            continue;
        }
        size_t rd = fread( framebuf, 1, fsz, in );
        if( rd != fsz ) {
            fprintf( stderr, "Truncated frame data\n" );
            exit( EXIT_FAILURE );
        }
        struct videocodec_enc_frame_t blob = videocodec_enc_encode_yuv420( enc, framebuf );
        if( !( blob.data && blob.size ) ) {
            fprintf( stderr, "Encoding failed at frame %zu\n", frames );
            exit( EXIT_FAILURE );
        }
        if( write_all( out, blob.data, blob.size ) != 0 ) {
            fprintf( stderr, "Failed to write frame data\n" );
            exit( EXIT_FAILURE );
        }
        frames++;
        if( ( frames & 0xF ) == 0 ) {
            fprintf( stderr, "\rframes: %zu", frames );
            fflush( stderr );
        }
    }
    struct videocodec_enc_frame_t blob = videocodec_enc_finalize( enc );
    if( !( blob.data && blob.size ) ) {
        fprintf( stderr, "Encoding failed during finalization\n" );
        exit( EXIT_FAILURE );
    }
    if( write_all( out, blob.data, blob.size ) != 0 ) {
        fprintf( stderr, "Failed to write final frame data\n" );
        exit( EXIT_FAILURE );
    }
    fprintf( stderr, "\r                           \n" );
    {
        long pos = ftell( out );
        size_t bytes = pos >= 0 ? (size_t) pos : 0;
        videocodec_print_stats( &stats,
                         fn,
                         fd,
                         w,
                         h,
                         sar_n,
                         sar_d,
                         bytes,
                         qlevel );
    }
    fprintf( stderr, "wrote %s\n", out_path );
    if( framebuf ) {
        free( framebuf );
    }
    if( enc ) {
        videocodec_enc_destroy( enc );
    }
    if( out ) {
        fclose( out );
    }
    if( out_path ) {
        free( out_path );
    }
    if( in ) {
        fclose( in );
    }
    return EXIT_SUCCESS;
}


static int cmp_strptr( const void* a, const void* b ) {
    const char* const* sa = (const char* const*) a;
    const char* const* sb = (const char* const*) b;
    return strcmp( *sa, *sb );
}


static void free_name_list( char** names, size_t cnt ) {
    if( !names ) {
        return;
    }
    for( size_t i = 0; i < cnt; i++ ) {
        free( names[i] );
    }
    free( names );
}


static int process_png_dir( const char* dir_path, int fps_n, int fps_d, enum videocodec_quality_t qlevel ) {
    dir_t* d = NULL;
    char** names = NULL;
    size_t cnt = 0, cap = 0;
    FILE* out = NULL;
    char* out_path = NULL;
    struct videocodec_enc_t* enc = NULL;
    struct videocodec_enc_stats_t stats;
    int enc_w = 0, enc_h = 0;
    size_t frames = 0;

    d = dir_open( dir_path );
    if( !d ) {
        fprintf( stderr, "Cannot open directory '%s'\n", dir_path );
        exit( EXIT_FAILURE );
    }

    cap = 64;
    names = (char**) malloc( cap * sizeof( char* ) );
    if( !names ) {
        fprintf( stderr, "Out of memory\n" );
        exit( EXIT_FAILURE );
    }
    for( ;; ) {
        dir_entry_t* e = dir_read( d );
        if( !e ) {
            break;
        }
        if( !dir_is_file( e ) ) {
            continue;
        }
        const char* nm = dir_name( e );
        const char* ext = strrchr( nm, '.' );
        if( !ext ) {
            continue;
        }
        if( tolower( (unsigned char) ext[1] ) == 'p' && tolower( (unsigned char) ext[2] ) == 'n' && tolower( (unsigned char) ext[3] ) == 'g' && ext[4] == 0 ) {
            if( cnt == cap ) {
                cap *= 2;
                char** nn = (char**) realloc( names, cap * sizeof( char* ) );
                if( !nn ) {
                    fprintf( stderr, "Out of memory\n" );
                    exit( EXIT_FAILURE );
                }
                names = nn;
            }
            names[cnt] = (char*) malloc( strlen( nm ) + 1 );
            if( !names[cnt] ) {
                fprintf( stderr, "Out of memory\n" );
                exit( EXIT_FAILURE );
            }
            strcpy( names[cnt], nm );
            cnt++;
        }
    }
    if( cnt == 0 ) {
        fprintf( stderr, "No PNG files found in directory '%s'\n", dir_path );
        exit( EXIT_FAILURE );
    }
    qsort( names, cnt, sizeof( char* ), cmp_strptr );

    out_path = out_path_from( dir_path );
    if( !out_path ) {
        fprintf( stderr, "Out of memory\n" );
        exit( EXIT_FAILURE );
    }
    out = fopen( out_path, "wb" );
    if( !out ) {
        fprintf( stderr, "Cannot create output file '%s'\n", out_path );
        exit( EXIT_FAILURE );
    }

    for( size_t i = 0; i < cnt; i++ ) {
        char* full = join_path( dir_path, names[i] );
        if( !full ) {
            fprintf( stderr, "Out of memory\n" );
            exit( EXIT_FAILURE );
        }
        int w = 0, h = 0, nc = 0;
        unsigned char* img = stbi_load( full, &w, &h, &nc, 4 );
        free( full );
        if( !img ) {
            fprintf( stderr, "Cannot load PNG file '%s'\n", names[i] );
            exit( EXIT_FAILURE );
        }

        if( !enc ) {
            enc_w = w;
            enc_h = h;
            if( ( enc_w & 7 ) || ( enc_h & 7 ) ) {
                fprintf( stderr, "PNG dimensions must be multiples of 8 (got %dx%d)\n", enc_w, enc_h );
                stbi_image_free( img );
                exit( EXIT_FAILURE );
            }
            enc = videocodec_enc_create( enc_w, enc_h, fps_n, fps_d, 1, 1, qlevel, &stats );
            if( !enc ) {
                fprintf( stderr, "Failed to initialize encoder\n" );
                stbi_image_free( img );
                exit( EXIT_FAILURE );
            }
        } else {
            if( !( w == enc_w && h == enc_h ) ) {
                fprintf( stderr, "Size mismatch in '%s' (got %dx%d, expected %dx%d)\n", names[i], w, h, enc_w, enc_h );
                stbi_image_free( img );
                exit( EXIT_FAILURE );
            }
        }

        struct videocodec_enc_frame_t blob = videocodec_enc_encode_xbgr( enc, (const uint32_t*) img );
        stbi_image_free( img );
        if( !( blob.data && blob.size ) ) {
            fprintf( stderr, "Encoding failed at frame %zu (%s)\n", frames, names[i] );
            exit( EXIT_FAILURE );
        }
        if( write_all( out, blob.data, blob.size ) != 0 ) {
            fprintf( stderr, "Failed to write frame data\n" );
            exit( EXIT_FAILURE );
        }

        frames++;
        if( ( frames & 0xF ) == 0 ) {
            fprintf( stderr, "\rframes: %zu", frames );
            fflush( stderr );
        }
    }

    fprintf( stderr, "\rframes: %zu\n", frames );
    if( enc ) {
        long pos = ftell( out );
        size_t bytes = pos >= 0 ? (size_t) pos : 0;
        videocodec_print_stats( &stats,
                         fps_n,
                         fps_d,
                         enc_w,
                         enc_h,
                         1,
                         1,
                         bytes,
                         qlevel );
    }
    fprintf( stderr, "wrote %s\n", out_path );
    if( enc ) {
        videocodec_enc_destroy( enc );
    }
    if( out ) {
        fclose( out );
    }
    if( out_path ) {
        free( out_path );
    }
    free_name_list( names, cnt );
    if( d ) {
        dir_close( d );
    }
    return EXIT_SUCCESS;
}


int main( int argc, char** argv ) {
    if( argc < 2 ) {
        printf( "Usage:\n  %s input.y4m [quality:1..5]\n  %s <png_folder> <fps_n[:fps_d]> [quality:1..5]\n", argv[0], argv[0] );
        return EXIT_FAILURE;
    }
    enum videocodec_quality_t qlevel = VIDEOCODEC_QUALITY_DEFAULT;
    const char* in = argv[1];
    const char* ext = path_extname( in );
    int is_y4m = strcmp( ext, ".y4m" ) == 0;
    int has_ext = ext[0] != '\0';

    if( is_y4m ) {
        if( argc >= 3 ) {
            enum videocodec_quality_t qtmp;
            if( !parse_quality_arg( argv[2], &qtmp ) ) {
                fprintf( stderr, "Invalid quality setting '%s' (must be 1-5)\n", argv[2] );
                exit( EXIT_FAILURE );
            }
            qlevel = qtmp;
        }
        return process_y4m( in, qlevel );
    } else if( !has_ext ) {
        if( argc < 3 ) {
            fprintf( stderr, "PNG mode requires framerate\nUsage: %s <png_folder> <fps_n[:fps_d]> [quality]\n", argv[0] );
            exit( EXIT_FAILURE );
        }
        int fps_n = 0, fps_d = 1;
        if( sscanf( argv[2], "%d:%d", &fps_n, &fps_d ) < 1 || fps_n <= 0 || fps_d <= 0 ) {
            fprintf( stderr, "Invalid framerate format '%s' (expected N or N:D)\n", argv[2] );
            exit( EXIT_FAILURE );
        }
        if( argc >= 4 ) {
            enum videocodec_quality_t qtmp;
            if( !parse_quality_arg( argv[3], &qtmp ) ) {
                fprintf( stderr, "Invalid quality setting '%s' (must be 1-5)\n", argv[3] );
                exit( EXIT_FAILURE );
            }
            qlevel = qtmp;
        }
        return process_png_dir( in, fps_n, fps_d, qlevel );
    } else {
        fprintf( stderr, "Unknown input type '%s' (expected .y4m file or folder without extension)\n", in );
        exit( EXIT_FAILURE );
    }
}

#ifndef VIDEOCODEC_IMPLEMENTATION
    #define VIDEOCODEC_IMPLEMENTATION
    #include "videocodec.h"
#endif

#define DIR_IMPLEMENTATION
#define DIR_WINDOWS
#include "dir.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#endif // VIDEOCODEC_BUILD_ENCODER



#ifdef VIDEOCODEC_BUILD_PLAYER

#define _CRT_SECURE_NO_WARNINGS

#ifndef videocodec_h
#include "videocodec.h"
#endif

#include "app.h"
#include "crtemu.h"
#include "stb_image.h"
#include "sysfont.h"

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "crtframe_tv.h"


static uint32_t* build_scale_map( int sw, int dw ) {
    uint32_t* m = (uint32_t*) malloc( (size_t) dw * sizeof( uint32_t ) );
    uint32_t step = (uint32_t) ( ( (uint64_t) sw << 8 ) / (uint32_t) dw );
    int32_t start = (int32_t) ( ( (int32_t) step - 256 ) >> 1 );
    int32_t sx = start;
    for( int x = 0; x < dw; ++x, sx += (int32_t) step ) {
        int xi = sx >> 8;
        uint8_t a = 0;
        if( xi > 0 && xi < sw - 1 ) {
            a = (uint8_t) ( sx & 0xFF );
        }
        if( xi <= 0 ) {
            xi = 0;
        } else if( xi >= sw - 1 ) {
            xi = sw - 1;
        }
        m[x] = (uint32_t) xi | ( (uint32_t) a << 16 );
    }
    return m;
}


// color1 and color2 are R8G8B8 24bit RGB color values, alpha is 0-255
static uint32_t blend_24bit( uint32_t color1, uint32_t color2, uint8_t alpha ) {
	uint64_t c1 = ( (uint64_t) color1 ) & 0xffffff;
	uint64_t c2 = ( (uint64_t) color2 ) & 0xffffff;
	uint64_t a = (uint64_t) alpha;
	// bit magic to alpha blend R G B with single mul
	uint64_t cx1 = ( c1 | ( c1 << 24 ) ) & 0x00ff00ff00ffull;
	uint64_t cx2 = ( c2 | ( c2 << 24 ) ) & 0x00ff00ff00ffull;
	uint64_t o = ( ( ( ( cx2 - cx1 ) * a + 0x008000800080ull ) >> 8 ) + cx1 ) & 0x00ff00ff00ffull;
	return a == 0xff ? (uint32_t) c2 : (uint32_t) ( o | ( o >> 24 ) );
}


static void scale_horizontal( uint32_t* dst, int dw, const uint32_t* src, int sw, int h, const uint32_t* m ) {
    for( int y = 0; y < h; ++y ) {
        const uint32_t* s = src + (size_t) y * (size_t) sw;
        uint32_t* d = dst + (size_t) y * (size_t) dw;
        for( int x = 0; x < dw; ++x ) {
            uint32_t v = m[x];
            uint16_t x0 = (uint16_t) ( v & 0xFFFF );
            uint8_t a = (uint8_t) ( v >> 16 );
            uint32_t c0 = s[x0];
            if( a == 0 ) {
                d[x] = c0;
                continue;
            }
            uint16_t x1 = ( x0 < (uint16_t) ( sw - 1 ) ) ? (uint16_t) ( x0 + 1 ) : x0;
            uint32_t c1 = s[x1];
            d[x] = a == 255 ? c1 : blend_24bit( c0, c1, a );
        }
    }
}


typedef struct {
    int w, h, fps_n, fps_d;
    struct videocodec_dec_t* dec;
    FILE* dec_fp;
    uint8_t* in_buf;
    size_t in_cap;
    uint32_t* frame;
    stb_vorbis* ogg;
    int have_audio;
    volatile APP_U64 frames_played;
    int vid_idx;
} player_t;


static inline APP_U64 muldiv_u64( APP_U64 a, APP_U64 b, APP_U64 c ) {
    APP_U64 q = a / c, r = a - q * c;
    return q * b + ( r * b ) / c;
}


static void audio_cb_silence( APP_S16* dst, int frames, void* user ) {
    player_t* p = (player_t*) user;
    memset( dst, 0, (size_t) frames * 2 * sizeof( APP_S16 ) );
    p->frames_played += (APP_U64) frames;
}


static void audio_cb( APP_S16* dst, int frames, void* user ) {
    player_t* p = (player_t*) user;
    int filled = 0;
    while( filled < frames ) {
        int got = stb_vorbis_get_samples_short_interleaved( p->ogg, 2, dst + filled * 2, ( frames - filled ) * 2 );
        if( got == 0 ) {
            memset( dst + filled * 2, 0, (size_t) ( frames - filled ) * 2 * sizeof( APP_S16 ) );
            filled = frames;
            break;
        }
        filled += got;
    }
    p->frames_played += (APP_U64) frames;
}


static int audio_open( player_t* p, const char* path ) {
    int err = 0;
    p->ogg = stb_vorbis_open_filename( path, &err, NULL );
    if( !p->ogg ) {
        return 0;
    }
    stb_vorbis_info vi = stb_vorbis_get_info( p->ogg );
    if( vi.channels != 2 || vi.sample_rate != 44100 ) {
        stb_vorbis_close( p->ogg );
        p->ogg = NULL;
        return 0;
    }
    p->have_audio = 1;
    return 1;
}


static int videocodec_decode_one( player_t* p ) {
    size_t need = videocodec_dec_next_frame( p->dec, NULL, 0, NULL );
    if( need == 0 ) {
        return 0;
    }
    if( need > p->in_cap ) {
        void* nb = realloc( p->in_buf, need );
        if( !nb ) {
            return 0;
        }
        p->in_buf = (uint8_t*) nb;
        p->in_cap = need;
    }
    size_t rd = fread( p->in_buf, 1, need, p->dec_fp );
    if( rd != need ) {
        return 0;
    }
    size_t next = videocodec_dec_next_frame( p->dec, p->in_buf, need, p->frame );
    if( next == 0 ) {
        return 0;
    }
    return 1;
}


static int videocodec_restart( player_t* p ) {
    if( fseek( p->dec_fp, 0, SEEK_SET ) != 0 ) {
        return 0;
    }
    uint8_t hdr[ VIDEOCODEC_DEC_HEADER_SIZE ];
    size_t rd = fread( hdr, 1, VIDEOCODEC_DEC_HEADER_SIZE, p->dec_fp );
    if( rd != VIDEOCODEC_DEC_HEADER_SIZE ) {
        return 0;
    }
    if( p->dec ) {
        videocodec_dec_destroy( p->dec );
    }
    p->dec = videocodec_dec_create( hdr );
    if( !p->dec ) {
        return 0;
    }
    p->w = videocodec_dec_width( p->dec );
    p->h = videocodec_dec_height( p->dec );
    videocodec_dec_fps( p->dec, &p->fps_n, &p->fps_d );
    return 1;
}


static int app_main( app_t* app, void* ud ) {
    crtemu_t* cr = crtemu_create( CRTEMU_TYPE_TV, NULL );
    int fw, fh, fc;
    uint32_t* fr = (uint32_t*) stbi_load_from_memory( crtframe_tv, sizeof( crtframe_tv ), &fw, &fh, &fc, 4 );
    for( int i = 0; i < fw * fh; ++i ) {
        fr[i] &= 0xff000000;
    }
    crtemu_frame( cr, (CRTEMU_U32*) fr, fw, fh );
    stbi_image_free( fr );

    player_t* p = (player_t*) ud;
    app_screenmode( app, APP_SCREENMODE_WINDOW );
    app_interpolation( app, APP_INTERPOLATION_LINEAR );

    int ar_n, ar_d;
    videocodec_dec_ar( p->dec, &ar_n, &ar_d );
    int out_w = (int) ( ( (long long) p->w * ar_n + ar_d / 2 ) / ar_d );
    app_window_size( app, out_w, p->h );

    uint32_t* map = build_scale_map( p->w, out_w );
    uint32_t* scaled = (uint32_t*) malloc( (size_t) out_w * (size_t) p->h * sizeof( uint32_t ) );
    if( !scaled ) {
        free( map );
        return 0;
    }
    memset( scaled, 0, (size_t) out_w * (size_t) p->h * sizeof( uint32_t ) );

    #ifdef __wasm__
        int start = 0;
        sysfont_9x16_u32( scaled, out_w, p->h, out_w / 2 - ( 13 * 9 ) / 2, p->h / 2 - 16, "CLICK TO PLAY", 0xffffffff );
        sysfont_9x16_u32( scaled, out_w, p->h, out_w / 2 - ( 23 * 9 ) / 2, p->h / 2 + 8, " F9 - TOGGLE CRT MODE  ", 0xffffffff );
        sysfont_9x16_u32( scaled, out_w, p->h, out_w / 2 - ( 23 * 9 ) / 2, p->h / 2 + 24, "F11 - TOGGLE FULLSCREEN", 0xffffffff );
        while( app_yield( app ) != APP_STATE_EXIT_REQUESTED && !start ) {
            app_input_t in = app_input( app );
            for( int i = 0; i < in.count; ++i )
                if( in.events[i].type == APP_INPUT_KEY_DOWN ) {
                    start = 1;
                    break;
                }
            app_present( app, scaled, out_w, p->h, 0xffffff, 0x000000 );
        }
        app_interpolation( app, APP_INTERPOLATION_NONE );
    #endif

    p->frames_played = 0;
    if( p->have_audio ) {
        app_sound( app, 5880, audio_cb, p );
    } else {
        app_sound( app, 5880, audio_cb_silence, p );
    }

    if( videocodec_decode_one( p ) ) {
        scale_horizontal( scaled, out_w, p->frame, p->w, p->h, map );
    }
    p->vid_idx = 0;

    const APP_U64 freq_counts = app_time_freq( app );
    const int AUDIO_BLOCK = 5880;
    const APP_U64 block_counts = muldiv_u64( (APP_U64) AUDIO_BLOCK, freq_counts, (APP_U64) 44100 );
    APP_U64 t_start = app_time_count( app );
    APP_U64 t_last = t_start;
    APP_U64 video_counts = 0;
    APP_U64 last_frames_seen = p->frames_played;
    int64_t corr_num = 0;
    APP_U64 corr_win_left = 0;
    int64_t corr_accum = 0;
    APP_U64 frame_denom = freq_counts * (APP_U64) p->fps_d;

    bool fullscreen = false, crtmode = false;
    #ifdef __wasm__
        crtmode = true;
    #endif

    while( app_yield( app ) != APP_STATE_EXIT_REQUESTED ) {
        app_input_t in = app_input( app );
        for( int i = 0; i < in.count; ++i )
            if( in.events[i].type == APP_INPUT_KEY_DOWN ) {
                if( in.events[i].data.key == APP_KEY_F11 ) {
                    fullscreen = !fullscreen;
                    app_screenmode( app, fullscreen ? APP_SCREENMODE_FULLSCREEN : APP_SCREENMODE_WINDOW );
                }
                if( in.events[i].data.key == APP_KEY_F9 ) {
                    crtmode = !crtmode;
                }
            }

        APP_U64 now = app_time_count( app );
        APP_U64 dt = now - t_last;
        t_last = now;
        video_counts += dt;

        if( corr_win_left ) {
            APP_U64 step = dt;
            if( step > corr_win_left ) {
                step = corr_win_left;
            }
            int64_t add_num = (int64_t) step * corr_num;
            corr_accum += add_num;
            int64_t delta = 0;
            if( block_counts ) {
                delta = corr_accum / (int64_t) block_counts;
                corr_accum -= delta * (int64_t) block_counts;
            }
            if( delta >= 0 ) {
                video_counts += (APP_U64) delta;
            } else {
                APP_U64 sub = (APP_U64) ( -delta );
                if( sub > video_counts ) {
                    video_counts = 0;
                } else {
                    video_counts -= sub;
                }
            }
            corr_win_left -= step;
        }

        APP_U64 fnow = p->frames_played;
        if( fnow != last_frames_seen ) {
            last_frames_seen = fnow;
            APP_U64 audio_counts = muldiv_u64( fnow, freq_counts, (APP_U64) 44100 );
            int64_t drift = (int64_t) audio_counts - (int64_t) video_counts;
            corr_num = drift;
            corr_win_left = block_counts;
            corr_accum = 0;
        }

        APP_U64 q = video_counts / frame_denom;
        APP_U64 r = video_counts - q * frame_denom;
        APP_U64 target_u64 = q * (APP_U64) p->fps_n + ( ( r * (APP_U64) p->fps_n + frame_denom / 2 ) / frame_denom );
        int target = (int) target_u64;

        int advanced = 0;
        while( p->vid_idx < target ) {
            if( !videocodec_decode_one( p ) ) {
                if( !videocodec_restart( p ) ) {
                    break;
                }
                if( p->have_audio ) {
                    app_sound( app, 0, NULL, NULL );
                    stb_vorbis_seek_start( p->ogg );
                    app_sound( app, 5880, audio_cb, p );
                }
                p->frames_played = 0;
                t_start = app_time_count( app );
                t_last = t_start;
                video_counts = 0;
                last_frames_seen = p->frames_played;
                corr_num = 0;
                corr_win_left = 0;
                corr_accum = 0;
                p->vid_idx = 0;
                target = 0;
                continue;
            }
            p->vid_idx++;
            advanced = 1;
        }
        if( advanced ) {
            scale_horizontal( scaled, out_w, p->frame, p->w, p->h, map );
        }

        uint64_t time_us = ( app_time_count( app ) - t_start ) / ( freq_counts / 1000000 );
        crtemu_present( cr, time_us, scaled, out_w, p->h, 0xffffff, 0x000000 );
        if( crtmode ) {
            app_present( app, NULL, 0, 0, 0xffffff, 0x000000 );
        } else {
            app_present( app, scaled, out_w, p->h, 0xffffff, 0x000000 );
        }
    }

    free( scaled );
    free( map );
    app_sound( app, 0, NULL, NULL );
    return 0;
}


int main( int argc, char** argv ) {
    if( argc < 2 ) {
        fprintf( stderr, "usage: %s input.fmv [audio.ogg]\n", argv[0] );
        return 1;
    }

    player_t p;
    memset( &p, 0, sizeof( p ) );

    p.dec_fp = fopen( argv[1], "rb" );
    if( !p.dec_fp ) {
        fprintf( stderr, "open fmv failed\n" );
        return 1;
    }

    uint8_t hdr[ VIDEOCODEC_DEC_HEADER_SIZE ];
    size_t rd = fread( hdr, 1, VIDEOCODEC_DEC_HEADER_SIZE, p.dec_fp );
    if( rd != VIDEOCODEC_DEC_HEADER_SIZE ) {
        fprintf( stderr, "fmv header read failed\n" );
        fclose( p.dec_fp );
        return 1;
    }

    p.dec = videocodec_dec_create( hdr );
    if( !p.dec ) {
        fprintf( stderr, "fmv open failed\n" );
        fclose( p.dec_fp );
        return 1;
    }
    p.w = videocodec_dec_width( p.dec );
    p.h = videocodec_dec_height( p.dec );
    videocodec_dec_fps( p.dec, &p.fps_n, &p.fps_d );

    p.frame = (uint32_t*) malloc( (size_t) p.w * (size_t) p.h * 4u );
    if( !p.frame ) {
        fprintf( stderr, "alloc frame buffer failed\n" );
        videocodec_dec_destroy( p.dec );
        fclose( p.dec_fp );
        return 1;
    }

    if( argc >= 3 ) {
        if( !audio_open( &p, argv[2] ) ) {
            fprintf( stderr, "audio open failed or wrong format (need 44.1kHz stereo)\n" );
        }
    }

    int rc = app_run( app_main, &p, NULL, NULL );

    if( p.have_audio && p.ogg ) {
        stb_vorbis_close( p.ogg );
    }
    if( p.dec ) {
        videocodec_dec_destroy( p.dec );
    }
    if( p.dec_fp ) {
        fclose( p.dec_fp );
    }
    free( p.in_buf );
    free( p.frame );
    return rc;
}

#ifndef VIDEOCODEC_IMPLEMENTATION
    #define VIDEOCODEC_IMPLEMENTATION
    #include "videocodec.h"
#endif

#define APP_IMPLEMENTATION
#ifdef _WIN32
    #define APP_WINDOWS
#else
    #define APP_WASM
#endif
#include "app.h"

#define CRTEMU_IMPLEMENTATION
#include "crtemu.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SYSFONT_IMPLEMENTATION
#include "sysfont.h"

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#endif // VIDEOCODEC_BUILD_PLAYER


/*
revision history:
    0.1     initial release
*/

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2025 Mattias Gustavsson

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
