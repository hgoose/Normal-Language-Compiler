#ifndef NLC_AST_STRUCTURES_H
#define NLC_AST_STRUCTURES_H

#include "types.h"

extern const ReservedWords reserved_words;

enum class NODE_TYPE : unsigned int {
    null, NOT, AND, OR,
    LESS, LEQ, GREATER,
    GEQ, EQ, NEQ,
    ADD, SUB, MULT, 
    DIV, MOD, EXP, 
    UPLUS, UNEG, DECL, 
    ASSIGN, PRINT, READ, 
    BLOCK, INT, VAR, STR, 
    BOOL, IF, ELSE, WHILE
};

enum class TYPE : unsigned int {
    null, INT4, STRING, BOOL, TYPE_MISMATCH
};

#endif
