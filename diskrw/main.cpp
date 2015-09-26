/*
 *	DISK READ/WRITE MONITOR
 *	diskrw version 0.1
 */
#include <windows.h>
#include <pdh.h>
#include "resource.h"

// conf
#define	WCLSNAME			TEXT( "disk read/write" )
#define	WTITLE				NULL
#define	MYWM_NOTIFYICON		(WM_APP + 17)
#define	TM_INTERVAL			500					// time-out value, in milliseconds
#define	TIPS				TEXT( "Disk Access Lamp Ver 0.1" )
#define	MSG_TITLE			WCLSNAME
typedef enum {
	RW_ERR = -1, RW_NONE = 0, RW_READ, RW_WRITE, RW_BOTH, RW_ERROR, RW_MAX
} rw_stat_t;

// BSS
HINSTANCE	HInstance;
UINT		TimerId;
HICON		HIcons[RW_MAX];


// pdh
HQUERY		hQueryRead;
HCOUNTER	hCounterRead;
HQUERY		hQueryWrite;
HCOUNTER	hCounterWrite;


//
// -- REPORT -------------------------------------------------------
//
void REPORT( TCHAR * format, ... ) {
  TCHAR buf[1024];
  va_list args;
  va_start( args, format );
  wvsprintf( buf, format, args );
  MessageBox( NULL, buf, TEXT( "message" ), MB_OK | MB_ICONINFORMATION );
  va_end( args );
}

/*
 *	-- load icon ------------------------------------------------------
 */
BOOL	makeIcon( HWND hWnd ) {
	UNREFERENCED_PARAMETER( hWnd );

	HIcons[RW_NONE] = LoadIcon( HInstance, MAKEINTRESOURCE( IDI_NOACCESS ) );
	HIcons[RW_READ] = LoadIcon( HInstance, MAKEINTRESOURCE( IDI_READONLY ) );
	HIcons[RW_WRITE] = LoadIcon( HInstance, MAKEINTRESOURCE( IDI_WRITEDONLY ) );
	HIcons[RW_BOTH] = LoadIcon( HInstance, MAKEINTRESOURCE( IDI_READWRITED ) );
	HIcons[RW_ERROR] = LoadIcon( HInstance, MAKEINTRESOURCE( IDI_SOMEERROR ) );
	
	if ( NULL == HIcons[RW_NONE] || NULL == HIcons[RW_READ]
	|| NULL == HIcons[RW_WRITE] || NULL == HIcons[RW_BOTH]
	|| NULL == HIcons[RW_ERROR]
	) {
		return FALSE;
	}
	return TRUE;
}
VOID	deleteIcon( VOID ) {
}


/*
 *	-- pdh ------------------------------------------------------------
 */
BOOL	pdh_init( VOID ) {
	PDH_STATUS	stat;
	LPCTSTR		szPathRead	= TEXT( "\\PhysicalDisk(_Total)\\% Disk Read Time" );
	LPCTSTR		szPathWrite	= TEXT( "\\PhysicalDisk(_Total)\\% Disk Write Time" );
	
	stat = PdhOpenQuery( NULL, 0, &hQueryRead );
	if ( ERROR_SUCCESS != stat ) {
		goto stage1;
	}
	stat = PdhAddCounter( hQueryRead, szPathRead, 0, &hCounterRead );
	if ( ERROR_SUCCESS != stat ) {
		goto stage2;
	}
	stat = PdhOpenQuery( NULL, 0, &hQueryWrite );
	if ( ERROR_SUCCESS != stat ) {
		goto stage3;
	}
	stat = PdhAddCounter( hQueryWrite, szPathWrite, 0, &hCounterWrite );
	if ( ERROR_SUCCESS != stat ) {
		goto stage4;
	}
	return TRUE;
stage4:
	PdhCloseQuery( hQueryWrite );
stage3:
stage2:
	PdhCloseQuery( hQueryRead );
stage1:
	return FALSE;
}
VOID	pdh_settle( VOID ) {
	PdhCloseQuery( hQueryWrite );
	PdhCloseQuery( hQueryRead );
}
rw_stat_t	getstat( VOID ) {
	PDH_FMT_COUNTERVALUE	cvRead;
	PDH_FMT_COUNTERVALUE	cvWrite;
	PDH_STATUS				stat;
	
	stat = PdhCollectQueryData( hQueryRead );
	if ( ERROR_SUCCESS != stat )	return RW_ERROR;
	stat = PdhGetFormattedCounterValue(
		hCounterRead, PDH_FMT_LONG, NULL, &cvRead
	);
	if ( ERROR_SUCCESS != stat )	return RW_ERROR;
	
	stat = PdhCollectQueryData( hQueryWrite );
	if ( ERROR_SUCCESS != stat )	return RW_ERROR;
	stat = PdhGetFormattedCounterValue(
		hCounterWrite, PDH_FMT_LONG, NULL, &cvWrite
	);
	if ( ERROR_SUCCESS != stat )	return RW_ERROR;
	
	if ( 0 < cvRead.longValue && 0 < cvWrite.longValue )	return RW_BOTH;
	if ( 0 < cvRead.longValue )								return RW_READ;
	if ( 0 < cvWrite.longValue )							return RW_WRITE;
	return RW_NONE;
}


