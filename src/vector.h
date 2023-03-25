// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VEC_INITIAL_CAPACITY 16

typedef struct vector {
    size_t len;
    size_t capacity;
    void **items;
} vec_t;


/*! @brief Creates new vec_t structure and allocates memory for it.
 *
 * @paragraph Memory deallocation
 * Destroy vec_t structure using vec_destroy function.
 * 
 * @return Pointer to the created vec_t, if successful. NULL if not successful.
 */
vec_t *vec_new(void);


/*! @brief Destroys the vec_t structure properly deallocating memory.
 *
 * @param vector        vector to destroy
 */
void vec_destroy(vec_t *vector);


/*! @brief Returns the pointer stored at target index of the vector.
 *
 * @param vector        vector to use
 * @param index         index of the item to get
 * 
 * @return Item stored at target index. NULL if out-of-bounds or if vector does not exist.
 */
void *vec_get(const vec_t *const vector, const size_t index);


/*! @brief Adds a new item to the end of the vector.
 * 
 * @param vector        vector to use
 * @param item          item to add
 * 
 * @return 0 if successful. 1 if memory for the items field could not be reallocated. 99 if the vector does not exist.
 */
int vec_push(vec_t *vector, void *const item);


/*! @brief Removes the last item from the vector and returns it.
 * 
 * @param vector        vector to use
 * 
 * @return The last item from the vector, if successful. NULL if the vector is NULL or if there are no items.
 */
void *vec_pop(vec_t *vector);


/*! @brief Removes the element located at target index.
 * 
 * @param vector        vector to use
 * @param index         index of item to be removed
 * 
 * @return The element that was removed from the vector, if successful. NULL if the vector is NULL or if the index does not correspond to any item in the vector.
 */
void *vec_remove(vec_t *vector, const size_t index);

#endif /* VECTOR_H */