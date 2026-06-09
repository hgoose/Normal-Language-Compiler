#include "parser_structures.h"
#include "ast_structures.h"

AST_NODE* parse_print();
AST_NODE* parse_read();
AST_NODE* parse_decl_int4();
AST_NODE* parse_assign();
AST_NODE* parse_if();
AST_NODE* parse_else();
AST_NODE* parse_while();

bool evaluate_print(AST_NODE*);
bool init_var(AST_NODE*);
bool update_var(AST_NODE*);
bool process_read(AST_NODE*);
bool process_if(AST_NODE*);
bool process_while(AST_NODE*);

const ParseMap parse_map = {
    {"print", parse_print},
    {"read", parse_read},
    {"int4", parse_decl_int4},
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
