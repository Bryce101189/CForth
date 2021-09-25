#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdbool.h>

#include "instruction.h"

typedef struct {
    Instruction* instructions;

    long* stack;
    unsigned long stack_size;
    unsigned long stack_index;

    bool has_printed;
} Interpreter;

bool reached_end(Interpreter* interpreter);
bool run_instruction(Interpreter* interpreter);
void debug_print_stack(Interpreter* interpreter);

#endif // INTERPRETER_H_
