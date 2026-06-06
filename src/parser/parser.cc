// ELEGANT PARSING
// Nate Warner z2004109
// Assignment 4
 
/* EXPRESSION SPEC
 *
    CFG:
        A &\to BA^{\prime} \\
        A^{\prime} &\to \text{or } BA^{\prime} \mid \varepsilon \\
        B &\to CB^{\prime} \\
        B^{\prime} &\to \text{and }CB^{\prime} \mid \varepsilon \\
        C &\to\; \sim C \mid D\\
        D &\to ED^{\prime} \\
        D^{\prime} &\to ==E \mid \ne E \mid <E \mid \leq E \mid > E \mid \geq E \mid \varepsilon\\
        E &\to TE^{\prime} \\
        E^{\prime} &\to +TE^{\prime} \ \mid \ -TE^{\prime} \ \mid \ \varepsilon \\
        T &\to NT^{\prime} \\
        T^{\prime} &\to *NT^{\prime} \ \mid \ /NT^{\prime} \ \mid \ \text{ MOD } NT^{\prime} \ \mid \ \varepsilon \\
        N &\to \oplus N \ \mid \ \neg N \ \mid \ F \\
        F &\to SF^{\prime} \\
        F^{\prime} &\to \land SF^{\prime} \ \mid \ \varepsilon\\
        S &\to (A) \ \mid \ \text{int} \ \mid \ \text{var} \mid \text{true} \mid \text{false} \mid \text{string}

    FIRST:
        \text{FIRST}(S) &= \{(,\text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(F^{\prime}) &= \{\land, \varepsilon\} \\
        \text{FIRST}(F) &=  \text{FIRST}(S) = \{(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(N) &= \{\oplus,\neg\} \cup \text{FIRST}(F) = \left\{\oplus, \neg\right\} \cup \text{FIRST}(S) \\
        &= \{\oplus,\neg,(,\text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(T^{\prime}) &= \{*, /, \text{MOD}, \varepsilon\} \\
        \text{FIRST}(T) &= \text{FIRST}(N) = \{\oplus,\neg,(,\text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(E^{\prime}) &= \{+,-,\epsilon\} \\
        \text{FIRST}(E) &=\text{FIRST}(T) = \{\oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(D^{\prime}) &= \left\{==,\ne,<, \leq,>, \geq,\varepsilon\right\} \\
        \text{FIRST}(D) &= \text{FIRST}(E) = \{\oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(C) &= \left\{\sim\right\} \cup \text{FIRST}(D) = \{\sim, \oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(B^{\prime}) &= \left\{\text{and}, \varepsilon\right\} \\
        \text{FIRST}(B) &= \text{FIRST}(C) = \{\sim, \oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(A^{\prime}) &= \left\{\text{or}, \varepsilon\right\} \\
        \text{FIRST}(A) &= \text{FIRST}(B) =\{\sim, \oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} 

    FOLLOW: 
        \text{FOLLOW}(A) &= \left\{), \$\right\} \\
        \text{FOLLOW}(A^{\prime}) &= \text{FOLLOW}(A) = \left\{), \$\right\} \\
        \text{FOLLOW}(B) &= \text{FIRST}(A^{\prime}) - \left\{\varepsilon\right\} \cup \text{FOLLOW}(A) = \left\{\text{or}, ), \$\right\} \\
        \text{FOLLOW}(B^{\prime}) &= \text{FOLLOW}(B) = \left\{\text{or}, ), \$\right\} \\
        \text{FOLLOW}(C) &= \text{FIRST}(B^{\prime}) - \left\{\varepsilon\right\}\cup \text{FOLLOW}(B^{\prime}) \cup \text{FOLLOW}(B) = \left\{\text{and}, \text{or}, ), \$\right\} \\
        \text{FOLLOW}(D) &= \text{FOLLOW}(C) = \left\{\text{and}, \text{or}, ), \$\right\} \\
        \text{FOLLOW}(D^{\prime}) &= \text{FOLLOW}(D) = \left\{\text{and}, \text{or}, ), \$\right\} \\
        \text{FOLLOW}(E) &= \text{FIRST}(D^{\prime}) \cup \text{FOLLOW}(D) \cup \text{FOLLOW}(D^{\prime}) \\
        &= \left\{==, \ne, <, \leq, >, \geq, \text{and}, \text{or}, ), \$\right\} \\
        \text{FOLLOW}(E^{\prime}) &= \text{FOLLOW}(E) \\
        &= \left\{==, \ne, <, \leq, >, \geq, \text{and}, \text{or}, ), \$\right\} \\
        \text{FOLLOW}(T) &= \left\{+, -, ==, \ne, <, \leq, >, \geq, \text{and}, \text{or}, ), \$\right\} \\ 
        \text{FOLLOW}(T^{\prime}) &= \left\{+, -, ==, \ne, <, \leq, >, \geq, \text{and}, \text{or}, ), \$\right\} \\ 
        \text{FOLLOW}(N) &= \left\{*, /, \text{MOD}, +, -, ==, \ne, <, \leq, >, \geq, \text{and}, \text{or}, ), \$\right\} \\ 
        \text{FOLLOW}(F) &= \left\{*, /, \text{MOD}, +, -, ==, \ne, <, \leq, >, \geq, \text{and}, \text{or}, ), \$\right\} \\ 
        \text{FOLLOW}(F^{\prime}) &= \left\{*, /, \text{MOD}, +, -, ==, \ne, <, \leq, >, \geq, \text{and}, \text{or}, ), \$\right\} \\ 
        \text{FOLLOW}(S) &= \left\{\land, *, /, \text{MOD}, +, -, ==, \ne, <, \leq, >, \geq, \text{and}, \text{or}, ), \$\right\} 

    FIRST of sequences:
        \text{FIRST}(\varepsilon) &= \left\{\varepsilon\right\}  \\
        \text{FIRST}(BA^{\prime}) &= \text{FIRST}(B) = \{\sim, \oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(\text{or} BA^{\prime}) &= \left\{\text{or}\right\} \\
        \text{FIRST}(CB^{\prime}) &= \text{FIRST}(C) = \{\sim, \oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(\text{and}CB^{\prime}) &= \left\{\text{and}\right\} \\
        \text{FIRST}(\sim C) &= \left\{\sim\right\} \\
        \text{FIRST}(D) &= \{\oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(ED^{\prime}) &= \text{FIRST}(E) =  \{\oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\} \\
        \text{FIRST}(==E) &= \left\{==\right\} \\
        \text{FIRST}(\ne E) &= \left\{\ne\right\} \\
        \text{FIRST}(<E) &= \left\{<\right\} \\
        \text{FIRST}(\leq E) &= \left\{\leq\right\} \\
        \text{FIRST}(>E) &= \left\{>\right\} \\
        \text{FIRST}(\geq E) &= \left\{\geq\right\} \\
        \text{FIRST}(TE^{\prime}) &= \{\oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\}\\
        \text{FIRST}(+TE^{\prime}) &= \{+\}\\
        \text{FIRST}(-TE^{\prime}) &= \{-\}\\
        \text{FIRST}(NT^{\prime}) &= \{\oplus,\neg,(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\}\\
        \text{FIRST}(*NT^{\prime}) &= \{*\}\\
        \text{FIRST}(/NT^{\prime}) &= \{/\}\\
        \text{FIRST}(\text{MOD}NT^{\prime}) &= \{\text{MOD}\}\\
        \text{FIRST}(\oplus N) &= \{\oplus\}\\
        \text{FIRST}(\neg N) &= \{\neg\} \\
        \text{FIRST}(SF^{\prime}) &= \{(, \text{int}, \text{var}, \text{true}, \text{false}, \text{string}\}\\
        \text{FIRST}(\land SF^{\prime}) &= \{\land\}\\
        \text{FIRST}((A)) &= \{(\}\\
        \text{FIRST}(\text{int}) &= \{\text{int}\}\\
        \text{FIRST}(\text{var}) &= \{\text{var}\} \\
        \text{FIRST}(\text{true}) &= \{\text{true}\} \\
        \text{FIRST}(\text{false}) &= \{\text{false}\} \\
        \text{FIRST}(\text{string}) &= \{\text{string}\}

    Recall the selection rule (considering current level A \to \alpha_1 | ... | \alpha_k)
        \begin{enumerate}
            \item If $a \in \text{FIRST}(\alpha_{i}) $, choose the production $\alpha_{i}$, otherwise
            \item If $\varepsilon \in \text{FIRST}(\alpha_{k})$, and $a \in \text{FOLLOW}(A)$, choose $\alpha_{k}$,
            \item Otherwise, syntax error.
        \end{enumerate}
*/


