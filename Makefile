test: tests/test_file_items.c
	clang -std=c99 -o test tests/test_file_items.c -lcriterion

clean:
	rm -rf test
