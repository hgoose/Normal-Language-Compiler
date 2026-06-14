#include "token.h"
#include "error.h"
#include "buffio.h"
#include "util.h"
#include "lex.h"
#include "lexstate.h"
#include "parser.h"
#include "types.h"
#include "lex_structures.h"
#include "lex_utils.h"

#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace {
    Token last_token{};
}

// Checks for escape sequence and replaces character
static ReturnCode check_replace_escape(char& c, std::string& hex) {
    char next{};

    if (create_error(buffer_get_next_char(next)).is_eof()) {
        return NLC_UNEXPECTED_EOF;
    }

    if (next == 'u') {
        // Try to consume the next six, should be hex digits.
        if (create_error(buffer_consume_k(6,hex)).is_eof()) {
            return NLC_UNEXPECTED_EOF;
        }

        // Get the utf8 rep
        hex = hex6_to_utf8(hex);

        return NLC_OK;
    }

    if (!replace_char_with_escape(c, next)) {
        return NLC_ILLEGAL_ESCAPE;
    }

    return NLC_OK;
}

// Initializes lexer
Error lex_init(const char* src_code) {
    // Call buffer init and get the error code
    ReturnCode rc = buffer_init(src_code); 
    return Error{rc, src_line_no, src_col_no};
}

// Gets the next token
Error get_token(Token& t) {
    // Reset token
    t.reset();
    t.set_line_and_col(src_line_no, src_col_no);

    Error err{};

    char curr_char{};

    // Either we get first valid (non white space) character, or hit EOF
    Error error = create_error(buffer_get_next_char(curr_char));

    if (error.is_eof()) {
        t.set_eof();
        t.set_line_and_col(src_line_no, src_col_no);

        return Error{};
    }

    while (char_is_whitespace(curr_char)) {
        Error error = create_error(buffer_get_next_char(curr_char));

        if (error.is_eof()) {
            t.set_eof();
            t.set_line_and_col(src_line_no, src_col_no);

            return error;
        }
    }

    t.set_line_and_col(src_line_no, src_col_no);

    // Add to lexeme string, we don't want to push the opening quote for a string though
    if (curr_char != '"') t.append_lexeme(curr_char);

    if (char_in_simple_lex_set(curr_char)) {
        t.set_id(char_to_token.at(curr_char));
    }

    else if (curr_char == '+') {
        t = last_token;
        // Either unary plus or plus
        if (!t.set_id_from_predicate(TOKEN_UPLUS, &Token::legal_before_uplus_or_uneg)) {
            t.set_id(TOKEN_PLUS);
        }

        t.set_lexeme("+");
    } 

    else if (curr_char == '-') {
        error = create_error(buffer_get_next_char(curr_char));

        if (error.is_eof() || curr_char != '>') {
            buffer_back_char();

            t = last_token;
            // Either unary negation or minus
            if (!t.set_id_from_predicate(TOKEN_UNEG, &Token::legal_before_uplus_or_uneg)) {
                t.set_id(TOKEN_MINUS);
            }

            t.set_lexeme("-");
        }

        else if (curr_char == '>') {
            t.set_id(TOKEN_ARROW);
            t.append_lexeme('>');
        }
    } 

    // Possible identifier
    else if (valid_identifier_start(curr_char)) {
        err = lex_char_alpha(t, curr_char);
    } 

    else if (is_digit(curr_char)) {
        err = lex_char_numeric(t, curr_char);
    }

    else {
        LexMethod lex_method = map_char_to_lex_method(curr_char);
        if (lex_method) {
            err = lex_method(t, curr_char);
        }
    }

    last_token = t;
    return err;
}

// Read past end of input stream?
bool lex_eof(void) {
    return buffer_eof();
}

// Cleanup the buffer
void lex_cleanup() {
    buffer_cleanup(); 
}

LexState lex_save() {
    BufState bufstate = buffer_save_state(); 
    return LexState{next_token, bufstate};
}

void lex_goto_last_save(const LexState& state) {
    next_token = state.next_token;
    buffer_load_state(state.bufstate);
}

Error lex_char_less(Token& t, char& curr_char) {
    return token_either_this_or_that(
        t, TOKEN_LESS_EQ, TOKEN_LESS, curr_char, '='
    );
}

Error lex_char_greater(Token& t, char& curr_char) {
    return token_either_this_or_that(
        t, TOKEN_GREATER_EQ, TOKEN_GREATER, curr_char, '='
    );
}

Error lex_char_equal(Token& t, char& curr_char) {
    return token_either_this_or_that(
        t, TOKEN_EQUAL, TOKEN_ASSIGN, curr_char, '='
    );
}

