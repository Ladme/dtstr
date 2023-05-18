// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <stdio.h>
#include "../src/graph.h"

#define UNUSED(x) (void)(x)

static void int_multiply(void *item, void *unused)
{
    UNUSED(unused);

    int *value = (int *) item;
    *value *= 2;
}

static void int_extract(void *item, void *wrapped_vector)
{
    vec_t *vector = (vec_t *) wrapped_vector;

    vec_push(vector, item, sizeof(int));
}

static int compare_pointers(const void *p1, const void *p2)
{
    return p1 == p2;
}

static int compare_pointer_with_ppointer(const void *p1, const void *p2)
{
    return *(void **) p1 == p2;
}

inline static int closef(const float a, const float b, const float limit)
{
    return (a > b - limit && a < b + limit);
}

/* ************************************************ */
/*                TESTS FOR GRAPHD_T                */
/* ************************************************ */


static graphd_t *create_graphd_representative(void)
{
    graphd_t *graph = graphd_new();

    for (int i = 0; i < 9; ++i) {
        graphd_vertex_add(graph, &i, sizeof(int));
    }

    graphd_edge_add(graph, 0, 1, 1.0);
    graphd_edge_add(graph, 1, 0, 1.0);
    graphd_edge_add(graph, 1, 2, 1.0);
    graphd_edge_add(graph, 2, 2, 1.0);
    graphd_edge_add(graph, 1, 4, 1.0);
    graphd_edge_add(graph, 5, 2, 1.0);
    graphd_edge_add(graph, 4, 3, 1.0);
    graphd_edge_add(graph, 4, 5, 1.0);
    graphd_edge_add(graph, 4, 7, 1.0);
    graphd_edge_add(graph, 7, 4, 1.0);
    graphd_edge_add(graph, 8, 5, 1.0);

    return graph;
}


static int test_graphd_destroy_nonexistent(void)
{
    printf("%-40s", "test_graphd_destroy (nonexistent) ");

    graphd_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_graphd_new(void)
{
    printf("%-40s", "test_graphd_new ");

    graphd_t *graph = graphd_new();

    assert(graph->vertices->len == 0);
    assert(graph->vertices->capacity == GRAPHD_DEFAULT_CAPACITY);
    assert(graph->amatrix);
    assert(graph->allocated = GRAPHD_DEFAULT_CAPACITY);
    assert(graph->base_capacity == GRAPHD_DEFAULT_CAPACITY);

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_with_capacity(void)
{
    printf("%-40s", "test_graphd_with_capacity ");

    graphd_t *graph = graphd_with_capacity(100);

    assert(graph->vertices->len == 0);
    assert(graph->vertices->capacity == 100);
    assert(graph->amatrix);
    assert(graph->allocated == 100);
    assert(graph->base_capacity == 100);
    graphd_destroy(graph);

    printf("OK\n");
    return 0;

}

static int test_graphd_vertex_add(void)
{
    printf("%-40s", "test_graphd_vertex_add ");

    int random = 765;
    assert(graphd_vertex_add(NULL, &random, sizeof(int)) == -99);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 1000; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) == i);
        assert(graph->vertices->len == (size_t) i + 1);
    }

    assert(graph->allocated == 1024);
    assert(graph->base_capacity == GRAPHD_DEFAULT_CAPACITY);

    for (int i = 0; i < 1024; ++i) {
        for (int j = 0; j < 1024; ++j) {
            assert(graph->amatrix[i][j].exists == 0);
            assert(graph->amatrix[i][j].weight == 0.0);
        }
    }

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_vertex_get(void)
{
    printf("%-40s", "test_graphd_vertex_get ");

    assert(graphd_vertex_get(NULL, 0) == NULL);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 65; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 64; i >= 0; --i) {
        assert(*(int *) graphd_vertex_get(graph, i) == i);
    }

    assert(graphd_vertex_get(graph, 65) == NULL);

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_edge_add(void)
{
    printf("%-40s", "test_graphd_edge_add ");

    assert(graphd_edge_add(NULL, 0, 2, 4) == 99);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphd_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, -2.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, 5.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0.0) == 0);

    // print edges
    /*for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 32; ++j) {
            printf("%d ", graph->amatrix[i][j].exists);
        }
        printf("\n");
    }

    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 32; ++j) {
            printf("%d ", graph->amatrix[i][j].weight);
        }
        printf("\n");
    }*/

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 1.0, 0.0001));
            else if (i >= 3 && j == 0) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 5.0, 0.0001));
            else if (i == 3 && j == 4) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 2.0, 0.0001));
            else if (i == 10 && j == 3) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 7.2, 0.0001));
            else if (i == 5 && j == 8) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, -3.5, 0.0001));
            else if (i == 1 && j == 16) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 6.0, 0.0001));
            else if (i == 7 && j == 7) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 0.0, 0.0001));
            else assert(!graph->amatrix[i][j].exists);
        }
    }

    assert(graphd_edge_add(graph, 0, 17, 1.0) == 1);
    assert(graphd_edge_add(graph, 25, 4, 5.0) == 1);
    assert(graphd_edge_add(graph, 17, 19, 0.0) == 1);
    assert(graphd_edge_add(graph, 23, 23, 2.0) == 1);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 1.0, 0.0001));
            else if (i >= 3 && j == 0) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 5.0, 0.0001));
            else if (i == 3 && j == 4) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 2.0, 0.0001));
            else if (i == 10 && j == 3) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 7.2, 0.0001));
            else if (i == 5 && j == 8) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, -3.5, 0.0001));
            else if (i == 1 && j == 16) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 6.0, 0.0001));
            else if (i == 7 && j == 7) assert(graph->amatrix[i][j].exists && closef(graph->amatrix[i][j].weight, 0.0, 0.0001));
            else assert(!graph->amatrix[i][j].exists);
        }
    }

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}