#include <algorithm>
#include <iostream>

#include "parser.h"
#include "error.h"
#include "lex.h"
#include "ast_node.h"
#include "ast_utils.h"
#include "tree_eval.h"
#include "codegen.h"
#include "ncc_integers.h"
#include "symtable.h"
#include "util.h"

Token next_token;

static void inhouse_cleanup(AST_NODE*& parse_tree);

// Check if the current token is invalid
static bool invalid_lookahead() {
    return next_token.id != TOKEN_INTEGER && next_token.id != TOKEN_UNEG    && 
        next_token.id != TOKEN_UPLUS   && next_token.id != TOKEN_PLUS    && 
        next_token.id != TOKEN_MINUS   && next_token.id != TOKEN_MULT    && 
        next_token.id != TOKEN_DIV     && next_token.id != TOKEN_MOD     && 
        next_token.id != TOKEN_EXP     && next_token.id != TOKEN_LPAREN  && 
        next_token.id != TOKEN_RPAREN && next_token.id != TOKEN_NULL && 
        next_token.id != TOKEN_EOF && next_token.id != TOKEN_IDENT &&
        next_token.id != TOKEN_COMMA && next_token.id != TOKEN_SEMICOLON &&
        next_token.id != TOKEN_ASSIGN && next_token.id != TOKEN_STRING &&
        next_token.id != TOKEN_OR && next_token.id != TOKEN_AND && 
        next_token.id != TOKEN_NOT && next_token.id != TOKEN_LESS &&
        next_token.id != TOKEN_LESS_EQ && next_token.id != TOKEN_GREATER &&
        next_token.id != TOKEN_GREATER_EQ && next_token.id != TOKEN_EQUAL &&
        next_token.id != TOKEN_NOT_EQUAL && next_token.id != TOKEN_LBRACE  &&
        next_token.id != TOKEN_RBRACE;
}

// Initialize the parser. 
Error parser_init(const char* src_code) {
    Error err = lex_init(src_code);
    return err;
}

