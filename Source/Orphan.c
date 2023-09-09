#include <stdio.h>
#include <unistd.h>

int main(void){
    pid_t pid;
    pid_t ppid;

    pid = fork();

    if(pid == -1){
        perror("Unsucessful Fork!");
    }

    else if(pid == 0){
        ppid = getppid();
        printf("PPID of child : %u\n", ppid);
        sleep(15);
        printf("This is Child process\n");
    }

    else{
        ppid = getppid();

        printf("PID & PPID is : %u %u\n", pid, ppid);
        printf("This is Parent Process\n");
    }
}