// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/avl_tree.h"

#define UNUSED(x) (void)(x)

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

        size_t prefilled = i * 10000;
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
    printf("%s\n", "benchmark_avl_height");

    size_t items = 1000000;
    avl_t *tree = avl_new(sizeof(int), avl_compare_ints);
    size_t actually_added = 0;

    for (size_t i = 1; i <= items; ++i) {
        int random = rand();

        if (avl_insert(tree, &random) == 0) ++actually_added;

        if (i % 50000 == 0) {
            printf("> tried inserting %12lu items, actually inserted %12lu items, height is %12lu\n", i, actually_added, avl_height(tree));
        }
    }

    avl_destroy(tree);
    printf("\n");
}


static void benchmark_avl_find(const size_t items)
{
    printf("%s\n", "benchmark_avl_find [O(log n)]");

   for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 10000;
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

static void multiply_by_two(void *item, void *unused)
{
    UNUSED(unused);
    int *integer = (int *) item;
    *integer *= 2;
}

static void benchmark_avl_map(const size_t repeats)
{
    printf("%s\n", "benchmark_avl_map [O(n)]");

   for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 10000;
        avl_t *tree = avl_fill(prefilled);

        // level-order
        clock_t start = clock();
        for (size_t j = 0; j < repeats; ++j) {
            
            avl_map_levelorder(tree, multiply_by_two, NULL);
        }
        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("> LEVEL-ORDER: prefilled with %12lu items, mapping repeats %lu times: %f s\n", prefilled, repeats, time_elapsed);

        // pre-order
        start = clock();
        for (size_t j = 0; j < repeats; ++j) {
            
            avl_map_preorder(tree, multiply_by_two, NULL);
        }
        end = clock();
        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("> PRE-ORDER:   prefilled with %12lu items, mapping repeats %lu times: %f s\n", prefilled, repeats, time_elapsed);

        // in-order
        start = clock();
        for (size_t j = 0; j < repeats; ++j) {
            
            avl_map_inorder(tree, multiply_by_two, NULL);
        }
        end = clock();
        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("> IN-ORDER:    prefilled with %12lu items, mapping repeats %lu times: %f s\n", prefilled, repeats, time_elapsed);

        // post-order
        start = clock();
        for (size_t j = 0; j < repeats; ++j) {
            
            avl_map_postorder(tree, multiply_by_two, NULL);
        }
        end = clock();
        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("> POST-ORDER:  prefilled with %12lu items, mapping repeats %lu times: %f s\n", prefilled, repeats, time_elapsed);

        avl_destroy(tree);
        printf("\n");
    }
    printf("\n");
}


static vec_t *vec_fill(const size_t items)
{
    vec_t *vector = vec_with_capacity(items);

    for (int i = 0; i < items; ++i) {

        vec_push(vector, &i, sizeof(int));
    }

    vec_shuffle(vector);

    return vector;
}


static int vec_equal_function(const void *data, const void *target)
{
    return *((int *) data) == *((int *) target);
}

static int vec_compare_function(const void *first, const void *second)
{
    return *((int *) first) - *((int *) second);
}

static int vec_compare_function_qsort(const void *first, const void *second)
{
    return **((int **) first) - **((int **) second);
}

/** @brief A number of items (prefilled) is inserted into AVL or vector. These structures are then searched `items` times. */
static void benchmarks_search_vec_vs_avl(const size_t fill_factor, const size_t items, const int no_sorting)
{
    printf("%s\n", "benchmark_search_vec_vs_avl");

   for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * fill_factor;
        vec_t *input = vec_fill(prefilled);

        // AVL TREE
        
        clock_t start = clock();

        avl_t *tree = avl_new(sizeof(int), avl_compare_ints);

        for (size_t j = 0; j < prefilled; ++j) {
            assert(avl_insert(tree, vec_get(input, j)) == 0);
        }

        for (size_t j = 0; j < items; ++j) {
            int random = rand() % (int) prefilled;

            assert(avl_find(tree, &random));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> AVL TREE:           filling with %12lu items, searching for %12lu items: %f s\n", prefilled, items, time_elapsed);

        avl_destroy(tree);

        // VECTOR, NO SORTING
        if (no_sorting) {

            start = clock();

            vec_t *vector = vec_with_capacity(prefilled);

            for (size_t j = 0; j < prefilled; ++j) {
                assert(vec_push(vector, vec_get(input, j), sizeof(int)) == 0);
            }

            assert(vector->len == prefilled);

            for (size_t j = 0; j < items; ++j) {
                int random = rand() % (int) prefilled;

                assert(vec_find(vector, vec_equal_function, &random) >= 0);
            }

            end = clock();
            time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

            printf("> VECTOR, NO SORTING: filling with %12lu items, searching for %12lu items: %f s\n", prefilled, items, time_elapsed);

            vec_destroy(vector);

        }
        
        // VECTOR, SORTING
        start = clock();

        vec_t *vector = vec_with_capacity(prefilled);

        for (size_t j = 0; j < prefilled; ++j) {
            assert(vec_push(vector, vec_get(input, j), sizeof(int)) == 0);
        }

        assert(vector->len == prefilled);

        vec_sort_quick(vector, vec_compare_function_qsort);

        for (size_t j = 0; j < items; ++j) {
            int random = rand() % (int) prefilled;

            assert(vec_find_bsearch(vector, vec_compare_function, &random) >= 0);
        }

        end = clock();
        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> VECTOR, SORTING:    filling with %12lu items, searching for %12lu items: %f s\n", prefilled, items, time_elapsed);   

        vec_destroy(vector);

        vec_destroy(input);
        printf("\n");
    }
    printf("\n");
}


int main(void)
{
    srand(time(NULL));
    
    benchmark_avl_insert(10000);
    benchmark_avl_height();
    benchmark_avl_find(10000);

    benchmark_avl_map(20);

    benchmarks_search_vec_vs_avl(1000, 10000, 1);
    benchmarks_search_vec_vs_avl(1000, 100000, 1);
    benchmarks_search_vec_vs_avl(1000, 1000000, 1);

    benchmarks_search_vec_vs_avl(10000, 10000, 1);
    benchmarks_search_vec_vs_avl(10000, 100000, 0);
    benchmarks_search_vec_vs_avl(10000, 1000000, 0);

    benchmarks_search_vec_vs_avl(100000, 10000, 0);
    benchmarks_search_vec_vs_avl(100000, 100000, 0);
    benchmarks_search_vec_vs_avl(100000, 1000000, 0);


    return 0;

}