#ifndef NLC_AST_STRUCTURES_H
#define NLC_AST_STRUCTURES_H

#include "types.h"

enum class NODE_TYPE : unsigned int {
    null, NOT, AND, OR,
    LESS, LEQ, GREATER,
    GEQ, EQ, NEQ,
    ADD, SUB, MULT, 
    DIV, MOD, EXP, 
    UPLUS, UNEG, DECL, 
    ASSIGN, PRINT, READ, 
    BLOCK, INT, VAR, STR, 
    BOOL, IF, ELSE, WHILE,
    FUNCTION, PARAMETER_LIST
};

enum class TYPE : unsigned int {
    null, INT, STRING, BOOL, TYPE_MISMATCH
};

extern const StatementNodes statement_nodes;
extern const NodeTypeMap<std::string> node_type_to_string_map;

#endif
