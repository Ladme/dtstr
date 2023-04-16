// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "vector.h"

/* *************************************************************************** */
/*                 PRIVATE FUNCTIONS ASSOCIATED WITH VEC_T                     */
/* *************************************************************************** */

/** @brief Checks whether vector is sufficiently small to be shrunk. Returns 1, if that is the case. Else returns 0.*/
inline static int vec_check_shrink(vec_t *vector)
{
    return (vector->capacity > vector->base_capacity) && (vector->len <= vector->capacity / 4);
}

/** @brief Reallocates memory for vector based on its current capacity. Returns 0, if successful. Else return non-zero. */
static int vec_reallocate(vec_t *vector, const size_t old_capacity)
{
    void **new_items = realloc(vector->items, vector->capacity * sizeof(void *));
    if (new_items == NULL) {
        vec_destroy(vector);
        return 1;
    }

    vector->items = new_items;
    if (old_capacity < vector->capacity) {
        memset(vector->items + vector->len, 0, sizeof(void *) * (vector->capacity - old_capacity));
    }

    return 0;
}

/** @brief Shrinks the vector in capacity by half. Returns 0, if successful. Else returns non-zero. */
inline static int vec_shrink(vec_t *vector)
{
    const size_t old_capacity = vector->capacity;
    vector->capacity >>= 1;

    return vec_reallocate(vector, old_capacity);
}

/** @brief Expands the capacity of vector. Returns 0, if successful. Else return non-zero. */
inline static int vec_expand(vec_t *vector)
{
    const size_t old_capacity = vector->capacity;
    vector->capacity <<= 1;
    return vec_reallocate(vector, old_capacity);
}

/** @brief Swaps two items in a vector. */
inline static void vec_swap(vec_t *vector, const size_t i, const size_t j)
{
    void *tmp = vector->items[i];
    vector->items[i] = vector->items[j];
    vector->items[j] = tmp;
}

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH VEC_T                      */
/* *************************************************************************** */

vec_t *vec_new(void) 
{
    return vec_with_capacity(VEC_DEFAULT_CAPACITY);
}

vec_t *vec_with_capacity(const size_t base_capacity)
{
    vec_t *vector = calloc(1, sizeof(vec_t));
    if (vector == NULL) return NULL;

    vector->items = calloc(base_capacity, sizeof(void *));
    if (vector->items == NULL) {
        free(vector);
        return NULL;
    }

    vector->capacity = base_capacity;
    vector->base_capacity = base_capacity;

    return vector;
}

void vec_destroy(vec_t *vector)
{
    if (vector == NULL) return;

    for (size_t i = 0; i < vector->len; ++i) {
        free(vector->items[i]);
    }

    free(vector->items);
    free(vector);
}

void *vec_get(const vec_t *vector, const size_t index) 
{
    if (vector == NULL) return NULL;

    if (index < 0 || index >= vector->len) return NULL;

    return vector->items[index];
}


int vec_push(vec_t *vector, const void *item, const size_t itemsize) 
{
    if (vector == NULL) return 99;

    if (vector->len >= vector->capacity && vec_expand(vector)) return 1;
    
    vector->items[vector->len] = malloc(itemsize);
    memcpy(vector->items[vector->len], item, itemsize);
    ++(vector->len);

    return 0;
}

int vec_insert(vec_t *vector, const void *item, const size_t itemsize, const size_t index)
{
    if (vector == NULL) return 99;
    if (index < 0 || index > vector->len) return 2;
    if (index == vector->len) return vec_push(vector, item, itemsize);

    if (vector->len >= vector->capacity) if (vec_expand(vector) != 0) return 1;

    // move all items located at index or further
    memcpy(vector->items + index + 1, vector->items + index, sizeof(void *) * (vector->len - index));
    // insert new item at target index
    vector->items[index] = malloc(itemsize);
    memcpy(vector->items[index], item, itemsize);
    vector->len++;

    return 0;

}

void *vec_pop(vec_t *vector)
{
    if (vector == NULL) return NULL;
    if (vector->len == 0) return NULL;

    void *item = vector->items[vector->len - 1];
    vector->items[vector->len - 1] = NULL;
    vector->len--;

    if (vec_check_shrink(vector)) {
        vec_shrink(vector); // ignore if this fails
    }

    return item;
}


