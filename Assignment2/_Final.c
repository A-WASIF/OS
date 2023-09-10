#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define HISTORY_FILE "history.txt"
FILE* file_History;

// Data structure to store command history with process IDs and execution times
struct CommandHistory {
    char command[256];
    pid_t pid;
    double execution_time;
};

void promtdisplay(char *current_dir);
void run_command(char *input_command, struct CommandHistory *history, int *history_count);
void writeHistory(struct CommandHistory *history, int history_count);
int launch(char *input_command, struct CommandHistory *history, int *history_count);
char *userinput();
void printHistory();
void tempHistory();
char * readfl(char *filename);


int main(){
    int size = 100;

    char current_dir[size];
    char initial_dir[size];
    getcwd(initial_dir, size);
    char *input;
    int execution;
    int history_count = 0;

    struct CommandHistory history[100];

    while (1){
        promtdisplay(current_dir);
        input = userinput();

        // printf("%s\n", input + 2);

        if(strcmp(input, "exit") == 0){ 
            writeHistory(history, history_count);
            printf("\n************************************************\n\n");
            printHistory();
            printf("\n************************************************\n");
            printf("Shell Terminated!\n");
            break;
        }

        else if(strcmp(input, "") == 0){
            free(input);
            continue;
        }

        else if(strlen(input) >= 3 && strcmp(input + strlen(input) - 3, ".sh") == 0){
            // printf("This is .sh file\n");
            char *str = input + 2;
            char *fileContent = readfl(str);
            if (fileContent != NULL) {
                execution = launch(fileContent, history, &history_count);
                free(fileContent);
            }
            free(input);
        }

        else{
            execution = launch(input, history, &history_count);
            free(input);
        }
    }

    return 0;
}

// Display Current Directory
void promtdisplay(char * current_dir){
    getcwd(current_dir, 100);
    printf("%s $ ", current_dir);
}

// Execution of an input_command
void run_command(char *input_command, struct CommandHistory *history, int *history_count){
    pid_t pid;
    int status;
    struct timeval start_time, end_time;

    // Record start time before forking
    gettimeofday(&start_time, NULL);

    // Create a child process
    pid = fork();

    if(pid < 0){
        perror("Fork Failed");
    }
    
    else if(pid == 0){
        // In the child process execute the command
        execl("/bin/sh", "sh", "-c", input_command, (char *)NULL);

        perror("Execution command error");
        exit(EXIT_FAILURE);
    }

    else{
        // In the parent process wait for the child
        waitpid(pid, &status, 0);

        // Record end time
        gettimeofday(&end_time, NULL);

        if (WIFEXITED(status)){
            double execution_time = (double)(end_time.tv_sec - start_time.tv_sec) + (double)(end_time.tv_usec - start_time.tv_usec) / 1e6;

            // Storing command, pid, and execution time in the history
            snprintf(history[*history_count].command, sizeof(history[*history_count].command), "%s", input_command);
            history[*history_count].pid = pid;
            history[*history_count].execution_time = execution_time;
            (*history_count)++;
        }
    }

    return;
}


// Print the command history with process IDs and execution times
void writeHistory(struct CommandHistory *history, int history_count){
    FILE *fd = fopen(HISTORY_FILE, "w");
    if (fd == NULL){
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < history_count; i++){
        fprintf(fd, "Command: %s, PID: %u, Execution Time: %f seconds\n", history[i].command, history[i].pid, history[i].execution_time);
    }

    fclose(fd);
}

// Checking inputs and calling for Execution
int launch(char *input_command, struct CommandHistory *history, int *history_count) {
    // Add the command to history
    // for tracing the history
    file_History = fopen(HISTORY_FILE, "a");
    if (file_History == NULL){
        perror("Error opening file");
    }
    
    else{
        fprintf(file_History, "%s\n", input_command);
        fclose(file_History);
    }

    if (strcmp(input_command, "history") == 0) {
        tempHistory();
    }
    
    else {
        run_command(input_command, history, history_count);
    }

    return 1;
}

// Take input from Users
char *userinput(){
    int size = 100;
    char *command = malloc(sizeof(char) * size);
    if (command == NULL) {
        printf("%s\n", "Could not allocate memory!! Try again");
        command[0] = '\0';
        return command;
    }
    fgets(command, size, stdin);
    int len = strlen(command);
    len = len - 1;
    for (int i = len; i > 0; i--){
        if (command[len] == ' ' || command[len] == '\n') {
            command[len--] = '\0';
        }
        
        else{
            break;
        }
    }
    return command;
}

// Show history.txt with pids, execution time, etc. at termination of program
void printHistory(){
    FILE *file = fopen("history.txt", "r");
    if (file == NULL){
        perror("Error opening file");
        return;
    }

    char line[256];

    // Read and print each line from the file
    while(fgets(line, sizeof(line), file) != NULL){
        printf("%s", line);
    }

    fclose(file);
}

// Show history of commands written so far in program during execution program
void tempHistory(){
        char chr;
        FILE * fd;

        fd = fopen("history.txt", "r");

        if(fd == NULL){
            perror("Error in Opening History!");
        }

        while((chr = fgetc(fd)) != EOF) printf("%c", chr);

        fclose(fd);

        return;
}

char *readfl(char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return NULL;
    }

    // Initialize variables to read lines from the file
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *fileContent = NULL;

    // Read lines from the file and append them to fileContent
    while ((read = getline(&line, &len, file)) != -1) {
        if (fileContent == NULL) {
            fileContent = strdup(line);
        } else {
            size_t oldLen = strlen(fileContent);
            size_t newLen = oldLen + strlen(line);
            fileContent = realloc(fileContent, newLen + 1);
            if (fileContent == NULL) {
                perror("Memory allocation failed");
                free(line);
                fclose(file);
                return NULL;
            }
            strcat(fileContent, line);
        }
    }

    free(line);
    fclose(file);

    return fileContent;
}