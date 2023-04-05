// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/vector.h"

#define UNUSED(x) (void)(x)

static void print_sizet(void *item, void *unused)
{
    UNUSED(unused);
    printf("%ld ", *(size_t *) item);
}

inline static void print_sizet_vec(vec_t *vector)
{
    vec_map(vector, print_sizet, NULL);
    printf("\n");
}

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
    assert(vector->capacity == VEC_DEFAULT_CAPACITY);
    assert(vector->base_capacity == VEC_DEFAULT_CAPACITY);

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
    assert(vector->capacity == vector->base_capacity);

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
    assert(vector->capacity == vector->base_capacity);

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

static int test_vec_push_pop_preallocated(void)
{
    printf("%-40s", "test_vec_push_pop (preallocated) ");

    vec_t *vector = vec_with_capacity(128);

    assert(vector->len == 0);
    assert(vector->capacity == 128);
    assert(vector->base_capacity == 128);

    for (size_t i = 0; i < 128; ++i) {
        assert(!vec_push(vector, &i, sizeof(size_t)));
    }

    assert(vector->len == 128);
    assert(vector->capacity == 128);
    assert(vector->base_capacity == 128);

    size_t additional = 129;
    assert(!vec_push(vector, &additional, sizeof(size_t)));

    assert(vector->len == 129);
    assert(vector->capacity == 256);
    assert(vector->base_capacity == 128);

    for (size_t i = 0; i < 129; ++i) {
        void *item = vec_pop(vector);
        assert(item);
        free(item);
    }

    assert(vector->len == 0);
    assert(vector->capacity == 128);
    assert(vector->base_capacity == 128);  

    vec_destroy(vector);
    
    printf("OK\n");
    return 0;
}

struct test_heterogeneous {
    long b;
    void *random_data;
};

static int test_vec_heterogeneous(void)
{
    printf("%-40s", "test_vec_heterogeneous ");

    vec_t *vector = vec_new();

    size_t item1 = 500384;
    int item2 = -9742;
    char item3 = 'w';
    char *item4 = "something";
    struct test_heterogeneous item5 = { .b = 872, .random_data = &item1};

    vec_push(vector, &item1, sizeof(vec_t *));
    vec_push(vector, &item2, sizeof(int));
    vec_push(vector, &item3, 1);
    vec_push(vector, item4, strlen(item4) + 1);
    vec_push(vector, &item5, sizeof(struct test_heterogeneous));

    void *item = vec_get(vector, 0);
    assert(*(size_t *) item == item1);

    item = vec_pop(vector);
    assert(((struct test_heterogeneous *) item)->b == item5.b);
    assert(((struct test_heterogeneous *) item)->random_data == &item1);
    free(item);

    item = vec_pop(vector);
    assert(strcmp((char *) item, item4) == 0);
    free(item);

    item = vec_pop(vector);
    assert(*(char *) item == item3);
    free(item);

    item = vec_pop(vector);
    assert(*(int *) item == item2);
    free(item);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}


static int test_filter_function(const void *data)
{
    return *((size_t *) data) >= 5;
}

