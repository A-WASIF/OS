#include <stdio.h>
#include <unistd.h>

int main(){
    int a, b;
    a = 10;
    b = 20;

    pid_t pid;
    pid = getpid();

    pid_t ppid;
    ppid = getppid();

    printf("Sum : %d\n", a + b);
    printf("Process Identifier : %u\n", pid);
    printf("Parent Process ID : %u\n", ppid);
}
