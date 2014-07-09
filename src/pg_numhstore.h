#ifndef ADJUST_PG_NUMHSTORE
#define ADJUST_PG_NUMHSTORE

#include <postgres.h>
#include <fmgr.h>
#include <string.h>
#include <utils/array.h>
#include <stdlib.h>
#include <catalog/pg_type.h>
#include "hstore.h"
#include "data_types.h"

/*
 * reimplementation of static postgres/hstore functions
 * shamelessly copied over
 */
HStore* hstorePairs(Pairs *pairs, int4 pcount, int4 buflen);
HStore* hstoreUpgrade(Datum orig);
size_t hstoreCheckKeyLen(size_t len);
extern void get_typlenbyvalalign(Oid eltype, int16 *i_typlen, bool *i_typbyval, char *i_typalign);

/*
 * array_add function implemented in array_add.c.
 * used to merge two array into an inthstore,
 * while the first array provides the keys and
 * the second the array the corresponding values
 */
Datum array_add(PG_FUNCTION_ARGS);

/*
 * hstore_array functions and aggregate implemented
 * in hstore_array.c. used to merge an array of inthstores
 * into one big hstore.
 */
Datum hstore_array(PG_FUNCTION_ARGS);
Datum hstore_array_finalfn(PG_FUNCTION_ARGS);
Datum array_to_hstore(Datum *data, int count, bool *nulls);

/*
 * array_count functions implemented in array_count.c.
 * used to count the elements of the given text or integer array
 * into an hstore structure
 */
Datum array_count(PG_FUNCTION_ARGS);
HStore* adeven_count_int_array(Datum *i_data, int n, bool *nulls);
HStore* adeven_count_text_array(Datum *i_data, int n, bool *nulls);

/*
 * hstore_add function used to merge two inthstores into one
 */
Datum hstore_add( PG_FUNCTION_ARGS );

/*
 * helper function used to get the width of the string
 * representation of a given long integer excluding terminating NULL
 */
size_t width_as_string(long number);

/*
 * create and return an empty hstore
 */
#define PG_RETURN_EMPTY_HSTORE PG_RETURN_POINTER(hstorePairs( 0, 0, 0))


/*
 * macro used to extract an hstore key from position _index from
 * an HEntry array
 */
#define HENTRY_KEY(_entries, _index, _base, _key, _keylen)      \
    do {                                                        \
        _keylen = HS_KEYLEN(_entries, _index);                  \
        _key = palloc0(_keylen + 1);                            \
        memcpy(_key, HS_KEY(_entries, _base, _index), _keylen); \
    } while(0)

/*
 * macro used to extract an hstore int value from position _index from
 * an HEntry array
 */
#define HENTRY_VAL(_entries, _index, _base, _val)                  \
    do {                                                           \
        char   *_valstr,                                           \
               *_ptr;                                              \
        size_t  _vallen = HS_VALLEN(_entries, _index);             \
        _valstr = _ptr = palloc0(_vallen + 1);                     \
        memcpy(_valstr, HS_VAL(_entries, _base, _index), _vallen); \
        _val = strtol(_ptr, NULL, 10);                             \
        pfree(_valstr);                                            \
    } while(0)

/*
 * macro used to extract an hstore pair from position _index from
 * an HEntry array
 */
#define READ_PAIR(_entries, _base, _index, _key, _val, _keylen) \
    do {                                                        \
        HENTRY_KEY(_entries, _index, _base, *_key, *_keylen);   \
        HENTRY_VAL(_entries, _index, _base, *_val);             \
    } while(0)

/*
 * macro to create a c string representation of a
 * varlena postgres text. allocates extra memory with palloc.
 * expects the datum to be NULL terminated
 */
#define DATUM_TO_CSTRING(_datum, _str, _len) \
    do {                                     \
    _len = VARSIZE(_datum) - VARHDRSZ;       \
    _str = palloc0(_len + 1);                \
    memcpy(_str, VARDATA(_datum), _len);     \
    } while(0)

/*
 * macro to create a c string representation of a
 * long integer. allocates extra memory with palloc.
 * the resulting string is NULL terminated.
 */
#define LONG_TO_STR(_long, _str, _width)  \
    do {                                  \
    _width = width_as_string(_long);      \
    _str = palloc(_width + 1);            \
    sprintf(_str, "%ld", _long);          \
    } while(0)

/*
 * macro to add a key/value pair to an pairs array at position n.
 * expects the key to be a char* and the value to be long integer.
 * allocates extra memory for key and value with palloc, so the
 * source can be safely pfree'd.
 */
#define ADD_PAIR(_key, _keywidth, _value, _pairs, _n, _buflen) \
    do {                                                       \
    char *_valstr;                                             \
    int _valwidth;                                             \
    LONG_TO_STR(_value, _valstr, _valwidth);                   \
    _pairs[_n].key = pstrdup(_key);                            \
    _pairs[_n].keylen = _keywidth;                             \
    _pairs[_n].val = _valstr;                                  \
    _pairs[_n].vallen = _valwidth;                             \
    _pairs[_n].isnull = false;                                 \
    _pairs[_n].needfree = false;                               \
    _buflen += _pairs[_n].keylen + _pairs[_n].vallen;          \
    } while(0)

/*
 * macro to add a key/value pair to an pairs array at position n.
 * expects the key and the value to be long integer.
 * allocates extra memory for key and value with palloc, so the
 * source can be safely pfree'd.
 */
#define ADD_INTPAIR(_key, _value, _pairs, _n, _buflen) \
    do {                                               \
    char *_keystr,                                     \
         *_valstr;                                     \
    int _keywidth,                                     \
        _valwidth;                                     \
    LONG_TO_STR((long) _key, _keystr, _keywidth);      \
    LONG_TO_STR(_value, _valstr, _valwidth);           \
    _pairs[_n].key = _keystr;                          \
    _pairs[_n].keylen = _keywidth;                     \
    _pairs[_n].val = _valstr;                          \
    _pairs[_n].vallen = _valwidth;                     \
    _pairs[_n].isnull = false;                         \
    _pairs[_n].needfree = false;                       \
    _buflen += _pairs[_n].keylen + _pairs[_n].vallen;  \
    } while(0)

#endif // ADJUST_PG_NUMHSTORE
