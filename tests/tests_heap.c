// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/heap.h"
#include "../src/vector.h"

#define UNUSED(x) (void)(x)

inline static void print_int_heap(heap_t *heap)
{
    for (size_t i = 0; i < heap->len; ++i) {
        printf("%d ", *(int *) heap->items[i]);
    }
    printf("\n");
}

static int min_heap_compare_ints(const void *x, const void *y)
{
    return (*(int *) x - *(int *) y);
}

static int max_heap_compare_ints(const void *x, const void *y)
{
    return (*(int *) y - *(int *) x);
}

static void assert_heap_balance(const heap_t *heap)
{
    // check that all children of all nodes are of correct size
    for (size_t j = 0; j < heap->len; ++j) {
        size_t left = (2 * j) + 1;
        size_t right = (2 * j) + 2;

        if (left < heap->len) assert(heap->compare_function(heap->items[left], heap->items[j]) >= 0);
        if (right < heap->len) assert(heap->compare_function(heap->items[right], heap->items[j]) >= 0);
    }
}

static int test_heap_destroy_null(void)
{
    printf("%-40s", "test_heap_destroy (null) ");

    heap_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_heap_new(void) 
{
    printf("%-40s", "test_heap_new ");

    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);

    assert(heap);
    assert(heap->len == 0);
    assert(heap->capacity == HEAP_DEFAULT_CAPACITY);
    assert(heap->base_capacity == HEAP_DEFAULT_CAPACITY);

    heap_destroy(heap);

    printf("OK\n");
    return 0;
}

static int test_heap_insert_min(void)
{
    printf("%-40s", "test_heap_insert (min) ");

    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);

    int data[] = {7, 5, 6, 8, 5, 2, 3, 2, 0, 9};

    heap_insert(heap, &data[0]);
    assert(*(int *) heap->items[0] == 7);
    heap_insert(heap, &data[1]);
    assert(*(int *) heap->items[0] == 5);
    assert(*(int *) heap->items[1] == 7);
    heap_insert(heap, &data[2]);
    assert(*(int *) heap->items[0] == 5);
    assert(*(int *) heap->items[1] == 7);
    assert(*(int *) heap->items[2] == 6);
    heap_insert(heap, &data[3]);
    assert(*(int *) heap->items[0] == 5);
    assert(*(int *) heap->items[1] == 7);
    assert(*(int *) heap->items[2] == 6);
    assert(*(int *) heap->items[3] == 8);
    heap_insert(heap, &data[4]);
    assert(*(int *) heap->items[0] == 5);
    assert(*(int *) heap->items[1] == 5);
    assert(*(int *) heap->items[2] == 6);
    assert(*(int *) heap->items[3] == 8);
    assert(*(int *) heap->items[4] == 7);
    heap_insert(heap, &data[5]);
    assert(*(int *) heap->items[0] == 2);
    assert(*(int *) heap->items[1] == 5);
    assert(*(int *) heap->items[2] == 5);
    assert(*(int *) heap->items[3] == 8);
    assert(*(int *) heap->items[4] == 7);
    assert(*(int *) heap->items[5] == 6);
    heap_insert(heap, &data[6]);
    assert(*(int *) heap->items[0] == 2);
    assert(*(int *) heap->items[1] == 5);
    assert(*(int *) heap->items[2] == 3);
    assert(*(int *) heap->items[3] == 8);
    assert(*(int *) heap->items[4] == 7);
    assert(*(int *) heap->items[5] == 6);
    assert(*(int *) heap->items[6] == 5);
    heap_insert(heap, &data[7]);
    assert(*(int *) heap->items[0] == 2);
    assert(*(int *) heap->items[1] == 2);
    assert(*(int *) heap->items[2] == 3);
    assert(*(int *) heap->items[3] == 5);
    assert(*(int *) heap->items[4] == 7);
    assert(*(int *) heap->items[5] == 6);
    assert(*(int *) heap->items[6] == 5);
    assert(*(int *) heap->items[7] == 8);
    heap_insert(heap, &data[8]);
    assert(*(int *) heap->items[0] == 0);
    assert(*(int *) heap->items[1] == 2);
    assert(*(int *) heap->items[2] == 3);
    assert(*(int *) heap->items[3] == 2);
    assert(*(int *) heap->items[4] == 7);
    assert(*(int *) heap->items[5] == 6);
    assert(*(int *) heap->items[6] == 5);
    assert(*(int *) heap->items[7] == 8);
    assert(*(int *) heap->items[8] == 5);
    heap_insert(heap, &data[9]);
    assert(*(int *) heap->items[0] == 0);
    assert(*(int *) heap->items[1] == 2);
    assert(*(int *) heap->items[2] == 3);
    assert(*(int *) heap->items[3] == 2);
    assert(*(int *) heap->items[4] == 7);
    assert(*(int *) heap->items[5] == 6);
    assert(*(int *) heap->items[6] == 5);
    assert(*(int *) heap->items[7] == 8);
    assert(*(int *) heap->items[8] == 5);
    assert(*(int *) heap->items[9] == 9);

    heap_destroy(heap);

    printf("OK\n");
    return 0;
}

