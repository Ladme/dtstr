// Released under MIT License.
// Copyright (c) 2023 Ladislav Bartos

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/dictionary.h"
#include "../src/alist.h"


static dict_t *dict_fill(const size_t items)
{
    dict_t *dict = dict_new();

    for (int i = 0; i < items; ++i) {

        char key[20] = "";
        sprintf(key, "key%d", i);

        dict_set(dict, key, &i, sizeof(int));
    }

    return dict;
}

static void benchmark_dict_set(const size_t items)
{
    printf("%s\n", "benchmark_dict_set [O(1)]");

    for (size_t i = 0; i <= 10; ++i) {

        size_t prefilled = i * 1000;
        dict_t *dict = dict_fill(prefilled);

        size_t initial = dict->allocated;

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand() % (prefilled + items);
            char key[20] = "";
            sprintf(key, "key%d", random);

            dict_set(dict, key, &random, sizeof(int));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, setting %12lu items: %f s  (I: %ld, F: %ld, N: %d)\n", 
            prefilled, items, time_elapsed, 
            initial, dict->allocated, (int) log2f( (float) dict->allocated / initial));

        dict_destroy(dict);
    }
    printf("\n");
}

static void benchmark_dict_get(const size_t items)
{
    printf("%s\n", "benchmark_dict_get [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 100000;
        dict_t *dict = dict_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            int random = rand() % prefilled;
            char key[20] = "";
            sprintf(key, "key%d", random);

            dict_get(dict, key);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, getting %12lu items: %f s\n", prefilled, items, time_elapsed);

        dict_destroy(dict);
    }
    printf("\n");
}

static void benchmark_dict_len(const size_t repeats)
{
    printf("%s\n", "benchmark_dict_len [O(n)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 10000;
        dict_t *dict = dict_fill(prefilled);

        clock_t start = clock();

        for (size_t j = 0; j < repeats; ++j) {
            dict_len(dict);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, obtaining length %12lu times: %f s\n", prefilled, repeats, time_elapsed);

        dict_destroy(dict);
    }
    printf("\n");
}

static void benchmark_dict_del(const size_t items)
{
    printf("%s\n", "benchmark_dict_del [O(1)]");

    for (size_t i = 1; i <= 10; ++i) {

        size_t prefilled = i * 10000;
        dict_t *dict = dict_fill(prefilled);

        size_t initial = dict->allocated;

        clock_t start = clock();

        for (size_t j = 0; j < items; ++j) {
            char key[20] = "";
            sprintf(key, "key%ld", i);

            dict_del(dict, key);
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> prefilled with %12lu items, removing %12lu items: %f s  (I: %ld, F: %ld, N: %d)\n", 
        prefilled, items, time_elapsed,
        initial, dict->allocated, (int) log2f( (float) dict->allocated / initial));

        dict_destroy(dict);
    }
    printf("\n");
}

static void benchmark_dict_set_del(const size_t repeats)
{
    printf("%s\n", "benchmark_dict_set_del ");

    for (size_t i = 1; i <= 10; ++i) {
        size_t items = i * 10000;
        
        dict_t *dict = dict_new();
        size_t initial = dict->allocated;
        size_t expanding = 0;
        size_t shrinking = 0;

        clock_t start = clock();

        for (size_t j = 0; j < repeats; ++j) {

            for (int k = 0; k < items; ++k) {
                char key[20] = "";
                sprintf(key, "key%d", k);

                dict_set(dict, key, &k, sizeof(int));
            }

            expanding += (size_t) log2f( (float) dict->allocated / initial);
            initial = dict->allocated;

            for (int k = 0; k < items; ++k) {
                int negative_k = -k;
                char key[20] = "";
                sprintf(key, "key%d", k);

                dict_set(dict, key, &negative_k, sizeof(int));
            }

            expanding += (size_t) log2f( (float) dict->allocated / initial);
            initial = dict->allocated;

            for (int k = 0; k < items; ++k) {
                char key[20] = "";
                sprintf(key, "key%d", k);

                dict_del(dict, key);
            }

            shrinking += (size_t) log2f( (float) initial / dict->allocated);
            initial = dict->allocated;

        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> setting and deleting %12lu items, %ld times: %f s  (E: %ld, S: %ld)\n", items, repeats, time_elapsed, expanding, shrinking);

        dict_destroy(dict);
    }
    printf("\n");
}

static void benchmarks_dict_set_preallocated(void)
{
    printf("%s\n", "benchmark_dict_set (default vs. preallocated)");

    size_t items[] = {100, 1000, 10000, 100000, 1000000};

    for (size_t j = 0; j < 5; ++j) {

        clock_t start = clock();
        dict_t *dict = dict_new();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            dict_set(dict, key, &i, sizeof(size_t));
        }

        clock_t end = clock();
        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> setting %12lu items, base capacity %12lu: %f s\n", items[j], DICT_DEFAULT_CAPACITY, time_elapsed);

        dict_destroy(dict);

        //***

        start = clock();
        dict = dict_with_capacity(items[j]);

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            dict_set(dict, key, &i, sizeof(size_t));
        }

        end = clock();
        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> setting %12lu items, base capacity %12lu: %f s\n", items[j], items[j], time_elapsed);

        dict_destroy(dict);

        printf("\n");
    }
}


