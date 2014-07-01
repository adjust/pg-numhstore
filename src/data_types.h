#ifndef _DATA_TYPES_H
#define _DATA_TYPES_H

#include "pg_numhstore.h"

struct AvlNode;
typedef struct AvlNode AvlNode;
typedef struct AvlNode *Position;
typedef struct AvlNode *AvlTree;

struct AvlNode
{
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
};

int node_value( Position p );

AvlTree make_empty( AvlTree t );

int compare( char * key, int keylen, AvlTree node );
int int_compare( int key, AvlTree node );

Position find( char * key, int keylen, AvlTree t );
Position int_find( int key, AvlTree t );

AvlTree insert( char * key, int keylen, int value, AvlTree t );
AvlTree int_insert( int key, int value, AvlTree t );

int sort_perm( Position p, int * perm );
int tree_length(Position p);

int int_tree_to_pairs(Position p, Pairs *pairs, int4 *buflen, int n);
int tree_to_pairs(Position p, Pairs *pairs, int4 *buflen, int n);

typedef struct {
    char  **keys;
    char  **vstr;
    long   *vals;
    int    *sizes;
    size_t  used;
    size_t  size;
    bool   *found;
} Array;

void init_array(Array *a, size_t initial_size);
void insert_array(Array *a, char *key, long val, int elem_size);
void free_array(Array *a);
int  compare_array(char *key1, int keylen1, char *key2, int keylen2);

#endif // _DATA_TYPES_H
