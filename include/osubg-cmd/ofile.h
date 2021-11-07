// Operations for file I/O in the context of .osu files, mapset directories, and partial copies of .osu files.
// Also includes operations for the config file, osubg.cfg.
#ifndef OSUBG_OFILE
#define OSUBG_OFILE

#include <stdio.h>
#include <stdint.h>

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

// Creates config file with base defaults in [user]/AppData/Roaming/osubg.
// Returns 0 on failiure, and a nonzero value on success.
// This function also creates the osubg directory in appdata.
int ofileCreateConfig( void );

typedef struct osubgConfig_t {
	char *osuPath;
	uint32_t mapsetCount;
	// The current mode is a string in the config file
	// for easier interpretation by the user--"normal" or "gray".
	// is easier to look at than "1" or "0", but those can be used internally.
	// 1 = gray, 0 = normal.
	int currentMode;
} osubgConfig;

// Gets a config object from the config file.
// Returns 0 on failiure, nonzero otherwise.
int ofileGetConfig( osubgConfig *cfg );

// Writes a config object to the config file.
// Returns 0 on failiure, nonzero otherwise.
int ofileSetConfig( osubgConfig *cfg );

// Destroys a config object.
// Returns 0 on failiure, nonzero otherwise.
int ofileDestroyConfig( osubgConfig *cfg );

#endif