void *vec_remove(vec_t *vector, const size_t index)
{
    if (vector == NULL) return NULL;
    if (index < 0 || index >= vector->len) return NULL;
    if (index == vector->len) return vec_pop(vector);

    void *item = vector->items[index];

    // move all items located after index
    memcpy(vector->items + index, vector->items + index + 1, sizeof(void *) * (vector->len - index));
    vector->items[vector->len - 1] = NULL;
    vector->len--;

    if (vec_check_shrink(vector)) {
        vec_shrink(vector); // ignore if this fails
    }

    return item;
}

size_t vec_len(const vec_t *vector) 
{ 
    return (vector == NULL) ? 0 : vector->len; 
}


size_t vec_filter_mut(vec_t *vector, int (*filter_function)(const void *))
{
    size_t removed = 0;
    if (vector == NULL) return 0;

    for (size_t i = 0; i < vector->len; ++i) {

        if (!filter_function(vector->items[i])) {
            free(vec_remove(vector, i));
            --i;
            ++removed;
        }

    }

    return removed;
}


vec_t *vec_filter(const vec_t *vector, int (*filter_function)(const void *), const size_t itemsize)
{
    if (vector == NULL) return NULL;
    vec_t *filtered = vec_new();

    for (size_t i = 0; i < vector->len; ++i) {

        void *item = vector->items[i];

        if (filter_function(item)) {
            vec_push(filtered, item, itemsize);
        }
    }

    return filtered;
}


void *vec_find(const vec_t *vector, int (*equal_function)(const void *, const void *), const void *target)
{
    if (vector == NULL) return NULL;
    
    for (size_t i = 0; i < vector->len; ++i) {

        void *item = vector->items[i];
        if (equal_function(item, target)) return item;
    }

    return NULL;
}


void *vec_find_bsearch(const vec_t *vector, int (*compare_function)(const void *, const void *), const void *target)
{
    if (vector == NULL) return NULL;

    size_t first = 0;
    size_t last = vector->len - 1;

    while (first <= last && first >= 0 && last < vector->len) {
        size_t middle = (first + last) / 2;

        // if middle item is smaller than target, we know that target is on the right side of the vector
        if (compare_function(target, vector->items[middle]) > 0) first = middle + 1;
        // the target is on the left side of the vector
        else if (compare_function(target, vector->items[middle]) < 0) last = middle - 1;
        // rescan to make sure that this is the first occurence of the searched item
        else if (first != middle) last = middle;
        // the target is in the middle
        else return vector->items[middle];
    }

    return NULL;
}

void *vec_find_min(const vec_t *vector, int (*compare_function)(const void *, const void *))
{
    if (vector == NULL || vector->len == 0) return NULL;

    void *min_item = vector->items[0];
    for (size_t i = 1; i < vector->len; ++i) {

        void *item = vector->items[i];
        if (compare_function(item, min_item) < 0) min_item = item;
    }

    return min_item;
}


void *vec_find_max(const vec_t *vector, int (*compare_function)(const void *, const void *))
{
    if (vector == NULL || vector->len == 0) return NULL;

    void *max_item = vector->items[0];
    for (size_t i = 1; i < vector->len; ++i) {

        void *item = vector->items[i];
        if (compare_function(item, max_item) > 0) max_item = item;
    }

    return max_item;
}

void vec_map(vec_t *vector, void (*function)(void *, void *), void *pointer)
{
    if (vector == NULL) return;
    for (size_t i = 0; i < vector->len; ++i) function(vector->items[i], pointer);
}


void vec_shuffle(vec_t *vector)
{
    if (vector == NULL || vector->len < 2) return;

    for (size_t i = 0; i < vector->len - 1; ++i) {
        size_t j = rand() % (vector->len - i) + i;
        
        if (i != j) vec_swap(vector, i, j);
    }
}

void vec_reverse(vec_t *vector)
{
    if (vector == NULL || vector-> len < 2) return;

    for (size_t i = 0; i < vector->len / 2; ++i) {
        vec_swap(vector, i, vector->len - i - 1);
    }
}