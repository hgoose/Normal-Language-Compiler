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
#include <unordered_map>
#include <functional>

#include "parser.h"
#include "parser_structures.h"
#include "parserutils.h"
#include "util.h"
#include "lex.h"
#include "error.h"
#include "ast_node.h"
#include "ast_utils.h"
#include "tree_eval.h"
#include "codegen.h"
#include "nlc_integers.h"
#include "symtable.h"
#include "types.h"
#include "table_structures.h"
#include "sets.h"

Token next_token;

// Calls the correct function in parse_map based on the
// current identifier.
static AST_NODE* get_statement() {
    std::string ident = next_token.identifier;
    if (parse_map.find(ident) != parse_map.end()) {
        return parse_map.at(ident)();
    }

    return parse_assign();
}

Error parser_init(const char* src_code) {
    Error err = lex_init(src_code);
    return err;
}

// Parses the input program into an abstract syntax tree, then traverses
// the AST to write x86 bytes into a program buffer. After compilation into x86,
// we execute the program buffer.
//
// Returns: Zero on success, -1 on failure
int parse() {
    if (pspace_init() == -1) {
        std::cerr << "Error requesting address space\n";
        return -1;
    }

    AST_NODE* program_tree = new AST_NODE();
    program_tree->node_type = NODE_TYPE::BLOCK;

    get_token(next_token);

    for (;;) {
        if (next_token.is_ident()) {
            AST_NODE* statement_root{get_statement()};

            if (statement_root) {
                program_tree->add_children(statement_root);
            }
        } 

        if (next_token.is_eof()) break;
    }

    x86_pushr64(REGISTER::R12);
    x86_pushr64(REGISTER::R15);

    std::for_each(program_tree->children.begin(), program_tree->children.end(), [](AST_NODE*& statement) -> void {
        if (!statement) return;

        NODE_TYPE statement_type = statement->node_type;

        // Essentially a continue
        if (eval_map.find(statement_type) == eval_map.end()) return;

        bool success = eval_map.at(statement_type)(statement);

        if (!success) {
            free_tree(statement);
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

    lex_error = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_error)) {
        free_tree(print_root);
        return nullptr;
    }

    // Missing ( after print
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(print_root);
        return nullptr;
    }
    
    lex_error = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_error)) {
        free_tree(print_root);
        return nullptr;
    }

    // If we have an rparen here the print statement is empty, illegal.
    if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION)) {
        free_tree(print_root);
        return nullptr;
    }

    // Process all expressions, add as children to print node
    AST_NODE* expr = A(expr_error);
    AST_NODE* ast_expr = pttoast(expr);

    // Free the parse tree after converting to an AST
    free_tree(expr);

    // Toss out the print statement if the expression does not form a 
    // syntactically and semantically valid AST
    if (!ast_expr) {
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(print_root);
        return nullptr;
    }

    // Otherwise the expression is valid and we add it to the root
    print_root->add_child(ast_expr);

    // Process all subsequent expressions
    while (next_token.is(TOKEN_COMMA)) {
        lex_error = get_token(next_token);
        if (skip_if_invalid_or_lexerr(lex_error)) {
            free_tree(print_root);
            return nullptr;
        }

        // No expression following the comma
        if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION)) {
            free_tree(print_root);
            return nullptr;
        }

        expr = A(expr_error);
        ast_expr = pttoast(expr);
        free_tree(expr);

        if (!ast_expr) {
            onepast_semi_or_block(LBRACE_COUNT_ZERO);
            free_tree(print_root);
            return nullptr;
        }
        print_root->add_child(ast_expr);
    }

    // Missing ) after expressions
    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(print_root);
        return nullptr;
    }

    // Lexer error
    lex_error = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_error)) {
        free_tree(print_root);
        return nullptr;
    }

    // Missing semicolon after )
    if (unexpected_token(TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
        free_tree(print_root);
        return nullptr;
    }

    // At semicolon. Consume it.
    lex_error = get_token(next_token);
    skip_if_invalid_or_lexerr(lex_error);

    return print_root;
}

