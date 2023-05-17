// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include "graph.h"

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH GRAPHD_T                 */
/* *************************************************************************** */

#define UNUSED(x) (void)(x)

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

/** @brief Expands the adjacency matrix. Returns 0 if successful or non-zero in case of memory allocation error. */
static int amatrix_expand(edged_t ***amatrix, const size_t old_capacity, const size_t new_capacity)
{
    if (old_capacity == new_capacity) return 0;

    edged_t **new_amatrix = realloc(*amatrix, new_capacity * sizeof(edged_t *));
    if (new_amatrix == NULL) return 1;

    *amatrix = new_amatrix;

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

    return 0;
}

/** @brief Shrinks the adjacency matrix. Returns 0 if successful or non-zero in case of memory allocation error. */
static int amatrix_shrink(edged_t ***amatrix, const size_t old_capacity, const size_t new_capacity)
{
    if (old_capacity == new_capacity) return 0;

    for (size_t i = new_capacity; i < old_capacity; ++i) free((*amatrix)[i]);

    edged_t **new_amatrix = realloc(*amatrix, new_capacity * sizeof(edged_t *));
    if (new_amatrix == NULL) return 1;

    *amatrix = new_amatrix;

    for (size_t i = 0; i < new_capacity; ++i) {
        edged_t *new_row = realloc((*amatrix)[i], new_capacity * sizeof(edged_t));
        if (new_row == NULL) return 2;
        (*amatrix)[i] = new_row;
    }

    return 0;
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
static inline int graphd_index_valid(const graphd_t *graph, const size_t index)
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
    if (!graphd_index_valid(graph, index)) return 2;
 
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
    if (graph == NULL || !graphd_index_valid(graph, index)) return NULL;

    return graph->vertices->items[index];
}

int graphd_edge_add(graphd_t *graph, const size_t index_source, const size_t index_target, const int weight)
{
    if (graph == NULL) return 99;

    // check that the indices are valid
    if (!graphd_index_valid(graph, index_source) || !graphd_index_valid(graph, index_target)) return 1;

    graph->amatrix[index_source][index_target].exists = 1;
    graph->amatrix[index_source][index_target].weight = weight;

    return 0;
}

int graphd_edge_remove(graphd_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 99;

    if (!graphd_index_valid(graph, index_source) || !graphd_index_valid(graph, index_target)) return 1;

    graph->amatrix[index_source][index_target].exists = 0;

    return 0;
}

int graphd_edge_exists(const graphd_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 0;

    if (!graphd_index_valid(graph, index_source) || !graphd_index_valid(graph, index_target)) return 0;

    return edged_exists(graph, index_source, index_target);
}

int graphd_edge_weight(const graphd_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return INT_MIN;
    if (!graphd_index_valid(graph, index_source) || !graphd_index_valid(graph, index_target)) return INT_MIN;
    if (!edged_exists(graph, index_source, index_target)) return INT_MIN;

    return graph->amatrix[index_source][index_target].weight;
}

