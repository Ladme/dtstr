// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of queue using doubly linked list.

#ifndef QUEUE_H
#define QUEUE_H

#include "dlinked_list.h"

typedef dllist_t queue_t;


/*! @brief Creates new queue structure and allocates memory for it.
 *
 * @paragraph Memory deallocation
 * Destroy queue_t structure using queue_destroy function.
 *
 * @return Pointer to the created queue, if successful. NULL if not successful.
 */
queue_t *queue_new();


/*! @brief Destroys the queue structure properly deallocating memory.
 *
 * @param queue        queue to destroy
 */
void queue_destroy(queue_t *queue);


/*! @brief Adds item to queue.
 *
 * @paragraph Note on data copying
 * Data provided using the 'item' pointer is copied to the queue.
 * You can therefore freely deallocate the original data.
 * 
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param queue     queue to which the item should be added
 * @param item      item to add
 * @param itemsize  size of the item to add
 * 
 * @return Zero if successful, else non-zero.
 */
int queue_en(queue_t *queue, const void *item, const size_t itemsize);


/*! @brief Dequeues an item from queue.
 * 
 * @paragraph Note on memory deallocation
 * The caller is responsible for deallocating memory for the dequeued item.
 *
 * @paragraph Asymptotic Complexity
 * Constant, O(1)
 * 
 * @param queue     queue from which an item should be dequeued
 * @param itemsize  size of the requested item
 * 
 * @return Void pointer to the dequeued item. NULL if not successful.
 */
void *queue_de(queue_t *queue, const size_t itemsize);


/*! @brief Loops through all items in queue and applies 'function' to each item.
 * 
 * @param queue                    queue to apply the function to
 * @param function                 function to apply
 */
void queue_map(queue_t *queue, void (*function)(void *));

#endif /* QUEUE_H */