static int test_graphd_edge_exists(void)
{
    printf("%-40s", "test_graphd_edge_exists ");

    assert(graphd_edge_exists(NULL, 0, 2) == 0);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphd_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7.0) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3.0) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0.0) == 0);

   for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(graphd_edge_exists(graph, i, j));
            else if (i >= 3 && j == 0) assert(graphd_edge_exists(graph, i, j));
            else if (i == 3 && j == 4) assert(graphd_edge_exists(graph, i, j));
            else if (i == 10 && j == 3) assert(graphd_edge_exists(graph, i, j));
            else if (i == 5 && j == 8) assert(graphd_edge_exists(graph, i, j));
            else if (i == 1 && j == 16) assert(graphd_edge_exists(graph, i, j));
            else if (i == 7 && j == 7) assert(graphd_edge_exists(graph, i, j));
            else assert(!graphd_edge_exists(graph, i, j));
        }
    }

    assert(graphd_edge_exists(graph, 0, 17) == 0);
    assert(graphd_edge_exists(graph, 25, 4) == 0);
    assert(graphd_edge_exists(graph, 17, 19) == 0);
    assert(graphd_edge_exists(graph, 23, 23) == 0);

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_edge_weight(void)
{
    printf("%-40s", "test_graphd_edge_weight ");

    assert(isnan(graphd_edge_weight(NULL, 0, 2)));

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphd_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0.0) == 0);

   for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(closef(graphd_edge_weight(graph, i, j), 1.0, 0.0001));
            else if (i >= 3 && j == 0) assert(closef(graphd_edge_weight(graph, i, j), 5.0, 0.0001));
            else if (i == 3 && j == 4) assert(closef(graphd_edge_weight(graph, i, j), 2.0, 0.0001));
            else if (i == 10 && j == 3) assert(closef(graphd_edge_weight(graph, i, j), 7.2, 0.0001));
            else if (i == 5 && j == 8) assert(closef(graphd_edge_weight(graph, i, j), -3.5, 0.0001));
            else if (i == 1 && j == 16) assert(closef(graphd_edge_weight(graph, i, j), 6.0, 0.0001));
            else if (i == 7 && j == 7) assert(closef(graphd_edge_weight(graph, i, j), 0.0, 0.0001));
            else assert(isnan(graphd_edge_weight(graph, i, j)));
        }
    }

    assert(isnan(graphd_edge_weight(graph, 0, 17)));
    assert(isnan(graphd_edge_weight(graph, 25, 4)));
    assert(isnan(graphd_edge_weight(graph, 17, 19)));
    assert(isnan(graphd_edge_weight(graph, 23, 23)));

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}


static int test_graphd_edge_remove(void)
{
    printf("%-40s", "test_graphd_edge_remove ");

    assert(graphd_edge_remove(NULL, 0, 2) == 99);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    // connect all vertices
    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            assert(graphd_edge_add(graph, i, j, 1.0) == 0);
        }
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_remove(graph, 0, i) == 0);
        assert(graphd_edge_remove(graph, i, 0) == 0);
    }

    assert(graphd_edge_remove(graph, 3, 4) == 0);
    assert(graphd_edge_remove(graph, 10, 3) == 0);
    assert(graphd_edge_remove(graph, 5, 8) == 0);
    assert(graphd_edge_remove(graph, 1, 16) == 0);
    assert(graphd_edge_remove(graph, 7, 7) == 0);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(!graphd_edge_exists(graph, i, j));
            else if (i >= 3 && j == 0) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 3 && j == 4) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 10 && j == 3) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 5 && j == 8) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 1 && j == 16) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 7 && j == 7) assert(!graphd_edge_exists(graph, i, j));
            else assert(graphd_edge_exists(graph, i, j));
        }
    }

    assert(graphd_edge_remove(graph, 0, 17) == 1);
    assert(graphd_edge_remove(graph, 25, 4) == 1);
    assert(graphd_edge_remove(graph, 17, 19) == 1);
    assert(graphd_edge_remove(graph, 23, 23) == 1);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(!graphd_edge_exists(graph, i, j));
            else if (i >= 3 && j == 0) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 3 && j == 4) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 10 && j == 3) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 5 && j == 8) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 1 && j == 16) assert(!graphd_edge_exists(graph, i, j));
            else if (i == 7 && j == 7) assert(!graphd_edge_exists(graph, i, j));
            else assert(graphd_edge_exists(graph, i, j));
        }
    }

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_vertex_remove(void)
{
    printf("%-40s", "test_graphd_vertex_remove ");

    assert(graphd_vertex_remove(NULL, 0) == 99);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) == i);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphd_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0.0) == 0);

    // remove vertex with low index
    assert(graphd_vertex_remove(graph, 0) == 0);
    assert(graph->vertices->len == 16);

    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            if (i == 2 && j == 3) assert(graphd_edge_exists(graph, 2, 3));
            else if (i == 9 && j == 2) assert(graphd_edge_exists(graph, 9, 2));
            else if (i == 4 && j == 7) assert(graphd_edge_exists(graph, 4, 7));
            else if (i == 0 && j == 15) assert(graphd_edge_exists(graph, 0, 15));
            else if (i == 6 && j == 6) assert(graphd_edge_exists(graph, 6, 6));
            else assert(!graphd_edge_exists(graph, i, j));
        }
    }

    // remove vertex from the middle
    assert(graphd_vertex_remove(graph, 9) == 0);
    assert(graph->vertices->len == 15);

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (i == 2 && j == 3) assert(graphd_edge_exists(graph, 2, 3));
            else if (i == 4 && j == 7) assert(graphd_edge_exists(graph, 4, 7));
            else if (i == 0 && j == 14) assert(graphd_edge_exists(graph, 0, 14));
            else if (i == 6 && j == 6) assert(graphd_edge_exists(graph, 6, 6));
            else assert(!graphd_edge_exists(graph, i, j));
        }
    }

    // remove vertex from the end
    assert(graphd_vertex_remove(graph, 14) == 0);
    assert(graph->vertices->len == 14);

    for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (i == 2 && j == 3) assert(graphd_edge_exists(graph, 2, 3));
            else if (i == 4 && j == 7) assert(graphd_edge_exists(graph, 4, 7));
            else if (i == 6 && j == 6) assert(graphd_edge_exists(graph, 6, 6));
            else assert(!graphd_edge_exists(graph, i, j));
        }
    }

    // try removing non-existent vertex
    assert(graphd_vertex_remove(graph, 14) == 2);
    assert(graph->vertices->len == 14);

    for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (i == 2 && j == 3) assert(graphd_edge_exists(graph, 2, 3));
            else if (i == 4 && j == 7) assert(graphd_edge_exists(graph, 4, 7));
            else if (i == 6 && j == 6) assert(graphd_edge_exists(graph, 6, 6));
            else assert(!graphd_edge_exists(graph, i, j));
        }
    }

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_vertex_remove_large(void)
{
    printf("%-40s", "test_graphd_vertex_remove (large) ");

    graphd_t *graph = graphd_with_capacity(32);

    for (int i = 0; i < 550; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (size_t i = 0; i < 550; i += 2) {
        for (size_t j = 0; j < 550; ++j) {
            assert(graphd_edge_add(graph, i, j, 1.0) == 0);
        }
    }

    assert(graph->allocated == 1024);

    for (size_t i = 0; i < 550; i++) {
        assert(graphd_vertex_remove(graph, 0) == 0);
        for (size_t j = 0; j < 550 - i - 1; ++j) assert(graphd_edge_exists(graph, 0, j) == (int) (i % 2));
    }

    assert(graph->allocated == 32);
    assert(graph->vertices->capacity == 32);

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_vertex_successors(void)
{
    printf("%-40s", "test_graphd_vertex_successors ");

    assert(graphd_vertex_successors(NULL, 0) == NULL);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphd_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, -2.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, 5.0) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0.0) == 0);

    for (size_t i = 0; i < 17; ++i) {
        vec_t *successors = graphd_vertex_successors(graph, i);

        switch (i) {
        case 0: 
            assert(vec_len(successors) == 14); 
            for (size_t j = 0; j < 14; ++j) {
                assert(*(void **) vec_get(successors, j) == (void *) vec_get(graph->vertices, j + 3));
            }
            break;
        case 1: assert(vec_len(successors) == 1); break;
        case 2: assert(vec_len(successors) == 0); break;
        case 3: assert(vec_len(successors) == 2); break;
        case 4: assert(vec_len(successors) == 1); break;
        case 5: assert(vec_len(successors) == 2); break;
        case 6: assert(vec_len(successors) == 1); break;
        case 7: assert(vec_len(successors) == 2); break;
        case 8: 
        case 9: assert(vec_len(successors) == 1); break;
        case 10: assert(vec_len(successors) == 2); break;
        case 11: 
        case 12:
        case 13:
        case 14:
        case 15: 
        case 16: assert(vec_len(successors) == 1); break;
        }

        vec_destroy(successors);
    }

    assert(graphd_vertex_successors(graph, 17) == NULL);

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

