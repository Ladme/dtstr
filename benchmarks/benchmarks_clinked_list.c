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

int main(void)
{
    srand(time(NULL));

    benchmark_cllist_push_first(100000);
    benchmark_cllist_push_last(100000);
    benchmark_cllist_get(1000);

}