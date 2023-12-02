// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of Disjoint-set (Union-Find). 
// The underlying data structure is set from "set.h" allowing for adding of items with O(1) time complexity.
// The individual items inside the set are arranged into trees. 
// Path compression is implemented for `unionfind_find_set`.
// Union by rank is implemented for `unionfind_union`.
// Removal of items and breaking of sets is NOT implemented (and never will be).

#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "set.h"
#include "vector.h"


/**
 * @brief Entry in a Union-Find structure.
 * 
 * @note - `item` is void pointer to the actual item.
 * @note - `parent` is pointer to some previous unionfind entry in the set (ideally, root).
 * @note - `rank` used for unions.
 * @note - `equal_function` is used for comparing `item` with other `items`.
 *          Each entry carries its own copy which is then unpacked inside `unionfind_equal_function`.
 * @note - `hashable` specifies part of `item` to be used for hashing.
 *          Each entry carries its own copy which is then unpacked inside `unionfind_hashable`.
 */
typedef struct unionfind_entry {
    void *item;
    struct unionfind_entry *parent;
    size_t rank;
    int (*equal_function)(const void *, const void *);
    const void* (*hashable)(const void *);
} uf_entry_t;

/**
 * @brief Union-Find structure. The underlying data structure is `set_t` allowing for O(1) insertion of items.
 * 
 * @note - `entries` is a set of entries in the Union-Find structure.
 * @note - `equal_function` is used for comparing items and is distributed into unionfind entries.
 * @note - `hashable` is used to specify hashable part of items and is distributed into unionfind entries.
 */
typedef struct unionfind {
    set_t *entries;
    int (*equal_function)(const void *, const void *);
    const void* (*hashable)(const void *);
} unionfind_t;

#define UNIONFIND_DEFAULT_CAPACITY 16UL


/** 
 * @brief Creates new `unionfind_t` structure and allocates memory for it.
 * 
 * @param equal_function    Function pointer defining how to compare items in a the unionfind
 * @param hashable          Function specifying the part of the item to be used for hashing
 * 
 * @note
 * - `equal_function` is a pointer to a function that returns an integer and accepts two void pointers.
 * The void pointers correspond to two items in the unionfind.
 * 
 * The equality function should return a value greater than 0 (true) if the two compared items match each other.
 * The equality function should return 0 (false) if the two compared items do not match each other.
 * 
 * @note - Destroy `unionfind_t` structure using unionfind_destroy function.
 * @note - Allocates space for at least `UNIONFIND_DEFAULT_CAPACITY` items.
 *         This space is dynamically expanded when needed but expanding the Union-Find is a very costly operation.
 *         You may want to preallocate memory for a specific number of items using `unionfind_with_capacity` function.
 * 
 * @return Pointer to the created `unionfind_t`, if successful. NULL if not successful.
 */
unionfind_t *unionfind_new(int (*equal_function)(const void *, const void *), const void* (*hashable)(const void *));


/** 
 * @brief Creates new `unionfind_t` structure and preallocates space for a specified number of items.
 * 
 * @param capacity          The guaranteed number of items that the unionfind can store without having to reallocate memory
 * @param equal_function    Function pointer defining how to compare items in a the unionfind
 * @param hashable          Function specifying the part of the item to be used for hashing
 * 
 * @note - Note that the capacity does NOT directly correspond to the number of allocated positions for items.
 *         Instead it specifies the guaranteed number of items that the unionfind can store without having to
 *         reallocate memory.
 * @note - Note that the real number of items that the Union-Find fits before reallocating can be much higher
 *         depending on the number of hash collisions.
 * @note - The Union-Find will never shrink below the specified `capacity`.
 * @note - Destroy `unionfind_t` structure using `unionfind_destroy` function.
 * 
 * @return Pointer to the created `unionfind_t`, if successful. NULL if not successful.
 */
unionfind_t *unionfind_with_capacity(
        const size_t capacity,
        int (*equal_function)(const void *, const void *), 
        const void* (*hashable)(const void *));

/**
 * @brief Destroys `unionfind_t` structure while properly deallocating memory.
 *
 * @param unionfind  Unionfind to destroy
 */
void unionfind_destroy(unionfind_t *sets);

/**
 * @brief Creates a new set in the Union-Find data structure and adds the specified item to it.
 *
 * @param sets      Pointer to the Union-Find data structure.
 * @param item      Pointer to the item to be added to the set.
 * @param itemsize  Size of the item in bytes.
 * @param hashsize  Size of the hashable part of the item in bytes.
 *
 * @note - If the item is already present in the unionfind, this function does nothing and returns 0.
 * @note - Amortized time complexity: Constant, O(1).
 * 
 * @note
 * - This function may return the following error codes:
 * @note 1, if memory could not be allocated for new Union-Find entry.
 * @note 3, if pushing into linked list failed.
 * @note 4, if new linked list could not be created.
 * @note 5, if Union-Find could not be expanded.
 * @note 99, if the Union-Find does not exist.
 * 
 * @return Returns 0 on success, or a non-zero error code in case of an error.
 */
