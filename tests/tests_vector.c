
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

static int test_vec_get(void)
{
    printf("%-40s", "test_vec_get ");

    vec_t *vector = vec_new();

    char test1[] = "test1";
    char test2[] = "test2";

    vector->items[4] = (void *) test1;
    vector->items[8] = (void *) test2;
    vector->len = 9;

    char *item = (char *) vec_get(vector, 4);
    assert(strcmp(item, test1) == 0);

    item = (char *) vec_get(vector, 8);
    assert(strcmp(item, test2) == 0);

    item = (char *) vec_get(vector, 12);
    assert(!item);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_get_invalid(void)
{
    printf("%-40s", "test_vec_get (invalid vector) ");

    char *item = (char *) vec_get(NULL, 1);
    assert(!item);

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
        assert(!vec_push(vector, &test_array[i]));
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

static int test_vec_push_invalid(void)
{
    printf("%-40s", "test_vec_push (invalid vector) ");

    char test1[] = "test1";

    assert(vec_push(NULL, test1) == 99);

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
        assert(!vec_push(vector, &test_array[i]));
    }

    assert(vector->len == 130);

    for (int i = 129; i >= 0; --i) {
        void *popped = vec_pop(vector);
        assert(popped);
        assert(*(int *) popped == i);

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
        assert(!vec_push(vector, &test_array[i]));
    }

    assert(vector->len == 130);

    for (int i = 129; i >= 0; --i) {
        void *popped = vec_pop(vector);
        assert(popped);
        assert(*(int *) popped == i);

        if (vector->len == 64) assert(vector->capacity == 128);
        else if (vector->len == 32) assert(vector->capacity == 64);
        else if (vector->len == 16) assert(vector->capacity == 32);
        else if (vector->len == 8) assert(vector->capacity == 16);
    }

    assert(vector->len == 0);
    assert(vector->capacity == VEC_INITIAL_CAPACITY);

    for (size_t i = 0; i < 130; ++i) {
        assert(!vec_push(vector, &test_array[i]));
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

static int test_vec_remove(void)
{
    printf("%-40s", "test_vec_remove ");

    vec_t *vector = vec_new();

    int test_array[130] = {0};
    for (size_t i = 0; i < 130; ++i) {
        test_array[i] = i;
        assert(!vec_push(vector, &test_array[i]));
    }

    assert(vector->len == 130);

    // remove sixth element of the array
    void *removed = vec_remove(vector, 5);
    assert(removed);
    assert(*(int *) removed == 5);
    assert(vector->len == 129);

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
        assert(!vec_push(vector, &test_array[i]));
    }

    assert(vector->len == 130);

    for (size_t i = 0; i < 130; ++i) {
        void *removed = vec_remove(vector, 0);
        assert(removed);
        assert(*(int *) removed == i);

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
    test_vec_get();
    test_vec_get_invalid();
    test_vec_push();
    test_vec_push_invalid();
    test_vec_pop();
    test_vec_pop_invalid();
    test_vec_pop_and_push();
    test_vec_remove();
    test_vec_remove_all();

    return 0;
}