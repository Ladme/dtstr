structures: src/vector.o
	ar -rcs libdtstr.a src/vector.o
	
vector: src/vector.c src/vector.h
	gcc -c src/vector.c -std=c99 -pedantic -Wall -O2 -o src/vector.o

tests: tests/tests_vector.c libdtstr.a
	make tests_vector

tests_vector: tests/tests_vector.c src/vector.o
	gcc tests/tests_vector.c libdtstr.a -std=c99 -pedantic -Wall -O2 -g -o tests/tests_vector

clean: 
	rm -f *.a *.o src/*.a src/*.o