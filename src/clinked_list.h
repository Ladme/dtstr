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


/*! @brief Adds node to the head of circular doubly linked list.
 *
 * @paragraph Note on data copying
 * Data provided using the 'data' pointer is copied to the target node.
 * You can therefore freely deallocate the original data.
 * 
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
 * If you navigate the list in counter-clockwise direction (using ->previous),
 * you will reach the new node immediately.
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
 * @param list      circular doubly linked list to rotate
 * @param places    number positions the nodes shall be rotated by
 * 
 * @return Zero if successful. 99 if the provided list is invalid.
 */
int cllist_rotate(cllist_t *list, const int places);

#endif /* CLINKED_LIST_H */