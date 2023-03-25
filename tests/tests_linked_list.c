// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/linked_list.h"

inline static void print_sizet_list(const llist_t *list, const size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        printf("%ld ", *((size_t *) llist_get(list, i)));
    }
    printf("\n");
}

static int test_llist_destroy_null(void)
{
    printf("%-40s", "test_llist_destroy (null) ");

    llist_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_llist_new(void)
{
    printf("%-40s", "test_llist_new ");

    llist_t *list = llist_new();

    assert(list);
    assert(!list->head);

    llist_destroy(list);

    printf("OK\n");
    return 0;
}

static int test_llist_push_first(void)
{
    printf("%-40s", "test_llist_push_first ");

    llist_t *list = llist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        assert(llist_push_first(list, (void *) &data[i], sizeof(size_t)) == 0);
    }

    assert(*((size_t *) list->head->data) == data[4] );
    assert(*((size_t *) list->head->next->data) == data[3] );
    assert(*((size_t *) list->head->next->next->data) == data[2] );
    assert(*((size_t *) list->head->next->next->next->data) == data[1] );
    assert(*((size_t *) list->head->next->next->next->next->data) == data[0] );

    llist_destroy(list);

    assert(llist_push_first(NULL, (void *) &data[0], sizeof(size_t)) == 99);

    printf("OK\n");
    return 0;
}

static int test_llist_push_last(void)
{
    printf("%-40s", "test_llist_push_last ");

    llist_t *list = llist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        assert(llist_push_last(list, (void *) &data[i], sizeof(size_t)) == 0);
    }

    assert(*((size_t *) list->head->data) == data[0] );
    assert(*((size_t *) list->head->next->data) == data[1] );
    assert(*((size_t *) list->head->next->next->data) == data[2] );
    assert(*((size_t *) list->head->next->next->next->data) == data[3] );
    assert(*((size_t *) list->head->next->next->next->next->data) == data[4] );

    llist_destroy(list);

    assert(llist_push_last(NULL, (void *) &data[0], sizeof(size_t)) == 99);

    printf("OK\n");
    return 0;
}

static int test_llist_push_first_last(void)
{
    printf("%-40s", "test_llist_push_first/last (mixed) ");

    llist_t *list = llist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        if (i % 2) assert(llist_push_first(list, (void *) &data[i], sizeof(size_t)) == 0);
        else assert(llist_push_last(list, (void *) &data[i], sizeof(size_t)) == 0);
    }

    //print_sizet_list(list, 5);

    assert(*((size_t *) list->head->data) == data[3] );
    assert(*((size_t *) list->head->next->data) == data[1] );
    assert(*((size_t *) list->head->next->next->data) == data[0] );
    assert(*((size_t *) list->head->next->next->next->data) == data[2] );
    assert(*((size_t *) list->head->next->next->next->next->data) == data[4] );

    llist_destroy(list);
    printf("OK\n");
    return 0;
}

static int test_llist_get(void)
{
    printf("%-40s", "test_llist_get ");

    llist_t *list = llist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        assert(llist_push_first(list, (void *) &data[i], sizeof(size_t)) == 0);
    }

    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == data[3] );
    assert(*((size_t *) llist_get(list, 2)) == data[2] );
    assert(*((size_t *) llist_get(list, 3)) == data[1] );
    assert(*((size_t *) llist_get(list, 4)) == data[0] );
    assert(!llist_get(list, -1));
    assert(!llist_get(list, 5));
    assert(!llist_get(list, 9643));

    llist_destroy(list);

    assert(!llist_get(NULL, 0));

    printf("OK\n");
    return 0;
}