/*
 *	-- Timer ----------------------------------------------------------
 */
VOID	starttimer( HWND hWnd ) {
	TimerId = SetTimer( hWnd, GetCurrentProcessId(), TM_INTERVAL, NULL );
	if ( 0 == TimerId ) {
		REPORT( TEXT( "ERROR: SetTimer()" ) );
		SendMessage( hWnd, WM_DESTROY, 0, 0 );
	}
}
VOID	stoptimer( HWND hWnd ) {
	KillTimer( hWnd, TimerId );
}


/*
 *	-- Task Tray ------------------------------------------------------
 */
VOID	staytray( HWND hWnd ) {
	NOTIFYICONDATA	notifyicondata;
	
	notifyicondata.cbSize = sizeof( notifyicondata );
	notifyicondata.hWnd = hWnd;
	notifyicondata.uID = GetCurrentProcessId();
	notifyicondata.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	notifyicondata.uCallbackMessage = MYWM_NOTIFYICON;
	notifyicondata.hIcon = HIcons[RW_NONE];
	lstrcpyn(
		  notifyicondata.szTip
		, TIPS
		, sizeof( notifyicondata.szTip )
	);
	BOOL	b = Shell_NotifyIcon( NIM_ADD, &notifyicondata );
	if ( 0 == b ) {
		REPORT( TEXT( "ERROR: Shell_NotifyIcon( NIM_ADD )" ) );
		SendMessage( hWnd, WM_DESTROY, 0, 0 );
	}
}
VOID	removetray( HWND hWnd ) {
	NOTIFYICONDATA	notifyicondata;
	
	notifyicondata.cbSize = sizeof( notifyicondata );
	notifyicondata.hWnd = hWnd;
	notifyicondata.uID = GetCurrentProcessId();
	notifyicondata.uFlags = 0;
	Shell_NotifyIcon( NIM_DELETE, &notifyicondata );
}
VOID	updatetray( HWND hWnd ) {
	static rw_stat_t	prev_stat	= RW_NONE;
	rw_stat_t			stat;
	NOTIFYICONDATA		notifyicondata;
	
	stat = getstat();
	/*
	if ( RW_ERR == stat ) {
		SendMessage( hWnd, WM_DESTROY, 0, 0 );
		return;
	}
	*/
	if ( prev_stat == stat ) {
		return;
	}
	notifyicondata.cbSize = sizeof( notifyicondata );
	notifyicondata.hWnd = hWnd;
	notifyicondata.uID = GetCurrentProcessId();
	notifyicondata.uFlags = NIF_ICON;
	notifyicondata.hIcon = HIcons[stat];
	BOOL	b = Shell_NotifyIcon( NIM_MODIFY, &notifyicondata );
	if ( 0 != b ) {
		prev_stat = stat;
	}
}


/*
 *	-- Timer Handler --------------------------------------------------
 */
VOID	updateview( HWND hWnd ) {
	updatetray( hWnd );
}


/*
 *	-- Popup Menu -----------------------------------------------------
 */
