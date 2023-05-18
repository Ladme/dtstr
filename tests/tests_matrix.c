// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/matrix.h"
#include "../src/vector.h"

#define UNUSED(x) (void)(x)

struct iterator {
    size_t *n_cols;
    size_t row_index;
    size_t col_index;
};

static void print_int(void *item, void *row_iterator)
{
    struct iterator *it = (struct iterator *) row_iterator;

    if (it->col_index == 0) printf("\nRow %lu: ", it->row_index);

    while (it->n_cols[it->row_index] == 0) {
        ++(it->row_index);
        printf("\nRow %lu: ", it->row_index);
    }

    
    printf("%d ", *(int *) item);

    ++(it->col_index);
    if (it->col_index >= it->n_cols[it->row_index]) {
        ++(it->row_index);
        it->col_index = 0;
    }
}

static void matrix_print_int(matrix_t *matrix)
{
    struct iterator it = { .n_cols = matrix->n_cols, .row_index = 0, .col_index = 0 };

    matrix_map(matrix, print_int, &it);
    printf("\n");
}

static int test_matrix_destroy_null(void)
{
    printf("%-40s", "test_matrix_destroy (null) ");

    matrix_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_matrix_new(void) 
{
    printf("%-40s", "test_matrix_new ");

    matrix_t *matrix = matrix_new();

    assert(matrix);
    assert(matrix->len == 0);
    assert(matrix->n_rows == 0);
    for (size_t i = 0; i < MATRIX_DEFAULT_CAPACITY_ROWS; ++i) {
        assert(matrix->n_cols[i] == 0);
    }

    assert(matrix->base_capacity_cols == MATRIX_DEFAULT_CAPACITY_COLS);
    assert(matrix->base_capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);
    assert(matrix->capacity_cols == MATRIX_DEFAULT_CAPACITY_COLS);
    assert(matrix->capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;
}

static int test_matrix_with_capacity(void) 
{
    printf("%-40s", "test_matrix_with_capacity ");

    size_t rows_capacity = 64;
    size_t cols_capacity = 64;

    matrix_t *matrix = matrix_with_capacity(rows_capacity, cols_capacity);

    assert(matrix);
    assert(matrix->len == 0);
    assert(matrix->n_rows == 0);
    for (size_t i = 0; i < rows_capacity; ++i) {
        assert(matrix->n_cols[i] == 0);
    }

    assert(matrix->base_capacity_cols == cols_capacity);
    assert(matrix->base_capacity_rows == rows_capacity);
    assert(matrix->capacity_cols == cols_capacity);
    assert(matrix->capacity_rows == rows_capacity);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;
}

static int test_matrix_fit(void)
{
    printf("%-40s", "test_matrix_fit ");

    matrix_t *matrix = matrix_fit(60, 18);

    assert(matrix);
    assert(matrix->len == 0);
    assert(matrix->n_rows == 0);
    for (size_t i = 0; i < 64; ++i) {
        assert(matrix->n_cols[i] == 0);
    }

    assert(matrix->base_capacity_cols == 8);
    assert(matrix->base_capacity_rows == 8);
    assert(matrix->capacity_cols == 32);
    assert(matrix->capacity_rows == 64);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;

}

static int test_matrix_push_small(void) 
{
    printf("%-40s", "test_matrix_push (small) ");

    int value = 666;

    assert(matrix_push(NULL, 0, &value, sizeof(int)) == 99);

    matrix_t *matrix = matrix_new();

    
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);

    assert(matrix_push(matrix, 1, &value, sizeof(int)) == 0);

    assert(matrix_push(matrix, 3, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 3, &value, sizeof(int)) == 0);
    
    for (size_t i = 0; i < 8; ++i) {
        assert(matrix_push(matrix, 7, &value, sizeof(int)) == 0);
    }

    assert(*(int *) matrix->items[0][0] == value);
    assert(*(int *) matrix->items[0][1] == value);
    assert(*(int *) matrix->items[0][2] == value);

    assert(*(int *) matrix->items[1][0] == value);

    assert(*(int *) matrix->items[3][0] == value);
    assert(*(int *) matrix->items[3][1] == value);

    for (size_t i = 0; i < 8; ++i) {
        assert(*(int *) matrix->items[7][i] == value);
    }

    assert(matrix->len == 14);
    assert(matrix->n_rows == 8);
    assert(matrix->n_cols[0] == 3);
    assert(matrix->n_cols[1] == 1);
    assert(matrix->n_cols[2] == 0);
    assert(matrix->n_cols[3] == 2);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 0);
    assert(matrix->n_cols[6] == 0);
    assert(matrix->n_cols[7] == 8);

    assert(matrix->capacity_cols == matrix->base_capacity_cols);
    assert(matrix->capacity_cols == MATRIX_DEFAULT_CAPACITY_COLS);
    assert(matrix->capacity_rows == matrix->base_capacity_rows);
    assert(matrix->capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);

    //matrix_print_int(matrix);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;
}

