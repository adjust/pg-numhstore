#include "data_types.h"

int inline
node_value(Position p)
{
    return p->value;
}

static inline int
height(Position p)
{
    if (p == NULL)
        return -1;
    else
        return p->height;
}

static inline int
max(int lhs, int rhs)
{
    return lhs > rhs ? lhs : rhs;
}

static inline int
min(int lhs, int rhs)
{
    return lhs < rhs ? lhs : rhs;
}

AvlTree
make_empty(AvlTree t)
{
    if (t != NULL)
    {
        make_empty(t->left);
        make_empty(t->right);
        pfree(t);
    }
    return NULL;
}

int
compare(char * key, int keylen, AvlTree node)
{
    int len;
    int cmp;

    if (keylen < node->keylen)
        return -1;
    if (keylen > node->keylen)
        return 1;

    len = min(keylen, node->keylen);
    cmp = strncmp(key, node->key, len);

    return cmp;
}

int
int_compare(int key, AvlTree node)
{
    if (key == node->intkey)
        return 0;
    else if (key < node->intkey)
        return -1;
    else
        return 1;
}

Position
find(char *key, int keylen, AvlTree t)
{
    int cmp;

    if (t == NULL)
        return NULL;

    cmp = compare(key, keylen, t);
    if (cmp < 0)
        return find(key, keylen, t->left);
    else if(cmp > 0)
        return find(key, keylen, t->right);
    else
        return t;
}

Position
int_find(int key, AvlTree t)
{
    int cmp;

    if (t == NULL)
        return NULL;

    cmp = int_compare(key, t);
    if (cmp < 0)
        return int_find(key, t->left);
    else if (cmp > 0)
        return int_find(key, t->right);
    else
        return t;
}

/* This function can be called only if k2 has a left child */
/* Perform a rotate between a node (k2) and its left child */
/* Update heights, then return new root */
static Position
singleRotateWithLeft(Position k2)
{
    Position k1;

    k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = max(height(k2->left), height(k2->right)) + 1;
    k1->height = max(height(k1->left), k2->height) + 1;

    return k1;  /* New root */
}


/* This function can be called only if k1 has a right child */
/* Perform a rotate between a node (k1) and its right child */
/* Update heights, then return new root */
static Position
singleRotateWithRight(Position k1)
{
    Position k2;

    k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = max(height(k1->left), height(k1->right)) + 1;
    k2->height = max(height(k2->right), k1->height) + 1;

    return k2;  /* New root */
}


/* This function can be called only if k3 has a left */
/* child and k3's left child has a right child */
/* Do the left-right double rotation */
/* Update heights, then return new root */
static Position
doubleRotateWithLeft(Position k3)
{
    /* Rotate between k1 and k2 */
    k3->left = singleRotateWithRight(k3->left);

    /* Rotate between k3 and k2 */
    return singleRotateWithLeft(k3);
}

/* This function can be called only if k1 has a right */
/* child and k1's right child has a left child */
/* Do the right-left double rotation */
/* Update heights, then return new root */
static Position
doubleRotateWithRight(Position k1)
{
    /* Rotate between k3 and k2 */
    k1->right = singleRotateWithLeft(k1->right);

    /* Rotate between k1 and k2 */
    return singleRotateWithRight(k1);
}

AvlTree
insert(char *key, int keylen, int value, AvlTree t)
{
    if (t == NULL)
    {
        /* Create and return a one-node tree */
        t = palloc0(sizeof(struct AvlNode));
        if (t == NULL)
            elog(ERROR, "AvlTree insert: could not allocate memory");
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
        int cmp = compare(key, keylen, t);
        if (cmp < 0)
        {
            t->left = insert(key, keylen, value, t->left);
            if (height(t->left) - height(t->right) == 2)
            {
                if (compare(key, keylen, t->left))
                    t = singleRotateWithLeft(t);
                else
                    t = doubleRotateWithLeft(t);
            }
        }
        else if(cmp > 0)
        {
            t->right = insert(key, keylen, value, t->right);
            if (height(t->right) - height(t->left) == 2)
            {
                if (compare(key, keylen, t->right))
                    t = singleRotateWithRight(t);
                else
                    t = doubleRotateWithRight(t);
            }
        }
    }

    t->height = max(height(t->left), height(t->right)) + 1;
    return t;
}

