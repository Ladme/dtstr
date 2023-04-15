// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "str.h"



vec_t *str_split(const char *string, const char *delim)
{
    vec_t *vector = vec_new();
    if (vector == NULL) return NULL;

    if (string[0] == '\0') return vector;

    char *parse = strdup(string);

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
    char *d = string;

    do {
        while (isspace(*d)) {
            ++d;
        }
    *string = *d;
    string++;
    d++;
    } while (*string);
}