AST_NODE* parse_read() {
    AST_NODE* read_root = new AST_NODE();

    read_root->node_type = NODE_TYPE::READ;
    read_root->token = next_token;

    Error lex_err{};

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(read_root);
        return nullptr;
    }

    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(read_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(read_root);
        return nullptr;
    }

    if (next_token.is(TOKEN_RPAREN) || next_token.is_not(TOKEN_IDENT)) {
        set_print_token_error(Error{}, NLC_EXPECTED_VAR);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(read_root);
        return nullptr;
    }

    AST_NODE* var_node = new AST_NODE(next_token);
    var_node->symbol_type = SYMTYPE::VAR;
    var_node->node_type = NODE_TYPE::VAR;

    read_root->add_children(var_node);

    // Note: var_node must not be freed explicitly, a
    // call to free_tree() with read_root will free it.

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(read_root);
        return nullptr;
    }

    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(read_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(read_root);
        return nullptr;
    }

    if (unexpected_token(TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
        free_tree(read_root);
        return nullptr;
    }

    // At semicolon. Consume it.
    lex_err = get_token(next_token);
    skip_if_invalid_or_lexerr(lex_err);

    return read_root;
}

AST_NODE* parse_decl_int4() {
    Error lex_err{};

    AST_NODE* declare_root = new AST_NODE();
    declare_root->node_type = NODE_TYPE::DECL;
    declare_root->token = next_token;

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(declare_root);
        return nullptr;
    }

    // Variable name must be an identifier token
    if (unexpected_token(TOKEN_IDENT, NLC_INVALID_IDENTIFIER)) {
        free_tree(declare_root);
        return nullptr;
    }

    // Variable name is reserved
    if (is_reserved(next_token)) {
        set_print_token_error(Error{}, NLC_VARIABLE_NAME_RESERVED);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(declare_root);
        return nullptr;
    }

    // Put into symbol table 
    SYMINFO* entry = SYMTABLE::add_symbol(SYMINFO(next_token.identifier, TYPE::INT4, SYMTYPE::VAR));
    if (!entry) {
        set_print_token_error(Error{}, NLC_SYMBOL_ALREADY_EXISTS);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(declare_root);
        return nullptr; 
    }

    AST_NODE* var = new AST_NODE(next_token);
    var->syminfo = entry;

    declare_root->add_children(var);

    // Note: var must not be freed explicitly on an error, a call to 
    // free_tree with declare_root with free it.

    // Look for semicolon
    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(declare_root);
        return nullptr;
    }

    // Statement does not end with a semicolon
    if (unexpected_token(TOKEN_SEMICOLON, NLC_SYNTAX_ERROR)) {
        free_tree(declare_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    skip_if_invalid_or_lexerr(lex_err);

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
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(assign_root);
        return nullptr;
    }

    // Next token must be assignment
    if (unexpected_token(TOKEN_ASSIGN, NLC_SYNTAX_ERROR)) {
        free_tree(assign_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(assign_root);
        return nullptr;
    }

    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);

    free_tree(expr);

    if (!ast_expr) {
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(assign_root);
        return nullptr;
    }

    assign_root->add_children(ast_expr);

    // Statement does not end with a semicolon
    if (unexpected_token(TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
        free_tree(assign_root);
        return nullptr;
    }

    // At semicolon. Consume it.
    lex_err = get_token(next_token);
    skip_if_invalid_or_lexerr(lex_err);

    return assign_root;
}

// If this function is called while the current token is not
// the identifier else, it returns nullptr.
AST_NODE* parse_else() {
    if (!next_token.is_ident_else()) return nullptr;

    AST_NODE* else_root = new AST_NODE();
    else_root->node_type = NODE_TYPE::ELSE;
    else_root->token = next_token;

    Error lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_else, LBRACE_COUNT_ZERO)) {
        free_tree(else_root);
        return nullptr;
    }

    bool block = next_token.is_lbrace(); 

    // Grab the single statement and return
    if (!block) {
        AST_NODE* statement = get_statement();
        if (statement) else_root->add_children(statement);

        return else_root;
    }

    // From this point on we are inside a block

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_else, LBRACE_COUNT_ONE)) {
        free_tree(else_root);
        return nullptr;
    }

    // Otherwise we have a block, parse all statements.
    for(;;) {
        // Eat statements
        AST_NODE* statement = get_statement();
        if (statement) else_root->add_children(statement);

        // Structure was exited
        if (next_token.is_rbrace()) {
            get_next_token_and_print_error();
            break;
        } 
        // Never terminated structure
        else if (next_token.is_eof()) {
            set_print_token_error(Error{}, NLC_UNEXPECTED_EOF);
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
    if (skip_if_invalid_or_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return nullptr;
    }

    // Next token after if keyword must be an lparen
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return nullptr;
    }

    // If the next token after lparen is an rparen, we are missing an expression
    if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return nullptr;
    }

    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);
    free_tree(expr);

    if (!ast_expr) {
        skip_if(LBRACE_COUNT_ZERO);
        free_tree(ast_expr);
        free_tree(if_root);
        return nullptr;
    }

    // Ensure that the conditional is indeed a conditional
    if (!ast_expr->is_type_logical()) {
        set_print_token_error(Error{}, ast_expr->token, NLC_NON_LOGICAL_CONDITION);
        skip_if(LBRACE_COUNT_ZERO);
        free_tree(ast_expr);
        free_tree(if_root);
        return nullptr;
    }

    // No longer need to explicitly free ast_expr
    if_root->add_child(ast_expr);

    // Token following logical expression must be a right parenthesis
    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return nullptr;
    }

    // Empty if statement. Parse optional else and return
    if (next_token.is_semicolon()) {
        get_next_token_and_print_error();

        // Parse else if it exists
        AST_NODE* else_root = parse_else();
        if (else_root) if_root->add_children(else_root);

        return if_root;
    }

    // Otherwise we have at least one statement or a block. 
    // In that case, we first check if we have block.
    bool block = next_token.is_lbrace(); 

    if (!block) {
        AST_NODE* statement = get_statement();
        if (statement) if_root->add_children(statement);
    } else {

        lex_err = get_token(next_token);
        if (skip_if_invalid_or_lexerr(lex_err, skip_if, LBRACE_COUNT_ONE)) {
            free_tree(if_root);
            return nullptr;
        }

        // Parse all statements in block
        for(;;) {
            AST_NODE* statement = get_statement();
            if (statement) if_root->add_children(statement);

            // If block is concluded. Move past the right brace.
            if (next_token.is_rbrace()) {
                get_next_token_and_print_error();
                break;
            } 
            // Structure was never terminated
            else if (next_token.is_eof()) {
                set_print_token_error(Error{}, NLC_UNEXPECTED_EOF);
                free_tree(if_root);
                return nullptr;
            }
        }  
    }

    // If there exists an else, parse it.
    AST_NODE* else_root = parse_else();
    if (else_root) if_root->add_children(else_root);

    return if_root;
}

