#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdint.h>
#include <stringapiset.h>

#include "osubg-cmd/ofile.h"

int seekEventHeader( FILE *f, wchar_t *filename ) {
	// [Events] = 8 chars
	wchar_t
		header[9] = { 0 },
		current = 0;

	return 0;
}