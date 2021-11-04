#include <windows.h>
#include "sbwnd/sbgui.h"
#include "osubg-cmd/ofile.h"
#include "osubg-cmd/osubg.h"

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

    wchar_t **argv;
    int argc;

    argv = CommandLineToArgvW( GetCommandLine( ), &argc );
    int res = osubgExecuteCommand( argv, argc );

    //msgloop( );

    //shutdownSB( );
    
    return EXIT_SUCCESS;
}

int osubgExecuteCommand( wchar_t **argv, int argc ) {
    for ( int i = 0; i < argc; i++ )
        wprintf( L"%s\n", argv[i] );

    return 0;
}