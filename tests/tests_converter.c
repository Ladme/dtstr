// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/converter.h"

#define UNUSED(x) (void)(x)

inline static int closef(const float a, const float b, const float limit)
{
    return (a > b - limit && a < b + limit);
}

static int test_str2matrix_integers(void)
{
    printf("%-40s", "test_str2matrix (integer) ");

    char string_matrix[] = "1,2,3,4,5\n6,7\n8\n9,10,11,12,13,14,15,16,17,18,19,20,21,22\nx\n23,24,25";
    matrix_t *matrix = str2matrix(string_matrix, "\n", ",", str2matrix_convert_int);

    assert(matrix);
    assert(matrix->len == 25);
    assert(matrix->n_rows == 6);
    assert(matrix->n_cols[0] == 5);
    assert(matrix->n_cols[1] == 2);
    assert(matrix->n_cols[2] == 1);
    assert(matrix->n_cols[3] == 14);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 3);

    assert(*(int *) matrix_get(matrix, 0, 0) == 1);
    assert(*(int *) matrix_get(matrix, 0, 1) == 2);
    assert(*(int *) matrix_get(matrix, 0, 2) == 3);
    assert(*(int *) matrix_get(matrix, 0, 3) == 4);
    assert(*(int *) matrix_get(matrix, 0, 4) == 5);

    assert(*(int *) matrix_get(matrix, 1, 0) == 6);
    assert(*(int *) matrix_get(matrix, 1, 1) == 7);

    assert(*(int *) matrix_get(matrix, 2, 0) == 8);

    for (size_t i = 0; i < matrix->n_cols[3]; ++i) {
        assert(*(int *) matrix_get(matrix, 3, i) == (int) i + 9);
    }
    
    assert(*(int *) matrix_get(matrix, 5, 0) == 23);
    assert(*(int *) matrix_get(matrix, 5, 1) == 24);
    assert(*(int *) matrix_get(matrix, 5, 2) == 25);

    // invalid indices
    assert(!matrix_get(matrix, 0, 5));
    assert(!matrix_get(matrix, 1, 2));
    assert(!matrix_get(matrix, 2, 1));
    assert(!matrix_get(matrix, 2, 2));
    assert(!matrix_get(matrix, 3, 14));
    assert(!matrix_get(matrix, 4, 0));
    assert(!matrix_get(matrix, 5, 3));

    matrix_destroy(matrix);
    printf("OK\n");
    return 0;    
}

static int test_str2matrix_floats(void)
{
    printf("%-40s", "test_str2matrix (float) ");

    char string_matrix[] = "1.0 2.2 3.5 4.3 5.1\n6.9 7.8\n8.3\n9.01 10.02 11.03 12.04 13.05 14.06 15.07 16.08 17.09 18.1 19.11 20.12 21.13 22.14\nx\n23.5 24.5 25.0";
    matrix_t *matrix = str2matrix(string_matrix, "\n", " ", str2matrix_convert_float);
    
    assert(matrix);
    assert(matrix->len == 25);
    assert(matrix->n_rows == 6);
    assert(matrix->n_cols[0] == 5);
    assert(matrix->n_cols[1] == 2);
    assert(matrix->n_cols[2] == 1);
    assert(matrix->n_cols[3] == 14);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 3);

    assert(closef(*(float *) matrix_get(matrix, 0, 0), 1.0, 0.0001));
    assert(closef(*(float *) matrix_get(matrix, 0, 1), 2.2, 0.0001));
    assert(closef(*(float *) matrix_get(matrix, 0, 2), 3.5, 0.0001));
    assert(closef(*(float *) matrix_get(matrix, 0, 3), 4.3, 0.0001));
    assert(closef(*(float *) matrix_get(matrix, 0, 4), 5.1, 0.0001));

    assert(closef(*(float *) matrix_get(matrix, 1, 0), 6.9, 0.0001));
    assert(closef(*(float *) matrix_get(matrix, 1, 1), 7.8, 0.0001));

    assert(closef(*(float *) matrix_get(matrix, 2, 0), 8.3, 0.0001));

    for (size_t i = 0; i < matrix->n_cols[3]; ++i) {
        assert(closef(*(float *) matrix_get(matrix, 3, i), (float) (i + 9) + ((float) (i + 1) / 100), 0.0001));
    }
    
    assert(closef(*(float *) matrix_get(matrix, 5, 0), 23.5, 0.0001));
    assert(closef(*(float *) matrix_get(matrix, 5, 1), 24.5, 0.0001));
    assert(closef(*(float *) matrix_get(matrix, 5, 2), 25.0, 0.0001));

    // invalid indices
    assert(!matrix_get(matrix, 0, 5));
    assert(!matrix_get(matrix, 1, 2));
    assert(!matrix_get(matrix, 2, 1));
    assert(!matrix_get(matrix, 2, 2));
    assert(!matrix_get(matrix, 3, 14));
    assert(!matrix_get(matrix, 4, 0));
    assert(!matrix_get(matrix, 5, 3));

    matrix_destroy(matrix);
    printf("OK\n");
    return 0;    
}

