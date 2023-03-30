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


/*! @brief Creates new dict_t structure and allocates memory for it.
 *
 * @paragraph Memory deallocation
 * Destroy dict_t structure using dict_destroy function.
 * 
 * @return Pointer to the created dict_t, if successful. NULL if not successful.
 */
dict_t *dict_new(void);


/*! @brief Destroys dict_t structure properly deallocating memory.
 *
 * @param dict        dictionary to destroy
 */
void dict_destroy(dict_t *dict);


/*! @brief Adds key with its associated value into dictionary.
 *
 * @paragraph Possible error return codes
 * This function may return the following error codes:
 * 1, if new memory could not be allocated for new dictionary entry.
 * 2, if previous instance of key in dictionary could not be properly destroyed.
 * 3, if pushing into linked list failed.
 * 4, if new linked list could not be created.
 * 99, if the dictionary does not exist (the dict pointer is NULL).
 * 
 * @param dict          dictionary to add the key-value pair to
 * @param key           key for hashing
 * @param value         value to be stored
 * @param valuesize     size of the value
 * 
 * @return Zero, if the item has been succesfully added. Else non-zero.
 */
int dict_set(dict_t *dict, const char *key, const void *value, const size_t valuesize);


/*! @brief Gets value associated with a key from dictionary.
 *  
 * @param dict          dictionary to search in
 * @param key           key to search for
 * 
 * @return 
 * Void pointer to the value associated with target key. 
 * NULL if the key is not present in the dictionary or the dictionary does not exist.
 */
void *dict_get(const dict_t *dict, const char *key);


/*! @brief Calculates the number of key-value pairs in dictionary.
 *
 * @param dict          concerned dictionary
 * 
 * @return Number of key-value pairs in the dictionary. If dict is NULL, returns 0.
 */
size_t dict_len(const dict_t *dict);


/*! @brief Removes dictionary entry with corresponding key from the dictionary.
 *  
 * @param dict          concerned dictionary
 * @param key           key to search for
 * 
 * @return 
 * 0, if entry successfully removed.
 * 1, if entry could not be removed.
 * 2, if entry with corresponding key does not exist.
 * 99, if dictionary does not exist.
 */
int dict_del(dict_t *dict, const char *key);


/*! @brief Fetches all keys from dictionary. Returns a vector.
 *
 * @paragraph Invalid dictionary
 * If dict is NULL, the returned vector is empty.
 * 
 * @paragraph Memory deallocation
 * The caller is responsible for deallocating memory for the output vector
 * by calling vec_destroy function.
 *
 * @param dict          concerned dictionary
 * 
 * @return 
 * Vector of void pointers pointing to arrays of chars. 
 * NULL if vector could not be created.
 */
vec_t *dict_keys(const dict_t *dict);


/*! @brief Fetches all values from dictionary. Returns a vector.
 *
 * @paragraph Invalid dictionary
 * If dict is NULL, the returned vector is empty.
 * 
 * @paragraph Memory deallocation
 * The caller is responsible for deallocating memory for the output vector
 * by calling vec_destroy function.
 *
 * @param dict          concerned dictionary
 * 
 * @return 
 * Vector of void pointers pointing to values. 
 * NULL if vector could not be created.
 */
vec_t *dict_values(const dict_t *dict);

#endif /* DICTIONARY_H */