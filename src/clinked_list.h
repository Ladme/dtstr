// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of circular doubly linked list.

#ifndef CLINKED_LIST_H
#define CLINKED_LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct cnode {
    void *data;
    struct cnode *previous;
    struct cnode *next;
} cnode_t;

typedef struct cllist {
    cnode_t *head;
    size_t len;
} cllist_t;


/*! @brief Creates new circular doubly linked list structure and allocates memory for it.
 *
 * @paragraph Memory deallocation
 * Destroy cllist_t structure using cllist_destroy function.
 *
 * @return Pointer to the created cllist_t, if successful. NULL if not successful.
 */
cllist_t *cllist_new(void);


/*! @brief Destroys circular doubly linked list structure properly deallocating memory.
 *
 * @param list        circular doubly linked list to destroy
 */
void cllist_destroy(cllist_t *list);


/*! @brief Adds node before the provided node of circular doubly linked list.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Next can be NULL
 * If 'next' is NULL, node will be added before the current head of the list (to the last index).
 * In other words, if 'next' is NULL, this replicates cllist_push_last.
 * There is not way to add a node to the start of the circular linked list using this function.
 * For that, use cllist_push_first or rotate the list using cllist_rotate.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      circular linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param next      pointer to cnode_t structure which should be positioned right after the newly added node in the modified list
 * 
 * @return Zero if successful. Else non-zero.
 */
int cllist_insert_before_node(cllist_t *list, const void *data, const size_t datasize, cnode_t *next);


/*! @brief Adds node after the provided node of circular doubly linked list.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Previous can be NULL
 * If 'previous' is NULL, node will be added after the current head of the list (to index 1).
 * There is not way to add a node to the start of the circular linked list using this function.
 * For that, use cllist_push_first or rotate the list using cllist_rotate.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      circular linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param next      pointer to cnode_t structure which should be positioned right before the newly added node in the modified list
 * 
 * @return Zero if successful. Else non-zero.
 */
int cllist_insert_after_node(cllist_t *list, const void *data, const size_t datasize, cnode_t *previous);


/*! @brief Adds node to the head of circular doubly linked list.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      circular doubly linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * 
 * @return Zero if successful, else non-zero.
 */
int cllist_push_first(cllist_t *list, const void *data, const size_t datasize);


/*! @brief Adds node before the current head of circular doubly linked list.
 *
 * @paragraph Note on node position
 * New node will be placed right before the head of the list. If you navigate
 * the circular linked list in clockwise direction (using ->next), you
 * will reach the new node last before returning to head.
 * 
 * If you navigate the list in counterclockwise direction (using ->previous),
 * you will reach the new node immediately after head.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      circular doubly linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * 
 * @return Zero if successful, else non-zero.
 */
int cllist_push_last(cllist_t *list, const void *data, const size_t datasize);


/*! @brief Adds node before another node with a specified index.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Note on indexing
 * Note that if the index is out of bounds, this function fails with a return code of 2.
 * If the index is 0, the node is added to the start of the list (same behavior as cllist_push_first)
 * If the index is equal to the length of the list, the node is added to the end of the list (same behavior as cllist_push_last).
 * 
 * @paragraph Index of the added node
 * The added node will have the same index as the one that is provided.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n); If you want O(1), use cllist_insert_before_node.
 * 
 * @param list      circular doubly linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param index     index of the new node
 * 
 * @return 0 if successful. 1 if new node could not be created. 2 if the index is out of bounds. 99 if the list does not exist.
 */
int cllist_insert(cllist_t *list, const void *data, const size_t datasize, const size_t index);


/*! @brief Returns pointer to the data of the Nth node of circular doubly linked list. Circular doubly linked list is 0-indexed.
 * 
 * @paragraph Supported indices
 * Only non-negative indices can be used in this function. 
 * This function returns NULL, if the index used is invalid.
 *
 * @paragraph Asymptotic Complexity
 * Linear, O(n). Should be much faster than llist_get and similarly fast as dllist_get.
 * 
 * @param list      circular doubly linked list to search in
 * @param index     index of the node to get
 * 
 * @return Void pointer to the data stored in the node at given index. 
 * NULL if the index is invalid, i.e. negative or too large.
 */
