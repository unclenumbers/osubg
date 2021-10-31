#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdint.h>
#include <windows.h>
#include "osubg-cmd/ofile.h"

int seekEventHeader( FILE *f, wchar_t *filename ) {
	// [Events] = 8 chars
	wchar_t
		header[9] = { 0 },
		current = 0;

	FILE *test = _wfopen( L"test.txt", L"r" );
	size_t fileChCount = 0;
	while ( fgetwc( test )!= WEOF )
		fileChCount++;

	wprintf( L"Character count: %llu\n", fileChCount );
	wchar_t *fString = calloc( ( fileChCount + 1 ), sizeof( wchar_t ) );

	fseek( test, 0, SEEK_SET );

	FILE *out = _wfopen( L"testout.txt", L"w" );
	wchar_t ch = 0;
	size_t curCh = 0;
	while ( ( ch = fgetwc( test ) ) != WEOF ) {
		fString[ curCh++ ] = ch;
		int chars = fwprintf( out, L"%s\n", fString );
		NormalizeString( NormalizationC, fString, -1, NULL, 0 );
		int normal = IsNLSDefinedString( COMPARE_STRING, 0, NULL, fString, -1 );
		if ( !normal ) {
			printf( "Input unicode bad! " );
		}

		printf( "Chars written: %i\n", chars );
	}
	return 0;
}