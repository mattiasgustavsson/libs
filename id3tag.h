/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

id3tag.h - v0.1 - Read/write ID3 tags from/to mp3 files in C/C++.

Do this:
    #define ID3TAG_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef id3tag_h
#define id3tag_h

#ifndef ID3TAG_U32
    #define ID3TAG_U32 unsigned int
#endif
   
#define ID3TAG_PIC_TYPE_COVER_FRONT 3
#define ID3TAG_PIC_TYPE_COVER_BACK 4
#define ID3TAG_PIC_TYPE_LEAFLET_PAGE 5
#define ID3TAG_PIC_TYPE_MEDIA 6
#define ID3TAG_PIC_TYPE_ARTIST 8
#define ID3TAG_PIC_TYPE_ARTIST_LOGO 13


typedef struct id3tag_pic_t
    {
    int pic_type;
    char const* description;
    char const* mime_type;
    void const* data;
    size_t size;
    } id3tag_pic_t;


typedef struct id3tag_t
    {
    char const* title;
    char const* artist;
    char const* album_artist;
    char const* album;
    char const* sort_title;
    char const* sort_artist;
    char const* sort_album_artist;
    char const* sort_album;
    char const* genre;
    char const* year;
    char const* track;
    char const* tracks;
    char const* disc;
    char const* discs;
    char const* compilation;
    int track_length;

    int play_counter;

    int pics_count;
    id3tag_pic_t const* pics;
    } id3tag_t;

#define ID3TAG_ALL_FIELDS ( 0xFFFFFFFF )
#define ID3TAG_FIELD_TITLE ( 1U )
#define ID3TAG_FIELD_ARTIST ( 1U << 1U )
#define ID3TAG_FIELD_ALBUM_ARTIST ( 1U << 2U )
#define ID3TAG_FIELD_ALBUM ( 1U << 3U )
#define ID3TAG_FIELD_SORT_TITLE ( 1U << 4U )
#define ID3TAG_FIELD_SORT_ARTIST ( 1U << 5U )
#define ID3TAG_FIELD_SORT_ALBUM_ARTIST ( 1U << 6U )
#define ID3TAG_FIELD_SORT_ALBUM ( 1U << 7U )
#define ID3TAG_FIELD_GENRE ( 1U << 8U )
#define ID3TAG_FIELD_YEAR ( 1U << 9U )
#define ID3TAG_FIELD_TRACK ( 1U << 10U )
#define ID3TAG_FIELD_TRACKS ( 1U << 11U )
#define ID3TAG_FIELD_DISC ( 1U << 12U )
#define ID3TAG_FIELD_DISCS ( 1U << 13U )
#define ID3TAG_FIELD_COMPILATION ( 1U << 14U )
#define ID3TAG_FIELD_PLAY_COUNTER ( 1U << 15U )
#define ID3TAG_FIELD_PICS ( 1U << 16U )
#define ID3TAG_FIELD_TRACK_LENGTH ( 1U << 17U )

size_t id3tag_size( void const* first_ten_bytes );

id3tag_t* id3tag_load( void const* data, size_t size, ID3TAG_U32 fields, void* memctx );
id3tag_t* id3tag_load_id3v1( void const* data, size_t size, void* memctx );
void id3tag_free( id3tag_t* id3tag );

size_t id3tag_save( id3tag_t const* id3tag, ID3TAG_U32 fields, void const* data, size_t capacity );


#endif /* id3tag_h */


/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef ID3TAG_IMPLEMENTATION
#undef ID3TAG_IMPLEMENTATION

#ifndef ID3TAG_U8
    #define ID3TAG_U8 unsigned char
#endif

#ifndef ID3TAG_U16
    #define ID3TAG_U16 unsigned short
#endif


#ifndef ID3TAG_MALLOC
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdlib.h>
    #define ID3TAG_MALLOC( ctx, size ) ( malloc( size ) )
    #define ID3TAG_FREE( ctx, ptr ) ( free( ptr ) )
#endif


