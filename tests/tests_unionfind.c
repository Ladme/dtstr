// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/unionfind.h"

#define UNUSED(x) (void)(x)

static int equal_int(const void *i1, const void *i2)
{
    return *(int *) i1 == *(int *) i2;
}

static void print_int(void *item, void *unused)
{
    UNUSED(unused);

    printf("%d ", *(int *) item);
}

static void collect_int_to_vec(void *item, void *wrapped_vec)
{
    vec_t *vector = (vec_t *) wrapped_vec;

    vec_push(vector, item, sizeof(int));
}

static int equal_pointer2pointer_with_pointer(const void *pp, const void *p)
{
    return *(void **) pp == p;
}

/** @brief Checks that the root of target item has the expected rank. Returns 1 if it does, else returns 0. */
static inline int check_root_rank(unionfind_t *sets, void *item, const size_t expected_rank, const size_t itemsize)
{
    uf_entry_t search = { .item = item, .equal_function = sets->equal_function, .hashable = sets->hashable };
    uf_entry_t *found = (uf_entry_t *) set_get(sets->entries, &search, itemsize);

    uf_entry_t *root = found;
    while (root->parent != NULL) {
        root = root->parent;
    }

    return root->rank == expected_rank;
}

/** @brief Checks that the target NON-ROOT item has the expected rank. Returns 1 if it does or if it is a root of a set. Else returns 0. */
static inline int check_nonroot_rank(unionfind_t *sets, void *item, const size_t expected_rank, const size_t itemsize)
{
    uf_entry_t search = { .item = item, .equal_function = sets->equal_function, .hashable = sets->hashable };
    uf_entry_t *found = (uf_entry_t *) set_get(sets->entries, &search, itemsize);

    if (found->parent == NULL) return 1;

    return found->rank == expected_rank;
}

/************/

