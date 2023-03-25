// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/dlinked_list.h"

inline static void print_sizet_list(const dllist_t *list, const size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        printf("%ld ", *((size_t *) dllist_get(list, i)));
    }
    printf("\n");
}

static int test_dllist_destroy_null(void)
{
    printf("%-40s", "test_dllist_destroy (null) ");

    dllist_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_dllist_new(void)
{
    printf("%-40s", "test_dllist_new ");

    dllist_t *list = dllist_new();

    assert(list);
    assert(!list->head);
    assert(!list->tail);

    dllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_dllist_push_first(void)
{
    printf("%-40s", "test_dllist_push_first ");

    dllist_t *list = dllist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        assert(dllist_push_first(list, (void *) &data[i], sizeof(size_t)) == 0);
        if (i == 0) assert(list->head == list->tail);
    }

    assert(*((size_t *) list->head->data) == data[4] );
    assert(*((size_t *) list->head->next->data) == data[3] );
    assert(*((size_t *) list->head->next->next->data) == data[2] );
    assert(*((size_t *) list->head->next->next->next->data) == data[1] );
    assert(*((size_t *) list->head->next->next->next->next->data) == data[0] );
    assert(list->tail);
    assert(list->head);
    assert(list->len == 5);

    dllist_destroy(list);

    assert(dllist_push_first(NULL, (void *) &data[0], sizeof(size_t)) == 99);

    printf("OK\n");
    return 0;
}

static int test_dllist_push_last(void)
{
    printf("%-40s", "test_dllist_push_last ");

    dllist_t *list = dllist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        assert(dllist_push_last(list, (void *) &data[i], sizeof(size_t)) == 0);
        if (i == 0) assert(list->head == list->tail);
    }

    assert(*((size_t *) list->head->data) == data[0] );
    assert(*((size_t *) list->head->next->data) == data[1] );
    assert(*((size_t *) list->head->next->next->data) == data[2] );
    assert(*((size_t *) list->head->next->next->next->data) == data[3] );
    assert(*((size_t *) list->head->next->next->next->next->data) == data[4] );
    assert(list->tail);
    assert(list->head);
    assert(list->len == 5);

    dllist_destroy(list);

    assert(dllist_push_last(NULL, (void *) &data[0], sizeof(size_t)) == 99);

    printf("OK\n");
    return 0;
}

static int test_dllist_push_first_last(void)
{
    printf("%-40s", "test_dllist_push_first/last (mixed) ");

    dllist_t *list = dllist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        if (i % 2) assert(dllist_push_first(list, (void *) &data[i], sizeof(size_t)) == 0);
        else assert(dllist_push_last(list, (void *) &data[i], sizeof(size_t)) == 0);

        if (i == 0) assert(list->head == list->tail);
    }

    assert(*((size_t *) list->head->data) == data[3] );
    assert(*((size_t *) list->head->next->data) == data[1] );
    assert(*((size_t *) list->head->next->next->data) == data[0] );
    assert(*((size_t *) list->head->next->next->next->data) == data[2] );
    assert(*((size_t *) list->head->next->next->next->next->data) == data[4] );
    assert(list->tail);
    assert(list->head);
    assert(list->len == 5);

    dllist_destroy(list);
    printf("OK\n");
    return 0;
}

static int test_dllist_get(void)
{
    printf("%-40s", "test_dllist_get ");

    dllist_t *list = dllist_new();

    size_t data[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    for (size_t i = 0; i < 7; ++i) {
        assert(dllist_push_last(list, (void *) &data[i], sizeof(size_t)) == 0);
    }

    assert(*((size_t *) dllist_get(list, 0)) == data[0] );
    assert(*((size_t *) dllist_get(list, 1)) == data[1] );
    assert(*((size_t *) dllist_get(list, 2)) == data[2] );
    assert(*((size_t *) dllist_get(list, 3)) == data[3] );
    assert(*((size_t *) dllist_get(list, 4)) == data[4] );
    assert(*((size_t *) dllist_get(list, 5)) == data[5] );
    assert(*((size_t *) dllist_get(list, 6)) == data[6] );
    assert(!dllist_get(list, -1));
    assert(!dllist_get(list, 7));
    assert(!dllist_get(list, 9643));

    assert(dllist_push_last(list, (void *) &data[7], sizeof(size_t)) == 0);
    assert(*((size_t *) dllist_get(list, 7)) == data[7] );
    assert(*((size_t *) dllist_get(list, 0)) == data[0] );

    dllist_destroy(list);

    assert(!dllist_get(NULL, 0));

    printf("OK\n");
    return 0;
}

int main(void) 
{
    test_dllist_destroy_null();

    test_dllist_new();
    test_dllist_push_first();
    test_dllist_push_last();
    test_dllist_push_first_last();
    test_dllist_get();

    return 0;
}