static int test_vec_filter_mut(void)
{
    printf("%-40s", "test_vec_filter_mut ");

    // perform filter on non-existent vector
    assert(vec_filter_mut(NULL, test_filter_function) == 0);

    vec_t *vector = vec_new();

    // perform filter on empty vector
    assert(vec_filter_mut(vector, test_filter_function) == 0);

    size_t data[] = {9, 3, 2, 0, 5, 5, 4, 6, 3, 1};

    for (size_t i = 0; i < 10; ++i) {
        vec_push(vector, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_vec(vector);

    assert(vec_filter_mut(vector, test_filter_function) == 6);
    assert(vector->len == 4);
    //print_sizet_vec(vector);

    assert(*((size_t *) vec_get(vector, 0)) == 9);
    assert(*((size_t *) vec_get(vector, 1)) == 5);
    assert(*((size_t *) vec_get(vector, 2)) == 5);
    assert(*((size_t *) vec_get(vector, 3)) == 6);

    // applying the same filter again should do nothing
    assert(vec_filter_mut(vector, test_filter_function) == 0);
    assert(vector->len == 4);

    assert(*((size_t *) vec_get(vector, 0)) == 9);
    assert(*((size_t *) vec_get(vector, 1)) == 5);
    assert(*((size_t *) vec_get(vector, 2)) == 5);
    assert(*((size_t *) vec_get(vector, 3)) == 6);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_filter_function_large(const void *data)
{
    return *((size_t *) data) >= 500;
}

static int test_vec_filter_mut_large(void)
{
    printf("%-40s", "test_vec_filter_mut (large) ");

    vec_t *vector = vec_new();

    for (size_t i = 0; i < 100; ++i) {
        size_t random = rand() % 1000;
        vec_push(vector, (void *) &random, sizeof(size_t));
    }

    vec_filter_mut(vector, test_filter_function_large);

    for (size_t i = 0; i < vector->len; ++i) {
        assert(*((size_t *) vec_get(vector, i)) >= 500);
    }

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}


struct test_filter_structure {
    int element_a;
    char some_string[];
};

static int test_filter_function_complex(const void *data)
{
    struct test_filter_structure *pStructure = (struct test_filter_structure *) data;

    return (pStructure->element_a % 3 == 0 && strcmp(pStructure->some_string, "example1") == 0);
}

static int test_vec_filter_mut_complex(void)
{
    printf("%-40s", "test_vec_filter_mut (complex) ");

    char *string1 = "example1";
    char *string2 = "example2";

    vec_t *vector = vec_new();

    for (size_t i = 0; i < 13; ++i) {

        struct test_filter_structure *structure = malloc(sizeof(struct test_filter_structure) + 20);

        structure->element_a = (int) i;
        if (i % 2 == 0) {
            strncpy(structure->some_string, string1, 19);
        } else {
            strncpy(structure->some_string, string2, 19);
        }

        vec_push(vector, (void *) structure, sizeof(struct test_filter_structure) + 20);

        free(structure);

    }

    assert(vec_filter_mut(vector, test_filter_function_complex) == 10);

    assert(vector->len == 3);
    int expected_elements[] = {0, 6, 12};

    for (size_t i = 0; i < 3; ++i) {
        struct test_filter_structure *pStructure = (struct test_filter_structure *) vec_get(vector, i);

        //printf("%d %s\n", pStructure->element_a, pStructure->some_string);

        assert(pStructure->element_a == expected_elements[i] && strcmp(pStructure->some_string, string1) == 0);
    }

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_filter(void)
{
    printf("%-40s", "test_vec_filter ");

    // perform filter on non-existent vector
    assert(vec_filter(NULL, test_filter_function, 1) == NULL);

    vec_t *vector = vec_new();

    // perform filter on empty vector
    vec_t *empty = vec_filter(vector, test_filter_function, sizeof(size_t));
    assert(empty);
    assert(empty->len == 0);
    vec_destroy(empty);

    size_t data[] = {9, 3, 2, 0, 5, 5, 4, 6, 3, 1};

    for (size_t i = 0; i < 10; ++i) {
        vec_push(vector, (void *) &data[i], sizeof(size_t));
    }

    //print_sizet_vec(vector);

    vec_t *filtered1 = vec_filter(vector, test_filter_function, sizeof(size_t));
    
    //print_sizet_vec(vector);
    //print_sizet_vec(filtered1);

    assert(filtered1->len == 4);
    assert(vector->len == 10);
    
    // check that the original vector is unchanged
    for (size_t i = 0; i < vector->len; ++i) {
        assert(*((size_t *) vec_get(vector, i)) == data[i]);
    }

    // check the new vector
    assert(*((size_t *) vec_get(filtered1, 0)) == 9);
    assert(*((size_t *) vec_get(filtered1, 1)) == 5);
    assert(*((size_t *) vec_get(filtered1, 2)) == 5);
    assert(*((size_t *) vec_get(filtered1, 3)) == 6);


    // applying the filtering function again should lead to the same result as before
    vec_t *filtered2 = vec_filter(vector, test_filter_function, sizeof(size_t));
    assert(filtered2->len == 4);
    assert(vector->len == 10);

    // deallocate vector early to check that the values are properly copied
    vec_destroy(vector);

    // compare filtered1 and filtered2
    assert(*((size_t *) vec_get(filtered1, 0)) == *((size_t *) vec_get(filtered2, 0)));
    assert(*((size_t *) vec_get(filtered1, 1)) == *((size_t *) vec_get(filtered2, 1)));
    assert(*((size_t *) vec_get(filtered1, 2)) == *((size_t *) vec_get(filtered2, 2)));
    assert(*((size_t *) vec_get(filtered1, 3)) == *((size_t *) vec_get(filtered2, 3)));

    vec_destroy(filtered1);
    vec_destroy(filtered2);

    printf("OK\n");
    return 0;
}


static int test_vec_filter_complex(void)
{
    printf("%-40s", "test_vec_filter (complex) ");

    char *string1 = "example1";
    char *string2 = "example2";

    vec_t *vector = vec_new();

    for (size_t i = 0; i < 13; ++i) {

        struct test_filter_structure *structure = malloc(sizeof(struct test_filter_structure) + 20);

        structure->element_a = (int) i;
        if (i % 2 == 0) {
            strncpy(structure->some_string, string1, 19);
        } else {
            strncpy(structure->some_string, string2, 19);
        }

        vec_push(vector, (void *) structure, sizeof(struct test_filter_structure) + 20);

        free(structure);

    }

    vec_t *filtered = vec_filter(vector, test_filter_function_complex, sizeof(struct test_filter_structure) + 20);

    assert(vector->len == 13);
    assert(filtered->len == 3);

    int expected_elements[] = {0, 6, 12};
    for (size_t i = 0; i < 3; ++i) {
        struct test_filter_structure *pStructure = (struct test_filter_structure *) vec_get(filtered, i);

        //printf("%d %s\n", pStructure->element_a, pStructure->some_string);

        assert(pStructure->element_a == expected_elements[i] && strcmp(pStructure->some_string, string1) == 0);
    }

    vec_destroy(vector);
    vec_destroy(filtered);

    printf("OK\n");
    return 0;
}


static int test_equality_function(const void *data, const void *target)
{
    return *((size_t *) data) == *((size_t *) target);
}

static int test_vec_find(void)
{
    printf("%-40s", "test_vec_find ");

    size_t search[] = {9, 1, 5, 3, 19};

    // search in non-existent vector
    assert(vec_find(NULL, test_equality_function, (void *) &search[0]) == -99);

    vec_t *vector = vec_new();

    // search in an empty vector
    assert(vec_find(vector, test_equality_function, (void *) &search[0]) == -1);

    size_t data[] = {9, 3, 2, 0, 5, 5, 4, 6, 3, 1};

    for (size_t i = 0; i < 10; ++i) {
        vec_push(vector, (void *) &data[i], sizeof(size_t));
    }

    // find item at the beginning of vector
    assert(vec_find(vector, test_equality_function, (void *) &search[0]) == 0);
    // find item at the end of vector
    assert(vec_find(vector, test_equality_function, (void *) &search[1]) == 9);
    // find first of many items
    assert(vec_find(vector, test_equality_function, (void *) &search[2]) == 4);
    assert(vec_find(vector, test_equality_function, (void *) &search[3]) == 1);
    // search for non-existent item
    assert(vec_find(vector, test_equality_function, (void *) &search[4]) == -1);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_comparison_function(const void *first, const void *second)
{
    return ((int) *((size_t *) first)) - ((int) *((size_t *) second));
}

static int test_vec_find_bsearch(void)
{
    printf("%-40s", "test_vec_find_bsearch ");

    size_t search[] = {0, 9, 5, 3, 19};

    // search in non-existent vector
    assert(vec_find_bsearch(NULL, test_comparison_function, (void *) &search[0]) == -99);

    vec_t *vector = vec_new();

    // search in an empty vector
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[0]) == -1);

    size_t data[] = {0, 1, 2, 3, 3, 4, 5, 5, 6, 9};

    for (size_t i = 0; i < 10; ++i) {
        vec_push(vector, (void *) &data[i], sizeof(size_t));
    }

    // find item at the beginning of vector
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[0]) == 0);
    // find item at the end of vector
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[1]) == 9);
    // find first of many items
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[2]) == 6);
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[3]) == 3);
    // search for non-existent item
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[4]) == -1);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_int_comparison_function(const void *first, const void *second)
{
    return ((int) *((int *) first)) - ((int) *((int *) second));
}

