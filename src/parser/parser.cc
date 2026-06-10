/* Selection rule (considering current level A \to \alpha_1 | ... | \alpha_k)
    \begin{enumerate}
        \item If $a \in \text{FIRST}(\alpha_{i}) $, choose the production $\alpha_{i}$, otherwise
        \item If $\varepsilon \in \text{FIRST}(\alpha_{k})$, and $a \in \text{FOLLOW}(A)$, choose $\alpha_{k}$,
        \item Otherwise, syntax error.
    \end{enumerate}
*/

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
#include "token_structures.h"
#include "token.h"
#include "sets.h"
#include "lexstate.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <functional>

bool SUPPRESS_PARSER_ERRORS{};

Token next_token;

// Globals local to this unit
namespace {
    AST_NODE* program_tree{new AST_NODE(NODE_TYPE::BLOCK)};
}

Error parser_init(const char* src_code) {
    Error err = lex_init(src_code);
    return err;
}

// Calls the correct function in parse_map based on the
// current identifier.
static StatementReturns get_statement() {
    std::string ident = next_token.identifier;
    if (parse_map.find(ident) != parse_map.end()) {
        return parse_map.at(ident)();
    }

    // Suppress errors, save state, and try to 
    // parse as assignment.
    SUPPRESS_PARSER_ERRORS = true;

    LexState state = lex_save();
    StatementReturns possible_assign = parse_assign();

    SUPPRESS_PARSER_ERRORS = false;

    if (possible_assign.size()) {
        return {possible_assign};
    }

    // Either the assignment had an error
    // or we have an expression.

    // Just call it an expression
    lex_goto_last_save(state);
    try_expression();

    return {};
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

    get_token(next_token);

    for (;;) {
        if (next_token.is_ident()) {
            StatementReturns statements{get_statement()};

            if (statements.size()) {
                program_tree->add_all_statements(statements);
            }
        } 

        else {
            try_expression();
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

StatementReturns parse_print() {
    Error lex_error{}, expr_error{};

    AST_NODE* print_root = new AST_NODE();
    print_root->node_type = NODE_TYPE::PRINT;
    print_root->token = next_token;

    lex_error = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_error)) {
        free_tree(print_root);
        return {};
    }

    // Missing ( after print
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(print_root);
        return {};
    }
    
    lex_error = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_error)) {
        free_tree(print_root);
        return {};
    }

    // If we have an rparen here the print statement is empty, illegal.
    if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION)) {
        free_tree(print_root);
        return {};
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
        return {};
    }

    // Otherwise the expression is valid and we add it to the root
    print_root->add_child(ast_expr);

    // Process all subsequent expressions
    while (next_token.is(TOKEN_COMMA)) {
        lex_error = get_token(next_token);
        if (skip_if_invalid_or_lexerr(lex_error)) {
            free_tree(print_root);
            return {};
        }

        // No expression following the comma
        if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION)) {
            free_tree(print_root);
            return {};
        }

        expr = A(expr_error);
        ast_expr = pttoast(expr);
        free_tree(expr);

        if (!ast_expr) {
            onepast_semi_or_block(LBRACE_COUNT_ZERO);
            free_tree(print_root);
            return {};
        }
        print_root->add_child(ast_expr);
    }

    // Missing ) after expressions
    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(print_root);
        return {};
    }

    // Lexer error
    lex_error = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_error)) {
        free_tree(print_root);
        return {};
    }

    // Missing semicolon after )
    if (unexpected_token(TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
        free_tree(print_root);
        return {};
    }

    // At semicolon. Consume it.
    lex_error = get_token(next_token);
    skip_if_invalid_or_lexerr(lex_error);

    return {print_root};
}

StatementReturns parse_read() {
    AST_NODE* read_root = new AST_NODE();

    read_root->node_type = NODE_TYPE::READ;
    read_root->token = next_token;

    Error lex_err{};

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(read_root);
        return {};
    }

    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(read_root);
        return {};
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(read_root);
        return {};
    }

    if (next_token.is(TOKEN_RPAREN) || next_token.is_not(TOKEN_IDENT)) {
        set_print_token_error(Error{}, NLC_EXPECTED_VAR);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(read_root);
        return {};
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
        return {};
    }

    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(read_root);
        return {};
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(read_root);
        return {};
    }

    if (unexpected_token(TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
        free_tree(read_root);
        return {};
    }

    // At semicolon. Consume it.
    lex_err = get_token(next_token);
    skip_if_invalid_or_lexerr(lex_err);

    return {read_root};
}

