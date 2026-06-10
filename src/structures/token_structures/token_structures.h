#ifndef NLC_TOKEN_STRUCTURES
#define NLC_TOKEN_STRUCTURES

#include "types.h"

namespace TOKEN_STRUCTURES {
    extern const TokenMap<std::string> operator_names;
    extern const TokenMap<NODE_TYPE> node_types;
    extern const IdentMap<bool> booleans;
    extern const TokenNames token_names;
    extern const Types types;
    extern const TokenSet terminals; 
    extern const TokenSet binary_arithmetic_operators;
    extern const TokenSet unary_arithmetic_operators;
    extern const TokenSet binary_relational_operators;
    extern const TokenSet binary_logical_operators;
    extern const TokenSet unary_logical_operators;
};

#endif
