// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/dlinked_list.h"

static dllist_t *dllist_fill(size_t items)
{
    dllist_t *list = dllist_new();

    for (size_t i = 0; i < items; ++i) {
        int random = rand();

        dllist_push_first(list, &random, sizeof(int));
    }

    return list;
}

static void benchmark_dllist_push_first(size_t items)
{
    printf("%s\n", "benchmark_dllist_push_first [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        dllist_t *list = dllist_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();

            dllist_push_first(list, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, pushing %12lu items: %f s\n", prefilled, items, time_elapsed);

        dllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_dllist_push_last(size_t items)
{
    printf("%s\n", "benchmark_dllist_push_last [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        dllist_t *list = dllist_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();

            dllist_push_last(list, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, pushing %12lu items: %f s\n", prefilled, items, time_elapsed);

        dllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_dllist_get(size_t items)
{
    printf("%s\n", "benchmark_dllist_get [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        dllist_t *list = dllist_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            size_t random_index = rand() % prefilled;

            dllist_get(list, random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, getting %12lu items: %f s\n", prefilled, items, time_elapsed);

        dllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_dllist_insert(size_t items)
{
    printf("%s\n", "benchmark_dllist_insert [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        dllist_t *list = dllist_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random_data = rand();
            size_t random_index = rand() % prefilled;

            dllist_insert(list, &random_data, sizeof(int), random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, inserting %12lu items: %f s\n", prefilled, items, time_elapsed);

        dllist_destroy(list);
    }
    printf("\n");
}

static void benchmark_dllist_remove(size_t items)
{
    printf("%s\n", "benchmark_dllist_remove [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        dllist_t *list = dllist_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            size_t random_index = rand() % prefilled;

            dllist_remove(list, random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, removing %12lu items: %f s\n", prefilled, items, time_elapsed);

        dllist_destroy(list);
    }
    printf("\n");
}

static int filter_function(const void *data)
{
    return *((size_t *) data) % 2;
}

static void benchmark_dllist_filter_mut(void)
{
    printf("%s\n", "benchmark_dllist_filter_mut [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 1000000;
        dllist_t *list = dllist_fill(prefilled);

        clock_t start = clock();

        size_t removed = dllist_filter_mut(list, filter_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, filtered out %12lu items: %f s\n", prefilled, removed, time_elapsed);

        dllist_destroy(list);
    }
    printf("\n");
}


int main(void)
{
    srand(time(NULL));

    benchmark_dllist_push_first(100000);
    benchmark_dllist_push_last(100000);
    benchmark_dllist_get(1000);
    benchmark_dllist_insert(1000);
    benchmark_dllist_remove(1000);
    benchmark_dllist_filter_mut();

}