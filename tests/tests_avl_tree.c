// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/avl_tree.h"

#define UNUSED(x) (void)(x)

static int avl_compare_ints(const void *x, const void *y)
{
    return (*(int *) x - *(int *) y);
}

/** @brief Returns the balance factor of the given AVL node. */
static int avl_node_balance(const avl_node_t *node)
{
    size_t left = (node->left == NULL) ? 0 : node->left->height + 1;
    size_t right = (node->right == NULL) ? 0 : node->right->height + 1;
    
    return (int) right - (int) left;
}

static size_t compute_branch_height(const avl_node_t *node)
{   
    if (node == NULL) return 0;

    size_t left = (node->left == NULL) ? 0 : compute_branch_height(node->left) + 1;
    size_t right = (node->right == NULL) ? 0 : compute_branch_height(node->right) + 1;

    if (left == 0 && right == 0) return 0;
    
    return (left > right) ? left : right;
}

inline static void avl_print_int(const avl_t *tree)
{
    cbuf_t *queue = cbuf_new();
    cbuf_t *levels = cbuf_new();

    avl_node_t *node = tree->root;
    void *item = NULL;
    int parent_level = 1;
    int previous_level = 1;

    while (node != NULL) {
        if (parent_level > previous_level) {
            printf("\n");
            previous_level = parent_level;
        }
        printf("%d ", *(int *) node->data);

        if (node->left != NULL) {
            int level = parent_level + 1;
            cbuf_enqueue(queue, &(node->left), sizeof(avl_node_t *));
            cbuf_enqueue(levels, &level, sizeof(int));
        }
        if (node->right != NULL) {
            int level = parent_level + 1;
            cbuf_enqueue(queue, &(node->right), sizeof(avl_node_t *));
            cbuf_enqueue(levels, &level, sizeof(int));
        }

        free(item);
        item = cbuf_dequeue(queue);
        if (item == NULL) node = NULL;
        else node = *(avl_node_t **) item;

        void *p_parent_level = cbuf_dequeue(levels);
        parent_level = *(int *) p_parent_level;
        free(p_parent_level);
    }

    free(item);
    cbuf_destroy(queue);
    cbuf_destroy(levels);
}

