/*
 * Desktop Menu 用にキャッシュ読み込みさせる
 */
#define STRICT
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shlobj.h>


// -- REPORT ---------------------------------------------------------
void REPORT( const TCHAR * format, ... ) {
  TCHAR buf[1024];
  va_list args;
  va_start( args, format );
  wvsprintf( buf, format, args );
  va_end( args );
  MessageBox( NULL, buf, TEXT( "message" ), MB_OK | MB_ICONINFORMATION );
}
void ABORT( const TCHAR * format, ... ) {
  TCHAR buf[1024];
  va_list args;
  va_start( args, format );
  wvsprintf( buf, format, args );
  va_end( args );
  MessageBox( NULL, buf, TEXT( "message" ), MB_OK | MB_ICONINFORMATION );
  exit( EXIT_FAILURE );
}


// -- get desktop directory  -----------------------------------------
BOOL getDesktopDir( HWND hwnd, TCHAR path[MAX_PATH] ) {
  BOOL rc = TRUE;
  HRESULT hres;

  IMalloc * ima;
  hres = SHGetMalloc( &ima );
  if ( FAILED( hres ) ) {
    REPORT( TEXT( "SHGetMalloc" ) );
    return FALSE;
  }

  LPITEMIDLIST pidl;
  hres = SHGetSpecialFolderLocation(
				    hwnd
				    , CSIDL_DESKTOPDIRECTORY
				    , &pidl
				    );
  if( SUCCEEDED( hres ) ) {
    if ( ! SHGetPathFromIDList( pidl, path ) ) {
      REPORT( TEXT( "SHGetPathFromIDList" ) );
      rc = FALSE;
    }
  }
  ima->Free( pidl );
  ima->Release();
  return rc;
}


// -- Search Items --------------------------------------------------
BOOL findItems( HINSTANCE hInstance, TCHAR dir[MAX_PATH] ) {

  // file  mask
  TCHAR path[MAX_PATH];
  lstrcpy( path, dir );
  lstrcat( path, TEXT( "\\*.*" ) );

  HANDLE find;
  WIN32_FIND_DATA fdata;
  BOOL findnext;
  find = FindFirstFile( path, &fdata );
  if ( INVALID_HANDLE_VALUE == find ) {
    REPORT( TEXT( "FindFirstFile: directory: %s" ), path );
    return FALSE;
  }
  do {
    if (
	fdata.dwFileAttributes
	& (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) ) {
      // no action
    } else if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
      // directory. except ., ..
      if ( 0 != lstrcmp( TEXT( "." ), fdata.cFileName )
	   && 0 != lstrcmp( TEXT( ".." ), fdata.cFileName )
	   ) {
	TCHAR dirname[MAX_PATH];
	lstrcpy( dirname, dir );
	lstrcat( dirname, TEXT( "\\" ) );
	lstrcat( dirname, fdata.cFileName );
	if ( ! findItems( hInstance, dirname ) ) {
	  REPORT( TEXT( "Error: findItems(): '%s'"), dirname );
	  goto error_exit;
	}
      }
    } else {
      // normal file
      HICON hicon;
      WORD iicon;
      hicon = ExtractAssociatedIcon(
				    hInstance,
				    fdata.cFileName,
				    &iicon
				    );
      if ( NULL == hicon ) {
	REPORT( TEXT( "ERROR: ExtractAssociatedIcon()" ));
	goto error_exit;
      }
    }

    findnext = FindNextFile( find, &fdata );
    if ( ERROR_NO_MORE_FILES == findnext ) {
      break;
    }
  } while ( 0 != findnext );
  FindClose( find );
  return TRUE;

 error_exit:
  FindClose( find );
  return FALSE;
}

// -- main ----------------------------------------------
int WINAPI WinMain(
		   HINSTANCE hInstance,
		   HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine,
		   int nCmdShow
) {

  // リスト要素の作成
  TCHAR desktop[MAX_PATH];
  if ( ! getDesktopDir( NULL, desktop ) ) {
    REPORT( TEXT( "ERROR: getDesktopDir()" ));
    return !0;
  }
  // REPORT( TEXT( "desktop: '%s'" ), desktop );

  if ( ! findItems( hInstance, desktop) ) {
    REPORT( TEXT( "ERROR: findItems()" ));
    return EXIT_FAILURE;
  }

  // REPORT( TEXT( "done: '%s'" ), desktop );

  return EXIT_SUCCESS;
}
