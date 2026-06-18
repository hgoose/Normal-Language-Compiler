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
#include "scope_stack.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

bool SUPPRESS_PARSER_ERRORS{};
bool ENFORCE_TERMINATING_SEMICOLON{true};

Token next_token;
Token prev_token;

static StatementReturns sub_parse_assign();
static StatementReturns continue_assign_parse();

// Globals local to this unit
namespace {
    AST_NODE* program_tree{new AST_NODE(NODE_TYPE::BLOCK, Scope::Scope::level())};
}

Error parser_init(const char* src_code) {
    return lex_init(src_code);
}

// Calls the correct function in parse_map based on the
// current identifier.
StatementReturns get_statement() {
    // Changed from identifier to lexeme for better generality.
    // For example, a block begin ({) is not an identifier. 
    std::string lexeme = next_token.lexeme;
    if (parse_map.find(lexeme) != parse_map.end()) {
        return parse_map.at(lexeme)();
    }

    Token peek = tpeek();

    if (peek.is_assign()) {
        return parse_assign();
    }

    AST_NODE* possible_expression = try_expression(DONT_EAT_SEMICLOON);

    if (possible_expression && possible_expression->is_fn_call()) {
        return {possible_expression};
    }

    if (possible_expression) {
        if (ENFORCE_TERMINATING_SEMICOLON && next_token.is_not_semicolon()) 
            return {};
        return {possible_expression};
    }

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

    Scope::enter_level();
    munch();

    for (;;) {
        if (next_token.is_semicolon()) {
            munch_all_semicolons();
        }

        if (next_token.is_eof()) break;

        if (next_token.is_ident() || next_token.is_block_begin()) {
            StatementReturns statements{get_statement()};

            program_tree->add_all_statements(statements);
        } 

        else {
            try_expression();
        }
    }

    x86_pushr64(REGISTER::R12);
    x86_pushr64(REGISTER::R15);

    process_block(program_tree);

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
    AST_NODE* print_root = new AST_NODE(
        NODE_TYPE::PRINT,
        Scope::level()
    );

    Error lex_error{}, expr_error{};
    lex_error = munch();
    if (skip_if_lexerr(lex_error)) {
        free_tree(print_root);
        return {};
    }

    // Missing ( after print
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(print_root);
        return {};
    }
    
    lex_error = munch();
    if (skip_if_lexerr(lex_error)) {
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
        lex_error = munch();
        if (skip_if_lexerr(lex_error)) {
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
    lex_error = munch();
    if (skip_if_lexerr(lex_error)) {
        free_tree(print_root);
        return {};
    }

    if (ENFORCE_TERMINATING_SEMICOLON) {
        // Missing semicolon after )
        if (unexpected_token(prev_token, TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
            free_tree(print_root);
            return {};
        }

        // At semicolon. Consume it.
        lex_error = munch();
        skip_if_lexerr(lex_error);
    }

    return {print_root};
}

StatementReturns parse_read() {
    AST_NODE* read_root = new AST_NODE(
        NODE_TYPE::READ,
        Scope::level()
    );

    Error lex_err{};

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_tree(read_root);
        return {};
    }

    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(read_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_tree(read_root);
        return {};
    }

    if (next_token.is(TOKEN_RPAREN) || next_token.is_not(TOKEN_IDENT)) {
        set_print_token_error(Error{}, NLC_EXPECTED_VAR);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(read_root);
        return {};
    }

    AST_NODE* var_node = new AST_NODE(next_token, NODE_TYPE::VAR, SYMTYPE::VAR, Scope::level());

    read_root->add_children(var_node);

    // Note: var_node must not be freed explicitly, a
    // call to free_tree() with read_root will free it.

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_tree(read_root);
        return {};
    }

    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR)) {
        free_tree(read_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_tree(read_root);
        return {};
    }

    if (ENFORCE_TERMINATING_SEMICOLON) {
        if (unexpected_token(prev_token, TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
            free_tree(read_root);
            return {};
        }

        // At semicolon. Consume it.
        lex_err = munch();
        skip_if_lexerr(lex_err);
    }

    return {read_root};
}

StatementReturns parse_decl_int() {
    AST_NODE* declare_root = new AST_NODE(
        NODE_TYPE::DECL,
        Scope::level()
    );

    bool top = next_token.is_type();
    StatementReturns declares{};
    Error lex_err{};

    if (next_token.is_type()) {
        lex_err = munch();
        if (skip_if_lexerr(lex_err)) {
            free_tree(declare_root);
            return {};
        }
    }

    if (next_token.is_not_ident() || next_token.is_ident_reserved()) {
        set_print_token_error(Error{}, NLC_INVALID_IDENTIFIER);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
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
    SYMINFO* syminfo_entry = new SYMINFO(next_token.identifier, TYPE::INT, SYMTYPE::VAR, Scope::level());
    SYMINFO* entry = SYMTABLE::add_symbol(syminfo_entry);

    if (!entry) {
        set_print_token_error(Error{}, NLC_SYMBOL_ALREADY_EXISTS);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(declare_root);

        // If the add was good, SYMINFO memory is 
        // managed by the symbol table and scope stack.
        delete syminfo_entry;
        return {}; 
    }

    AST_NODE* var = new AST_NODE(next_token, NODE_TYPE::VAR, SYMTYPE::VAR, Scope::level());
    var->install_symbol(entry);

    declare_root->add_children(var);

    // Note: var must not be freed explicitly on an error, a call to 
    // free_tree with declare_root with free it.

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_tree(declare_root);

        SYMTABLE::remove_symbol(entry);
        return {};
    }

    // Declare with assignment
    AST_NODE* expression{};
    if (next_token.is(TOKEN_ASSIGN)) {
        lex_err = munch();
        if (skip_if_lexerr(lex_err)) {
            free_tree(declare_root);

            SYMTABLE::remove_symbol(entry);
            return {};
        }

        expression = get_initial_value(); 
        if (expression) declare_root->add_children(expression);

        else {
            onepast_semi_or_block(LBRACE_COUNT_ZERO);

            SYMTABLE::remove_symbol(entry);
            return {};
        }
    }

    // First good declare statement
    declares.push_back(declare_root);

    if (next_token.is_comma()) {
        get_next_token_and_print_error();

        // Semicolon immediately after a comma implies 
        // missing identifier.
        if (wrong_next_token(TOKEN_SEMICOLON, NLC_INVALID_IDENTIFIER)) {
            free_statement_return_list(declares);

            SYMTABLE::remove_symbol(entry);
            return {};
        }

        // Next token after a comma must be an identifier 
        // that is not reserved
        if (next_token.is_not_ident() || next_token.is_ident_reserved()) {
            set_print_token_error(Error{}, NLC_INVALID_IDENTIFIER);
            onepast_semi_or_block(LBRACE_COUNT_ZERO);
            free_statement_return_list(declares);

            SYMTABLE::remove_symbol(entry);
            return {};
        }

        StatementReturns others = parse_decl_int();

        // Call either produced an error or was 
        // missing an initialization.
        if (others.empty()) {
            onepast_semi_or_block(LBRACE_COUNT_ZERO);
            free_statement_return_list(declares);

            SYMTABLE::remove_symbol(entry);
            return {};
        }

        merge_statement_returns(declares, others);
    }

    if (!top) return declares;

    if (ENFORCE_TERMINATING_SEMICOLON) {
        // Statement does not end with a semicolon
        if (unexpected_token(prev_token, TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
            free_statement_return_list(declares);

            SYMTABLE::remove_symbol(entry);
            return {};
        }

        lex_err = munch();
        skip_if_lexerr(lex_err);
    }

    return declares;
}

static bool continue_assign_parse(StatementReturns& assigns) {
    if (next_token.is_comma()) {
        get_next_token_and_print_error();

        // Semicolon immediately after a comma implies 
        // missing identifier.
        if (wrong_next_token(TOKEN_SEMICOLON, NLC_INVALID_IDENTIFIER)) {
            free_statement_return_list(assigns);
            return false;
        }

        // Next token after a comma must be an identifier 
        // that is not reserved
        if (next_token.is_not_ident() || next_token.is_ident_reserved()) {
            set_print_token_error(Error{}, NLC_INVALID_IDENTIFIER);
            onepast_semi_or_block(LBRACE_COUNT_ZERO);
            free_statement_return_list(assigns);
            return false;
        }

        StatementReturns others = sub_parse_assign();

        // Call either produced an error or was 
        // missing an initialization.
        if (others.empty()) {
            onepast_semi_or_block(LBRACE_COUNT_ZERO);
            free_statement_return_list(assigns);
            return false;
        }

        merge_statement_returns(assigns, others);
    }

    return true;
}

// Handles one assign in a comma separated list of assignments.
static StatementReturns sub_parse_assign() {
    AST_NODE* assign_root = new AST_NODE(
        NODE_TYPE::ASSIGN,
        Scope::level()
    );

    AST_NODE* var_node = new AST_NODE(
        next_token,
        NODE_TYPE::VAR,
        SYMTYPE::VAR, Scope::level()
    );

    SYMINFO* symbol = SYMTABLE::get_symbol(
        var_node->token.identifier,
        SYMTYPE::VAR,
        Scope::level()
    );

    if (!symbol) {
        set_print_token_error(Error{}, var_node->token, NLC_UNKNOWN_VARIABLE);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(assign_root);
        return {};
    }

    var_node->install_symbol(symbol);

    StatementReturns assigns{};
    Error lex_err{}, expr_err{};

    assign_root->add_children(var_node);

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_tree(assign_root);
        return {};
    }

    // Next token must be assignment
    if (unexpected_token(TOKEN_ASSIGN, NLC_SYNTAX_ERROR)) {
        free_tree(assign_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
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

    assigns.push_back(assign_root);

    if (!continue_assign_parse(assigns)) {
        return {};
    }

    return assigns;
}

StatementReturns parse_assign() {
    Error lex_err{};

    StatementReturns assigns = sub_parse_assign();

    // Call above found error in statement.
    if (assigns.empty()) {
        return {};
    }

    if (!continue_assign_parse(assigns)) {
        return {};
    }

    if (ENFORCE_TERMINATING_SEMICOLON) {
        // Statement does not end with a semicolon
        if (unexpected_token(prev_token, TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
            free_statement_return_list(assigns);
            return {};
        }

        // At semicolon. Consume it.
        lex_err = munch();
        skip_if_lexerr(lex_err);
    }

    return assigns;
}

// If this function is called while the current token is not
// the identifier else, it returns nullptr.
StatementReturns parse_else() {
    if (!next_token.is_ident_else()) return {};

    AST_NODE* else_root = new AST_NODE(
        NODE_TYPE::ELSE,
        Scope::level()
    );

    Error lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_else, LBRACE_COUNT_ZERO)) {
        free_trees(else_root);
        return {};
    }

    if (next_token.is_semicolon()) {
        get_next_token_and_print_error();
        return {else_root};
    }

    // Grab the single statement and return
    if (!next_token.is_lbrace()) {
        AST_NODE* body = parse_implicit_block_for_one_statement();
        if (!body) {
            free_trees(else_root);
            return {};
        }
        else_root->add_children(body);
        return {else_root};
    }

    StatementReturns body = parse_block();
    if (body.empty()) {
        free_trees(else_root);
        return {};
    }

    else_root->add_all_statements(body);

    return {else_root};
}

StatementReturns parse_if() {
    AST_NODE* if_root = new AST_NODE(
        NODE_TYPE::IF,
        Scope::level()
    ); 

    Error lex_err{}, expr_err{};

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_trees(if_root);
        return {};
    }

    // Next token after if keyword must be an lparen
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR, skip_if, LBRACE_COUNT_ZERO)) {
        free_trees(if_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_trees(if_root);
        return {};
    }

    // If the next token after lparen is an rparen, we are missing an expression
    if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION, skip_if, LBRACE_COUNT_ZERO)) {
        free_trees(if_root);
        return {};
    }

    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);
    free_tree(expr);

    if (!ast_expr) {
        skip_if(LBRACE_COUNT_ZERO);
        free_trees(if_root);
        return {};
    }

    // Ensure that the conditional is indeed a conditional
    if (!ast_expr->is_type_logical()) {
        set_print_token_error(Error{}, ast_expr->token, NLC_NON_LOGICAL_CONDITION);
        skip_if(LBRACE_COUNT_ZERO);
        free_trees(ast_expr, if_root);
        return {};
    }

    // No longer need to explicitly free ast_expr
    if_root->add_child(ast_expr);

    // Token following logical expression must be a right parenthesis
    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR, skip_if, LBRACE_COUNT_ZERO)) {
        free_trees(if_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_if, LBRACE_COUNT_ZERO)) {
        free_trees(if_root);
        return {};
    }

    // Empty if statement. Parse optional else and return
    if (next_token.is_semicolon()) {
        get_next_token_and_print_error();

        // Parse else if it exists
        StatementReturns else_root = parse_else();
        if_root->add_all_statements(else_root);

        return {if_root};
    }

    // Otherwise we have at least one statement or a block. 
    // In that case, we first check if we have block.
    if (!next_token.is_lbrace()) {
        AST_NODE* body = parse_implicit_block_for_one_statement();
        if_root->add_child(body);
    } 

    else {
        StatementReturns body = parse_block();
        if_root->add_all_statements(body);
    }

    // If there exists an else, parse it.
    StatementReturns else_root = parse_else();
    if_root->add_all_statements(else_root);

    return {if_root};
}