StatementReturns parse_decl_int4() {
    StatementReturns declares{};

    Error lex_err{};

    AST_NODE* declare_root = new AST_NODE();
    declare_root->node_type = NODE_TYPE::DECL;
    declare_root->token = next_token;

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(declare_root);
        return {};
    }

    // Variable name must be an identifier token
    if (unexpected_token(TOKEN_IDENT, NLC_INVALID_IDENTIFIER)) {
        free_tree(declare_root);
        return {};
    }

    // Variable name is reserved
    if (is_reserved(next_token)) {
        set_print_token_error(Error{}, NLC_VARIABLE_NAME_RESERVED);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(declare_root);
        return {};
    }

    // Put into symbol table 
    SYMINFO* entry = SYMTABLE::add_symbol(
        SYMINFO(next_token.identifier, TYPE::INT4, SYMTYPE::VAR)
    );

    if (!entry) {
        set_print_token_error(Error{}, NLC_SYMBOL_ALREADY_EXISTS);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(declare_root);
        return {}; 
    }

    AST_NODE* var = new AST_NODE(next_token, NODE_TYPE::VAR, SYMTYPE::VAR);
    var->install_symbol(entry);

    declare_root->add_children(var);

    // Note: var must not be freed explicitly on an error, a call to 
    // free_tree with declare_root with free it.

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(declare_root);
        return {};
    }

    // Declare with assignment
    AST_NODE* expression{};
    if (next_token.is(TOKEN_ASSIGN)) {
        lex_err = get_token(next_token);
        if (skip_if_invalid_or_lexerr(lex_err)) {
            free_tree(declare_root);
            return {};
        }

        expression = get_initial_value(); 
        if (expression) declare_root->add_children(expression);

        else {
            onepast_semi_or_block(LBRACE_COUNT_ZERO);
            return {};
        }
    }

    // First good declare statement
    declares.push_back(declare_root);

    // Statement does not end with a semicolon
    if (unexpected_token(TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
        free_statement_return_list(declares);
        return {};
    }

    lex_err = get_token(next_token);
    skip_if_invalid_or_lexerr(lex_err);

    return declares;
}

StatementReturns parse_assign() {
    Error lex_err{}, expr_err{};

    AST_NODE* assign_root = new AST_NODE(NODE_TYPE::ASSIGN);
    AST_NODE* var_node = new AST_NODE(next_token, NODE_TYPE::VAR, SYMTYPE::VAR);

    assign_root->add_children(var_node);

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(assign_root);
        return {};
    }

    // Next token must be assignment
    if (unexpected_token(TOKEN_ASSIGN, NLC_SYNTAX_ERROR)) {
        free_tree(assign_root);
        return {};
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err)) {
        free_tree(assign_root);
        return {};
    }

    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);

    free_tree(expr);

    if (!ast_expr) {
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(assign_root);
        return {};
    }

    assign_root->add_children(ast_expr);

    // Statement does not end with a semicolon
    if (unexpected_token(TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
        free_tree(assign_root);
        return {};
    }

    // At semicolon. Consume it.
    lex_err = get_token(next_token);
    skip_if_invalid_or_lexerr(lex_err);

    return {assign_root};
}

// If this function is called while the current token is not
// the identifier else, it returns nullptr.
StatementReturns parse_else() {
    if (!next_token.is_ident_else()) return {};

    AST_NODE* else_root = new AST_NODE();
    else_root->node_type = NODE_TYPE::ELSE;
    else_root->token = next_token;

    Error lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_else, LBRACE_COUNT_ZERO)) {
        free_tree(else_root);
        return {};
    }

    bool block = next_token.is_lbrace(); 

    // Grab the single statement and return
    if (!block) {
        StatementReturns statements = get_statement();
        if (statements.size()) else_root->add_all_statements(statements);

        return {else_root};
    }

    // From this point on we are inside a block

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_else, LBRACE_COUNT_ONE)) {
        free_tree(else_root);
        return {};
    }

    // Otherwise we have a block, parse all statements.
    for(;;) {
        // Eat statements
        StatementReturns statements = get_statement();
        if (statements.size()) else_root->add_all_statements(statements);

        // Structure was exited
        if (next_token.is_rbrace()) {
            get_next_token_and_print_error();
            break;
        } 
        // Never terminated structure
        else if (next_token.is_eof()) {
            set_print_token_error(Error{}, NLC_UNEXPECTED_EOF);
            free_tree(else_root);
            return {};
        }
    }

    return {else_root};
}

