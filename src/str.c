// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "str.h"

/* *************************************************************************** */
/*                      FUNCTIONS FOR WORKING WITH STRINGS                     */
/* *************************************************************************** */

vec_t *str_split(const char *string, const char *delim)
{
    vec_t *vector = vec_new();
    if (vector == NULL) return NULL;

    if (string[0] == '\0') return vector;

    char *parse = malloc(strlen(string) + 1);
    strcpy(parse, string);

    // initialize strtok
    char *substring = NULL;
    if ((substring = strtok(parse, delim)) == NULL) {
        vec_destroy(vector);
        return NULL;
    }

    if (vec_push(vector, substring, strlen(substring) + 1) != 0) return NULL;

    // repeat strtok until the string is split completely
    while ((substring = strtok(NULL, delim)) != NULL) {
        if (vec_push(vector, substring, strlen(substring) + 1) != 0) return NULL;
    }

    free(parse);
    return vector;
}

vec_t *str_splitlines(const char *string) 
{ 
    return str_split(string, "\n"); 
}

vec_t *str_splitwhite(const char *string)
{
    return str_split(string, " \t\n\v\f\r");
}

vec_t *str_fragmentize(const char *string)
{
    size_t len = strlen(string);

    vec_t *vector = vec_fit(len);
    if (vector == NULL) return NULL;

    for (size_t i = 0; string[i]; ++i) {
        if (vec_push(vector, &(string[i]), 1) != 0) return NULL;
    }

    return vector;
}

void str_strip(char *string)
{
    char *p = string;
    int l = strlen(p);

    if (l == 0) return;

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;

    memmove(string, p, l + 1);    
}


void str_remwhite(char *string)
{
    char *out = string;

    while (*string != '\0') {
        if (!isspace(*string)) {
            *out = *string;
            ++out;
        }
        ++string;
    }

    *out = '\0';
}


/* *************************************************************************** */
/*                          FUNCTIONS FOR READING INPUT                        */
/* *************************************************************************** */

char *read_line(FILE *stream)
{
    char *buffer = malloc(INIT_BUFFER_LEN);
    if (buffer == NULL) return NULL;

    size_t size = INIT_BUFFER_LEN;
    size_t index = 0;
    int c = 0;

    while ((c = fgetc(stream)) != EOF) {
        if (c == '\n') break;

        // reallocate buffer
        if (index >= size - 1) {
            size *= 2;
            char *new_buffer = realloc(buffer, size);
            if (new_buffer == NULL) {
                free(buffer);
                return NULL;
            }

            buffer = new_buffer;
        }

        buffer[index++] = c;
    }

    if (index == 0 && c == EOF) {
        free(buffer);
        return NULL;
    }

    buffer[index] = 0;
    return buffer;
}


/* *************************************************************************** */
/*                        FUNCTIONS FOR PARSING STRINGS                        */
/* *************************************************************************** */

int_option_t str_parse_int(const char *string)
{
    int_option_t value = { .valid = 0, .value = 0 };
    char *endptr = NULL;

    long raw = strtol(string, &endptr, 10);

    if (*endptr == 0 && raw <= INT_MAX && raw >= INT_MIN) {
        value.value = (int) raw;
        value.valid = 1;
    }

    return value;
}

long_option_t str_parse_long(const char *string)
{
    long_option_t value = { .valid = 0, .value = 0 };
    char *endptr = NULL;

    value.value = strtol(string, &endptr, 10);
    
    if (*endptr == 0) value.valid = 1;
    return value;
}

sizet_option_t str_parse_sizet(const char *string)
{
    sizet_option_t value = { .valid = 0, .value = 0 };
    char *endptr = NULL;

    long long raw = strtoll(string, &endptr, 10);

    if (*endptr == 0 && raw >= 0) {
        value.value = (size_t) raw;
        value.valid = 1;
    }
    
    return value;
}

float_option_t str_parse_float(const char *string)
{
    float_option_t value = { .valid = 0, .value = 0.0 };
    char *endptr = NULL;

    value.value = strtof(string, &endptr);

    if (*endptr == 0) value.valid = 1;
    return value;
}


int int_option_unwrap(const int_option_t value) { return value.value; }
long long_option_unwrap(const long_option_t value) { return value.value; }
size_t sizet_option_unwrap(const sizet_option_t value) { return value.value; }
float float_option_unwrap(const float_option_t value) { return value.value; }

int int_option_check(const int_option_t value) { return value.valid; }
int long_option_check(const long_option_t value) { return value.valid; }
int sizet_option_check(const sizet_option_t value) { return value.valid; }
int float_option_check(const float_option_t value) { return value.valid; }