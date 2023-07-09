#include <assert.h> // assert()
#include <stdio.h>  // printf()

#define ASSETSYS_IMPLEMENTATION
#include "../assetsys.h"

#define STRPOOL_IMPLEMENTATION
#include "../strpool.h"

int main( void ) {
    // Create the asset system
    assetsys_t* assetsys = assetsys_create( 0 );
    assert( assetsys );

    // Mount current working folder as a virtual "/data" path
    assetsys_error_t result = assetsys_mount( assetsys, "..", "/data" );
    assert( result == ASSETSYS_SUCCESS );

    // Load a file
    assetsys_file_t file;
    result = assetsys_file( assetsys, "/data/README.md", &file );
    assert( result == ASSETSYS_SUCCESS );

    // Find the size of the file
    int size = assetsys_file_size( assetsys, file );
    assert( size > 30 );

    // Load the file
    char* content = (char*) malloc( size + 1 ); // extra space for '\0'
    int loaded_size = 0;
    result = assetsys_file_load( assetsys, file, &loaded_size, content, size );
    assert( result == ASSETSYS_SUCCESS );
    content[ size ] = '\0'; // zero terminate the text file

    // Clean up
    free( content );
    assetsys_destroy( assetsys );

    // Output result
    printf("assetsys: Tests passed\n");
}