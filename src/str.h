// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Useful functions for working with strings.
// All functions expect valid char* pointers terminated by \0.

#ifndef STR_H
#define STR_H

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

/** @brief Result of parsing. Integer that can be valid, if parsing was successful, or invalid, if parsing failed. */
typedef struct { int valid; int value; } int_option_t;
/** @brief Result of parsing. Long that can be valid, if parsing was successful, or invalid, if parsing failed. */
typedef struct { int valid; long value; } long_option_t;
/** @brief Result of parsing. Size_t that can be valid, if parsing was successful, or invalid, if parsing failed. */
typedef struct { int valid; size_t value; } sizet_option_t;
/** @brief Result of parsing. Float that can be valid, if parsing was successful, or invalid, if parsing failed. */
typedef struct { int valid; float value; } float_option_t;


/** @brief Initial length of buffer for `read_line` function. */
#define INIT_BUFFER_LEN 64

/** @brief Splits string by delimiter and saves the substrings into a vector.
 * 
 * @param string    String to be split
 * @param delim     Delimiter for spliting
 * 
 * @note - The original string is unchanged.
 * @note - The caller is responsible for deallocating the returned vector by calling `vec_destroy`.
 * @note - Multiple delimiters can be used by supplying all of them. E.g. using delimiters ' ;' will split
 * the string 'This;is ; an example string;' into 'This', 'is', 'an', 'example', 'string'.
 * @note - If the string is empty, returns an empty vector.
 * @note - If delimiter is an empty string, returns a vector containing only the entire input string (no splitting).
 * 
 * @return Pointer to vector containing the substrings. NULL in case of an error.
 */
vec_t *str_split(const char *string, const char *delim);


/** @brief Splits string by new line characters and saves the substrings into a vector.
 * 
 * @param string    String to be split
 * 
 * @note - The original string is unchanged.
 * @note - The caller is responsible for deallocating the returned vector by calling `vec_destroy`.
 * 
 * @return Pointer to vector containing the substrings. NULL in case of an error.
 */
vec_t *str_splitlines(const char *string);


/** @brief Splits string by whitespace and saves the substrings into a vector.
 * 
 * @param string    String to be split
 * 
 * @note - The original string is unchanged.
 * @note - The caller is responsible for deallocating the returned vector by calling `vec_destroy`.
 * 
 * @return Pointer to vector containing the substrings. NULL in case of an error.
 */
vec_t *str_splitwhite(const char *string);


/** @brief Splits string into individual characters and saves them into a vector.
 * 
 * @param string    String to fragmentize
 * 
 * @note - The original string is unchanged.
 * @note - The caller is responsible for deallocating the returned vector by calling `vec_destroy`.
 * 
 * @return Pointer to vector containing the characters. NULL if memory allocation fails.
 */
vec_t *str_fragmentize(const char *string);


/** @brief Strips leading and trailing whitespaces from string.
 * 
 * @param string    String to strip
 * 
 * @note
 * - This function is a slightly modified version of jkramer's trim() from 
 * https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
 */
void str_strip(char *string);


/** @brief Removes all white spaces from a string.
 * 
 * @param string    String to modify
 */
void str_remwhite(char *string);


/** @brief Reads a line of text from a given stream and returns a pointer to a dynamically allocated buffer containing the line.
 * 
 * @param stream    Pointer to the stream to read from
 * 
 * @note - This function returns NULL if an error occurred or if the end of the file was reached before any characters were read.
 * @note - The output buffer does NOT contain the newline character.
 * @note - The buffer will be resized as needed, and the line will be null-terminated.
 * @note - The caller is responsible for freeing the buffer when it is no longer needed.
 * 
 * @return Pointer to the buffer containing the line, or NULL.
 */
char *read_line(FILE *stream);


/**
 * Parses the given string as an integer and returns a corresponding `int_option_t` struct.
 *
 * @param string    The string to parse as an integer
 * 
 * @note - An `int_option_t` struct contains a flag indicating whether the struct contains a valid integer value, and the parsed integer value itself.
 * @note - You can check the validity of the option using `int_option_check` and you can unwrap the value stored in the option using `int_option_unwrap`.
 * 
 * @return An `int_option_t` struct representing the parsed integer, or an invalid struct if the string could not be parsed.
 */
int_option_t str_parse_int(const char *string);


/**
 * Parses the given string as a long and returns a corresponding `long_option_t` struct.
 *
 * @param string    The string to parse as a long
 * 
 * @note - A `long_option_t` struct contains a flag indicating whether the struct contains a valid long value, and the parsed long value itself.
 * @note - You can check the validity of the option using `long_option_check` and you can unwrap the value stored in the option using `long_option_unwrap`.
 * 
 * @return A `long_option_t` struct representing the parsed long, or an invalid struct if the string could not be parsed.
 */
long_option_t str_parse_long(const char *string);


/**
 * Parses the given string as a size_t and returns a corresponding `sizet_option_t` struct.
 *
 * @param string    The string to parse as a size_t
 * 
 * @note - A `sizet_option_t` struct contains a flag indicating whether the struct contains a valid size_t value, and the parsed size_t value itself.
 * @note - You can check the validity of the option using `sizet_option_check` and you can unwrap the value stored in the option using `sizet_option_unwrap`.
 * 
 * @return A `sizet_option_t` struct representing the parsed size_t, or an invalid struct if the string could not be parsed.
 */
sizet_option_t str_parse_sizet(const char *string);


/**
 * Parses the given string as a float and returns a corresponding `float_option_t` struct.
 *
 * @param string    The string to parse as a float
 * 
 * @note - A `float_option_t` struct contains a flag indicating whether the struct contains a valid float value, and the parsed float value itself.
 * @note - You can check the validity of the option using `float_option_check` and you can unwrap the value stored in the option using `float_option_unwrap`.
 * 
 * @return A `float_option_t` struct representing the parsed float, or an invalid struct if the string could not be parsed.
 */
float_option_t str_parse_float(const char *string);


/** @brief Extracts integer from option while ignoring validity of the option. */
int int_option_unwrap(const int_option_t value);
/** @brief Extracts long from option while ignoring validity of the option. */
long long_option_unwrap(const long_option_t value);
/** @brief Extracts size_t from option while ignoring validity of the option. */
size_t sizet_option_unwrap(const sizet_option_t value);
/** @brief Extracts float from option while ignoring validity of the option. */
float float_option_unwrap(const float_option_t value);

/** @brief Checks whether the option is valid. Returns 1 if it is valid. Else returns 0. */
int int_option_check(const int_option_t value);
/** @brief Checks whether the option is valid. Returns 1 if it is valid. Else returns 0. */
int long_option_check(const long_option_t value);
/** @brief Checks whether the option is valid. Returns 1 if it is valid. Else returns 0. */
int sizet_option_check(const sizet_option_t value);
/** @brief Checks whether the option is valid. Returns 1 if it is valid. Else returns 0. */
int float_option_check(const float_option_t value);

#endif /* STR_H */