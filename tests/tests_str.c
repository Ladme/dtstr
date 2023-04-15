// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "../src/str.h"

inline static int closef(const float a, const float b, const float limit)
{
    return (a > b - limit && a < b + limit);
}

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

static int test_str_strip(void)
{
    printf("%-40s", "test_str_strip ");

    char string1[] = "  Example string number 1";
    str_strip(string1);
    assert(!strcmp(string1, "Example string number 1"));

    /****/

    char string2[] = "Example string number 2    ";
    str_strip(string2);
    assert(!strcmp(string2, "Example string number 2"));

    /****/

    char string3[] = "  Example string number 3    ";
    str_strip(string3);
    assert(!strcmp(string3, "Example string number 3"));

    /****/

    char string4[] = "\n\n\tExample string number 4\r\f";
    str_strip(string4);
    assert(!strcmp(string4, "Example string number 4"));

    printf("OK\n");
    return 0;
}

static int test_str_remwhite(void)
{
    printf("%-40s", "test_str_remwhite ");

    char string1[] = "This will be joined";
    str_remwhite(string1);
    assert(!strcmp(string1, "Thiswillbejoined"));

    /****/

    char string2[] = "  T hi  s wil l    als  o b e joined  ";
    str_remwhite(string2);
    assert(!strcmp(string2, "Thiswillalsobejoined"));

    /****/

    char string3[] = "\nAn\rd thi\ns w\ti\f\vll b e jo\n\n\n\nined to\t\t\to ";
    str_remwhite(string3);
    assert(!strcmp(string3, "Andthiswillbejoinedtoo"));

    /****/

    char string4[] = "Dontdoanything";
    str_remwhite(string4);
    assert(!strcmp(string4, "Dontdoanything"));

    printf("OK\n");
    return 0;
}

static int test_read_line(void)
{
    printf("%-40s", "test_read_line ");

    FILE *input = fopen("tests/read_line_test.txt", "r");
    assert(input);

    char *line = read_line(input);
    assert(!strcmp(line, "This file contains some lines used for testing the validity of the read_line."));
    free(line);

    line = read_line(input);
    assert(!strcmp(line, "The next line is empty."));
    free(line);

    line = read_line(input);
    assert(!strcmp(line, ""));
    free(line);

    line = read_line(input);
    assert(!strcmp(line, "This line contains very random characters FUEB8934RZB  ifohoiurfhneo iofjiojhjsodfnior wqhfiore jiwodfjio hfiorwjhe iocjiorehvferiohvcioqwjiowhrfoi sjcwiojfrioejcoisjqvifoj wijf ionrioneviorenoxn"));
    free(line);

    line = read_line(input);
    size_t length = 0;
    for (char *p = line; *p != 0; ++p) ++length;
    assert(length == 79321);
    assert(line[length - 1] == '@');
    free(line);

    line = read_line(input);
    assert(!strcmp(line, "No newline character here..."));
    free(line);

    fclose(input);

    printf("OK\n");
    return 0;
}

static int test_read_line_load(void)
{
    printf("%-40s", "test_read_line (loading data) ");

    FILE *input = fopen("tests/read_line_test2.txt", "r");
    assert(input);

    vec_t *vector = vec_new();

    char *line = NULL;
    while ((line = read_line(input)) != NULL) {

        int parsed = 0;
        assert(sscanf(line, "%d", &parsed) == 1);
        vec_push(vector, &parsed, sizeof(int));

        free(line);
    }

    assert(vector->len == 50);

    fclose(input);

    for (size_t i = 0; i < 50; ++i) {
        assert(*(int *) vec_get(vector, i) == i);
    }

    vec_destroy(vector);

    printf("OK\n");
    return 0;
}

static int test_parse_int(void)
{
    printf("%-40s", "test_parse_int ");

    char *to_parse[] = {"72", "-5", "99273836472", "5.32", "14b"};

    // valid positive value
    int_option_t integer = str_parse_int(to_parse[0]);
    assert(check_int_option(integer));
    assert(unwrap_int_option(integer) == 72);

    // valid negative value
    integer = str_parse_int(to_parse[1]);
    assert(check_int_option(integer));
    assert(unwrap_int_option(integer) == -5);

    // value too large
    integer = str_parse_int(to_parse[2]);
    assert(!check_int_option(integer));

    // float value
    integer = str_parse_int(to_parse[3]);
    assert(!check_int_option(integer));

    // value contains a letter
    integer = str_parse_int(to_parse[4]);
    assert(!check_int_option(integer));

    printf("OK\n");
    return 0;
}

static int test_parse_long(void)
{
    printf("%-40s", "test_parse_long ");

    char *to_parse[] = {"72", "-5", "99273836472", "5.32", "14b"};

    // valid positive value
    long_option_t value = str_parse_long(to_parse[0]);
    assert(check_long_option(value));
    assert(unwrap_long_option(value) == 72);

    // valid negative value
    value = str_parse_long(to_parse[1]);
    assert(check_long_option(value));
    assert(unwrap_long_option(value) == -5);

    // large positive value
    value = str_parse_long(to_parse[2]);
    assert(check_long_option(value));
    assert(unwrap_long_option(value) == 99273836472);

    // float value
    value = str_parse_long(to_parse[3]);
    assert(!check_long_option(value));

    // value contains a letter
    value = str_parse_long(to_parse[4]);
    assert(!check_long_option(value));

    printf("OK\n");
    return 0;
}

static int test_parse_sizet(void)
{
    printf("%-40s", "test_parse_sizet ");

    char *to_parse[] = {"72", "-5", "99273836472", "5.32", "14b"};

    // valid positive value
    sizet_option_t value = str_parse_sizet(to_parse[0]);
    assert(check_sizet_option(value));
    assert(unwrap_sizet_option(value) == 72);

    // invalid negative value
    value = str_parse_sizet(to_parse[1]);
    assert(!check_sizet_option(value));

    // large positive value
    value = str_parse_sizet(to_parse[2]);
    assert(check_sizet_option(value));
    assert(unwrap_sizet_option(value) == 99273836472);

    // float value
    value = str_parse_sizet(to_parse[3]);
    assert(!check_sizet_option(value));

    // value contains a letter
    value = str_parse_sizet(to_parse[4]);
    assert(!check_sizet_option(value));

    printf("OK\n");
    return 0;
}

static int test_parse_float(void)
{
    printf("%-40s", "test_parse_float ");

    char *to_parse[] = {"72", "99273836472", "5.32", "-82.287654", "14b"};

    // valid positive value
    float_option_t floating = str_parse_float(to_parse[0]);
    assert(check_float_option(floating));
    assert(unwrap_float_option(floating) == 72);

    // valid large value
    floating = str_parse_float(to_parse[1]);
    assert(check_float_option(floating));
    assert(unwrap_float_option(floating) == 99273836472);

    // with decimal point (positive)
    floating = str_parse_float(to_parse[2]);
    assert(check_float_option(floating));
    assert(closef(unwrap_float_option(floating), 5.32, 0.00001));

    // with decimal point (negative)
    floating = str_parse_float(to_parse[3]);
    assert(check_float_option(floating));
    assert(closef(unwrap_float_option(floating), -82.287654, 0.00001));

    // value contains a letter
    floating = str_parse_float(to_parse[4]);
    assert(!check_float_option(floating));

    printf("OK\n");
    return 0;
}


int main(void) 
{
    test_str_split();
    test_str_splitlines();
    test_str_splitwhite();

    test_str_strip();
    test_str_remwhite();

    test_read_line();
    test_read_line_load();

    test_parse_int();
    test_parse_long();
    test_parse_sizet();
    test_parse_float();


}