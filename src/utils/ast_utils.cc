#include "ast_utils.h"
#include "ast_node.h"
#include "token.h"
#include "token_structures.h"
#include "error.h"
#include "parser.h"

#include <queue>
#include <stack>
#include <iostream>
#include <algorithm>

// Functions local to this unit
static void gen_queue(AST_NODE* p, std::queue<AST_NODE*>& terminals);
static void r_ast_out(AST_NODE* node, int depth);
static const char* op_name(int id);

void syntax_error(Error& err) {
    err.error = NLC_SYNTAX_ERROR;
    err.line = next_token.line_no;
    err.col = next_token.col_no;

    print_error(err);
}

// Simulate building the AST from the parse tree for expressions, 
// but only check for errors, see below function for details
bool is_st_valid(AST_NODE* root) {
    if (!root) return true;

    std::stack<AST_NODE*> children;
    std::queue<AST_NODE*> terminals;

    gen_queue(root, terminals);

    while (!terminals.empty()) {
        AST_NODE* curr = terminals.front();
        terminals.pop();

        if (curr->is_nop()) {
            children.push(curr); 
        } 
        // Unary operator, check for a single child of the correct type
        // a A -> A | L \ell -> L
        else if (curr->operator_is_unary()) { 
            if (children.empty()) {
                return false;
            }

            children.pop();
            children.push(curr);
        }
        // Check that for a binary operator, we have two children available 
        // A a A -> A | A r A -> L | (L | (A r A) \ell (L | (A r A)) -> L)
        else if (curr->operator_is_binary()) {
            if (children.empty()) {
                return false;
            }
            children.pop();

            if (children.empty()) {
                return false;
            }
            children.pop(); 

            children.push(curr);
        } 
    }

    // Children must end with at least one node (the root of the AST). 
    return children.size() == 1;
}

// Traverse parse tree bottom up, place terminals in queue
void gen_queue(AST_NODE* p, std::queue<AST_NODE*>& terminals) {
    if (!p) return;

    std::for_each(p->children.begin(), p->children.end(), [&terminals](auto& it)->void{
        gen_queue(it, terminals);
    });

    if (p->is_terminal()) {
        terminals.push(new AST_NODE(*p));
    }
}

// Elegantly converts a parse tree to an abstract syntax tree (for expressions only)
AST_NODE* pttoast(AST_NODE* root) {
    if (!root) return nullptr;

    AST_NODE* ast_root = nullptr;

    // Holds children in AST creation
    std::stack<AST_NODE*> children;

    // Holds terminals found in parse tree
    std::queue<AST_NODE*> terminals;

    // Fill the queue by doing a post order of the parse tree
    gen_queue(root,terminals);

    // Can't make a valid AST for the whole tree for whatever reason, 
    // through out the tree
    if (!is_st_valid(root)) {
        return nullptr;
    }

    // This can proceed with no errors thanks to the call above
    while (!terminals.empty()) {
        AST_NODE* curr = terminals.front();
        terminals.pop();

        // Not an operator, although if integer would also work
        // add to the stack
        if (curr->is_nop()) {
            children.push(curr); 
        } 
        // Unary operator, grab a single child
        else if (curr->operator_is_unary()) {
            AST_NODE* right = children.top(); children.pop();
            curr->add_child(right);

            // Throw this node onto the stack
            children.push(curr);
        }
        // Binary operator, attach stack top to right, stack top-1 to left
        else if (curr->operator_is_binary()){
            AST_NODE* right = children.top(); children.pop();
            AST_NODE* left = children.top(); children.pop();
            
            curr->add_child(left);
            curr->add_child(right);

            // Throw this node onto the stack
            children.push(curr);

        } 
        // Track top most node, which is the root
        ast_root = curr;
    }

    return assign_types(ast_root) == TYPE::TYPE_MISMATCH ? nullptr : ast_root;
}