StatementReturns parse_while() {
    int lbrace_count{};
    Error lex_err{}, expr_err{};

    AST_NODE* while_root = new AST_NODE(
        NODE_TYPE::WHILE,
        Scope::level()
    );

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_while, lbrace_count)) {
        free_trees(while_root);
        return {};
    }

    // Missing (
    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR, skip_while, lbrace_count)) {
        free_trees(while_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_while, lbrace_count)) {
        free_trees(while_root);
        return {};
    }

    // Missing condition
    if (wrong_next_token(TOKEN_RPAREN, NLC_EXPECTED_EXPRESSION, skip_while, lbrace_count)) {
        free_trees(while_root);
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
        free_trees(while_root);
        return {};
    }

    // Condition was not boolean
    if (!ast_expr->is_type_logical()) {
        set_print_token_error(Error{}, ast_expr->token, NLC_NON_LOGICAL_CONDITION);
        skip_while(lbrace_count);
        free_trees(ast_expr, while_root);
        return {};
    }

    while_root->add_child(ast_expr);

    // Token after condition was not a right parenthesis
    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR, skip_while, lbrace_count)) {
        free_trees(while_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_while, lbrace_count)) {
        free_trees(while_root);
        return {};
    }

    // Explicitly empty while 
    if (next_token.is_semicolon()) {
        get_next_token_and_print_error();

        // Useless stack frame attachment.
        return {while_root};
    }

    // Check if we have a block ({...})
    if (!next_token.is_lbrace()) {
        AST_NODE* body = parse_implicit_block_for_one_statement();
        while_root->add_children(body);
        return {while_root};
    }

    StatementReturns body_statements = parse_block();
    while_root->add_all_statements(body_statements);

    if (body_statements.empty()) {
        free_trees(while_root);
        return {};
    }

    return {while_root};
}

