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

static int edges_equal(const void *item1, const void *item2)
{
    edges_t *edge1 = (edges_t *) item1;
    edges_t *edge2 = (edges_t *) item2;

    return edge1->vertex_tar == edge2->vertex_tar;
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

    graphd_edge_add(graph, 0, 1, 1);
    graphd_edge_add(graph, 1, 0, 1);
    graphd_edge_add(graph, 1, 2, 1);
    graphd_edge_add(graph, 2, 2, 1);
    graphd_edge_add(graph, 1, 4, 1);
    graphd_edge_add(graph, 5, 2, 1);
    graphd_edge_add(graph, 4, 3, 1);
    graphd_edge_add(graph, 4, 5, 1);
    graphd_edge_add(graph, 4, 7, 1);
    graphd_edge_add(graph, 7, 4, 1);
    graphd_edge_add(graph, 8, 5, 1);

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
            assert(graph->amatrix[i][j].weight == 0);
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
        assert(graphd_edge_add(graph, 0, i, 1) == 0);
        assert(graphd_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0) == 0);

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
            if (i == 0 && j >= 3) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 1);
            else if (i >= 3 && j == 0) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 5);
            else if (i == 3 && j == 4) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 2);
            else if (i == 10 && j == 3) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 7);
            else if (i == 5 && j == 8) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == -3);
            else if (i == 1 && j == 16) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 6);
            else if (i == 7 && j == 7) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 0);
            else assert(!graph->amatrix[i][j].exists);
        }
    }

    assert(graphd_edge_add(graph, 0, 17, 1) == 1);
    assert(graphd_edge_add(graph, 25, 4, 5) == 1);
    assert(graphd_edge_add(graph, 17, 19, 0) == 1);
    assert(graphd_edge_add(graph, 23, 23, 2) == 1);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 1);
            else if (i >= 3 && j == 0) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 5);
            else if (i == 3 && j == 4) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 2);
            else if (i == 10 && j == 3) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 7);
            else if (i == 5 && j == 8) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == -3);
            else if (i == 1 && j == 16) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 6);
            else if (i == 7 && j == 7) assert(graph->amatrix[i][j].exists && graph->amatrix[i][j].weight == 0);
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
        assert(graphd_edge_add(graph, 0, i, 1) == 0);
        assert(graphd_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0) == 0);

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

    assert(graphd_edge_weight(NULL, 0, 2) == INT_MIN);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_add(graph, 0, i, 1) == 0);
        assert(graphd_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0) == 0);

   for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(graphd_edge_weight(graph, i, j) == 1);
            else if (i >= 3 && j == 0) assert(graphd_edge_weight(graph, i, j) == 5);
            else if (i == 3 && j == 4) assert(graphd_edge_weight(graph, i, j) == 2);
            else if (i == 10 && j == 3) assert(graphd_edge_weight(graph, i, j) == 7);
            else if (i == 5 && j == 8) assert(graphd_edge_weight(graph, i, j) == -3);
            else if (i == 1 && j == 16) assert(graphd_edge_weight(graph, i, j) == 6);
            else if (i == 7 && j == 7) assert(graphd_edge_weight(graph, i, j) == 0);
            else assert(graphd_edge_weight(graph, i, j) == INT_MIN);
        }
    }

    assert(graphd_edge_weight(graph, 0, 17) == INT_MIN);
    assert(graphd_edge_weight(graph, 25, 4) == INT_MIN);
    assert(graphd_edge_weight(graph, 17, 19) == INT_MIN);
    assert(graphd_edge_weight(graph, 23, 23) == INT_MIN);

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
            assert(graphd_edge_add(graph, i, j, 1) == 0);
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
        assert(graphd_edge_add(graph, 0, i, 1) == 0);
        assert(graphd_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0) == 0);

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
            assert(graphd_edge_add(graph, i, j, 1) == 0);
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
        assert(graphd_edge_add(graph, 0, i, 1) == 0);
        assert(graphd_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4, 2) == 0);
    assert(graphd_edge_add(graph, 10, 3, 7) == 0);
    assert(graphd_edge_add(graph, 5, 8, -3) == 0);
    assert(graphd_edge_add(graph, 1, 16, 6) == 0);
    assert(graphd_edge_add(graph, 7, 7, 0) == 0);

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

    graphs_edge_add(graph, 0, 1, 1);
    graphs_edge_add(graph, 1, 0, 1);
    graphs_edge_add(graph, 1, 2, 1);
    graphs_edge_add(graph, 2, 2, 1);
    graphs_edge_add(graph, 1, 4, 1);
    graphs_edge_add(graph, 5, 2, 1);
    graphs_edge_add(graph, 4, 3, 1);
    graphs_edge_add(graph, 4, 5, 1);
    graphs_edge_add(graph, 4, 7, 1);
    graphs_edge_add(graph, 7, 4, 1);
    graphs_edge_add(graph, 8, 5, 1);

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
        assert((*(vec_t **) vec_get(graph->edges, i))->len == 0);
        assert((*(vec_t **) vec_get(graph->edges, i))->capacity == GRAPHS_DEFAULT_CAPACITY);
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
        assert(graphs_edge_add(graph, 0, i, 1) == 0);
        assert(graphs_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0) == 0);

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
        assert(graphs_edge_add(graph, 0, i, 1) == 0);
        assert(graphs_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0) == 0);

    for (size_t i = 0; i < 17; ++i) {
        vec_t *successors = graphs_vertex_successors(graph, i);

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

    assert(graphs_vertex_successors(graph, 17) == NULL);

    graphs_destroy(graph);

    printf("OK\n");
    return 0;
}

