// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "set.h"

#define UNUSED(x) (void)(x)

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH SET_T                    */
/* *************************************************************************** */

/** @brief Hashing constant for hash_key function */
static const unsigned long FNV_OFFSET = 14695981039346656037UL;
/** @brief Hashing constant for hash_key function */
static const unsigned long FNV_PRIME = 1099511628211UL;


/** @brief Hashing function. `n_bytes` specifies the length of key. */
static size_t hash_key(const void *key, const size_t n_bytes)
{
    unsigned char *key_bytes = (unsigned char *) key;

    size_t hash = FNV_OFFSET;
    for (size_t i = 0; i < n_bytes; ++i) {
        hash ^= (size_t)(unsigned char)(key_bytes[i]);
        hash *= FNV_PRIME;
    }

    return hash;
}

/** @brief Returns index at which target key points in a specific set. */
inline static size_t set_index(const set_t *set, const void *item, const size_t n_bytes)
{
    size_t index = hash_key(set->hashable(item), n_bytes) % set->allocated;
    return index;
}

/** @brief Allocate memory for new set entry. Returns pointer to new entry or NULL if allocation fails. */
static set_entry_t *set_entry_new(const void *item, const size_t itemsize, const size_t hashsize)
{
    set_entry_t *entry = calloc(1, sizeof(set_entry_t));
    if (entry == NULL) return NULL;

    entry->item = malloc(itemsize);
    if (entry->item == NULL) {
        free(entry);
        return NULL;
    }
    memcpy(entry->item, item, itemsize);
    
    entry->itemsize = itemsize;
    entry->hashsize = hashsize;

    return entry;
}

/** @brief Frees memory allocated for set entry. */
static void set_entry_destroy(void *item, void *unused)
{
    UNUSED(unused);

    if (item == NULL) return;
    set_entry_t *entry = *(set_entry_t **) item;
    free(entry->item);
    free(entry);
}

/** @brief Properly frees memory for set entry in target node and removes the node from linked list.
 * Returns zero if successful, else returns non-zero. */
static int set_node_entry_destroy(dllist_t *list, dnode_t *node)
{
    set_entry_destroy(node->data, NULL);
    return dllist_remove_node(list, node);
}

/** @brief Gets pointer to node containing the given item. If such node does not exist, returns NULL. */
static dnode_t *set_get_node(const dllist_t *list, const void *item, int (*equal_function)(const void *, const void *))
{
    dnode_t *node = list->head;

    while (node != NULL) {
        if (equal_function((*(set_entry_t **) node->data)->item, item)) return node;

        node = node->next;
    }

    return NULL;
}

/** @brief Collects all entries from a set. Returns NULL, if unsuccessful. */
static vec_t *set_collect_entries(const set_t *set)
{
    vec_t *entries = vec_with_capacity(set->allocated);

    for (size_t i = 0; i < set->allocated; ++i) {
        if (set->items[i] == NULL) continue;

        dnode_t *node = set->items[i]->head;
        while (node != NULL) {
            
            if (vec_push(entries, node->data, sizeof(set_entry_t *)) != 0) return NULL;
            node = node->next;
        }
    }

    return entries;
}

/** @brief Assigns all entries from 'entries' vector to target set. Returns 0, if successful. Else returns non-zero. */
static int set_assign_entries(set_t *set, const vec_t *entries)
{
    for (size_t i = 0; i < entries->len; ++i) {
        
        const set_entry_t *entry = *(set_entry_t **) entries->items[i];
        const size_t index = set_index(set, entry->item, entry->hashsize);

        if (set->items[index] == NULL) {
            set->items[index] = dllist_new();
            if (set->items[index] == NULL) return 1;
            --set->available;
        }

        if (dllist_push_first(set->items[index], &entry, sizeof(set_entry_t *)) == 1) return 2;
    }

    return 0;
}

