//
// pdumpfs like backup
//
/* Usage: bup /home/yourname /backup
 *   --> /backup/2001/02/19/yourname/..
 *   --> /backup/YYYY/MM/DD/hh/mm/yourname/..
 */
/* ToDo:
 * 統計情報とか
 */
/* Memo:
 * エラー処理：API が失敗したら、即、ABORT() し、REPORT() させる
 */
#define STRICT
#define UNICODE
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <winioctl.h>

// 処理したファイル数のカウント
int NumOfCopy = 0;
int NumOfLink = 0;

// バッファサイズ
#define MAX_BUFSIZ_ERR (2049) // エラー時のメッセージ用


// element number
#define numberof( a ) (sizeof( a ) / sizeof( a[0] ) )

// make path
const LPCTSTR PATHDELIM = TEXT( "\\" );
const LPCTSTR URN = TEXT( "\\\\?\\" );






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
  MessageBox( NULL,
	      (LPCTSTR)lpMsgBuf, TEXT( "GetLastError" ),
	      MB_OK | MB_ICONINFORMATION
	     );
  MessageBox( NULL, msg, TEXT( "Message" ), MB_OK | MB_ICONINFORMATION );
  LocalFree( lpMsgBuf );
}
void ABORT( LPCTSTR msg ) {
  REPORT( msg );
  ExitProcess( EXIT_FAILURE );
}
void MESSAGE( LPCTSTR format, ... ) {
  TCHAR buf[ MAX_BUFSIZ_ERR ];
  va_list args;
  va_start( args, format );
  wvsprintf( buf, format, args );
  MessageBox( NULL, buf, TEXT( "message" ), MB_OK | MB_ICONINFORMATION );
  va_end( args );
}


//
// -- need update ?  -----------------------------------------------------
// check: size, mtime (nocheck: ctime, atime, etc)
//
BOOL isNeedUpdate( LPCTSTR srcfile, LPCTSTR lastfile ) {
  BOOL updateflag = 0;
  WIN32_FIND_DATA stFindDataLast;
  HANDLE handleLast;
  if ( INVALID_HANDLE_VALUE ==
       (handleLast = FindFirstFile( lastfile, &stFindDataLast )) ) {
    // MESSAGE( TEXT( "Update: It's New !!!" ) );
    return (!updateflag);    // lastfile not exist, internal error ?
  }
  WIN32_FIND_DATA stFindDataSrc;
  HANDLE handleSrc;
  if ( INVALID_HANDLE_VALUE ==
       (handleSrc = FindFirstFile( srcfile, &stFindDataSrc )) ) {
    ABORT( TEXT( "ERROR: FindFirstFile(): srctfile" ) );
  }


  // size
  if ( stFindDataSrc.nFileSizeLow != stFindDataLast.nFileSizeLow ) {
    // MESSAGE( TEXT( "Update: size: Low" ) );
    updateflag = !updateflag;
    goto eof;
  }
  if ( stFindDataSrc.nFileSizeHigh != stFindDataLast.nFileSizeHigh ) {
    // MESSAGE( TEXT( "Update: size: High" ) );
    updateflag = !updateflag;
    goto eof;
  }

  // mtime
  if ( 0 != CompareFileTime(
			    &(stFindDataSrc.ftLastWriteTime),
			    &(stFindDataLast.ftLastWriteTime) ) ) {
    // MESSAGE( TEXT( "Update: mtime" ) );
    updateflag = !updateflag;
    goto eof;
  }

  // attr: FILE_ATTRIBUTE_SPARSE_FILE が再現できていないので単純比較はできない
  if (
      ((FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN |
       FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE |
       FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_COMPRESSED)
       & stFindDataSrc.dwFileAttributes)
      !=
      ((FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN |
       FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE |
       FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_COMPRESSED)
       & stFindDataLast.dwFileAttributes)
       ) {
    /*
    MESSAGE( TEXT( "attr: %X <--> %X \n %s --> %s" ),
	     stFindDataSrc.dwFileAttributes,
	     stFindDataLast.dwFileAttributes,
	     srcfile, lastfile
	     );
    */
    updateflag = !updateflag;
    goto eof;
  }


 eof:
  if ( 0 == FindClose( handleSrc ) ) {
    FindClose( handleLast );
    ABORT( TEXT( "ERROR: FindClose(): srcfile" ) );
  }
  if ( 0 == FindClose( handleLast ) ) {
    ABORT( TEXT( "ERROR: FindClose(): lastfile" ) );
  }
  return updateflag;
}


