//
// make hard-link
//
/*
 * Usage: hardlink source destination
 * ToDo:
 *   -f: force create hard-link
 */
#define STRICT
#define UNICODE
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#include <windows.h>

// element number
#define numberof( a ) (sizeof( a ) / sizeof( a[0] ) )


//
// -- Put Info & Abort  --------------------------------------------------
//
void REPORT( LPCTSTR msg ) {
  LPVOID lpMsgBuf;
  DWORD r;
  r = FormatMessage(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER |
		    FORMAT_MESSAGE_FROM_SYSTEM |
		    FORMAT_MESSAGE_IGNORE_INSERTS,
		    NULL,
		    GetLastError(),
		    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		    (LPTSTR)&lpMsgBuf,
		    0,
		    NULL
		    );
  if ( 0 == r ) {
    MessageBox( NULL,
		TEXT( "Fail: get & format Info: GetLastError()" ),
		msg,
		MB_OK | MB_ICONINFORMATION
		);
  }
  MessageBox( NULL, (LPCTSTR)lpMsgBuf, msg, MB_OK | MB_ICONINFORMATION );
  LocalFree( lpMsgBuf );
}
void ABORT( LPCTSTR msg ) {
  REPORT( msg );
  ExitProcess( EXIT_FAILURE );
}
void MESSAGE( LPCTSTR format, ... ) {
  TCHAR buf[1024];
  va_list args;
  va_start( args, format );
  wvsprintf( buf, format, args );
  MessageBox( NULL, buf, TEXT( "message" ), MB_OK | MB_ICONINFORMATION );
  va_end( args );
}


// -- create link
BOOL CreateLink( LPCTSTR srcfile, LPCTSTR dstfile ) {
    if ( 0 == CreateHardLink( dstfile, srcfile, NULL ) ) {
        TCHAR msg[ MAX_PATH ];
        wsprintf( msg,
		  TEXT( "ERROR: CreateHardLink(): %s --> %s" ),
		  srcfile, dstfile
		  );
	ABORT( msg );
    }
    return (!0);
}


//
// -- main -------------------------------------------------------
//
int WINAPI WinMain(
		   HINSTANCE hInstance,
		   HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine,
		   int nCmdShow
		   ) {
    UNREFERENCED_PARAMETER( hInstance );
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    UNREFERENCED_PARAMETER( nCmdShow );

    int argc;
    LPWSTR * argv;
    argv = CommandLineToArgvW(
			      GetCommandLine(),
			      &argc
			      );
    // MESSAGE( TEXT( "Args: %d: %s  -->  %s" ), argc, argv[1], argv[2] );

    if ( 3 != argc ) {
        MESSAGE( TEXT( "ERROR: use only 2 argument" ) );
	ExitProcess( EXIT_FAILURE );
    }
    CreateLink( argv[1], argv[2] );
    return EXIT_SUCCESS;
}
