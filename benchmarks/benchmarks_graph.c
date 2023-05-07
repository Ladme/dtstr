// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/graph.h"

#define UNUSED(x) (void)(x)

static graphd_t *create_random_graphd(const size_t n_vertices, const size_t n_edges)
{
    assert(n_edges <= n_vertices * n_vertices);

    graphd_t *graph = graphd_new();

    for (size_t i = 0; i < n_vertices; ++i) {
        int value = rand();
        graphd_vertex_add(graph, &value, sizeof(int));
    }

    //size_t edges_not_added = 0;
    for (size_t i = 0; i < n_edges; ++i) {
        int src = rand() % n_vertices;
        int tar = rand() % n_vertices;
        int weight = rand() % 1000;

        //if (graphd_edge_exists(graph, src, tar)) ++edges_not_added;
        graphd_edge_add(graph, src, tar, weight);
    }
    //printf("Actually added %lu edges.\n", n_edges - edges_not_added);

    return graph;
}

static graphs_t *create_random_graphs(const size_t n_vertices, const size_t n_edges)
{
    assert(n_edges <= n_vertices * n_vertices);

    graphs_t *graph = graphs_new();

    for (size_t i = 0; i < n_vertices; ++i) {
        int value = rand() % 10000;
        graphs_vertex_add(graph, &value, sizeof(int));
    }

    //size_t edges_not_added = 0;
    for (size_t i = 0; i < n_edges; ++i) {
        int src = rand() % n_vertices;
        int tar = rand() % n_vertices;
        int weight = rand() % 1000;

        //if (graphs_edge_exists(graph, src, tar)) ++edges_not_added;
        graphs_edge_add(graph, src, tar, weight);
    }
    //printf("Actually added %lu edges.\n", n_edges - edges_not_added);

    return graph;
}

