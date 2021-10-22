// Operations for file I/O in the context of .osu files and partial copies of .osu files
#include <stdio.h>
#include <stdint.h>
#include <wchar.h>

// Seeks to the position immediately after the [Events] header.
// Only searches after the current position.
// Returns zero if search was a failiure or the file does not end in ".osu".
int seekEventHeader( FILE *f, wchar_t *filename );

// Gets the size of the filename of the background,
// and the name of the file if the pointer supplied is not null.
int getBgFilename( FILE *f, uint32_t *length, wchar_t *filename );