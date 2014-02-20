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
    KeyValuePair * result;
    int i = 0, j = 0;
    bool * key_nulls, * val_nulls;
    int key_count, val_count;
    int16 key_typlen, val_typlen;
    bool key_typbyval, val_typbyval;
    char key_typalign, val_typalign;

    Pairs * hPairs;
    HStore * out;
    int4 buflen = 0;
    AvlTree tree;
    AEArray array;

    tree = make_empty( NULL );
    AEArray_init( &array, 100 );

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

    result = ( KeyValuePair * ) palloc0( key_count * sizeof( KeyValuePair ) );
    hPairs = ( Pairs * ) palloc0( key_count * sizeof( Pairs ) );

    for( i = 0; i < key_count; ++i )
    {
        if( key_nulls[i] ) continue;

        Position position;
        size_t key_len = VARSIZE( key_data[i] ) - VARHDRSZ;
        char * current_key = ( char * ) palloc0( key_len +1 );
        memcpy( current_key, VARDATA( key_data[i] ), key_len );
        current_key[key_len] = '\0';
        long current_value = DatumGetInt32( val_data[i] );
        if( current_value == 0 )
            continue;
        position = find( current_key, key_len, tree );
        if( position == NULL )
        {
            j = array.used;
            tree = insert( current_key, key_len, j, tree );
            AEArray_insert( &array, current_key, key_len );
        }
        else
        {
            j = value( position );
        }

        array.counts[j] += current_value;
    }

    // save sort permutation to create pairs in order of ascending keys
    // we assume that postgres stores the pairs in that order
    int * perm;
    perm = ( int * ) palloc ( array.used * sizeof( int ) );
    sort_perm( tree, perm );

    make_empty( tree );

    hPairs = palloc0( array.used * sizeof( Pairs ) );
    for( i = 0; i < array.used; i++ )
    {
        j = perm[i];
        if( array.array[j] == NULL ) break;

        int digit_num = adeven_get_digit_num( array.counts[j] );
        char * dig_str = (char * ) palloc0 ( digit_num + 1 );
        dig_str[digit_num] = '\0';
        sprintf( dig_str, "%ld", array.counts[j] );
        hPairs[i].key = array.array[j];
        hPairs[i].keylen = array.sizes[j];
        hPairs[i].val = dig_str;
        hPairs[i].vallen = digit_num;
        hPairs[i].isnull = false;
        hPairs[i].needfree = false;
        buflen += hPairs[i].keylen;
        buflen += hPairs[i].vallen;
    }

    out = hstorePairs( hPairs, i, buflen );
    PG_RETURN_POINTER( out );
}
