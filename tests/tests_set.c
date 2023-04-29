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

    // adding items that are already present
    for (int i = 0; i < 100; ++i) {
        assert(set_add(set, &i, sizeof(int), sizeof(int)) == 0);
    }

    assert(set->allocated == 16384);

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

    return 0;
}