AvlTree
int_insert(int key, int value, AvlTree t)
{
    if(t == NULL)
    {
        /* Create and return a one-node tree */
        t = palloc0(sizeof(struct AvlNode));
        if (t == NULL)
            elog(ERROR, "AvlTree insert: could not allocate memory");
        else
        {
            t->intkey = key;
            t->value = value;
            t->height = 0;
            t->left = NULL;
            t->right = NULL;
        }
    }
    else
    {
        int cmp = int_compare(key, t);
        if (cmp < 0)
        {
            t->left = int_insert(key, value, t->left);
            if (height(t->left) - height(t->right) == 2)
            {
                if (int_compare( key, t->left))
                    t = singleRotateWithLeft(t);
                else
                    t = doubleRotateWithLeft(t);
            }
        }
        else if(cmp > 0)
        {
            t->right = int_insert(key, value, t->right);
            if (height(t->right) - height(t->left) == 2)
            {
                if (int_compare(key, t->right))
                    t = singleRotateWithRight(t);
                else
                    t = doubleRotateWithRight(t);
            }
        }
    }

    t->height = max(height(t->left), height(t->right)) + 1;
    return t;
}

int
sort_perm(Position p, int *perm)
{
    int n;
    if (p == NULL)
        return 0;

    n = sort_perm(p->left, perm);
    perm[n++] = p->value;
    n += sort_perm(p->right, perm + n);
    return n;
}

// return number of nodes
int
tree_length(Position p)
{
    int n;
    if(p == NULL)
        return 0;

    n = tree_length(p->left);
    ++n;
    n += tree_length(p->right);
    return n;
}

int
tree_to_pairs(Position p, Pairs *pairs, int4* buflen, int n)
{
    if(p == NULL)
        return n;
    n = tree_to_pairs(p->left, pairs, buflen, n);
    ADD_PAIR(p->key, p->keylen, p->value, pairs, n, *buflen);
    ++n;
    n = tree_to_pairs(p->right, pairs, buflen, n);
    return n;
}

int
int_tree_to_pairs(Position p, Pairs *pairs, int4* buflen, int n)
{
    if(p == NULL)
        return n;
    n = int_tree_to_pairs(p->left, pairs, buflen, n);
    ADD_INTPAIR(p->intkey, p->value, pairs, n, *buflen);
    ++n;
    n = int_tree_to_pairs(p->right, pairs, buflen, n);
    return n;
}

void
init_array(Array *a, size_t initial_size)
{
    a->keys  = (char **)palloc0(initial_size * sizeof(char *));
    a->vstr  = (char **)palloc0(initial_size * sizeof(char *));
    a->vals  = (long * )palloc0(initial_size * sizeof(long *));
    a->sizes = (int  * )palloc0(initial_size * sizeof(int  *));
    a->found = (bool * )palloc0(initial_size * sizeof(bool *));
    a->used  = 0;
    a->size  = initial_size;
}

void
insert_array(Array *a, char *key, long val, int elem_size)
{
    if (a->used == a->size)
    {
        char **keys_swap;
        char **vstr_swap;
        long  *vals_swap;
        int   *sizes_swap = a->sizes;
        bool  *found_swap = a->found;
        int    i = a->size;
        a->size *= 2;

        keys_swap = a->keys;
        a->keys = (char **)palloc0(a->size * sizeof(char *));
        memcpy(a->keys, keys_swap, sizeof(char *) * i);
        pfree(keys_swap);

        vstr_swap = a->vstr;
        a->vstr = (char **)palloc0(a->size * sizeof(char *));
        memcpy(a->vstr, vstr_swap, sizeof(char *) * i);
        pfree(vstr_swap);

        vals_swap = a->vals;
        a->vals = (long *)palloc0(a->size * sizeof(long));
        memcpy(a->vals, vals_swap, sizeof(long) * i);
        pfree(vals_swap);

        sizes_swap = a->sizes;
        a->sizes = (int *)palloc0(a->size * sizeof(int));
        memcpy(a->sizes, sizes_swap, sizeof(int) * i);
        pfree(sizes_swap);

        found_swap = a->found;
        a->found = (bool *)palloc0(a->size * sizeof(bool));
        memcpy(a->found, found_swap, sizeof(bool) * i);
        pfree(found_swap);
    }
    a->keys[a->used]   = key;
    a->sizes[a->used]  = elem_size;
    a->vals[a->used++] = val;
}

void
free_array(Array *a)
{
    pfree(a->keys);
    pfree(a->vstr);
    pfree(a->vals);
    pfree(a->sizes);
    pfree(a->found);
    a->keys  = NULL;
    a->vstr  = NULL;
    a->vals  = NULL;
    a->found = NULL;
    a->used  = a->size = 0;
}

int
compare_array(char *key1, int keylen1, char *key2, int keylen2)
{
    int  cmp;
    if (keylen1 < keylen2)
        return -1;
    if (keylen1 > keylen2)
        return 1;

    cmp = strncmp(key1, key2, keylen1);
    return cmp;
}
