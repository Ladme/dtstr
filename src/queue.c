// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "queue.h"

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH QUEUE_T                    */
/* *************************************************************************** */

void *queue_de(queue_t *queue, const size_t itemsize)
{
    if (queue == NULL || queue->head == NULL) return NULL;

    dnode_t *node = queue->head;
    void *data = malloc(itemsize);
    memcpy(data, node->data, itemsize);

    dllist_remove_node(queue, node);

    return data;
}