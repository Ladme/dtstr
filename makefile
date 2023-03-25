structures: src/vector.o src/linked_list.o
	ar -rcs libdtstr.a src/vector.o src/linked_list.o
	
vector: src/vector.c src/vector.h
	gcc -c src/vector.c -std=c99 -pedantic -Wall -O2 -o src/vector.o

linked_list: src/linked_list.c src/linked_list.h
	gcc -c src/linked_list.c -std=c99 -pedantic -Wall -O2 -o src/linked_list.o

tests: tests/tests_vector.c tests/tests_linked_list.c libdtstr.a
	make tests_vector
	make tests_linked_list

tests_vector: tests/tests_vector.c src/vector.o
	gcc tests/tests_vector.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_vector

tests_linked_list: tests/tests_linked_list.c src/linked_list.o
	gcc tests/tests_linked_list.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_linked_list

clean: 
	rm -f *.a *.o src/*.a src/*.o