static int test_llist_insert_after_node(void)
{
    printf("%-40s", "test_llist_insert_after_node ");

    llist_t *list = llist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        llist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_list(list, 5);

    size_t inserted_data[5] = {11, 22, 33, 44, 55};

    // adding item to the middle of the list
    node_t *previous = list->head->next->next;

    assert(llist_insert_after_node(list, (void *) &inserted_data[0], sizeof(size_t), previous) == 0);

    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == data[3] );
    assert(*((size_t *) llist_get(list, 2)) == data[2] );
    assert(*((size_t *) llist_get(list, 3)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 4)) == data[1] );
    assert(*((size_t *) llist_get(list, 5)) == data[0] );

    //print_sizet_list(list, 6);

    // adding item after the first element
    previous = list->head;

    assert(llist_insert_after_node(list, (void *) &inserted_data[1], sizeof(size_t), previous) == 0);

    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == data[2] );
    assert(*((size_t *) llist_get(list, 4)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );

    //print_sizet_list(list, 7);

    // adding item to the end of the list
    previous = list->head->next->next->next->next->next->next;

    assert(llist_insert_after_node(list, (void *) &inserted_data[2], sizeof(size_t), previous) == 0);

    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == data[2] );
    assert(*((size_t *) llist_get(list, 4)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );
    assert(*((size_t *) llist_get(list, 7)) == inserted_data[2] );

    //print_sizet_list(list, 8);

    // adding item to the start of the list
    assert(llist_insert_after_node(list, (void *) &inserted_data[3], sizeof(size_t), NULL) == 0);

    assert(*((size_t *) llist_get(list, 0)) == inserted_data[3] );
    assert(*((size_t *) llist_get(list, 1)) == data[4] );
    assert(*((size_t *) llist_get(list, 2)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 3)) == data[3] );
    assert(*((size_t *) llist_get(list, 4)) == data[2] );
    assert(*((size_t *) llist_get(list, 5)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 6)) == data[1] );
    assert(*((size_t *) llist_get(list, 7)) == data[0] );
    assert(*((size_t *) llist_get(list, 8)) == inserted_data[2] );

    //print_sizet_list(list, 9);

    assert(llist_insert_after_node(NULL, (void *) &inserted_data[0], sizeof(size_t), list->head) == 99);

    llist_destroy(list);

    // operating on an empty linked list
    llist_t *empty_list = llist_new();
    assert(llist_insert_after_node(empty_list, (void *) &inserted_data[4], sizeof(size_t), NULL) == 0);

    assert(*((size_t *) llist_get(empty_list, 0)) == inserted_data[4] );

    llist_destroy(empty_list);

    printf("OK\n");
    return 0;

}

static int test_llist_insert_after(void)
{
    printf("%-40s", "test_llist_insert_after ");

    llist_t *list = llist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        llist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_list(list, 5);

    size_t inserted_data[5] = {11, 22, 33, 44, 55};

    assert(llist_insert_after(list, (void *) &inserted_data[0], sizeof(size_t), 2) == 0);
    
    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == data[3] );
    assert(*((size_t *) llist_get(list, 2)) == data[2] );
    assert(*((size_t *) llist_get(list, 3)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 4)) == data[1] );
    assert(*((size_t *) llist_get(list, 5)) == data[0] );

    //print_sizet_list(list, 6);

    assert(llist_insert_after(list, (void *) &inserted_data[1], sizeof(size_t), 0) == 0);

    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == data[2] );
    assert(*((size_t *) llist_get(list, 4)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );

    //print_sizet_list(list, 7);

    assert(llist_insert_after(list, (void *) &inserted_data[2], sizeof(size_t), 6) == 0);

    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == data[2] );
    assert(*((size_t *) llist_get(list, 4)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );
    assert(*((size_t *) llist_get(list, 7)) == inserted_data[2] );

    //print_sizet_list(list, 8);

    assert(llist_insert_after(list, (void *) &inserted_data[3], sizeof(size_t), 6) == 0);

    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == data[2] );
    assert(*((size_t *) llist_get(list, 4)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );
    assert(*((size_t *) llist_get(list, 7)) == inserted_data[3] );
    assert(*((size_t *) llist_get(list, 8)) == inserted_data[2] );

    //print_sizet_list(list, 9);

    assert(llist_insert_after(list, (void *) &inserted_data[4], sizeof(size_t), -1) != 0);
    assert(llist_insert_after(list, (void *) &inserted_data[4], sizeof(size_t), 9) != 0);
    assert(llist_insert_after(list, (void *) &inserted_data[4], sizeof(size_t), 156) != 0);

    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == data[2] );
    assert(*((size_t *) llist_get(list, 4)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );
    assert(*((size_t *) llist_get(list, 7)) == inserted_data[3] );
    assert(*((size_t *) llist_get(list, 8)) == inserted_data[2] );

    llist_destroy(list);

    // using insert_after on an empty list
    llist_t *empty_list = llist_new();
    assert(llist_insert_after(empty_list, (void *) &inserted_data[4], sizeof(size_t), 0) != 0);

    llist_destroy(empty_list);

    assert(llist_insert_after(NULL, (void *) &inserted_data[0], sizeof(size_t), 2) == 99);

    printf("OK\n");
    return 0;
}

