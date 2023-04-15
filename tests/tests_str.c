// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/str.h"

inline static void print_vec_string(const vec_t *vector)
{
    for (size_t i = 0; i < vector->len; ++i) {
        printf("'%s', ", (char *) vec_get(vector, i));
    }
    printf("\n");
}

static int test_str_split(void)
{
    printf("%-40s", "test_str_split ");

    char string1[] = "This is an example string number 1";

    vec_t *split = str_split(string1, " ");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 7);
    assert(!strcmp((char *) vec_get(split, 0), "This"));
    assert(!strcmp((char *) vec_get(split, 1), "is"));
    assert(!strcmp((char *) vec_get(split, 2), "an"));
    assert(!strcmp((char *) vec_get(split, 3), "example"));
    assert(!strcmp((char *) vec_get(split, 4), "string"));
    assert(!strcmp((char *) vec_get(split, 5), "number"));
    assert(!strcmp((char *) vec_get(split, 6), "1"));

    vec_destroy(split);

    assert(!strcmp(string1, "This is an example string number 1"));

    /****/
    char string2[] = " This    is an example    string number 2   ";

    split = str_split(string2, " ");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 7);
    assert(!strcmp((char *) vec_get(split, 0), "This"));
    assert(!strcmp((char *) vec_get(split, 1), "is"));
    assert(!strcmp((char *) vec_get(split, 2), "an"));
    assert(!strcmp((char *) vec_get(split, 3), "example"));
    assert(!strcmp((char *) vec_get(split, 4), "string"));
    assert(!strcmp((char *) vec_get(split, 5), "number"));
    assert(!strcmp((char *) vec_get(split, 6), "2"));

    vec_destroy(split);

    assert(!strcmp(string2, " This    is an example    string number 2   "));

    /****/
    char string3[] = "This;is;an;example;string;number;3";

    split = str_split(string3, ";");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 7);
    assert(!strcmp((char *) vec_get(split, 0), "This"));
    assert(!strcmp((char *) vec_get(split, 1), "is"));
    assert(!strcmp((char *) vec_get(split, 2), "an"));
    assert(!strcmp((char *) vec_get(split, 3), "example"));
    assert(!strcmp((char *) vec_get(split, 4), "string"));
    assert(!strcmp((char *) vec_get(split, 5), "number"));
    assert(!strcmp((char *) vec_get(split, 6), "3"));

    vec_destroy(split);

    assert(!strcmp(string3, "This;is;an;example;string;number;3"));

    /****/
    char string4[] = ";This;;;;is;an;example;;;;string;number;4;;;";

    split = str_split(string4, ";");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 7);
    assert(!strcmp((char *) vec_get(split, 0), "This"));
    assert(!strcmp((char *) vec_get(split, 1), "is"));
    assert(!strcmp((char *) vec_get(split, 2), "an"));
    assert(!strcmp((char *) vec_get(split, 3), "example"));
    assert(!strcmp((char *) vec_get(split, 4), "string"));
    assert(!strcmp((char *) vec_get(split, 5), "number"));
    assert(!strcmp((char *) vec_get(split, 6), "4"));

    vec_destroy(split);

    assert(!strcmp(string4, ";This;;;;is;an;example;;;;string;number;4;;;"));

    /****/
    char string5[] = " This  ; is an example ;; string number 5  ;";

    split = str_split(string5, " ;");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 7);
    assert(!strcmp((char *) vec_get(split, 0), "This"));
    assert(!strcmp((char *) vec_get(split, 1), "is"));
    assert(!strcmp((char *) vec_get(split, 2), "an"));
    assert(!strcmp((char *) vec_get(split, 3), "example"));
    assert(!strcmp((char *) vec_get(split, 4), "string"));
    assert(!strcmp((char *) vec_get(split, 5), "number"));
    assert(!strcmp((char *) vec_get(split, 6), "5"));

    vec_destroy(split);

    assert(!strcmp(string5, " This  ; is an example ;; string number 5  ;"));

    /****/
    char string6[] = "NA ;;large #@Number o@f \ndelimiters will\tbe.used hQere| ";

    split = str_split(string6, " ;#@N\n.Q|\t");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 11);
    assert(!strcmp((char *) vec_get(split, 0), "A"));
    assert(!strcmp((char *) vec_get(split, 1), "large"));
    assert(!strcmp((char *) vec_get(split, 2), "umber"));
    assert(!strcmp((char *) vec_get(split, 3), "o"));
    assert(!strcmp((char *) vec_get(split, 4), "f"));
    assert(!strcmp((char *) vec_get(split, 5), "delimiters"));
    assert(!strcmp((char *) vec_get(split, 6), "will"));
    assert(!strcmp((char *) vec_get(split, 7), "be"));
    assert(!strcmp((char *) vec_get(split, 8), "used"));
    assert(!strcmp((char *) vec_get(split, 9), "h"));
    assert(!strcmp((char *) vec_get(split, 10), "ere"));

    vec_destroy(split);

    assert(!strcmp(string6, "NA ;;large #@Number o@f \ndelimiters will\tbe.used hQere| "));

    /****/
    char string7[] = "Nothing to split";

    split = str_split(string7, ";");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 1);
    assert(!strcmp((char *) vec_get(split, 0), "Nothing to split"));

    vec_destroy(split);

    assert(!strcmp(string7, "Nothing to split"));

    /****/
    char string8[] = "";

    split = str_split(string8, " ");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 0);

    vec_destroy(split);

    assert(!strcmp(string8, ""));

    /****/
    char string9[] = "Example line 9";

    split = str_split(string9, "");
    //print_vec_string(split);
    assert(split);
    assert(split->len == 1);
    assert(!strcmp((char *) vec_get(split, 0), "Example line 9"));

    vec_destroy(split);

    assert(!strcmp(string9, "Example line 9"));

    printf("OK\n");
    return 0;
}


