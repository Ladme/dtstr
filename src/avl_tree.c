// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "avl_tree.h"

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH AVL_T                    */
/* *************************************************************************** */

enum direction { LEFT, RIGHT };

/** @brief Destroy the given branch of the AVL tree recursively. */
static void avl_branch_destroy(avl_node_t *node)
{
    if (node == NULL) return;

    avl_branch_destroy(node->left);
    avl_branch_destroy(node->right);
    
    free(node->data);
    free(node);
}

/** @brief Create a new node of an avl tree and assign it to the provided parent. 
 * Returns pointer to the node, if successful, else returns NULL. 
 */
static avl_node_t *avl_node_create(avl_t *tree, const void *item, avl_node_t *parent, const size_t datasize, const enum direction dir)
{
    avl_node_t *node = calloc(1, sizeof(avl_node_t));
    if (node == NULL) return NULL;
    node->data = malloc(datasize);
    if (node->data == NULL) {
        free(node);
        return NULL;
    }
    memcpy(node->data, item, datasize);

    if (parent != NULL) {
        if (dir == LEFT) parent->left = node;
        else parent->right = node;
        node->parent = parent;
    } else {
        tree->root = node;
        node->parent = NULL;
    }
    
    return node;
}

/** @brief Computes the height of the longest branch of the AVL subtree rooted at the given node. */
static size_t avl_branch_height(const avl_node_t *node)
{
    if (node == NULL) return 0;

    size_t left_height = avl_branch_height(node->left);
    size_t right_height = avl_branch_height(node->right);

    return (left_height > right_height) ? left_height + 1 : right_height + 1;
}

/** @brief Calculates the balance factor of the given AVL node. */
static int avl_node_balance(const avl_node_t *node)
{
    size_t left_height = avl_branch_height(node->left);
    size_t right_height = avl_branch_height(node->right);

    return (int) right_height - (int) left_height;
}

/** @brief Redirects pointers between parent and child nodes after rotation. */
static void avl_rotation_parents(avl_t *tree, avl_node_t *unbalanced, avl_node_t *central)
{
    central->parent = unbalanced->parent;
    if (central->parent == NULL) tree->root = central;
    else {
        if (central->parent->right == unbalanced) central->parent->right = central;
        else central->parent->left = central;
    }

    if (unbalanced->right != NULL) unbalanced->right->parent = unbalanced;
    if (unbalanced->left != NULL)  unbalanced->left->parent = unbalanced;

    unbalanced->parent = central;
}

/** @brief Performs a right rotation of an unbalanced AVL tree node. */
static void avl_right_rotation(avl_t *tree, avl_node_t *unbalanced)
{
    avl_node_t *central = unbalanced->left;

    unbalanced->left = central->right;
    central->right = unbalanced;
    avl_rotation_parents(tree, unbalanced, central);
}

/** @brief Performs a left rotation of an unbalanced AVL tree node. */
static void avl_left_rotation(avl_t *tree, avl_node_t *unbalanced)
{
    avl_node_t *central = unbalanced->right;

    unbalanced->right = central->left;
    central->left = unbalanced;
    avl_rotation_parents(tree, unbalanced, central);   
}

/** @brief Performs a right-left rotation of an unbalanced AVL tree node. */
static void avl_rightleft_rotation(avl_t *tree, avl_node_t *unbalanced)
{
    avl_node_t *central = unbalanced->right;
    avl_right_rotation(tree, central);
    avl_left_rotation(tree, unbalanced);
}

/** @brief Performs a left-right rotation of an unbalanced AVL tree node. */
static void avl_leftright_rotation(avl_t *tree, avl_node_t *unbalanced)
{
    avl_node_t *central = unbalanced->left;
    avl_left_rotation(tree, central);
    avl_right_rotation(tree, unbalanced);
}

