// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/linked_list.h"

static llist_t *llist_fill(size_t items)
{
    llist_t *list = llist_new();

    for (size_t i = 0; i < items; ++i) {
        int random = rand();

        llist_push_first(list, &random, sizeof(int));
    }

    return list;
}

static void benchmark_llist_push_first(size_t items)
{
    printf("%s\n", "benchmark_llist_push_first [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        llist_t *list = llist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            int random = rand();

            llist_push_first(list, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, pushing %12lu items: %f s\n", prefilled, items, time_elapsed);

        llist_destroy(list);
    }
    printf("\n");
}

static void benchmark_llist_push_last(size_t items)
{
    printf("%s\n", "benchmark_llist_push_last [O(n)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        llist_t *list = llist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            int random = rand();

            llist_push_last(list, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, pushing %12lu items: %f s\n", prefilled, items, time_elapsed);

        llist_destroy(list);
    }
    printf("\n");
}

static void benchmark_llist_get(size_t items)
{
    printf("%s\n", "benchmark_llist_get [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        llist_t *list = llist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            size_t random_index = rand() % prefilled;

            llist_get(list, random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, getting %12lu items: %f s\n", prefilled, items, time_elapsed);

        llist_destroy(list);
    }
    printf("\n");
}

static void benchmark_llist_insert(size_t items)
{
    printf("%s\n", "benchmark_llist_insert [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        llist_t *list = llist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            int random_data = rand();
            size_t random_index = rand() % prefilled;

            llist_insert(list, &random_data, sizeof(int), random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, inserting %12lu items: %f s\n", prefilled, items, time_elapsed);

        llist_destroy(list);
    }
    printf("\n");
}

static void benchmark_llist_remove(size_t items)
{
    printf("%s\n", "benchmark_llist_remove [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        llist_t *list = llist_fill(prefilled);

        clock_t start = clock();

        for (size_t i = 0; i < items; ++i) {
            size_t random_index = rand() % prefilled;

            llist_remove(list, random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, removing %12lu items: %f s\n", prefilled, items, time_elapsed);

        llist_destroy(list);
    }
    printf("\n");
}

static int filter_function(void *data)
{
    return *((size_t *) data) % 2;
}

static void benchmark_llist_filter_mut()
{
    printf("%s\n", "benchmark_llist_filter_mut [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 1000000;
        llist_t *list = llist_fill(prefilled);

        clock_t start = clock();

        size_t removed = llist_filter_mut(list, filter_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, filtered out %12lu items: %f s\n", prefilled, removed, time_elapsed);

        llist_destroy(list);
    }
    printf("\n");
}


int main(void)
{
    srand(time(NULL));

    benchmark_llist_push_first(100000);
    benchmark_llist_push_last(1000);
    benchmark_llist_get(1000);
    benchmark_llist_insert(1000);
    benchmark_llist_remove(1000);
    benchmark_llist_filter_mut();
}