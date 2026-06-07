// Nate warner 
// CS 515
// Assignment 4

#ifndef TREE_EVAL_H
#define TREE_EVAL_H

#include <cstddef>

struct AST_NODE;

void evaluate_expr(AST_NODE*);
void evaluate_print_expr(AST_NODE*);
bool evaluate_print(AST_NODE*);
bool init_var(AST_NODE*);
bool update_var(AST_NODE*);
bool process_read(AST_NODE*);
bool process_if(AST_NODE*);
bool process_while(AST_NODE*);

size_t sizeof_print();

#endif
