#ifndef NLC_PARSER_H
#define NLC_PARSER_H

#include "token.h"

struct AST_NODE;
struct Error;

// Lookahead token
extern Token next_token;
extern Token prev_token;

extern bool SUPPRESS_PARSER_ERRORS;

// INITIALIZATION
Error parser_init(const char* src_code);

// PARSING AND GENERATING ASTS
int parse();

StatementReturns parse_print();
StatementReturns parse_read();
StatementReturns parse_decl_int();
StatementReturns parse_assign();
StatementReturns parse_if();
StatementReturns parse_else();
StatementReturns parse_while();

// PRODUCTION RULES
AST_NODE* A(Error&);
AST_NODE* AP(Error&);
AST_NODE* B(Error&);
AST_NODE* BP(Error&);
AST_NODE* C(Error&);
AST_NODE* D(Error&);
AST_NODE* DP(Error&);
AST_NODE* E(Error&);
AST_NODE* EP(Error&);
AST_NODE* T(Error&);
AST_NODE* TP(Error&);
AST_NODE* N(Error&);
AST_NODE* F(Error&);
AST_NODE* FP(Error&);
AST_NODE* S(Error&);

AST_NODE* integer_terminal(Error&);
AST_NODE* paren_expression(Error&, AST_NODE*&);
AST_NODE* string_terminal(Error&);
AST_NODE* boolean_terminal(Error&);
AST_NODE* variable_terminal(Error&);

// CLEANUP
void free_tree(AST_NODE*& p);
void parser_cleanup();

#endif
