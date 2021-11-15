#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#include "osubg-cmd/ohashtable.h"

osubgHashTable *oHashTableCreate( size_t tableSize ) {
    osubgHashTable *table = malloc( sizeof( osubgHashTable ) );
    if ( table == NULL )
        return NULL;
    table->table = calloc( tableSize, sizeof( char * ) );
    table->tableSize = tableSize;

    if ( table->table == NULL )
        return NULL;

    return table;
}

uint64_t oHashtableHash( char *str ) {
    uint64_t sum = 0;
    size_t len = strlen( str );

    for ( size_t i = 0; i < len; i++ )
        sum += ( uint64_t )str[i] * pow( 31, ( len - 1 - i ) );

    return sum;
}

int oHashTableAdd( osubgHashTable *table, char *str ) {
    uint64_t hash = oHashtableHash( str ) % table->tableSize;
    
    while ( table->table[ hash ] != NULL )
        hash++;

    table->table[ hash ] = str;
    return 0;
}

int oHashTableSearch( osubgHashTable *table, char *str ) {
    uint64_t hash = oHashtableHash( str ) % table->tableSize;
    while ( table->table[ hash ] != NULL ) {
        if ( !strcmp( table->table[ hash++ ], str ) )
            return 0;
    }

    return 1;
}

void osubgDestroyHashTable( osubgHashTable **table ) {
    if ( *table == NULL )
        return;

    free( ( *table )->table );
    free( *table );
    *table = NULL;
}