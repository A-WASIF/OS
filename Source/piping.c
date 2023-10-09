#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main(){
	int arr[2];
    
    // arr[0] --> reading
    // arr[1] --> writing

    if(pipe(arr) == -1){
        perror("Error occured in piping!\n");
    }

    else{
        pid_t pid = fork();
        if(pid == -1){
            perror("Error occured in forking!\n");
        }

        else if(pid == 0){
            close(arr[0]);
            char message[100];

            printf("Enter Your message for parent : ");
            // scanf("%99[^\n]%*c", message);
            fgets(message, sizeof(message), stdin);

            write(arr[1], message, sizeof(message));
            close(arr[1]);
            exit(0);
        }

        else{
            close(arr[1]);
            char mess[100];
            read(arr[0], mess, sizeof(mess));
            close(arr[0]);

            printf("Message by child : %s", mess);
            wait(NULL);
            return 0;
        }
    }

    return 0;
}