/** @brief Tests that the successors vector is valid even after the graph is reallocated. */
static int test_graphd_vertex_successors_advanced(void)
{   
    printf("%-40s", "test_graphd_vertex_successors (advanced) ");

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 20; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 1; i < 20; ++i) {
        assert(graphd_edge_add(graph, 0, i, 1.0) == 0);
    }

    vec_t *successors = graphd_vertex_successors(graph, 0);

    assert(successors->len == 19);

    for (int i = 1; i < 20; ++i) {
        assert(vec_contains(successors, compare_pointer_with_ppointer, graph->vertices->items[i]));
    }

    // force graph reallocation    
    for (int i = 20; i < 1000; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 1; i < 20; ++i) {
        assert(vec_contains(successors, compare_pointer_with_ppointer, graph->vertices->items[i]));
    }

    for (int i = 20; i < 1000; ++i) {
        assert(!vec_contains(successors, compare_pointer_with_ppointer, graph->vertices->items[i]));
    }

    graphd_destroy(graph);
    vec_destroy(successors);

    printf("OK\n");
    return 0;
}


static int test_graphd_vertex_map(void)
{
    printf("%-40s", "test_graphd_vertex_map ");

    graphd_vertex_map(NULL, int_multiply, NULL);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 65; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    graphd_vertex_map(graph, int_multiply, NULL);

    for (int i = 64; i >= 0; --i) {
        assert(*(int *) graphd_vertex_get(graph, i) == i * 2);
    }

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_vertex_map_bfs(void)
{
    printf("%-40s", "test_graphd_vertex_map_bfs ");

    vec_t *extracted = vec_new();

    assert(graphd_vertex_map_bfs(NULL, 0, int_extract, extracted) == 0);

    graphd_t *graph = create_graphd_representative();

    // starting from vertex 1
    assert(graphd_vertex_map_bfs(graph, 1, int_extract, extracted) == 7);
    assert(extracted->len == 7);
   
    assert(*(int *) extracted->items[0] == 1);
    assert(*(int *) extracted->items[1] == 0 || *(int *) extracted->items[1] == 2 || *(int *) extracted->items[1] == 4);
    assert(*(int *) extracted->items[2] == 0 || *(int *) extracted->items[2] == 2 || *(int *) extracted->items[2] == 4);
    assert(*(int *) extracted->items[3] == 0 || *(int *) extracted->items[3] == 2 || *(int *) extracted->items[3] == 4);

    assert(*(int *) extracted->items[4] == 3 || *(int *) extracted->items[4] == 5 || *(int *) extracted->items[4] == 7);
    assert(*(int *) extracted->items[5] == 3 || *(int *) extracted->items[5] == 5 || *(int *) extracted->items[5] == 7);
    assert(*(int *) extracted->items[6] == 3 || *(int *) extracted->items[6] == 5 || *(int *) extracted->items[6] == 7);

    vec_clear(extracted);

    // starting from vertex 2
    assert(graphd_vertex_map_bfs(graph, 2, int_extract, extracted) == 1);
    assert(extracted->len == 1);

    assert(*(int *) extracted->items[0] == 2);

    vec_clear(extracted);

    // starting from vertex 6
    assert(graphd_vertex_map_bfs(graph, 6, int_extract, extracted) == 1);
    assert(extracted->len == 1);

    assert(*(int *) extracted->items[0] == 6);

    vec_clear(extracted);

    // starting from vertex 8
    assert(graphd_vertex_map_bfs(graph, 8, int_extract, extracted) == 3);
    assert(extracted->len == 3);

    assert(*(int *) extracted->items[0] == 8);
    assert(*(int *) extracted->items[1] == 5);
    assert(*(int *) extracted->items[2] == 2);

    vec_clear(extracted);

    // starting from vertex 9 (non-existent)
    assert(graphd_vertex_map_bfs(graph, 9, int_extract, extracted) == 0);
    assert(extracted->len == 0);

    vec_destroy(extracted);

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphd_vertex_map_dfs(void)
{
    printf("%-40s", "test_graphd_vertex_map_dfs ");

    vec_t *extracted = vec_new();

    assert(graphd_vertex_map_dfs(NULL, 0, int_extract, extracted) == 0);

    graphd_t *graph = create_graphd_representative();

    // starting from vertex 2
    assert(graphd_vertex_map_dfs(graph, 2, int_extract, extracted) == 1);
    assert(extracted->len == 1);

    assert(*(int *) extracted->items[0] == 2);

    vec_clear(extracted);

    // starting from vertex 6
    assert(graphd_vertex_map_dfs(graph, 6, int_extract, extracted) == 1);
    assert(extracted->len == 1);

    assert(*(int *) extracted->items[0] == 6);

    vec_clear(extracted);

    // starting from vertex 8
    assert(graphd_vertex_map_dfs(graph, 8, int_extract, extracted) == 3);
    assert(extracted->len == 3);

    assert(*(int *) extracted->items[0] == 8);
    assert(*(int *) extracted->items[1] == 5);
    assert(*(int *) extracted->items[2] == 2);

    vec_clear(extracted);

    // starting from vertex 8 with slightly modified graph
    graphd_edge_add(graph, 8, 7, 1);
    graphd_edge_remove(graph, 4, 5);
    assert(graphd_vertex_map_dfs(graph, 8, int_extract, extracted) == 6);
    assert(extracted->len == 6);

    assert(*(int *) extracted->items[0] == 8);
    assert(*(int *) extracted->items[1] == 7 || *(int *) extracted->items[1] == 5);
    if (*(int *) extracted->items[1] == 7) {
        assert(*(int *) extracted->items[2] == 4);
        assert(*(int *) extracted->items[3] == 3);
        assert(*(int *) extracted->items[4] == 5);
        assert(*(int *) extracted->items[5] == 2);
    } else {
        assert(*(int *) extracted->items[2] == 2);
        assert(*(int *) extracted->items[3] == 7);
        assert(*(int *) extracted->items[4] == 4);
        assert(*(int *) extracted->items[5] == 3);
    }

    vec_clear(extracted);

    // starting from vertex 9 (non-existent)
    assert(graphd_vertex_map_dfs(graph, 9, int_extract, extracted) == 0);
    assert(extracted->len == 0);

    vec_destroy(extracted);

    graphd_destroy(graph);

    printf("OK\n");
    return 0;
}

/* ************************************************ */
/*                TESTS FOR GRAPHS_T                */
/* ************************************************ */

static graphs_t *create_graphs_representative(void)
{
    graphs_t *graph = graphs_new();

    for (int i = 0; i < 9; ++i) {
        graphs_vertex_add(graph, &i, sizeof(int));
    }

    graphs_edge_add(graph, 0, 1, 1.0);
    graphs_edge_add(graph, 1, 0, 1.0);
    graphs_edge_add(graph, 1, 2, 1.0);
    graphs_edge_add(graph, 2, 2, 1.0);
    graphs_edge_add(graph, 1, 4, 1.0);
    graphs_edge_add(graph, 5, 2, 1.0);
    graphs_edge_add(graph, 4, 3, 1.0);
    graphs_edge_add(graph, 4, 5, 1.0);
    graphs_edge_add(graph, 4, 7, 1.0);
    graphs_edge_add(graph, 7, 4, 1.0);
    graphs_edge_add(graph, 8, 5, 1.0);

    return graph;
}

static graphs_t *create_graphs_representative_weighted(void)
{
    graphs_t *graph = graphs_new();

    for (int i = 0; i < 9; ++i) {
        graphs_vertex_add(graph, &i, sizeof(int));
    }

    graphs_edge_add(graph, 0, 1, 5.2);
    graphs_edge_add(graph, 1, 0, 1.8);
    graphs_edge_add(graph, 1, 2, 3.4);
    graphs_edge_add(graph, 2, 1, 4.1);
    graphs_edge_add(graph, 2, 2, 1.1);
    graphs_edge_add(graph, 1, 4, 2.4);
    graphs_edge_add(graph, 5, 2, 3.0);
    graphs_edge_add(graph, 2, 5, 2.0);
    graphs_edge_add(graph, 4, 3, 1.5);
    graphs_edge_add(graph, 4, 5, 6.2);
    graphs_edge_add(graph, 4, 7, 3.0);
    graphs_edge_add(graph, 7, 4, 4.9);
    graphs_edge_add(graph, 8, 5, 2.1);

    return graph;
}

static int test_graphs_destroy_nonexistent(void)
{
    printf("%-40s", "test_graphs_destroy (nonexistent) ");

    graphs_destroy(NULL);

    printf("OK\n");
    return 0;
}

static int test_graphs_new(void)
{
    printf("%-40s", "test_graphs_new ");

    graphs_t *graph = graphs_new();

    assert(graph->vertices->len == 0);
    assert(graph->vertices->capacity == GRAPHS_DEFAULT_CAPACITY);
    assert(graph->edges->len == 0);
    assert(graph->edges->capacity == GRAPHS_DEFAULT_CAPACITY);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_with_capacity(void)
{
    printf("%-40s", "test_graphs_with_capacity ");

    graphs_t *graph = graphs_with_capacity(100);

    assert(graph->vertices->len == 0);
    assert(graph->vertices->capacity == 100);
    assert(graph->edges->len == 0);
    assert(graph->edges->capacity == 100);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_vertex_add(void)
{
    printf("%-40s", "test_graphs_vertex_add ");

    int random = 765;
    assert(graphs_vertex_add(NULL, &random, sizeof(int)) == -99);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 1000; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) == i);
        assert(graph->vertices->len == (size_t) i + 1);
        assert(graph->edges->len == (size_t) i + 1);
    }

    assert(graph->vertices->capacity == 1024);
    assert(graph->edges->capacity == 1024);
    for (size_t i = 0; i < graph->edges->len; ++i) {
        assert((*(set_t **) vec_get(graph->edges, i))->len == 0);
        assert((*(set_t **) vec_get(graph->edges, i))->allocated == GRAPHS_DEFAULT_CAPACITY * 2);
    }

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_vertex_get(void)
{
    printf("%-40s", "test_graphs_vertex_get ");

    assert(graphs_vertex_get(NULL, 0) == NULL);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 65; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 64; i >= 0; --i) {
        assert(*(int *) graphs_vertex_get(graph, i) == i);
    }

    assert(graphs_vertex_get(graph, 65) == NULL);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_vertex_remove(void)
{
    printf("%-40s", "test_graphs_vertex_remove ");

    assert(graphs_vertex_remove(NULL, 0) == 99);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) == i);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphs_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphs_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, -2.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, 5.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0.0) == 0);

    // remove vertex with low index
    assert(graphs_vertex_remove(graph, 0) == 0);
    assert(graph->vertices->len == 16);

    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            if (i == 2 && j == 3) assert(graphs_edge_exists(graph, 2, 3));
            else if (i == 9 && j == 2) assert(graphs_edge_exists(graph, 9, 2));
            else if (i == 4 && j == 7) assert(graphs_edge_exists(graph, 4, 7));
            else if (i == 0 && j == 15) assert(graphs_edge_exists(graph, 0, 15));
            else if (i == 6 && j == 6) assert(graphs_edge_exists(graph, 6, 6));
            else assert(!graphs_edge_exists(graph, i, j));
        }
    }

    // remove vertex from the middle
    assert(graphs_vertex_remove(graph, 9) == 0);
    assert(graph->vertices->len == 15);

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (i == 2 && j == 3) assert(graphs_edge_exists(graph, 2, 3));
            else if (i == 4 && j == 7) assert(graphs_edge_exists(graph, 4, 7));
            else if (i == 0 && j == 14) assert(graphs_edge_exists(graph, 0, 14));
            else if (i == 6 && j == 6) assert(graphs_edge_exists(graph, 6, 6));
            else assert(!graphs_edge_exists(graph, i, j));
        }
    }

    // remove vertex from the end
    assert(graphs_vertex_remove(graph, 14) == 0);
    assert(graph->vertices->len == 14);

    for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (i == 2 && j == 3) assert(graphs_edge_exists(graph, 2, 3));
            else if (i == 4 && j == 7) assert(graphs_edge_exists(graph, 4, 7));
            else if (i == 6 && j == 6) assert(graphs_edge_exists(graph, 6, 6));
            else assert(!graphs_edge_exists(graph, i, j));
        }
    }

    // try removing non-existent vertex
    assert(graphs_vertex_remove(graph, 14) == 2);
    assert(graph->vertices->len == 14);

    for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (i == 2 && j == 3) assert(graphs_edge_exists(graph, 2, 3));
            else if (i == 4 && j == 7) assert(graphs_edge_exists(graph, 4, 7));
            else if (i == 6 && j == 6) assert(graphs_edge_exists(graph, 6, 6));
            else assert(!graphs_edge_exists(graph, i, j));
        }
    }

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_vertex_remove_large(void)
{
    printf("%-40s", "test_graphs_vertex_remove (large) ");

    graphs_t *graph = graphs_with_capacity(32);

    for (int i = 0; i < 550; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (size_t i = 0; i < 550; i += 2) {
        for (size_t j = 0; j < 550; ++j) {
            assert(graphs_edge_add(graph, i, j, 1) == 0);
        }
    }

    assert(graph->vertices->capacity == 1024);
    assert(graph->edges->capacity == 1024);

    for (size_t i = 0; i < 550; i++) {
        assert(graphs_vertex_remove(graph, 0) == 0);
        for (size_t j = 0; j < 550 - i - 1; ++j) assert(graphs_edge_exists(graph, 0, j) == (int) (i % 2));
    }

    assert(graph->vertices->capacity == 32);
    assert(graph->edges->capacity == 32);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_vertex_successors(void)
{
    printf("%-40s", "test_graphs_vertex_successors ");

    assert(graphs_vertex_successors(NULL, 0) == NULL);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphs_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphs_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, -2.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, 5.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0.0) == 0);

    for (size_t i = 0; i < 17; ++i) {
        vec_t *successors = graphs_vertex_successors(graph, i);

        switch (i) {
        case 0: 
            assert(vec_len(successors) == 14); 
            for (size_t j = 0; j < 14; ++j) {
                assert(vec_find(graph->vertices, compare_pointers, *(void **) vec_get(successors, j)));
            }
            break;
        case 1: assert(vec_len(successors) == 1); break;
        case 2: assert(vec_len(successors) == 0); break;
        case 3: assert(vec_len(successors) == 2); break;
        case 4: assert(vec_len(successors) == 1); break;
        case 5: assert(vec_len(successors) == 2); break;
        case 6: assert(vec_len(successors) == 1); break;
        case 7: assert(vec_len(successors) == 2); break;
        case 8: 
        case 9: assert(vec_len(successors) == 1); break;
        case 10: assert(vec_len(successors) == 2); break;
        case 11: 
        case 12:
        case 13:
        case 14:
        case 15: 
        case 16: assert(vec_len(successors) == 1); break;
        }

        vec_destroy(successors);
    }

    assert(graphs_vertex_successors(graph, 17) == NULL);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

/** @brief Tests that the successors vector is valid even after the graph is reallocated. */
static int test_graphs_vertex_successors_advanced(void)
{   
    printf("%-40s", "test_graphs_vertex_successors (advanced) ");

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 20; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 1; i < 20; ++i) {
        assert(graphs_edge_add(graph, 0, i, 1.0) == 0);
    }

    vec_t *successors = graphs_vertex_successors(graph, 0);

    assert(successors->len == 19);

    for (int i = 1; i < 20; ++i) {
        assert(vec_contains(successors, compare_pointer_with_ppointer, graph->vertices->items[i]));
    }

    // force graph reallocation    
    for (int i = 20; i < 1000; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 1; i < 20; ++i) {
        assert(vec_contains(successors, compare_pointer_with_ppointer, graph->vertices->items[i]));
    }

    for (int i = 20; i < 1000; ++i) {
        assert(!vec_contains(successors, compare_pointer_with_ppointer, graph->vertices->items[i]));
    }

    graphs_destroy(graph);
    vec_destroy(successors);

    printf("OK\n");
    return 0;
}