static void benchmarks_dict_vs_alist_set(void)
{
    printf("%s\n", "benchmark_set (dict vs. alist)");

    size_t items[] = {10, 50, 100, 200, 300, 400, 500, 1000, 10000};

    for (size_t j = 0; j < 9; ++j) {

        clock_t start = clock();
        dict_t *dict = dict_new();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            dict_set(dict, key, &i, sizeof(size_t));
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> DICTIONARY: setting %12lu items: %f s\n", items[j], time_elapsed);

        dict_destroy(dict);

        // ***

        start = clock();
        alist_t *list = alist_new();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            alist_set(list, key, &i, sizeof(size_t));
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> ALIST:      setting %12lu items: %f s\n", items[j], time_elapsed);

        alist_destroy(list);

        printf("\n");
    }
}

static void benchmarks_dict_vs_alist_set_preallocated(void)
{
    printf("%s\n", "benchmark_set (dict vs. alist) [preallocated]");

    size_t items[] = {10, 50, 100, 200, 300, 400, 500, 1000, 10000};

    for (size_t j = 0; j < 9; ++j) {

        clock_t start = clock();
        dict_t *dict = dict_with_capacity(items[j]);

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            dict_set(dict, key, &i, sizeof(size_t));
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> DICTIONARY: setting %12lu items: %f s\n", items[j], time_elapsed);

        dict_destroy(dict);

        // ***

        start = clock();
        alist_t *list = alist_with_capacity(items[j]);

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            alist_set(list, key, &i, sizeof(size_t));
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> ALIST:      setting %12lu items: %f s\n", items[j], time_elapsed);

        alist_destroy(list);

        printf("\n");
    }
}

static void benchmarks_dict_vs_alist_get(const size_t get)
{
    printf("%s\n", "benchmark_get (dict vs. alist)");

    size_t items[] = {10, 20, 30, 40, 50, 100, 200, 500, 1000, 10000};

    for (size_t j = 0; j < 10; ++j) {

        dict_t *dict = dict_new();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            dict_set(dict, key, &i, sizeof(size_t));
        }

        clock_t start = clock();

        for (size_t i = 0; i < get; ++i) {
            size_t random = rand() % items[j];

            char key[40] = "";
            sprintf(key, "key%lu", random);
            dict_get(dict, key);
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> DICTIONARY: getting %12lu items from %12lu items: %f s\n", get, items[j], time_elapsed);

        dict_destroy(dict);

        // ***

        alist_t *list = alist_new();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            alist_set(list, key, &i, sizeof(size_t));
        }

        start = clock();

        for (size_t i = 0; i < get; ++i) {
            size_t random = rand() % items[j];

            char key[40] = "";
            sprintf(key, "key%lu", random);
            alist_get(list, key);
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> ALIST:      getting %12lu items from %12lu items: %f s\n", get, items[j], time_elapsed);

        alist_destroy(list);

        printf("\n");
    }
}

static void benchmarks_dict_vs_alist_del(void)
{
    printf("%s\n", "benchmark_del (dict vs. alist)");

    size_t items[] = {10, 50, 100, 200, 500, 600, 700, 800, 900, 1000, 10000};

    for (size_t j = 0; j < 10; ++j) {

        dict_t *dict = dict_new();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            dict_set(dict, key, &i, sizeof(size_t));
        }

        clock_t start = clock();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[40] = "";
            sprintf(key, "key%lu", i);
            assert(dict_del(dict, key) == 0);
        }

        clock_t end = clock();

        double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> DICTIONARY: deleting %12lu items: %f s\n", items[j], time_elapsed);

        dict_destroy(dict);

        // ***

        alist_t *list = alist_new();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[20] = "";
            sprintf(key, "key%lu", i);
            alist_set(list, key, &i, sizeof(size_t));
        }

        start = clock();

        for (size_t i = 0; i < items[j]; ++i) {
            char key[40] = "";
            sprintf(key, "key%lu", i);
            assert(alist_del(list, key) == 0);
        }

        end = clock();

        time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("> ALIST:      deleting %12lu items: %f s\n", items[j], time_elapsed);

        alist_destroy(list);

        printf("\n");
    }
}


int main(void)
{
    srand(time(NULL));

    benchmark_dict_set(100000);
    benchmark_dict_get(10000);
    benchmark_dict_len(1000);
    benchmark_dict_del(10000);
    benchmark_dict_set_del(10);

    benchmarks_dict_set_preallocated();
    benchmarks_dict_vs_alist_set();
    benchmarks_dict_vs_alist_set_preallocated();
    benchmarks_dict_vs_alist_get(1000);
    benchmarks_dict_vs_alist_del();

}