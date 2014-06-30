#include "hstore_array.h"

Datum array_to_hstore(Datum *data, int count, bool *nulls)
{
    HStore *hstore;
    HStore *out;
    HEntry *entries;
    char   *key,
           *base;
    long    val;
    size_t  len;
    int     hstore_count,
            index,
            i,
            n,
            j = 0;
    Pairs   *pairs;
    bool     skip   = false;
    int4     buflen = 0;
    AvlTree  tree;
    Position position;

    tree = make_empty(NULL);
    for (i = 0; i < count; ++i)
    {
        if (nulls[i])
            continue;
        hstore = (HStore *) data[i];
        entries = ARRPTR(hstore);
        base = STRPTR(hstore);
        hstore_count = HS_COUNT(hstore);
        for (index = 0; index < hstore_count; ++index)
        {
            adeven_add_read_pair(entries, base, index, &key, &val, &len);
            position = find(key, len, tree);
            if (position == NULL)
                tree = insert(key, len, val, tree);
            else
                position->value += val;
        }
    }
    n = tree_length(tree);
    pairs = palloc0(n * sizeof *pairs);
    tree_to_pairs(tree, pairs, &buflen, 0);
    out = hstorePairs(pairs, n, buflen);
    PG_RETURN_POINTER(out);
}

Datum hstore_array(PG_FUNCTION_ARGS)
{
    ArrayType  *input;
    Oid         type;
    int16       typlen;
    bool        typbyval;
    char        typalign;
    Datum      *data;
    bool       *nulls;
    int         count;

    if (PG_ARGISNULL(0))
        PG_RETURN_NULL();

    input = PG_GETARG_ARRAYTYPE_P(0);
    type  = ARR_ELEMTYPE(input);

    get_typlenbyvalalign(
        type,
        &typlen,
        &typbyval,
        &typalign
    );

    deconstruct_array(
        input,
        type,
        typlen,
        typbyval,
        typalign,
        &data,
        &nulls,
        &count
    );

    return array_to_hstore(data, count, nulls);
}

Datum hstore_array_finalfn(PG_FUNCTION_ARGS)
{
    ArrayBuildState *input;
    Datum           *data;
    bool            *nulls;
    int              count;

    if (PG_ARGISNULL(0))
        PG_RETURN_NULL();

    Assert(AggCheckCallContext(fcinfo, NULL));

    input = (ArrayBuildState *) PG_GETARG_POINTER(0);
    count = input->nelems;
    nulls = input->dnulls;
    data  = input->dvalues;

    return array_to_hstore(data, count, nulls);
}