static int test_vec_find_min(void)
{
    printf("%-40s", "test_vec_find_min ");

    int items[] = {5, 3, 1, 7, 6, 0, -1};

    // search in a non-existent vector
    assert(vec_find_min(NULL, test_int_comparison_function) == -99);

    // search in an empty vector
    vec_t *vector = vec_new();
    assert(vec_find_min(vector, test_int_comparison_function) == -1);

    // search in a vector of length 1
    vec_push(vector, &items[0], sizeof(int));
    assert(vec_find_min(vector, test_int_comparison_function) == 0);

    // search for a minimum located in the middle of the vector
    for (size_t i = 1; i < 5; ++i) {
        vec_push(vector, &items[i], sizeof(int));
    }
    assert(vec_find_min(vector, test_int_comparison_function) == 2);

    // search for a minimum located at the start of the vector
    vec_insert(vector, &items[5], sizeof(int), 0);
    assert(vec_find_min(vector, test_int_comparison_function) == 0);

    // search for a minimum located at the end of the vector
    vec_push(vector, &items[6], sizeof(int));
    assert(vec_find_min(vector, test_int_comparison_function) == 6);

    // multiple minima in the vector
    vec_insert(vector, &items[6], sizeof(int), 2);
    assert(vec_find_min(vector, test_int_comparison_function) == 2);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_find_max(void)
{
    printf("%-40s", "test_vec_find_max ");

    int items[] = {-4, -2, -1, -3, -5, 0, 7};

    // search in a non-existent vector
    assert(vec_find_max(NULL, test_int_comparison_function) == -99);

    // search in an empty vector
    vec_t *vector = vec_new();
    assert(vec_find_max(vector, test_int_comparison_function) == -1);

    // search in a vector of length 1
    vec_push(vector, &items[0], sizeof(int));
    assert(vec_find_max(vector, test_int_comparison_function) == 0);

    // search for a maximum located in the middle of the vector
    for (size_t i = 1; i < 5; ++i) {
        vec_push(vector, &items[i], sizeof(int));
    }
    assert(vec_find_max(vector, test_int_comparison_function) == 2);

    // search for a maximum located at the start of the vector
    vec_insert(vector, &items[5], sizeof(int), 0);
    assert(vec_find_max(vector, test_int_comparison_function) == 0);

    // search for a maximum located at the end of the vector
    vec_push(vector, &items[6], sizeof(int));
    assert(vec_find_max(vector, test_int_comparison_function) == 6);

    // multiple maxima in the vector
    vec_insert(vector, &items[6], sizeof(int), 2);
    assert(vec_find_max(vector, test_int_comparison_function) == 2);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

struct person {
    unsigned age_years;
    unsigned age_months;
    char name[20];
};

static int age_comparison_function(const void *first, const void *second)
{
    struct person *p1 = (struct person *) first;
    struct person *p2 = (struct person *) second;

    if (p1->age_years != p2->age_years) return p1->age_years - p2->age_years;
    else return p1->age_months - p2->age_months;
}

static int test_vec_find_min_max_complex(void)
{
    printf("%-40s", "test_vec_find_min_max (complex) ");

    struct person p1 = {.age_years = 8, .age_months = 2, .name = "Kate"};
    struct person p2 = {.age_years = 37, .age_months = 8, .name = "Elizabeth"};
    struct person p3 = {.age_years = 37, .age_months = 11, .name = "John"};
    struct person p4 = {.age_years = 8, .age_months = 0, .name = "Jimmy"};
    struct person p5 = {.age_years = 37, .age_months = 11, .name = "Rebecca"};

    vec_t *vector = vec_new();

    vec_push(vector, &p1, sizeof(struct person));
    vec_push(vector, &p2, sizeof(struct person));
    vec_push(vector, &p3, sizeof(struct person));

    size_t found = 0;

    // find youngest person
    found = vec_find_min(vector, age_comparison_function);
    assert(found == 0);
    assert(strcmp(((struct person *) vec_get(vector, found))->name, "Kate") == 0);
    //printf("%s\n", ((struct person *) vec_get(vector, found))->name);

    // add Jimmy and find new youngest person
    vec_push(vector, &p4, sizeof(struct person));
    found = vec_find_min(vector, age_comparison_function);
    assert(found == 3);
    assert(strcmp(((struct person *) vec_get(vector, found))->name, "Jimmy") == 0);
    //printf("%s\n", ((struct person *) vec_get(vector, found))->name);

    // find oldest person
    found = vec_find_max(vector, age_comparison_function);
    assert(found == 2);
    assert(strcmp(((struct person *) vec_get(vector, found))->name, "John") == 0);
    //printf("%s\n", ((struct person *) vec_get(vector, found))->name);

    // add Rebecca to the end of the vector and find oldest person
    vec_push(vector, &p5, sizeof(struct person));
    found = vec_find_max(vector, age_comparison_function);
    assert(found == 2);
    assert(strcmp(((struct person *) vec_get(vector, found))->name, "John") == 0);
    //printf("%s\n", ((struct person *) vec_get(vector, found))->name);

    // move Rebecca to the front and find oldest person
    free(vec_pop(vector));
    vec_insert(vector, &p5, sizeof(struct person), 1);
    found = vec_find_max(vector, age_comparison_function);
    assert(found == 1);
    assert(strcmp(((struct person *) vec_get(vector, found))->name, "Rebecca") == 0);
    //printf("%s\n", ((struct person *) vec_get(vector, found))->name);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}


static void multiply_by_two(void *item, void *unused)
{
    UNUSED(unused);
    size_t *ptr = (size_t *) item;
    *ptr *= 2;
}

static int test_vec_map(void)
{
    printf("%-40s", "test_vec_map ");

    vec_map(NULL, multiply_by_two, NULL);

    vec_t *vector = vec_new();

    for (size_t i = 0; i < 200; ++i) {
        vec_push(vector, &i, sizeof(size_t));
    }

    vec_map(vector, multiply_by_two, NULL);

    for (size_t i = 0; i < vector->len; ++i) {
        assert( *(size_t *) vec_get(vector, i) == i * 2);
    }

    //print_sizet_vec(vector);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_shuffle(void)
{
    printf("%-40s", "test_vec_shuffle ");

    // shuffle non-existent vector
    vec_shuffle(NULL);

    // shuffle empty vector
    vec_t *vector = vec_new();
    vec_shuffle(vector);
    
    // shuffle vector with one element
    size_t first = 0;
    vec_push(vector, &first, sizeof(size_t));
    vec_shuffle(vector);

    for (size_t i = 1; i < 200; ++i) {
        vec_push(vector, &i, sizeof(size_t));
    }

    //print_sizet_vec(vector);

    // shuffle vector with 200 items
    vec_shuffle(vector);

    // check that all original items are present in the vector
    // and that the vector is NOT sorted
    short sorted = 1;
    for (size_t i = 0; i < 200; ++i) {
        assert(vec_find(vector, test_equality_function, &i) >= 0);
        if (i >= 1 && vec_get(vector, i) > vec_get(vector, i - 1)) sorted = 0;
    }

    assert(!sorted);

    //print_sizet_vec(vector);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_vec_reverse(void)
{
    printf("%-40s", "test_vec_reverse ");

    // reverse non-existent vector
    vec_reverse(NULL);

    // reverse empty vector
    vec_t *vector = vec_new();
    vec_reverse(vector);
    
    // reverse vector with one element
    size_t first = 0;
    vec_push(vector, &first, sizeof(size_t));
    vec_reverse(vector);

    for (size_t i = 1; i < 200; ++i) {
        vec_push(vector, &i, sizeof(size_t));
    }

    // reverse vector with 200 items
    vec_reverse(vector);

    //print_sizet_vec(vector);

    for (size_t i = 0; i < 200; ++i) {
        assert(*(size_t *) vec_get(vector, i) == 200 - i - 1);
    }

    // add one more item and reverse again
    size_t one_more = 200;
    vec_insert(vector, &one_more, sizeof(size_t), 0);

    //print_sizet_vec(vector);

    vec_reverse(vector);
    for (size_t i = 0; i < 201; ++i) {
        assert(*(size_t *) vec_get(vector, i) == i);
    }

    //print_sizet_vec(vector);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}



static int test_comparison_function_reverse(const void *first, const void *second)
{
    return ((int) *((size_t *) second)) - ((int) *((size_t *) first));
}

static int test_vec_sort_selection(void)
{
    printf("%-40s", "test_vec_sort_selection ");

    // sort non-existent vector
    assert(vec_sort_selection(NULL, test_comparison_function) == 99);
    
    // vector #1
    size_t data1[] = {0, 7, 3, 2, 9, 5, 6, 1, 8, 4};
    vec_t *vector1 = vec_new();

    // sort empty vector
    assert(vec_sort_selection(vector1, test_comparison_function) == 0);

    for (size_t i = 0; i < 10; ++i) vec_push(vector1, (void *) &data1[i], sizeof(size_t));

    assert(vec_sort_selection(vector1, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector1, i)) == i);

    vec_destroy(vector1);

    // vector #2
    size_t data2[] = {8, 2, 9, 4, 5, 3, 7, 6, 1, 0};
    vec_t *vector2 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector2, (void *) &data2[i], sizeof(size_t));

    assert(vec_sort_selection(vector2, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector2, i)) == i);

    vec_destroy(vector2);
    
    // vector #3
    size_t data3[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector3 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector3, (void *) &data3[i], sizeof(size_t));

    assert(vec_sort_selection(vector3, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    // sorting sorted list
    assert(vec_sort_selection(vector3, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    vec_destroy(vector3);

    // vector #4
    size_t data4[] = {3, 5, 3, 1, 3, 7, 2, 6, 6, 4};
    vec_t *vector4 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector4, (void *) &data4[i], sizeof(size_t));

    assert(vec_sort_selection(vector4, test_comparison_function) == 0);
    assert(*((size_t *) vec_get(vector4, 0)) == 1);
    assert(*((size_t *) vec_get(vector4, 1)) == 2);
    assert(*((size_t *) vec_get(vector4, 2)) == 3);
    assert(*((size_t *) vec_get(vector4, 3)) == 3);
    assert(*((size_t *) vec_get(vector4, 4)) == 3);
    assert(*((size_t *) vec_get(vector4, 5)) == 4);
    assert(*((size_t *) vec_get(vector4, 6)) == 5);
    assert(*((size_t *) vec_get(vector4, 7)) == 6);
    assert(*((size_t *) vec_get(vector4, 8)) == 6);
    assert(*((size_t *) vec_get(vector4, 9)) == 7);

    vec_destroy(vector4);

    // vector #5
    size_t data5[] = {5, 5, 5, 5, 5};
    vec_t *vector5 = vec_new();
    for (size_t i = 0; i < 5; ++i) vec_push(vector5, (void *) &data5[i], sizeof(size_t));

    assert(vec_sort_selection(vector5, test_comparison_function) == 0);
    for (size_t i = 0; i < 5; ++i) assert(*((size_t *) vec_get(vector5, i)) == 5);

    vec_destroy(vector5);

    // vector #6
    // reverse sort
    size_t data6[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector6 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector6, (void *) &data6[i], sizeof(size_t));

    assert(vec_sort_selection(vector6, test_comparison_function_reverse) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector6, i)) == (9 - i));

    vec_destroy(vector6);

    printf("OK\n");
    return 0;
}

static int test_vec_sort_bubble(void)
{
    printf("%-40s", "test_vec_sort_bubble ");

    // sort non-existent vector
    assert(vec_sort_bubble(NULL, test_comparison_function) == 99);
    
    // vector #1
    size_t data1[] = {0, 7, 3, 2, 9, 5, 6, 1, 8, 4};
    vec_t *vector1 = vec_new();

    // sort empty vector
    assert(vec_sort_bubble(vector1, test_comparison_function) == 0);

    for (size_t i = 0; i < 10; ++i) vec_push(vector1, (void *) &data1[i], sizeof(size_t));

    assert(vec_sort_bubble(vector1, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector1, i)) == i);

    vec_destroy(vector1);

    // vector #2
    size_t data2[] = {8, 2, 9, 4, 5, 3, 7, 6, 1, 0};
    vec_t *vector2 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector2, (void *) &data2[i], sizeof(size_t));

    assert(vec_sort_bubble(vector2, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector2, i)) == i);

    vec_destroy(vector2);
    
    // vector #3
    size_t data3[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector3 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector3, (void *) &data3[i], sizeof(size_t));

    assert(vec_sort_bubble(vector3, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    // sorting sorted list
    assert(vec_sort_bubble(vector3, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    vec_destroy(vector3);

    // vector #4
    size_t data4[] = {3, 5, 3, 1, 3, 7, 2, 6, 6, 4};
    vec_t *vector4 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector4, (void *) &data4[i], sizeof(size_t));

    assert(vec_sort_bubble(vector4, test_comparison_function) == 0);
    assert(*((size_t *) vec_get(vector4, 0)) == 1);
    assert(*((size_t *) vec_get(vector4, 1)) == 2);
    assert(*((size_t *) vec_get(vector4, 2)) == 3);
    assert(*((size_t *) vec_get(vector4, 3)) == 3);
    assert(*((size_t *) vec_get(vector4, 4)) == 3);
    assert(*((size_t *) vec_get(vector4, 5)) == 4);
    assert(*((size_t *) vec_get(vector4, 6)) == 5);
    assert(*((size_t *) vec_get(vector4, 7)) == 6);
    assert(*((size_t *) vec_get(vector4, 8)) == 6);
    assert(*((size_t *) vec_get(vector4, 9)) == 7);

    vec_destroy(vector4);

    // vector #5
    size_t data5[] = {5, 5, 5, 5, 5};
    vec_t *vector5 = vec_new();
    for (size_t i = 0; i < 5; ++i) vec_push(vector5, (void *) &data5[i], sizeof(size_t));

    assert(vec_sort_bubble(vector5, test_comparison_function) == 0);
    for (size_t i = 0; i < 5; ++i) assert(*((size_t *) vec_get(vector5, i)) == 5);

    vec_destroy(vector5);

    // vector #6
    // reverse sort
    size_t data6[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector6 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector6, (void *) &data6[i], sizeof(size_t));

    assert(vec_sort_bubble(vector6, test_comparison_function_reverse) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector6, i)) == (9 - i));

    vec_destroy(vector6);

    printf("OK\n");
    return 0;
}


static int test_vec_sort_insertion(void)
{
    printf("%-40s", "test_vec_sort_insertion ");

    // sort non-existent vector
    assert(vec_sort_insertion(NULL, test_comparison_function) == 99);
    
    // vector #1
    size_t data1[] = {0, 7, 3, 2, 9, 5, 6, 1, 8, 4};
    vec_t *vector1 = vec_new();

    // sort empty vector
    assert(vec_sort_insertion(vector1, test_comparison_function) == 0);

    for (size_t i = 0; i < 10; ++i) vec_push(vector1, (void *) &data1[i], sizeof(size_t));

    assert(vec_sort_insertion(vector1, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector1, i)) == i);

    vec_destroy(vector1);

    // vector #2
    size_t data2[] = {8, 2, 9, 4, 5, 3, 7, 6, 1, 0};
    vec_t *vector2 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector2, (void *) &data2[i], sizeof(size_t));

    assert(vec_sort_insertion(vector2, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector2, i)) == i);

    vec_destroy(vector2);
    
    // vector #3
    size_t data3[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector3 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector3, (void *) &data3[i], sizeof(size_t));

    assert(vec_sort_insertion(vector3, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    // sorting sorted list
    assert(vec_sort_insertion(vector3, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    vec_destroy(vector3);

    // vector #4
    size_t data4[] = {3, 5, 3, 1, 3, 7, 2, 6, 6, 4};
    vec_t *vector4 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector4, (void *) &data4[i], sizeof(size_t));

    assert(vec_sort_insertion(vector4, test_comparison_function) == 0);
    assert(*((size_t *) vec_get(vector4, 0)) == 1);
    assert(*((size_t *) vec_get(vector4, 1)) == 2);
    assert(*((size_t *) vec_get(vector4, 2)) == 3);
    assert(*((size_t *) vec_get(vector4, 3)) == 3);
    assert(*((size_t *) vec_get(vector4, 4)) == 3);
    assert(*((size_t *) vec_get(vector4, 5)) == 4);
    assert(*((size_t *) vec_get(vector4, 6)) == 5);
    assert(*((size_t *) vec_get(vector4, 7)) == 6);
    assert(*((size_t *) vec_get(vector4, 8)) == 6);
    assert(*((size_t *) vec_get(vector4, 9)) == 7);

    vec_destroy(vector4);

    // vector #5
    size_t data5[] = {5, 5, 5, 5, 5};
    vec_t *vector5 = vec_new();
    for (size_t i = 0; i < 5; ++i) vec_push(vector5, (void *) &data5[i], sizeof(size_t));

    assert(vec_sort_insertion(vector5, test_comparison_function) == 0);
    for (size_t i = 0; i < 5; ++i) assert(*((size_t *) vec_get(vector5, i)) == 5);

    vec_destroy(vector5);

    // vector #6
    // reverse sort
    size_t data6[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector6 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector6, (void *) &data6[i], sizeof(size_t));

    assert(vec_sort_insertion(vector6, test_comparison_function_reverse) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector6, i)) == (9 - i));

    vec_destroy(vector6);

    printf("OK\n");
    return 0;
}


