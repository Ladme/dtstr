// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

// Implementations of graphs. All graphs are oriented and support integer weights for edges.

/* SOME NOTES ON PERFORMANCE:
 * (Tested on graphs with betwen 500 and 10,000 vertices.)
 *
 * 0) Memory             {graphs_t wins}
 * > graphs_t usually requires much less memory than graphd_t (an order of magnitude for sparse graphs)
 * > for graphs in which at least ~12% of all possible edges are present, graphd_t will be more memory efficient than graphs_t
 *
 * 1) Adding vertices    {graphs_t wins}
 * > graphs_t is usually MUCH faster than graphd_t
 * > graphd_t is faster than graphs_t if no memory reallocation is needed, 
 *   i.e. if you know how many vertices the graph should fit and preallocate enough space for them
 * 
 * 2) Removing vertices  {graphs_t wins}
 * > graphs_t is SEVERAL ORDERS OF MAGNITUDE faster than graphd_t for all graphs except for very small graphs (~100 vertices)
 * 
 * 3) Adding edges       {graphd_t wins}
 * > graphd_t is always faster than graphs_t, especially for dense graphs
 * > the speed difference increases with increasing density of the graph
 * > for dense graphs, graphd_t is always at least an order of magnitude faster (often two orders of magnitude) than graphs_t
 * 
 * 4) Checking edges     {comparable}
 * > for most graphs, graphd_t and graphs_t are similar in speed
 * > for dense graphs, graphd_t tends to be faster
 * 
 * 5) Removing edges     {graphd_t wins}
 * > graphd_t is slightly faster than graphs_t for most graphs
 * 
 * tl;dr Use graphs_t, unless...
 * a) your graph contains a large number of edges (E > 0.1 V^2), or
 * b) you plan to repeatedly add and remove edges from the graph AND are not limited by memory.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cbuffer.h"
#include "set.h"
#include "vector.h"

/* ************************************************ */
/*              GRAPHD_T: DENSE GRAPH               */
/* ************************************************ */

/** @brief Edge in a dense graph. */
typedef struct edge_dense {
    int exists;
    int weight;
} edged_t;

/** 
 * @brief Structure efficiently representing dense oriented graph. 
 * 
 * @note - Edges are represented using adjacency matrix.
 */
typedef struct graph_dense {
    vec_t *vertices;    // vector of vertices in the graph
    edged_t **amatrix;   // adjacency matrix
    size_t allocated;   // number of vertices for which space has been allocated in amatrix
    size_t base_capacity;
} graphd_t;

/** @brief Default capacity used for vertices and amatrix (dense graph). */
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
 * @brief Removes vertex from graph.
 * 
 * @param graph     The graph to operate on
 * @param index     Index of the vertex to remove
 * 
 * @return 
 * 0 if successfully removed. 
 * 1 if the vertex could not be removed. 
 * 2 if the index is out of range. 
 * 3 if the adjacency matrix could not be shrunk.
 * 99 if the graph does not exist.
 */
int graphd_vertex_remove(graphd_t *graph, const size_t index);

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
 * @return 0 if the edge was successfully removed (or was not present), 
 *         1 if the indices are out of range, or 99 if the graph is NULL.
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
 * @brief Retrieves the weight of the edge between two vertices in a graph.
 *
 * @param graph         The graph to operate on
 * @param index_source  Index of the source vertex
 * @param index_target  Index of the target vertex
 *
 * @return The weight of the edge between the vertices at the specified indices. Returns INT_MIN if the graph is NULL,
 * the specified indices are invalid, or there is no edge between the vertices.
 */
int graphd_edge_weight(const graphd_t *graph, const size_t index_source, const size_t index_target);


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
 * @note - All vertices are traversed, even if completely unreachable.
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


/* ************************************************ */
/*              GRAPHS_T: SPARSE GRAPH              */
/* ************************************************ */

/** @brief Edge in a sparse graph. */
typedef struct edge_sparse {
    void *vertex_tar;   // pointer to target vertex
    int weight;
} edges_t;


/** 
 * @brief Structure efficiently representing sparse oriented graph. 
 * 
 * @note - Edges are represented using adjacency list.
 */
typedef struct graph_sparse {
    vec_t *vertices;     // vector of vertices in the graph
    vec_t *edges;        // vector of sets of edges
} graphs_t;

/** @brief Default capacity used for vertices and edges (sparse graph). */
#define GRAPHS_DEFAULT_CAPACITY 16UL


/** 
 * @brief Creates new `graphs_t` structure and allocates memory for it.
 * 
 * @note - Destroy `graphs_t` structure using graphs_destroy function.
 * @note - Allocates space for `GRAPHS_DEFAULT_CAPACITY` vertices and `GRAPHS_DEFAULT_CAPACITY` adjacency lists.
 *         This space is dynamically expanded when needed but expanding the graph is a costly operation.
 *         You may want to preallocate memory for a specific number of vertices using `graphs_with_capacity` function.
 * 
 * @return Pointer to the created `graphs_t`, if successful. NULL if not successful.
 */
