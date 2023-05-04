// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementations of graphs.

#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cbuffer.h"
#include "set.h"
#include "vector.h"

/* ************************************************ */
/*              GRAPHD_T: DENSE GRAPH               */
/* ************************************************ */

typedef struct edge {
    int exists;
    int weight;
} edge_t;

/** @brief Structure efficiently representing dense oriented graph. */
typedef struct graph_dense {
    vec_t *vertices;    // vector of vertices in the graph
    edge_t **amatrix;   // adjacency matrix
    size_t allocated;   // number of vertices for which space has been allocated in amatrix
    size_t base_capacity;
} graphd_t;

/** @brief Default capacity used for vertices and amatrix (dense graph). Should be divisible by 8. */
#define GRAPHD_DEFAULT_CAPACITY 16UL


/** 
 * @brief Creates new `graphd_t` structure and allocates memory for it.
 * 
 * @note - Destroy `graphd_t` structure using graphd_destroy function.
 * @note - Allocates space for `GRAPHD_DEFAULT_CAPACITY` vertices and `GRAPHD_DEFAULT_CAPACITY`^2 edges.
 *         This space is dynamically expanded when needed but expanding the graph is a costly operation.
 *         You may want to preallocate memory for a specific number of vertices using `graphd_with_capacity` function.
 * 
 * @return Pointer to the created `graphd_t`, if successful. NULL if not successful.
 */
graphd_t *graphd_new(void);


/**
 * @brief Creates a new `graphd_t` structure and preallocates space for a specified number of vertices and edges.
 *
 * @param capacity     The initial capacity of the graph.
 * 
 * @note - Destroy `graphd_t` structure using graphd_destroy function.
 * @note - The graph will never shrink below the specified `capacity`.
 * 
 * @return Pointer to the created `graphd_t`, if successful. NULL if not successful.
 */
graphd_t *graphd_with_capacity(const size_t capacity);


/**
 * @brief Properly deallocates memory for the given dense graph and destroys the `graphd_t` structure.
 *
 * @param graph    The `graphd_t` structure to destroy.
 */
void graphd_destroy(graphd_t *graph);


/**
 * @brief Adds vertex to graph. Returns the index that was assigned to the vertex.
 * 
 * @param graph         The graph to operate on
 * @param vertex        Pointer to the vertex that should be added to the graph
 * @param vertexsize    Size of the vertex in bytes
 *
 * @note - Vertex can be of any data type (vertices pointing to heap-allocated data must be manually deallocated).
 * @note - In case of an error, the function returns a negative value. If the graph is NULL, -99 is returned.
 *         If the adjacency matrix of the graph could not be expanded (memory allocation error), -1 is returned.
 * 
 * @return Index of the added vertex. Negative value if the vertex could not be added to the graph.
 */
long graphd_vertex_add(graphd_t *graph, const void *vertex, const size_t vertexsize);


/**
 * @brief Returns a pointer to the vertex with the specified index.
 * 
 * @param vector    The graph to operate on
 * @param index     The index of the vertex to retrieve
 * 
 * @note - The returned pointer is no longer valid once the parent graph is destroyed.
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return Pointer to the vertex with specified index. NULL if index is out-of-bounds or if the graph does not exist.
 */
void *graphd_vertex_get(const graphd_t *graph, const size_t index);


/** 
 * @brief Adds an edge connecting vertex with index `index_source` to vertex with index `index_target`. 
 *
 * @param graph         The graph to operate on
 * @param index_source  Index of the source vertex 
 * @param index_target  Index of the target vertex
 * @param weight        Weight of the edge
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return 0 if the edge was successfully added (or already exists), 1 if the indices are out of range.
 */
int graphd_edge_add(graphd_t *graph, const size_t index_source, const size_t index_target, const int weight);

/** 
 * @brief Removes an edge connecting vertex with index `index_source` to vertex with index `index_target`. 
 *
 * @param graph         The graph to operate on
 * @param index_source  Index of the source vertex 
 * @param index_target  Index of the target vertex
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return 0 if the edge was successfully removed (or was not present), 1 if the indices are out of range.
 */
int graphd_edge_remove(graphd_t *graph, const size_t index_source, const size_t index_target);

/** 
 * @brief Checks whether an edge connecting vertex with index `index_source` to vertex with index `index_target` exists.
 *
 * @param graph         The graph to operate on
 * @param index_source  Index of the source vertex 
 * @param index_target  Index of the target vertex
 * 
 * @note - If the graph is NULL or the indices are out of range, returns 0.
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return 1 if the edge exists, else 0.
 */
int graphd_edge_exists(const graphd_t *graph, const size_t index_source, const size_t index_target);


/** 
 * @brief Returns a vector of successor vertices for vertex at target index.
 *
 * @param graph         The graph to operate on
 * @param index         Index of the source vector
 * 
 * @note - The output vector contains pointers to the pointers stored in the parent graph.
 *         Therefore, the pointers in the output vector are only valid while the parent graph exists.
 * @note - The caller is responsible for deallocating memory for the output vector.
 * 
 * @note - If the graph is NULL or the index is out of range, returns NULL.
 * @note - If memory allocation fails, returns NULL.
 * @note - Asymptotic Complexity: Linear, O(n)
 * 
 * @return Pointer to a vector of successor vertices, if successful. Else returns NULL.
 */
vec_t *graphd_vertex_successors(const graphd_t *graph, const size_t index);


/** 
 * @brief Loops through all vertices in graph and applies `function` to each vertex.
 * 
 * @param graph     Graph to operate on
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Vertices are traversed starting at index 0, i.e. not respecting the structure of the graph.
 * @note - All vertices are traversed, even though that are completely unreachable.
 */
void graphd_vertex_map(graphd_t *graph, void (*function)(void *, void *), void *pointer);


/**
 * @brief Performs a breadth-first search of the graph starting 
 * from the vertex specified by `index` and applies `function` to each vertex.
 *
 * @param graph     The graph to perform the search on.
 * @param index     The index of the vertex to start the search from.
 * @param function  The function to call with the vertex data and the pointer.
 * @param pointer   The pointer to pass to the function.
 * 
 * @return The number of vertices visited.
 * 
 * @note - If graph is NULL or the index is out of range, this function does nothing and returns 0.
 * @note - When multiple vertices are at the same distance from 
 *         the initial vertex, the order in which they are visited is undefined.
 * @note - The `function` must not modify the graph structure, or the behavior is undefined.
 */
size_t graphd_vertex_map_bfs(
        graphd_t *graph, 
        const size_t index, 
        void (*function)(void *, void *), 
        void *pointer);


/**
 * @brief Performs a depth-first search of the graph starting 
 * from the vertex specified by `index` and applies `function` to each vertex.
 *
 * @param graph     The graph to perform the search on.
 * @param index     The index of the vertex to start the search from.
 * @param function  The function to call with the vertex data and the pointer.
 * @param pointer   The pointer to pass to the function.
 * 
 * @return The number of vertices visited.
 * 
 * @note - If graph is NULL or the index is out of range, this function does nothing and returns 0.
 * @note - When multiple vertices are at the same distance from 
 *         the initial vertex, the order in which they are visited is undefined.
 * @note - The `function` must not modify the graph structure, or the behavior is undefined.
 */
size_t graphd_vertex_map_dfs(
        graphd_t *graph, 
        const size_t index, 
        void (*function)(void *, void *), 
        void *pointer);

#endif /* GRAPH_H */