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


/**
 * @brief Creates a new circular doubly linked list structure and allocates memory for it.
 * 
 * @note The memory allocated for the circular doubly linked list structure must be freed using the `cllist_destroy` function.
 * 
 * @return Pointer to the created `cllist_t` structure if successful. NULL if not successful.
 */
cllist_t *cllist_new(void);


/** 
 * @brief Destroys the circular doubly linked list structure properly deallocating memory.
 *
 * @param list  A pointer to the circular doubly linked list to destroy.
 */
void cllist_destroy(cllist_t *list);


/**
 * @brief Adds a node before the provided node of a circular doubly linked list.
 *
 * @note - Data provided using the `data` pointer is copied to the target node. Therefore, you can freely deallocate the original data.
 *
 * @note 
 * - If `next` is NULL, the node will be added before the current head of the list (to the last index). 
 *   In other words, if `next` is NULL, this replicates `cllist_push_last`. 
 * @note
 * - There is no way to add a node to the start of the circular linked list using this function. 
 *   For that, use `cllist_push_first` or rotate the list using `cllist_rotate`.
 *
 * @note - Asymptotic Complexity: Constant, O(1).
 *
 * @param list      Pointer to the circular linked list to which the node should be added.
 * @param data      Pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 * @param next      Pointer to the `cnode_t` structure which should be positioned right after the newly added node in the modified list.
 *
 * @return Zero if successful. Else non-zero.
 */
int cllist_insert_before_node(cllist_t *list, const void *data, const size_t datasize, cnode_t *next);


/**
 * @brief Adds a node after the provided node of a circular doubly linked list.
 *
 * @note - Data provided using the `data` pointer is copied to the target node. Therefore, you can freely deallocate the original data.
 *
 * @note 
 * - If `next` is NULL, the node will be added after the current head of the list (to index 1). 
 * @note
 * - There is no way to add a node to the start of the circular linked list using this function. 
 *   For that, use `cllist_push_first` or rotate the list using `cllist_rotate`.
 *
 * @note - Asymptotic Complexity: Constant, O(1).
 *
 * @param list      Pointer to the circular linked list to which the node should be added.
 * @param data      Pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 * @param next      Pointer to the `cnode_t` structure which should be positioned right before the newly added node in the modified list.
 *
 * @return Zero if successful. Else non-zero.
 */
int cllist_insert_after_node(cllist_t *list, const void *data, const size_t datasize, cnode_t *previous);


/**
 * @brief Adds a node to the beginning of the circular doubly linked list as the first element.
 *
 * @param list      A pointer to the circular doubly linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 *
 * @note - The data provided using the 'data' pointer is copied to the target node. Therefore, the original data can be safely deallocated.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int cllist_push_first(cllist_t *list, const void *data, const size_t datasize);


/*! 
 * @brief Adds a node before the current head of a circular doubly linked list.
 *
 * @param list      A pointer to the circular doubly linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 * 
 * @note 
 * - The new node will be placed right before the head of the list. If you navigate
 * the circular linked list in clockwise direction (using ->next), you will reach the new 
 * node last before returning to head. If you navigate the list in counterclockwise 
 * direction (using ->previous), you will reach the new node immediately after head.
 *
 * @note 
 * - Data provided using the 'data' pointer is copied to the target node. You can 
 * therefore freely deallocate the original data.
 * 
 * @note - Asymptotic complexity: Constant, O(1).
 * 
 * @return Zero if successful, else non-zero.
 */ 
int cllist_push_last(cllist_t *list, const void *data, const size_t datasize);


/** 
 * @brief Adds node before another node with a specified index.
 *
 * @param list      A pointer to the circular doubly linked list to which the node should be added.
 * @param data      A pointer to the data to include in the node.
 * @param datasize  The size of the data to include.
 * @param index     Index of the new node.
 * 
 * @note - Data provided using the 'data' pointer is copied to the target node. You can therefore freely deallocate the original data.
 * 
 * @note - Note that if the index is out of bounds, this function fails with a return code of 2.
 * @note - If the index is 0, the node is added to the start of the list (same behavior as `cllist_push_first`)
 * @note - If the index is equal to the length of the list, the node is added to the end of the list (same behavior as `cllist_push_last`).
 * 
 * @note - The added node will have the same index as the one that is provided.
 * 
 * @note Asymptotic Complexity: Linear, O(n); If you want O(1), use `cllist_insert_before_node`.
 * 
 * @return 0 if successful. 1 if new node could not be created. 2 if the index is out of bounds. 99 if the doubly linked list does not exist.
 */
