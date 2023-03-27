// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/vector.h"

static vec_t *vec_fill(size_t items)
{
    vec_t *vector = vec_new();

    for (size_t i = 0; i < items; ++i) {
        int random = rand();

        vec_push(vector, &random, sizeof(int));
    }

    return vector;
}

static vec_t *vec_fill_sorted(size_t items)
{
    vec_t *vector = vec_new();

    for (size_t i = 0; i < items; ++i) {
        int value = (int) i;

        vec_push(vector, &value, sizeof(int));
    }

    return vector;
}

static void benchmark_vec_push(size_t items)
{
    printf("%s\n", "benchmark_vec_push [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            int random = rand();

            vec_push(vector, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, pushing %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_insert(size_t items)
{
    printf("%s\n", "benchmark_vec_insert [O(n)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            int random = rand();

            vec_insert(vector, &random, sizeof(int), 0);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, inserting %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_get(size_t items)
{
    printf("%s\n", "benchmark_vec_get [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 1; i < items; ++i) {
            size_t random_index = rand() % prefilled;

            vec_get(vector, random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, getting %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_pop(size_t items)
{
    printf("%s\n", "benchmark_vec_pop [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 1; i < items; ++i) {
            free(vec_pop(vector));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, popping %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_remove(size_t items)
{
    printf("%s\n", "benchmark_vec_remove [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 1; i < items; ++i) {
            size_t random_index = rand() % prefilled;

            free(vec_remove(vector, random_index));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, removing %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}


static int filter_function(const void *data)
{
    return *((int *) data) % 2;
}

static void benchmark_vec_filter_mut()
{
    printf("%s\n", "benchmark_vec_filter_mut [O(n^2)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 10000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        size_t removed = vec_filter_mut(vector, filter_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, filtered out %12lu items: %f s\n", prefilled, removed, time_elapsed);

        vec_destroy(vector);
    }
    printf("\n");
}

static void benchmark_vec_filter()
{
    printf("%s\n", "benchmark_vec_filter [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 1000000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        vec_t *filtered = vec_filter(vector, filter_function, sizeof(size_t));

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, filtered in %12lu items: %f s\n", prefilled, filtered->len, time_elapsed);

        vec_destroy(vector);
        vec_destroy(filtered);
    }

    printf("\n");
}


static int compare_function(const void *first, const void *second)
{
    return *((int *) first) - *((int *) second);
}

static void benchmark_vec_sort_selection()
{
    printf("%s\n", "benchmark_vec_sort_selection [O(n^2)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 5000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        vec_sort_selection(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_bubble()
{
    printf("%s\n", "benchmark_vec_sort_bubble [O(n^2)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 5000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        vec_sort_bubble(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_bubble_sorted()
{
    printf("%s\n", "benchmark_vec_sort_bubble (sorted data) [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_sorted(prefilled);

        clock_t start = clock();

        vec_sort_bubble(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_quicknaive()
{
    printf("%s\n", "benchmark_vec_sort_quicknaive [O(n^2)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        vec_sort_quicknaive(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static int compare_function_qsort(const void *first, const void *second)
{
    return **((int **) first) - **((int **) second);
}

static void benchmark_vec_sort_quick()
{
    printf("%s\n", "benchmark_vec_sort_quick [O(n^2)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill(prefilled);

        clock_t start = clock();

        vec_sort_quick(vector, compare_function_qsort);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}



int main(void)
{
    /*benchmark_vec_push(100000);
    benchmark_vec_get(100000);
    benchmark_vec_insert(10000);
    benchmark_vec_pop(100000);
    benchmark_vec_remove(10000);
    benchmark_vec_filter_mut();
    benchmark_vec_filter();*/

    //benchmark_vec_sort_selection();
    //benchmark_vec_sort_bubble();
    benchmark_vec_sort_bubble_sorted();
    benchmark_vec_sort_quicknaive();
    benchmark_vec_sort_quick();

    return 0;

}