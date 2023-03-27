// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/clinked_list.h"


static cllist_t *cllist_fill(size_t items)
{
    cllist_t *list = cllist_new();

    for (size_t i = 0; i < items; ++i) {
        int random = rand();

        cllist_push_first(list, &random, sizeof(int));
    }

    return list;
}

static void benchmark_cllist_push_first(size_t items)
{
    printf("%s\n", "benchmark_cllist_push_first [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        cllist_t *list = cllist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            int random = rand();

            cllist_push_first(list, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, pushing %12lu items: %f s\n", prefilled, items, time_elapsed);

        cllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_cllist_push_last(size_t items)
{
    printf("%s\n", "benchmark_cllist_push_last [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        cllist_t *list = cllist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            int random = rand();

            cllist_push_last(list, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, pushing %12lu items: %f s\n", prefilled, items, time_elapsed);

        cllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_cllist_get(size_t items)
{
    printf("%s\n", "benchmark_cllist_get [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        cllist_t *list = cllist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            size_t random_index = rand() % prefilled;

            cllist_get(list, random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, getting %12lu items: %f s\n", prefilled, items, time_elapsed);

        cllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_cllist_insert(size_t items)
{
    printf("%s\n", "benchmark_cllist_insert [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        cllist_t *list = cllist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            int random_data = rand();
            size_t random_index = rand() % prefilled;

            cllist_insert(list, &random_data, sizeof(int), random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, inserting %12lu items: %f s\n", prefilled, items, time_elapsed);

        cllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_cllist_remove(size_t items)
{
    printf("%s\n", "benchmark_cllist_remove [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        cllist_t *list = cllist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            size_t random_index = rand() % prefilled;

            cllist_remove(list, random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, removing %12lu items: %f s\n", prefilled, items, time_elapsed);

        cllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_cllist_rotate(size_t repeats)
{
    printf("%s\n", "benchmark_cllist_rotate [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        cllist_t *list = cllist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < repeats; ++i) {
            int random_rotation = rand() % 10000;
            int negative = rand() % 2;

            if (negative) cllist_rotate(list, -random_rotation);
            else cllist_rotate(list, random_rotation);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, rotating %12lu times: %f s\n", prefilled, repeats, time_elapsed);

        cllist_destroy(list);
    }
    printf("\n");
}

static int filter_function(const void *data)
{
    return *((size_t *) data) % 2;
}

static void benchmark_cllist_filter_mut()
{
    printf("%s\n", "benchmark_cllist_filter_mut [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 1000000;
        cllist_t *list = cllist_fill(prefilled);

        clock_t start = clock();

        size_t removed = cllist_filter_mut(list, filter_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, filtered out %12lu items: %f s\n", prefilled, removed, time_elapsed);

        cllist_destroy(list);
    }
    printf("\n");
}

int main(void)
{
    srand(time(NULL));

    benchmark_cllist_push_first(100000);
    benchmark_cllist_push_last(100000);
    benchmark_cllist_get(1000);
    benchmark_cllist_insert(1000);
    benchmark_cllist_remove(1000);
    benchmark_cllist_rotate(10000);
    benchmark_cllist_filter_mut();
}