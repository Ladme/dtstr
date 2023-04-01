// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "avl_tree.h"

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH AVL_T                    */
/* *************************************************************************** */

enum direction { LEFT, RIGHT };

static void avl_branch_destroy(avl_node_t *node)
{
    if (node->left != NULL)  avl_branch_destroy(node->left);
    if (node->right != NULL) avl_branch_destroy(node->right);
    free(node->data);
    free(node);
}

/*! Create a new node of an avl tree and assign it to the provided parent. Returns 0, if successful, else returns 1. */
static int avl_node_create(avl_t *tree, const void *item, avl_node_t *parent, const size_t datasize, const enum direction dir)
{
    avl_node_t *node = calloc(1, sizeof(avl_node_t));
    if (node == NULL) return 1;
    node->data = malloc(datasize);
    if (node->data == NULL) {
        free(node);
        return 1;
    }
    memcpy(node->data, item, datasize);

    if (parent != NULL) {
        if (dir == LEFT) parent->left = node;
        else parent->right = node;
    } else {
        tree->root = node;
    }
    
    return 0;
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
        } else {
            return 1;
        }

    }

    return avl_node_create(tree, item, parent, tree->datasize, dir);
}

void avl_map(avl_t *tree, void (*function)(void *))
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