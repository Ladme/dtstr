// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/avl_tree.h"

static int avl_compare_ints(const void *x, const void *y)
{
    return (*(int *) x - *(int *) y);
}

inline static void avl_print_int(const avl_t *tree)
{
    queue_t *queue = queue_new();
    queue_t *levels = queue_new();

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
            queue_en(queue, &(node->left), sizeof(avl_node_t *));
            queue_en(levels, &level, sizeof(int));
        }
        if (node->right != NULL) {
            int level = parent_level + 1;
            queue_en(queue, &(node->right), sizeof(avl_node_t *));
            queue_en(levels, &level, sizeof(int));
        }

        free(item);
        item = queue_de(queue, sizeof(avl_node_t *));
        if (item == NULL) node = NULL;
        else node = *(avl_node_t **) item;

        void *p_parent_level = queue_de(levels, sizeof(int *));
        parent_level = *(int *) p_parent_level;
        free(p_parent_level);
    }

    free(item);
    queue_destroy(queue);
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

static int test_avl_insert(void)
{
    printf("%-40s", "test_avl_insert ");

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    
    int data[] = {5, 3, 4, 6, 7, 2, 9, 0, 1, 7};
    
    assert(avl_insert(tree, &data[0]) == 0);
    assert(*(int *) tree->root->data == 5);

    assert(avl_insert(tree, &data[1]) == 0);
    assert(*(int *) tree->root->left->data == 3);

    assert(avl_insert(tree, &data[2]) == 0);
    assert(*(int *) tree->root->left->right->data == 4);

    assert(avl_insert(tree, &data[3]) == 0);
    assert(*(int *) tree->root->right->data == 6);

    assert(avl_insert(tree, &data[4]) == 0);
    assert(*(int *) tree->root->right->right->data == 7);

    assert(avl_insert(tree, &data[5]) == 0);
    assert(*(int *) tree->root->left->left->data == 2);

    assert(avl_insert(tree, &data[6]) == 0);
    assert(*(int *) tree->root->right->right->right->data == 9);

    assert(avl_insert(tree, &data[7]) == 0);
    assert(*(int *) tree->root->left->left->left->data == 0);

    assert(avl_insert(tree, &data[8]) == 0);
    assert(*(int *) tree->root->left->left->left->right->data == 1);

    assert(avl_insert(tree, &data[9]) == 1);
    assert(*(int *) tree->root->left->left->left->right->data == 1);

    //avl_print_int(tree);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

static void multiply_by_two(void *item)
{
    int *ptr = (int *) item;
    *ptr *= 2;
}

static int test_avl_map(void)
{
    printf("%-40s", "test_avl_map ");

    avl_map(NULL, multiply_by_two);

    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);

    int data[] = {5, 3, 4, 6, 7, 2, 9, 0, 1, 7};

    for (size_t i = 0; i < 9; ++i) {
        avl_insert(tree, &data[i]);
    }

    avl_map(tree, multiply_by_two);

    assert(*(int *) tree->root->data == 10);
    assert(*(int *) tree->root->left->data == 6);
    assert(*(int *) tree->root->left->right->data == 8);
    assert(*(int *) tree->root->right->data == 12);
    assert(*(int *) tree->root->right->right->data == 14);
    assert(*(int *) tree->root->left->left->data == 4);
    assert(*(int *) tree->root->right->right->right->data == 18);
    assert(*(int *) tree->root->left->left->left->data == 0);
    assert(*(int *) tree->root->left->left->left->right->data == 2);

    //avl_print_int(tree);

    avl_destroy(tree);

    printf("OK\n");
    return 0;
}

int main(void)
{
    test_avl_destroy_null();
    test_avl_new();

    test_avl_insert();

    test_avl_map();

    return 0;
}
