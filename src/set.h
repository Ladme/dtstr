// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of hash-based set.

#ifndef SET_H
#define SET_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dlinked_list.h"
#include "vector.h"

typedef struct set_entry {
    void *item;
    size_t itemsize;
    size_t hashsize;
} set_entry_t;

typedef struct set {
    size_t allocated;                                   // the number of positions for items for which memory has been allocated
    size_t available;                                   // the number of positions for items that are available
    size_t base_capacity;                               // the number of positions for items that are initially allocated
    size_t len;                                         // the number of items in the set
    int (*equal_function)(const void *, const void *);  // function used to compare the items in a set
    const void* (*hashable)(const void *);              // function specifying the part of item to be used for hashing
    dllist_t **items;
} set_t;


/** @brief The number of entries that are GUARANTEED to fit into a set created by `set_new` without reallocating. */
#define SET_DEFAULT_CAPACITY 16UL


/** 
 * @brief Creates new `set_t` structure and allocates memory for it.
 * 
 * @param equal_function    Function pointer defining how to compare items in a set
 * @param hashable          Function specifying the part of the item to be used for hashing
 * 
 * @note
 * - `equal_function` is a pointer to a function that returns an integer and accepts two void pointers.
 * The void pointers correspond to two items in a list.
 * 
 * The equality function should return a value greater than 0 (true) if the two compared items match each other.
 * The equality function should return 0 (false) if the two compared items do not match each other.
 * 
 * @note - Destroy `set_t` structure using set_destroy function.
 * @note - Allocates space for at least `SET_DEFAULT_CAPACITY` items.
 *         This space is dynamically expanded when needed but expanding the set is a very costly operation.
 *         You may want to preallocate memory for a specific number of items using `set_with_capacity` function.
 * 
 * @return Pointer to the created set_t, if successful. NULL if not successful.
 */
set_t *set_new(int (*equal_function)(const void *, const void *), const void* (*hashable)(const void *));


/**
 * @brief Creates a new `set_t` structure and preallocates space for a specified number of items.
 *
 * @param capacity          The guaranteed number of items that the set can store without having to reallocate memory
 * @param equal_function    Function pointer defining how to compare items in a set
 * @param hashable          Function specifying the part of the item to be used for hashing
 * 
 * @note - Note that the capacity does NOT directly correspond to the number of allocated positions for items.
 *         Instead it specifies the guaranteed number of items that the set can store without having to
 *         reallocate memory.
 * @note - Note that the real number of items that the set fits before reallocating can be much higher
 *         depending on the number of hash collisions.
 * @note - Properly setting the expected capacity of a set can bring MASSIVE performance benefits. Do not underestimate it.
 * @note - The set will never shrink below the specified `capacity`.
 * 
 * @return A pointer to the newly allocated set structure, or NULL if memory allocation fails.
 */
set_t *set_with_capacity(
        const size_t capacity, 
        int (*equal_function)(const void *, const void *), 
        const void* (*hashable)(const void *));


/**
 * @brief Destroys `set_t` structure while properly deallocating memory.
 *
 * @param set  Set to destroy
 */
void set_destroy(set_t *set);


/**
 * @brief Returns the provided item. Can be used as `hashable` in `set_new` or `set_with_capacity` if you want to use the entire item for hashing.
 *
 * @param item  Item that is being added to set.
 * 
 * @return The provided item.
 */
const void *hash_full(const void *item);


/** 
 * @brief Adds item into a set.
 *
 * @param set       Set to add the item to.
 * @param item      Item to add.
 * @param itemsize  Size of the item to add (in bytes).
 * @param hashsize  Size of the hashable part of the item (in bytes).
 * 
 * @note - If the item is already present in the set, this function does nothing and returns 0.
 * @note - If you want to use the entire item for hashing, you can use `hash_full` function as hashable.
 * 
 * @note
 * - This function may return the following error codes:
 * @note 1, if memory could not be allocated for new set entry.
 * @note 3, if pushing into linked list failed.
 * @note 4, if new linked list could not be created.
 * @note 5, if set could not be expanded.
 * @note 99, if the dictionary does not exist (the dict pointer is NULL).
 * 
 * @return Zero, if the item has been succesfully added or if the same item already exists. Else non-zero.
 */
int set_add(set_t *set, const void *item, const size_t itemsize, const size_t hashsize);


/** 
 * @brief Removes item from a set.
 *  
 * @param dict  Concerned set
 * @param key   Item to remove
 * 
 * @return 
 * 0, if item successfully removed.
 * 1, if item could not be removed.
 * 2, if item does not exist.
 * 3, if set could not be shrunk.
 * 99, if set does not exist.
 */
int set_remove(set_t *set, const void *item, const size_t hashsize);


/**
 * @brief Checks if an item is present in the set.
 *
 * @param set         Pointer to the set to be checked.
 * @param item        Pointer to the item to be checked.
 * @param hashsize    The size of the hash key in bytes.
 *
 * @return 1 if the item is in the set, 0 otherwise.
 * 
 * @note - If the set is NULL, returns 0.
 */
