CC=clang
camphore.out: main.o file_items.o
	clang -std=c99 -o camphore.out main.o file_items.o
main.o: main.c
	clang -std=c99 -c main.c
file_items.o: file_items.c
	clang -std=c99 -c file_items.c
debug:
	clang -std=c99 -o camphore.out main.c file_items.c -g
test: tests/test_file_items.c file_items.o
	clang -std=c99 -o test tests/test_file_items.c file_items.o -lcriterion
clean:
	rm -rf test *.o