StatementReturns parse_block() {
    Scope::enter_level();
    AST_NODE* block_root = new AST_NODE(
        NODE_TYPE::BLOCK,
        Scope::level()
    );

    Error lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_block, LBRACE_COUNT_ONE)) {
        free_trees(block_root);

        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {};
    }

    // Empty block
    if (next_token.is_block_end()) {
        get_next_token_and_print_error();

        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {block_root}; 
    }
    
    Error err{};
    StatementReturns statements = get_all_statements_in_block(err);
    if (err.is_not_ok()) {
        skip_block(LBRACE_COUNT_ONE);
        free_trees(block_root);

        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {};
    }

    block_root->set_scope_stack_frame(Scope::get_top_bucket()); 
    block_root->add_all_statements(statements);

    // Only block_root is attached to program_tree. All statements
    // encountered here are attached to block_root.
    Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
    return {block_root};
}

AST_NODE* parse_implicit_block_for_one_statement() {
    Scope::enter_level();

    AST_NODE* block = new AST_NODE(NODE_TYPE::BLOCK, Scope::level());

    StatementReturns statement = get_statement();

    SymbolBucket frame = Scope::get_top_bucket();

    block->set_scope_stack_frame(frame);
    block->add_all_statements(statement);

    Scope::tear_down_frame(frame, Scope::level());

    return block;
}

