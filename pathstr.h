/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

pathstr.h - v0.1 - Filename/path string manipulation functions for C/C++.

Do this:
    #define PATHSTR_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/


#ifndef pathstr_h
#define pathstr_h

#include <stddef.h>

typedef enum pathstr_case_t { PATHSTR_CASE_SENSITIVE, PATHSTR_CASE_INSENSITIVE } pathstr_case_t;

size_t pathstr_basename( char const* path, char* out, size_t capacity );
size_t pathstr_dirname( char const* path, char* out, size_t capacity );
size_t pathstr_extname( char const* path, char* out, size_t capacity );
size_t pathstr_replace_extension( char const* path, char const* new_ext, char* out, size_t capacity );
size_t pathstr_join( char const* a, char const* b, char* out, size_t capacity );
size_t pathstr_normalize( char const* path, char* out, size_t capacity );
size_t pathstr_to_posix( char const* path, char* out, size_t capacity );
size_t pathstr_to_windows( char const* path, char* out, size_t capacity );
size_t pathstr_make_relative( char const* base, char const* target, pathstr_case_t case_mode, char* out, size_t capacity );
int pathstr_is_absolute( char const* path );
int pathstr_is_relative( char const* path );
int pathstr_wildcard_compare( char const* pattern, char const* string, pathstr_case_t case_mode );


// wrappers to be used with my cstr.h string interning library (optional)
#ifdef PATHSTR_CSTR

    #ifndef CSTR_NO_GLOBAL_API
        char const* cstr_path_basename( char const* path );
        char const* cstr_path_dirname( char const* path );
        char const* cstr_path_extname( char const* path );
        char const* cstr_path_replace_extension( char const* path, char const* new_ext );
        char const* cstr_path_join( char const* a, char const* b );
        char const* cstr_path_normalize( char const* path );
        char const* cstr_path_to_posix( char const* path );
        char const* cstr_path_to_windows( char const* path );
        char const* cstr_path_make_relative( char const* base, char const* target );
        char const* cstr_path_make_relative_nocase( char const* base, char const* target );
        int cstr_path_is_absolute( char const* path );
        int cstr_path_is_relative( char const* path );
        int cstr_path_wildcard_compare( char const* pattern, char const* string );
        int cstr_path_wildcard_compare_nocase( char const* pattern, char const* string );
    #endif // CSTR_NO_GLOBAL_API

    #ifdef CSTR_INSTANCE_API
        char const* cstri_path_basename( cstri_t* cstri, char const* path );
        char const* cstri_path_dirname( cstri_t* cstri, char const* path );
        char const* cstri_path_extname( cstri_t* cstri, char const* path );
        char const* cstri_path_replace_extension( cstri_t* cstri, char const* path, char const* new_ext );
        char const* cstri_path_join( cstri_t* cstri, char const* a, char const* b );
        char const* cstri_path_normalize( cstri_t* cstri, char const* path );
        char const* cstri_path_to_posix( cstri_t* cstri, char const* path );
        char const* cstri_path_to_windows( cstri_t* cstri, char const* path );
        char const* cstri_path_make_relative( cstri_t* cstri, char const* base, char const* target );
        char const* cstri_path_make_relative_nocase( cstri_t* cstri, char const* base, char const* target );
        int cstri_path_is_absolute( cstri_t* cstri, char const* path );
        int cstri_path_is_relative( cstri_t* cstri, char const* path );
        int cstri_path_wildcard_compare( cstri_t* cstri, char const* pattern, char const* string );
        int cstri_path_wildcard_compare_nocase( cstri_t* cstri, char const* pattern, char const* string );
    #endif // CSTR_INSTANCE_API
        
#endif


#endif // pathstr_h


/** 
 
pathstr.h
=========

Filename/path string manipulation functions for C/C++.


pathstr_basename
-----------------

    size_t pathstr_basename( char const* path, char* out, size_t capacity )

Extract the last path component (file name) into `out`. Returns the number of
bytes required including the zero-terminator. If `out` is NULL or `capacity` is 0,
nothing is written, but the function still returns the required size. The function
identifies the final component, skipping trailing separators. Drive roots (e.g., 
"C:/") and UNC share roots (e.g., "//server/share") yield an empty string. Special 
 names "." and ".." are preserved as-is. Hidden files (starting with '.') are 
treated as complete filenames. If the path ends with aseparator, the last 
non-empty component is extracted.


pathstr_dirname
----------------

    size_t pathstr_dirname( char const* path, char* out, size_t capacity )

Extract the parent directory into `out`. Returns required size
including zero-terminator. The function removes the final path component and any trailing
separators. Root inputs (e.g., "/", "C:/") preserve the root exactly. Drive‑relative
inputs (e.g., "C:file") return just the drive prefix "C:". UNC paths preserve the
server/share portion. Special names "." and ".." are preserved as-is. If the input
has no parent (e.g., "file.txt"), returns ".". 


pathstr_extname
----------------

    size_t pathstr_extname( char const* path, char* out, size_t capacity )

Extract the extension (including the leading '.') into `out`. Returns
required size including zero-terminator. The function scans the string to find the 
last '.' that is not followed by a separator. Files starting with '.' are treated 
as having no extension. If the path ends with '.', the extension is empty. Directory 
inputs (ending with separator) yield an empty string. If no extension is found, 
returns an empty string.


pathstr_replace_extension
-------------------------

    size_t pathstr_replace_extension( char const* path, char const* new_ext, char* out, size_t capacity )

Replace the final extension of `path` with `new_ext` (which may be empty or start with 
'.'). Writes the result to `out` and returns required size including zero-terminator.
The function first identifies the extension using the same logic as `pathstr_extname`. 
If `new_ext` is empty, the extension is completely removed. If `new_ext` doesn't start 
with '.', one is automatically added. Root paths (e.g., "/","C:\", "//server/share") 
remain unchanged regardless of `new_ext`. 


pathstr_join
------------

    size_t pathstr_join( char const* a, char const* b, char* out, size_t capacity )

Join `a` and `b`, inserting exactly one appropriate separator. If `b` is absolute/
rooted or device/UNC, it replaces `a` completely. The function determines the
appropriate separator based on the existing separators in `a`. If `a` ends with a 
separator or `b` begins with one, no additional separator is inserted. Empty paths 
are treated a s no-op (empty `a` yields `b`, empty `b` yields `a`). Returns required 
size including zero-terminator.


pathstr_normalize
-----------------

    size_t pathstr_normalize( char const* path, char* out, size_t capacity )

Normalize `path` by collapsing redundant separators, resolving '.' and '..' where 
legal, and preserving drive/UNC/device roots. Returns required size including zero-
terminator. The function collapses multiple consecutive separators into single ones.
It resolves '.' by removing it (unless it's the entire path). It resolves '..' by 
removing the preceding component, but only if that component is not a root. Drive 
roots (e.g., "C:\"), UNC roots (e.g., "\\server\share"), and device paths (e.g., 
"\\?\C:\") are preserved exactly. 


pathstr_to_posix
----------------

    size_t pathstr_to_posix( char const* path, char* out, size_t capacity )

Replaces all backslashes with forward slashes. Returns required size including 
zero-terminator. 


pathstr_to_windows
------------------

    size_t pathstr_to_windows( char const* path, char* out, size_t capacity )

Replaces all forward slashes with backslashes. Returns required size including 
zero-terminator. 


pathstr_make_relative
---------------------

    size_t pathstr_make_relative( char const* base, char const* target, pathstr_case_t case_mode, char* out, size_t capacity )

Compute a relative path from `base` to `target`, honoring `case_mode`. Returns 
required size including zero-terminator. The function finds the common prefix 
between the paths and constructs a relative path using '..' to navigate up from 
the base and then down to the target. If the paths are on different drives or 
devices, returns an empty string. Case sensitivity is determined by `case_mode`. 


pathstr_is_absolute
-------------------

    int pathstr_is_absolute( char const* path )

Return 1 if `path` is absolute (POSIX root, drive absolute, or device/UNC),
otherwise 0.


pathstr_is_relative
-------------------

    int pathstr_is_relative( char const* path )

Return 1 if `path` is relative (including drive‑relative), otherwise 0.


pathstr_wildcard_compare
------------------------

    int pathstr_wildcard_compare( char const* pattern, char const* string, pathstr_case_t case_mode )

Compare `string` to `pattern` with '*' and '?', honoring `case_mode`. Returns 1
for match, 0 otherwise. Matching is path-segment scoped:
- '*' matches zero or more characters but never crosses '/' or '\\'. Consecutive '*'
  collapse and are treated as a single star within the same segment. Backtracking
  is performed only within the current segment.
- '?' matches exactly one non-separator character and does not match a leading '.'
  at the start of a segment.
- Character classes '[...]' are supported, with optional leading '!' or '^' for
  negation, ranges 'a-z', and escape sequences using '\\' inside the class. Classes
  never match '/' or '\\'.
- '\\' escapes the next character to force a literal match (both in and outside
  character classes).
- Directory separators in the pattern must match separators in the string; a
  separator resets any prior '*' context (wildcards do not span across segments).
- Leading '.' at a segment start is special: '*' and '?' do not match it unless the
  pattern explicitly uses a literal '.'.
- Case sensitivity is controlled by `case_mode`: with PATHSTR_CASE_INSENSITIVE,
  ASCII letters are compared according to case_mode; otherwise comparisons are exact.
*/


#ifdef PATHSTR_IMPLEMENTATION
#undef PATHSTR_IMPLEMENTATION

#include <string.h>

