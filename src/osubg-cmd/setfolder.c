#include <stdio.h>
#include <windows.h>
#include "osubg-cmd/setfolder.h"
#include "osubg-cmd/ofile.h"

int osubgSetfolder( wchar_t *newpath ) {
    
    osubgConfig cfg = { 0 };
    ofileGetConfig( &cfg );

    int u8size = WideCharToMultiByte( CP_UTF8, 0, newpath, -1, NULL, 0, NULL, NULL );
    char *u8path = calloc( u8size, sizeof( char ) );
    WideCharToMultiByte( CP_UTF8, 0, newpath, -1, u8path, u8size, NULL, NULL );

    cfg.osuPath = u8path;
    // do NOT free u8path, that job now goes to ofileDestroyConfig
    return !ofileSetConfig( &cfg );
    
}