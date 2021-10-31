/**
 * osubg needs 3 main commands: refresh, sync, and swap. 
 * refresh simply updates the list of mapsets on file
 * sync switches all maps to the current mode (this will NOT protect against custom user updates)
 * swap changes modes and switches all bgs
 * 
 * folder structure:
 * 	config file (has mapset folder path, current mode, mapset count)
 * 	exact copy of dir names of mapset folder
 * 		each of these dirs has a mode file inside for tracking purposes
 * 	troll ecchi image??
 *  png and jpg gray bgs
 * 
 *  .osu files have bg lines, I can just replace the bg entry with my "graybg.png"
 *  can also gen unique names if needed
 * 
 * The general idea is to:
 *  Copy all directories from Songs folder
 *  copy all .osu files into folders, but only with bg filenames and the current mode (gray or normal).
 *  maintain map count, print statements to console or gui.
 */

int osubgRefresh( void );