size_t pathstr_basename( char const* path, char* out, size_t capacity ) {
    if( !path ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    if( path[0] == '.' && path[1] == '\0' ) {
        if( out && capacity > 0 ) {
            out[0] = '.';
            if( capacity > 1 ) {
                out[1] = '\0';
            }
        }
        return 2;
    }
    if( path[0] == '.' && path[1] == '.' && path[2] == '\0' ) {
        if( out && capacity > 0 ) {
            out[0] = '.';
            if( capacity > 1 ) {
                out[1] = '.';
                if( capacity > 2 ) {
                    out[2] = '\0';
                }
            }
        }
        return 3;
    }

    size_t len = strlen( path );

    // Drive root: "X:/" or "X:\" (with only trailing seps) -> return ""
    if( len >= 3 &&
        ( ((unsigned char)path[0] >= 'A' && (unsigned char)path[0] <= 'Z') ||
          ((unsigned char)path[0] >= 'a' && (unsigned char)path[0] <= 'z') ) &&
        path[1] == ':' )
    {
        size_t i = 2;
        if( path[i] == '/' || path[i] == '\\' ) {
            while( i < len && (path[i] == '/' || path[i] == '\\') ) {
                ++i;
            }
            if( i == len ) {
                if( out && capacity > 0 ) {
                    out[0] = '\0';
                }
                return 1;
            }
        }
    }

    // Bare UNC share root: //server/share or \\server\share (optionally with trailing seps) -> return ""
    if( len >= 2 && (path[0] == '/' || path[0] == '\\') && (path[1] == '/' || path[1] == '\\') ) {
        size_t i = 2;
        size_t server_start = i;
        while( i < len && !(path[i] == '/' || path[i] == '\\') ) {
            ++i;
        }
        size_t server_len = i - server_start;
        if( server_len > 0 && i < len && (path[i] == '/' || path[i] == '\\') ) {
            ++i;
            size_t share_start = i;
            while( i < len && !(path[i] == '/' || path[i] == '\\') ) {
                ++i;
            }
            size_t share_len = i - share_start;
            if( share_len > 0 ) {
                size_t k = i;
                while( k < len && (path[k] == '/' || path[k] == '\\') ) {
                    ++k;
                }
                if( k == len ) {
                    if( out && capacity > 0 ) {
                        out[0] = '\0';
                    }
                    return 1;
                }
            }
        }
    }

    // Trim trailing separators
    char const* start = path;
    char const* end = path + len;
    while( end > start && (end[-1] == '/' || end[-1] == '\\') ) {
        --end;
    }

    if( end == start ) {
        if( len == 0 ) {
            if( out && capacity > 0 ) {
                out[0] = '\0';
            }
            return 1;
        }
        int only_fwd = 1, only_back = 1;
        for( size_t i = 0; i < len; ++i ) {
            char ch = path[i];
            if( ch != '/' ) {
                only_fwd = 0;
            }
            if( ch != '\\' ) {
                only_back = 0;
            }
            if( !only_fwd && !only_back ) {
                break;
            }
        }
        if( only_fwd ) {
            if( out && capacity > 0 ) {
                out[0] = '/';
                if( capacity > 1 ) {
                    out[1] = '\0';
                }
            }
            return 2;
        }
        if( only_back ) {
            if( out && capacity > 0 ) {
                out[0] = '\\';
                if( capacity > 1 ) {
                    out[1] = '\0';
                }
            }
            return 2;
        }
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    // Find last component start
    char const* last = start;
    for( char const* p = start; p < end; ++p ) {
        if( *p == '/' || *p == '\\' ) {
            last = p + 1;
        }
    }

    // Skip "X:" prefix for drive-relative ("X:foo")
    if( last == start && len >= 2 &&
        ( ((unsigned char)path[0] >= 'A' && (unsigned char)path[0] <= 'Z') ||
          ((unsigned char)path[0] >= 'a' && (unsigned char)path[0] <= 'z') ) &&
        path[1] == ':' )
    {
        if( !(len >= 3 && (path[2] == '/' || path[2] == '\\')) ) {
            last = start + 2;
            if( last > end ) {
                last = end;
            }
        }
    }

    size_t blen = (size_t)(end - last);
    size_t needed = blen + 1;
    if( out && capacity > 0 ) {
        size_t n = (blen < capacity - 1) ? blen : (capacity - 1);
        if( n > 0 ) {
            memcpy( out, last, n );
        }
        out[n] = '\0';
    }
    return needed;
}


size_t pathstr_dirname( char const* path, char* out, size_t capacity ) {
    if( !path ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    size_t len = strlen( path );
    if( len == 0 ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    // Fix: "." and ".." -> "."
    if( path[0] == '.' && path[1] == '\0' ) {
        if( out && capacity > 0 ) {
            out[0] = '.';
            if( capacity > 1 ) {
                out[1] = '\0';
            }
        }
        return 2;
    }
    if( path[0] == '.' && path[1] == '.' && path[2] == '\0' ) {
        if( out && capacity > 0 ) {
            out[0] = '.';
            if( capacity > 1 ) {
                out[1] = '\0';
            }
        }
        return 2;
    }

    // Preserve drive root "C:\" or "C:/" exactly
    if( len >= 3 &&
        ( ((unsigned char)path[0] >= 'A' && (unsigned char)path[0] <= 'Z') ||
          ((unsigned char)path[0] >= 'a' && (unsigned char)path[0] <= 'z') ) &&
        path[1] == ':' && (path[2] == '/' || path[2] == '\\') )
    {
        char sep = path[2];
        size_t i = 3;
        while( i < len && (path[i] == '/' || path[i] == '\\') ) {
            ++i;
        }
        if( i == len ) {
            size_t needed = 4;
            if( out && capacity > 0 ) {
                if( capacity > 1 ) {
                    out[0] = path[0];
                }
                if( capacity > 2 ) {
                    out[1] = ':';
                }
                if( capacity > 3 ) {
                    out[2] = sep;
                }
                out[ (capacity > 3) ? 3 : (capacity - 1) ] = '\0';
            }
            return needed;
        }
    }

    // Trim trailing separators
    size_t end = len;
    while( end > 0 && (path[end - 1] == '/' || path[end - 1] == '\\') ) {
        --end;
    }

    // If path was only separators, keep one of that kind
    if( end == 0 ) {
        size_t needed = 2;
        if( out && capacity > 0 ) {
            out[0] = path[0];
            if( capacity > 1 ) {
                out[1] = '\0';
            }
        }
        return needed;
    }

    // Preserve bare UNC root //server/share
    if( (path[0] == '/' || path[0] == '\\') && (path[1] == '/' || path[1] == '\\') ) {
        size_t i = 2;
        size_t server_start = i;
        while( i < end && !(path[i] == '/' || path[i] == '\\') ) {
            ++i;
        }
        size_t server_len = i - server_start;
        if( server_len > 0 && i < end && (path[i] == '/' || path[i] == '\\') ) {
            ++i;
            size_t share_start = i;
            while( i < end && !(path[i] == '/' || path[i] == '\\') ) {
                ++i;
            }
            size_t share_len = i - share_start;
            if( share_len > 0 && i == end ) {
                size_t needed = end + 1;
                if( out && capacity > 0 ) {
                    size_t n = (end < capacity - 1) ? end : (capacity - 1);
                    if( n > 0 ) {
                        memcpy( out, path, n );
                    }
                    out[n] = '\0';
                }
                return needed;
            }
        }
    }

    // Find last separator before 'end'
    size_t i = end;
    while( i > 0 && !(path[i - 1] == '/' || path[i - 1] == '\\') ) {
        --i;
    }

    if( i == 0 ) {
        // No separator found: return "." (fix) or "C:" for bare drive-relative
        if( len >= 2 && ( ((unsigned char)path[0] >= 'A' && (unsigned char)path[0] <= 'Z') ||
            ((unsigned char)path[0] >= 'a' && (unsigned char)path[0] <= 'z') ) && path[1] == ':' ) {

            size_t needed = 3;
            if( out && capacity > 0 ) {
                size_t n = (2 < capacity - 1) ? 2 : (capacity - 1);
                if( n > 0 ) {
                    memcpy( out, path, n );
                }
                out[n] = '\0';
            }
            return needed;
        }
        if( out && capacity > 0 ) {
            out[0] = '.';
            if( capacity > 1 ) {
                out[1] = '\0';
            }
        }
        return 2;
    }

    size_t sep_index = i - 1;
    size_t parent_len = sep_index;

    // Keep minimal roots
    if( sep_index == 2 &&
        ( ((unsigned char)path[0] >= 'A' && (unsigned char)path[0] <= 'Z') ||
          ((unsigned char)path[0] >= 'a' && (unsigned char)path[0] <= 'z') ) &&
        path[1] == ':' && (path[2] == '/' || path[2] == '\\') ) {
        parent_len = 3; // "C:/"
    }
    if( sep_index == 0 && (path[0] == '/' || path[0] == '\\') ) {
        parent_len = 1; // "/" or "\"
    }

    size_t needed = parent_len + 1;
    if( out && capacity > 0 ) {
        size_t n = (parent_len < capacity - 1) ? parent_len : (capacity - 1);
        if( n > 0 ) {
            memcpy( out, path, n );
        }
        out[n] = '\0';
    }
    return needed;
}


size_t pathstr_extname( char const* path, char* out, size_t capacity ) {
    if( !path ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    size_t len = strlen( path );

    // Trim trailing separators
    size_t end = len;
    while( end > 0 && ( path[end - 1] == '/' || path[end - 1] == '\\' ) ) {
        --end;
    }

    // Find start of last component
    size_t start = 0;
    for( size_t i = 0; i < end; ++i ) {
        if( path[i] == '/' || path[i] == '\\' ) {
            start = i + 1;
        }
    }

    // Find last dot after start
    size_t dot = (size_t)(-1);
    for( size_t i = start; i < end; ++i ) {
        if( path[i] == '.' ) {
            dot = i;
        }
    }

    if( dot == (size_t)(-1) ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    // Leading-dot special (".hidden" has no extension unless another dot later)
    if( dot == start ) {
        int any_more_dots = 0;
        for( size_t i = dot + 1; i < end; ++i ) {
            if( path[i] == '.' ) {
                any_more_dots = 1;
                break;
            }
        }
        if( !any_more_dots ) {
            if( out && capacity > 0 ) {
                out[0] = '\0';
            }
            return 1;
        }
    }

    // Trailing dot means no extension (e.g., "file.")
    if( dot == end - 1 ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    size_t extlen = end - dot;
    size_t needed = extlen + 1;

    if( out && capacity > 0 ) {
        size_t n = ( extlen < capacity - 1 ) ? extlen : capacity - 1;
        if( n > 0 ) {
            memcpy( out, path + dot, n );
        }
        out[n] = '\0';
    }

    return needed;
}


size_t pathstr_replace_extension( char const* path, char const* new_ext, char* out, size_t capacity ) {
    if( !path ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    size_t len = strlen( path );

    // Last separator (either '/' or '\')
    char const* sep_f = strrchr( path, '/' );
    char const* sep_b = strrchr( path, '\\' );
    char const* last_sep = sep_f && sep_b ? ( sep_f > sep_b ? sep_f : sep_b ) : ( sep_f ? sep_f : sep_b );

    // Start of final component
    char const* name = last_sep ? last_sep + 1 : path;

    // If nothing after the last separator, unchanged
    if( *name == '\0' ) {
        size_t needed = len + 1;
        if( out && capacity > 0 ) {
            size_t n = ( len < capacity - 1 ) ? len : capacity - 1;
            if( n > 0 ) {
                memcpy( out, path, n );
            }
            out[n] = '\0';
        }
        return needed;
    }

    // UNC share root without trailing separator: \\server\share  -> unchanged
    // Detect leading // or \\ , then exactly two non-empty components and end-of-string
    if( len >= 2 && ( ( path[0] == '\\' || path[0] == '/' ) && ( path[1] == '\\' || path[1] == '/' ) ) ) {
        size_t i = 2;
        // server
        size_t server_start = i;
        while( path[i] && path[i] != '/' && path[i] != '\\' ) {
            ++i;
        }
        size_t server_len = i - server_start;
        if( server_len > 0 && ( path[i] == '/' || path[i] == '\\' ) ) {
            ++i;
            // share
            size_t share_start = i;
            while( path[i] && path[i] != '/' && path[i] != '\\' ) {
                ++i;
            }
            size_t share_len = i - share_start;
            if( share_len > 0 && path[i] == '\0' ) {
                size_t needed = len + 1;
                if( out && capacity > 0 ) {
                    size_t n = ( len < capacity - 1 ) ? len : capacity - 1;
                    if( n > 0 ) {
                        memcpy( out, path, n );
                    }
                    out[n] = '\0';
                }
                return needed;
            }
        }
    }

    // "." and ".." as final component: unchanged
    if( ( name[0] == '.' && name[1] == '\0' ) ||
        ( name[0] == '.' && name[1] == '.' && name[2] == '\0' ) ) {
        size_t needed = len + 1;
        if( out && capacity > 0 ) {
            size_t n = ( len < capacity - 1 ) ? len : capacity - 1;
            if( n > 0 ) {
                memcpy( out, path, n );
            }
            out[n] = '\0';
        }
        return needed;
    }

    // Last dot inside the final component (if any)
    char const* last_dot = strrchr( name, '.' );

    // New extension (ignore leading '.')
    char const* extsrc = NULL;
    size_t extlen = 0;
    if( new_ext && new_ext[0] ) {
        extsrc = ( new_ext[0] == '.' ) ? ( new_ext + 1 ) : new_ext;
        extlen = strlen( extsrc );
    }

    int leading_dot_only = 0;
    int trailing_dot = 0;

    if( last_dot ) {
        // If the final component is ".hidden", last_dot == name
        // If it is "file.", last_dot points to the trailing dot and last_dot[1] == '\0'
        leading_dot_only = ( last_dot == name );
        trailing_dot = ( last_dot[1] == '\0' );
    }

    size_t result_len = 0;
    if( !last_dot ) {
        // No dot -> append extension if provided
        result_len = extlen ? ( len + 1 + extlen ) : len;
    } else if( leading_dot_only ) {
        // Hidden file with no extension -> allow appending extension (was unchanged before)
        result_len = extlen ? ( len + 1 + extlen ) : len;
    } else if( trailing_dot ) {
        // Trailing dot means no extension; if adding, attach directly (no extra '.')
        result_len = extlen ? ( len + extlen ) : len;
    } else {
        // Has an extension -> replace from last_dot
        size_t stem_len = (size_t)( last_dot - path );
        result_len = extlen ? ( stem_len + 1 + extlen ) : stem_len;
    }

    size_t needed = result_len + 1;

    if( out && capacity > 0 ) {
        size_t n = 0;

        if( !last_dot ) {
            size_t m = ( len < capacity - 1 ) ? len : capacity - 1;
            if( m > 0 ) {
                memcpy( out, path, m );
            }
            n = m;
            if( extlen ) {
                if( n < capacity - 1 ) {
                    out[n++] = '.';
                }
                m = ( extlen < capacity - n - 1 ) ? extlen : capacity - n - 1;
                if( m > 0 ) {
                    memcpy( out + n, extsrc, m );
                    n += m;
                }
            }
        } else if( leading_dot_only ) {
            size_t m = ( len < capacity - 1 ) ? len : capacity - 1;
            if( m > 0 ) {
                memcpy( out, path, m );
            }
            n = m;
            if( extlen ) {
                if( n < capacity - 1 ) {
                    out[n++] = '.';
                }
                m = ( extlen < capacity - n - 1 ) ? extlen : capacity - n - 1;
                if( m > 0 ) {
                    memcpy( out + n, extsrc, m );
                    n += m;
                }
            }
        } else if( trailing_dot ) {
            size_t m = ( len < capacity - 1 ) ? len : capacity - 1;
            if( m > 0 ) {
                memcpy( out, path, m );
            }
            n = m;
            if( extlen ) {
                m = ( extlen < capacity - n - 1 ) ? extlen : capacity - n - 1;
                if( m > 0 ) {
                    memcpy( out + n, extsrc, m );
                    n += m;
                }
            }
        } else {
            size_t stem_len = (size_t)( last_dot - path );
            size_t m = ( stem_len < capacity - 1 ) ? stem_len : capacity - 1;
            if( m > 0 ) {
                memcpy( out, path, m );
            }
            n = m;
            if( extlen ) {
                if( n < capacity - 1 ) {
                    out[n++] = '.';
                }
                m = ( extlen < capacity - n - 1 ) ? extlen : capacity - n - 1;
                if( m > 0 ) {
                    memcpy( out + n, extsrc, m );
                    n += m;
                }
            }
        }

        out[ ( n < capacity ) ? n : capacity - 1 ] = '\0';
    }

    return needed;
}


size_t pathstr_join( char const* a, char const* b, char* out, size_t capacity ) {
    if( !a ) {
        a = "";
    }
    if( !b ) {
        b = "";
    }

    size_t alen = strlen( a );
    size_t blen = strlen( b );

    if( alen == 0 ) {
        size_t need = blen + 1;
        if( out && capacity > 0 ) {
            size_t n = ( blen < capacity - 1 ) ? blen : capacity - 1;
            if( n > 0 ) {
                memcpy( out, b, n );
            }
            out[n] = '\0';
        }
        return need;
    }

    // classify b
    unsigned char c0 = (unsigned char)( blen > 0 ? b[0] : 0 );
    unsigned char c1 = (unsigned char)( blen > 1 ? b[1] : 0 );
    unsigned char c2 = (unsigned char)( blen > 2 ? b[2] : 0 );

    int b_starts_sep = ( c0 == '/' || c0 == '\\' );
    int is_unc_back  = ( c0 == '\\' && c1 == '\\' );
    int is_unc_fwd   = ( c0 == '/'  && c1 == '/' && c2 != '/' );
    int is_device    = ( c0 == '\\' && c1 == '?' && c2 == '\\' );
    int is_driveabs  = ( ( ( c0 >= 'A' && c0 <= 'Z' ) || ( c0 >= 'a' && c0 <= 'z' ) ) && c1 == ':' && ( c2 == '/' || c2 == '\\' ) );

    // does a have a drive? ("X:" prefix)
    int a_has_drive = 0;
    if( alen >= 2 ) {
        unsigned char a0 = (unsigned char)a[0];
        if( ( ( a0 >= 'A' && a0 <= 'Z' ) || ( a0 >= 'a' && a0 <= 'z' ) ) && a[1] == ':' ) {
            a_has_drive = 1;
        }
    }

    // rooted-only RHS: starts with sep but is not device/UNC/drive-abs
    if( b_starts_sep && !is_device && !is_unc_back && !is_unc_fwd && !is_driveabs ) {
        if( a_has_drive ) {
            // join as X:/<trimmed b>
            size_t b_lead = 0;
            while( b_lead < blen && ( b[b_lead] == '/' || b[b_lead] == '\\' ) ) {
                ++b_lead;
            }
            size_t b_trim_len = blen - b_lead;

            // choose inserted sep from a (last seen), default '/'
            char sep_out = '/';
            for( size_t i = 0; i < alen; ++i ) {
                char ch = a[i];
                if( ch == '/' || ch == '\\' ) {
                    sep_out = ch;
                }
            }

            size_t need = 2 /* X: */ + 1 /* sep */ + b_trim_len + 1 /* NUL */;
            if( !out || capacity == 0 ) {
                return need;
            }

            size_t n = 0;
            if( n < capacity - 1 ) {
                out[n++] = a[0];
            }
            if( n < capacity - 1 ) {
                out[n++] = ':';
            }
            if( n < capacity - 1 ) {
                out[n++] = sep_out;
            }

            if( b_trim_len > 0 ) {
                size_t copy_b = ( b_trim_len < capacity - 1 - n ) ? b_trim_len : ( capacity - 1 - n );
                if( copy_b > 0 ) {
                    memcpy( out + n, b + b_lead, copy_b );
                    n += copy_b;
                }
            }
            out[ ( n < capacity ) ? n : ( capacity - 1 ) ] = '\0';
            return need;
        } else {
            // no drive in a: treat as absolute override
            size_t need = blen + 1;
            if( out && capacity > 0 ) {
                size_t n = ( blen < capacity - 1 ) ? blen : capacity - 1;
                if( n > 0 ) {
                    memcpy( out, b, n );
                }
                out[n] = '\0';
            }
            return need;
        }
    }

    // absolute override cases
    if( blen > 0 && ( is_unc_back || is_unc_fwd || is_device || is_driveabs ) ) {
        size_t need = blen + 1;
        if( out && capacity > 0 ) {
            size_t n = ( blen < capacity - 1 ) ? blen : capacity - 1;
            if( n > 0 ) {
                memcpy( out, b, n );
            }
            out[n] = '\0';
        }
        return need;
    }

    // Trim trailing seps from a
    size_t a_end = alen;
    while( a_end > 0 ) {
        char ch = a[a_end - 1];
        if( ch == '/' || ch == '\\' ) {
            --a_end;
        } else {
            break;
        }
    }
    int a_only_seps = ( a_end == 0 );
    int a_had_trail = ( a_end < alen );
    char a_trail_sep = a_had_trail ? a[alen - 1] : '/';
    if( a_trail_sep != '/' && a_trail_sep != '\\' ) {
        a_trail_sep = '/';
    }

    // Determine preferred inserted separator from a (if any)
    char a_any_sep = 0;
    for( size_t i = 0; i < a_end; ++i ) {
        char ch = a[i];
        if( ch == '/' || ch == '\\' ) {
            a_any_sep = ch;
        }
    }

    // Trim leading seps from b (since a is non-empty)
    size_t b_lead = 0;
    while( b_lead < blen && ( b[b_lead] == '/' || b[b_lead] == '\\' ) ) {
        ++b_lead;
    }
    size_t b_trim_len = blen - b_lead;

    // Drive-relative base "X:" => do not insert separator
    int a_is_drive_rel_only = 0;
    if( a_end == 2 ) {
        unsigned char cA0 = (unsigned char)a[0];
        unsigned char cA1 = (unsigned char)a[1];
        if( ( ( cA0 >= 'A' && cA0 <= 'Z' ) || ( cA0 >= 'a' && cA0 <= 'z' ) ) && cA1 == ':' ) {
            a_is_drive_rel_only = 1;
        }
    }

    // Compute needed size
    size_t need = 0;
    if( a_only_seps ) {
        size_t base_seps = ( alen >= 2 ) ? 2u : 1u;
        need = base_seps + b_trim_len + 1;
    } else {
        need = a_end;
        if( a_had_trail ) {
            need += 1;
        } else {
            if( b_trim_len > 0 && !a_is_drive_rel_only ) {
                need += 1;
            }
        }
        need += b_trim_len;
        need += 1;
    }

    if( !out || capacity == 0 ) {
        return need;
    }

    // Emit
    size_t n = 0;

    if( a_only_seps ) {
        size_t base_seps = ( alen >= 2 ) ? 2u : 1u;
        char sep_out = ( a_trail_sep == '\\' ) ? '\\' : '/';
        for( size_t i = 0; i < base_seps && n < capacity - 1; ++i ) {
            out[n++] = sep_out;
        }
        if( b_trim_len > 0 ) {
            size_t copy_b = ( b_trim_len < capacity - 1 - n ) ? b_trim_len : ( capacity - 1 - n );
            if( copy_b > 0 ) {
                memcpy( out + n, b + b_lead, copy_b );
                n += copy_b;
            }
        }
        out[ ( n < capacity ) ? n : ( capacity - 1 ) ] = '\0';
        return need;
    }

    // Normal: copy a (without trailing seps), then join
    {
        size_t copy_a = ( a_end < capacity - 1 - n ) ? a_end : ( capacity - 1 - n );
        if( copy_a > 0 ) {
            memcpy( out + n, a, copy_a );
            n += copy_a;
        }
        if( a_had_trail && n < capacity - 1 ) {
            out[n++] = a_trail_sep;
        }
        if( !a_had_trail && b_trim_len > 0 && !a_is_drive_rel_only && n < capacity - 1 ) {
            out[n++] = ( a_any_sep ? a_any_sep : '/' );
        }
    }

    if( b_trim_len > 0 ) {
        size_t copy_b = ( b_trim_len < capacity - 1 - n ) ? b_trim_len : ( capacity - 1 - n );
        if( copy_b > 0 ) {
            memcpy( out + n, b + b_lead, copy_b );
            n += copy_b;
        }
    }

    out[ ( n < capacity ) ? n : ( capacity - 1 ) ] = '\0';
    return need;
}


size_t pathstr_normalize( char const* path, char* out, size_t capacity ) {
    if( !path ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    size_t len = strlen( path );

    // Device path: keep verbatim
    if( len >= 4 && path[0] == '\\' && path[1] == '\\' && path[2] == '?' && (path[3] == '\\' || path[3] == '/') ) {
        size_t needed = len + 1;
        if( out && capacity > 0 ) {
            size_t n = (len < capacity - 1) ? len : capacity - 1;
            if( n > 0 ) {
                memcpy( out, path, n );
            }
            out[n] = '\0';
        }
        return needed;
    }

    #define PATHSTR_NORMALIZE_IS_SEP(c) ( (c) == '/' || (c) == '\\' )

    // Choose output separator: last seen in the input (default '/')
    char sep_out = '/';
    for( size_t i = 0; i < len; ++i ) {
        char ch = path[i];
        if( PATHSTR_NORMALIZE_IS_SEP(ch) ) {
            sep_out = ch;
        }
    }

    // Root detection
    int is_unc = 0;
    int is_posix_abs = 0;
    int is_drive_abs = 0;
    int is_drive_rel = 0;
    char drive = 0;
    size_t root_len = 0;

    // For POSIX-style roots, preserve exactly two leading seps if input started with exactly two (POSIX behavior)
    size_t lead = 0;
    while( lead < len && PATHSTR_NORMALIZE_IS_SEP(path[lead]) ) {
        ++lead;
    }

    // UNC: //server/share (or \\server\share)
    if( lead >= 2 ) {
        size_t i = 2;
        // try to parse //server/share
        size_t server_start = i;
        while( i < len && !PATHSTR_NORMALIZE_IS_SEP(path[i]) ) {
            ++i;
        }
        size_t server_len = i - server_start;
        if( server_len > 0 && i < len && PATHSTR_NORMALIZE_IS_SEP(path[i]) ) {
            ++i;
            size_t share_start = i;
            while( i < len && !PATHSTR_NORMALIZE_IS_SEP(path[i]) ) {
                ++i;
            }
            size_t share_len = i - share_start;
            if( share_len > 0 ) {
                is_unc = 1;
                root_len = i; // up to end of share
            }
        }
        if( !is_unc ) {
            // Not a valid UNC. POSIX rule: exactly two leading slashes preserved; 3+ collapse to single
            is_posix_abs = 1;
            root_len = (lead == 2) ? 2u : 1u;
        }
    }
    else if( len >= 2 &&
             ( ((unsigned char)path[0] >= 'A' && (unsigned char)path[0] <= 'Z') ||
               ((unsigned char)path[0] >= 'a' && (unsigned char)path[0] <= 'z') ) &&
             path[1] == ':' )
    {
        drive = path[0]; // preserve case
        if( len >= 3 && PATHSTR_NORMALIZE_IS_SEP(path[2]) ) {
            is_drive_abs = 1;
            root_len = 3;
        } else {
            is_drive_rel = 1;
            root_len = 2;
        }
    }
    else if( lead == 1 ) {
        is_posix_abs = 1;
        root_len = 1;
    }

    // Collect normalized segments (no malloc)
    enum { MAX_SEGS = 256 };
    char const* segs[MAX_SEGS];
    size_t lens[MAX_SEGS];
    size_t count = 0;

    size_t i = root_len;
    while( i < len ) {
        while( i < len && PATHSTR_NORMALIZE_IS_SEP(path[i]) ) {
            ++i;
        }
        if( i >= len ) {
            break;
        }
        size_t s = i;
        while( i < len && !PATHSTR_NORMALIZE_IS_SEP(path[i]) ) {
            ++i;
        }
        size_t l = i - s;

        if( l == 1 && path[s] == '.' ) {
            // skip "."
        } else if( l == 2 && path[s] == '.' && path[s+1] == '.' ) {
            if( is_unc || is_posix_abs || is_drive_abs ) {
                if( count > 0 ) {
                    --count; // don't ascend above root
                }
            } else {
                // relative: collapse against prior unless prior is '..'
                if( count > 0 && !(lens[count-1] == 2 && segs[count-1][0] == '.' && segs[count-1][1] == '.') ) {
                    --count;
                } else if( count < MAX_SEGS ) {
                    segs[count] = path + s;
                    lens[count] = l;
                    ++count;
                }
            }
        } else {
            if( count < MAX_SEGS ) {
                segs[count] = path + s;
                lens[count] = l;
                ++count;
            }
        }
    }

    // Compute needed length
    size_t needed = 0;
    if( is_unc ) {
        // two leading seps + server + sep + share
        needed += 2; // \\ or //
        // extract server/share lengths from root_len once more
        size_t j = 2;
        size_t server_start = j;
        while( j < root_len && !PATHSTR_NORMALIZE_IS_SEP(path[j]) ) {
            ++j;
        }
        size_t server_len = j - server_start;
        if( j < root_len ) {
            ++j;
        }
        size_t share_start = j;
        while( j < root_len && !PATHSTR_NORMALIZE_IS_SEP(path[j]) ) {
            ++j;
        }
        size_t share_len = j - share_start;

        needed += server_len + 1 + share_len;
        if( count > 0 ) {
            needed += 1; // sep after root
        }
    } else if( is_drive_abs ) {
        needed += 3; // "X:<sep>"
    } else if( is_drive_rel ) {
        needed += 2; // "X:"
    } else if( is_posix_abs ) {
        needed += (root_len == 2 ? 2 : 1); // preserve exactly two leading seps if we had exactly two
    }

    if( count > 0 ) {
        for( size_t k = 0; k < count; ++k ) {
            needed += lens[k];
        }
        if( count > 1 ) {
            needed += (count - 1); // separators between segments
        }
    }

    needed += 1; // NUL

    if( !out || capacity == 0 ) {
        return needed;
    }

    // Emit
    size_t n = 0;

    if( is_unc ) {
        char rsep = PATHSTR_NORMALIZE_IS_SEP(path[0]) ? path[0] : '\\'; // keep initial style for the root
        if( n < capacity - 1 ) {
            out[n++] = rsep;
        }
        if( n < capacity - 1 ) {
            out[n++] = rsep;
        }

        size_t j = 2;
        while( j < root_len && !PATHSTR_NORMALIZE_IS_SEP(path[j]) ) {
            if( n < capacity - 1 ) {
                out[n++] = path[j];
            }
            ++j;
        }
        if( n < capacity - 1 ) {
            out[n++] = rsep;
        }
        if( j < root_len ) {
            ++j;
        }
        while( j < root_len && !PATHSTR_NORMALIZE_IS_SEP(path[j]) ) {
            if( n < capacity - 1 ) {
                out[n++] = path[j];
            }
            ++j;
        }

        if( count > 0 && n < capacity - 1 ) {
            out[n++] = sep_out;
        }
    } else if( is_drive_abs ) {
        if( n < capacity - 1 ) {
            out[n++] = drive;
        }
        if( n < capacity - 1 ) {
            out[n++] = ':';
        }
        // keep the root separator as written
        char rsep = path[2];
        if( n < capacity - 1 ) {
            out[n++] = (rsep == '/' || rsep == '\\') ? rsep : sep_out;
        }
    } else if( is_drive_rel ) {
        if( n < capacity - 1 ) {
            out[n++] = drive;
        }
        if( n < capacity - 1 ) {
            out[n++] = ':';
        }
    } else if( is_posix_abs ) {
        size_t reps = (root_len == 2 ? 2u : 1u);
        char rsep = (lead > 0) ? path[0] : sep_out;
        for( size_t t = 0; t < reps; ++t ) {
            if( n < capacity - 1 ) {
                out[n++] = (rsep == '/' || rsep == '\\') ? rsep : sep_out;
            }
        }
    }

    for( size_t k = 0; k < count; ++k ) {
        size_t m = lens[k];
        size_t copy = (m < (capacity - 1 - n)) ? m : (capacity - 1 - n);
        if( copy > 0 ) {
            memcpy( out + n, segs[k], copy );
            n += copy;
        }
        if( k + 1 < count ) {
            if( n < capacity - 1 ) {
                out[n++] = sep_out;
            }
        }
    }

    out[ (n < capacity) ? n : (capacity - 1) ] = '\0';
    return needed;

    #undef PATHSTR_NORMALIZE_IS_SEP
}


size_t pathstr_to_posix( char const* path, char* out, size_t capacity ) {
    if( !path ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    size_t len = strlen( path );
    size_t needed = len + 1;

    if( out && capacity > 0 ) {
        size_t n = ( len < capacity - 1 ) ? len : capacity - 1;
        for( size_t i = 0; i < n; ++i ) {
            char c = path[i];
            out[i] = ( c == '\\' ) ? '/' : c;
        }
        out[n] = '\0';
    }

    return needed;
}


size_t pathstr_to_windows( char const* path, char* out, size_t capacity ) {
    if( !path ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }

    size_t len = strlen( path );
    size_t needed = len + 1;

    if( out && capacity > 0 ) {
        size_t n = ( len < capacity - 1 ) ? len : capacity - 1;
        for( size_t i = 0; i < n; ++i ) {
            char c = path[i];
            out[i] = ( c == '/' ) ? '\\' : c;
        }
        out[n] = '\0';
    }

    return needed;
}


size_t pathstr_make_relative( char const* base, char const* target, pathstr_case_t case_mode, char* out, size_t capacity ) {
    if( !target ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }
    if( !base || base[0] == '\0' ) {
        size_t tlen = strlen( target );
        size_t need = tlen + 1;
        if( out && capacity > 0 ) {
            size_t n = ( tlen < capacity - 1 ) ? tlen : capacity - 1;
            if( n > 0 ) {
                memcpy( out, target, n );
            }
            out[n] = '\0';
        }
        return need;
    }

    #define PATHSTR_MAKE_RELATIVE_IS_SEP(c) ( (c) == '/' || (c) == '\\' )
    #define PATHSTR_MAKE_RELATIVE_TOLOWER(c) ( ( (c) >= 'A' && (c) <= 'Z' ) ? ( (c) + 32 ) : (c) )
    int ci = ( case_mode == PATHSTR_CASE_INSENSITIVE );

    enum { KIND_REL = 0, KIND_POSIX_ABS, KIND_DRIVE_ABS, KIND_DRIVE_REL, KIND_UNC, KIND_DEVICE };

    int b_kind = KIND_REL, t_kind = KIND_REL;
    char b_drive = 0, t_drive = 0;
    size_t b_root = 0, t_root = 0;

    char const* b_srv = NULL; size_t b_srv_len = 0;
    char const* b_shr = NULL; size_t b_shr_len = 0;
    char const* t_srv = NULL; size_t t_srv_len = 0;
    char const* t_shr = NULL; size_t t_shr_len = 0;

    size_t blen = strlen( base );
    size_t tlen = strlen( target );

    // base parse
    if( blen >= 4 && base[0] == '\\' && base[1] == '\\' && base[2] == '?' && PATHSTR_MAKE_RELATIVE_IS_SEP( base[3] ) ) {
        size_t i = 4;
        if( i + 3 < blen && ( ( base[i] | 32 ) == 'u' ) && ( ( base[i + 1] | 32 ) == 'n' ) && ( ( base[i + 2] | 32 ) == 'c' ) && PATHSTR_MAKE_RELATIVE_IS_SEP( base[i + 3] ) ) {
            i += 4;
            size_t ss = i;
            while( i < blen && !PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                ++i;
            }
            size_t s_len = i - ss;
            if( s_len > 0 && i < blen && PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                ++i;
                size_t sh = i;
                while( i < blen && !PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                    ++i;
                }
                size_t sh_len = i - sh;
                if( sh_len > 0 ) {
                    b_kind = KIND_UNC;
                    b_srv = base + ss;
                    b_srv_len = s_len;
                    b_shr = base + sh;
                    b_shr_len = sh_len;
                    b_root = i;
                }
            }
        } else if( i + 1 < blen && ( ( (unsigned char)base[i] | 32 ) >= 'a' ) && ( ( (unsigned char)base[i] | 32 ) <= 'z' ) && base[i + 1] == ':' ) {
            b_kind = KIND_DEVICE;
            b_drive = base[i];
            i += 2;
            if( i < blen && PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                ++i;
            }
            b_root = i;
        }
    }
    if( b_kind == KIND_REL ) {
        if( blen >= 2 && PATHSTR_MAKE_RELATIVE_IS_SEP( base[0] ) && PATHSTR_MAKE_RELATIVE_IS_SEP( base[1] ) ) {
            size_t i = 2;
            size_t ss = i;
            while( i < blen && !PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                ++i;
            }
            size_t s_len = i - ss;
            if( s_len > 0 && i < blen && PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                ++i;
                size_t sh = i;
                while( i < blen && !PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                    ++i;
                }
                size_t sh_len = i - sh;
                if( sh_len > 0 ) {
                    b_kind = KIND_UNC;
                    b_srv = base + ss;
                    b_srv_len = s_len;
                    b_shr = base + sh;
                    b_shr_len = sh_len;
                    b_root = i;
                } else {
                    b_kind = KIND_POSIX_ABS;
                    b_root = 1;
                }
            } else {
                b_kind = KIND_POSIX_ABS;
                b_root = 1;
            }
        } else if( blen >= 2 && ( ( (unsigned char)base[0] | 32 ) >= 'a' ) && ( ( (unsigned char)base[0] | 32 ) <= 'z' ) && base[1] == ':' ) {
            if( blen >= 3 && PATHSTR_MAKE_RELATIVE_IS_SEP( base[2] ) ) {
                b_kind = KIND_DRIVE_ABS;
                b_drive = base[0];
                b_root = 3;
            } else {
                b_kind = KIND_DRIVE_REL;
                b_drive = base[0];
                b_root = 2;
            }
        } else if( blen >= 1 && PATHSTR_MAKE_RELATIVE_IS_SEP( base[0] ) ) {
            b_kind = KIND_POSIX_ABS;
            b_root = 1;
        }
    }

    // target parse
    if( tlen >= 4 && target[0] == '\\' && target[1] == '\\' && target[2] == '?' && PATHSTR_MAKE_RELATIVE_IS_SEP( target[3] ) ) {
        size_t i = 4;
        if( i + 3 < tlen && ( ( target[i] | 32 ) == 'u' ) && ( ( target[i + 1] | 32 ) == 'n' ) && ( ( target[i + 2] | 32 ) == 'c' ) && PATHSTR_MAKE_RELATIVE_IS_SEP( target[i + 3] ) ) {
            i += 4;
            size_t ss = i;
            while( i < tlen && !PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                ++i;
            }
            size_t s_len = i - ss;
            if( s_len > 0 && i < tlen && PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                ++i;
                size_t sh = i;
                while( i < tlen && !PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                    ++i;
                }
                size_t sh_len = i - sh;
                if( sh_len > 0 ) {
                    t_kind = KIND_UNC;
                    t_srv = target + ss;
                    t_srv_len = s_len;
                    t_shr = target + sh;
                    t_shr_len = sh_len;
                    t_root = i;
                }
            }
        } else if( i + 1 < tlen && ( ( (unsigned char)target[i] | 32 ) >= 'a' ) && ( ( (unsigned char)target[i] | 32 ) <= 'z' ) && target[i + 1] == ':' ) {
            t_kind = KIND_DEVICE;
            t_drive = target[i];
            i += 2;
            if( i < tlen && PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                ++i;
            }
            t_root = i;
        }
    }
    if( t_kind == KIND_REL ) {
        if( tlen >= 2 && PATHSTR_MAKE_RELATIVE_IS_SEP( target[0] ) && PATHSTR_MAKE_RELATIVE_IS_SEP( target[1] ) ) {
            size_t i = 2;
            size_t ss = i;
            while( i < tlen && !PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                ++i;
            }
            size_t s_len = i - ss;
            if( s_len > 0 && i < tlen && PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                ++i;
                size_t sh = i;
                while( i < tlen && !PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                    ++i;
                }
                size_t sh_len = i - sh;
                if( sh_len > 0 ) {
                    t_kind = KIND_UNC;
                    t_srv = target + ss;
                    t_srv_len = s_len;
                    t_shr = target + sh;
                    t_shr_len = sh_len;
                    t_root = i;
                } else {
                    t_kind = KIND_POSIX_ABS;
                    t_root = 1;
                }
            } else {
                t_kind = KIND_POSIX_ABS;
                t_root = 1;
            }
        } else if( tlen >= 2 && ( ( (unsigned char)target[0] | 32 ) >= 'a' ) && ( ( (unsigned char)target[0] | 32 ) <= 'z' ) && target[1] == ':' ) {
            if( tlen >= 3 && PATHSTR_MAKE_RELATIVE_IS_SEP( target[2] ) ) {
                t_kind = KIND_DRIVE_ABS;
                t_drive = target[0];
                t_root = 3;
            } else {
                t_kind = KIND_DRIVE_REL;
                t_drive = target[0];
                t_root = 2;
            }
        } else if( tlen >= 1 && PATHSTR_MAKE_RELATIVE_IS_SEP( target[0] ) ) {
            t_kind = KIND_POSIX_ABS;
            t_root = 1;
        }
    }

    // domain check (return target verbatim if incomparable)
    int domain_ok = 0;
    if( b_kind == t_kind ) {
        if( b_kind == KIND_POSIX_ABS || b_kind == KIND_REL ) {
            domain_ok = 1;
        } else if( b_kind == KIND_DRIVE_ABS || b_kind == KIND_DRIVE_REL ) {
            unsigned char a = (unsigned char)b_drive;
            unsigned char d = (unsigned char)t_drive;
            if( ci ) {
                a = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( a );
                d = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( d );
            }
            domain_ok = ( a == d );
        } else if( b_kind == KIND_UNC ) {
            if( b_srv_len == t_srv_len && b_shr_len == t_shr_len ) {
                int ok = 1;
                for( size_t i = 0; i < b_srv_len; ++i ) {
                    unsigned char a = (unsigned char)b_srv[i];
                    unsigned char d = (unsigned char)t_srv[i];
                    if( ci ) {
                        a = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( a );
                        d = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( d );
                    }
                    if( a != d ) {
                        ok = 0;
                        break;
                    }
                }
                if( ok ) {
                    for( size_t i = 0; i < b_shr_len; ++i ) {
                        unsigned char a = (unsigned char)b_shr[i];
                        unsigned char d = (unsigned char)t_shr[i];
                        if( ci ) {
                            a = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( a );
                            d = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( d );
                        }
                        if( a != d ) {
                            ok = 0;
                            break;
                        }
                    }
                }
                domain_ok = ok;
            }
        } else if( b_kind == KIND_DEVICE ) {
            unsigned char a = (unsigned char)b_drive;
            unsigned char d = (unsigned char)t_drive;
            if( ci ) {
                a = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( a );
                d = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( d );
            }
            domain_ok = ( a == d );
        }
    }
    if( !domain_ok ) {
        if( out && capacity > 0 ) {
            out[0] = '\0';
        }
        return 1;
    }
    // split into normalized segments (drops '.'; resolves '..' lexically)
    enum { MAX_SEGS = 256 };
    char const* bseg[MAX_SEGS];
    size_t bls[MAX_SEGS];
    size_t bcnt = 0;
    char const* tseg[MAX_SEGS];
    size_t tls[MAX_SEGS];
    size_t tcnt = 0;

    {
        size_t i = b_root;
      while( i < blen ) {
            while( i < blen && PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                ++i;
            }
            if( i >= blen ) {
                break;
            }
            size_t s = i;
            while( i < blen && !PATHSTR_MAKE_RELATIVE_IS_SEP( base[i] ) ) {
                ++i;
            }
            size_t l = i - s;
            if( l == 1 && base[s] == '.' ) {
                // drop '.'
            } else if( l == 2 && base[s] == '.' && base[s + 1] == '.' ) {
                if( b_kind == KIND_POSIX_ABS || b_kind == KIND_DRIVE_ABS || b_kind == KIND_UNC || b_kind == KIND_DEVICE ) {
                    if( bcnt > 0 ) {
                        --bcnt;
                    }
                } else {
                    if( bcnt > 0 && !( bls[bcnt - 1] == 2 && bseg[bcnt - 1][0] == '.' && bseg[bcnt - 1][1] == '.' ) ) {
                        --bcnt;
                    } else if( bcnt < MAX_SEGS ) {
                        bseg[bcnt] = base + s;
                        bls[bcnt] = l;
                        ++bcnt;
                    }
                }
            } else {
                if( bcnt < MAX_SEGS ) {
                    bseg[bcnt] = base + s;
                    bls[bcnt] = l;
                    ++bcnt;
                }
            }
        }
    }

    {
        size_t i = t_root;
      while( i < tlen ) {
            while( i < tlen && PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                ++i;
            }
            if( i >= tlen ) {
                break;
            }
            size_t s = i;
            while( i < tlen && !PATHSTR_MAKE_RELATIVE_IS_SEP( target[i] ) ) {
                ++i;
            }
            size_t l = i - s;
            if( l == 1 && target[s] == '.' ) {
                // drop '.'
            } else if( l == 2 && target[s] == '.' && target[s + 1] == '.' ) {
                if( t_kind == KIND_POSIX_ABS || t_kind == KIND_DRIVE_ABS || t_kind == KIND_UNC || t_kind == KIND_DEVICE ) {
                    if( tcnt > 0 ) {
                        --tcnt;
                    }
                } else {
                    if( tcnt > 0 && !( tls[tcnt - 1] == 2 && tseg[tcnt - 1][0] == '.' && tseg[tcnt - 1][1] == '.' ) ) {
                        --tcnt;
                    } else if( tcnt < MAX_SEGS ) {
                        tseg[tcnt] = target + s;
                        tls[tcnt] = l;
                        ++tcnt;
                    }
                }
            } else {
                if( tcnt < MAX_SEGS ) {
                    tseg[tcnt] = target + s;
                    tls[tcnt] = l;
                    ++tcnt;
                }
            }
        }
    }

    // choose output separator based on base
    size_t b_end = blen;
    while( b_end > 0 && PATHSTR_MAKE_RELATIVE_IS_SEP( base[b_end - 1] ) ) {
        --b_end;
    }
    int b_had_trail = ( b_end < blen );
    char b_trail = b_had_trail ? base[blen - 1] : '/';
    if( b_trail != '/' && b_trail != '\\' ) {
        b_trail = '/';
    }
    char sep_out = '/';
    char any_sep = 0;
    for( size_t i = 0; i < b_end; ++i ) {
        char ch = base[i];
        if( PATHSTR_MAKE_RELATIVE_IS_SEP( ch ) ) {
            any_sep = ch;
        }
    }
    if( b_had_trail ) {
        sep_out = b_trail;
    } else if( any_sep ) {
        sep_out = any_sep;
    } else {
        sep_out = '/';
    }

    // base is a directory by default (no trailing slash needed)
    int base_is_dir = 1;

    size_t bdir_cnt = base_is_dir ? bcnt : ( bcnt > 0 ? bcnt - 1 : 0 );

    // common prefix (case policy)
    size_t common = 0;
    {
        size_t limit = ( bdir_cnt < tcnt ) ? bdir_cnt : tcnt;
        for( ; common < limit; ++common ) {
            if( bls[common] != tls[common] ) {
                break;
            }
            size_t l = bls[common];
            int ok = 1;
            for( size_t k = 0; k < l; ++k ) {
                unsigned char a = (unsigned char)bseg[common][k];
                unsigned char d = (unsigned char)tseg[common][k];
                if( ci ) {
                    a = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( a );
                    d = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( d );
                }
                if( a != d ) {
                    ok = 0;
                    break;
                }
            }
            if( !ok ) {
                break;
            }
        }
    }

    // identical -> "."
    if( bdir_cnt == tcnt ) {
        int same = 1;
        for( size_t i = 0; i < bdir_cnt && same; ++i ) {
            if( bls[i] != tls[i] ) {
                same = 0;
                break;
            }
            for( size_t k = 0; k < bls[i]; ++k ) {
                unsigned char a = (unsigned char)bseg[i][k];
                unsigned char d = (unsigned char)tseg[i][k];
                if( ci ) {
                    a = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( a );
                    d = (unsigned char)PATHSTR_MAKE_RELATIVE_TOLOWER( d );
                }
                if( a != d ) {
                    same = 0;
                    break;
                }
            }
        }
        if( same ) {
            size_t need = 2;
            if( out && capacity > 0 ) {
                out[0] = '.';
                out[( capacity > 1 ) ? 1 : ( capacity - 1 )] = '\0';
            }
            return need;
        }
    }

    size_t ups = ( bdir_cnt > common ) ? ( bdir_cnt - common ) : 0;
    size_t downs = ( tcnt > common ) ? ( tcnt - common ) : 0;

    size_t down_chars = 0;
    for( size_t i = 0; i < downs; ++i ) {
        down_chars += tls[common + i];
    }
    size_t total = ups + downs;
    size_t need = ( ups * 2 ) + down_chars + ( total > 1 ? ( total - 1 ) : 0 ) + 1;

    if( !out || capacity == 0 ) {
        return need;
    }

    size_t n = 0;
    for( size_t i = 0; i < total; ++i ) {
        if( i > 0 ) {
            if( n < capacity - 1 ) {
                out[n++] = sep_out;
            }
        }
        if( i < ups ) {
            if( n < capacity - 1 ) {
                out[n++] = '.';
            }
            if( n < capacity - 1 ) {
                out[n++] = '.';
            }
        } else {
            size_t idx = common + ( i - ups );
            size_t m = tls[idx];
            size_t copy = ( m < ( capacity - 1 - n ) ) ? m : ( capacity - 1 - n );
            if( copy > 0 ) {
                memcpy( out + n, tseg[idx], copy );
                n += copy;
        }
    }
    }
    out[( n < capacity ) ? n : ( capacity - 1 )] = '\0';
    return need;

    #undef PATHSTR_MAKE_RELATIVE_IS_SEP
    #undef PATHSTR_MAKE_RELATIVE_TOLOWER
}


int pathstr_is_absolute( char const* path ) {
    if( !path ) {
        return 0;
    }

    size_t len = strlen( path );
    if( len == 0 ) {
        return 0;
    }

    if( len >= 4 && path[0] == '\\' && path[1] == '\\' && path[2] == '?' && ( path[3] == '\\' || path[3] == '/' ) ) {
        return 1;
    }

    if( path[0] == '/' || path[0] == '\\' ) {
        return 1;
    }

    if( len >= 2 ) {
        unsigned char c0 = (unsigned char)path[0];
        if( ( ( c0 >= 'A' && c0 <= 'Z' ) || ( c0 >= 'a' && c0 <= 'z' ) ) && path[1] == ':' ) {
            if( len >= 3 && ( path[2] == '/' || path[2] == '\\' ) ) {
                return 1;
            }
            return 0;
        }
    }

    return 0;
}


int pathstr_is_relative( char const* path ) {
    if( !path ) {
        return 0;
    }

    size_t len = strlen( path );
    if( len == 0 ) {
        return 1;
    }

    if( len >= 4 && path[0] == '\\' && path[1] == '\\' && path[2] == '?' && ( path[3] == '\\' || path[3] == '/' ) ) {
        return 0;
    }

    if( path[0] == '/' || path[0] == '\\' ) {
        return 0;
    }

    if( len >= 2 ) {
        unsigned char c0 = (unsigned char)path[0];
        if( ( ( c0 >= 'A' && c0 <= 'Z' ) || ( c0 >= 'a' && c0 <= 'z' ) ) && path[1] == ':' ) {
            if( len >= 3 && ( path[2] == '/' || path[2] == '\\' ) ) {
                return 0;
            }
            return 1;
        }
    }

    return 1;
}


int pathstr_wildcard_compare( char const* pattern, char const* string, pathstr_case_t case_mode ) {
    if( !pattern || !string ) {
        return 0;
    }

    char const* p = pattern;
    char const* s = string;

    char const* star_p = NULL;        // position in pattern after last '*'
    char const* star_s = NULL;        // current string position tried for that '*'
    char const* star_origin = NULL;   // where that '*' began matching
    char const* star_seg_end = NULL;  // end of current segment for star_s
    int star_forbid_dot = 0;          // '*' at seg-start must not eat leading '.'

    for( ;; ) {
        // Collapse consecutive '*' and record backtrack info (segment-scoped)
        if( *p == '*' ) {
            while( *p == '*' ) {
                ++p;
            }
            star_p = p;
            star_s = s;
            star_origin = s;
            star_seg_end = s;
            while( *star_seg_end && *star_seg_end != '/' && *star_seg_end != '\\' ) {
                ++star_seg_end;
            }
            {
                int seg_start = ( s == string ) || ( ( s > string ) && ( s[-1] == '/' || s[-1] == '\\' ) );
                star_forbid_dot = seg_start && *s == '.';
            }
            continue;
        }

        // End of string: remaining pattern must be only '*'
        if( *s == '\0' ) {
            while( *p == '*' ) {
                ++p;
            }
            return *p == '\0';
        }

        // Escaped literal
        if( *p == '\\' && p[1] ) {
            unsigned char pc = (unsigned char) p[1];
            unsigned char sc = (unsigned char) *s;
            if( case_mode == PATHSTR_CASE_INSENSITIVE ) {
                if( pc >= 'A' && pc <= 'Z' ) {
                    pc = (unsigned char)(pc + 32);
                }
                if( sc >= 'A' && sc <= 'Z' ) {
                    sc = (unsigned char)(sc + 32);
                }
            }
            if( pc == sc ) {
                p += 2;
                ++s;
                continue;
            }
            goto backtrack;
        }

        // '?' matches any non-separator, but not a leading '.' at segment start
        if( *p == '?' ) {
            if( *s != '/' && *s != '\\' ) {
                int seg_start = ( s == string ) || ( ( s > string ) && ( s[-1] == '/' || s[-1] == '\\' ) );
                if( !( seg_start && *s == '.' ) ) {
                    ++p;
                    ++s;
                    continue;
                }
            }
            goto backtrack;
        }

        // Character class [...] (ranges, negation, escapes); never matches separators
        if( *p == '[' ) {
            char const* q = p + 1;
            int negate = 0;
            int matched = 0;
            int closed = 0;
            int first = 1;
            unsigned char sc = (unsigned char) *s;
            if( sc == '/' || sc == '\\' ) {
                goto backtrack;
            }
            if( *q == '!' || *q == '^' ) {
                negate = 1;
                ++q;
            }
            while( *q ) {
                if( *q == ']' && !first ) {
                    closed = 1;
                    ++q;
                    break;
                }
                unsigned char a;
                if( *q == '\\' && q[1] ) {
                    a = (unsigned char) q[1];
                    q += 2;
                } else {
                    a = (unsigned char) *q++;
                }
                first = 0;
                if( *q == '-' && q[1] && q[1] != ']' ) {
                    ++q;
                    unsigned char b;
                    if( *q == '\\' && q[1] ) {
                        b = (unsigned char) q[1];
                        q += 2;
                    } else {
                        b = (unsigned char) *q++;
                    }
                    unsigned char ca = a;
                    unsigned char cb = b;
                    unsigned char cs = sc;
                    if( case_mode == PATHSTR_CASE_INSENSITIVE ) {
                        if( ca >= 'A' && ca <= 'Z' ) {
                            ca = (unsigned char)(ca + 32);
                        }
                        if( cb >= 'A' && cb <= 'Z' ) {
                            cb = (unsigned char)(cb + 32);
                        }
                        if( cs >= 'A' && cs <= 'Z' ) {
                            cs = (unsigned char)(cs + 32);
                        }
                    }
                    if( ca > cb ) {
                        unsigned char t = ca;
                        ca = cb;
                        cb = t;
                    }
                    if( cs >= ca && cs <= cb ) {
                        matched = 1;
                    }
                } else {
                    unsigned char ca = a;
                    unsigned char cs = sc;
                    if( case_mode == PATHSTR_CASE_INSENSITIVE ) {
                        if( ca >= 'A' && ca <= 'Z' ) {
                            ca = (unsigned char)(ca + 32);
                        }
                        if( cs >= 'A' && cs <= 'Z' ) {
                            cs = (unsigned char)(cs + 32);
                        }
                    }
                    if( ca == cs ) {
                        matched = 1;
                    }
                }
            }
            if( closed && ( negate ? !matched : matched ) ) {
                p = q;
                ++s;
                continue;
            }
            if( !closed ) {
                // treat '[' as literal if class not closed
                unsigned char pc = (unsigned char) '[';
                unsigned char sc2 = (unsigned char) *s;
                if( case_mode == PATHSTR_CASE_INSENSITIVE ) {
                    if( pc >= 'A' && pc <= 'Z' ) {
                        pc = (unsigned char)(pc + 32);
                    }
                    if( sc2 >= 'A' && sc2 <= 'Z' ) {
                        sc2 = (unsigned char)(sc2 + 32);
                    }
                }
                if( pc == sc2 ) {
                    ++p;
                    ++s;
                    continue;
                }
            }
            goto backtrack;
        }

        // Separator must match separator; crossing dirs clears prior '*' context
        if( *p == '/' || *p == '\\' ) {
            if( *s == '/' || *s == '\\' ) {
                ++p;
                ++s;
                star_p = NULL;
                continue;
            }
            goto backtrack;
        }

        // Literal
        {
            unsigned char pc = (unsigned char) *p;
            unsigned char sc = (unsigned char) *s;
            if( case_mode == PATHSTR_CASE_INSENSITIVE ) {
                if( pc >= 'A' && pc <= 'Z' ) {
                    pc = (unsigned char)(pc + 32);
                }
                if( sc >= 'A' && sc <= 'Z' ) {
                    sc = (unsigned char)(sc + 32);
                }
            }
            if( pc == sc ) {
                ++p;
                ++s;
                continue;
            }
        }

    backtrack:
        if( star_p ) {
            // '*' cannot consume a leading '.' at segment start unless explicitly matched by pattern
            if( star_forbid_dot && star_s == star_origin ) {
                // zero-width was already tried; no valid consumption possible
                star_p = NULL;
                continue;
            }
            // Grow within current segment only (never across separators)
            if( star_s < star_seg_end ) {
                ++star_s;
                s = star_s;
                p = star_p;
                continue;
            }
            // At segment end: allow zero-width at the boundary only if next pattern token is a separator
            if( star_p[0] == '/' || star_p[0] == '\\' ) {
                s = star_seg_end;
                p = star_p;
                continue;
            }
        }
        return 0;
    }
}


#ifdef PATHSTR_CSTR

    #ifndef CSTR_NO_GLOBAL_API

    char const* cstr_path_basename( char const* path ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_basename( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_basename( path, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_dirname( char const* path ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_dirname( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_dirname( path, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_extname( char const* path ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_extname( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_extname( path, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_replace_extension( char const* path, char const* new_ext ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_replace_extension( path, new_ext, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_replace_extension( path, new_ext, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_join( char const* a, char const* b ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_join( a, b, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_join( a, b, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_normalize( char const* path ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_normalize( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_normalize( path, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_to_posix( char const* path ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_to_posix( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_to_posix( path, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_to_windows( char const* path ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_to_windows( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_to_windows( path, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_make_relative( char const* base, char const* target ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_make_relative( base, target, PATHSTR_CASE_SENSITIVE, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_make_relative( base, target, PATHSTR_CASE_SENSITIVE, out, capacity );
        }
        return cstr( out );
    }


    char const* cstr_path_make_relative_nocase( char const* base, char const* target ) {
        size_t capacity = cstr_temp_buffer_capacity();
        char* out = cstr_temp_buffer( capacity );
        size_t needed = pathstr_make_relative( base, target, PATHSTR_CASE_INSENSITIVE, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstr_temp_buffer( capacity );
            pathstr_make_relative( base, target, PATHSTR_CASE_INSENSITIVE, out, capacity );
        }
        return cstr( out );
    }


    int cstr_path_is_absolute( char const* path ) {
        return pathstr_is_absolute( path );
    }


    int cstr_path_is_relative( char const* path ) {
        return pathstr_is_relative( path );
    }


    int cstr_path_wildcard_compare( char const* pattern, char const* string ) {
        return pathstr_wildcard_compare( pattern, string, PATHSTR_CASE_SENSITIVE ); 
    }


    int cstr_path_wildcard_compare_nocase( char const* pattern, char const* string ) {
        return pathstr_wildcard_compare( pattern, string, PATHSTR_CASE_INSENSITIVE ); 
    }

    #endif // CSTR_NO_GLOBAL_API


    #ifdef CSTR_INSTANCE_API

    char const* cstri_path_basename( struct cstri_t* inst, char const* path ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_basename( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_basename( path, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_dirname( struct cstri_t* inst, char const* path ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_dirname( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_dirname( path, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_extname( struct cstri_t* inst, char const* path ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_extname( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_extname( path, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_replace_extension( struct cstri_t* inst, char const* path, char const* new_ext ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_replace_extension( path, new_ext, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_replace_extension( path, new_ext, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_join( struct cstri_t* inst, char const* a, char const* b ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_join( a, b, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_join( a, b, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_normalize( struct cstri_t* inst, char const* path ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_normalize( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_normalize( path, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_to_posix( struct cstri_t* inst, char const* path ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_to_posix( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_to_posix( path, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_to_windows( struct cstri_t* inst, char const* path ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_to_windows( path, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_to_windows( path, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_make_relative( struct cstri_t* inst, char const* base, char const* target ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_make_relative( base, target, PATHSTR_CASE_SENSITIVE, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_make_relative( base, target, PATHSTR_CASE_SENSITIVE, out, capacity );
        }
        return cstri( inst, out );
    }


    char const* cstri_path_make_relative_nocase( struct cstri_t* inst, char const* base, char const* target ) {
        size_t capacity = cstri_temp_buffer_capacity( inst );
        char* out = cstri_temp_buffer( inst, capacity );
        size_t needed = pathstr_make_relative( base, target, PATHSTR_CASE_INSENSITIVE, out, capacity );
        if( needed > capacity ) {
            capacity = needed;
            out = cstri_temp_buffer( inst, capacity );
            pathstr_make_relative( base, target, PATHSTR_CASE_INSENSITIVE, out, capacity );
        }
        return cstri( inst, out );
    }


    int cstri_path_is_absolute( struct cstri_t* inst, char const* path ) {
        return pathstr_is_absolute( path );
    }


    int cstri_path_is_relative( struct cstri_t* inst, char const* path ) {
        return pathstr_is_relative( path );
    }


    int cstri_path_wildcard_compare( struct cstri_t* inst, char const* pattern, char const* string ) {
        return pathstr_wildcard_compare( pattern, string, PATHSTR_CASE_SENSITIVE ); 
    }


    int cstri_path_wildcard_compare_nocase( struct cstri_t* inst, char const* pattern, char const* string ) {
        return pathstr_wildcard_compare( pattern, string, PATHSTR_CASE_INSENSITIVE ); 
    }

    #endif // cstri_INSTANCE_API

#endif // PATHSTR_CSTR 

    
#endif // PATHSTR_IMPLEMENTATION 


/*
-------------
 TESTS
-------------

To build and run the test suite, compile it like this:
	
	cl /Tc pathstr.h /DPATHSTR_RUN_TESTS /DPATHSTR_IMPLEMENTATION

and then simply run this from the commandline: 

	pathstr.exe

For more extensive tests, like memory leaks of access violations, you can use the 
testfw.h test framework by placing it in the same directory as pathstr.h, and 
defining the PATHSTR_USE_EXTERNAL_TESTFW when building

	cl /Tc pathstr.h /DPATHSTR_RUN_TESTS /D PATHSTR_USE_EXTERNAL_TESTFW /MTd

Note that to get correct memory leak reporting, you must specify /MTd to use the
debug memory allocation system in MSVC.

You can find testfw.h here:

	https://github.com/mattiasgustavsson/libs/blob/main/testfw.h

*/


#ifdef PATHSTR_RUN_TESTS

#ifdef PATHSTR_USE_EXTERNAL_TESTFW
	#include "testfw.h"
#else
	#pragma warning( push )
	#pragma warning( disable: 4710 ) // function not inlined
	#include <stdio.h>
	#pragma warning( pop )

	char const* g_testfw_desc = NULL; int g_testfw_line = 0, g_testfw_current_test_failed = 0, g_testfw_tests_count = 0, g_testfw_asserts_count = 0, g_testfw_tests_failed = 0, g_testfw_asserts_failed = 0;
	#define TESTFW_INIT() g_testfw_desc = NULL; g_testfw_line = 0; g_testfw_current_test_failed = 0; g_testfw_tests_count = 0; g_testfw_asserts_count = 0; g_testfw_tests_failed = 0; g_testfw_asserts_failed = 0;
	#define TESTFW_SUMMARY() \
		( ( g_testfw_tests_failed == 0 ) ? printf( "\n===============================================================================\nAll tests passed (%d assertions in %d test cases)\n", g_testfw_asserts_count, g_testfw_tests_count ) : \
		( printf( "\n===============================================================================\ntest cases: %4d | %4d passed | %4d failed\n",  g_testfw_tests_count, g_testfw_tests_count - g_testfw_tests_failed, g_testfw_tests_failed ),\
		printf( "assertions: %4d | %4d passed | %4d failed\n",  g_testfw_asserts_count, g_testfw_asserts_count - g_testfw_asserts_failed, g_testfw_asserts_failed ) ), g_testfw_tests_failed != 0 )
	#define TESTFW_TEST_BEGIN( desc ) { ++g_testfw_tests_count; g_testfw_desc = (desc); g_testfw_line = __LINE__; g_testfw_current_test_failed = 0;
	#define TESTFW_TEST_END() if( g_testfw_current_test_failed ) { ++g_testfw_tests_failed; } } g_testfw_desc = NULL; g_testfw_line = 0;
	#define TESTFW_EXPECTED( expression ) \
		++g_testfw_asserts_count; if( !(expression) ) { ++g_testfw_asserts_failed; g_testfw_current_test_failed = 1; if( g_testfw_desc )  {  printf( "\n-------------------------------------------------------------------------------\n" ); \
		printf( "%s\n", g_testfw_desc );  printf( "-------------------------------------------------------------------------------\n" ); printf( "%s(%d): %s\n", __FILE__, g_testfw_line, __func__ ); \
		printf( "...............................................................................\n" ); g_testfw_desc = NULL; }  printf( "\n%s(%d): FAILED:\n", __FILE__, __LINE__ );  printf( "\n  TESTFW_EXPECTED( %s )\n", #expression ); }  
#endif

static void test_pathstr_basename( void ) {
    TESTFW_TEST_BEGIN( "NULL -> empty string" )
        char out[8]; memset(out, 0xEE, sizeof( out ));
        char guard[4] = { 'X','Y','Z','W' };
        size_t need0 = pathstr_basename( NULL, NULL, 0 );
        TESTFW_EXPECTED( need0 == 1 );
        size_t need1 = pathstr_basename( NULL, out, sizeof( out ) );
        TESTFW_EXPECTED( need1 == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
        size_t need2 = pathstr_basename( NULL, guard, 0 );
        TESTFW_EXPECTED( need2 == 1 );
        TESTFW_EXPECTED( guard[0] == 'X' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty string -> empty string" )
        char out[8];
        size_t need = pathstr_basename( "", NULL, 0 );
        TESTFW_EXPECTED( need == 1 );
        size_t need2 = pathstr_basename( "", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/' -> '/'" )
        char out[8];
        size_t need = pathstr_basename( "/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp(out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'////' -> '/'" )
        char out[8];
        size_t need = pathstr_basename( "////", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp(out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\' and '\\\\\\\\' -> '\\'" )
        char out[8];
        size_t need1 = pathstr_basename( "\\\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need1 == 2 );
        TESTFW_EXPECTED( strcmp(out, "\\" ) == 0 );
        size_t need2 = pathstr_basename( "\\\\\\\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 2 );
        TESTFW_EXPECTED( strcmp(out, "\\" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'file' -> buffer management" )
        char const* p = "file";
        char const* e = "file";
        size_t need = pathstr_basename( p, NULL, 0 );
        TESTFW_EXPECTED( need == strlen(e) + 1 );
        char exact[5];
        size_t need2 = pathstr_basename( p, exact, sizeof(exact) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( strcmp(exact, e) == 0 );
        char tiny[3];
        size_t need3 = pathstr_basename( p, tiny, sizeof(tiny) );
        TESTFW_EXPECTED( need3 == need );
        TESTFW_EXPECTED( strncmp(tiny, e, sizeof(tiny)-1) == 0 );
        TESTFW_EXPECTED( tiny[sizeof(tiny)-1] == '\0' );
        char guard[3] = { 'A','B','C' };
        size_t need4 = pathstr_basename( p, guard, 0 );
        TESTFW_EXPECTED( need4 == need );
        TESTFW_EXPECTED( guard[0] == 'A' );
        TESTFW_EXPECTED( guard[1] == 'B' );
        TESTFW_EXPECTED( guard[2] == 'C' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'file.txt' -> 'file.txt'" )
        char out[32];
        size_t need = pathstr_basename( "file.txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("file.txt") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file.txt" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.' and '..' -> preserved" )
        char out[8];
        size_t need = pathstr_basename( ".", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp(out, "." ) == 0 );
        size_t need2 = pathstr_basename( "..", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 3 );
        TESTFW_EXPECTED( strcmp(out, ".." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/.' and '/..' -> '.' and '..'" )
        char out[8];
        size_t need = pathstr_basename( "/.", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp(out, "." ) == 0 );
        size_t need2 = pathstr_basename( "/..", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 3 );
        TESTFW_EXPECTED( strcmp(out, ".." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.hidden' and 'file.' -> preserved" )
        char out[32];
        size_t need = pathstr_basename( ".hidden", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen(".hidden") + 1 );
        TESTFW_EXPECTED( strcmp(out, ".hidden" ) == 0 );
        size_t need2 = pathstr_basename( "file.", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == strlen("file.") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir/file' -> 'file'" )
        char out[32];
        size_t need = pathstr_basename( "dir/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("file") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir/subdir/' -> 'subdir'" )
        char out[32];
        size_t need = pathstr_basename( "dir/subdir/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("subdir") + 1 );
        TESTFW_EXPECTED( strcmp(out, "subdir" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/usr/local/bin//' -> 'bin'" )
        char out[32];
        size_t need = pathstr_basename( "/usr/local/bin//", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("bin") + 1 );
        TESTFW_EXPECTED( strcmp(out, "bin" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'///usr///local///x' -> 'x'" )
        char out[32];
        size_t need = pathstr_basename( "///usr///local///x", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("x") + 1 );
        TESTFW_EXPECTED( strcmp(out, "x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir\\file' -> 'file'" )
        char out[32];
        size_t need = pathstr_basename( "dir\\file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("file") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir\\subdir\\' -> 'subdir'" )
        char out[32];
        size_t need = pathstr_basename( "dir\\subdir\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("subdir") + 1 );
        TESTFW_EXPECTED( strcmp(out, "subdir" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share\\file' -> 'file'" )
        char out[32];
        size_t need = pathstr_basename( "\\\\server\\share\\file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("file") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share\\' -> empty string" )
        char out[64];
        size_t need = pathstr_basename( "\\\\server\\share\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share/file' -> 'file'" )
        char out[32];
        size_t need = pathstr_basename( "//server/share/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("file") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/' -> empty string" )
        char out[8];
        size_t need = pathstr_basename( "C:/", out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:\\' -> empty string" )
        char out[8];
        size_t need2 = pathstr_basename( "C:\\\\", out, sizeof(out) );
        TESTFW_EXPECTED( need2 == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/dir/file' -> 'file'" )
        char out[16];
        size_t need = pathstr_basename( "C:/dir/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("file") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:temp.txt' -> 'temp.txt'" )
        char out[32];
        size_t need = pathstr_basename( "C:temp.txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("temp.txt") + 1 );
        TESTFW_EXPECTED( strcmp(out, "temp.txt" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a/b\\c//d\\e' -> 'e'" )
        char out[32];
        size_t need = pathstr_basename( "a/b\\c//d\\e", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("e") + 1 );
        TESTFW_EXPECTED( strcmp(out, "e" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'./file' -> 'file'" )
        char out[32];
        size_t need = pathstr_basename( "./file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("file") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file" ) == 0 );
        size_t need2 = pathstr_basename( "../file", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == strlen("file") + 1 );
        TESTFW_EXPECTED( strcmp(out, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/deep/path/with/veryverylongname' -> 'veryverylongname' (truncation)" )
        char const* p2 = "/deep/path/with/veryverylongname";
        char const* e2 = "veryverylongname";
        size_t need2 = pathstr_basename( p2, NULL, 0 );
        TESTFW_EXPECTED( need2 == strlen(e2) + 1 );
        char out2[64];
        size_t need3 = pathstr_basename( p2, out2, sizeof( out2 ) );
        TESTFW_EXPECTED( need3 == need2 );
        TESTFW_EXPECTED( strcmp(out2, e2) == 0 );
        char tiny2[3];
        size_t need4 = pathstr_basename( p2, tiny2, sizeof(tiny2) );
        TESTFW_EXPECTED( need4 == need2 );
        TESTFW_EXPECTED( strncmp(tiny2, e2, sizeof(tiny2)-1) == 0 );
        TESTFW_EXPECTED( tiny2[sizeof(tiny2)-1] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share' -> empty string" )
        char out3[64];
        size_t need = pathstr_basename( "//server/share", out3, sizeof( out3 ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out3, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share' -> empty string" )
        char out4[64];
        size_t need = pathstr_basename( "\\\\server\\share", out4, sizeof( out4 ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out4, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share///' -> empty string" )
        char out5[64];
        size_t need5 = pathstr_basename( "//server/share///", out5, sizeof( out5 ) );
        TESTFW_EXPECTED( need5 == 1 );
        TESTFW_EXPECTED( strcmp(out5, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.' -> capacity 1" )
        char out6[1] = { 'X' };
        size_t need6 = pathstr_basename( ".", out6, sizeof( out6 ) );
        TESTFW_EXPECTED( need6 == 2 );
        TESTFW_EXPECTED( out6[0] == '.' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'..' -> tiny buffers" )
        { char out1[1] = { 'X' }; size_t need1b = pathstr_basename( "..", out1, sizeof( out1 ) ); TESTFW_EXPECTED( need1b == 3 ); TESTFW_EXPECTED( out1[0] == '.' ); }
        { char out2[2] = { 'X','X' }; size_t need2b = pathstr_basename( "..", out2, sizeof( out2 ) ); TESTFW_EXPECTED( need2b == 3 ); TESTFW_EXPECTED( out2[0] == '.' ); TESTFW_EXPECTED( out2[1] == '.' ); }
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:' -> empty string" )
        char out7[8];
        size_t need = pathstr_basename( "C:", out7, sizeof( out7 ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out7, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\?\\C:\\path\\file' -> 'file'" )
        char out8[32];
        size_t need = pathstr_basename( "\\\\?\\C:\\path\\file", out8, sizeof( out8 ) );
        TESTFW_EXPECTED( need == strlen("file") + 1 );
        TESTFW_EXPECTED( strcmp( out8, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:' -> empty string" )
        char out9[8];
        size_t need9 = pathstr_basename( "C:", NULL, 0 );
        TESTFW_EXPECTED( need9 == 1 );
        size_t need10 = pathstr_basename( "C:", out9, sizeof( out9 ) );
        TESTFW_EXPECTED( need10 == 1 );
        TESTFW_EXPECTED( strcmp( out9, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:foo' -> 'foo'" )
        char out10[8];
        size_t need10 = pathstr_basename( "C:foo", NULL, 0 );
        TESTFW_EXPECTED( need10 == 4 );
        size_t need11 = pathstr_basename( "C:foo", out10, sizeof( out10 ) );
        TESTFW_EXPECTED( need11 == 4 );
        TESTFW_EXPECTED( strcmp( out10, "foo" ) == 0 );
    TESTFW_TEST_END();
}


static void test_pathstr_dirname( void ) {
    TESTFW_TEST_BEGIN( "NULL -> empty string" )
        char out[8];
        TESTFW_EXPECTED( pathstr_dirname( NULL, NULL, 0 ) == 1 );
        TESTFW_EXPECTED( pathstr_dirname( NULL, out, sizeof( out ) ) == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty string -> empty string" )
        char out[8];
        size_t need = pathstr_dirname( "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'file' -> '.'" )
        char out[8];
        size_t need = pathstr_dirname( "file", out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.' -> '.'" )
        char out[8];
        size_t need = pathstr_dirname( ".", out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'..' -> '.'" )
        char out[8];
        size_t need = pathstr_dirname( "..", out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir/file' -> 'dir'" )
        char out[32];
        size_t need = pathstr_dirname( "dir/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("dir") + 1 );
        TESTFW_EXPECTED( strcmp(out, "dir" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir/subdir/file.txt' -> 'dir/subdir'" )
        char out[64];
        size_t need = pathstr_dirname( "dir/subdir/file.txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("dir/subdir") + 1 );
        TESTFW_EXPECTED( strcmp(out, "dir/subdir" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir/subdir/' -> 'dir'" )
        char out[64];
        size_t need = pathstr_dirname( "dir/subdir/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("dir") + 1 );
        TESTFW_EXPECTED( strcmp(out, "dir" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/usr/local/bin//' -> '/usr/local'" )
        char out[64];
        size_t need = pathstr_dirname( "/usr/local/bin//", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("/usr/local") + 1 );
        TESTFW_EXPECTED( strcmp(out, "/usr/local" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/' -> '/'" )
        char out[8];
        size_t need = pathstr_dirname( "/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp(out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'////' -> '/'" )
        char out[8];
        size_t need = pathstr_dirname( "////", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp(out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir\\file' -> 'dir'" )
        char out[32];
        size_t need = pathstr_dirname( "dir\\file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("dir") + 1 );
        TESTFW_EXPECTED( strcmp(out, "dir" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share\\file' -> '\\\\server\\share'" )
        char out[128];
        size_t need = pathstr_dirname( "\\\\server\\share\\file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("\\\\server\\share") + 1 );
        TESTFW_EXPECTED( strcmp(out, "\\\\server\\share" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share/file' -> '//server/share'" )
        char out[128];
        size_t need = pathstr_dirname( "//server/share/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("//server/share") + 1 );
        TESTFW_EXPECTED( strcmp(out, "//server/share" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/' -> 'C:/'" )
        char out[16];
        size_t need = pathstr_dirname( "C:/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:/") + 1 );
        TESTFW_EXPECTED( strcmp(out, "C:/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:\\' -> 'C:\\'" )
        char out[16];
        size_t need = pathstr_dirname( "C:\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:\\") + 1 );
        TESTFW_EXPECTED( strcmp(out, "C:\\" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/dir/file' -> 'C:/dir'" )
        char out[32];
        size_t need = pathstr_dirname( "C:/dir/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:/dir") + 1 );
        TESTFW_EXPECTED( strcmp(out, "C:/dir" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\?\\C:\\path\\to\\f' -> '\\\\?\\C:\\path\\to'" )
        char out[128];
        size_t need = pathstr_dirname( "\\\\?\\C:\\path\\to\\f", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("\\\\?\\C:\\path\\to") + 1 );
        TESTFW_EXPECTED( strcmp(out, "\\\\?\\C:\\path\\to" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a/b\\c//d\\e' -> 'a/b\\c//d'" )
        char out[128];
        size_t need = pathstr_dirname( "a/b\\c//d\\e", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a/b\\c//d") + 1 );
        TESTFW_EXPECTED( strcmp(out, "a/b\\c//d" ) == 0 );
    TESTFW_TEST_END();
        
    TESTFW_TEST_BEGIN( "'C:temp.txt' -> 'C:'" )
        char out[16];
        size_t need = pathstr_dirname( "C:temp.txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:") + 1 );
        TESTFW_EXPECTED( strcmp(out, "C:" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/a/b/c/d/e/f/g/h/i/j/k/l/file' -> '/a/b/c/d/e/f/g/h/i/j/k/l' (truncation)" )
        char const* path = "/a/b/c/d/e/f/g/h/i/j/k/l/file";
        char const* exp  = "/a/b/c/d/e/f/g/h/i/j/k/l";
        size_t need = pathstr_dirname( path, NULL, 0 );
        TESTFW_EXPECTED( need == strlen(exp) + 1 );

        char out[64];
        size_t need2 = pathstr_dirname( path, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( strcmp(out, exp) == 0 );

        char tiny[4];
        size_t need3 = pathstr_dirname( path, tiny, sizeof(tiny) );
        TESTFW_EXPECTED( need3 == need );
        TESTFW_EXPECTED( tiny[3] == '\0' );
        TESTFW_EXPECTED( strncmp(tiny, exp, 3) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share' -> '//server/share'" )
        char out[64];
        size_t need = pathstr_dirname( "//server/share", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("//server/share") + 1 );
        TESTFW_EXPECTED( strcmp(out, "//server/share" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share' -> '\\\\server\\share'" )
        char out[64];
        size_t need = pathstr_dirname( "\\\\server\\share", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("\\\\server\\share") + 1 );
        TESTFW_EXPECTED( strcmp(out, "\\\\server\\share" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'////' -> capacity 1" )
        char out[1] = { 'X' };
        size_t need = pathstr_dirname( "////", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( out[0] == '/' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:' -> 'C:'" )
        char out[8];
        size_t need = pathstr_dirname( "C:", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/a' -> '/'" )
        char out[8];
        size_t need = pathstr_dirname( "/a", NULL, 0 );
        TESTFW_EXPECTED( need == 2 );
        size_t need2 = pathstr_dirname( "/a", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 2 );
        TESTFW_EXPECTED( strcmp( out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/a/' -> '/'" )
        char out[8];
        size_t need = pathstr_dirname( "/a/", NULL, 0 );
        TESTFW_EXPECTED( need == 2 );
        size_t need2 = pathstr_dirname( "/a/", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 2 );
        TESTFW_EXPECTED( strcmp( out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/.hidden' -> '/'" )
        char out[8];
        size_t need = pathstr_dirname( "/.hidden", NULL, 0 );
        TESTFW_EXPECTED( need == 2 );
        size_t need2 = pathstr_dirname( "/.hidden", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 2 );
        TESTFW_EXPECTED( strcmp( out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/foo.' -> '/'" )
        char out[8];
        size_t need = pathstr_dirname( "/foo.", NULL, 0 );
        TESTFW_EXPECTED( need == 2 );
        size_t need2 = pathstr_dirname( "/foo.", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 2 );
        TESTFW_EXPECTED( strcmp( out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard unchanged" )
        char guard[ 3 ] = { 'A','B','C' };
        size_t need = pathstr_dirname( "dir/file", guard, 0 );
        TESTFW_EXPECTED( need == strlen( "dir" ) + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'A' );
        TESTFW_EXPECTED( guard[ 1 ] == 'B' );
        TESTFW_EXPECTED( guard[ 2 ] == 'C' );
    TESTFW_TEST_END();
}



static void test_pathstr_extname( void ) {
    TESTFW_TEST_BEGIN( "NULL -> empty string" )
        char out[8];
        TESTFW_EXPECTED( pathstr_extname( NULL, NULL, 0 ) == 1 );
        TESTFW_EXPECTED( pathstr_extname( NULL, out, sizeof( out ) ) == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty string -> empty string" )
        char out[8];
        size_t need = pathstr_extname( "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'file.txt' -> '.txt'" )
        char out[16];
        size_t need = pathstr_extname( "file.txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen(".txt") + 1 );
        TESTFW_EXPECTED( strcmp(out, ".txt" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'archive.tar.gz' -> '.gz'" )
        char out[16];
        size_t need = pathstr_extname( "archive.tar.gz", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen(".gz") + 1 );
        TESTFW_EXPECTED( strcmp(out, ".gz" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'file.' -> empty string" )
        char out[8];
        size_t need = pathstr_extname( "file.", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.hidden' -> empty string" )
        char out[8];
        size_t need = pathstr_extname( ".hidden", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.' and '..' -> empty string" )
        char out[8];
        size_t need = pathstr_extname( ".", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
        size_t need2 = pathstr_extname( "..", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir/file.c' -> '.c'" )
        char out[8];
        size_t need = pathstr_extname( "dir/file.c", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen(".c") + 1 );
        TESTFW_EXPECTED( strcmp(out, ".c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'dir/subdir/' -> empty string" )
        char out[8];
        size_t need = pathstr_extname( "dir/subdir/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/usr/lib/.config' -> empty string" )
        char out[8];
        size_t need = pathstr_extname( "/usr/lib/.config", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/path/to/file.cpp' -> '.cpp'" )
        char out[8];
        size_t need = pathstr_extname( "C:/path/to/file.cpp", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen(".cpp") + 1 );
        TESTFW_EXPECTED( strcmp(out, ".cpp" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share\\file.TXT' -> '.TXT'" )
        char out[16];
        size_t need = pathstr_extname( "\\\\server\\share\\file.TXT", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen(".TXT") + 1 );
        TESTFW_EXPECTED( strcmp(out, ".TXT" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:temp.' -> empty string" )
        char out[8];
        size_t need = pathstr_extname( "C:temp.", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp(out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'verylong.name.part' -> truncation" )
        char const* p = "dir/veryveryverylong.name.part";
        char const* e = ".part";
        size_t need = pathstr_extname( p, NULL, 0 );
        TESTFW_EXPECTED( need == strlen(e) + 1 );

        char out[16];
        size_t need2 = pathstr_extname( p, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( strcmp(out, e) == 0 );

        char tiny[3];
        size_t need3 = pathstr_extname( p, tiny, sizeof(tiny) );
        TESTFW_EXPECTED( need3 == need );
        TESTFW_EXPECTED( strncmp(tiny, e, sizeof(tiny)-1) == 0 );
        TESTFW_EXPECTED( tiny[sizeof(tiny)-1] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'name..' -> empty string" )
        char out[8];
        size_t need = pathstr_extname( "name..", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'...rc' -> '.rc'" )
        char out[8];
        size_t need = pathstr_extname( "...rc", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen(".rc") + 1 );
        TESTFW_EXPECTED( strcmp( out, ".rc" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard unchanged" )
        char guard[ 3 ] = { 'A','B','C' };
        size_t need = pathstr_extname( "file.txt", guard, 0 );
        TESTFW_EXPECTED( need == strlen( ".txt" ) + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'A' );
        TESTFW_EXPECTED( guard[ 1 ] == 'B' );
        TESTFW_EXPECTED( guard[ 2 ] == 'C' );
    TESTFW_TEST_END();
}


static void test_pathstr_replace_extension( void ) {
    TESTFW_TEST_BEGIN( "NULL -> empty string" )
        size_t need = pathstr_replace_extension( NULL, "txt", NULL, 0 );
        TESTFW_EXPECTED( need == 1 );

        char out[ 8 ];
        size_t need2 = pathstr_replace_extension( NULL, "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 1 );
        TESTFW_EXPECTED( out[ 0 ] == '\0' );

        char guard[ 2 ] = { 'G', 'G' };
        size_t need3 = pathstr_replace_extension( NULL, "txt", guard, 0 );
        TESTFW_EXPECTED( need3 == 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'G' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty string + 'txt' -> unchanged" )
        char const* path = "";
        char out[ 8 ];
        size_t need = pathstr_replace_extension( path, "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( path ) + 1 );
        TESTFW_EXPECTED( strcmp( out, path ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/' + 'txt' -> unchanged" )
        char const* path = "/";
        char out[ 8 ];
        size_t need = pathstr_replace_extension( path, "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( path ) + 1 );
        TESTFW_EXPECTED( strcmp( out, path ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'////' + 'txt' -> unchanged" )
        char const* path = "////";
        char out[ 16 ];
        size_t need = pathstr_replace_extension( path, "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( path ) + 1 );
        TESTFW_EXPECTED( strcmp( out, path ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\' + 'txt' -> unchanged" )
        char const* path = "\\\\";
        char out[ 8 ];
        size_t need = pathstr_replace_extension( path, "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( path ) + 1 );
        TESTFW_EXPECTED( strcmp( out, path ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/' and 'C:\\' + 'txt' -> unchanged" )
        char out[ 16 ];
        size_t need1 = pathstr_replace_extension( "C:/", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need1 == strlen( "C:/" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:/" ) == 0 );

        size_t need2 = pathstr_replace_extension( "C:\\", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == strlen( "C:\\" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:\\" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC roots + 'txt' -> unchanged" )
        char out[ 64 ];
        size_t need1 = pathstr_replace_extension( "\\\\server\\share", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need1 == strlen( "\\\\server\\share" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\server\\share" ) == 0 );

        size_t need2 = pathstr_replace_extension( "\\\\server\\share\\", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == strlen( "\\\\server\\share\\" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\server\\share\\" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\?\\C:\\' + 'txt' -> unchanged" )
        char const* path = "\\\\?\\C:\\";
        char out[ 64 ];
        size_t need = pathstr_replace_extension( path, "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( path ) + 1 );
        TESTFW_EXPECTED( strcmp( out, path ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/dir/' + 'txt' -> unchanged" )
        char const* path = "/dir/";
        char out[ 32 ];
        size_t need = pathstr_replace_extension( path, "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( path ) + 1 );
        TESTFW_EXPECTED( strcmp( out, path ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'file.txt' + 'bin' -> 'file.bin'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "file.txt", "bin", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "file.bin" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "file.bin" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "remove extension 'file.txt' + \"\" -> 'file'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "file.txt", "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "file" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "ignore leading dot in new_ext 'file.txt' + '.bin' -> 'file.bin'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "file.txt", ".bin", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "file.bin" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "file.bin" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "no extension 'name' + 'txt' -> 'name.txt'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "name", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "name.txt" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "name.txt" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "no extension 'name' + '' -> unchanged" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "name", "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "name" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "name" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "trailing dot is not an extension 'name.' + 'txt' -> 'name.txt'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "name.", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "name.txt" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "name.txt" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "trailing dot remains when removing 'name.' + '' -> 'name.'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "name.", "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "name." ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "name." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "hidden file '.gitignore' + 'bak' -> '.gitignore.bak'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( ".gitignore", "bak", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( ".gitignore.bak" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, ".gitignore.bak" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "hidden multi-dot '.foo.bar' + 'zip' -> '.foo.zip'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( ".foo.bar", "zip", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( ".foo.zip" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, ".foo.zip" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "remove last extension 'name.tar.gz' + '' -> 'name.tar'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "name.tar.gz", "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "name.tar" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "name.tar" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "replace last extension 'name.tar.gz' + 'zip' -> 'name.tar.zip'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "name.tar.gz", "zip", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "name.tar.zip" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "name.tar.zip" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-relative 'C:temp.txt' + 'bin' -> 'C:temp.bin'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "C:temp.txt", "bin", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "C:temp.bin" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:temp.bin" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-relative remove 'C:temp.txt' + '' -> 'C:temp'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "C:temp.txt", "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "C:temp" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:temp" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "windows backslashes preserved 'C:\\dir\\file.txt' + 'bin'" )
        char out[ 128 ];
        size_t need = pathstr_replace_extension( "C:\\dir\\file.txt", "bin", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "C:\\dir\\file.bin" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:\\dir\\file.bin" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC preserved '\\\\server\\share\\file.txt' + 'bin'" )
        char out[ 128 ];
        size_t need = pathstr_replace_extension( "\\\\server\\share\\file.txt", "bin", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "\\\\server\\share\\file.bin" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\server\\share\\file.bin" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "device path '\\\\?\\C:\\path\\f' + 'txt' -> '\\\\?\\C:\\path\\f.txt'" )
        char out[ 128 ];
        size_t need = pathstr_replace_extension( "\\\\?\\C:\\path\\f", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "\\\\?\\C:\\path\\f.txt" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\?\\C:\\path\\f.txt" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "mixed seps 'a/b\\c//d\\e.txt' + 'bin' -> 'a/b\\c//d\\e.bin'" )
        char out[ 128 ];
        size_t need = pathstr_replace_extension( "a/b\\c//d\\e.txt", "bin", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "a/b\\c//d\\e.bin" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b\\c//d\\e.bin" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "dotfile component 'path/.' unchanged with 'txt'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "path/.", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "path/." ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "path/." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "dotdot component 'path/..' unchanged with 'txt'" )
        char out[ 64 ];
        size_t need = pathstr_replace_extension( "path/..", "txt", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "path/.." ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "path/.." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "tiny buffer truncation still '\0'-terminates" )
        char const* in = "/deep/path/name.ext";
        char const* ex = "/deep/path/name.bin";
        size_t need = pathstr_replace_extension( in, "bin", NULL, 0 );
        TESTFW_EXPECTED( need == strlen( ex ) + 1 );

        char tiny[ 5 ];
        size_t need2 = pathstr_replace_extension( in, "bin", tiny, sizeof( tiny ) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( tiny[ sizeof( tiny ) - 1 ] == '\0' );
        TESTFW_EXPECTED( strncmp( tiny, ex, sizeof( tiny ) - 1 ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard intact" )
        char const* in = "name.ext";
        char guard[ 2 ] = { 'Z', 'Z' };
        size_t need = pathstr_replace_extension( in, "x", guard, 0 );
        TESTFW_EXPECTED( need == strlen( "name.x" ) + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'Z' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'file.txt' + '.' -> 'file'" )
        char out[32];
        size_t need = pathstr_replace_extension( "file.txt", ".", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "file" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'name' + '.' -> 'name'" )
        char out[32];
        size_t need = pathstr_replace_extension( "name", ".", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "name" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "name" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'name' + '.ext' -> 'name.ext'" )
        char out[32];
        size_t need = pathstr_replace_extension( "name", ".ext", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "name.ext" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "name.ext" ) == 0 );
    TESTFW_TEST_END();
}


static void test_pathstr_join( void ) {
    TESTFW_TEST_BEGIN( "NULL,NULL -> empty string" )
        size_t need = pathstr_join( NULL, NULL, NULL, 0 );
        TESTFW_EXPECTED( need == 1 );
        char out[ 4 ] = { 'X','X','X','X' };
        size_t need2 = pathstr_join( NULL, NULL, out, sizeof(out) );
        TESTFW_EXPECTED( need2 == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty + empty -> empty string" )
        char out[ 8 ];
        size_t need = pathstr_join( "", "", out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "NULL + 'b' -> 'b'" )
        char out[ 8 ];
        size_t need = pathstr_join( NULL, "b", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a' + NULL -> 'a'" )
        char out[ 8 ];
        size_t need = pathstr_join( "a", NULL, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("a") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a' + 'b' -> 'a/b'" )
        char out[ 8 ];
        size_t need = pathstr_join( "a", "b", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("a/b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a/' + 'b' -> 'a/b'" )
        char out[ 8 ];
        size_t need = pathstr_join( "a/", "b", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("a/b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a' + '/b' -> '/b' (absolute override)" )
        char out[ 8 ];
        size_t need = pathstr_join( "a", "/b", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "/b" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "/b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a/' + '/b' -> '/b' (absolute override)" )
        char out[ 8 ];
        size_t need = pathstr_join( "a/", "/b", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "/b" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "/b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a\\b' + 'c\\d' -> 'a\\b\\c\\d' (preserve separator type)" )
        char out[ 32 ];
        size_t need = pathstr_join( "a\\b", "c\\d", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("a\\b\\c\\d") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a\\b\\c\\d" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a\\' + 'b' -> 'a\\b' (trailing backslash)" )
        char out[ 16 ];
        size_t need = pathstr_join( "a\\", "b", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("a\\b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a\\b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'x' + '\\\\server\\share\\file' -> '\\\\server\\share\\file' (UNC override)" )
        char const* b = "\\\\server\\share\\file";
        char out[ 64 ];
        size_t need = pathstr_join( "x", b, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen(b) + 1 );
        TESTFW_EXPECTED( strcmp( out, b ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'x' + '//server/share/file' -> '//server/share/file' (UNC override)" )
        char const* b = "//server/share/file";
        char out[ 64 ];
        size_t need = pathstr_join( "x", b, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen(b) + 1 );
        TESTFW_EXPECTED( strcmp( out, b ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'x' + '\\\\?\\C:\\path\\to\\f' -> '\\\\?\\C:\\path\\to\\f' (device override)" )
        char const* b = "\\\\?\\C:\\path\\to\\f";
        char out[ 64 ];
        size_t need = pathstr_join( "x", b, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen(b) + 1 );
        TESTFW_EXPECTED( strcmp( out, b ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "POSIX root '/' + 'file' -> '/file'" )
        char out[ 16 ];
        size_t need = pathstr_join( "/", "file", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "absolute override POSIX root '/' + '/file' -> '/file'" )
        char out[ 16 ];
        size_t need = pathstr_join( "/", "/file", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive root 'C:/' + 'file' -> 'C:/file'" )
        char out[ 32 ];
        size_t need = pathstr_join( "C:/", "file", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("C:/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive root 'C:\\' + 'file' -> 'C:\\file' (no inserted '/')" )
        char out[ 32 ];
        size_t need = pathstr_join( "C:\\", "file", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("C:\\file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:\\file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-relative base 'C:' + 'temp.txt' -> 'C:temp.txt' (no '/')" )
        char out[ 32 ];
        size_t need = pathstr_join( "C:", "temp.txt", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("C:temp.txt") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:temp.txt" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-relative dir 'C:dir' + 'file' -> 'C:dir/file'" )
        char out[ 32 ];
        size_t need = pathstr_join( "C:dir", "file", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("C:dir/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:dir/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC root '//server/share' + 'file' -> '//server/share/file'" )
        char out[ 64 ];
        size_t need = pathstr_join( "//server/share", "file", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("//server/share/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "//server/share/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC root with trailing sep '//server/share/' + 'file' -> '//server/share/file'" )
        char out[ 64 ];
        size_t need = pathstr_join( "//server/share/", "file", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("//server/share/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "//server/share/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "absolute override 'dir///' + '///file' -> '///file'" )
        char out[ 32 ];
        size_t need = pathstr_join( "dir///", "///file", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("///file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "///file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "a is only separators '////' + 'b' -> '//b'" )
        char out[ 16 ];
        size_t need = pathstr_join( "////", "b", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("//b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "//b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'' + '/x' -> '/x'" )
        char out[ 16 ];
        size_t need = pathstr_join( "", "/x", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("/x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "/x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty a + 'b' -> 'b'" )
        char out[ 8 ];
        size_t need = pathstr_join( "", "b", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a' + empty b -> 'a'" )
        char out[ 8 ];
        size_t need = pathstr_join( "a", "", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("a") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "truncation probe" )
        char const* a = "very/long/base/path";
        char const* b = "and/child";
        char const* exp = "very/long/base/path/and/child";
        size_t need = pathstr_join( a, b, NULL, 0 );
        TESTFW_EXPECTED( need == strlen( exp ) + 1 );

        char out[ 12 ];
        size_t need2 = pathstr_join( a, b, out, sizeof(out) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( out[ sizeof(out) - 1 ] == '\0' );
        TESTFW_EXPECTED( strncmp( out, exp, sizeof(out) - 1 ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard unchanged" )
        char guard[ 3 ] = { 'A','B','C' };
        size_t need = pathstr_join( "a", "b", guard, 0 );
        TESTFW_EXPECTED( need == strlen("a/b") + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'A' );
        TESTFW_EXPECTED( guard[ 1 ] == 'B' );
        TESTFW_EXPECTED( guard[ 2 ] == 'C' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "absolute drive b overrides: 'a' + 'C:/x' -> 'C:/x'" )
        char out[16];
        size_t need = pathstr_join( "a", "C:/x", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "C:/x" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:/x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "absolute drive b overrides: 'a' + 'C:\\x' -> 'C:\\x'" )
        char out[16];
        size_t need = pathstr_join( "a", "C:\\x", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "C:\\x" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:\\x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "a is only backslashes '\\\\\\\\' + 'b' -> '\\\\b'" )
        char out[8];
        size_t need = pathstr_join( "\\\\\\\\", "b", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("\\\\b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "preserve inserted sep only: 'a\\b' + 'c/d' -> 'a\\b\\c/d'" )
        char out[32];
        size_t need = pathstr_join( "a\\b", "c/d", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a\\b\\c/d") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a\\b\\c/d" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/base/dir' + '/foo/bar' -> 'C:/foo/bar'" )
        char out[64];
        size_t need = pathstr_join( "C:/base/dir", "/foo/bar", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("C:/foo/bar") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:/foo/bar" ) == 0 );

        size_t need2 = pathstr_join( "D:\\data", "\\logs", out, sizeof(out) );
        TESTFW_EXPECTED( need2 == strlen("D:\\logs") + 1 );
        TESTFW_EXPECTED( strcmp( out, "D:\\logs" ) == 0 );

        size_t need3 = pathstr_join( "E:/root", "////xyz", out, sizeof(out) );
        TESTFW_EXPECTED( need3 == strlen("E:/xyz") + 1 );
        TESTFW_EXPECTED( strcmp( out, "E:/xyz" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/usr/local' + '/bin' -> '/bin'" )
        char out[64];
        size_t need = pathstr_join( "/usr/local", "/bin", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("/bin") + 1 );
        TESTFW_EXPECTED( strcmp( out, "/bin" ) == 0 );
        size_t need2 = pathstr_join( "relative", "\\abc", out, sizeof(out) );
        TESTFW_EXPECTED( need2 == strlen("\\abc") + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\abc" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "normal relative rhs still joins under base" )
        char out[64];
        size_t need = pathstr_join( "C:/base", "file.txt", out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("C:/base/file.txt") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:/base/file.txt" ) == 0 );
    TESTFW_TEST_END();

}


static void test_pathstr_normalize( void ) {
    TESTFW_TEST_BEGIN( "NULL -> empty string" )
        size_t need = pathstr_normalize( NULL, NULL, 0 );
        TESTFW_EXPECTED( need == 1 );
        char out[ 4 ] = { 'X','X','X','X' };
        size_t need2 = pathstr_normalize( NULL, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 1 );
        TESTFW_EXPECTED( out[ 0 ] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty -> empty string" )
        char out[ 4 ];
        size_t need = pathstr_normalize( "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "relative collapse 'a//b///c' -> 'a/b/c'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "a//b///c", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "a/b/c" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b/c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "remove single dot 'a/b/./c' -> 'a/b/c'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "a/b/./c", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a/b/c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b/c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "fold parent '..' 'a/b/../c' -> 'a/c'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "a/b/../c", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a/c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "keep leading '..' '../a/../b' -> '../b'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "../a/../b", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("../b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "cannot pop past start 'a/../../b' -> '../b'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "a/../../b", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("../b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "trailing slash removed 'rel/dir/' -> 'rel/dir'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "rel/dir/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("rel/dir") + 1 );
        TESTFW_EXPECTED( strcmp( out, "rel/dir" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.' -> ''" )
        char out[ 8 ];
        size_t need = pathstr_normalize( ".", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'././' -> ''" )
        char out[ 8 ];
        size_t need = pathstr_normalize( "././", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'..' -> '..'" )
        char out[ 8 ];
        size_t need = pathstr_normalize( "..", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 3 );
        TESTFW_EXPECTED( strcmp( out, ".." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/a/./b//c/' -> '/a/b/c'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "/a/./b//c/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("/a/b/c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "/a/b/c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/../a' -> '/a'" )
        char out[ 16 ];
        size_t need = pathstr_normalize( "/../a", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("/a") + 1 );
        TESTFW_EXPECTED( strcmp( out, "/a" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/.' -> '/'" )
        char out[ 8 ];
        size_t need = pathstr_normalize( "/.", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/..' -> '/'" )
        char out[ 8 ];
        size_t need = pathstr_normalize( "/..", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'////' -> '/'" )
        char out[ 8 ];
        size_t need = pathstr_normalize( "////", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:\\\\a\\\\b\\\\..\\\\c\\\\' -> 'C:\\a\\c'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "C:\\\\a\\\\b\\\\..\\\\c\\\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:\\a\\c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:\\a\\c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'d:/./x//y/../' -> 'd:/x'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "d:/./x//y/../", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("d:/x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "d:/x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'E:\\.' -> 'E:\\'" )
        char out[ 8 ];
        size_t need = pathstr_normalize( "E:\\.", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("E:\\") + 1 );
        TESTFW_EXPECTED( strcmp( out, "E:\\" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:foo\\\\bar\\\\..\\\\baz' -> 'C:foo\\baz'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "C:foo\\\\bar\\\\..\\\\baz", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:foo\\baz") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:foo\\baz" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:..\\\\x' -> 'C:..\\x'" )
        char out[ 32 ];
        size_t need = pathstr_normalize( "C:..\\\\x", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:..\\x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:..\\x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:.' -> 'C:'" )
        char out[ 8 ];
        size_t need = pathstr_normalize( "C:.", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share\\a\\.\\b\\..\\c\\\\' -> '\\\\server\\share\\a\\c'" )
        char out[ 64 ];
        size_t need = pathstr_normalize( "\\\\server\\share\\a\\.\\b\\..\\c\\\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("\\\\server\\share\\a\\c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\server\\share\\a\\c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share///' -> '//server/share'" )
        char out[ 64 ];
        size_t need = pathstr_normalize( "//server/share///", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("//server/share") + 1 );
        TESTFW_EXPECTED( strcmp( out, "//server/share" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share/..' -> '//server/share'" )
        char out[ 64 ];
        size_t need = pathstr_normalize( "//server/share/..", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("//server/share") + 1 );
        TESTFW_EXPECTED( strcmp( out, "//server/share" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "device path '\\\\?\\C:\\path\\to\\..\\f' -> unchanged" )
        char const* p = "\\\\?\\C:\\path\\to\\..\\f";
        char out[ 128 ];
        size_t need = pathstr_normalize( p, out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( p ) + 1 );
        TESTFW_EXPECTED( strcmp( out, p ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "device UNC '\\\\?\\UNC\\server\\share\\x\\y\\..' -> unchanged" )
        char const* p = "\\\\?\\UNC\\server\\share\\x\\y\\..";
        char out[ 128 ];
        size_t need = pathstr_normalize( p, out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( p ) + 1 );
        TESTFW_EXPECTED( strcmp( out, p ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "mixed 'a\\b///c\\.\\d\\..\\e' -> 'a\\b\\c\\e'" )
        char out[ 64 ];
        size_t need = pathstr_normalize( "a\\b///c\\.\\d\\..\\e", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a\\b\\c\\e") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a\\b\\c\\e" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/file' -> '/file'" )
        char out[ 16 ];
        size_t need = pathstr_normalize( "/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/file' -> 'C:/file'" )
        char out[ 16 ];
        size_t need = pathstr_normalize( "C:/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share/file' -> '//server/share/file'" )
        char out[ 64 ];
        size_t need = pathstr_normalize( "//server/share/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("//server/share/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "//server/share/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "size probe returns needed length" )
        char const* in = "alpha//beta/./gamma/..//delta";
        char const* exp = "alpha/beta/delta";
        size_t need = pathstr_normalize( in, NULL, 0 );
        TESTFW_EXPECTED( need == strlen( exp ) + 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "truncation keeps prefix and NUL-terminates" )
        char const* in = "/very/long/path/with/many////segments/../names";
        char const* exp = "/very/long/path/with/many/names";
        size_t need = pathstr_normalize( in, NULL, 0 );
        TESTFW_EXPECTED( need == strlen( exp ) + 1 );
        char tiny[ 10 ];
        size_t need2 = pathstr_normalize( in, tiny, sizeof( tiny ) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( tiny[ sizeof( tiny ) - 1 ] == '\0' );
        TESTFW_EXPECTED( strncmp( tiny, exp, sizeof( tiny ) - 1 ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard unchanged" )
        char guard[ 3 ] = { 'A','B','C' };
        size_t need = pathstr_normalize( "a//b/./c", guard, 0 );
        TESTFW_EXPECTED( need == strlen( "a/b/c" ) + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'A' );
        TESTFW_EXPECTED( guard[ 1 ] == 'B' );
        TESTFW_EXPECTED( guard[ 2 ] == 'C' );
    TESTFW_TEST_END();
}


static void test_pathstr_to_posix( void ) {
    TESTFW_TEST_BEGIN( "NULL -> empty string" )
        size_t need = pathstr_to_posix( NULL, NULL, 0 );
        TESTFW_EXPECTED( need == 1 );

        char out[ 4 ] = { 'G','U','A','R' };
        size_t need2 = pathstr_to_posix( NULL, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 1 );
        TESTFW_EXPECTED( out[ 0 ] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty -> empty string" )
        char out[ 4 ];
        size_t need = pathstr_to_posix( "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "no changes 'a/b' -> 'a/b'" )
        char out[ 16 ];
        size_t need = pathstr_to_posix( "a/b", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( "a/b" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "backslashes 'a\\b\\c' -> 'a/b/c'" )
        char out[ 16 ];
        size_t need = pathstr_to_posix( "a\\b\\c", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a/b/c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b/c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "mixed 'a/b\\c//d\\e' -> 'a/b/c//d/e'" )
        char out[ 32 ];
        size_t need = pathstr_to_posix( "a/b\\c//d\\e", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a/b/c//d/e") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b/c//d/e" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "trailing backslash 'dir\\subdir\\' -> 'dir/subdir/'" )
        char out[ 32 ];
        size_t need = pathstr_to_posix( "dir\\subdir\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("dir/subdir/") + 1 );
        TESTFW_EXPECTED( strcmp( out, "dir/subdir/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "only backslashes '\\\\\\\\' -> '////'" )
        char out[ 16 ];
        size_t need = pathstr_to_posix( "\\\\\\\\", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("////") + 1 );
        TESTFW_EXPECTED( strcmp( out, "////" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC backslash '\\\\server\\share\\file' -> '//server/share/file'" )
        char out[ 64 ];
        size_t need = pathstr_to_posix( "\\\\server\\share\\file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("//server/share/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "//server/share/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "device path '\\\\?\\C:\\path\\to\\f' -> '//?/C:/path/to/f'" )
        char out[ 64 ];
        size_t need = pathstr_to_posix( "\\\\?\\C:\\path\\to\\f", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("//?/C:/path/to/f") + 1 );
        TESTFW_EXPECTED( strcmp( out, "//?/C:/path/to/f" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive 'C:\\dir\\file' -> 'C:/dir/file'" )
        char out[ 64 ];
        size_t need = pathstr_to_posix( "C:\\dir\\file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("C:/dir/file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C:/dir/file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "truncation probe" )
        char const* in  = "very\\\\long\\\\path\\\\segment";
        char const* exp = "very//long//path//segment";
        size_t need = pathstr_to_posix( in, NULL, 0 );
        TESTFW_EXPECTED( need == strlen( exp ) + 1 );

        char out[ 10 ];
        size_t need2 = pathstr_to_posix( in, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( out[ sizeof( out ) - 1 ] == '\0' );
        TESTFW_EXPECTED( strncmp( out, exp, sizeof( out ) - 1 ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard unchanged" )
        char guard[ 3 ] = { 'A','B','C' };
        size_t need = pathstr_to_posix( "a\\b", guard, 0 );
        TESTFW_EXPECTED( need == strlen( "a/b" ) + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'A' );
        TESTFW_EXPECTED( guard[ 1 ] == 'B' );
        TESTFW_EXPECTED( guard[ 2 ] == 'C' );
    TESTFW_TEST_END();
}


static void test_pathstr_to_windows( void ) {
    TESTFW_TEST_BEGIN( "NULL -> empty string" )
        size_t need = pathstr_to_windows( NULL, NULL, 0 );
        TESTFW_EXPECTED( need == 1 );

        char out[ 4 ] = { 'G','U','A','R' };
        size_t need2 = pathstr_to_windows( NULL, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 1 );
        TESTFW_EXPECTED( out[ 0 ] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty -> empty string" )
        char out[ 4 ];
        size_t need = pathstr_to_windows( "", out, sizeof( out ) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( strcmp( out, "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "simple 'a/b' -> 'a\\b'" )
        char out[ 16 ];
        size_t need = pathstr_to_windows( "a/b", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a\\b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a\\b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "mixed 'a/b\\c//d\\e' -> 'a\\b\\c\\\\d\\e'" )
        char out[ 32 ];
        size_t need = pathstr_to_windows( "a/b\\c//d\\e", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("a\\b\\c\\\\d\\e") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a\\b\\c\\\\d\\e" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "trailing slash 'dir/subdir/' -> 'dir\\subdir\\'" )
        char out[ 32 ];
        size_t need = pathstr_to_windows( "dir/subdir/", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("dir\\subdir\\") + 1 );
        TESTFW_EXPECTED( strcmp( out, "dir\\subdir\\" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "only slashes '////' -> '\\\\\\\\'" )
        char out[ 16 ];
        size_t need = pathstr_to_windows( "////", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("\\\\\\\\") + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\\\\\" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC forward '//server/share/file' -> '\\\\server\\share\\file'" )
        char out[ 64 ];
        size_t need = pathstr_to_windows( "//server/share/file", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("\\\\server\\share\\file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\server\\share\\file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "device already windows '\\\\?\\C:\\path\\to\\f' -> unchanged" )
        char const* in = "\\\\?\\C:\\path\\to\\f";
        char out[ 64 ];
        size_t need = pathstr_to_windows( in, out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen( in ) + 1 );
        TESTFW_EXPECTED( strcmp( out, in ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "device with slashes '\\\\?/C:/x/y' -> '\\\\?\\C:\\x\\y'" )
        char out[ 64 ];
        size_t need = pathstr_to_windows( "//?/C:/x/y", out, sizeof( out ) );
        TESTFW_EXPECTED( need == strlen("\\\\?\\C:\\x\\y") + 1 );
        TESTFW_EXPECTED( strcmp( out, "\\\\?\\C:\\x\\y" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "truncation probe" )
        char const* in  = "very/long/path/segment";
        char const* exp = "very\\long\\path\\segment";
        size_t need = pathstr_to_windows( in, NULL, 0 );
        TESTFW_EXPECTED( need == strlen( exp ) + 1 );

        char out[ 10 ];
        size_t need2 = pathstr_to_windows( in, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( out[ sizeof( out ) - 1 ] == '\0' );
        TESTFW_EXPECTED( strncmp( out, exp, sizeof( out ) - 1 ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard unchanged" )
        char guard[ 3 ] = { 'A','B','C' };
        size_t need = pathstr_to_windows( "a/b", guard, 0 );
        TESTFW_EXPECTED( need == strlen( "a\\b" ) + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'A' );
        TESTFW_EXPECTED( guard[ 1 ] == 'B' );
        TESTFW_EXPECTED( guard[ 2 ] == 'C' );
    TESTFW_TEST_END();
}


static void test_pathstr_make_relative_sensitive( void ) {
    TESTFW_TEST_BEGIN( "NULL,NULL -> empty string" )
        char out[ 4 ] = { 'A','B','C','D' };
        size_t need = pathstr_make_relative( NULL, NULL, PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "NULL + 't' -> 't'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( NULL, "t", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("t") + 1 );
        TESTFW_EXPECTED( strcmp( out, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty + 't' -> 't'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "", "t", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("t") + 1 );
        TESTFW_EXPECTED( strcmp( out, "t" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "identical POSIX '/a/b' vs '/a/b' -> '.'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "/a/b", "/a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "POSIX dir base '/a/b/c/' to '/a/b/c/d/e.txt' -> 'd/e.txt'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "/a/b/c/", "/a/b/c/d/e.txt", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen( "d/e.txt" ) + 1 );
        TESTFW_EXPECTED( strcmp( out, "d/e.txt" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "POSIX '/a/b/c' to '/a/b/x/y' -> '../x/y'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "/a/b/c", "/a/b/x/y", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../x/y") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../x/y" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "POSIX from '/' to '/x' -> 'x'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "/", "/x", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "POSIX '/' to '/' -> '.'" )
        char out[ 4 ];
        size_t need = pathstr_make_relative( "/", "/", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "POSIX case-sensitive '/A/B' to '/a/b/C' -> '../../a/b/C'" )
        char out[ 64 ];
        size_t need = pathstr_make_relative( "/A/B", "/a/b/C", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../../a/b/C") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../../a/b/C" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "rel dir 'a/b/c/' to 'a/b/c/d' -> 'd'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "a/b/c/", "a/b/c/d", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("d") + 1 );
        TESTFW_EXPECTED( strcmp( out, "d" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "rel file 'a/b/c' to 'a/b' -> '..'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "a/b/c", "a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("..") + 1 );
        TESTFW_EXPECTED( strcmp( out, ".." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drop leading './' in target 'a' to './x' -> '../x'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "a", "./x", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "rel './a/b' to './a/c' -> '../c'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "./a/b", "./a/c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive abs 'C:/a/b/' to 'C:/a/b/c/d' -> 'c/d'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "C:/a/b/", "C:/a/b/c/d", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("c/d") + 1 );
        TESTFW_EXPECTED( strcmp( out, "c/d" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive abs 'C:/a/b' to 'C:/a/b' -> '.'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "C:/a/b", "C:/a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive abs 'C:/a/b/file' to 'C:/a/b/c' -> '../c'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "C:/a/b/file", "C:/a/b/c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive mismatch 'C:/a' vs 'D:/a/b' -> empty" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "C:/a", "D:/a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive abs backslashes 'C:\\a\\b\\' to 'C:\\a\\b\\c' -> 'c'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "C:\\a\\b\\", "C:\\a\\b\\c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive abs mixed seps base 'C:\\a/b\\c/' to 'C:\\a/b\\c\\d/e' -> 'd/e'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "C:\\a/b\\c/", "C:\\a/b\\c\\d/e", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("d/e") + 1 );
        TESTFW_EXPECTED( strcmp( out, "d/e" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-rel cancel '..' 'C:foo\\bar' to 'C:foo\\bar\\..\\baz' -> '..\\baz'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "C:foo\\bar", "C:foo\\bar\\..\\baz", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("..\\baz") + 1 );
        TESTFW_EXPECTED( strcmp( out, "..\\baz" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-rel preserve leading '..' 'C:dir' to 'C:../x' -> '../../x'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "C:dir", "C:../x", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../../x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../../x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-rel dir 'C:dir\\sub\\' to 'C:dir\\file' -> '..\\file'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "C:dir\\sub\\", "C:dir\\file", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("..\\file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "..\\file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-rel 'C:dir' to 'C:dir' -> '.'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "C:dir", "C:dir", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:dir' to 'C:../x' -> '../../x'" )
        char out2[ 16 ];
        size_t need = pathstr_make_relative( "C:dir", "C:../x", PATHSTR_CASE_SENSITIVE, out2, sizeof(out2) );
        TESTFW_EXPECTED( need == strlen("../../x") + 1 );
        TESTFW_EXPECTED( strcmp( out2, "../../x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC forward '//server/share/a/b/' to '//server/share/a/b/c' -> 'c'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "//server/share/a/b/", "//server/share/a/b/c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC backslash '\\\\server\\share\\a\\b' to '\\\\server\\share\\a\\x\\y' -> '..\\x\\y'" )
        char out[ 64 ];
        size_t need = pathstr_make_relative( "\\\\server\\share\\a\\b", "\\\\server\\share\\a\\x\\y", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("..\\x\\y") + 1 );
        TESTFW_EXPECTED( strcmp( out, "..\\x\\y" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//s1/share/a' + '//s2/share/a/b' -> empty string" )
        char out[ 64 ];
        size_t need = pathstr_make_relative( "//s1/share/a", "//s2/share/a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/s1/a' + '//server/s2/a/b' -> empty string" )
        char out[ 64 ];
        size_t need = pathstr_make_relative( "//server/s1/a", "//server/s2/a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC root only base '//server/share' to '//server/share/a/b' -> 'a/b'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "//server/share", "//server/share/a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("a/b") + 1 );
        TESTFW_EXPECTED( strcmp( out, "a/b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "device same drive '\\\\?\\C:\\a\\b' to '\\\\?\\C:\\a\\b\\c' -> 'c'" )
        char out[ 64 ];
        size_t need = pathstr_make_relative( "\\\\?\\C:\\a\\b", "\\\\?\\C:\\a\\b\\c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\?\\C:\\a\\b' + '\\\\?\\D:\\a\\b\\c' -> empty string" )
        char out[ 64 ];
        size_t need = pathstr_make_relative( "\\\\?\\C:\\a\\b", "\\\\?\\D:\\a\\b\\c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/a/b' + 'C:/a/b' -> empty string" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "/a/b", "C:/a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/a' + '/a' -> empty string" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "C:/a", "/a", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "base 'a/b' to 'a/b/c' -> 'c'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "a/b", "a/b/c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "base 'a/b/' to 'a/b' -> '.'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "a/b/", "a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "base 'a/b/c.txt' to 'a/b' -> '..'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "a/b/c.txt", "a/b", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("..") + 1 );
        TESTFW_EXPECTED( strcmp( out, ".." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "inserted sep from base '\\\\' -> '..\\\\..\\\\c'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "x\\y\\", "x\\c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("..\\c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "..\\c" ) == 0 );
        size_t need2 = pathstr_make_relative( "x\\y\\z\\", "x\\c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need2 == strlen("..\\..\\c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "..\\..\\c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "inserted sep from base '/' -> '../../c'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "x/y/z/", "x/c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../../c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../../c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "POSIX abs ignore leading '..' at root '/a' vs '/a/../../x' -> '../x'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "/a", "/a/../../x", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "relative keep leading '..' when needed 'a' vs '../x' -> '../../x'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "a", "../x", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../../x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../../x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "base '////' to '/b/c' -> 'b/c'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "////", "/b/c", PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("b/c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "b/c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "truncation probe" )
        char const* base = "/very/long/base/path";
        char const* targ = "/very/long/base/path/child/node";
        char const* exp  = "child/node";
        size_t need = pathstr_make_relative( base, targ, PATHSTR_CASE_SENSITIVE, NULL, 0 );
        TESTFW_EXPECTED( need == strlen( exp ) + 1 );
        char out[ 6 ];
        size_t need2 = pathstr_make_relative( base, targ, PATHSTR_CASE_SENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( out[ sizeof(out) - 1 ] == '\0' );
        TESTFW_EXPECTED( strncmp( out, exp, sizeof(out) - 1 ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard unchanged" )
        char guard[ 3 ] = { 'A','B','C' };
        size_t need = pathstr_make_relative( "a/b", "a/b/c", PATHSTR_CASE_SENSITIVE, guard, 0 );
        TESTFW_EXPECTED( need == strlen( "c" ) + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'A' );
        TESTFW_EXPECTED( guard[ 1 ] == 'B' );
        TESTFW_EXPECTED( guard[ 2 ] == 'C' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\?\\UNC\\srv\\shr\\a' -> '\\\\?\\UNC\\srv\\shr\\b' == '..\\b'" )
        char out[16];
        size_t need = pathstr_make_relative( "\\\\?\\UNC\\srv\\shr\\a", "\\\\?\\UNC\\srv\\shr\\b", PATHSTR_CASE_SENSITIVE, NULL, 0 );
        TESTFW_EXPECTED( need == 5 );
        size_t need2 = pathstr_make_relative( "\\\\?\\UNC\\srv\\shr\\a", "\\\\?\\UNC\\srv\\shr\\b", PATHSTR_CASE_SENSITIVE, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 5 );
        TESTFW_EXPECTED( strcmp( out, "..\\b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "equal device UNC roots -> '.'" )
        char out[8];
        size_t need = pathstr_make_relative( "\\\\?\\UNC\\srv\\shr\\", "\\\\?\\UNC\\srv\\shr\\", PATHSTR_CASE_SENSITIVE, NULL, 0 );
        TESTFW_EXPECTED( need == 2 );
        size_t need2 = pathstr_make_relative( "\\\\?\\UNC\\srv\\shr\\", "\\\\?\\UNC\\srv\\shr\\", PATHSTR_CASE_SENSITIVE, out, sizeof( out ) );
        TESTFW_EXPECTED( need2 == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\?\\UNC\\srv\\shr1\\x\\y' + '\\\\?\\UNC\\srv\\shr2\\p\\q' -> empty string" )
        char out[128];
        const char* base2 = "\\\\?\\UNC\\srv\\shr1\\x\\y";
        const char* targ2 = "\\\\?\\UNC\\srv\\shr2\\p\\q";
        size_t need3 = pathstr_make_relative( base2, targ2, PATHSTR_CASE_SENSITIVE, NULL, 0 );
        TESTFW_EXPECTED( need3 == 1 );
        size_t need4 = pathstr_make_relative( base2, targ2, PATHSTR_CASE_SENSITIVE, out, sizeof( out ) );
        TESTFW_EXPECTED( need4 == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();
}


static void test_pathstr_make_relative_insensitive( void ) {
    TESTFW_TEST_BEGIN( "NULL,NULL -> empty string" )
        char out[ 4 ] = { 'A','B','C','D' };
        size_t need = pathstr_make_relative( NULL, NULL, PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 1 );
        TESTFW_EXPECTED( out[0] == '\0' );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "identical POSIX '/a/b' vs '/a/b' -> '.'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "/a/b", "/a/b", PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == 2 );
        TESTFW_EXPECTED( strcmp( out, "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "rel 'a' to './x' -> '../x'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "a", "./x", PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("../x") + 1 );
        TESTFW_EXPECTED( strcmp( out, "../x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "POSIX '/A/B' to '/a/b/C' -> 'C'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "/A/B", "/a/b/C", PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("C") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive abs 'C:/A/B' to 'c:/a/b/C' -> 'C'" )
        char out[ 8 ];
        size_t need = pathstr_make_relative( "C:/A/B", "c:/a/b/C", PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("C") + 1 );
        TESTFW_EXPECTED( strcmp( out, "C" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "drive-rel dir 'C:Dir\\Sub\\' to 'c:dir\\file' -> '..\\file'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "C:Dir\\Sub\\", "c:dir\\file", PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("..\\file") + 1 );
        TESTFW_EXPECTED( strcmp( out, "..\\file" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "UNC case-fold server/share '//Server/Share/a' to '//server/share/A/B' -> 'B'" )
        char out[ 32 ];
        size_t need = pathstr_make_relative( "//Server/Share/a", "//server/share/A/B", PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("B") + 1 );
        TESTFW_EXPECTED( strcmp( out, "B" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "device same drive '\\\\?\\c:\\A\\B' to '\\\\?\\C:\\a\\b\\c' -> 'c'" )
        char out[ 16 ];
        size_t need = pathstr_make_relative( "\\\\?\\c:\\A\\B", "\\\\?\\C:\\a\\b\\c", PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need == strlen("c") + 1 );
        TESTFW_EXPECTED( strcmp( out, "c" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "truncation probe" )
        char const* base = "/very/long/base/path";
        char const* targ = "/very/long/base/path/child/node";
        char const* exp  = "child/node";
        size_t need = pathstr_make_relative( base, targ, PATHSTR_CASE_INSENSITIVE, NULL, 0 );
        TESTFW_EXPECTED( need == strlen( exp ) + 1 );
        char out[ 6 ];
        size_t need2 = pathstr_make_relative( base, targ, PATHSTR_CASE_INSENSITIVE, out, sizeof(out) );
        TESTFW_EXPECTED( need2 == need );
        TESTFW_EXPECTED( out[ sizeof(out) - 1 ] == '\0' );
        TESTFW_EXPECTED( strncmp( out, exp, sizeof(out) - 1 ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "zero capacity leaves guard unchanged" )
        char guard[ 3 ] = { 'A','B','C' };
        size_t need = pathstr_make_relative( "a/b", "a/b/c", PATHSTR_CASE_INSENSITIVE, guard, 0 );
        TESTFW_EXPECTED( need == strlen( "c" ) + 1 );
        TESTFW_EXPECTED( guard[ 0 ] == 'A' );
        TESTFW_EXPECTED( guard[ 1 ] == 'B' );
        TESTFW_EXPECTED( guard[ 2 ] == 'C' );
    TESTFW_TEST_END();
}


static void test_pathstr_is_absolute( void ) {
    TESTFW_TEST_BEGIN( "NULL -> 0" )
        TESTFW_EXPECTED( pathstr_is_absolute( NULL ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty -> 0" )
        TESTFW_EXPECTED( pathstr_is_absolute( "" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.' -> 0" )
        TESTFW_EXPECTED( pathstr_is_absolute( "." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'..' -> 0" )
        TESTFW_EXPECTED( pathstr_is_absolute( ".." ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "/" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'////' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "////" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "/a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "\\a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "//" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "//server/share" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share/x' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "//server/share/x" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "\\\\server\\share" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share\\x' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "\\\\server\\share\\x" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "C:/" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:\\' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "C:\\" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'c:/a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "c:/a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'Z:\\a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "Z:\\a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:' -> 0" )
        TESTFW_EXPECTED( pathstr_is_absolute( "C:" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:foo' -> 0" )
        TESTFW_EXPECTED( pathstr_is_absolute( "C:foo" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\?\\C:\\\\a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "\\\\?\\C:\\a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a' -> 0" )
        TESTFW_EXPECTED( pathstr_is_absolute( "a" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a/b' -> 0" )
        TESTFW_EXPECTED( pathstr_is_absolute( "a/b" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "\\\\server\\share" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share\\x' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "\\\\server\\share\\x" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:\\' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "C:\\" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'Z:\\a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "Z:\\a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\?\\C:\\a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_absolute( "\\?\\C:\\a" ) == 1 );
    TESTFW_TEST_END();
}


static void test_pathstr_is_relative( void ) {

    TESTFW_TEST_BEGIN( "NULL -> 0" )
        TESTFW_EXPECTED( pathstr_is_relative( NULL ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty -> 1" )
        TESTFW_EXPECTED( pathstr_is_relative( "" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'.' -> 1" )
        TESTFW_EXPECTED( pathstr_is_relative( "." ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'..' -> 1" )
        TESTFW_EXPECTED( pathstr_is_relative( ".." ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_relative( "a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a/b' -> 1" )
        TESTFW_EXPECTED( pathstr_is_relative( "a/b" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'./a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_relative( "./a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'../a' -> 1" )
        TESTFW_EXPECTED( pathstr_is_relative( "../a" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'/' -> 0" )
        TESTFW_EXPECTED( pathstr_is_relative( "/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'////' -> 0" )
        TESTFW_EXPECTED( pathstr_is_relative( "////" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\a' -> 0" )
        TESTFW_EXPECTED( pathstr_is_relative( "\\a" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:/' -> 0 (drive abs)" )
        TESTFW_EXPECTED( pathstr_is_relative( "C:/" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:\\\\' -> 0 (drive abs)" )
        TESTFW_EXPECTED( pathstr_is_relative( "C:\\\\" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:' -> 1 (drive-relative)" )
        TESTFW_EXPECTED( pathstr_is_relative( "C:" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:foo' -> 1 (drive-relative)" )
        TESTFW_EXPECTED( pathstr_is_relative( "C:foo" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:.\\\\x' -> 1 (drive-relative)" )
        TESTFW_EXPECTED( pathstr_is_relative( "C:.\\x" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'C:..\\\\x' -> 1 (drive-relative)" )
        TESTFW_EXPECTED( pathstr_is_relative( "C:..\\x" ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share' -> 0 (UNC root)" )
        TESTFW_EXPECTED( pathstr_is_relative( "//server/share" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'//server/share/x' -> 0 (UNC path)" )
        TESTFW_EXPECTED( pathstr_is_relative( "//server/share/x" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\server\\share' -> 0 (UNC root)" )
        TESTFW_EXPECTED( pathstr_is_relative( "\\\\server\\share" ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\\\\?\\C:\\\\a' -> 0 (device path)" )
        TESTFW_EXPECTED( pathstr_is_relative( "\\\\?\\C:\\a" ) == 0 );
    TESTFW_TEST_END();
}


static void test_pathstr_wildcard_compare_sensitive( void ) {
    TESTFW_TEST_BEGIN( "NULL,NULL -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( NULL, NULL, PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();
    TESTFW_TEST_BEGIN( "NULL,'' -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( NULL, "", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'' ,NULL -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "", NULL, PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty + empty -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "", "", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty + 'a' -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "", "a", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'abc' + 'abc' -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "abc", "abc", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'AbC' + 'aBc' -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "AbC", "aBc", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*' matches empty -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*", "", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*' does not cross separators" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*", "any/thing\\goes.txt", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'**' collapses -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "**", "x", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'?' vs empty -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "?", "", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'?' one char -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "?", "a", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'?' not two chars -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "?", "ab", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'\?\?' vs 'ab' -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "??", "ab", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'ab*cd' middle -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "ab*cd", "abXYZcd", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*cd' endswith -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*cd", "abXYZcd", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*cd' not endswith -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*cd", "abXYZc", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'ab*' startswith -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "ab*", "abXYZcd", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*?*' requires >=1 -> empty fails" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*?*", "", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*?*' one char -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*?*", "x", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'?*' non-empty -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "?*", "x", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'?*' empty -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "?*", "", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a*b' + 'ab' -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a*b", "ab", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'a*b*c' cannot cross dirs" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a*b*c", "a/b/c", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*.txt' positive" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*.txt", "file.txt", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*.txt' negative" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*.txt", "file.tx", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*' must not match leading dot" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*", ".hidden", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'? must not match leading dot" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "?", ".", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "dot allowed if literal dot present" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*.*", ".bashrc", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "literal case mismatch" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "AbC", "aBc", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "deep backtracking succeeds" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*a*b*c*d*e*f*g*h*i*j*k*", "zzzaaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkk", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "deep backtracking fails" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*a*b*c*d*e*f*g*h*i*j*k*", "zzzaaaabbbbccccddddeeeeffffgggghhhhiiiijjjj", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "anchored tail is case-sensitive" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*cd", "xxcD", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "path-like segment star" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "src/*/file.?pp", "src/foo/file.cpp", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "star does not span multiple dirs" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "src/*/file.?pp", "src/foo/bar/file.cpp", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'?' does not match separator" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a?b", "a/b", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*' does not match separator" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a*b", "a/b", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "segmented suffix" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*/ending", "some/ending", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class simple" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "[abc]", "b", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class negation" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "[!abc]", "d", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class range" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "file.[a-c]", "file.b", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class does not match separator" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "[a-z]", "/", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class range case-sensitive" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "[A-C]", "b", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "escaped star is literal" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "\\*", "*", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "escaped question is literal" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a\\?b", "a?b", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "escaped bracket literal" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "file\\[1\\]", "file[1]", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class with escaped ]" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "file[\\]]", "file]", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "long repeating pattern success" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a*a*a*a*a*a*a*a*a*a*b", "aaaaaaaaaab", PATHSTR_CASE_SENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "long repeating pattern mismatch" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a*a*a*a*a*a*a*a*a*a*b", "aaaaaaaaaa", PATHSTR_CASE_SENSITIVE ) == 0 );
    TESTFW_TEST_END();
}

static void test_pathstr_wildcard_compare_insensitive( void ) {
    TESTFW_TEST_BEGIN( "NULL,NULL -> 0" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( NULL, NULL, PATHSTR_CASE_INSENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "empty + empty -> 1" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "", "", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "literal case-insensitive match" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "AbC", "aBc", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*' does not cross separators" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*", "any/thing\\goes.txt", PATHSTR_CASE_INSENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'?' does not match separator" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a?b", "a/b", PATHSTR_CASE_INSENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "segment star" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "src/*/file.?pp", "src/FOO/file.CPP", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "star not across dirs" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "src/*/file.?pp", "src/foo/bar/file.cpp", PATHSTR_CASE_INSENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "segmented suffix" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*/ending", "SOME/ENDING", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "'*' must not match leading dot" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*", ".hidden", PATHSTR_CASE_INSENSITIVE ) == 0 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "dot allowed if literal dot present" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*.*", ".BASHRC", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "anchored tail case-insensitive" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*cd", "xxcD", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class range folds case" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "[A-C]", "b", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class negation" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "[!x-z]", "A", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "escaped star literal" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "\\*", "*", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "escaped question literal" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "a\\?b", "A?B", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "class with escaped ]" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "file[\\]]", "FILE]", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();

    TESTFW_TEST_BEGIN( "deep backtracking succeeds" )
        TESTFW_EXPECTED( pathstr_wildcard_compare( "*a*b*c*d*e*f*g*h*i*j*k*", "ZZZaaaabbbbCCCCddddeeeeffffgggghhhhiiiijjjjkkkk", PATHSTR_CASE_INSENSITIVE ) == 1 );
    TESTFW_TEST_END();
}


int main( int argc, char** argv ) {
    (void) argc, (void) argv;

    TESTFW_INIT();
	
    test_pathstr_basename();
	test_pathstr_dirname();
	test_pathstr_extname();
    test_pathstr_replace_extension();
    test_pathstr_join();
    test_pathstr_normalize();
    test_pathstr_to_posix();
    test_pathstr_to_windows();
    test_pathstr_make_relative_sensitive();
    test_pathstr_make_relative_insensitive();
    test_pathstr_is_absolute();
    test_pathstr_is_relative();
    test_pathstr_wildcard_compare_sensitive();
    test_pathstr_wildcard_compare_insensitive();

    return TESTFW_SUMMARY();
}


#ifdef PATHSTR_USE_EXTERNAL_TESTFW
	#define TESTFW_IMPLEMENTATION
	#ifdef _MSC_VER
		#pragma warning( push )
		#pragma warning( disable: 4710 ) // function not inlined
		#pragma warning( disable: 4820 ) // '4' bytes padding added after data member
	#endif
	#include "testfw.h"
	#ifdef _MSC_VER
		#pragma warning( pop )
	#endif
#endif /* PATHSTR_USE_EXTERNAL_TESTFW */


#endif // PATHSTR_RUN_TESTS

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