static int test_unionfind_destroy_nonexistent(void)
{
    printf("%-40s", "test_unionfind_destroy (nonexistent) ");

    unionfind_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_unionfind_new(void) 
{
    printf("%-40s", "test_unionfind_new ");

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    assert(sets);
    assert(sets->entries->allocated == 2 * UNIONFIND_DEFAULT_CAPACITY);
    assert(sets->equal_function == equal_int);
    assert(sets->hashable == hash_full);

    unionfind_destroy(sets);

    printf("OK\n");
    return 0;
}

static int test_unionfind_with_capacity(void) 
{
    printf("%-40s", "test_unionfind_with_capacity ");

    unionfind_t *sets = unionfind_with_capacity(32, equal_int, hash_full);

    assert(sets);
    assert(sets->entries->allocated == 64);
    assert(sets->equal_function == equal_int);
    assert(sets->hashable == hash_full);

    unionfind_destroy(sets);

    printf("OK\n");
    return 0;
}

static int test_unionfind_make_set(void)
{
    printf("%-40s", "test_unionfind_make_set ");

    int val = 667;
    assert(unionfind_make_set(NULL, &val, sizeof(int), sizeof(int)) == 99);

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    for (int i = 0; i < 100; ++i) {
        assert(unionfind_make_set(sets, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(sets->entries->len == 100);

    for (int i = 0; i < 100; ++i) {
        uf_entry_t entry = { .item = &i, .equal_function = sets->equal_function, .hashable = sets->hashable };
        assert(set_contains(sets->entries, &entry, sizeof(int)));
    }

    unionfind_destroy(sets);
    printf("OK\n");
    return 0;
}

static int test_unionfind_len(void)
{
    printf("%-40s", "test_unionfind_len ");

    assert(unionfind_len(NULL) == 0);

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    for (int i = 0; i < 100; ++i) {
        assert(unionfind_make_set(sets, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(unionfind_len(sets) == 100);

    unionfind_destroy(sets);
    printf("OK\n");
    return 0;
}

static int test_unionfind_map(void)
{
    printf("%-40s", "test_unionfind_map ");

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    for (int i = 0; i < 100; ++i) {
        assert(unionfind_make_set(sets, &i, sizeof(int), sizeof(int)) == 0);
    }

    vec_t *collected = vec_new();
    unionfind_map(sets, collect_int_to_vec, collected);

    assert(collected->len == 100);
    for (int i = 0; i < 100; ++i) {
        assert(vec_contains(collected, equal_int, &i));
    }

    vec_destroy(collected);

    unionfind_destroy(sets);
    printf("OK\n");
    return 0;
}

static int test_unionfind_find_set(void)
{
    printf("%-40s", "test_unionfind_find_set ");

    // non-existent set
    int val = 666;
    assert(unionfind_find_set(NULL, &val, sizeof(int)) == NULL);

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    for (int i = 0; i < 100; ++i) {
        assert(unionfind_make_set(sets, &i, sizeof(int), sizeof(int)) == 0);
    }

    for (int i = 99; i >= 0; --i) {
        assert(*(int *) unionfind_find_set(sets, &i, sizeof(int)) == i);
    }

    // search for non-existent items
    for (int i = 100; i < 105; ++i) {
        assert(unionfind_find_set(sets, &i, sizeof(int)) == NULL);
    }

    unionfind_destroy(sets);
    printf("OK\n");
    return 0;
}

static int test_unionfind_union(void)
{
    printf("%-40s", "test_unionfind_union ");

    // non-existent set
    int val1 = 666;
    int val2 = 420;
    assert(unionfind_union(NULL, &val1, &val2, sizeof(int), sizeof(int)) == 99);

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    for (int i = 0; i < 100; ++i) {
        assert(unionfind_make_set(sets, &i, sizeof(int), sizeof(int)) == 0);
    }

    // merge existing sets
    for (int i = 0; i < 99; i += 2) {
        int i1 = i + 1;
        assert(unionfind_union(sets, &i, &i1, sizeof(int), sizeof(int)) == 0);
        assert(unionfind_find_set(sets, &i, sizeof(int)) == unionfind_find_set(sets, &i1, sizeof(int)));
    }

    for (int i = 0; i < 98; i += 4) {
        int i2 = i + 2;
        assert(unionfind_union(sets, &i, &i2, sizeof(int), sizeof(int)) == 0);
        assert(unionfind_find_set(sets, &i, sizeof(int)) == unionfind_find_set(sets, &i2, sizeof(int)));
    }

    for (int i = 0; i < 96; i += 8) {
        int i4 = i + 4;
        assert(unionfind_union(sets, &i, &i4, sizeof(int), sizeof(int)) == 0);
        assert(unionfind_find_set(sets, &i, sizeof(int)) == unionfind_find_set(sets, &i4, sizeof(int)));
    }

    // try merging non-existing sets
    for (int i = 100; i < 105; ++i) {
        int i1 = i + 1;
        assert(unionfind_union(sets, &i, &i1, sizeof(int), sizeof(int)) == 1);
    }

    // try merging null sets
    assert(unionfind_union(sets, &val1, NULL, sizeof(int), sizeof(int)));
    assert(unionfind_union(sets, NULL, &val2, sizeof(int), sizeof(int)));
    assert(unionfind_union(sets, NULL, NULL, sizeof(int), sizeof(int)));

    unionfind_destroy(sets);
    printf("OK\n");
    return 0;
}

static int test_unionfind_find_share_set(void)
{
    printf("%-40s", "test_unionfind_share_set ");

    // non-existent set
    int val1 = 666;
    int val2 = 420;
    assert(!unionfind_share_set(NULL, &val1, &val2, sizeof(int), sizeof(int)));

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    for (int i = 0; i < 100; ++i) {
        assert(unionfind_make_set(sets, &i, sizeof(int), sizeof(int)) == 0);
    }

    // merge existing sets
    for (int i = 0; i < 99; i += 2) {
        int i1 = i + 1;
        assert(unionfind_union(sets, &i, &i1, sizeof(int), sizeof(int)) == 0);
        assert(unionfind_share_set(sets, &i, &i1, sizeof(int), sizeof(int)));

        if (i < 98) {
            int i2 = i + 2;
            assert(!unionfind_share_set(sets, &i, &i2, sizeof(int), sizeof(int)));
        }
        
    }

    for (int i = 0; i < 98; i += 4) {
        int i2 = i + 2;
        assert(unionfind_union(sets, &i, &i2, sizeof(int), sizeof(int)) == 0);
        assert(unionfind_share_set(sets, &i, &i2, sizeof(int), sizeof(int)));

        if (i < 90) {
            int i10 = i + 10;
            assert(!unionfind_share_set(sets, &i, &i10, sizeof(int), sizeof(int)));
        }
    }

    for (int i = 0; i < 96; i += 8) {
        int i4 = i + 4;
        assert(unionfind_union(sets, &i, &i4, sizeof(int), sizeof(int)) == 0);
        assert(unionfind_share_set(sets, &i, &i4, sizeof(int), sizeof(int)));

        if (i < 90) {
            int i10 = i + 10;
            assert(!unionfind_share_set(sets, &i, &i10, sizeof(int), sizeof(int)));
        }
    }

    // non-existent items
    for (int i = 100; i < 105; ++i) {
        int i1 = i + 1;
        assert(!unionfind_share_set(sets, &i, &i1, sizeof(int), sizeof(int)));
    }

    // NULL items
    assert(!unionfind_share_set(sets, &val1, NULL, sizeof(int), sizeof(int)));
    assert(!unionfind_share_set(sets, NULL, &val2, sizeof(int), sizeof(int)));
    assert(!unionfind_share_set(sets, NULL, NULL, sizeof(int), sizeof(int)));

    

    unionfind_destroy(sets);
    printf("OK\n");
    return 0;
}

static int test_unionfind_collect_sets(void)
{
    printf("%-40s", "test_unionfind_collect_sets ");

    vec_t *collected = unionfind_collect_sets(NULL);
    assert(collected);
    assert(collected->len == 0);
    vec_destroy(collected);

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    for (int i = 0; i < 100; ++i) {
        assert(unionfind_make_set(sets, &i, sizeof(int), sizeof(int)) == 0);
    }

    collected = unionfind_collect_sets(sets);

    assert(collected->len == 100);
    for (int i = 0; i < 100; ++i) {
        void *pointer_to_set = unionfind_find_set(sets, &i, sizeof(int));
        assert(vec_contains(collected, equal_pointer2pointer_with_pointer, pointer_to_set));
    }

    vec_destroy(collected);


    // merge half of the sets
    for (int i = 0; i < 99; i += 2) {
        int i1 = i + 1;
        assert(unionfind_union(sets, &i, &i1, sizeof(int), sizeof(int)) == 0);
    }

    collected = unionfind_collect_sets(sets);

    assert(collected->len == 50);
    vec_destroy(collected);

    unionfind_destroy(sets);
    printf("OK\n");
    return 0;
}

static int test_unionfind_add_set(void)
{
    printf("%-40s", "test_unionfind_add_set ");

    set_t *add1 = set_new(equal_int, hash_full);
    for (int i = 0; i < 100; ++i) {
        set_add(add1, &i, sizeof(int), sizeof(int));
    }

    set_t *add2 = set_new(equal_int, hash_full);
    for (int i = 100; i < 200; ++i) {
        set_add(add2, &i, sizeof(int), sizeof(int));
    }

    unionfind_t *sets = unionfind_new(equal_int, hash_full);

    // nonexistent unionfind/set
    assert(unionfind_add_set(NULL, add1) == 99);
    assert(unionfind_add_set(sets, NULL) == 99);
    assert(unionfind_add_set(NULL, NULL) == 99);

    int val1 = 84;
    int val2 = 125;

    // add first set
    assert(unionfind_add_set(sets, add1) == 0);
    assert(unionfind_len(sets) == 100);
    vec_t *collect = unionfind_collect_sets(sets);
    assert(collect->len == 1);
    vec_destroy(collect);

    // verify rank of root
    assert(check_root_rank(sets, &val1, 1, sizeof(int)));
    // verify rank of other items
    for (int i = 0; i < 100; ++i) {
        assert(check_nonroot_rank(sets, &i, 0, sizeof(int)));
    }

    // add second set
    assert(unionfind_add_set(sets, add2) == 0);
    assert(unionfind_len(sets) == 200);
    collect = unionfind_collect_sets(sets);
    assert(collect->len == 2);
    vec_destroy(collect);

    assert(check_root_rank(sets, &val1, 1, sizeof(int)));
    assert(check_root_rank(sets, &val2, 1, sizeof(int)));
    for (int i = 0; i < 200; ++i) {
        assert(check_nonroot_rank(sets, &i, 0, sizeof(int)));
    }

    set_destroy(add1);
    set_destroy(add2);

    // add one more value
    int val = 666;
    assert(unionfind_make_set(sets, &val, sizeof(int), sizeof(int)) == 0);
    assert(unionfind_len(sets) == 201);
    collect = unionfind_collect_sets(sets);
    assert(collect->len == 3);
    vec_destroy(collect);

    assert(check_root_rank(sets, &val1, 1, sizeof(int)));
    assert(check_root_rank(sets, &val2, 1, sizeof(int)));
    assert(check_root_rank(sets, &val, 0, sizeof(int)));
    for (int i = 0; i < 200; ++i) {
        assert(check_nonroot_rank(sets, &i, 0, sizeof(int)));
    }

    // union of add1 and add2
    assert(unionfind_union(sets, &val1, &val2, sizeof(int), sizeof(int)) == 0);
    assert(unionfind_len(sets) == 201);
    collect = unionfind_collect_sets(sets);
    assert(collect->len == 2);
    vec_destroy(collect);

    // add partly overlapping values
    set_t *add3 = set_new(equal_int, hash_full);
    for (int i = 150; i < 250; ++i) {
        set_add(add3, &i, sizeof(int), sizeof(int));
    }

    assert(unionfind_add_set(sets, add3) == 0);
    set_destroy(add3);

    assert(unionfind_len(sets) == 251);
    collect = unionfind_collect_sets(sets);
    assert(collect->len == 3);
    vec_destroy(collect);

    // different partly overlapping values
    set_t *add4 = set_new(equal_int, hash_full);
    for (int i = 300; i > 200; --i) {
        set_add(add4, &i, sizeof(int), sizeof(int));
    }

    assert(unionfind_add_set(sets, add4) == 0);
    set_destroy(add4);

    assert(unionfind_len(sets) == 302);
    collect = unionfind_collect_sets(sets);
    assert(collect->len == 4);
    vec_destroy(collect);

    // fully overlapping values
    set_t *add5 = set_new(equal_int, hash_full);
    for (int i = 100; i > 200; ++i) {
        set_add(add5, &i, sizeof(int), sizeof(int));
    }

    assert(unionfind_add_set(sets, add5) == 0);
    set_destroy(add5);

    assert(unionfind_len(sets) == 302);
    collect = unionfind_collect_sets(sets);
    assert(collect->len == 4);
    vec_destroy(collect);


    unionfind_destroy(sets);

    printf("OK\n");
    return 0;
}


int main(void) 
{
    test_unionfind_destroy_nonexistent();
    test_unionfind_new();
    test_unionfind_with_capacity();

    test_unionfind_make_set();
    test_unionfind_len();

    test_unionfind_find_set();
    test_unionfind_union();

    test_unionfind_map();

    test_unionfind_find_share_set();
    test_unionfind_collect_sets();
    test_unionfind_add_set();

    return 0;
}