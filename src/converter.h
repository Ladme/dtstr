// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#ifndef CONVERTER_H
#define CONVERTER_H

#include "matrix.h"
#include "str.h"
#include "vector.h"

/** @brief Conversion function to be used with `str2*` functions. Converts string to... string. Returns NULL if parsing fails. */
void *str_convert_string(const char *string);

/** 
 * @brief Conversion function to be used with `str2*` functions. Converts string to char. Assumes that the string has length 1.
 *        Returns NULL if the first character of the string is 0.
 */
void *str_convert_char(const char *string);

/** @brief Conversion function to be used with `str2*` functions. Converts string to integer. Returns NULL if parsing fails. */
void *str_convert_int(const char *string);

/** @brief Conversion function to be used with `str2*` functions. Converts string to float. Returns NULL if parsing fails. */
void *str_convert_float(const char *string);

/** @brief Conversion function to be used with `str2*` functions. Converts string to size_t. Returns NULL if parsing fails. */
void *str_convert_sizet(const char *string);


/**
 * @brief Converts a string representation of a matrix to a matrix structure.
 *
 * @param string                The string representation of the matrix.
 * @param row_delimiters        Characters used to separate rows in the string.
 * @param element_delimiters    Characters used to separate elements within each row.
 * @param conversion_function   A function pointer that performs the conversion of each element from string format to the desired data type.             
 *
 * @return A pointer to the created matrix, or NULL if an error occurred.
 *
 * @note - The string should contain elements separated by the specified row and element delimiters. 
 *         The conversion of each element from string format to the desired data type is performed by the provided conversion function.
 * @note - The rows are not required to be the same length.
 * @note - If string is empty, returns empty matrix.
 * 
 * @note - `row_delimiters` and `element_delimiters` each specify a string of characters, each of which separates the rows or elements in the string.
 * @note - Typically, `row_delimiters` will be "\\n", while `element_delimiters` might be " " or "," or ", " etc.
 * @note - If `element_delimiters` is an empty string (""), individual characters will be treated as individual elements.
 * 
 * @note - The `conversion_function` should accept string and return void pointer to the desired DYNAMICALLY ALLOCATED data type.
 *         This data type can be anything, even a very complex structure.
 * @note - See the natively provided `str_convert_*` functions for conversions to basic data types.
 * @note - The `conversion_function` should return NULL if parsing of the item fails. 
 *         In such case, the item is NOT added into the matrix and parsing continues with the next item.
 * @note - Note that if you want to skip a row in the matrix, it is NOT possible to do this by using two row_delimiters directly following each other.
 *         You have to include an invalid string on the row that should be skipped (i.e. do "1,2,3\\n \\n4,5,6" instead of "1,2,3\\n\\n4,5,6").
 * @note - Note that skipping COLUMNS by incorporating invalid elements is UNDEFINED BEHAVIOR. It may work but it is untested and unsupported.
 * 
 * @note - The caller is responsible for freeing the memory allocated for the matrix using `matrix_destroy`.
 */
matrix_t *str2matrix(
        const char *string, 
        const char *row_delimiters, 
        const char *element_delimiters, 
        void* (*conversion_function)(const char *));


/**
 * @brief Converts a string representation of a vector to a vector structure.
 *
 * @param string                The string representation of the vector.
 * @param delimiters            Characters used to separate the items in the string.
 * @param conversion_function   A function pointer that performs the conversion of each element from string format to the desired data type.             
 *
 * @return A pointer to the created vector, or NULL if an error occurred.
 * 
 * @note - The string should contain items separated by the specified delimiters. 
 *         A typical example includes a string "1,2,3,4,5,6,7" with "," being used as delimiters.
 * 
 * @note - If `element_delimiters` is an empty string (""), individual characters will be treated as individual elements.
 * 
 * @note - The `conversion_function` should accept string and return void pointer to the desired DYNAMICALLY ALLOCATED data type.
 *         This data type can be anything, even a very complex structure.
 * @note - The `conversion_function` should return NULL if parsing of the item fails. In such case, the `str2vec` function returns NULL.
 * @note - See the natively provided `str_convert_*` functions for conversions to basic data types.
 * 
 * @note - The caller is responsible for freeing the memory allocated for the vector using `vec_destroy`.
 */
vec_t *str2vec(const char *string, const char *delimiters, void* (*conversion_function)(const char *));


#endif /* CONVERTER_H */