static int test_heap_insert_max(void)
{
    printf("%-40s", "test_heap_insert (max) ");

    heap_t *heap = heap_new(sizeof(int), max_heap_compare_ints);

    int data[] = {7, 5, 8, 6, 5, 9, 3, 6, 10, 0};

    heap_insert(heap, &data[0]);
    assert(*(int *) heap->items[0] == 7);
    heap_insert(heap, &data[1]);
    assert(*(int *) heap->items[0] == 7);
    assert(*(int *) heap->items[1] == 5);
    heap_insert(heap, &data[2]);
    assert(*(int *) heap->items[0] == 8);
    assert(*(int *) heap->items[1] == 5);
    assert(*(int *) heap->items[2] == 7);
    heap_insert(heap, &data[3]);
    assert(*(int *) heap->items[0] == 8);
    assert(*(int *) heap->items[1] == 6);
    assert(*(int *) heap->items[2] == 7);
    assert(*(int *) heap->items[3] == 5);
    heap_insert(heap, &data[4]);
    assert(*(int *) heap->items[0] == 8);
    assert(*(int *) heap->items[1] == 6);
    assert(*(int *) heap->items[2] == 7);
    assert(*(int *) heap->items[3] == 5);
    assert(*(int *) heap->items[4] == 5);
    heap_insert(heap, &data[5]);
    assert(*(int *) heap->items[0] == 9);
    assert(*(int *) heap->items[1] == 6);
    assert(*(int *) heap->items[2] == 8);
    assert(*(int *) heap->items[3] == 5);
    assert(*(int *) heap->items[4] == 5);
    assert(*(int *) heap->items[5] == 7);
    heap_insert(heap, &data[6]);
    assert(*(int *) heap->items[0] == 9);
    assert(*(int *) heap->items[1] == 6);
    assert(*(int *) heap->items[2] == 8);
    assert(*(int *) heap->items[3] == 5);
    assert(*(int *) heap->items[4] == 5);
    assert(*(int *) heap->items[5] == 7);
    assert(*(int *) heap->items[6] == 3);
    heap_insert(heap, &data[7]);
    assert(*(int *) heap->items[0] == 9);
    assert(*(int *) heap->items[1] == 6);
    assert(*(int *) heap->items[2] == 8);
    assert(*(int *) heap->items[3] == 6);
    assert(*(int *) heap->items[4] == 5);
    assert(*(int *) heap->items[5] == 7);
    assert(*(int *) heap->items[6] == 3);
    assert(*(int *) heap->items[7] == 5);
    heap_insert(heap, &data[8]);
    assert(*(int *) heap->items[0] == 10);
    assert(*(int *) heap->items[1] == 9);
    assert(*(int *) heap->items[2] == 8);
    assert(*(int *) heap->items[3] == 6);
    assert(*(int *) heap->items[4] == 5);
    assert(*(int *) heap->items[5] == 7);
    assert(*(int *) heap->items[6] == 3);
    assert(*(int *) heap->items[7] == 5);
    assert(*(int *) heap->items[8] == 6);
    heap_insert(heap, &data[9]);
    assert(*(int *) heap->items[0] == 10);
    assert(*(int *) heap->items[1] == 9);
    assert(*(int *) heap->items[2] == 8);
    assert(*(int *) heap->items[3] == 6);
    assert(*(int *) heap->items[4] == 5);
    assert(*(int *) heap->items[5] == 7);
    assert(*(int *) heap->items[6] == 3);
    assert(*(int *) heap->items[7] == 5);
    assert(*(int *) heap->items[8] == 6);
    assert(*(int *) heap->items[9] == 0);

    heap_destroy(heap);

    printf("OK\n");
    return 0;
}

