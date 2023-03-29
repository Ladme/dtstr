// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of hash map.

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dlinked_list.h"

typedef struct dict_entry {
    char *key;
    void *value;
} dict_entry_t;

typedef struct dict {
    size_t allocated;       // the number of entries for which memory has been allocated
    size_t available;       // the number of positions for entries that are available
    dllist_t **items;
} dict_t;


dict_t *dict_new(void);

void dict_destroy(dict_t *dict);

int dict_set(dict_t *dict, const char *key, const void *value, const size_t valuesize);

#endif /* DICTIONARY_H */