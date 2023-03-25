// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "dlinked_list.h"

/* *************************************************************************** */
/*                 PRIVATE FUNCTIONS ASSOCIATED WITH DLLIST_T                  */
/* *************************************************************************** */

/*! @brief Creates a doubly linked list node. Allocates memory for the node and for the data inside the node. Copies the data from the data void pointer.
 *
 * @return Pointer to the dnode_t structure, if successful. Else NULL.
 */
static dnode_t *dnode_new(const void *data, const unsigned datasize)
{
    dnode_t *node = calloc(1, sizeof(dnode_t));

    node->data = malloc(datasize);
    if (node->data == NULL) {
        free(node);
        return NULL;
    }

    memcpy(node->data, data, datasize);

    return node;
}

/*! @brief Deallocates memory for doubly linked list node. Node must be a valid pointer. */
void dnode_destroy(dnode_t *node)
{
    free(node->data);
    free(node);
}

/*! @brief Returns pointer to the Nth node of doubly linked list. If unsuccessful, returns NULL. */
static dnode_t *dllist_get_node(const dllist_t *list, const size_t index)
{
    // empty list
    if (list->len == 0) return NULL;
    // index out of bounds
    if (index < 0 || index >= list->len) return NULL;

    // we navigate the doubly linked list either from HEAD or from TAIL depending on what is faster
    dnode_t *target = NULL;
    if (index < list->len / 2) {

        target = list->head;

        for (size_t i = 0; i < index; ++i) {
            target = target->next;
            if (target == NULL) return NULL;
        }
    } else {

        target = list->tail;

        for (size_t i = 0; i < (list->len - index - 1); ++i) {
            target = target->previous;
            if (target == NULL) return NULL;
        }
    }

    return target;
}

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH DLLIST_T                   */
/* *************************************************************************** */

dllist_t *dllist_new(void) 
{
    dllist_t *list = calloc(1, sizeof(dllist_t));

    return list;
}

void dllist_destroy(dllist_t *list)
{
    if (list == NULL) return;

    dnode_t *head = list->head;
    dnode_t *next = NULL;

    while (head != NULL) {
        next = head->next;
        dnode_destroy(head);
        head = next;
    }

    free(list);
}


int dllist_push_first(dllist_t *list, const void *data, const size_t datasize)
{
    if (list == NULL) return 99;

    dnode_t *node = dnode_new(data, datasize);
    if (node == NULL) return 1;

    node->next = list->head;
    if (node->next != NULL) node->next->previous = node;
    list->head = node;

    if (list->tail == NULL) list->tail = node;

    ++(list->len);

    return 0;
}


int dllist_push_last(dllist_t *list, const void *data, const size_t datasize)
{
    if (list == NULL) return 99;

    dnode_t *node = dnode_new(data, datasize);
    if (node == NULL) return 1;

    node->previous = list->tail;
    if (node->previous != NULL) node->previous->next = node;
    list->tail = node;

    if (list->head == NULL) list->head = node;

    ++(list->len);

    return 0;
}


void *dllist_get(const dllist_t *list, const size_t index)
{
    if (list == NULL) return NULL;

    dnode_t *node = dllist_get_node(list, index);

    if (node == NULL) return NULL;
    else return node->data;
}