AST_NODE* parse_while() {
    int lbrace_count{};

    AST_NODE* while_root = new AST_NODE();
    while_root->node_type = NODE_TYPE::WHILE;

    Error lex_err{}, expr_err{};

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
        free_tree(while_root);
        return nullptr;
    }

    // Missing (
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR, skip_while, lbrace_count)) {
        free_tree(while_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
        free_tree(while_root);
        return nullptr;
    }

    // Missing condition
    if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION, skip_while, lbrace_count)) {
        free_tree(while_root);
        return nullptr;
    }

    // Get parse tree for expression then convert to AST
    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);

    // Free the parse tree
    free_tree(expr);

    // Invalid expression
    if (!ast_expr) {
        skip_while(lbrace_count);
        free_tree(ast_expr);
        free_tree(while_root);
        return nullptr;
    }

    // Condition was not boolean
    if (!ast_expr->is_type_logical()) {
        set_print_token_error(Error{}, ast_expr->token, NLC_NON_LOGICAL_CONDITION);
        skip_while(lbrace_count);
        free_tree(ast_expr);
        free_tree(while_root);
        return nullptr;
    }

    while_root->add_child(ast_expr);

    // Token after condition was not a right parenthesis
    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR, skip_while, lbrace_count)) {
        free_tree(while_root);
        return nullptr;
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
        free_tree(while_root);
        return nullptr;
    }

    // Explicitly empty while 
    if (next_token.is_semicolon()) {
        lex_err = get_token(next_token);
        if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
            free_tree(while_root);
            return nullptr;
        }
        return while_root;
    }

    // Check if we have a block ({...})
    bool block = next_token.is_lbrace();

    // If we have no block, we have a single statement
    if (!block) {
        AST_NODE* statement{get_statement()};
        if (statement) while_root->add_children(statement);

        return while_root;
    }

    // Otherwise, we have a statement block
    ++lbrace_count;

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
        free_tree(while_root);
        return nullptr;
    }

    // Empty block ({})
    if (next_token.is_rbrace()) {
        return while_root;
    }

    // Process all statements in block. Note that the block is 
    // strictly non-empty thanks to the check above
    for(;;) {
        AST_NODE* statement = get_statement();
        if (statement) while_root->add_children(statement);

        // Bad statements inside a while loop could be detrimental. 
        // For example, it could cause an infinite loop.
        // Therefore, if a bad statement is encountered inside a loop,
        // we eat the remainder of the structure.
        else {
            skip_while(lbrace_count);
            free_tree(while_root);
            return nullptr;
        }

        // At the rbrace that terminates the while loop.
        // Move one past it.
        if (next_token.is_rbrace()) {
            get_next_token_and_print_error();
            break;
        } 
        // Structure was never terminated
        else if (next_token.is_eof()) {
            set_print_token_error(Error{}, NLC_UNEXPECTED_EOF);
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
    if (next_token.in(First::BAP)) {
        left = B(err);
        right = AP(err);

        here->add_children(left, right);
        return here;
    } 

    free_tree(here);
    return nullptr;
}

// A' -> or BA' | \varepsilon
AST_NODE* AP(Error& err) {
    AST_NODE* here{};
    AST_NODE* left{}, *right{};

    // FIRST(or BA')
    if (next_token.in(First::orBAP)) {
        here = new AST_NODE(next_token, NODE_TYPE::OR, OPERATOR);

        // Consume the or token
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            return nullptr;
        }

        left = B(err);
        right = AP(err);

        here->add_children(left, right);
        return here;
    } 
    // Takes A' -> \varepsilon
    return nullptr;
}