static void vertex_add(const size_t items_to_add, const size_t vertex_factor, const size_t edge_factor)
{
    for (size_t i = 0; i <= 10; ++i) {

        graphd_t *graph_dense = create_random_graphd(vertex_factor * i, edge_factor * i);
        graphs_t *graph_sparse = create_random_graphs(vertex_factor * i, edge_factor * i);

        assert(graph_dense->vertices->len == vertex_factor * i);
        assert(graph_sparse->vertices->len == vertex_factor * i);

        printf("\nPreallocated vertices: %lu\n", vertex_factor * i);
        printf("Preallocated edges: %lu\n", edge_factor * i);

        clock_t start = clock();

        for (size_t j = 0; j < items_to_add; ++j) {
            int value = rand();
            assert(graphd_vertex_add(graph_dense, &value, sizeof(int)) >= 0);
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> dense graph:  adding %lu vertices: %f s\n", items_to_add, time_elapsed);

        start = clock();

        for (size_t j = 0; j < items_to_add; ++j) {
            int value = rand();
            assert(graphs_vertex_add(graph_sparse, &value, sizeof(int)) >= 0);
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> sparse graph: adding %lu vertices: %f s\n", items_to_add, time_elapsed);

        graphd_destroy(graph_dense);
        graphs_destroy(graph_sparse);
    }
}

static void benchmark_vertex_add_sparse(const size_t items)
{
    printf("%s\n", "benchmark_vertex_add (sparse)");

    vertex_add(items, 1000, 1000);

    printf("\n\n");
}

static void benchmark_vertex_add_intermediate(const size_t items)
{
    printf("%s\n", "benchmark_vertex_add (intermediate)");

    vertex_add(items, 1000, 10000);

    printf("\n\n");
}

static void benchmark_vertex_add_dense(const size_t items)
{
    printf("%s\n", "benchmark_vertex_add (dense)");

    vertex_add(items, 500, 200000);

    printf("\n\n");
}

static void edge_add(const size_t items_to_add, const size_t vertex_factor, const size_t edge_factor)
{
    for (size_t i = 1; i <= 10; ++i) {

        graphd_t *graph_dense = create_random_graphd(vertex_factor * i, edge_factor * i);
        graphs_t *graph_sparse = create_random_graphs(vertex_factor * i, edge_factor * i);

        assert(graph_dense->vertices->len == vertex_factor * i);
        assert(graph_sparse->vertices->len == vertex_factor * i);

        printf("\nPreallocated vertices: %lu\n", vertex_factor * i);
        printf("Preallocated edges: %lu\n", edge_factor * i);

        clock_t start = clock();

        for (size_t j = 0; j < items_to_add; ++j) {
            int src = rand() % graph_dense->vertices->len;
            int tar = rand() % graph_dense->vertices->len;
            int weight = rand() % 1000;
            assert(graphd_edge_add(graph_dense, src, tar, weight) == 0);
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> dense graph:  adding %lu edges: %f s\n", items_to_add, time_elapsed);

        start = clock();

        for (size_t j = 0; j < items_to_add; ++j) {
            int src = rand() % graph_sparse->vertices->len;
            int tar = rand() % graph_sparse->vertices->len;
            int weight = rand() % 1000;
            assert(graphs_edge_add(graph_sparse, src, tar, weight) == 0);
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> sparse graph: adding %lu edges: %f s\n", items_to_add, time_elapsed);

        graphd_destroy(graph_dense);
        graphs_destroy(graph_sparse);
    }
}

static void benchmark_edge_add_sparse(const size_t items)
{
    printf("%s\n", "benchmark_edge_add (sparse)");

    edge_add(items, 1000, 1000);

    printf("\n\n");
}

static void benchmark_edge_add_intermediate(const size_t items)
{
    printf("%s\n", "benchmark_edge_add (intermediate)");

    edge_add(items, 1000, 10000);

    printf("\n\n");
}

static void benchmark_edge_add_dense(const size_t items)
{
    printf("%s\n", "benchmark_edge_add (dense)");

    edge_add(items, 500, 200000);

    printf("\n\n");
}


static void edge_check(const size_t repeats, const size_t vertex_factor, const size_t edge_factor)
{
    for (size_t i = 1; i <= 10; ++i) {

        graphd_t *graph_dense = create_random_graphd(vertex_factor * i, edge_factor * i);
        graphs_t *graph_sparse = create_random_graphs(vertex_factor * i, edge_factor * i);

        assert(graph_dense->vertices->len == vertex_factor * i);
        assert(graph_sparse->vertices->len == vertex_factor * i);

        printf("\nPreallocated vertices: %lu\n", vertex_factor * i);
        printf("Preallocated edges: %lu\n", edge_factor * i);

        clock_t start = clock();

        for (size_t j = 0; j < repeats; ++j) {
            int src = rand() % graph_dense->vertices->len;
            int tar = rand() % graph_dense->vertices->len;
            graphd_edge_exists(graph_dense, src, tar);
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> dense graph:  checking %lu edges: %f s\n", repeats, time_elapsed);

        start = clock();

        for (size_t j = 0; j < repeats; ++j) {
            int src = rand() % graph_sparse->vertices->len;
            int tar = rand() % graph_sparse->vertices->len;
            graphs_edge_exists(graph_sparse, src, tar);
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> sparse graph: checking %lu edges: %f s\n", repeats, time_elapsed);

        graphd_destroy(graph_dense);
        graphs_destroy(graph_sparse);
    }
}

static void benchmark_edge_check_sparse(const size_t items)
{
    printf("%s\n", "benchmark_edge_check (sparse)");

    edge_check(items, 1000, 1000);

    printf("\n\n");
}

static void benchmark_edge_check_intermediate(const size_t items)
{
    printf("%s\n", "benchmark_edge_check (intermediate)");

    edge_check(items, 1000, 10000);

    printf("\n\n");
}

static void benchmark_edge_check_dense(const size_t items)
{
    printf("%s\n", "benchmark_edge_check (dense)");

    edge_check(items, 500, 200000);

    printf("\n\n");
}

static void vertex_remove(const size_t items_to_remove, const size_t vertex_factor, const size_t edge_factor)
{
    for (size_t i = 1; i <= 10; ++i) {

        graphd_t *graph_dense = create_random_graphd(vertex_factor * i, edge_factor * i);
        graphs_t *graph_sparse = create_random_graphs(vertex_factor * i, edge_factor * i);

        assert(graph_dense->vertices->len == vertex_factor * i);
        assert(graph_sparse->vertices->len == vertex_factor * i);

        printf("\nPreallocated vertices: %lu\n", vertex_factor * i);
        printf("Preallocated edges: %lu\n", edge_factor * i);

        clock_t start = clock();

        for (size_t j = 0; j < items_to_remove; ++j) {
            size_t index = rand() % graph_dense->vertices->len;
            assert(graphd_vertex_remove(graph_dense, index) == 0);
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> dense graph:  removing %lu vertices: %f s\n", items_to_remove, time_elapsed);

        start = clock();

        for (size_t j = 0; j < items_to_remove; ++j) {
            size_t index = rand() % graph_sparse->vertices->len;
            assert(graphs_vertex_remove(graph_sparse, index) == 0);
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> sparse graph: removing %lu vertices: %f s\n", items_to_remove, time_elapsed);

        graphd_destroy(graph_dense);
        graphs_destroy(graph_sparse);
    }
}

static void benchmark_vertex_remove_sparse(const size_t items)
{
    printf("%s\n", "benchmark_vertex_remove (sparse)");

    vertex_remove(items, 1000, 1000);

    printf("\n\n");
}

static void benchmark_vertex_remove_intermediate(const size_t items)
{
    printf("%s\n", "benchmark_vertex_remove (intermediate)");

    vertex_remove(items, 1000, 10000);

    printf("\n\n");
}

static void benchmark_vertex_remove_dense(const size_t items)
{
    printf("%s\n", "benchmark_vertex_remove (dense)");

    vertex_remove(items, 500, 200000);

    printf("\n\n");
}

static void edge_remove(const size_t items_to_remove, const size_t vertex_factor, const size_t edge_factor)
{
    for (size_t i = 1; i <= 10; ++i) {

        graphd_t *graph_dense = create_random_graphd(vertex_factor * i, edge_factor * i);
        graphs_t *graph_sparse = create_random_graphs(vertex_factor * i, edge_factor * i);

        assert(graph_dense->vertices->len == vertex_factor * i);
        assert(graph_sparse->vertices->len == vertex_factor * i);

        printf("\nPreallocated vertices: %lu\n", vertex_factor * i);
        printf("Preallocated edges: %lu\n", edge_factor * i);

        clock_t start = clock();

        for (size_t j = 0; j < items_to_remove; ++j) {
            int src = rand() % graph_dense->vertices->len;
            int tar = rand() % graph_dense->vertices->len;

            assert(graphd_edge_remove(graph_dense, src, tar) == 0);
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> dense graph:  removing %lu edges: %f s\n", items_to_remove, time_elapsed);

        start = clock();

        for (size_t j = 0; j < items_to_remove; ++j) {
            int src = rand() % graph_sparse->vertices->len;
            int tar = rand() % graph_sparse->vertices->len;
            assert(graphs_edge_remove(graph_sparse, src, tar) == 0);
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> sparse graph: removing %lu edges: %f s\n", items_to_remove, time_elapsed);

        graphd_destroy(graph_dense);
        graphs_destroy(graph_sparse);
    }
}

static void benchmark_edge_remove_sparse(const size_t items)
{
    printf("%s\n", "benchmark_edge_remove (sparse)");

    edge_remove(items, 1000, 1000);

    printf("\n\n");
}

static void benchmark_edge_remove_intermediate(const size_t items)
{
    printf("%s\n", "benchmark_edge_remove (intermediate)");

    edge_remove(items, 1000, 10000);

    printf("\n\n");
}

static void benchmark_edge_remove_dense(const size_t items)
{
    printf("%s\n", "benchmark_edge_remove (dense)");

    edge_remove(items, 500, 200000);

    printf("\n\n");
}

int main(void)
{
    srand(time(NULL));
    
    benchmark_vertex_add_sparse(1000);
    benchmark_vertex_add_intermediate(1000);
    benchmark_vertex_add_dense(1000);

    benchmark_edge_add_sparse(1000);
    benchmark_edge_add_intermediate(1000);
    benchmark_edge_add_dense(1000);

    benchmark_edge_check_sparse(1000);
    benchmark_edge_check_intermediate(1000);
    benchmark_edge_check_dense(1000);

    benchmark_vertex_remove_sparse(100);
    benchmark_vertex_remove_intermediate(100);
    benchmark_vertex_remove_dense(100);

    benchmark_edge_remove_sparse(1000);
    benchmark_edge_remove_intermediate(1000);
    benchmark_edge_remove_dense(1000);



    return 0;

}