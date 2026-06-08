#include "error.h"
#include "parser.h"
#include "buffio.h"
#include "lex.h"

#include <iostream>
using std::cerr;

// Returns the error message for a given error code
const char* error_string(int err) {
    // Run through the cases
    switch (err) {
        case NLC_FILE_NOT_FOUND: return "file not found";
        case NLC_UNEXPECTED_EOF: return "unexpected eof";
        case NLC_IO_ERROR: return "I/O error";
        case NLC_NOT_FOUND: return "line not found";
        case NLC_ILLEGAL_COMMENT: return "illegal comment";
        case NLC_ILLEGAL_ESCAPE:  return "illegal escape";
        case NLC_UNDEFINED_TOKEN: return "undefined token";
        case NLC_INVALID_NUMBER: return "invalid number";
        case NLC_SYNTAX_ERROR:   return "Syntax error";
        case NLC_EXPECTED_RPAREN: return "expected )";
        case NLC_EXPECTED_EXPRESSION: return "expected expression, none provided";
        case NLC_EXPECTED_STATEMENT: return "expected statement, none provided";
        case NLC_EXPECTED_SEMICOLON: return "missing semicolon";
        case NLC_STR_TABLE_OVERFLOW: return "string table overrun";
        case NLC_INVALID_OPERAND_TYPE: return "invalid operand type";
        case NLC_INT_TABLE_OVERFLOW: return "integer table overrun";
        case NLC_VARIABLE_NAME_RESERVED: return "attempted to create a variable using a reserved name";
        case NLC_SYMBOL_ALREADY_EXISTS: return "variable already exists";
        case NLC_INVALID_IDENTIFIER: return "invalid identifier";
        case NLC_UNKNOWN_VARIABLE: return "variable has not been declared";
        case NLC_EXPECTED_VAR: return "expected variable, none provided";
        case NLC_UNACCEPTABLE_TYPE_MISMATCH: return "type mismatch cannot be justified";
        case NLC_NON_LOGICAL_CONDITION: return "the provided expression cannot be interpreted as logical";
        default: return "unknown error";
    }
}

// Check if the current token is invalid
bool invalid_lookahead() {
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

// True if error is critical, false otherwise
bool handle_lex_error(const Error& err) {
    print_error(err);

    if (err.error != NLC_OK && err.error != NLC_EOF) {
        return true;
    }

    return false;
}

// Prints error message. If the error is from the lexer, output message, line, and column number.
// If the error is not from the lexer, just output the message.
// Note that OK, EOF, and BOF is not technically an error, nothing should be reported
void print_error(const Error& e) {
    // Non error errors
    if (e.error == NLC_EOF || e.error == NLC_BOF || e.error == NLC_OK) return;

    // These shouldn't print a line and column number
    if (e.error == NLC_FILE_NOT_FOUND || e.error == NLC_IO_ERROR) {
        // Just output the error string and return
        error_string(e.error);
        return;
    }

    // Otherwise, lexer error, print error message, line, and column
    cerr << (e.error == NLC_SYNTAX_ERROR ? "" : "error: ") << error_string(e.error) << " at line " << e.line << ", column " << e.col << '\n'; 

    // More verbose error reporting, reports source line and shows column position
    
    std::string line{};
    if (get_src_line(e.line, line) == NLC_OK)  {
        cerr << line << '\n';

        for (int i=1; i<e.col; ++i) {
            cerr << '-';
        }
        cerr << "^\n\n";
    }
}

void get_next_token_and_print_error() {
    Error e{get_token(next_token)};
    print_error(e);
}

static void set_token_error(Error& e, const Token& token, ErrorValue error) {
    e.error = error;
    e.line = token.line_no;
    e.col = token.col_no;
}

// Sets and print error, uses current parsing token
void set_print_token_error(Error& e, ErrorValue error) {
    set_token_error(e, next_token, error);

    print_error(e);
}

void set_print_token_error(Error&& e, ErrorValue error) {
    set_token_error(e, next_token, error);

    print_error(e);
}

// Requests a specific taken
void set_print_token_error(Error& e, const Token& token, ErrorValue error) {
    set_token_error(e, token, error);

    print_error(e);
}

void set_print_token_error(Error&& e, const Token& token, ErrorValue error) {
    set_token_error(e, token, error);

    print_error(e);
}
