#include "lex_utils.h"
#include "lex.h"
#include "buffio.h"
#include "error.h"
#include "token.h"
#include "types.h"
#include "lex_structures.h"
#include "token_structures.h"

#include <string>

Error skip_whitespace(char& curr_char) {
    Error err{};
    while (char_is_whitespace(curr_char) && err.is_ok()) {
        Error err = create_error(buffer_get_next_char(curr_char));
    }

    return err;
}

Error skip_current_and_whitespace(char& curr_char) {
    Error err = create_error(buffer_get_next_char(curr_char));
    if (err.is_not_ok()) {
        return err;
    }

    return skip_whitespace(curr_char);
}

Error get_end_of_block_comment(char& curr_char) {
    Error buf_error = create_error(buffer_get_next_char(curr_char));
    while (curr_char != '*') {
        if (buf_error.is_eof()) {
            return Error{NLC_UNEXPECTED_EOF, src_line_no, src_col_no};
        }

        buf_error = create_error(buffer_get_next_char(curr_char));
    }

    buf_error = create_error(buffer_get_next_char(curr_char));
    if (buf_error.is_eof()) {
        return Error{NLC_UNEXPECTED_EOF, src_line_no, src_col_no};
    }

    if (curr_char != '/') {
        get_end_of_block_comment(curr_char);
    } 

    return Error{};
}

Error token_either_this_or_that(Token& t, TokenValue if_char_is, TokenValue if_char_is_not, char& curr_char, char expected_char) {
    Error err = create_error(buffer_get_next_char(curr_char));

    if (err.is_eof() || curr_char != expected_char) {
        buffer_back_char();
        t.set_id(if_char_is_not);
        return Error{};

    }

    t.append_lexeme(curr_char);
    t.set_id(if_char_is);

    return Error{};
}

std::string consume_digits_after_current(char& curr_char) {
    std::string lexeme{};

    for (;;) {
        Error err = create_error(buffer_get_next_char(curr_char));

        if (err.is_eof() || !is_digit(curr_char)) {
            buffer_back_char();
            break;
        }

        lexeme.push_back(curr_char);
    }

    return lexeme;
}

Error handle_exponent(Token& t, char& curr_char) {
    // record exponent start position 
    int e_line = src_line_no;
    int e_col  = src_col_no;

    t.append_lexeme('e'); 

    // If we don't have a sign or a digit after exponent, number is invalid.
    Error err = create_error(buffer_get_next_char(curr_char));
    if (err.is_eof() || (!char_is_sign(curr_char) && !is_digit(curr_char))) {
        buffer_back_char();

        return Error{NLC_INVALID_NUMBER, e_line, e_col};
    }

    if (is_digit(curr_char)) {
        t.append_lexeme(curr_char);
    }

    if (char_is_sign(curr_char)) {
        t.append_lexeme(curr_char);

        int sign_line = src_line_no;
        int sign_col  = src_col_no;

        // Try to get digits after exponent
        err = create_error(buffer_get_next_char(curr_char));
        if (err.is_eof() || !is_digit(curr_char)) {
            buffer_back_char();

            return Error{NLC_INVALID_NUMBER, sign_line, sign_col};
        }

        t.append_lexeme(curr_char);
    } 

    t.append_lexeme(consume_digits_after_current(curr_char));

    return Error{};
}

// Should be called while curr_char is a dot
Error build_float_after_dot(Token& t, char& curr_char) {
    t.append_lexeme(curr_char);

    std::string digits_after_current = consume_digits_after_current(curr_char);
    if (digits_after_current.empty()) {
        return Error{NLC_NO_DIGITS};
    }

    t.append_lexeme(digits_after_current);

    // If the next char is not e | E, the number is complete.
    Error err = create_error(buffer_get_next_char(curr_char));
    if (err.is_eof() || !char_is_exponent(curr_char)) {
        buffer_back_char();

        return Error{};
    }

    return handle_exponent(t, curr_char);
}

bool unknown_token_name(TokenValue token_value) {
    return TOKEN_STRUCTURES::token_name_map.find(token_value) 
        == TOKEN_STRUCTURES::token_name_map.end();
}

std::string get_token_name(TokenValue token_value) {
    return unknown_token_name(token_value) 
        ? "Unknown token" : TOKEN_STRUCTURES::token_name_map.at(token_value);
}