static int test_str_splitlines(void)
{
    printf("%-40s", "test_str_splitlines ");

    char string1[] = "Example line 1\nExample line 2\nExample line 3\nExample line 4\n";

    vec_t *split = str_splitlines(string1);
    //print_vec_string(split);
    assert(split);
    assert(split->len == 4);
    assert(!strcmp((char *) vec_get(split, 0), "Example line 1"));
    assert(!strcmp((char *) vec_get(split, 1), "Example line 2"));
    assert(!strcmp((char *) vec_get(split, 2), "Example line 3"));
    assert(!strcmp((char *) vec_get(split, 3), "Example line 4"));

    vec_destroy(split);

    assert(!strcmp(string1, "Example line 1\nExample line 2\nExample line 3\nExample line 4\n"));

    /****/
    char string2[] = "\nExample line 1\n\nExample line 2\nExample line 3\n\nExample line 4\n";

    split = str_splitlines(string2);
    //print_vec_string(split);
    assert(split);
    assert(split->len == 4);
    assert(!strcmp((char *) vec_get(split, 0), "Example line 1"));
    assert(!strcmp((char *) vec_get(split, 1), "Example line 2"));
    assert(!strcmp((char *) vec_get(split, 2), "Example line 3"));
    assert(!strcmp((char *) vec_get(split, 3), "Example line 4"));

    vec_destroy(split);

    assert(!strcmp(string2, "\nExample line 1\n\nExample line 2\nExample line 3\n\nExample line 4\n"));

    /****/
    char string3[] = "\nExample line 1\n";

    split = str_splitlines(string3);
    //print_vec_string(split);
    assert(split);
    assert(split->len == 1);
    assert(!strcmp((char *) vec_get(split, 0), "Example line 1"));

    vec_destroy(split);

    assert(!strcmp(string3, "\nExample line 1\n"));

    printf("OK\n");
    return 0;
}

static int test_str_splitwhite(void)
{
    printf("%-40s", "test_str_splitwhite ");

    char string1[] = "This is an example string number 1";

    vec_t *split = str_splitwhite(string1);
    //print_vec_string(split);
    assert(split);
    assert(split->len == 7);
    assert(!strcmp((char *) vec_get(split, 0), "This"));
    assert(!strcmp((char *) vec_get(split, 1), "is"));
    assert(!strcmp((char *) vec_get(split, 2), "an"));
    assert(!strcmp((char *) vec_get(split, 3), "example"));
    assert(!strcmp((char *) vec_get(split, 4), "string"));
    assert(!strcmp((char *) vec_get(split, 5), "number"));
    assert(!strcmp((char *) vec_get(split, 6), "1"));

    vec_destroy(split);

    assert(!strcmp(string1, "This is an example string number 1"));

    /****/
    char string2[] = "\nThis \n is an\fexample  \rstring\tnumber\v2   ";

    split = str_splitwhite(string2);
    //print_vec_string(split);
    assert(split);
    assert(split->len == 7);
    assert(!strcmp((char *) vec_get(split, 0), "This"));
    assert(!strcmp((char *) vec_get(split, 1), "is"));
    assert(!strcmp((char *) vec_get(split, 2), "an"));
    assert(!strcmp((char *) vec_get(split, 3), "example"));
    assert(!strcmp((char *) vec_get(split, 4), "string"));
    assert(!strcmp((char *) vec_get(split, 5), "number"));
    assert(!strcmp((char *) vec_get(split, 6), "2"));

    vec_destroy(split);

    assert(!strcmp(string2, "\nThis \n is an\fexample  \rstring\tnumber\v2   "));

    /****/
    char string3[] = "This;is;an;example;string;number;3";

    split = str_splitwhite(string3);
    //print_vec_string(split);
    assert(split);
    assert(split->len == 1);
    assert(!strcmp((char *) vec_get(split, 0), "This;is;an;example;string;number;3"));

    vec_destroy(split);

    assert(!strcmp(string3, "This;is;an;example;string;number;3"));

    /****/
    char string4[] = "\t\t\tExample_string_number_4   ";

    split = str_splitwhite(string4);
    //print_vec_string(split);
    assert(split);
    assert(split->len == 1);
    assert(!strcmp((char *) vec_get(split, 0), "Example_string_number_4"));

    vec_destroy(split);

    assert(!strcmp(string4, "\t\t\tExample_string_number_4   "));

    printf("OK\n");
    return 0;
}

// TODO: tests for str_strip and str_remwhite

int main(void) 
{
    test_str_split();
    test_str_splitlines();
    test_str_splitwhite();


}