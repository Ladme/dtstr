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

/*! @brief Returns index at which target key points in a specific dictionary. */
inline static size_t dict_index(const dict_t *dict, const char *key)
{
    return hash_key(key) % dict->allocated;
}

/*! @brief Creates a new dictionary allocating space for 'capacity' linked lists. */
inline static dict_t *dict_with_capacity(const size_t capacity)
{
    dict_t *dict = calloc(1, sizeof(dict_t));
    if (dict == NULL) {
        return NULL;
    }

    // allocate memory for items
    dict->items = calloc(capacity, sizeof(dllist_t *));
    if (dict->items == NULL) {
        free(dict);
        return NULL;
    }

    dict->allocated = capacity;
    dict->available = capacity / 2;

    return dict;
}

/*! @brief Allocate memory for new dictionary entry. Returns pointer to new entry or NULL if allocation fails. */
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

/*! @brief Frees memory allocated for dictionary entry. */
static void dict_entry_destroy(void *item)
{
    if (item == NULL) return;
    dict_entry_t *entry = *(dict_entry_t **) item;
    free(entry->value);
    free(entry->key);
    free(entry);
}

/*! @brief Returns key of entry in target node. */
inline static char *node_get_key(const dnode_t *node)
{
    return (*(dict_entry_t **) node->data)->key;
}

/*! @brief Returns value of entry in target node. */
inline static void *node_get_value(const dnode_t *node)
{
    return (*(dict_entry_t **) node->data)->value;
}

/*! @brief Gets pointer to node containing entry with the given key. If such node does not exist, returns NULL. */
static dnode_t *dict_get_node(const dllist_t *list, const char *key, const size_t index)
{
    dnode_t *node = list->head;

    while (node != NULL) {
        if (strcmp(node_get_key(node), key) == 0) return node;
        node = node->next;
    }

    return NULL;
}

/*! @brief Gets pointer to dictionary entry with given key. If such entry does not exist, returns NULL. */
static dict_entry_t *dict_get_entry(const dict_t *dict, const char *key)
{
    const size_t index = dict_index(dict, key);
    if (dict->items[index] == NULL) return NULL;

    const dnode_t *node = dict_get_node(dict->items[index], key, index);
    if (node == NULL) return NULL;

    return *(dict_entry_t **) node->data;
}

/*! @brief Properly frees memory for dictionary entry in target node and removes the node from linked list.
 * Returns zero if successful, else returns non-zero. */
static int dict_node_entry_destroy(dllist_t *list, dnode_t *node)
{
    dict_entry_destroy(node->data);
    return dllist_remove_node(list, node);
}

/*! @brief Collects all entries from a dictionary. Returns NULL, if unsuccessful. */
static vec_t *dict_collect_entries(const dict_t *dict)
{
    vec_t *entries = vec_new();

    for (size_t i = 0; i < dict->allocated; ++i) {
        if (dict->items[i] == NULL) continue;

        dnode_t *node = dict->items[i]->head;
        while (node != NULL) {
            
            if (vec_push(entries, node->data, sizeof(dict_entry_t *)) != 0) return NULL;
            node = node->next;
        }
    }

    return entries;
}

/*! @brief Assigns all entries from 'entries' vector to target dictionary. Returns 0, if successful. Else returns non-zero. */
static int dict_assign_entries(dict_t *dict, const vec_t *entries)
{
    for (size_t i = 0; i < entries->len; ++i) {
        
        const dict_entry_t *entry = *(dict_entry_t **) entries->items[i];
        const size_t index = dict_index(dict, entry->key);

        if (dict->items[index] == NULL) {
            dict->items[index] = dllist_new();
            if (dict->items[index] == NULL) return 1;
            --dict->available;
        }

        if (dllist_push_first(dict->items[index], &entry, sizeof(dict_entry_t *)) == 1) return 2;
    }

    return 0;
}

/*! @brief Expands dictionary, allocating more memory for it and reassigning entries. Returns 0, if successful, else returns non-zero. */
static int dict_expand(dict_t *dict)
{
    vec_t *entries = dict_collect_entries(dict);
    if (entries == NULL) return 1;
    
    for (size_t i = 0; i < dict->allocated; ++i) {
        if (dict->items[i] == NULL) continue;
        dllist_destroy(dict->items[i]);
        dict->items[i] = NULL;
    }

    size_t prev_allocated = dict->allocated;
    dict->allocated *= 2;
    dict->available = dict->allocated / 2;
    dict->items = realloc(dict->items, dict->allocated * sizeof(void *));
    memset(dict->items + prev_allocated, 0, (dict->allocated - prev_allocated) * sizeof(void *));
    if (dict->items == NULL) return 3;

    int return_code = dict_assign_entries(dict, entries);
    vec_destroy(entries);
    return return_code;
}

/*! @brief Shrinks dictionary, deallocating unneeded memory and reasssigning entries. Returns 0, if successful, else returns non-zero.*/
static int dict_shrink(dict_t *dict)
{
    vec_t *entries = dict_collect_entries(dict);
    if (entries == NULL) return 1;

    for (size_t i = 0; i < dict->allocated; ++i) {
        if (dict->items[i] == NULL) continue;
        dllist_destroy(dict->items[i]);
        dict->items[i] = NULL;
    }

    dict->allocated /= 2;
    dict->available = dict->allocated / 2;
    dict->items = realloc(dict->items, dict->allocated * sizeof(void *));
    if (dict->items == NULL) return 3;

    int return_code = dict_assign_entries(dict, entries);
    vec_destroy(entries);
    return return_code;
}

