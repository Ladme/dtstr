// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of hash map.

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dlinked_list.h"
#include "vector.h"

typedef struct dict_entry {
    char *key;
    void *value;
} dict_entry_t;

typedef struct dict {
    size_t allocated;       // the number of positions for entries for which memory has been allocated
    size_t available;       // the number of positions for entries that are available
    size_t base_capacity;   // the number of positions for entries that are initially allocated
    dllist_t **items;
} dict_t;

/** @brief The number of entries that are GUARANTEED to fit into a dictionary created by `dict_new` without reallocating. */
#define DICT_DEFAULT_CAPACITY 16UL

/** 
 * @brief Creates new `dict_t` structure and allocates memory for it.
 *
 * @note - Destroy `dict_t` structure using dict_destroy function.
 * @note - Allocates space for at least `DICT_DEFAULT_CAPACITY` key-value pairs. 
 *         This space is dynamically expanded when needed but expanding the dictionary is a very costly operation.
 *         You may want to preallocate memory for a specific number of key-value pairs using `dict_with_capacity` function.
 * 
 * @return Pointer to the created dict_t, if successful. NULL if not successful.
 */
dict_t *dict_new(void);


/**
 * @brief Creates a new `dict_t` structure and preallocates space for a specified number of entries.
 *
 * @param capacity  The guaranteed number of key-value pairs that the dictionary can store without having to reallocate memory
 * 
 * @note - Note that the capacity does NOT directly correspond to the number of allocated positions for key-value pairs.
 *         Instead it specifies the guaranteed number of key-value pairs that the dictionary can store without having to
 *         reallocate memory.
 * @note - Note that the real number of key-value pairs that the dictionary fits before reallocating can be much higher
 *         depending on the number of hash collisions.
 * @note - Properly setting the expected capacity of a dictionary can bring MASSIVE performance benefits. Do not underestimate it.
 * @note - The dictionary will never shrink below the specified `capacity`.
 * 
 * @return A pointer to the newly allocated dictionary structure, or NULL if memory allocation fails.
 */
dict_t *dict_with_capacity(const size_t capacity);


/**
 * @brief Destroys `dict_t` structure while properly deallocating memory.
 *
 * @param dict  Dictionary to destroy
 */
void dict_destroy(dict_t *dict);


/** 
 * @brief Adds key with its associated value into dictionary.
 *
 * @param dict      Dictionary to add the key-value pair to
 * @param key       Key for hashing
 * @param value     Value to be stored
 * @param valuesize Size of the value
 * 
 * @note
 * - This function may return the following error codes:
 * @note 1, if memory could not be allocated for new dictionary entry.
 * @note 2, if previous instance of key in dictionary could not be overwritten
 * @note 3, if pushing into linked list failed.
 * @note 4, if new linked list could not be created.
 * @note 5, if dictionary could not be expanded.
 * @note 99, if the dictionary does not exist (the dict pointer is NULL).
 * 
 * @return Zero, if the item has been succesfully added. Else non-zero.
 */
int dict_set(dict_t *dict, const char *key, const void *value, const size_t valuesize);


/**
 * @brief Gets value associated with a key from dictionary.
 *  
 * @param dict  Dictionary to search in
 * @param key   Key to search for
 * 
 * @return 
 * Void pointer to the value associated with target key. 
 * NULL if the key is not present in the dictionary or the dictionary does not exist.
 */
void *dict_get(const dict_t *dict, const char *key);


/** 
 * @brief Calculates the number of key-value pairs in dictionary.
 *
 * @param dict  Concerned dictionary
 * 
 * @return Number of key-value pairs in the dictionary. If dict is NULL, returns 0.
 */
size_t dict_len(const dict_t *dict);


/** 
 * @brief Removes dictionary entry with corresponding key from the dictionary.
 *  
 * @param dict  Concerned dictionary
 * @param key   Key to search for
 * 
 * @return 
 * 0, if entry successfully removed.
 * 1, if entry could not be removed.
 * 2, if entry with corresponding key does not exist.
 * 3, if dictionary could not be shrunk.
 * 99, if dictionary does not exist.
 */
int dict_del(dict_t *dict, const char *key);


/** 
 * @brief Fetches all keys from dictionary. Returns a vector.
 *
 * @param dict  Concerned dictionary
 * 
 * @note - If dict is NULL, the returned vector is empty.
 * 
 * @note - The caller is responsible for deallocating memory for the output vector by calling `vec_destroy` function.
 * 
 * @return 
 * Vector of void pointers pointing to arrays of chars. 
 * NULL if vector could not be created.
 */
vec_t *dict_keys(const dict_t *dict);


/** 
 * @brief Fetches all values from dictionary. Returns a vector.
 *
 * @param dict  Concerned dictionary
 * 
 * @note - If dict is NULL, the returned vector is empty.
 * 
 * @note - The caller is responsible for deallocating memory for the output vector by calling `vec_destroy` function.
 * 
 * @return 
 * Vector of void pointers pointing to values. 
 * NULL if vector could not be created.
 */
vec_t *dict_values(const dict_t *dict);


/** 
 * @brief Loops through all values in dictionary and applies 'function' to each value.
 * 
 * @param dict      Dictionary to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - The order in which the entries are traversed is not defined.
 */
void dict_map(dict_t *dict, void (*function)(void *, void *), void *pointer);


/** 
 * @brief Loops through all entries in dictionary and applies 'function' to each entry.
 * 
 * @param dict      Dictionary to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - The order in which the entries are traversed is not defined.
 */
void dict_map_entries(dict_t *dict, void (*function)(void *, void *), void *pointer);

#endif /* DICTIONARY_H */