//
// compression: set NTFS attr
//
/*
 * Usage: compression file
 * ToDo:
 *   ReadOnly
 */
#define STRICT
#define UNICODE
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <winioctl.h>

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
BOOL setComp( LPCTSTR file ) {

  // MESSAGE( TEXT( "In: setComp: %s" ), file );

  HANDLE h;
  h = CreateFile(
		 file,                         // ファイル名
		 GENERIC_READ | GENERIC_WRITE, // アクセスモード
		 0,                            // 共有モード
		 NULL,                         // セキュリティ記述子
		 OPEN_EXISTING,                // 作成方法
		 FILE_ATTRIBUTE_NORMAL,        // ファイル属性
		 NULL                          // テンプレートファイル
		 );
  if ( INVALID_HANDLE_VALUE == h ) {
    ABORT( TEXT( "ERROR: CreateFile" ) );
  }

  BOOL b;
  USHORT InBuffer = COMPRESSION_FORMAT_DEFAULT;
  // COMPRESSION_FORMAT_DEFAULT
  //   Compress the file or directory, using the default compression format.
  // COMPRESSION_FORMAT_LZNT1
  //  Compress the file or directory, using the LZNT1 compression format.
  //COMPRESSION_FORMAT_NONE
  //  Decompress the file or directory.
  // OVERLAPPED oberlaped;
  DWORD bytes;
  b = DeviceIoControl(
		      h,  // [in] Handle
		      FSCTL_SET_COMPRESSION, // [in] 実行する動作の制御コード
		      &InBuffer,      // [in] 入力データ
		      sizeof( InBuffer ), // [in]  入力バッファサイズ
		      NULL,      // 出力バッファ: Not used. Set to NULL.
		      0,        // サイズ: Not used. Set to NULL.
		      &bytes,   // [out] バイト数を受け取る変数
		      NULL // 非同期動作を表す構造体: Not used
		      );
  if ( 0 == b ) {
    ABORT( TEXT( "ERROR: DeviceIoControl" ) );
  }
  if ( 0 == CloseHandle( h ) ) {
    ABORT( TEXT( "ERROR: CloseHandle()" ) );
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
  // MESSAGE( TEXT( "Args: %d: %s" ), argc, argv[1] );

  if ( 2 != argc ) {
    MESSAGE( TEXT( "ERROR: need target name" ) );
    ExitProcess( EXIT_FAILURE );
  }

  setComp( argv[1] );
  return EXIT_SUCCESS;
}
