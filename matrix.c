#include <stdio.h>
#include <stdlib.h>

int main(){
    system("gcc element.c -o element_exe -pthread");
    system("./element_exe");

    system("gcc row.c -o row_exe -pthread");
    system("./row_exe");
}