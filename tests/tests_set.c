// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/set.h"

#define UNUSED(x) (void)(x)

typedef struct {
    float value_x;
    size_t hash_value;
    char some_char;
} test_struct_t;

static const void *select_hash_structure(const void *structure)
{
    test_struct_t *converted = (test_struct_t *) structure;

    return &(converted->hash_value);
}

static void print_string(void *item, void *unused)
{
    UNUSED(unused);

    printf("%s ", (char *) item);
}

static inline void set_print_string(set_t *set)
{
    set_map(set, print_string, NULL);
    printf("\n");
}

static void print_int(void *item, void *unused)
{
    UNUSED(unused);

    printf("%d ", *(int *) item);
}

static inline void set_print_int(set_t *set)
{
    set_map(set, print_int, NULL);
    printf("\n");
}

static int equal_string(const void *str1, const void *str2)
{
    if (strcmp((char *) str1, (char *) str2) == 0) return 1;
    
    return 0;
}

static int equal_structure(const void *s1, const void *s2)
{
    test_struct_t *structure1 = (test_struct_t *) s1;
    test_struct_t *structure2 = (test_struct_t *) s2;

    return (structure1->value_x == structure2->value_x && structure1->hash_value == structure2->hash_value && structure1->some_char == structure2->some_char);
}

static int equal_int(const void *i1, const void *i2)
{
    return *(int *) i1 == *(int *) i2;
}

static int compare_int(const void *i1, const void *i2)
{
    return *(int *) i1 - *(int *) i2;
}

inline static int closef(const float a, const float b, const float limit)
{
    return (a > b - limit && a < b + limit);
}

/* DO NOT USE; USE hash_full */
static const void *hash_int(const void *item) { return item; }

/* ****** */

