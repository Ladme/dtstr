// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of AVL tree.

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "queue.h"
#include "vector.h"

typedef struct avl_node {
    void *data;
    size_t height;
    struct avl_node *parent;
    struct avl_node *left;
    struct avl_node *right;
} avl_node_t;


typedef struct avl {
    avl_node_t *root;
    size_t datasize;
    int (*compare_function)(const void *, const void *);
} avl_t;


/**
 * @brief Allocates memory for a new empty AVL tree.
 *
 * @param datasize          The size of each item's data in bytes
 * @param compare_function  The function to use to compare the items in the AVL tree
 * 
 * @note 
 * - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * If the first item is greater than the second, the function should return a positive integer.
 * If the first item is smaller, it should return a negative integer.
 * If the two items are equal, it should return 0.
 * 
 * @return A pointer to the newly allocated AVL tree.
 */
avl_t *avl_new(const size_t datasize, int (*compare_function)(const void *, const void *));


/**
 * @brief Properly deallocates memory for the given AVL tree and destroys the `avl_t` structure.
 *
 * @param tree    The `avl_t` structure to destroy.
 */
void avl_destroy(avl_t *tree);


int avl_insert(avl_t *tree, const void *item);


avl_node_t *avl_find(const avl_t *tree, const void *target);

/** 
 * @brief Computes the height of the AVL tree.
 * 
 * @param tree      Tree to calculate the height of
 * 
 */
size_t avl_height(const avl_t *tree);


/** 
 * @brief Traverses all items in AVL tree breadth-first and applies `function` to each item.
 * 
 * @param tree      Tree to apply the function to
 * @param function  Function to apply
 * 
 * @note - Note that modifying the values stored in an AVL tree using the avl_map_breadth function can disrupt the balance of the tree.
 * @note - The avl_map_breadth function utilizes level-order (breadth-first) traversal over the nodes in the tree.
 *         Starting from the root node, the specified `function` is applied to each node in the tree's levels. 
 *         Nodes at the same level are processed from left to right, ensuring that each node is visited before moving on to the next level.
 */
void avl_map_breadth(avl_t *tree, void (*function)(void *));


/** 
 * @brief Traverses all items in AVL tree depth-first and applies `function` to each item.
 * 
 * @param tree      Tree to apply the function to
 * @param function  Function to apply
 * 
 * @note - Note that modifying the values stored in an AVL tree using the avl_map_depth function can disrupt the balance of the tree.
 * @note - The avl_map_depth function performs a depth-first traversal over the nodes of the tree,
 *         starting from the root node, and applies the specified `function` to each node of the tree in a specific order.
 *         The function is first applied to each node in the left-most branch of the tree, followed by the second left-most branch and so on,
 *         until the right-most branch is processed last.
 */
void avl_map_depth(avl_t *tree, void (*function)(void *));


/** 
 * @brief Traverses all items in AVL tree and applies `function` to each item. The traversal order is undefined.
 * 
 * @param tree      Tree to apply the function to
 * @param function  Function to apply
 * 
 * @note - Note that modifying the values stored in an AVL tree using the avl_map function can disrupt the balance of the tree.
 */
void avl_map(avl_t *tree, void (*function)(void *));



#endif /* AVL_TREE_H */