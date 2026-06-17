#include "parserutils.h"
#include "error.h"
#include "parser.h"
#include "lex.h"
#include "types.h"
#include "token.h"
#include "ast_node.h"
#include "error.h"
#include "ast_utils.h"
#include "scope_stack.h"
#include "ast_structures.h"
#include "lexstate.h"

#include <algorithm>
#include <iostream>
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

void skip_fn(int lbrace_count) {
    onepast_semi_or_block(lbrace_count);
}

void skip_for(int lbrace_count) {
    onepast_semi_or_block(lbrace_count);
}

// If the next token is invalid or a lexer error occurred,
// skip to start of next statement or eof.
bool skip_if_lexerr(const Error& err, MoveProcedure mv_proc, int lbrace_count){
    if (handle_lex_error(err)) {
        mv_proc(lbrace_count);
        return true;
    } 
    return false;
}

// Anything other than the expected token is an unexpected.
// Return true to indicate that the next_token is unexpected.
bool unexpected_token(TokenValue expected_token, ErrorValue error_to_submit, MoveProcedure mv_proc, int lbrace_count){
    if (next_token.id != expected_token) {
        set_print_token_error(Error{}, error_to_submit);
        mv_proc(lbrace_count);
        return true;
    }
    return false;
}

bool unexpected_token(const Token& token, TokenValue expected_token, ErrorValue error_to_submit, MoveProcedure mv_proc, int lbrace_count){
    if (next_token.id != expected_token) {
        set_print_token_error(Error{}, token, error_to_submit);
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

    for (AST_NODE*& child : p->children) {
        free_tree(child);
    }

    // if (p->owns_scope_stack_frame()) {
    //     for (SYMINFO*& symbol : p->scope_stack_frame) {
    //         if (!symbol) continue;
    //
    //         delete symbol;
    //         symbol = nullptr;
    //     }
    //
    //     p->scope_stack_frame.clear();
    // }

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

StatementReturns process_loop_statements(Error& err) {
    StatementReturns all_statements{};
    if (!next_token.is_lbrace()) {
        return get_statement();
    }

    Error lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_for, LBRACE_COUNT_ONE)) {
        return {};
    }

    // Empty block ({})
    if (next_token.is_rbrace()) {
        get_next_token_and_print_error();
        return {};
    }

    // Process all statements in block. Note that the block is 
    // strictly non-empty thanks to the check above
    for(;;) {
        StatementReturns statements = get_statement();
        if (statements.size()) merge_statement_returns(all_statements, statements);

        // Bad statements inside a while loop could be detrimental. 
        // For example, it could cause an infinite loop.
        // Therefore, if a bad statement is encountered inside a loop,
        // we eat the remainder of the structure.
        else {
            skip_for(LBRACE_COUNT_ONE);
            err = Error{NLC_BAD};
            free_statement_list(all_statements);
            return {};
        }

        // At the rbrace that terminates the while loop.
        // Move one past it.
        if (next_token.is_rbrace()) {
            get_next_token_and_print_error();
            break;
        } 

        // Structure was never terminated
        else if (next_token.is_eof()) {
            set_print_token_error(err, NLC_UNEXPECTED_EOF);
            free_statement_list(all_statements);
            return {};
        }
    }

    return {all_statements};
}

StatementReturns get_all_statements_in_block(Error& err) {
    StatementReturns all_statements{};
    for (;;) {
        StatementReturns statements = get_statement();
        merge_statement_returns(all_statements, statements);

        // At the rbrace that terminates the while loop.
        // Move one past it.
        if (next_token.is_rbrace()) {
            get_next_token_and_print_error();
            break;
        } 

        // Structure was never terminated
        else if (next_token.is_eof()) {
            set_print_token_error(err, NLC_UNEXPECTED_EOF);
            free_statement_list(all_statements);
            return {};
        }
    }

    return all_statements;
}

// Free all nodes created for the AST and parse tree
void parser_cleanup() {
    lex_cleanup();
}

