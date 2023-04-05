// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementation of queue using doubly linked list.
// Dynamic circular buffer (see cbuffer.h) will probably be faster in most applications.

#ifndef QUEUE_H
#define QUEUE_H

#include "dlinked_list.h"

typedef dllist_t queue_t;


/**
 * @brief Creates a new queue and allocates memory for it.
 * 
 * @note - The created `queue_t` structure must be deallocated using the `queue_destroy` function.
 * 
 * @return Pointer to the created queue, or NULL if memory allocation was unsuccessful.
 */
queue_t *queue_new();


/** 
 * @brief Destroys queue while properly deallocating memory.
 *
 * @param queue     Queue to destroy
 */
void queue_destroy(queue_t *queue);


/**
 * @brief Enqueues (adds) an item to queue.
 *
 * @param queue     Queue to which the item should be added
 * @param item      Item to add
 * @param itemsize  Size of the item to add
 *
 * @note - Data provided using the 'item' pointer is copied to the queue. You can therefore freely deallocate the original data.
 *
 * @note - Asymptotic complexity: Constant, O(1).
 *
 * @return Zero if successful, else non-zero.
 */
int queue_en(queue_t *queue, const void *item, const size_t itemsize);


/**
 * @brief Dequeues an item from queue.
 *
 * @param queue     Queue from which an item should be dequeued
 * @param itemsize  Size of the requested item
 * 
 * @note - The caller is responsible for deallocating memory for the dequeued item.
 *
 * @note - Asymptotic Complexity: Constant, O(1).
 * 
 * @return Void pointer to the dequeued item. NULL if not successful.
 */
void *queue_de(queue_t *queue, const size_t itemsize);


/**
 * @brief Returns the item at the head of the queue. Does not modify the queue.
 *
 * @param queue     Queue to peek at
 *
 * @note - The returned pointer is no longer valid once the parent queue is destroyed.
 * 
 * @note - Asymptotic Complexity: Constant, O(1).
 * 
 * @return Void pointer to the first item in queue. NULL if not successful.
 */
void *queue_peek(const queue_t *queue);


/** 
 * @brief Loops through all items in queue and applies 'function' to each item.
 * 
 * @param list      A pointer to the queue to apply the function to
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Items are traversed starting from the item that is the first in queue.
 */
void queue_map(queue_t *queue, void (*function)(void *, void *), void *pointer);

#endif /* QUEUE_H */