// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/vector.h"

static vec_t *vec_fill_rand(const int items)
{
    vec_t *vector = vec_new();

    for (int i = 0; i < items; ++i) {
        int random = rand();

        vec_push(vector, &random, sizeof(int));
    }

    return vector;
}

static vec_t *vec_fill_sorted(const size_t items)
{
    vec_t *vector = vec_new();

    for (size_t i = 0; i < items; ++i) {
        int value = (int) i;

        vec_push(vector, &value, sizeof(int));
    }

    return vector;
}

static vec_t *vec_fill_specific(const size_t items, int *specific, const size_t n_specific)
{
    vec_t *vector = vec_new();

    for (size_t i = 0; i < items - n_specific; ++i) {
        int random = rand();

        vec_push(vector, &random, sizeof(int));
    }

    for (size_t i = 0; i < n_specific; ++i) {
        int random_index = rand() % (items - n_specific);
        vec_insert(vector, &specific[i], sizeof(int), random_index);
    }
    
    return vector;
}

static void benchmark_vec_push(size_t items)
{
    printf("%s\n", "benchmark_vec_push [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();

            vec_push(vector, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, pushing %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_insert(size_t items)
{
    printf("%s\n", "benchmark_vec_insert [O(n)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand();

            vec_insert(vector, &random, sizeof(int), 0);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, inserting %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_get(size_t items)
{
    printf("%s\n", "benchmark_vec_get [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        for (size_t j = 1; j < items; ++j) {
            size_t random_index = rand() % prefilled;

            vec_get(vector, random_index);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, getting %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_pop(size_t items)
{
    printf("%s\n", "benchmark_vec_pop [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        for (size_t j = 1; j < items; ++j) {
            free(vec_pop(vector));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, popping %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_remove(size_t items)
{
    printf("%s\n", "benchmark_vec_remove [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        for (size_t j = 1; j < items; ++j) {
            size_t random_index = rand() % prefilled;

            free(vec_remove(vector, random_index));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, removing %12lu items: %f s\n", prefilled, items, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}


static int filter_function(const void *data)
{
    return *((int *) data) % 2;
}

static void benchmark_vec_filter_mut()
{
    printf("%s\n", "benchmark_vec_filter_mut [O(n^2)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 10000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        size_t removed = vec_filter_mut(vector, filter_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, filtered out %12lu items: %f s\n", prefilled, removed, time_elapsed);

        vec_destroy(vector);
    }
    printf("\n");
}

static void benchmark_vec_filter(void)
{
    printf("%s\n", "benchmark_vec_filter [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 1000000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        vec_t *filtered = vec_filter(vector, filter_function, sizeof(int));

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, filtered in %12lu items: %f s\n", prefilled, filtered->len, time_elapsed);

        vec_destroy(vector);
        vec_destroy(filtered);
    }

    printf("\n");
}

static int equal_function(const void *data, const void *target)
{
    return *((int *) data) == *((int *) target);
}


static void benchmark_vec_find(void)
{
    printf("%s\n", "benchmark_vec_find [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        int items_to_find[1000] = {0};
        for (size_t j = 0; j < 1000; ++j) items_to_find[j] = rand() % 1000;

        vec_t *vector = vec_fill_specific(prefilled, items_to_find, 1000);

        clock_t start = clock();

        for (size_t j = 0; j < 1000; ++j) {
            vec_find(vector, equal_function, &items_to_find[j]);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, performing 1000 searches: %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static int compare_function(const void *first, const void *second)
{
    return *((int *) first) - *((int *) second);
}

static void benchmark_vec_find_bsearch(void)
{
    printf("%s\n", "benchmark_vec_find_bsearch [O(log n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_sorted(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < 1000000; ++j) {
            int item_to_find = rand() % prefilled;
            vec_find_bsearch(vector, compare_function, &item_to_find);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, performing 1 million searches: %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmarks_vec_push_preallocated(void)
{
    printf("%s\n", "benchmark_vec_push (default vs. preallocated)");

    size_t items[] = {100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    for (size_t j = 0; j < 7; ++j) {

        clock_t start = clock();
        vec_t *vector = vec_new();

        for (size_t i = 0; i < items[j]; ++i) {
            vec_push(vector, &i, sizeof(size_t));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> pushing %12lu items, base capacity %12lu: %f s\n", items[j], VEC_DEFAULT_CAPACITY, time_elapsed);

        vec_destroy(vector);

        //***

        start = clock();
        vector = vec_with_capacity(items[j]);

        for (size_t i = 0; i < items[j]; ++i) {
            vec_push(vector, &i, sizeof(size_t));
        }

        end = clock();
        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> pushing %12lu items, base capacity %12lu: %f s\n", items[j], items[j], time_elapsed);

        vec_destroy(vector);

        printf("\n");
    }
}


static void benchmark_vec_slicecpy(const size_t slice_start, const size_t slice_end)
{
    printf("%s\n", "benchmark_vec_slicecpy [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 1000000;
        vec_t *vector = vec_fill_sorted(prefilled);

        clock_t start = clock();

        vec_t *slice = vec_slicecpy(vector, slice_start, slice_end, sizeof(int));
        assert(slice->len == 400000);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, slicing from %lu to %lu: %f s\n", prefilled, slice_start, slice_end, time_elapsed);

        vec_destroy(slice);
        vec_destroy(vector);
    }

    printf("\n");
}

/* *************************************************************************** */
/*                      BENCHMARKS OF SORTING ALGORITHMS                       */
/* *************************************************************************** */

static void benchmark_vec_sort_selection(void)
{
    printf("%s\n", "benchmark_vec_sort_selection");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 5000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        vec_sort_selection(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_selection_sorted(void)
{
    printf("%s\n", "benchmark_vec_sort_selection (sorted data)");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 5000;
        vec_t *vector = vec_fill_sorted(prefilled);

        clock_t start = clock();

        vec_sort_selection(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_bubble(void)
{
    printf("%s\n", "benchmark_vec_sort_bubble");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 5000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        vec_sort_bubble(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_bubble_sorted(void)
{
    printf("%s\n", "benchmark_vec_sort_bubble (sorted data)");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_sorted(prefilled);

        clock_t start = clock();

        vec_sort_bubble(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_insertion(void)
{
    printf("%s\n", "benchmark_vec_sort_insertion");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 5000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        vec_sort_insertion(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_insertion_sorted(void)
{
    printf("%s\n", "benchmark_vec_sort_insertion (sorted data)");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_sorted(prefilled);

        clock_t start = clock();

        vec_sort_insertion(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_quicknaive(void)
{
    printf("%s\n", "benchmark_vec_sort_quicknaive");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        vec_sort_quicknaive(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_quicknaive_sorted(void)
{
    printf("%s\n", "benchmark_vec_sort_quicknaive (sorted data)");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_sorted(prefilled);

        clock_t start = clock();

        vec_sort_quicknaive(vector, compare_function);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static int compare_function_qsort(const void *first, const void *second)
{
    return **((int **) first) - **((int **) second);
}

static void benchmark_vec_sort_quick(void)
{
    printf("%s\n", "benchmark_vec_sort_quick");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_rand(prefilled);

        clock_t start = clock();

        vec_sort_quick(vector, compare_function_qsort);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}

static void benchmark_vec_sort_quick_sorted(void)
{
    printf("%s\n", "benchmark_vec_sort_quick (sorted data)");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        vec_t *vector = vec_fill_sorted(prefilled);

        clock_t start = clock();

        vec_sort_quick(vector, compare_function_qsort);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }

    printf("\n");
}


static void benchmark_vec_sort_quicknaive_and_find(void)
{
    printf("%s\n", "benchmark_vec_sort_quicknaive_and_find ");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        int items_to_find[10000] = {0};
        for (size_t j = 0; j < 10000; ++j) items_to_find[j] = rand() % 10000;

        vec_t *vector = vec_fill_specific(prefilled, items_to_find, 10000);

        clock_t start = clock();

        vec_sort_quicknaive(vector, compare_function);

        for (size_t j = 0; j < 10000; ++j) {
            vec_find_bsearch(vector, compare_function, &items_to_find[j]);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, performing 10,0000 searches: %f s\n", prefilled, time_elapsed);

        vec_destroy(vector);
    }
}


int main(void)
{
    benchmark_vec_push(100000);
    benchmark_vec_get(100000);
    benchmark_vec_insert(10000);
    benchmark_vec_pop(100000);
    benchmark_vec_remove(10000);
    benchmark_vec_filter_mut();
    benchmark_vec_filter();
    benchmark_vec_find();
    benchmark_vec_find_bsearch();
    benchmark_vec_slicecpy(100000, 500000);
    benchmarks_vec_push_preallocated();

    benchmark_vec_sort_selection();
    benchmark_vec_sort_selection_sorted();
    benchmark_vec_sort_bubble();
    benchmark_vec_sort_bubble_sorted();
    benchmark_vec_sort_insertion();
    benchmark_vec_sort_insertion_sorted();
    benchmark_vec_sort_quicknaive();
    benchmark_vec_sort_quicknaive_sorted();
    benchmark_vec_sort_quick();
    benchmark_vec_sort_quick_sorted();
    benchmark_vec_sort_quicknaive_and_find();

    return 0;

}