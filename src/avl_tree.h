// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of AVL tree.

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "queue.h"

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


/*! @brief Loops through all items in AVL tree and applies 'function' to each item.
 * 
 * @paragraph Node traversal order
 * The avl_map function utilizes breadth-first search to traverse the nodes in the tree. 
 * Starting from the root node, the specified 'function' is applied to each node in the tree's levels. 
 * Nodes at the same level are processed from left to right, ensuring that each node is visited before moving on to the next level.
 * 
 * @paragraph Modifying the values is dangerous
 * Note that modifying the values stored in an AVL tree using the avl_map function can disrupt the balance of the tree.
 * 
 * @param tree                     tree to apply the function to
 * @param function                 function to apply
 */
void avl_map(avl_t *tree, void (*function)(void *));



#endif /* AVL_TREE_H */