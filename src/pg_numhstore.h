#ifndef ADEVEN_PG_NUMHSTORE
#define ADEVEN_PG_NUMHSTORE

#include "postgres.h"
#include "hstore.h"
#include "fmgr.h"
#include "avltree.h"
#include <string.h>
#include <utils/array.h>
#include <stdlib.h>
#include <catalog/pg_type.h>

HStore * hstorePairs( Pairs *pairs, int4 pcount, int4 buflen );
HStore * hstoreUpgrade( Datum orig );
size_t adeven_get_digit_num( long number );
extern void get_typlenbyvalalign( Oid eltype, int16 * i_typlen, bool * i_typbyval, char * i_typalign );
size_t hstoreCheckKeyLen( size_t len );

#define DATUM_TO_CSTRING(_datum, _str, _len) \
    do {                                     \
    _len = VARSIZE(_datum) - VARHDRSZ;       \
    _str = palloc(_len);                     \
    memcpy(_str, VARDATA(_datum), _len);     \
    } while(0)

#define LONG_TO_STR(_long, _str, _width)  \
    do {                                  \
    _width = adeven_get_digit_num(_long); \
    _str = palloc(_width + 1);            \
    sprintf(_str, "%ld", _long);          \
    } while(0)

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

#define ADD_INTPAIR(_key, _value, _pairs, _n, _buflen) \
    do {                                               \
    char *_keystr,                                     \
         *_valstr;                                     \
    int _keywidth,                                     \
        _valwidth;                                     \
    LONG_TO_STR(_key, _keystr, _keywidth);             \
    LONG_TO_STR(_value, _valstr, _valwidth);           \
    _pairs[_n].key = _keystr;                          \
    _pairs[_n].keylen = _keywidth;                     \
    _pairs[_n].val = _valstr;                          \
    _pairs[_n].vallen = _valwidth;                     \
    _pairs[_n].isnull = false;                         \
    _pairs[_n].needfree = false;                       \
    _buflen += _pairs[_n].keylen + _pairs[_n].vallen;  \
    } while(0)

#endif
