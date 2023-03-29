// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/dictionary.h"

static void dict_print_sizet(const dict_t *dict)
{
    for (size_t i = 0; i < dict->allocated; ++i) {
        dnode_t *head = dict->items[i]->head;
        while (head != NULL) {
            if (head->data != NULL) {
                dict_entry_t *entry = *(dict_entry_t **) head->data;
                printf("%s: %lu\n", entry->key, *(size_t *) entry->value);
            }

            head = head->next;
        }
    }
}

static int test_dict_new(void) 
{
    printf("%-40s", "test_dict_new ");

    dict_t *dict = dict_new();

    assert(dict);
    assert(dict->allocated > 0);
    assert(dict->available > 0);

    dict_destroy(dict);

    printf("OK\n");
    return 0;
}

static int test_dict_set(void)
{
    printf("%-40s", "test_dict_set ");

    dict_t *dict = dict_new();

    // do not change the keys nor DICT_BLOCK
    // sun & this clash (1)
    // linked_list, number3, reasonable clash (9)
    // beta, hashtag clash (39)
    // something, alpha clash (43)
    // array does not clash with anything (38)

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    for (size_t i = 0; i < 10; ++i) {
        assert(dict_set(dict, keys[i], &(values[i]), sizeof(size_t)) == 0);
    }

    //dict_print_sizet(dict);

    // assert length of the individual linked lists
    assert(dict->items[1]->len == 2);
    assert(dict->items[9]->len == 3);
    assert(dict->items[38]->len == 1);
    assert(dict->items[39]->len == 2);
    assert(dict->items[43]->len == 2);
    for (size_t i = 0; i < 64; ++i) {
        if (i != 1 && i != 9 && i != 38 && i != 39 && i != 43) {
            assert(dict->items[i]->len == 0);
        }
    }

    // check specific keys and values
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->data)->key, "reasonable") == 0);
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->next->data)->key, "number3") == 0);
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->next->next->data)->key, "linked_list") == 0);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->data)->value == 234);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->next->data)->value == 42);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->next->next->data)->value == 666);

    assert(dict->available == 22);

    // add item with a key that is already present in the dictionary
    size_t new_value = 99;
    assert(dict_set(dict, keys[2], &new_value, sizeof(size_t)) == 0);

    //dict_print_sizet(dict);

    assert(dict->items[1]->len == 2);
    assert(dict->items[9]->len == 3);
    assert(dict->items[38]->len == 1);
    assert(dict->items[39]->len == 2);
    assert(dict->items[43]->len == 2);
    for (size_t i = 0; i < 64; ++i) {
        if (i != 1 && i != 9 && i != 38 && i != 39 && i != 43) {
            assert(dict->items[i]->len == 0);
        }
    }

    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->data)->key, "number3") == 0);
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->next->data)->key, "reasonable") == 0);
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->next->next->data)->key, "linked_list") == 0);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->data)->value == 99);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->next->data)->value == 234);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->next->next->data)->value == 666);

    assert(dict->available == 22);

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}

int main(void) 
{
    test_dict_new();
    test_dict_set();

    return 0;
}