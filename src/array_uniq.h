#include "postgres.h"
#include "fmgr.h"
#include <string.h>
#include <utils/array.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1( array_uniq );
Datum array_uniq( PG_FUNCTION_ARGS );
extern void get_typlenbyvalalign( Oid eltype, int16 * i_typlen, bool * i_typbyval, char * i_typalign );

Datum * adeven_uniq_radix_uniq( Datum *data, int n, int * dims );
