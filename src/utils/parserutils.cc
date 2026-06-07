#include "parserutils.h"
#include "error.h"
#include "parser.h"
#include "lex.h"

#include <string>
#include <cstdint>
#include <algorithm>
#include <functional>

// Skips to the next semicolon
void goto_next_semicolon() {
    while (!next_token.is_semicolon() && !next_token.is_eof()) {
        get_next_token_and_print_error();
    } 
}

void onepast_next_token(int token) {
    while (next_token.id != token && !next_token.is_eof()) {
        get_next_token_and_print_error();
    }
    get_next_token_and_print_error();
}

// Moves one past next semicolon (or to eof). Prints
// errors as it finds them.
void onepast_next_semicolon(int sh) {
    onepast_next_token(TOKEN_SEMICOLON);
}

void onepast_next_rbrace(int sh) {
    onepast_next_token(TOKEN_RBRACE);
}

// Skips to the next rbrace
void goto_next_rbrace() {
    while (!next_token.is_rbrace() && !next_token.is_eof()) {
        get_next_token_and_print_error();
    } 
}

// Skips to the next semicolon or else identifier
void goto_semi_or_else() {
    do {
        get_next_token_and_print_error();
    } while (next_token.id != TOKEN_SEMICOLON &&
            next_token.identifier != "else"
    );
}

// Skips entirety of if statement
void skip_if() {
//     int lbrace_count{};
//     Error e{};
//
//     // Either we reach eof, semicolon, or an lbrace
//     while (next_token.id != TOKEN_SEMICOLON && next_token.id != TOKEN_EOF) {
//         // Found an lbrace
//         if (next_token.id == TOKEN_LBRACE) {
//             ++lbrace_count;
//             break;
//         }
//
//         get_next_token_and_print_error();
//     }
//     
//     e = get_token(next_token);
//
//     if (next_token.id == TOKEN_EOF) {
//         return;
//     }
//
//     // Move one past lbrace and call skip_block
//     else {
//         print_error(e);
//         skip_block();
//     }
//
//     if (next_token.identifier != "else") {
//         get_next_token_and_print_error();
//     } else {
//         get_next_token_and_print_error();
//
//         if (next_token.id == TOKEN_LBRACE) {
//             skip_block();
//         } else {
//             goto_next_semicolon();
//         }
//
//         get_next_token_and_print_error();
//     }
}

// Skips the block (ends at the last rbrace)
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

void skip_while(int lbrace_count) {
    if (lbrace_count != 0) {
        skip_block(lbrace_count);
    }

    while (next_token.id != TOKEN_SEMICOLON 
            && next_token.id != TOKEN_LBRACE
            && next_token.id != TOKEN_EOF
    ) {
        get_next_token_and_print_error();
    }

    if (next_token.id == TOKEN_EOF) return;

    if (next_token.id == TOKEN_SEMICOLON) {
        get_next_token_and_print_error();
        return;
    } else ++lbrace_count;

    skip_block(lbrace_count);
}

// If the next token is invalid or a lexer error occurred,
// skip to start of next statement or eof.
bool skip_if_invalid_or_lexerr(const Error& err, MOVE_PROCEDURE mv_proc, int lbrace_count){
    if (invalid_lookahead() || handle_lex_error(err)) {
        mv_proc(lbrace_count);
        return true;
    } 
    return false;
}

// Anything other than the expected token is an error.
// Return true to indicate that the next_token is unexpected.
bool unexpected_token(int expected_token, int error_to_submit, MOVE_PROCEDURE mv_proc, int lbrace_count){
    if (next_token.id != expected_token) {
        set_print_token_error(Error{}, error_to_submit);
        mv_proc(lbrace_count);
        return true;

    }

    return false;
}

// If the next_token is wrong_token, error.
// Return true to indicate that the next_token is wrong
bool wrong_next_token(int wrong_token, int error_to_submit, MOVE_PROCEDURE mv_proc, int lbrace_count){
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

// Free parse tree nodes
void inhouse_cleanup(AST_NODE*& parse_tree) {
    free_tree(parse_tree);
}

// Free all nodes created for the AST and parse tree
void parser_cleanup() {
    lex_cleanup();
}
