// Nate warner z2004109
// error.cc

#include "error.h"
#include "parser.h"
#include "buffio.h"

#include <iostream>
using std::cerr;

// Returns the error message for a given error code
const char* error_string(int err) {
    // Run through the cases
    switch (err) {
        case NCC_FILE_NOT_FOUND: return "file not found";
        case NCC_UNEXPECTED_EOF: return "unexpected eof";
        case NCC_IO_ERROR: return "I/O error";
        case NCC_NOT_FOUND: return "line not found";
        case NCC_ILLEGAL_COMMENT: return "illegal comment";
        case NCC_ILLEGAL_ESCAPE:  return "illegal escape";
        case NCC_UNDEFINED_TOKEN: return "undefined token";
        case NCC_INVALID_NUMBER: return "invalid number";
        case NCC_SYNTAX_ERROR:   return "Syntax error";
        case NCC_EXPECTED_RPAREN: return "expected )";
        case NCC_EXPECTED_EXPRESSION: return "expected expression, none provided";
        case NCC_EXPECTED_STATEMENT: return "expected statement, none provided";
        case NCC_STR_TABLE_OVERFLOW: return "string table overrun";
        case NCC_INVALID_OPERAND_TYPE: return "invalid operand type";
        case NCC_INT_TABLE_OVERFLOW: return "integer table overrun";
        case NCC_VARIABLE_NAME_RESERVED: return "attempted to create a variable using a reserved name";
        case NCC_SYMBOL_ALREADY_EXISTS: return "variable already exists";
        case NCC_INVALID_IDENTIFIER: return "invalid identifier";
        case NCC_UNKNOWN_VARIABLE: return "variable has not be declared";
        case NCC_EXPECTED_VAR: return "expected variable, none provided";
        case NCC_UNACCEPTABLE_TYPE_MISMATCH: return "type mismatch cannot be justified";
        case NCC_NON_LOGICAL_CONDITION: return "the provided expression cannot be interpreted as logical";
        default: return "unknown error";
    }
}

// Prints error message. If the error is from the lexer, output message, line, and column number.
// If the error is not from the lexer, just output the message.
// Note that OK, EOF, and BOF is not technically an error, nothing should be reported
void print_error(const Error& e) {
    // Non error errors
    if (e.error == NCC_EOF || e.error == NCC_BOF || e.error == NCC_OK) return;

    // These shouldn't print a line and column number
    if (e.error == NCC_FILE_NOT_FOUND || e.error == NCC_IO_ERROR) {
        // Just output the error string and return
        error_string(e.error);
        return;
    }

    // Otherwise, lexer error, print error message, line, and column
    cerr << (e.error == NCC_SYNTAX_ERROR ? "" : "error: ") << error_string(e.error) << " at line " << e.line << ", column " << e.col << '\n'; 

    // More verbose error reporting, reports source line and shows column position
    
    std::string line{};
    if (get_src_line(e.line, line) == NCC_OK)  {
        cerr << line << '\n';

        for (int i=1; i<e.col; ++i) {
            cerr << '-';
        }
        cerr << "^\n\n";
    }
}

// Sets and print error, uses current parsing token
void set_print_token_error(Error& e, int error) {
    e.error = error;
    e.line = next_token.line_no;
    e.col = next_token.col_no;

    print_error(e);
}

void set_print_token_error(Error&& e, int error) {
    e.error = error;
    e.line = next_token.line_no;
    e.col = next_token.col_no;

    print_error(e);
}

// Requests a specific taken
void set_print_token_error(Error& e, const Token& token, int error) {
    e.error = error;
    e.line = token.line_no;
    e.col = token.col_no;

    print_error(e);
}

void set_print_token_error(Error&& e, const Token& token, int error) {
    e.error = error;
    e.line = token.line_no;
    e.col = token.col_no;

    print_error(e);
}