static int test_llist_insert_before(void)
{
    printf("%-40s", "test_llist_insert_before ");

    llist_t *list = llist_new();

    size_t data[5] = {10, 36, 74, 8, 6};

    for (size_t i = 0; i < 5; ++i) {
        llist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_list(list, 5);

    size_t inserted_data[5] = {11, 22, 33, 44, 55};

    assert(llist_insert_before(list, (void *) &inserted_data[0], sizeof(size_t), 2) == 0);
    
    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == data[3] );
    assert(*((size_t *) llist_get(list, 2)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 3)) == data[2] );
    assert(*((size_t *) llist_get(list, 4)) == data[1] );
    assert(*((size_t *) llist_get(list, 5)) == data[0] );

    //print_sizet_list(list, 6);

    assert(llist_insert_before(list, (void *) &inserted_data[1], sizeof(size_t), 0) == 0);

    assert(*((size_t *) llist_get(list, 0)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 1)) == data[4] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 4)) == data[2] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );

    //print_sizet_list(list, 7);

    assert(llist_insert_before(list, (void *) &inserted_data[2], sizeof(size_t), 7) == 0);

    assert(*((size_t *) llist_get(list, 0)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 1)) == data[4] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 4)) == data[2] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );
    assert(*((size_t *) llist_get(list, 7)) == inserted_data[2] );

    //print_sizet_list(list, 8);

    assert(llist_insert_before(list, (void *) &inserted_data[4], sizeof(size_t), 9) != 0);
    assert(llist_insert_before(list, (void *) &inserted_data[4], sizeof(size_t), 1847) != 0);
    assert(llist_insert_before(list, (void *) &inserted_data[4], sizeof(size_t), -1) != 0);
    assert(llist_insert_before(list, (void *) &inserted_data[4], sizeof(size_t), -53) != 0);

    assert(*((size_t *) llist_get(list, 0)) == inserted_data[1] );
    assert(*((size_t *) llist_get(list, 1)) == data[4] );
    assert(*((size_t *) llist_get(list, 2)) == data[3] );
    assert(*((size_t *) llist_get(list, 3)) == inserted_data[0] );
    assert(*((size_t *) llist_get(list, 4)) == data[2] );
    assert(*((size_t *) llist_get(list, 5)) == data[1] );
    assert(*((size_t *) llist_get(list, 6)) == data[0] );
    assert(*((size_t *) llist_get(list, 7)) == inserted_data[2] );

    llist_destroy(list);

    // using insert_before on an empty list
    llist_t *empty_list = llist_new();
    assert(llist_insert_before(empty_list, (void *) &inserted_data[4], sizeof(size_t), 0) == 0);

    assert(*((size_t *) llist_get(empty_list, 0)) == inserted_data[4] );

    llist_destroy(empty_list);

    assert(llist_insert_before(NULL, (void *) &inserted_data[0], sizeof(size_t), 2) == 99);

    printf("OK\n");
    return 0;
}

static int test_llist_len(void)
{
    printf("%-40s", "test_llist_len ");

    llist_t *list = llist_new();

    size_t data[] = {10, 36, 74, 8, 6, 762, 1, 98, 0, 27364, 9273682, 29219};

    for (size_t i = 0; i < 12; ++i) {
        llist_push_first(list, (void *) &data[i], sizeof(size_t));
        assert(llist_len(list) == i + 1);
    }

    llist_destroy(list);

    // calculating length of an empty list
    llist_t *empty_list = llist_new();
    assert(llist_len(empty_list) == 0);
    llist_destroy(empty_list);

    // non-existent list has a length of 0
    assert(llist_len(NULL) == 0);

    printf("OK\n");
    return 0;
}

