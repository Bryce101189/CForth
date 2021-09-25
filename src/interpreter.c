#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "instruction.h"
#include "interpreter.h"
#include "util.h"

#define TRY_PUSH(val) if(!push(interpreter, val)) return false
#define TRY_POP(val) if(!pop(interpreter, val)) return false
#define TRY_POP_NTH(nth, val) if(!pop_nth(interpreter, nth, val)) return false
#define TRY_PEEK(val) if(!peek(interpreter, val)) return false
#define TRY_PEEK_NTH(nth, val) if(!peek_nth(interpreter, nth, val)) return false

bool reached_end(Interpreter* interpreter) {
    return interpreter->instructions->type == CtlEof;
}

static bool push(Interpreter* interpreter, long val) {
    // Increase stack size if needed
    if(interpreter->stack_size <= interpreter->stack_index + 1) {
        interpreter->stack_size = interpreter->stack_index + 1;
        interpreter->stack = realloc(interpreter->stack, interpreter->stack_size * sizeof(long*));

        if(interpreter->stack == NULL) {
            fprintf(stderr, "ERROR: Failed to reallocate interpreter stack\n");
            return false;
        }
    }

    interpreter->stack[interpreter->stack_index++] = val;
    return true;
}

static bool pop(Interpreter* interpreter, long* val) {
    if(interpreter->stack_index == 0) {
        fprintf(stderr, "ERROR: Stack underflow\n");
        return false;
    }

    if(val != NULL) {
        *val = interpreter->stack[--interpreter->stack_index];
    } else {
        --interpreter->stack_index;
    }
    return true;
}

static bool pop_nth(Interpreter* interpreter, long nth, long* val) {
    if(nth < 0) {
        nth += interpreter->stack_index;
    }

    if(nth < 0) {
        fprintf(stderr, "ERROR: Stack underflow\n");
        return false;
    } else if(nth >= (long)interpreter->stack_index) {
        fprintf(stderr, "ERROR: Stack overflow\n");
        return false;
    }

    if(val != NULL) {
        *val = interpreter->stack[nth];
    }

    // Shift preceding entries
    --interpreter->stack_index;
    for(long i = nth; i < (long)interpreter->stack_index; ++i) {
        interpreter->stack[i] = interpreter->stack[i + 1];
    }

    return true;
}

static bool peek(Interpreter* interpreter, long* val) {
    if(interpreter->stack_index == 0) {
        fprintf(stderr, "ERROR: Stack underflow\n");
        return false;
    }

    *val = interpreter->stack[interpreter->stack_index - 1];
    return true;
}

static bool peek_nth(Interpreter* interpreter, long nth, long* val) {
    if(nth < 0) {
        nth += interpreter->stack_index;
    }

    if(nth < 0) {
        fprintf(stderr, "ERROR: Stack underflow\n");
        return false;
    } else if(nth >= (long)interpreter->stack_index) {
        fprintf(stderr, "ERROR: Stack overflow\n");
        return false;
    }

    *val = interpreter->stack[nth];
    return true;
}

bool run_instruction(Interpreter* interpreter) {
    Instruction ins = *(interpreter->instructions);

    switch(ins.type) {
    // Stack operations
    case InsPush: {
        TRY_PUSH(ins.num_val);
        break;
    }
    case InsDup: {
        long val;
        TRY_PEEK(&val);
        TRY_PUSH(val);
        break;
    }
    case InsDrop: {
        TRY_POP(NULL);
        break;
    }
    case InsSwap: {
        long val;
        TRY_POP_NTH(-2, &val);
        TRY_PUSH(val);
        break;
    }
    case InsOver: {
        long val;
        TRY_PEEK_NTH(-2, &val);
        TRY_PUSH(val);
        break;
    }
    case InsRot: {
        long val;
        TRY_POP_NTH(0, &val);
        TRY_PUSH(val);
        break;
    }

    // Input/output
    case InsPrint: {
        long val;
        TRY_POP(&val);
        printf("%li ", val);
        interpreter->has_printed = true;
        break;
    }
    case InsPrintStr: {
        printf("%s ", ins.str_val);
        interpreter->has_printed = true;

        break;
    }

    // TODO: InsPrintVar
    
    case InsKey: {
        TRY_PUSH(getchar());
        interpreter->has_printed = true;
        flush_stdin();  // Consume extra chars
        break;
    }
    case InsEmit: {
        long val;
        TRY_POP(&val);
        printf("%c ", (char)val);
        interpreter->has_printed = true;
        break;
    }
    case InsCr: {
        printf("\n");
        interpreter->has_printed = false;  
        break;
    }

    // TODO: Variable/alloc instructions

    // Control flow
    case InsIf: {
        long val;
        TRY_POP(&val);
        if(val == 0) {
            do {
                ++interpreter->instructions;

                if(interpreter->instructions->type == CtlEof) {
                    fprintf(stderr, "ERROR: UNable to find closing 'else' or 'then' to 'if'\n");
                    return false;
                }
            } while(interpreter->instructions->type != InsElse && interpreter->instructions->type != InsThen);
        }
        break;
    }

    // Ignore
    case InsElse: break;
    case InsThen: break;

    // TODO: InsDo, InsLoop, InsBegin, InsUntil

    // Math
    case InsPlus: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 + val2);
        break;
    }
    case InsMinus: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 - val2);
        break;
    }
    case InsMult: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 * val2);
        break;
    }
    case InsDiv: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 / val2);
        break;
    }
    case InsMod: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 % val2);
        break;
    }

    // Binary operators
    case InsAnd: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 & val2);
        break;
    }
    case InsOr: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 | val2);
        break;
    }
    case InsInvert: {
        long val;
        TRY_POP(&val);
        TRY_PUSH(-val - 1);
        break;
    }

    case InsEq: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 == val2 ? -1 : 0);
        break;
    }
    case InsLess: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 < val2 ? -1 : 0);
        break;
    }
    case InsGreater: {
        long val1, val2;
        TRY_POP(&val2);
        TRY_POP(&val1);
        TRY_PUSH(val1 > val2 ? -1 : 0);
        break;
    }

    case InsIdentifier: {
        fprintf(stderr, "ERROR: Unrecognized word '%s' found\n", ins.str_val);
        return false;
    }

    default:
        fprintf(stderr, "ERROR: Unrecognized instruction #%lu found\n", (unsigned long)ins.type);
        return false;
    }

    ++interpreter->instructions;
    return true;
}

void debug_print_stack(Interpreter* interpreter) {
    printf("DEBUG: interpreter->stack = [");
    bool has_printed = false;

    for(unsigned int i = 0; i < interpreter->stack_index; ++i) {
        printf(has_printed ? ", %li" : "%li", interpreter->stack[i]);
        has_printed = true;
    }

    printf("]\n");
}
