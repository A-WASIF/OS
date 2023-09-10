#include <stdio.h>
#include <unistd.h>

int main(void){
    pid_t pid;

    printf("Before fork I am one\n");

    pid = fork();

    if(pid == -1){
        perror("Unable to fork");
        
        return 1;
    }

    // printf("pid : %u\n", pid);
    printf("After fork i am two\n");
    return 0;
}