// B -> CB'
AST_NODE* B(Error& err) {
    AST_NODE* here = new AST_NODE();
    AST_NODE* left{}, *right{};

    // Consider FIRST(CB')
    if (next_token.in(First::CBP)) {
        left = C(err);
        right = BP(err);

        here->add_children(left, right);
        return here;
    } 
    return nullptr;
}

// B' -> and CB' | \varepsilon
AST_NODE* BP(Error& err) {
    AST_NODE* here{};
    AST_NODE* left{}, *right{};

    // t \in FIRST(and CB')
    if (next_token.in(First::andCBP)) {
        here = new AST_NODE(next_token, NODE_TYPE::AND, OPERATOR);

        // Consume and token
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            return nullptr;
        }

        left = C(err);
        right = BP(err);

        here->add_children(left, right);
        return here;
    } 
    return nullptr;
}

// C -> ~C | D
AST_NODE* C(Error& err) {
    AST_NODE* here = new AST_NODE();
    AST_NODE* left{};

    // t \in FIRST(~C)
    if (next_token.in(First::notC)) {
        here = new AST_NODE(next_token, NODE_TYPE::NOT, OPERATOR);

        // Consume not token
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            return nullptr;
        }

        left = C(err);
    }

    // t \in FIRST(D)
    else if (next_token.in(First::D)) {
        left = D(err);
    } 

    else {
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    here->add_children(left);
    return here;
}

