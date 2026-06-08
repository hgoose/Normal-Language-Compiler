// Nate warner 
// CS 515
// Assignment 3

#ifndef PARSER_H
#define PARSER_H

#include "ast_node.h"
#include "error.h"

// Lookahead token
extern Token next_token;

// INITIALIZATION
Error parser_init(const char* src_code);

// PARSING AND GENERATING ASTS
int parse();

AST_NODE* parse_print();
AST_NODE* parse_read();
AST_NODE* parse_decl_int4();
AST_NODE* parse_assign();
AST_NODE* parse_if();
AST_NODE* parse_else();
AST_NODE* parse_while();

// PRODUCTION RULES
AST_NODE* A(Error& err);
AST_NODE* AP(Error& err);
AST_NODE* B(Error& err);
AST_NODE* BP(Error& err);
AST_NODE* C(Error& err);
AST_NODE* D(Error& err);
AST_NODE* DP(Error& err);
AST_NODE* E(Error& err);
AST_NODE* EP(Error& err);
AST_NODE* T(Error& err);
AST_NODE* TP(Error& err);
AST_NODE* N(Error& err);
AST_NODE* F(Error& err);
AST_NODE* FP(Error& err);
AST_NODE* S(Error& err);

// CLEANUP
void free_tree(AST_NODE*& p);
void parser_cleanup();

#endif