static int test_heap_insert_min_large(void)
{
    printf("%-40s", "test_heap_insert (min, large) ");

    srand(99884673);

    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);

    for (size_t i = 0; i < 1000; ++i) {
        int r_int = rand() % 1000; 
        
        heap_insert(heap, &r_int);
        
        assert_heap_balance(heap);
    }

    assert(heap->capacity == 1024);

    //print_int_heap(heap);

    heap_destroy(heap);

    printf("OK\n");
    return 0;
}

static int test_heap_insert_max_large(void)
{
    printf("%-40s", "test_heap_insert (max, large) ");

    srand(1084386746);

    heap_t *heap = heap_new(sizeof(int), max_heap_compare_ints);

    for (size_t i = 0; i < 1000; ++i) {
        int r_int = rand() % 1000; 
        
        heap_insert(heap, &r_int);
        
        assert_heap_balance(heap);
    }

    assert(heap->capacity == 1024);

    //print_int_heap(heap);

    heap_destroy(heap);

    printf("OK\n");
    return 0;
}


static int test_heap_len(void)
{
    printf("%-40s", "test_heap_len ");

    assert(heap_len(NULL) == 0);

    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);

    assert(heap_len(heap) == 0);

    for (size_t i = 0; i < 130; ++i) {

        assert(heap_insert(heap, &i) == 0);
        assert(heap_len(heap) == i + 1);
    }

    heap_destroy(heap);

    printf("OK\n");
    return 0;
}


static int test_heap_peek_min(void)
{
    printf("%-40s", "test_heap_peek (min) ");

    srand(9374686);

    assert(heap_peek(NULL) == NULL);

    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);
    vec_t *vector = vec_new();

    assert(heap_peek(heap) == NULL);

    for (size_t i = 0; i < 1000; ++i) {
        int r_int = rand() % 1000; 
        
        heap_insert(heap, &r_int);
        vec_push(vector, &r_int, sizeof(int));
    }

    assert(*(int *) heap_peek(heap) == *(int *) vec_find_min(vector, min_heap_compare_ints));
    heap_destroy(heap);
    vec_destroy(vector);

    printf("OK\n");
    return 0;
}


static int test_heap_peek_max(void)
{
    printf("%-40s", "test_heap_peek (man) ");

    srand(9374686);

    assert(heap_peek(NULL) == NULL);

    heap_t *heap = heap_new(sizeof(int), max_heap_compare_ints);
    vec_t *vector = vec_new();

    assert(heap_peek(heap) == NULL);

    for (size_t i = 0; i < 1000; ++i) {
        int r_int = rand() % 1000; 
        
        heap_insert(heap, &r_int);
        vec_push(vector, &r_int, sizeof(int));
    }

    assert(*(int *) heap_peek(heap) == *(int *) vec_find_max(vector, min_heap_compare_ints));
    heap_destroy(heap);
    vec_destroy(vector);

    printf("OK\n");
    return 0;
}


static int test_heap_pop_min(void)
{
    printf("%-40s", "test_heap_pop (min) ");

    assert(heap_pop(NULL) == NULL);

    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);

    assert(heap_pop(heap) == NULL);

    int data[] = {7, 5, 6, 8, 5, 2, 3, 2, 0, 9};

    for (size_t i = 0; i < 10; ++i) {
        heap_insert(heap, &data[i]);
    }

    assert_heap_balance(heap);

    void *item = heap_pop(heap);
    assert(*(int *) item == 0);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 2);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 2);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 3);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 5);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 5);
    assert_heap_balance(heap);
    free(item);
    
    item = heap_pop(heap);
    assert(*(int *) item == 6);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 7);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 8);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 9);
    assert_heap_balance(heap);
    free(item);

    heap_destroy(heap);

    printf("OK\n");
    return 0;
}