/** @brief Expands set, allocating more memory for it and reassigning items. Returns 0, if successful, else returns non-zero. */
static int set_expand(set_t *set)
{
    vec_t *entries = set_collect_entries(set);
    if (entries == NULL) return 1;
    
    for (size_t i = 0; i < set->allocated; ++i) {
        if (set->items[i] == NULL) continue;
        dllist_destroy(set->items[i]);
        set->items[i] = NULL;
    }

    size_t prev_allocated = set->allocated;
    set->allocated *= 2;
    set->available = set->allocated / 2;
    set->items = realloc(set->items, set->allocated * sizeof(void *));
    if (set->items == NULL) return 3;
    memset(set->items + prev_allocated, 0, (set->allocated - prev_allocated) * sizeof(void *));
    

    int return_code = set_assign_entries(set, entries);
    vec_destroy(entries);
    return return_code;
}

/** @brief Shrinks set, deallocating unneeded memory and reasssigning items. Returns 0, if successful, else returns non-zero.*/
static int set_shrink(set_t *set)
{
    vec_t *entries = set_collect_entries(set);
    if (entries == NULL) return 1;

    for (size_t i = 0; i < set->allocated; ++i) {
        if (set->items[i] == NULL) continue;
        dllist_destroy(set->items[i]);
        set->items[i] = NULL;
    }

    set->allocated /= 2;
    set->available = set->allocated / 2;
    set->items = realloc(set->items, set->allocated * sizeof(void *));
    if (set->items == NULL) return 3;

    int return_code = set_assign_entries(set, entries);
    vec_destroy(entries);
    return return_code;
}

/** @brief Copies a set entry. */
static set_entry_t *set_entry_copy(set_entry_t *entry)
{
    set_entry_t *copy = set_entry_new(entry->item, entry->itemsize, entry->hashsize);
    return copy;
}

/** @brief Compares two sets. Returns 1 if all items of set1 are also in set 2. Returns 0 otherwise. */
static int set_contains_set(const set_t *set1, const set_t *set2)
{
    for (size_t i = 0; i < set1->allocated; ++i) {
         if (set1->items[i] == NULL) continue;

        dnode_t *node = set1->items[i]->head;
        while (node != NULL) {

            if (node->data != NULL) {
                set_entry_t *entry = *(set_entry_t **) node->data;
                if (!set_contains(set2, entry->item, entry->hashsize)) return 0;
            }
            node = node->next;
        }
    }

    return 1;
}


/** @brief Same as set_map_entries but guarantees that the set will not be changed by the function. */
void set_map_entries_const(const set_t *set, void (*function)(const void *, void *), void *pointer)
{
    if (set == NULL) return;

    for (size_t i = 0; i < set->allocated; ++i) {
        if (set->items[i] == NULL) continue;

        dnode_t *node = set->items[i]->head;
        while (node != NULL) {

            if (node->data != NULL) function(node->data, pointer);
            node = node->next;
        }
    }
}

/** @brief Function for collecting items using `set_map_entries_const`. */
static void set_collect_map(const void *wrapped_entry, void *wrapped_vec)
{
    vec_t *output = (vec_t *) wrapped_vec;
    set_entry_t *entry = *(set_entry_t **) wrapped_entry;

    vec_push(output, entry->item, entry->itemsize);
}

/** @brief Function for creating unions using `set_map_entries_const`. */
static void set_union_map(const void *wrapped_entry, void *wrapped_set)
{
    set_t *output = (set_t *) wrapped_set;
    set_entry_t *entry = *(set_entry_t **) wrapped_entry;

    set_add(output, entry->item, entry->itemsize, entry->hashsize);
}

/** @brief Function for creating intersections using `set_map_entries_const`. */
static void set_intersection_map(const void *wrapped_entry, void *wrapped_sets)
{
    set_t **sets = (set_t **) wrapped_sets;
    set_t *set2 = sets[0];
    set_t *output = sets[1];
    set_entry_t *entry = *(set_entry_t **) wrapped_entry;

    if (set_contains(set2, entry->item, entry->hashsize)) {
        set_add(output, entry->item, entry->itemsize, entry->hashsize);
    }
}

/** @brief Function for creating differences using `set_map_entries_const`. */
static void set_difference_map(const void *wrapped_entry, void *wrapped_sets)
{
    set_t **sets = (set_t **) wrapped_sets;
    set_t *set2 = sets[0];
    set_t *output = sets[1];
    set_entry_t *entry = *(set_entry_t **) wrapped_entry;

    if (!set_contains(set2, entry->item, entry->hashsize)) {
        set_add(output, entry->item, entry->itemsize, entry->hashsize);
    }
}