inline static edges_t *get_edge(const set_t *adl, void *tar)
{
    edges_t target = { .vertex_tar = tar , .weight = 0.0 };
    return (edges_t *) set_get(adl, &target, sizeof(void *));
}

static int test_graphs_edge_add(void)
{
    printf("%-40s", "test_graphs_edge_add ");

    assert(graphs_edge_add(NULL, 0, 2, 4) == 99);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphs_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphs_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0.0) == 0);

    /*for (size_t i = 0; i < 17; ++i) {
        set_t *adl = *(set_t **) graph->edges->items[i];
        printf("%p [%lu]: ", graph->vertices->items[i], i);
        for (size_t j = 0; j < adl->allocated; ++j) {
            if (adl->items[j] == NULL) continue;
            
            dnode_t *node = adl->items[j]->head;
            while (node != NULL) {
                printf("%p-%d ", ((edges_t *) (*(set_entry_t **) node->data)->item)->vertex_tar, ((edges_t *) (*(set_entry_t **) node->data)->item)->weight);
                node = node->next;
            }

            
        }
        printf("\n");
    }*/

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 1.0, 0.0001));
            else if (i >= 3 && j == 0) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 5.0, 0.0001));
            else if (i == 3 && j == 4) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 2.0, 0.0001));
            else if (i == 10 && j == 3) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 7.2, 0.0001));
            else if (i == 5 && j == 8) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, -3.5, 0.0001));
            else if (i == 1 && j == 16) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 6.0, 0.0001));
            else if (i == 7 && j == 7) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 0.0, 0.0001));
            else assert(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j]) == NULL);
        }
    }

    assert(graphs_edge_add(graph, 0, 17, 1.0) == 1);
    assert(graphs_edge_add(graph, 25, 4, 5.5) == 1);
    assert(graphs_edge_add(graph, 17, 19, 0.25) == 1);
    assert(graphs_edge_add(graph, 23, 23, 2.3) == 1);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 1.0, 0.0001));
            else if (i >= 3 && j == 0) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 5.0, 0.0001));
            else if (i == 3 && j == 4) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 2.0, 0.0001));
            else if (i == 10 && j == 3) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 7.2, 0.0001));
            else if (i == 5 && j == 8) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, -3.5, 0.0001));
            else if (i == 1 && j == 16) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 6.0, 0.0001));
            else if (i == 7 && j == 7) assert(closef(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j])->weight, 0.0, 0.0001));
            else assert(get_edge(*(set_t **) graph->edges->items[i], graph->vertices->items[j]) == NULL);
        }
    }

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_edge_exists(void)
{
    printf("%-40s", "test_graphs_edge_exists ");

    assert(graphs_edge_exists(NULL, 0, 2) == 0);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphs_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphs_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0.0) == 0);

   for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(graphs_edge_exists(graph, i, j));
            else if (i >= 3 && j == 0) assert(graphs_edge_exists(graph, i, j));
            else if (i == 3 && j == 4) assert(graphs_edge_exists(graph, i, j));
            else if (i == 10 && j == 3) assert(graphs_edge_exists(graph, i, j));
            else if (i == 5 && j == 8) assert(graphs_edge_exists(graph, i, j));
            else if (i == 1 && j == 16) assert(graphs_edge_exists(graph, i, j));
            else if (i == 7 && j == 7) assert(graphs_edge_exists(graph, i, j));
            else assert(!graphs_edge_exists(graph, i, j));
        }
    }

    assert(graphs_edge_exists(graph, 0, 17) == 0);
    assert(graphs_edge_exists(graph, 25, 4) == 0);
    assert(graphs_edge_exists(graph, 17, 19) == 0);
    assert(graphs_edge_exists(graph, 23, 23) == 0);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}