int parse() {
    if (pspace_init() == -1) {
        std::cerr << "Error requesting address space\n";
        return -1;
    }

    AST_NODE* program_tree = new AST_NODE();
    program_tree->node_type = NODE_TYPE::BLOCK;

    Error e;

    get_token(next_token);

    for (;;) {
        if (next_token.id == TOKEN_IDENT) {
            AST_NODE* statement_root{};
            // Print statement
            if (next_token.identifier == "print") {
                statement_root = parse_print();
            } 
            // Read statement
            else if (next_token.identifier == "read") {
                statement_root = parse_read();
            } 
            // Variable declaration
            else if (next_token.identifier == "int4") {
                statement_root = parse_decl_int4();
            } else if (next_token.identifier == "if") {
                statement_root = parse_if();
                if (statement_root) {
                }
            } else if (next_token.identifier == "while") {
                statement_root = parse_while();
                if (statement_root) {
                }
            }
            // Variable assignment? 
            else {
                statement_root = parse_assign();
            }

            if (statement_root) {
                program_tree->add_children(statement_root);
            }

            // If and while end at the start of the next statement
            if (statement_root && statement_root->node_type != NODE_TYPE::IF && statement_root->node_type != NODE_TYPE::WHILE) {
                if (invalid_lookahead() || handle_lex_error(get_token(next_token))) {
                    goto_next_semicolon(); 
                }
            }

            if (!statement_root) {
                if (next_token.id == TOKEN_SEMICOLON) {
                    invalid_lookahead();
                    handle_lex_error(get_token(next_token));
                }
            }
        } 

        if (next_token.id == TOKEN_EOF) break;
    }

    x86_pushr64(REGISTER::R12);
    x86_pushr64(REGISTER::R15);

    std::for_each(program_tree->children.begin(), program_tree->children.end(), [](AST_NODE* statement) -> void {
        if (!statement) return;

        if (statement->node_type == NODE_TYPE::PRINT) {
            evaluate_print(statement);
        } else if (statement->node_type == NODE_TYPE::DECL) {
            init_var(statement);
        } else if (statement->node_type == NODE_TYPE::ASSIGN) {
            update_var(statement);
        } else if (statement->node_type == NODE_TYPE::READ) {
            process_read(statement);
        } else if (statement->node_type == NODE_TYPE::IF) {
            process_if(statement);
        } else if (statement->node_type == NODE_TYPE::WHILE) {
            process_while(statement);
        }
    });

    ast_out(program_tree);

    std::cout << "Code size: " << byte_count << " bytes.\n";
    std::cout << "Code execution: \n";

    int res = x86_exec();

    if (pspace_reclaim() == -1) {
        std::cerr << "Did not succeed in reclaiming allocated program space\n";
        return -1;
    }

    free_tree(program_tree);

    return 0;
}

AST_NODE* parse_print() {
    Error lex_error{}, expr_error{};

    AST_NODE* print_root = new AST_NODE();
    print_root->node_type = NODE_TYPE::PRINT;
    print_root->token = next_token;

    // Lexer error
    lex_error = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_error)) {
        goto_next_semicolon();
        free_tree(print_root);
        return nullptr;
    }

    // Missing ( after print
    if (next_token.id != TOKEN_LPAREN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        goto_next_semicolon();
        free_tree(print_root);
        return nullptr;
    }
    
    // Lexer error
    lex_error = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_error)) {
        goto_next_semicolon();
        free_tree(print_root);
        return nullptr;
    }

    // Empty print
    if (next_token.id == TOKEN_RPAREN) {
        set_print_token_error(Error{}, NCC_EXPECTED_EXPRESSION);
        goto_next_semicolon();
        free_tree(print_root);
        return nullptr;
    }

    // Process all expressions, add as children to print node
    AST_NODE* expr = A(expr_error);
    AST_NODE* ast_expr = pttoast(expr);

    free_tree(expr);

    // Toss out the print statement if the expression does not form a 
    // syntactically and semantically valid AST
    if (!ast_expr) {
        goto_next_semicolon();
        free_tree(print_root);
        return nullptr;
    }

    print_root->add_child(ast_expr);

    while (next_token.id == TOKEN_COMMA) {
        lex_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_error)) {
            goto_next_semicolon();
            free_tree(print_root);
            return nullptr;
        }

        // Comma with no expression
        if (next_token.id == TOKEN_RPAREN) {
            set_print_token_error(Error{}, NCC_EXPECTED_EXPRESSION);
            goto_next_semicolon();
            free_tree(print_root);
            return nullptr;
        }

        expr = A(expr_error);
        ast_expr = pttoast(expr);
        free_tree(expr);

        if (!ast_expr) {
            goto_next_semicolon();
            free_tree(print_root);
            return nullptr;
        }
        print_root->add_child(ast_expr);
    }

    // Missing ) after expressions
    if (next_token.id != TOKEN_RPAREN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        goto_next_semicolon();
        free_tree(print_root);
        return nullptr;
    }

    // Lexer error
    lex_error = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_error)) {
        goto_next_semicolon();
        free_tree(print_root);
        return nullptr;
    }

    // Missing semicolon after )
    if (next_token.id != TOKEN_SEMICOLON) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        // goto_next_semicolon();
        free_tree(print_root);
        return nullptr;
    }

    return print_root;
}

