#include "pg_numhstore.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

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

size_t adeven_add_get_digit_num( long number )
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