int cllist_insert(cllist_t *list, const void *data, const size_t datasize, const size_t index);


/** 
 * @brief Calculates the length of the circular doubly linked list, i.e. number of nodes.
 * 
 * @param list      Circular doubly linked list which length should be calculated
 * 
 * @note Asymptotic Complexity: Constant, O(1)
 *
 * @return The length of the circular doubly linked list. 0 if the list does not exist.
 */
extern inline size_t cllist_len(const cllist_t *list)
{
    return (list == NULL) ? 0 : list->len; 
}


/**
 * @brief Returns a pointer to the data of the Nth node of the circular doubly linked list. The circular doubly linked list is 0-indexed.
 *
 * @param list      A pointer to the list to search in.
 * @param index     The index of the node to get.
 *
 * @note - Only non-negative indices can be used in this function. This function returns NULL if the index used is invalid.
 *
 * @note - Asymptotic complexity: Linear, O(n). 
 * @note - Should be much faster than `llist_get` and similarly fast as `dllist_get`.
 *
 * @return A void pointer to the data stored in the node at the given index. NULL if the index is invalid, i.e., negative or too large.
 */
void *cllist_get(const cllist_t *list, const size_t index);


/** 
 * @brief Rotates the provided circular doubly linked list by a given number of places.
 * 
 * @param list   A pointer to the circular doubly linked list to rotate.
 * @param places The number of positions the nodes should be rotated by.
 *
 * @note - If a positive integer is used for `places`, it will result in a clockwise rotation (0->1->2->3 --> 3->0->1->2)
 * @note - If a negative integer is used for `places`, it will result in a counterclockwise rotation (0->1->2->3 --> 1->2->3->0)
 * @note - If the list is empty or contains only one element, no rotation will be performed.
 * 
 * @note - Asymptotic Complexity: Constant, O(1).
 * 
 * @return 0 if successful. 99 if the provided list is invalid.
 */
int cllist_rotate(cllist_t *list, const int places);


/** 
 * @brief Removes the node that is being pointed at with pointer `node`.
 * 
 * @param list      A pointer to the circular doubly linked list from which the node should be removed.
 * @param pointer   Pointer to node that should be removed
 * 
 * @note Asymptotic Complexity: Constant, O(1)
 * 
 * @return 0 if the node was successfully removed. 1 if the node does not exist. 99 if the circular doubly linked list does not exist.
 */
int cllist_remove_node(cllist_t *list, cnode_t *node);


/*! @brief Removes node located at provided index.
 * 
 * @param list      A pointer to the circular doubly linked list from which the node should be removed
 * @param index     Index of the node to remove
 * 
 * @note - Note that the index must correspond to an existing node in the circular doubly linked list, 
 *         otherwise this function fails with a return code of 1.
 * 
 * @note - Asymptotic Complexity: Linear, O(n); If you want O(1), use `cllist_remove_node`.
 * 
 * @return 0 if the node was successfully removed, else non-zero.
 */
int cllist_remove(cllist_t *list, const size_t index);


/**
 * @brief Removes all items from the list that do not fulfill a condition. Modifies the list.
 *
 * @param list              A pointer to the circular doubly linked list that should be filtered
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
size_t cllist_filter_mut(cllist_t *list, int (*filter_function)(const void *));


/**
 * @brief Searches for data in circular doubly linked list and returns pointer to node containing this data.
 * 
 * @param list              A pointer to the circular doubly linked list that should be searched
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
 * @note - Note that this function navigates the circular list in clockwise direction (in ->next direction).
 * 
 * @note - If `list` is NULL, NULL is returned.
 * 
 * @note - If no corresponding item has been found, NULL is returned.
 * 
 * @note - Asymptotic Complexity: Linear, O(n).
 * 
 * @return Pointer to the first node containing the same data as target. NULL, if unsuccessful.
 */ 
cnode_t *cllist_find(const cllist_t *list, int (*equal_function)(const void *, const void *), const void *target);


/** 
 * @brief Loops through all items in circular doubly linked list and applies 'function' to each item.
 * 
 * @param list      A pointer to the circular doubly linked list to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Items are traversed starting from the head of the circular doubly linked list and going in clockwise direction.
 */
void cllist_map(cllist_t *list, void (*function)(void *, void *), void *pointer);

#endif /* CLINKED_LIST_H */