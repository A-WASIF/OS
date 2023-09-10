#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){
    int size = 100;
    char curr_dir[size];

    getcwd(curr_dir, sizeof(curr_dir));

    printf("%s\n", curr_dir);
    return 0;
}