#include <windows.h>
#include "sbwnd/sbgui.h"
#include "osubg-cmd/ofile.h"

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

    FILE *f = _wfopen( L"test.txt", L"r" );
    int out = ofileSeekEventHeader( f );
    printf( "%d\n", out );

    uint32_t length = 0;
    out = ofileReadQuotes( f, &length, NULL );
    printf( "%d\n", out );

    char *bgname = calloc( length + 1, sizeof( char ) );
    out = ofileReadQuotes( f, &length, bgname );
    printf( "%d\n", out );

    out = ofileCreateConfigFile( );
    printf( "%d\n", out );

    //msgloop( );

    //shutdownSB( );
    
    return EXIT_SUCCESS;
}