// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of vector-based association list.
// Behaves like a dictionary but the key-value pairs are stored in a vector.

#ifndef ALIST_H
#define ALIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vector.h"

#define UNUSED(x) (void)(x)

typedef struct alist_entry {
    char *key;
    void *value;
} alist_entry_t;

typedef vec_t alist_t;
#define ALIST_DEFAULT_CAPACITY 16UL

/** 
 * @brief Creates new `alist_t` structure and allocates memory for it.
 *
 * @note - Destroy `alist_t` structure using `alist_destroy` function.
 * @note - Allocates space for `ALIST_DEFAULT_CAPACITY` items. 
 *         This space is dynamically expanded when needed but expanding the association list is a costly operation.
 *         You may want to preallocate memory for a specific number of items using `alist_with_capacity` function.
 * 
 * @return Pointer to the created alist_t, if successful. NULL if not successful.
 */
alist_t *alist_new(void);


/**
 * @brief Creates a new `alist_t` structure and preallocates space for a specified number of items.
 *
 * @param base_capacity     The initial capacity of the association list
 * 
 * @note - To release the memory allocated for `alist_t`, use the `alist_destroy` function.
 * @note - The list will never shrink below the specified `base_capacity`.
 * 
 * @return A pointer to the newly created `alist_t` structure if successful; otherwise, NULL.
 */
alist_t *alist_with_capacity(const size_t base_capacity);


/**
 * @brief Properly deallocates memory for the given `list` and destroys the `alist_t` structure.
 *
 * @param list    The `alist_t` structure to destroy.
 */
void alist_destroy(alist_t *list);


/** 
 * @brief Adds key with its associated value into association list.
 *
 * @param list      Association list to add the key-value pair to
 * @param key       Key to be stored
 * @param value     Value to be stored
 * @param valuesize Size of the value
 * 
 * @note - Asymptotic Complexity: Linear, O(n)
 * 
 * @return 0, if the key-value pair has been succesfully added. 1 if memory allocation failed. 2 if the key already exists. 99 if list is NULL.
 */
int alist_set(alist_t *list, const char *key, const void *value, const size_t valuesize);


/**
 * @brief Gets value associated with a key from association list.
 *  
 * @param list  Association list to search in
 * @param key   Key to search for
 * 
 * @note - The returned pointer is no longer valid once the parent association list is destroyed.
 * @note - Asymptotic Complexity: Linear, O(n)
 * 
 * @return 
 * Void pointer to the value associated with target key. 
 * NULL if the key is not present in the list or the list does not exist.
 */
void *alist_get(const alist_t *list, const char *key);


/** 
 * @brief Removes alist entry with corresponding key from the association list.
 *  
 * @param list  Concerned association list
 * @param key   Key to search for
 * 
 * @return 
 * 0, if entry successfully removed.
 * 1, if entry could not be removed.
 * 2, if entry with corresponding key does not exist.
 * 99, if association list does not exist.
 */
int alist_del(alist_t *list, const char *key);


/** 
 * @brief Loops through all values in association list and applies 'function' to each value.
 * 
 * @param alist     Association list to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Values are traversed starting from entry that was added first.
 */
void alist_map(alist_t *list, void (*function)(void *, void *), void *pointer);


/** 
 * @brief Loops through all entries in association list and applies 'function' to each entry.
 * 
 * @param alist     Association list to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Entries are traversed starting from entry that was added first.
 */
void alist_map_entries(alist_t *list, void (*function)(void *, void *), void *pointer);

#endif /* ALIST_H */