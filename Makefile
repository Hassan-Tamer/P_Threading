all: matrix_merge

matrix: matrix.c
	$(CC) matrix.c -o matrix_merge
	./matrix_merge

.PHONY: clean

clean:
	rm -f matrix_merge
	rm -rf output.txt
