#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "instruction.h"
#include "lexer.h"

// Maximum length of a line that can be read from stdin
#define MAX_INPUT_LEN 256

static int run_repl(void) {
    int return_val = EXIT_SUCCESS;

    printf (
        "CForth REPL\n"
        "Suirabu 2021 <suirabu.dev@gmail.com>\n"
        "Type 'bye' to exit the REPL\n"
    );
    
    // Create lexer
    char* lex_buffer = malloc(DEFAULT_BUFFER_LEN);

    if(lex_buffer == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for lexer buffer\n");
        return EXIT_FAILURE;
    }

    Lexer lexer = { .buffer = lex_buffer, .buffer_len = DEFAULT_BUFFER_LEN };

    // Start REPL
    while(true) {
        char input_buffer[MAX_INPUT_LEN];

        if(fgets(input_buffer, MAX_INPUT_LEN, stdin) == NULL) {
            fprintf(stderr, "ERROR: Failed to read line from stdin\n");
            return_val = EXIT_FAILURE;
            break;
        }

        if(strcmp(input_buffer, "bye\n") == 0) {
            break;
        }

        // Lex line
        Instruction ins;
        Instruction* instructions = NULL;
        size_t n_instructions = 0;

        lexer.source = input_buffer;
        lexer.source_len = strlen(input_buffer);
        lexer.index = 0;

        while(true) {
            instructions = realloc(instructions, sizeof(Instruction) * ++n_instructions);

            if(instructions == NULL) {
                fprintf(stderr, "ERROR: Failed to reallocate instructions array\n");
                break;
            }
            
            ins = collect_instruction(&lexer);
            instructions[n_instructions - 1] = ins;

            if(ins.type == CtlEof || ins.type == CtlError) {
                break;
            }
        }

        // Free and exit on error
        if(ins.type == CtlError || instructions == NULL) {
            // Free previously allocated memory
            if(instructions != NULL) {
                for(size_t i = 0; i < n_instructions; ++i) {
                    if(instructions[i].type == InsPrintStr && instructions[i].str_val != NULL) {
                        free(instructions[i].str_val);
                    }
                }

                free(instructions);
            }

            return_val = EXIT_FAILURE;
            break;
        }

        // Free previously allocated memory
        if(instructions != NULL) {
            for(size_t i = 0; i < n_instructions; ++i) {
                if(instructions[i].type == InsPrintStr && instructions[i].str_val != NULL) {
                    free(instructions[i].str_val);
                }
            }

            free(instructions);
        }
    }

    free(lex_buffer);
    return return_val;
}

int main(void) {
    return run_repl();
}