inline static edges_t *get_edge(const vec_t *adl, void *tar)
{
    edges_t target = { .weight = 0, .vertex_tar = tar };
    edges_t *edge = (edges_t *) vec_find(adl, edges_equal, &target);
    return edge;
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
        assert(graphs_edge_add(graph, 0, i, 1) == 0);
        assert(graphs_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0) == 0);

    /*for (size_t i = 0; i < graph->edges->len; ++i) {
        vec_t *adjacency_list = *(vec_t **) vec_get(graph->edges, i);
        printf("%lu: ", i);
        for (size_t j = 0; j < adjacency_list->len; ++j) {
            printf("%d ", *(int *) vec_get(adjacency_list, j));
        }
        printf("\n");
    }*/

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 1);
            else if (i >= 3 && j == 0) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 5);
            else if (i == 3 && j == 4) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 2);
            else if (i == 10 && j == 3) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 7);
            else if (i == 5 && j == 8) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == -3);
            else if (i == 1 && j == 16) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 6);
            else if (i == 7 && j == 7) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 0);
            else assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j]) == NULL);
        }
    }

    assert(graphs_edge_add(graph, 0, 17, 1) == 1);
    assert(graphs_edge_add(graph, 25, 4, 5) == 1);
    assert(graphs_edge_add(graph, 17, 19, 0) == 1);
    assert(graphs_edge_add(graph, 23, 23, 2) == 1);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 1);
            else if (i >= 3 && j == 0) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 5);
            else if (i == 3 && j == 4) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 2);
            else if (i == 10 && j == 3) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 7);
            else if (i == 5 && j == 8) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == -3);
            else if (i == 1 && j == 16) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 6);
            else if (i == 7 && j == 7) assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j])->weight == 0);
            else assert(get_edge(*(vec_t **) graph->edges->items[i], graph->vertices->items[j]) == NULL);
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
        assert(graphs_edge_add(graph, 0, i, 1) == 0);
        assert(graphs_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0) == 0);

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

    assert(graphs_edge_weight(NULL, 0, 2) == INT_MIN);

    graphs_t *graph = graphs_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphs_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphs_edge_add(graph, 0, i, 1) == 0);
        assert(graphs_edge_add(graph, i, 0, 5) == 0);
    }

    assert(graphs_edge_add(graph, 3, 4, 2) == 0);
    assert(graphs_edge_add(graph, 10, 3, 7) == 0);
    assert(graphs_edge_add(graph, 5, 8, -3) == 0);
    assert(graphs_edge_add(graph, 1, 16, 6) == 0);
    assert(graphs_edge_add(graph, 7, 7, 0) == 0);

   for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(graphs_edge_weight(graph, i, j) == 1);
            else if (i >= 3 && j == 0) assert(graphs_edge_weight(graph, i, j) == 5);
            else if (i == 3 && j == 4) assert(graphs_edge_weight(graph, i, j) == 2);
            else if (i == 10 && j == 3) assert(graphs_edge_weight(graph, i, j) == 7);
            else if (i == 5 && j == 8) assert(graphs_edge_weight(graph, i, j) == -3);
            else if (i == 1 && j == 16) assert(graphs_edge_weight(graph, i, j) == 6);
            else if (i == 7 && j == 7) assert(graphs_edge_weight(graph, i, j) == 0);
            else assert(graphs_edge_weight(graph, i, j) == INT_MIN);
        }
    }

    assert(graphs_edge_weight(graph, 0, 17) == INT_MIN);
    assert(graphs_edge_weight(graph, 25, 4) == INT_MIN);
    assert(graphs_edge_weight(graph, 17, 19) == INT_MIN);
    assert(graphs_edge_weight(graph, 23, 23) == INT_MIN);

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
            assert(graphs_edge_add(graph, i, j, 1) == 0);
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

    test_graphs_vertex_map();
    test_graphs_vertex_map_bfs();
    test_graphs_vertex_map_dfs();


    return 0;
}