#define STRICT
#define UNICODE
#include <windows.h>


int WINAPI WinMain(
		   HINSTANCE hInstance
		   , HINSTANCE hPrevInstance
		   , LPSTR lpCmdLine
		   , int nShowCmd
) {
  SHFILEOPSTRUCT op;
  int argc;
  LPTSTR * argv;
  LPTSTR p, buf;
  int i, ret;

  argv = CommandLineToArgvW( GetCommandLine(), &argc );
  if ( NULL == argv ) {
    MessageBox( NULL, TEXT( "commandline" ), TEXT( "error" ), MB_OK );
    return !0;
  }
  buf = VirtualAlloc(
		     NULL /* use strlen(), not lstrlen() */
		     , sizeof( TCHAR ) * (strlen( lpCmdLine ) + 2)
		     , MEM_COMMIT
		     , PAGE_READWRITE
		     );
  if ( NULL == buf ) {
    MessageBox( NULL, TEXT( "malloc" ), TEXT( "error" ), MB_OK );
    return !0;
  }

  for ( p = buf, i = 1; i < argc; ++i ) {
    lstrcpy( p, argv[i] );
    p += lstrlen( argv[i] );
    *p++ = TEXT( '\0' );
  }
  *p = TEXT( '\0' );

  ZeroMemory( &op, sizeof( op ) );
  op.hwnd = NULL;
  op.wFunc= FO_DELETE;
  op.pFrom= buf;
  op.fFlags= FOF_ALLOWUNDO;
  ret = SHFileOperation( &op );

  VirtualFree( buf, 0, MEM_RELEASE );
  LocalFree( LocalHandle( argv ) );

  return ret;
}
