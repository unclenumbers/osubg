/* This file contains the prototypes for osubg's internal hash table for strings.
 * No delete function provided because delete operations do not exist in this context.
 * Rehashing is also not provided as the maximum size of the table will be known before its creation.
 * The hash table is only a search structure that only ever gets added to.
 * The table uses linear (how stoopid) probing for collisions.
 * 
 * Honestly, this is closer to an array than a full hash table.
 * No deletion, internal table size, rehashing, none of that.
 * It's really just an array with a thin skin and an included hashing function.
 */ 

#ifndef OSUBG_OHASHTABLE
#define OSUBG_OHASHTABLE

#include <stddef.h>
#include <stdint.h>

typedef struct osubgHashTable_t {
    char **table;
    uint64_t tableSize;
} osubgHashTable;

// Returns NULL on failure. This table will probably be massive,
// so remember to destroy it!!!
osubgHashTable *oHashTableCreate( size_t tableSize );

// Uses the java method for hash codes.
uint64_t oHashTableHash( char *str );

// Returns a nonzero value if no index was found, zero on success.
int oHashTableAdd( osubgHashTable *table, char *str );

// Returns a nonzero value if search fails, zero on success.
int oHashTableSearch( osubgHashTable *table, char *str );

// Does not free the strings in the table, only the array of strings.
void osubgDestroyHashTable( osubgHashTable **table );

#endif