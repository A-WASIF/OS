#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void){

    pid_t pid;

    pid = fork();

    char *pname;
    char *argv[] = {"/bin/ls", "-la", NULL};

    if(pid == -1){
        perror("Unsucessful Fork!");
        return 1;
    }

    else if(pid == 0){
        execve(argv[0], argv, NULL);
        // printf("This is child");
    }

    else{
        wait(NULL);
        printf("Process Completed\n");
    }
    return 0;
}