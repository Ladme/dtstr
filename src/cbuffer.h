// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of dynamic circular buffer. 
// Can be used as queue and is usually faster than the linked-list based queue (see queue.h), 
// especially if you preallocate memory for the buffer with `cbuf_with_capacity`.

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

/**
 * @brief Creates a new `cbuf_t` structure and preallocates space for a specified number of items.
 *
 * @param base_capacity     The initial capacity of the buffer
 * 
 * @note - To release the memory allocated for `cbuf_t`, use the `cbuf_destroy` function.
 * @note - The buffer will never shrink below the specified `base_capacity`.
 * 
 * @return Pointer to the created buffer, or NULL if memory allocation was unsuccessful.
 */
cbuf_t *cbuf_with_capacity(const size_t base_capacity);


/**
 * @brief Creates a new dynamic circular buffer and allocates memory for it.
 * 
 * @note - To release the memory allocated for `cbuf_t`, use the `cbuf_destroy` function.
 * @note - Allocates space for `CBUF_DEFAULT_CAPACITY` items. 
 *         This space is dynamically expanded when needed but expanding the buffer is a costly operation.
 *         You may want to preallocate memory for a specific number of items using `cbuf_with_capacity` function.
 * 
 * @return Pointer to the created buffer, or NULL if memory allocation was unsuccessful.
 */
cbuf_t *cbuf_new(void);


/**
 * @brief Properly deallocates memory for the given `buffer` and destroys the `cbuf_t` structure.
 *
 * @param buffer    The `cbuf_t` structure to destroy.
 */
void cbuf_destroy(cbuf_t *buffer);


/**
 * @brief Enqueues (adds) an item to circular buffer.
 *
 * @param buffer    Circular buffer to which the item should be added
 * @param item      Item to add
 * @param itemsize  Size of the item to add
 *
 * @note - Data provided using the 'item' pointer is copied to the buffer. You can therefore freely deallocate the original data.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int cbuf_enqueue(cbuf_t *buffer, const void *item, const size_t itemsize);


/**
 * @brief Dequeues an item from circular buffer.
 *
 * @param buffer    Circular buffer from which an item should be dequeued
 * 
 * @note - The caller is responsible for deallocating memory for the dequeued item.
 *
 * @note - Asymptotic Complexity: Constant, O(1).
 * 
 * @return Void pointer to the dequeued item. NULL if not successful.
 */
void *cbuf_dequeue(cbuf_t *buffer);


/**
 * @brief Returns the item at the head of the circular buffer. Does not modify the buffer.
 *
 * @param buffer     Buffer to peek at
 *
 * @note - The returned pointer is no longer valid once the parent buffer is destroyed.
 * 
 * @note - Asymptotic Complexity: Constant, O(1).
 * 
 * @return Void pointer to the item at the head of buffer. NULL if not successful.
 */
void *cbuf_peek(const cbuf_t *buffer);


/** 
 * @brief Returns the number of items in circular buffer.
 *
 * @param buffer  Concerned buffer.
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return Number of items in circular buffer. If buffer is NULL, returns 0.
 */
extern inline size_t cbuf_len(const cbuf_t *buffer) 
{ 
    return (buffer == NULL) ? 0 : buffer->len; 
}


/** 
 * @brief Loops through all items in circular buffer and applies 'function' to each item.
 * 
 * @param list      A pointer to the buffer to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Items are traversed starting from item positioned at the `tail` index.
 */
void cbuf_map(cbuf_t *buffer, void (*function)(void *, void *), void *pointer);

#endif /* CBUFFER_H */