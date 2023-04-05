// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/cbuffer.h"

#define UNUSED(x) (void)(x)

static void print_int(void *item, void *unused)
{
    UNUSED(unused);
    printf("%d ", *(int *) item);
}

inline static void print_int_buffer(cbuf_t *buffer)
{
    cbuf_map(buffer, print_int, NULL);
    printf("\n");
}

static int test_cbuf_destroy_null(void)
{
    printf("%-40s", "test_cbuf_destroy (null) ");

    cbuf_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_cbuf_new(void) 
{
    printf("%-40s", "test_cbuf_new ");

    cbuf_t *buffer = cbuf_new();

    assert(buffer);
    assert(buffer->len == 0);
    assert(buffer->capacity == CBUF_DEFAULT_CAPACITY);
    assert(buffer->base_capacity == CBUF_DEFAULT_CAPACITY);

    cbuf_destroy(buffer);

    printf("OK\n");
    return 0;
}

static int test_cbuf_enqueue(void)
{
    printf("%-40s", "test_cbuf_enqueue ");

    cbuf_t *buffer = cbuf_new();

    for (int i = 0; i < 130; ++i) {
        assert(cbuf_enqueue(buffer, &i, sizeof(int)) == 0);
    }

    for (int i = 0; i < 130; ++i) {
        assert(*(int *) buffer->items[i] == i);
    }

    //print_int_buffer(buffer);
    assert(buffer->len == 130);
    assert(buffer->capacity == 256);
    assert(buffer->head == 130);
    assert(buffer->tail == 0);

    cbuf_destroy(buffer);

    printf("OK\n");
    return 0;
}

static int test_cbuf_dequeue(void)
{
    printf("%-40s", "test_cbuf_dequeue ");

    cbuf_t *buffer = cbuf_new();

    for (int i = 0; i < 130; ++i) {
        assert(cbuf_enqueue(buffer, &i, sizeof(int)) == 0);
    }

    for (int i = 0; i < 130; ++i) {
        void *data = cbuf_dequeue(buffer, sizeof(int));
        assert(data);
        assert(*(int *) data == i);
        free(data);
    }

    //print_int_buffer(buffer);
    assert(buffer->len == 0);
    assert(buffer->capacity == CBUF_DEFAULT_CAPACITY);
    assert(buffer->head == 8);
    assert(buffer->tail == 8);

    cbuf_destroy(buffer);

    printf("OK\n");
    return 0;
}

static int test_cbuf_enqueue_dequeue(void)
{
    printf("%-40s", "test_cbuf_enqueue_dequeue ");

    cbuf_t *buffer = cbuf_new();

    for (int i = 0; i < 130; ++i) {
        assert(cbuf_enqueue(buffer, &i, sizeof(int)) == 0);
    }

    assert(buffer->len == 130);
    assert(buffer->head == 130);
    assert(buffer->tail == 0);

    for (int i = 0; i < 60; ++i) {
        void *data = cbuf_dequeue(buffer, sizeof(int));
        assert(data);
        assert(*(int *) data == i);
        free(data);
    }

    assert(buffer->capacity == 256);
    assert(buffer->len == 70);
    assert(buffer->head == 130);
    assert(buffer->tail == 60);

    for (int i = 130; i < 256; ++i) {
        assert(cbuf_enqueue(buffer, &i, sizeof(int)) == 0);
    }

    assert(buffer->capacity == 256);
    assert(buffer->len == 196);
    assert(buffer->head == 0);
    assert(buffer->tail == 60);

    // wrapped to the start of the buffer
    for (int i = 500; i < 560; ++i) {
        assert(cbuf_enqueue(buffer, &i, sizeof(int)) == 0);
    }

    //print_int_buffer(buffer);
    assert(buffer->head == buffer->tail);
    assert(buffer->head == 60);
    assert(buffer->capacity == 256);
    assert(buffer->len == 256);

    // remove all data, keeping only the last 20 items
    for (int i = 0; i < 236; ++i) {
        void *data = cbuf_dequeue(buffer, sizeof(int));
        assert(data);
        free(data);
    }

    assert(buffer->capacity == 64);
    assert(buffer->len == 20);
    assert(buffer->head == 32);
    assert(buffer->tail == 12);

    //print_int_buffer(buffer);

    cbuf_destroy(buffer);

    printf("OK\n");
    return 0;

}

int main(void) 
{
    
    test_cbuf_destroy_null();
    test_cbuf_new();
    test_cbuf_enqueue();
    test_cbuf_dequeue();    
    test_cbuf_enqueue_dequeue();

    return 0;
}