AST_NODE* parse_read() {
    AST_NODE* read_root = new AST_NODE();

    read_root->node_type = NODE_TYPE::READ;
    read_root->token = next_token;

    Error lex_err{};

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(read_root);
        return nullptr;
    }

    if (next_token.id != TOKEN_LPAREN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        goto_next_semicolon();
        free_tree(read_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(read_root);
        return nullptr;
    }

    if (next_token.id == TOKEN_RPAREN || next_token.id != TOKEN_IDENT) {
        set_print_token_error(Error{}, NCC_EXPECTED_VAR);
        goto_next_semicolon();
        free_tree(read_root);
        return nullptr;
    }

    AST_NODE* var_node = new AST_NODE(next_token);
    var_node->symbol_type = SYMTYPE::VAR;
    var_node->node_type = NODE_TYPE::VAR;

    read_root->add_children(var_node);

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(read_root);
        free_tree(var_node);
        return nullptr;
    }

    if (next_token.id != TOKEN_RPAREN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        goto_next_semicolon();
        free_tree(read_root);
        free_tree(var_node);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(read_root);
        free_tree(var_node);
        return nullptr;
    }

    if (next_token.id != TOKEN_SEMICOLON) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        // goto_next_semicolon();
        free_tree(read_root);
        free_tree(var_node);
        return nullptr;
    }

    return read_root;
}

AST_NODE* parse_decl_int4() {
    Error lex_err{};

    AST_NODE* declare_root = new AST_NODE();
    declare_root->node_type = NODE_TYPE::DECL;
    declare_root->token = next_token;

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(declare_root);
        return nullptr;
    }

    // Variable name must be an identifier token
    if (next_token.id != TOKEN_IDENT) {
        set_print_token_error(Error{}, NCC_INVALID_IDENTIFIER);
        goto_next_semicolon();
        free_tree(declare_root);
        return nullptr;
    }

    // Variable name is reserved
    if (is_reserved(next_token)) {
        set_print_token_error(Error{}, NCC_VARIABLE_NAME_RESERVED);
        goto_next_semicolon();
        free_tree(declare_root);
        return nullptr;
    }

    // Put into symbol table 
    SYMINFO* entry = SYMTABLE::add_symbol(SYMINFO(next_token.identifier, TYPE::INT4, SYMTYPE::VAR));
    if (!entry) {
        set_print_token_error(Error{}, NCC_SYMBOL_ALREADY_EXISTS);
        goto_next_semicolon();
        free_tree(declare_root);
        return nullptr; 
    }

    AST_NODE* var = new AST_NODE(next_token);
    var->syminfo = entry;

    declare_root->add_children(var);

    // Look for semicolon
    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(declare_root);
        free_tree(var);
        return nullptr;
    }

    if (next_token.id != TOKEN_SEMICOLON) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        // goto_next_semicolon();
        free_tree(declare_root);
        free_tree(var);
        return nullptr;
    }

    return declare_root;
}

AST_NODE* parse_assign() {
    AST_NODE* assign_root = new AST_NODE();
    assign_root->node_type = NODE_TYPE::ASSIGN;

    Error lex_err{}, expr_err{};

    AST_NODE* var_node = new AST_NODE();
    var_node->token = next_token;
    var_node->node_type = NODE_TYPE::VAR;
    var_node->symbol_type = SYMTYPE::VAR;

    assign_root->add_children(var_node);

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(assign_root);
        return nullptr;
    }

    if (next_token.id != TOKEN_ASSIGN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        goto_next_semicolon();
        free_tree(assign_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(assign_root);
        return nullptr;
    }

    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);

    free_tree(expr);

    if (!ast_expr) {
        goto_next_semicolon();
        free_tree(assign_root);
        return nullptr;
    }

    assign_root->add_children(ast_expr);

    if (next_token.id != TOKEN_SEMICOLON) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        // goto_next_semicolon();
        free_tree(assign_root);
        return nullptr;
    }

    return assign_root;
}

// Statement list
AST_NODE* get_statement() {

    if (next_token.identifier == "print") {
        return parse_print();
    } else if (next_token.identifier == "read") {
        return parse_read();
    } else if (next_token.identifier == "int4") {
        return parse_decl_int4();
    } else if (next_token.identifier == "if") {
        return parse_if();
    } else if (next_token.identifier == "while") {
        return parse_while();
    } 

    return parse_assign();
}

AST_NODE* parse_else() {
    AST_NODE* else_root = new AST_NODE();
    else_root->node_type = NODE_TYPE::ELSE;
    else_root->token = next_token;

    Error lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        goto_next_semicolon();
        free_tree(else_root);
        return nullptr;
    }

    bool block = next_token.id == TOKEN_LBRACE ? true : false;
    bool empty{};
    if (block) {
        lex_err = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_err)) {
            goto_next_semicolon();
            free_tree(else_root);
            return nullptr;
        }

        if (next_token.id == TOKEN_RBRACE) {
            empty = true;
        }
    }

    bool broken_noblock{};
    bool dont_move{};
    if (!empty) {
        do {
            dont_move = false;
            AST_NODE* statement = get_statement();
            if (statement) {
                else_root->add_child(statement);
            }

            if (statement->node_type == NODE_TYPE::IF || 
                    statement->node_type == NODE_TYPE::WHILE
            ) {
                dont_move = true;
            }

            if (!block && !statement) {
                broken_noblock = true;
            }

            if (block) {
                if (next_token.id == TOKEN_RBRACE) {
                    break;
                }

                if (!dont_move) {
                    lex_err = get_token(next_token);
                    if (invalid_lookahead() || handle_lex_error(lex_err)) {
                        goto_next_semicolon();
                        free_tree(else_root);
                        return nullptr;
                    }
                }

                if (next_token.id == TOKEN_EOF) {
                    goto_next_semicolon();
                    set_print_token_error(Error{}, NCC_UNEXPECTED_EOF); 
                    free_tree(else_root);
                    return nullptr;
                }

                if (next_token.id == TOKEN_RBRACE) {
                    break;
                }
            }
        } while (block);

    }

    if (!broken_noblock && !dont_move || block) {
        lex_err = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_err)) {
            goto_next_semicolon();
            free_tree(else_root);
            return nullptr;
        }
    }

    return else_root;
}

