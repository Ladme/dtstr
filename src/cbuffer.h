// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of dynamic circular buffer. Can be used as queue.

#ifndef CBUFFER_H
#define CBUFFER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct cbuffer {
    size_t len;
    size_t capacity;
    size_t base_capacity;
    size_t head;
    size_t tail;
    void **items;
} cbuf_t;

#define CBUF_DEFAULT_CAPACITY 16UL


cbuf_t *cbuf_with_capacity(const size_t base_capacity);
cbuf_t *cbuf_new(void);
void cbuf_destroy(cbuf_t *buffer);

int cbuf_enqueue(cbuf_t *buffer, const void *item, const size_t itemsize);
void *cbuf_dequeue(cbuf_t *buffer, const size_t itemsize);

void cbuf_map(cbuf_t *buffer, void (*function)(void *, void *), void *pointer);

#endif /* CBUFFER_H */