StatementReturns parse_if() {
    AST_NODE* if_root = new AST_NODE(); 
    if_root->token = next_token;
    if_root->node_type = NODE_TYPE::IF;

    Error lex_err{}, expr_err{};

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return {};
    }

    // Next token after if keyword must be an lparen
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return {};
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return {};
    }

    // If the next token after lparen is an rparen, we are missing an expression
    if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return {};
    }

    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);
    free_tree(expr);

    if (!ast_expr) {
        skip_if(LBRACE_COUNT_ZERO);
        free_tree(ast_expr);
        free_tree(if_root);
        return {};
    }

    // Ensure that the conditional is indeed a conditional
    if (!ast_expr->is_type_logical()) {
        set_print_token_error(Error{}, ast_expr->token, NLC_NON_LOGICAL_CONDITION);
        skip_if(LBRACE_COUNT_ZERO);
        free_tree(ast_expr);
        free_tree(if_root);
        return {};
    }

    // No longer need to explicitly free ast_expr
    if_root->add_child(ast_expr);

    // Token following logical expression must be a right parenthesis
    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return {};
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_tree(if_root);
        return {};
    }

    // Empty if statement. Parse optional else and return
    if (next_token.is_semicolon()) {
        get_next_token_and_print_error();

        // Parse else if it exists
        StatementReturns else_root = parse_else();
        if (else_root.size()) if_root->add_all_statements(else_root);

        return {if_root};
    }

    // Otherwise we have at least one statement or a block. 
    // In that case, we first check if we have block.
    bool block = next_token.is_lbrace(); 

    if (!block) {
        StatementReturns statements = get_statement();
        if (statements.size()) if_root->add_all_statements(statements);
    } else {

        lex_err = get_token(next_token);
        if (skip_if_invalid_or_lexerr(lex_err, skip_if, LBRACE_COUNT_ONE)) {
            free_tree(if_root);
            return {};
        }

        // Parse all statements in block
        for(;;) {
            StatementReturns statements = get_statement();
            if (statements.size()) if_root->add_all_statements(statements);

            // If block is concluded. Move past the right brace.
            if (next_token.is_rbrace()) {
                get_next_token_and_print_error();
                break;
            } 

            // Structure was never terminated
            else if (next_token.is_eof()) {
                set_print_token_error(Error{}, NLC_UNEXPECTED_EOF);
                free_tree(if_root);
                return {};
            }
        }  
    }

    // If there exists an else, parse it.
    StatementReturns else_root = parse_else();
    if (else_root.size()) if_root->add_all_statements(else_root);

    return {if_root};
}

StatementReturns parse_while() {
    int lbrace_count{};

    AST_NODE* while_root = new AST_NODE();
    while_root->node_type = NODE_TYPE::WHILE;

    Error lex_err{}, expr_err{};

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
        free_tree(while_root);
        return {};
    }

    // Missing (
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR, skip_while, lbrace_count)) {
        free_tree(while_root);
        return {};
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
        free_tree(while_root);
        return {};
    }

    // Missing condition
    if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION, skip_while, lbrace_count)) {
        free_tree(while_root);
        return {};
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
        return {};
    }

    // Condition was not boolean
    if (!ast_expr->is_type_logical()) {
        set_print_token_error(Error{}, ast_expr->token, NLC_NON_LOGICAL_CONDITION);
        skip_while(lbrace_count);
        free_tree(ast_expr);
        free_tree(while_root);
        return {};
    }

    while_root->add_child(ast_expr);

    // Token after condition was not a right parenthesis
    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR, skip_while, lbrace_count)) {
        free_tree(while_root);
        return {};
    }

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
        free_tree(while_root);
        return {};
    }

    // Explicitly empty while 
    if (next_token.is_semicolon()) {
        lex_err = get_token(next_token);
        if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
            free_tree(while_root);
            return {};
        }
        return {while_root};
    }

    // Check if we have a block ({...})
    bool block = next_token.is_lbrace();

    // If we have no block, we have a single statement
    if (!block) {
        StatementReturns statements = get_statement();
        if (statements.size()) while_root->add_all_statements(statements);

        return {while_root};
    }

    // Otherwise, we have a statement block
    ++lbrace_count;

    lex_err = get_token(next_token);
    if (skip_if_invalid_or_lexerr(lex_err, skip_while, lbrace_count)) {
        free_tree(while_root);
        return {};
    }

    // Empty block ({})
    if (next_token.is_rbrace()) {
        return {while_root};
    }

    // Process all statements in block. Note that the block is 
    // strictly non-empty thanks to the check above
    for(;;) {
        StatementReturns statements = get_statement();
        if (statements.size()) while_root->add_all_statements(statements);

        // Bad statements inside a while loop could be detrimental. 
        // For example, it could cause an infinite loop.
        // Therefore, if a bad statement is encountered inside a loop,
        // we eat the remainder of the structure.
        else {
            skip_while(lbrace_count);
            free_tree(while_root);
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
            set_print_token_error(Error{}, NLC_UNEXPECTED_EOF);
            free_tree(while_root);
            return {};
        }
    }

    return {while_root};
}