static int test_avl_destroy_null(void)
{
    printf("%-40s", "test_avl_destroy (null) ");

    avl_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_avl_new(void)
{
    printf("%-40s", "test_avl_new ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    assert(tree);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_basic(void)
{
    printf("%-40s", ">>> basic ");

    int item = 8;
    int item2 = 7;

    assert(avl_insert(NULL, &item) == 99);

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);

    assert(avl_insert(tree, &item) == 0);
    assert(*(int *) tree->root->data == 8);
    assert(avl_insert(tree, &item) == 1);

    assert(tree->root->height == 0);

    assert(avl_insert(tree, &item2) == 0);
    assert(*(int *) tree->root->left->data == 7);
    assert(tree->root->height == 1);
    assert(tree->root->left->height == 0);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}


static int test_avl_insert_left_right(void)
{
    printf("%-40s", ">>> ROT left + right ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 6, 7, 4, 3};
    
    // creating right-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);
    
    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->right->data == 6);

    assert(avl_insert(tree, &data[2]) == 0);
    // left-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    // creating left-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->left->left->data == 4);

    assert(avl_insert(tree, &data[4]) == 0);
    // right-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 4);
    assert(*(int *) tree->root->left->left->data == 3);
    assert(*(int *) tree->root->left->right->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_left_left(void)
{
    printf("%-40s", ">>> ROT left + left ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 6, 7, 8, 9};
    
    // creating right-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->right->data == 6);

    assert(avl_insert(tree, &data[2]) == 0);
    // left-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    // creating right-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->right->right->data == 8);

    assert(avl_insert(tree, &data[4]) == 0);
    // left-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 8);
    assert(*(int *) tree->root->right->left->data == 7);
    assert(*(int *) tree->root->right->right->data == 9);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_right_left(void)
{
    printf("%-40s", ">>> ROT right + left ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 4, 3, 6, 7};
    
    // creating left-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->left->data == 4);

    assert(avl_insert(tree, &data[2]) == 0);
    // right-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 5);
    

    // creating right-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->right->right->data == 6);

    assert(avl_insert(tree, &data[4]) == 0);
    // left-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 6);
    assert(*(int *) tree->root->right->right->data == 7);
    assert(*(int *) tree->root->right->left->data == 5);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_right_right(void)
{
    printf("%-40s", ">>> ROT right + right ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 4, 3, 2, 1};
    
    // creating left-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->left->data == 4);

    assert(avl_insert(tree, &data[2]) == 0);
    // right-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 5);
    

    // creating left-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->left->left->data == 2);

    assert(avl_insert(tree, &data[4]) == 0);
    // right-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 2);
    assert(*(int *) tree->root->right->data == 5);
    assert(*(int *) tree->root->left->left->data == 1);
    assert(*(int *) tree->root->left->right->data == 3);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_left_leftright(void)
{
    printf("%-40s", ">>> ROT left + left-right ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 6, 7, 3, 4};
    
    // creating right-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->right->data == 6);

    assert(avl_insert(tree, &data[2]) == 0);
    // left-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    // creating right-left-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->left->left->data == 3);

    assert(avl_insert(tree, &data[4]) == 0);
    // left-right-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 4);
    assert(*(int *) tree->root->left->left->data == 3);
    assert(*(int *) tree->root->left->right->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_leftright_left(void)
{
    printf("%-40s", ">>> ROT left-right + left ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 3, 4, 6, 7};
    
    // creating right-left-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->left->data == 3);

    assert(avl_insert(tree, &data[2]) == 0);
    // left-right-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 5);

    // creating right-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->right->right->data == 6);

    assert(avl_insert(tree, &data[4]) == 0);
    // left-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 6);
    assert(*(int *) tree->root->right->left->data == 5);
    assert(*(int *) tree->root->right->right->data == 7);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_left_rightleft(void)
{
    printf("%-40s", ">>> ROT left + right-left ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 6, 7, 9, 8};
    
    // creating right-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->right->data == 6);

    assert(avl_insert(tree, &data[2]) == 0);
    // left-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    // creating left-right-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->right->right->data == 9);

    assert(avl_insert(tree, &data[4]) == 0);
    // right-left-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 8);
    assert(*(int *) tree->root->right->left->data == 7);
    assert(*(int *) tree->root->right->right->data == 9);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_rightleft_left(void)
{
    printf("%-40s", ">>> ROT right-left + left ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 7, 6, 8, 9};
    
    // creating right-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->right->data == 7);

    assert(avl_insert(tree, &data[2]) == 0);
    // left-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    // creating left-right-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->right->right->data == 8);

    assert(avl_insert(tree, &data[4]) == 0);
    // right-left-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 8);
    assert(*(int *) tree->root->right->left->data == 7);
    assert(*(int *) tree->root->right->right->data == 9);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_right_leftright(void)
{
    printf("%-40s", ">>> ROT right + left-right ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 4, 3, 1, 2};
    
    // creating left-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->left->data == 4);

    assert(avl_insert(tree, &data[2]) == 0);
    // right-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 5);

    // creating right-left-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->left->left->data == 1);

    assert(avl_insert(tree, &data[4]) == 0);
    // left-right-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 2);
    assert(*(int *) tree->root->left->left->data == 1);
    assert(*(int *) tree->root->left->right->data == 3);
    assert(*(int *) tree->root->right->data == 5);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_leftright_right(void)
{
    printf("%-40s", ">>> ROT left-right + right ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 3, 4, 2, 1};
    
    // creating right-left-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->left->data == 3);

    assert(avl_insert(tree, &data[2]) == 0);
    // left-right-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 5);

    // creating left-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->left->left->data == 2);

    assert(avl_insert(tree, &data[4]) == 0);
    // right-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 2);
    assert(*(int *) tree->root->left->left->data == 1);
    assert(*(int *) tree->root->left->right->data == 3);
    assert(*(int *) tree->root->right->data == 5);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_right_rightleft(void)
{
    printf("%-40s", ">>> ROT right + right-left ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 4, 3, 7, 6};
    
    // creating left-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->left->data == 4);

    assert(avl_insert(tree, &data[2]) == 0);
    // right-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 5);

    // creating left-right-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->right->right->data == 7);

    assert(avl_insert(tree, &data[4]) == 0);
    // right-left-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 6);
    assert(*(int *) tree->root->right->left->data == 5);
    assert(*(int *) tree->root->right->right->data == 7);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_rightleft_right(void)
{
    printf("%-40s", ">>> ROT right-left + right ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 7, 6, 4, 3};
    
    // creating left-right-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->right->data == 7);

    assert(avl_insert(tree, &data[2]) == 0);
    // right-left-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    // creating left-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->left->left->data == 4);

    assert(avl_insert(tree, &data[4]) == 0);
    // right-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 4);
    assert(*(int *) tree->root->right->data == 7);
    assert(*(int *) tree->root->left->left->data == 3);
    assert(*(int *) tree->root->left->right->data == 5);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_leftright_leftright(void)
{
    printf("%-40s", ">>> ROT left-right + left-right ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 3, 4, 1, 2};
    
    // creating right-left-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->left->data == 3);

    assert(avl_insert(tree, &data[2]) == 0);
    // left-right-rotation occurs here ^
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 3);
    assert(*(int *) tree->root->right->data == 5);

    // creating right-left-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->left->left->data == 1);

    assert(avl_insert(tree, &data[4]) == 0);
    // left-right-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 4);
    assert(*(int *) tree->root->left->data == 2);
    assert(*(int *) tree->root->left->left->data == 1);
    assert(*(int *) tree->root->left->right->data == 3);
    assert(*(int *) tree->root->right->data == 5);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_rightleft_rightleft(void)
{
    printf("%-40s", ">>> ROT right-left + right-left ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 7, 6, 9, 8};
    
    // creating left-right-heavy tree
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->right->data == 7);

    assert(avl_insert(tree, &data[2]) == 0);
    // right-left-rotation occurs here ^
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 7);

    // creating left-right-heavy tree
    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->right->right->data == 9);

    assert(avl_insert(tree, &data[4]) == 0);
    // right-left-rotation occurs here ^
    //avl_print_int(tree);
    assert(*(int *) tree->root->data == 6);
    assert(*(int *) tree->root->left->data == 5);
    assert(*(int *) tree->root->right->data == 8);
    assert(*(int *) tree->root->right->left->data == 7);
    assert(*(int *) tree->root->right->right->data == 9);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_insert_large(void)
{
    srand(94378348);

    printf("%-40s", ">>> large tree ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    vec_t *data = vec_with_capacity(1000);

    for (int i = 0; i < 1000; ++i) {
        vec_push(data, &i, sizeof(int));
    }

    vec_shuffle(data);

    for (int i = 0; i < 1000; ++i) {
        assert(avl_insert(tree, vec_get(data, i)) == 0);
    }

    // verify the validity of the tree
    // I) depth-first comparison of values in the nodes
    // II) sanity check of the node balance
    // III) sanity check of the height labels
    avl_node_t *node = tree->root;
    vec_t *stack = vec_new();
    vec_push(stack, &node, sizeof(avl_node_t *));

    while (stack->len != 0) {

        void *item = vec_pop(stack);
        node = *(avl_node_t **) item;

        assert(abs(avl_node_balance(node)) <= 1);
        assert(node->height == compute_branch_height(node));

        if (node->right != NULL) {
            assert(avl_compare_ints(node->data, node->right->data) < 0); 
            //printf("%d (parent) vs %d (right)\n", *(int *) node->data, *(int *) node->right->data);
            vec_push(stack, &(node->right), sizeof(avl_node_t *));
        }
        if (node->left != NULL) {
            assert(avl_compare_ints(node->data, node->left->data) > 0);
            //printf("%d (parent) vs %d (left)\n", *(int *) node->data, *(int *) node->left->data);
            vec_push(stack, &(node->left), sizeof(avl_node_t *));
        }

        free(item);
    }

    vec_destroy(stack);

    //printf("Height: %ld\n", avl_height(tree));

    //avl_print_int(tree);

    vec_destroy(data);
    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_height(void)
{
    srand(24347348);

    printf("%-40s", "test_avl_height ");

    size_t height = avl_height(NULL);
    assert(height == 0);

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    height = avl_height(tree);
    assert(height == 0);

    vec_t *data = vec_with_capacity(1000);

    for (int i = 0; i < 1000; ++i) {
        vec_push(data, &i, sizeof(int));
    }

    vec_shuffle(data);

    for (int i = 0; i < 1000; ++i) {
        assert(avl_insert(tree, vec_get(data, i)) == 0);
        if (i == 0) assert(avl_height(tree) == 0);
    }

    assert(avl_height(tree) == compute_branch_height(tree->root));

    vec_destroy(data);
    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static int test_avl_find(void)
{
    srand(94378348);

    printf("%-40s", "test_avl_find ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    vec_t *data = vec_with_capacity(1000);

    for (int i = 0; i < 1000; ++i) {
        vec_push(data, &i, sizeof(int));
    }

    vec_shuffle(data);

    for (int i = 0; i < 1000; ++i) {
        assert(avl_insert(tree, vec_get(data, i)) == 0);
    }

    int to_find[] = {12, 100, 167, 243, 555, 873, 943, 1045, -15};

    assert(avl_find(NULL, &to_find[0]) == NULL);
    assert(*(int *) (avl_find(tree, &to_find[0]))->data == 12);
    assert(*(int *) (avl_find(tree, &to_find[1]))->data == 100);
    assert(*(int *) (avl_find(tree, &to_find[2]))->data == 167);
    assert(*(int *) (avl_find(tree, &to_find[3]))->data == 243);
    assert(*(int *) (avl_find(tree, &to_find[4]))->data == 555);
    assert(*(int *) (avl_find(tree, &to_find[5]))->data == 873);
    assert(*(int *) (avl_find(tree, &to_find[6]))->data == 943);
    assert(avl_find(tree, &to_find[7]) == NULL);
    assert(avl_find(tree, &to_find[8]) == NULL);

    vec_destroy(data);
    avl_destroy(tree);

    printf("OK\n");
    return 0;
}



static int test_avl_insert(void)
{
    printf("test_avl_insert \n");

    test_avl_insert_basic();

    test_avl_insert_left_right();
    test_avl_insert_left_left();
    test_avl_insert_right_left();
    test_avl_insert_right_right();

    test_avl_insert_left_leftright();
    test_avl_insert_leftright_left();
    test_avl_insert_left_rightleft();
    test_avl_insert_rightleft_left();

    test_avl_insert_right_leftright();
    test_avl_insert_leftright_right();
    test_avl_insert_right_rightleft();
    test_avl_insert_rightleft_right();

    test_avl_insert_leftright_leftright();
    test_avl_insert_rightleft_rightleft();

    test_avl_insert_large();

    return 0;

}

static void extract_int(void *item, void *pointer2vector)
{
    vec_t *vector = (vec_t *) pointer2vector;

    vec_push(vector, item, sizeof(int));
}

static void print_int(void *item, void *empty_pointer)
{
    UNUSED(empty_pointer);
    printf("%d ", *(int *) item);
}

static avl_t *construct_tree(const int *data)
{
    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);

    for (size_t i = 0; i < 7; ++i) {
        assert(avl_insert(tree, &data[i]) == 0);
    }
    
    return tree;
}

static int test_avl_map_levelorder(void)
{
    printf("%-40s", "test_avl_map_levelorder ");

    int input[] = {2, 4, 1, 3, 5, 0, 7};
    int expected[] = {2, 1, 4, 0, 3, 5, 7};

    avl_t *tree = construct_tree(input);
    vec_t *vector = vec_new();

    //avl_map_levelorder(tree, print_int, NULL);
    avl_map_levelorder(tree, extract_int, vector);

    for (size_t i = 0; i < 7; ++i) {
        assert(*(int *) vec_get(vector, i) == expected[i]);
    }

    avl_destroy(tree);
    vec_destroy(vector);
    printf("OK\n");
    return 0;
}

static int test_avl_map_inorder(void)
{
    printf("%-40s", "test_avl_map_inorder ");

    int input[] = {2, 4, 1, 3, 5, 0, 7};
    int expected[] = {0, 1, 2, 3, 4, 5, 7};

    avl_t *tree = construct_tree(input);
    vec_t *vector = vec_new();

    //avl_map_inorder(tree, print_int, NULL);
    avl_map_inorder(tree, extract_int, vector);

    for (size_t i = 0; i < 7; ++i) {
        assert(*(int *) vec_get(vector, i) == expected[i]);
    }

    avl_destroy(tree);
    vec_destroy(vector);
    printf("OK\n");
    return 0;
}

static int test_avl_map_preorder(void)
{
    printf("%-40s", "test_avl_map_preorder ");

    int input[] = {2, 4, 1, 3, 5, 0, 7};
    int expected[] = {2, 1, 0, 4, 3, 5, 7};

    avl_t *tree = construct_tree(input);
    vec_t *vector = vec_new();

    //avl_map_preorder(tree, print_int, NULL);
    avl_map_preorder(tree, extract_int, vector);

    for (size_t i = 0; i < 7; ++i) {
        assert(*(int *) vec_get(vector, i) == expected[i]);
    }

    avl_destroy(tree);
    vec_destroy(vector);
    printf("OK\n");
    return 0;
}

static int test_avl_map_postorder(void)
{
    printf("%-40s", "test_avl_map_postorder ");

    int input[] = {2, 4, 1, 3, 5, 0, 7};
    int expected[] = {0, 1, 3, 7, 5, 4, 2};

    avl_t *tree = construct_tree(input);
    vec_t *vector = vec_new();

    //avl_map_postorder(tree, print_int, NULL);
    avl_map_postorder(tree, extract_int, vector);

    for (size_t i = 0; i < 7; ++i) {
        assert(*(int *) vec_get(vector, i) == expected[i]);
    }

    avl_destroy(tree);
    vec_destroy(vector);
    printf("OK\n");
    return 0;
}

static int test_equality_function(const void *data, const void *target)
{
    return *((int *) data) == *((int *) target);
}

static int test_avl_map(void)
{
    printf("%-40s", "test_avl_map ");

    int input[] = {2, 4, 1, 3, 5, 0, 7};

    avl_t *tree = construct_tree(input);
    vec_t *vector = vec_new();

    //avl_map(tree, print_int, NULL);
    avl_map(tree, extract_int, vector);

    // here we just check whether the input ints are present in the vector
    for (size_t i = 0; i < 7; ++i) {
        assert(vec_find(vector, test_equality_function, &input[i]) >= 0);
    }

    avl_destroy(tree);
    vec_destroy(vector);
    printf("OK\n");
    return 0;
}



int main(void)
{

    test_avl_destroy_null();
    test_avl_new();

    test_avl_insert();
    test_avl_height();

    test_avl_find();

    test_avl_map_levelorder();
    test_avl_map_inorder();
    test_avl_map_preorder();
    test_avl_map_postorder();
    test_avl_map();

    return 0;
}
