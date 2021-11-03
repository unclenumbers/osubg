// Operations for file I/O in the context of .osu files, mapset directories, and partial copies of .osu files.
// Also includes operations for the config file, osubg.cfg.
#include <stdio.h>
#include <stdint.h>
#include <wchar.h>

// Seeks to the position immediately after the [Events] header.
// Only searches after the current position.
// Returns zero if search was a failiure, nonzero otherwise.
int ofileSeekEventHeader( FILE *f );

// Gets the size of the string between the next pair of quotation marks,
// and the string itself if the char pointer supplied is not null.
// Returns size of the string in the length argument, and 0 on failiure.
// If the char pointer is null, the file seeks back to before the quotes
// so that it can be called again.
// Returns a nonzero value on success.
int ofileReadQuotes( FILE *f, uint32_t *length, char *str );

// Creates config file in [user]/AppData/Roaming/osubg.
// Returns 0 on failiure, and a nonzero value on success.
// This function also creates the osubg directory in appdata.
int ofileCreateConfigFile( void );