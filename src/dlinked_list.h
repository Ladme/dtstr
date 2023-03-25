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

#endif /* LINKED_LIST_H */