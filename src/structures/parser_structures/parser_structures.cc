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
    {"call", parse_call}
};

const EvalMap eval_map = {
    {NODE_TYPE::PRINT, evaluate_print},
    {NODE_TYPE::DECL, init_var},
    {NODE_TYPE::ASSIGN, update_var},
    {NODE_TYPE::READ, process_read},
    {NODE_TYPE::IF, process_if},
    {NODE_TYPE::WHILE, process_while},
    {NODE_TYPE::FUNCTION, process_fn},
};