typedef struct id3tag_internal_t
{
    id3tag_t tag;
    void* memctx;
    int pics_capacity;
} id3tag_internal_t;


int id3tag_internal_utf16_to_utf8( ID3TAG_U16* str, int len, char* out, int capacity, int endian_swap )
    {       
    ID3TAG_U16* in_ptr = str;
    char* out_ptr = out;
    char* out_end = out + capacity;

    for( int i = 0; i < len; ++i )
        {
        ID3TAG_U32 cp = (ID3TAG_U32) *in_ptr++;
        if( endian_swap ) cp = ( ( cp & 0xff00 ) >> 8 ) | ( ( cp & 0x00ff ) << 8 );

        if( cp == 0x0000 ) break;

        if( cp >= 0xd800u && cp <= 0xdbffu )
            cp = ( cp << 10 ) + (*in_ptr++) + ( 0x10000u - ( 0xd800u << 10 ) - 0xdc00u );

        if( cp < 0x80 )
            {
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) cp ); else ++out_ptr;
            }
        else if( cp < 0x800 ) 
            {
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( cp >> 6) | 0xc0 ) ); else ++out_ptr;
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( cp & 0x3f ) | 0x80 ) ); else ++out_ptr;
            }
        else if (cp < 0x10000) 
            {
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( cp >> 12 ) | 0xe0 ) ); else ++out_ptr;
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( ( cp >> 6 ) & 0x3f ) | 0x80 ) ); else ++out_ptr;
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( cp & 0x3f ) | 0x80 ) ); else ++out_ptr;
            }
        else 
            {
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( cp >> 18 ) | 0xf0 ) ); else ++out_ptr;
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( ( cp >> 12 ) & 0x3f ) | 0x80 ) ); else ++out_ptr;
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( ( cp >> 6 ) & 0x3f ) | 0x80 ) ); else ++out_ptr;
            if( out && out_ptr < out_end ) *out_ptr++ = (char)( (ID3TAG_U8) ( ( cp & 0x3f ) | 0x80 ) ); else ++out_ptr;
            }
        }
    if( out && out_ptr < out_end ) *out_ptr++ = '\0'; else ++out_ptr;

    int out_len = out ? (int)( out_ptr - out ) : (int)(uintptr_t)( out_ptr );
    return out_len;
    }


// TODO: Proper unicode support with a font that can handle it, instead of doing this
char* utf8_to_latin1( char* s )
    {
    for( int readIndex = 0, writeIndex = 0 ; ; writeIndex++ )
    {
        if( s[ readIndex ] == 0 ) { s[ writeIndex ] = 0; break; }

        int len; 
        if ( (ID3TAG_U8) s[ readIndex ] < 0x80 ) len = 1;
        else if ( ( (ID3TAG_U8) s[ readIndex ] & 0x20 ) == 0 ) len = 2;
        else if ( ( (ID3TAG_U8) s[ readIndex ] & 0x10 ) == 0 ) len = 3;
        else if ( ( (ID3TAG_U8) s[ readIndex ] & 0x08 ) == 0 ) len = 4;
        else if ( ( (ID3TAG_U8) s[ readIndex ] & 0x04 ) == 0 ) len = 5;
        else len = 6;
        
        char c = '\0';
        if( len == 1 ) 
            {
            s[ writeIndex ] = s[ readIndex++ ];
            }
        else
            {
            int v = ( s[ readIndex++ ] & ( 0xff >> ( len + 1 ) ) ) << ( ( len - 1 ) * 6 );
            for( len-- ; len > 0 ; len-- ) v |= ( (ID3TAG_U8)( s[ readIndex++ ] ) - 0x80 ) << ( ( len - 1 ) * 6 );
            if( v == 8216 ) v = '\'';
            if( v == 8217 ) v = '\'';
            if( v == 9733 ) v = '*';
            if( v == 8208 ) v = '-';
            if( v == 8211 ) v = '-';
            if( v == 8220 ) v = '"';
            if( v == 8221 ) v = 'r';
            if( v == 8482 ) 
                v = ' '; // TM
            if( v == 263 ) v = 'c';
            if( v == 65533 ) v = '?';
            if( v == 8230 ) { // '...'
                s[ writeIndex++ ] = '.';
                s[ writeIndex++ ] = '.';
                v = '.';
            }
            if( v == 339 ) { 
                s[ writeIndex++ ] = 'o';
                v = 'e';
            }
            if( v > 0xff ) {
                v = ' ';
            }
            c = (char) v;
            s[ writeIndex ] = c;
            }
        }

    return s;
    }


