#ifndef NLC_TREE_EVAL_H
#define NLC_TREE_EVAL_H

#include <cstddef>

extern int pushed;

struct AST_NODE;

void dispatch_statement(AST_NODE*);

void evaluate_expr(AST_NODE*);
void evaluate_print_expr(AST_NODE*);

bool evaluate_print(AST_NODE*);
bool init_var(AST_NODE*);
bool update_var(AST_NODE*);
bool process_read(AST_NODE*);
bool process_if(AST_NODE*);
bool process_while(AST_NODE*);
bool process_block(AST_NODE*);
bool process_fn(AST_NODE*);
bool process_call(AST_NODE*);

size_t sizeof_print();

void bind_function_parameters(AST_NODE* ppack);

#endif