/** @brief Rebalances the AVL tree after insertion of a node. */
static void avl_rebalance(avl_t *tree, avl_node_t *node)
{
    while (node != NULL) {

        int node_balance = avl_node_balance(node);

        if (node_balance > 1) {

            if (avl_node_balance(node->right) >= 0) avl_left_rotation(tree, node);
            else avl_rightleft_rotation(tree, node);

            return;

        } 
        
        if (node_balance < -1) {

            if (avl_node_balance(node->left) <= 0) avl_right_rotation(tree, node);
            else avl_leftright_rotation(tree, node);

            return;
        } 

        node = node->parent;
        
    }
}

/* *************************************************************************** */
/*                   PUBLIC FUNCTIONS ASSOCIATED WITH AVL_T                    */
/* *************************************************************************** */

avl_t *avl_new(const size_t datasize, int (*compare_function)(const void *, const void *))
{
    avl_t *tree = calloc(1, sizeof(avl_t));
    tree->datasize = datasize;
    tree->compare_function = compare_function;
    return tree;
}

void avl_destroy(avl_t *tree)
{
    if (tree == NULL) return;

    if (tree->root == NULL) {
        free(tree);
        return;
    } 

    avl_branch_destroy(tree->root);
    free(tree);
}

int avl_insert(avl_t *tree, const void *item)
{
    if (tree == NULL) return 99;

    avl_node_t *node = tree->root;
    avl_node_t *parent = NULL;

    enum direction dir = LEFT;

    while (node != NULL) {

        if (tree->compare_function(item, node->data) > 0) {
            parent = node;
            node = node->right;
            dir = RIGHT;
        } else if (tree->compare_function(item, node->data) < 0) {
            parent = node;
            node = node->left;
            dir = LEFT;
        } else {
            return 1;
        }
    }

    avl_node_t *new_node = avl_node_create(tree, item, parent, tree->datasize, dir);
    if (new_node == NULL) {
        free(new_node->data);
        free(new_node);
        return 2;
    }

    avl_rebalance(tree, new_node);

    return 0;
}

avl_node_t *avl_find(const avl_t *tree, const void *target)
{
    if (tree == NULL) return NULL;

    avl_node_t *node = tree->root;

    while (node != NULL) {

        int comparison = tree->compare_function(target, node->data);

        if (comparison > 0) {
            node = node->right;
        } else if (comparison < 0) {
            node = node->left;
        } else {
            return node;
        }
    }

    return NULL;
}

// TODO: write tests for this function
size_t avl_height(const avl_t *tree)
{
    if (tree == NULL) return 0;

    return avl_branch_height(tree->root);
}

void avl_map_breadth(avl_t *tree, void (*function)(void *))
{
    if (tree == NULL) return;

    queue_t *queue = queue_new();
    void *item = NULL;

    avl_node_t *node = tree->root;
    while (node != NULL) {
        function(node->data);

        if (node->left != NULL) queue_en(queue, &(node->left), sizeof(avl_node_t *));
        if (node->right != NULL) queue_en(queue, &(node->right), sizeof(avl_node_t *));

        // free memory for item obtained in previous cycle
        free(item);
        
        item = queue_de(queue, sizeof(avl_node_t *));
        if (item == NULL) node = NULL;
        else node = *(avl_node_t **) item;
    }

    // free last item
    free(item);
    queue_destroy(queue);
}

void avl_map_depth(avl_t *tree, void (*function)(void *))
{
    if (tree == NULL) return;

    vec_t *stack = vec_new();
    void *item = NULL;

    avl_node_t *node = tree->root;
    while (node != NULL) {
        function(node->data);

        if (node->right != NULL) vec_push(stack, &(node->right), sizeof(avl_node_t *));
        if (node->left != NULL) vec_push(stack, &(node->left), sizeof(avl_node_t *));
        
        // free memory for item obtained in previous cycle
        free(item);
        
        item = vec_pop(stack);
        if (item == NULL) node = NULL;
        else node = *(avl_node_t **) item;
    }

    // free last item
    free(item);
    vec_destroy(stack);
}

void avl_map(avl_t *tree, void (*function)(void *))
{
    avl_map_breadth(tree, function);
}