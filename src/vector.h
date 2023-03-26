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
 * @paragraph Note on deallocation
 * Note that this function returns POINTER to the target item.
 * This pointer is no longer valid once the parent vector is destroyed.
 *
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 *  
 * @param vector        vector to use
 * @param index         index of the item to get
 * 
 * @return Item stored at target index. NULL if out-of-bounds or if vector does not exist.
 */
void *vec_get(const vec_t *const vector, const size_t index);


/*! @brief Adds a new item to the end of the vector.
 * 
 * @paragraph Note on item copying
 * Data provided using the 'item' pointer is copied to new memory location.
 * You can therefore freely deallocate the original item.
 *
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param vector        vector to use
 * @param item          item to add
 * @param itemsize      size of the item to add
 * 
 * @return 0 if successful. 1 if memory for the items field could not be reallocated. 99 if the vector does not exist.
 */
int vec_push(vec_t *vector, void *const item, const size_t itemsize);


/*! @brief Adds a new item at target index of the vector.
 * 
 * @paragraph Note on item copying
 * Data provided using the 'item' pointer is copied to new memory location.
 * You can therefore freely deallocate the original item.
 *
 * @paragraph Asymptotic Complexity
 * Linear, O(n)
 * 
 * @param vector        vector to use
 * @param item          item to add
 * @param itemsize      size of the item to add
 * @param index         position to which the new item should be added
 * 
 * @return 
 * 0 if successful. 
 * 1 if memory for the items field could not be reallocated. 
 * 2 if index is out of bounds.
 * 99 if the vector does not exist.
 */
int vec_insert(vec_t *vector, void *const item, const size_t itemsize, const size_t index);


/*! @brief Removes the last item from the vector and returns it.
 * 
 * @paragraph Note on memory deallocation
 * The caller is responsible for deallocating memory for the popped item.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param vector        vector to use
 * 
 * @return The last item from the vector, if successful. NULL if the vector is NULL or if there are no items.
 */
void *vec_pop(vec_t *vector);


/*! @brief Removes the element located at target index.
 * 
 * @paragraph Note on memory deallocation
 * The caller is responsible for deallocating memory for the removed item.
 *
 * @paragraph Asymptotic Complexity
 * Linear, O(n)
 * 
 * @param vector        vector to use
 * @param index         index of item to be removed
 * 
 * @return The element that was removed from the vector, if successful. NULL if the vector is NULL or if the index does not correspond to any item in the vector.
 */
void *vec_remove(vec_t *vector, const size_t index);


/*! @brief Removes all items from vector that do not fulfill a condition. Modifies the vector.
 *
 * @paragraph Filter function
 * 'filter_function' is a pointer to function that returns integer and accepts void pointer to data at target index.
 * The function should return >0 (true), if the item is supposed to STAY in the list.
 * The function should return 0 (false), if the item is supposed to be REMOVED from the list.
 * 
 * @paragraph Invalid vector
 * If 'vector' is NULL, no operation is performed.
 * 
 * @paragraph Asymptotic Complexity
 * Quadratic, O(n^2). This operation is expensive. It might be better to use vec_filter function.
 * 
 * @param vector            vector which should be filtered
 * @param filter_function   function pointer defining filtering condition
 * 
 * @return The number of removed items.
 */
size_t vec_filter_mut(vec_t *vector, int (*filter_function)(void *));


/*! @brief Selects all items from vector that fulfill a condition and copies them into another vector. Original vector is not changed.
 
 * @paragraph Filter function
 * 'filter_function' is a pointer to function that returns integer and accepts void pointer to data at target index.
 * The function should return >0 (true), if the item is supposed to be copied into the output vector.
 * The function should return 0 (false), if the item is NOT supposed to be copied into the output vector.
 * 
 * @paragraph Invalid vector
 * If 'vector' is NULL, NULL is returned.
 * 
 * @paragraph Output order of items
 * This function maintains the order of items from the original vector.
 * 
 * @paragraph Deallocation of the output vector
 * Caller is responsible of properly deallocating memory for the returned vec_t structure (use vec_destroy function).
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n).
 * 
 * @param vector            input vector for filtering
 * @param filter_function   function pointer defining filtering condition
 * @param itemsize          size of the items in vector
 * 
 * @return Pointer to vec_t structure with items fulfilling filtering condition. NULL if unsuccessful.
 */
vec_t *vec_filter(const vec_t *vector, int (*filter_function)(void *), const size_t itemsize);


/*! @brief Searches for item in vector and returns its index.
 
 * @paragraph Comparison function
 * 'comparison_function' is a pointer to function that returns integer and accept two void pointers.
 * One void pointer corresponds to pointer to data at particular index. The other void pointer is pointer to
 * data which is being searched for in the vector.
 * 
 * The comparison function should return >0 (true), if the two compared items match each other.
 * The comparison function should return 0 (false), if the two compared items DO NOT match each other.
 * 
 * @paragraph Invalid vector
 * If 'vector' is NULL, -99 is returned.
 * 
 * @paragraph No item found
 * If no corresponding item has been found, -1 is returned.
 * 
 * @paragraph Multiple identical items in vector
 * The function always returns the index of the first matching item in the vector.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n).
 * 
 * @param vector            input vector for filtering
 * @param compare_function  function pointer defining how the items should be compared
 * @param target            pointer to data that is searched in the vector
 * 
 * @return Index of the first matching item. Negative value, if unsuccessful.
 */
long vec_find(const vec_t *vector, int (*compare_function)(void *, void *), void *target);


#endif /* VECTOR_H */