static int test_vec_sort_quicknaive(void)
{
    printf("%-40s", "test_vec_sort_quicknaive ");

    // sort non-existent vector
    assert(vec_sort_quicknaive(NULL, test_comparison_function) == 99);
    
    // vector #1
    size_t data1[] = {0, 7, 3, 2, 9, 5, 6, 1, 8, 4};
    vec_t *vector1 = vec_new();

    // sort empty vector
    assert(vec_sort_quicknaive(vector1, test_comparison_function) == 0);

    for (size_t i = 0; i < 10; ++i) vec_push(vector1, (void *) &data1[i], sizeof(size_t));

    assert(vec_sort_quicknaive(vector1, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector1, i)) == i);

    vec_destroy(vector1);

    // vector #2
    size_t data2[] = {8, 2, 9, 4, 5, 3, 7, 6, 1, 0};
    vec_t *vector2 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector2, (void *) &data2[i], sizeof(size_t));

    assert(vec_sort_quicknaive(vector2, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector2, i)) == i);

    vec_destroy(vector2);
    
    // vector #3
    size_t data3[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector3 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector3, (void *) &data3[i], sizeof(size_t));

    assert(vec_sort_quicknaive(vector3, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    // sorting sorted list
    assert(vec_sort_quicknaive(vector3, test_comparison_function) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    vec_destroy(vector3);

    // vector #4
    size_t data4[] = {3, 5, 3, 1, 3, 7, 2, 6, 6, 4};
    vec_t *vector4 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector4, (void *) &data4[i], sizeof(size_t));

    assert(vec_sort_quicknaive(vector4, test_comparison_function) == 0);
    assert(*((size_t *) vec_get(vector4, 0)) == 1);
    assert(*((size_t *) vec_get(vector4, 1)) == 2);
    assert(*((size_t *) vec_get(vector4, 2)) == 3);
    assert(*((size_t *) vec_get(vector4, 3)) == 3);
    assert(*((size_t *) vec_get(vector4, 4)) == 3);
    assert(*((size_t *) vec_get(vector4, 5)) == 4);
    assert(*((size_t *) vec_get(vector4, 6)) == 5);
    assert(*((size_t *) vec_get(vector4, 7)) == 6);
    assert(*((size_t *) vec_get(vector4, 8)) == 6);
    assert(*((size_t *) vec_get(vector4, 9)) == 7);

    vec_destroy(vector4);

    // vector #5
    size_t data5[] = {5, 5, 5, 5, 5};
    vec_t *vector5 = vec_new();
    for (size_t i = 0; i < 5; ++i) vec_push(vector5, (void *) &data5[i], sizeof(size_t));

    assert(vec_sort_quicknaive(vector5, test_comparison_function) == 0);
    for (size_t i = 0; i < 5; ++i) assert(*((size_t *) vec_get(vector5, i)) == 5);

    vec_destroy(vector5);

    // vector #6
    // reverse sort
    size_t data6[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector6 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector6, (void *) &data6[i], sizeof(size_t));

    assert(vec_sort_quicknaive(vector6, test_comparison_function_reverse) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector6, i)) == (9 - i));

    vec_destroy(vector6);

    printf("OK\n");
    return 0;
}

