#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>


int main(){
    printf("Hello\n");
    for(int i=0; i<3; i++) {
        if(fork() == 0) {
            printf("Hi\n");
        }
    }
    printf("World\n");
    
    return 0;
}