#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    char *home_dir = getenv("HOME"); // Get the user's home directory

    if (home_dir == NULL) {
        fprintf(stderr, "Error: HOME environment variable not set\n");
        return 1;
    }

    char history_path[256];
    snprintf(history_path, sizeof(history_path), "%s/.bash_history", home_dir);

    FILE *history_file;
    char line[1024];

    // Open the Bash history file
    history_file = fopen(history_path, "r");

    if (history_file == NULL) {
        perror("Error opening history file");
        return 1;
    }

    // Read and display each line from the history file
    while (fgets(line, sizeof(line), history_file)) {
        printf("%s", line);
    }

    // Close the history file
    fclose(history_file);

    return 0;
}
