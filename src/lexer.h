#ifndef LEXER_H_
#define LEXER_H_

#include "instruction.h"

// Default size of word buffer
#define DEFAULT_BUFFER_LEN 64
// Size buffer should be increased by when it becomes too small
#define BUFFER_INC_SIZE 32

typedef struct {
    char* source;
    unsigned long source_len;

    char* buffer;
    unsigned long buffer_len;

    unsigned long index;
} Lexer;

Instruction collect_instruction(Lexer* lexer);

#endif // LEXER_H