static int test_str2matrix_sizet(void)
{
    printf("%-40s", "test_str2matrix (size_t) ");

    char string_matrix[] = "1 2 3 4 5|6 7|8|9 10 11 12 13 14 15 16 17 18 19 20 21 22|x|23 24 25";
    matrix_t *matrix = str2matrix(string_matrix, "|", " ", str2matrix_convert_sizet);

    assert(matrix);
    assert(matrix->len == 25);
    assert(matrix->n_rows == 6);
    assert(matrix->n_cols[0] == 5);
    assert(matrix->n_cols[1] == 2);
    assert(matrix->n_cols[2] == 1);
    assert(matrix->n_cols[3] == 14);
    assert(matrix->n_cols[4] == 0);
    assert(matrix->n_cols[5] == 3);

    assert(*(size_t *) matrix_get(matrix, 0, 0) == 1);
    assert(*(size_t *) matrix_get(matrix, 0, 1) == 2);
    assert(*(size_t *) matrix_get(matrix, 0, 2) == 3);
    assert(*(size_t *) matrix_get(matrix, 0, 3) == 4);
    assert(*(size_t *) matrix_get(matrix, 0, 4) == 5);

    assert(*(size_t *) matrix_get(matrix, 1, 0) == 6);
    assert(*(size_t *) matrix_get(matrix, 1, 1) == 7);

    assert(*(size_t *) matrix_get(matrix, 2, 0) == 8);

    for (size_t i = 0; i < matrix->n_cols[3]; ++i) {
        assert(*(size_t *) matrix_get(matrix, 3, i) == i + 9);
    }
    
    assert(*(size_t *) matrix_get(matrix, 5, 0) == 23);
    assert(*(size_t *) matrix_get(matrix, 5, 1) == 24);
    assert(*(size_t *) matrix_get(matrix, 5, 2) == 25);

    // invalid indices
    assert(!matrix_get(matrix, 0, 5));
    assert(!matrix_get(matrix, 1, 2));
    assert(!matrix_get(matrix, 2, 1));
    assert(!matrix_get(matrix, 2, 2));
    assert(!matrix_get(matrix, 3, 14));
    assert(!matrix_get(matrix, 4, 0));
    assert(!matrix_get(matrix, 5, 3));

    matrix_destroy(matrix);
    printf("OK\n");
    return 0;    
}

static int test_str2matrix_strings(void)
{
    printf("%-40s", "test_str2matrix (string) ");

    char string_matrix[] = "r1c1 r1c2 r1c3, r2c1,r3c1 r3c2";
    matrix_t *matrix = str2matrix(string_matrix, ",", " ", str2matrix_convert_string);

    assert(matrix);
    assert(matrix->len == 6);
    assert(matrix->n_rows == 3);
    assert(matrix->n_cols[0] == 3);
    assert(matrix->n_cols[1] == 1);
    assert(matrix->n_cols[2] == 2);

    assert(strcmp(matrix_get(matrix, 0, 0), "r1c1") == 0);
    assert(strcmp(matrix_get(matrix, 0, 1), "r1c2") == 0);
    assert(strcmp(matrix_get(matrix, 0, 2), "r1c3") == 0);

    assert(strcmp(matrix_get(matrix, 1, 0), "r2c1") == 0);
    
    assert(strcmp(matrix_get(matrix, 2, 0), "r3c1") == 0);
    assert(strcmp(matrix_get(matrix, 2, 1), "r3c2") == 0);

    // invalid indices
    assert(!matrix_get(matrix, 0, 3));
    assert(!matrix_get(matrix, 1, 1));
    assert(!matrix_get(matrix, 2, 2));

    matrix_destroy(matrix);
    printf("OK\n");
    return 0;    
}

