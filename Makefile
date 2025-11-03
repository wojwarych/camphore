CC=clang
camphore.out: main.o file_items.o
	clang -std=c99 -o camphore.out main.o file_items.o
main.o: main.c
	clang -std=c99 -c main.c
file_items.o: file_items.c
	clang -std=c99 -c file_items.c
test: tests/test_file_items.c
	clang -std=c99 -o test tests/test_file_items.c -lcriterion
clean:
	rm -rf test *.o
