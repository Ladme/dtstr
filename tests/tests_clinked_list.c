// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/clinked_list.h"

#define UNUSED(x) (void)(x)

inline static void print_sizet_list(const cllist_t *list)
{
    for (size_t i = 0; i < list->len; ++i) {
        printf("%ld ", *((size_t *) cllist_get(list, i)));
    }
    printf("\n");
}

inline static int cllist_check_fully_linked(const cllist_t *list)
{
    if (list->len == 0) return 1;
    if (list->len == 1) return (list->head == list->head->previous && list->head == list->head->next);

    if (list->head == list->head->previous || list->head == list->head->next) return 0;

    cnode_t *clockwise = list->head;
    cnode_t *counterclockwise = list->head;
    for (size_t i = 0; i < list->len; ++i) {
        clockwise = clockwise->next;
        counterclockwise = counterclockwise->previous;
    }

    return (clockwise == counterclockwise);
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

static int test_cllist_insert_before_node(void)
{
    printf("%-40s", "test_cllist_insert_before_node ");

    size_t data[5] = {1, 2, 3, 4, 5};

    // invalid list
    assert(cllist_insert_before_node(NULL, &data[0], sizeof(size_t), NULL) == 99);

    cllist_t *list = cllist_new();

    // empty list
    assert(cllist_insert_before_node(list, &data[0], sizeof(size_t), NULL) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 1);

    // end of the list
    assert(cllist_insert_before_node(list, &data[1], sizeof(size_t), NULL) == 0);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 2);

    // middle of the list
    assert(cllist_insert_before_node(list, &data[2], sizeof(size_t), list->head->next) == 0);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[2] );
    assert(*((size_t *) cllist_get(list, 2)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 3);

    // middle of the list #2
    assert(cllist_insert_before_node(list, &data[3], sizeof(size_t), list->head->previous->previous) == 0);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[3] );
    assert(*((size_t *) cllist_get(list, 2)) == data[2] );
    assert(*((size_t *) cllist_get(list, 3)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 4);
    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_cllist_insert_after_node(void)
{
    printf("%-40s", "test_cllist_insert_after_node ");

    size_t data[5] = {1, 2, 3, 4, 5};

    // invalid list
    assert(cllist_insert_after_node(NULL, &data[0], sizeof(size_t), NULL) == 99);

    cllist_t *list = cllist_new();

    // empty list
    assert(cllist_insert_after_node(list, &data[0], sizeof(size_t), NULL) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 1);

    // end of the list
    assert(cllist_insert_after_node(list, &data[1], sizeof(size_t), list->head) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 2);

    // middle of the list
    assert(cllist_insert_after_node(list, &data[2], sizeof(size_t), list->head) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[2] );
    assert(*((size_t *) cllist_get(list, 2)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 3);

    // end of the list #2
    assert(cllist_insert_after_node(list, &data[3], sizeof(size_t), list->head->previous) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[2] );
    assert(*((size_t *) cllist_get(list, 2)) == data[1] );
    assert(*((size_t *) cllist_get(list, 3)) == data[3] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 4);

    // second element
    assert(cllist_insert_after_node(list, &data[4], sizeof(size_t), NULL) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[4] );
    assert(*((size_t *) cllist_get(list, 2)) == data[2] );
    assert(*((size_t *) cllist_get(list, 3)) == data[1] );
    assert(*((size_t *) cllist_get(list, 4)) == data[3] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 5);

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

    assert(cllist_check_fully_linked(list));

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

    assert(cllist_check_fully_linked(list));

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

    assert(cllist_push_last(NULL, (void *) &data[0], sizeof(size_t)) == 99);

    printf("OK\n");
    return 0;
}

static int test_cllist_insert(void)
{
    printf("%-40s", "test_cllist_insert ");

    size_t data[5] = {1, 2, 3, 4, 5};

    // invalid list
    assert(cllist_insert(NULL, &data[0], sizeof(size_t), 0) == 99);

    cllist_t *list = cllist_new();

    // empty list
    assert(cllist_insert(list, &data[0], sizeof(size_t), 1) == 2);
    assert(cllist_insert(list, &data[0], sizeof(size_t), -1) == 2);
    assert(cllist_insert(list, &data[0], sizeof(size_t), 0) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 1);

    // end of the list
    assert(cllist_insert(list, &data[1], sizeof(size_t), 1) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 2);

    // middle of the list
    assert(cllist_insert(list, &data[2], sizeof(size_t), 1) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[0] );
    assert(*((size_t *) cllist_get(list, 1)) == data[2] );
    assert(*((size_t *) cllist_get(list, 2)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 3);

    // start of the list
    assert(cllist_insert(list, &data[3], sizeof(size_t), 0) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[3] );
    assert(*((size_t *) cllist_get(list, 1)) == data[0] );
    assert(*((size_t *) cllist_get(list, 2)) == data[2] );
    assert(*((size_t *) cllist_get(list, 3)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 4);

    // second to last item
    assert(cllist_insert(list, &data[4], sizeof(size_t), 3) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[3] );
    assert(*((size_t *) cllist_get(list, 1)) == data[0] );
    assert(*((size_t *) cllist_get(list, 2)) == data[2] );
    assert(*((size_t *) cllist_get(list, 3)) == data[4] );
    assert(*((size_t *) cllist_get(list, 4)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 5);

    // out of bounds indices
    assert(cllist_insert(list, &data[4], sizeof(size_t), -1) == 2);
    assert(cllist_insert(list, &data[4], sizeof(size_t), -15) == 2);
    assert(cllist_insert(list, &data[4], sizeof(size_t), 10247) == 2);
    assert(cllist_insert(list, &data[4], sizeof(size_t), 6) == 2);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == data[3] );
    assert(*((size_t *) cllist_get(list, 1)) == data[0] );
    assert(*((size_t *) cllist_get(list, 2)) == data[2] );
    assert(*((size_t *) cllist_get(list, 3)) == data[4] );
    assert(*((size_t *) cllist_get(list, 4)) == data[1] );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 5);

    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_cllist_len(void)
{
    printf("%-40s", "test_cllist_len ");

    assert(cllist_len(NULL) == 0);

    cllist_t *list = cllist_new();

    assert(cllist_len(list) == 0);

    for (size_t i = 0; i < 1000; ++i) {
        assert(cllist_push_last(list, &i, sizeof(size_t)) == 0);
        assert(cllist_len(list) == i + 1);
    }

    cllist_destroy(list);

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

static int test_cllist_remove_node(void)
{
    printf("%-40s", "test_cllist_remove_node ");

    // non-existent list
    assert(cllist_remove_node(NULL, NULL) == 99);

    cllist_t *list = cllist_new();

    // empty list
    assert(cllist_remove_node(list, NULL) == 1);

    size_t data[] = {1, 2, 3, 4, 5};

    for (size_t i = 0; i < 5; ++i) {
        cllist_push_last(list, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_list(list);

    // remove item from the middle
    assert(cllist_remove_node(list, list->head->next->next) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 1 );
    assert(*((size_t *) cllist_get(list, 1)) == 2 );
    assert(*((size_t *) cllist_get(list, 2)) == 4 );
    assert(*((size_t *) cllist_get(list, 3)) == 5 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 4);

    // remove item from the start
    assert(cllist_remove_node(list, list->head) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 2 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(*((size_t *) cllist_get(list, 2)) == 5 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 3);

    // remove item from the end
    assert(cllist_remove_node(list, list->head->previous) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 2 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 2);

    // NULL pointer for node (nothing should change)
    assert(cllist_remove_node(list, NULL) == 1);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 2 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 2);

    // remove the remaining two items
    assert(cllist_remove_node(list, list->head) == 0);
    //print_sizet_list(list);
    assert(*((size_t *) cllist_get(list, 0)) == 4 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 1);
    
    assert(cllist_remove_node(list, list->head->next->next) == 0);
    //print_sizet_list(list);
    assert(cllist_check_fully_linked(list));
    assert(list->len == 0);
    assert(!list->head);
    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_cllist_remove(void)
{
    printf("%-40s", "test_cllist_remove ");

    // non-existent list
    assert(cllist_remove(NULL, 0) == 99);

    cllist_t *list = cllist_new();

    // empty list
    assert(cllist_remove(list, 0) == 1);
    assert(cllist_remove(list, 1) == 1);
    assert(cllist_remove(list, 42) == 1);
    assert(cllist_remove(list, -1) == 1);
    assert(cllist_remove(list, -17) == 1);

    size_t data[] = {1, 2, 3, 4, 5};

    for (size_t i = 0; i < 5; ++i) {
        cllist_push_last(list, (void *) &data[i], sizeof(size_t));
    }

    // remove item from the middle
    assert(cllist_remove(list, 2) == 0);
    assert(*((size_t *) cllist_get(list, 0)) == 1 );
    assert(*((size_t *) cllist_get(list, 1)) == 2 );
    assert(*((size_t *) cllist_get(list, 2)) == 4 );
    assert(*((size_t *) cllist_get(list, 3)) == 5 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 4);

    // remove item from the start
    assert(cllist_remove(list, 0) == 0);
    assert(*((size_t *) cllist_get(list, 0)) == 2 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(*((size_t *) cllist_get(list, 2)) == 5 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 3);

    // remove item from the end
    assert(cllist_remove(list, 2) == 0);
    assert(*((size_t *) cllist_get(list, 0)) == 2 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 2);

    // index out of bounds (nothing should change)
    assert(cllist_remove(list, 2) == 1);
    assert(cllist_remove(list, 42) == 1);
    assert(cllist_remove(list, -1) == 1);
    assert(cllist_remove(list, -17) == 1);
    assert(*((size_t *) cllist_get(list, 0)) == 2 );
    assert(*((size_t *) cllist_get(list, 1)) == 4 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 2);

    // remove the remaining two items
    assert(cllist_remove(list, 0) == 0);
    assert(*((size_t *) cllist_get(list, 0)) == 4 );
    assert(cllist_check_fully_linked(list));
    assert(list->len == 1);
    
    assert(cllist_remove(list, 0) == 0);
    assert(cllist_check_fully_linked(list));
    assert(list->len == 0);
    assert(!list->head);

    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_filter_function(const void *data)
{
    return *((size_t *) data) >= 5;
}

static int test_cllist_filter_mut(void)
{
    printf("%-40s", "test_cllist_filter_mut ");

    // perform filter on non-existent list
    assert(cllist_filter_mut(NULL, test_filter_function) == 0);

    cllist_t *list = cllist_new();

    // perform filter on empty list
    assert(cllist_filter_mut(list, test_filter_function) == 0);

    size_t data[] = {1, 3, 6, 4, 5, 5, 0, 2, 3, 9};

    for (size_t i = 0; i < 10; ++i) {
        cllist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_list(list);

    assert(cllist_filter_mut(list, test_filter_function) == 6);

    //print_sizet_list(list);
    assert(list->len == 4);

    assert(*((size_t *) cllist_get(list, 0)) == 9);
    assert(*((size_t *) cllist_get(list, 1)) == 5);
    assert(*((size_t *) cllist_get(list, 2)) == 5);
    assert(*((size_t *) cllist_get(list, 3)) == 6);

    // applying the same filter again should do nothing
    assert(cllist_filter_mut(list, test_filter_function) == 0);
    assert(list->len == 4);

    assert(*((size_t *) cllist_get(list, 0)) == 9);
    assert(*((size_t *) cllist_get(list, 1)) == 5);
    assert(*((size_t *) cllist_get(list, 2)) == 5);
    assert(*((size_t *) cllist_get(list, 3)) == 6);

    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_comparison_function(const void *data, const void *target)
{
    return *((size_t *) data) == *((size_t *) target);
}

static int test_cllist_find(void)
{
    printf("%-40s", "test_cllist_find ");

    size_t search[] = {9, 1, 5, 3, 19};

    // search in non-existent list
    assert(cllist_find(NULL, test_comparison_function, (void *) &search[0]) == NULL);

    cllist_t *list = cllist_new();

    // search in an empty list
    assert(cllist_find(list, test_comparison_function, (void *) &search[0]) == NULL);

    size_t data[] = {1, 3, 6, 4, 5, 5, 0, 2, 3, 9};

    for (size_t i = 0; i < 10; ++i) {
        cllist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    cnode_t *found = NULL;
    // find item at the beginning of list
    found = cllist_find(list, test_comparison_function, (void *) &search[0]);
    assert(found == list->head);
    assert(*((size_t *) found->data) == search[0]);
    // find item at the end of list
    found = cllist_find(list, test_comparison_function, (void *) &search[1]);
    assert(found == list->head->previous);
    assert(*((size_t *) found->data) == search[1]);
    // find first of many items
    found = cllist_find(list, test_comparison_function, (void *) &search[2]);
    assert(found == list->head->next->next->next->next);
    assert(*((size_t *) found->data) == search[2]);

    found = cllist_find(list, test_comparison_function, (void *) &search[3]);
    assert(found == list->head->next);
    assert(*((size_t *) found->data) == search[3]);

    // search for non-existent item
    assert(cllist_find(list, test_comparison_function, (void *) &search[4]) == NULL);

    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

static void multiply_by_two(void *item, void *unused)
{
    UNUSED(unused);
    size_t *ptr = (size_t *) item;
    *ptr *= 2;
}

static int test_cllist_map(void)
{
    printf("%-40s", "test_cllist_map ");

    cllist_map(NULL, multiply_by_two, NULL);

    cllist_t *list = cllist_new();

    for (size_t i = 0; i < 100; ++i) {
        cllist_push_last(list, &i, sizeof(size_t));
    }

    cllist_map(list, multiply_by_two, NULL);

    for (size_t i = 0; i < 100; ++i) {
        assert( *(size_t *) cllist_get(list, i) == i * 2);
    }

    cllist_destroy(list);

    printf("OK\n");
    return 0;
}

int main(void) 
{
    test_cllist_destroy_null();

    test_cllist_new();
    test_cllist_insert_before_node();
    test_cllist_insert_after_node();
    test_cllist_push_first();
    test_cllist_push_last();
    test_cllist_insert();
    test_cllist_len();

    test_cllist_get();

    test_cllist_rotate();

    test_cllist_remove_node();
    test_cllist_remove();

    test_cllist_filter_mut();

    test_cllist_find();

    test_cllist_map();

    return 0;
}