//
// -- util ---------------------------------------------------------------
//

// make path
LPTSTR BuildPath( LPTSTR buf, LPCTSTR path, LPCTSTR base ) {
  wsprintf( buf, TEXT( "%s%s%s" ), path, PATHDELIM, base );
  return buf;
}

// make URN path
LPTSTR BuildURNPath( LPTSTR buf, LPCTSTR path, LPCTSTR base ) {
  wsprintf( buf, TEXT( "%s%s%s%s" ), URN, path, PATHDELIM, base );
  return buf;
}
LPTSTR BuildURNPath( LPTSTR buf, LPCTSTR path ) {
  wsprintf( buf, TEXT( "%s%s" ), URN, path );
  return buf;
}

// byte size for URN path
int getURNlen( LPCTSTR path ) {
  return
    sizeof( TCHAR ) * (
		       lstrlen( URN ) +
		       lstrlen( PATHDELIM ) +
		       lstrlen( path ) +
		       2 // sizeof TEXT( '\0' )
		       );
}


// isExist, RTN Ver.
BOOL isExistForDirectory( LPCTSTR dirname ) {
  DWORD attr;
  LPTSTR ptr;
  BOOL ret = TRUE;

  ptr = (LPTSTR)VirtualAlloc(
			     NULL,
			     getURNlen( dirname ),
			     MEM_COMMIT,
			     PAGE_READWRITE
			     );
  if ( NULL == ptr ) {
    ABORT( TEXT( "ERROR: VirtualAlloc()") );
  }

  ptr = BuildURNPath( ptr, dirname );
  attr = GetFileAttributes( ptr );
  if ( (DWORD)-1 == attr ) {
    ret = FALSE;
  }
  if ( 0 == (attr & FILE_ATTRIBUTE_DIRECTORY) ) {
    ret = FALSE;
  }

  if ( 0 == VirtualFree( ptr, 0, MEM_RELEASE ) ) {
    ABORT( TEXT( "ERROR: VirtualFree()") );
  }
  return ret;
}



// search baseneme: in path string
LPTSTR findBasename( LPCTSTR path, LPTSTR * baseptr ) {
  TCHAR buf[ MAX_PATH ];
  LPTSTR bufptr;
  DWORD r = GetFullPathName( path, numberof( buf ), buf, &bufptr );
  if ( 0 == r ) {
    ABORT( TEXT( "ERROR: GetFullPathName()" ) );
  }
  if ( numberof( buf ) <= r ) {  // バッファが必要なサイズより小さかった場合
    // do not retry
    ABORT( TEXT( "ERROR: GetFullPathName(): Need more buffer" ) );
  }
  if ( NULL != baseptr ) {
    *baseptr = bufptr;
  }
  return bufptr;
}


//  memblock: ディレクトリパス保持用。MAX_PATH 超え対応
// VirtualAlloc() でコミット～デコミット～コミットで 0 メモリがクリアされる
// クリアしないでデコミット～コミットする方法が見つからない
// GlobalAlloc() は推奨されていないっほい
// LocalAlloc() は 16 ビットアプリとの互換用
// HeapAlloc() は移動可能でないって事は malloc() と同じ
// 取りあえず VirtualAlloc() で常にコミット状態として処理を進める。
// つまり、unlock() は飾り
class memblock {
public:
  memblock();
  memblock( SIZE_T size );
  memblock( memblock org, LPCTSTR path );
  VOID alloc( SIZE_T size );
  LPTSTR lock( void );
  VOID unlock( void );
  VOID release( void );
  BOOL isOk( void );
  SIZE_T getSize( void );
  SIZE_T getTSize( void );
private:
  LPVOID baseaddress;
  SIZE_T areasize;
};
memblock::memblock() {
  baseaddress = NULL;
  areasize = 0;
}
memblock::memblock( SIZE_T size ) {
  baseaddress = NULL;
  areasize = 0;
  this->alloc( size );
}
memblock::memblock( memblock org, LPCTSTR path ) {
  baseaddress = NULL;
  areasize = 0;
  this->alloc( org.getSize() + getURNlen( path ) );
  BuildPath( this->lock(), org.lock(), path );
  org.unlock();
  this->unlock();
}
VOID memblock::alloc( SIZE_T size ) {
  if ( isOk() ) {
    release();
  }
  if ( NULL == (baseaddress = VirtualAlloc(
					   NULL,
					   size,
					   MEM_RESERVE,
					   PAGE_READWRITE
					   )) ) {
    ABORT( TEXT( "ERROR: VirtualAlloc(): MEM_RESERVE") );
  }
  areasize = size;
}
LPTSTR memblock::lock( void ) {
  LPTSTR lp;
  if ( NULL == (lp = (LPTSTR)VirtualAlloc(
					  baseaddress,
					  areasize,
					  MEM_COMMIT,
					  PAGE_READWRITE
					  )) ) {
    ABORT( TEXT( "ERROR: VirtualAlloc(): MEM_COMMIT") );
  }
  return lp;
}
VOID memblock::unlock( void ) {
  // stub
  // コミット解除し、かつ、メモリの内容を保持する方法が不明
  // なので、コミットしっ放し
}
VOID memblock::release( void ) {
  if ( 0 == VirtualFree( baseaddress, 0, MEM_RELEASE ) ) {
    ABORT( TEXT( "ERROR: VirtualFree(): RELEASE") );
  }
  baseaddress = NULL;
  areasize = 0;
}
BOOL memblock::isOk( void ) {
  return (0 != areasize);
}
SIZE_T memblock::getSize( void ) {
  return areasize;
}
SIZE_T memblock::getTSize( void ) {
  return (areasize / sizeof( TCHAR ));
}

