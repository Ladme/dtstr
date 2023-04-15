// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Useful functions for working with strings.
// All functions expect valid char* pointers terminated by \0.

#ifndef STR_H
#define STR_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"


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
 * @return Pointer to vector containing the substrings. NULL if parsing fails.
 */
vec_t *str_split(const char *string, const char *delim);


/** @brief Splits string by new line characters and saves the substrings into a vector.
 * 
 * @param string    String to be split
 * 
 * @note - The original string is unchanged.
 * @note - The caller is responsible for deallocating the returned vector by calling `vec_destroy`.
 * 
 * @return Pointer to vector containing the substrings. NULL if parsing fails.
 */
inline vec_t *str_splitlines(const char *string) { return str_split(string, "\n"); }


/** @brief Splits string by whitespace and saves the substrings into a vector.
 * 
 * @param string    String to be split
 * 
 * @note - The original string is unchanged.
 * @note - The caller is responsible for deallocating the returned vector by calling `vec_destroy`.
 * 
 * @return Pointer to vector containing the substrings. NULL if parsing fails.
 */
inline vec_t *str_splitwhite(const char *string) { return str_split(string, " \t\n\v\f\r"); }


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
 * 
 * @note
 * - This function is a slightly modified version of a function from
 * https://stackoverflow.com/questions/1726302/remove-spaces-from-a-string-in-c
 */
void str_remwhite(char *string);

#endif /* STR_H */