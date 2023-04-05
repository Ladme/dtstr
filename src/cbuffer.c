// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "cbuffer.h"

/* *************************************************************************** */
/*                PRIVATE FUNCTIONS ASSOCIATED WITH CBUF_T                     */
/* *************************************************************************** */
#define UNUSED(x) (void)(x)

/*! @brief Checks whether buffer is sufficiently small to be shrunk. Returns 1, if that is the case. Else returns 0.*/
static inline int cbuf_check_shrink(cbuf_t *buffer)
{
    return (buffer->capacity > buffer->base_capacity) && (buffer->len <= buffer->capacity / 4);
}

/*! @brief Reallocates memory for circular buffer. Returns 0, if successful. Else return non-zero. */
static int cbuf_reallocate(cbuf_t *buffer)
{
    buffer->capacity *= 2;

    void **new_items = realloc(buffer->items, buffer->capacity * sizeof(void *));
    if (new_items == NULL) {
        cbuf_destroy(buffer);
        return 1;
    }

    buffer->items = new_items;
    // move items that are positioned in front of the tail pointer to the end of the array
    memcpy(buffer->items + buffer->len, buffer->items, buffer->tail * sizeof(void *));
    memset(buffer->items, 0, buffer->tail * sizeof(void *));
    memset(buffer->items + buffer->tail + buffer->len, 0, ((buffer->capacity / 2) - buffer->tail) * sizeof(void *));

    // move head to the new empty position
    buffer->head += buffer->len;

    return 0;
}

/*! @brief Shrinks the circular buffer in capacity by half. Returns 0, if successful. Else returns non-zero. */
static int cbuf_shrink(cbuf_t *buffer)
{
    // move items so they fit into the first half the array
    if (buffer->head == 0 || buffer->head > buffer->tail) {
        // all items are in one block which is moved to the start of the array
        memcpy(buffer->items, buffer->items + buffer->tail, buffer->len * sizeof(void *));
    } else if (buffer->head < buffer->tail) {
        // items are in two separate groups: one at the start of the array, one at the end of the array
        memcpy(buffer->items + buffer->capacity - buffer->tail, buffer->items, buffer->head * sizeof(void *));
        memcpy(buffer->items, buffer->items + buffer->tail, (buffer->capacity - buffer->tail) * sizeof(void *));
    }

    buffer->tail = 0;
    buffer->head = buffer->len;

    buffer->capacity /= 2;
    void **new_items = realloc(buffer->items, buffer->capacity * sizeof(void *));
    if (new_items == NULL) return 1;

    buffer->items = new_items;

    return 0;
}

static void cbuf_item_free(void *item, void *unused)
{
    UNUSED(unused);
    free(item);
}


/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH CBUF_T                     */
/* *************************************************************************** */

cbuf_t *cbuf_new(void)
{
    return cbuf_with_capacity(CBUF_DEFAULT_CAPACITY);
}

cbuf_t *cbuf_with_capacity(const size_t base_capacity)
{
    cbuf_t *buffer = calloc(1, sizeof(cbuf_t));
    if (buffer == NULL) return NULL;

    buffer->items = calloc(base_capacity, sizeof(void *));
    if (buffer->items == NULL) {
        free(buffer);
        return NULL;
    }

    buffer->capacity = base_capacity;
    buffer->base_capacity = base_capacity;

    return buffer;
}

void cbuf_destroy(cbuf_t *buffer)
{
    if (buffer == NULL) return;

    cbuf_map(buffer, cbuf_item_free, NULL);    

    free(buffer->items);
    free(buffer);
}

int cbuf_enqueue(cbuf_t *buffer, const void *item, const size_t itemsize)
{
    if (buffer == NULL) return 99;

    if (buffer->len >= buffer->capacity) if (cbuf_reallocate(buffer) != 0) return 1;

    // add the item
    buffer->items[buffer->head] = malloc(itemsize);
    memcpy(buffer->items[buffer->head], item, itemsize);

    // move the head pointer
    buffer->head = (buffer->head + 1) % buffer->capacity;
    ++(buffer->len);

    return 0;
}

void *cbuf_dequeue(cbuf_t *buffer, const size_t itemsize)
{
    if (buffer == NULL || buffer->len == 0) return NULL;

    void *data = malloc(itemsize);
    memcpy(data, buffer->items[buffer->tail], itemsize);

    // remove the item
    free(buffer->items[buffer->tail]);
    buffer->items[buffer->tail] = NULL;

    // move the tail pointer
    buffer->tail = (buffer->tail + 1) % buffer->capacity;
    --(buffer->len);

    if (cbuf_check_shrink(buffer)) {
        cbuf_shrink(buffer);
    }

    return data;
}

void *cbuf_peek(const cbuf_t *buffer)
{
    if (buffer == NULL || buffer->len == 0) return NULL;

    return buffer->items[buffer->tail];
}


void cbuf_map(cbuf_t *buffer, void (*function)(void *, void *), void *pointer)
{
    if (buffer == NULL) return;

    for (size_t i = 0; i < buffer->len; ++i) {
        function(buffer->items[(i + buffer->tail) % buffer->capacity], pointer);
    }
}