// add path: with realloc memory
LPTSTR AddPath( memblock & mem, LPCTSTR base ) {
  memblock temp( mem.getSize() + getURNlen( base ) );
  LPTSTR tmpptr = temp.lock();
  LPTSTR path = mem.lock();
  BuildPath( tmpptr, path, base );
  mem.alloc( getURNlen( tmpptr ) );
  path = mem.lock();
  if ( NULL == lstrcpyn( path, tmpptr, mem.getTSize() ) ) {
    ABORT( TEXT( "ERROR: lstrcpyn(): AddPath" ) );
  }
  temp.release();
  return path;
}








//
// set [c|a|m]date
// readonly file: 一旦、READONLY フラグを落とす
VOID setFileTime( WIN32_FIND_DATA * find_data_ptr, LPCTSTR filename ) {
  DWORD accessmode = find_data_ptr->dwFileAttributes;
  if ( FILE_ATTRIBUTE_READONLY | accessmode ) {
    accessmode &= (~FILE_ATTRIBUTE_READONLY);
    if ( 0 == SetFileAttributes( filename, accessmode ) ) {
      TCHAR msg[ MAX_BUFSIZ_ERR ];
      wsprintf( msg, TEXT( "ERROR: SetFileAttributes(): %s" ), filename );
      ABORT( msg );
    }
  }
  if ( FILE_ATTRIBUTE_DIRECTORY | accessmode ) {
    accessmode |= FILE_FLAG_BACKUP_SEMANTICS;
  }
  // do not wait offline media. ex. FD, MD, CD
  UINT prevmode = SetErrorMode( SEM_FAILCRITICALERRORS );
  HANDLE f = CreateFile(
			filename,
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			accessmode,
			NULL
			);
  SetErrorMode( prevmode );
  // 他のプロセスでオープン中だと失敗するので、その時はスキップ
  if ( INVALID_HANDLE_VALUE != f ) {
    if ( 0 == SetFileTime(
			  f,
			  &(find_data_ptr->ftCreationTime),
			  &(find_data_ptr->ftLastAccessTime),
			  &(find_data_ptr->ftLastWriteTime)
			  ) ) {
      TCHAR msg[ MAX_BUFSIZ_ERR ];
      wsprintf( msg, TEXT( "ERROR: SetFileTime(): %s" ), filename );
      ABORT( msg );
    }
    if ( 0 == CloseHandle( f ) ) {
      ABORT( TEXT( "ERROR: CloseHandle()" ) );
    }
  }

  if ( FILE_ATTRIBUTE_READONLY | (find_data_ptr->dwFileAttributes) ) {
    if ( 0 == SetFileAttributes(
				filename,
				find_data_ptr->dwFileAttributes
				) ) {
      TCHAR msg[ MAX_BUFSIZ_ERR ];
      wsprintf( msg, TEXT( "ERROR: Re: SetFileAttributes(): %s" ), filename );
      ABORT( msg );
    }
  }
}


//
// -- create link  -------------------------------------------------------
//
VOID CreateLink( LPCTSTR srcfile, LPCTSTR dstfile ) {
  if ( 0 == CreateHardLink( dstfile, srcfile, NULL ) ) {
    TCHAR msg[ MAX_BUFSIZ_ERR ];
    wsprintf( msg,
	      TEXT( "ERROR: CreateHardLink(): %s --> %s" ),
	      srcfile, dstfile
	      );
    ABORT( msg );
  }
}


