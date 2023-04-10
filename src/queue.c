// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "queue.h"

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH QUEUE_T                    */
/* *************************************************************************** */

queue_t *queue_new() 
{
    return dllist_new();
}

void queue_destroy(queue_t *queue) 
{
    dllist_destroy(queue);
}

int queue_en(queue_t *queue, const void *item, const size_t itemsize)
{
    return dllist_push_last(queue, item, itemsize);
}

void *queue_de(queue_t *queue, const size_t itemsize)
{
    if (queue == NULL || queue->head == NULL) return NULL;

    dnode_t *node = queue->head;
    void *data = node->data;

    queue->head = node->next;

    if (node->next != NULL) {
        node->next->previous = node->previous;
    } else {
        queue->tail = node->previous;
    }

    free(node);

    return data;
}

void *queue_peek(const queue_t *queue)
{
    if (queue == NULL || queue->head == NULL) return NULL;

    return queue->head->data;
}

void queue_map(queue_t *queue, void (*function)(void *, void *), void *pointer) 
{
    dllist_map(queue, function, pointer);
}