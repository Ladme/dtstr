// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "graph.h"

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH GRAPHD_T                 */
/* *************************************************************************** */

/** @brief Deallocates memory for adjacency matrix. */
static void amatrix_destroy(edged_t **amatrix, const size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        free(amatrix[i]); 
    }

    free(amatrix);
}

/** 
 * @brief Allocates memory for new adjacency matrix. 
 * Returns pointer to amatrix or NULL if memory allocation fails. 
 */
static edged_t **amatrix_new(const size_t capacity)
{
    edged_t **amatrix = calloc(capacity, sizeof(edged_t *));
    if (amatrix == NULL) return NULL;

    for (size_t i = 0; i < capacity; ++i) {
        amatrix[i] = calloc(capacity, sizeof(edged_t));
        
        if (amatrix[i] == NULL) {
            amatrix_destroy(amatrix, i);
            return NULL;
        }
    }

    return amatrix;
}

/** @brief Reallocates memory for adjacency matrix, expanding it or shrinking it.
 * Returns 0 if successful or non-zero in case of memory allocation error. 
 */
static int amatrix_realloc(edged_t ***amatrix, const size_t old_capacity, const size_t new_capacity)
{
    if (old_capacity == new_capacity) return 0;

    // shrinking matrix
    if (new_capacity < old_capacity) {
        // free unused rows
        for (size_t i = new_capacity; i < old_capacity; ++i) free((*amatrix)[i]);
    }

    edged_t **new_amatrix = realloc(*amatrix, new_capacity * sizeof(edged_t *));
    if (new_amatrix == NULL) return 1;

    *amatrix = new_amatrix;

    // expanding matrix
    if (old_capacity < new_capacity) {
        for (size_t i = 0; i < old_capacity; ++i) {
            edged_t *new_row = realloc((*amatrix)[i], new_capacity * sizeof(edged_t));
            if (new_row == NULL) return 2;
            (*amatrix)[i] = new_row;
            memset((*amatrix)[i] + old_capacity, 0, (new_capacity - old_capacity) * sizeof(edged_t));
        }

        for (size_t i = old_capacity; i < new_capacity; ++i) {
            (*amatrix)[i] = calloc(new_capacity, sizeof(edged_t));
            if ((*amatrix)[i] == NULL) return 3;
        }
    } 
    // shrinking matrix
    else {
        for (size_t i = 0; i < new_capacity; ++i) {
            edged_t *new_row = realloc((*amatrix)[i], new_capacity * sizeof(edged_t));
            if (new_row == NULL) return 2;
            (*amatrix)[i] = new_row;
        }
    }

    return 0;
}

/** @brief Expands adjacency matrix. Returns 0 if successful or non-zero in case of memory allocation error. */
static inline int amatrix_expand(edged_t ***amatrix, const size_t old_capacity, const size_t new_capacity)
{
    return amatrix_realloc(amatrix, old_capacity, new_capacity);
}

/** @brief Shrink the adjacency matrix. Returns 0 if successful or non-zero in case of memory allocation error. */
static inline int amatrix_shrink(edged_t ***amatrix, const size_t old_capacity, const size_t new_capacity)
{
    return amatrix_realloc(amatrix, old_capacity, new_capacity);
}

static void amatrix_remove_vertex(edged_t **amatrix, const size_t index, const size_t length)
{
    free(amatrix[index]);

    // move pointers to rows
    memcpy(amatrix + index, amatrix + index + 1, sizeof(edged_t *) * (length - index - 1));
    amatrix[length - 1] = NULL;

    // loop through the rows and move the individual edges
    for (size_t i = 0; i < length - 1; ++i) {
        memcpy(amatrix[i] + index, amatrix[i] + index + 1, sizeof(edged_t) * (length - index - 1));
    }
}

/** @brief Checks whether an edge connecting isrc with itar exists. Returns 1 if it does, else returns 0.
 *  Raw function, no sanity checks performed. 
 */
static inline int edged_exists(const graphd_t *graph, const size_t isrc, const size_t itar)
{
    return graph->amatrix[isrc][itar].exists;
}

/** @brief Returns 1 if index points to valid vertex in graph. Else returns 0. */
static inline int graph_index_valid(const graphd_t *graph, const size_t index)
{
    return index < graph->vertices->len;
}

/** @brief Utility function for BFS/DFS. */
static int equal_sizet(const void *item1, const void *item2)
{
    return (*(size_t *) item1) == (*(size_t *) item2);
}

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH GRAPHD_T                   */
/* *************************************************************************** */

graphd_t *graphd_new(void)
{
    return graphd_with_capacity(GRAPHD_DEFAULT_CAPACITY);
}

graphd_t *graphd_with_capacity(const size_t capacity)
{
    graphd_t *graph = calloc(1, sizeof(graphd_t));
    if (graph == NULL) return NULL;

    graph->vertices = vec_with_capacity(capacity);
    if (graph->vertices == NULL) {
        free(graph);
        return NULL;
    }

    graph->amatrix = amatrix_new(capacity);
    if (graph->amatrix == NULL) {
        vec_destroy(graph->vertices);
        free(graph);
        return NULL;
    }

    graph->allocated = capacity;
    graph->base_capacity = capacity;

    return graph;
}

void graphd_destroy(graphd_t *graph)
{
    if (graph == NULL) return;

    vec_destroy(graph->vertices);
    amatrix_destroy(graph->amatrix, graph->allocated);
    free(graph);
}