// A -> BA'
AST_NODE* A(Error& err) {
    AST_NODE* here{}, *left{}, *right{};

    // FIRST(BA')
    if (next_token.in(First::BAP)) {
        here = new AST_NODE();

        left = B(err);
        right = AP(err);

        here->add_children(left, right);
        return here;
    } 

    set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
    return nullptr;
}

// A' -> or BA' | \varepsilon
AST_NODE* AP(Error& err) {
    AST_NODE* here{}, *left{}, *right{};

    // FIRST(or BA')
    if (next_token.in(First::orBAP)) {
        here = new AST_NODE(next_token, NODE_TYPE::OR, OPERATOR);

        // Consume the or token
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            free_tree(here);
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
    AST_NODE* here, *left{}, *right{};

    // Consider FIRST(CB')
    if (next_token.in(First::CBP)) {
        here = new AST_NODE();

        left = C(err);
        right = BP(err);

        here->add_children(left, right);
        return here;
    } 

    set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
    return nullptr;
}

// B' -> and CB' | \varepsilon
AST_NODE* BP(Error& err) {
    AST_NODE* here{}, *left{}, *right{};

    // t \in FIRST(and CB')
    if (next_token.in(First::andCBP)) {
        here = new AST_NODE(next_token, NODE_TYPE::AND, OPERATOR);

        // Consume and token
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            free_tree(here);
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
    AST_NODE* here{}, *left{};

    // t \in FIRST(~C)
    if (next_token.in(First::notC)) {
        here = new AST_NODE(next_token, NODE_TYPE::NOT, OPERATOR);

        // Consume not token
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error; 
            free_tree(here);
            return nullptr;
        }

        left = C(err);
    }

    // t \in FIRST(D)
    else if (next_token.in(First::D)) {
        here = new AST_NODE();
        left = D(err);
    } 

    else {
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
        return nullptr;
    }

    here->add_children(left);
    return here;
}

// D -> ED'
AST_NODE* D(Error& err) {
    AST_NODE* here{}, *left{}, *right{};

    // t \in FIRST(ED')
    if (next_token.in(First::EDP)) {
        here = new AST_NODE();

        left = E(err);
        right = DP(err);

        here->add_children(left, right);
        return here;
    } 

    set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
    return nullptr;
}

// D' -> ==E | \ne E | <E | <=E | >E | >=E | \varepsilon
AST_NODE* DP(Error& err) {
    AST_NODE* here{}, *left{};

    // Consider these various first sets
    if (next_token.in_union(First::equalE, First::neqE, First::lessE, 
        First::leqE, First::greaterE, First::geqE
    )) {
        // Take E and consume the token
        here = new AST_NODE(next_token, get_node_type(next_token), OPERATOR);

        // Consume the operator
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
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
    AST_NODE* left{}; 

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
                free_tree(left);
            	return nullptr;
            }

            AST_NODE* here = new AST_NODE(op, get_node_type(op), OPERATOR);
            AST_NODE* rhs = T(err);

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
    AST_NODE* left{};

    // t \in FIRST(NT')
    if (next_token.in(First::NTP)) {
        left = N(err);

        // Left fold *,/,mod. Same idea as for +,-
        // t \in FIRST(*NT') \cup FIRST(/NT') \cup FIRST(modNT')
        while (next_token.in_union(First::multNTP, First::divNTP, First::modNTP)) {
            Token op = next_token;

            Error tmp_err = get_token(next_token);
            if (invalid_lookahead() || handle_lex_error(tmp_err)) {
                err = tmp_err;
                free_tree(left);
            	return nullptr;
            }

            AST_NODE* node = new AST_NODE(op, get_node_type(op), OPERATOR);
            AST_NODE* rhs = N(err);

            node->add_children(left, rhs);

            left = node;
        }
        return left;
    } 

    set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
    return nullptr;
}

// N -> \oplus F | \neg F | F
AST_NODE* N(Error& err) {
    AST_NODE* here{}, *left{}, *right{};

    // t \in FIRST(\neg F) or t \in FIRST(\oplus F)
    if (next_token.in_union(First::uplusN, First::unegN)) {
        here = new AST_NODE(next_token, get_node_type(next_token), OPERATOR);

        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error;
            free_tree(here);
        	return nullptr;
        }
    
        left = F(err);
    } 

    // t \in FIRST(F)
    else if (next_token.in(First::F)) {
        here = new AST_NODE();

        left = F(err);
    } 

    else {
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
        return nullptr;
    }

    here->add_children(left);
    return here;
}

