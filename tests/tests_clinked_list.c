// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/clinked_list.h"

inline static void print_sizet_list(const cllist_t *list)
{
    for (size_t i = 0; i < list->len; ++i) {
        printf("%ld ", *((size_t *) cllist_get(list, i)));
    }
    printf("\n");
}

static int test_cllist_destroy_null(void)
{
    printf("%-40s", "test_cllist_destroy (null) ");

    cllist_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_cllist_new(void)
{
    printf("%-40s", "test_cllist_new ");

    cllist_t *list = cllist_new();

    assert(list);
    assert(!list->head);

    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_cllist_push_first(void)
{
    printf("%-40s", "test_cllist_push_first ");

    cllist_t *list = cllist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        assert(cllist_push_first(list, (void *) &data[i], sizeof(size_t)) == 0);
    }

    //print_sizet_list(list);

    assert(list->head);
    assert(list->head->next->next->next->next->next == list->head);
    assert(list->head->previous->previous->previous->previous->previous == list->head);
    assert(list->head->next->next->next->next == list->head->previous);
    assert(list->head->previous->previous->previous->previous == list->head->next);

    assert(*((size_t *) list->head->data) == data[4] );
    assert(*((size_t *) list->head->next->data) == data[3] );
    assert(*((size_t *) list->head->next->next->data) == data[2] );
    assert(*((size_t *) list->head->next->next->next->data) == data[1] );
    assert(*((size_t *) list->head->next->next->next->next->data) == data[0] );

    assert(*((size_t *) list->head->data) == data[4] );
    assert(*((size_t *) list->head->previous->data) == data[0] );
    assert(*((size_t *) list->head->previous->previous->data) == data[1] );
    assert(*((size_t *) list->head->previous->previous->previous->data) == data[2] );
    assert(*((size_t *) list->head->previous->previous->previous->previous->data) == data[3] );
    
    assert(list->len == 5);

    cllist_destroy(list);

    assert(cllist_push_first(NULL, (void *) &data[0], sizeof(size_t)) == 99);

    printf("OK\n");
    return 0;
}

static int test_cllist_push_last(void)
{
    printf("%-40s", "test_cllist_push_last ");

    cllist_t *list = cllist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        assert(cllist_push_last(list, (void *) &data[i], sizeof(size_t)) == 0);
    }

    //print_sizet_list(list);

    assert(list->head);
    assert(list->head->next->next->next->next->next == list->head);
    assert(list->head->previous->previous->previous->previous->previous == list->head);
    assert(list->head->next->next->next->next == list->head->previous);
    assert(list->head->previous->previous->previous->previous == list->head->next);

    assert(*((size_t *) list->head->data) == data[0] );
    assert(*((size_t *) list->head->next->data) == data[1] );
    assert(*((size_t *) list->head->next->next->data) == data[2] );
    assert(*((size_t *) list->head->next->next->next->data) == data[3] );
    assert(*((size_t *) list->head->next->next->next->next->data) == data[4] );

    assert(*((size_t *) list->head->data) == data[0] );
    assert(*((size_t *) list->head->previous->data) == data[4] );
    assert(*((size_t *) list->head->previous->previous->data) == data[3] );
    assert(*((size_t *) list->head->previous->previous->previous->data) == data[2] );
    assert(*((size_t *) list->head->previous->previous->previous->previous->data) == data[1] );
    
    assert(list->len == 5);

    cllist_destroy(list);

    assert(cllist_push_first(NULL, (void *) &data[0], sizeof(size_t)) == 99);

    printf("OK\n");
    return 0;
}

static int test_cllist_get(void)
{
    printf("%-40s", "test_cllist_get ");

    cllist_t *list = cllist_new();

    size_t data[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    for (size_t i = 0; i < 7; ++i) {
        assert(cllist_push_last(list, (void *) &data[i], sizeof(size_t)) == 0);
    }

    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[1] );
    assert(*((size_t *) cllist_get(list, 2)) == data[2] );
    assert(*((size_t *) cllist_get(list, 3)) == data[3] );
    assert(*((size_t *) cllist_get(list, 4)) == data[4] );
    assert(*((size_t *) cllist_get(list, 5)) == data[5] );
    assert(*((size_t *) cllist_get(list, 6)) == data[6] );
    assert(!cllist_get(list, -1));
    assert(!cllist_get(list, 7));
    assert(!cllist_get(list, 9643));

    assert(cllist_push_last(list, (void *) &data[7], sizeof(size_t)) == 0);
    assert(*((size_t *) cllist_get(list, 7)) == data[7] );
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );

    cllist_destroy(list);

    assert(!cllist_get(NULL, 0));

    printf("OK\n");
    return 0;
}

