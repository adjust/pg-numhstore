#include "pg_numhstore.h"

typedef struct {
    char * key;
    size_t key_len;
    long value;
} KeyValuePair;

int key_value_compare( const void * a, const void * b );
void shrink_pairs(KeyValuePair * pairs, int new_size, int * old_size );

PG_FUNCTION_INFO_V1( array_add );
Datum array_add( PG_FUNCTION_ARGS );