// F -> SF'
AST_NODE* F(Error& err) {
    AST_NODE* here{}, *left{}, *right{};

    // t \in FIRST(SF')
    if (next_token.in(First::SFP)) {
        here = new AST_NODE();

        left = S(err);
        right = FP(err);

        here->add_children(left, right);
        return here;
    } 

    set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
    return nullptr;
}

// F' -> ^SF' | \varepsilon
AST_NODE* FP(Error& err) {
    AST_NODE* here{}, *left{}, *right{};

    // t \in FIRST(^SF')
    if (next_token.in(First::expSFP)) {
        here = new AST_NODE(next_token, get_node_type(next_token), OPERATOR);

        // Consume operator exp
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error;
            free_tree(here);
        	return nullptr;
        }

        // Take F' -> ^SF'
        left = S(err);
        right = FP(err);

        here->add_children(left, right);
        return here;
    } 

    return nullptr;
}

// S -> (A) | int | ident | string
AST_NODE* S(Error& err) {
    AST_NODE* here{}, *left{};

    // t \in FIRST(int)
    if (next_token.in(First::integer)) {
        here = integer_terminal(err);
    } 

    // t \in FIRST((A))
    else if (next_token.in(First::lpArp)) { 
        here = paren_expression(err, left);
        here->add_children(left);
    } 

    // t \in FIRST(string)
    else if (next_token.in(First::string)) {
        here = string_terminal(err);
    } 

    else if (next_token.is_boolean()) {
        here = boolean_terminal(err);
    }

    // t \in FIRST(ident) (and not boolean)
    else if (next_token.in(First::ident)) {
        here = variable_terminal(err);
    } 

    else {
        set_print_token_error(Error{}, NLC_SYNTAX_ERROR);
        free_tree(here);
        return nullptr;
    }

    return here;
}

AST_NODE* integer_terminal(Error& err) {
    AST_NODE* here = new AST_NODE(next_token, NODE_TYPE::INT, TYPE::INT4);

    Error tmp_error = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(tmp_error)) {
        err = tmp_error;
        free_tree(here);
        return nullptr;
    }

    return here;
}

AST_NODE* paren_expression(Error& err, AST_NODE* left) {
    AST_NODE* here = new AST_NODE();

    // Eat lparen
    Error tmp_error = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(tmp_error)) {
        err = tmp_error;
        free_tree(here);
        return nullptr;
    }

    left = A(err);

    // If next token is not ), error
    if (next_token.is_not(TOKEN_RPAREN)) {
        set_print_token_error(err, NLC_EXPECTED_RPAREN);
        free_tree(here);
        return nullptr;
    } 

    // Otherwise eat the )
    else {
        Error tmp_error = get_token(next_token);
        if (invalid_lookahead() || handle_lex_error(tmp_error)) {
            err = tmp_error;
            free_tree(here);
            return nullptr;
        }
    }

    return here;
}

AST_NODE* string_terminal(Error& err) {
    AST_NODE* here = new AST_NODE(next_token, NODE_TYPE::STR, TYPE::STRING);

    STR_TABLE_ENTRY entry = STR_TABLE::add_string(next_token.str);

    // Attempted to overrun the internal string table
    if (entry.is_not_valid()) {
        free_tree(here);
        return nullptr;
    }

    here->entry = entry;

    Error tmp_error = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(tmp_error)) {
        err = tmp_error;
        free_tree(here);
        return nullptr;
    }

    return here;
}

AST_NODE* boolean_terminal(Error& err) {
    AST_NODE* here = new AST_NODE(next_token, NODE_TYPE::BOOL, TYPE::BOOL); 

    Error tmp_error = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(tmp_error)) {
        err = tmp_error;
        free_tree(here);
        return nullptr;
    }

    return here;
}

AST_NODE* variable_terminal(Error& err) {
    AST_NODE* here = new AST_NODE(next_token, NODE_TYPE::VAR, TYPE::null);

    // Search the symbol table
    SYMINFO* syminfo = SYMTABLE::get_symbol(next_token.identifier, SYMTYPE::VAR);

    if (!syminfo) {
        set_print_token_error(Error{}, NLC_UNKNOWN_VARIABLE);
        free_tree(here);
        return nullptr;
    }

    here->install_symbol(syminfo);

    Error tmp_error = get_token(next_token);
    if (invalid_lookahead() || handle_lex_error(tmp_error)){
        err = tmp_error;
        free_tree(here);
        return nullptr;
    }

    return here;
}

