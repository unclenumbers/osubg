#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdint.h>
#include <windows.h>
#include "osubg-cmd/ofile.h"

#define _USE_MINGW_ANSI_STDIO 1

int seekEventHeader( FILE *f, wchar_t *filename ) {
	// [Events] = 8 chars
	wchar_t
		header[9] = { 0 },
		current = 0;

	FILE
		*test = _wfopen( L"test.txt", L"r" ),
		*test2 = fopen( "test.txt", "r" );
	size_t
		fileChCount = 0,
		fileChCount2 = 0;

	wchar_t cha = 0;
	char cha2 = 0;
	while ( ( cha = fgetwc( test ) ) != WEOF ) {
		fileChCount++;
		wprintf( L"Character code wide: %hx\n", cha );
	}

	while ( ( cha2 = fgetc( test2 ) )!= EOF ) {
		fileChCount2++;
		wprintf( L"Character code: %hhx\n", cha2 );
	}
	
	wchar_t *imstr = L"妹妹妹😊";
	size_t sz = wcslen( imstr );
	wprintf( L"size of string: %zu\n", sz );

	wprintf( L"single-byte char count: %u\n", ( unsigned int )fileChCount2 );

	wprintf( L"Character count: %llu\n", fileChCount );
	wchar_t *fString = calloc( ( fileChCount + 1 ), sizeof( wchar_t ) );

	fseek( test, 0, SEEK_SET );

	FILE *out = _wfopen( L"testout.txt", L"w" );
	wchar_t ch = 0;
	size_t curCh = 0;
	while ( ( ch = fgetwc( test ) ) != WEOF ) {
		fString[ curCh++ ] = ch;
		int chars = fwprintf( out, L"%s\n", fString );
		//NormalizeString( NormalizationC, fString, -1, NULL, 0 );
		//int normal = IsNLSDefinedString( COMPARE_STRING, 0, NULL, fString, -1 );
		//if ( !normal ) {
			printf( "Input unicode bad! " );
		//}

		printf( "Chars written: %i\n", chars );
	}
	return 0;
}