// Operations that deal with osubg commands and general flow.
// So far, the commands are:
//  help: command usage and a small guide
//  refresh: gets new maps from the specified osu! folder.
//   also forces reloading of all gray bgs everywhere, to support
//   custom images (will require recompile).
//  sync: syncs all maps with current state, uses a stripped-down refresh internally.
//  swap: syncs state of maps, then swaps all backgrounds.
//  setfolder: sets the location of the osu! folder.
//
// each command gets its own file.

#ifndef OSUBG_OSUBG
#define OSUBG_OSUBG


#include <wchar.h>

// The entry point for the osubg command interpreter.
// This function will also be used for calls from the GUI
// when that gets implemented.
// Can take up to 3 arguments, 2 for most commands.
// The only command that gets 3 is setfolder.
int osubgExecuteCommand( wchar_t **argv, int argc );

#endif