http.h
======

Library: [http.h](../http.h)


Example
=======

```cpp
#define HTTP_IMPLEMENTATION
#include "http.h"                                                                                                                                                            

int main( int argc, char** argv )                                                                                                                          
	{                                                                                                                                                       
	(void) argc, argv;

	http_t* request = http_get( "http://www.mattiasgustavsson.com/http_test.txt", NULL );
	if( !request )
	{
		printf( "Invalid request.\n" );
		return 1;
	}

	http_status_t status = HTTP_STATUS_PENDING;
	int prev_size = -1;
	while( status == HTTP_STATUS_PENDING )
	{
		status = http_process( request );
		if( prev_size != (int) request->response_size )
		{
			printf( "%d byte(s) received.\n", (int) request->response_size );
			prev_size = (int) request->response_size;
		}
	}

	if( status == HTTP_STATUS_FAILED )
	{
		printf( "HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase );
		http_release( request );
		return 1;
	}

	printf( "\nContent type: %s\n\n%s\n", request->content_type, (char const*)request->response_data );        
	http_release( request );
	return 0;
	}
```


API Documentation
=================

http.h is a small library for making http requests from a web server. It only supports GET and POST http commands, and
is designed for when you just need a very basic way of communicating over http. http.h does not support https 
connections, just plain http.

http.h is a single-header library, and does not need any .lib files or other binaries, or any build scripts. To use 
it, you just include http.h to get the API declarations. To get the definitions, you must include http.h from 
*one* single C or C++ file, and #define the symbol `HTTP_IMPLEMENTATION` before you do. 


Customization
-------------

### Custom memory allocators

For working memory and to store the retrieved data, http.h needs to do dynamic allocation by calling `malloc`. Programs 
might want to keep track of allocations done, or use custom defined pools to allocate memory from. http.h allows 
for specifying custom memory allocation functions for `malloc` and `free`. This is done with the following code:

    #define HTTP_IMPLEMENTATION
    #define HTTP_MALLOC( ctx, size ) ( my_custom_malloc( ctx, size ) )
    #define HTTP_FREE( ctx, ptr ) ( my_custom_free( ctx, ptr ) )
    #include "http.h"

where `my_custom_malloc` and `my_custom_free` are your own memory allocation/deallocation functions. The `ctx` parameter
is an optional parameter of type `void*`. When `http_get` or `http_post` is called, , you can pass in a `memctx` 
parameter, which can be a pointer to anything you like, and which will be passed through as the `ctx` parameter to every 
`HTTP_MALLOC`/`HTTP_FREE` call. For example, if you are doing memory tracking, you can pass a pointer to your 
tracking data as `memctx`, and in your custom allocation/deallocation function, you can cast the `ctx` param back to the 
right type, and access the tracking data.

If no custom allocator is defined, http.h will default to `malloc` and `free` from the C runtime library.


http_get
--------

    http_t* http_get( char const* url, void* memctx )

Initiates a http GET request with the specified url. `url` is a zero terminated string containing the request location,
just like you would type it in a browser, for example `http://www.mattiasgustavsson.com:80/http_test.txt`. `memctx` is a 
pointer to user defined data which will be passed through to the custom HTTP_MALLOC/HTTP_FREE calls. It can be NULL if 
no user defined data is needed. Returns a `http_t` instance, which needs to be passed to `http_process` to process the
request. When the request is finished (or have failed), the returned `http_t` instance needs to be released by calling
`http_release`. If the request was invalid, `http_get` returns NULL.


http_post
---------

    http_t* http_post( char const* url, void const* data, size_t size, void* memctx )

Initiates a http POST request with the specified url. `url` is a zero terminated string containing the request location,
just like you would type it in a browser, for example `http://www.mattiasgustavsson.com:80/http_test.txt`. `data` is a
pointer to the data to be sent along as part of the request, and `size` is the number of bytes to send. `memctx` is a 
pointer to user defined data which will be passed through to the custom HTTP_MALLOC/HTTP_FREE calls. It can be NULL if 
no user defined data is needed. Returns a `http_t` instance, which needs to be passed to `http_process` to process the
request. When the request is finished (or have failed), the returned `http_t` instance needs to be released by calling
`http_release`. If the request was invalid, `http_post` returns NULL.


http_process
------------

    http_status_t http_process( http_t* http )

http.h uses non-blocking sockets, so after a request have been made by calling either `http_get` or `http_post`, you 
have to keep calling `http_process` for as long as it returns `HTTP_STATUS_PENDING`. You can call it from a loop which 
does other work too, for example from inside a game loop or from a loop which calls `http_process` on multiple requests.
If the request fails, `http_process` returns `HTTP_STATUS_FAILED`, and the fields `status_code` and `reason_phrase` may
contain more details (for example, status code can be 404 if the requested resource was not found on the server). If the 
request completes successfully, it returns `HTTP_STATUS_COMPLETED`. In this case, the `http_t` instance will contain 
details about the result. `status_code` and `reason_phrase` contains the details about the result, as specified in the
HTTP protocol. `content_type` contains the MIME type for the returns resource, for example `text/html` for a normal web
page. `response_data` is the pointer to the received data, and `resonse_size` is the number of bytes it contains. In the
case when the response data is in text format, http.h ensures there is a zero terminator placed immediately after the
response data block, so it is safe to interpret the resonse data as a `char*`. Note that the data size in this case will 
be the length of the data without the additional zero terminator.


http_release
------------

    void http_release( http_t* http )

Releases the resources acquired by `http_get` or `http_post`. Should be call when you are finished with the request.
