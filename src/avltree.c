#include "avltree.h"
#include "postgres.h"
#include <stdlib.h>

struct AvlNode
{
    // payload
    char * key;
    int keylen;
    int value;

    AvlTree  left;
    AvlTree  right;
    int      height;
};

AvlTree make_empty( AvlTree t )
{
    if( t != NULL )
    {
        make_empty( t->left );
        make_empty( t->right );
        pfree( t );
    }
    return NULL;
}

static int max( int lhs, int rhs )
{
    return lhs > rhs ? lhs : rhs;
}

static int min( int lhs, int rhs )
{
    return lhs < rhs ? lhs : rhs;
}

int compare( char * key, int keylen, AvlTree node )
{
    if( keylen < node->keylen )
        return -1;
    if( keylen > node->keylen )
        return 1;

    int len = min( keylen, node->keylen );
    int cmp = strncmp( key, node->key, len );

    return cmp;
}

Position find( char * key, int keylen, AvlTree t )
{
    if( t == NULL )
        return NULL;

    int cmp = compare( key, keylen, t );
    if( cmp < 0 )
        return find( key, keylen, t->left );
    else if( cmp > 0 )
        return find( key, keylen, t->right );
    else
        return t;
}

static int height( Position p )
{
    if( p == NULL )
        return -1;
    else
        return p->height;
}


/* This function can be called only if k2 has a left child */
/* Perform a rotate between a node (k2) and its left child */
/* Update heights, then return new root */

static Position singleRotateWithLeft( Position k2 )
{
    Position k1;

    k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = max( height( k2->left ), height( k2->right ) ) + 1;
    k1->height = max( height( k1->left ), k2->height ) + 1;

    return k1;  /* New root */
}


/* This function can be called only if k1 has a right child */
/* Perform a rotate between a node (k1) and its right child */
/* Update heights, then return new root */

static Position singleRotateWithRight( Position k1 )
{
    Position k2;

    k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = max( height( k1->left ), height( k1->right ) ) + 1;
    k2->height = max( height( k2->right ), k1->height ) + 1;

    return k2;  /* New root */
}


/* This function can be called only if k3 has a left */
/* child and k3's left child has a right child */
/* Do the left-right double rotation */
/* Update heights, then return new root */

static Position doubleRotateWithLeft( Position k3 )
{
    /* Rotate between k1 and k2 */
    k3->left = singleRotateWithRight( k3->left );

    /* Rotate between k3 and k2 */
    return singleRotateWithLeft( k3 );
}


/* This function can be called only if k1 has a right */
/* child and k1's right child has a left child */
/* Do the right-left double rotation */
/* Update heights, then return new root */

static Position doubleRotateWithRight( Position k1 )
{
    /* Rotate between k3 and k2 */
    k1->right = singleRotateWithLeft( k1->right );

    /* Rotate between k1 and k2 */
    return singleRotateWithRight( k1 );
}



AvlTree insert( char * key, int keylen, int value, AvlTree t )
{
    if( t == NULL )
    {
        /* Create and return a one-node tree */
        t = palloc0( sizeof( struct AvlNode ) );
        if( t == NULL )
        {
            // out of space
        }
        else
        {
            t->key = key;
            t->keylen = keylen;
            t->value = value;

            t->height = 0;
            t->left = NULL;
            t->right = NULL;
        }
    }
    else
    {
        int cmp = compare( key, keylen, t );
        if( cmp < 0 )
        {
            t->left = insert( key, keylen, value, t->left );
            if( height( t->left ) - height( t->right ) == 2 )
                if( compare( key, keylen, t->left ) )
                    t = singleRotateWithLeft( t );
                else
                    t = doubleRotateWithLeft( t );
        }
        else if( cmp > 0)
        {
            t->right = insert( key, keylen, value, t->right );
            if( height( t->right ) - height( t->left ) == 2 )
                if( compare( key, keylen, t->right ) )
                    t = singleRotateWithRight( t );
                else
                    t = doubleRotateWithRight( t );
        }
    }

    t->height = max( height( t->left ), height( t->right ) ) + 1;
    return t;
}

int value( Position p )
{
    return p->value;
}

// return number of nodes
int sort_perm( Position p, int * perm ) {
    if( p == NULL )
        return 0;

    int n = sort_perm( p->left, perm );
    perm[n++] = p->value;
    n += sort_perm( p->right, perm + n );
    return n;
}