/* *************************************************************************** */
/*                  PUBLIC FUNCTIONS ASSOCIATED WITH SET_T                     */
/* *************************************************************************** */

set_t *set_new(int (*equal_function)(const void *, const void *), const void* (*hashable)(const void *))
{
    return set_with_capacity(SET_DEFAULT_CAPACITY, equal_function, hashable);
}

set_t *set_with_capacity(
        const size_t capacity, 
        int (*equal_function)(const void *, const void *), 
        const void* (*hashable)(const void *))
{
    set_t *set = calloc(1, sizeof(set_t));
    if (set == NULL) return NULL;

    // allocate memory for items
    set->items = calloc(capacity * 2, sizeof(dllist_t *));
    if (set->items == NULL) {
        free(set);
        return NULL;
    }

    set->allocated = capacity * 2;
    set->base_capacity = set->allocated;
    set->available = capacity;
    set->len = 0;

    set->equal_function = equal_function;
    set->hashable = hashable;

    return set;
}

void set_destroy(set_t *set)
{
    if (set == NULL) return;

    for (size_t i = 0; i < set->allocated; ++i) {

        if (set->items[i] == NULL) continue;

        dllist_map(set->items[i], set_entry_destroy, NULL);
        dllist_destroy(set->items[i]);
    }

    free(set->items);
    free(set);
}


const void *hash_full(const void *item)
{
    return item;
}


int set_add(set_t *set, const void *item, const size_t itemsize, const size_t hashsize)
{
    if (set == NULL) return 99;

    size_t index = set_index(set, item, hashsize);

    // expand set, if capacity is reached
    if (set->available == 0 && set->items[index] == NULL) {
        if (set_expand(set) != 0) return 5;
        index = set_index(set, item, hashsize);
    }

    // check whether target position of array already contains a linked list
    // if it does not, create it
    if (set->items[index] == NULL) {
        set->items[index] = dllist_new();
        if (set->items[index] == NULL) return 4;
        --set->available;
    // item already exists -> do nothing
    } else if (set_get_node(set->items[index], item, set->equal_function) != NULL) return 0;

    // create new entry
    const set_entry_t *new_entry = set_entry_new(item, itemsize, hashsize);
    if (new_entry == NULL) return 1;

    // add entry to linked list
    if (dllist_push_first(set->items[index], &new_entry, sizeof(set_entry_t *)) == 1) return 3;

    ++set->len;

    return 0;
}

int set_remove(set_t *set, const void *item, const size_t hashsize)
{
    if (set == NULL) return 99;

    size_t index = set_index(set, item, hashsize);
    if (set->items[index] == NULL) return 2;

    dnode_t *node = set_get_node(set->items[index], item, set->equal_function);
    if (node == NULL) return 2;

    if (set_node_entry_destroy(set->items[index], node) != 0) return 1;

    // check if the linked list is empty; if it is, remove it
    if (set->items[index]->len == 0) {
        dllist_destroy(set->items[index]);
        set->items[index] = NULL;
        ++set->available;
    }

    --set->len;

    // shrink dictionary
    if (set->allocated > set->base_capacity &&  3 * set->allocated <= 8 * set->available && set_shrink(set) != 0) return 3;

    return 0;
}


int set_contains(const set_t *set, const void *item, const size_t hashsize)
{
    if (set == NULL) return 0;

    size_t index = set_index(set, item, hashsize);

    if (set->items[index] == NULL) return 0;
    if (set_get_node(set->items[index], item, set->equal_function) == NULL) return 0;
    
    return 1;
}


vec_t *set_collect(const set_t *set)
{
    vec_t *items = vec_new();
    if (items == NULL) return NULL;
    if (set == NULL) return items;

    set_map_entries_const(set, set_collect_map, items);
    
    return items;
}

size_t set_len(const set_t *set)
{
    if (set == NULL) return 0;

    return set->len;
}

