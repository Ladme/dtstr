// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/avl_tree.h"

static int avl_compare_ints(const void *x, const void *y)
{
    return (*(int *) x - *(int *) y);
}

static avl_t *avl_fill(const size_t items)
{
    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);

    vec_t *vector = vec_with_capacity(items);

    for (int i = 0; i < items; ++i) {
        vec_push(vector, &i, sizeof(int));
    }

    vec_shuffle(vector);

    for (size_t i = 0; i < items; ++i) {
        avl_insert(tree, vector->items[i]);
    }

    vec_destroy(vector);

    return tree;
}

static avl_t *avl_fill_random(const size_t items)
{
    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);

    for (size_t i = 0; i < items; ++i) {
        int random = rand();
        avl_insert(tree, &random);
    }

    return tree;
}

static void benchmark_avl_insert(const size_t items)
{
    printf("%s\n", "benchmark_avl_insert [O(log n)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 5000;
        avl_t *tree = avl_fill_random(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();

            avl_insert(tree, &random);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, inserting %12lu items: %f s\n", prefilled, items, time_elapsed);

        avl_destroy(tree);
    }
    printf("\n");
}


static void benchmark_avl_height(void)
{
    printf("%s\n", "benchmark_avl_height [O(log n)]");


    size_t items = 100000;
    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);

    for (size_t i = 0; i <= items; ++i) {
        int random = rand();

        avl_insert(tree, &random);
        if (i % 5000 == 0)  printf("> tree contains %12lu items, height is %12lu\n", i, avl_height(tree));
    }

    avl_destroy(tree);
    printf("\n");
}


static void benchmark_avl_find(const size_t items)
{
    printf("%s\n", "benchmark_avl_find [O(log n)]");

   for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 5000;
        avl_t *tree = avl_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand() % (int) prefilled;

            assert(avl_find(tree, &random));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, searching for %12lu items: %f s\n", prefilled, items, time_elapsed);

        avl_destroy(tree);
    }
    printf("\n");
}


int main(void)
{
    srand(time(NULL));
    
    //benchmark_avl_insert(1000);
    //benchmark_avl_height();
    benchmark_avl_find(10000);

    return 0;

}