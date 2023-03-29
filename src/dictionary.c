// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "dictionary.h"

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH DICT_T                   */
/* *************************************************************************** */

/*! @brief Hashing constant for hash_key function */
static const unsigned long FNV_OFFSET = 14695981039346656037UL;
/*! @brief Hashing constant for hash_key function */
static const unsigned long FNV_PRIME = 1099511628211UL;

/*! @brief The number of linked lists for which memory is initially allocated. */
static const size_t DICT_BLOCK = 64;


/*! @brief Hashing function for string. */
static size_t hash_key(const char *key)
{
    size_t hash = FNV_OFFSET;
    for (size_t i = 0; key[i]; ++i) {
        hash ^= (size_t)(unsigned char)(key[i]);
        hash *= FNV_PRIME;
    }

    return hash;
}

static dict_entry_t *dict_entry_new(const char *key, const void *value, const size_t valuesize)
{
    dict_entry_t *entry = calloc(1, sizeof(dict_entry_t));
    if (entry == NULL) return NULL;

    entry->key = malloc(strlen(key) + 1);
    if (entry->key == NULL) {
        free(entry);
        return NULL;
    }
    memcpy(entry->key, key, strlen(key) + 1);

    entry->value = malloc(valuesize);
    if (entry->value == NULL) {
        free(entry->key);
        free(entry);
        return NULL;
    }
    memcpy(entry->value, value, valuesize);

    return entry;
}

static void dict_entry_destroy(dict_entry_t *entry)
{
    free(entry->value);
    free(entry->key);
    free(entry);
}

/* *************************************************************************** */
/*                  PUBLIC FUNCTIONS ASSOCIATED WITH DICT_T                    */
/* *************************************************************************** */


dict_t *dict_new(void)
{
    dict_t *dict = calloc(1, sizeof(dict_t));
    if (dict == NULL) {
        return NULL;
    }

    // allocate memory for items
    dict->items = calloc(DICT_BLOCK, sizeof(dllist_t *));
    if (dict->items == NULL) {
        free(dict);
        return NULL;
    }

    dict->allocated = DICT_BLOCK;
    dict->available = DICT_BLOCK / 2;

    for (size_t i = 0; i < dict->allocated; ++i) {
        dict->items[i] = dllist_new();
    }

    return dict;
}


void dict_destroy(dict_t *dict)
{
    if (dict == NULL) return;

    for (size_t i = 0; i < dict->allocated; ++i) {

        dnode_t *head = dict->items[i]->head;
        while (head != NULL) {
            if (head->data != NULL) dict_entry_destroy(*(dict_entry_t **) head->data);
            head = head->next;
        }

        dllist_destroy(dict->items[i]);
    }

    free(dict->items);
    free(dict);
}


// TODO: reallocation
int dict_set(dict_t *dict, const char *key, const void *value, const size_t valuesize)
{
    if (dict == NULL) return 99;

    size_t index = hash_key(key) % dict->allocated;

    dict_entry_t *new_entry = dict_entry_new(key, value, valuesize);
    if (new_entry == NULL) return 1;

    // check if this key already exists; if it does, remove the previous instance of this key
    dnode_t *node = dict->items[index]->head;
    while (node != NULL) {
        if (node->data != NULL) {
            dict_entry_t *check = *(dict_entry_t **) node->data;
            if (strcmp(check->key, key) == 0) {
                // remove entry from the linked list
                dnode_t *temporary = node->next;
                dict_entry_destroy(check);
                if (dllist_remove_node(dict->items[index], node) != 0) return 2;
                ++dict->available;
                node = temporary;
                continue;
            }
        }

        node = node->next;
    }

    if (dllist_push_first(dict->items[index], &new_entry, sizeof(dict_entry_t *)) == 1) return 3;
    
    --dict->available;
    return 0;
}