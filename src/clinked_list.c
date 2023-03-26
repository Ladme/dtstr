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

int cllist_insert_before_node(cllist_t *list, const void *data, const size_t datasize, cnode_t *next)
{
    if (list == NULL) return 99;
    if (next == NULL) next = list->head;
    
    cnode_t *node = cnode_new(data, datasize);
    if (node == NULL) return 1;

    if (next != NULL) {
        node->previous = next->previous;
        node->next = next;
        next->previous->next = node;
        next->previous = node;  
    } else {
        node->next = node;
        node->previous = node;
        list->head = node;
    }

    ++(list->len);

    return 0;
}

int cllist_insert_after_node(cllist_t *list, const void *data, const size_t datasize, cnode_t *previous)
{
    if (list == NULL) return 99;
    if (previous == NULL && list->head == NULL) return cllist_insert_before_node(list, data, datasize, NULL);
    if (previous == NULL && list->head != NULL) return cllist_insert_before_node(list, data, datasize, list->head->next);

    return cllist_insert_before_node(list, data, datasize, previous->next);
}


int cllist_push_first(cllist_t *list, const void *data, const size_t datasize)
{   
    if (list == NULL) return 99;

    int code = 0;
    if ((code = cllist_insert_before_node(list, data, datasize, list->head)) != 0) return code;

    list->head = list->head->previous;

    return 0;
}

int cllist_push_last(cllist_t *list, const void *data, const size_t datasize)
{
    if (list == NULL) return 99;

    return cllist_insert_before_node(list, data, datasize, list->head);
}

int cllist_insert(cllist_t *list, const void *data, const size_t datasize, const size_t index)
{
    if (list == NULL) return 99;

    if (index == list->len) return cllist_push_last(list, data, datasize);
    if (index == 0) return cllist_push_first(list, data, datasize);

    cnode_t *next = cllist_get_node(list, index);
    if (next == NULL) return 2;

    return cllist_insert_before_node(list, data, datasize, next);
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
            for (size_t i = 0; i < list->len - real_places; ++i) list->head = list->head->next;
        } else {
            for (size_t i = 0; i < real_places; ++i) list->head = list->head->previous;
        }
        
    } else {
        // counterclockwise rotation
        int real_places = -places % list->len;
        
        if (real_places > list->len / 2) {
            // in this case, it is faster to perform clockwise rotation
            for (size_t i = 0; i < list->len - real_places; ++i) list->head = list->head->previous;
        } else {
            for (size_t i = 0; i < real_places; ++i) list->head = list->head->next;
        }
    }

    return 0;
}

int cllist_remove_node(cllist_t *list, cnode_t *node)
{
    if (list == NULL) return 99;
    if (node == NULL) return 1;

    if (list->len <= 1) {
        list->head = NULL;
    } else {
        if (list->head == node) list->head = node->next;
        
        node->next->previous = node->previous;
        node->previous->next = node->next;
    }

    cnode_destroy(node);
    --(list->len);

    return 0;
}

int cllist_remove(cllist_t *list, const size_t index)
{
    if (list == NULL) return 99;
    cnode_t *node = cllist_get_node(list, index);
    return cllist_remove_node(list, node);
}


size_t cllist_filter_mut(cllist_t *list, int (*filter_function)(void *))
{
    if (list == NULL) return 0;
    
    size_t orig_len = list->len;
    size_t removed = 0;
    cnode_t *node = list->head;

    for (size_t i = 0; i < orig_len; ++i) {

        if (!filter_function(node->data)) {
            cnode_t *next = node->next;
            cllist_remove_node(list, node);
            node = next;
            ++removed;
        } else {
            node = node->next;
        }

    }

    return removed;
}