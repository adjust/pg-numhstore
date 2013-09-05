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

HStore * hstorePairs( Pairs *pairs, int4 pcount, int4 buflen );
HStore * hstoreUpgrade( Datum orig );
size_t adeven_add_get_digit_num( long number );
extern void get_typlenbyvalalign( Oid eltype, int16 * i_typlen, bool * i_typbyval, char * i_typalign );

#endif
