// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "matrix.h"

/* *************************************************************************** */
/*                PRIVATE FUNCTIONS ASSOCIATED WITH MATRIX_T                   */
/* *************************************************************************** */

/** @brief Expands all rows of a matrix in capacity. Returns 0 if successful, else returns 1. */
static int matrix_add_cols(matrix_t *matrix)
{
    size_t old_capacity = matrix->capacity_cols;
    matrix->capacity_cols <<= 1;

    for (size_t i = 0; i < matrix->capacity_rows; ++i) {
        void **new_row = realloc(matrix->items[i], matrix->capacity_cols * sizeof(void *));
        if (new_row == NULL) return 1;
        
        matrix->items[i] = new_row;
        memset(matrix->items[i] + matrix->n_cols[i], 0, sizeof(void *) * (matrix->capacity_cols - old_capacity));
    }

    return 0;
}

/** @brief Adds new rows into a matrix. Returns 0 if successful, else returns 1. */
static int matrix_add_rows(matrix_t *matrix, const size_t rows_to_add)
{
    size_t old_capacity = matrix->capacity_rows;
    matrix->capacity_rows += rows_to_add;

    void ***new_items = realloc(matrix->items, matrix->capacity_rows * sizeof(void **));
    if (new_items == NULL) return 1;

    matrix->items = new_items;
    for (size_t i = old_capacity; i < matrix->capacity_rows; ++i) {
        matrix->items[i] = calloc(matrix->capacity_cols, sizeof(void *));
        if (matrix->items[i] == NULL) return 1;
    }

    size_t *new_cols = realloc(matrix->n_cols, matrix->capacity_rows * sizeof(size_t));
    if (new_cols == NULL) return 1;
    matrix->n_cols = new_cols;
    memset(matrix->n_cols + old_capacity, 0, sizeof(size_t) * (matrix->capacity_rows - old_capacity));

    return 0;
}

/** @brief  Sets the item at the given row and column of the matrix. Returns 0 if successful, else returns 1. */
static inline int matrix_set_raw(matrix_t *matrix, const size_t row, const size_t col, const void *item, const size_t itemsize)
{
    matrix->items[row][col] = malloc(itemsize);
    if (matrix->items[row][col] == NULL) return 1;
    memcpy(matrix->items[row][col], item, itemsize);

    return 0;
}

/** @brief Updates the length of the matrix. */
static inline void matrix_increase_length(matrix_t *matrix, const size_t row)
{
    ++(matrix->len);
    ++(matrix->n_cols[row]);
    if (row >= matrix->n_rows) matrix->n_rows = row + 1;
}

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH MATRIX_T                   */
/* *************************************************************************** */

matrix_t *matrix_new(void)
{
    return matrix_with_capacity(MATRIX_DEFAULT_CAPACITY_ROWS, MATRIX_DEFAULT_CAPACITY_COLS);
}


matrix_t *matrix_with_capacity(const size_t capacity_rows, const size_t capacity_cols)
{
    matrix_t *matrix = calloc(1, sizeof(matrix_t));
    if (matrix == NULL) return NULL;

    matrix->base_capacity_cols = capacity_cols;
    matrix->capacity_cols = capacity_cols;
    matrix->base_capacity_rows = capacity_rows;
    matrix->capacity_rows = capacity_rows;

    matrix->n_cols = calloc(matrix->capacity_rows, sizeof(size_t));
    if (matrix->n_cols == NULL) {
        free(matrix);
        return NULL;
    }

    matrix->items = calloc(matrix->capacity_rows, sizeof(void *));
    if (matrix->items == NULL) {
        free(matrix->n_cols);
        free(matrix);
        return NULL;
    }
    for (size_t i = 0; i < matrix->capacity_rows; ++i) {
        matrix->items[i] = calloc(matrix->capacity_cols, sizeof(void *));
        
        if (matrix->items[i] == NULL) {
            for (size_t j = 0; j < i; ++j) {
                free(matrix->items[j]);
            }
            
            free(matrix->n_cols);
            free(matrix->items);
            free(matrix);
            return NULL;
        }
        
    }

    return matrix;
}

void matrix_destroy(matrix_t *matrix)
{
    if (matrix == NULL) return;

    for (size_t i = 0; i < matrix->capacity_rows; ++i) {
        for (size_t j = 0; j < matrix->n_cols[i]; ++j) {
            free(matrix->items[i][j]);
        }
        free(matrix->items[i]);
    }

    free(matrix->n_cols);
    free(matrix->items);
    free(matrix);
}

int matrix_push(matrix_t *matrix, const size_t row, const void *item, const size_t itemsize)
{
    if (matrix == NULL) return 99;
    
    // adding rows
    if (row >= matrix->capacity_rows && matrix_add_rows(matrix, row - matrix->capacity_rows + 1) != 0) return 1;

    // adding columns
    if (matrix->n_cols[row] >= matrix->capacity_cols && matrix_add_cols(matrix) != 0) return 2;
    
    // pushing item into matrix
    if (matrix_set_raw(matrix, row, matrix->n_cols[row], item, itemsize) != 0) return 3;

    // increasing the matrix length
    matrix_increase_length(matrix, row);

    return 0;
}

int matrix_push_static(matrix_t *matrix, const void *item, const size_t itemsize)
{
    if (matrix == NULL) return 99;

    for (size_t i = 0; i < matrix->capacity_rows; ++i) {
        if (matrix->n_cols[i] == matrix->capacity_cols) continue;

        size_t index = matrix->n_cols[i];
        if (matrix_set_raw(matrix, i, index, item, itemsize) != 0) return 3;
        
        matrix_increase_length(matrix, i);
        return 0;
    }

    return 1;
}

void *matrix_get(const matrix_t *matrix, const size_t row, const size_t column)
{
    if (matrix == NULL) return NULL;
    if (row >= matrix->n_rows) return NULL;
    if (column >= matrix->n_cols[row]) return NULL;

    return matrix->items[row][column];
}


void matrix_map(matrix_t *matrix, void (*function)(void *, void *), void *pointer)
{
    if (matrix == NULL) return;

    for (size_t i = 0; i < matrix->n_rows; ++i) {
        for (size_t j = 0; j < matrix->n_cols[i]; ++j) {
            function(matrix->items[i][j], pointer);
        }
    }
}