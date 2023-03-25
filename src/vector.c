// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "vector.h"

/* *************************************************************************** */
/*                 PRIVATE FUNCTIONS ASSOCIATED WITH VEC_T                     */
/* *************************************************************************** */

/*! @brief Checks whether vector is sufficiently small to be shrunk. Returns 1, if that is the case. Else returns 0.*/
static int vec_check_shrink(vec_t *vector)
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

    free(vector->items);
    free(vector);
}

void *vec_get(const vec_t *const vector, const size_t index) 
{
    if (vector == NULL) return NULL;

    if (index < 0 || index >= vector->len) return NULL;

    return vector->items[index];
}


int vec_push(vec_t *vector, void *const item) 
{
    if (vector == NULL) return 99;

    if (vector->len >= vector->capacity) {
        vector->capacity *= 2;
        void **new_items = realloc(vector->items, vector->capacity * sizeof(void *));
        if (new_items == NULL) return 1;

        vector->items = new_items;
        memset(vector->items + vector->len, 0, sizeof(void *) * vector->capacity / 2);
    }

    vector->items[vector->len] = item;
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

    void *item = vector->items[index];

    for (size_t i = index + 1; i < vector->len; ++i) {
        vector->items[i - 1] = vector->items[i];
    }

    vector->items[vector->len - 1] = NULL;
    vector->len--;

    if (vec_check_shrink(vector)) {
        vec_shrink(vector); // ignore if this fails
    }

    return item;
}