#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <windows.h>

#include "osubg-cmd/refresh.h"
#include "osubg-cmd/ofile.h"

int osubgRefresh( void ) {
	return 1;
}

int osubgRefreshReload( void ) {
	return 1;
}

int osubgRefreshReplace( void ) {
	return 1;
}

int orefreshGetMapsetArray( osubgMapset *arr, size_t *size ) {

	uint32_t currentPathSize = GetCurrentDirectory( 0, NULL );
	wchar_t *currentPath = calloc( currentPathSize, sizeof( wchar_t ) );
	GetCurrentDirectory( currentPathSize, currentPath );

	osubgConfig cfg = { 0 };
	ofileGetConfig( &cfg );

	int wsize = MultiByteToWideChar( CP_UTF8, 0, cfg.osuPath, -1, NULL, 0 );
	printf( "%d\n", wsize );

	return 1;
}