//
// -- procedures  --------------------------------------------------------
//
//
// serarch latest backup
// not return if error
// 最新の時刻ディレクトリに basename ディレクトリがあるかどうか
// 最新でない時刻ディレクトリに有っても見つからない
// つまり、併用するなら、常に同時実行が必要
//
// URN: latest 用にメモリ領域を確保し、内容をセットし、コミット解除して返す
//      エラー時はメモリ解放し、 FALSE を返す
//      注：memblock は参照渡し
BOOL getLatest( memblock & srcdir,memblock & dstdir, memblock & latest ) {

  // YYYY/MM/DD/hh/mm
  static LPCTSTR FMT[] = {
    TEXT( "????" ), TEXT( "??" ), TEXT( "??" ), TEXT( "??" ), TEXT( "??" )
  };

  // env
  const LPCTSTR BASE_INIT = TEXT( "0" );  // init value: less than "00"
  BOOL retval = FALSE;

  LPTSTR dst = dstdir.lock();
  latest.alloc( getURNlen( dst ) );
  LPTSTR path = latest.lock();
  if ( NULL == lstrcpyn( path, dst, latest.getTSize() ) ) {
    ABORT( TEXT( "ERROR: lstrcpyn(): latest" ) );
  }
  dstdir.unlock();

  unsigned i;
  WIN32_FIND_DATA find_data;
  HANDLE h;
  for ( i =  0; i < numberof( FMT ); ++i ) {
    TCHAR basename[ MAX_PATH ];
    memblock mask;

    mask.alloc( getURNlen( path ) + getURNlen( FMT[ i ] ) );
    LPTSTR pathmask = mask.lock();
    BuildPath( pathmask, path, FMT[ i ] );
    h = FindFirstFile( pathmask, &find_data );
    if ( INVALID_HANDLE_VALUE == h ) {
      ABORT( TEXT( "ERROR: FindFirstFile(): pathmask" ) );
    }
    mask.unlock();
    if ( NULL == lstrcpyn( basename, BASE_INIT, numberof( basename ) ) ) {
      FindClose( h );
      ABORT( TEXT( "ERROR: lstrcpyn(): basename" ) );
    }

    do {
      TCHAR findbase[ MAX_PATH ];
      if ( NULL == lstrcpyn(
			    findbase,
			    find_data.cFileName,
			    numberof( findbase )
			    ) ) {
	FindClose( h );
	ABORT( TEXT( "ERROR: lstrcpyn(): findbase" ) );
      }
      // check: length
      int len = lstrlen( findbase );
      if ( len != lstrlen( FMT[ i ] ) ) {
	continue;
      }

      // check: type
      BOOL isTarget = TRUE;
      for ( int p = 0; p < len; ++p ) {
	TCHAR ch = findbase[ p ];
	// Numeric only
	if ( ( ! IsCharAlphaNumeric( ch  ) ) || (IsCharAlpha( ch  )) ) {
	  isTarget = FALSE;
	  break;
	}
      }
      if ( ! isTarget ) {
	continue;
      }

      // compair: newer (=more big)
      if ( 0 > lstrcmp( basename, findbase ) ) {
	if ( NULL == lstrcpyn( basename, findbase, numberof( basename ) ) ) {
	  FindClose( h );
	  ABORT( TEXT( "ERROR: lstrcpyn(): New One" ) );
	}
      }
    } while ( FindNextFile( h, &find_data ) );
    if ( ERROR_NO_MORE_FILES != GetLastError() ) {
      ABORT( TEXT( "ERROR: FindNextFile()" ) );
    }
    if ( 0 == FindClose( h ) ) {
      ABORT( TEXT( "ERROR: FindClose()" ) );
    }
    mask.release();

    // found ?
    if ( 0 == lstrcmp( basename, BASE_INIT ) ) {
      goto eof;
    }

    // for next search
    path = AddPath( latest, basename );
  }

  // check exist: basedir in date dir
  LPTSTR bufptr;
  findBasename( srcdir.lock(), &bufptr );
  path = AddPath( latest, bufptr );
  srcdir.unlock();
  h = FindFirstFile( path, &find_data );
  if ( INVALID_HANDLE_VALUE == h ) {
    goto eof;
  }
  if ( 0 == FindClose( h ) ) {
    ABORT( TEXT( "ERROR: base: FindClose()" ) );
  }

  retval = TRUE;

 eof:
  latest.unlock();
  return retval;
}


