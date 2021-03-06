#include "array_count.h"

size_t hstoreCheckKeyLen( size_t len )
{
    if( len > HSTORE_MAX_KEY_LEN )
        ereport( ERROR,
                ( errcode( ERRCODE_STRING_DATA_RIGHT_TRUNCATION ),
                  errmsg( "string too long for hstore key" ) ) );
    return len;
}

HStore * adeven_count_text_array( Datum* i_data, int n, bool * nulls )
{
    int i, j;
    int * perm;
    AvlTree tree;
    Pairs * pairs;

    int4 buflen = 0;

    HStore * out;
    AEArray a;
    AEArray_init( &a, 100 );
    tree = make_empty( NULL );

    for( i = 0; i < n; ++i )
    {
        if( ! nulls[i] )
        {
            Position position;
            size_t datum_len = VARSIZE( i_data[i] ) - VARHDRSZ;
            char * current_datum = ( char * ) palloc ( datum_len );
            memcpy( current_datum, VARDATA( i_data[i] ), datum_len );

            position = find( current_datum, datum_len, tree );
            if( position == NULL )
            {
                j = a.used;
                tree = insert( current_datum, datum_len, j, tree );
                AEArray_insert( &a, current_datum, datum_len );
            }
            else
            {
                j = value( position );
            }

            a.counts[j] += 1;
        }
    }

    // save sort permutation to create pairs in order of ascending keys
    // we assume that postgres stores the pairs in that order
    perm = ( int * ) palloc ( a.used * sizeof( int ) );
    sort_perm( tree, perm );

    make_empty( tree );

    pairs = palloc0( a.used * sizeof( Pairs ) );
    for( i = 0; i < a.used; ++i )
    {
        j = perm[i];
        if( a.array[j] != NULL )
        {
            size_t datum_len = a.sizes[j];
            int digit_num = adeven_get_digit_num( a.counts[j] );
            char * dig_str = palloc0( digit_num );
            sprintf( dig_str, "%ld", a.counts[j] );
            a.counts_str[j] = dig_str;
            pairs[i].key = a.array[j];
            pairs[i].keylen =  datum_len;
            pairs[i].val = dig_str;
            pairs[i].vallen =  digit_num;
            pairs[i].isnull = false;
            pairs[i].needfree = false;
            buflen += pairs[i].keylen;
            buflen += pairs[i].vallen;
        }
    }
    out = hstorePairs( pairs, a.used, buflen );
    //adeven_count_free_array( &a );
    return out;
}
HStore * adeven_count_int_array( Datum* i_data, int n, bool * nulls )
{
    int i = 0, j = 0, biggest = 0;
    int * a = NULL;
    int * b = NULL;
    int * c = NULL;
    int exp = 1;
    int m = 0;
    int notNullCnt = 0;
    Pairs * pairs = NULL;
    HStore * out = NULL;
    int4 buflen = 0;

    if( n == 1 ) {
        int value, digit_key_num;
        char *dig_key_str, *dig_val_str;
        pairs = palloc0( sizeof( Pairs ) );
        value = DatumGetInt32( i_data[0] );
        digit_key_num = adeven_get_digit_num( value );
        dig_key_str = palloc0( digit_key_num );
        dig_val_str = palloc0( 1 );
        sprintf( dig_key_str, "%d", value );
        sprintf( dig_val_str, "%d", 1 );
        pairs[0].key = dig_key_str;
        pairs[0].keylen =  digit_key_num;
        pairs[0].val = dig_val_str;
        pairs[0].vallen =  1;
        pairs[0].isnull = false;
        pairs[0].needfree = false;
        buflen += pairs[0].keylen;
        buflen += pairs[0].vallen;
        out = hstorePairs( pairs, n, buflen );
        return out;
    }


    a = palloc( sizeof( int ) * ( n + 1 ) );
    for( i=0;i<n+1;++i)
        a[i] = -1;
    b = palloc0( sizeof( int ) * n );
    c = palloc0( sizeof( int ) * n );

    for( i = 0; i < n; ++i )
    {
        if( !nulls[i] )
        {
            a[notNullCnt] = DatumGetInt32( i_data[i] );
            if ( a[notNullCnt] < 0 ) {
                elog( ERROR, "cannot count int_array that has negative integers" );
            }
            if( a[notNullCnt] > biggest )
            {
                biggest = a[notNullCnt];
            }
            ++notNullCnt;
        }
    }

    n = notNullCnt;

    while( biggest / exp > 0 )
    {
        int box[10] = { 0 };
        for( i = 0; i < n; i++ )
        {
            box[a[i] / exp % 10]++;
        }
        for( i = 1; i < 10; i++ )
        {
            box[i] += box[i - 1];
        }
        for( i = n - 1; i >= 0; i-- )
        {
            b[--box[a[i] / exp % 10]] = a[i];
        }
        for( i = 0; i < n; i++ )
        {
            a[i] = b[i];
        }
        exp *= 10;
    }

    for( i = 0; i < n; ++i )
    {
        b[i] = 0;
    }

    for( i = 0, j = i + 1; j <= n; j++ )
    {
        if( a[i]==a[j] )
        {
            b[m] = a[i];
            if( c[m] == 0 )
            {
                c[m]++;
            }
            c[m]++;
            continue;
        }
        if( c[m] == 0 )
        {
            c[m]  = 1;
        }
        b[m++]=a[i];
        i=j;
    }

    pairs = palloc0( m * sizeof( Pairs ) );

    for( i = 0; i < m; ++i )
    {
        int digit_key_num = adeven_get_digit_num( b[i] );
        int digit_val_num = adeven_get_digit_num( c[i] );
        char * dig_key_str = palloc0( digit_key_num );
        char * dig_val_str = palloc0( digit_val_num );
        sprintf( dig_key_str, "%d", b[i] );
        sprintf( dig_val_str, "%d", c[i] );
        pairs[i].key = dig_key_str;
        pairs[i].keylen =  digit_key_num;
        pairs[i].val = dig_val_str;
        pairs[i].vallen =  digit_val_num;
        pairs[i].isnull = false;
        pairs[i].needfree = false;
        buflen += pairs[i].keylen;
        buflen += pairs[i].vallen;
    }
    out = hstorePairs( pairs, m, buflen );
    return out;
}


Datum array_count( PG_FUNCTION_ARGS )
{
    bool * nulls;
    int n;
    int16 i_typlen;
    bool  i_typbyval;
    char  i_typalign;

    ArrayType * input;
    Datum * i_data;

    Oid i_eltype;
    if( PG_ARGISNULL( 0 ) )
    {
        PG_RETURN_NULL();
    }

    input = PG_GETARG_ARRAYTYPE_P( 0 );


    i_eltype = ARR_ELEMTYPE( input );


    get_typlenbyvalalign(
            i_eltype,
            &i_typlen,
            &i_typbyval,
            &i_typalign
            );


    deconstruct_array(
            input,
            i_eltype,
            i_typlen,
            i_typbyval,
            i_typalign,
            &i_data,
            &nulls,
            &n
            );

    if( n == 0 || ( n == 1 && nulls[0] ) )
        PG_RETURN_NULL();

    switch( i_eltype )
    {
        case INT4OID:
            PG_RETURN_POINTER( adeven_count_int_array( i_data, n, nulls ) );
            break;
        case TEXTOID:
            PG_RETURN_POINTER( adeven_count_text_array( i_data, n, nulls ) );
            break;
        default:
            elog( ERROR, "INVALID input data type" );
            break;
    }
    PG_RETURN_NULL();
}

