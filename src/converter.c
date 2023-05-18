// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "converter.h"

/* *************************************************************************** */
/*               PUBLIC STR2* FUNCTIONS FOR STRING CONVERSIONS                 */
/* *************************************************************************** */

void *str_convert_string(const char *string) 
{ 
    char *copy = malloc(strlen(string) + 1);
    strcpy(copy, string);

    return copy; 
}

void *str_convert_char(const char *string)
{
    if (string[0] == '\0') return NULL;

    char *character = malloc(1);
    character[0] = string[0];
    return character;
}

void *str_convert_int(const char *string) 
{
    int_option_t option = str_parse_int(string);
    if (!int_option_check(option)) return NULL;

    int *value = malloc(sizeof(int));
    *value = int_option_unwrap(option);
    return value;
}

void *str_convert_float(const char *string)
{
    float_option_t option = str_parse_float(string);
    if (!float_option_check(option)) return NULL;

    float *value = malloc(sizeof(float));
    *value = float_option_unwrap(option);
    return value;
}

void *str_convert_sizet(const char *string)
{
    sizet_option_t option = str_parse_sizet(string);
    if (!sizet_option_check(option)) return NULL;

    size_t *value = malloc(sizeof(size_t));
    *value = sizet_option_unwrap(option);
    return value;
}

matrix_t *str2matrix(
        const char *string, 
        const char *row_delimiters, 
        const char *element_delimiters, 
        void* (*conversion_function)(const char *))
{
    vec_t *lines = str_split(string, row_delimiters);
    vec_t **elements = calloc(lines->len, sizeof(vec_t *));
    size_t max_cols = 0;

    // get the required size of the matrix
    for (size_t i = 0; i < lines->len; ++i) {
        if (element_delimiters[0] == '\0') elements[i] = str_fragmentize(lines->items[i]);
        else elements[i] = str_split(lines->items[i], element_delimiters);

        // if parsing fails
        if (elements[i] == NULL) {
            vec_destroy(lines);
            for (size_t j = 0; j < i; ++j) {
                vec_destroy(elements[j]);
            }
            free(elements);
            return NULL;
        }

        if (elements[i]->len > max_cols) max_cols = elements[i]->len;
    }

    // construct the matrix
    matrix_t *matrix = matrix_fit(lines->len, max_cols);
    if (matrix == NULL) {
        vec_destroy(lines);
        for (size_t i = 0; i < lines->len; ++i) {
            vec_destroy(elements[i]);
        }
        free(elements);

        return NULL;
    }

    matrix->n_rows = lines->len;

    // assign elements to matrix
    for (size_t i = 0; i < lines->len; ++i) {

        size_t total_row_len = elements[i]->len;
        for (size_t j = 0; j < elements[i]->len; ++j) {

            void *converted = conversion_function((char *) elements[i]->items[j]);
            if (converted == NULL) {
                --total_row_len;
                continue;
            }

            // assign item to the correct position in the matrix
            matrix->items[i][j] = converted;
            ++(matrix->len);
        }

        matrix->n_cols[i] = total_row_len;
        vec_destroy(elements[i]);
    }

    vec_destroy(lines);
    free(elements);

    return matrix;
}


vec_t *str2vec(const char *string, const char *delimiters, void* (*conversion_function)(const char *))
{
    vec_t *split = NULL;
    if (delimiters[0] == '\0') split = str_fragmentize(string);
    else split = str_split(string, delimiters);
    if (!split) return NULL;

    vec_t *vector = vec_fit(split->len);

    for (size_t i = 0; i < split->len; ++i) {
        void *converted = conversion_function((char *) split->items[i]);
        if (converted == NULL) {
            vec_destroy(vector);
            vec_destroy(split);
            return NULL;
        }

        vector->items[i] = converted;
        ++(vector->len);
    }

    vec_destroy(split);
    return vector;
}