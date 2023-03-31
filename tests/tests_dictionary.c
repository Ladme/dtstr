// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/dictionary.h"

static void dict_print_sizet(const dict_t *dict)
{
    for (size_t i = 0; i < dict->allocated; ++i) {
        if (dict->items[i] == NULL) continue;
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

    // working with invalid dictionary
    size_t test_value = 12;
    assert(dict_set(NULL, "test_key", &test_value, sizeof(size_t)) == 99);

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
            assert(dict->items[i] == NULL);
        }
    }

    // check specific keys and values
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->data)->key, "reasonable") == 0);
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->next->data)->key, "number3") == 0);
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->next->next->data)->key, "linked_list") == 0);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->data)->value == 234);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->next->data)->value == 42);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->next->next->data)->value == 666);

    assert(dict->available == 27);

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
            assert(dict->items[i] == NULL);
        }
    }

    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->data)->key, "reasonable") == 0);
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->next->data)->key, "number3") == 0);
    assert(strcmp((*(dict_entry_t **) dict->items[9]->head->next->next->data)->key, "linked_list") == 0);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->data)->value == 234);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->next->data)->value == 99);
    assert(*(size_t *) (*(dict_entry_t **) dict->items[9]->head->next->next->data)->value == 666);

    assert(dict->available == 27);

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}

static int test_dict_get(void)
{
    printf("%-40s", "test_dict_get ");

    dict_t *dict = dict_new();

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};
    
    for (size_t i = 0; i < 10; ++i) {
        assert(dict_set(dict, keys[i], &(values[i]), sizeof(size_t)) == 0);
    }

    assert(*(size_t *) dict_get(dict, keys[9]) == 0);
    assert(*(size_t *) dict_get(dict, keys[8]) == 5000);
    assert(*(size_t *) dict_get(dict, keys[7]) == 10);
    assert(*(size_t *) dict_get(dict, keys[6]) == 888);
    assert(*(size_t *) dict_get(dict, keys[5]) == 234);
    assert(*(size_t *) dict_get(dict, keys[4]) == 0);
    assert(*(size_t *) dict_get(dict, keys[3]) == 10000);
    assert(*(size_t *) dict_get(dict, keys[2]) == 42);
    assert(*(size_t *) dict_get(dict, keys[1]) == 666);
    assert(*(size_t *) dict_get(dict, keys[0]) == 123);

    // reassigning value of key
    size_t val = 555;
    assert(dict_set(dict, "something", &val, sizeof(size_t)) == 0);
    assert(*(size_t *) dict_get(dict, "something") == 555);

    // getting non-existent key
    assert(dict_get(dict, "nonexistent") == NULL);

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}

static int test_dict_set_get_large(void)
{
    printf("%-40s", "test_dict_set_get (large) ");

    dict_t *dict = dict_new();

    for (size_t i = 0; i < 10000; ++i) {

        char key[20] = "";
        sprintf(key, "key%lu", i);

        assert(dict_set(dict, key, &i, sizeof(size_t)) == 0);
    }

    //dict_print_sizet(dict);
    assert(dict->allocated == 16384);

    for (size_t i = 0; i < 10000; ++i) {

        char key[20] = "";
        sprintf(key, "key%lu", i);

        assert(*(size_t *) dict_get(dict, key) == i);
    }

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}

static int test_dict_len(void)
{
    printf("%-40s", "test_dict_len ");

    assert(dict_len(NULL) == 0);

    dict_t *dict = dict_new();

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    
    assert(dict_len(dict) == 0);

    for (size_t i = 0; i < 10; ++i) {
        assert(dict_set(dict, keys[i], &(values[i]), sizeof(size_t)) == 0);
        assert(dict_len(dict) == i + 1);
    }

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}

static int test_dict_del(void)
{
    printf("%-40s", "test_dict_del ");

    assert(dict_del(NULL, "test") == 99);

    dict_t *dict = dict_new();

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    for (size_t i = 0; i < 10; ++i) {
        assert(dict_set(dict, keys[i], &(values[i]), sizeof(size_t)) == 0);
    }

    assert(dict->available == 27);

    // attempt to delete non-existent key
    assert(dict_del(dict, "nonexistent") == 2);

    // delete all keys
    for (int i = 9; i >= 0; --i) {
        assert(dict_del(dict, keys[i]) == 0);
        assert(dict_len(dict) == i);
    }

    assert(dict->available == 32);

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}

