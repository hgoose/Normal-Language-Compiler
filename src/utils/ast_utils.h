#ifndef NLC_AST_UTILS_H
#define NLC_AST_UTILS_H

#include "ast_node.h"

struct Error;

void syntax_error(Error& err);
bool is_st_valid(AST_NODE* root, bool accept_empty);

// Parse tree to ast
AST_NODE* pttoast(AST_NODE* root);
AST_NODE* type_compliance(AST_NODE* parent, AST_NODE* left, AST_NODE* right);
TYPE assign_types(AST_NODE*);

// Traversals
void ast_preorder(AST_NODE* root);
void ast_inorder(AST_NODE* root);
void ast_postorder(AST_NODE* root);

void ast_out(AST_NODE* node);
void clean_tree(AST_NODE*& root);

#endif
