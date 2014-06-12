#include "hstore_array.h"

void HAArray_init(HAArray *a, size_t initial_size)
{
    a->entry = palloc0(initial_size * sizeof(HAEntry));
    a->used  = 0;
    a->size  = initial_size;
}

void HAArray_insert(HAArray *a, char *key, int len, long val)
{
    if(a->size == a->used) {
        HAEntry  *entry_swap = a->entry;
        a->size *= 2;
        a->entry = palloc0(a->size * sizeof(HAEntry));
        memcpy(a->entry, entry_swap, a->used * sizeof(HAEntry));
        pfree(entry_swap);
    }
    a->entry[a->used].key = key;
    a->entry[a->used].len = len;
    a->entry[a->used++].val = val;
}

int HAArray_cmp(const void *a, const void *b)
{
    HAEntry *_a = (HAEntry *)a;
    HAEntry *_b = (HAEntry *)b;
    if(_a->len < _b->len)
        return -1;
    else if (_a->len > _b->len)
        return 1;
    else
        return strncmp(_a->key, _b->key, _a->len);
}

void inline HAArray_sort(HAArray *a)
{
    qsort(a->entry, a->used, sizeof(HAEntry), HAArray_cmp);
}

Datum array_to_hstore(HAArray *a)
{
    int i, j = 0;
    Pairs *pairs;
    bool skip = false;
    HStore *out;
    int4 buflen = 0;
    pairs = palloc0(a->used * sizeof(Pairs));
    for (i = 0; i < a->used; ++i)
    {
        int digit_num;
        char *num_str;
        int local = i+1;

        while (HAArray_cmp(&a->entry[i], &a->entry[local]) == 0)
        {
            a->entry[i].val += a->entry[local++].val;
            skip = true;
        }
        digit_num = adeven_get_digit_num(a->entry[i].val);
        num_str = palloc0(digit_num+1);
        sprintf(num_str, "%ld", a->entry[i].val);
        pairs[j].key = a->entry[i].key;
        pairs[j].keylen = a->entry[i].len;
        pairs[j].val = num_str;
        pairs[j].vallen = digit_num;
        pairs[j].isnull = false;
        pairs[j].needfree = false;
        buflen += pairs[j].keylen;
        buflen += pairs[j++].vallen;
        if(skip)
        {
            i = --local;
            skip = false;
        }
    }
    out = hstorePairs(pairs, j, buflen);
    PG_RETURN_POINTER(out);
}

Datum hstore_array( PG_FUNCTION_ARGS )
{
    ArrayType  *input;
    Oid         type;
    int16       typlen;
    bool        typbyval;
    char        typalign;
    Datum      *data;
    bool       *nulls;
    int         count;
    int         i;
    HAArray     a;

    // working vars
    HStore *hstore;
    HEntry *entries;
    char *key;
    long val;
    char *base;
    size_t len;
    int hstore_count;
    int index;

    if (PG_ARGISNULL(0))
    {
            PG_RETURN_NULL();
    }
    input = PG_GETARG_ARRAYTYPE_P(0);
    type = ARR_ELEMTYPE(input);

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

    HAArray_init(&a, 200);
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
            HAArray_insert(&a, key, len, val);
        }
    }
    HAArray_sort(&a);
    return array_to_hstore(&a);
}