AST_NODE* parse_if() {
    AST_NODE* if_root = new AST_NODE(); 
    if_root->token = next_token;
    if_root->node_type = NODE_TYPE::IF;

    Error lex_err{}, expr_err{};

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        skip_if();
        free_tree(if_root);
        return nullptr;
    }

    if (next_token.id != TOKEN_LPAREN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        skip_if();
        free_tree(if_root);
    }

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        skip_if();
        free_tree(if_root);
        return nullptr;
    }

    if (next_token.id == TOKEN_RPAREN) {
        skip_if();
        set_print_token_error(Error{}, NCC_EXPECTED_EXPRESSION);
        free_tree(if_root);
        return nullptr;
    }

    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);
    free_tree(expr);

    if (!ast_expr) {
        skip_if();
        free_tree(ast_expr);
        free_tree(if_root);
        return nullptr;
    }

    if (ast_expr->data_type != TYPE::BOOL) {
        set_print_token_error(Error{}, ast_expr->token, NCC_NON_LOGICAL_CONDITION);
        skip_if();
        free_tree(ast_expr);
        free_tree(if_root);
        return nullptr;
    }

    if_root->add_child(ast_expr);

    if (next_token.id != TOKEN_RPAREN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        skip_if();
        free_tree(if_root);
    }

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        skip_if();
        free_tree(if_root);
        return nullptr;
    }

    if (next_token.id == TOKEN_SEMICOLON) {
        lex_err = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_err)) {
            skip_if();
            free_tree(if_root);
            return nullptr;
        }
        return if_root;
    }

    bool block = next_token.id == TOKEN_LBRACE ? true : false;
    if (block) {
        lex_err = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_err)) {
            skip_if();
            free_tree(if_root);
            return nullptr;
        }
    }

    bool empty{};
    if (next_token.id == TOKEN_RBRACE || next_token.identifier == "else") {
        empty = true;
    }

    bool dont_move{};
    if (!empty) {
        do {
            dont_move = false;
            AST_NODE* statement = get_statement();
            if (statement) {
                if_root->add_child(statement);
            } 

            if (next_token.identifier == "else" || 
                    statement->node_type == NODE_TYPE::IF || 
                    statement->node_type == NODE_TYPE::WHILE
            ) {
                dont_move = true;
            }

            if (block) {
                if (next_token.id == TOKEN_RBRACE) {
                    break;
                }

                if (!dont_move) {
                    lex_err = get_token(next_token);
                    if (invalid_lookahead() || handle_lex_error(lex_err)) {
                        skip_if();
                        free_tree(if_root);
                        return nullptr;
                    }
                }

                if (next_token.id == TOKEN_EOF) {
                    skip_if();
                    set_print_token_error(Error{}, NCC_UNEXPECTED_EOF); 
                    free_tree(if_root);
                    return nullptr;
                }

                if (next_token.id == TOKEN_RBRACE) break;
            } 

        } while (block);

    }

    if (!empty && !dont_move || block) {
        lex_err = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_err)) {
            skip_if();
            free_tree(if_root);
            return nullptr;
        }
    }

    bool has_else = next_token.identifier == "else" ? true : false;
    if (has_else) {
        AST_NODE* else_root = parse_else();
        if (else_root) {
            if_root->add_child(else_root);
        }
    } 

    return if_root;
}

AST_NODE* parse_while() {
    AST_NODE* while_root = new AST_NODE();
    while_root->node_type = NODE_TYPE::WHILE;

    Error lex_err{}, expr_err{};

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        skip_while();
        free_tree(while_root);
        return nullptr;
    }

    if (next_token.id != TOKEN_LPAREN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        skip_while();
        free_tree(while_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        skip_while();
        free_tree(while_root);
        return nullptr;
    }

    if (next_token.id == TOKEN_RPAREN) {
        skip_while();
        set_print_token_error(Error{}, NCC_EXPECTED_EXPRESSION);
        free_tree(while_root);
        return nullptr;
    }

    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);
    free_tree(expr);

    if (!ast_expr) {
        skip_while();
        free_tree(ast_expr);
        free_tree(while_root);
        return nullptr;
    }

    if (ast_expr->data_type != TYPE::BOOL) {
        set_print_token_error(Error{}, ast_expr->token, NCC_NON_LOGICAL_CONDITION);
        skip_while();
        free_tree(ast_expr);
        free_tree(while_root);
        return nullptr;
    }

    while_root->add_child(ast_expr);

    if (next_token.id != TOKEN_RPAREN) {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        skip_while();
        free_tree(while_root);
    }

    lex_err = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(lex_err)) {
        skip_while();
        free_tree(while_root);
        return nullptr;
    }

    // Explicitly empty while 
    if (next_token.id == TOKEN_SEMICOLON) {
        lex_err = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_err)) {
            skip_while();
            free_tree(while_root);
            return nullptr;
        }
        return while_root;
    }

    bool block = next_token.id == TOKEN_LBRACE ? true : false;
    if (block) {
        lex_err = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_err)) {
            skip_while();
            free_tree(while_root);
            return nullptr;
        }
    }

    bool empty{};
    if (next_token.id == TOKEN_RBRACE) {
        empty = true;
    }

    bool dont_move{};
    if (!empty) do {
        dont_move = false;
        AST_NODE* statement = get_statement();
        if (statement) {
            while_root->add_child(statement);
        }

        if (statement->node_type == NODE_TYPE::IF || 
                statement->node_type == NODE_TYPE::WHILE
        ){
            dont_move = true;
        }

        if (block) {
            if (next_token.id == TOKEN_RBRACE) {
                break;
            }

            if (!dont_move) {
                lex_err = get_token(next_token);
                if (invalid_lookahead() || handle_lex_error(lex_err)) {
                    skip_while();
                    free_tree(while_root);
                    return nullptr;
                }
            }

            if (next_token.id == TOKEN_EOF) {
                skip_while();
                set_print_token_error(Error{}, NCC_UNEXPECTED_EOF); 
                free_tree(while_root);
                return nullptr;
            }

            if (next_token.id == TOKEN_RBRACE) break;
        }

    } while (block);

    if (!empty && !dont_move || block)  {
        lex_err = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(lex_err)) {
            skip_while();
            free_tree(while_root);
            return nullptr;
        }
    }

    return while_root;
}

