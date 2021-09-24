#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdnoreturn.h>

// Maximum length of a line that can be read from stdin
#define MAX_INPUT_LEN 256

noreturn static void run_repl(void) {
    printf (
        "CForth REPL\n"
        "Suirabu 2021 <suirabu.dev@gmail.com>\n"
        "Type 'bye' to exit the REPL\n"
    );
    
    while(true) {
        char input_buffer[MAX_INPUT_LEN];

        if(fgets(input_buffer, MAX_INPUT_LEN, stdin) == NULL) {
            fprintf(stderr, "ERROR: Failed to read line from stdin\n");
            exit(EXIT_FAILURE);
        }

        printf("> %s", input_buffer);

        if(strcmp(input_buffer, "bye\n") == 0) {
            exit(EXIT_SUCCESS);
        }
    }
}

int main(void) {
    run_repl();
}
