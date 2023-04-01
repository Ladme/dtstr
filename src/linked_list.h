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


/*! @brief Creates new linked list structure and allocates memory for it.
 *
 * @paragraph Memory deallocation
 * Destroy llist_t structure using llist_destroy function.
 *
 * @return Pointer to the created llist_t, if successful. NULL if not successful.
 */
llist_t *llist_new(void);


/*! @brief Destroys the linked list structure properly deallocating memory.
 *
 * @param list        linked list to destroy
 */
void llist_destroy(llist_t *list);


/*! @brief Adds node to the beginning of the linked list (as the first element).
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * 
 * @return Zero if successful, else non-zero.
 */
int llist_push_first(llist_t *list, const void *data, const size_t datasize);


/*! @brief Adds node to the end of the linked list (as the last element).
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n)
 * 
 * @param list      linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * 
 * @return Zero if successful, else non-zero.
 */
int llist_push_last(llist_t *list, const void *data, const size_t datasize);


/*! @brief Returns pointer to the data of the Nth node of the linked list. Linked list is 0-indexed.
 * 
 * @paragraph Supported indices
 * Only non-negative indices can be used in this function. 
 * This function returns NULL, if the index used is invalid.
 *
 * @paragraph Asymptotic Complexity
 * Linear, O(n)
 * 
 * @param list      linked list to search in
 * @param index     index of the node to get
 * 
 * @return Void pointer to the data stored in the node at given index. 
 * NULL if the index is invalid, i.e. negative or too large.
 */
void *llist_get(const llist_t *list, const size_t index);


/*! @brief Adds node after the provided node.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Previous can be NULL
 * If 'previous' is NULL, node will be added to the beginning of the linked list (as the first element).
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param previous  pointer to node_t structure which should be positioned right before the newly added node in the modified linked list
 * 
 * @return Zero if successful. Else non-zero.
 */
int llist_insert_after_node(llist_t *list, const void *data, const size_t datasize, node_t *previous);


/*! @brief Adds node after another node with a specified index.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Note on indexing
 * Note that the index must correspond to an existing node in the linked list, 
 * otherwise this function fails with a return code of 2.
 * In other words, you can not use this function to add a node to the beginning of the list
 * or to an empty list. For that, use llist_push_first or llist_push_last functions.
 * 
 * @paragraph Index of the added node
 * The added node will have an index one larger than the provided one.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n); If you want O(1), use llist_insert_after_node.
 * 
 * @param list      linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param index     index of the node after which the new node shall be inserted
 * 
 * @return 0 if successful. 1 if new node could not be created. 2 if the index is out of bounds. 99 if the linked list does not exist.
 */
int llist_insert_after(llist_t *list, const void *data, const size_t datasize, const size_t index);


/*! @brief Adds node before another node with a specified index.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Note on indexing
 * Note that if the index is out of bounds, this function fails with a return code of 2.
 * If the index is 0, the node is added to the start of the list (same behavior as llist_push_first)
 * If the index is equal to the length of the list, the node is added to the end of the list (same behavior as llist_push_last).
 * 
 * @paragraph Index of the added node
 * The added node will have the same index as the one that is provided.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n); If you want O(1), use llist_insert_after_node.
 * 
 * @param list      linked list to which the node should be added
 * @param data      data to include in the node
 * @param datasize  size of the data to include
 * @param index     index of the node before which the new node shall be inserted
 * 
 * @return 0 if successful. 1 if new node could not be created. 2 if the index is out of bounds. 99 if the linked list does not exist.
 */
int llist_insert_before(llist_t *list, const void *data, const size_t datasize, const size_t index);


/*! @brief Adds node to the linked list at specified index. Identical to llist_insert_before. */
inline int llist_insert(llist_t *list, const void *data, const size_t datasize, const size_t index) {
    return llist_insert_before(list, data, datasize, index);
}


/*! @brief Calculates the length of the linked list, i.e. number of nodes.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n)
 *
 * @param list      linked list which length should be calculated
 * 
 * @return The length of the linked list. 0 if the linked list does not exist.
 */
size_t llist_len(const llist_t *list);


/*! @brief Removes a node located right after the provided node.
 *
 * @paragraph Previous can be NULL
 * If 'previous' is NULL, node from the beginning of the linked list will be removed.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param list      linked list from which the node should be removed
 * @param previous  pointer to node that directly precedes the node that should be removed
 * 
 * @return 0 if the node was successfully removed, else non-zero.
 */
int llist_remove_after_node(llist_t *list, node_t *previous);


/*! @brief Removes a node located at the provided index.
 *
 * @paragraph Note on indexing
 * Note that the index must correspond to an existing node in the linked list, 
 * otherwise this function fails with a return code of 1.
 * 
 * @paragraph Asymptotic Complexity
 * Linear, O(n); If you want O(1), use llist_remove_after_node.
 * 
 * @param list      linked list from which the node should be removed
 * @param index     index of the node to remove
 * 
 * @return 0 if the node was successfully removed, else non-zero.
 */
int llist_remove(llist_t *list, const size_t index);


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
 * @param list              linked list which should be filtered
 * @param filter_function   function pointer defining filtering condition
 * 
 * @return The number of removed items.
 */
size_t llist_filter_mut(llist_t *list, int (*filter_function)(const void *));


/*! @brief Searches for data in linked list and returns pointer to node containing this data.
 
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
node_t *llist_find(const llist_t *list, int (*equal_function)(const void *, const void *), const void *target);


/*! @brief Loops through all items in linked list and applies 'function' to each item.
 * 
 * @param list                     linked list to apply the function to
 * @param function                 function to apply
 */
void llist_map(llist_t *list, void (*function)(void *));


#endif /* LINKED_LIST_H */