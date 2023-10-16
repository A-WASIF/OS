#include <stdio.h>
#include <unistd.h>

int main(void){
    pid_t pid;

    var = fork();

    if(pid == -1){
        perror("Unable to Fork\n");
        return 1;
    }

    else if(var == 0){
        sleep(40);
        printf("")
        printf("This is Child process\n");
        wait(8)
    }

    else{
        printf("This is Parent\n");
    }

    return 0;
}