int unionfind_make_set(unionfind_t *sets, const void *item, const size_t itemsize, const size_t hashsize);


/**
 * @brief Finds the root item of the set that contains the specified item in the Union-Find data structure.
 *
 * @param sets       Pointer to the Union-Find data structure.
 * @param item       Pointer to the item whose root is to be found.
 * @param hashsize   Size of the hashable part of the item in bytes.
 * 
 * @note - Performs path compression.
 * @note - Amortized time complexity: Almost constant, O(~1).
 *
 * @return           Returns a pointer to the root item on success, or NULL if sets is NULL or the item is not found.
 */
void *unionfind_find_set(unionfind_t *sets, void *item, const size_t hashsize);

/**
 * @brief Checks if the two specified items share the same set in the Union-Find data structure.
 *
 * @param sets          Pointer to the Union-Find data structure.
 * @param item1         Pointer to the first item.
 * @param item2         Pointer to the second item.
 * @param hashsize1     Size of the hashable part of the first item in bytes.
 * @param hashsize2     Size of the hashable part of the second item in bytes.
 *
 * @note - Amortized time complexity: Almost constant, O(~1).
 * 
 * @return              Returns 1 if item1 and item2 share the same set, 0 otherwise. Returns 0 if sets, item1, or item2 is NULL.
 */
int unionfind_share_set(unionfind_t *sets, void *item1, void *item2, const size_t hashsize1, const size_t hashsize2);

/**
 * @brief Merges the sets containing the specified items in the Union-Find data structure.
 *
 * @param sets          Pointer to the Union-Find data structure.
 * @param item1         Pointer to the first item.
 * @param item2         Pointer to the second item.
 * @param hashsize1     Size of the hashable part of the first item in bytes.
 * @param hashsize2     Size of the hashable part of the second item in bytes.
 *
 * @note - Performs union by rank.
 * @note - Amortized time complexity: Almost constant, O(~1).
 * 
 * @return              Returns 0 on success. Returns 99 if sets is NULL. Returns 1 if the items do not exist.
 */
int unionfind_union(unionfind_t *sets, void *item1, void *item2, const size_t hashsize1, const size_t hashsize2);


/**
 * @brief Collects and returns all sets from the Union-Find data structure as a vector.
 *
 * @param sets      Pointer to the Union-Find data structure.
 *
 * @note - The caller is responsible for freeing the vector using `vec_destroy` when it is no longer needed.
 * @note - The vector contains pointers to the root items of each set in the Union-Find structure. The order of sets is undefined.
 * 
 * @note - Amortized time complexity: Roughly linear, O(n).
 * 
 * @return Returns a vector containing all sets from the Union-Find data structure. If sets is NULL or empty, returns an empty vector.           
 */
vec_t *unionfind_collect_sets(unionfind_t *sets);


/** 
 * @brief Loops through all items in Union-Find and applies `function` to each item.
 * 
 * @param sets      Union-Find to apply the function to.
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - The order in which the items are traversed is not defined.
 * @note - Modifying the hashable parts of items in the Union-Find using this function leads to undefined behavior.
 * 
 */
void unionfind_map(unionfind_t *sets, void (*function)(void *, void *), void *pointer);

/**
 * @brief Adds items to the Union-Find data structure as a separate set.
 *
 * @param sets      Pointer to the Union-Find data structure.
 * @param add       Pointer to the set containing items to be added.
 *
 * @note - Items that already exist in the Union-Find data structure will not be duplicated. 
 *         The original items from Union-Find are kept in the structure.
 * @note - The items from the `add` are copied into the Union-Find structure. You can therefore freely free it.
 * @note - Note that `equal_function` and `hashable` of Union-Find and the added set must match each other.
 *         Otherwise, no operation is performed and the function returns 6.
 * 
 * @note - Amortized time complexity: Roughly linear, O(n).
 * 
 * @note
 * - This function may return the following error codes:
 * @note 1, if memory could not be allocated for new Union-Find entry.
 * @note 3, if pushing into linked list failed.
 * @note 4, if new linked list could not be created.
 * @note 5, if Union-Find could not be expanded.
 * @note 6, if the `equal_function` or the `hashable` function of Union-Find and set do not match.
 * @note 99, if the Union-Find or the set does not exist
 * 
 * @return Returns 0 on success or non-zero in case of an error.
 */
int unionfind_add_set(unionfind_t *sets, const set_t *add);


/** 
 * @brief Returns the number of items in all Union-Find sets.
 *
 * @param sets    Pointer to the Union-Find data structure.
 * 
 * @note - Asymptotic time complexity: Constant, O(1).
 * 
 * @return Number of items in the Union-Find. If Union-Find is NULL, returns 0.
 */
size_t unionfind_len(const unionfind_t *sets);


#endif /* UNIONFIND_H */