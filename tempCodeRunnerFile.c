#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readfl(const char *filename) {
    FILE *file = fopen(filename, "r");
    
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return NULL;
    }
    
    // Initialize variables to read lines from the file
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    // Initialize a buffer to store the file content
    size_t buffer_size = 1024; // Adjust this buffer size as needed
    char *buffer = (char *)malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    buffer[0] = '\0'; // Initialize the buffer as an empty string
    
    // Read lines from the file and append them to the buffer
    while ((read = getline(&line, &len, file)) != -1) {
        // Resize the buffer if needed
        if (len + strlen(buffer) >= buffer_size) {
            buffer_size *= 2; // Double the buffer size
            buffer = (char *)realloc(buffer, buffer_size);
            if (buffer == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                free(line);
                fclose(file);
                return NULL;
            }
        }
        strcat(buffer, line);
    }
    
    free(line); // Free the memory used for line
    fclose(file); // Close the file
    
    return buffer;
}

int main() {
    const char *filename = "months.txt"; // Replace with the desired file name
    char *fileContent = readfl(filename);
    
    if (fileContent != NULL) {
        printf("File content:\n%s\n", fileContent);
        free(fileContent); // Free the memory used for file content
    }
    
    return 0;
}
