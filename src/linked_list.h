// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct node {
    void *data;
    struct node *next;
} node_t;

typedef struct llist {
    node_t *head;
} llist_t;


/**
 * @brief Creates a new linked list structure and allocates memory for it.
 * 
 * @note - The memory allocated for the linked list structure must be freed using the `llist_destroy` function.
 * 
 * @return Pointer to the created `llist_t` structure if successful. NULL if not successful.
 */
llist_t *llist_new(void);


/**
 * @brief Destroys the linked list structure, properly deallocating memory.
 *
 * @param list  A pointer to the linked list to destroy.
 */
void llist_destroy(llist_t *list);


/**
 * @brief Adds a node to the beginning of the linked list as the first element.
 *
 * @param list      A pointer to the linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 *
 * @note - The data provided using the 'data' pointer is copied to the target node. Therefore, the original data can be safely deallocated.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int llist_push_first(llist_t *list, const void *data, const size_t datasize);


/**
 * @brief Adds a node to the end of the linked list as the last element.
 *
 * @param list      A pointer to the linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 *
 * @note - The data provided using the 'data' pointer is copied to the target node. Therefore, the original data can be safely deallocated.
 *
 * @note - Asymptotic complexity: Linear, O(n).
 *
 * @return Zero if successful, else non-zero.
 */
int llist_push_last(llist_t *list, const void *data, const size_t datasize);


/**
 * @brief Returns a pointer to the data of the Nth node of the linked list. The linked list is 0-indexed.
 *
 * @param list      A pointer to the linked list to search in.
 * @param index     The index of the node to get.
 *
 * @note - Only non-negative indices can be used in this function. This function returns NULL if the index used is invalid.
 *
 * @note - Asymptotic complexity: Linear, O(n).
 *
 * @return A void pointer to the data stored in the node at the given index. NULL if the index is invalid, i.e., negative or too large.
 */
void *llist_get(const llist_t *list, const size_t index);


/**
 * @brief Adds a node after the provided node.
 *
 * @param list      A pointer to the linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 * @param previous  A pointer to the node_t structure that should be positioned right before the newly added node in the modified linked list.
 *
 * @note - The data provided using the 'data' pointer is copied to the target node. Therefore, the original data can be safely deallocated.
 *
 * @note - If 'previous' is NULL, the node will be added to the beginning of the linked list as the first element.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int llist_insert_after_node(llist_t *list, const void *data, const size_t datasize, node_t *previous);


/** 
 * @brief Adds node after another node with a specified index.
 * 
 * @param list      Pointer to the linked list structure to which the node should be added.
 * @param data      Pointer to the data to include in the node.
 * @param datasize  Size of the data to include.
 * @param index     Index of the node after which the new node should be inserted.
 *
 * @note - Data provided using the 'data' pointer is copied to the target node. You can therefore freely deallocate the original data.
 *
 * @note - Note that the index must correspond to an existing node in the linked list, otherwise this function fails with a return code of 2. 
 * @note - In other words, you cannot use this function to add a node to the beginning of the list or to an empty list. 
 * For that, use `llist_push_first` or `llist_push_last` functions.
 *
 * @note - The added node will have an index one larger than the provided one.
 *
 * @note - Asymptotic Complexity: Linear, O(n). If you want O(1), use `llist_insert_after_node`.
 *
 * @return 0 if successful. 1 if new node could not be created. 2 if the index is out of bounds. 99 if the linked list does not exist.
 */
int llist_insert_after(llist_t *list, const void *data, const size_t datasize, const size_t index);

