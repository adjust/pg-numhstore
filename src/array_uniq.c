#include "array_uniq.h"

Datum * adeven_uniq_radix_uniq( Datum *data, int n, int * dims )
{
    Datum * c;
    int i, j, m = 0, exp = 1;
    int * a = palloc0( n * sizeof( n ) );
    int * b = palloc0( n * sizeof( n ) );
    for( i = 0; i < n; i++ )
    {
        a[i] = DatumGetInt32( data[i] );
        if ( a[i] > m )
        {
            m = a[i];
        }
    }

    while( m / exp > 0 )
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

    m = 0;

    for( i = 0, j = i + 1; j <= n; j++ )
    {
        if( a[i]==a[j] )
        {
            continue;
        }
        b[m++]=a[i];
        i=j;
    }

    c = ( Datum* )palloc0( m * sizeof( Datum ) );
    for( i = 0; i < m; i++ )
    {
        c[i] = Int32GetDatum( ( int32 )b[i] );
    }
    *dims = m;
    pfree( a );
    pfree( b );
    return c;
}

Datum array_uniq( PG_FUNCTION_ARGS )
{
    ArrayType * input;
    Datum * i_data;
    Datum * uniq;

    Oid i_eltype;
    int16 i_typlen;
    bool  i_typbyval;
    char  i_typalign;
    bool * nulls;
    int n;

    int ndims, *dims, *lbs;
    ArrayType *result;


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

    ndims = ARR_NDIM( input );
    dims  = ARR_DIMS( input );
    lbs   = ARR_LBOUND( input );

    uniq = adeven_uniq_radix_uniq( i_data, n, dims );
    result = construct_md_array(
            ( void * )uniq,
            NULL,
            ndims,
            dims,
            lbs,
            i_eltype,
            i_typlen,
            i_typbyval,
            i_typalign
    );
    pfree( uniq );
    PG_RETURN_POINTER( result );
}
