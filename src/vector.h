// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vector {
    size_t len;
    size_t capacity;
    size_t base_capacity;
    void **items;
} vec_t;

#define VEC_DEFAULT_CAPACITY 16UL

/**
 * @brief Creates a new `vec_t` structure and allocates memory for it.
 *
 * @note - To release the memory allocated for `vec_t`, use the `vec_destroy` function.
 * @note - Allocates space for `VEC_DEFAULT_CAPACITY` items. 
 *         This space is dynamically expanded when needed but expanding the vector is a costly operation.
 *         You may want to preallocate memory for a specific number of items using `vec_with_capacity` function.
 * 
 * @return A pointer to the newly created `vec_t` structure if successful; otherwise, NULL.
 */
vec_t *vec_new(void);


/**
 * @brief Creates a new `vec_t` structure and preallocates space for a specified number of items.
 *
 * @param base_capacity     The initial capacity of the vector
 * 
 * @note - To release the memory allocated for `vec_t`, use the `vec_destroy` function.
 * @note - The vector will never shrink below the specified `base_capacity`.
 * @note - If you want the vector to be able to shrink below the specified `base_capacity`, use `vec_fit`.
 * 
 * @return A pointer to the newly created `vec_t` structure if successful; otherwise, NULL.
 */
vec_t *vec_with_capacity(const size_t base_capacity);


/** 
 * @brief Creates a new vector that fits `n_items` items while also having the default `base capacity`. 
 * 
 * @param items     The number of items that should fit into vector without preallocating.
 * 
 * @note - To release the memory allocated for `vec_t`, use the `vec_destroy` function.
 * @note - If all the items are removed from vector created by this function, 
 *         the vector's capacity will decrease to `VEC_DEFAULT_CAPACITY`.
 * 
 * @return A pointer to the newly created vector, or NULL if memory allocation fails. 
 */
vec_t *vec_fit(const size_t n_items);


/**
 * @brief Creates a new vector and populates it with items from an array.
 *
 * @param array      The array containing the items to be copied into the vector.
 * @param n_items    The number of items in the array.
 * @param itemsize   The size of each item in bytes.
 *
 * @note - To release the memory allocated for `vec_t`, use the `vec_destroy` function.
 * @note - The `base_capacity` of the vector is `VEC_DEFAULT_CAPACITY` items.
 * 
 * @return A pointer to the newly created vector, or NULL if memory allocation fails.
 */
vec_t *vec_from_arr(const void *array, const size_t n_items, const size_t itemsize);


/**
 * @brief Creates a new vector and fills it with copies of a given value.
 *
 * @param value    The value to be copied and stored in each element of the vector.
 * @param n_items  The number of items to be inserted into the vector.
 * @param itemsize The size of each item in bytes.
 * 
 * @note - To release the memory allocated for `vec_t`, use the `vec_destroy` function.
 * @note - The `base_capacity` of the vector is `VEC_DEFAULT_CAPACITY` items.
 * 
 * @return A pointer to the newly created and filled vector, or NULL if memory allocation fails.
 */
vec_t *vec_fill(const void *value, const size_t n_items, const size_t itemsize);


/**
 * @brief Properly deallocates memory for the given `vector` and destroys the `vec_t` structure.
 *
 * @param vector    The `vec_t` structure to destroy.
 */
void vec_destroy(vec_t *vector);


/**
 * @brief Returns a pointer to the item stored at the specified index of the vector.
 * 
 * @param vector    The vector to operate on
 * @param index     The index of the item to retrieve
 * 
 * @note - The returned pointer is no longer valid once the parent vector is destroyed.
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return Pointer to the item stored at the specified index. NULL if index is out-of-bounds or if the vector does not exist.
 */
void *vec_get(const vec_t *vector, const size_t index);


/** 
 * @brief Adds a new item to the end of the vector.
 * 
 * @param vector    The vector to which to add the item.
 * @param item      The item to add.
 * @param itemsize  The size of the item to add.
 * 
 * @note - Data provided using the `item` pointer is copied to a new memory location. You can therefore safely deallocate the original item.
 *
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return 0 if successful, 1 if memory for the items field could not be reallocated, and 99 if the vector does not exist.
 */
