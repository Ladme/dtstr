// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "vector.h"

/* *************************************************************************** */
/*                    PRIVATE FUNCTIONS FOR VEC_T SORTING                      */
/* *************************************************************************** */

/** @brief Swaps two items in a vector. */
inline static void vec_swap(vec_t *vector, const size_t i, const size_t j)
{
    void *tmp = vector->items[i];
    vector->items[i] = vector->items[j];
    vector->items[j] = tmp;
}

/** Insertion sort applied only to a part of the vector. */
static void sort_insertion_part(vec_t *vector, const size_t first, const size_t last, int (*compare_function)(const void *, const void *))
{
    for (size_t i = first; i < last + 1; ++i) {
        size_t j = i;
        void* current = vector->items[j];

        while (j > first && compare_function(vector->items[j - 1], current) > 0) {
            vector->items[j] = vector->items[j - 1];
            --j;
        }

        vector->items[j] = current;
    }
}

/** @brief Partitioning algorithm for quicksort. */
static size_t partition(vec_t *vector, const size_t first, const size_t last, int (*compare_function)(const void *, const void *))
{
    // we select pivot in the middle of the array to avoid worst case complexity on sorted data
    size_t pivot_index = (first + last) / 2;  
    const void *pivot = vector->items[pivot_index];
    size_t i = first;

    // swap the pivot with the last item, so it does not disrupt sorting
    vec_swap(vector, pivot_index, last);

    for (size_t j = first; j < last; ++j) {
        if (compare_function(pivot, vector->items[j]) > 0) {
            if (i != j) vec_swap(vector, i, j);
            ++i;
        }
    }

    if (i != last) vec_swap(vector, i, last);
    return i;
}

/** @brief Actual implementation of quicksort. */
static void quicksort(vec_t *vector, const size_t first, const size_t last, int (*compare_function)(const void *, const void *))
{
    if (first >= last || first < 0 || last > vector->len) return;

    // switch to insertion sort if the segment is too small
    if (last - first < 8) {
        sort_insertion_part(vector, first, last, compare_function);
        return;
    }

    size_t pindex = partition(vector, first, last, compare_function);

    quicksort(vector, first, pindex - 1, compare_function);
    quicksort(vector, pindex + 1, last, compare_function);
}


/* *************************************************************************** */
/*                     PUBLIC FUNCTIONS FOR VEC_T SORTING                      */
/* *************************************************************************** */

int vec_sort_selection(vec_t *vector, int (*compare_function)(const void *, const void *))
{
    if (vector == NULL) return 99;
    if (vector->len <= 1) return 0;

    size_t min_index = 0;
    
    for (size_t i = 0; i < vector->len - 1; ++i) {
        min_index = i;
        for (size_t j = i + 1; j < vector->len; ++j) {
            if (compare_function(vector->items[min_index], vector->items[j]) > 0) {
                min_index = j;
            }
        }

        vec_swap(vector, i, min_index);
    }

    return 0;
}


int vec_sort_bubble(vec_t *vector, int (*compare_function)(const void *, const void *))
{
    if (vector == NULL) return 99;
    if (vector->len <= 1) return 0;

    for (size_t i = 0; i < vector->len; ++i) {

        int swapped = 0;
        for (size_t j = 0; j < vector->len - i - 1; ++j) {
            if (compare_function(vector->items[j], vector->items[j + 1]) > 0) {
                vec_swap(vector, j, j + 1);
                swapped = 1;
            }
        }

        if (!swapped) return 0;
    }

    return 0;
}

int vec_sort_insertion(vec_t *vector, int (*compare_function)(const void *, const void *))
{
    if (vector == NULL) return 99;

    sort_insertion_part(vector, 0, vector->len - 1, compare_function);

    return 0;
}

int vec_sort_quicknaive(vec_t *vector, int (*compare_function)(const void *, const void *))
{
    if (vector == NULL) return 99;
    if (vector->len <= 1) return 0;

    quicksort(vector, 0, vector->len - 1, compare_function);

    return 0;
}


int vec_sort_quick(vec_t *vector, int (*qsort_compare_function)(const void *, const void *))
{
    if (vector == NULL) return 99;

    qsort((void *) vector->items, vector->len, sizeof(void *), qsort_compare_function);

    return 0;
}