// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "clinked_list.h"

/* *************************************************************************** */
/*                 PRIVATE FUNCTIONS ASSOCIATED WITH CLLIST_T                  */
/* *************************************************************************** */

/*! @brief Creates a circular linked list node. Allocates memory for the node and for the data inside the node. Copies the data from the data void pointer.
 *
 * @return Pointer to the cnode_t structure, if successful. Else NULL.
 */
static cnode_t *cnode_new(const void *data, const size_t datasize)
{
    cnode_t *node = calloc(1, sizeof(cnode_t));

    node->data = malloc(datasize);
    if (node->data == NULL) {
        free(node);
        return NULL;
    }

    memcpy(node->data, data, datasize);

    return node;
}

/*! @brief Deallocates memory for circular linked list node. Node must be a valid pointer. */
void cnode_destroy(cnode_t *node)
{
    free(node->data);
    free(node);
}

/*! @brief Returns pointer to the Nth node of circular doubly linked list. If unsuccessful, returns NULL. */
static cnode_t *cllist_get_node(const cllist_t *list, const size_t index)
{
    // empty list
    if (list->len == 0) return NULL;
    // index out of bounds
    if (index < 0 || index >= list->len) return NULL;

    // we navigate the circular list either in clockwise or counterclockwise direction
    cnode_t *target = list->head;
    if (index < list->len / 2) {

        for (size_t i = 0; i < index; ++i) {
            target = target->next;
        }
    } else {

        for (size_t i = 0; i < (list->len - index); ++i) {
            target = target->previous;
        }
    }

    return target;
}

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH CLLIST_T                   */
/* *************************************************************************** */

cllist_t *cllist_new(void) 
{
    cllist_t *list = calloc(1, sizeof(cllist_t));

    return list;
}

void cllist_destroy(cllist_t *list)
{
    if (list == NULL) return;
    if (list->head == NULL) {
        free(list);
        return;
    }

    cnode_t *tail = list->head->previous;
    cnode_t *head = list->head;
    cnode_t *next = NULL;

    do {
        next = head->next;
        cnode_destroy(head);
        head = next;
    } while (head != list->head);

    free(list);
}

int cllist_push_first(cllist_t *list, const void *data, const size_t datasize)
{
    if (list == NULL) return 99;

    cnode_t *node = cnode_new(data, datasize);
    if (node == NULL) return 1;

    if (list->head != NULL) {
        node->previous = list->head->previous;
        node->next = list->head;
        list->head->previous->next = node;
        list->head->previous = node;  
    } else {
        node->next = node;
        node->previous = node;
    }

    list->head = node;

    ++(list->len);

    return 0;
}

int cllist_push_last(cllist_t *list, const void *data, const size_t datasize)
{
    // the same function as cllist_push_first, but we do not set list->head to the new node unless list->head is NULL
    if (list == NULL) return 99;

    cnode_t *node = cnode_new(data, datasize);
    if (node == NULL) return 1;

    if (list->head != NULL) {
        node->previous = list->head->previous;
        node->next = list->head;
        list->head->previous->next = node;
        list->head->previous = node;  
    } else {
        node->next = node;
        node->previous = node;
        list->head = node;
    }

    ++(list->len);

    return 0;
}

void *cllist_get(const cllist_t *list, const size_t index)
{
    if (list == NULL) return NULL;

    cnode_t *node = cllist_get_node(list, index);

    if (node == NULL) return NULL;
    else return node->data;
}

int cllist_rotate(cllist_t *list, const int places)
{
    if (list == NULL) return 99;
    if (list->head == NULL || list->len == 1) return 0;

    if (places > 0) {
        // clockwise rotation
        int real_places = places % list->len;
        if (real_places > list->len / 2) {
            // in this case, it is faster to perform counterclockwise rotation
            //printf("Performing counterclockwise rotation by %d instead of clockwise by %d\n", list->len - real_places, real_places);
            for (size_t i = 0; i < list->len - real_places; ++i) list->head = list->head->next;
        } else {
            for (size_t i = 0; i < real_places; ++i) list->head = list->head->previous;
        }
        
    } else {
        // counterclockwise rotation
        int real_places = -places % list->len;
        
        if (real_places > list->len / 2) {
            // in this case, it is faster to perform clockwise rotation
            //printf("Performing clockwise rotation by %d instead of counterclockwise by %d\n", list->len - real_places, real_places);
            for (size_t i = 0; i < list->len - real_places; ++i) list->head = list->head->previous;
        } else {
            for (size_t i = 0; i < real_places; ++i) list->head = list->head->next;
        }
    }

    return 0;
}