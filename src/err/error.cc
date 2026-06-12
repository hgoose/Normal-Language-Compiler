#include "error.h"
#include "parser.h"
#include "buffio.h"
#include "lex.h"
#include "parserutils.h"
#include "error_structures.h"

#include <iostream>

// True if error is critical, false otherwise
bool handle_lex_error(const Error& err) {
    if (err.is_not_critical()) return false;

    print_error(err);
    return false;
}

// Prints error message. If the error is from the lexer, output message, line, and column number.
// If the error is not from the lexer, just output the message.
// Note that OK, EOF, and BOF is not technically an error, nothing should be reported
void print_error(const Error& e) {
    if (SUPPRESS_PARSER_ERRORS || e.is_not_critical()) return;

    // These shouldn't print a line and column number
    if (e.is_file_error() || e.is_unknown()) {
        std::cerr << error_string(e.error) << '\n';
        return;
    }

    // Otherwise, lexer error, print error message, line, and column
    std::cerr << (e.is_syntax_error() ? "" : "error: ") 
        << error_string(e.error) 
        << " at line " << e.line 
        << ", column " << e.col << '\n'; 

    // More verbose error reporting, reports source line and shows column position
    std::string line{};
    if (create_error(get_src_line(e.line, line)).is_ok()) {
        std::cerr << line << '\n';
        for (int i=1; i<e.col; ++i) {
            std::cerr << '-';
        }
        std::cerr << "^\n\n";
    }
}

void get_next_token_and_print_error() {
    Error e = munch();
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

Error create_error(ErrorValue error_value) {
    return Error{error_value};
}

void Error::set_error(ErrorValue error_value) {
    error = error_value;
}

void Error::set_line_and_column(int line_no, int col_no) {
    line = line_no;
    col = col_no;
}

bool Error::is(ErrorValue error_value) const {
    return error == error_value;
}

bool Error::is_not(ErrorValue error_value) const {
    return !is(error_value);
}

bool Error::is_eof() const {
    return is(NLC_EOF);
}

bool Error::is_not_eof() const {
    return !is_eof();
}

bool Error::is_bof() const {
    return is(NLC_BOF);
}

bool Error::is_ok() const {
    return is(NLC_OK);
}

bool Error::is_not_ok() const {
    return !is_ok();
}

bool Error::is_not_critical() const {
    return is_eof() || is_bof() || is_ok();
}

bool Error::is_critical() const {
    return !is_not_critical();
}

bool Error::is_unexpected_eof() const {
    return is(NLC_UNEXPECTED_EOF);
}

bool Error::is_file_error() const {
    return is(NLC_FILE_NOT_FOUND) || is(NLC_IO_ERROR);    
}

bool Error::is_syntax_error() const {
    return is(NLC_SYNTAX_ERROR);
}

bool Error::is_unknown() const {
    return error_unknown(error); 
}
