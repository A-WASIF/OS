#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(){
    execl("/bin/sh", "sh", "-c", "history", NULL);

    // If execl returns, there was an error
    perror("execl");
    printf("END!");

    return 0;

}


