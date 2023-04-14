// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/vector.h"
#include "../src/heap.h"

#define UNUSED(x) (void)(x)

static int min_heap_compare_ints(const void *x, const void *y)
{
    return (*(int *) x - *(int *) y);
}

static heap_t *heap_fill(const size_t items)
{
    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);

    for (size_t i = 0; i < items; ++i) {
        int random = rand();

        heap_insert(heap, &random);
    }

    return heap;
}


static void benchmark_heap_insert(const size_t items)
{
    printf("%s\n", "benchmark_heap_insert [O(log n)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        heap_t *heap = heap_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();

            heap_insert(heap, &random);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, inserting %12lu items: %f s\n", prefilled, items, time_elapsed);

        heap_destroy(heap);
    }

    printf("\n");
}

static void benchmark_heap_peek(const size_t items)
{
    printf("%s\n", "benchmark_heap_peek [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        heap_t *heap = heap_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            heap_peek(heap);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, peeking %12lu times: %f s\n", prefilled, items, time_elapsed);

        heap_destroy(heap);
    }

    printf("\n");
}


static void benchmark_heap_pop(const size_t items)
{
    printf("%s\n", "benchmark_heap_pop [O(log n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        heap_t *heap = heap_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            free(heap_pop(heap));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, popping %12lu items: %f s\n", prefilled, items, time_elapsed);

        heap_destroy(heap);
    }

    printf("\n");
}


static void benchmark_heapsort(void)
{
    printf("%s\n", "benchmark_heapsort");

    for (size_t i = 1; i <= 10; ++i) {

        clock_t start = clock();

        heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);

        for (int j = 0; j < i * 100000; ++j) {
            int random = rand();
            heap_insert(heap, &random);
        }

        for (int j = 0; j < i * 100000; ++j) {
            free(heap_pop(heap));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> loading and sorting %lu items: %f s\n", i * 100000, time_elapsed);

        heap_destroy(heap);
    }

    printf("\n");
}

static void benchmark_quicksort(void)
{
    printf("%s\n", "benchmark_quicksort");

    for (size_t i = 1; i <= 10; ++i) {

        clock_t start = clock();

        vec_t *vector = vec_new();

        for (int j = 0; j < i * 100000; ++j) {
            int random = rand();
            vec_push(vector, &random, sizeof(int));
        }

        vec_sort_quicknaive(vector, min_heap_compare_ints);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> loading and sorting %lu items: %f s\n", i * 100000, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}


int main(void)
{
    srand(time(NULL));
    
    benchmark_heap_insert(100000);
    benchmark_heap_peek(100000);
    benchmark_heap_pop(100000);

    benchmark_heapsort();
    benchmark_quicksort();


    return 0;

}