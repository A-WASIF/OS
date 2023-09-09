#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid;

    pid = fork();

    if(pid == -1){
        perror("Unable to fork!\n");
        return 1;
    }

    else if(pid == 0){
        printf("This is child!\n");
    }

    else{
        wait(NULL);
        sleep(20);
        printf("This is Parent!\n");
    }

    return 0;
}