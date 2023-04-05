// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/cbuffer.h"
#include "../src/queue.h"

#define UNUSED(x) (void)(x)

static void benchmark_queue_enqueue(void)
{
    printf("%s\n", "benchmark_queue_enqueue [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        queue_t *queue = queue_new();
        size_t items = (i == 0) ? 10000 : i * 1000000;

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();
            queue_en(queue, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> LINKED LIST-BASED QUEUE: enqueueing %12lu items: %f s\n", items, time_elapsed);

        queue_destroy(queue);
    }

    printf("\n");
}

static void benchmark_cbuf_enqueue(void)
{
    printf("%s\n", "benchmark_cbuf_enqueue [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        cbuf_t *queue = cbuf_new();
        size_t items = (i == 0) ? 10000 : i * 1000000;

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();
            cbuf_enqueue(queue, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> CIRCULAR BUFFER-BASED QUEUE: enqueueing %12lu items: %f s\n", items, time_elapsed);

        cbuf_destroy(queue);
    }

    printf("\n");
}

static void benchmark_cbuf_enqueue_with_capacity(void)
{
    printf("%s\n", "benchmark_cbuf_enqueue_with_capacity [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t items = (i == 0) ? 10000 : i * 1000000;
        cbuf_t *queue = cbuf_with_capacity(items);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();
            cbuf_enqueue(queue, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> CIRCULAR BUFFER-BASED QUEUE: enqueueing %12lu items: %f s\n", items, time_elapsed);

        cbuf_destroy(queue);
    }

    printf("\n");
}

static void benchmark_queue_dequeue(void)
{
    printf("%s\n", "benchmark_queue_dequeue [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        queue_t *queue = queue_new();
        size_t items = (i == 0) ? 10000 : i * 1000000;

        for (size_t j = 0; j < items; ++j) {
            int random = rand();
            queue_en(queue, &random, sizeof(int));
        }

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            free(queue_de(queue, sizeof(int)));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> LINKED LIST-BASED QUEUE: dequeueing %12lu items: %f s\n", items, time_elapsed);

        queue_destroy(queue);
    }

    printf("\n");
}

static void benchmark_cbuf_dequeue(void)
{
    printf("%s\n", "benchmark_cbuf_dequeue [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        cbuf_t *queue = cbuf_new();
        size_t items = (i == 0) ? 10000 : i * 1000000;

        for (size_t j = 0; j < items; ++j) {
            int random = rand();
            cbuf_enqueue(queue, &random, sizeof(int));
        }

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            free(cbuf_dequeue(queue));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> CIRCULAR BUFFER-BASED QUEUE: dequeueing %12lu items: %f s\n", items, time_elapsed);

        cbuf_destroy(queue);
    }

    printf("\n");
}


int main(void)
{
    srand(time(NULL));

    benchmark_queue_enqueue();
    benchmark_cbuf_enqueue();
    benchmark_cbuf_enqueue_with_capacity();

    benchmark_queue_dequeue();
    benchmark_cbuf_dequeue();

    return 0;

}