void *cllist_get(const cllist_t *list, const size_t index);


/*! @brief Rotates circular doubly linked list by K places.
 * 
 * @paragraph Clockwise and counterclockwise rotation
 * Using positive integer for places leads to clockwise rotation:
 * 0 -> 1 -> 2 -> 3 -> 4   --->   4 -> 0 -> 1 -> 2 -> 3
 * Using negative integer for places leads to counterclockwise rotation:
 * 0 -> 1 -> 2 -> 3 -> 4   --->   1 -> 2 -> 3 -> 4 -> 0
 * 
 * @paragraph Empty lists and lists with one element
 * Empty lists and lists with one element cannot be rotated. 
 * When supplied to this function, no operation is performed.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1).
 * 
 * @param list      circular doubly linked list to rotate
 * @param places    number positions the nodes shall be rotated by
 * 
 * @return Zero if successful. 99 if the provided list is invalid.
 */
int cllist_rotate(cllist_t *list, const int places);


/*! @brief Removes the node that is being pointed at with pointer 'node'
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      circular doubly linked list from which the node should be removed
 * @param pointer   pointer to node that should be removed
 * 
 * @return 0 if the node was successfully removed. 1 if the node does not exist. 99 if the list does not exist.
 */
int cllist_remove_node(cllist_t *list, cnode_t *node);


/*! @brief Removes a node located at the provided index.
 *
 * @paragraph Note on indexing
 * Note that the index must correspond to an existing node in the circular doubly linked list, 
 * otherwise this function fails with a return code of 1.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n); If you want O(1), use cllist_remove_node.
 * 
 * @param list      circular doubly linked list from which the node should be removed
 * @param index     index of the node to remove
 * 
 * @return 0 if the node was successfully removed, else non-zero.
 */
int cllist_remove(cllist_t *list, const size_t index);


/*! @brief Removes all items from the list that do not fulfill a condition. Modifies the list.
 *
 * @paragraph Filter function
 * 'filter_function' is a pointer to function that returns integer and accepts void pointer to data in node.
 * The function should return >0 (true), if the item is supposed to STAY in the list.
 * The function should return 0 (false), if the item is supposed to be REMOVED from the list.
 * 
 * @paragraph Invalid list
 * If 'list' is NULL, no operation is performed.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n).
 * 
 * @param list              circular doubly linked list which should be filtered
 * @param filter_function   function pointer defining filtering condition
 * 
 * @return The number of removed items.
 */
size_t cllist_filter_mut(cllist_t *list, int (*filter_function)(const void *));


/*! @brief Searches for data in circular doubly linked list and returns pointer to node containing this data.
 
 * @paragraph Equality function
 * 'equal_function' is a pointer to function that returns integer and accept two void pointers.
 * One void pointer corresponds to pointer to value in particular node. The other void pointer is pointer to
 * value which is being searched for in the linked list.
 * 
 * The equality function should return >0 (true), if the two compared values match each other.
 * The equality function should return 0 (false), if the two compared values DO NOT match each other.
 * 
 * @paragraph Invalid list
 * If 'list' is NULL, NULL is returned.
 * 
 * @paragraph No item found
 * If no corresponding item has been found, NULL is returned.
 * 
 * @paragraph Multiple nodes with identical values in list
 * The function always returns pointer to the first matching node it encounters (with the lowest index).
 * Note that this function navigates the circular list in clockwise direction (in ->next direction).
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n).
 * 
 * @param list              list to search in
 * @param equal_function    function pointer defining how the items should be compared
 * @param target            pointer to data that is searched in the list
 * 
 * @return Pointer to the first node containing the same data as target. NULL, if unsuccessful.
 */
cnode_t *cllist_find(const cllist_t *list, int (*equal_function)(const void *, const void *), const void *target);


/*! @brief Loops through all items in circular doubly linked list and applies 'function' to each item.
 * 
 * @param list                     circular doubly linked list to apply the function to
 * @param function                 function to apply
 */
void cllist_map(cllist_t *list, void (*function)(void *));

#endif /* CLINKED_LIST_H */