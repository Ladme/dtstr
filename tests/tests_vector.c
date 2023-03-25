// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/vector.h"

static int test_vec_destroy_null(void)
{
    printf("%-40s", "test_vec_destroy (null) ");

    vec_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_vec_new(void) 
{
    printf("%-40s", "test_vec_new ");

    vec_t *vector = vec_new();

    assert(vector);
    assert(vector->len == 0);
    assert(vector->capacity == VEC_INITIAL_CAPACITY);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_push(void)
{
    printf("%-40s", "test_vec_push ");

    vec_t *vector = vec_new();

    int test_array[130] = {0};
    for (size_t i = 0; i < 130; ++i) {
        test_array[i] = i;
        assert(vec_push(vector, &test_array[i], sizeof(int)) == 0);
    }

    for (size_t i = 0; i < 130; ++i) {
        int item = *(int *) vec_get(vector, i);
        assert(item == test_array[i]);
    }

    assert(vector->len == 130);
    assert(vector->capacity == 256);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_get(void)
{
    printf("%-40s", "test_vec_get ");

    vec_t *vector = vec_new();

    char test1[] = "test1";
    char test2[] = "test2";

    vec_push(vector, (void *) test1, 6);
    vec_push(vector, (void *) test2, 6);

    void *item = vec_get(vector, 0);
    assert(strcmp((char *) item, test1) == 0);

    item = vec_get(vector, 1);
    assert(strcmp((char *) item, test2) == 0);

    assert(vec_get(vector, 2) == NULL);
    assert(vec_get(vector, -1) == NULL);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_get_invalid(void)
{
    printf("%-40s", "test_vec_get (invalid vector) ");

    assert(vec_get(NULL, 0) == NULL);

    printf("OK\n");
    return 0;
}

static int test_vec_insert(void)
{
    printf("%-40s", "test_vec_insert ");

    vec_t *vector = vec_new();

    int data[5] = {1, 2, 3, 4, 5};

    for (size_t i = 0; i < 5; ++i) {
        vec_push(vector, &data[i], sizeof(int));
    }

    int inserted[5] = {10, 20, 30, 40, 50};
    
    // inserting to the end of the vector
    assert(vec_insert(vector, &inserted[0], sizeof(int), 5) == 0);
    assert(vector->len == 6);
    assert(*((int *) vec_get(vector, 0)) == 1);
    assert(*((int *) vec_get(vector, 1)) == 2);
    assert(*((int *) vec_get(vector, 2)) == 3);
    assert(*((int *) vec_get(vector, 3)) == 4);
    assert(*((int *) vec_get(vector, 4)) == 5);
    assert(*((int *) vec_get(vector, 5)) == 10);

    // inserting to the start of the vector
    assert(vec_insert(vector, &inserted[1], sizeof(int), 0) == 0);
    assert(vector->len == 7);
    assert(*((int *) vec_get(vector, 0)) == 20);
    assert(*((int *) vec_get(vector, 1)) == 1);
    assert(*((int *) vec_get(vector, 2)) == 2);
    assert(*((int *) vec_get(vector, 3)) == 3);
    assert(*((int *) vec_get(vector, 4)) == 4);
    assert(*((int *) vec_get(vector, 5)) == 5);
    assert(*((int *) vec_get(vector, 6)) == 10);

    // inserting multiple elements the middle of the vector
    assert(vec_insert(vector, &inserted[2], sizeof(int), 1) == 0);
    assert(vec_insert(vector, &inserted[3], sizeof(int), 1) == 0);
    assert(vec_insert(vector, &inserted[4], sizeof(int), 6) == 0);
    assert(vector->len == 10);
    assert(*((int *) vec_get(vector, 0)) == 20);
    assert(*((int *) vec_get(vector, 1)) == 40);
    assert(*((int *) vec_get(vector, 2)) == 30);
    assert(*((int *) vec_get(vector, 3)) == 1);
    assert(*((int *) vec_get(vector, 4)) == 2);
    assert(*((int *) vec_get(vector, 5)) == 3);
    assert(*((int *) vec_get(vector, 6)) == 50);
    assert(*((int *) vec_get(vector, 7)) == 4);
    assert(*((int *) vec_get(vector, 8)) == 5);
    assert(*((int *) vec_get(vector, 9)) == 10);

    // inserting elements out of bounds
    assert(vec_insert(vector, &inserted[0], sizeof(int), -1) == 2);
    assert(vec_insert(vector, &inserted[0], sizeof(int), 12) == 2);
    assert(vec_insert(vector, &inserted[0], sizeof(int), -54) == 2);
    assert(vec_insert(vector, &inserted[0], sizeof(int), 1082) == 2);
    assert(vector->len == 10);
    assert(*((int *) vec_get(vector, 0)) == 20);
    assert(*((int *) vec_get(vector, 1)) == 40);
    assert(*((int *) vec_get(vector, 2)) == 30);
    assert(*((int *) vec_get(vector, 3)) == 1);
    assert(*((int *) vec_get(vector, 4)) == 2);
    assert(*((int *) vec_get(vector, 5)) == 3);
    assert(*((int *) vec_get(vector, 6)) == 50);
    assert(*((int *) vec_get(vector, 7)) == 4);
    assert(*((int *) vec_get(vector, 8)) == 5);
    assert(*((int *) vec_get(vector, 9)) == 10);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_insert_large(void)
{
    printf("%-40s", "test_vec_insert (large) ");

    vec_t *vector = vec_new();

    for (size_t i = 0; i < 1000; ++i) {
        int item = i;
        assert(vec_insert(vector, &item, sizeof(int), 0) == 0);
    }

    assert(vector->len == 1000);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_insert_single_gigantic(void)
{
    printf("%-40s", "test_vec_insert (single insertion, gigantic vector) ");

    vec_t *vector = vec_new();

    for (size_t i = 0; i < 100000; ++i) {
        int item = i;
        vec_push(vector, &item, sizeof(int));
    }

    int different_value = -1;
    assert(vec_insert(vector, &different_value, sizeof(int), 76500) == 0);

    for (size_t i = 0; i < 100001; ++i) {
        if (i == 76500) assert(*((int *) vec_get(vector, i)) == -1);
        else if (i < 76500) assert(*((int *) vec_get(vector, i)) == i);
        else assert(*((int *) vec_get(vector, i)) == i - 1);
    }

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_insert_invalid(void)
{
    printf("%-40s", "test_vec_insert (invalid vector) ");

    int value = 10;

    assert(vec_insert(NULL, &value, sizeof(int), 0) == 99);

    printf("OK\n");
    return 0;
}

/*! @brief This function tests whether the values pushed or inserted into vector will live on after the original memory is deallocated. */
static int test_vec_push_insert_deallocated(void)
{
    printf("%-40s", "test_vec_push/insert (deallocated) ");

    vec_t *vector = vec_new();
    for (size_t i = 0; i < 1000; ++i) {
        int value = 5;
        assert(vec_push(vector, &value, sizeof(int)) == 0);
    }

    for (size_t i = 0; i < 20; ++i) {
        int value = 9;
        assert(vec_insert(vector, &value, sizeof(int), i) == 0);
    }

    assert(vector->len == 1020);

    for (size_t i = 0; i < vector->len; ++i) {

        int value = *((int *) vec_get(vector, i));
        assert(value == 5 || value == 9);
    }

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_push_invalid(void)
{
    printf("%-40s", "test_vec_push (invalid vector) ");

    char test1[] = "test1";

    assert(vec_push(NULL, test1, sizeof(int)) == 99);

    printf("OK\n");
    return 0;
}

static int test_vec_pop(void)
{
    printf("%-40s", "test_vec_pop ");

    vec_t *vector = vec_new();

    int test_array[130] = {0};
    for (size_t i = 0; i < 130; ++i) {
        test_array[i] = i;
        assert(!vec_push(vector, &test_array[i], sizeof(int)));
    }

    assert(vector->len == 130);

    for (int i = 129; i >= 0; --i) {
        void *popped = vec_pop(vector);
        assert(popped);
        assert(*(int *) popped == i);

        free(popped);

        if (vector->len == 64) assert(vector->capacity == 128);
        else if (vector->len == 32) assert(vector->capacity == 64);
        else if (vector->len == 16) assert(vector->capacity == 32);
        else if (vector->len == 8) assert(vector->capacity == 16);
    }

    assert(vector->len == 0);
    assert(vector->capacity == VEC_INITIAL_CAPACITY);

    vec_destroy(vector);
    
    printf("OK\n");
    return 0;
}

static int test_vec_pop_invalid(void)
{
    printf("%-40s", "test_vec_pop (invalid vector) ");

    vec_t *vector = vec_new();

    assert(!vec_pop(vector));
    assert(!vec_pop(NULL));

    vec_destroy(vector);
    
    printf("OK\n");
    return 0;
}

static int test_vec_pop_and_push(void)
{
    printf("%-40s", "test_vec_pop_and_push ");

    vec_t *vector = vec_new();

    int test_array[130] = {0};
    for (size_t i = 0; i < 130; ++i) {
        test_array[i] = i;
        assert(!vec_push(vector, &test_array[i], sizeof(int)));
    }

    assert(vector->len == 130);

    for (int i = 129; i >= 0; --i) {
        void *popped = vec_pop(vector);
        assert(popped);
        assert(*(int *) popped == i);

        free(popped);

        if (vector->len == 64) assert(vector->capacity == 128);
        else if (vector->len == 32) assert(vector->capacity == 64);
        else if (vector->len == 16) assert(vector->capacity == 32);
        else if (vector->len == 8) assert(vector->capacity == 16);
    }

    assert(vector->len == 0);
    assert(vector->capacity == VEC_INITIAL_CAPACITY);

    for (size_t i = 0; i < 130; ++i) {
        assert(!vec_push(vector, &test_array[i], sizeof(int)));
    }

    for (size_t i = 0; i < 130; ++i) {
        void *item =  vec_get(vector, i);
        assert(*((int *) item) == test_array[i]);
    }

    assert(vector->len == 130);
    assert(vector->capacity == 256);

    vec_destroy(vector);
    
    printf("OK\n");
    return 0;
}

static int test_vec_remove(void)
{
    printf("%-40s", "test_vec_remove ");

    vec_t *vector = vec_new();

    int test_array[130] = {0};
    for (size_t i = 0; i < 130; ++i) {
        test_array[i] = i;
        assert(!vec_push(vector, &test_array[i], sizeof(int)));
    }

    assert(vector->len == 130);

    // remove sixth element of the array
    void *removed = vec_remove(vector, 5);
    assert(removed);
    assert(*(int *) removed == 5);
    assert(vector->len == 129);

    free(removed);

    for (size_t i = 5; i < 129; ++i) {
        void *item = vec_get(vector, i);
        assert(item);
        assert(*(int *) item == i + 1);
    }

    // remove the first element of the array
    removed = vec_remove(vector, 0);
    assert(removed);
    assert(*(int *) removed == 0);
    assert(vector->len == 128);

    free(removed);

    for (size_t i = 0; i < 4; ++i) {
        void *item = vec_get(vector, i);
        assert(item);
        assert(*(int *) item == i + 1);
    }

    for (size_t i = 5; i < 128; ++i) {
        void *item = vec_get(vector, i);
        assert(item);
        assert(*(int *) item == i + 2);
    }

    // remove the last element of the array
    removed = vec_remove(vector, 127);
    assert(removed);
    assert(*(int *) removed = 129);
    assert(vector->len == 127);
    free(removed);

    for (size_t i = 0; i < 4; ++i) {
        void *item = vec_get(vector, i);
        assert(item);
        assert(*(int *) item == i + 1);
    }

    for (size_t i = 5; i < 127; ++i) {
        void *item = vec_get(vector, i);
        assert(item);
        assert(*(int *) item == i + 2);
    }

    vec_destroy(vector);
    
    printf("OK\n");
    return 0;
}

static int test_vec_remove_all(void)
{
    printf("%-40s", "test_vec_remove (all) ");

    vec_t *vector = vec_new();

    int test_array[130] = {0};
    for (size_t i = 0; i < 130; ++i) {
        test_array[i] = i;
        assert(!vec_push(vector, &test_array[i], sizeof(int)));
    }

    assert(vector->len == 130);

    for (size_t i = 0; i < 130; ++i) {
        void *removed = vec_remove(vector, 0);
        assert(removed);
        assert(*(int *) removed == i);
        free(removed);

        if (vector->len == 64) assert(vector->capacity == 128);
        else if (vector->len == 32) assert(vector->capacity == 64);
        else if (vector->len == 16) assert(vector->capacity == 32);
        else if (vector->len == 8) assert(vector->capacity == 16);
    }

    vec_destroy(vector);
    
    printf("OK\n");
    return 0;
}



int main(void) 
{
    test_vec_destroy_null();
    test_vec_new();
    test_vec_push();
    test_vec_push_invalid();

    test_vec_get();
    test_vec_get_invalid();

    test_vec_insert();
    test_vec_insert_large();
    test_vec_insert_single_gigantic();
    test_vec_insert_invalid();

    test_vec_push_insert_deallocated();

    test_vec_pop();
    test_vec_pop_invalid();
    test_vec_pop_and_push();
    test_vec_remove();
    test_vec_remove_all();

    return 0;
}