/* *************************************************************************** */
/*                  PUBLIC FUNCTIONS ASSOCIATED WITH DICT_T                    */
/* *************************************************************************** */

dict_t *dict_new(void)
{
    return dict_with_capacity(DICT_BLOCK);
}


void dict_destroy(dict_t *dict)
{
    if (dict == NULL) return;

    for (size_t i = 0; i < dict->allocated; ++i) {

        if (dict->items[i] == NULL) continue;

        dllist_map(dict->items[i], dict_entry_destroy);
        dllist_destroy(dict->items[i]);
    }

    free(dict->items);
    free(dict);
}


void *dict_get(const dict_t *dict, const char *key)
{
    if (dict == NULL) return NULL;

    const size_t index = dict_index(dict, key);;
    const dict_entry_t *entry = dict_get_entry(dict, key);

    if (entry == NULL) return NULL;

    return entry->value;
}


int dict_set(dict_t *dict, const char *key, const void *value, const size_t valuesize)
{
    if (dict == NULL) return 99;

    size_t index = dict_index(dict, key);

    // expand dict, if capacity is reached
    if (dict->available == 0 && dict->items[index] == NULL) {
        if (dict_expand(dict) != 0) return 5;
        index = dict_index(dict, key);
    }

    // check whether target position of array already contains a linked list
    // if not, create it
    if (dict->items[index] == NULL) {
        dict->items[index] = dllist_new();
        if (dict->items[index] == NULL) return 4;
        --dict->available;
    } else {
        // check if this key already exists; if it does, overwrite the previous instance of this key
        dnode_t *check_node = dict_get_node(dict->items[index], key, index); 
        if (check_node != NULL) {
            dict_entry_t *check_entry = *(dict_entry_t **) check_node->data;
            free(check_entry->value);
            check_entry->value = malloc(valuesize);
            if (check_entry->value == NULL) return 2;
            memcpy(check_entry->value, value, valuesize);
            return 0;
        }
    }

    // create new entry
    const dict_entry_t *new_entry = dict_entry_new(key, value, valuesize);
    if (new_entry == NULL) return 1;

    // add entry to linked list
    if (dllist_push_first(dict->items[index], &new_entry, sizeof(dict_entry_t *)) == 1) return 3;
    
    return 0;
}


size_t dict_len(const dict_t *dict)
{
    if (dict == NULL) return 0;

    size_t count = 0;
    for (size_t i = 0; i < dict->allocated; ++i) {
        if (dict->items[i] == NULL) continue;

        count += dict->items[i]->len;
    }

    return count;
}

int dict_del(dict_t *dict, const char *key)
{
    if (dict == NULL) return 99;

    size_t index = dict_index(dict, key);
    if (dict->items[index] == NULL) return 2;
    dnode_t *node = dict_get_node(dict->items[index], key, index);
    if (node == NULL) return 2;

    if (dict_node_entry_destroy(dict->items[index], node) != 0) return 1;

    // check if the linked list is empty; if it is, remove it
    if (dict->items[index]->len == 0) {
        dllist_destroy(dict->items[index]);
        dict->items[index] = NULL;
        ++dict->available;
    }
    
    // shrink dictionary
    if (dict->allocated > DICT_BLOCK &&  3 * dict->allocated <= 8 * dict->available && dict_shrink(dict) != 0) return 3;

    return 0;
}

vec_t *dict_keys(const dict_t *dict)
{
    vec_t *keys = vec_new();
    if (keys == NULL) return NULL;

    if (dict == NULL) return keys;

    for (size_t i = 0; i < dict->allocated; ++i) {
        if (dict->items[i] == NULL) continue;

        dnode_t *node = dict->items[i]->head;
        while (node != NULL) {
            char *key = node_get_key(node);
            if (vec_push(keys, key, strlen(key) + 1) != 0) return NULL; 
            node = node->next;
        }
    }

    return keys;
}

vec_t *dict_values(const dict_t *dict)
{
    vec_t *values = vec_new();
    if (values == NULL) return NULL;

    if (dict == NULL) return values;

    for (size_t i = 0; i < dict->allocated; ++i) {
        if (dict->items[i] == NULL) continue;

        dnode_t *node = dict->items[i]->head;
        while (node != NULL) {
            if (vec_push(values, node_get_value(node), sizeof(void *)) != 0) return NULL; 
            node = node->next;
        }
    }

    return values;
}

void dict_map(dict_t *dict, void (*function)(void *))
{
    if (dict == NULL) return;

    for (size_t i = 0; i < dict->allocated; ++i) {
        if (dict->items[i] == NULL) continue;

        dnode_t *node = dict->items[i]->head;
        while (node != NULL) {

            if (node->data != NULL) function((*(dict_entry_t **) node->data)->value);            
            node = node->next;
        }
    }
}

void dict_map_entries(dict_t *dict, void (*function)(void *))
{
    if (dict == NULL) return;

    for (size_t i = 0; i < dict->allocated; ++i) {
        dllist_map(dict->items[i], function);
    }
}