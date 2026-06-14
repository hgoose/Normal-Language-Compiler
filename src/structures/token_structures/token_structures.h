#ifndef NLC_TOKEN_STRUCTURES
#define NLC_TOKEN_STRUCTURES

#include "types.h"

namespace TOKEN_STRUCTURES {
    extern const ReservedWords reserved_words;
    extern const TokenMap<std::string> operator_names;
    extern const TokenMap<NODE_TYPE> node_types;
    extern const IdentMap<bool> booleans;
    extern const TokenMap<std::string> token_name_map;
    extern const TokenNames token_names;
    extern const Types types;
    extern const TokenSet terminals; 
    extern const TokenSet data_terminals;
    extern const TokenSet non_data_terminals;
    extern const TokenSet before_uplus_or_uneg;
    extern const TokenSet operators;
    extern const TokenSet binary_arithmetic_operators;
    extern const TokenSet unary_arithmetic_operators;
    extern const TokenSet binary_relational_operators;
    extern const TokenSet binary_logical_operators;
    extern const TokenSet unary_logical_operators;
    extern const TypeMap<std::string> type_map;
};

#endif