int vec_push(vec_t *vector, const void *item, const size_t itemsize);


/**
 * @brief Adds a new item at target index of the vector.
 * 
 * @param vector    Pointer to the vector to use.
 * @param item      Pointer to the item to add.
 * @param itemsize  The size of the item to add.
 * @param index     The position to which the new item should be added.
 * 
 * @note - Data provided using the `item` pointer is copied to new memory location. You can therefore freely deallocate the original item.
 * 
 * @note - Asymptotic Complexity: Linear, O(n)
 * 
 * @return 
 * - 0 if successful. 
 * - 1 if memory for the items field could not be reallocated. 
 * - 2 if index is out of bounds.
 * - 99 if the vector does not exist.
 */
int vec_insert(vec_t *vector, const void *item, const size_t itemsize, const size_t index);


/**
 * @brief Set the value of an item at a specific index in a given vector.
 *
 * @param vector    A pointer to the vector to modify.
 * @param item      A pointer to the value to set in the vector.
 * @param itemsize  The size of the item to set in bytes.
 * @param index     The index in the vector at which to set the value.
 *
 * @note - This function replaces the item at specific index in a given vector with the provided item.
 * @note - The memory for the original item is deallocated.
 * 
 * @return 0 if the item was set successfully, 1 if there was an error allocating memory, 
 * 2 if the index is out of range for the vector, or 99 if the vector is NULL.
 */
int vec_set(vec_t *vector, const void *item, const size_t itemsize, const size_t index);


/** 
 * @brief Removes the last item from the vector and returns it.
 * 
 * @param vector    The vector to use.
 * 
 * @note - The caller is responsible for deallocating memory for the popped item.
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return The last item from the vector, if successful. NULL if the vector is NULL or if there are no items.
 */
void *vec_pop(vec_t *vector);


/**
 * @brief Removes the element located at the target index and returns it.
 * 
 * @param vector    The vector to use.
 * @param index     Index of the item to be removed.
 * 
 * @note - The caller is responsible for deallocating memory for the removed item.
 * 
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return The element that was removed from the vector, if successful. NULL if the vector is NULL or if the index is out of bounds.
 */
void *vec_remove(vec_t *vector, const size_t index);


/**
 * @brief Returns a new vector that contains a copy of a part of the provided `vector`.
 * 
 * @param vector        A pointer to the vector to slice and copy elements from.
 * @param start         The index of the first element to be included in the slice.
 * @param end           The index of the first element to be excluded from the slice.
 * @param itemsize      The size of each element in bytes.
 * 
 * @note - Item located at index `start` will be included in the output vector. Item located at index `end` will not be included.
 * @note - To slice a single item, use start = ITEM_INDEX, end = ITEM_INDEX + 1.
 * @note - To slice the entire vector, use start = 0, end = VECTOR_LENGTH.
 * @note - Slice-copying is only allowed for vectors that contain items of the same size.
 * 
 * @return Pointer to the new vector. NULL if the provided `vector` is NULL, if `start` or `end` are out of range, or if memory allocation fails.
 */
vec_t *vec_slicecpy(const vec_t *vector, const size_t start, const size_t end, const size_t itemsize);


/**
 * @brief Transfers part of `vector` to new vector.
 * 
 * @param vector        A pointer to the vector to slice and transfer elements from.
 * @param start         The index of the first element to be included in the slice.
 * @param end           The index of the first element to be excluded from the slice.
 * 
 * @note - Sliced items are removed from the original vector and transfered to the new vector.
 * @note - Item located at index `start` will be included in the output vector. Item located at index `end` will not be included.
 * @note - To slice a single item, use start = ITEM_INDEX, end = ITEM_INDEX + 1.
 * @note - To slice the entire vector, use start = 0, end = VECTOR_LENGTH.
 * 
 * @return Pointer to the new vector. NULL if the provided `vector` is NULL, if `start` or `end` are out of range, or if memory allocation fails.
 */
