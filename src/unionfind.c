// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "unionfind.h"

#define UNUSED(x) (void)(x)

/* *************************************************************************** */
/*             PRIVATE FUNCTIONS ASSOCIATED WITH UNIONFIND_T                   */
/* *************************************************************************** */

/** 
 * @brief Equal function for unionfind compatible with equal function for set. 
 *
 * Each `uf_entry_t` packs its own `equal_function` which is used by unionfind to compare the items.  
 */
static int unionfind_equal_function(const void *item1, const void *item2)
{
    uf_entry_t *entry1 = (uf_entry_t *) item1;
    uf_entry_t *entry2 = (uf_entry_t *) item2;

    return entry1->equal_function(entry1->item, entry2->item);
}

/** 
 * @brief Hashable for unionfind compatible with hashable for set. 
 *
 * Each `uf_entry_t` packs its own `hashable` which is used by unionfind to specify hashable parts of items.  
 */
static const void *unionfind_hashable(const void *item)
{
    uf_entry_t *entry = (uf_entry_t *) item;

    return entry->hashable(entry->item);
}

/**
 * @brief Structure used in `unionfind_map_function` for function mapping.
 * 
 * It packes pointer provided by the user when calling `unionfind_map` and
 * pointer to function that should be applied to the item.
 */
typedef struct {
    void *pointer;  // pointer provided to the function
    void (*function)(void *, void *);   // function to be mapped
} unionfind_map_struct_t;

/**
 * @brief Wrapper around map function for unionfind.
 */
static void unionfind_map_function(void *item, void *wrapped)
{
    uf_entry_t *entry = (uf_entry_t *) item;

    unionfind_map_struct_t *unwrapped = (unionfind_map_struct_t *) wrapped;

    unwrapped->function(entry->item, unwrapped->pointer);
}


/** @brief Frees items in a set. Used with `set_map`. */
static void unionfind_free_item(void *item, void *unused)
{
    UNUSED(unused);

    uf_entry_t *entry = (uf_entry_t *) item;

    free(entry->item);
}


/** @brief Fetches root entry of the provided entry. */
static uf_entry_t *get_root_entry(uf_entry_t *entry)
{
    if (entry->parent != NULL) {
        entry->parent = get_root_entry(entry->parent);
        return entry->parent;
    }

    return entry;
}

/** @brief Finds root entry for an item. Returns NULL if the item does not exist. */
static uf_entry_t *find_root_entry(const unionfind_t *sets, void *item, const size_t hashsize)
{
    uf_entry_t search = { .item = item, .equal_function = sets->equal_function, .hashable = sets->hashable };

    uf_entry_t *entry = (uf_entry_t *) set_get(sets->entries, &search, hashsize);
    if (entry == NULL) return NULL;

    return get_root_entry(entry);
}

/** @brief Function for collecting pointers to sets stored in unionfind. Used with `set_map` function. */
static void collect_sets(void *item, void *wrapped_set)
{
    uf_entry_t *entry = (uf_entry_t *) item;
    set_t *set = (set_t *) wrapped_set;

    uf_entry_t *root = get_root_entry(entry);

    set_add(set, &(root->item), sizeof(void *), sizeof(void *));
}

/** @brief Service structure (wrapped unionfind) for transfering items from set to unionfind. */
typedef struct {
    unionfind_t *unionfind;   // unionfind to transfer the items to
    uf_entry_t *root;         // pointer to the root item of the set
    int return_code;          // 0 if everything is ok, non-zero in case an error occurs at any point during transfer
} wuf_t;

/** @brief Function for transfering items from set to unionfind. Used with `set_map_entries_const` function. */
static void transfer_items(const void *item, void *wrapped)
{
    wuf_t *uf_wrapped = (wuf_t *) wrapped;
    // we immediately end this function if any of its previous iterations led to an error
    if (uf_wrapped->return_code != 0) return;

    set_entry_t *entry = *(set_entry_t **) item;
    unionfind_t *unionfind = uf_wrapped->unionfind;

    // prepare entry for the unionfind
    uf_entry_t uf_entry = { 
                .item = NULL, 
                .equal_function = unionfind->equal_function, 
                .hashable = unionfind->hashable, 
                .parent = uf_wrapped->root,
                .rank = 0 };
    uf_entry.item = malloc(entry->itemsize);
    memcpy(uf_entry.item, entry->item, entry->itemsize);

    // add unionfind entry into the unionfind entries set
    size_t orig_len = unionfind->entries->len;
    uf_wrapped->return_code = set_add(unionfind->entries, &uf_entry, sizeof(uf_entry_t), entry->hashsize);
    size_t new_len = unionfind->entries->len;

    // check whether the item was actually added
    // if it was not, it overlaps with something and...
    // ...we must deallocate memory for this item and skip the rest of the function
    if (orig_len == new_len) {
        free(uf_entry.item);
        return;
    }

    // if this is root, we have to get pointer to it
    if (uf_wrapped->root == NULL) {
        uf_wrapped->root = set_get(unionfind->entries, &uf_entry, entry->hashsize);
        // we also set rank of root to 1
        uf_wrapped->root->rank = 1;
    }
}

