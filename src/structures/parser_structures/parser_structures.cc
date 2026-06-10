#include "parser_structures.h"
#include "ast_structures.h"

StatementReturns parse_print();
StatementReturns parse_read();
StatementReturns parse_decl_int4();
StatementReturns parse_assign();
StatementReturns parse_if();
StatementReturns parse_else();
StatementReturns parse_while();

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
