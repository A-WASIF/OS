#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    int size = 100;
    char * curr_dir[size];

    getcwd(curr_dir, size);

    printf("%s", curr_dir);
}