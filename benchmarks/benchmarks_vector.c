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


int main(void)
{
    benchmark_vec_push(100000);
    benchmark_vec_get(100000);
    benchmark_vec_insert(10000);
    benchmark_vec_pop(100000);
    benchmark_vec_remove(10000);

    return 0;

}