static int test_heap_pop_max(void)
{
    printf("%-40s", "test_heap_pop (max) ");

    assert(heap_pop(NULL) == NULL);

    heap_t *heap = heap_new(sizeof(int), max_heap_compare_ints);

    assert(heap_pop(heap) == NULL);

    int data[] = {7, 5, 8, 6, 5, 9, 3, 6, 10, 0};

    for (size_t i = 0; i < 10; ++i) {
        heap_insert(heap, &data[i]);
    }

    assert_heap_balance(heap);

    void *item = heap_pop(heap);
    assert(*(int *) item == 10);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 9);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 8);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 7);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 6);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 6);
    assert_heap_balance(heap);
    free(item);
    
    item = heap_pop(heap);
    assert(*(int *) item == 5);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 5);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 3);
    assert_heap_balance(heap);
    free(item);

    item = heap_pop(heap);
    assert(*(int *) item == 0);
    assert_heap_balance(heap);
    free(item);

    heap_destroy(heap);

    printf("OK\n");
    return 0;
}

static int test_heap_pop_min_large(void)
{
    printf("%-40s", "test_heap_pop (min, large) ");

    srand(2478340);

    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);
    vec_t *vector = vec_new();

    for (size_t i = 0; i < 1000; ++i) {
        int r_int = rand() % 1000; 
        
        heap_insert(heap, &r_int);
        vec_push(vector, &r_int, sizeof(int));
    }

    assert(heap->capacity == 1024);

    vec_sort_quicknaive(vector, min_heap_compare_ints);

    for (size_t i = 0; i < 1000; ++i) {
        void *item = heap_pop(heap);
        assert(*(int *) item == *(int *) vec_get(vector, i));
        free(item);
        assert_heap_balance(heap);
    }

    assert(heap->capacity == HEAP_DEFAULT_CAPACITY);

    heap_destroy(heap);
    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_heap_pop_max_large(void)
{
    printf("%-40s", "test_heap_pop (max, large) ");

    srand(2478340);

    heap_t *heap = heap_new(sizeof(int), max_heap_compare_ints);
    vec_t *vector = vec_new();

    for (size_t i = 0; i < 1000; ++i) {
        int r_int = rand() % 1000; 
        
        heap_insert(heap, &r_int);
        vec_push(vector, &r_int, sizeof(int));
    }

    assert(heap->capacity == 1024);

    vec_sort_quicknaive(vector, max_heap_compare_ints);

    for (size_t i = 0; i < 1000; ++i) {
        void *item = heap_pop(heap);
        assert(*(int *) item == *(int *) vec_get(vector, i));
        free(item);
        assert_heap_balance(heap);
    }

    assert(heap->capacity == HEAP_DEFAULT_CAPACITY);

    heap_destroy(heap);
    vec_destroy(vector);

    printf("OK\n");
    return 0;
}


static void set_to_three(void *item, void *unused)
{
    UNUSED(unused);
    int *ptr = (int *) item;
    *ptr = 3;
}

static int test_heap_map(void)
{
    printf("%-40s", "test_heap_map ");

    srand(8273846);

    heap_map(NULL, set_to_three, NULL);

    heap_t *heap = heap_new(sizeof(int), min_heap_compare_ints);

    for (size_t i = 0; i < 1000; ++i) {
        int r_int = rand() % 1000; 
        
        heap_insert(heap, &r_int);
    }

    heap_map(heap, set_to_three, NULL);
    for (size_t i = 0; i < 1000; ++i) {
        assert(*(int *) heap->items[i] == 3);
    }
    
    heap_destroy(heap);

    printf("OK\n");
    return 0;
}

//TODO: preallocated tests

int main(void) 
{
    test_heap_destroy_null();
    test_heap_new();
    test_heap_insert_min();
    test_heap_insert_max();
    test_heap_insert_min_large();
    test_heap_insert_max_large();

    test_heap_len();

    test_heap_peek_min();
    test_heap_peek_max();
    test_heap_pop_min();
    test_heap_pop_max();
    test_heap_pop_min_large();
    test_heap_pop_max_large();


    test_heap_map();

    return 0;
}