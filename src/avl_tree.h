// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of AVL tree.

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cbuffer.h"
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


/**
 * @brief Inserts an item into an AVL tree.
 *
 * @param tree      AVL tree to insert the item into
 * @param item      Void pointer to the item to be inserted
 *
 * @note - Asymptotic Complexity: Logarithmic, O(log n)
 * 
 * @return Returns 0 on success, 1 if the item with the same value exists in the tree, 
 * 2 if memory allocation fails, and 99 if the tree is NULL.
 */
int avl_insert(avl_t *tree, const void *item);


/**
 * @brief Finds a node in an AVL tree with the specified value and returns pointer to this node.
 *
 * @param tree      AVL tree to search in
 * @param target    Pointer to the searched value
 
 * @note - Asymptotic Complexity: Logarithmic, O(log n)
 * 
 * @return Pointer to the node with the specified value if found, NULL otherwise.
 */
avl_node_t *avl_find(const avl_t *tree, const void *target);


/** 
 * @brief Returns the height of the AVL tree.
 * 
 * @param tree      Tree to calculate the height of
 * 
 * @note - Actually just returns the height label for root of the three. 
 *         Therefore, the tree must be balanced and updated.
 * @note - Empty tree and tree with a single node have height of 0.
 * 
 * @return Height of the tree. Zero, if the tree is NULL.
 */
size_t avl_height(const avl_t *tree);


/** 
 * @brief Returns the number of nodes in an AVL tree.
 *
 * @param tree      Tree to calculate the length of.
 * 
 * @note - Asymptotic Complexity: Linear, O(n)
 * 
 * @return Number of nodes in the AVL tree. If tree is NULL, returns 0.
 */
size_t avl_len(avl_t *tree);


/** 
 * @brief Traverses all items in AVL tree in level-order (breadth-first) fashion and applies `function` to each item.
 * 
 * @param tree      Tree to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Note that modifying the values stored in an AVL tree using the function can disrupt the balance of the tree.
 * @note - The avl_map_levelorder functions utilizes level-order (breadth-first) traversal over the nodes in the tree.
 *         Starting from the root node, the specified `function` is applied to each node in the tree's levels. 
 *         Nodes at the same level are processed from left to right.
 */
void avl_map_levelorder(avl_t *tree, void (*function)(void *, void *), void *pointer);


/** 
 * @brief Traverses all items in AVL tree in in-order (left-root-right) fashion and applies `function` to each item.
 * 
 * @param tree      Tree to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Note that modifying the values stored in an AVL tree using the function can disrupt the balance of the tree.
 * @note - The avl_map_inorder function utilizes in-order (depth-first) traversal over the nodes in the tree.
 *         Starting from the leftmost child of the root node, the specified `function` is applied to each node in the tree.
 * @note - The left subtree is visited first, then the root node, and finally the right subtree.
 */
void avl_map_inorder(avl_t *tree, void (*function)(void *, void *), void *pointer);


/** 
 * @brief Traverses all items in AVL tree in pre-order (root-left-right) fashion and applies `function` to each item.
 * 
 * @param tree      Tree to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Note that modifying the values stored in an AVL tree using the function can disrupt the balance of the tree.
 * @note - The avl_map_preorder function applies the specified `function` to each node in the tree's root-left-right order.
 * @note - The root node is visited first, followed by the left subtree, and then the right subtree.
 */
void avl_map_preorder(avl_t *tree, void (*function)(void *, void *), void *pointer);


/** 
 * @brief Traverses all items in AVL tree in post-order (left-right-root) fashion and applies `function` to each item.
 * 
 * @param tree      Tree to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Note that modifying the values stored in an AVL tree using the function can disrupt the balance of the tree.
 * @note - The avl_map_postorder function applies the specified `function` to each node in the tree's left-right-root order.
 * @note - The left subtree is visited first, followed by the right subtree, and then the root node.
 */
void avl_map_postorder(avl_t *tree, void (*function)(void *, void *), void *pointer);


/** 
 * @brief Traverses all items in AVL tree and applies `function` to each item. The traversal order is undefined.
 * 
 * @param tree      Tree to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Note that modifying the values stored in an AVL tree using the avl_map function can disrupt the balance of the tree.
 */
void avl_map(avl_t *tree, void (*function)(void *, void *), void *pointer);



#endif /* AVL_TREE_H */