static inline void test_str2matrix_chars_check(const matrix_t *matrix)
{
    assert(matrix);
    assert(matrix->len == 26);
    assert(matrix->n_rows == 5);
    assert(matrix->n_cols[0] == 6);
    assert(matrix->n_cols[1] == 3);
    assert(matrix->n_cols[2] == 2);
    assert(matrix->n_cols[3] == 12);
    assert(matrix->n_cols[4] == 3);

    assert(*(char *) matrix_get(matrix, 0, 0) == 'a');
    assert(*(char *) matrix_get(matrix, 0, 1) == 'b');
    assert(*(char *) matrix_get(matrix, 0, 2) == 'c');
    assert(*(char *) matrix_get(matrix, 0, 3) == 'd');
    assert(*(char *) matrix_get(matrix, 0, 4) == 'e');
    assert(*(char *) matrix_get(matrix, 0, 5) == 'f');

    assert(*(char *) matrix_get(matrix, 1, 0) == 'g');
    assert(*(char *) matrix_get(matrix, 1, 1) == 'h');
    assert(*(char *) matrix_get(matrix, 1, 2) == 'i');

    assert(*(char *) matrix_get(matrix, 2, 0) == 'j');
    assert(*(char *) matrix_get(matrix, 2, 1) == 'k');

    assert(*(char *) matrix_get(matrix, 3, 0) == 'l');
    assert(*(char *) matrix_get(matrix, 3, 1) == 'm');
    assert(*(char *) matrix_get(matrix, 3, 2) == 'n');
    assert(*(char *) matrix_get(matrix, 3, 3) == 'o');
    assert(*(char *) matrix_get(matrix, 3, 4) == 'p');
    assert(*(char *) matrix_get(matrix, 3, 5) == 'q');
    assert(*(char *) matrix_get(matrix, 3, 6) == 'r');
    assert(*(char *) matrix_get(matrix, 3, 7) == 's');
    assert(*(char *) matrix_get(matrix, 3, 8) == 't');
    assert(*(char *) matrix_get(matrix, 3, 9) == 'u');
    assert(*(char *) matrix_get(matrix, 3, 10) == 'v');
    assert(*(char *) matrix_get(matrix, 3, 11) == 'w');

    assert(*(char *) matrix_get(matrix, 4, 0) == 'x');
    assert(*(char *) matrix_get(matrix, 4, 1) == 'y');
    assert(*(char *) matrix_get(matrix, 4, 2) == 'z');

    // invalid indices
    assert(!matrix_get(matrix, 0, 6));
    assert(!matrix_get(matrix, 1, 3));
    assert(!matrix_get(matrix, 2, 2));
    assert(!matrix_get(matrix, 3, 12));
    assert(!matrix_get(matrix, 4, 3));
}

static int test_str2matrix_chars(void)
{
    printf("%-40s", "test_str2matrix (char) ");

    char string_matrix[] = "abcdef|ghi|jk|lmnopqrstuvw|xyz";
    matrix_t *matrix = str2matrix(string_matrix, "|", "", str2matrix_convert_char);

    test_str2matrix_chars_check(matrix);

    matrix_destroy(matrix);
    printf("OK\n");
    return 0;    
}

static int test_str2matrix_chars_spaces(void)
{
    printf("%-40s", "test_str2matrix (char, spaces) ");

    char string_matrix[] = "a b c d e f| g h i |j k |l m n o p q r s t u v w| x y z";
    matrix_t *matrix = str2matrix(string_matrix, "|", " ", str2matrix_convert_char);

    test_str2matrix_chars_check(matrix);

    matrix_destroy(matrix);
    printf("OK\n");
    return 0;    
}

typedef struct person {
    char *name;
    size_t id;
    int age;
    float height;
} person_t;