int set_contains(const set_t *set, const void *item, const size_t hashsize);


/**
 * @brief Collects all items from set into a vector.
 *
 * @param set       Set from which the items should be collected
 *
 * @return Pointer to vector containing the items. NULL if vector could not be created.
 * 
 * @note - If the set is NULL or empty, returns an empty vector.
 * @note - The caller is responsible for deallocating memory for the output vector by calling `vec_destroy` function.
 * @note - The items are copied into the vector. You can freely deallocate the original set 
 *         and the collected items will still be available in the vector.
 * @note - The order in which the items are collected is arbitrary but always the same when applied to the same set that was not changed.
 */
vec_t *set_collect(const set_t *set);


/** 
 * @brief Returns the number of items in a set.
 *
 * @param set  Concerned set
 * 
 * @return Number of items in a set. If set is NULL, returns 0.
 */
size_t set_len(const set_t *set);


/** 
 * @brief Copies a set.
 *
 * @param set  Set to copy
 * 
 * @return Independent copy of the set. NULL if set is NULL or memory allocation fails at any point during the copying.
 * 
 * @note - The order of the items in the copied set is guaranteed to be the same as in the original set.
 * @note - The base capacity of the copied set is the same as the base capacity of the original set.
 */
set_t *set_copy(const set_t *set);


/**
 * @brief Checks if two sets contain the same items.
 *
 * @param set1 Pointer to the first set to compare.
 * @param set2 Pointer to the second set to compare.
 *
 * @return 1 if the sets are equal, 0 otherwise.
 * 
 * @note - Sets that are NULL are equal.
 * @note - Note that both the sets must also use the same equality function to be equal.
 */
int set_equal(const set_t *set1, const set_t *set2);


/**
 * Checks if `set1` is a subset of `set2`.
 *
 * @param set1 Pointer to the first set to compare.
 * @param set2 Pointer to the second set to compare.
 *
 * @return 1 if `set1` is a subset of `set2`, 0 otherwise.
 * 
 * @note - NULL set is a subset of any set (including NULL set).
 * @note - Note that both the sets must use the same equality function to be comparable.
 */
int set_subset(const set_t *set1, const set_t *set2);

/**
 * @brief Returns a new set containing the union of `set1` and `set2`.
 * 
 * @param set1 Pointer to the first set.
 * @param set2 Pointer to the second set.
 *
 * @return A new set containing the union of set1 and set2, or NULL if there was an error.
 *
 * @note - If one of the sets is NULL, returns a copy of the other set.
 * @note - If both sets are NULL, returns NULL.
 * @note - If the sets use different hash functions or equality functions, returns NULL.
 * @note - If memory allocation fails, returns NULL.
 * @note - The base capacity of the returned set is the same as the base capacity of the larger of the provided sets.
 */
set_t *set_union(const set_t *set1, const set_t *set2);


/**
 * @brief Returns a new set containing the intersection of `set1` and `set2`.
 * 
 * @param set1 Pointer to the first set.
 * @param set2 Pointer to the second set.
 *
 * @return A new set containing the intersection of set1 and set2, or NULL if there was an error.
 *
 * @note - If any of the sets is NULL, returns NULL.
 * @note - If the sets use different hash functions or equality functions, returns NULL.
 * @note - If memory allocation fails, returns NULL.
 * @note - The base capacity of the returned set is twice the SET_DEFAULT_CAPACITY.
 */
set_t *set_intersection(const set_t *set1, const set_t *set2);


/**
 * @brief Returns a new set containing the difference between `set1` and `set2` (items that are in `set1` but not in `set2`).
 * 
 * @param set1 Pointer to the first set.
 * @param set2 Pointer to the second set.
 *
 * @return A new set containing the difference between set1 and set2, or NULL if there was an error.
 *
 * @note - If the first set is NULL, returns NULL.
 * @note - If the second set is NULL, returns a copy of the first set.
 * @note - If the sets use different hash functions or equality functions, returns NULL.
 * @note - If memory allocation fails, returns NULL.
 * @note - The base capacity of the returned set is twice the SET_DEFAULT_CAPACITY, unless `set2` is NULL.
 */
set_t *set_difference(const set_t *set1, const set_t *set2);


/** 
 * @brief Loops through all items in a set and applies 'function' to each item.
 * 
 * @param set       Set to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - The order in which the items are traversed is not defined.
 * @note - Modifying the hashable parts of items in the set using this function leads to undefined behavior.
 */
void set_map(set_t *set, void (*function)(void *, void *), void *pointer);


/** 
 * @brief Loops through all entries in a set and applies 'function' to each entry.
 * 
 * @param set       Set to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - The order in which the entires are traversed is not defined.
 * @note - Modifying the hashable parts of items in the set using this function leads to undefined behavior.
 */
void set_map_entries(set_t *set, void (*function)(void *, void *), void *pointer);

#endif /* SET_H */