// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "alist.h"

/* *************************************************************************** */
/*                 PRIVATE FUNCTIONS ASSOCIATED WITH ALIST_T                   */
/* *************************************************************************** */

/** @brief Deallocates memory allocated for a given alist entry. To be used with map function. */
void alist_entry_destroy_map(void *raw_entry, void *unused)
{
    UNUSED(unused);

    alist_entry_t *entry = *(alist_entry_t **) raw_entry;

    free(entry->key);
    free(entry->value);
    free(entry);
}

/** @brief Deallocates memory allocated for a given alist entry. Stand-alone version. */
void alist_entry_destroy(alist_entry_t *entry)
{
    free(entry->key);
    free(entry->value);
    free(entry);
}

/*! @brief Allocate memory for new alist entry. Returns pointer to new entry or NULL if allocation fails. */
static alist_entry_t *alist_entry_new(const char *key, const void *value, const size_t valuesize)
{
    alist_entry_t *entry = calloc(1, sizeof(alist_entry_t));
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


/* *************************************************************************** */
/*                  PUBLIC FUNCTIONS ASSOCIATED WITH ALIST_T                   */
/* *************************************************************************** */

alist_t *alist_new(void)
{
    return vec_with_capacity(ALIST_DEFAULT_CAPACITY);
}

alist_t *alist_with_capacity(const size_t base_capacity)
{
    return vec_with_capacity(base_capacity);
}

void alist_destroy(alist_t *list)
{
    if (list == NULL) return;
    vec_map(list, alist_entry_destroy_map, NULL);
    vec_destroy(list);
}


int alist_set(alist_t *list, const char *key, const void *value, const size_t valuesize)
{
    if (list == NULL) return 99;

    // check whether the key already exists in the list
    for (size_t i = 0; i < list->len; ++i) {
        if (strcmp((*(alist_entry_t **) list->items[i])->key, key) == 0) return 2;
    }

    // create new alist entry
    alist_entry_t *new_entry = alist_entry_new(key, value, valuesize);

    // add entry to the list
    int return_code = vec_push(list, &new_entry, sizeof(alist_entry_t *));

    if (return_code != 0) alist_entry_destroy(new_entry);

    return return_code;
}


void *alist_get(const alist_t *list, const char *key)
{
    if (list == NULL) return NULL;

    for (size_t i = 0; i < list->len; ++i) {
        alist_entry_t *entry = *(alist_entry_t **) list->items[i];
        if (strcmp(entry->key, key) == 0) return entry->value;
    }

    return NULL;
}


int alist_del(alist_t *list, const char *key)
{
    if (list == NULL) return 99;

    for (size_t i = 0; i < list->len; ++i) {
        alist_entry_t *entry = *(alist_entry_t **) list->items[i];
        if (strcmp(entry->key, key) == 0) {
            void *raw = vec_remove(list, i); 
            if (raw == NULL) return 1;
            else {
                alist_entry_destroy(entry);
                free(raw);
                return 0;
            }
        }
    }

    return 2;
}

size_t alist_len(const alist_t *list) 
{ 
    return (list == NULL) ? 0 : list->len; 
}

void alist_map(alist_t *list, void (*function)(void *, void *), void *pointer)
{
    if (list == NULL) return;

    for (size_t i = 0; i < list->len; ++i) {
        alist_entry_t *entry = *(alist_entry_t **) list->items[i];
        function(entry->value, pointer);
    }
}

void alist_map_entries(alist_t *list, void (*function)(void *, void *), void *pointer)
{
    vec_map(list, function, pointer);
}