size_t id3tag_size( void const* first_ten_bytes )
    {
    ID3TAG_U8* ptr = (ID3TAG_U8*) first_ten_bytes;

    // verify the header to see if this is a valid ID3 tag
    if( *ptr++ != 0x49 || *ptr++ != 0x44 || *ptr++ != 0x33 || *ptr++ >= 0xff || *ptr++ >= 0xff ) return 0;

    ++ptr; // skip flags field

    ID3TAG_U32 size = 0;
    for( int i = 0; i < 4; ++i )
        {
        ID3TAG_U8 b = *ptr++;
        if( b >= 0x80 ) return 0; // Not a valid ID3
        size |= ( (ID3TAG_U32) b ) << ( 7 * ( 3 - i ) );    
        }

    return (size_t) size + 10;
    }


char* id3tag_internal_get_string( int encoding, ID3TAG_U8* ptr, int size, void* memctx, int* bytes_consumed )
    {
    (void) memctx;
    if( encoding == 0) // Latin1
        {
        char* str = (char*)ID3TAG_MALLOC( memctx, (size_t)( size + 1 ) );
        memcpy( str, ptr, (size_t) size );
        str[ size ] = '\0';
        if( bytes_consumed ) *bytes_consumed = (int) strlen( str ) + 1;
        return str;
        }
    else if( encoding == 1 ) // UTF16 with BOM
        {
        ID3TAG_U16 bom = *( (ID3TAG_U16*) ptr );
        if( bom == 0xfffe || bom == 0xfeff ) { ptr += 2; size -= 2; }
        int endian_swap = ( bom == 0xfffe );                

        int len = id3tag_internal_utf16_to_utf8( (ID3TAG_U16*) ptr, size / 2, 0, 0, endian_swap );
        char* str = (char*)ID3TAG_MALLOC( memctx, (size_t) len );
        id3tag_internal_utf16_to_utf8( (ID3TAG_U16*) ptr, size / 2, str, len, endian_swap );

        if( bytes_consumed ) 
            {
            *bytes_consumed = ( ( bom == 0xfffe || bom == 0xfeff ) ? 2 : 0 );
            ID3TAG_U16* s = (ID3TAG_U16*) ptr;
            while( *s++ ) *bytes_consumed = ( *bytes_consumed ) + 2;
            *bytes_consumed = ( *bytes_consumed ) + 2;
            }

        utf8_to_latin1( str );
        return str;
        }
    else if( encoding == 2 ) // UTF16BE without BOM
        {
        int endian_swap = 1;                

        int len = id3tag_internal_utf16_to_utf8( (ID3TAG_U16*) ptr, size / 2, 0, 0, endian_swap );
        char* str = (char*)ID3TAG_MALLOC( memctx, (size_t) len );
        id3tag_internal_utf16_to_utf8( (ID3TAG_U16*) ptr, size / 2, str, len, endian_swap );

        if( bytes_consumed ) 
            {
            *bytes_consumed = 0;
            ID3TAG_U16* s = (ID3TAG_U16*) ptr;
            while( *s++ ) *bytes_consumed = ( *bytes_consumed ) + 2;
            *bytes_consumed = ( *bytes_consumed ) + 2;
        }

        utf8_to_latin1( str );
        return str;
        }
    else if( encoding == 3 ) // UTF8
        {
        char* str = (char*)ID3TAG_MALLOC( memctx, (size_t) size + 1 );
        memcpy( str, ptr, (size_t) size );
        str[ size ] = '\0';
        if( bytes_consumed ) *bytes_consumed = (int) strlen( str ) + 1;

        utf8_to_latin1( str );
        return str;
        }
    else
        {
        return 0;
        }
    }


