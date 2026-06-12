#include "parser_structures.h"
#include "ast_structures.h"
#include "parser.h"

bool evaluate_print(AST_NODE*);
bool init_var(AST_NODE*);
bool update_var(AST_NODE*);
bool process_read(AST_NODE*);
bool process_if(AST_NODE*);
bool process_while(AST_NODE*);

const ParseMap parse_map = {
    {"print", parse_print},
    {"read", parse_read},
    {"int", parse_decl_int},
    {"if", parse_if},
    {"while", parse_while}
};

const EvalMap eval_map = {
    {NODE_TYPE::PRINT, evaluate_print},
    {NODE_TYPE::DECL, init_var},
    {NODE_TYPE::ASSIGN, update_var},
    {NODE_TYPE::READ, process_read},
    {NODE_TYPE::IF, process_if},
    {NODE_TYPE::WHILE, process_while}
};
