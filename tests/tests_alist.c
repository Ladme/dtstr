// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/alist.h"

static int test_alist_destroy_null(void)
{
    printf("%-40s", "test_alist_destroy (null) ");

    alist_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_alist_new(void) 
{
    printf("%-40s", "test_alist_new ");

    alist_t *list = alist_new();

    assert(list);
    assert(list->len == 0);
    assert(list->capacity == ALIST_DEFAULT_CAPACITY);
    assert(list->base_capacity == ALIST_DEFAULT_CAPACITY);

    alist_destroy(list);

    printf("OK\n");
    return 0;
}


static int test_alist_set(void)
{
    printf("%-40s", "test_alist_set");

    alist_t *list = alist_new();

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    assert(alist_set(NULL, keys[0], &(values[0]), sizeof(size_t)) == 99);
    
    for (size_t i = 0; i < 10; ++i) {
        assert(alist_set(list, keys[i], &(values[i]), sizeof(size_t)) == 0);
    }

    assert(alist_set(list, keys[0], &(values[0]), sizeof(size_t)) == 2);

    for (size_t i = 0; i < 10; ++i) {
        assert(strcmp((*(alist_entry_t **) list->items[i])->key, keys[i]) == 0);
        assert(*((size_t *) (*(alist_entry_t **) list->items[i])->value) == values[i]);
    }

    alist_destroy(list);

    printf("OK\n");
    return 0;
}


static int test_alist_get(void)
{
    printf("%-40s", "test_alist_get");

    assert(alist_get(NULL, "target") == NULL);

    alist_t *list = alist_new();

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};
    
    for (size_t i = 0; i < 10; ++i) {
        assert(alist_set(list, keys[i], &(values[i]), sizeof(size_t)) == 0);
    }

    for (size_t i = 0; i < 10; ++i) {
        assert(*(size_t *) alist_get(list, keys[i]) == values[i]);
    }

    alist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_alist_del(void)
{
    printf("%-40s", "test_alist_del ");

    assert(alist_del(NULL, "test") == 99);

    alist_t *list = alist_new();

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    for (size_t i = 0; i < 10; ++i) {
        assert(alist_set(list, keys[i], &(values[i]), sizeof(size_t)) == 0);
    }

    // attempt to delete non-existent key
    assert(alist_del(list, "nonexistent") == 2);

    // delete all keys
    for (int i = 9; i >= 0; --i) {
        assert(alist_del(list, keys[i]) == 0);
        assert(list->len == i);
        for (int j = 0; j < i; ++j) {
            assert(*(size_t *) alist_get(list, keys[j]) == values[j]);
        }
    }

    alist_destroy(list);
    printf("OK\n");
    return 0;
}


static int test_alist_set_del_large(void)
{
    printf("%-40s", "test_alist_set_del (large) ");

    alist_t *list = alist_new();

    for (size_t i = 0; i < 1000; ++i) {

        char key[20] = "";
        sprintf(key, "key%lu", i);

        assert(alist_set(list, key, &i, sizeof(size_t)) == 0);
    }

    assert(list->capacity == 1024);

    for (int i = 999; i >= 0; --i) {

        char key[20] = "";
        sprintf(key, "key%d", i);

        assert(alist_del(list, key) == 0);
    }

    assert(list->capacity == ALIST_DEFAULT_CAPACITY);

    for (size_t i = 0; i < 1000; ++i) {

        char key[20] = "";
        sprintf(key, "key%lu", i);

        assert(alist_set(list, key, &i, sizeof(size_t)) == 0);
    }

    assert(list->capacity == 1024);

    alist_destroy(list);
    printf("OK\n");
    return 0;
}


// TODO: test_alist_with_capacity
// TODO: test_alist_set_del_preallocated


static void multiply_by_two(void *item, void *unused)
{
    UNUSED(unused);
    size_t *ptr = (size_t *) item;
    *ptr *= 2;
}

static int test_alist_map(void)
{
    printf("%-40s", "test_alist_map ");

    alist_map(NULL, multiply_by_two, NULL);

    alist_t *list = alist_new();

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    for (size_t i = 0; i < 10; ++i) {
        alist_set(list, keys[i], &values[i], sizeof(size_t));
    }

    alist_map(list, multiply_by_two, NULL);

    for (size_t i = 0; i < 10; ++i) {
        assert( *(size_t *) alist_get(list, keys[i]) == values[i] * 2);
    }

    alist_destroy(list);

    printf("OK\n");
    return 0;
}

static void multiply_by_two_from_entry(void *item, void *unused)
{
    UNUSED(unused);

    alist_entry_t *entry = *(alist_entry_t **) item;
    size_t *val_ptr = (size_t *) entry->value;
    *val_ptr *= 2;
}

static int test_alist_map_entries(void)
{
    printf("%-40s", "test_alist_map_entries ");

    alist_map_entries(NULL, multiply_by_two_from_entry, NULL);

    alist_t *list = alist_new();

    char *keys[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};
    size_t values[] = {123, 666, 42, 10000, 0,
                       234, 888, 10, 5000,  0};

    for (size_t i = 0; i < 10; ++i) {
        alist_set(list, keys[i], &values[i], sizeof(size_t));
    }

    alist_map_entries(list, multiply_by_two_from_entry, NULL);

    for (size_t i = 0; i < 10; ++i) {
        assert( *(size_t *) alist_get(list, keys[i]) == values[i] * 2);
    }

    alist_destroy(list);

    printf("OK\n");
    return 0;
}


int main(void) 
{
    test_alist_destroy_null();
    test_alist_new();

    test_alist_set();
    test_alist_get();
    test_alist_del();
    test_alist_set_del_large();

    test_alist_map();
    test_alist_map_entries();

    return 0;
}