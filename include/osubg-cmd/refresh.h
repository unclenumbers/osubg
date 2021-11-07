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
 *  "main menu backgrounds" : "gray"
 *  "nnnn whatever" : "gray"
 *  \t"number of bgs in set"
 *  \t"bg1"
 *  \t"bg2"
 *  \t"bgn"
 *  nnnn whatever: "gray"
 *  ...
 *  
 *  The file is parsed by reading quoted sections one by one
 * 
 *  the thumbnails are tied to the mapset, not each map.
 *  the main menu bgs are treated like one mapset.
 *  for each mapset: find all .osu files, read bg names of all of them, 
 */
#include "osubg-cmd/ofile.h"

#ifndef OSUBG_REFRESH
#define OSUBG_REFRESH

// Top-level function for the exposed refresh
int osubgRefresh( void );

typedef struct osubgMapset_t {
    char *mapsetTitle;
    int mode;
    size_t mapCount;
    char **mapBgNames;
} osubgMapset;

int orefreshGetMapsetArray( osubgMapset *arr, size_t *size );

int osubgRefreshReload( void );
int osubgRefreshReplace( void );

#endif