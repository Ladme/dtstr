// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of AVL tree.

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct avl_node {
    void *data;
    struct avl_node *left;
    struct avl_node *right;
} avl_node_t;


typedef struct avl {
    avl_node_t *root;
    size_t datasize;
    int (*compare_function)(const void *, const void *);
} avl_t;


avl_t *avl_new(const size_t datasize, int (*compare_function)(const void *, const void *));
void avl_destroy(avl_t *tree);

int avl_insert(avl_t *tree, const void *item);

#endif /* AVL_TREE_H */