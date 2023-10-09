#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main(){
    char arr[100];
    // char arr[] = "hello world!";
    // write(1, arr, sizeof(arr));
    read(0, arr, sizeof(arr));
    write(1, arr, sizeof(arr));
    return 0;
}