vec_t *graphd_vertex_successors(const graphd_t *graph, const size_t index)
{
    if (graph == NULL || !graphd_index_valid(graph, index)) return NULL;

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
    if (graph == NULL || !graphd_index_valid(graph, index)) return 0;

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
    if (graph == NULL || !graphd_index_valid(graph, index)) return 0;

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

/* *************************************************************************** */
/*                  PRIVATE FUNCTIONS ASSOCIATED WITH GRAPHS_T                 */
/* *************************************************************************** */

void set_destroy_for_map(void *wrapped_set, void *unused)
{
    UNUSED(unused);

    set_t *set = *(set_t **) wrapped_set;
    set_destroy(set);
}

/** @brief Returns 1 if index points to valid vertex in graph. Else returns 0. */
static inline int graphs_index_valid(const graphs_t *graph, const size_t index)
{
    return index < graph->vertices->len;
}

/** @brief Returns a pointer to target edge. Returns NULL if no such edge exists. Raw function. */
static edges_t *edges_get(const graphs_t *graph, const size_t isrc, const size_t itar)
{
    set_t *adjacency_list = *(set_t **) graph->edges->items[isrc];
    edges_t edge = { .vertex_tar = graph->vertices->items[itar], .weight = 0 };

    return (edges_t *) set_get(adjacency_list, &edge, sizeof(void *));
}

/** @brief Checks whether an edge connecting isrc with itar exists. Returns 1 if it does, else returns 0. */
static inline int edges_exists(const graphs_t *graph, const size_t isrc, const size_t itar)
{
    return edges_get(graph, isrc, itar) != NULL ?  1 : 0;
}

/** @brief Compares targets of two edges. Used as equal_function for adjacency list. */
static int edges_match(const void *edge1, const void *edge2)
{
    return ((edges_t *) edge1)->vertex_tar  == ((edges_t *) edge2)->vertex_tar;
}

/** @brief Hash selection for edges. Used as hashable for adjacency list. */
static const void *edges_hash(const void *edge)
{
    return (void *) &(((edges_t *) edge)->vertex_tar);
}

static void extract_successors(void *item, void *wrapped_vec)
{
    edges_t *edge = (edges_t *) item;
    vec_t *successors = (vec_t *) wrapped_vec;

    vec_push(successors, &(edge->vertex_tar), sizeof(void *));
}

/* PRIVATE FUNCTIONS FOR PATH ALGORITHMS */

/** @brief Key-value entry where key is a pointer to vertex and value is the index of the corresponding vertex. */
typedef struct vertex_index {
    void *vertex;
    size_t index;
} vertex_index_t;

/** @brief `hashable` function for vertex_index. */
static const void *hash_vertex(const void *item)
{
    vertex_index_t *vertex_index = (vertex_index_t *) item;

    return &(vertex_index->vertex);
}

/** @brief Equality function for vertex_index. */
static int vertex_equal(const void *item1, const void *item2)
{
    vertex_index_t *vertex_index1 = (vertex_index_t *) item1;
    vertex_index_t *vertex_index2 = (vertex_index_t *) item2;

    return vertex_index1->vertex == vertex_index2->vertex;
}

/** @brief Performs one relaxation cycle for Bellman-Ford algorithm. Returns 1 if any distance has been updated, else returns 0. */
static int graphs_bellman_ford_relax(const graphs_t *graph, const set_t *vertex_index, int *distances, void **previous)
{
    int updated = 0;

    for (size_t index_src = 0; index_src < graph->vertices->len; ++index_src) {
        vec_t *adjacent = graphs_vertex_successors(graph, index_src);
        for (size_t j = 0; j < adjacent->len; ++j) {

            void *target = *((void **) adjacent->items[j]);

            // get index of the target vertex
            vertex_index_t search = { .vertex = target, .index = 0 };
            vertex_index_t *item = (vertex_index_t *) set_get(vertex_index, &search, sizeof(void *));
            
            size_t index_tar = item->index;

            edges_t *edge = edges_get(graph, index_src, index_tar);

            if (distances[index_src] < INT_MAX && distances[index_tar] > distances[index_src] + edge->weight) {
                distances[index_tar] = distances[index_src] + edge->weight;
                previous[index_tar] = graph->vertices->items[index_src];
                updated = 1;
            }

        }

        vec_destroy(adjacent);
    }

    return updated;
}

/* *************************************************************************** */
/*                 PUBLIC FUNCTIONS ASSOCIATED WITH GRAPHS_T                   */
/* *************************************************************************** */

graphs_t *graphs_new(void)
{
    return graphs_with_capacity(GRAPHS_DEFAULT_CAPACITY);
}

graphs_t *graphs_with_capacity(const size_t capacity)
{
    graphs_t *graph = calloc(1, sizeof(graphs_t));
    if (graph == NULL) return NULL;

    graph->vertices = vec_with_capacity(capacity);
    if (graph->vertices == NULL) {
        free(graph);
        return NULL;
    }

    graph->edges = vec_with_capacity(capacity);
    if (graph->edges == NULL) {
        vec_destroy(graph->vertices);
        free(graph);
        return NULL;
    }

    return graph;
}

void graphs_destroy(graphs_t *graph)
{
    if (graph == NULL) return;

    vec_destroy(graph->vertices);
    vec_map(graph->edges, set_destroy_for_map, NULL);
    vec_destroy(graph->edges);
    free(graph);
}

long graphs_vertex_add(graphs_t *graph, const void *vertex, const size_t vertexsize)
{
    if (graph == NULL) return -99;

    set_t *adjacency_list = set_new(edges_match, edges_hash);
    if (adjacency_list == NULL) return -1;

    vec_push(graph->vertices, vertex, vertexsize);
    vec_push(graph->edges, &adjacency_list, sizeof(set_t *));

    if (graph->vertices->len != graph->edges->len || graph->vertices->capacity != graph->edges->capacity) return -2;

    return graph->vertices->len - 1;
}

void *graphs_vertex_get(const graphs_t *graph, const size_t index)
{
    if (graph == NULL || !graphs_index_valid(graph, index)) return NULL;

    return graph->vertices->items[index];
}

int graphs_vertex_remove(graphs_t *graph, const size_t index)
{
    if (graph == NULL) return 99;
    if (!graphs_index_valid(graph, index)) return 2;

    // remove all edges from target vertex
    void *adjacency_list = vec_remove(graph->edges, index);
    if (adjacency_list == NULL) return 3;
    set_destroy(*(set_t **) adjacency_list);
    free(adjacency_list);

    // remove all edges to target vertex
    for (size_t i = 0; i < graph->edges->len; ++i) {
        set_t *adjacent = *(set_t **) graph->edges->items[i];
        edges_t edge = { .vertex_tar = graph->vertices->items[index], .weight = 0 };
        set_remove(adjacent, &edge, sizeof(void *));
    }

    // remove target vertex
    void *vertex = vec_remove(graph->vertices, index);
    if (vertex == NULL) return 1;
    free(vertex);

    return 0;
}

int graphs_edge_add(graphs_t *graph, const size_t index_source, const size_t index_target, const int weight)
{
    if (graph == NULL) return 99;

    if (!graphs_index_valid(graph, index_source) || !graphs_index_valid(graph, index_target)) return 1;

    set_t *adjacency_list = *(set_t **) graph->edges->items[index_source];
    edges_t edge = { .vertex_tar = graph->vertices->items[index_target], .weight = weight };
    set_add_overwrite(adjacency_list, &edge, sizeof(edges_t), sizeof(void *));

    return 0;
}

int graphs_edge_remove(graphs_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 99;

    if (!graphs_index_valid(graph, index_source) || !graphs_index_valid(graph, index_target)) return 1;

    edges_t edge_to_remove = { .vertex_tar = graph->vertices->items[index_target], .weight = 0 };
    set_remove(*(set_t **) graph->edges->items[index_source], &edge_to_remove, sizeof(void *));

    return 0;
}

int graphs_edge_exists(const graphs_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return 0;

    if (!graphs_index_valid(graph, index_source) || !graphs_index_valid(graph, index_target)) return 0;

    return edges_exists(graph, index_source, index_target);
}

int graphs_edge_weight(const graphs_t *graph, const size_t index_source, const size_t index_target)
{
    if (graph == NULL) return INT_MIN;
    if (!graphs_index_valid(graph, index_source) || !graphs_index_valid(graph, index_target)) return INT_MIN;
    if (!edges_exists(graph, index_source, index_target)) return INT_MIN;

    return edges_get(graph, index_source, index_target)->weight;
}

vec_t *graphs_vertex_successors(const graphs_t *graph, const size_t index)
{
    if (graph == NULL || !graphs_index_valid(graph, index)) return NULL;

    vec_t *successors = vec_new();
    if (successors == NULL) return NULL;

    set_map(*(set_t **) graph->edges->items[index], extract_successors, successors);

    return successors;
}

void graphs_vertex_map(graphs_t *graph, void (*function)(void *, void *), void *pointer)
{
    if (graph == NULL) return;

    vec_map(graph->vertices, function, pointer);
}

size_t graphs_vertex_map_bfs(
        graphs_t *graph, 
        const size_t index, 
        void (*function)(void *, void *), 
        void *pointer)
{
    if (graph == NULL || !graphs_index_valid(graph, index)) return 0;

    set_t *visited = set_with_capacity(graph->vertices->len, equal_sizet, hash_full);
    cbuf_t *queue = cbuf_new();

    cbuf_enqueue(queue, &index, sizeof(size_t));
    set_add(visited, &index, sizeof(size_t), sizeof(size_t));

    while (queue->len != 0) {

        size_t *vertex = (size_t *) cbuf_dequeue(queue);
        function(graph->vertices->items[*vertex], pointer);

        // find successors of vertex and add their indices to the queue, if not visited
        for (size_t i = 0; i < graph->vertices->len; ++i) {
            if (edges_exists(graph, *vertex, i) && !set_contains(visited, &i, sizeof(size_t))) {
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

size_t graphs_vertex_map_dfs(
        graphs_t *graph, 
        const size_t index, 
        void (*function)(void *, void *), 
        void *pointer)
{
    if (graph == NULL || !graphs_index_valid(graph, index)) return 0;

    set_t *visited = set_with_capacity(graph->vertices->len, equal_sizet, hash_full);
    vec_t *stack = vec_new();

    vec_push(stack, &index, sizeof(size_t));
    set_add(visited, &index, sizeof(size_t), sizeof(size_t));

    while (stack->len != 0) {

        size_t *vertex = (size_t *) vec_pop(stack);
        function(graph->vertices->items[*vertex], pointer);

        // find successors of vertex and add their indices to the stack, if not visited
        for (size_t i = 0; i < graph->vertices->len; ++i) {
            if (edges_exists(graph, *vertex, i) && !set_contains(visited, &i, sizeof(size_t))) {
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

int graphs_bellman_ford(
        const graphs_t *graph, 
        const size_t vertex_src, 
        const size_t vertex_tar,
        vec_t **path)
{   
    if (graph == NULL) return INT_MAX;

    if (vertex_src == vertex_tar) {
        *path = vec_new();
        vec_push(*path, &(graph->vertices->items[vertex_tar]), sizeof(void *));
        return 0;
    }

    // dictionary for converting between pointers to vertices and their indices
    set_t *vertex_index = set_with_capacity(graph->vertices->len, vertex_equal, hash_vertex);

    for (size_t i = 0; i < graph->vertices->len; ++i) {
        vertex_index_t item = { .vertex = graph->vertices->items[i], .index = i };
        set_add(vertex_index, &item, sizeof(vertex_index_t), sizeof(void *));
    }

    // array of distances for each vertex
    int *distances = malloc(graph->vertices->len * sizeof(int));
    for (size_t i = 0; i < graph->vertices->len; ++i) distances[i] = INT_MAX;

    // previous vertex for each vertex
    void **previous = calloc(graph->vertices->len, sizeof(void *));

    distances[vertex_src] = 0;

    for (size_t n = 0; n < graph->vertices->len; ++n) {
        // if no distance has been updated, skip to the end of the function
        if (!graphs_bellman_ford_relax(graph, vertex_index, distances, previous)) goto skip;
    }

    // check for negative cycles
    if (graphs_bellman_ford_relax(graph, vertex_index, distances, previous)) return INT_MAX;

    skip:


    if (distances[vertex_tar] == INT_MAX) {
        *path = NULL;
        free(distances);
        free(previous);
        set_destroy(vertex_index);
        return INT_MAX;
    }
    
    *path = vec_new();

    vec_push(*path, &(graph->vertices->items[vertex_tar]), sizeof(void *));

    void *curr = previous[vertex_tar];
    while (curr != NULL) {
        vec_push(*path, &curr, sizeof(void *));

        // get index of the target vertex
        vertex_index_t search = { .vertex = curr, .index = 0 };
        vertex_index_t *item = (vertex_index_t *) set_get(vertex_index, &search, sizeof(void *));
        
        size_t index_tar = item->index;

        curr = previous[index_tar];
    }

    vec_reverse(*path);

    free(previous);
    set_destroy(vertex_index);

    int fdistance = distances[vertex_tar];

    free(distances);

    return fdistance;
}