// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/dlinked_list.h"

inline static void print_sizet_list(const dllist_t *list)
{
    for (size_t i = 0; i < list->len; ++i) {
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

static int test_dllist_insert_after_node(void)
{
    printf("%-40s", "test_dllist_insert_after_node ");

    size_t data[5] = {1, 2, 3, 4, 5};

    // invalid list
    assert(dllist_insert_after_node(NULL, &data[0], sizeof(size_t), NULL) == 99);

    dllist_t *list = dllist_new();

    // empty list
    assert(dllist_insert_after_node(list, &data[0], sizeof(size_t), NULL) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[0] );
    assert(list->head);
    assert(list->tail);
    assert(list->head == list->tail);
    assert(list->len == 1);

    // end of the list
    assert(dllist_insert_after_node(list, &data[1], sizeof(size_t), list->head) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[0] );
    assert(*((size_t *) dllist_get(list, 1)) == data[1] );
    assert(list->head == list->tail->previous);
    assert(list->tail = list->head->next);
    assert(list->len == 2);

    // start of the list
    assert(dllist_insert_after_node(list, &data[2], sizeof(size_t), NULL) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[2] );
    assert(*((size_t *) dllist_get(list, 1)) == data[0] );
    assert(*((size_t *) dllist_get(list, 2)) == data[1] );
    assert(list->head == list->tail->previous->previous);
    assert(list->tail = list->head->next->next);
    assert(list->len == 3);

    // middle of the list
    assert(dllist_insert_after_node(list, &data[3], sizeof(size_t), list->head->next) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[2] );
    assert(*((size_t *) dllist_get(list, 1)) == data[0] );
    assert(*((size_t *) dllist_get(list, 2)) == data[3] );
    assert(*((size_t *) dllist_get(list, 3)) == data[1] );
    assert(list->head == list->tail->previous->previous->previous);
    assert(list->tail = list->head->next->next->next);
    assert(list->len == 4);

    // middle of the list #2
    assert(dllist_insert_after_node(list, &data[4], sizeof(size_t), list->tail->previous->previous) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[2] );
    assert(*((size_t *) dllist_get(list, 1)) == data[0] );
    assert(*((size_t *) dllist_get(list, 2)) == data[4] );
    assert(*((size_t *) dllist_get(list, 3)) == data[3] );
    assert(*((size_t *) dllist_get(list, 4)) == data[1] );
    assert(list->head == list->tail->previous->previous->previous->previous);
    assert(list->tail = list->head->next->next->next->next);
    assert(list->len == 5);

    //print_sizet_list(list);

    dllist_destroy(list);

    printf("OK\n");
    return 0;
}


static int test_dllist_insert_before_node(void)
{
    printf("%-40s", "test_dllist_insert_before_node ");

    size_t data[5] = {1, 2, 3, 4, 5};

    // invalid list
    assert(dllist_insert_before_node(NULL, &data[0], sizeof(size_t), NULL) == 99);

    dllist_t *list = dllist_new();

    // empty list
    assert(dllist_insert_before_node(list, &data[0], sizeof(size_t), NULL) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[0] );
    assert(list->head);
    assert(list->tail);
    assert(list->head == list->tail);
    assert(list->len == 1);

    // start of the list
    assert(dllist_insert_before_node(list, &data[1], sizeof(size_t), list->head) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[1] );
    assert(*((size_t *) dllist_get(list, 1)) == data[0] );
    assert(list->head == list->tail->previous);
    assert(list->tail = list->head->next);
    assert(list->len == 2);

    //print_sizet_list(list);

    // end of the list
    assert(dllist_insert_before_node(list, &data[2], sizeof(size_t), NULL) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[1] );
    assert(*((size_t *) dllist_get(list, 1)) == data[0] );
    assert(*((size_t *) dllist_get(list, 2)) == data[2] );
    assert(list->head == list->tail->previous->previous);
    assert(list->tail = list->head->next->next);
    assert(list->len == 3);

    // middle of the list
    assert(dllist_insert_before_node(list, &data[3], sizeof(size_t), list->head->next) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[1] );
    assert(*((size_t *) dllist_get(list, 1)) == data[3] );
    assert(*((size_t *) dllist_get(list, 2)) == data[0] );
    assert(*((size_t *) dllist_get(list, 3)) == data[2] );
    assert(list->head == list->tail->previous->previous->previous);
    assert(list->tail = list->head->next->next->next);
    assert(list->len == 4);

    // middle of the list #2
    assert(dllist_insert_before_node(list, &data[4], sizeof(size_t), list->tail->previous->previous) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[1] );
    assert(*((size_t *) dllist_get(list, 1)) == data[4] );
    assert(*((size_t *) dllist_get(list, 2)) == data[3] );
    assert(*((size_t *) dllist_get(list, 3)) == data[0] );
    assert(*((size_t *) dllist_get(list, 4)) == data[2] );
    assert(list->head == list->tail->previous->previous->previous->previous);
    assert(list->tail = list->head->next->next->next->next);
    assert(list->len == 5);

    //print_sizet_list(list);

    dllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_dllist_insert(void)
{
    printf("%-40s", "test_dllist_insert ");

    size_t data[5] = {1, 2, 3, 4, 5};

    // invalid list
    assert(dllist_insert(NULL, &data[0], sizeof(size_t), 0) == 99);

    dllist_t *list = dllist_new();

    // empty list fails
    assert(dllist_insert(list, &data[0], sizeof(size_t), 1) == 2);
    assert(dllist_insert(list, &data[0], sizeof(size_t), 6) == 2);
    assert(dllist_insert(list, &data[0], sizeof(size_t), -1) == 2);
    assert(dllist_insert(list, &data[0], sizeof(size_t), -381) == 2);
    assert(!list->head);
    assert(!list->tail);
    assert(list->len == 0);

    // empty list
    assert(dllist_insert(list, &data[0], sizeof(size_t), 0) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[0] );
    assert(list->head);
    assert(list->tail);
    assert(list->head == list->tail);
    assert(list->len == 1);

    // start of the list
    assert(dllist_insert(list, &data[1], sizeof(size_t), 0) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[1] );
    assert(*((size_t *) dllist_get(list, 1)) == data[0] );
    assert(list->head == list->tail->previous);
    assert(list->tail = list->head->next);
    assert(list->len == 2);

    //print_sizet_list(list);

    // end of the list
    assert(dllist_insert(list, &data[2], sizeof(size_t), 2) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[1] );
    assert(*((size_t *) dllist_get(list, 1)) == data[0] );
    assert(*((size_t *) dllist_get(list, 2)) == data[2] );
    assert(list->head == list->tail->previous->previous);
    assert(list->tail = list->head->next->next);
    assert(list->len == 3);

    // middle of the list
    assert(dllist_insert(list, &data[3], sizeof(size_t), 1) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[1] );
    assert(*((size_t *) dllist_get(list, 1)) == data[3] );
    assert(*((size_t *) dllist_get(list, 2)) == data[0] );
    assert(*((size_t *) dllist_get(list, 3)) == data[2] );
    assert(list->head == list->tail->previous->previous->previous);
    assert(list->tail = list->head->next->next->next);
    assert(list->len == 4);

    // second to last
    assert(dllist_insert(list, &data[4], sizeof(size_t), 3) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == data[1] );
    assert(*((size_t *) dllist_get(list, 1)) == data[3] );
    assert(*((size_t *) dllist_get(list, 2)) == data[0] );
    assert(*((size_t *) dllist_get(list, 3)) == data[4] );
    assert(*((size_t *) dllist_get(list, 4)) == data[2] );
    assert(list->head == list->tail->previous->previous->previous->previous);
    assert(list->tail = list->head->next->next->next->next);
    assert(list->len == 5);

    //print_sizet_list(list);

    dllist_destroy(list);

    printf("OK\n");
    return 0;
}


static int test_dllist_remove_node(void)
{
    printf("%-40s", "test_dllist_remove_node ");

    // non-existent list
    assert(dllist_remove_node(NULL, NULL) == 99);

    dllist_t *list = dllist_new();

    // empty list
    assert(dllist_remove_node(list, NULL) == 1);

    size_t data[] = {1, 2, 3, 4, 5};

    for (size_t i = 0; i < 5; ++i) {
        dllist_push_last(list, (void *) &data[i], sizeof(size_t));
    }

    // remove item from the middle
    assert(dllist_remove_node(list, list->head->next->next) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == 1 );
    assert(*((size_t *) dllist_get(list, 1)) == 2 );
    assert(*((size_t *) dllist_get(list, 2)) == 4 );
    assert(*((size_t *) dllist_get(list, 3)) == 5 );
    assert(list->len == 4);
    assert(list->head == list->tail->previous->previous->previous);
    assert(list->tail = list->head->next->next->next);

    // remove item from the start
    assert(dllist_remove_node(list, list->head) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == 2 );
    assert(*((size_t *) dllist_get(list, 1)) == 4 );
    assert(*((size_t *) dllist_get(list, 2)) == 5 );
    assert(list->len == 3);
    assert(list->head == list->tail->previous->previous);
    assert(list->tail = list->head->next->next);

    // remove item from the end
    assert(dllist_remove_node(list, list->tail) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == 2 );
    assert(*((size_t *) dllist_get(list, 1)) == 4 );
    assert(list->len == 2);
    assert(list->head == list->tail->previous);
    assert(list->tail = list->head->next);

    // NULL pointer for node (nothing should change)
    assert(dllist_remove_node(list, NULL) == 1);
    assert(*((size_t *) dllist_get(list, 0)) == 2 );
    assert(*((size_t *) dllist_get(list, 1)) == 4 );
    assert(list->len == 2);
    assert(list->head == list->tail->previous);
    assert(list->tail = list->head->next);

    // remove the remaining two items
    assert(dllist_remove_node(list, list->head) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == 4 );
    assert(list->len == 1);
    assert(list->head == list->tail);
    
    assert(dllist_remove_node(list, list->tail) == 0);
    assert(list->len == 0);
    assert(!list->head);
    assert(!list->tail);

    dllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_dllist_remove(void)
{
    printf("%-40s", "test_dllist_remove ");

    // non-existent list
    assert(dllist_remove(NULL, 0) == 99);

    dllist_t *list = dllist_new();

    // empty list
    assert(dllist_remove(list, 0) == 1);
    assert(dllist_remove(list, 1) == 1);
    assert(dllist_remove(list, 42) == 1);
    assert(dllist_remove(list, -1) == 1);
    assert(dllist_remove(list, -17) == 1);

    size_t data[] = {1, 2, 3, 4, 5};

    for (size_t i = 0; i < 5; ++i) {
        dllist_push_last(list, (void *) &data[i], sizeof(size_t));
    }

    // remove item from the middle
    assert(dllist_remove(list, 2) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == 1 );
    assert(*((size_t *) dllist_get(list, 1)) == 2 );
    assert(*((size_t *) dllist_get(list, 2)) == 4 );
    assert(*((size_t *) dllist_get(list, 3)) == 5 );
    assert(list->len == 4);
    assert(list->head == list->tail->previous->previous->previous);
    assert(list->tail = list->head->next->next->next);

    // remove item from the start
    assert(dllist_remove(list, 0) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == 2 );
    assert(*((size_t *) dllist_get(list, 1)) == 4 );
    assert(*((size_t *) dllist_get(list, 2)) == 5 );
    assert(list->len == 3);
    assert(list->head == list->tail->previous->previous);
    assert(list->tail = list->head->next->next);

    // remove item from the end
    assert(dllist_remove(list, 2) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == 2 );
    assert(*((size_t *) dllist_get(list, 1)) == 4 );
    assert(list->len == 2);
    assert(list->head == list->tail->previous);
    assert(list->tail = list->head->next);

    // index out of bounds (nothing should change)
    assert(dllist_remove(list, 2) == 1);
    assert(dllist_remove(list, 42) == 1);
    assert(dllist_remove(list, -1) == 1);
    assert(dllist_remove(list, -17) == 1);
    assert(*((size_t *) dllist_get(list, 0)) == 2 );
    assert(*((size_t *) dllist_get(list, 1)) == 4 );
    assert(list->len == 2);
    assert(list->head == list->tail->previous);
    assert(list->tail = list->head->next);

    // remove the remaining two items
    assert(dllist_remove(list, 0) == 0);
    assert(*((size_t *) dllist_get(list, 0)) == 4 );
    assert(list->len == 1);
    assert(list->head == list->tail);
    
    assert(dllist_remove(list, 0) == 0);
    assert(list->len == 0);
    assert(!list->head);
    assert(!list->tail);

    dllist_destroy(list);

    printf("OK\n");
    return 0;
}


static int test_filter_function(const void *data)
{
    return *((size_t *) data) >= 5;
}

static int test_dllist_filter_mut(void)
{
    printf("%-40s", "test_dllist_filter_mut ");

    // perform filter on non-existent list
    assert(dllist_filter_mut(NULL, test_filter_function) == 0);

    dllist_t *list = dllist_new();

    // perform filter on empty list
    assert(dllist_filter_mut(list, test_filter_function) == 0);

    size_t data[] = {1, 3, 6, 4, 5, 5, 0, 2, 3, 9};

    for (size_t i = 0; i < 10; ++i) {
        dllist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_list(list);

    assert(dllist_filter_mut(list, test_filter_function) == 6);

    //print_sizet_list(list);
    assert(list->len == 4);

    assert(*((size_t *) dllist_get(list, 0)) == 9);
    assert(*((size_t *) dllist_get(list, 1)) == 5);
    assert(*((size_t *) dllist_get(list, 2)) == 5);
    assert(*((size_t *) dllist_get(list, 3)) == 6);

    // applying the same filter again should do nothing
    assert(dllist_filter_mut(list, test_filter_function) == 0);
    assert(list->len == 4);

    assert(*((size_t *) dllist_get(list, 0)) == 9);
    assert(*((size_t *) dllist_get(list, 1)) == 5);
    assert(*((size_t *) dllist_get(list, 2)) == 5);
    assert(*((size_t *) dllist_get(list, 3)) == 6);

    dllist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_comparison_function(const void *data, const void *target)
{
    return *((size_t *) data) == *((size_t *) target);
}

static int test_dllist_find(void)
{
    printf("%-40s", "test_dllist_find ");

    size_t search[] = {9, 1, 5, 3, 19};

    // search in non-existent list
    assert(dllist_find(NULL, test_comparison_function, (void *) &search[0]) == NULL);

    dllist_t *list = dllist_new();

    // search in an empty list
    assert(dllist_find(list, test_comparison_function, (void *) &search[0]) == NULL);

    size_t data[] = {1, 3, 6, 4, 5, 5, 0, 2, 3, 9};

    for (size_t i = 0; i < 10; ++i) {
        dllist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    dnode_t *found = NULL;
    // find item at the beginning of list
    found = dllist_find(list, test_comparison_function, (void *) &search[0]);
    assert(found == list->head);
    assert(*((size_t *) found->data) == search[0]);
    // find item at the end of list
    found = dllist_find(list, test_comparison_function, (void *) &search[1]);
    assert(found == list->tail);
    assert(*((size_t *) found->data) == search[1]);
    // find first of many items
    found = dllist_find(list, test_comparison_function, (void *) &search[2]);
    assert(found == list->head->next->next->next->next);
    assert(*((size_t *) found->data) == search[2]);

    found = dllist_find(list, test_comparison_function, (void *) &search[3]);
    assert(found == list->head->next);
    assert(*((size_t *) found->data) == search[3]);

    // search for non-existent item
    assert(dllist_find(list, test_comparison_function, (void *) &search[4]) == NULL);

    dllist_destroy(list);

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

    test_dllist_insert_after_node();
    test_dllist_insert_before_node();
    test_dllist_insert();

    test_dllist_remove_node();
    test_dllist_remove();

    test_dllist_filter_mut();

    test_dllist_find();

    return 0;
}