#ifndef ADEVEN_PG_NUMHSTORE
#define ADEVEN_PG_NUMHSTORE

#include "postgres.h"
#include "hstore.h"
#include "fmgr.h"
#include "avltree.h"
#include <string.h>
#include <utils/array.h>
#include <stdlib.h>
#include <catalog/pg_type.h>

typedef struct {
    char ** array;
    char ** counts_str;
    size_t  used;
    size_t  size;
    long *  counts;
    int *   sizes;
} AEArray;

void AEArray_init( AEArray *a, size_t initial_size );
void AEArray_insert( AEArray *a, char* elem, size_t elem_size );

HStore * hstorePairs( Pairs *pairs, int4 pcount, int4 buflen );
HStore * hstoreUpgrade( Datum orig );
size_t adeven_get_digit_num( long number );
extern void get_typlenbyvalalign( Oid eltype, int16 * i_typlen, bool * i_typbyval, char * i_typalign );

#endif
