#include "pg_numhstore.h"

PG_FUNCTION_INFO_V1(array_add);

Datum
array_add(PG_FUNCTION_ARGS)
{
    Datum     *key_data,
              *val_data;
    Oid        key_type,
               val_type;
    ArrayType *input_keys,
              *input_vals;
    bool      *key_nulls,
              *val_nulls;
    int        key_count,
               val_count,
               i = 0,
               n;
    int16      key_typlen,
               val_typlen;
    bool       key_typbyval,
               val_typbyval;
    char       key_typalign,
               val_typalign;
    Pairs     *pairs;
    HStore    *out;
    int4       buflen = 0;
    AvlTree    tree;
    /*
     * working vars
     */
    Position position;
    size_t len;
    char * key;
    long value;

    tree = make_empty(NULL);

    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    input_keys = PG_GETARG_ARRAYTYPE_P(0);
    input_vals = PG_GETARG_ARRAYTYPE_P(1);

    key_type = ARR_ELEMTYPE(input_keys);
    val_type = ARR_ELEMTYPE(input_vals);

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

    if (key_count != val_count)
        elog(ERROR, "key and value error have different lenghts");

    for (i = 0; i < key_count; ++i)
    {
        if (key_nulls[i])
            continue;
        DATUM_TO_CSTRING(key_data[i], key, len);
        if (val_nulls[i])
            continue;
        value = DatumGetInt32(val_data[i]);
        position = find(key, len, tree);
        if (position == NULL)
            tree = insert(key, len, value, tree);
        else
            position->value += value;
    }

    n = tree_length(tree);
    pairs = palloc0(n * sizeof(Pairs));
    tree_to_pairs(tree, pairs, &buflen, 0);
    out = hstorePairs(pairs, n, buflen);
    make_empty(tree);
    PG_RETURN_POINTER(out);
}
