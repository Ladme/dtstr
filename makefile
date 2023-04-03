structures: src/vector.o src/vector_sort.o src/linked_list.o src/dlinked_list.o src/clinked_list.o src/dictionary.o src/queue.o src/avl_tree.o
	ar -rcs libdtstr.a src/vector.o src/vector_sort.o src/linked_list.o src/dlinked_list.o src/clinked_list.o src/dictionary.o src/queue.o src/avl_tree.o
	
vector: src/vector.c src/vector.h
	gcc -c src/vector.c -std=c99 -pedantic -Wall -O2 -o src/vector.o
	make vector_sort

vector_sort: src/vector_sort.c src/vector.h
	gcc -c src/vector_sort.c -std=c99 -pedantic -Wall -O2 -o src/vector_sort.o

linked_list: src/linked_list.c src/linked_list.h
	gcc -c src/linked_list.c -std=c99 -pedantic -Wall -O2 -o src/linked_list.o

dlinked_list: src/dlinked_list.c src/dlinked_list.h
	gcc -c src/dlinked_list.c -std=c99 -pedantic -Wall -O2 -o src/dlinked_list.o

clinked_list: src/clinked_list.c src/clinked_list.h
	gcc -c src/clinked_list.c -std=c99 -pedantic -Wall -O2 -o src/clinked_list.o

dictionary: src/dictionary.c src/dictionary.h src/linked_list.c src/linked_list.h
	gcc -c src/dictionary.c -std=c99 -pedantic -Wall -O2 -o src/dictionary.o

queue: src/queue.c src/queue.h src/dlinked_list.c src/dlinked_list.h
	gcc -c src/queue.c -std=c99 -pedantic -Wall -O2 -o src/queue.o

avl_tree: src/avl_tree.c src/avl_tree.h src/queue.c src/queue.h
	gcc -c src/avl_tree.c -std=c99 -pedantic -Wall -O2 -o src/avl_tree.o

tests: tests/tests_vector.c tests/tests_linked_list.c tests/tests_dlinked_list.c tests/tests_clinked_list.c tests/tests_dictionary.c tests/tests_avl_tree.c libdtstr.a
	make tests_vector
	make tests_linked_list
	make tests_dlinked_list
	make tests_clinked_list
	make tests_dictionary
	make tests_queue
	make tests_avl_tree

tests_vector: tests/tests_vector.c src/vector.o libdtstr.a
	gcc tests/tests_vector.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_vector

tests_linked_list: tests/tests_linked_list.c src/linked_list.o libdtstr.a
	gcc tests/tests_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_linked_list

tests_dlinked_list: tests/tests_dlinked_list.c src/dlinked_list.o libdtstr.a
	gcc tests/tests_dlinked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_dlinked_list

tests_clinked_list: tests/tests_clinked_list.c src/clinked_list.o libdtstr.a
	gcc tests/tests_clinked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_clinked_list

tests_dictionary: tests/tests_dictionary.c src/dictionary.o libdtstr.a
	gcc tests/tests_dictionary.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_dictionary

tests_queue: tests/tests_queue.c src/queue.o src/dlinked_list.o libdtstr.a
	gcc tests/tests_queue.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_queue

tests_avl_tree: tests/tests_avl_tree.c src/avl_tree.o libdtstr.a
	gcc tests/tests_avl_tree.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_avl_tree

benchmarks: benchmarks/benchmarks_vector.c benchmarks/benchmarks_linked_list.c benchmarks/benchmarks_dlinked_list.c benchmarks/benchmarks_dictionary.c benchmarks/benchmarks_avl_tree.c libdtstr.a
	make benchmarks_vector
	make benchmarks_linked_list
	make benchmarks_dlinked_list
	make benchmarks_clinked_list
	make benchmarks_dictionary
	make benchmarks_avl_tree
	
benchmarks_vector: benchmarks/benchmarks_vector.c src/vector.o libdtstr.a
	gcc benchmarks/benchmarks_vector.c libdtstr.a -std=c99 -pedantic -Wall -O2 -o benchmarks/benchmarks_vector

benchmarks_linked_list: benchmarks/benchmarks_linked_list.c src/linked_list.o libdtstr.a
	gcc benchmarks/benchmarks_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -o benchmarks/benchmarks_linked_list

benchmarks_dlinked_list: benchmarks/benchmarks_dlinked_list.c src/dlinked_list.o libdtstr.a
	gcc benchmarks/benchmarks_dlinked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -o benchmarks/benchmarks_dlinked_list

benchmarks_clinked_list: benchmarks/benchmarks_clinked_list.c src/clinked_list.o libdtstr.a
	gcc benchmarks/benchmarks_clinked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -o benchmarks/benchmarks_clinked_list

benchmarks_dictionary: benchmarks/benchmarks_dictionary.c src/dictionary.o libdtstr.a
	gcc benchmarks/benchmarks_dictionary.c libdtstr.a -lm -std=c99 -pedantic -Wall -O2 -o benchmarks/benchmarks_dictionary

benchmarks_avl_tree: benchmarks/benchmarks_avl_tree.c src/avl_tree.o src/vector.o libdtstr.a
	gcc benchmarks/benchmarks_avl_tree.c libdtstr.a -lm -std=c99 -pedantic -Wall -O2 -o benchmarks/benchmarks_avl_tree

clean: 
	rm -f *.a *.o src/*.a src/*.o