#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


struct args {
    int *arr;
    int low;
    int high;
};

void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}


void* mergeSort(void *fun_args) {
    struct args *args = (struct args *)fun_args;
    int *arr = args->arr;
    int l = args->low;
    int r = args->high;
    if (l < r) {
        int m = l + (r - l) / 2;
        pthread_t left_thread, right_thread;
        struct args args1 = { arr, l, m };
        struct args args2 = { arr, m + 1, r };
        pthread_create(&left_thread, NULL, mergeSort, &args1);
        pthread_create(&right_thread, NULL, mergeSort, &args2);
        pthread_join(left_thread, NULL);
        pthread_join(right_thread, NULL);
        merge(arr, l, m, r);
    }
}

int main() {
    FILE *fp;
    fp = fopen("inputMerge.txt", "r");
    if (fp == NULL) {
        printf("Error opening the file.\n");
        exit(EXIT_FAILURE);
    }

    int arr_size;

    fscanf(fp, "%d\n", &arr_size);
    int* arr = (int *)malloc(sizeof(int) * arr_size);
    for(int i = 0; i < arr_size; i++){
        int num;
        fscanf(fp, "%d", &num);
        arr[i] = num;
    }

    printf("Given array is \n");
    for (int i = 0; i < arr_size; i++)
        printf("%d ", arr[i]);

    pthread_t main;
    struct args argos = { arr, 0, arr_size - 1 };
    pthread_create(&main, NULL, mergeSort, &argos);
    pthread_join(main, NULL);

    fp = fopen("outputMerge.txt","w");
    
    printf("\nSorted array is \n");
    for (int i = 0; i < arr_size; i++){
        printf("%d ", arr[i]);
        fprintf(fp, "%d ", arr[i]);
    }

    printf("\n");
    return 0;
}