static int test_graphs_edge_weight(void)
{
    printf("%-40s", "test_graphs_edge_weight ");

    assert(isnan(graphs_edge_weight(NULL, 0, 2)));

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphs_edge_add(graph, 0, i, 1.0) == 0);
        assert(graphs_edge_add(graph, i, 0, 5.0) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2.0) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7.2) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3.5) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6.0) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0.0) == 0);

   for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(closef(graphs_edge_weight(graph, i, j), 1.0, 0.0001));
            else if (i >= 3 && j == 0) assert(closef(graphs_edge_weight(graph, i, j), 5.0, 0.0001));
            else if (i == 3 && j == 4) assert(closef(graphs_edge_weight(graph, i, j), 2.0, 0.0001));
            else if (i == 10 && j == 3) assert(closef(graphs_edge_weight(graph, i, j), 7.2, 0.0001));
            else if (i == 5 && j == 8) assert(closef(graphs_edge_weight(graph, i, j), -3.5, 0.0001));
            else if (i == 1 && j == 16) assert(closef(graphs_edge_weight(graph, i, j), 6.0, 0.0001));
            else if (i == 7 && j == 7) assert(closef(graphs_edge_weight(graph, i, j), 0.0, 0.0001));
            else assert(isnan(graphs_edge_weight(graph, i, j)));
        }
    }

    assert(isnan(graphs_edge_weight(graph, 0, 17)));
    assert(isnan(graphs_edge_weight(graph, 25, 4)));
    assert(isnan(graphs_edge_weight(graph, 17, 19)));
    assert(isnan(graphs_edge_weight(graph, 23, 23)));

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_edge_remove(void)
{
    printf("%-40s", "test_graphs_edge_remove ");

    assert(graphs_edge_remove(NULL, 0, 2) == 99);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    // connect all vertices
    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            assert(graphs_edge_add(graph, i, j, 1.0) == 0);
        }
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphs_edge_remove(graph, 0, i) == 0);
        assert(graphs_edge_remove(graph, i, 0) == 0);
    }

    assert(graphs_edge_remove(graph, 3, 4) == 0);
    assert(graphs_edge_remove(graph, 10, 3) == 0);
    assert(graphs_edge_remove(graph, 5, 8) == 0);
    assert(graphs_edge_remove(graph, 1, 16) == 0);
    assert(graphs_edge_remove(graph, 7, 7) == 0);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(!graphs_edge_exists(graph, i, j));
            else if (i >= 3 && j == 0) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 3 && j == 4) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 10 && j == 3) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 5 && j == 8) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 1 && j == 16) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 7 && j == 7) assert(!graphs_edge_exists(graph, i, j));
            else assert(graphs_edge_exists(graph, i, j));
        }
    }

    assert(graphs_edge_remove(graph, 0, 17) == 1);
    assert(graphs_edge_remove(graph, 25, 4) == 1);
    assert(graphs_edge_remove(graph, 17, 19) == 1);
    assert(graphs_edge_remove(graph, 23, 23) == 1);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(!graphs_edge_exists(graph, i, j));
            else if (i >= 3 && j == 0) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 3 && j == 4) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 10 && j == 3) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 5 && j == 8) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 1 && j == 16) assert(!graphs_edge_exists(graph, i, j));
            else if (i == 7 && j == 7) assert(!graphs_edge_exists(graph, i, j));
            else assert(graphs_edge_exists(graph, i, j));
        }
    }

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_vertex_map(void)
{
    printf("%-40s", "test_graphs_vertex_map ");

    graphs_vertex_map(NULL, int_multiply, NULL);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 65; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    graphs_vertex_map(graph, int_multiply, NULL);

    for (int i = 64; i >= 0; --i) {
        assert(*(int *) graphs_vertex_get(graph, i) == i * 2);
    }

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_vertex_map_bfs(void)
{
    printf("%-40s", "test_graphs_vertex_map_bfs ");

    vec_t *extracted = vec_new();

    assert(graphs_vertex_map_bfs(NULL, 0, int_extract, extracted) == 0);

    graphs_t *graph = create_graphs_representative();

    // starting from vertex 1
    assert(graphs_vertex_map_bfs(graph, 1, int_extract, extracted) == 7);
    assert(extracted->len == 7);
   
    assert(*(int *) extracted->items[0] == 1);
    assert(*(int *) extracted->items[1] == 0 || *(int *) extracted->items[1] == 2 || *(int *) extracted->items[1] == 4);
    assert(*(int *) extracted->items[2] == 0 || *(int *) extracted->items[2] == 2 || *(int *) extracted->items[2] == 4);
    assert(*(int *) extracted->items[3] == 0 || *(int *) extracted->items[3] == 2 || *(int *) extracted->items[3] == 4);

    assert(*(int *) extracted->items[4] == 3 || *(int *) extracted->items[4] == 5 || *(int *) extracted->items[4] == 7);
    assert(*(int *) extracted->items[5] == 3 || *(int *) extracted->items[5] == 5 || *(int *) extracted->items[5] == 7);
    assert(*(int *) extracted->items[6] == 3 || *(int *) extracted->items[6] == 5 || *(int *) extracted->items[6] == 7);

    vec_clear(extracted);

    // starting from vertex 2
    assert(graphs_vertex_map_bfs(graph, 2, int_extract, extracted) == 1);
    assert(extracted->len == 1);

    assert(*(int *) extracted->items[0] == 2);

    vec_clear(extracted);

    // starting from vertex 6
    assert(graphs_vertex_map_bfs(graph, 6, int_extract, extracted) == 1);
    assert(extracted->len == 1);

    assert(*(int *) extracted->items[0] == 6);

    vec_clear(extracted);

    // starting from vertex 8
    assert(graphs_vertex_map_bfs(graph, 8, int_extract, extracted) == 3);
    assert(extracted->len == 3);

    assert(*(int *) extracted->items[0] == 8);
    assert(*(int *) extracted->items[1] == 5);
    assert(*(int *) extracted->items[2] == 2);

    vec_clear(extracted);

    // starting from vertex 9 (non-existent)
    assert(graphs_vertex_map_bfs(graph, 9, int_extract, extracted) == 0);
    assert(extracted->len == 0);

    vec_destroy(extracted);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_vertex_map_dfs(void)
{
    printf("%-40s", "test_graphs_vertex_map_dfs ");

    vec_t *extracted = vec_new();

    assert(graphs_vertex_map_dfs(NULL, 0, int_extract, extracted) == 0);

    graphs_t *graph = create_graphs_representative();

    // starting from vertex 2
    assert(graphs_vertex_map_dfs(graph, 2, int_extract, extracted) == 1);
    assert(extracted->len == 1);

    assert(*(int *) extracted->items[0] == 2);

    vec_clear(extracted);

    // starting from vertex 6
    assert(graphs_vertex_map_dfs(graph, 6, int_extract, extracted) == 1);
    assert(extracted->len == 1);

    assert(*(int *) extracted->items[0] == 6);

    vec_clear(extracted);

    // starting from vertex 8
    assert(graphs_vertex_map_dfs(graph, 8, int_extract, extracted) == 3);
    assert(extracted->len == 3);

    assert(*(int *) extracted->items[0] == 8);
    assert(*(int *) extracted->items[1] == 5);
    assert(*(int *) extracted->items[2] == 2);

    vec_clear(extracted);

    // starting from vertex 8 with slightly modified graph
    graphs_edge_add(graph, 8, 7, 1);
    graphs_edge_remove(graph, 4, 5);
    assert(graphs_vertex_map_dfs(graph, 8, int_extract, extracted) == 6);
    assert(extracted->len == 6);

    assert(*(int *) extracted->items[0] == 8);
    assert(*(int *) extracted->items[1] == 7 || *(int *) extracted->items[1] == 5);
    if (*(int *) extracted->items[1] == 7) {
        assert(*(int *) extracted->items[2] == 4);
        assert(*(int *) extracted->items[3] == 3);
        assert(*(int *) extracted->items[4] == 5);
        assert(*(int *) extracted->items[5] == 2);
    } else {
        assert(*(int *) extracted->items[2] == 2);
        assert(*(int *) extracted->items[3] == 7);
        assert(*(int *) extracted->items[4] == 4);
        assert(*(int *) extracted->items[5] == 3);
    }

    vec_clear(extracted);

    // starting from vertex 9 (non-existent)
    assert(graphs_vertex_map_dfs(graph, 9, int_extract, extracted) == 0);
    assert(extracted->len == 0);

    vec_destroy(extracted);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

static int test_graphs_bellman_ford(void)
{
    printf("%-40s", "test_graphs_bellman_ford ");

    vec_t *path = NULL;

    // vector is NULL
    assert(isnan(graphs_bellman_ford(NULL, 0, 1, &path)));

    graphs_t *graph = create_graphs_representative_weighted();

    // index is out of range
    assert(isnan(graphs_bellman_ford(graph, 0, 10, &path)));
    assert(isnan(graphs_bellman_ford(graph, 12, 1, &path)));

    // one pathway
    float distance = graphs_bellman_ford(graph, 0, 7, &path);

    //printf("%f\n", distance);
    assert(closef(distance, 10.6, 0.0001));
    assert(path->len == 4);
    assert(*((void **) path->items[0]) == graph->vertices->items[0]); 
    assert(*((void **) path->items[1]) == graph->vertices->items[1]);
    assert(*((void **) path->items[2]) == graph->vertices->items[4]);
    assert(*((void **) path->items[3]) == graph->vertices->items[7]);

    vec_destroy(path);

    // two pathways
    distance = graphs_bellman_ford(graph, 1, 5, &path);
    //printf("%f\n", distance);
    assert(closef(distance, 5.4, 0.0001));
    assert(path->len == 3);
    assert(*((void **) path->items[0]) == graph->vertices->items[1]); 
    assert(*((void **) path->items[1]) == graph->vertices->items[2]);
    assert(*((void **) path->items[2]) == graph->vertices->items[5]);

    vec_destroy(path);

    // no pathway (long)
    distance = graphs_bellman_ford(graph, 0, 8, &path);
    //printf("%f\n", distance);
    assert(isinf(distance));
    assert(path == NULL);

    // no pathway (simple)
    distance = graphs_bellman_ford(graph, 6, 3, &path);
    //printf("%f\n", distance);
    assert(isinf(distance));
    assert(path == NULL);

    // source == target
    distance = graphs_bellman_ford(graph, 2, 2, &path);
    //printf("%f\n", distance);
    assert(distance == 0.0);
    assert(path->len == 1);
    assert(*((void **) path->items[0]) == graph->vertices->items[2]);

    vec_destroy(path); 

    // 7 to 0
    distance = graphs_bellman_ford(graph, 7, 0, &path);
    //printf("%f\n", distance);
    assert(closef(distance, 20.0, 0.0001));
    assert(path->len == 6);
    assert(*((void **) path->items[0]) == graph->vertices->items[7]); 
    assert(*((void **) path->items[1]) == graph->vertices->items[4]);
    assert(*((void **) path->items[2]) == graph->vertices->items[5]);
    assert(*((void **) path->items[3]) == graph->vertices->items[2]);
    assert(*((void **) path->items[4]) == graph->vertices->items[1]);
    assert(*((void **) path->items[5]) == graph->vertices->items[0]);

    vec_destroy(path);


    // introduce a negative loop
    graphs_edge_add(graph, 3, 0, -92);
    
    distance = graphs_bellman_ford(graph, 0, 7, &path);
    //printf("%f\n", distance);
    assert(isnan(distance));
    assert(path == NULL);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}


int main(void) 
{
    test_graphd_destroy_nonexistent();
    test_graphd_new();
    test_graphd_with_capacity();

    test_graphd_vertex_add();
    test_graphd_vertex_get();

    test_graphd_edge_add();
    test_graphd_edge_exists();
    test_graphd_edge_weight();
    test_graphd_edge_remove();

    test_graphd_vertex_remove();
    test_graphd_vertex_remove_large();

    test_graphd_vertex_successors();
    test_graphd_vertex_successors_advanced();

    test_graphd_vertex_map();
    test_graphd_vertex_map_bfs();
    test_graphd_vertex_map_dfs();

    printf("*****\n");

    test_graphs_destroy_nonexistent();
    test_graphs_new();
    test_graphs_with_capacity();

    test_graphs_vertex_add();
    test_graphs_vertex_get();

    test_graphs_edge_add();
    test_graphs_edge_exists();
    test_graphs_edge_weight();
    test_graphs_edge_remove();

    test_graphs_vertex_remove();
    test_graphs_vertex_remove_large();

    test_graphs_vertex_successors();
    test_graphs_vertex_successors_advanced();

    test_graphs_vertex_map();
    test_graphs_vertex_map_bfs();
    test_graphs_vertex_map_dfs();

    test_graphs_bellman_ford();


    return 0;
}