vec_t *vec_slicerm(vec_t *vector, const size_t start, const size_t end);


/**
 * @brief Transfers `items` items from the end of `vector` to new vector.
 * 
 * @param vector        A pointer to the vector to slice and transfer elements from.
 * @param items         The number of items to slice off the end of the vector.
 * 
 * @note - Sliced items are removed from the original vector and transfered to the new vector.
 * @note - Number of items to slice off can not be larger than the length of the vector.
 * @note - If the number of items to slice off is 0, an empty vector is returned. 
 * 
 * @return Pointer to the new vector. NULL if the provided `vector` is NULL, if the number of items to slice off is too large, or if memory allocation fails.
 */
vec_t *vec_slicepop(vec_t *vector, const size_t items);


/**
 * @brief Copies the target vector.
 * 
 * @param vector        A pointer to the vector to copy.
 * @param itemsize      The size of each element in bytes.
 * 
 * @note - Copying is only defined for vectors that contain items of the same size.
 * 
 * @return Pointer to the new vector. NULL if the provided `vector` is NULL or if memory allocation fails.
 */
vec_t *vec_copy(const vec_t *vector, const size_t itemsize);


/**
 * @brief Extends a destination vector by appending the items of another vector.
 *
 * @param vector_dest   The destination vector to extend.
 * @param vector_ext    The vector whose items will be appended to the destination vector.
 * @param itemsize      The size of an individual item in `vector_ext`.
 *
 * @note - All items of `vector_ext` must be of the same size.
 * 
 * @return Returns 0 if successful, 1 if reallocation fails, or 99 if `vector_dest` is NULL.
 */
int vec_extend(vec_t *vector_dest, const vec_t *vector_ext, const size_t itemsize);


/**
 * @brief Concatenates two vectors into a new vector.
 *
 * @param vector1       The first vector to concatenate.
 * @param vector2       The second vector to concatenate.
 * @param itemsize      The size of the individual items in the vectors.
 *
 * @note - New vector contains all items of the first vector (maintaining their order) followed by all items of the second vector (also maintaining their order).
 * @note - If one of the vectors is NULL, the returned vector is a copy of the non-NULL vector.
 * @note - Concatenation is only allowed for vectors that contain items of the same size.
 * 
 * @return Returns pointer to new vector or NULL if both input vectors are NULL or memory allocation failed.
 */
vec_t *vec_cat(const vec_t *vector1, const vec_t *vector2, const size_t itemsize);


/** 
 * @brief Returns the number of items in vector.
 *
 * @param vector  Concerned vector.
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return Number of items in vector. If vector is NULL, returns 0.
 */
size_t vec_len(const vec_t *vector);


/**
 * @brief Clears all elements of a vector and resets its length to zero.
 * 
 * @param vector    A pointer to the vector to be cleared.
 * 
 * @note - All pointers in the vector are set to NULL but the vector keeps its original capacity.
 * @note - If the vector is NULL, does nothing.
 */
void vec_clear(vec_t *vector);


/** 
 * @brief Removes all items from vector that do not fulfill a condition. Modifies the vector.
 * 
 * @param vector            The vector which should be filtered
 * @param filter_function   Function pointer defining the filtering condition
 * 
 * @note
 * - The `filter_function` parameter is a function pointer that should be defined by the caller. 
 * The function should accept a void pointer to the data at the target index, and return an int value.
 * If the function returns a value greater than zero, the item is kept in the vector. 
 * If it returns zero, the item is removed from the vector.
 * 
 * @note - This operation modifies the input vector.
 * @note - The asymptotic complexity of this operation is quadratic, O(n^2). 
 * This operation is expensive. It might be better to use the vec_filter function.
 * 
 * @return The number of removed items.
 */
size_t vec_filter_mut(vec_t *vector, int (*filter_function)(const void *));


