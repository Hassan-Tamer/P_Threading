#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

double cpu_time_used;

// Struct to hold multiple arguments
struct Matrix {
    int **matrix;
    int row;
    int column;
};

struct args{
    struct Matrix *mat1;
    struct Matrix *mat2;
    struct tuple *tup;

    // index of thread
    int indx;
};

struct tuple{
    int mul1_indx;
    int mul2_indx;
};

struct Result {
    int val;
    int row;
    int column;
};

void initializeMatrix(struct Matrix *mat, int rows, int columns) {
    mat->row = rows;
    mat->column = columns;

    mat->matrix = (int **)malloc(rows * sizeof(int *));
    if (mat->matrix == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        mat->matrix[i] = (int *)malloc(columns * sizeof(int));
        if (mat->matrix[i] == NULL) {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
    }
}

struct Matrix* transpose(struct Matrix* mat){

    int rows = mat->row;
    int columns = mat->column;
    int **matrix = mat->matrix;
    int **newMatrix = (int **)malloc(columns * sizeof(int *));
    if (newMatrix == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < columns; i++) {
        newMatrix[i] = (int *)malloc(rows * sizeof(int));
        if (newMatrix[i] == NULL) {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    for(int i=0;i<rows;i++){
        for(int j=0;j<columns;j++){
            newMatrix[j][i] = matrix[i][j];
        }
    }

    struct Matrix *mat3 = (struct Matrix *)malloc(sizeof(struct Matrix));
    mat3->matrix = newMatrix;
    mat3->row = columns;
    mat3->column = rows;
    return mat3;

}

void print_matrix(struct Matrix* mat){
    int rows = mat->row;
    int columns = mat->column;
    int **matrix = mat->matrix;
    for(int i=0;i<rows;i++){
        for(int j=0;j<columns;j++){
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
}

struct args* init_args(struct Matrix* mat1, struct Matrix* mat2,int index){
    struct args *args = (struct args *)malloc(sizeof(struct args));
    int num_thrds = mat1->row*mat2->column;
    struct Matrix * matT;
    matT = transpose(mat2);
    struct tuple *tup = (struct tuple *)malloc(sizeof(struct tuple)*num_thrds);
    int row1 = mat1->row;
    int column2 = mat2->column;
    int k=0;
    for(int i=0;i<row1;i++){
        for(int j=0;j<column2;j++){
            tup[k].mul1_indx = i;
            tup[k].mul2_indx = j;
            k++;
        }
    }
    args->mat1 = mat1;
    args->mat2 = matT;
    args->tup = tup;
    // args->row_out = row1;
    // args->column_out = column2;
    args->indx = index;
    return args;
}

int dot_product(int *arr1, int *arr2, int size) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        result += arr1[i] * arr2[i];
    }
    return result;
}

void* multiply_mat(void *fun_args){
    struct args *my_args = (struct args *)fun_args;
    struct Matrix *mat1 = my_args->mat1;
    struct Matrix *matT = my_args->mat2;
    struct tuple *tup = my_args->tup;
    int indx = my_args->indx;
    int num_iterations = mat1->row*matT->row;
    int count = 0;
    struct tuple *tup_local;
    tup_local = &my_args->tup[indx];
    int row = tup_local->mul1_indx;
    int col = tup_local->mul2_indx;
    int* arr1 = mat1->matrix[row];
    int* arr2 = matT->matrix[col];
    count = dot_product(arr1,arr2,mat1->column);
    struct Result *res = (struct Result *)malloc(sizeof(struct Result));
    res->val = count;
    res->row = row;
    res->column = col;
    return res;
}


int main() {
    FILE *fp;
    fp = fopen("input.txt", "r");
    if (fp == NULL) {
        printf("Error opening the file.\n");
        exit(EXIT_FAILURE);
    }

    int row1, column1, row2, column2;

    fscanf(fp, "%d %d\n", &row1, &column1);

    struct Matrix *mat1 = (struct Matrix *)malloc(sizeof(struct Matrix));
    initializeMatrix(mat1, row1, column1);

    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < column1; j++) {
            fscanf(fp, "%d", &mat1->matrix[i][j]);
        }
    }

    fscanf(fp, "%d %d\n", &row2, &column2);

    struct Matrix *mat2 = (struct Matrix *)malloc(sizeof(struct Matrix));
    initializeMatrix(mat2, row2, column2);

    for (int i = 0; i < row2; i++) {
        for (int j = 0; j < column2; j++) {
            fscanf(fp, "%d", &mat2->matrix[i][j]);
        }
    }

    fclose(fp);

    int num_thrds = row1*column2;
    pthread_t threads[num_thrds];
    struct Result *results[num_thrds];
    struct args **all_args = (struct args **)malloc(num_thrds * sizeof(struct args *));
    
    clock_t start, end;
    start = clock();
    for (int i = 0; i < num_thrds; i++) {
        all_args[i] = (struct args *)malloc(sizeof(struct args));
        all_args[i] = init_args(mat1, mat2,i);
        pthread_create(&threads[i], NULL, multiply_mat, all_args[i]);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


    for (int i = 0; i < num_thrds; i++) {
        pthread_join(threads[i], (void **)&results[i]);
    }

    // display results
    struct Matrix *mat3 = (struct Matrix *)malloc(sizeof(struct Matrix));
    initializeMatrix(mat3, row1, column2);
    for (int i = 0; i < num_thrds; i++) {
        struct Result *res = results[i];
        mat3->matrix[res->row][res->column] = res->val;
    }

    print_matrix(mat3);
    fp = fopen("output.txt","w");
    for(int i=0;i<row1;i++){
        for(int j=0;j<column2;j++){
            fprintf(fp,"%d ",mat3->matrix[i][j]);
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"%f",cpu_time_used);
    fclose(fp);

    return 0;
}
