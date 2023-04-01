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
    size_t allocated;       // the number of entries for which memory has been allocated
    size_t available;       // the number of positions for entries that are available
    dllist_t **items;
} dict_t;


/** 
 * @brief Creates new `dict_t` structure and allocates memory for it.
 *
 * @note Destroy `dict_t` structure using dict_destroy function.
 * 
 * @return Pointer to the created dict_t, if successful. NULL if not successful.
 */
dict_t *dict_new(void);


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
 */
void dict_map(dict_t *dict, void (*function)(void *));


/** 
 * @brief Loops through all entries in dictionary and applies 'function' to each entry.
 * 
 * @param dict      Dictionary to apply the function to
 * @param function  Function to apply
 */
void dict_map_entries(dict_t *dict, void (*function)(void *));

#endif /* DICTIONARY_H */