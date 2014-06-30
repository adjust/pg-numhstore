#ifndef _AvlTree_H
#define _AvlTree_H

#include "pg_numhstore.h"

struct AvlNode;
typedef struct AvlNode *Position;
typedef struct AvlNode *AvlTree;

struct AvlNode
{
    // payload
    union
    {
        char *key;
        int   intkey;
    };
    int keylen;
    long value;

    AvlTree  left;
    AvlTree  right;
    int      height;
    bool     has_str;
};

typedef struct AvlNode AvlNode;

AvlTree make_empty( AvlTree t );
AvlTree insert( char * key, int keylen, int value, AvlTree t );
AvlTree int_insert( int key, int value, AvlTree t );
Position find( char * key, int keylen, AvlTree t );
Position int_find( int key, AvlTree t );
int value( Position p );
int sort_perm( Position p, int * perm );
int tree_length(Position p);
int compare( char * key, int keylen, AvlTree node );
int int_compare( int key, AvlTree node );
int int_tree_to_pairs(Position p, Pairs *pairs, int4 *buflen, int n);
int tree_to_pairs(Position p, Pairs *pairs, int4 *buflen, int n);
#endif
