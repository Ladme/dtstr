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

/** @brief Updates the height label of the given AVL tree node based on the height of its children. */
static void avl_node_update_height(avl_node_t *node)
{
    size_t left = (node->left == NULL) ? 0 : node->left->height + 1;
    size_t right = (node->right == NULL) ? 0 : node->right->height + 1;

    if (left == 0 && right == 0) node->height = 0;
    else node->height = (left > right) ? left : right;
}

/** @brief Returns the balance factor of the given AVL node. */
static int avl_node_balance(const avl_node_t *node)
{
    size_t left = (node->left == NULL) ? 0 : node->left->height + 1;
    size_t right = (node->right == NULL) ? 0 : node->right->height + 1;
    
    return (int) right - (int) left;
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

    avl_node_update_height(unbalanced);
    avl_node_update_height(central);
}

/** @brief Performs a left rotation of an unbalanced AVL tree node. */
static void avl_left_rotation(avl_t *tree, avl_node_t *unbalanced)
{
    avl_node_t *central = unbalanced->right;

    unbalanced->right = central->left;
    central->left = unbalanced;
    avl_rotation_parents(tree, unbalanced, central);   

    avl_node_update_height(unbalanced);
    avl_node_update_height(central);
}

/** @brief Performs a right-left rotation of an unbalanced AVL tree node. */
static void avl_rightleft_rotation(avl_t *tree, avl_node_t *unbalanced)
{
    avl_right_rotation(tree, unbalanced->right);
    avl_left_rotation(tree, unbalanced);
}

/** @brief Performs a left-right rotation of an unbalanced AVL tree node. */
static void avl_leftright_rotation(avl_t *tree, avl_node_t *unbalanced)
{
    avl_left_rotation(tree, unbalanced->left);
    avl_right_rotation(tree, unbalanced);
}

/** @brief Rebalances the AVL tree after insertion of a node. */
static void avl_rebalance(avl_t *tree, avl_node_t *node)
{
    while (node != NULL) {

        avl_node_update_height(node);
        int node_balance = avl_node_balance(node);

        if (node_balance > 1) {

            if (avl_node_balance(node->right) >= 0) avl_left_rotation(tree, node);
            else avl_rightleft_rotation(tree, node);

            if (node->parent != NULL) node = node->parent->parent;
            else node = NULL;
            
            continue;

        } 
        
        if (node_balance < -1) {

            if (avl_node_balance(node->left) <= 0) avl_right_rotation(tree, node);
            else avl_leftright_rotation(tree, node);

            if (node->parent != NULL) node = node->parent->parent;
            else node = NULL;
            
            continue;
        } 

        node = node->parent;
        
    }
}

/** @brief Traverses all nodes in a node's subtree in-order (left, root, right). */
static void avl_node_inorder(avl_node_t *node, void (*function)(void *, void *), void *pointer)
{
    if (node == NULL) return;

    avl_node_inorder(node->left, function, pointer);
    function(node->data, pointer);
    avl_node_inorder(node->right, function, pointer);
}

/** @brief Traverses all nodes in a node's subtree pre-order (root, left, right). */
static void avl_node_preorder(avl_node_t *node, void (*function)(void *, void *), void *pointer)
{
    if (node == NULL) return;

    function(node->data, pointer);
    avl_node_preorder(node->left, function, pointer);
    avl_node_preorder(node->right, function, pointer);
}

/** @brief Traverses all nodes in a node's subtree post-order (left, right, root). */
static void avl_node_postorder(avl_node_t *node, void (*function)(void *, void *), void *pointer)
{
    if (node == NULL) return;

    avl_node_postorder(node->left, function, pointer);
    avl_node_postorder(node->right, function, pointer);
    function(node->data, pointer);
}

/** @brief Function to be used with a map function to count the number of nodes in a tree. */
static void avl_node_count(void *node, void *counter)
{
    if (node == NULL) return;

    size_t *count = (size_t *) counter;
    ++(*count);
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

    avl_rebalance(tree, parent);

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

size_t avl_height(const avl_t *tree)
{
    if (tree == NULL || tree->root == NULL) return 0;

    return tree->root->height;
}

size_t avl_len(avl_t *tree)
{
    size_t count = 0;
    avl_map(tree, avl_node_count, &count);

    return count;
}

void avl_map_levelorder(avl_t *tree, void (*function)(void *, void *), void *pointer)
{
    if (tree == NULL) return;

    cbuf_t *queue = cbuf_new();
    void *item = NULL;

    avl_node_t *node = tree->root;
    while (node != NULL) {
        function(node->data, pointer);

        if (node->left != NULL) cbuf_enqueue(queue, &(node->left), sizeof(avl_node_t *));
        if (node->right != NULL) cbuf_enqueue(queue, &(node->right), sizeof(avl_node_t *));

        // free memory for item obtained in previous cycle
        free(item);
        
        item = cbuf_dequeue(queue);
        if (item == NULL) node = NULL;
        else node = *(avl_node_t **) item;
    }

    // free last item
    free(item);
    cbuf_destroy(queue);
}

void avl_map_inorder(avl_t *tree, void (*function)(void *, void *), void *pointer)
{
    if (tree == NULL) return;

    avl_node_inorder(tree->root, function, pointer);
}

void avl_map_preorder(avl_t *tree, void (*function)(void *, void *), void *pointer)
{
    if (tree == NULL) return;

    avl_node_preorder(tree->root, function, pointer);
}

void avl_map_postorder(avl_t *tree, void (*function)(void *, void *), void *pointer)
{
    if (tree == NULL) return;

    avl_node_postorder(tree->root, function, pointer);
}

void avl_map(avl_t *tree, void (*function)(void *, void *), void *pointer)
{
    avl_map_postorder(tree, function, pointer);
}