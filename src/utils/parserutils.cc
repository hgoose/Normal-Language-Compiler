#include "parserutils.h"
#include "error.h"
#include "parser.h"
#include "lex.h"
#include "types.h"
#include "token.h"
#include "ast_node.h"
#include "error.h"
#include "ast_utils.h"

#include <string>
#include <cstdint>
#include <algorithm>
#include <functional>

// Skips to the next semicolon
void onepast_next_token(TokenValue token) {
    while (next_token.id != token && !next_token.is_eof()) {
        get_next_token_and_print_error();
    }
    get_next_token_and_print_error();
}

// Skips the block (ends one past closing rbrace)
void skip_block(int lbrace_count) {
    while (lbrace_count > 0) {
        get_next_token_and_print_error();

        if (next_token.is_eof()) break;

        if (next_token.is_rbrace()) {
            --lbrace_count;
        } else if (next_token.is_lbrace()) {
            ++lbrace_count;
        }
    }

    // At this point we are on an rbrace, so move past it.
    get_next_token_and_print_error();
}

// If called with skip_while, skips entirety of while.
// If called with skip_if, either skips entirety (if no else), 
// or to the else identifier.
// If called generically, either skips to the next semicolon, or if a
// lbrace is encounted before a semicolon, skips to the closing brace.
void onepast_semi_or_block(int lbrace_count) {
    // If we call skip_while or skip_if while inside of a block,
    // lbrace_count will be one. So, this call will jump one past the end 
    // of the block (terminating }). Then, we pass control back to caller. 
    if (lbrace_count != 0) {
        skip_block(lbrace_count);
        return;
    }

    // Find semicolon, lbrace, or eof
    while (!next_token.is_semicolon()
            && !next_token.is_lbrace()
            && !next_token.is_eof()
    ) {
        get_next_token_and_print_error();
    }

    if (next_token.is_eof()) return;

    if (next_token.is_semicolon()) {
        get_next_token_and_print_error();
        return;
    } 
    // Otherwise we are on a left brace
    else ++lbrace_count;

    // This will leave us one past the terminating right brace
    skip_block(lbrace_count);
}

// Skips entirety of if statement
void skip_if(int lbrace_count) {
    onepast_semi_or_block(lbrace_count);

    if (next_token.is_ident_else()) {
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
    }
}

// Skips entirety of while statement
void skip_while(int lbrace_count) {
    onepast_semi_or_block(lbrace_count);
}

void skip_else(int lbrace_count) {
    onepast_semi_or_block(lbrace_count);
}

// If the next token is invalid or a lexer error occurred,
// skip to start of next statement or eof.
bool skip_if_invalid_or_lexerr(const Error& err, MoveProcedure mv_proc, int lbrace_count){
    if (invalid_lookahead() || handle_lex_error(err)) {
        mv_proc(lbrace_count);
        return true;
    } 
    return false;
}

// Anything other than the expected token is an error.
// Return true to indicate that the next_token is unexpected.
bool unexpected_token(TokenValue expected_token, ErrorValue error_to_submit, MoveProcedure mv_proc, int lbrace_count){
    if (next_token.id != expected_token) {
        set_print_token_error(Error{}, error_to_submit);
        mv_proc(lbrace_count);
        return true;
    }
    return false;
}

// If the next_token is wrong_token, error.
// Return true to indicate that the next_token is wrong
bool wrong_next_token(TokenValue wrong_token, ErrorValue error_to_submit, MoveProcedure mv_proc, int lbrace_count){
    if (next_token.id == wrong_token) {
        set_print_token_error(Error{}, error_to_submit);
        mv_proc(lbrace_count);
        return true;
    }
    return false;
}

// Reclaim memory associated with tree nodes
void free_tree(AST_NODE*& p) {
    if (!p) return;

    std::for_each(p->children.begin(), p->children.end(), [](auto it) -> void {
        free_tree(it);
    });

    delete p;
    p=nullptr;
}

void free_statement_return_list(StatementReturns& returns) {
    while (returns.size()) {
        AST_NODE* front = returns.front();
        returns.pop_front();

        free_tree(front);
    }
}

// Free all nodes created for the AST and parse tree
void parser_cleanup() {
    lex_cleanup();
}

AST_NODE* try_expression() {
    Error err{};
    AST_NODE* unknown = A(err);
    AST_NODE* ast = pttoast(unknown);

    if (ast) {
        if (next_token.is_semicolon() || next_token.is_comma()) {
            get_next_token_and_print_error();
        }

        return ast;
    } 

    onepast_semi_or_block(LBRACE_COUNT_ZERO);
    return nullptr;
}

AST_NODE* get_initial_value() {
    Error err{};
    return pttoast(A(err));
}

// Creates an AST for an assignment. We copy the nodes here
// so that we can free the returned tree after use and not 
// free the original variable or expression trees.
AST_NODE* create_assign(AST_NODE* var_node, AST_NODE* expression) {
    AST_NODE* var_copy = new AST_NODE(*var_node, COPY_CHILDREN);
    AST_NODE* expression_copy = new AST_NODE(*expression, COPY_CHILDREN);

    AST_NODE* assign_root = new AST_NODE(NODE_TYPE::ASSIGN);

    assign_root->add_children(var_copy, expression_copy);

    return assign_root;
}