static int test_cllist_rotate(void)
{
    printf("%-40s", "test_cllist_rotate ");

    // rotate non-existent list
    assert(cllist_rotate(NULL, 3) == 99);
    assert(cllist_rotate(NULL, -1) == 99);

    cllist_t *list = cllist_new();

    // rotate empty list
    assert(cllist_rotate(list, 7) == 0);
    assert(cllist_rotate(list, -12) == 0);

    size_t data[5] = {0, 1, 2, 3, 4};

    for (size_t i = 0; i < 5; ++i) {
        cllist_push_last(list, (void *) &data[i], sizeof(size_t));

        // rotate list with one element
        if (i == 0) {
            assert(cllist_rotate(list, 1) == 0);
            assert(cllist_rotate(list, 9273) == 0);
            assert(cllist_rotate(list, -15) == 0);
            assert(cllist_rotate(list, -9948273) == 0);
        }

        // rotate list with two elements
        if (i == 1) {
            assert(cllist_rotate(list, 1) == 0);
            assert(*((size_t *) cllist_get(list, 0)) == 1 );
            assert(*((size_t *) cllist_get(list, 1)) == 0 );

            assert(cllist_rotate(list, 1) == 0);
            assert(*((size_t *) cllist_get(list, 0)) == 0 );
            assert(*((size_t *) cllist_get(list, 1)) == 1 );

            assert(cllist_rotate(list, -1) == 0);
            assert(*((size_t *) cllist_get(list, 0)) == 1 );
            assert(*((size_t *) cllist_get(list, 1)) == 0 );

            assert(cllist_rotate(list, -1) == 0);
            assert(*((size_t *) cllist_get(list, 0)) == 0 );
            assert(*((size_t *) cllist_get(list, 1)) == 1 );
        }
    }

    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 0 );
    assert(*((size_t *) cllist_get(list, 1)) == 1 );
    assert(*((size_t *) cllist_get(list, 2)) == 2 );
    assert(*((size_t *) cllist_get(list, 3)) == 3 );
    assert(*((size_t *) cllist_get(list, 4)) == 4 );

    // single rotations
    cllist_rotate(list, -1);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 1 );
    assert(*((size_t *) cllist_get(list, 1)) == 2 );
    assert(*((size_t *) cllist_get(list, 2)) == 3 );
    assert(*((size_t *) cllist_get(list, 3)) == 4 );
    assert(*((size_t *) cllist_get(list, 4)) == 0 );

    cllist_rotate(list, -1);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 2 );
    assert(*((size_t *) cllist_get(list, 1)) == 3 );
    assert(*((size_t *) cllist_get(list, 2)) == 4 );
    assert(*((size_t *) cllist_get(list, 3)) == 0 );
    assert(*((size_t *) cllist_get(list, 4)) == 1 );

    cllist_rotate(list, 1);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 1 );
    assert(*((size_t *) cllist_get(list, 1)) == 2 );
    assert(*((size_t *) cllist_get(list, 2)) == 3 );
    assert(*((size_t *) cllist_get(list, 3)) == 4 );
    assert(*((size_t *) cllist_get(list, 4)) == 0 );

    // multiple rotations
    cllist_rotate(list, -5);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 1 );
    assert(*((size_t *) cllist_get(list, 1)) == 2 );
    assert(*((size_t *) cllist_get(list, 2)) == 3 );
    assert(*((size_t *) cllist_get(list, 3)) == 4 );
    assert(*((size_t *) cllist_get(list, 4)) == 0 );

    cllist_rotate(list, -12);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 3 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(*((size_t *) cllist_get(list, 2)) == 0 );
    assert(*((size_t *) cllist_get(list, 3)) == 1 );
    assert(*((size_t *) cllist_get(list, 4)) == 2 );

    cllist_rotate(list, 7);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 1 );
    assert(*((size_t *) cllist_get(list, 1)) == 2 );
    assert(*((size_t *) cllist_get(list, 2)) == 3 );
    assert(*((size_t *) cllist_get(list, 3)) == 4 );
    assert(*((size_t *) cllist_get(list, 4)) == 0 );

    cllist_rotate(list, -3);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 4 );
    assert(*((size_t *) cllist_get(list, 1)) == 0 );
    assert(*((size_t *) cllist_get(list, 2)) == 1 );
    assert(*((size_t *) cllist_get(list, 3)) == 2 );
    assert(*((size_t *) cllist_get(list, 4)) == 3 );

    cllist_rotate(list, -2);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 1 );
    assert(*((size_t *) cllist_get(list, 1)) == 2 );
    assert(*((size_t *) cllist_get(list, 2)) == 3 );
    assert(*((size_t *) cllist_get(list, 3)) == 4 );
    assert(*((size_t *) cllist_get(list, 4)) == 0 );


    // a very large number of rotations
    cllist_rotate(list, -489226);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 2 );
    assert(*((size_t *) cllist_get(list, 1)) == 3 );
    assert(*((size_t *) cllist_get(list, 2)) == 4 );
    assert(*((size_t *) cllist_get(list, 3)) == 0 );
    assert(*((size_t *) cllist_get(list, 4)) == 1 );

    cllist_rotate(list, 2429);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 3 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(*((size_t *) cllist_get(list, 2)) == 0 );
    assert(*((size_t *) cllist_get(list, 3)) == 1 );
    assert(*((size_t *) cllist_get(list, 4)) == 2 );

    // no rotation
    cllist_rotate(list, 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 3 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(*((size_t *) cllist_get(list, 2)) == 0 );
    assert(*((size_t *) cllist_get(list, 3)) == 1 );
    assert(*((size_t *) cllist_get(list, 4)) == 2 );

    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

int main(void) 
{
    test_cllist_destroy_null();

    test_cllist_new();
    test_cllist_push_first();
    test_cllist_push_last();

    test_cllist_get();

    test_cllist_rotate();

    return 0;
}