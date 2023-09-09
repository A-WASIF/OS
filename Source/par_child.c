#include <stdio.h>
#include <unistd.h>

int main(void){
    pid_t pid;

    pid = fork();

    if(pid == -1){
        perror("Unable to Fork\n");
        return 1;
    }

    else if(pid == 0){
        sleep(40);
        printf("This is Child process\n");
    }

    else{
        printf("This is Parent\n");
    }

    return 0;
}