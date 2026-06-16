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
    FUNCTION, PARAMETER_PACK,
    FUNCTION_IDENT, RETURN_VALUE, CALL,
    ARGUMENT_PACK, RETURN, FOR, FOR_INIT,
    FOR_COND, FOR_UPDATE
};

enum class TYPE : unsigned int {
    null, INT, STRING, BOOL, TYPE_MISMATCH
};

extern const StatementNodes statement_nodes;
extern const NodeTypeMap<std::string> node_type_to_string_map;
extern const InvTypeMap<std::string> inv_type_map;
extern const TypeSet assignable_types;
extern const InvTypeMap<std::size_t> type_to_size;

size_t get_type_size(TYPE);

#endif
