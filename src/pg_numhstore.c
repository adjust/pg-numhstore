#include "pg_numhstore.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

HStore*
hstorePairs(Pairs *pairs, int4 pcount, int4 buflen)
{
    HStore *out;
    HEntry *entry;
    char   *ptr;
    char   *buf;
    int4    len;
    int4    i;

    len = CALCDATASIZE(pcount, buflen);
    out = palloc0(len);
    SET_VARSIZE(out, len);
    HS_SETCOUNT(out, pcount);

    if ( pcount == 0 )
        return out;

    entry = ARRPTR(out);
    buf = ptr = STRPTR(out);

    for (i = 0; i < pcount; i++)
        HS_ADDITEM(entry, buf, ptr, pairs[i]);

    HS_FINALIZE(out, pcount, buf, ptr);
    return out;
}

HStore*
hstoreUpgrade(Datum orig)
{
    HStore *hs = (HStore *) PG_DETOAST_DATUM(orig);
    return hs;
}

size_t
hstoreCheckKeyLen(size_t len)
{
    if (len > HSTORE_MAX_KEY_LEN)
        ereport(ERROR,
                (errcode(ERRCODE_STRING_DATA_RIGHT_TRUNCATION),
                  errmsg("string too long for hstore key")));
    return len;
}

inline size_t
width_as_string(long number)
{
    size_t count = 0;
    if (number == 0 || number < 0)
        ++count;
    while (number != 0)
    {
        number /= 10;
        ++count;
    }
    return count;
}

