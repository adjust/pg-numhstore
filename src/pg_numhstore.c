#include "pg_numhstore.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

void AEArray_init( AEArray *a, size_t initial_size )
{
    int i = 0;
    a->array      = ( char ** )palloc0( initial_size * sizeof( char* ) );
    a->counts_str = ( char ** )palloc0( initial_size * sizeof( char* ) );
    a->used       = 0;
    a->size       = initial_size;
    a->counts     = ( long * )palloc0( initial_size * sizeof( long ) );
    a->sizes      = ( int * )palloc0( initial_size * sizeof( int ) );
    for( i = 0; i < a->size; ++i )
    {
        a->counts[i] = 0;
    }
}

void AEArray_insert( AEArray *a, char* elem, size_t elem_size )
{
    if( a->used == a->size )
    {
        char ** array_swap;
        char ** counts_str_swap;
        int * sizes_swap;
        long * count_swap;
        int i = a->size;
        a->size *= 2;

        array_swap = a->array;
        a->array = ( char ** )palloc0( a->size * sizeof( char* ) );
        memcpy( a->array, array_swap, sizeof( char* ) * i );
        pfree( array_swap );

        counts_str_swap = a->counts_str;
        a->counts_str = ( char ** )palloc0( a->size * sizeof( char* ) );
        memcpy( a->counts_str, counts_str_swap, sizeof( char* ) * i );
        pfree( counts_str_swap );

        count_swap = a->counts;
        a->counts = ( long * )palloc0( a->size * sizeof( long ) );
        memcpy( a->counts, count_swap, sizeof( int ) * i );
        pfree( count_swap );

        sizes_swap = a->sizes;
        a->sizes = ( int * )palloc0( a->size * sizeof( int ) );
        memcpy( a->sizes, sizes_swap, sizeof( int ) * i );
        pfree( sizes_swap );

        for( ; i < a->size; ++i )
        {
            a->counts[i] = 0;
            a->sizes[i]  = 0;
        }
    }
    a->sizes[a->used] = ( int ) elem_size;
    a->array[a->used++] = elem;
}

HStore * hstorePairs( Pairs *pairs, int4 pcount, int4 buflen )
{
    HStore     *out;
    HEntry     *entry;
    char       *ptr;
    char       *buf;
    int4        len;
    int4        i;

    len = CALCDATASIZE( pcount, buflen );
    out = palloc0( len );
    SET_VARSIZE( out, len );
    HS_SETCOUNT( out, pcount );

    if ( pcount == 0 )
        return out;

    entry = ARRPTR( out );
    buf = ptr = STRPTR( out );

    for( i = 0; i < pcount; i++ )
    {
        HS_ADDITEM( entry, buf, ptr, pairs[i] );
    }

    HS_FINALIZE( out, pcount, buf, ptr );

    return out;
}

HStore * hstoreUpgrade( Datum orig )
{
    HStore     *hs = ( HStore * ) PG_DETOAST_DATUM( orig );
    return hs;
}

size_t adeven_get_digit_num( long number )
{
    size_t count = 0;
    if( number == 0 )
        return 1;
    while( number != 0 )
    {
        number /= 10;
        ++count;
    }
    return count;
}