id3tag_t* id3tag_load_id3v1( void const* data, size_t size, void* memctx )
    {
    if( size != 128 ) return NULL;
    char const* ptr = (char const*) data;
    if( strncmp( ptr, "TAG", 3 ) != 0 ) return NULL;
    ptr += 3;

    id3tag_internal_t* tag = (id3tag_internal_t*) ID3TAG_MALLOC( memctx, sizeof( id3tag_internal_t ) + size );
    memset( tag, 0, sizeof( id3tag_internal_t ) );
    memcpy( tag + 1, data, size );
    tag->memctx = memctx;

    tag->tag.title = (char*) ID3TAG_MALLOC( memctx, 31 );
    strncpy( (char*) tag->tag.title, ptr, 30 );
    ( (char*) tag->tag.title )[ 30 ] = '\0';
    ptr += 30;

    tag->tag.artist = (char*) ID3TAG_MALLOC( memctx, 31 );
    strncpy( (char*) tag->tag.artist, ptr, 30 );
    ( (char*) tag->tag.artist )[ 30 ] = '\0';
    ptr += 30;

    tag->tag.album = (char*) ID3TAG_MALLOC( memctx, 31 );
    strncpy( (char*) tag->tag.album, ptr, 30 );
    ( (char*) tag->tag.album )[ 30 ] = '\0';
    ptr += 30;

    tag->tag.year = (char*) ID3TAG_MALLOC( memctx, 5 );    
    strncpy( (char*) tag->tag.year, ptr, 4 );
    ( (char*) tag->tag.year)[ 4 ] = '\0';
    ptr += 4;

    char const* comment = ptr;
    if( comment[ 28 ] == 0 && (int)(ID3TAG_U8)( ptr[ 29 ] ) > 0 )
        {
        tag->tag.track = (char*) ID3TAG_MALLOC( memctx, 30 );    
        sprintf( (char*) tag->tag.track, "%d", (int)(ID3TAG_U8)( ptr[ 29 ] ) );
        }
    ptr += 30;

    int genre = (int)(ID3TAG_U8)( *ptr );
    if( genre != 255 )
        {
        tag->tag.genre = (char*) ID3TAG_MALLOC( memctx, 30 );    
        sprintf( (char*) tag->tag.genre, "(%d)", genre );
        }

    return (id3tag_t*) tag;
    }


