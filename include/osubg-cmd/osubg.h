// Operations that deal with osubg commands and general flow.
// So far, the commands are:
//  help: command usage and a small guide
//  refresh: gets new maps from the specified osu! folder.
//  sync: syncs all maps with current state, uses refresh internally.
//  swap: syncs state of maps, then swaps all backgrounds.
//  setfolder: sets the location of the osu! folder.
//
// each command gets its own file.

#ifndef OSUBG_OSUBG
#define OSUBG_OSUBG
#endif

#include <wchar.h>

int osubgExecuteCommand( wchar_t **argv, int argc );