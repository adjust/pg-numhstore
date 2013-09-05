#include "array_add.h"

int key_value_compare( const void * a, const void * b )
{
    KeyValuePair * key1 = ( KeyValuePair * ) a;
    KeyValuePair * key2 = ( KeyValuePair * ) b;
    return strcmp( key1->key , key2->key );
}

void shrink_pairs(KeyValuePair * pairs, int new_size, int * old_size )
{
    if( new_size == * old_size )
        /* no need to shrink */
        return;
    KeyValuePair * swap;
    swap = ( KeyValuePair * ) palloc( new_size * sizeof( KeyValuePair ) );
    memcpy(swap, pairs, new_size * sizeof( KeyValuePair ) );
    pairs = swap;
    *old_size = new_size;
}

Datum array_add( PG_FUNCTION_ARGS )
{
    KeyValuePair * pairs, * result;
    int i = 0, j = 0, real_count = 0;
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
        if( key_nulls[i] )
        {
            continue;
        }
        size_t key_len = VARSIZE( key_data[i] ) - VARHDRSZ;
        char * current_key = ( char * ) palloc0( key_len );
        memcpy( current_key, VARDATA( key_data[i] ), key_len );
        pairs[real_count].key = current_key;
        pairs[real_count].key_len = key_len;
        pairs[real_count].value = DatumGetInt32( val_data[i] );
        ++real_count;
    }

    shrink_pairs( pairs, real_count, &key_count );
    qsort( pairs, key_count, sizeof( KeyValuePair ), key_value_compare );

    for( i = 0; i < key_count; ++i )
    {
        result[j].key     = pairs[i].key;
        result[j].value   = pairs[i].value;
        result[j].key_len = pairs[i].key_len;

        while(i < key_count -1 && pairs[i].key_len == pairs[i+1].key_len && strncmp(pairs[i].key, pairs[i+1].key, pairs[i].key_len ) == 0 )
        {
                result[j].value += pairs[++i].value;
        }
        ++j;
    }

    for( i = 0; i < j; i++ )
    {
        if( result[i].key == NULL )
            break;
        char * value_str;
        size_t val_len;
        val_len = adeven_add_get_digit_num( result[i].value );
        value_str = (char * ) palloc0 ( val_len );
        sprintf( value_str, "%ld", result[i].value );
        hPairs[i].key = result[i].key;
        hPairs[i].keylen = result[i].key_len;
        hPairs[i].val = value_str;
        hPairs[i].vallen = val_len;
        hPairs[i].isnull = false;
        hPairs[i].needfree = false;
        buflen += hPairs[i].keylen;
        buflen += hPairs[i].vallen;
    }

    out = hstorePairs( hPairs, i, buflen );
    PG_RETURN_POINTER( out );
}
