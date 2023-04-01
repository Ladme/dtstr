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


/*! @brief Creates new doubly linked list structure and allocates memory for it.
 *
 * @paragraph Memory deallocation
 * Destroy dllist_t structure using dllist_destroy function.
 *
 * @return Pointer to the created dllist_t, if successful. NULL if not successful.
 */
dllist_t *dllist_new(void);


/*! @brief Destroys the doubly linked list structure properly deallocating memory.
 *
 * @param list        doubly linked list to destroy
 */
void dllist_destroy(dllist_t *list);


/*! @brief Adds node to the head of doubly linked list.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      doubly linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * 
 * @return Zero if successful, else non-zero.
 */
int dllist_push_first(dllist_t *list, const void *data, const size_t datasize);


/*! @brief Adds node to the tail of doubly linked list.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      doubly linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * 
 * @return Zero if successful, else non-zero.
 */
int dllist_push_last(dllist_t *list, const void *data, const size_t datasize);


/*! @brief Returns pointer to the data of the Nth node of doubly linked list. Doubly linked list is 0-indexed.
 * 
 * @paragraph Supported indices
 * Only non-negative indices can be used in this function. 
 * This function returns NULL, if the index used is invalid.
 *
 * @paragraph Asymptotic Complexity
 * Linear, O(n). At the same time, it should by almost twice as fast as llist_get as dllist can be navigated in either direction.
 * 
 * @param list      doubly linked list to search in
 * @param index     index of the node to get
 * 
 * @return Void pointer to the data stored in the node at given index. 
 * NULL if the index is invalid, i.e. negative or too large.
 */
void *dllist_get(const dllist_t *list, const size_t index);


/*! @brief Adds node after the provided node.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Previous can be NULL
 * If 'previous' is NULL, node will be added to the head of doubly linked list.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      doubly linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param previous  pointer to dnode_t structure which should be positioned right before the newly added node in the modified doubly linked list
 * 
 * @return Zero if successful. Else non-zero.
 */
int dllist_insert_after_node(dllist_t *list, const void *data, const size_t datasize, dnode_t *previous);


/*! @brief Adds node before the provided node.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Next can be NULL
 * If 'next' is NULL, node will be added to the tail of doubly linked list.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      doubly linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param next      pointer to dnode_t structure which should be positioned right after the newly added node in the modified doubly linked list
 * 
 * @return Zero if successful. Else non-zero.
 */
int dllist_insert_before_node(dllist_t *list, const void *data, const size_t datasize, dnode_t *next);


/*! @brief Adds node before another node with a specified index.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Note on indexing
 * Note that if the index is out of bounds, this function fails with a return code of 2.
 * If the index is 0, the node is added to the start of the list (same behavior as dllist_push_first)
 * If the index is equal to the length of the list, the node is added to the end of the list (same behavior as dllist_push_last).
 * 
 * @paragraph Index of the added node
 * The added node will have the same index as the one that is provided.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n); If you want O(1), use dllist_insert_before_node.
 * 
 * @param list      doubly linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param index     index of the new node
 * 
 * @return 0 if successful. 1 if new node could not be created. 2 if the index is out of bounds. 99 if the doubly linked list does not exist.
 */
int dllist_insert(dllist_t *list, const void *data, const size_t datasize, const size_t index);


/*! @brief Removes the node that is being pointed at with pointer 'node'
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      doubly linked list from which the node should be removed
 * @param pointer   pointer to node that should be removed
 * 
 * @return 0 if the node was successfully removed. 1 if the node does not exist. 99 if the doubly linked list does not exist.
 */
int dllist_remove_node(dllist_t *list, dnode_t *node);


/*! @brief Removes a node located at the provided index.
 *
 * @paragraph Note on indexing
 * Note that the index must correspond to an existing node in the doubly linked list, 
 * otherwise this function fails with a return code of 1.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n); If you want O(1), use dllist_remove_node.
 * 
 * @param list      doubly linked list from which the node should be removed
 * @param index     index of the node to remove
 * 
 * @return 0 if the node was successfully removed, else non-zero.
 */
int dllist_remove(dllist_t *list, const size_t index);


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
 * @param list              doubly linked list which should be filtered
 * @param filter_function   function pointer defining filtering condition
 * 
 * @return The number of removed items.
 */
size_t dllist_filter_mut(dllist_t *list, int (*filter_function)(const void *));


/*! @brief Searches for data in doubly linked list and returns pointer to node containing this data.
 
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
dnode_t *dllist_find(const dllist_t *list, int (*equal_function)(const void *, const void *), const void *target);


/*! @brief Loops through all items in doubly linked list and applies 'function' to each item.
 * 
 * @param list                     doubly linked list to apply the function to
 * @param function                 function to apply
 */
void dllist_map(dllist_t *list, void (*function)(void *));

#endif /* DLINKED_LIST_H */