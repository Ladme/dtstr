// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/set.h"
#include "../src/vector.h"

static int equal_int(const void *i1, const void *i2)
{
    return *(int *) i1 == *(int *) i2;
}

static set_t *set_fill(const int items)
{
    set_t *set = set_new(equal_int, hash_full);

    for (int i = 0; i < items; ++i) {
        set_add(set, &i, sizeof(int), sizeof(int));
    }

    return set;
}

static vec_t *vec_fill_sequence(const int items)
{
    vec_t *vector = vec_new();

    for (int i = 0; i < items; ++i) {
        vec_push(vector, &i, sizeof(int));
    }

    return vector;
}

static void benchmark_set_add_vs_vec_push(const size_t items)
{
    printf("%s\n", "benchmark_set_add_vs_vec_push (adding unique items)");

    for (size_t i = 0; i <= 10; ++i) {

        // set
        size_t prefilled = i * 1000;
        set_t *set = set_fill(prefilled);

        size_t initial = set->len;

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand() % (prefilled + items);
            
            set_add(set, &random, sizeof(int), sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> SET: prefilled with %12lu items, adding %12lu items (%lu of which were unique): %f s \n", 
            prefilled, items, set->len - initial, time_elapsed);

        set_destroy(set);

        // vector
        vec_t *vector = vec_fill_sequence(prefilled);

        initial = vector->len;

        start = clock();
        for (size_t j = 0; j < items; ++j) {
            int random = rand() % (prefilled + items);

            if (vec_find(vector, equal_int, &random) == NULL) {
                vec_push(vector, &random, sizeof(int));
            }
        }

        end = clock();
        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> VEC: prefilled with %12lu items, adding %12lu items (%lu of which were unique): %f s \n", 
            prefilled, items, vector->len - initial, time_elapsed);

        vec_destroy(vector);
        printf("\n");

    }
    printf("\n");
}

static void benchmark_set_union_sl(void)
{
    printf("%s\n", "benchmark_set_union (small + large) ");

    set_t *small = set_new(equal_int, hash_full);
    for (int i = -1000; i < 0; ++i) set_add(small, &i, sizeof(int), sizeof(int));

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 10000;
        set_t *large = set_fill(prefilled);

        clock_t start = clock();

        set_t *union1 = set_union(small, large);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> union of %12lu items and %12lu items (final: %12lu): %f s \n", 
            small->len, large->len, union1->len, time_elapsed);

        set_destroy(union1);
        set_destroy(large);

    }
    printf("\n");

    set_destroy(small);

}

static void benchmark_set_union_ls(void)
{
    printf("%s\n", "benchmark_set_union (large + small) ");

    set_t *small = set_new(equal_int, hash_full);
    for (int i = -1000; i < 0; ++i) set_add(small, &i, sizeof(int), sizeof(int));

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 10000;
        set_t *large = set_fill(prefilled);

        clock_t start = clock();

        set_t *union1 = set_union(large, small);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> union of %12lu items and %12lu items (final: %12lu): %f s \n", 
            small->len, large->len, union1->len, time_elapsed);

        set_destroy(union1);
        set_destroy(large);

    }
    printf("\n");

    set_destroy(small);

}


int main(void)
{
    srand(time(NULL));

    benchmark_set_add_vs_vec_push(10000);
    benchmark_set_union_sl();
    benchmark_set_union_ls();


}