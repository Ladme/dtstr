// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of dynamic two-dimensional matrix.
// Matrix is always allocated as a rectangular matrix but the real length of the individual rows can differ between each other.

#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct matrix {
    size_t len;
    size_t n_rows;
    size_t *n_cols;
    size_t capacity_rows;
    size_t capacity_cols;
    size_t base_capacity_rows;
    size_t base_capacity_cols;
    void ***items;
} matrix_t;

#define MATRIX_DEFAULT_CAPACITY_ROWS 8UL
#define MATRIX_DEFAULT_CAPACITY_COLS 8UL

/**
 * @brief Creates a new `matrix_t` structure and allocates memory for it.
 *
 * @note - To release the memory allocated for `matrix_t`, use the `matrix_destroy` function.
 * @note - Allocates space for `MATRIX_DEFAULT_CAPACITY_ROWS` rows and `MATRIX_DEFAULT_CAPACITY_COLS` cols.
 *         This space is dynamically expanded when needed but expanding the matrix is a costly operation.
 *         You can preallocate space for a matrix using `matrix_with_capacity` function.
 * 
 * @return A pointer to the newly created `matrix_t` structure if successful; otherwise, NULL.
 */
matrix_t *matrix_new(void);


/**
 * @brief Creates a new `matrix_t` structure and allocates memory of specific shape.
 *
 * @param capacity_rows     Initial capacity for the number of rows.
 * @param capacity_cols     Initial capacity for the number of columns.
 * 
 * @note - To release the memory allocated for `matrix_t`, use the `matrix_destroy` function.
 * 
 * @return A pointer to the newly created `matrix_t` structure if successful; otherwise, NULL.
 */
matrix_t *matrix_with_capacity(const size_t capacity_rows, const size_t capacity_cols);


/**
 * @brief Properly deallocates memory for the given `matrix` and destroys the `matrix_t` structure.
 *
 * @param matrix    The `matrix_t` structure to destroy.
 */
void matrix_destroy(matrix_t *matrix);


/**
 * @brief Adds an item to the end of the row in the matrix, expanding the matrix if needed.
 *
 * @param matrix    Pointer to the matrix to which the item should be pushed.
 * @param row       The row index of the matrix where the item should be pushed.
 * @param item      Pointer to the item that should be pushed into the matrix.
 * @param itemsize  The size of the item in bytes.
 * 
 * @note - If the item is pushed into a row that does not currently exist in the matrix, 
 * the matrix will be expanded to accommodate the new row.
 * @note - If the item is pushed into a row that is already at its maximum capacity, 
 * the matrix will be expanded to increase the capacity of ALL rows.
 * @note - Note that matrix always expands in such a way that all rows have the same capacity.
 * 
 * @return 0 if successful, 1 if memory allocation for rows fails, 2 if memory allocation for columns fails,
 * 3 if memory allocation for the item fails, or 99 if matrix pointer is null.
 */
int matrix_push(matrix_t *matrix, const size_t row, const void *item, const size_t itemsize);


/**
 * @brief Adds an item to the first available spot in the matrix, without expanding the matrix capacity.
 *
 * @param matrix    Pointer to the matrix to which the item should be pushed.
 * @param item      Pointer to the item that should be pushed into the matrix.
 * @param itemsize  The size of the item in bytes.
 *
 * @note - This function does not expand the capacity of the matrix, so it will not add new rows or columns.
 * @note - The first available spot is defined as the first element in the first row that has not reached its
 * maximum capacity, going from left to right and top to bottom.
 * 
 * @return 0 if successful, 1 if no spots are available in the matrix, 3 if memory allocation for the item fails,
 * or 99 if matrix pointer is null.
 */
int matrix_push_static(matrix_t *matrix, const void *item, const size_t itemsize);


/**
 * @brief Returns a pointer to the item at the specified row and column of the matrix.
 *
 * @param matrix    Pointer to the matrix from which the item should be retrieved.
 * @param row       The row index of the item to be retrieved.
 * @param column    The column index of the item to be retrieved.
 * 
 * @return Pointer to the item at the specified row and column of the matrix, or NULL if the matrix pointer is null,
 * or the row or column is out of bounds.
 */
void *matrix_get(const matrix_t *matrix, const size_t row, const size_t column);


/**
 * @brief Applies a given function to each element of the matrix in row-major order.
 *
 * @param matrix        Pointer to the matrix to be mapped.
 * @param function      Function to apply
 * @param pointer       Pointer to value that the function can operate on
 *
 * @note - The items of the matrix are traversed in row-major order, meaning that the function is applied to
 * all the elements of the first row, then to all the elements of the second row, and so on, going from left
 * to right and top to bottom.
 */
void matrix_map(matrix_t *matrix, void (*function)(void *, void *), void *pointer);

#endif /* MATRIX_H */