static int test_matrix_push_addcols(void) 
{
    printf("%-40s", "test_matrix_push (add cols) ");

    matrix_t *matrix = matrix_new();

    int value = 666;
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);

    assert(matrix_push(matrix, 1, &value, sizeof(int)) == 0);

    assert(matrix_push(matrix, 3, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 3, &value, sizeof(int)) == 0);
    
    for (size_t i = 0; i < 129; ++i) {
        assert(matrix_push(matrix, 7, &value, sizeof(int)) == 0);
    }

    assert(*(int *) matrix->items[0][0] == value);
    assert(*(int *) matrix->items[0][1] == value);
    assert(*(int *) matrix->items[0][2] == value);

    assert(*(int *) matrix->items[1][0] == value);

    assert(*(int *) matrix->items[3][0] == value);
    assert(*(int *) matrix->items[3][1] == value);

    for (size_t i = 0; i < 129; ++i) {
        assert(*(int *) matrix->items[7][i] == value);
    }

    assert(matrix->len == 135);
    assert(matrix->n_rows == 8);
    assert(matrix->n_cols[0] == 3);
    assert(matrix->n_cols[1] == 1);
    assert(matrix->n_cols[2] == 0);
    assert(matrix->n_cols[3] == 2);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 0);
    assert(matrix->n_cols[6] == 0);
    assert(matrix->n_cols[7] == 129);

    assert(matrix->base_capacity_cols == MATRIX_DEFAULT_CAPACITY_COLS);
    assert(matrix->capacity_cols == 256);
    assert(matrix->base_capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);
    assert(matrix->capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);

    int value2 = 111;

    for (size_t i = 0; i < 200; ++i) {
        assert(matrix_push(matrix, 5, &value2, sizeof(int)) == 0);
    }

    for (size_t i = 0; i < 200; ++i) {
        assert(*(int *) matrix->items[5][i] == value2);
    }

    assert(matrix->len == 335);
    assert(matrix->n_rows == 8);
    assert(matrix->n_cols[0] == 3);
    assert(matrix->n_cols[1] == 1);
    assert(matrix->n_cols[2] == 0);
    assert(matrix->n_cols[3] == 2);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 200);
    assert(matrix->n_cols[6] == 0);
    assert(matrix->n_cols[7] == 129);

    assert(matrix->base_capacity_cols == MATRIX_DEFAULT_CAPACITY_COLS);
    assert(matrix->capacity_cols == 256);
    assert(matrix->base_capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);
    assert(matrix->capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);

    //matrix_print_int(matrix);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;
}

