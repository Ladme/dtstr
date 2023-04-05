// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/queue.h"

#define UNUSED(x) (void)(x)

static int test_queue_destroy_null(void)
{
    printf("%-40s", "test_queue_destroy (null) ");

    queue_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_queue_new(void)
{
    printf("%-40s", "test_queue_new ");

    queue_t *queue = queue_new();

    assert(queue);
    assert(!queue->head);
    assert(!queue->tail);

    queue_destroy(queue);

    printf("OK\n");
    return 0;
}

static int test_queue_operations(void)
{
    printf("%-40s", "test_queue_operations ");

    size_t test_item = 10;
    assert(queue_en(NULL, &test_item, sizeof(size_t)) == 99);

    queue_t *queue = queue_new();

    assert(queue_de(queue, sizeof(size_t)) == NULL);

    for (size_t i = 0; i < 1000; ++i) {
        assert(queue_en(queue, &i, sizeof(size_t)) == 0);
    }

    for (size_t i = 0; i < 100; ++i) {
        void *p_item = queue_peek(queue);
        assert(p_item);
        assert(*(size_t *) p_item == 0);
    }

    for (size_t i = 0; i < 100; ++i) {
        void *p_item = queue_de(queue, sizeof(size_t));
        assert(p_item);
        assert(*(size_t *) p_item == i);
        free(p_item);
    }

    for (size_t i = 0; i < 100; ++i) {
        assert(queue_en(queue, &i, sizeof(size_t)) == 0);
    }

    for (size_t i = 0; i < 100; ++i) {
        void *p_item = queue_peek(queue);
        assert(p_item);
        assert(*(size_t *) p_item == 100);
    }

    for (size_t i = 0; i < 100; ++i) {
        void *p_item = queue_de(queue, sizeof(size_t));
        assert(p_item);
        assert(*(size_t *) p_item == i + 100);
        free(p_item);
    }

    queue_destroy(queue);

    printf("OK\n");
    return 0;
}


static void multiply_by_two(void *item, void *unused)
{
    UNUSED(unused);
    size_t *ptr = (size_t *) item;
    *ptr *= 2;
}


static int test_queue_map(void)
{
    printf("%-40s", "test_queue_map ");

    queue_map(NULL, multiply_by_two, NULL);

    queue_t *queue = queue_new();

    for (size_t i = 0; i < 100; ++i) {
        queue_en(queue, &i, sizeof(size_t));
    }

    queue_map(queue, multiply_by_two, NULL);

    for (size_t i = 0; i < 100; ++i) {
        void *item = queue_de(queue, sizeof(size_t));
        assert( *(size_t *) item == i * 2);
        free(item);
    }

    queue_destroy(queue);

    printf("OK\n");
    return 0;
}


int main(void)
{
    test_queue_destroy_null();
    test_queue_new();

    test_queue_operations();

    test_queue_map();

    return 0;
}