id3tag_t* id3tag_load( void const* data, size_t size, ID3TAG_U32 fields, void* memctx )
    {
    enum fields_t
        {
        FIELD_TITLE,
        FIELD_ARTIST,
        FIELD_ALBUM_ARTIST,
        FIELD_ALBUM,
        FIELD_SORT_TITLE,
        FIELD_SORT_ARTIST,
        FIELD_SORT_ALBUM_ARTIST,
        FIELD_SORT_ALBUM,
        FIELD_GENRE,
        FIELD_YEAR,
        FIELD_TRACK,
        FIELD_TRACKS,
        FIELD_DISC,
        FIELD_DISCS,
        FIELD_COMPILATION,
        FIELD_PLAY_COUNTER,
        FIELD_TRACK_LENGTH,

        FIELDCOUNT,
        };
    bool fields_found[ FIELDCOUNT ] = { false };

    int fields_requested = 0;
    if( fields & ID3TAG_FIELD_TITLE ) ++fields_requested;
    if( fields & ID3TAG_FIELD_ARTIST ) ++fields_requested;
    if( fields & ID3TAG_FIELD_ALBUM_ARTIST ) ++fields_requested;
    if( fields & ID3TAG_FIELD_ALBUM ) ++fields_requested;
    if( fields & ID3TAG_FIELD_SORT_TITLE ) ++fields_requested;
    if( fields & ID3TAG_FIELD_SORT_ARTIST ) ++fields_requested;
    if( fields & ID3TAG_FIELD_SORT_ALBUM_ARTIST ) ++fields_requested;
    if( fields & ID3TAG_FIELD_SORT_ALBUM ) ++fields_requested;
    if( fields & ID3TAG_FIELD_GENRE ) ++fields_requested;
    if( fields & ID3TAG_FIELD_YEAR ) ++fields_requested;
    if( fields & ID3TAG_FIELD_TRACK ) ++fields_requested;
    if( fields & ID3TAG_FIELD_TRACKS ) ++fields_requested;
    if( fields & ID3TAG_FIELD_DISC ) ++fields_requested;
    if( fields & ID3TAG_FIELD_DISCS ) ++fields_requested;
    if( fields & ID3TAG_FIELD_COMPILATION ) ++fields_requested;
    if( fields & ID3TAG_FIELD_PLAY_COUNTER ) ++fields_requested;
    if( fields & ID3TAG_FIELD_TRACK_LENGTH ) ++fields_requested;

    ID3TAG_U8* ptr = (ID3TAG_U8*) data;

    // validate data
    if( size < 10 ) return NULL; // Not a valid ID3
    size_t tag_size = id3tag_size( data );
    if( tag_size == 0 ) return NULL; // Not a valid ID3
    if( size < tag_size ) return NULL; // Not enough data

    id3tag_internal_t* tag = (id3tag_internal_t*) ID3TAG_MALLOC( memctx, sizeof( id3tag_internal_t ) + size );
    memset( tag, 0, sizeof( id3tag_internal_t ) );
    memcpy( tag + 1, data, size );
    tag->memctx = memctx;

    ptr += 3; // skip past "ID3" marker
    
    // read version and flags
    int version = *ptr++;
    int revision = *ptr++; (void) revision;
    ID3TAG_U8 header_flags = *ptr++;
    (void) header_flags; // TODO: inspect flags

    ptr += 4; // skip past size field

    ID3TAG_U8* end = ( (ID3TAG_U8*) data ) + tag_size;
    
    while( ptr <= ( end - 10 ) )
        {        
        if( ( fields & ID3TAG_FIELD_PICS ) == 0 )
            {
            int fields_found_count = 0;
            for( int i = 0; i < FIELDCOUNT; ++i ) if( fields_found[ i ] ) ++fields_found_count;
            if( fields_found_count == fields_requested ) break;
            }

        if( version >= 3 && ptr[ 0 ] == 0 && ptr[ 1 ] == 0 && ptr[ 2 ] == 0 && ptr[ 3 ] == 0 ) break; // end of frames (in padding)
        if( version < 3 && ptr[ 0 ] == 0 && ptr[ 1 ] == 0 && ptr[ 2 ] == 0 ) break; // end of frames (in padding)

        char frame_id[ 5 ]= { (char) *ptr++, (char) *ptr++, (char) *ptr++, '\0', '\0' };
        if( version >= 3 ) frame_id[ 3 ] = (char) *ptr++;
        int frame_size = 0;
        if( version >= 4 )
            for( int i = 3; i >= 0 ; --i ) frame_size |= ( (ID3TAG_U32) ( ( *ptr++ )  ) ) << ( 7 *  i );    
        else if( version >= 3 )
            for( int i = 3; i >= 0 ; --i ) frame_size |= ( (ID3TAG_U32) ( ( *ptr++ )  ) ) << ( 8 *  i );    
        else
            for( int i = 2; i >= 0 ; --i ) frame_size |= ( (ID3TAG_U32) ( ( *ptr++ )  ) ) << ( 8 *  i );    
        if( frame_size <= 0 ) break; // end of frames
        if( frame_size >= ( end - ptr )  ) break; // invalid frame
        ID3TAG_U16 flags = 0;
        if( version >= 3 ) flags = (ID3TAG_U16)( ( ( (ID3TAG_U16)( *ptr++ ) ) << 8 ) | ( *ptr++ ) );
       

        if( ( fields & ID3TAG_FIELD_TITLE ) && ( strcmp( frame_id , "TIT2" ) == 0 || strcmp( frame_id , "TT2" ) == 0 ) )
            {
            if( !tag->tag.title ) tag->tag.title = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_TITLE ] = true;
            }

        if( ( fields & ID3TAG_FIELD_ARTIST ) && ( strcmp( frame_id , "TPE1" ) == 0 || strcmp( frame_id , "TP1" ) == 0 ) )
            {
            if( !tag->tag.artist ) tag->tag.artist = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_ARTIST ] = true;
            }

        if( ( fields & ID3TAG_FIELD_ALBUM_ARTIST) && ( strcmp( frame_id , "TPE2" ) == 0 || strcmp( frame_id , "TP2" ) == 0 ) )
            {
            if( !tag->tag.album_artist ) tag->tag.album_artist = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_ALBUM_ARTIST ] = true;
            }

        if( ( fields & ID3TAG_FIELD_ALBUM ) && ( strcmp( frame_id , "TALB" ) == 0 || strcmp( frame_id , "TAL" ) == 0 ) )
            {
            if( !tag->tag.album ) tag->tag.album = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_ALBUM ] = true;
            }
       
        if( ( fields & ID3TAG_FIELD_GENRE ) && ( strcmp( frame_id , "TCON" ) == 0 || strcmp( frame_id , "TCO" ) == 0 ) )
            {
            if( !tag->tag.genre ) tag->tag.genre = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_GENRE ] = true;
            }

        if( ( fields & ID3TAG_FIELD_YEAR ) && ( strcmp( frame_id , "TYER" ) == 0 || strcmp( frame_id , "TYE" ) == 0 ) )
            {
            if( !tag->tag.year ) tag->tag.year = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_YEAR ] = true;
            }

        if( ( fields & ID3TAG_FIELD_COMPILATION ) && ( strcmp( frame_id , "TCMP" ) == 0 || strcmp( frame_id , "TCP" ) == 0 ) )
            {
            if( !tag->tag.compilation ) tag->tag.compilation = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_COMPILATION ] = true;
            }

        if( ( fields & ID3TAG_FIELD_SORT_TITLE ) && ( strcmp( frame_id , "TSOT" ) == 0 || strcmp( frame_id , "TST" ) == 0 ) )
            {
            if( !tag->tag.sort_title ) tag->tag.sort_title = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_SORT_TITLE ] = true;
            }

        if( ( fields & ID3TAG_FIELD_SORT_ARTIST ) && ( strcmp( frame_id , "TSOP" ) == 0 || strcmp( frame_id , "TSP" ) == 0 ) )
            {
            if( !tag->tag.sort_artist ) tag->tag.sort_artist = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_SORT_ARTIST ] = true;
            }

        if( ( fields & ID3TAG_FIELD_SORT_ALBUM ) && ( strcmp( frame_id , "TSOA" ) == 0 || strcmp( frame_id , "TSA" ) == 0 ) )
            {
            if( !tag->tag.sort_album ) tag->tag.sort_album = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_SORT_ALBUM ] = true;
            }

        if( ( fields & ID3TAG_FIELD_SORT_ALBUM_ARTIST ) && ( strcmp( frame_id , "TSO2" ) == 0 || strcmp( frame_id , "TS2" ) == 0 ) )
            {
            if( !tag->tag.sort_album_artist ) tag->tag.sort_album_artist = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            fields_found[ FIELD_SORT_ALBUM_ARTIST ] = true;
            }

        if( ( fields & ID3TAG_FIELD_TRACK_LENGTH ) && ( strcmp( frame_id , "TLEN" ) == 0 || strcmp( frame_id , "TLE" ) == 0 ) )
            {
            char* str = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            if( tag->tag.track_length == 0 ) tag->tag.track_length = atoi( str );
            ID3TAG_FREE( memctx, str );
            if( tag->tag.track_length > 0 ) fields_found[ FIELD_TRACK_LENGTH ] = true;
            }

        if( ( ( fields & ID3TAG_FIELD_TRACK ) || ( fields & ID3TAG_FIELD_TRACKS ) ) && 
             ( strcmp( frame_id , "TRCK" ) == 0 || strcmp( frame_id , "TRK" ) == 0 ) )
            {
            char* first_str = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            char* separator = strchr( first_str, '/' );
            if( separator ) *separator = '\0';
            char* second_str = separator ? ( separator + 1 ) : NULL;
            
            int first = atoi( first_str );
            int second = second_str ? atoi( second_str ) : 0;
            ID3TAG_FREE( memctx, first_str );

            char buf[ 64 ];

            if( first > 0 )
                {
                sprintf( buf, "%d", first );
                if( strlen( buf ) > 0 && !tag->tag.track )
                    {
                    tag->tag.track = (char*) ID3TAG_MALLOC( memctx, strlen( buf ) + 1 );
                    strcpy( (char*) tag->tag.track, buf );
                    fields_found[ FIELD_TRACK ] = true;
                    }
                }

            if( second > 0 )
                {
                sprintf( buf, "%d", second );
                if( strlen( buf ) > 0 && !tag->tag.tracks )
                    {
                    tag->tag.tracks = (char*) ID3TAG_MALLOC( memctx, strlen( buf ) + 1 );
                    strcpy( (char*) tag->tag.tracks, buf );
                    fields_found[ FIELD_TRACKS ] = true;
                    }
                }            
            }

        if( ( ( fields & ID3TAG_FIELD_DISC ) || ( fields & ID3TAG_FIELD_DISCS ) ) && 
             ( strcmp( frame_id , "TPOS" ) == 0 || strcmp( frame_id , "TPA" ) == 0 ) )
            {
            char* first_str = id3tag_internal_get_string( *ptr, ptr + 1, frame_size - 1, memctx, NULL );
            char* separator = strchr( first_str, '/' );
            if( separator ) *separator = '\0';
            char* second_str = separator ? ( separator + 1 ) : NULL;
            
            int first = atoi( first_str );
            int second = second_str ? atoi( second_str ) : 0;
            ID3TAG_FREE( memctx, first_str );

            char buf[ 64 ];

            if( first > 0 )
                {
                sprintf( buf, "%d", first );
                if( strlen( buf ) > 0 && !tag->tag.disc )
                    {
                    tag->tag.disc = (char*) ID3TAG_MALLOC( memctx, strlen( buf ) + 1 );
                    strcpy( (char*) tag->tag.disc, buf );
                    fields_found[ FIELD_DISC ] = true;
                    }
                }

            if( second > 0 )
                {
                sprintf( buf, "%d", second );
                if( strlen( buf ) > 0 && !tag->tag.discs )
                    {
                    tag->tag.discs = (char*) ID3TAG_MALLOC( memctx, strlen( buf ) + 1 );
                    strcpy( (char*) tag->tag.discs, buf );
                    fields_found[ FIELD_DISCS ] = true;
                    }
                }            
            }


        if( ( fields & ID3TAG_FIELD_PICS ) && ( strcmp( frame_id , "APIC" ) == 0 || strcmp( frame_id , "PIC" ) == 0 ) )
            {
            if( !tag->tag.pics )
                {
                tag->pics_capacity = 16;
                tag->tag.pics = (id3tag_pic_t*) ID3TAG_MALLOC( memctx, sizeof( id3tag_pic_t ) * tag->pics_capacity );
                tag->tag.pics_count = 0;
                }
            if( tag->tag.pics_count >= tag->pics_capacity )
                {
                tag->pics_capacity *= 2;
                id3tag_pic_t* new_pics = (id3tag_pic_t*) ID3TAG_MALLOC( memctx, sizeof( id3tag_pic_t ) * tag->pics_capacity );
                memcpy( new_pics, tag->tag.pics, sizeof( id3tag_pic_t ) * tag->tag.pics_count );
                ID3TAG_FREE( memctx, (id3tag_pic_t*)tag->tag.pics );
                tag->tag.pics = new_pics;
                }

            int encoding = *ptr++;
            --frame_size;

            id3tag_pic_t* pic = (id3tag_pic_t*) &tag->tag.pics[ tag->tag.pics_count++ ];
            memset( pic, 0, sizeof( *pic ) );

            if( version < 3 )
                {
                char str[ 4 ] = { (char) *ptr++, (char) *ptr++, (char) *ptr++, '\0' };
                frame_size -= 3;

                pic->mime_type = (char*) ID3TAG_MALLOC( memctx, 32 );
                if( strcmp( str, "JPG" ) == 0 ) 
                    strcpy( (char*) pic->mime_type, "image/jpeg" );
                else if( strcmp( str, "PNG" ) == 0 ) 
                    strcpy( (char*) pic->mime_type, "image/png" );
                else if( strcmp( str, "GIF" ) == 0 ) 
                    strcpy( (char*) pic->mime_type, "image/gif" );
                else if( strcmp( str, "BMP" ) == 0 ) 
                    strcpy( (char*) pic->mime_type, "image/bmp" );
                else
                    strcpy( (char*) pic->mime_type, str );                
                }
            else
                {
                int bytes = 0;
                pic->mime_type = id3tag_internal_get_string( encoding, ptr, frame_size, memctx, &bytes );
                ptr += bytes;
                frame_size -= bytes;
                }

            pic->pic_type = *ptr++;
            --frame_size;

            if( pic->pic_type != 0 )
                ptr = ptr;

            int bytes = 0;
            pic->description = id3tag_internal_get_string( encoding, ptr, frame_size, memctx, &bytes );
            ptr += bytes;
            frame_size -= bytes;

            if( frame_size )
                {
                pic->data = ID3TAG_MALLOC( memctx, (size_t) frame_size );
                memcpy( (void*) pic->data, ptr, (size_t) frame_size );
                pic->size = (size_t) frame_size;
                }
            else
                {
                if( pic->mime_type ) ID3TAG_FREE( memctx, (char*) pic->mime_type );
                if( pic->description ) ID3TAG_FREE( memctx, (char*) pic->description );
                --tag->tag.pics_count;
                }
            }

        ptr += frame_size;
        }

    return (id3tag_t*) tag;
    }


