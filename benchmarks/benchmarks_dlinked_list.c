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



static void repeat_dllist_push_first(size_t repeats)
{
    dllist_t *list = dllist_new();
    clock_t start = clock();

    for (size_t i = 0; i < repeats; ++i) {
        int random = rand();

        dllist_push_first(list, &random, sizeof(int));
    }

    clock_t end = clock();
    double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("> %12lu repeats: %f s\n", repeats, time_elapsed);
    dllist_destroy(list);
}

static void benchmark_dllist_push_first(void)
{
    printf("%-40s\n", "benchmark_dllist_push_first ");
    repeat_dllist_push_first(100000);
    printf("\n");
}



static void repeat_dllist_push_last(size_t repeats)
{
    dllist_t *list = dllist_new();
    clock_t start = clock();

    for (size_t i = 0; i < repeats; ++i) {
        int random = rand();

        dllist_push_last(list, &random, sizeof(int));
    }

    clock_t end = clock();
    double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("> %12lu repeats: %f s\n", repeats, time_elapsed);
    dllist_destroy(list);
}

static void benchmark_dllist_push_last(void)
{
    printf("%-40s\n", "benchmark_dllist_push_last ");
    repeat_dllist_push_last(100000);
    printf("\n");
}


static void benchmark_dllist_get(size_t repeats)
{
    printf("%-40s\n", "benchmark_dllist_get [100,000 items] ");

    dllist_t *list = dllist_fill(100000);

    clock_t start = clock();

    for (size_t i = 0; i < repeats; ++i) {
        int random_index = rand() % 100000;
        //printf("%d\n", *((int *) llist_get(list, random_index)));
        dllist_get(list, random_index);
    }

    clock_t end = clock();
    double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("> %12lu repeats: %f s\n\n", repeats, time_elapsed);
}


int main(void)
{
    srand(time(NULL));

    benchmark_dllist_push_first();
    benchmark_dllist_push_last();

    benchmark_dllist_get(50000);

}