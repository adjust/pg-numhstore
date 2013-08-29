#include "postgres.h"
#include "hstore.h"
#include "avltree.h"
#include "fmgr.h"
#include <string.h>
#include <utils/array.h>
#include <catalog/pg_type.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

typedef struct {
    char ** array;
    char ** counts_str;
    size_t  used;
    size_t  size;
    int *   counts;
    int *   sizes;
} adeven_count_Array;

extern void get_typlenbyvalalign( Oid eltype, int16 * i_typlen, bool * i_typbyval, char * i_typalign );

PG_FUNCTION_INFO_V1( array_count );
Datum array_count( PG_FUNCTION_ARGS);
void adeven_count_init_array( adeven_count_Array *a, size_t initial_size );
void adeven_count_insert_array(adeven_count_Array *a, char* elem, size_t elem_size );
int adeven_count_get_digit_num( int number );
HStore * adeven_count_int_array( Datum* i_data, int n, bool * nulls );
HStore * adeven_count_text_array( Datum* i_data, int n, bool * nulls );
void adeven_count_free_array( adeven_count_Array *a );
