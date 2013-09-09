#include "pg_numhstore.h"

extern void get_typlenbyvalalign( Oid eltype, int16 * i_typlen, bool * i_typbyval, char * i_typalign );

PG_FUNCTION_INFO_V1( array_count );
Datum array_count( PG_FUNCTION_ARGS );
HStore * adeven_count_int_array( Datum* i_data, int n, bool * nulls );
HStore * adeven_count_text_array( Datum* i_data, int n, bool * nulls );
