// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "heap.h"

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH HEAP_T                   */
/* *************************************************************************** */

/** Returns the index of the parent node in a heap given a node index. */
inline static size_t heap_index_parent(const size_t node)
{
    return (node - 1) / 2;
}

/** Returns the index of the left child node in a heap given a node index. */
inline static size_t heap_index_left(const size_t node)
{
    return (2 * node) + 1;
}

/** Returns the index of the right child node in a heap given a node index. */
inline static size_t heap_index_right(const size_t node)
{
    return (2 * node) + 2;
}

/*! @brief Swaps two items in a heap. */
inline static void heap_swap(heap_t *heap, const size_t i, const size_t j)
{
    void *tmp = heap->items[i];
    heap->items[i] = heap->items[j];
    heap->items[j] = tmp;
}

/*! @brief Reallocates memory for heap. Returns 0, if successful. Else return non-zero. */
static int heap_reallocate(heap_t *heap)
{
    heap->capacity *= 2;
    void **new_items = realloc(heap->items, heap->capacity * sizeof(void *));
    if (new_items == NULL) {
        heap_destroy(heap);
        return 1;
    }

    heap->items = new_items;
    memset(heap->items + heap->len, 0, sizeof(void *) * heap->capacity / 2);

    return 0;
}

/* *************************************************************************** */
/*                  PUBLIC FUNCTIONS ASSOCIATED WITH HEAP_T                    */
/* *************************************************************************** */


heap_t *heap_new(const size_t datasize, int (*compare_function)(const void *, const void *))
{
    return heap_with_capacity(HEAP_DEFAULT_CAPACITY, datasize, compare_function);
}


heap_t *heap_with_capacity(const size_t base_capacity, const size_t datasize, int (*compare_function)(const void *, const void *))
{
    heap_t *heap = calloc(1, sizeof(heap_t));
    if (heap == NULL) return NULL;

    heap->items = calloc(base_capacity, sizeof(void *));
    if (heap->items == NULL) {
        free(heap);
        return NULL;
    }

    heap->capacity = base_capacity;
    heap->base_capacity = base_capacity;
    heap->compare_function = compare_function;
    heap->datasize = datasize;

    return heap;
}

void heap_destroy(heap_t *heap)
{
    if (heap == NULL) return;

    for (size_t i = 0; i < heap->len; ++i) {
        free(heap->items[i]);
    }

    free(heap->items);
    free(heap);
}


int heap_insert(heap_t *heap, const void *item)
{
    if (heap == NULL) return 99;

    if (heap->len >= heap->capacity && heap_reallocate(heap)) return 1;

    heap->items[heap->len] = malloc(heap->datasize);
    if (heap->items[heap->len] == 0) return 1;

    memcpy(heap->items[heap->len], item, heap->datasize);
    ++(heap->len);

    size_t current = heap->len - 1;
    size_t parent = heap_index_parent(current);
    while (current != 0 && heap->compare_function(heap->items[parent], heap->items[current]) > 0) {
        heap_swap(heap, current, parent);
        current = parent;
        parent = heap_index_parent(current);
    }

    return 0;
}


void *heap_peek(const heap_t *heap)
{
    if (heap == NULL) return NULL;
    return heap->items[0];
}