/** 
 * @brief Selects all items from vector that fulfill a condition and copies them into another vector. Original vector is not changed.
 * 
 * @param vector            Input vector for filtering.
 * @param filter_function   Function pointer defining the filtering condition
 * @param itemsize          Size of the items in the vector.
 * 
 * @note 
 * - The `filter_function` parameter is a function pointer that should be defined by the caller. 
 * The function should accept a void pointer to the data at the target index, and return an `int` value.
 * If the function returns a value greater than zero, the item is copied into the output vector.
 * If it returns zero, the item is excluded from the output vector.
 * 
 * @note - If 'vector' is NULL, NULL is returned.
 * @note - This function maintains the order of items from the original vector.
 * @note - The caller is responsible for properly deallocating memory for the returned `vec_t` structure (use `vec_destroy` function).
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return Pointer to `vec_t` structure with items fulfilling the filtering condition. NULL if unsuccessful.
 */
vec_t *vec_filter(const vec_t *vector, int (*filter_function)(const void *), const size_t itemsize);


/** 
 * @brief Checks whether an item exists in a vector.
 * 
 * @param vector            The vector to search in
 * @param equal_function    The function pointer defining how the items should be compared
 * @param target            The pointer to the data that is being searched for in the vector
 * 
 * @note
 * - `equal_function` is a pointer to a function that returns an integer and accepts two void pointers.
 * The first void pointer corresponds to the pointer to the data at a particular index, and the second void 
 * pointer is the pointer to the data that is being searched for in the vector.
 * 
 * The equality function should return a value greater than 0 (true) if the two compared items match each other.
 * The equality function should return 0 (false) if the two compared items do not match each other.
 * 
 * @note - If the input vector is NULL, this function returns 0.
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return One (true) if the item exists, else zero.
 */
int vec_contains(const vec_t *vector, int (*equal_function)(const void *, const void *), const void *target);

/** 
 * @brief Searches for an item in the vector and returns pointer to the item.
 * 
 * @param vector            The vector to search in
 * @param equal_function    The function pointer defining how the items should be compared
 * @param target            The pointer to the data that is being searched for in the vector
 * 
 * @note
 * - This function has a linear time complexity. If you plan to perform multiple searches in the same vector, 
 * it might be better to first sort the vector using `vec_sort_quick` or `vec_sort_quicknaive` and then search it 
 * using `vec_find_bsearch` for a faster search.
 * 
 * @note
 * - `equal_function` is a pointer to a function that returns an integer and accepts two void pointers.
 * The first void pointer corresponds to the pointer to the data at a particular index, and the second void 
 * pointer is the pointer to the data that is being searched for in the vector.
 * 
 * The equality function should return a value greater than 0 (true) if the two compared items match each other.
 * The equality function should return 0 (false) if the two compared items do not match each other.
 * 
 * @note - If the input vector is NULL or corresponding item is not found in the vector, the function returns NULL.
 * @note - The function always returns pointer to the first matching item in the vector (with the lowest index).
 * @note - The returned pointer is no longer valid once the parent vector is destroyed.
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return Void pointer to the first matching item. NULL if unsuccessful.
 */
void *vec_find(const vec_t *vector, int (*equal_function)(const void *, const void *), const void *target);


/** 
 * @brief Searches for an item in the vector and removes it. Returns a pointer to the removed item.
 * 
 * @param vector            The vector to search in
 * @param equal_function    The function pointer defining how the items should be compared
 * @param target            The pointer to the data that is being searched for in the vector
 * 
 * @note
 * - `equal_function` is a pointer to a function that returns an integer and accepts two void pointers.
 * The first void pointer corresponds to the pointer to the data at a particular index, and the second void 
 * pointer is the pointer to the data that is being searched for in the vector.
 * 
 * The equality function should return a value greater than 0 (true) if the two compared items match each other.
 * The equality function should return 0 (false) if the two compared items do not match each other.
 * 
 * @note - If the input vector is NULL or corresponding item is not found in the vector, the function returns NULL.
 * @note - The function always removes the first matching item in the vector (with the lowest index).
 * @note - The caller is responsible for properly deallocating memory for the removed item.
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return Void pointer to the removed item. NULL if no such item found or vector is NULL.
 */
void *vec_find_remove(vec_t *vector, int (*equal_function)(const void *, const void *), const void *target);