void id3tag_free( id3tag_t* id3tag )
    {
    id3tag_internal_t* internal = (id3tag_internal_t*) id3tag;

    if( internal->tag.title) ID3TAG_FREE( internal->memctx, (char*) internal->tag.title );
    if( internal->tag.artist ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.artist );
    if( internal->tag.album_artist ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.album_artist );
    if( internal->tag.album ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.album );
    if( internal->tag.sort_title) ID3TAG_FREE( internal->memctx, (char*) internal->tag.sort_title );
    if( internal->tag.sort_artist ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.sort_artist );
    if( internal->tag.sort_album_artist ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.sort_album_artist );
    if( internal->tag.sort_album ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.sort_album );
    if( internal->tag.genre ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.genre );
    if( internal->tag.year ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.year );
    if( internal->tag.track ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.track );
    if( internal->tag.tracks ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.tracks );
    if( internal->tag.disc ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.disc );
    if( internal->tag.discs ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.discs );
    if( internal->tag.compilation ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.compilation );

    if( internal->tag.pics )
        {
        for( int i = 0; i < internal->tag.pics_count; ++i )
            {
            if( internal->tag.pics[ i ].description ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.pics[ i ].description );
            if( internal->tag.pics[ i ].mime_type ) ID3TAG_FREE( internal->memctx, (char*) internal->tag.pics[ i ].mime_type );
            if( internal->tag.pics[ i ].data ) ID3TAG_FREE( internal->memctx, (void*) internal->tag.pics[ i ].data );
            }

        if( internal->tag.pics ) ID3TAG_FREE( internal->memctx, (id3tag_pic_t*) internal->tag.pics );
        }

    ID3TAG_FREE( internal->memctx, id3tag );
    }


size_t id3tag_save( id3tag_t const* id3tag, ID3TAG_U32 fields, void const* data, size_t capacity )
    {
    (void) id3tag, fields, data, capacity;
    return 0;
    }


#endif /* ID3TAG_IMPLEMENTATION */

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2018 Mattias Gustavsson

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