AST_NODE* try_expression(bool eat_semicolon) {
    Error err{};
    AST_NODE* unknown = A(err);
    AST_NODE* ast = pttoast(unknown);
    free_trees(unknown);

    if (!ast) {
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        return nullptr;
    }

    if (next_token.is_comma()) {
        get_next_token_and_print_error();
        return nullptr;
    }

    if (ENFORCE_TERMINATING_SEMICOLON) {
        if (unexpected_token(prev_token, TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
            get_next_token_and_print_error();
            return nullptr;
        }

        // Eat semicolon
        get_next_token_and_print_error();
    }

    return ast;
}

AST_NODE* get_initial_value() {
    Error err{};
    AST_NODE* expr = A(err);
    AST_NODE* ast_expr = pttoast(expr);
    free_trees(expr);

    return ast_expr;
}

// Creates an AST for an assignment. We copy the nodes here
// so that we can free the returned tree after use and not 
// free the original variable or expression trees.
AST_NODE* create_assign(AST_NODE* var_node, AST_NODE* expression) {
    AST_NODE* var_copy = new AST_NODE(*var_node, COPY_CHILDREN);
    AST_NODE* expression_copy = new AST_NODE(*expression, COPY_CHILDREN);

    AST_NODE* assign_root = new AST_NODE(NODE_TYPE::ASSIGN, var_copy->syminfo->scope_level);

    assign_root->add_children(var_copy, expression_copy);

    return assign_root;
}

void merge_statement_returns(StatementReturns& dest, StatementReturns src) {
    while (src.size()) {
        AST_NODE* statement = src.front();
        src.pop_front();

        dest.push_back(statement);
    }
}

Error munch() {
    prev_token = next_token;
    return get_token(next_token);
}

void munch_all_semicolons() {
    while (next_token.is_semicolon()) {
        munch();
    }
}

AST_NODE* get_parameter() {
    if (next_token.is_not_type()){
        set_print_token_error(Error{}, NLC_EXPECTED_PARAMETER_TYPE);
        skip_fn(LBRACE_COUNT_ZERO);

        return nullptr;
    }

    AST_NODE* var = new AST_NODE(
        Token{}, NODE_TYPE::VAR, 
        next_token.get_type(), Scope::level()
    );

    Error lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        free_trees(var);
        return {};
    }

    if (next_token.is_not_ident() || next_token.is_ident_reserved()) {
        set_print_token_error(Error{}, NLC_INVALID_IDENTIFIER);
        skip_fn(LBRACE_COUNT_ZERO);
        free_trees(var);

        return nullptr;
    }

    var->set_token(next_token);

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        free_trees(var);
        return nullptr;
    }

    return var;
}

AST_NODE* get_parameter_pack() {
    AST_NODE* parameter_list = new AST_NODE(
        Token{}, NODE_TYPE::PARAMETER_PACK,
        Scope::level()
    );

    Error lex_err{};

    // Empty parameter list
    if (next_token.is_rparen()) {
        get_next_token_and_print_error();
        return parameter_list;
    }

    for (;;) {
        AST_NODE* var = get_parameter();

        if (!var) {
            skip_fn(LBRACE_COUNT_ZERO);
            free_trees(parameter_list);
            return nullptr;
        }

        parameter_list->add_children(var);
        
        if (!next_token.is_comma()) {
            break;
        }

        lex_err = munch();
        if (skip_if_lexerr(lex_err, skip_fn)) {
            free_trees(parameter_list);
            return nullptr;
        }
    }

    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR, skip_fn)) {
        free_trees(parameter_list); 
        return nullptr;
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        free_trees(parameter_list);
        return nullptr;
    }

    return parameter_list;
}

AST_NODE* get_argument() {
    Error expr_err{};
    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);
    free_trees(expr);

    return ast_expr;
}

AST_NODE* get_argument_pack() {
    AST_NODE* arg_pack = new AST_NODE(
        Token{},
        NODE_TYPE::ARGUMENT_PACK,
        Scope::level()
    );

    Error lex_err{};

    if (next_token.is_rparen()) {
        get_next_token_and_print_error();
        return arg_pack;
    }

    for (;;) {
        AST_NODE* arg = get_argument();

        if (!arg) {
            onepast_semi_or_block(LBRACE_COUNT_ZERO);
            free_trees(arg_pack);
            return nullptr;
        }

        // Sets all nodes in this argument expression
        // as function arg nodes.
        set_all_as_fn_arg(arg);

        arg_pack->add_children(arg);

        if (!next_token.is_comma()) break;

        lex_err = munch();
        if (skip_if_lexerr(lex_err)) {
            free_trees(arg_pack);
            return nullptr;
        }
    }

    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR)) {
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_trees(arg_pack);
        return nullptr;
    }

    get_next_token_and_print_error();

    return arg_pack;
}

bool match_packs(AST_NODE* parameter_pack, AST_NODE* argument_pack) {
    if (parameter_pack->children.size() != argument_pack->children.size())
        return false;

    auto argstart = argument_pack->children.begin();
    for (auto pstart = parameter_pack->children.begin(); pstart != parameter_pack->children.end(); ++pstart, ++argstart) {
        if ((*pstart)->data_type != (*argstart)->data_type) return false;
    }

    return true;
}

void stack_locals_layout(const SymbolBucket& frame, const AST_NODE* ppack) {
    if (frame.size() == ppack->children.size()) return;

    auto syminfo = frame.begin();
    for (auto& parameter : ppack->children) syminfo++;

    int rbp{};
    for (; syminfo != frame.end(); ++syminfo) {
        std::size_t type_size = get_type_size((*syminfo)->data_type);
        (*syminfo)->location.stack_offset = rbp - type_size;
        rbp = (*syminfo)->location.stack_offset;
    }
}

bool verify_init_or_assign(const StatementReturns& statements) {
    if (statements.front()->node_type != NODE_TYPE::DECL 
        && statements.front()->node_type != NODE_TYPE::ASSIGN
    ) {
        return false;
    }

    return true;
}

Token tpeek() {
    Token token;
    LexState state = lex_save();
    get_token(token);
    lex_goto_last_save(state);

    return token;
}

void free_statement_list(const StatementReturns& statements) {
    for (auto& statement : statements) {
        free_trees(statement);
    }
}

void free_parameter_pack_symbols(AST_NODE* ppack) {
    for (AST_NODE*& parameter : ppack->children) {
        if (parameter->syminfo) {
            SYMTABLE::remove_symbol(parameter->syminfo); 
            delete parameter->syminfo;
            parameter->syminfo = nullptr;
        }
    }
}
