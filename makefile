structures: src/vector.o src/vector_sort.o src/linked_list.o src/dlinked_list.o src/clinked_list.o src/dictionary.o src/alist.o src/cbuffer.o src/queue.o src/avl_tree.o src/heap.o src/str.o src/matrix.o src/set.o src/graph.o src/unionfind.o src/converter.o
	ar -rcs libdtstr.a src/vector.o src/vector_sort.o src/linked_list.o src/dlinked_list.o src/clinked_list.o src/dictionary.o src/alist.o src/cbuffer.o src/queue.o src/avl_tree.o src/heap.o src/str.o src/matrix.o src/set.o src/graph.o src/unionfind.o src/converter.o
	
vector: src/vector.c src/vector.h
	gcc -c src/vector.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/vector.o
	make vector_sort

vector_sort: src/vector_sort.c src/vector.h
	gcc -c src/vector_sort.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/vector_sort.o

linked_list: src/linked_list.c src/linked_list.h
	gcc -c src/linked_list.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/linked_list.o

dlinked_list: src/dlinked_list.c src/dlinked_list.h
	gcc -c src/dlinked_list.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/dlinked_list.o

clinked_list: src/clinked_list.c src/clinked_list.h
	gcc -c src/clinked_list.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/clinked_list.o

dictionary: src/dictionary.c src/dictionary.h src/linked_list.c src/linked_list.h
	gcc -c src/dictionary.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/dictionary.o

alist: src/alist.c src/alist.h
	gcc -c src/alist.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/alist.o

cbuffer: src/cbuffer.c src/cbuffer.h
	gcc -c src/cbuffer.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/cbuffer.o

queue: src/queue.c src/queue.h src/dlinked_list.c src/dlinked_list.h
	gcc -c src/queue.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/queue.o

avl_tree: src/avl_tree.c src/avl_tree.h src/cbuffer.c src/cbuffer.h
	gcc -c src/avl_tree.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/avl_tree.o

heap: src/heap.c src/heap.h
	gcc -c src/heap.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/heap.o

str: src/str.c src/str.h src/vector.c src/vector.h
	gcc -c src/str.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/str.o

matrix: src/matrix.c src/matrix.h
	gcc -c src/matrix.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/matrix.o

set: src/set.c src/set.h
	gcc -c src/set.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/set.o

graph: src/graph.c src/graph.h
	gcc -c src/graph.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/graph.o

unionfind: src/unionfind.c src/unionfind.h
	gcc -c src/unionfind.c -std=c99 -pedantic -Wall -Wextra -O3 -o src/unionfind.o

converter: src/converter.c src/converter.h
	gcc -c src/converter.c -std=c99 -pedantic -Wall -Wextra -O3 src/converter.o

tests: tests/tests_vector.c tests/tests_linked_list.c tests/tests_dlinked_list.c tests/tests_clinked_list.c tests/tests_dictionary.c tests/tests_cbuffer.c tests/tests_queue.c tests/tests_avl_tree.c tests/tests_alist.c tests/tests_heap.c tests/tests_str.c tests/tests_matrix.c tests/tests_set.c tests/tests_graph.c tests/tests_unionfind.c tests/tests_converter.c libdtstr.a
	make tests_vector
	make tests_linked_list
	make tests_dlinked_list
	make tests_clinked_list
	make tests_dictionary
	make tests_alist
	make tests_cbuffer
	make tests_queue
	make tests_avl_tree
	make tests_heap
	make tests_str
	make tests_matrix
	make tests_set
	make tests_graph
	make tests_unionfind
	make tests_converter

tests_vector: tests/tests_vector.c src/vector.o
	gcc tests/tests_vector.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_vector

tests_linked_list: tests/tests_linked_list.c src/linked_list.o
	gcc tests/tests_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_linked_list

tests_dlinked_list: tests/tests_dlinked_list.c src/dlinked_list.o
	gcc tests/tests_dlinked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_dlinked_list

tests_clinked_list: tests/tests_clinked_list.c src/clinked_list.o
	gcc tests/tests_clinked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_clinked_list

