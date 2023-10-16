#include <stdio.h>
#include <string.h>

int main() {
    char* input = "submit ./fibfh;asdgljasgflhsda45 4";
    int start_position = 7; // Start position (0-based)
    int end_position = strlen(input) - 2; // End position (2nd last character)
    char string[100];

    if (start_position >= 0 && start_position <= end_position) {
        int length = end_position - start_position + 1;
        strncpy(string, input + start_position, length);
        string[length] = '\0'; // Null-terminate the copied string
        printf("%s\n", string);
    } else {
        printf("Invalid start and end positions.\n");
    }

    return 0;
}
