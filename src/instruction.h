#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

typedef enum {
    // Stack operations
    InsPush,
    InsDup,
    InsDrop,
    InsSwap,
    InsOver,
    InsRot,

    // Input/output
    InsPrint,
    InsPrintStr,
    InsPrintVar,
    InsKey,
    InsEmit,
    InsCr,

    // Variable/alloc instructions
    InsVariable,
    InsStore,
    InsFetch,
    InsInc,
    InsConstant,
    InsAllot,
    InsCells,

    // Control flow
    InsIf,
    InsElse,
    InsThen,
    InsDo,
    InsLoop,
    InsBegin,
    InsUntil,

    // Math
    InsPlus,
    InsMinus,
    InsMult,
    InsDiv,
    InsMod,

    // Binary operators
    InsAnd,
    InsOr,
    InsInvert,

    // Comparisons
    InsEq,
    InsLess,
    InsGreater,

    // Definitions
    InsDefine,
    InsEndDef,

    // Control instructions
    CtlEof,
    CtlError,
} InstructionType;

typedef struct {
    InstructionType type;

    union {
        struct { char* str_val; };
        struct { long num_val; };
    };
} Instruction;

typedef struct {
    const char* string;
    InstructionType instruction;
} InstructionStringPair;

#define PAIR_AMOUNT 37

static const InstructionStringPair pairs[] = {
    { "dup", InsDup },
    { "drop", InsDrop },
    { "swap", InsSwap },
    { "over", InsOver },
    { "rot", InsRot },

    { ".", InsPrint },
    { "?", InsPrintVar },
    { "key", InsKey },
    { "emit", InsEmit },
    { "cr", InsCr },

    { "variable", InsVariable },
    { "!", InsStore },
    { "@", InsFetch },
    { "+!", InsInc },
    { "constant", InsConstant },
    { "allot", InsAllot },
    { "cells", InsCells },

    { "if", InsIf },
    { "else", InsElse },
    { "then", InsThen },
    { "do", InsDo },
    { "loop", InsLoop },
    { "begin", InsBegin },
    { "until", InsUntil },

    { "+", InsPlus },
    { "-", InsMinus },
    { "*", InsMult },
    { "/", InsDiv },
    { "mod", InsMod },

    { "and", InsAnd },
    { "or", InsOr },
    { "invert", InsInvert },

    { "=", InsEq },
    { "<", InsLess },
    { ">", InsGreater },

    { "define", InsDefine },
    { ";", InsEndDef },
};

#endif