AST_NODE* type_compliance(AST_NODE* parent, AST_NODE* left, AST_NODE* right) {
    AST_NODE* offender{};

    if (parent->operator_is_binary_arithmetic()) {
        AST_NODE* offender = 
            binary_arithmetic_type_compliance(left, right);

    } else if (parent->operator_is_unary_arithmetic()) {
        AST_NODE* offender = 
            unary_arithmetic_type_compliance(left);

    } else if (parent->operator_is_binary_relational()) {
        AST_NODE* offender = 
            binary_relational_type_compliance(left, right);

    } else if (parent->operator_is_binary_logical()) {
        AST_NODE* offender = 
            binary_logical_type_compliance(left, right);

    } else if (parent->operator_is_unary_logical()) {
        AST_NODE* offender = 
            unary_logical_type_compliance(left);
    }

    return offender;
}

TYPE assign_types(AST_NODE* root) {
    if (!root) return TYPE::null;

    TYPE here_type = root->data_type;

    auto child = root->children.begin();

    TYPE left_type, right_type;

    AST_NODE* left{}, *right{};

    if (child != root->children.end() && *child) left = *child;
    ++child;
    if (child != root->children.end() && *child) right = *child;

    left_type = assign_types(left);
    right_type = assign_types(right);

    if (left_type == TYPE::TYPE_MISMATCH || right_type == TYPE::TYPE_MISMATCH)  {
        return TYPE::TYPE_MISMATCH;
    }

    if (root->is_operator) {
        AST_NODE* offender = type_compliance(root, left, right);
        if (offender) {
            set_print_token_error(Error{}, offender->token, NLC_UNACCEPTABLE_TYPE_MISMATCH);
            return TYPE::TYPE_MISMATCH;
        }

        if (root->operator_is_arithmetic()) {
            root->data_type = left_type > right_type ? left_type : right_type;
        } else if (root->operator_is_relational()) {
            root->data_type = TYPE::BOOL;
        } else if (root->operator_is_logical()) {
            root->data_type = TYPE::BOOL;
        }

        return root->data_type;
    }

    return here_type;
}

void ast_preorder(AST_NODE* root) {
    if (!root) return;

    std::cout << (root->token.id != -1 ? TOKEN_STRUCTURES::token_names[root->token.id] : "Empty") << ", ";
    std::for_each(root->children.begin(), root->children.end(), [](auto& it) -> void {
        ast_preorder(it);
    });
}

void ast_inorder(AST_NODE* root) {
    if (!root) return;

    std::for_each(root->children.begin(), root->children.end(), [&root](auto& it) -> void {
            std::cout << (root->token.id != -1 ? TOKEN_STRUCTURES::token_names[root->token.id] : "Empty") << ", ";
        ast_inorder(it);
    });

}
void ast_postorder(AST_NODE* root) {
    if (!root) return;

    std::for_each(root->children.begin(), root->children.end(), [](auto& it) -> void {
        ast_postorder(it);
    });
    std::cout << (root->token.id != -1 ? root->token.lexeme : "Empty") << ", ";

}

const char* op_name(int id) {
    auto m_find = TOKEN_STRUCTURES::operator_names.find(id);
    auto end = TOKEN_STRUCTURES::operator_names.end();

    return m_find != end ? m_find->second.data() : "unknown";
}

void r_ast_out(AST_NODE* node, int depth) {
    if (!node) return;

    // indentation (2 spaces per level)
    for (int i = 0; i < depth; ++i)
        std::cout << "  ";

    // Integer node
    if (node->token.id == TOKEN_INTEGER || node->token.id == TOKEN_STRING) {
        std::cout << node->token.lexeme << "\n";
    } 
    else if (node->is_boolean == true) {
        std::cout << std::boolalpha << node->boolean << '\n'; 
    } else if (node->token.id == TOKEN_IDENT && !is_reserved(node->token)) {
        std::cout << "Var: " << node->token.identifier << '\n';
    }
    // Operator node
    else if (node->is_op()) {
        std::cout << node->token.lexeme
                  << " (" << op_name(node->token.id) << ")"
                  << "\n";
    } else if (node->is_statement()) {
        std::cout << node->str_node_type() << '\n';
    } 
    // Other internal nodes (if any) (useful for debugging, this should not hit)
    else {
        std::cout << "?\n";
    }

    // Recurse to children
    std::for_each(node->children.begin(), node->children.end(), [&depth](auto& it) -> void {
        r_ast_out(it, depth+1);
    });
}

// Initiate output
void ast_out(AST_NODE* root) {
    if (!root) return;

    std::cout << "Code tree:\n";
    r_ast_out(root, 0);
}
