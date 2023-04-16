structures: src/vector.o src/vector_sort.o src/linked_list.o src/dlinked_list.o src/clinked_list.o src/dictionary.o src/alist.o src/cbuffer.o src/queue.o src/avl_tree.o src/heap.o src/str.o src/matrix.o
	ar -rcs libdtstr.a src/vector.o src/vector_sort.o src/linked_list.o src/dlinked_list.o src/clinked_list.o src/dictionary.o src/alist.o src/cbuffer.o src/queue.o src/avl_tree.o src/heap.o src/str.o src/matrix.o
	
vector: src/vector.c src/vector.h
	gcc -c src/vector.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/vector.o
	make vector_sort

vector_sort: src/vector_sort.c src/vector.h
	gcc -c src/vector_sort.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/vector_sort.o

linked_list: src/linked_list.c src/linked_list.h
	gcc -c src/linked_list.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/linked_list.o

dlinked_list: src/dlinked_list.c src/dlinked_list.h
	gcc -c src/dlinked_list.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/dlinked_list.o

clinked_list: src/clinked_list.c src/clinked_list.h
	gcc -c src/clinked_list.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/clinked_list.o

dictionary: src/dictionary.c src/dictionary.h src/linked_list.c src/linked_list.h
	gcc -c src/dictionary.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/dictionary.o

alist: src/alist.c src/alist.h
	gcc -c src/alist.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/alist.o

cbuffer: src/cbuffer.c src/cbuffer.h
	gcc -c src/cbuffer.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/cbuffer.o

queue: src/queue.c src/queue.h src/dlinked_list.c src/dlinked_list.h
	gcc -c src/queue.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/queue.o

avl_tree: src/avl_tree.c src/avl_tree.h src/cbuffer.c src/cbuffer.h
	gcc -c src/avl_tree.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/avl_tree.o

heap: src/heap.c src/heap.h
	gcc -c src/heap.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/heap.o

str: src/str.c src/str.h src/vector.c src/vector.h
	gcc -c src/str.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/str.o

matrix: src/matrix.c src/matrix.h
	gcc -c src/matrix.c -std=c99 -pedantic -Wall -Wextra -O2 -o src/matrix.o

tests: tests/tests_vector.c tests/tests_linked_list.c tests/tests_dlinked_list.c tests/tests_clinked_list.c tests/tests_dictionary.c tests/tests_cbuffer.c tests/tests_queue.c tests/tests_avl_tree.c tests/tests_alist.c tests/tests_heap.c tests/tests_str.c tests/tests_matrix.c libdtstr.a
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

tests_vector: tests/tests_vector.c src/vector.o libdtstr.a
	gcc tests/tests_vector.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_vector

tests_linked_list: tests/tests_linked_list.c src/linked_list.o libdtstr.a
	gcc tests/tests_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_linked_list

tests_dlinked_list: tests/tests_dlinked_list.c src/dlinked_list.o libdtstr.a
	gcc tests/tests_dlinked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_dlinked_list

tests_clinked_list: tests/tests_clinked_list.c src/clinked_list.o libdtstr.a
	gcc tests/tests_clinked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_clinked_list

tests_dictionary: tests/tests_dictionary.c src/dictionary.o libdtstr.a
	gcc tests/tests_dictionary.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_dictionary

tests_alist: tests/tests_alist.c src/alist.o libdtstr.a
	gcc tests/tests_alist.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_alist

tests_cbuffer: tests/tests_cbuffer.c src/cbuffer.o libdtstr.a
	gcc tests/tests_cbuffer.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_cbuffer

tests_queue: tests/tests_queue.c src/queue.o src/dlinked_list.o libdtstr.a
	gcc tests/tests_queue.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_queue

tests_avl_tree: tests/tests_avl_tree.c src/avl_tree.o libdtstr.a
	gcc tests/tests_avl_tree.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_avl_tree

tests_heap: tests/tests_heap.c src/heap.o libdtstr.a
	gcc tests/tests_heap.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_heap

tests_str: tests/tests_str.c src/str.o libdtstr.a
	gcc tests/tests_str.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_str

tests_matrix: tests/tests_matrix.c src/matrix.o libdtstr.a
	gcc tests/tests_matrix.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -g -o tests/tests_matrix

benchmarks: benchmarks/benchmarks_vector.c benchmarks/benchmarks_linked_list.c benchmarks/benchmarks_dlinked_list.c benchmarks/benchmarks_dictionary.c benchmarks/benchmarks_queue_cbuffer.c benchmarks/benchmarks_avl_tree.c benchmarks/benchmarks_heap.c libdtstr.a
	make benchmarks_vector
	make benchmarks_linked_list
	make benchmarks_dlinked_list
	make benchmarks_clinked_list
	make benchmarks_dictionary
	make benchmarks_queue_cbuffer
	make benchmarks_avl_tree
	make benchmarks_heap
	
benchmarks_vector: benchmarks/benchmarks_vector.c src/vector.o libdtstr.a
	gcc benchmarks/benchmarks_vector.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -o benchmarks/benchmarks_vector

benchmarks_linked_list: benchmarks/benchmarks_linked_list.c src/linked_list.o libdtstr.a
	gcc benchmarks/benchmarks_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -o benchmarks/benchmarks_linked_list

benchmarks_dlinked_list: benchmarks/benchmarks_dlinked_list.c src/dlinked_list.o libdtstr.a
	gcc benchmarks/benchmarks_dlinked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -o benchmarks/benchmarks_dlinked_list

benchmarks_clinked_list: benchmarks/benchmarks_clinked_list.c src/clinked_list.o libdtstr.a
	gcc benchmarks/benchmarks_clinked_list.c libdtstr.a -std=c99 -pedantic -Wall -Wextra -O2 -o benchmarks/benchmarks_clinked_list

benchmarks_dictionary: benchmarks/benchmarks_dictionary.c src/dictionary.o libdtstr.a
	gcc benchmarks/benchmarks_dictionary.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O2 -o benchmarks/benchmarks_dictionary

benchmarks_queue_cbuffer: benchmarks/benchmarks_queue_cbuffer.c src/queue.o src/cbuffer.o libdtstr.a
	gcc benchmarks/benchmarks_queue_cbuffer.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O2 -o benchmarks/benchmarks_queue_cbuffer

benchmarks_avl_tree: benchmarks/benchmarks_avl_tree.c src/avl_tree.o src/vector.o libdtstr.a
	gcc benchmarks/benchmarks_avl_tree.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O2 -o benchmarks/benchmarks_avl_tree

benchmarks_heap: benchmarks/benchmarks_heap.c src/heap.o libdtstr.a
	gcc benchmarks/benchmarks_heap.c libdtstr.a -lm -std=c99 -pedantic -Wall -Wextra -O2 -o benchmarks/benchmarks_heap

clean: 
	rm -f *.a *.o src/*.a src/*.o