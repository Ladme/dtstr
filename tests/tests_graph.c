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

static graphd_t *create_graphd_representative(void)
{
    graphd_t *graph = graphd_new();

    for (int i = 0; i < 9; ++i) {
        graphd_vertex_add(graph, &i, sizeof(int));
    }

    graphd_edge_add(graph, 0, 1);
    graphd_edge_add(graph, 1, 0);
    graphd_edge_add(graph, 1, 2);
    graphd_edge_add(graph, 2, 2);
    graphd_edge_add(graph, 1, 4);
    graphd_edge_add(graph, 5, 2);
    graphd_edge_add(graph, 4, 3);
    graphd_edge_add(graph, 4, 5);
    graphd_edge_add(graph, 4, 7);
    graphd_edge_add(graph, 7, 4);
    graphd_edge_add(graph, 8, 5);

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

    graphd_t *graph = graphd_with_capacity(64);

    assert(graph->vertices->len == 0);
    assert(graph->vertices->capacity == 64);
    assert(graph->amatrix);
    assert(graph->allocated == 64);
    assert(graph->base_capacity == 64);
    graphd_destroy(graph);

    graph = graphd_with_capacity(9);
    
    assert(graph->vertices->len == 0);
    assert(graph->vertices->capacity == 16);
    assert(graph->amatrix);
    assert(graph->allocated == 16);
    assert(graph->base_capacity == 16);
    graphd_destroy(graph);

    graph = graphd_with_capacity(714);
    
    assert(graph->vertices->len == 0);
    assert(graph->vertices->capacity == 720);
    assert(graph->amatrix);
    assert(graph->allocated == 720);
    assert(graph->base_capacity == 720);
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
        for (int j = 0; j < 1024 / 8; ++j) {
            assert(graph->amatrix[i][j] == 0);
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

    assert(graphd_edge_add(NULL, 0, 2) == 99);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_add(graph, 0, i) == 0);
        assert(graphd_edge_add(graph, i, 0) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4) == 0);
    assert(graphd_edge_add(graph, 10, 3) == 0);
    assert(graphd_edge_add(graph, 5, 8) == 0);
    assert(graphd_edge_add(graph, 1, 16) == 0);
    assert(graphd_edge_add(graph, 7, 7) == 0);

    // print edges
    /*for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 8; ++k) {
                printf("%d", !!((graph->amatrix[i][j] << k) & 0x80));
            }
            printf(" ");
        }
        printf("\n");
    }*/

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i >= 3 && j == 0) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 3 && j == 4) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 10 && j == 3) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 5 && j == 8) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 1 && j == 16) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 7 && j == 7) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 0);
        }
    }

    assert(graphd_edge_add(graph, 0, 17) == 1);
    assert(graphd_edge_add(graph, 25, 4) == 1);
    assert(graphd_edge_add(graph, 17, 19) == 1);
    assert(graphd_edge_add(graph, 23, 23) == 1);

    for (int i = 0; i < 17; ++i) {
        for (int j = 0; j < 17; ++j) {
            if (i == 0 && j >= 3) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i >= 3 && j == 0) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 3 && j == 4) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 10 && j == 3) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 5 && j == 8) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 1 && j == 16) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else if (i == 7 && j == 7) assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 1);
            else assert(!!((graph->amatrix[i][j / 8] << (7 - (j % 8))) & 0x80) == 0);
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
        assert(graphd_edge_add(graph, 0, i) == 0);
        assert(graphd_edge_add(graph, i, 0) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4) == 0);
    assert(graphd_edge_add(graph, 10, 3) == 0);
    assert(graphd_edge_add(graph, 5, 8) == 0);
    assert(graphd_edge_add(graph, 1, 16) == 0);
    assert(graphd_edge_add(graph, 7, 7) == 0);

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
            assert(graphd_edge_add(graph, i, j) == 0);
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

    // print edges
    /*for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 8; ++k) {
                printf("%d", !!((graph->amatrix[i][j] << k) & 0x80));
            }
            printf(" ");
        }
        printf("\n");
    }*/

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


static int test_graphd_vertex_successors(void)
{
    printf("%-40s", "test_graphd_vertex_successors ");

    assert(graphd_vertex_successors(NULL, 0) == NULL);

    graphd_t *graph = graphd_new();

    for (int i = 0; i < 17; ++i) {
        assert(graphd_vertex_add(graph, &i, sizeof(int)) >= 0);
    }

    for (int i = 3; i < 17; ++i) {
        assert(graphd_edge_add(graph, 0, i) == 0);
        assert(graphd_edge_add(graph, i, 0) == 0);
    }

    assert(graphd_edge_add(graph, 3, 4) == 0);
    assert(graphd_edge_add(graph, 10, 3) == 0);
    assert(graphd_edge_add(graph, 5, 8) == 0);
    assert(graphd_edge_add(graph, 1, 16) == 0);
    assert(graphd_edge_add(graph, 7, 7) == 0);

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
    graphd_edge_add(graph, 8, 7);
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


int main(void) 
{
    test_graphd_destroy_nonexistent();
    test_graphd_new();
    test_graphd_with_capacity();

    test_graphd_vertex_add();
    test_graphd_vertex_get();

    test_graphd_edge_add();
    test_graphd_edge_exists();
    test_graphd_edge_remove();

    test_graphd_vertex_successors();

    test_graphd_vertex_map();
    test_graphd_vertex_map_bfs();
    test_graphd_vertex_map_dfs();

    return 0;
}