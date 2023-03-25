structures: src/vector.o src/linked_list.o src/dlinked_list.o src/clinked_list.o
	ar -rcs libdtstr.a src/vector.o src/linked_list.o src/dlinked_list.o src/clinked_list.o
	
vector: src/vector.c src/vector.h
	gcc -c src/vector.c -std=c99 -pedantic -Wall -O2 -o src/vector.o

linked_list: src/linked_list.c src/linked_list.h
	gcc -c src/linked_list.c -std=c99 -pedantic -Wall -O2 -o src/linked_list.o

dlinked_list: src/dlinked_list.c src/dlinked_list.h
	gcc -c src/dlinked_list.c -std=c99 -pedantic -Wall -O2 -o src/dlinked_list.o

clinked_list: src/clinked_list.c src/clinked_list.h
	gcc -c src/clinked_list.c -std=c99 -pedantic -Wall -O2 -o src/clinked_list.o

tests: tests/tests_vector.c tests/tests_linked_list.c tests/tests_dlinked_list.c tests/tests_clinked_list.c libdtstr.a
	make tests_vector
	make tests_linked_list
	make tests_dlinked_list
	make tests_clinked_list

tests_vector: tests/tests_vector.c src/vector.o libdtstr.a
	gcc tests/tests_vector.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_vector

tests_linked_list: tests/tests_linked_list.c src/linked_list.o libdtstr.a
	gcc tests/tests_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_linked_list

tests_dlinked_list: tests/tests_dlinked_list.c src/dlinked_list.o libdtstr.a
	gcc tests/tests_dlinked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_dlinked_list

tests_clinked_list: tests/tests_clinked_list.c src/clinked_list.o libdtstr.a
	gcc tests/tests_clinked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_clinked_list

benchmarks: benchmarks/benchmarks_vector.c benchmarks/benchmarks_linked_list.c benchmarks/benchmarks_dlinked_list.c libdtstr.a
	make benchmarks_vector
	make benchmarks_linked_list
	make benchmarks_dlinked_list
	make benchmarks_clinked_list
	
benchmarks_vector: benchmarks/benchmarks_vector.c src/vector.o libdtstr.a
	gcc benchmarks/benchmarks_vector.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o benchmarks/benchmarks_vector

benchmarks_linked_list: benchmarks/benchmarks_linked_list.c src/linked_list.o libdtstr.a
	gcc benchmarks/benchmarks_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o benchmarks/benchmarks_linked_list

benchmarks_dlinked_list: benchmarks/benchmarks_dlinked_list.c src/dlinked_list.o libdtstr.a
	gcc benchmarks/benchmarks_dlinked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o benchmarks/benchmarks_dlinked_list

benchmarks_clinked_list: benchmarks/benchmarks_clinked_list.c src/clinked_list.o libdtstr.a
	gcc benchmarks/benchmarks_clinked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o benchmarks/benchmarks_clinked_list

clean: 
	rm -f *.a *.o src/*.a src/*.o