//
// make new directory
//
VOID makeDestination( memblock & srcdir, memblock & dstdir, memblock & newdir
		      ) {
  // 現在時刻の取得
  SYSTEMTIME stSystemTime;
  GetLocalTime( &stSystemTime );

  // yyyy/mm/dd/HH/MM
  const LPCTSTR DIR_FMT_4 = TEXT( "%04d" );
  const LPCTSTR DIR_FMT_2 = TEXT( "%02d" );
  static LPCTSTR DIR_FMT[] = {
    DIR_FMT_4, DIR_FMT_2, DIR_FMT_2, DIR_FMT_2, DIR_FMT_2,
  };
  int dirvalue[ numberof( DIR_FMT ) ];
  dirvalue[ 0 ] = stSystemTime.wYear;
  dirvalue[ 1 ] = stSystemTime.wMonth;
  dirvalue[ 2 ] = stSystemTime.wDay;
  dirvalue[ 3 ] = stSystemTime.wHour;
  dirvalue[ 4 ] = stSystemTime.wMinute;

  LPCTSTR dst = dstdir.lock();
  newdir.alloc( getURNlen( dst ) );
  LPTSTR path = newdir.lock();
  if ( NULL == lstrcpyn( path, dst, dstdir.getTSize() ) ) {
    ABORT( TEXT( "ERROR: lstrcpyn(): path" ) );
  }
  dstdir.unlock();
  for ( unsigned i = 0; i < numberof( DIR_FMT ); ++i ) {
    TCHAR dirname[ MAX_PATH ]; // dirname only
    wsprintf( dirname, DIR_FMT[ i ], dirvalue[ i ] );
    memblock work;
    work.alloc( newdir.getSize() + getURNlen( dirname ) );
    LPTSTR workbuf = work.lock();
    LPTSTR base;
    DWORD r = SearchPath(
			 path, dirname, NULL, work.getTSize(), workbuf, &base
			 );
    work.release();
    path = AddPath( newdir, dirname );
    if ( 0 == r ) {
      if ( 0 == CreateDirectory( path, NULL ) ) {
	ABORT( TEXT( "ERROR: CreateDirectory()" ) );
      }
    }
  }

  LPTSTR bufptr;
  findBasename( srcdir.lock(), &bufptr );
  path = AddPath( newdir, bufptr );
  srcdir.unlock();
  if ( 0 == CreateDirectory( path, NULL ) ) {
    ABORT( TEXT( "ERROR: CreateDirectory(): base" ) );
  }
  newdir.unlock();
}



