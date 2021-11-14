#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include <windows.h>

#include "osubg-cmd/refresh.h"
#include "osubg-cmd/ofile.h"
#include "osubg-cmd/ohashtable.h"

int osubgRefresh( void ) {
	osubgRefreshReload( );
	osubgRefreshReplace( );
	return 1;
}

int osubgRefreshReload( void ) {

	osubgMapset *arr = NULL;
	size_t arrSize = 0;

	orefreshGetMapsetArray( &arr, &arrSize );
	if ( !arrSize )
		return 1;
	
	osubgHashTable *hashTable = oHashTableCreate( arrSize * 3 );
	if ( hashTable == NULL )
		return 1;

	for ( size_t i = 0; i < arrSize; i++ )
		oHashTableAdd( hashTable, arr[i].mapsetTitle );

	
	
	return 0;
}

int osubgRefreshReplace( void ) {
	return 1;
}

int orefreshGetMapsetArray( osubgMapset **arr, size_t *size ) {

	FILE *mapsetsFile = _wfopen( L"mapsets.txt", L"r" );
	uint32_t
		length = 0,
		mapsetCount = 0,
		currentMapsetMapCount = 0;

	int	res = ofileReadQuotes( mapsetsFile, &length, NULL );
	osubgMapsetFlag flag = MAPSET_NAME;

	// counting loop
	while ( res ) {
		res = ofileReadQuotes( mapsetsFile, &length, NULL );
		char *str = malloc( res + 1 );
		res = ofileReadQuotes( mapsetsFile, &length, str );

		switch( flag ) {
			case MAPSET_NAME:
				mapsetCount++;
				break;
			
			case MAPSET_BG_COUNT:
				if ( currentMapsetMapCount ) {
					currentMapsetMapCount--;
					break;
				}
				currentMapsetMapCount = strtoul( str, NULL, 0 );
				break;
				
			default:
				break;
		}

		if ( !currentMapsetMapCount )
			flag = ( flag + 1 ) % MAPSET_FLAG_MODULO;

		free( str );
	}

	fseek( mapsetsFile, 0, SEEK_SET );
	if ( !mapsetCount ) {
		*arr = NULL;
		*size = 0;
		return 1;
	}

	osubgMapset *mapsetArray = calloc( mapsetCount, sizeof( osubgMapset ) );
	*size = mapsetCount;

	res = 1;
	flag = MAPSET_NAME;
	currentMapsetMapCount = 0;
	uint32_t
		idx = 0,
		mapIdx = 0;

	// reading loop
	while ( res ) {
		res = ofileReadQuotes( mapsetsFile, &length, NULL );
		char *str = malloc( res + 1 );
		res = ofileReadQuotes( mapsetsFile, &length, str );

		switch( flag ) {
			case MAPSET_NAME:
				mapsetArray[ idx ].mapsetTitle = str;
				break;
			
			case MAPSET_MODE:
				mapsetArray[ idx ].mode = !strcmp( str, "gray" ) ? 1 : 0;
				free( str );
				break;

			case MAPSET_ID:
				mapsetArray[ idx ].mapsetId = str;
				break;

			case MAPSET_BG_COUNT:
				if ( currentMapsetMapCount ) {
					mapsetArray[ idx ].mapBgNames[ mapIdx++ ] = str;
					currentMapsetMapCount--;
					break;
				}
				
				currentMapsetMapCount = strtoul( str, NULL, 0 );
				mapsetArray[ idx ].mapCount = currentMapsetMapCount;
				mapsetArray[ idx ].mapBgNames = calloc( currentMapsetMapCount, sizeof( char * ) );
				mapIdx = 0;
				free( str );
				break;
		}

		if ( !currentMapsetMapCount )
			flag = ( flag + 1 ) % MAPSET_FLAG_MODULO;

	}

	fclose( mapsetsFile );
	return 0;
}