graphs_t *graphs_new(void);


/**
 * @brief Creates a new `graphs_t` structure and preallocates space for a specified number of vertices and adjacency lists.
 *
 * @param capacity     The initial capacity of the graph.
 * 
 * @note - Destroy `graphs_t` structure using graphs_destroy function.
 * @note - The graph will never shrink below the specified `capacity`.
 * 
 * @return Pointer to the created `graphs_t`, if successful. NULL if not successful.
 */
graphs_t *graphs_with_capacity(const size_t capacity);


/**
 * @brief Properly deallocates memory for the given sparse graph and destroys the `graphs_t` structure.
 *
 * @param graph    The `graphs_t` structure to destroy.
 */
void graphs_destroy(graphs_t *graph);


/**
 * @brief Adds a new vertex to the graph and initializes an adjacency list for it.
 *
 * @param graph         The graph to operate on
 * @param vertex        Pointer to the vertex data
 * @param vertexsize    The size of the vertex data
 *
 * @note - In case of an error, the function returns a negative value: 
 * @note -99, if the graph is NULL.
 * @note -1, if the adjancency list could not be initialized.
 * @note -2, if there is a mismatch between the length or capacity of the vertices and edges vectors.
 *         (This would mean that pushing into one of the vectors failed, i.e. likely a memory error.)
 * 
 * @return Index of the added vertex. Negative value if the vertex could not be added to the graph.
 */
long graphs_vertex_add(graphs_t *graph, const void *vertex, const size_t vertexsize);


/**
 * @brief Removes vertex from graph.
 * 
 * @param graph     The graph to operate on
 * @param index     Index of the vertex to remove
 * 
 * @return 
 * 0 if successfully removed. 
 * 1 if the vertex could not be removed. 
 * 2 if the index is out of range. 
 * 3 if the adjacency list for the vertex could not be removed.
 * 99 if the graph does not exist.
 */
int graphs_vertex_remove(graphs_t *graph, const size_t index);

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
 * @return Pointer to the vertex with specified index. NULL if index is invalid or if the graph does not exist.
 */
void *graphs_vertex_get(const graphs_t *graph, const size_t index);


/**
 * @brief Adds an edge to a graph.
 *
 * @param graph         The graph to operate on
 * @param index_source  Index of the source vertex.
 * @param index_target  Index of the target vertex.
 * @param weight        Weight of the edge.
 * 
 * @return 0 if successful, 1 if index is out of range, 99 if graph is NULL.
 */
int graphs_edge_add(graphs_t *graph, const size_t index_source, const size_t index_target, const int weight);

/** 
 * @brief Removes an edge connecting vertex with index `index_source` to vertex with index `index_target`. 
 *
 * @param graph         The graph to operate on
 * @param index_source  Index of the source vertex 
 * @param index_target  Index of the target vertex
 * 
 * @note - Asymptotic Complexity: Constant, O(1)
 * 
 * @return 0 if the edge was successfully removed (or was not present), 
 *         1 if the indices are out of range, or 99 if the graph is NULL.
 */
int graphs_edge_remove(graphs_t *graph, const size_t index_source, const size_t index_target);

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
int graphs_edge_exists(const graphs_t *graph, const size_t index_source, const size_t index_target);

/**
 * @brief Retrieves the weight of the edge between two vertices in a graph.
 *
 * @param graph         The graph to operate on
 * @param index_source  Index of the source vertex
 * @param index_target  Index of the target vertex
 *
 * @return The weight of the edge between the vertices at the specified indices. Returns INT_MIN if the graph is NULL,
 * the specified indices are invalid, or there is no edge between the vertices.
 */
int graphs_edge_weight(const graphs_t *graph, const size_t index_source, const size_t index_target);


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
vec_t *graphs_vertex_successors(const graphs_t *graph, const size_t index);


/** 
 * @brief Loops through all vertices in graph and applies `function` to each vertex.
 * 
 * @param graph     Graph to operate on
 * @param function  Function to apply
 * @param pointer   Pointer to value that the function can operate on
 * 
 * @note - Vertices are traversed starting at index 0, i.e. not respecting the structure of the graph.
 * @note - All vertices are traversed, even if completely unreachable.
 */
void graphs_vertex_map(graphs_t *graph, void (*function)(void *, void *), void *pointer);

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
size_t graphs_vertex_map_bfs(
        graphs_t *graph, 
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
size_t graphs_vertex_map_dfs(
        graphs_t *graph, 
        const size_t index, 
        void (*function)(void *, void *), 
        void *pointer);

#endif /* GRAPH_H */