static int test_dict_set_del_large(void)
{
    printf("%-40s", "test_dict_set_del (large) ");

    dict_t *dict = dict_new();

    for (size_t i = 0; i < 10000; ++i) {

        char key[20] = "";
        sprintf(key, "key%lu", i);

        assert(dict_set(dict, key, &i, sizeof(size_t)) == 0);
    }

    //dict_print_sizet(dict);
    assert(dict->allocated == 16384);

    for (size_t i = 0; i < 10000; ++i) {

        char key[20] = "";
        sprintf(key, "key%lu", i);

        assert(dict_del(dict, key) == 0);
    }

    //dict_print_sizet(dict);
    assert(dict->allocated == 64);

    for (size_t i = 0; i < 10000; ++i) {

        char key[20] = "";
        sprintf(key, "key%lu", i);

        assert(dict_set(dict, key, &i, sizeof(size_t)) == 0);
    }

    //dict_print_sizet(dict);
    assert(dict->allocated == 16384);

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}

static int test_eq_string(const void *string1, const void *string2)
{
    return !strcmp((char *) string1, (char *) string2);
}

static int test_dict_keys(void)
{
    printf("%-40s", "test_dict_keys ");

    // invalid dict
    vec_t *vec = dict_keys(NULL);
    assert(vec->len == 0);
    vec_destroy(vec);

    dict_t *dict = dict_new();

    // empty dict
    vec = dict_keys(dict);
    assert(vec->len == 0);
    vec_destroy(vec);

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    for (size_t i = 0; i < 10; ++i) {
        assert(dict_set(dict, keys[i], &(values[i]), sizeof(size_t)) == 0);
    }

    // 10 keys
    vec = dict_keys(dict);
    assert(vec->len == 10);

    // check that the original keys are actually present in the vector
    for (size_t i = 0; i < 10; ++i) {
        assert(vec_find(vec, test_eq_string, keys[i]) >= 0);
    }

    vec_destroy(vec);

    // overwriting key
    size_t value = 12;
    assert(dict_set(dict, "array", &value, sizeof(size_t)) == 0);
    vec = dict_keys(dict);
    assert(vec->len == 10);
    vec_destroy(vec);

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}

static int test_eq_sizet(const void *value1, const void *value2)
{
    return (*(size_t *) value1) == (*(size_t *) value2); 
}


static int test_dict_values(void)
{
    printf("%-40s", "test_dict_values ");

    // invalid dict
    vec_t *vec = dict_values(NULL);
    assert(vec->len == 0);
    vec_destroy(vec);

    dict_t *dict = dict_new();

    // empty dict
    vec = dict_values(dict);
    assert(vec->len == 0);
    vec_destroy(vec);

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    for (size_t i = 0; i < 10; ++i) {
        assert(dict_set(dict, keys[i], &(values[i]), sizeof(size_t)) == 0);
    }

    // 10 values
    vec = dict_values(dict);
    assert(vec->len == 10);

    // check that the original values are actually present in the vector
    for (size_t i = 0; i < 10; ++i) {
        assert(vec_find(vec, test_eq_sizet, &values[i]) >= 0);
    }

    vec_destroy(vec);

    // overwriting key
    size_t value = 12;
    assert(dict_set(dict, "array", &value, sizeof(size_t)) == 0);
    vec = dict_values(dict);
    assert(vec->len == 10);
    assert(vec_find(vec, test_eq_sizet, &value) >= 0);
    vec_destroy(vec);

    dict_destroy(dict);
    printf("OK\n");
    return 0;
}


int main(void) 
{
    test_dict_new();
    test_dict_set();

    test_dict_get();

    test_dict_set_get_large();

    test_dict_len();
    test_dict_del();

    test_dict_set_del_large();

    test_dict_keys();
    test_dict_values();

    return 0;
}