//
// -- backup: entry
//
VOID doBackup( memblock & srcdir, memblock & dstdir, memblock & latest ) {

  WIN32_FIND_DATA find_data;
  HANDLE h;
  memblock maskbuf( srcdir, TEXT( "*.*" ) );
  LPTSTR pathmask = maskbuf.lock();
  if ( INVALID_HANDLE_VALUE == (h = FindFirstFile( pathmask, &find_data )) ) {
    ABORT( TEXT( "ERROR: doBackup: FindFirstFile()" ) );
  }
  maskbuf.unlock();
  maskbuf.release();

  do {
    if ( 0 == lstrcmp( find_data.cFileName, TEXT( "." ) ) ) {
      continue;
    }
    if ( 0 == lstrcmp( find_data.cFileName, TEXT( ".." ) ) ) {
      continue;
    }

    memblock srcbuf( srcdir, find_data.cFileName );
    memblock dstbuf( dstdir, find_data.cFileName );
    memblock lastbuf( latest, find_data.cFileName );
    LPTSTR src = srcbuf.lock();
    LPTSTR dst = dstbuf.lock();
    LPTSTR last = lastbuf.lock();


    // directory ro file
    if ( FILE_ATTRIBUTE_DIRECTORY & find_data.dwFileAttributes ) {
      if ( 0 == CreateDirectory( dst, NULL ) ) {
	ABORT( TEXT( "ERROR: CreateDirectory(): dst" ) );
      }

      // recursion
      srcbuf.unlock();
      dstbuf.unlock();
      lastbuf.unlock();
      doBackup( srcbuf, dstbuf, lastbuf );
      // set [c|a|m]date: dir
      // 配下のファイル操作を行った時点で更新されるはずなので、最後に実行
      setFileTime( &find_data, dstbuf.lock() );
      dstbuf.unlock();
    } else {
      BOOL bFailIfExists= TRUE;
      if ( isNeedUpdate( src, last ) ) {
	// 圧縮属性のファイルを先行作成。無用な圧縮＆伸長をしない様に
	DWORD attr = GetFileAttributes( src );
	if ( (DWORD)-1 == attr ) {
	  ABORT( TEXT( "ERROR: GetFileAttributes(): src" ) );
	}
	if ( FILE_ATTRIBUTE_COMPRESSED & attr ) {
	  HANDLE h = CreateFile(
				dst,
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL,
				NULL
				);

	  if ( INVALID_HANDLE_VALUE == h ) {
	    ABORT( TEXT( "ERROR: CreateFile(): dst" ) );
	  }
	  USHORT InBuffer = COMPRESSION_FORMAT_DEFAULT;
	  DWORD bytes;
	  BOOL b = DeviceIoControl(
				   h,
				   FSCTL_SET_COMPRESSION,
				   &InBuffer,
				   sizeof( InBuffer ),
				   NULL,
				   0,
				   &bytes,
				   NULL
				   );
	  if ( 0 == b ) {
	    ABORT( TEXT( "ERROR: DeviceIoControl" ) );
	  }
	  if ( 0 == CloseHandle( h ) ) {
	    ABORT( TEXT( "ERROR: CloseHandle()" ) );
	  }
	  bFailIfExists = FALSE;
	}
	if ( 0 == CopyFile( src, dst, bFailIfExists ) ) {
	  TCHAR msg[ MAX_BUFSIZ_ERR ];
	  wsprintf( msg, TEXT( "ERROR: CopyFile(): %s --> %s" ), src, dst );
	  ABORT( msg );
	}
	// set [c|a|m]date: dst
	setFileTime( &find_data, dst) ;
	++NumOfCopy;
	// set [c|a|m]date: src
	setFileTime( &find_data, src );
      } else {
	CreateLink( last, dst );
	// set [c|a|m]date: link
	setFileTime( &find_data, dst );
	++NumOfLink;
      }
      srcbuf.unlock();
      dstbuf.unlock();
      lastbuf.unlock();
    }
    srcbuf.release();
    dstbuf.release();
    lastbuf.release();
  } while ( FindNextFile( h, &find_data ) );

  if ( 0 == FindClose( h ) ) {
    ABORT( TEXT( "ERROR: doBackup: FindClose()" ) );
  }
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
    MESSAGE( TEXT( "ERROR: need argument: src & dst" ) );
    ExitProcess( EXIT_FAILURE );
  }

  LPCTSTR src = argv[1];
  LPCTSTR dst = argv[2];
  if ( ! isExistForDirectory( src ) ) {
    MESSAGE( TEXT( "ERROR: Source Directory" ) );
    ExitProcess( EXIT_FAILURE );
  }
  if ( ! isExistForDirectory( dst ) ) {
    MESSAGE( TEXT( "ERROR: Destination Directory" ) );
    ExitProcess( EXIT_FAILURE );
  }


  // alloc for src/dst
  memblock srcdir( getURNlen( src ) );
  BuildURNPath( srcdir.lock(), src );
  srcdir.unlock();
  memblock dstdir( getURNlen( dst ) );
  BuildURNPath( dstdir.lock(), dst );
  dstdir.unlock();


  // search latest
  memblock latest;
  BOOL r = getLatest( srcdir, dstdir, latest );
  if ( FALSE == r ) {
    // Mark: if no latest backup
    latest.alloc( getURNlen( dstdir.lock() ) );
    dstdir.unlock();
    LPTSTR lp = latest.lock();
    lp[0] = TEXT( '\0' );
    latest.unlock();
  }

  // make destination directory from Localtime
  memblock newdir;
  makeDestination( srcdir, dstdir, newdir );
  dstdir.release();


  // if no latest backup
  LPCTSTR last = latest.lock();
  if ( 0 == lstrlen( last ) ) {
    AddPath( latest, newdir.lock() );
  }
  newdir.unlock();
  latest.unlock();


  // do backup
  doBackup( srcdir, newdir, latest );

  // 後始末: プロセスが終了するので、不要ではある
  newdir.release();
  latest.release();
  srcdir.release();


  // fine ...
  TCHAR msg[ MAX_BUFSIZ_ERR ];
  wsprintf( msg,
	    TEXT( "Success !!\n  Copy: %d files.\n  Link: %d files" ),
	    NumOfCopy,
	    NumOfLink
	    );
  MESSAGE( msg );


  return EXIT_SUCCESS;
}