StatementReturns parse_fn() {
    AST_NODE* fn_root = new AST_NODE(
        NODE_TYPE::FUNCTION,
        Scope::level()
    );

    Error lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        free_trees(fn_root);
        return {};
    }

    if (next_token.is_not_ident() || next_token.is_ident_reserved()) {
        set_print_token_error(Error{}, NLC_INVALID_IDENTIFIER);
        skip_fn(LBRACE_COUNT_ZERO);
        free_trees(fn_root);
        return {};
    }

    std::string fn_name = next_token.identifier;
    AST_NODE* fn_name_node = new AST_NODE(next_token, NODE_TYPE::FUNCTION_IDENT, Scope::level());
    SYMINFO* fn_symbol = new SYMINFO(fn_name, SYMTYPE::FN, Scope::level());
    SYMINFO* entry = SYMTABLE::add_symbol(fn_symbol);

    if (!entry) {
        set_print_token_error(Error{}, NLC_SYMBOL_ALREADY_EXISTS);
        skip_fn(LBRACE_COUNT_ZERO);
        free_trees(fn_root, fn_name_node);
        delete fn_symbol;
        return {};
    }

    fn_name_node->install_symbol(entry);
    fn_root->add_children(fn_name_node);

    // NOTE: entry is not in this new bucket, must remove it on errors
    // manually
    Scope::enter_function();

    auto fail_in_function_scope = [&]() -> StatementReturns {
        SymbolBucket frame = Scope::get_top_bucket();

        Scope::tear_down_frame(frame, Scope::level());
        free_frame_symbols(frame);

        SYMTABLE::remove_symbol(entry);
        delete entry;

        free_trees(fn_root);
        return {};
    };

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        return fail_in_function_scope();
    }

    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR, skip_fn)) {
        return fail_in_function_scope();
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        return fail_in_function_scope();
    }

    AST_NODE* parameter_pack = get_parameter_pack();

    if (!parameter_pack) {
        return fail_in_function_scope();
    }

    // Throw parameters into symbol table 
    for (auto& parameter : parameter_pack->children) {
        SYMINFO* symbol = new SYMINFO(
            parameter->token.identifier,
            parameter->data_type,
            SYMTYPE::VAR, 
            Scope::level()
        );
        SYMINFO* syminfo = SYMTABLE::add_symbol(symbol);

        // Duplicate parameter name
        if (!syminfo) {
            delete symbol;
            set_print_token_error(Error{}, NLC_SYMBOL_ALREADY_EXISTS);

            return fail_in_function_scope();
        }
        parameter->install_symbol(syminfo);
    }

    fn_root->add_children(parameter_pack);

    if (unexpected_token(TOKEN_ARROW, NLC_SYNTAX_ERROR, skip_fn)) {
        return fail_in_function_scope();
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        return fail_in_function_scope();
    }

    if (next_token.is_not_type()) {
        set_print_token_error(Error{}, NLC_EXPECTED_RETURN_TYPE);
        skip_fn(LBRACE_COUNT_ZERO);

        return fail_in_function_scope();
    }

    AST_NODE* return_value = 
        new AST_NODE(
        next_token, 
        NODE_TYPE::RETURN_VALUE,
        next_token.get_type(),
        Scope::level()
    );

    fn_root->add_children(return_value);

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        return fail_in_function_scope();
    }
    
    // Function with no definition. I.e a declaration.
    if (next_token.is_semicolon()) {
        AST_NODE* block = new AST_NODE(Token{},
            NODE_TYPE::BLOCK,
            Scope::level()
        );
        block->set_scope_stack_frame(Scope::get_top_bucket());
        fn_root->add_children(block);
        entry->install_function(fn_name, parameter_pack, return_value, block);

        get_next_token_and_print_error();
        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {fn_root};
    }

    if (unexpected_token(TOKEN_LBRACE, NLC_EXPECTED_FN_BODY, skip_fn)) {
        return fail_in_function_scope();
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_fn)) {
        return fail_in_function_scope();
    }

    // Empty function
    if (next_token.is_rbrace()) {
        AST_NODE* block = new AST_NODE(Token{},
            NODE_TYPE::BLOCK,
            Scope::level()
        );
        block->set_scope_stack_frame(Scope::get_top_bucket());
        fn_root->add_children(block);
        entry->install_function(fn_name, parameter_pack, return_value, block);

        get_next_token_and_print_error();
        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {fn_root};
    }

    // Note: Once a function body is attached and given the scope frame,
    // parameter symbols are managed by that block node.

    AST_NODE* fn_body = new AST_NODE(
        Token{}, 
        NODE_TYPE::BLOCK,
        Scope::level()
    );

    fn_root->add_children(fn_body);
    entry->install_function(fn_name, parameter_pack, return_value, fn_body);

    // Otherwise get all statements in body
    Error body_errors{};
    StatementReturns body = get_all_statements_in_block(body_errors);

    if (body_errors.is_not_ok()) {
        return fail_in_function_scope();
    }

    fn_body->add_all_statements(body);
    fn_body->set_scope_stack_frame(Scope::get_top_bucket());

    // Get the type of the return statement
    for (auto& statement : fn_body->children) {
        if (statement->is_return_statement()) {
            entry->set_fn_return_type(statement->children.front()->data_type);
        }
    }

    for (auto& symbol : fn_body->get_scope_stack_frame()) {
        symbol->set_in_function();
        symbol->set_location_type_stack();
    }

    stack_locals_layout(fn_body->get_scope_stack_frame(), parameter_pack);

    Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
    return {fn_root};
}