static int test_matrix_push_addrows(void) 
{
    printf("%-40s", "test_matrix_push (add rows) ");

    matrix_t *matrix = matrix_new();

    int value = 666;
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);

    assert(matrix_push(matrix, 1, &value, sizeof(int)) == 0);

    assert(matrix_push(matrix, 3, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 3, &value, sizeof(int)) == 0);
    
    for (size_t i = 0; i < 8; ++i) {
        assert(matrix_push(matrix, 7, &value, sizeof(int)) == 0);
    }

    assert(matrix_push(matrix, 8, &value, sizeof(int)) == 0);

    assert(*(int *) matrix->items[0][0] == value);
    assert(*(int *) matrix->items[0][1] == value);
    assert(*(int *) matrix->items[0][2] == value);

    assert(*(int *) matrix->items[1][0] == value);

    assert(*(int *) matrix->items[3][0] == value);
    assert(*(int *) matrix->items[3][1] == value);

    for (size_t i = 0; i < 8; ++i) {
        assert(*(int *) matrix->items[7][i] == value);
    }

    assert(*(int *) matrix->items[8][0] == value);

    assert(matrix->len == 15);
    assert(matrix->n_rows == 9);
    assert(matrix->n_cols[0] == 3);
    assert(matrix->n_cols[1] == 1);
    assert(matrix->n_cols[2] == 0);
    assert(matrix->n_cols[3] == 2);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 0);
    assert(matrix->n_cols[6] == 0);
    assert(matrix->n_cols[7] == 8);
    assert(matrix->n_cols[8] == 1);

    assert(matrix->capacity_cols == matrix->base_capacity_cols);
    assert(matrix->base_capacity_cols == MATRIX_DEFAULT_CAPACITY_COLS);
    assert(matrix->capacity_rows == 9);
    assert(matrix->base_capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);

    assert(matrix_push(matrix, 93, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 93, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 93, &value, sizeof(int)) == 0);

    assert(*(int *) matrix->items[0][0] == value);
    assert(*(int *) matrix->items[0][1] == value);
    assert(*(int *) matrix->items[0][2] == value);

    assert(*(int *) matrix->items[1][0] == value);

    assert(*(int *) matrix->items[3][0] == value);
    assert(*(int *) matrix->items[3][1] == value);

    for (size_t i = 0; i < 8; ++i) {
        assert(*(int *) matrix->items[7][i] == value);
    }

    assert(*(int *) matrix->items[8][0] == value);
    assert(*(int *) matrix->items[93][0] == value);
    assert(*(int *) matrix->items[93][1] == value);
    assert(*(int *) matrix->items[93][1] == value);

    assert(matrix->len == 18);
    assert(matrix->n_rows == 94);
    assert(matrix->n_cols[0] == 3);
    assert(matrix->n_cols[1] == 1);
    assert(matrix->n_cols[2] == 0);
    assert(matrix->n_cols[3] == 2);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 0);
    assert(matrix->n_cols[6] == 0);
    assert(matrix->n_cols[7] == 8);
    assert(matrix->n_cols[8] == 1);
    for (size_t i = 9; i < 93; ++i) {
        assert(matrix->n_cols[i] == 0);
    }
    assert(matrix->n_cols[93] == 3);

    assert(matrix->capacity_cols == matrix->base_capacity_cols);
    assert(matrix->base_capacity_cols == MATRIX_DEFAULT_CAPACITY_COLS);
    assert(matrix->capacity_rows == 94);
    assert(matrix->base_capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);

    //matrix_print_int(matrix);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;
}

static int test_matrix_push_addboth(void) 
{
    printf("%-40s", "test_matrix_push (add both) ");

    matrix_t *matrix = matrix_new();

    int value = 666;
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 0, &value, sizeof(int)) == 0);

    assert(matrix_push(matrix, 1, &value, sizeof(int)) == 0);

    assert(matrix_push(matrix, 3, &value, sizeof(int)) == 0);
    assert(matrix_push(matrix, 3, &value, sizeof(int)) == 0);
    
    for (size_t i = 0; i < 8; ++i) {
        assert(matrix_push(matrix, 7, &value, sizeof(int)) == 0);
    }

    for (size_t i = 0; i < 257; ++i) {
        assert(matrix_push(matrix, 13, &value, sizeof(int)) == 0);
    }

    assert(*(int *) matrix->items[0][0] == value);
    assert(*(int *) matrix->items[0][1] == value);
    assert(*(int *) matrix->items[0][2] == value);

    assert(*(int *) matrix->items[1][0] == value);

    assert(*(int *) matrix->items[3][0] == value);
    assert(*(int *) matrix->items[3][1] == value);

    for (size_t i = 0; i < 8; ++i) {
        assert(*(int *) matrix->items[7][i] == value);
    }

    assert(matrix->len == 271);
    assert(matrix->n_rows == 14);
    assert(matrix->n_cols[0] == 3);
    assert(matrix->n_cols[1] == 1);
    assert(matrix->n_cols[2] == 0);
    assert(matrix->n_cols[3] == 2);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 0);
    assert(matrix->n_cols[6] == 0);
    assert(matrix->n_cols[7] == 8);
    assert(matrix->n_cols[13] == 257);

    assert(matrix->capacity_cols == 512);
    assert(matrix->base_capacity_cols == MATRIX_DEFAULT_CAPACITY_COLS);
    assert(matrix->capacity_rows == 14);
    assert(matrix->base_capacity_rows == MATRIX_DEFAULT_CAPACITY_ROWS);

    //matrix_print_int(matrix);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;
}