/* *************************************************************************** */
/*             PUBLIC FUNCTIONS ASSOCIATED WITH UNIONFIND_T                    */
/* *************************************************************************** */

unionfind_t *unionfind_new(int (*equal_function)(const void *, const void *), const void* (*hashable)(const void *))
{
    return unionfind_with_capacity(UNIONFIND_DEFAULT_CAPACITY, equal_function, hashable);
}


unionfind_t *unionfind_with_capacity(
        const size_t capacity,
        int (*equal_function)(const void *, const void *), 
        const void* (*hashable)(const void *))
{
    unionfind_t *sets = calloc(1, sizeof(unionfind_t));
    if (sets == NULL) return NULL;

    // for the underlying set, we do not use the user-provided `equal_function` and `hashable`
    // instead, we use versions designed for unionfind which unpack unionfind entries
    sets->entries = set_with_capacity(capacity, unionfind_equal_function, unionfind_hashable);
    if (sets->entries == NULL) {
        free(sets);
        return NULL;
    }

    sets->equal_function = equal_function;
    sets->hashable = hashable;

    return sets;
}

void unionfind_destroy(unionfind_t *sets)
{
    if (sets == NULL) return;

    // deallocate memory for items
    set_map(sets->entries, unionfind_free_item, NULL);

    set_destroy(sets->entries);
    free(sets);
}


int unionfind_make_set(unionfind_t *sets, const void *item, const size_t itemsize, const size_t hashsize)
{
    if (sets == NULL) return 99;

    uf_entry_t entry = { .item = NULL, 
                         .equal_function = sets->equal_function, 
                         .hashable = sets->hashable, 
                         .parent = NULL, 
                         .rank = 0};
    entry.item = malloc(itemsize);
    memcpy(entry.item, item, itemsize);

    return set_add(sets->entries, &entry, sizeof(uf_entry_t), hashsize);
}

void *unionfind_find_set(unionfind_t *sets, void *item, const size_t hashsize)
{
    if (sets == NULL) return NULL;

    uf_entry_t *root = find_root_entry(sets, item, hashsize);
    if (root == NULL) return NULL;
    return root->item;
}

int unionfind_share_set(unionfind_t *sets, void *item1, void *item2, const size_t hashsize1, const size_t hashsize2)
{
    if (sets == NULL || item1 == NULL || item2 == NULL) return 0;

    uf_entry_t *root1 = find_root_entry(sets, item1, hashsize1);
    uf_entry_t *root2 = find_root_entry(sets, item2, hashsize2);

    if (root1 == NULL || root2 == NULL) return 0;
    if (root1 == root2) return 1;
    return 0;
}

int unionfind_union(unionfind_t *sets, void *item1, void *item2, const size_t hashsize1, const size_t hashsize2)
{
    if (sets == NULL) return 99;
    if (item1 == NULL || item2 == NULL) return 1;

    uf_entry_t *root1 = find_root_entry(sets, item1, hashsize1);
    uf_entry_t *root2 = find_root_entry(sets, item2, hashsize2);

    if (root1 == NULL || root2 == NULL) return 1;
    if (root1 == root2) return 0;

    if (root1->rank >= root2->rank) {
        root2->parent = root1;
        if (root1->rank == root2->rank) {
            ++(root1->rank);
        }
    } else {
        root1->parent = root2;
    }

    return 0;
}

vec_t *unionfind_collect_sets(unionfind_t *sets)
{
    if (sets == NULL || sets->entries->len == 0) return vec_new();

    set_t *set = set_new(sets->equal_function, sets->hashable);
    set_map(sets->entries, collect_sets, set);

    vec_t *collected = set_collect(set);
    set_destroy(set);

    return collected;
}

int unionfind_add_set(unionfind_t *sets, const set_t *add)
{
    if (sets == NULL || add == NULL) return 99;
    if (sets->equal_function != add->equal_function || sets->hashable != add->hashable) return 6;

    wuf_t unionfind_wrapped = { .unionfind = sets, .root = NULL, .return_code = 0 };
    set_map_entries_const(add, transfer_items, &unionfind_wrapped);

    return unionfind_wrapped.return_code;
}

size_t unionfind_len(const unionfind_t *sets)
{
    if (sets == NULL) return 0;

    return sets->entries->len;
}

// TODO: collects all items that are part of the same set as `item` into a vector
//vec_t *unionfind_collect(unionfind_t *sets, void *item, const size_t hashsize)

// TODO: unionfind_union_all

void unionfind_map(unionfind_t *sets, void (*function)(void *, void *), void *pointer)
{
    unionfind_map_struct_t wrapped = { .pointer = pointer, .function = function };
    set_map(sets->entries, unionfind_map_function, &wrapped);
}