/**
 * @brief Adds node before another node with a specified index.
 *
 * @param list      Linked list to which the node should be added.
 * @param data      Data to include in the node.
 * @param datasize  Size of the data to include.
 * @param index     Index of the node before which the new node shall be inserted.
 * 
 * @note - Data provided using the 'data' pointer is copied to the target node.
 *
 * @note - Note that if the index is out of bounds, this function fails with a return code of 2.
 * @note - If the index is 0, the node is added to the start of the list (same behavior as `llist_push_first`)
 * @note - If the index is equal to the length of the list, the node is added to the end of the list (same behavior as `llist_push_last`).
 *
 * @note - The added node will have the same index as the one that is provided.
 *
 * @return 0 if successful. 1 if new node could not be created. 2 if the index is out of bounds. 99 if the linked list does not exist.
 */
int llist_insert_before(llist_t *list, const void *data, const size_t datasize, const size_t index);


/** @brief Adds node to the linked list at specified index. Identical to llist_insert_before. */
int llist_insert(llist_t *list, const void *data, const size_t datasize, const size_t index);


/** 
 * @brief Calculates the length of the linked list, i.e. number of nodes.
 * 
 * @param list      linked list which length should be calculated
 * 
 * @note Asymptotic Complexity: Linear, O(n)
 *
 * @return The length of the linked list. 0 if the linked list does not exist.
 */
size_t llist_len(const llist_t *list);


/** 
 * @brief Removes a node located right after the provided node.
 * 
 * @note - If `previous` is NULL, the first node in the linked list will be removed.
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @param List     Linked list from which the node should be removed.
 * @param previous Pointer to node that directly precedes the node that should be removed.
 * 
 * @return 0 if the node was successfully removed, else non-zero.
 */
int llist_remove_after_node(llist_t *list, node_t *previous);


/**
 * @brief Removes a node located at the provided index.
 * 
 * @param list      Linked list from which the node should be removed.
 * @param index     Index of the node to remove.
 * 
 * @note - Note that the index must correspond to an existing node in the linked list, 
 *         otherwise this function fails with a return code of 1.
 * 
 * @note - Asymptotic Complexity: Linear, O(n); If you want O(1), use llist_remove_after_node.
 * 
 * @return 0 if the node was successfully removed, else non-zero.
 */
int llist_remove(llist_t *list, const size_t index);


/**
 * @brief Removes all items from the list that do not fulfill a condition. Modifies the list.
 *
 * @param list              Linked list which should be filtered
 * @param filter_function   Function pointer defining filtering condition
 * 
 * @note 
 * - `filter_function` is a pointer to function that returns integer and accepts void pointer to data in node.
 * The function should return >0 (true), if the item is supposed to STAY in the list.
 * The function should return 0 (false), if the item is supposed to be REMOVED from the list.
 * 
 * @note - If `list` is NULL, no operation is performed.
 * 
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return The number of removed items.
 */
size_t llist_filter_mut(llist_t *list, int (*filter_function)(const void *));


/**
 * @brief Searches for data in linked list and returns pointer to node containing this data.
 * 
 * @param list              List to search in
 * @param equal_function    Function pointer defining how the items should be compared
 * @param target            Pointer to data that is searched in the list
 * 
 * @note 
 * - `equal_function` is a pointer to function that returns integer and accept two void pointers.
 * One void pointer corresponds to pointer to value in particular node. The other void pointer is pointer to
 * value which is being searched for in the linked list.
 * 
 * @note - The equality function should return >0 (true), if the two compared values match each other.
 * @note - The equality function should return 0 (false), if the two compared values DO NOT match each other.
 * 
 * @note - The function always returns pointer to the first matching node it encounters (with the lowest index).
 * 
 * @note - If 'list' is NULL, NULL is returned.
 * 
 * @note - If no corresponding item has been found, NULL is returned.
 * 
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return Pointer to the first node containing the same data as target. NULL, if unsuccessful.
 */ 
node_t *llist_find(const llist_t *list, int (*equal_function)(const void *, const void *), const void *target);


/** 
 * @brief Loops through all items in linked list and applies `function` to each item.
 * 
 * @param list      Linked list to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Items are traversed starting from the head of the linked list.
 */
void llist_map(llist_t *list, void (*function)(void *, void *), void *pointer);


#endif /* LINKED_LIST_H */