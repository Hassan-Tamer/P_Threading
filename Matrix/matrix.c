#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

double cpu_time_used;

// Struct to represent a matrix
struct Matrix {
    int **matrix;
    int row;
    int column;
};

// Struct to hold multiple arguments
struct args{
    struct Matrix *mat1;
    struct Matrix *mat2;
    struct tuple *tup;

    // index of thread
    int indx;
};

// Helper struct to hold indices of rows that need to be multiplied
struct tuple{
    int mul1_indx;
    int mul2_indx;
};

// struct to hold result
struct Result {
    int val;
    int row;
    int column;
    int* rowval;
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

void* multiply_mat_element(void *fun_args){
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

void* multiply_mat_row(void *fun_args){
    struct args *my_args = (struct args *)fun_args;
    struct Matrix *mat1 = my_args->mat1;
    struct Matrix *matT = my_args->mat2;
    int indx = my_args->indx;
    int* count = (int *)malloc(matT->row * sizeof(int));
    for(int i=0;i<matT->row;i++){
        int* arr1 = mat1->matrix[indx];
        int* arr2 = matT->matrix[i];
        count[i] = dot_product(arr1,arr2,mat1->column);
    }

    struct Result *res = (struct Result *)malloc(sizeof(struct Result));
    res->rowval = count;
    res->row = indx;
    return res;
}

void displayResults(FILE *fp,int row_res,int col_res,int num_thrds,struct Result **results) {
    struct Matrix *mat3 = (struct Matrix *)malloc(sizeof(struct Matrix));
    initializeMatrix(mat3, row_res, col_res);
    for (int i = 0; i < num_thrds; i++) {
        struct Result *res = results[i];
        mat3->matrix[res->row][res->column] = res->val;
    }

    print_matrix(mat3);
    fp = fopen("outputMatrix.txt","w");
    for(int i=0;i<row_res;i++){
        for(int j=0;j<col_res;j++){
            fprintf(fp,"%d ",mat3->matrix[i][j]);
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"%f",cpu_time_used);
    fprintf(fp,"\n");
    fclose(fp);
}

void free_Matrix(struct Matrix *mat){
    int rows = mat->row;
    for(int i=0;i<rows;i++){
        free(mat->matrix[i]);
    }
    free(mat->matrix);
    free(mat);
}

void free_args(struct args **all_args,int num_thrds){
    for(int i=0;i<num_thrds;i++){
        free(all_args[i]->tup);
        // free_Matrix(all_args[i]->mat1);
        // free_Matrix(all_args[i]->mat2);
        free(all_args[i]);
    }
    free(all_args);
}

void free_Result(struct Result **results,int num_thrds){
    for(int i=0;i<num_thrds;i++){
        // free(results[i]->rowval);
        // free(results[i]);
    }
    free(results);
}





int main() {
    //**************//
    // processing input
    //**************//

    FILE *fp;
    fp = fopen("inputMatrix.txt", "r");
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


    // ************************** //
        // Multiply element wise
    // ************************** //

    int num_thrds = row1*column2;
    pthread_t threads[num_thrds];
    struct Result *results[num_thrds];
    struct args **all_args = (struct args **)malloc(num_thrds * sizeof(struct args *));
    
    clock_t start, end;
    start = clock();
    for (int i = 0; i < num_thrds; i++) {
        all_args[i] = (struct args *)malloc(sizeof(struct args));
        all_args[i] = init_args(mat1, mat2,i);
        pthread_create(&threads[i], NULL, multiply_mat_element, all_args[i]);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


    for (int i = 0; i < num_thrds; i++) {
        pthread_join(threads[i], (void **)&results[i]);
    }

    displayResults(fp,row1,column2,num_thrds,results);

    // ************************** //
        // Multiply row wise
    // ************************** //

    num_thrds = row1;

    pthread_t threads_row[num_thrds];
    struct Result **results_row = (struct Result **)malloc(num_thrds * sizeof(struct Result *));
    struct args **all_args_row = (struct args **)malloc(num_thrds * sizeof(struct args *));
    
    // clock_t start, end;
    start = clock();
    for (int i = 0; i < num_thrds; i++) {
        all_args_row[i] = (struct args *)malloc(sizeof(struct args));
        all_args_row[i] = init_args(mat1, mat2,i);
        pthread_create(&threads_row[i], NULL, multiply_mat_row, all_args_row[i]);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


    for (int i = 0; i < num_thrds; i++) {
        pthread_join(threads_row[i], (void **)&results[i]);
    }


    struct Matrix *mat3 = (struct Matrix *)malloc(sizeof(struct Matrix));
    initializeMatrix(mat3, row1, column2);
    for (int i = 0; i < num_thrds; i++) {
        struct Result *res = results[i];
        mat3->matrix[res->row] = res->rowval;
    }

    print_matrix(mat3);
    fp = fopen("outputMatrix.txt","a");
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
