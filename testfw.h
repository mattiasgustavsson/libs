/*
------------------------------------------------------------------------------
		  Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

testfw.h - v1.2 - Basic test framework for C/C++.

Do this:
	#define TESTFW_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef testfw_h
#define testfw_h

#define TESTFW_INIT() testfw_init()
#define TESTFW_SUMMARY() testfw_summary( __FILE__, __func__, __LINE__ )
#if defined( _WIN32 ) && !defined( TESTFW_NO_CRASH_HANDLING ) && !defined( __TINYC__ ) && !defined( __GNUC__ ) && !defined( __clang__ )
    #if defined __cplusplus
        extern "C" unsigned long __cdecl _exception_code(void);
    #else
        unsigned long __cdecl _exception_code(void);
    #endif
    #define TESTFW_TEST_BEGIN( desc ) testfw_test_begin( desc, __FILE__, __func__, __LINE__ ); __try {
    #define TESTFW_TEST_END() } __except( 1 /*EXCEPTION_EXECUTE_HANDLER*/ ) \
        { testfw_exception( _exception_code/*GetExceptionCode*/() ); } \
        testfw_test_end( __FILE__, __func__, __LINE__ )
#elif ( defined( __linux__ ) || defined( __APPLE__ ) ) && !defined( TESTFW_NO_CRASH_HANDLING )
    #if defined( __linux__ ) && !defined( _DEFAULT_SOURCE )
        #define _DEFAULT_SOURCE
    #endif
    #include <setjmp.h>
    #define TESTFW_TEST_BEGIN( desc ) testfw_test_begin( desc, __FILE__, __func__, __LINE__ ); \
        { int testfw_internal_signum = sigsetjmp( *testfw_jmpbuf(), 1 ); if( testfw_internal_signum == 0 ) {
    #define TESTFW_TEST_END() } else { testfw_signal( testfw_internal_signum ); } } \
        testfw_test_end( __FILE__, __func__, __LINE__ )
#else
    #define TESTFW_TEST_BEGIN( desc ) testfw_test_begin( desc, __FILE__, __func__, __LINE__ ); {
    #define TESTFW_TEST_END() testfw_test_end( __FILE__, __func__, __LINE__ ); }
#endif
#define TESTFW_EXPECTED( expression ) testfw_expected( (expression) ? 1 : 0, #expression, __FILE__, __func__, __LINE__ )

void testfw_init( void );
int testfw_summary( char const* filename, char const* funcname, int line );
void testfw_test_begin( char const* desc, char const* filename, char const* funcname, int line );
void testfw_test_end( char const* filename, char const* funcname, int line );
void testfw_expected( int expression, char const* expression_str, char const* filename, char const* funcname, int line );
void testfw_print_test_desc( void );
void testfw_print_failure( char const* filename, int line );
void testfw_assertion_count_inc( void );
void testfw_current_test_assertion_failed( void );
#if defined( _WIN32 ) && !defined( TESTFW_NO_CRASH_HANDLING ) && !defined( __TINYC__ ) && !defined( __GNUC__ ) && !defined( __clang__ )
    void testfw_exception( unsigned int exception_code );
#elif ( defined( __linux__ ) || defined( __APPLE__ ) ) && !defined( TESTFW_NO_CRASH_HANDLING )
    sigjmp_buf* testfw_jmpbuf( void );
    void testfw_signal( int signum );
#endif


#endif /* testfw_h */


/**
Example use:

    #define TESTFW_IMPLEMENTATION
    #include "testfw.h"

    int main( int argc, char** argv ) {
        TESTFW_INIT();

        TESTFW_TEST_BEGIN( "arithmetic" );
            TESTFW_EXPECTED( 1 + 1 == 2 );
            TESTFW_EXPECTED( 7 / 2 == 3 );
        TESTFW_TEST_END();

        TESTFW_TEST_BEGIN( "null pointer" );
            volatile int* ptr = NULL;
            TESTFW_EXPECTED( *ptr == 0 ); // crashes, but is reported as a failed test and the run continues
        TESTFW_TEST_END();

        TESTFW_TEST_BEGIN( "comparison" );
            TESTFW_EXPECTED( 3 > 2 );
        TESTFW_TEST_END();

        return TESTFW_SUMMARY(); // non-zero if any test failed
    }
*/



#if defined( TESTFW_IMPLEMENTATION ) && !defined( TESTFW_IMPLEMENTATION_INCLUDED )
#define TESTFW_IMPLEMENTATION_INCLUDED

#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>

#if defined( _WIN32 ) && !defined( __TINYC__ ) && !defined( __GNUC__ ) && !defined( __clang__ )
    #pragma warning( push ) 
    #pragma warning( disable: 4619 ) // pragma warning : there is no warning number 'number'
    #pragma warning( disable: 4668 ) // 'symbol' is not defined as a preprocessor macro, replacing with '0' 
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
    #pragma warning( pop ) 
#endif /* _WIN32 */

#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#ifndef TESTFW_PRINTF
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdio.h>

    #define TESTFW_PRINTF( format, ... ) printf( format, ##__VA_ARGS__ ) 
#endif

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

#ifdef TESTFW_NO_ANSI
    #define TESTFW_ANSI_BLACK ""
    #define TESTFW_ANSI_BLUE ""
    #define TESTFW_ANSI_GREEN ""
    #define TESTFW_ANSI_CYAN ""
    #define TESTFW_ANSI_RED ""
    #define TESTFW_ANSI_MAGENTA ""
    #define TESTFW_ANSI_BROWN ""
    #define TESTFW_ANSI_LIGHT_GREY ""
    #define TESTFW_ANSI_GREY ""
    #define TESTFW_ANSI_LIGHT_BLUE ""
    #define TESTFW_ANSI_LIGHT_GREEN ""
    #define TESTFW_ANSI_LIGHT_CYAN ""
    #define TESTFW_ANSI_LIGHT_RED ""
    #define TESTFW_ANSI_LIGHT_MAGENTA ""
    #define TESTFW_ANSI_YELLOW ""
    #define TESTFW_ANSI_WHITE ""
    #define TESTFW_ANSI_RESET ""
#else
    #ifndef TESTFW_ANSI_BLACK
        #define TESTFW_ANSI_BLACK "\x1b[30m"
    #endif
    #ifndef TESTFW_ANSI_BLUE
        #define TESTFW_ANSI_BLUE "\x1b[34m"
    #endif
    #ifndef TESTFW_ANSI_GREEN
        #define TESTFW_ANSI_GREEN "\x1b[32m"
    #endif
    #ifndef TESTFW_ANSI_CYAN
        #define TESTFW_ANSI_CYAN "\x1b[36m"
    #endif
    #ifndef TESTFW_ANSI_RED
        #define TESTFW_ANSI_RED "\x1b[31m"
    #endif
    #ifndef TESTFW_ANSI_MAGENTA
        #define TESTFW_ANSI_MAGENTA "\x1b[35m"
    #endif
    #ifndef TESTFW_ANSI_BROWN
        #define TESTFW_ANSI_BROWN "\x1b[33m"
    #endif
    #ifndef TESTFW_ANSI_LIGHT_GREY
        #define TESTFW_ANSI_LIGHT_GREY "\x1b[37m"
    #endif
    #ifndef TESTFW_ANSI_GREY
        #define TESTFW_ANSI_GREY "\x1b[30;1m"
    #endif
    #ifndef TESTFW_ANSI_LIGHT_BLUE
        #define TESTFW_ANSI_LIGHT_BLUE "\x1b[34;1m"
    #endif
    #ifndef TESTFW_ANSI_LIGHT_GREEN
        #define TESTFW_ANSI_LIGHT_GREEN "\x1b[32;1m"
    #endif
    #ifndef TESTFW_ANSI_LIGHT_CYAN
        #define TESTFW_ANSI_LIGHT_CYAN "\x1b[36;1m"
    #endif
    #ifndef TESTFW_ANSI_LIGHT_RED
        #define TESTFW_ANSI_LIGHT_RED "\x1b[31;1m"
    #endif
    #ifndef TESTFW_ANSI_LIGHT_MAGENTA
        #define TESTFW_ANSI_LIGHT_MAGENTA "\x1b[35;1m"
    #endif
    #ifndef TESTFW_ANSI_YELLOW
        #define TESTFW_ANSI_YELLOW "\x1b[33;1m"
    #endif
    #ifndef TESTFW_ANSI_WHITE
        #define TESTFW_ANSI_WHITE "\x1b[37;1m"
    #endif
    #ifndef TESTFW_ANSI_RESET
        #define TESTFW_ANSI_RESET "\x1b[0m"
    #endif
#endif


struct testfw_internal_current_test_state_t
    {
    int is_active;
    char const* file;
    char const* func;
    int line;
    char const* desc;
    int desc_printed;
    int counted_as_failed;
    };


static struct 
    {
    long long tests_total;
    long long tests_failed;
    long long assertions_total;
    long long assertions_failed;
    struct testfw_internal_current_test_state_t current_test;
    #if defined( _WIN32 ) && defined( _DEBUG )  && !defined( __TINYC__ ) && !defined( __GNUC__ ) && !defined( __clang__ )
        int total_leaks;
    #endif
    } testfw_internal_state;


static void testfw_internal_print_progress_divider( char ch, long long fail, long long total )
    {
    int width = 79;
    int first = (int)( ( width * fail ) / ( total ? total : 1 ) );
    int second = width - first;
    if( fail > 0 && first == 0 )
        {
        ++first;
        --second;
        }
    TESTFW_PRINTF( "%s", TESTFW_ANSI_LIGHT_RED );
    for( int i = 0; i < first; ++i ) TESTFW_PRINTF( "%c", ch );
    TESTFW_PRINTF( "%s", TESTFW_ANSI_LIGHT_GREEN );
    for( int i = 0; i < second; ++i ) TESTFW_PRINTF( "%c", ch );
    TESTFW_PRINTF( "%s", TESTFW_ANSI_RESET );
    TESTFW_PRINTF( "\n" );
    }


#if defined( _WIN32 ) && defined( _DEBUG ) && !defined( __TINYC__ ) && !defined( __GNUC__ ) && !defined( __clang__ )

    static int testfw_internal_debug_report_hook( int report_type, char* message, int* return_value ) 
        { 
        _flushall();
        (void) report_type;
        *return_value = 0; // Don't break to debugger

        if( stricmp( message, "Detected memory leaks!\n" ) == 0 )
            {
            TESTFW_PRINTF( "-------------------------------------------------------------------------------\n" );
            TESTFW_PRINTF( "%sMEMORY CHECKS FAILED:%s Detected memory leaks\n", TESTFW_ANSI_LIGHT_RED, 
                TESTFW_ANSI_RESET );
            return 1; // Tell CRT not to print the message
            }
        else if( stricmp( message, "Dumping objects ->\n" ) == 0 )
            {
            TESTFW_PRINTF( "-------------------------------------------------------------------------------\n" );
            TESTFW_PRINTF( "\n" );
            return 1; // Tell CRT not to print the message
            }
        else if( stricmp( message, "Object dump complete.\n" ) == 0 )
            {
            TESTFW_PRINTF( "\n" );
            _CrtMemState state;
            _CrtMemCheckpoint( &state );

            static char const* const block_use_names[ 5 ] = { "Free", "Normal", "CRT", "Ignore", "Client", };
            for( unsigned int i = 0; i < 5; ++i )
                {
                if( i != 2 && state.lCounts[ i ] )
                    TESTFW_PRINTF( "%lld bytes in %lld %hs Blocks.\n", (long long) state.lSizes[ i ], 
                        (long long) state.lCounts[ i ], block_use_names[ i ] );
                }

            TESTFW_PRINTF( "\n" );
            TESTFW_PRINTF( "High water mark: %lld bytes.\n", (long long)state.lHighWaterCount);
            TESTFW_PRINTF( "All allocations: %lld bytes.\n",   (long long)state.lTotalCount);
            TESTFW_PRINTF( "\n" );
            TESTFW_PRINTF( "===============================================================================\n" );
            TESTFW_PRINTF( "%sMEMORY LEAKS: %d%s\n\n", TESTFW_ANSI_LIGHT_RED, testfw_internal_state.total_leaks, TESTFW_ANSI_RESET );
            return 1; // Tell CRT not to print the message
            }
        else if( strnicmp( message, " Data: <", 8 ) == 0 )
            {
            ++testfw_internal_state.total_leaks;
            return 1; // Tell CRT not to print the message
            }
    
        return 0;  // Tell CRT it can print the message, as we didn't handle it
        }

#endif /* _WIN32 && _DEBUG */


#if defined( _WIN32 ) 
    #pragma warning( push )
    #pragma warning( disable: 4255 )
    #pragma warning( disable: 4668 )
    #pragma warning( disable: 5105 )
    #include <windows.h>
    #pragma warning( pop )
#endif


#if ( defined( __linux__ ) || defined( __APPLE__ ) ) && !defined( TESTFW_NO_CRASH_HANDLING )
    #include <signal.h>
    #include <stdio.h>

    static int const testfw_internal_signals[] = { SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGTRAP };
    #define TESTFW_INTERNAL_SIGNAL_COUNT ( (int)( sizeof( testfw_internal_signals ) / sizeof( *testfw_internal_signals ) ) )

    static sigjmp_buf testfw_internal_jmpbuf;
    static struct sigaction testfw_internal_old_actions[ TESTFW_INTERNAL_SIGNAL_COUNT ];
    static stack_t testfw_internal_old_altstack;
    static char testfw_internal_altstack[ 64 * 1024 ]; // handler only calls siglongjmp, but must run after stack overflow


    static void testfw_internal_signal_handler( int signum )
        {
        siglongjmp( testfw_internal_jmpbuf, signum );
        }


    static void testfw_internal_install_signal_handlers( void )
        {
        stack_t altstack;
        memset( &altstack, 0, sizeof( altstack ) );
        altstack.ss_sp = testfw_internal_altstack;
        altstack.ss_size = sizeof( testfw_internal_altstack );
        altstack.ss_flags = 0;
        sigaltstack( &altstack, &testfw_internal_old_altstack );

        struct sigaction action;
        memset( &action, 0, sizeof( action ) );
        action.sa_handler = testfw_internal_signal_handler;
        action.sa_flags = SA_ONSTACK;
        sigemptyset( &action.sa_mask );
        for( int i = 0; i < TESTFW_INTERNAL_SIGNAL_COUNT; ++i )
            sigaction( testfw_internal_signals[ i ], &action, &testfw_internal_old_actions[ i ] );
        }


    static void testfw_internal_restore_signal_handlers( void )
        {
        for( int i = 0; i < TESTFW_INTERNAL_SIGNAL_COUNT; ++i )
            sigaction( testfw_internal_signals[ i ], &testfw_internal_old_actions[ i ], NULL );
        sigaltstack( &testfw_internal_old_altstack, NULL );
        }


    sigjmp_buf* testfw_jmpbuf( void )
        {
        return &testfw_internal_jmpbuf;
        }
#endif


void testfw_init( void )
    {
    memset( &testfw_internal_state, 0, sizeof( testfw_internal_state ) );

    #if defined( _WIN32 ) && !defined( __GNUC__ ) && !defined( __clang__ )
        #if !defined( __TINYC__ )
            int flag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ); // Get current flag
            flag ^= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
            _CrtSetDbgFlag( flag ); // Set flag to the new value
            _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
            _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
            _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
            _CrtSetReportHook2( _CRT_RPTHOOK_INSTALL, testfw_internal_debug_report_hook );
            _CrtSetReportHook( testfw_internal_debug_report_hook );
        #endif /* __TINYC__ */
        HANDLE handle = GetStdHandle( STD_OUTPUT_HANDLE );
        if( handle != INVALID_HANDLE_VALUE ) {
            DWORD mode = 0;
            GetConsoleMode( handle, &mode );
            SetConsoleMode( handle, mode | 0x0004 /*ENABLE_VIRTUAL_TERMINAL_PROCESSING*/ );
        }
    #endif /* _WIN32 */
    }


static int testfw_internal_must_be_in_test( void )
    {
    if( !testfw_internal_state.current_test.is_active )
        TESTFW_PRINTF( "\n\n%sEXPECTED TO BE IN AN ACTIVE TEST, BUT NO TEST IS CURRENTLY ACTIVE.%s\n\n", 
            TESTFW_ANSI_LIGHT_RED, TESTFW_ANSI_RESET );
    return !testfw_internal_state.current_test.is_active;
    }


static int testfw_internal_can_not_be_in_test( void )
    {
    if( testfw_internal_state.current_test.is_active )
        TESTFW_PRINTF( "\n\n%sEXPECTED TO NOT BE IN AN ACTIVE TEST, BUT A TEST IS CURRENTLY ACTIVE.%s\n\n", 
            TESTFW_ANSI_LIGHT_RED, TESTFW_ANSI_RESET );
    return testfw_internal_state.current_test.is_active;
    }


void testfw_print_test_desc( void )
    {
    if( testfw_internal_must_be_in_test() ) return;

    if( !testfw_internal_state.current_test.desc_printed ) 
        {
        testfw_internal_state.current_test.desc_printed = 1;
        
        TESTFW_PRINTF( "\n" );
        TESTFW_PRINTF( "-------------------------------------------------------------------------------\n" );
        TESTFW_PRINTF( "%s\n", testfw_internal_state.current_test.desc ? testfw_internal_state.current_test.desc : 
            "<NO DESCRIPTION>" ); 
        TESTFW_PRINTF( "-------------------------------------------------------------------------------\n" );
        TESTFW_PRINTF( "%s%s(%d): %s%s\n", TESTFW_ANSI_LIGHT_GREY, testfw_internal_state.current_test.file, 
            testfw_internal_state.current_test.line, TESTFW_ANSI_RESET, testfw_internal_state.current_test.func );
        
        TESTFW_PRINTF( "%s", TESTFW_ANSI_LIGHT_GREY );
        TESTFW_PRINTF( "...............................................................................\n" );
        TESTFW_PRINTF( "%s", TESTFW_ANSI_RESET );
        } 
    }


int testfw_summary( char const* filename, char const* funcname, int line )
    {
    (void) filename; (void) funcname; (void) line;
    
    if( testfw_internal_can_not_be_in_test() ) return 1;

    TESTFW_PRINTF( "\n" );
    testfw_internal_print_progress_divider( '=', testfw_internal_state.tests_failed, testfw_internal_state.tests_total );   
    if( testfw_internal_state.tests_failed == 0 && testfw_internal_state.assertions_failed == 0 )
        {
        TESTFW_PRINTF( "%sAll tests passed%s (%lld assertions in %lld test cases)\n", TESTFW_ANSI_LIGHT_GREEN, 
            TESTFW_ANSI_RESET, testfw_internal_state.assertions_total, testfw_internal_state.tests_total );
        }
    else
        {
        TESTFW_PRINTF( "test cases: %4lld |%s %4lld passed %s|%s %4lld failed%s\n", testfw_internal_state.tests_total,
            TESTFW_ANSI_LIGHT_GREEN, testfw_internal_state.tests_total - testfw_internal_state.tests_failed, 
            TESTFW_ANSI_RESET, TESTFW_ANSI_LIGHT_RED, testfw_internal_state.tests_failed, TESTFW_ANSI_RESET );
        TESTFW_PRINTF( "assertions: %4lld |%s %4lld passed %s|%s %4lld failed%s\n", testfw_internal_state.assertions_total, 
            TESTFW_ANSI_LIGHT_GREEN, testfw_internal_state.assertions_total - testfw_internal_state.assertions_failed, 
            TESTFW_ANSI_RESET, TESTFW_ANSI_LIGHT_RED, testfw_internal_state.assertions_failed, TESTFW_ANSI_RESET );
        }

    TESTFW_PRINTF( "\n\n" );

    #if defined( _WIN32 ) && defined( _DEBUG ) && !defined( __TINYC__ ) && !defined( __GNUC__ ) && !defined( __clang__ )
        int result = _CrtDumpMemoryLeaks();
        testfw_internal_state.tests_failed += result ? 1 : 0;
    #endif

    return testfw_internal_state.tests_failed > 0 ? 1 : 0;
    }


void testfw_test_begin( char const* desc, char const* filename, char const* funcname, int line )
    {
    if( testfw_internal_can_not_be_in_test() ) return;

    memset( &testfw_internal_state.current_test, 0, sizeof( testfw_internal_state.current_test ) );    
    testfw_internal_state.current_test.is_active = 1;
    testfw_internal_state.current_test.file = filename;
    testfw_internal_state.current_test.func = funcname;
    testfw_internal_state.current_test.line = line;
    testfw_internal_state.current_test.desc = desc;
    testfw_internal_state.current_test.desc_printed = 0;
    testfw_internal_state.current_test.counted_as_failed = 0;

    ++testfw_internal_state.tests_total;

    #if ( defined( __linux__ ) || defined( __APPLE__ ) ) && !defined( TESTFW_NO_CRASH_HANDLING )
        testfw_internal_install_signal_handlers();
    #endif
    }


void testfw_test_end( char const* filename, char const* funcname, int line )
    {
    (void) filename; (void) funcname; (void) line;
    if( testfw_internal_must_be_in_test() ) return;

    #if ( defined( __linux__ ) || defined( __APPLE__ ) ) && !defined( TESTFW_NO_CRASH_HANDLING )
        testfw_internal_restore_signal_handlers();
    #endif

    memset( &testfw_internal_state.current_test, 0, sizeof( testfw_internal_state.current_test ) );    
    }


#if defined( _WIN32 ) && !defined( TESTFW_NO_CRASH_HANDLING ) && !defined( __TINYC__ ) && !defined( __GNUC__ ) && !defined( __clang__ )
    #pragma warning( push )
    #pragma warning( disable: 4668 )
    #include <windows.h>
    #pragma warning( pop )

    void testfw_exception( unsigned int exception_code )
        {
        if( testfw_internal_must_be_in_test() ) return;

        if( !testfw_internal_state.current_test.counted_as_failed ) 
            {
            testfw_internal_state.current_test.counted_as_failed = 1;
            ++testfw_internal_state.tests_failed;
            }

        char exception_str[ 64 ];
        switch( exception_code ) 
            {
            case EXCEPTION_ACCESS_VIOLATION:
                strcpy( exception_str, "EXCEPTION_ACCESS_VIOLATION" );
                break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
                strcpy( exception_str, "EXCEPTION_DATATYPE_MISALIGNMENT" );
                break;
            case EXCEPTION_BREAKPOINT:
                strcpy( exception_str, "EXCEPTION_BREAKPOINT" );
                break;
            case EXCEPTION_SINGLE_STEP:
                strcpy( exception_str, "EXCEPTION_SINGLE_STEP" );
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                strcpy( exception_str, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED" );
                break;
            case EXCEPTION_FLT_DENORMAL_OPERAND:
                strcpy( exception_str, "EXCEPTION_FLT_DENORMAL_OPERAND" );
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                strcpy( exception_str, "EXCEPTION_FLT_DIVIDE_BY_ZERO" );
                break;
            case EXCEPTION_FLT_INEXACT_RESULT:
                strcpy( exception_str, "EXCEPTION_FLT_INEXACT_RESULT" );
                break;
            case EXCEPTION_FLT_INVALID_OPERATION:
                strcpy( exception_str, "EXCEPTION_FLT_INVALID_OPERATION" );
                break;
            case EXCEPTION_FLT_OVERFLOW:
                strcpy( exception_str, "EXCEPTION_FLT_OVERFLOW" );
                break;
            case EXCEPTION_FLT_STACK_CHECK:
                strcpy( exception_str, "EXCEPTION_FLT_STACK_CHECK" );
                break;
            case EXCEPTION_FLT_UNDERFLOW:
                strcpy( exception_str, "EXCEPTION_FLT_UNDERFLOW" );
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                strcpy( exception_str, "EXCEPTION_INT_DIVIDE_BY_ZERO" );
                break;
            case EXCEPTION_INT_OVERFLOW:
                strcpy( exception_str, "EXCEPTION_INT_OVERFLOW" );
                break;
            case EXCEPTION_PRIV_INSTRUCTION:
                strcpy( exception_str, "EXCEPTION_PRIV_INSTRUCTION" );
                break;
            case EXCEPTION_IN_PAGE_ERROR:
                strcpy( exception_str, "EXCEPTION_IN_PAGE_ERROR" );
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                strcpy( exception_str, "EXCEPTION_ILLEGAL_INSTRUCTION" );
                break;
            case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                strcpy( exception_str, "EXCEPTION_NONCONTINUABLE_EXCEPTION" );
                break;
            case EXCEPTION_STACK_OVERFLOW:
                strcpy( exception_str, "EXCEPTION_STACK_OVERFLOW" );
                break;
            case EXCEPTION_INVALID_DISPOSITION:
                strcpy( exception_str, "EXCEPTION_INVALID_DISPOSITION" );
                break;
            case EXCEPTION_GUARD_PAGE:
                strcpy( exception_str, "EXCEPTION_GUARD_PAGE" );
                break;
            case EXCEPTION_INVALID_HANDLE:
                strcpy( exception_str, "EXCEPTION_INVALID_HANDLE" );
                break;
            default:
                sprintf( exception_str, "%X", exception_code );
            }

        testfw_print_test_desc();
        TESTFW_PRINTF( "\n%s%s(%d): %sFAILED:%s\n", TESTFW_ANSI_LIGHT_GREY, testfw_internal_state.current_test.file, 
            testfw_internal_state.current_test.line, TESTFW_ANSI_LIGHT_RED, TESTFW_ANSI_RESET );
        TESTFW_PRINTF( "\n  %sEXCEPTION( %s%s%s )%s\n", TESTFW_ANSI_CYAN, TESTFW_ANSI_WHITE, exception_str, 
            TESTFW_ANSI_LIGHT_GREY, TESTFW_ANSI_RESET );
        }
#elif ( defined( __linux__ ) || defined( __APPLE__ ) ) && !defined( TESTFW_NO_CRASH_HANDLING )
    void testfw_signal( int signum )
        {
        if( testfw_internal_must_be_in_test() ) return;

        if( !testfw_internal_state.current_test.counted_as_failed ) 
            {
            testfw_internal_state.current_test.counted_as_failed = 1;
            ++testfw_internal_state.tests_failed;
            }

        char signal_str[ 64 ];
        switch( signum ) 
            {
            case SIGSEGV:
                strcpy( signal_str, "SIGSEGV" );
                break;
            case SIGBUS:
                strcpy( signal_str, "SIGBUS" );
                break;
            case SIGFPE:
                strcpy( signal_str, "SIGFPE" );
                break;
            case SIGILL:
                strcpy( signal_str, "SIGILL" );
                break;
            case SIGTRAP:
                strcpy( signal_str, "SIGTRAP" );
                break;
            default:
                sprintf( signal_str, "%d", signum );
            }

        testfw_print_test_desc();
        TESTFW_PRINTF( "\n%s%s(%d): %sFAILED:%s\n", TESTFW_ANSI_LIGHT_GREY, testfw_internal_state.current_test.file, 
            testfw_internal_state.current_test.line, TESTFW_ANSI_LIGHT_RED, TESTFW_ANSI_RESET );
        TESTFW_PRINTF( "\n  %sSIGNAL( %s%s%s )%s\n", TESTFW_ANSI_CYAN, TESTFW_ANSI_WHITE, signal_str, 
            TESTFW_ANSI_LIGHT_GREY, TESTFW_ANSI_RESET );
        }
#endif

void testfw_current_test_assertion_failed( void )
    {
    if( testfw_internal_must_be_in_test() ) return;

    ++testfw_internal_state.assertions_failed;

    if( !testfw_internal_state.current_test.counted_as_failed ) 
        {
        testfw_internal_state.current_test.counted_as_failed = 1;
        ++testfw_internal_state.tests_failed;
        }
    }


void testfw_assertion_count_inc( void )
    {
    if( testfw_internal_must_be_in_test() ) return;

    ++testfw_internal_state.assertions_total;
    }


void testfw_print_failure( char const* filename, int line )
    {
    TESTFW_PRINTF( "\n%s%s(%d): %sFAILED:%s\n", TESTFW_ANSI_LIGHT_GREY, filename, line, TESTFW_ANSI_LIGHT_RED, 
        TESTFW_ANSI_RESET );
    }


void testfw_expected( int expression, char const* expression_str, char const* filename, 
    char const* funcname, int line )
    {
    (void) funcname;
    if( testfw_internal_must_be_in_test() ) return;

    testfw_assertion_count_inc();

    if( !expression ) 
        { 
        testfw_current_test_assertion_failed();
        testfw_print_test_desc();
        testfw_print_failure( filename, line );
        TESTFW_PRINTF( "\n  %sTESTFW_EXPECTED( %s%s%s )%s\n", TESTFW_ANSI_LIGHT_MAGENTA, TESTFW_ANSI_WHITE, expression_str, 
            TESTFW_ANSI_LIGHT_MAGENTA, TESTFW_ANSI_RESET );
        }
    }

#endif /* TESTFW_IMPLEMENTATION */

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