static void *str2matrix_convert_person(const char *string) 
{ 
    vec_t *split = str_split(string, ":");
    if (!split || split->len != 4) {
        vec_destroy(split);
        return NULL;
    }

    char *name = vec_get(split, 0);

    sizet_option_t id_opt = str_parse_sizet(vec_get(split, 1));
    if (!sizet_option_check(id_opt)) { 
        vec_destroy(split); 
        return NULL;
    }
    size_t id = sizet_option_unwrap(id_opt);

    int_option_t age_opt = str_parse_int(vec_get(split, 2));
    if (!int_option_check(age_opt)) { 
        vec_destroy(split); 
        return NULL;
    }
    int age = int_option_unwrap(age_opt);

    float_option_t height_opt = str_parse_float(vec_get(split, 3));
    if (!float_option_check(height_opt)) { 
        vec_destroy(split);
        return NULL;
    }
    float height = float_option_unwrap(height_opt);

    person_t *person = malloc(sizeof(person_t));
    person->name = malloc(strlen(name) + 1);
    strcpy(person->name, name);
    person->id = id;
    person->age = age;
    person->height = height;

    vec_destroy(split);

    return person;
}

static void person_destroy_map(void *item, void *unused)
{
    UNUSED(unused);

    person_t *person = (person_t *) item;
    free(person->name);
}

static int test_str2matrix_structures(void)
{
    printf("%-40s", "test_str2matrix (structure) ");

    // person with name "Incorrect" will not be loaded because of incorrect format
    char string_matrix[] = "Mary:87252:65:163.4 Elizabeth:63252:36:171.5 Chris:65673:31:184.7\nJonathan:11432:22:179.5 Incorrect:x:22:179.5\nPatrick:65889:19:188.0 Julia:65552:21:153.2";
    matrix_t *matrix = str2matrix(string_matrix, "\n", " ", str2matrix_convert_person);


    assert(matrix);
    assert(matrix->len == 6);
    assert(matrix->n_rows == 3);
    assert(matrix->n_cols[0] == 3);
    assert(matrix->n_cols[1] == 1);
    assert(matrix->n_cols[2] == 2);

    person_t *p1 = (person_t *) matrix_get(matrix, 0, 0);
    person_t *p2 = (person_t *) matrix_get(matrix, 0, 1);
    person_t *p3 = (person_t *) matrix_get(matrix, 0, 2);

    person_t *p4 = (person_t *) matrix_get(matrix, 1, 0);

    person_t *p5 = (person_t *) matrix_get(matrix, 2, 0);
    person_t *p6 = (person_t *) matrix_get(matrix, 2, 1);

    assert(strcmp(p1->name, "Mary") == 0);
    assert(strcmp(p2->name, "Elizabeth") == 0);
    assert(strcmp(p3->name, "Chris") == 0);
    assert(strcmp(p4->name, "Jonathan") == 0);
    assert(strcmp(p5->name, "Patrick") == 0);
    assert(strcmp(p6->name, "Julia") == 0);

    assert(p1->id == 87252);
    assert(p2->id == 63252);
    assert(p3->id == 65673);
    assert(p4->id == 11432);
    assert(p5->id == 65889);
    assert(p6->id == 65552);

    assert(p1->age == 65);
    assert(p2->age == 36);
    assert(p3->age == 31);
    assert(p4->age == 22);
    assert(p5->age == 19);
    assert(p6->age == 21);

    assert(closef(p1->height, 163.4, 0.0001));
    assert(closef(p2->height, 171.5, 0.0001));
    assert(closef(p3->height, 184.7, 0.0001));
    assert(closef(p4->height, 179.5, 0.0001));
    assert(closef(p5->height, 188.0, 0.0001));
    assert(closef(p6->height, 153.2, 0.0001));

    // because the person structure contains dynamically allocated member `name`, we must deallocate it first manually
    matrix_map(matrix, person_destroy_map, NULL);

    matrix_destroy(matrix);
    printf("OK\n");
    return 0;    
}

int main(void)
{
    test_str2matrix_integers();
    test_str2matrix_floats();
    test_str2matrix_sizet();
    test_str2matrix_strings();
    test_str2matrix_chars();
    test_str2matrix_chars_spaces();
    test_str2matrix_structures();

    return 0;
}