StatementReturns parse_call() {
    AST_NODE* call_root = new AST_NODE(
        NODE_TYPE::CALL,
        Scope::level()
    );

    Error lex_err{};
    if (next_token.is_ident_call()) {
        lex_err = munch();
        if (skip_if_lexerr(lex_err)) {
            free_trees(call_root);
            return {};
        }
    } 

    if (next_token.is_not_ident() || next_token.is_ident_reserved()) {
        set_print_token_error(Error{}, NLC_INVALID_IDENTIFIER);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_trees(call_root);
        return {};
    }

    std::string fn_name = next_token.identifier;
    AST_NODE* fn_name_node = new AST_NODE(
        next_token,
        NODE_TYPE::FUNCTION_IDENT,
        Scope::level()
    );

    call_root->add_children(fn_name_node);

    SYMINFO* syminfo = SYMTABLE::get_symbol(
        fn_name,
        SYMTYPE::FN,
        Scope::level()
    );

    if (!syminfo) {
        set_print_token_error(Error{}, NLC_INVALID_IDENTIFIER);
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_trees(call_root, fn_name_node);
        return {};
    }

    fn_name_node->install_symbol(syminfo);

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_trees(call_root);
        return {};
    }

    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR)) {
        free_trees(call_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_trees(call_root);
        return {};
    }

    AST_NODE* argument_pack = get_argument_pack();
    if (!argument_pack) {
        free_trees(call_root);
        return {};
    }

    call_root->add_children(argument_pack);

    if (!match_packs(syminfo->function_info.parameter_pack, argument_pack)) {
        print_error(
            Error{NLC_INVALID_ARGPACK,
            fn_name_node->token.line_no,
            fn_name_node->token.col_no}
        );
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_trees(call_root);
        return {};
    }

    return {call_root};
}