static int test_set_destroy_nonexistent(void)
{
    printf("%-40s", "test_set_destroy (nonexistent) ");

    set_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_set_new(void) 
{
    printf("%-40s", "test_set_new ");

    set_t *set = set_new(equal_string, hash_full);

    assert(set);
    assert(set->allocated == SET_DEFAULT_CAPACITY * 2);
    assert(set->available == SET_DEFAULT_CAPACITY);
    assert(set->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(set->equal_function == equal_string);
    assert(set->len == 0);

    set_destroy(set);

    printf("OK\n");
    return 0;
}

static int test_set_with_capacity(void) 
{
    printf("%-40s", "test_set_with_capacity ");

    set_t *set = set_with_capacity(32, equal_string, hash_full);

    assert(set);
    assert(set->allocated ==  64);
    assert(set->available == 32);
    assert(set->base_capacity == 64);
    assert(set->equal_function == equal_string);
    assert(set->len == 0);

    set_destroy(set);

    printf("OK\n");
    return 0;
}


static int test_set_add_strings(void)
{
    printf("%-40s", "test_set_add (strings) ");

    // working with invalid set
    char item[] = "some item";
    assert(set_add(NULL, item, strlen(item) + 1, strlen(item)) == 99);

    set_t *set = set_with_capacity(32, equal_string, hash_full);

    // do not change the keys or the base capacity
    // sun & this clash (1)
    // linked_list, number3, reasonable clash (9)
    // beta, hashtag clash (39)
    // something, alpha clash (43)
    // array does not clash with anything (38)

    char *items[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};

    for (size_t i = 0; i < 10; ++i) {
        assert(set_add(set, items[i], strlen(items[i]) + 1, strlen(items[i])) == 0);
    }

    assert(set->len == 10);

    // assert length of the individual linked lists
    assert(set->items[1]->len == 2);
    assert(set->items[9]->len == 3);
    assert(set->items[38]->len == 1);
    assert(set->items[39]->len == 2);
    assert(set->items[43]->len == 2);
    for (size_t i = 0; i < 64; ++i) {
        if (i != 1 && i != 9 && i != 38 && i != 39 && i != 43) {
            assert(set->items[i] == NULL);
        }
    }

    // check specific items
    assert(strcmp((*(set_entry_t **) set->items[9]->head->data)->item , "reasonable") == 0);
    assert(strcmp((*(set_entry_t **) set->items[9]->head->next->data)->item, "number3") == 0);
    assert(strcmp((*(set_entry_t **) set->items[9]->head->next->next->data)->item, "linked_list") == 0);
    assert((*(set_entry_t **) set->items[9]->head->data)->itemsize == 11 );
    assert((*(set_entry_t **) set->items[9]->head->next->data)->itemsize == 8 );
    assert((*(set_entry_t **) set->items[9]->head->next->next->data)->itemsize == 12 );

    assert(set->available == 27);

    //set_print_string(set);

    // add item that is already present in the set
    assert(set_add(set, items[2], strlen(items[2]) + 1, strlen(items[2])) == 0);
    // nothing should change

    assert(set->len == 10);

    assert(set->items[1]->len == 2);
    assert(set->items[9]->len == 3);
    assert(set->items[38]->len == 1);
    assert(set->items[39]->len == 2);
    assert(set->items[43]->len == 2);
    for (size_t i = 0; i < 64; ++i) {
        if (i != 1 && i != 9 && i != 38 && i != 39 && i != 43) {
            assert(set->items[i] == NULL);
        }
    }

    assert(strcmp((*(set_entry_t **) set->items[9]->head->data)->item , "reasonable") == 0);
    assert(strcmp((*(set_entry_t **) set->items[9]->head->next->data)->item, "number3") == 0);
    assert(strcmp((*(set_entry_t **) set->items[9]->head->next->next->data)->item, "linked_list") == 0);
    assert((*(set_entry_t **) set->items[9]->head->data)->itemsize == 11 );
    assert((*(set_entry_t **) set->items[9]->head->next->data)->itemsize == 8 );
    assert((*(set_entry_t **) set->items[9]->head->next->next->data)->itemsize == 12 );

    assert(set->available == 27);

    //set_print_string(set);

    set_destroy(set);

    printf("OK\n");
    return 0;
}


static int test_set_add_structures(void)
{
    printf("%-40s", "test_set_add (structures) ");

    set_t *set = set_with_capacity(16, equal_structure, select_hash_structure);

    // hashes for structure1 and structure3 clash
    test_struct_t structure1 = {.value_x = 42.864, .hash_value = 143787373, .some_char = 'c'};
    test_struct_t structure2 = {.value_x = 8.1243, .hash_value = 95680988, .some_char = 'a'};
    test_struct_t structure3 = {.value_x = 666.666, .hash_value = 666, .some_char = 'f'};

    assert(set_add(set, &structure1, sizeof(test_struct_t), sizeof(size_t)) == 0);
    assert(set_add(set, &structure2, sizeof(test_struct_t), sizeof(size_t)) == 0);
    assert(set_add(set, &structure3, sizeof(test_struct_t), sizeof(size_t)) == 0);

    assert(set->len == 3);

    assert(set->available == 14);

    assert(set->items[21]->len == 2);
    assert(set->items[30]->len == 1);
    for (size_t i = 0; i < 32; ++i) {
        if (i != 21 && i != 30) {
            assert(set->items[i] == NULL);
        }
    }

    // this should do nothing as structure1 is already present in the set
    assert(set_add(set, &structure1, sizeof(test_struct_t), sizeof(size_t)) == 0);

    assert(set->len == 3);

    assert(set->available == 14);

    assert(set->items[21]->len == 2);
    assert(set->items[30]->len == 1);
    for (size_t i = 0; i < 32; ++i) {
        if (i != 21 && i != 30) {
            assert(set->items[i] == NULL);
        }
    }

    // check specific items
    test_struct_t *s1 = (test_struct_t *) (*(set_entry_t **) set->items[21]->head->next->data)->item;
    test_struct_t *s2 = (test_struct_t *) (*(set_entry_t **) set->items[30]->head->data)->item;
    test_struct_t *s3 = (test_struct_t *) (*(set_entry_t **) set->items[21]->head->data)->item;

    assert(closef(s1->value_x, 42.864, 0.0001));
    assert(closef(s2->value_x, 8.1243, 0.0001));
    assert(closef(s3->value_x, 666.666, 0.0001));
    assert(s1->hash_value == 143787373);
    assert(s2->hash_value == 95680988);
    assert(s3->hash_value == 666);
    assert(s1->some_char == 'c');
    assert(s2->some_char == 'a');
    assert(s3->some_char == 'f');

    set_destroy(set);

    printf("OK\n");
    return 0;
}


static int test_set_add_large(void)
{
    printf("%-40s", "test_set_add (large) ");

    set_t *set = set_new(equal_int, hash_full);

    for (int i = 0; i < 10000; ++i) {
        assert(set_add(set, &i, sizeof(int), sizeof(int)) == 0);
    }

    //dict_print_sizet(dict);
    assert(set->allocated == 16384);
    assert(set->len == 10000);

    // adding items that are already present
    for (int i = 0; i < 100; ++i) {
        assert(set_add(set, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(set->allocated == 16384);
    assert(set->len == 10000);

    // check sum of all integer
    int sum = 0;
    for (size_t i = 0; i < set->allocated; ++i) {
        if (set->items[i] == NULL) continue;

        dnode_t *node = set->items[i]->head;
        while (node != NULL) {

            if (node->data != NULL) sum += *(int *) (*(set_entry_t **) node->data)->item;
            node = node->next;
        }
    }

    assert(sum == 49995000);
   
    set_destroy(set);
    printf("OK\n");
    return 0;
}

static int test_set_contains(void)
{
    printf("%-40s", "test_set_contains ");

    // non-existent set
    int val = 5;
    assert(set_contains(NULL, &val, sizeof(int)) == 0);

    set_t *set = set_new(equal_int, hash_full);

    for (int i = 0; i < 1000; ++i) {
        assert(set_add(set, &i, sizeof(int), sizeof(int)) == 0);
    }

    for (int i = 999; i >= 0; --i) {
        assert(set_contains(set, &i, sizeof(int)) == 1);
    }

    for (int i = 1000; i < 1100; ++i) {
        assert(set_contains(set, &i, sizeof(int)) == 0);
    }

    set_destroy(set);
    printf("OK\n");
    return 0;
}


static int test_set_collect(void)
{
    printf("%-40s", "test_set_collect ");

    // collect from non-existent set
    vec_t *vector = set_collect(NULL);
    assert(vec_len(vector) == 0);
    vec_destroy(vector);

    set_t *set = set_new(equal_int, hash_full);

    // collect from empty set
    vector = set_collect(set);
    assert(vec_len(vector) == 0);
    vec_destroy(vector);

    for (int i = 0; i < 1000; ++i) {
        assert(set_add(set, &i, sizeof(int), sizeof(int)) == 0);
    }

    vector = set_collect(set);
    set_destroy(set);

    vec_sort_quicknaive(vector, compare_int);
    for (int i = 0; i < 1000; ++i) {
        //printf("%d ", *(int *) vec_get(vector, i));
        assert(*(int *) vec_get(vector, i) == i);
    }

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_set_len(void)
{
    printf("%-40s", "test_set_len ");

    // len of non-existent set
    assert(set_len(NULL) == 0);

    set_t *set = set_new(equal_int, hash_full);

    // len of empty set
    assert(set_len(set) == 0);

    for (int i = 0; i < 1000; ++i) {
        assert(set_add(set, &i, sizeof(int), sizeof(int)) == 0);
        assert(set_len(set) == (size_t) i + 1);
    }

    set_destroy(set);

    printf("OK\n");
    return 0;
}

static int test_set_remove_strings(void)
{
    printf("%-40s", "test_set_remove (strings) ");

    char *items[] = {"sun", "linked_list", "number3", "beta", "something",
                    "reasonable", "array", "alpha", "hashtag", "this"};

    assert(set_remove(NULL, items[0], strlen(items[0])) == 99);

    set_t *set = set_with_capacity(32, equal_string, hash_full);

    for (size_t i = 0; i < 10; ++i) {
        assert(set_add(set, items[i], strlen(items[i]) + 1, strlen(items[i])) == 0);
    }

    assert(set->available == 27);

    // attempt to remove non-existent item
    char nonexistent[] = "nonexistent";
    assert(set_remove(set, nonexistent, strlen(nonexistent)) == 2);

    // delete all keys
    for (int i = 9; i >= 0; --i) {
        assert(set_remove(set, items[i], strlen(items[i])) == 0);
        assert(set_len(set) == (size_t) i);
        assert(!set_contains(set, items[i], strlen(items[i])));
        for (int j = 0; j < i; ++j) {
            assert(set_contains(set, items[j], strlen(items[j])));
        }
    }

    assert(set->available == 32);

    set_destroy(set);
    printf("OK\n");
    return 0;
}

static int test_set_add_remove_large(void)
{
    printf("%-40s", "test_set_add_remove (large) ");

    set_t *set = set_new(equal_int, hash_full);

    for (int i = 0; i < 10000; ++i) {
        assert(set_add(set, &i, sizeof(int), sizeof(int)) == 0);
    }
    assert(set->allocated == 16384);

    for (int i = 9999; i >= 0; --i) {
        assert(set_remove(set, &i, sizeof(int)) == 0);
    }

    assert(set->allocated == SET_DEFAULT_CAPACITY * 2);

    for (int i = 0; i < 10000; ++i) {
        assert(set_add(set, &i, sizeof(int), sizeof(int)) == 0);
    }
    assert(set->allocated == 16384);

    set_destroy(set);
    printf("OK\n");
    return 0;
}

static int test_set_copy(void)
{
    printf("%-40s", "test_set_copy ");

    // copy non-existent set
    assert(set_copy(NULL) == NULL);

    set_t *set = set_new(equal_string, hash_full);

    // copy empty set
    set_t *copy = set_copy(set);
    assert(set_len(copy) == 0);
    assert(set->allocated == copy->allocated);
    assert(set->available == copy->available);
    assert(set->base_capacity == copy->base_capacity);
    assert(set->equal_function == copy->equal_function);
    assert(set->hashable == copy->hashable);

    set_destroy(copy);

    for (int i = 0; i < 1000; ++i) {

        char string[20] = "";
        sprintf(string, "string%d", i);
        assert(set_add(set, string, strlen(string) + 1, strlen(string)) == 0);
    }

    assert(set_len(set) == 1000);

    copy = set_copy(set);
    assert(set_len(copy) == 1000);
    assert(set->allocated == copy->allocated);
    assert(set->available == copy->available);
    assert(set->base_capacity == copy->base_capacity);
    assert(set->equal_function == copy->equal_function);
    assert(set->hashable == copy->hashable);

    for (int i = 0; i < 1000; ++i) {
        char string[20] = "";
        sprintf(string, "string%d", i);
        assert(set_contains(copy, string, strlen(string)));
    }

    // validate the order of the items
    vec_t *original = set_collect(set);
    vec_t *copied = set_collect(copy);
    for (int i = 0; i < 1000; ++i) {
        assert(strcmp(original->items[i], copied->items[i]) == 0);
    }

    vec_destroy(original);
    vec_destroy(copied);

    //set_print_string(set);
    //set_print_string(copy);

    set_destroy(set);
    set_destroy(copy);
    printf("OK\n");
    return 0;
}

static int test_set_equal(void)
{
    printf("%-40s", "test_set_equal ");

    // compare non-existent sets
    assert(set_equal(NULL, NULL));

    set_t *set1 = set_new(equal_int, hash_full);
    
    for (int i = 0; i < 1000; ++i) {
        assert(set_add(set1, &i, sizeof(int), sizeof(int)) == 0);
    }

    // compare existing set with null set
    assert(!set_equal(set1, NULL));
    assert(!set_equal(NULL, set1));

    set_t *set2 = set_with_capacity(2000, equal_int, hash_full);

    for (int i = 0; i < 10; ++i) {
        assert(set_add(set2, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(!set_equal(set1, set2));

    for (int i = 10; i < 1000; ++i) {
        assert(set_add(set2, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(set_equal(set1, set2));

    set_t *set3 = set_copy(set2);

    assert(set_equal(set1, set3));
    assert(set_equal(set3, set1));

    set_t *set4 = set_new(equal_int, hash_full);
    for (int i = 500; i < 1500; ++i) {
        assert(set_add(set4, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(set_len(set1) == set_len(set4));
    assert(!set_equal(set1, set4));

    set_t *set5 = set_new(equal_int, hash_full);
    for (int i = 0; i < 1500; ++i) {
        assert(set_add(set5, &i, sizeof(int), sizeof(int)) == 0);
    }
    
    assert(!set_equal(set1, set5));


    set_destroy(set1);
    set_destroy(set2);
    set_destroy(set3);
    set_destroy(set4);
    set_destroy(set5);
    printf("OK\n");
    return 0;
}


static int test_set_subset(void)
{
    printf("%-40s", "test_set_subset ");

    // compare non-existent sets
    assert(set_subset(NULL, NULL));

    set_t *set1 = set_new(equal_int, hash_full);
    
    for (int i = 0; i < 1000; ++i) {
        assert(set_add(set1, &i, sizeof(int), sizeof(int)) == 0);
    }

    // compare existing set with null set
    assert(!set_subset(set1, NULL));
    assert(set_subset(NULL, set1));

    set_t *set2 = set_with_capacity(2000, equal_int, hash_full);

    for (int i = 0; i < 10; ++i) {
        assert(set_add(set2, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(!set_subset(set1, set2));
    assert(set_subset(set2, set1));

    for (int i = 10; i < 1000; ++i) {
        assert(set_add(set2, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(set_subset(set1, set2));
    assert(set_subset(set2, set1));

    set_t *set3 = set_new(equal_int, hash_full);

    for (int i = -300; i < 999; ++i) {
        assert(set_add(set3, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(!set_subset(set1, set3));
    assert(!set_subset(set3, set1));

    int add = 999;
    assert(set_add(set3, &add, sizeof(int), sizeof(int)) == 0);
    assert(set_subset(set1, set3));
    assert(!set_subset(set3, set1));

    set_destroy(set1);
    set_destroy(set2);
    set_destroy(set3);

    printf("OK\n");
    return 0;
}


static int test_set_union(void)
{
    printf("%-40s", "test_set_union ");

    // union of two NULL sets
    assert(set_union(NULL, NULL) == NULL);

    set_t *small = set_new(equal_int, hash_full);
    set_t *large = set_with_capacity(1000, equal_int, hash_full);

    set_t *incompatible1 = set_new(equal_string, hash_full);
    set_t *incompatible2 = set_new(equal_int, hash_int);

    for (int i = 0; i < 16; ++i) {
        assert(set_add(small, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(set_union(small, incompatible1) == NULL);
    assert(set_union(small, incompatible2) == NULL);

    set_destroy(incompatible1);
    set_destroy(incompatible2);

    for (int i = 5; i < 1000; ++i) {
        assert(set_add(large, &i, sizeof(int), sizeof(int)) == 0);
    }

    set_t *union1a = set_union(small, NULL);
    set_t *union1b = set_union(NULL, small);
    set_t *union2a = set_union(NULL, large);
    set_t *union2b = set_union(large, NULL);

    assert(set_equal(small, union1a));
    assert(set_equal(union1b, small));
    assert(set_equal(large, union2a));
    assert(set_equal(union2b, large));

    assert(union1a->base_capacity == small->base_capacity);
    assert(union1b->base_capacity == small->base_capacity);
    assert(union2a->base_capacity == large->base_capacity);
    assert(union2b->base_capacity == large->base_capacity);

    set_destroy(union1a);
    set_destroy(union1b);
    set_destroy(union2a);
    set_destroy(union2b);

    set_t *union3 = set_union(small, large);
    set_t *union4 = set_union(large, small);

    assert(set_equal(union3, union4));
    assert(union3->base_capacity == large->base_capacity);
    assert(union4->base_capacity == large->base_capacity);

    set_t *union_of_unions = set_union(union3, union4);

    assert(set_equal(union3, union_of_unions));
    assert(set_equal(union4, union_of_unions));
    assert(union_of_unions->base_capacity == large->base_capacity);

    set_t *union_with_original = set_union(union3, large);
    assert(set_equal(union3, union_with_original));
    assert(union_with_original->base_capacity == large->base_capacity);

    set_destroy(union3);
    set_destroy(union4);
    set_destroy(union_of_unions);
    set_destroy(union_with_original);
    set_destroy(small);
    set_destroy(large);

    printf("OK\n");
    return 0;
}

static int test_set_intersection(void)
{
    printf("%-40s", "test_set_intersection ");

    // intersection of two NULL sets
    assert(set_intersection(NULL, NULL) == NULL);

    set_t *small = set_new(equal_int, hash_full);
    set_t *large = set_with_capacity(1000, equal_int, hash_full);

    // intersection of two empty sets
    set_t *empty = set_intersection(small, large);
    assert(empty->len == 0);
    assert(empty->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(empty->equal_function == small->equal_function);
    assert(empty->hashable == small->hashable);
    set_destroy(empty);

    set_t *incompatible1 = set_new(equal_string, hash_full);
    set_t *incompatible2 = set_new(equal_int, hash_int);

    for (int i = 0; i < 16; ++i) {
        assert(set_add(small, &i, sizeof(int), sizeof(int)) == 0);
    }

    // intersection with incompatible sets
    assert(set_intersection(small, incompatible1) == NULL);
    assert(set_intersection(small, incompatible2) == NULL);

    set_destroy(incompatible1);
    set_destroy(incompatible2);

    for (int i = 5; i < 1000; ++i) {
        assert(set_add(large, &i, sizeof(int), sizeof(int)) == 0);
    }

    // intersection of valid set with null set
    assert(set_intersection(small, NULL) == NULL);
    assert(set_intersection(NULL, small) == NULL);

    // intersection of two valid sets with non-empty intersection
    set_t *inter1 = set_intersection(small, large);
    assert(inter1->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(inter1->len == 11);
    assert(inter1->equal_function == small->equal_function);
    assert(inter1->hashable == small->hashable);
    for (int i = 5; i < 16; ++i) {
        assert(set_contains(inter1, &i, sizeof(int)));
    }

    set_destroy(inter1);
    set_destroy(small);

    // intersection of two valid sets with empty intersection
    set_t *different = set_new(equal_int, hash_full);
    for (int i = -70; i < 0; ++i) {
        assert(set_add(different, &i, sizeof(int), sizeof(int)) == 0);
    }
    
    set_t *inter2 = set_intersection(large, different);
    assert(inter2->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(inter2->len == 0);
    assert(inter2->equal_function == large->equal_function);
    assert(inter2->hashable == large->hashable);

    set_destroy(inter2);
    set_destroy(different);

    // intersection of two valid sets that are identical
    set_t *large_equal = set_with_capacity(4096, equal_int, hash_full);
    for (int i = 5; i < 1000; ++i) {
        assert(set_add(large_equal, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(set_equal(large, large_equal));

    set_t *inter3 = set_intersection(large, large_equal);
    assert(inter3->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(inter3->len == large->len);
    assert(inter3->equal_function == large->equal_function);
    assert(inter3->hashable == large->hashable);
    assert(set_equal(large, inter3));
    assert(set_equal(large_equal, inter3));

    set_destroy(large);
    set_destroy(large_equal);
    set_destroy(inter3);
    

    printf("OK\n");
    return 0;
}


static int test_set_difference(void)
{
    printf("%-40s", "test_set_difference ");

    // difference between two NULL sets
    assert(set_difference(NULL, NULL) == NULL);

    set_t *small = set_new(equal_int, hash_full);
    set_t *large = set_with_capacity(1000, equal_int, hash_full);

    // difference between two empty sets
    set_t *empty = set_difference(small, large);
    assert(empty->len == 0);
    assert(empty->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(empty->equal_function == small->equal_function);
    assert(empty->hashable == small->hashable);
    set_destroy(empty);

    set_t *incompatible1 = set_new(equal_string, hash_full);
    set_t *incompatible2 = set_new(equal_int, hash_int);

    for (int i = 0; i < 16; ++i) {
        assert(set_add(small, &i, sizeof(int), sizeof(int)) == 0);
    }

    // difference between incompatible sets
    assert(set_difference(small, incompatible1) == NULL);
    assert(set_difference(small, incompatible2) == NULL);

    set_destroy(incompatible1);
    set_destroy(incompatible2);

    for (int i = 5; i < 1000; ++i) {
        assert(set_add(large, &i, sizeof(int), sizeof(int)) == 0);
    }

    // difference between a valid set and a null set
    set_t *diff0 = set_difference(small, NULL);
    assert(set_equal(diff0, small));
    assert(diff0->base_capacity == small->base_capacity);
    assert(set_difference(NULL, small) == NULL);

    set_destroy(diff0);

    // difference between two valid sets with non-empty difference
    set_t *diff1 = set_difference(large, small);
    assert(diff1->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(diff1->len == 984);
    assert(diff1->equal_function == small->equal_function);
    assert(diff1->hashable == small->hashable);
    for (int i = 16; i < 1000; ++i) {
        assert(set_contains(diff1, &i, sizeof(int)));
    }

    set_destroy(diff1);
    set_destroy(small);

    // difference between two valid sets with empty result
    set_t *large_equal = set_with_capacity(4096, equal_int, hash_full);
    for (int i = 5; i < 1000; ++i) {
        assert(set_add(large_equal, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(set_equal(large, large_equal));

    set_t *diff2 = set_difference(large, large_equal);
    assert(diff2->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(diff2->len == 0);
    assert(diff2->equal_function == large->equal_function);
    assert(diff2->hashable == large->hashable);

    set_destroy(large_equal);
    set_destroy(diff2);

    // difference between two disjoint sets
    set_t *different = set_new(equal_int, hash_full);
    for (int i = -70; i < 0; ++i) {
        assert(set_add(different, &i, sizeof(int), sizeof(int)) == 0);
    }

    set_t *diff3 = set_difference(large, different);
    assert(diff3->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(diff3->len == large->len);
    assert(diff3->equal_function == large->equal_function);
    assert(diff3->hashable == large->hashable);
    assert(set_equal(large, diff3));
    set_destroy(diff3);

    set_t *diff4 = set_difference(different, large);
    assert(diff4->base_capacity == SET_DEFAULT_CAPACITY * 2);
    assert(diff4->len == different->len);
    assert(diff4->equal_function == different->equal_function);
    assert(diff4->hashable == different->hashable);
    assert(set_equal(different, diff4));
    set_destroy(diff4);

    set_destroy(large);
    set_destroy(different);    

    printf("OK\n");
    return 0;
}

// TODO: map function test


int main(void) 
{
    test_set_destroy_nonexistent();
    test_set_new();
    test_set_with_capacity();

    test_set_add_strings();
    test_set_add_structures();
    test_set_add_large();

    test_set_contains();
    test_set_collect();
    test_set_len();

    test_set_remove_strings();
    test_set_add_remove_large();

    test_set_copy();
    test_set_equal();
    test_set_subset();

    test_set_union();
    test_set_intersection();
    test_set_difference();

    return 0;
}