/** 
 * @brief Searches for an item in vector that is SORTED in ASCENDING order and returns pointer to the item. Uses binary search.
 * 
 * @param vector            Vector to search in
 * @param compare_function  Function pointer defining how the items should be compared
 * @param target            Pointer to data that is searched in the vector
 * 
 * @note
 * - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * 
 * The comparison function should have the following behavior:
 * It should return >0, if the first of the two compared items is larger.
 * It should return 0, if the compared items have the same value.
 * It should return <0, if the first of the two compared items is smaller.
 * 
 * @note - If the input vector is NULL or corresponding item is not found in the vector, the function returns NULL.
 * @note - The function always returns pointer to the first matching item in the vector (with the lowest index).
 * @note - The returned pointer is no longer valid once the parent vector is destroyed.
 * @note - Asymptotic Complexity: Logarithmic, O(log n).
 * 
 * @return Void pointer to the first matching item. NULL if unsuccessful.
 */
void *vec_find_bsearch(const vec_t *vector, int (*compare_function)(const void *, const void *), const void *target);


/**
 * @brief Returns pointer to the minimum item in a vector.
 * 
 * @param vector            The vector to search for the minimum item.
 * @param compare_function  The function to use to compare the items in the vector.
 * 
 * @note
 * - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * 
 * The comparison function should have the following behavior:
 * It should return >0, if the first of the two compared items is larger.
 * It should return 0, if the compared items have the same value.
 * It should return <0, if the first of the two compared items is smaller.
 * 
 * @note - If there are multiple items with the same value that is the minimum value in the vector, 
 * the function will return pointer to the first occurrence of that value in the vector.
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return Pointer to the minimum item in the vector. NULL if this fails.
 */
void *vec_find_min(const vec_t *vector, int (*compare_function)(const void *, const void *));


/**
 * @brief Returns pointer to the maximum item in a vector.
 * 
 * @param vector            The vector to search for the maximum item.
 * @param compare_function  The function to use to compare the items in the vector.
 * 
 * @note
 * - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * 
 * The comparison function should have the following behavior:
 * It should return >0, if the first of the two compared items is larger.
 * It should return 0, if the compared items have the same value.
 * It should return <0, if the first of the two compared items is smaller.
 * 
 * @note - If there are multiple items with the same value that is the maximum value in the vector, 
 * the function will return pointer to the first occurrence of that value in the vector.
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return Pointer to the maximum item in the vector. NULL if this fails.
 */
void *vec_find_max(const vec_t *vector, int (*compare_function)(const void *, const void *));


/** 
 * @brief Loops through all items in vector and applies 'function' to each item.
 * 
 * @param vector    Vector to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Items are traversed starting from index 0.
 */
void vec_map(vec_t *vector, void (*function)(void *, void *), void *pointer);


/**
 * @brief Shuffles the items of a vector.
 * 
 * @param vector A pointer to the vector structure to be shuffled
 * 
 * @note - The Fisher-Yates shuffle algorithm is used to randomly reorder items in the vector. 
 * @note - The random number generator used by this function must 
 *         be seeded before calling the function, e.g by calling `srand(time(NULL))`;
 */
void vec_shuffle(vec_t *vector);


/**
 * @brief Reverses the order of items in a vector.
 *
 * @param vector A pointer to the vector to be reversed
 */
void vec_reverse(vec_t *vector);


/** 
 * @brief Sorts all items in a vector using selection sort.
 *
 * @param vector             Vector to sort
 * @param compare_function   Function pointer defining how the items should be compared
 *
 * @note
 * - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * 
 * If you want the vector to be sorted in ascending order, the comparison function should have the following behavior:
 * It should return >0, if the first of the two compared items is larger.
 * It should return 0, if the compared items have the same value.
 * It should returns <0, if the first of the two compared items is smaller.
 * 
 * @note - If 'vector' is NULL, 99 is returned.
 * 
 * @return 0 if successfully sorted. Else non-zero.
 */
int vec_sort_selection(vec_t *vector, int (*compare_function)(const void *, const void *));


