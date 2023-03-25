// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "linked_list.h"

/* *************************************************************************** */
/*                 PRIVATE FUNCTIONS ASSOCIATED WITH LLIST_T                   */
/* *************************************************************************** */

/*! @brief Creates a linked list node. Allocates memory for the node and for the data inside the node. Copies the data from the data void pointer.
 *
 * @return Pointer to the node_t structure, if successful. Else NULL.
 */
static node_t *node_new(const void *data, const size_t datasize)
{
    node_t *node = calloc(1, sizeof(node_t));

    node->data = malloc(datasize);
    if (node->data == NULL) {
        free(node);
        return NULL;
    }

    memcpy(node->data, data, datasize);

    return node;
}

/*! @brief Deallocates memory for linked list node. Node must be a valid pointer. */
void node_destroy(node_t *node)
{
    free(node->data);
    free(node);
}

/*! @brief Returns pointer to the Nth node of the linked list. If unsuccessful, returns NULL. */
static node_t *llist_get_node(const llist_t *list, const size_t index)
{
    if (index < 0) return NULL;

    node_t *target = list->head;
    if (target == NULL) return NULL;

    for (size_t i = 0; i < index; ++i) {
        target = target->next;
        if (target == NULL) return NULL;
    }

    return target;
}

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH LLIST_T                    */
/* *************************************************************************** */

llist_t *llist_new(void) 
{
    llist_t *list = calloc(1, sizeof(llist_t));

    return list;
}

void llist_destroy(llist_t *list)
{
    if (list == NULL) return;

    node_t *head = list->head;
    node_t *next = NULL;

    while (head != NULL) {
        next = head->next;
        node_destroy(head);
        head = next;
    }

    free(list);
}

int llist_push_first(llist_t *list, const void *data, const size_t datasize)
{
    if (list == NULL) return 99;

    node_t *node = node_new(data, datasize);
    if (node == NULL) return 1;

    node->next = list->head;
    list->head = node;

    return 0;
}

int llist_push_last(llist_t *list, const void *data, const size_t datasize)
{
    if (list == NULL) return 99;

    node_t *node = node_new(data, datasize);
    if (node == NULL) return 1;

    node_t *target = list->head;
    if (target == NULL) {
        list->head = node;
        return 0;
    }

    while (target->next != NULL) {
        target = target->next;
    }

    target->next = node;

    return 0;
}

void *llist_get(const llist_t *list, const size_t index)
{
    if (list == NULL) return NULL;

    node_t *node = llist_get_node(list, index);

    if (node == NULL) return NULL;
    else return node->data;
}

int llist_insert_after_node(llist_t *list, const void *data, const size_t datasize, node_t *previous)
{
    if (list == NULL) return 99;

    node_t *node = node_new(data, datasize);
    if (node == NULL) return 1;

    node_t *next = NULL;
    if (previous == NULL) {
        node->next = list->head;
        list->head = node;
    } else {
        node->next = previous->next;
        previous->next = node;
    }

    return 0;
}

int llist_insert_after(llist_t *list, const void *data, const size_t datasize, const size_t index)
{
    if (list == NULL) return 99;

    node_t *previous = llist_get_node(list, index);
    if (previous == NULL) return 2;

    return llist_insert_after_node(list, data, datasize, previous);
}

int llist_insert_before(llist_t *list, const void *data, const size_t datasize, const size_t index)
{
    if (list == NULL) return 99;

    if (index == 0) return llist_insert_after_node(list, data, datasize, NULL);

    node_t *previous = llist_get_node(list, index - 1);
    if (previous == NULL) return 2;

    return llist_insert_after_node(list, data, datasize, previous);
}

size_t llist_len(const llist_t *list)
{
    if (list == NULL) return 0;

    size_t len = 0;
    node_t *head = list->head;

    while (head != NULL) {
        ++len;
        head = head->next;
    }

    return len;
}

int llist_remove_after_node(llist_t *list, node_t *previous)
{
    if (list == NULL) return 99;

    node_t *node = NULL;

    if (previous == NULL) {
        node = list->head;
        if (node == NULL) return 1;
        list->head = node->next;
        node_destroy(node);
        return 0;
    }

    node = previous->next;
    if (node == NULL) return 1;

    previous->next = node->next;
    node_destroy(node);
    return 0;
}

int llist_remove(llist_t *list, const size_t index)
{
    if (list == NULL) return 99;

    if (index == 0) return llist_remove_after_node(list, NULL);

    node_t *previous = llist_get_node(list, index - 1);
    if (previous == NULL) return 1;

    return llist_remove_after_node(list, previous);
}