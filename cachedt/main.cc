/*
 * Desktop Menu

ToDo:
・とりあえず STL で簡単に作る
・Win7 ではアイコンとメニューの間にスキマと線があるが、オーナードローでは
　うまく描けない。
　他のソースを見ると、qt でやってたり、IContextMenu とかいうのだったり
・リストのソートをうまくやれる
・new した要素がメモリリークするはずなのだが
・アイコンイメージ取得処理に時間が掛かって、右クリックの再入が発生する
　グローバル変数を使っているので、マズいはず。
・デスクトップの項目の取得は IE を使う方法があり、そちらだと「ごみ箱」や
　「マイコンピュータ」なんかもとれるらしい。でもディレクトリの扱いが不明。
・ディレクトリがオーナードローでない。なのでアイコンも無い。
・拡張予定：ディレクトリ指定して、その配下をメニュー化
・拡張予定：外部からの設定
・拡張予定：設定（＆読み込み）の作成。DLL 化
 */
#define STRICT
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <tchar.h>
#include <shlobj.h>
#include <list>
#include "resource.h"

#define MYWM_CLASSNAME TEXT( "DesktopMenu" )
#define MYWM_NOTIFYICON (WM_APP + 32)


// MUNU ID
const int IDM_FILE_BASE = 2001;
const int IDM_EXIT = 9999;

// data
const int MenuMin = IDM_FILE_BASE;
int MenuMax = MenuMin;
HINSTANCE HInstance = NULL;




// -- REPORT ---------------------------------------------------------
void REPORT( TCHAR * format, ... ) {
  TCHAR buf[1024];
  va_list args;
  va_start( args, format );
  wvsprintf( buf, format, args );
  va_end( args );
  MessageBox( NULL, buf, TEXT( "message" ), MB_OK | MB_ICONINFORMATION );
}
void ABORT( TCHAR * format, ... ) {
  TCHAR buf[1024];
  va_list args;
  va_start( args, format );
  wvsprintf( buf, format, args );
  va_end( args );
  MessageBox( NULL, buf, TEXT( "message" ), MB_OK | MB_ICONINFORMATION );
  exit( EXIT_FAILURE );
}



// -- list -----------------------------------------------------------
typedef struct  _node {
  BOOL isDir; // ディレクトリかどうか
  HMENU menu; // メニュー
  UINT wID; //ID
  HMENU parent; // 親メニュー
  TCHAR title[MAX_PATH]; // 表示
  TCHAR fullpath[MAX_PATH]; // ファイル
} node_t;
typedef node_t * nodeptr_t;


using namespace std;
list<node_t> MenuItems;

// 比較関数：ディレクトリ、出現順
bool comp( const node_t& l, const node_t& r ) {
  if ( l.isDir != r.isDir ) {
    return l.isDir > r.isDir;
  }
  return l.wID < r.wID;
}


// -- clear list -----------------------------------------------------
VOID initList( VOID ) {
  MenuItems.clear();
  MenuMax = MenuMin;
}



// -- get desktop directory  -----------------------------------------
BOOL getDesktopDir( HWND hwnd, TCHAR path[MAX_PATH] ) {
  IMalloc * ima;
  LPITEMIDLIST pidl;
  HRESULT hres;

  hres = SHGetMalloc( &ima );
  if ( FAILED( hres ) ) {
    REPORT( TEXT( "SHGetMalloc" ) );
    return FALSE;
  }
  hres = SHGetSpecialFolderLocation(
				    hwnd
				    , CSIDL_DESKTOPDIRECTORY
				    , &pidl
				    );
  if( SUCCEEDED( hres ) ) {
    if ( ! SHGetPathFromIDList( pidl, path ) ) {
      REPORT( TEXT( "SHGetPathFromIDList" ) );
      goto error_exit;
    }
    ima->Free( pidl );
  }
  ima->Release();
  return TRUE;

 error_exit:
  ima->Release();
  return FALSE;
}


// -- copy basename ------------------------------------------------
VOID basenamecpy( TCHAR * dst, TCHAR * src ) {
  TCHAR * s;
  TCHAR * d;
  TCHAR * p;
  TCHAR * e;
  TCHAR c;
  s = p = src;
  d = dst;
  e = NULL;
  while ( TEXT( '\0' ) != (c = *p++) ) {
    if ( TEXT( '.' ) == c ) {
      e = p;
    }
  }
  if ( NULL == e ) {
    lstrcpy( dst, src );
  } else {
    --e;
    for ( ; s < e; ) {
      *d++ = *s++;
    }
    *d = TEXT( '\0' );
  }
}


