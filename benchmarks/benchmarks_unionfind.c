// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/set.h"
#include "../src/unionfind.h"

static int equal_int(const void *i1, const void *i2)
{
    return *(int *) i1 == *(int *) i2;
}

static set_t *set_fill(const int start, const int end)
{
    set_t *set = set_new(equal_int, hash_full);

    for (int i = start; i < end + 1; ++i) {
        set_add(set, &i, sizeof(int), sizeof(int));
    }

    return set;
}

static void benchmark_union_two_sets()
{
    printf("%s\n", "benchmark_union_two_sets");

    for (size_t i = 1; i <= 10; ++i) {

        size_t total_items = 100000 * i;
        printf("> total items: %lu\n", total_items);
    
        set_t *set1 = set_fill(1, total_items / 2);
        set_t *set2 = set_fill(total_items / 2 + 1, total_items);

        clock_t start = clock();

        set_t *set3 = set_union(set1, set2);

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf(">>> SET:        %f s\n", time_elapsed);

        // ****

        unionfind_t *sets = unionfind_with_capacity(total_items, equal_int, hash_full);
        unionfind_add_set(sets, set1);
        unionfind_add_set(sets, set2);
        assert(unionfind_len(sets) == total_items);

        int val1 = 1;
        int val2 = (int) total_items;

        start = clock();

        assert(unionfind_union(sets, &val1, &val2, sizeof(int), sizeof(int)) == 0);

        end = clock();
        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf(">>> UNION-FIND: %f s\n", time_elapsed);

        set_destroy(set1);
        set_destroy(set2);
        set_destroy(set3);
        unionfind_destroy(sets);
    }
}

int main(void)
{
    srand(time(NULL));

    benchmark_union_two_sets();

}