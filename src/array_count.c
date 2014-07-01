#include "pg_numhstore.h"

PG_FUNCTION_INFO_V1(array_count);

HStore*
adeven_count_text_array(Datum *i_data, int n, bool *nulls)
{
    AvlTree  tree;
    Pairs   *pairs;
    char    *key;
    int4     buflen = 0;
    Position position;
    size_t   len;
    int      i;

    tree = make_empty(NULL);
    for (i = 0; i < n; ++i)
    {
        if (nulls[i])
            continue;
        DATUM_TO_CSTRING(i_data[i], key, len);
        position = find(key, len, tree);
        if (position == NULL)
            tree = insert(key, len, 1, tree);
        else
            position->value += 1;
    }
    n = tree_length(tree);
    pairs = palloc0(n * sizeof *pairs);
    tree_to_pairs(tree, pairs, &buflen, 0);
    return hstorePairs( pairs, n, buflen );
}

HStore*
adeven_count_int_array(Datum *i_data, int n, bool *nulls)
{
    AvlTree  tree;
    Pairs   *pairs;
    Position position;
    int      key,
             i;
    int4     buflen = 0;

    tree = make_empty(NULL);

    for (i = 0; i < n; ++i)
    {
        if (nulls[i])
            continue;
        key = DatumGetInt32(i_data[i]);
        if (key < 0)
            elog(ERROR, "cannot count int_array that has negative integers");
        position = int_find(key, tree);
        if (position == NULL)
            tree = int_insert(key, 1, tree);
        else
            position->value += 1;
    }
    n = tree_length(tree);
    pairs = palloc0(n * sizeof *pairs);
    int_tree_to_pairs(tree, pairs, &buflen, 0);
    return hstorePairs(pairs, n, buflen);
}

Datum
array_count(PG_FUNCTION_ARGS)
{
    ArrayType *input;
    Datum     *i_data;
    bool      *nulls;
    int        n;
    int16      i_typlen;
    bool       i_typbyval;
    char       i_typalign;
    Oid        i_eltype;

    if (PG_ARGISNULL(0))
        PG_RETURN_NULL();

    input = PG_GETARG_ARRAYTYPE_P(0);

    i_eltype = ARR_ELEMTYPE(input);

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

    if (n == 0 || (n == 1 && nulls[0]))
        PG_RETURN_NULL();

    switch(i_eltype)
    {
        case INT4OID:
            PG_RETURN_POINTER(adeven_count_int_array(i_data, n, nulls));
            break;
        case TEXTOID:
            PG_RETURN_POINTER(adeven_count_text_array(i_data, n, nulls));
            break;
        default:
            elog(ERROR, "INVALID input data type");
            break;
    }
    PG_RETURN_NULL();
}