// A -> BA'
AST_NODE* A(Error& err) {
    AST_NODE* here = new AST_NODE();
    AST_NODE* left{}, *right{};

    // FIRST(BA')
    if (next_token.id == TOKEN_NOT 
            || next_token.id == TOKEN_UPLUS
            || next_token.id == TOKEN_UNEG
            || next_token.id == TOKEN_LPAREN
            || next_token.id == TOKEN_INTEGER
            || next_token.id == TOKEN_STRING
            || next_token.id == TOKEN_IDENT // true, false, or var
    ) {
        left = B(err);
        right = AP(err);
    } else {
        free_tree(here);
        return nullptr;
    }

    here->add_children(left, right);

    return here;
}

// A' -> or BA' | \varepsilon
AST_NODE* AP(Error& err) {
    AST_NODE* here{};
    AST_NODE* left{}, *right{};

    // FIRST(or BA')
    if (next_token.id == TOKEN_OR) {
        here = new AST_NODE(next_token, NODE_TYPE::OR, OPERATOR);

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            return nullptr;
        }

        left = B(err);
        right = AP(err);
    } 
    // \epsilon \in FIRST(\varepsilon), so check if next_token \in FOLLOW(AP). 
    else {
        return nullptr;
    }


    here->add_children(left, right);
    return here;
}

// B -> CB'
AST_NODE* B(Error& err) {
    AST_NODE* here = new AST_NODE();
    AST_NODE* left{}, *right{};

    // Consider FIRST(CB')
    if (next_token.id == TOKEN_NOT |
        next_token.id == TOKEN_UNEG |
        next_token.id == TOKEN_LPAREN |
        next_token.id == TOKEN_INTEGER |
        next_token.id == TOKEN_IDENT |
        next_token.id == TOKEN_STRING
    ) {
        left = C(err);
        right = BP(err);
    } else {
        free_tree(here);
        return nullptr;
    }


    here->add_children(left, right);
    return here;
}

// B' -> and CB' | \varepsilon
AST_NODE* BP(Error& err) {
    AST_NODE* here{};
    AST_NODE* left{}, *right{};

    // Consider FIRST(and CB')
    if (next_token.id == TOKEN_AND) {
        here = new AST_NODE(next_token, NODE_TYPE::AND, OPERATOR);

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            return nullptr;
        }

        left = C(err);
        right = BP(err);
    } else {
        return nullptr;
    }


    here->add_children(left, right);
    return here;
}

// C -> ~C | D
AST_NODE* C(Error& err) {
    AST_NODE* here = new AST_NODE();
    AST_NODE* left{};

    // Consider FIRST(~C) 
    if (next_token.id == TOKEN_NOT) {
        // Take ~C
        here = new AST_NODE(next_token, NODE_TYPE::NOT, OPERATOR);

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            return nullptr;
        }

        left = C(err);
    }
    // Consider FIRST(D)
    else if (next_token.id == TOKEN_UPLUS ||
            next_token.id == TOKEN_UNEG ||
            next_token.id == TOKEN_LPAREN ||
            next_token.id == TOKEN_INTEGER ||
            next_token.id == TOKEN_IDENT ||
            next_token.id == TOKEN_STRING
    ) {
        // Take D
        left = D(err);

    } else {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }


    here->add_children(left);
    return here;
}

