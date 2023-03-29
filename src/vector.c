// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "vector.h"

/* *************************************************************************** */
/*                 PRIVATE FUNCTIONS ASSOCIATED WITH VEC_T                     */
/* *************************************************************************** */

/*! @brief Checks whether vector is sufficiently small to be shrunk. Returns 1, if that is the case. Else returns 0.*/
static inline int vec_check_shrink(vec_t *vector)
{
    return (vector->len <= vector->capacity / 4) && (vector->capacity > VEC_INITIAL_CAPACITY);
}

/*! @brief Shrinks the vector in capacity by half. Returns 0, if successful. Else returns non-zero. */
static int vec_shrink(vec_t *vector)
{
    vector->capacity /= 2;
    void **new_items = realloc(vector->items, vector->capacity * sizeof(void *));
    if (new_items == NULL) return 1;

    vector->items = new_items;

    return 0;
}

/*! @brief Reallocates memory for vector. Returns 0, if successful. Else return non-zero. */
static int vec_reallocate(vec_t **vector)
{
    (*vector)->capacity *= 2;
    void **new_items = realloc((*vector)->items, (*vector)->capacity * sizeof(void *));
    if (new_items == NULL) return 1;

    (*vector)->items = new_items;
    memset((*vector)->items + (*vector)->len, 0, sizeof(void *) * (*vector)->capacity / 2);

    return 0;
}

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH VEC_T                      */
/* *************************************************************************** */

vec_t *vec_new(void) 
{
    vec_t *vector = calloc(1, sizeof(vec_t));
    if (vector == NULL) return NULL;

    vector->items = calloc(VEC_INITIAL_CAPACITY, sizeof(void *));
    if (vector->items == NULL) return NULL;

    vector->capacity = VEC_INITIAL_CAPACITY;

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

void *vec_get(const vec_t *const vector, const size_t index) 
{
    if (vector == NULL) return NULL;

    if (index < 0 || index >= vector->len) return NULL;

    return vector->items[index];
}


int vec_push(vec_t *vector, void *const item, const size_t itemsize) 
{
    if (vector == NULL) return 99;

    if (vector->len >= vector->capacity) if (vec_reallocate(&vector) == 1) return 1;
    
    vector->items[vector->len] = malloc(itemsize);
    memcpy(vector->items[vector->len], item, itemsize);
    vector->len++;

    return 0;
}

int vec_insert(vec_t *vector, void *const item, const size_t itemsize, const size_t index)
{
    if (vector == NULL) return 99;
    if (index < 0 || index > vector->len) return 2;
    if (index == vector->len) return vec_push(vector, item, itemsize);

    if (vector->len >= vector->capacity) if (vec_reallocate(&vector) == 1) return 1;

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


long vec_find(const vec_t *vector, int (*equal_function)(const void *, const void *), const void *target)
{
    if (vector == NULL) return -99;
    
    for (size_t i = 0; i < vector->len; ++i) {

        void *item = vector->items[i];
        if (equal_function(item, target)) return (long) i;
    }

    return -1;
}


long vec_find_bsearch(const vec_t *vector, int (*compare_function)(const void *, const void *), const void *target)
{
    if (vector == NULL) return -99;

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
        else return (long) middle;
    }

    return -1;
}