/** 
 * @brief Sorts all items in a vector using bubble sort.
 *
 * @param vector             Vector to sort
 * @param compare_function   Function pointer defining how the items should be compared
 *
 * @note
 * - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * 
 * If you want the vector to be sorted in ascending order, the comparison function should have the following behavior:
 * It should return >0, if the first of the two compared items is larger.
 * It should return 0, if the compared items have the same value.
 * It should returns <0, if the first of the two compared items is smaller.
 * 
 * @note - If 'vector' is NULL, 99 is returned.
 * 
 * @return 0 if successfully sorted. Else non-zero.
 */
int vec_sort_bubble(vec_t *vector, int (*compare_function)(const void *, const void *));


/** 
 * @brief Sorts all items in a vector using insertion sort.
 *
 * @param vector             Vector to sort
 * @param compare_function   Function pointer defining how the items should be compared
 *
 * @note
 * - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * 
 * If you want the vector to be sorted in ascending order, the comparison function should have the following behavior:
 * It should return >0, if the first of the two compared items is larger.
 * It should return 0, if the compared items have the same value.
 * It should returns <0, if the first of the two compared items is smaller.
 * 
 * @note - If 'vector' is NULL, 99 is returned.
 * 
 * @return 0 if successfully sorted. Else non-zero.
 */
int vec_sort_insertion(vec_t *vector, int (*compare_function)(const void *, const void *));


/** 
 * @brief Sorts all items in a vector using naively implemented quicksort. If you want performance, use vec_sort_quick.
 *
 * @param vector             Vector to sort
 * @param compare_function   Function pointer defining how the items should be compared
 *
 * @note
 * - `compare_function` is a pointer to function that returns integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared.
 * 
 * If you want the vector to be sorted in ascending order, the comparison function should have the following behavior:
 * It should return >0, if the first of the two compared items is larger.
 * It should return 0, if the compared items have the same value.
 * It should returns <0, if the first of the two compared items is smaller.
 * 
 * @note - If 'vector' is NULL, 99 is returned.
 * 
 * @return 0 if successfully sorted. Else non-zero.
 */
int vec_sort_quicknaive(vec_t *vector, int (*compare_function)(const void *, const void *));


/** 
 * @brief Sorts all items in a vector using the standard library quicksort function.
 *
 * @param vector                   Vector to sort.
 * @param qsort_compare_function   Function pointer defining how the items should be compared.
 *  
 * @note - If the 'vector' parameter is NULL, 99 is returned.
 * 
 * @note
 * - The `qsort_compare_function` is a pointer to a function that returns an integer and accepts two void pointers.
 * The void pointers point to two particular pieces of data that are compared. 
 * 
 * @note 
 * - If you want the vector to be sorted in ascending order, the comparison function should have the following behavior:
 * 
 * It should return >0 if the first of the two compared items is larger.
 * It should return 0 if the compared items have the same value.
 * It should return <0 if the first of the two compared items is smaller.
 * 
 * @note 
 * - However, note that `vec_sort_quick` requires a slightly different comparison function than the other sorting
 * functions. See below for more details.
 * 
 * @note
 * - The `vec_t` structure contains an array of void pointers, which point to the items saved in the vector. 
 *   In the `vec_sort_*` functions, these pointers are passed directly to the comparison function for comparison. 
 *   In contrast, this function uses the `qsort` function from the standard library, which can sort an array of any items. 
 *   `qsort` also uses a `compare function` and provides it with pointers to the items in the array. 
 *   As `vec_t` already contains an array of void pointers, when it is used with `qsort`, `qsort` passes `void**` pointers to the `compare function`. 
 *   Therefore, in the `compare function`, to access the data from the void pointer, double dereferencing is required. 
 *   For example, **((int **) pointer) would be used to dereference an integer pointer.
 * 
 * @return 0 if the vector is successfully sorted; otherwise, a non-zero value is returned.
 */
int vec_sort_quick(vec_t *vector, int (*qsort_compare_function)(const void *, const void *));

#endif /* VECTOR_H */