StatementReturns parse_return() {
    AST_NODE* return_root = new AST_NODE(
        NODE_TYPE::RETURN,
        Scope::level()
    );

    Error lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_trees(return_root);
        return{};
    }

    Error expr_err{};
    AST_NODE* expr = A(expr_err);
    AST_NODE* ast_expr = pttoast(expr);
    free_trees(expr);

    if (!ast_expr) {
        onepast_semi_or_block(LBRACE_COUNT_ZERO);
        free_tree(return_root);
        return {};
    }

    return_root->add_children(ast_expr);

    if (unexpected_token(prev_token, TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON)) {
        free_trees(return_root);
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err)) {
        free_trees(return_root);
        return{};
    }

    return {return_root};
}

StatementReturns parse_for() {
    Scope::enter_level();

    AST_NODE* for_root = new AST_NODE(
        NODE_TYPE::FOR,
        Scope::level()
    );

    Error lex_err{}, expr_err{};

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_for)) {
        free_trees(for_root);

        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {};
    }

    if (unexpected_token(TOKEN_LPAREN, NLC_SYNTAX_ERROR, skip_for)) {
        free_trees(for_root);

        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_for)) {
        free_trees(for_root);

        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {};
    }

    AST_NODE* init = new AST_NODE(NODE_TYPE::FOR_INIT, Scope::level()),
            * cond = new AST_NODE(NODE_TYPE::FOR_COND, Scope::level()),
            * update = new AST_NODE(NODE_TYPE::FOR_UPDATE, Scope::level()),
            * block = new AST_NODE(NODE_TYPE::BLOCK, Scope::level());

    for_root->add_children(init, cond, update, block);

    // Initializer
    if (next_token.is_semicolon()) {
        lex_err = munch();
        if (skip_if_lexerr(lex_err, skip_for)) {
            free_trees(for_root);

            SymbolBucket frame = Scope::get_top_bucket();
            Scope::tear_down_frame(frame, Scope::level());
            return {};
        }
    }

    else {
        // Get statement will eat the semicolon after the statement.
        StatementReturns init_statements = get_statement();

        if (init_statements.empty() || !verify_init_or_assign(init_statements)) {
            // If init_statements is empty, error was handled by 
            // get_statement.
            if (init_statements.size()) {
                set_print_token_error(Error{}, NLC_BAD_INITIALIZATION);
            }

            skip_for(LBRACE_COUNT_ZERO);
            free_trees(for_root);

            SymbolBucket frame = Scope::get_top_bucket();
            Scope::tear_down_frame(frame, Scope::level());
            return {};
        }

        init->add_all_statements(init_statements);
    }

    // Condition. Note that valid expressions will leave us at the 
    // semicolon after the expression.
    if (next_token.is_not_semicolon()) {
        AST_NODE* expr = A(expr_err);
        AST_NODE* ast_expr = pttoast(expr);
        free_trees(expr);

        if (!ast_expr || !ast_expr->is_type_logical()) {
            set_print_token_error(Error{}, NLC_BAD_CONDITION);
            skip_for(LBRACE_COUNT_ZERO);
            free_trees(for_root, ast_expr);

            SymbolBucket frame = Scope::get_top_bucket();
            Scope::tear_down_frame(frame, Scope::level());
            return {};
        }

        cond->add_children(ast_expr);
    }

    if (unexpected_token(TOKEN_SEMICOLON, NLC_EXPECTED_SEMICOLON, skip_for)) {
        free_trees(for_root);

        SymbolBucket frame = Scope::get_top_bucket();
        Scope::tear_down_frame(frame, Scope::level());
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_for)) {
        free_trees(for_root);

        SymbolBucket frame = Scope::get_top_bucket();
        Scope::tear_down_frame(frame, Scope::level());
        return {};
    }

    bool old_enforce = ENFORCE_TERMINATING_SEMICOLON;
    ENFORCE_TERMINATING_SEMICOLON = false;

    // Update
    if (next_token.is_not_rparen()) {
        StatementReturns update_statement = get_statement();

        ENFORCE_TERMINATING_SEMICOLON = old_enforce;

        // Error handled by get_statement
        if (update_statement.empty()) {
            skip_for(LBRACE_COUNT_ZERO);
            free_trees(for_root);

            SymbolBucket frame = Scope::get_top_bucket();
            Scope::tear_down_frame(frame, Scope::level());
            return {};
        }

        update->add_all_statements(update_statement);
    }

    ENFORCE_TERMINATING_SEMICOLON = old_enforce;

    if (unexpected_token(TOKEN_RPAREN, NLC_SYNTAX_ERROR, skip_for)) {
        free_trees(for_root);

        SymbolBucket frame = Scope::get_top_bucket();
        Scope::tear_down_frame(frame, Scope::level());
        return {};
    }

    lex_err = munch();
    if (skip_if_lexerr(lex_err, skip_for)) {
        free_trees(for_root);

        SymbolBucket frame = Scope::get_top_bucket();
        Scope::tear_down_frame(frame, Scope::level());
        return {};
    }

    // Immediately terminated
    if (next_token.is_semicolon()) {
        for_root->set_scope_stack_frame(Scope::get_top_bucket());
        get_next_token_and_print_error();

        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        return {for_root};
    }

    Error body_err{};
    StatementReturns body_statements = process_loop_statements(body_err);
    block->add_all_statements(body_statements);

    if (body_err.is_not_ok()) {
        Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
        free_trees(for_root);
        return {}; 
    }

    for_root->set_scope_stack_frame(Scope::get_top_bucket());

    Scope::tear_down_frame(Scope::get_top_bucket(), Scope::level());
    return {for_root};
}

