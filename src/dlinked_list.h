// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#ifndef DLINKED_LIST_H
#define DLINKED_LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct dnode {
    void *data;
    struct dnode *previous;
    struct dnode *next;
} dnode_t;

typedef struct dllist {
    dnode_t *head;
    dnode_t *tail;
    size_t len;   // we save length of the linked list so we can more efficiently search in it
} dllist_t;


/**
 * @brief Creates a new doubly linked list structure and allocates memory for it.
 * 
 * @note The memory allocated for the doubly linked list structure must be freed using the `dllist_destroy` function.
 * 
 * @return Pointer to the created `dllist_t` structure if successful. NULL if not successful.
 */
dllist_t *dllist_new(void);


/** 
 * @brief Destroys the doubly linked list structure properly deallocating memory.
 *
 * @param list  A pointer to the doubly linked list to destroy.
 */
void dllist_destroy(dllist_t *list);


/**
 * @brief Adds a node to the beginning of the doubly linked list as the first element.
 *
 * @param list      A pointer to the doubly linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 *
 * @note - The data provided using the 'data' pointer is copied to the target node. Therefore, the original data can be safely deallocated.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int dllist_push_first(dllist_t *list, const void *data, const size_t datasize);


/**
 * @brief Adds a node to the end of the doubly linked list as the last element.
 *
 * @param list      A pointer to the doubly linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 *
 * @note - The data provided using the 'data' pointer is copied to the target node. Therefore, the original data can be safely deallocated.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int dllist_push_last(dllist_t *list, const void *data, const size_t datasize);


/**
 * @brief Returns a pointer to the data of the Nth node of the doubly linked list. The doubly linked list is 0-indexed.
 *
 * @param list      A pointer to the list to search in.
 * @param index     The index of the node to get.
 *
 * @note - Only non-negative indices can be used in this function. This function returns NULL if the index used is invalid.
 *
 * @note - Asymptotic complexity: Linear, O(n). 
 * @note - At the same time, it should by almost twice as fast as `llist_get` as `dllist` can be navigated in either direction.
 *
 * @return A void pointer to the data stored in the node at the given index. NULL if the index is invalid, i.e., negative or too large.
 */
void *dllist_get(const dllist_t *list, const size_t index);


/**
 * @brief Adds a node after the provided node.
 *
 * @param list      A pointer to the doubly linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 * @param previous  A pointer to the `dnode_t` structure that should be positioned right before the newly added node in the modified list.
 *
 * @note - The data provided using the 'data' pointer is copied to the target node. Therefore, the original data can be safely deallocated.
 *
 * @note - If 'previous' is NULL, the node will be added to the beginning of the doubly linked list as the first element.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int dllist_insert_after_node(dllist_t *list, const void *data, const size_t datasize, dnode_t *previous);


/**
 * @brief Adds a node before the provided node.
 *
 * @param list      A pointer to the doubly linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 * @param previous  A pointer to the `dnode_t` structure that should be positioned right after the newly added node in the modified list.
 *
 * @note - The data provided using the 'data' pointer is copied to the target node. Therefore, the original data can be safely deallocated.
 *
 * @note - If 'previous' is NULL, the node will be added to the beginning of the doubly linked list as the first element.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int dllist_insert_before_node(dllist_t *list, const void *data, const size_t datasize, dnode_t *next);


/** 
 * @brief Adds node before another node with a specified index.
 *
 * @param list      A pointer to the doubly linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 * @param index  Index of the new node.
 * 
 * @note - Data provided using the 'data' pointer is copied to the target node. You can therefore freely deallocate the original data.
 * 
 * @note - Note that if the index is out of bounds, this function fails with a return code of 2.
 * @note - If the index is 0, the node is added to the start of the list (same behavior as `dllist_push_first`)
 * @note - If the index is equal to the length of the list, the node is added to the end of the list (same behavior as `dllist_push_last`).
 * 
 * @note - The added node will have the same index as the one that is provided.
 * 
 * @note Asymptotic Complexity: Linear, O(n); If you want O(1), use `dllist_insert_before_node`.
 * 
 * @return 0 if successful. 1 if new node could not be created. 2 if the index is out of bounds. 99 if the doubly linked list does not exist.
 */
int dllist_insert(dllist_t *list, const void *data, const size_t datasize, const size_t index);


/** 
 * @brief Removes the node that is being pointed at with pointer `node`.
 * 
 * @param list      A pointer to the doubly linked list from which the node should be removed.
 * @param pointer   Pointer to node that should be removed
 * 
 * @note Asymptotic Complexity: Constant, O(1)
 * 
 * @return 0 if the node was successfully removed. 1 if the node does not exist. 99 if the doubly linked list does not exist.
 */
int dllist_remove_node(dllist_t *list, dnode_t *node);


/*! @brief Removes node located at provided index.
 * 
 * @param list      A pointer to the doubly linked list from which the node should be removed
 * @param index     Index of the node to remove
 * 
 * @note - Note that the index must correspond to an existing node in the doubly linked list, 
 *         otherwise this function fails with a return code of 1.
 * 
 * @note - Asymptotic Complexity: Linear, O(n); If you want O(1), use `dllist_remove_node`.
 * 
 * @return 0 if the node was successfully removed, else non-zero.
 */
int dllist_remove(dllist_t *list, const size_t index);


/**
 * @brief Removes all items from the list that do not fulfill a condition. Modifies the list.
 *
 * @param list              A pointer to the doubly linked list that should be filtered
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
size_t dllist_filter_mut(dllist_t *list, int (*filter_function)(const void *));


/**
 * @brief Searches for data in doubly linked list and returns pointer to node containing this data.
 * 
 * @param list              A pointer to the doubly linked list that should be searched
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
 * @note - If `list` is NULL, NULL is returned.
 * 
 * @note - If no corresponding item has been found, NULL is returned.
 * 
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return Pointer to the first node containing the same data as target. NULL, if unsuccessful.
 */ 
dnode_t *dllist_find(const dllist_t *list, int (*equal_function)(const void *, const void *), const void *target);


/** 
 * @brief Loops through all items in doubly linked list and applies 'function' to each item.
 * 
 * @param list      Doubly linked list to apply the function to
 * @param function  Function to apply
 */
void dllist_map(dllist_t *list, void (*function)(void *));

#endif /* DLINKED_LIST_H */