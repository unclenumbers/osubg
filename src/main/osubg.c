#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include "sbwnd/sbgui.h"
#include "osubg-cmd/ofile.h"
#include "osubg-cmd/osubg.h"
#include "osubg-cmd/setfolder.h"
#include "osubg-cmd/refresh.h"

//#define RELEASE

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hprev, LPSTR lpCmdLine, int nCmdShow ) {

	#ifdef RELEASE
	FreeConsole( );
	#endif

	//initGUI( );

	/*sbWnd_Layout lout = { 0 };
	lout.dims.intDims[0] = CW_USEDEFAULT;
	lout.dims.intDims[1] = CW_USEDEFAULT;
	lout.dims.floatDims[2] = 0.5f;
	lout.dims.floatDims[3] = 0.5f;
	lout.type = SB_DIMTYPE_IIFF_TL_SRNN;*/

	//sbWnd *first = SBMasterWindows.create( L"osubg v0.1", &lout );

	SetCurrentDirectory( L".." );
	WIN32_FIND_DATA data = { 0 };
	
	HANDLE h = FindFirstFile( L"*", &data );
	if ( h == INVALID_HANDLE_VALUE ) {
		DWORD er = GetLastError( );
		wchar_t *msg = NULL;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			er,
			0,
			( wchar_t * )&msg, 0, NULL
		);

		MessageBox( NULL, msg, L"cunny", MB_OK );
	} else {
		while ( GetLastError( ) == 0 ) {
			wprintf( L"%s\n", data.cFileName );
			FindNextFile( h, &data );
		}
	}

	int res = osubgToAppDataPath( );
	res = ofileCreateConfig( );

	osubgRefreshReload( );
	wchar_t **argv;
	int argc;

	argv = CommandLineToArgvW( GetCommandLine( ), &argc );
	res = osubgExecuteCommand( argv, argc );

	//msgloop( );

	//shutdownSB( );
	
	return !res;
}

int osubgExecuteCommand( wchar_t **argv, int argc ) {

	if ( argc < 2 ) {
		printf( "die\n" );
		return 0;
	}

	if ( !wcscmp( argv[1], L"setfolder" ) ) {

		if ( argc != 3 ) {
			printf(
				"setfolder usage: osubg setfolder \\path\\to\\osu\n"
				"If you would like to specify a path with spaces in the name,\n"
				"surround your path in quotes, as in \"\\path\\to\\osu folder\"\n" );
			return 0;
		}
		osubgSetfolder( argv[2] );

		return 1;
	} else if ( !wcscmp( argv[1], L"refresh" ) ) {

		if ( argc != 2 ) {
			printf( "refresh usage: osubg refresh\nType osubg help for more information.\n" );
			return 0;
		}
		int res = osubgRefresh( );
		
	} else
		printf( "Command unknown\n" );

	return 0;
}