// D -> ED'
// FIRST(ED') = {uplus, uneg, (, int, var, true, false, string}
AST_NODE* D(Error& err) {
    AST_NODE* here = new AST_NODE();
    AST_NODE* left{}, *right{};

    // Consider FIRST(ED')
    if (next_token.id == TOKEN_UPLUS ||
        next_token.id == TOKEN_UNEG ||
        next_token.id == TOKEN_LPAREN ||
        next_token.id == TOKEN_INTEGER ||
        next_token.id == TOKEN_IDENT ||
        next_token.id == TOKEN_STRING
    ) {
        // Take ED'
        left = E(err);
        right = DP(err);
    } else {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    here->add_children(left, right);
    return here;
}

// D' -> ==E | \ne E | <E | <=E | >E | >=E | \varepsilon
// FIRST(==) = {==}
// FIRST(\ne) = {\ne}
// FIRST(<) = {<}
// FIRST(<=) = {<=}
// FIRST(>) = {>}
// FIRST(>=) = {>=}
AST_NODE* DP(Error& err) {
    AST_NODE* here{};
    AST_NODE* left{};

    // Consider these various first sets
    if (next_token.id == TOKEN_EQUAL ||
        next_token.id == TOKEN_NOT_EQUAL ||
        next_token.id == TOKEN_GREATER ||
        next_token.id == TOKEN_GREATER_EQ ||
        next_token.id == TOKEN_LESS ||
        next_token.id == TOKEN_LESS_EQ

    ) {
        // Take E and consume the token
        here = new AST_NODE(next_token, get_node_type(next_token), OPERATOR);

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            goto_next_semicolon();
            err = tmp_error;
            return nullptr;
        }
    }
    // Otherwise annihilate the node
    else {
        return nullptr;
    }
    
    left = E(err);
    here->add_children(left);
    return here;
}

// E -> TE'
AST_NODE* E(Error& err) {
    AST_NODE* left = nullptr;

    // t \in FIRST(TE'). Removed E' for this fold operation, left folds
    // +,- so that these operations are not right associative
    if (next_token.id == TOKEN_UPLUS || next_token.id == TOKEN_UNEG ||
        next_token.id == TOKEN_LPAREN || next_token.id == TOKEN_INTEGER || 
        next_token.id == TOKEN_STRING || next_token.id == TOKEN_IDENT) {
        
        // Get lhs sub tree
        left = T(err);

        // Simulates the job of E', but makes +,- left associative
        while (next_token.id == TOKEN_PLUS || next_token.id == TOKEN_MINUS) {
            // Save current token
            Token op = next_token;

            // Get next token
            Error tmp_err = get_token(next_token);
            if (invalid_lookahead() || 
            	handle_lex_error(tmp_err)
            ) {
                err = tmp_err;
            	return nullptr;
            }

            // Create rhs subtree
            AST_NODE* rhs = T(err);

            // Here node
            AST_NODE* node = new AST_NODE();

            // Has previous token as its token (the operator)
            node->token = op;
            node->is_operator = true;
            if (op.id == TOKEN_PLUS) {
                node->node_type = NODE_TYPE::ADD;
            } else {
                node->node_type = NODE_TYPE::SUB;
            }

            node->add_children(left, rhs);

            left = node;
        }
        return left;
    } else {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
    }

    return nullptr;
}

AST_NODE* T(Error& err) {
    AST_NODE* left = nullptr;

    // Left folds *,/,mod, since the grammar I have makes these operations right associative
    if (next_token.id == TOKEN_UPLUS || next_token.id == TOKEN_UNEG ||
        next_token.id == TOKEN_LPAREN || next_token.id == TOKEN_INTEGER ||
        next_token.id == TOKEN_STRING || next_token.id == TOKEN_IDENT) {

        left = N(err);

        // Left fold *,/,mod, same idea as for +,-
        while (next_token.id == TOKEN_MULT ||
               next_token.id == TOKEN_DIV  ||
               next_token.id == TOKEN_MOD) {
            Token op = next_token;

            Error tmp_err = get_token(next_token);
            if (invalid_lookahead() || 
            	handle_lex_error(tmp_err)
            ) {
                err = tmp_err;
            	return nullptr;
            }

            AST_NODE* rhs = N(err);

            AST_NODE* node = new AST_NODE();
            node->token = op;
            node->is_operator = true;

            if (op.id == TOKEN_MULT) {
                node->node_type = NODE_TYPE::MULT;
            } else if (op.id == TOKEN_DIV){
                node->node_type = NODE_TYPE::DIV;
            } else {
                node->node_type = NODE_TYPE::MOD;
            }

            node->add_children(left, rhs);

            left = node;
        }
        return left;
    } else {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
    }

    return nullptr;
}


AST_NODE* N(Error& err) {
    AST_NODE* here = new AST_NODE();
	AST_NODE* left{}, *right{};

    // Consider FIRST(-F), FIRST(+F), and FIRST(F). No nullable productions.
    // No production rule is nullable, don't need to consider FOLLOW(N).
    
    // t \in FIRST(\neg F) or t \in FIRST(\oplus F)
    if (next_token.id == TOKEN_UPLUS || next_token.id == TOKEN_UNEG) {
        here->token = next_token;
        here->is_operator = true;

        if (next_token.id == TOKEN_UPLUS) {
            here->node_type = NODE_TYPE::UPLUS;
        } else {
            here->node_type = NODE_TYPE::UNEG;
        }

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || 
        	handle_lex_error(tmp_error)
        ) {
            err = tmp_error;
            free_tree(here);
        	return nullptr;
        }
    
        left = F(err);
    } 
    // t \in FIRST(F)
    else if (next_token.id == TOKEN_LPAREN 
            || next_token.id == TOKEN_INTEGER 
            || next_token.id == TOKEN_STRING
            || next_token.id == TOKEN_IDENT
    ) {
        left = F(err);
    } else {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    here->add_child(left);
    return here;
}