tests_dictionary: tests/tests_dictionary.c src/dictionary.o
	gcc tests/tests_dictionary.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_dictionary

tests_alist: tests/tests_alist.c src/alist.o
	gcc tests/tests_alist.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_alist

tests_cbuffer: tests/tests_cbuffer.c src/cbuffer.o
	gcc tests/tests_cbuffer.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_cbuffer

tests_queue: tests/tests_queue.c src/queue.o src/dlinked_list.o
	gcc tests/tests_queue.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_queue

tests_avl_tree: tests/tests_avl_tree.c src/avl_tree.o
	gcc tests/tests_avl_tree.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_avl_tree

tests_heap: tests/tests_heap.c src/heap.o
	gcc tests/tests_heap.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_heap

tests_str: tests/tests_str.c src/str.o
	gcc tests/tests_str.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_str

tests_matrix: tests/tests_matrix.c src/matrix.o
	gcc tests/tests_matrix.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_matrix

tests_set: tests/tests_set.c src/set.o
	gcc tests/tests_set.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_set

tests_graph: tests/tests_graph.c src/graph.o
	gcc tests/tests_graph.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_graph

tests_unionfind: tests/tests_unionfind.c src/unionfind.o
	gcc tests/tests_unionfind.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_unionfind

tests_converter: tests/tests_converter.c src/converter.o
	gcc tests/tests_converter.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -g -o tests/tests_converter

benchmarks: benchmarks/benchmarks_vector.c benchmarks/benchmarks_linked_list.c benchmarks/benchmarks_dlinked_list.c benchmarks/benchmarks_dictionary.c benchmarks/benchmarks_queue_cbuffer.c benchmarks/benchmarks_avl_tree.c benchmarks/benchmarks_heap.c benchmarks/benchmarks_set.c benchmarks/benchmarks_graph.c benchmarks/benchmarks_unionfind.c libdtstr.a
	make benchmarks_vector
	make benchmarks_linked_list
	make benchmarks_dlinked_list
	make benchmarks_clinked_list
	make benchmarks_dictionary
	make benchmarks_queue_cbuffer
	make benchmarks_avl_tree
	make benchmarks_heap
	make benchmarks_set
	make benchmarks_unionfind
	
benchmarks_vector: benchmarks/benchmarks_vector.c src/vector.o
	gcc benchmarks/benchmarks_vector.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_vector

benchmarks_linked_list: benchmarks/benchmarks_linked_list.c src/linked_list.o
	gcc benchmarks/benchmarks_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_linked_list

benchmarks_dlinked_list: benchmarks/benchmarks_dlinked_list.c src/dlinked_list.o
	gcc benchmarks/benchmarks_dlinked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_dlinked_list

benchmarks_clinked_list: benchmarks/benchmarks_clinked_list.c src/clinked_list.o
	gcc benchmarks/benchmarks_clinked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_clinked_list

benchmarks_dictionary: benchmarks/benchmarks_dictionary.c src/dictionary.o
	gcc benchmarks/benchmarks_dictionary.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_dictionary

benchmarks_queue_cbuffer: benchmarks/benchmarks_queue_cbuffer.c src/queue.o src/cbuffer.o
	gcc benchmarks/benchmarks_queue_cbuffer.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_queue_cbuffer

benchmarks_avl_tree: benchmarks/benchmarks_avl_tree.c src/avl_tree.o src/vector.o
	gcc benchmarks/benchmarks_avl_tree.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_avl_tree

benchmarks_heap: benchmarks/benchmarks_heap.c src/heap.o
	gcc benchmarks/benchmarks_heap.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_heap

benchmarks_set: benchmarks/benchmarks_set.c src/set.o
	gcc benchmarks/benchmarks_set.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_set

benchmarks_graph: benchmarks/benchmarks_graph.c src/graph.o
	gcc benchmarks/benchmarks_graph.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_graph

benchmarks_unionfind: benchmarks/benchmarks_unionfind.c src/unionfind.o
	gcc benchmarks/benchmarks_unionfind.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O3 -o benchmarks/benchmarks_unionfind

clean: 
	rm -f *.a *.o src/*.a src/*.o