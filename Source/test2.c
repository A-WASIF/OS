#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int global = 0;

int main(void){
    int status = fork();
    if (status < 0)
    {
        printf("Something bad happened\n");
    }
    else if (status == 0)
    {
        printf("I am the child process\n");
        global = 1000;
    }
    else
    {
        printf("I am the parent Shell\n");
        printf("Parent Global value = ");
    }

    printf("%d\n", global);
    return 0;
}