AST_NODE* F(Error& err) {
    // Consider FIRST(SF'), not nullable
    AST_NODE* here = new AST_NODE();
	AST_NODE* left{}, *right{};

    // t \in FIRST(SF')
    if (next_token.id == TOKEN_LPAREN 
        || next_token.id == TOKEN_INTEGER 
        || next_token.id == TOKEN_STRING 
        || next_token.id == TOKEN_IDENT
    ) {
        left = S(err);
        right = FP(err);
    } else {
        set_print_token_error(Error{}, NCC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    here->add_children(left, right);
    return here;
}

AST_NODE* FP(Error& err) {
    AST_NODE* here = new AST_NODE();
	AST_NODE* left{}, *right{};

    // Consider FIRST(\land SF') or FIRST(\varepsilon)
    if (next_token.id == TOKEN_EXP) {
        // Consume exponentiation
        here->token = next_token;
        here->node_type = NODE_TYPE::EXP;
        here->is_operator = true;

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || 
        	handle_lex_error(tmp_error)
        ) {
            err = tmp_error;
            free_tree(here);
        	return nullptr;
        }

        // Take F' -> ^SF'
        left = S(err);
        right = FP(err);
    } 
    // F' -> \varepsilon nullable, so consider FOLLOW(F') = FOLLOW(N). If
    // t \in FOLLOW(F'), annihilate this node (take F' \to \varepsilon)
    else {
        free_tree(here);
        return nullptr;
    }
    // else if (next_token.id == TOKEN_MULT
    //         || next_token.id == TOKEN_DIV
    //         || next_token.id == TOKEN_MOD
    //         || next_token.id == TOKEN_PLUS
    //         || next_token.id == TOKEN_MINUS
    //         || next_token.id == TOKEN_EOF
    //         || next_token.id == TOKEN_RPAREN
    // ) {
    //     free_tree(here);
    //     return nullptr;
    // } else {
    //     free_tree(here);
    //     return nullptr;
    // }

    here->add_children(left, right);
    return here;
}

AST_NODE* S(Error& err) {

    AST_NODE* here = new AST_NODE();
	AST_NODE* left{};

    // Consider FIRST((A)), FIRST(int), nothing nullable
    
    // t \in FIRST(int), token must be TOKEN_INTEGER, nothing to call.
    if (next_token.id == TOKEN_INTEGER) {
        here->token = next_token;
        here->node_type = NODE_TYPE::INT;
        here->data_type = TYPE::INT4;

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || 
        	handle_lex_error(tmp_error)
        ) {
            err = tmp_error;
            free_tree(here);
        	return nullptr;
        }
    // Token is LPAREN, so we choose S -> (E), make sure to eat the parenthesis
    } else if (next_token.id == TOKEN_LPAREN) {
        // Eat lparen
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || 
        	handle_lex_error(tmp_error)
        ) {
            err = tmp_error;
            free_tree(here);
        	return nullptr;
        }

        left = A(err);

        // If next token is not ), error
        if (next_token.id != TOKEN_RPAREN) {
            err.error = NCC_EXPECTED_RPAREN;
            err.line = next_token.line_no;
            err.col = next_token.col_no;
            print_error(err);

        } 
        // Otherwise eat the )
        else {
            Error tmp_error = get_token(next_token);
            if (invalid_lookahead() || 
            	handle_lex_error(tmp_error)
            ) {
                err = tmp_error;
                free_tree(here);
            	return nullptr;
            }
        }
    } else if (next_token.id == TOKEN_STRING) {
        here->token = next_token;
        here->node_type = NODE_TYPE::STR;
        here->data_type = TYPE::STRING;

        STR_TABLE_ENTRY entry = STR_TABLE::add_string(next_token.str);

        // Attempted to overrun the internal string table
        if (entry.vi == INVALID) {
            free_tree(here);
            return nullptr;
        }

        here->entry = entry;

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || 
        	handle_lex_error(tmp_error)
        ) {
            err = tmp_error;
            free_tree(here);
        	return nullptr;
        }
    } 
    // Variable
    else if (next_token.id == TOKEN_IDENT) {
        // Could be true or false
        if (next_token.identifier == "true" ||
            next_token.identifier == "false"
        ) {
            here = new AST_NODE(next_token, NODE_TYPE::BOOL, TYPE::BOOL); 
            here->boolean = next_token.identifier == "true" 
                ? true 
                : false;
            here->is_boolean = true;

            Error tmp_error = get_token(next_token);
            if (invalid_lookahead() || 
                handle_lex_error(tmp_error)
            ) {
                err = tmp_error;
                free_tree(here);
                return nullptr;
            }
        } else {
            here->token = next_token;
            here->node_type = NODE_TYPE::VAR;

            // Search the symbol table
            SYMINFO* syminfo = SYMTABLE::get_symbol(next_token.identifier, SYMTYPE::VAR);

            if (!syminfo) {
                set_print_token_error(Error{}, NCC_UNKNOWN_VARIABLE);
                free_tree(here);
                return nullptr;
            }
            here->syminfo = syminfo;
            here->data_type = syminfo->data_type;

            Error tmp_error = get_token(next_token);
            if (invalid_lookahead() ||
                    handle_lex_error(tmp_error)
               ){
                err = tmp_error;
                free_tree(here);
                return nullptr;
            }
        }

    } else {
        free_tree(here);
        return nullptr;
    }

    if (left) here->add_child(left);
    return here;
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

// Free parse tree nodes
void inhouse_cleanup(AST_NODE*& parse_tree) {
    free_tree(parse_tree);
}

// Free all nodes created for the AST and parse tree
void parser_cleanup() {
    lex_cleanup();
}

// True if error is critical, false otherwise
bool handle_lex_error(const Error& err) {
    print_error(err);

    if (err.error != NCC_OK && err.error != NCC_EOF) {
        return true;
    }

    return false;
}