static int test_comparison_function_qsort(const void *first, const void *second)
{
    return ((int) **((size_t **) first)) - ((int) **((size_t **) second));
}

static int test_comparison_function_qsort_reverse(const void *first, const void *second)
{
    return ((int) **((size_t **) second)) - ((int) **((size_t **) first));
}

static int test_vec_sort_quick(void)
{
    printf("%-40s", "test_vec_sort_quick ");

    // sort non-existent vector
    assert(vec_sort_quick(NULL, test_comparison_function_qsort) == 99);
    
    // vector #1
    size_t data1[] = {0, 7, 3, 2, 9, 5, 6, 1, 8, 4};
    vec_t *vector1 = vec_new();

    // sort empty vector
    assert(vec_sort_quick(vector1, test_comparison_function_qsort) == 0);

    for (size_t i = 0; i < 10; ++i) vec_push(vector1, (void *) &data1[i], sizeof(size_t));

    assert(vec_sort_quick(vector1, test_comparison_function_qsort) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector1, i)) == i);

    vec_destroy(vector1);

    // vector #2
    size_t data2[] = {8, 2, 9, 4, 5, 3, 7, 6, 1, 0};
    vec_t *vector2 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector2, (void *) &data2[i], sizeof(size_t));

    assert(vec_sort_quick(vector2, test_comparison_function_qsort) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector2, i)) == i);

    vec_destroy(vector2);
    
    // vector #3
    size_t data3[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector3 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector3, (void *) &data3[i], sizeof(size_t));

    assert(vec_sort_quick(vector3, test_comparison_function_qsort) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    // sorting sorted list
    assert(vec_sort_quick(vector3, test_comparison_function_qsort) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector3, i)) == i);

    vec_destroy(vector3);

    // vector #4
    size_t data4[] = {3, 5, 3, 1, 3, 7, 2, 6, 6, 4};
    vec_t *vector4 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector4, (void *) &data4[i], sizeof(size_t));

    assert(vec_sort_quick(vector4, test_comparison_function_qsort) == 0);
    assert(*((size_t *) vec_get(vector4, 0)) == 1);
    assert(*((size_t *) vec_get(vector4, 1)) == 2);
    assert(*((size_t *) vec_get(vector4, 2)) == 3);
    assert(*((size_t *) vec_get(vector4, 3)) == 3);
    assert(*((size_t *) vec_get(vector4, 4)) == 3);
    assert(*((size_t *) vec_get(vector4, 5)) == 4);
    assert(*((size_t *) vec_get(vector4, 6)) == 5);
    assert(*((size_t *) vec_get(vector4, 7)) == 6);
    assert(*((size_t *) vec_get(vector4, 8)) == 6);
    assert(*((size_t *) vec_get(vector4, 9)) == 7);

    vec_destroy(vector4);

    // vector #5
    size_t data5[] = {5, 5, 5, 5, 5};
    vec_t *vector5 = vec_new();
    for (size_t i = 0; i < 5; ++i) vec_push(vector5, (void *) &data5[i], sizeof(size_t));

    assert(vec_sort_quick(vector5, test_comparison_function_qsort) == 0);
    for (size_t i = 0; i < 5; ++i) assert(*((size_t *) vec_get(vector5, i)) == 5);

    vec_destroy(vector5);

    // vector #6
    // reverse sort
    size_t data6[] = {5, 1, 3, 2, 9, 7, 4, 6, 0, 8};
    vec_t *vector6 = vec_new();
    for (size_t i = 0; i < 10; ++i) vec_push(vector6, (void *) &data6[i], sizeof(size_t));

    assert(vec_sort_quick(vector6, test_comparison_function_qsort_reverse) == 0);
    for (size_t i = 0; i < 10; ++i) assert(*((size_t *) vec_get(vector6, i)) == (9 - i));

    vec_destroy(vector6);

    printf("OK\n");
    return 0;
}

