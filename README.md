# P_Threading

# Multithreaded Matrix Multiplication and Merge Sort in C

This project demonstrates multithreaded implementations of both row and element-wise matrix multiplication and compare the time taken by each of them, as well as the parallelization of the merge sort algorithm using C programming language and POSIX threads (pthread).

## Features

### Matrix Multiplication

- **Element-wise Multiplication:** Concurrently computes element-wise multiplication of matrices using pthreads.
- **Row-wise Multiplication:** Utilizes multiple threads to perform row-wise matrix multiplication of two matrices.

### Merge Sort

- **Multithreaded Merge Sort:** Parallelizes the merge sort algorithm using pthreads to sort large datasets efficiently.


## Usage

### Building the Project

To try the project, execute the following commands in the terminal:
and modify the input.txt file to change the inputs.

```bash
cd Matrix/ && ./matrix
cd Merge/ && ./merge