set_t *set_copy(const set_t *set)
{
    if (set == NULL) return NULL;

    set_t *copy = set_with_capacity(set->allocated / 2, set->equal_function, set->hashable);
    if (copy == NULL) return NULL;

    for (size_t i = 0; i < set->allocated; ++i) {
        if (set->items[i] == NULL) continue;

        copy->items[i] = dllist_new();

        dnode_t *node = set->items[i]->head;
        while (node != NULL) {
            set_entry_t *copied_entry = set_entry_copy( *(set_entry_t **) node->data);
            if (dllist_push_last(copy->items[i], &copied_entry, sizeof(set_entry_t *)) != 0) return NULL;

            node = node->next;
        }
    }

    copy->available = set->available;
    copy->base_capacity = set->base_capacity;
    copy->len = set->len;

    return copy;
}


int set_equal(const set_t *set1, const set_t *set2)
{
    if (set1 == NULL && set2 == NULL) return 1;
    if (set1 == NULL || set2 == NULL) return 0;

    if (set1->equal_function != set2->equal_function) return 0;
    
    if (set1->len != set2->len) return 0;

    return set_contains_set(set1, set2);
}

int set_subset(const set_t *set1, const set_t *set2)
{
    if (set1 == NULL) return 1;
    if (set2 == NULL) return 0;

    if (set1->equal_function != set2->equal_function) return 0;

    if (set1->len > set2->len) return 0;

    return set_contains_set(set1, set2);
}

set_t *set_union(const set_t *set1, const set_t *set2)
{
    if (set1 == NULL) return set_copy(set2);
    if (set2 == NULL) return set_copy(set1);

    if (set1->equal_function != set2->equal_function) return NULL;
    if (set1->hashable != set2->hashable) return NULL;

    // always copy the larger set
    const set_t *larger = set1;
    const set_t *smaller = set2;
    if (set2->len > set1->len) {
        larger = set2;
        smaller = set1;
    }

    set_t *new = set_copy(larger);
    if (new == NULL) return NULL;

    set_map_entries_const(smaller, set_union_map, new);

    return new;
}

set_t *set_intersection(const set_t *set1, const set_t *set2)
{
    if (set1 == NULL || set2 == NULL) return NULL;

    if (set1->equal_function != set2->equal_function) return NULL;
    if (set1->hashable != set2->hashable) return NULL;

    set_t *intersection = set_new(set1->equal_function, set1->hashable);
    
    // always loop through the smaller set
    const set_t *larger = set1;
    const set_t *smaller = set2;
    if (set2->len > set1->len) {
        larger = set2;
        smaller = set1;
    }

    const set_t **wrapped = calloc(2, sizeof(set_t *));
    wrapped[0] = larger;
    wrapped[1] = intersection;
    set_map_entries_const(smaller, set_intersection_map, wrapped);
    free(wrapped);

    return intersection;
}

set_t *set_difference(const set_t *set1, const set_t *set2)
{
    if (set1 == NULL) return NULL;
    if (set2 == NULL) return set_copy(set1);

    if (set1->equal_function != set2->equal_function) return NULL;
    if (set1->hashable != set2->hashable) return NULL;

    set_t *difference = set_new(set1->equal_function, set1->hashable);

    const set_t **wrapped = calloc(2, sizeof(set_t *));
    wrapped[0] = set2;
    wrapped[1] = difference;
    set_map_entries_const(set1, set_difference_map, wrapped);
    free(wrapped);

    return difference;
}


void set_map(set_t *set, void (*function)(void *, void *), void *pointer)
{
    if (set == NULL) return;

    for (size_t i = 0; i < set->allocated; ++i) {
        if (set->items[i] == NULL) continue;

        dnode_t *node = set->items[i]->head;
        while (node != NULL) {

            if (node->data != NULL) function((*(set_entry_t **) node->data)->item, pointer);
            node = node->next;
        }
    }
}


void set_map_entries(set_t *set, void (*function)(void *, void *), void *pointer)
{
    if (set == NULL) return;

    for (size_t i = 0; i < set->allocated; ++i) {
        dllist_map(set->items[i], function, pointer);
    }
}