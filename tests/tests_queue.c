// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/queue.h"

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
        void *p_item = queue_de(queue, sizeof(size_t));
        assert(p_item);
        assert(*(size_t *) p_item == i);
        free(p_item);
    }

    for (size_t i = 0; i < 100; ++i) {
        assert(queue_en(queue, &i, sizeof(size_t)) == 0);
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


int main(void)
{
    test_queue_destroy_null();
    test_queue_new();

    test_queue_operations();

    return 0;
}
