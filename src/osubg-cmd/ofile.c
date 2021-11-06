#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdint.h>

#include <windows.h>
#include "osubg-cmd/ofile.h"
#include "graybg/graybg.h"

int ofileSeekEventHeader( FILE *f ) {
	// [Events] = 8 chars
	char header[9] = { 0 };
	wchar_t
		current = 0,
		filler = 0;

	while ( ( current = fgetwc( f ) ) != WEOF ) {
		if ( current == L'[' ) {

			header[0] = current;
			for ( int i = 1; i < 8; i++ ) {
				filler = fgetwc( f );
				if  ( filler == WEOF ) return 0;
				header[i] = filler;
			}

			if ( !strcmp( header, "[Events]" ) )
				return 1;
			else
				fseek( f, -7, SEEK_CUR );
		}
	}

	return 0;
}

int ofileReadQuotes( FILE *f, uint32_t *length, char *str ) {
	wchar_t current = 0;
	uint32_t size = 0;

	while ( ( current = fgetwc( f ) ) != WEOF ) {
		if ( current == L'\"' ) {
			while ( ( current = fgetwc( f ) ) != L'\"' ) {
				if ( current == WEOF ) return 0;
				size++;
			}

			*length = size;
			int64_t seeker = size;
			// Seek to just after the first quote
			fseek( f, ( -seeker - 1 ), SEEK_CUR );

			if ( str == NULL ) {
				// Preserve quotes if user only wants size
				fseek( f, -1, SEEK_CUR );
				return 1;
			}

			for ( uint32_t i = 0; i < size; i++ )
				str[i] = fgetwc( f );

			fseek( f, 1, SEEK_CUR ); // Set cursor to after second quote
			return 1;
		}
	}

	return 0;
}

int ofileCreateConfig( void ) {
	DWORD usernameLength = 256;
	wchar_t username[ MAX_PATH ] = { 0 };
	GetUserName( username, &usernameLength );


	uint32_t currentSize = GetCurrentDirectory( 0, NULL );
	wchar_t *currentPath = calloc( currentSize, sizeof( wchar_t ) );
	GetCurrentDirectory( currentSize, currentPath );


	wchar_t path[ MAX_PATH ] = { 0 };
	swprintf( path, MAX_PATH, L"\\Users\\%ls\\AppData\\Roaming\\osubg", username );
	
	if ( GetFileAttributes( path ) == INVALID_FILE_ATTRIBUTES )
		CreateDirectory( path, NULL );
	
	SetCurrentDirectory( path );

	swprintf( path, MAX_PATH, L"osubg.conf" );
	FILE *f = _wfopen( path, L"r" );
	if ( f == NULL) {
		f = _wfopen( L"osubg.conf", L"w" );
		if ( f == NULL )
			return 0;
		fwprintf( f, L"osu! path: \"\"\nmapset count: \"0\"\ncurrent mode: \"normal\"\n" );
	}
	
	fclose( f );

	swprintf( path, MAX_PATH, L"mapsets.txt" );
	f = _wfopen( path, L"a" );
	if ( f == NULL)
		return 0;
	fclose( f );

	// graybgpng, graybgjpg, and graybgsmall are external array variables.
	// They represent the three backgrounds that osubg needs.
	f = _wfopen( L"graybg.png", L"rb" );
	if ( f == NULL ) {
		f = _wfopen( L"graybg.png", L"wb" );
		fwrite( graybgpng, 1, sizeof( graybgpng ), f );
		fclose( f );
	}

	f = _wfopen( L"graybg.jpg", L"rb" );
	if ( f == NULL ) {
		f = _wfopen( L"graybg.jpg", L"wb" );
		fwrite( graybgjpg, 1, sizeof( graybgjpg ), f );
		fclose( f );
	}

	f = _wfopen( L"graybgsmall.jpg", L"rb" );
	if ( f == NULL ) {
		f = _wfopen( L"graybgsmall.jpg", L"wb" );
		fwrite( graybgsmall, 1, sizeof( graybgsmall ), f );
		fclose( f );
	}

	return 1;
}

int ofileGetConfig( osubgConfig *cfg ) {

	FILE *f = _wfopen( L"osubg.conf", L"r" );
	if ( f == NULL )
		return 0;

	// Path, then mapset count, then mode.
	uint32_t len = 0;

	ofileReadQuotes( f, &len, NULL );
	char *str = calloc( ( len + 1 ), sizeof( char ) );

	ofileReadQuotes( f, &len, str );
	cfg->osuPath = str;


	ofileReadQuotes( f, &len, NULL );
	str = calloc( len + 1, sizeof( char ) );

	ofileReadQuotes( f, &len, str );
	cfg->mapsetCount = atol( str );

	free( str ); // Don't need the string this time.


	ofileReadQuotes( f, &len, NULL );
	str = calloc( len + 1, sizeof( char ) );

	ofileReadQuotes( f, &len, str );
	if ( !strcmp( str, "normal" ) )
		cfg->currentMode = 0;
	else if ( !strcmp( str, "gray" ) )
		cfg->currentMode = 1;
	else
		return 0;
	free( str ); // Don't need the string this time either.
	
	fclose( f );
	return 1;
}

int ofileSetConfig( osubgConfig *cfg ) {
	
	FILE *f = _wfopen( L"osubg.conf", L"w" );
	fprintf(
		f,
		"osu! path: \"%s\"\nmapset count: \"%u\"\ncurrent mode: \"%s\"\n",
		cfg->osuPath,
		cfg->mapsetCount,
		( cfg->currentMode ? "gray" : "normal" )
	);
	
	return 1;
}

int ofileDestroyConfig( osubgConfig *cfg ) {
	free( cfg->osuPath );
	return 1;
}