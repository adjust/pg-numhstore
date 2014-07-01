#include "pg_numhstore.h"

PG_FUNCTION_INFO_V1(hstore_add);

// works on sorted hstores only, returns sorted result
// further idea for improvement: work on array of hstores and use heap to
// select minimum key (avoids serialization of many hstores)
Datum hstore_add(PG_FUNCTION_ARGS)
{
    int i,
        cmp,
        count1,
        count2,
        index1 = 0,
        index2 = 0;

    int4 buflen = 0;

    char   *base1,
           *base2,
           *key1,
           *key2;
    size_t  keylen1,
            keylen2;
    long    val1,
            val2;

    HStore *out,
           *hstore1,
           *hstore2;
    HEntry *entries1,
           *entries2;
    Pairs  *pairs;

    Array a;

    if (PG_ARGISNULL(0) && ! PG_ARGISNULL(1))
        PG_RETURN_POINTER( PG_GETARG_HS( 1 ) );
    else if (! PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_POINTER(PG_GETARG_HS(0));
    else if (PG_ARGISNULL(0) && PG_ARGISNULL(1))
        PG_RETURN_EMPTY_HSTORE;

    hstore1 = PG_GETARG_HS(0);
    hstore2 = PG_GETARG_HS(1);
    entries1 = ARRPTR(hstore1);
    entries2 = ARRPTR(hstore2);
    base1 = STRPTR(hstore1);
    base2 = STRPTR(hstore2);
    count1 = HS_COUNT(hstore1);
    count2 = HS_COUNT(hstore2);

    init_array(&a, 200);

    // merge both lists by appending the smaller key
    // or the sum of the values if the keys equal
    while (index1 < count1 && index2 < count2)
    {
        READ_PAIR(entries1, base1, index1, &key1, &val1, &keylen1);
        READ_PAIR(entries2, base2, index2, &key2, &val2, &keylen2);

        cmp = compare_array(key1, keylen1, key2, keylen2);
        if (cmp < 0)
        {
            insert_array(&a, key1, val1, (int) keylen1);
            index1 += 1;
        }
        else if (cmp > 0)
        {
            insert_array(&a, key2, val2, (int) keylen2);
            index2 += 1;
        }
        else
        {
            insert_array(&a, key1, val1 + val2, (int) keylen1);
            index1 += 1;
            index2 += 1;
        }
    }

    // finish by appending the longer list
    while (index1 < count1)
    {
        READ_PAIR(entries1, base1, index1, &key1, &val1, &keylen1);
        insert_array(&a, key1, val1, (int) keylen1);
        index1 += 1;
    }
    while (index2 < count2)
    {
        READ_PAIR(entries2, base2, index2, &key2, &val2, &keylen2);
        insert_array(&a, key2, val2, (int) keylen2);
        index2 += 1;
    }

    pairs = palloc0(a.used * sizeof *pairs);
    for (i = 0; i < a.used; ++i)
        ADD_PAIR(a.keys[i], a.sizes[i], a.vals[i], pairs, i, buflen);

    out = hstorePairs(pairs, a.used, buflen);
    free_array(&a);

    PG_RETURN_POINTER(out);
}