// A -> BA'
AST_NODE* A(Error& err) {
    AST_NODE* here{}, *left{}, *right{};

    // FIRST(BA')
    if (next_token.in(First::BAP)) {
        here = new AST_NODE(Scope::level());

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
        here = new AST_NODE(next_token, NODE_TYPE::OR, OPERATOR, Scope::level());

        // Consume the or token
        Error tmp_error = munch();
        if (handle_lex_error(tmp_error)) {
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
        here = new AST_NODE(Scope::level());

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
        here = new AST_NODE(next_token, NODE_TYPE::AND, OPERATOR, Scope::level());

        // Consume and token
        Error tmp_error = munch();
        if (handle_lex_error(tmp_error)) {
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
        here = new AST_NODE(next_token, NODE_TYPE::NOT, OPERATOR, Scope::level());

        // Consume not token
        Error tmp_error = munch();
        if (handle_lex_error(tmp_error)) {
            err = tmp_error; 
            free_tree(here);
            return nullptr;
        }

        left = C(err);
    }

    // t \in FIRST(D)
    else if (next_token.in(First::D)) {
        here = new AST_NODE(Scope::level());

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
        here = new AST_NODE(Scope::level());

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
        here = new AST_NODE(next_token, get_node_type(next_token), OPERATOR, Scope::level());

        // Consume the operator
        Error tmp_error = munch();
        if (handle_lex_error(tmp_error)) {
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
            Error tmp_err = munch();
            if (handle_lex_error(tmp_err)) {
                err = tmp_err;
                free_tree(left);
            	return nullptr;
            }

            AST_NODE* here = new AST_NODE(op, get_node_type(op), OPERATOR, Scope::level());
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

            Error tmp_err = munch();
            if (handle_lex_error(tmp_err)) {
                err = tmp_err;
                free_tree(left);
            	return nullptr;
            }

            AST_NODE* node = new AST_NODE(op, get_node_type(op), OPERATOR, Scope::level());
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
        here = new AST_NODE(next_token, get_node_type(next_token), OPERATOR, Scope::level());

        Error tmp_error = munch();
        if (handle_lex_error(tmp_error)) {
            err = tmp_error;
            free_tree(here);
        	return nullptr;
        }
    
        left = F(err);
    } 

    // t \in FIRST(F)
    else if (next_token.in(First::F)) {
        here = new AST_NODE(Scope::level());

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
        here = new AST_NODE(Scope::level());

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
        here = new AST_NODE(next_token, get_node_type(next_token), OPERATOR, Scope::level());

        // Consume operator exp
        Error tmp_error = munch();
        if (handle_lex_error(tmp_error)) {
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
    AST_NODE* here = new AST_NODE(next_token, NODE_TYPE::INT, TYPE::INT, Scope::level());

    Error tmp_error = munch();
    if (handle_lex_error(tmp_error)) {
        err = tmp_error;
        free_tree(here);
        return nullptr;
    }

    return here;
}

AST_NODE* paren_expression(Error& err, AST_NODE*& left) {
    AST_NODE* here = new AST_NODE(Scope::level());

    // Eat left parenthesis
    Error tmp_error = munch();
    if (handle_lex_error(tmp_error)) {
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
        Error tmp_error = munch();
        if (handle_lex_error(tmp_error)) {
            err = tmp_error;
            free_tree(here);
            return nullptr;
        }
    }

    return here;
}

AST_NODE* string_terminal(Error& err) {
    AST_NODE* here = new AST_NODE(next_token, NODE_TYPE::STR, TYPE::STRING, Scope::level());

    STR_TABLE_ENTRY entry = STR_TABLE::add_string(next_token.str);

    // Attempted to overrun the internal string table
    if (entry.is_not_valid()) {
        free_tree(here);
        return nullptr;
    }

    here->entry = entry;

    Error tmp_error = munch();
    if (handle_lex_error(tmp_error)) {
        err = tmp_error;
        free_tree(here);
        return nullptr;
    }

    return here;
}

AST_NODE* boolean_terminal(Error& err) {
    AST_NODE* here = new AST_NODE(next_token, NODE_TYPE::BOOL, TYPE::BOOL, Scope::level()); 

    Error tmp_error = munch();
    if (handle_lex_error(tmp_error)) {
        err = tmp_error;
        free_tree(here);
        return nullptr;
    }

    return here;
}

AST_NODE* variable_terminal(Error& err) {
    AST_NODE* here = new AST_NODE(next_token, NODE_TYPE::VAR, TYPE::null, Scope::level());

    // Search the symbol table, targeting variables.
    SYMINFO* syminfo = SYMTABLE::get_symbol(next_token.identifier, SYMTYPE::VAR, Scope::level());

    // Try again but this time search for function
    if (!syminfo) {
        syminfo = SYMTABLE::get_symbol(next_token.identifier, SYMTYPE::FN, Scope::level());
    }

    // Still nothing
    if (!syminfo) {
        set_print_token_error(Error{}, NLC_UNKNOWN_VARIABLE);
        free_tree(here);
        return nullptr;
    }

    here->install_symbol(syminfo);

    if (syminfo->is_function()) {
        StatementReturns call_root = parse_call();
        if (call_root.empty()) 
            return nullptr;

        delete here;
        here = call_root.front();
        here->data_type = syminfo->get_fn_return_type();
        return here;
    }

    else {
        Error tmp_error = munch();
        if (handle_lex_error(tmp_error)){
            err = tmp_error;
            free_tree(here);
            return nullptr;
        }
    }

    return here;
}