// -- Search Items --------------------------------------------------
BOOL findItems( HMENU parent_menu, list<node_t>& list, TCHAR dir[MAX_PATH] ) {

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
      if ( 0 != lstrcmp( TEXT( "." ), fdata.cFileName )
	   && 0 != lstrcmp( TEXT( ".." ), fdata.cFileName )
	   ) {
	HMENU menu = CreatePopupMenu();
	if ( NULL == menu ) {
	  REPORT( TEXT( "ERROR: CreatePopupMenu()" ));
	  goto error_exit;
	}
	node_t * new_item = new node_t;
	if ( NULL == new_item ) {
	  goto error_exit;
	}
	new_item->isDir = TRUE;
	new_item->menu = menu;
	new_item->wID = MenuMax++;
	new_item->parent = parent_menu;
	basenamecpy( new_item->title, fdata.cFileName );
	lstrcpy( new_item->fullpath, dir );
	lstrcat( new_item->fullpath, TEXT( "\\" ) );
	lstrcat( new_item->fullpath, fdata.cFileName );
	list.push_back( *new_item );
	if ( ! findItems( menu, list, new_item->fullpath ) ) {
	  goto error_exit;
	}
      }
    } else {
      node_t * new_item = new node_t;
      if ( NULL == new_item ) {
	goto error_exit;
      }
      new_item->isDir = FALSE;
      new_item->menu = NULL;
      new_item->wID = MenuMax++;
      new_item->parent = parent_menu;
      basenamecpy( new_item->title, fdata.cFileName );
      lstrcpy( new_item->fullpath, dir );
      lstrcat( new_item->fullpath, TEXT( "\\" ) );
      lstrcat( new_item->fullpath, fdata.cFileName );
      list.push_back( *new_item );
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


// -- Pouup Menu ----------------------------------------------------
void popupmenu( HWND hwnd ) {
  list<node_t>::iterator i;

  // リストの初期化
  initList();

  // トップのメニューハンドラ
  HMENU topmenu = CreatePopupMenu();
  if ( NULL == topmenu ) {
    REPORT( TEXT( "ERROR: CreatePopupMenu()" ));
    goto error_exit;
  }

  // メニュー表示のみ時のおまじない
  if ( FALSE == SetForegroundWindow( hwnd ) ) {
    REPORT( TEXT( "ERROR: SetForegroundWindow()" ));
    goto error_exit;
  }

  // リスト要素の作成
  TCHAR desktop[MAX_PATH];
  if ( ! getDesktopDir( hwnd, desktop ) ) {
    REPORT( TEXT( "ERROR: getDesktopDir()" ));
    goto error_exit;
  }
  if ( ! findItems( topmenu, MenuItems, desktop) ) {
    REPORT( TEXT( "ERROR: findItems()" ));
    goto error_exit;
  }


  // リストの整列
  MenuItems.sort( comp );


  // メニュー作成：サブメニュー含む
  MENUITEMINFO mii;
  ZeroMemory( &mii, sizeof( mii ) );
  mii.cbSize = sizeof( mii );
  for ( i = MenuItems.begin(); i != MenuItems.end(); ++i ) {
    if ( i->isDir ) {
      if ( 0 == InsertMenu(
			   i->parent
			   , 0
			   , MF_POPUP | MF_STRING
			   , (UINT_PTR)(i->menu)
			   , i->title
			   ) ) {
	REPORT( TEXT( "ERROR: InsertMenu(%s)" ), i->title );
	goto error_exit;
      }
    } else {
      mii.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
      mii.fType = MF_OWNERDRAW;
      mii.dwTypeData = i->title;
      mii.wID = i->wID;
      if ( 0 == InsertMenuItem( i->parent, 0, FALSE, &mii ) ) {
	REPORT( TEXT( "ERROR: InsertMenuItem(%s)"), i->title );
	goto error_exit;
      }
    }
  }

  // 固定部の作成＆表示
  ZeroMemory( &mii, sizeof( mii ) );
  mii.cbSize = sizeof( mii );

  mii.fMask = MIIM_TYPE | MIIM_STATE;
  mii.fType = MFT_SEPARATOR;
  mii.fState = MFS_GRAYED;
  if ( 0 == InsertMenuItem( topmenu, IDM_EXIT - 1, FALSE, &mii ) ) {
    REPORT( TEXT( "ERROR: InsertMenuItem(): MFT_SEPARATOR" ) );
    goto error_exit;
  }
  mii.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
  mii.fType = MFT_STRING;
  mii.dwTypeData = TEXT( "Quit" );
  mii.wID = IDM_EXIT;
  if ( 0 == InsertMenuItem( topmenu, IDM_EXIT, FALSE, &mii ) ) {
    REPORT( TEXT( "ERROR: InsertMenuItem(): IDM_EXIT" ) );
    goto error_exit;
  }


  // メニュー表示
  POINT pt;
  GetCursorPos( &pt );
  TrackPopupMenu(
		 topmenu
		 , TPM_CENTERALIGN
		 , pt.x
		 , pt.y
		 , 0
		 , hwnd
		 , NULL
		 );
  if ( 0 == DestroyMenu( topmenu ) ) {
    REPORT( TEXT( "ERROR: DestroyMenu()" ));
    topmenu = NULL;
    goto error_exit;
  }

  return;

 error_exit:
  initList();
  if ( NULL != topmenu ) {
    if ( 0 == DestroyMenu( topmenu ) ) {
      REPORT( TEXT( "ERROR: error_exit: DestroyMenu()" ));
    }
  }
}


// -- on Tray Icon -------------------------------------------------
BOOL addtray( HWND hwnd, BOOL isAdd ) {
  static NOTIFYICONDATA nid;

  if ( isAdd ) {
    nid.cbSize = sizeof( nid );
    nid.hIcon = LoadIcon( HInstance, MAKEINTRESOURCE( IDI_MAIN ) );
    nid.hWnd = hwnd;
    lstrcpy( nid.szTip, MYWM_CLASSNAME );
    nid.uCallbackMessage = MYWM_NOTIFYICON;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP ;

    if ( FALSE == Shell_NotifyIcon( NIM_ADD, &nid ) ) {
      REPORT( TEXT( "ERROR: Shell( NIM_ADD )" ));
      return FALSE;
    }
  } else {
    if ( FALSE == Shell_NotifyIcon( NIM_DELETE, &nid ) ) {
      REPORT( TEXT( "ERROR: Shell( NIM_DELETE )" ));
      return FALSE; // no mean ?
    }
  }
  return TRUE;
}


// -- Exec ----------------------------------------------------------
void execCommand( HWND hwnd, UINT id ) {
  list<node_t>::iterator i;
  for ( i = MenuItems.begin(); i != MenuItems.end(); ++i ) {
    if ( id == i->wID ) {
      if ( (HINSTANCE)32 >= ShellExecute(
			     hwnd
			     , NULL
			     , i->fullpath
			     , NULL
			     , NULL
			     , SW_SHOWNORMAL ) ) {
	REPORT( TEXT( "ERROR: ShellExecute(): %s" ), i->fullpath );
      }
      // リストの初期化
      initList();
      break;
    }
  }
  if ( i == MenuItems.end() ) {
    REPORT( TEXT( "INFO: Not Exist in list: id=%d" ), id );
  }
}


// -- WindowProc ----------------------------------------------------
LRESULT CALLBACK WindowProc(
			    HWND hwnd
			    , UINT uMsg
			    , WPARAM wParam
			    , LPARAM lParam
			    ) {
  switch ( uMsg ) {

  case WM_COMMAND :
    {
      int id = LOWORD( wParam );
      if ( (MenuMin <= id) && ( id < MenuMax) ) {
	execCommand( hwnd, id );
      } else {
	switch ( id ) {
	case IDM_EXIT :
	  PostMessage( hwnd, WM_DESTROY, 0, 0 );
	  break;
	}
      }
    }
    break;


  case WM_DRAWITEM :
    if ( ODT_MENU == ((LPMEASUREITEMSTRUCT)lParam)->CtlType ) {
      DRAWITEMSTRUCT * lpdis = (LPDRAWITEMSTRUCT)lParam;
      UINT id = lpdis->itemID;
      list<node_t>::iterator i;
      for ( i = MenuItems.begin(); i != MenuItems.end(); ++i ) {
	if ( id == i->wID ) {

	  // ToDo: バックを塗りつぶす

	  // icon
	  WORD wIdx;
	  HICON hIcon = ExtractAssociatedIcon( NULL, i->fullpath, &wIdx );
	  if ( NULL == hIcon ) {
	    ABORT( TEXT( "ERROR: ExtractAssociatedIcon(): %s" ), i->fullpath );
	  }
	  if ( 0 == DrawIconEx(
			       lpdis->hDC
			       , lpdis->rcItem.left
			       , lpdis->rcItem.top
			       , hIcon
			       , GetSystemMetrics( SM_CXMENUCHECK )
			       , GetSystemMetrics( SM_CYMENUCHECK )
			       , 0
			       , NULL
			       , DI_NORMAL
			       ) ) {
	    ABORT( TEXT( "ERR: WM_DRAWITEM: DrawIconEx(): %s" ), i->fullpath );
	  }
	  lpdis->rcItem.left += GetSystemMetrics( SM_CXMENUCHECK );
	  lpdis->rcItem.left += 5;

	  // text
	  COLORREF cr;
	  if ( ODS_SELECTED & lpdis->itemState ) {

	    // cr = SetTextColor( lpdis->hDC, RGB( 0, 0, 255 ) );
	    // if ( CLR_INVALID == cr ) {
	    //   ABORT( TEXT( "ERROR: SetTextColor():
	    // }

	    // HBRUSH hb;
	    //	    hb = CreateSolidBrush( RGB( 0, 0, 255 ) );

	    // テキスト部のみ。背景色の指定。四角の描画はしない
	    SetBkColor( lpdis->hDC, RGB( 0, 0, 255 ) );
	    SetTextColor( lpdis->hDC, RGB( 255, 255, 255 ) );
	  }
	  if ( 0 == DrawText(
			     lpdis->hDC
			     , i->title
			     , lstrlen( i->title )
			     , &(lpdis->rcItem)
			     , DT_LEFT
			     ) ) {
	    ABORT( TEXT( "ERROR: WM_DRAWITEM: DrawText()" ) );
	  }
	  if ( ODS_SELECTED & lpdis->itemState ) {
	    cr = SetTextColor( lpdis->hDC, cr);
	    if ( CLR_INVALID == cr ) {
	      ABORT( TEXT( "ERROR: SetTextColor(): reset" ) );
	    }
	  }
	  break;
	}
      }
      if ( i == MenuItems.end() ) {
	REPORT( TEXT( "WARN: No id in list: %d" ), id );
      }
    }
    break;

  case WM_MEASUREITEM :
    if ( ODT_MENU == ((LPMEASUREITEMSTRUCT)lParam)->CtlType ) {
      LPMEASUREITEMSTRUCT lpMI = (LPMEASUREITEMSTRUCT)lParam;
      HDC hdc = GetDC( hwnd );
      if ( NULL == hdc ) {
        ABORT( TEXT( "ERR: WM_MEASUREITEM: GetDC()" ) );
      }
      SIZE sz;
      list<node_t>::iterator i;
      lpMI->itemWidth = lpMI->itemHeight = 0;
      for ( i = MenuItems.begin(); i != MenuItems.end(); ++i ) {
	if ( lpMI->itemID == i->wID ) {
	  if ( 0 == GetTextExtentPoint32(
					 hdc
					 , i->title
					 , lstrlen( i->title)
					 , &sz
					 ) ) {
	    ABORT( TEXT( "ERR: WM_MEASUREITEM: GetTextExtentPoint32()" ) );
	  }
	  lpMI->itemWidth = GetSystemMetrics( SM_CXMENUCHECK ) + sz.cx / 2;
	  // lpMI->itemWidth += 14;
	  lpMI->itemHeight = sz.cy;
	  // lpMI->itemHeight += 4;
	}
      }
      ReleaseDC( hwnd, hdc );
    }
    break;


    // ToDo: 左クリックで設定、とか
  case MYWM_NOTIFYICON :
    switch ( lParam ) {
    case WM_RBUTTONDOWN :
      popupmenu( hwnd );
      break;
    }
    break;

  case WM_CREATE :
    if ( !addtray( hwnd, TRUE ) ) {
      PostQuitMessage( 0 );
    }
    break;

  case WM_DESTROY:
    addtray( hwnd, FALSE );
    PostQuitMessage( 0 );
    break;

  default:
    return( DefWindowProc( hwnd, uMsg, wParam, lParam ) );
  }
  return (0);
}


// -- main ----------------------------------------------
int WINAPI WinMain(
		   HINSTANCE hInstance,
		   HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine,
		   int nCmdShow
) {
  MSG msg;
  HWND hwnd;
  WNDCLASS wc;

  HInstance = hInstance;

  ZeroMemory( &wc, sizeof( wc ) );
  wc.hInstance = hInstance;
  wc.lpszClassName = MYWM_CLASSNAME;
  wc.lpfnWndProc = WindowProc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_MAIN ) );
  wc.hCursor = LoadCursor( NULL, IDC_ARROW );
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszMenuName = NULL;
  if ( ! RegisterClass( &wc ) ) {
    REPORT( TEXT( "ERROR: RegisterClass()" ));
    return EXIT_FAILURE;
  }

  hwnd = CreateWindowEx(
                        WS_EX_CONTROLPARENT | WS_EX_WINDOWEDGE,
                        MYWM_CLASSNAME,
                        MYWM_CLASSNAME,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        NULL,
                        NULL,
                        hInstance,
                        NULL
			);
  if ( NULL == hwnd ) {
    REPORT( TEXT( "ERROR: CreateWindowEx()" ) );
    return EXIT_FAILURE;
  }

  // ShowWindow( hwnd, nCmdShow );

  while ( GetMessage( &msg, NULL, 0, 0 ) ) {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }
  return msg.wParam;
}