long graphd_vertex_add(graphd_t *graph, const void *vertex, const size_t vertexsize)
{
    if (graph == NULL) return -99;

    // expand the adjacency matrix, if needed
    if (graph->vertices->len >= graph->allocated) {
        if (amatrix_expand(&(graph->amatrix), graph->allocated, graph->allocated * 2) != 0) return -1;
        graph->allocated *= 2;
    }

    vec_push(graph->vertices, vertex, vertexsize);

    return graph->vertices->len - 1;
}

int graphd_vertex_remove(graphd_t *graph, const size_t index)
{
    if (graph == NULL) return 99;
    if (!graph_index_valid(graph, index)) return 2;
 
    void *vertex = vec_remove(graph->vertices, index);
    if (vertex == NULL) return 1;
    free(vertex);

    // modify amatrix
    amatrix_remove_vertex(graph->amatrix, index, graph->vertices->len + 1);

    // shrink amatrix
    if (graph->vertices->capacity < graph->allocated) {
        if (amatrix_shrink(&(graph->amatrix), graph->allocated, graph->vertices->capacity) != 0) return 3;
        graph->allocated = graph->vertices->capacity;
    }

    return 0;
}


void *graphd_vertex_get(const graphd_t *graph, const size_t index)
{
    if (graph == NULL || !graph_index_valid(graph, index)) return NULL;

    return graph->vertices->items[index];
}

int graphd_edge_add(graphd_t *graph, const size_t index_source, const size_t index_target, const int weight)
{
    if (graph == NULL) return 99;

    // check that the indices are valid
    if (!graph_index_valid(graph, index_source) || !graph_index_valid(graph, index_target)) return 1;

    graph->amatrix[index_source][index_target].exists = 1;
    graph->amatrix[index_source][index_target].weight = weight;

    return 0;
}

int graphd_edge_remove(graphd_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 99;

    if (!graph_index_valid(graph, index_source) || !graph_index_valid(graph, index_target)) return 1;

    graph->amatrix[index_source][index_target].exists = 0;

    return 0;
}

int graphd_edge_exists(const graphd_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 0;

    if (!graph_index_valid(graph, index_source) || !graph_index_valid(graph, index_target)) return 0;

    return edged_exists(graph, index_source, index_target);
}

vec_t *graphd_vertex_successors(const graphd_t *graph, const size_t index)
{
    if (graph == NULL || !graph_index_valid(graph, index)) return NULL;

    vec_t *successors = vec_new();
    if (successors == NULL) return NULL;

    for (size_t i = 0; i < graph->vertices->len; ++i) {
        if (edged_exists(graph, index, i)) {
            
            if (vec_push(successors, &(graph->vertices->items[i]), sizeof(void *)) != 0) {
                vec_destroy(successors);
                return NULL;
            }
        }   
    }

    return successors;
}

void graphd_vertex_map(graphd_t *graph, void (*function)(void *, void *), void *pointer)
{
    if (graph == NULL) return;

    vec_map(graph->vertices, function, pointer);
}

size_t graphd_vertex_map_bfs(
        graphd_t *graph, 
        const size_t index, 
        void (*function)(void *, void *), 
        void *pointer)
{
    if (graph == NULL || !graph_index_valid(graph, index)) return 0;

    set_t *visited = set_with_capacity(graph->vertices->len, equal_sizet, hash_full);
    cbuf_t *queue = cbuf_new();

    cbuf_enqueue(queue, &index, sizeof(size_t));
    set_add(visited, &index, sizeof(size_t), sizeof(size_t));

    while (queue->len != 0) {

        size_t *vertex = (size_t *) cbuf_dequeue(queue);
        function(graph->vertices->items[*vertex], pointer);

        // find successors of vertex and add their indices to the queue, if not visited
        for (size_t i = 0; i < graph->vertices->len; ++i) {
            if (edged_exists(graph, *vertex, i) && !set_contains(visited, &i, sizeof(size_t))) {
                cbuf_enqueue(queue, &i, sizeof(size_t));
                set_add(visited, &i, sizeof(size_t), sizeof(size_t));
            } 
        }

        free(vertex);

    }

    size_t n_visited = visited->len;

    set_destroy(visited);
    cbuf_destroy(queue);

    return n_visited;
}


size_t graphd_vertex_map_dfs(
        graphd_t *graph, 
        const size_t index, 
        void (*function)(void *, void *), 
        void *pointer)
{
    if (graph == NULL || !graph_index_valid(graph, index)) return 0;

    set_t *visited = set_with_capacity(graph->vertices->len, equal_sizet, hash_full);
    vec_t *stack = vec_new();

    vec_push(stack, &index, sizeof(size_t));
    set_add(visited, &index, sizeof(size_t), sizeof(size_t));

    while (stack->len != 0) {

        size_t *vertex = (size_t *) vec_pop(stack);
        function(graph->vertices->items[*vertex], pointer);

        // find successors of vertex and add their indices to the stack, if not visited
        for (size_t i = 0; i < graph->vertices->len; ++i) {
            if (edged_exists(graph, *vertex, i) && !set_contains(visited, &i, sizeof(size_t))) {
                vec_push(stack, &i, sizeof(size_t));
                set_add(visited, &i, sizeof(size_t), sizeof(size_t));
            } 
        }

        free(vertex);

    }

    size_t n_visited = visited->len;

    set_destroy(visited);
    vec_destroy(stack);

    return n_visited;
}