Error lex_char_not(Token& t, char& curr_char) {
    return token_either_this_or_that(
        t, TOKEN_NOT_EQUAL, TOKEN_NOT, curr_char, '='
    );
}

Error lex_char_slash(Token& t, char& curr_char) {
    Error err = create_error(buffer_get_next_char(curr_char));

    // Block comment
    if (curr_char == '*') {
        Error block_comment_error = get_end_of_block_comment(curr_char);

        if (block_comment_error.is_not_ok()) {
            return block_comment_error;
        }
    }

    if (err.is_eof() || curr_char != '/') {
        buffer_back_char();
        t.set_id(TOKEN_DIV);

        return Error{};
    }

    for (;;) {
        Error err = create_error(buffer_get_next_char(curr_char));

        if (curr_char == '\n') break;

        // Must end with newline, otherwise it's unexpected EOF
        if (err.is_eof()) {
            return Error{NLC_UNEXPECTED_EOF, src_line_no, src_col_no};
        }
    }

    return get_token(t);
}

Error lex_char_alpha(Token& t, char& curr_char) {
    // Assume identifier
    t.set_id(TOKEN_IDENT);

    // Start consuming characters to build identifier until whitespace
    for (;;) {
        Error err = create_error(buffer_get_next_char(curr_char));

        if (err.is_eof() || !valid_identifier_poststart(curr_char)) {
            buffer_back_char();
            t.set_identifier(t.lexeme);

            break;
        } 

        t.append_lexeme(curr_char);
    } 

    t.set_mod_if_mod();
    return Error{};
}

Error lex_char_quote(Token& t, char& curr_char) {
    for (;;) {
        Error err = create_error(buffer_get_next_char(curr_char));

        if (curr_char == '"') {
            break;
        }

        // Reached EOF before closing string
        if (err.is_eof()) {
            return Error{NLC_UNEXPECTED_EOF, src_line_no, src_col_no};
        }

        std::string hex{};
        if (curr_char == '\\') {
            err = create_error(check_replace_escape(curr_char, hex));
        }

        if (err.is_unexpected_eof()) {
            return Error{NLC_UNEXPECTED_EOF, src_line_no, src_col_no};
        } 

        else if (err.is(NLC_ILLEGAL_ESCAPE)) {
            return Error{NLC_ILLEGAL_ESCAPE, src_line_no, src_col_no};
        }

        // Escape was unicode \u
        if (!hex.empty()) {
            t.append_lexeme(hex);
            continue;
        }

        t.append_lexeme(curr_char);
    }

    t.set_id(TOKEN_STRING);
    t.set_str(t.lexeme);

    return Error{};
}

Error lex_char_dot(Token& t, char& curr_char) {
    Error err = create_error(buffer_get_next_char(curr_char));

    if (!is_digit(curr_char)) {
        buffer_back_char();
        t.set_id(TOKEN_DOT);

        return Error{};
    }

    t.set_id(TOKEN_REAL);
    t.set_lexeme("0");
    
    // Want to call build_float_after_dot while on dot
    buffer_get_back_char(curr_char);
    err = build_float_after_dot(t, curr_char);

    if (err.is_not_ok()) {
        t.set_null();

        return err;
    }

    t.set_float(std::stod(t.lexeme));

    return Error{};
}

Error lex_char_numeric(Token& t, char& curr_char) {
    t.append_lexeme(consume_digits_after_current(curr_char));

    // Decide if real either dot follows or exponent follows
    bool saw_dot = false;
    bool saw_frac_digit = false;

    // Check for dot
    Error err = create_error(buffer_get_next_char(curr_char));

    if (err.is_eof() || (!char_is_radix_point(curr_char) && !char_is_exponent(curr_char))) {
        t.set_id(TOKEN_INTEGER);
        t.set_integer(std::stoi(t.lexeme));
        buffer_back_char();

        return Error{};
    }

    if (char_is_exponent(curr_char)) {
        err = handle_exponent(t,curr_char);

        if (err.is_not_ok()) {
            t.set_null();
            return err;
        }

        t.set_id(TOKEN_REAL);
        t.set_float(std::stod(t.lexeme));
        return Error{};
    }

    // Call while on dot. The call will append the dot to the lexeme
    err = build_float_after_dot(t, curr_char);

    if (err.is(NLC_INVALID_NUMBER)) {
        t.set_null();
        return err;
    }

    if (err.is(NLC_NO_DIGITS)) {
        t.set_id(TOKEN_INTEGER);
        t.set_integer(std::stoi(t.lexeme));
        buffer_back_char();

        return Error{};
    }

    t.set_id(TOKEN_REAL);
    t.set_float(std::stod(t.lexeme));

    return Error{};
}