VOID	showpopupmenu( HWND hWnd ) {
	HMENU	hMenu;
	HMENU	hSubMenu;
	POINT	pt;
	
	if ( FALSE == SetForegroundWindow( hWnd ) ) {
		MessageBox( hWnd, TEXT( "ERROR: SetForegroundWindow" ), MSG_TITLE, MB_OK );
		return;
	}
	hMenu = LoadMenu( HInstance, MAKEINTRESOURCE( IDM_POPUPMENU ) );
	if ( NULL == hMenu ) {
		MessageBox( hWnd, TEXT( "ERROR:Load Menu" ), MSG_TITLE, MB_OK );
		return;
	}
	hSubMenu = GetSubMenu( hMenu, 0 );
	if ( NULL == hSubMenu ) {
		MessageBox( hWnd, TEXT( "ERROR:Load Popup Menu" ), MSG_TITLE, MB_OK );
		DestroyMenu( hMenu );
		return;
	}
	GetCursorPos( &pt );
	TrackPopupMenu(
		  hSubMenu
		, TPM_CENTERALIGN
		, pt.x
		, pt.y
		, 0
		, hWnd
		, NULL
	);
	DestroyMenu( hSubMenu );
	DestroyMenu( hMenu );
}


/*
 *	-- Main Process ---------------------------------------------------
 */
VOID	hidewindow( HWND hWnd ) {
	ShowWindow( hWnd, SW_HIDE );
}


/*
 *	-- Main Handle ----------------------------------------------------
 */

LRESULT CALLBACK WindowProcedure (
	  HWND		hwnd
	, UINT		message
	, WPARAM	wParam
	, LPARAM	lParam
) {
	switch ( message ) {
	case WM_CREATE:
		if ( FALSE == makeIcon( hwnd ) ) {
			REPORT( TEXT( "FAIL: WM_CREATE: makeIcon()" ));
			SendMessage( hwnd, WM_DESTROY, 0, 0 );
		}
		if ( FALSE == pdh_init() ) {
			REPORT( TEXT( "FAIL: WM_CREATE: pdh_init()" ));
			SendMessage( hwnd, WM_DESTROY, 0, 0 );
		}
		staytray( hwnd );
		starttimer( hwnd );
		break;
	case WM_DESTROY :
		stoptimer( hwnd );
		removetray( hwnd );
		pdh_settle();
		deleteIcon();
		PostQuitMessage( 0 );
		break;
	case WM_PAINT:
		if ( IsWindowVisible( hwnd ) ) {
			hidewindow( hwnd );
		}
		break;
	case WM_TIMER :
		if ( TimerId == wParam ) {
			updateview( hwnd );
		}
		break;
	case WM_COMMAND :
		switch ( LOWORD( wParam ) ) {
		case IDM_POPUP_CLOSE :
			SendMessage( hwnd, WM_DESTROY, 0, 0 );
			break;
		}
		break;
	case MYWM_NOTIFYICON :
		if ( WM_RBUTTONUP == lParam ) {
			showpopupmenu( hwnd );
		}
		break;
	default:
		break;
	}
	return DefWindowProc( hwnd, message, wParam, lParam );
}


/*
 * -- Main Entry ------------------------------------------------------
 */
int	WINAPI WinMain(
	  HINSTANCE	hThisInstance
	, HINSTANCE	hPrevInstance
	, LPSTR		lpCmdLine
	, int		nCmdShow
) {
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	UNREFERENCED_PARAMETER( nCmdShow );

	HWND		hwnd;
	MSG			messages;
	WNDCLASSEX	wincl;

	HInstance = hThisInstance;

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = WCLSNAME;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof( WNDCLASSEX );
	wincl.hIcon = LoadIcon( hThisInstance, MAKEINTRESOURCE( IDI_MAINWINDOW ) );
	wincl.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
	wincl.hCursor = LoadCursor( NULL, IDC_ARROW );
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	if( !RegisterClassEx( &wincl ) ) {
		REPORT( TEXT( "FAIL: RegisterClassEx()" ));
		return EXIT_FAILURE;
	}
	hwnd = CreateWindowEx(
		  0
		, WCLSNAME
		, WTITLE
		, WS_OVERLAPPEDWINDOW
		, 0
		, 0
		, 0
		, 0
		, NULL
		, NULL
		, hThisInstance
		, NULL
	);
	if ( NULL == hwnd ) {
		REPORT( TEXT( "ERROR: CreateWindowEx()" ) );
		return EXIT_FAILURE;
	}
//	ShowWindow( hwnd, nFunsterStil );
	while ( GetMessage( &messages, NULL, 0, 0 ) ) {
		TranslateMessage( &messages );
		DispatchMessage( &messages );
	}
	return messages.wParam;
}
