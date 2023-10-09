#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

typedef struct
{
    pid_t pid;
    time_t start_time;
    double duration;
    char command[4096];
} CommandHistory;

#define MAX_HISTORY_SIZE 100

void print_history(CommandHistory history[], int history_size)
{
    for (int i = 0; i < history_size; i++)
    {
        printf("PID: %d\n", history[i].pid);
        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        long long current_time_ms = current_time.tv_sec * 1000LL + current_time.tv_usec / 1e6;
        printf("Start Time: %s", ctime(&history[i].start_time));
        long long duration_ms = current_time_ms - (history[i].start_time * 1e6);
        printf("Duration: %f milliseconds\n", ((double)duration_ms / 1e9)/10);
        printf("Command: %s\n", history[i].command);
        printf("\n");
    }
}

int main()
{
    char perm_directory[4096];
    char temp_perm_directory[4096];
    getcwd(perm_directory, 4096);
    getcwd(temp_perm_directory, 4096);
    printf("Shell Has Been Started\n");
    printf("Shell Name: k@li\n");
    printf("Shell Author: Vishal(2022580) & Subham(2022510)\n");
    CommandHistory history[MAX_HISTORY_SIZE];
    int history_size = 0;
    bool flag=true;
    while (flag)
    {
        char curr_work_dir[4096];
        getcwd(curr_work_dir, 4096);
        strcat(perm_directory, "/");
        printf("K@li $-> ");
        size_t size = 1024;
        char arr_fgets[4096];
        fgets(arr_fgets, 4096, stdin);
        strncpy(history[history_size].command, arr_fgets, sizeof(history[history_size].command));
        char *delin_ch = " \n";
        char *token_ch;
        char **given_input = (char **)malloc(4096 * sizeof(char *));
        token_ch = strtok(arr_fgets, delin_ch);
        int i = 0;
        given_input[i] = token_ch;
        while (token_ch != NULL)
        {
            token_ch = strtok(NULL, delin_ch);
            i++;
            given_input[i] = token_ch;
        }
        int arg_count = 0;
        while (given_input[arg_count] != NULL)
        {
            arg_count++;
        }
        if (arg_count == 0)
        {
            continue;
        }
        pid_t pid = fork();
        CommandHistory.pid=pid;
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            if (strcmp(given_input[0], "echo") == 0) // pass  cat hw.c | wc -l
            {
                for (int i = 1; i < arg_count; i++)
                {
                    printf("%s ", given_input[i]);
                }
                printf("\n");
                exit(EXIT_SUCCESS);
            }

            if (strcmp(given_input[0], "history") == 0) // (-f) to print in the file and (-ot) to print on terminal.
            {
                if (arg_count == 1 || (arg_count == 2 && strcmp(given_input[1], "-ot") == 0))
                {
                    print_history(history, history_size);
                }
                else if (arg_count == 2 && strcmp(given_input[1], "-f") == 0)
                {
                    // Print history to a file named history.txt
                    FILE *history_file = fopen("history.txt", "w");
                    if (history_file == NULL)
                    {
                        perror("fopen");
                    }
                    else
                    {
                        for (int i = 0; i < history_size; i++)
                        {
                            fprintf(history_file, "PID: %d\n", history[i].pid);
                            fprintf(history_file, "Start Time: %s", ctime(&history[i].start_time));
                            fprintf(history_file, "Duration: %.2f seconds\n", history[i].duration);
                            fprintf(history_file, "Command: %s\n", history[i].command);
                            fprintf(history_file, "\n");
                        }
                        fclose(history_file);
                    }
                }
                else
                {
                    printf("Usage: history [-f] [-ot]\n");
                }
            }

            else if (strcmp(given_input[0], "cat") == 0) // pass
            {
                if (arg_count < 2)
                {
                    printf("Usage: cat <filename>\n");
                }
                else if (arg_count == 2)
                {
                    char *args[] = {"cat", given_input[1], NULL};
                    execv("/bin/cat", args);
                    perror("execv");
                    exit(EXIT_FAILURE);
                }
                else if (strcmp(given_input[2], "|") == 0) // pass
                {
                    char input_command[4096] = "";
                    for (int i = 0; i < arg_count; i++)
                    {
                        strcat(input_command, given_input[i]);
                        strcat(input_command, " ");
                    }
                    execl("/bin/sh", "sh", "-c", input_command, (char *)NULL);
                    perror("execl (shell)");
                    exit(EXIT_FAILURE);
                }
            }
            else if (strcmp(given_input[0], "exit") == 0 || strcmp(given_input[0], "close") == 0) // pass
            {
                printf("Exited Successfully!\n");
                if (kill(pid, SIGINT) == 0) {
                        printf("Ctrl+C (SIGINT) sent successfully.\n");
                    } 
                else {
                        perror("Error sending Ctrl+C (SIGINT) signal");
                    }
                exit(EXIT_SUCCESS);
            }
            else if (strcmp(given_input[0], "ls") == 0) // pass
            {
                if (arg_count == 1)
                {
                    char *args[] = {"ls", NULL};
                    execv("/bin/ls", args);
                    perror("execv");
                }
                else if (strcmp(given_input[1], "-l") == 0)
                {
                    char *args[] = {"ls", "-l", NULL};
                    execv("/bin/ls", args);
                    perror("execv");
                }
                else if (strcmp(given_input[1], "-i") == 0)
                {
                    char *args[] = {"ls", "-i", NULL};
                    execv("/bin/ls", args);
                    perror("execv");
                }
                else if (strcmp(given_input[1], "/home") == 0)
                {
                    char *args[] = {"ls", "/home", NULL};
                    execv("/bin/ls", args);
                    perror("execv");
                }
                else
                {
                    printf("Invalid ls command\n");
                }
                exit(EXIT_FAILURE);
            }
            else if (strcmp(given_input[0], "wc") == 0) // pass
            {
                execv("/usr/bin/wc", given_input);
                perror("execv");
                exit(EXIT_FAILURE);
            }

            else if (strncmp(given_input[0], "./", 2) == 0) // pass    // use "./<space>fib.c" to compile and run
            {
                if (arg_count == 1)
                {
                    char *execute_args[] = {"./temp_binary", given_input[2], NULL};
                    execvp(given_input[0], execute_args);
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
                else if (arg_count == 2)
                {
                    // Compile and run the .c file
                    char *compile_args[] = {"gcc", given_input[1], "-o", "temp_binary", NULL};
                    int compile_status = 0;
                    pid_t compile_pid = fork();
                    if (compile_pid == -1)
                    {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }
                    if (compile_pid == 0)
                    {
                        execvp("gcc", compile_args);
                        perror("execvp");
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        int compile_result;
                        waitpid(compile_pid, &compile_result, 0);
                        if (compile_result == 0)
                        {
                            // Compilation successful, execute the binary
                            // printf("%s\n", given_input[2]);
                            char *execute_args[] = {"./temp_binary", given_input[2], NULL};
                            execvp("./temp_binary", execute_args);
                            perror("execvp");
                            exit(EXIT_FAILURE);
                            printf("\n");
                        }
                        else
                        {
                            printf("Compilation failed\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                else
                {
                    printf("Usage: ./<filename> to execute the binary and ./<space>fib.c to compile and run \n");
                    exit(EXIT_FAILURE);
                }
            }

            else if (strcmp(given_input[0], "grep") == 0) // pass
            {
                if (arg_count == 3)
                {
                    char *args[] = {"grep", given_input[1], given_input[2], NULL};
                    execv("/bin/grep", args);
                    perror("execv");
                    exit(EXIT_FAILURE);
                }
                else if (arg_count == 4)
                {
                    char *args[] = {"grep", given_input[1], given_input[2], given_input[3], NULL};
                    execv("/bin/grep", args);
                    perror("execv");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    printf("Usage: grep <pattern> <filename>\n");
                    exit(EXIT_FAILURE);
                }
            }

            else if (strcmp(given_input[0], "cd") == 0) // Pass
            {
                if (arg_count != 2)
                {
                    printf("Usage: <directory>\n");
                }

                if (chdir(given_input[1]) == -1)
                {
                    perror("chdir");
                }

                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                {
                    printf("Current directory: %s\n", cwd);
                }

                else
                {
                    perror("getcwd");
                }
            }

            else if (strcmp(given_input[0], "sort") == 0) // pass
            {
                if (arg_count != 2)
                {
                    printf("Usage: sort <filename>\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    char *args[] = {"sort", given_input[1], NULL};
                    execv("/usr/bin/sort", args);
                    perror("execv");
                    exit(EXIT_FAILURE);
                }
            }
            else if (strcmp(given_input[0], "pwd") == 0)
            { // pass
                printf("%s\n", curr_work_dir);
            }
            else if (strcmp(given_input[0], "uniq") == 0) // pass
            {
                if (arg_count != 2)
                {
                    printf("Usage: uniq <filename>\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    char *args[] = {"uniq", given_input[1], NULL};
                    execv("/usr/bin/uniq", args);
                    perror("execv");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                printf("Command not supported yet or not a command!\n");
                exit(EXIT_FAILURE);
            }
            history_size++;
        }
        else // pass
        {
            CommandHistory cmd;
            cmd.pid = pid;
            cmd.start_time = time(NULL);
            cmd.duration = -1.0; // To be filled in when the command completes
            // strncpy(cmd.command, arr_fgets, sizeof(cmd.command));
            strncpy(history[history_size].command, arr_fgets, sizeof(history[history_size].command));

            // history[history_size] = cmd;
            history_size++;
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) != 0)
                {
                    printf("Child process exited with non-zero status: %d\n", WEXITSTATUS(status));
                }
            }
            else
            {
                printf("Child process terminated abnormally\n");
            }
        }
    }
    return 0;
}