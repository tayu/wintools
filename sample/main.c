#define STRICT
#define UNICODE
#include <windows.h>


int WINAPI WinMain(
		   HINSTANCE hInstance,
		   HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine,
		   int nShowCmd
		   ) {
  MessageBox(
	     NULL,
	     TEXT( "Hello mingw" ),
	     TEXT( "MessageBox" ),
	     MB_OK
	     );
  return 0;
}
