#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdint.h>

#define UNICODE

#include <windows.h>
#include "osubg-cmd/ofile.h"

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

int ofileCreateConfigFile( void ) {
	DWORD usernameLength = 256;
	wchar_t username[ MAX_PATH ] = { 0 };
	GetUserName( username, &usernameLength );

	wchar_t appDataPath[ MAX_PATH ] = { 0 };
	swprintf( appDataPath, MAX_PATH, L"\\Users\\%ls\\AppData\\Roaming\\osubg", username );

	if ( GetFileAttributes( appDataPath ) == INVALID_FILE_ATTRIBUTES ) {
		CreateDirectory( appDataPath, NULL );
	}

	swprintf( appDataPath, MAX_PATH, L"\\Users\\%ls\\AppData\\Roaming\\osubg\\osubg.conf", username );
	FILE *f = _wfopen( appDataPath, L"a" );
	return f != NULL;

}