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

int orefreshGetMapsetArray( osubgMapset **arr, size_t *size ) {

	osubgConfig cfg = { 0 };
	ofileGetConfig( &cfg );

	int wsize = MultiByteToWideChar( CP_UTF8, 0, cfg.osuPath, -1, NULL, 0 );
	wchar_t *wpath = calloc( wsize, sizeof( wchar_t ) );
	MultiByteToWideChar( CP_UTF8, 0, cfg.osuPath, -1, wpath, wsize );

	FILE *mapsetsFile = _wfopen( L"mapsets.txt", L"r" );
	int res = 1;
	uint32_t
		length = 0,
		mapsetCount = 0;

	while ( res ) {
		res = ofileReadQuotes( mapsetsFile, &length, NULL );
		char *str = malloc( res + 1 );
		res = ofileReadQuotes( mapsetsFile, &length, str );

		
	}

	return 1;
}
