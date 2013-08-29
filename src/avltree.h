#ifndef _AvlTree_H
#define _AvlTree_H

struct AvlNode;
typedef struct AvlNode *Position;
typedef struct AvlNode *AvlTree;

AvlTree make_empty( AvlTree t );
AvlTree insert( char * key, int keylen, int value, AvlTree t );
Position find( char * key, int keylen, AvlTree t );
int value( Position p );
int sort_perm( Position p, int * perm );
int compare( char * key, int keylen, AvlTree node );

#endif