static int test_vec_sort_and_find(void)
{
    printf("%-40s", "test_vec_sort_and_find ");

    size_t search[] = {9, 1, 5, 3};

    vec_t *vector = vec_new();
    size_t data[] = {9, 3, 2, 0, 5, 5, 4, 6, 3, 1};

    for (size_t i = 0; i < 10; ++i) {
        vec_push(vector, (void *) &data[i], sizeof(size_t));
    }

    // sort the vector
    vec_sort_quicknaive(vector, test_comparison_function);

    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[0]) == 9);
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[1]) == 1);
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[2]) == 6);
    assert(vec_find_bsearch(vector, test_comparison_function, (void *) &search[3]) == 3);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}


static int test_vec_shuffle_and_sort(void)
{
    printf("%-40s", "test_vec_shuffle_and_sort ");

    vec_t *vector = vec_new();

    for (size_t i = 0; i < 200; ++i) {
        vec_push(vector, &i, sizeof(size_t));
    }

    vec_shuffle(vector);

    //print_sizet_vec(vector);

    // check that all original items are present in the vector
    // and that the vector is NOT sorted
    short sorted = 1;
    for (size_t i = 0; i < 200; ++i) {
        assert(vec_find(vector, test_equality_function, &i) >= 0);
        if (i >= 1 && vector->items[i] > vector->items[i - 1]) sorted = 0;
    }

    assert(!sorted);

    // sort the vector again
    vec_sort_quicknaive(vector, test_comparison_function);

    // check that the vector is sorted
    for (size_t i = 0; i < 200; ++i) {
        assert(*(size_t *) vec_get(vector, i) == i);
    }

    //print_sizet_vec(vector);

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}


int main(void) 
{
    srand(time(NULL));

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
    test_vec_push_pop_preallocated();

    test_vec_heterogeneous();

    test_vec_filter_mut();
    test_vec_filter_mut_large();
    test_vec_filter_mut_complex();

    test_vec_filter();
    test_vec_filter_complex();

    test_vec_find();
    test_vec_find_bsearch();
    test_vec_find_min();
    test_vec_find_max();
    test_vec_find_min_max_complex();

    test_vec_map();

    test_vec_shuffle();
    test_vec_reverse();

    test_vec_sort_selection();
    test_vec_sort_bubble();
    test_vec_sort_insertion();
    test_vec_sort_quicknaive();
    test_vec_sort_quick();

    test_vec_sort_and_find();
    test_vec_shuffle_and_sort();

    return 0;
}