// D -> ED'
AST_NODE* D(Error& err) {
    AST_NODE* here = new AST_NODE();
    AST_NODE* left{}, *right{};

    // t \in FIRST(ED')
    if (next_token.in(First::EDP)) {
        left = E(err);
        right = DP(err);
    } else {
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    here->add_children(left, right);
    return here;
}

// D' -> ==E | \ne E | <E | <=E | >E | >=E | \varepsilon
AST_NODE* DP(Error& err) {
    AST_NODE* here{};
    AST_NODE* left{};

    // Consider these various first sets
    if (next_token.in_union(
        First::equalE, First::neqE, First::lessE, 
        First::leqE, First::greaterE, First::geqE
    )) {
        // Take E and consume the token
        here = new AST_NODE(next_token, get_node_type(next_token), OPERATOR);

        // Consume the operator
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            goto_next_semicolon();
            err = tmp_error;
            return nullptr;
        }

        left = E(err);

        here->add_children(left);
        return here;
    }
    return nullptr;
}

// E -> TE'
AST_NODE* E(Error& err) {
    AST_NODE* left = nullptr;

    // t \in FIRST(TE'). Removed E' for this fold operation, left folds
    // +,- so that these operations are not right associative
    if (next_token.in(First::TEP)) {
        left = T(err);

        // Simulates the job of E', but makes +,- left associative
        // t \in FIRST(+TE') \cup FIRST(-TE')
        while (next_token.in_union(First::plusTEP, First::minusTEP)) {
            Token op = next_token;

            // Consume operator
            Error tmp_err = get_token(next_token);
            if (invalid_lookahead() || handle_lex_error(tmp_err)) {
                err = tmp_err;
            	return nullptr;
            }

            AST_NODE* rhs = T(err);
            AST_NODE* here = new AST_NODE();

            // Has previous token as its token (the operator)
            here->token = op;
            here->is_operator = true;
            here->node_type = get_node_type(op);

            here->add_children(left, rhs);

            left = here;
        }
        return left;
    } 

    set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
    return nullptr;
}

// T -> NT'
AST_NODE* T(Error& err) {
    AST_NODE* left = nullptr;

    // t \in FIRST(NT')
    if (next_token.id == TOKEN_UPLUS || next_token.id == TOKEN_UNEG ||
        next_token.id == TOKEN_LPAREN || next_token.id == TOKEN_INTEGER ||
        next_token.id == TOKEN_STRING || next_token.id == TOKEN_IDENT) {

        left = N(err);

        // Left fold *,/,mod. Same idea as for +,-
        // t \in FIRST(*NT') \cup FIRST(/NT') \cup FIRST(modNT')
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
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
    }

    return nullptr;
}

// N -> \oplus N | \neg N | F
AST_NODE* N(Error& err) {
    AST_NODE* here = new AST_NODE();
	AST_NODE* left{}, *right{};

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
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    here->add_child(left);
    return here;
}

// F -> SF'
AST_NODE* F(Error& err) {
    AST_NODE* here = new AST_NODE();
	AST_NODE* left{}, *right{};

    // t \in FIRST(SF')
    if (next_token.id == TOKEN_LPAREN 
        || next_token.id == TOKEN_INTEGER 
        || next_token.id == TOKEN_STRING 
        || next_token.id == TOKEN_IDENT
    ) {
        // Take F -> SF'
        left = S(err);
        right = FP(err);
    } else {
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    here->add_children(left, right);
    return here;
}

// F' -> ^SF' | \varepsilon
AST_NODE* FP(Error& err) {
    AST_NODE* here = new AST_NODE();
	AST_NODE* left{}, *right{};

    // t \in FIRST(^SF')
    if (next_token.id == TOKEN_EXP) {
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
    // Take F' -> \varepsilon
    else {
        free_tree(here);
        return nullptr;
    }

    here->add_children(left, right);
    return here;
}

// S -> (A) | int | ident | string
AST_NODE* S(Error& err) {
    AST_NODE* here = new AST_NODE();
	AST_NODE* left{};

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
            err.error = NLC_EXPECTED_RPAREN;
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
                set_print_token_error(Error{}, NLC_UNKNOWN_VARIABLE);
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
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    if (left) here->add_child(left);
    return here;
}