static void extract2vector(void *item, void *vector)
{
    vec_t *unpacked = (vec_t *) vector;

    int value = *(int *) item;
    vec_push(unpacked, &value, sizeof(int));
}

static int test_matrix_push_static(void) 
{
    printf("%-40s", "test_matrix_push_static ");

    assert(matrix_push_static(NULL, NULL, sizeof(int)) == 99);

    matrix_t *matrix = matrix_with_capacity(5, 20);

    for (int i = 0; i < 110; ++i) {
        if (i < 100) {
            assert(matrix_push_static(matrix, &i, sizeof(int)) == 0);
        } else {
            assert(matrix_push_static(matrix, &i, sizeof(int)) == 1);
        }
    }

    assert(matrix->capacity_cols == 20);
    assert(matrix->base_capacity_cols == 20);
    assert(matrix->capacity_rows == 5);
    assert(matrix->base_capacity_rows == 5);

    assert(matrix->n_rows == 5);
    for (size_t i = 0; i < 5; ++i) assert(matrix->n_cols[i] == 20);

    //matrix_print_int(matrix);

    vec_t *vector = vec_new();
    matrix_map(matrix, extract2vector, vector);
    for (int i = 0; i < 100; ++i) {
        assert(*(int *) vec_get(vector, i) == i);
    }

    matrix_destroy(matrix);
    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static void multiply_by_two(void *item, void *unused)
{
    UNUSED(unused);
    int *ptr = (int *) item;
    *ptr *= 2;
}

static int test_matrix_map(void)
{
    printf("%-40s", "test_matrix_map ");

    matrix_map(NULL, multiply_by_two, NULL);

    matrix_t *matrix = matrix_with_capacity(5, 20);

    for (int i = 0; i < 100; ++i) {
        matrix_push_static(matrix, &i, sizeof(int));
    }

    matrix_map(matrix, multiply_by_two, NULL);

    int iterator = 0;
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 20; ++j) {
            assert(*(int *) matrix->items[i][j] == iterator * 2);
            ++iterator; 
        }
    }

    //matrix_print_int(matrix);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;

} 

static int test_matrix_get(void) 
{
    printf("%-40s", "test_matrix_get ");

    assert(matrix_get(NULL, 3, 4) == NULL);

    matrix_t *matrix = matrix_with_capacity(5, 20);

    for (int i = 0; i < 100; ++i) {
        matrix_push_static(matrix, &i, sizeof(int));
    }

    int iterator = 0;
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 20; ++j) {
            assert(*(int *) matrix_get(matrix, i, j) == iterator);
            ++iterator; 
        }
    }

    assert(matrix_get(matrix, 5, 3) == NULL);
    assert(matrix_get(matrix, 17, 0) == NULL);
    assert(matrix_get(matrix, 1, 20) == NULL);
    assert(matrix_get(matrix, 4, 153) == NULL);
    assert(matrix_get(matrix, 5, 20) == NULL);
    assert(matrix_get(matrix, -3, 4) == NULL);

    matrix_destroy(matrix);

    printf("OK\n");
    return 0;
}


int main(void)
{
    test_matrix_destroy_null();
    test_matrix_new();
    test_matrix_with_capacity();
    test_matrix_fit();

    test_matrix_push_small();
    test_matrix_push_addcols();
    test_matrix_push_addrows();
    test_matrix_push_addboth();
    test_matrix_push_static();

    test_matrix_map();
    test_matrix_get();

    return 0;
}