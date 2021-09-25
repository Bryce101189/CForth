#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instruction.h"
#include "lexer.h"

static bool reached_end(Lexer* lexer) {
    return lexer->index >= lexer->source_len;
}

static char peek(Lexer* lexer) {
    return lexer->source[lexer->index];
}

static char advance(Lexer* lexer) {
    return lexer->source[lexer->index++];
}

static void skip_whitespace(Lexer* lexer) {
    while(!reached_end(lexer) && isspace(peek(lexer))) {
        advance(lexer);
    }
}

static bool is_number(char* buffer) {
    size_t len = strlen(buffer);

    for(size_t i = 0; i < len; ++i) {
        if(!isdigit(buffer[i])) {
            return false;
        }
    }

    return true;
}

Instruction collect_instruction(Lexer* lexer) {
    skip_whitespace(lexer);

    if(reached_end(lexer)) {
        return (Instruction) { .type = CtlEof };
    }

    unsigned int buffer_index = 0;

    // Read next word into buffer char by char
    while(!reached_end(lexer) && !isspace(peek(lexer))) {
        lexer->buffer[buffer_index++] = advance(lexer);
    
        // Increase size of word buffer if we're running out of space
        if(buffer_index + 1 == lexer->buffer_len) {
            lexer->buffer = realloc(lexer->buffer, lexer->buffer_len + BUFFER_INC_SIZE);
            lexer->buffer_len += BUFFER_INC_SIZE;

            if(lexer->buffer == NULL) {
                fprintf(stderr, "ERROR: Failed to increse size of word buffer\n");
                return (Instruction) { .type = CtlError };
            }
        }
    }
    lexer->buffer[buffer_index] = '\0';

    // Check for instruction type
    if(strcmp(lexer->buffer, ".\"") == 0) {
        skip_whitespace(lexer);

        if(reached_end(lexer)) {
            fprintf(stderr, "ERROR: Failed to find closing double-quote for string literal\n");
            return (Instruction) { .type = CtlError };
        }
 
        // Lex string literal
        buffer_index = 0;

        while(!reached_end(lexer) && peek(lexer) != '"') {
            lexer->buffer[buffer_index++] = advance(lexer);
        
            // Increase size of word buffer if we're running out of space
            if(buffer_index + 1 == lexer->buffer_len) {
                lexer->buffer = realloc(lexer->buffer, lexer->buffer_len + BUFFER_INC_SIZE);
                lexer->buffer_len += BUFFER_INC_SIZE;

                if(lexer->buffer == NULL) {
                    fprintf(stderr, "ERROR: Failed to increse size of word buffer\n");
                    return (Instruction) { .type = CtlError };
                }
            }
        }
        lexer->buffer[buffer_index] = '\0';

        if(reached_end(lexer)) {
            fprintf(stderr, "ERROR: Failed to find closing double-quote for string literal\n");
            return (Instruction) { .type = CtlError };
        }

        advance(lexer); // Skip trailing double-quote

        char* str_val = malloc(buffer_index + 1);

        if(str_val == NULL) {
            fprintf(stderr, "ERROR: Failed to allocate memory from string value buffer\n");
            return (Instruction) { .type = CtlError };
        }
        strcpy(str_val, lexer->buffer);

        return (Instruction) { .type = InsPrintStr, .str_val = str_val };
    } else if(is_number(lexer->buffer)) {
        return (Instruction) { .type = InsPush, .num_val = atoi(lexer->buffer) };
    } else {
        for(size_t i = 0; i < PAIR_AMOUNT; ++i) {
            if(strcmp(lexer->buffer, pairs[i].string) == 0) {
                return (Instruction) { .type = pairs[i].instruction };
            }
        }

        return (Instruction) { .type = InsIdentifier, .str_val = lexer->buffer };
    }
}
