// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "graph.h"

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH GRAPHD_T                 */
/* *************************************************************************** */

/** @brief Deallocates memory for adjacency matrix. */
static void amatrix_destroy(char **amatrix, const size_t size)
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
static char **amatrix_new(const size_t capacity)
{
    const size_t blocks = capacity >> 3;

    char **amatrix = calloc(capacity, sizeof(char *));
    if (amatrix == NULL) return NULL;

    for (size_t i = 0; i < capacity; ++i) {
        amatrix[i] = calloc(blocks, 1);
        
        if (amatrix[i] == NULL) {
            amatrix_destroy(amatrix, i);
            return NULL;
        }
    }

    return amatrix;
}

/** @brief Expands adjacency matrix. Returns 0 if successful or non-zero in case of memory allocation error. */
static int amatrix_expand(char ***amatrix, const size_t old_capacity, const size_t new_capacity)
{
    const size_t old_blocks = old_capacity >> 3;
    const size_t new_blocks = new_capacity >> 3;

    char **new_amatrix = realloc(*amatrix, new_capacity * sizeof(char *));
    if (new_amatrix == NULL) return 1;

    *amatrix = new_amatrix;

    for (size_t i = 0; i < old_capacity; ++i) {
        char *new_row = realloc((*amatrix)[i], new_blocks);
        if (new_row == NULL) return 2;
        (*amatrix)[i] = new_row;
        memset((*amatrix)[i] + old_blocks, 0, new_blocks - old_blocks);
    }

    for (size_t i = old_capacity; i < new_capacity; ++i) {
        (*amatrix)[i] = calloc(new_blocks, 1);
        if ((*amatrix)[i] == NULL) return 3;
    }

    return 0;
}

/** @brief Checks whether an edge connecting isrc with itar exists. Returns 1 if it does, else returns 0.
 *  Raw function, no sanity checks performed. 
 */
static inline int amatrix_edge_exists(char **amatrix, const size_t isrc, const size_t itar)
{
    return amatrix[isrc][itar >> 3] >> (itar % 8) & 1U;
}

/** @brief Returns 1 if index points to valid vertex in graph. Else returns 0. */
static inline int graph_index_valid(const graphd_t *graph, const size_t index)
{
    return index < graph->vertices->len;
}

/** @brief Utility function for BFS. */
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
    // capacity must be divisible than 8
    // if it is not, we increase it until it reaches a number divisible by 8
    const size_t real_capacity = ((capacity + 7) & ~7);

    graphd_t *graph = calloc(1, sizeof(graphd_t));
    if (graph == NULL) return NULL;

    graph->vertices = vec_with_capacity(real_capacity);
    if (graph->vertices == NULL) {
        free(graph);
        return NULL;
    }

    graph->amatrix = amatrix_new(real_capacity);
    if (graph->amatrix == NULL) {
        vec_destroy(graph->vertices);
        free(graph);
        return NULL;
    }

    graph->allocated = real_capacity;
    graph->base_capacity = real_capacity;

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

void *graphd_vertex_get(const graphd_t *graph, const size_t index)
{
    if (graph == NULL || !graph_index_valid(graph, index)) return NULL;

    return graph->vertices->items[index];
}

int graphd_edge_add(graphd_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 99;

    // check that the indices are valid
    if (!graph_index_valid(graph, index_source) || !graph_index_valid(graph, index_target)) return 1;

    // set the bit corresponding to target edge
    graph->amatrix[index_source][index_target >> 3] |= 1U << (index_target % 8);

    return 0;
}

int graphd_edge_remove(graphd_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 99;

    if (!graph_index_valid(graph, index_source) || !graph_index_valid(graph, index_target)) return 1;

    graph->amatrix[index_source][index_target >> 3] &= ~(1UL << (index_target % 8));

    return 0;
}

int graphd_edge_exists(const graphd_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 0;

    if (!graph_index_valid(graph, index_source) || !graph_index_valid(graph, index_target)) return 0;

    return amatrix_edge_exists(graph->amatrix, index_source, index_target);
}

vec_t *graphd_vertex_successors(const graphd_t *graph, const size_t index)
{
    if (graph == NULL || !graph_index_valid(graph, index)) return NULL;

    vec_t *successors = vec_new();
    if (successors == NULL) return NULL;

    for (size_t i = 0; i < graph->vertices->len; ++i) {
        if (amatrix_edge_exists(graph->amatrix, index, i)) {
            
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
            if (amatrix_edge_exists(graph->amatrix, *vertex, i) && !set_contains(visited, &i, sizeof(size_t))) {
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

        // find successors of vertex and add their indices to the queue, if not visited
        for (size_t i = 0; i < graph->vertices->len; ++i) {
            if (amatrix_edge_exists(graph->amatrix, *vertex, i) && !set_contains(visited, &i, sizeof(size_t))) {
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