#include "array_add.h"

int key_value_compare( const void * a, const void * b )
{
    KeyValuePair * key1 = ( KeyValuePair * ) a;
    KeyValuePair * key2 = ( KeyValuePair * ) b;
    return strcmp( key1->key , key2->key );
}

Datum array_add( PG_FUNCTION_ARGS )
{
    KeyValuePair * pairs, * result;
    int i = 0, j = 0;
    bool * key_nulls, * val_nulls;
    int key_count, val_count;
    int16 key_typlen, val_typlen;
    bool key_typbyval, val_typbyval;
    char key_typalign, val_typalign;

    Pairs * hPairs;
    HStore * out;
    int4 buflen = 0;

    ArrayType * input_keys, * input_vals;
    Datum * key_data, * val_data;
    Oid key_type, val_type;

    if( PG_ARGISNULL( 0 ) || PG_ARGISNULL( 1 ) )
    {
        /* TODO: print warnings */
        PG_RETURN_NULL();
    }

    input_keys = PG_GETARG_ARRAYTYPE_P( 0 );
    input_vals = PG_GETARG_ARRAYTYPE_P( 1 );

    key_type = ARR_ELEMTYPE( input_keys );
    val_type = ARR_ELEMTYPE( input_vals );

    get_typlenbyvalalign(
            key_type,
            &key_typlen,
            &key_typbyval,
            &key_typalign
            );

    get_typlenbyvalalign(
            val_type,
            &val_typlen,
            &val_typbyval,
            &val_typalign
            );

    deconstruct_array(
            input_keys,
            key_type,
            key_typlen,
            key_typbyval,
            key_typalign,
            &key_data,
            &key_nulls,
            &key_count
            );

    deconstruct_array(
            input_vals,
            val_type,
            val_typlen,
            val_typbyval,
            val_typalign,
            &val_data,
            &val_nulls,
            &val_count
            );

    if( key_count != val_count )
        elog( ERROR, "key and value error have different lenghts" );

    pairs = ( KeyValuePair * ) palloc0( key_count * sizeof( KeyValuePair ) );
    result = ( KeyValuePair * ) palloc0( key_count * sizeof( KeyValuePair ) );
    hPairs = ( Pairs * ) palloc0( key_count * sizeof( Pairs ) );

    for( i = 0; i < key_count; ++i )
    {
        size_t key_len = VARSIZE( key_data[i] ) - VARHDRSZ;
        char * current_key = ( char * ) palloc0( key_len );
        memcpy( current_key, VARDATA( key_data[i] ), key_len );
        pairs[i].key = current_key;
        pairs[i].key_len = key_len;
        pairs[i].value = DatumGetInt32( val_data[i] );
    }

    qsort( pairs, key_count, sizeof( KeyValuePair ), key_value_compare );

    for( i = 0; i < key_count; ++i )
    {
        char * value_str;
        int val_len;
        char * current_key = pairs[i].key;
        int current_value = pairs[i].value;
        int current_key_len = pairs[i].key_len;
        while( strcmp(pairs[i].key, pairs[i+1].key ) == 0 )
        {
            current_value += pairs[++i].value;
            if( i == key_count - 1 )
                break;
        }
        val_len = adeven_add_get_digit_num( current_value );
        value_str = (char * ) palloc0 ( val_len );
        sprintf( value_str, "%ld", current_value );
        hPairs[j].key = current_key;
        hPairs[j].keylen = current_key_len;
        hPairs[j].val = value_str;
        hPairs[j].vallen = val_len;
        hPairs[j].isnull = false;
        hPairs[j].needfree = false;
        buflen += hPairs[j].keylen;
        buflen += hPairs[j++].vallen;
    }

    out = hstorePairs( hPairs, j, buflen );
    PG_RETURN_POINTER( out );
}
