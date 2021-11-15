/**
 * osubg needs 3 main commands: refresh, sync, and swap. 
 * refresh simply updates the list of mapsets on file and using it directly forces reload of bgs
 * sync switches all maps to the current mode (this will NOT protect against custom user updates)
 * swap changes modes and switches all bgs, calls sync FIRST.
 * sync does not need to be a different command on the outside since it's essential to the use case.
 * 
 * folder structure:
 * 	config file (has mapset folder path, current mode, mapset count)
 * 	exact copy of dir names of mapset folder in a text file
 *  after, a quoted "normal" or "gray" ( nnnnn whatever : "1" )
 * 	troll ecchi image?? actually maybe I can embed images in the .exe now
 *  png and jpg gray bgs and a small jpg for the thumbnails
 *  
 *  ~~~mapsets.txt~~~
 *  "main menu backgrounds" : "gray/normal" : "id"
 *  "0"
 *  "nnnn whatever" : "gray/normal" : "id"
 *  \t"n"
 *  \t"bg1"
 *  \t"bg2"
 *  \t"bgn"
 *  "nnnn whatever": "gray"
 *  ...
 *  
 *  id for unsubmitted map is -1
 *  id for main menu bgs is -2
 * 
 *  The file is parsed by reading quoted sections one by one
 * 
 *  the thumbnails are tied to the mapset, not each map.
 *  the main menu bgs are treated like one mapset, with the name "main menu backgrounds".
 *  for each mapset: find all .osu files, read bg names of all of them, 
 */
#include "osubg-cmd/ofile.h"
#include "osubg-cmd/ohashtable.h"

#ifndef OSUBG_REFRESH
#define OSUBG_REFRESH

// Top-level function for the exposed refresh
int osubgRefresh( void );

typedef struct osubgMapset_t {
    char
        *mapsetTitle,
        *mapsetId;

    int mode;
    size_t mapCount;
    char **mapBgNames;
} osubgMapset;

typedef enum osuMapsetFlag_t {

    MAPSET_NAME = 0,
    MAPSET_MODE = 1,
    MAPSET_ID = 2,
    MAPSET_BG_COUNT = 3,
    MAPSET_FLAG_MODULO = 4,

} osubgMapsetFlag;

// Returns a nonzero value if array has no length (failure), 0 on success.
// Gets all mapsets from mapsets.txt.
int orefreshGetMapsetArray( osubgMapset **arr, size_t *size );

// Returns a nonzero value on failure, 0 on success.
// Sets the information in mapsets.txt.
int orefreshSetMapsetsFile( osubgMapset *arr, size_t size );

// Returns a nonzero value on failure, 0 on success.
// Gets all the mapsets in the mapsets folder into an array.
// By the way, the count of mapsets may not be fully accurate (multiple downloads of mapsets)
// and the number of "maps" is actually the number of unique backgrounds (which may also no be fully accurate).
int orefreshReadMapsDirectory( osubgMapset **arr, size_t *size );

// Returns nonzero value on failure, zero on success.
int osubgRefreshReload( void );

// Returns nonzero value on failure, zero on success.
int osubgRefreshReplace( void );

#endif