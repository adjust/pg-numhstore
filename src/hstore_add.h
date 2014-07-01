#include "pg_numhstore.h"

typedef struct {
    char  **keys;
    char  **vstr;
    long   *vals;
    int    *sizes;
    size_t  used;
    size_t  size;
    bool   *found;
} Array;

void adeven_add_init_array( Array *a, size_t initial_size );
void adeven_add_insert_array( Array *a, char * key, long val, int elem_size );
void adeven_add_insert_array( Array *a, char * key, long val, int elem_size );
void adeven_add_free_array( Array *a );
int adeven_add_min( int a, int b );
int adeven_add_compare( char * key1, int keylen1, char * key2, int keylen2 );

PG_FUNCTION_INFO_V1( hstore_add );

Datum hstore_add( PG_FUNCTION_ARGS );
