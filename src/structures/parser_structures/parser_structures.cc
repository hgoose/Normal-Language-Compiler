#include "ast_structures.h"
#include "types.h"
#include "parser.h"
#include "tree_eval.h"
#include "parser_structures.h"

const ParseMap parse_map = {
    {"print", parse_print},
    {"read", parse_read},
    {"int", parse_decl_int},
    {"if", parse_if},
    {"while", parse_while},
    {"{", parse_block}, 
    {"fn", parse_fn},
    {"proc", parse_fn},
    {"procedure", parse_fn},
    {"call", parse_call},
    {"return", parse_return},
    {"for", parse_for}
};
