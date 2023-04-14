// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of an array-based binary heap.
// Nodes are stored in a dynamic array, in level-order.

#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct heap {
    void **items;
    size_t len;
    size_t capacity;
    size_t base_capacity;
    size_t datasize;
    int (*compare_function)(const void *, const void *);
} heap_t;

#define HEAP_DEFAULT_CAPACITY 16UL

/**
 * @brief Creates a new `heap_t` structure and allocates memory for it.
 *
 * @param datasize          The size of each item's data in bytes
 * @param compare_function  The function to use to compare the items in the heap
 * 
 * @note - To release the memory allocated for `heap_t`, use the `heap_destroy` function.
 * @note - Allocates space for `HEAP_DEFAULT_CAPACITY` items. 
 *         This space is dynamically expanded when needed but expanding the heap is a costly operation.
 *         You may want to preallocate memory for a specific number of items using `heap_with_capacity` function.
 * 
 * @note - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * @note - If you want to build a MIN heap, the compare function should have the following behavior:
 * If the first item is greater than the second, the function should return a positive integer.
 * If the first item is smaller, it should return a negative integer.
 * If the two items are equal, it should return 0.
 * @note - If you want to build a MAX heap, the compare function should have the following behavior:
 * If the first item is greater than the second, the function should return a negative integer.
 * If the first item is smaller, it should return a positive integer.
 * If the two items are equal, it should return 0.
 * 
 * @return A pointer to the newly created `heap_t` structure if successful; otherwise, NULL.
 */
heap_t *heap_new(const size_t datasize, int (*compare_function)(const void *, const void *));


/**
 * @brief Creates a new `heap_t` structure and preallocates space for a specified number of items.
 *
 * @param base_capacity     The initial capacity of the heap
 * @param datasize          The size of each item's data in bytes
 * @param compare_function  The function to use to compare the items in the heap
 * 
 * @note - To release the memory allocated for `heap_t`, use the `heap_destroy` function.
 * @note - The heap will never shrink below the specified `base_capacity`.
 * @note - See documentation of `heap_new` for the expected behavior of the `compare_function`.
 * 
 * @return A pointer to the newly created `heap_t` structure if successful; otherwise, NULL.
 */
heap_t *heap_with_capacity(const size_t base_capacity, const size_t datasize, int (*compare_function)(const void *, const void *));


/**
 * @brief Properly deallocates memory for the given `heap` and destroys the `heap_t` structure.
 *
 * @param heap    The `heap_t` structure to destroy.
 */
void heap_destroy(heap_t *heap);


/**
 * @brief Inserts an item into a heap.
 *
 * @param heap      Heap to insert the item into
 * @param item      Void pointer to the item to be inserted
 *
 * @note - Asymptotic Complexity: Logarithmic, O(log n)
 * 
 * @return Returns 0 on success, 1 if memory allocation fails, and 99 if the tree is NULL.
 */
int heap_insert(heap_t *heap, const void *item);


/** 
 * @brief Returns the number of items in heap.
 *
 * @param heap  Concerned heap
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return Number of items in heap. If vector is NULL, returns 0.
 */
inline size_t heap_len(const heap_t *heap) 
{ 
    return (heap == NULL) ? 0 : heap->len; 
}


/**
 * @brief Returns the minimum/maximum of the heap depending on its nature.
 *
 * @param heap      Heap to peek at
 *
 * @note - The returned pointer is no longer valid once the parent heap is destroyed.
 * 
 * @note - Asymptotic Complexity: Constant, O(1).
 * 
 * @return Void pointer to the minimum/maximum of the heap. NULL if the heap is NULL or if there are no items.
 */
void *heap_peek(const heap_t *heap);


/**
 * @brief Removes the minimum/maximum of the heap and returns it.
 * 
 * @param heap      Heap to pop
 *
 * @@note - The caller is responsible for deallocating memory for the popped item.
 * 
 * @note - Asymptotic Complexity: Constant, O(1).
 * 
 * @return Void pointer to the minimum/maximum of the heap. NULL if the heap is NULL or if there are no items.
 */
void *heap_pop(heap_t *heap);


/** 
 * @brief Loops through all nodes in heap and applies 'function' to each node.
 * 
 * @param heap      Heap to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Nodes are traversed in level-order.
 * @note - Note that modifying the values stored in a heap using the function can disrupt the balance of the heap.
 */
void heap_map(heap_t *heap, void (*function)(void *, void *), void *pointer);

#endif /* HEAP_H */