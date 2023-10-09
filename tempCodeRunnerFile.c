#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
// #include <sys/wait.h>

int main(){
    char arr[] = "hello world!";
    write(1, arr, 11);
    return 0;
}