static int test_llist_remove_after_node(void)
{
    printf("%-40s", "test_llist_remove_after_node ");

    llist_t *list = llist_new();

    size_t data[] = {1, 2, 3, 4, 5};

    for (size_t i = 0; i < 5; ++i) {
        llist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_list(list, 5);

    // remove item from the middle
    assert(llist_remove_after_node(list, list->head->next) == 0);
    assert(llist_len(list) == 4);
    //print_sizet_list(list, 4);
    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == data[3] );
    assert(*((size_t *) llist_get(list, 2)) == data[1] );
    assert(*((size_t *) llist_get(list, 3)) == data[0] );

    // remove item from the beginning
    assert(llist_remove_after_node(list, NULL) == 0);
    assert(llist_len(list) == 3);
    //print_sizet_list(list, 3);
    assert(*((size_t *) llist_get(list, 0)) == data[3] );
    assert(*((size_t *) llist_get(list, 1)) == data[1] );
    assert(*((size_t *) llist_get(list, 2)) == data[0] );

    // remove item from the end
    assert(llist_remove_after_node(list, list->head->next) == 0);
    assert(llist_len(list) == 2);
    //print_sizet_list(list, 2);
    assert(*((size_t *) llist_get(list, 0)) == data[3] );
    assert(*((size_t *) llist_get(list, 1)) == data[1] );

    // try removing non-existent node
    assert(llist_remove_after_node(list, list->head->next) != 0);
    assert(llist_len(list) == 2);
    //print_sizet_list(list, 2);
    assert(*((size_t *) llist_get(list, 0)) == data[3] );
    assert(*((size_t *) llist_get(list, 1)) == data[1] );

    assert(llist_remove_after_node(NULL, list->head) == 99);

    llist_destroy(list);

    // removing node from an empty linked list
    llist_t *empty_list = llist_new();
    assert(llist_remove_after_node(empty_list, NULL) != 0);
    llist_destroy(empty_list);
    

    printf("OK\n");
    return 0;
}

static int test_llist_remove(void)
{
    printf("%-40s", "test_llist_remove ");

    llist_t *list = llist_new();

    size_t data[] = {1, 2, 3, 4, 5};

    for (size_t i = 0; i < 5; ++i) {
        llist_push_first(list, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_list(list, 5);

    // remove item from the middle
    assert(llist_remove(list, 2) == 0);
    assert(llist_len(list) == 4);
    //print_sizet_list(list, 4);
    assert(*((size_t *) llist_get(list, 0)) == data[4] );
    assert(*((size_t *) llist_get(list, 1)) == data[3] );
    assert(*((size_t *) llist_get(list, 2)) == data[1] );
    assert(*((size_t *) llist_get(list, 3)) == data[0] );

    // remove item from the beginning
    assert(llist_remove(list, 0) == 0);
    assert(llist_len(list) == 3);
    //print_sizet_list(list, 3);
    assert(*((size_t *) llist_get(list, 0)) == data[3] );
    assert(*((size_t *) llist_get(list, 1)) == data[1] );
    assert(*((size_t *) llist_get(list, 2)) == data[0] );

    // remove item from the end
    assert(llist_remove(list, 2) == 0);
    assert(llist_len(list) == 2);
    //print_sizet_list(list, 2);
    assert(*((size_t *) llist_get(list, 0)) == data[3] );
    assert(*((size_t *) llist_get(list, 1)) == data[1] );

    // try removing non-existent node
    assert(llist_remove(list, 3) != 0);
    assert(llist_remove(list, -1) != 0);
    assert(llist_remove(list, 56) != 0);
    assert(llist_remove(list, -128) != 0);
    assert(llist_len(list) == 2);
    //print_sizet_list(list, 2);
    assert(*((size_t *) llist_get(list, 0)) == data[3] );
    assert(*((size_t *) llist_get(list, 1)) == data[1] );

    llist_destroy(list);

    // removing node from an empty linked list
    llist_t *empty_list = llist_new();
    assert(llist_remove(empty_list, 0) != 0);
    assert(llist_remove(empty_list, -1) != 0);
    assert(llist_remove(empty_list, 1) != 0);
    llist_destroy(empty_list);

    assert(llist_remove(NULL, 2) == 99);

    printf("OK\n");
    return 0;
}


int main(void) 
{
    test_llist_destroy_null();

    test_llist_new();
    test_llist_push_first();
    test_llist_push_last();
    test_llist_push_first_last();
    test_llist_get();

    test_llist_insert_after_node();
    test_llist_insert_after();
    test_llist_insert_before();
    
    test_llist_len();

    test_llist_remove_after_node();
    test_llist_remove();

    return 0;
}