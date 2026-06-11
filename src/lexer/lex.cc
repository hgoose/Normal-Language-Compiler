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

inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
inline bool valid_identifier_start(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
inline bool valid_identifier_poststart(char c) { return isdigit(c) || valid_identifier_start(c); }

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

    // Create and return the error object, set properties first
    Error err;
    err.error = rc;
    err.line = src_line_no;
    err.col = src_col_no;

    return err;
}

// Gets the next token
Error get_token(Token& t) {
    // Reset token
    t.reset();
    t.set_line_and_col(src_line_no, src_col_no);

    Error err{};
    ReturnCode rc{};

    // Set default token properties
    std::string lexeme{};
    std::string identifier{};
    std::string str{};
    unsigned long integer{};
    double fl{};

    char curr_char{};

    // Either we get first valid (non white space) character, or hit EOF
    Error error = create_error(buffer_get_next_char(curr_char));

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
    if (curr_char != '"') t.lexeme.push_back(curr_char);

    // Now we decide token class based on first character and consume the rest

    if (char_in_simple_lex_set(curr_char)) {
        t.set_id(char_to_token.at(curr_char));

        return Error{};
    }

    else if (curr_char == '+') {
        t = last_token;
        // Either unary plus or plus
        if (!t.set_id_from_predicate(TOKEN_UPLUS, &Token::legal_before_uplus_or_uneg)) {
            t.set_id(TOKEN_PLUS);
        }

        return Error{};
    } 

    else if (curr_char == '-') {
        t = last_token;
        // Either unary negation or minus
        if (!t.set_id_from_predicate(TOKEN_UNEG, &Token::legal_before_uplus_or_uneg)) {
            t.set_id(TOKEN_MINUS);
        }

        return Error{};
    } 

    else if (curr_char == '<') {
        return lex_char_less(t, curr_char);
    } 

    else if (curr_char == '>') {
        return lex_char_greater(t, curr_char);
    } 

    else if (curr_char == '=') {
        return lex_char_equal(t, curr_char);
    } 

    // Either TOKEN_NOT_EQUAL or undefined token
    else if (curr_char == '~') {
        return lex_char_not(t, curr_char);
    } 

    // Line comment, consume rest of line without doing anything
    else if (curr_char == '#') {
        return lex_char_pound(t, curr_char);
    } 

    // Possible identifier
    else if (valid_identifier_start(curr_char)) {
        return lex_char_alpha(t, curr_char);
    } 

    // Handle strings with escape sequences
    else if (curr_char == '"') {
        return lex_char_quote(t, curr_char);
    } 

    else if (curr_char == '.') {
        // Peek next char to decide between TOKEN_DOT and TOKEN_REAL
        char next{};
        rc = buffer_get_next_char(next);

        // Lone '.' at EOF, report token as dot and EOF
        if (rc == NLC_EOF) {
            t.id = TOKEN_DOT;
             
            err.error = NLC_EOF; 

            return err;
        }

        if (!is_digit(next)) {
            // not a real, put back what we grabbed
            buffer_back_char();  
            t.id = TOKEN_DOT;
             
        } else {
            // It's a real like .37 or .7e5 build lexeme as 0.digits
            t.lexeme = "0.";
            t.lexeme.push_back(next);

            // Consume remaining fractional digits
            for (;;) {
                rc = buffer_get_next_char(curr_char);
                if (rc == NLC_EOF) break;

                if (is_digit(curr_char)) {
                    t.lexeme.push_back(curr_char);
                } else {
                    buffer_back_char();
                    break;
                }
            }

            // Optional exponent is allowed here, we have fractional digits
            char epeek{};
            rc = buffer_get_next_char(epeek);
            if (rc != NLC_EOF && (epeek == 'e' || epeek == 'E')) {
                // record exponent start position (the 'e'/'E')
                int e_line = src_line_no;
                int e_col  = src_col_no;

                // Add exponent, just make always lower case e
                t.lexeme.push_back('e'); 

                // Optional sign
                char sign{};

                // Look for optional sign, if EOF, invalid number (last was e or E)
                // can't end with e
                rc = buffer_get_next_char(sign);
                if (rc == NLC_EOF) {
                    err.error = NLC_INVALID_NUMBER;
                    err.line = e_line;
                    err.col  = e_col;
                    return err;
                }

                // If we have a sign
                if (sign == '+' || sign == '-') {
                    // Add it to the lexeme
                    t.lexeme.push_back(sign);

                    // Record line and column number of sign
                    int s_line = src_line_no;
                    int s_col  = src_col_no;

                    // Try to get digits after exponent
                    char first_exp_digit{};
                    rc = buffer_get_next_char(first_exp_digit);

                    // If EOF, we have no digits after exponent and option sign, 
                    // not a valid real
                    if (rc == NLC_EOF || !is_digit(first_exp_digit)) {
                        // If its not EOF, back up, might be the start of something new
                        if (rc != NLC_EOF) buffer_back_char();

                        // Report invalid number
                        err.error = NLC_INVALID_NUMBER;
                        err.line = s_line;
                        err.col  = s_col; 
                        return err;
                    }
                    // Otherwise, push the digit
                    t.lexeme.push_back(first_exp_digit);
                // It could be that the sign char is actually a digit
                } else if (is_digit(sign)) {
                    t.lexeme.push_back(sign);
                // Otherwise, sign is not a + or -, and not a digit, invalid real
                } else {
                    // Back up, could be the start of something new
                    buffer_back_char();

                    // Report invalid number
                    err.error = NLC_INVALID_NUMBER;
                    err.line = e_line;
                    err.col  = e_col;
                    return err;
                }

                // Consume remaining exponent digits,
                // at this point we have an exponent, maybe a sign, and at least one
                // digit after the exponent. 
                for (;;) {
                    rc = buffer_get_next_char(curr_char);

                    // Break if at EOF
                    if (rc == NLC_EOF) break;

                    // If we have a new digit, push
                    if (is_digit(curr_char)) t.lexeme.push_back(curr_char);

                    // Otherwise, could be the start of something new, back up
                    else { 
                        buffer_back_char();
                        break; 
                    }
                }
            // Last thing we grabbed was not a new digit and not an exponent, put back whatever we grabbed
            // the real has concluded
            } else {
                if (rc != NLC_EOF) buffer_back_char();
            }

            t.id = TOKEN_REAL;
            

            // This will work no doubt
            t.fl = std::stod(t.lexeme);
        }
    
    // Start of token is a digit, could be an integer or a float
    } 

    else if (is_digit(curr_char)) {
        // Start consuming integer-part digits
        for (;;) {
            rc = buffer_get_next_char(curr_char);

            // End at EOF
            if (rc == NLC_EOF) break;

            // If digit, continue building
            if (is_digit(curr_char)) {
                t.lexeme.push_back(curr_char);
            // Otherwise, end of number, could be something new.
            } else {
                buffer_back_char();
                break;
            }
        }

        // Decide if real either dot follows or exponent follows
        bool saw_dot = false;
        bool saw_frac_digit = false;

        // Check for dot
        char peek{};
        rc = buffer_get_next_char(peek);

        // Found a DOT
        if (rc != NLC_EOF && peek == '.') {

            // Report that we saw a dot and push dot to lexeme
            saw_dot = true;
            t.lexeme.push_back('.');

            // Fractional digits (optional)
            char d{};
            rc = buffer_get_next_char(d);

            // Consume fractional digits if they exist
            if (rc != NLC_EOF && is_digit(d)) {

                // Report that we saw a fractional digit
                saw_frac_digit = true;

                // Push the digit
                t.lexeme.push_back(d);

                // Consume more
                for (;;) {
                    rc = buffer_get_next_char(curr_char);

                    // Break at EOF, done
                    if (rc == NLC_EOF) break;

                    // Consume digit
                    if (is_digit(curr_char)) {
                        t.lexeme.push_back(curr_char);
                    // Otherwise, we are done. So, we back up like before
                    } else { 
                        buffer_back_char(); 
                        break; 
                    }
                }
            } else {
                // No fractional digit after '.', so it's like "5." 
                // Note: Exponent after this is not allowed
                if (rc != NLC_EOF) buffer_back_char();
            }
        // not '.', put it back
        } else {
            if (rc != NLC_EOF) buffer_back_char();
        }

        // Optional exponent:
        // - allowed if: no dot at all (2e9), OR dot with at least one fractional digit (6.02e23)
        // - NOT allowed if: dot present but no fractional digits (1.e5, 3.E07)
        bool exp_allowed = (!saw_dot) || (saw_dot && saw_frac_digit);

        // If the exponent is allowed
        if (exp_allowed) {
            // Try to find exponent
            char epeek{};
            rc = buffer_get_next_char(epeek);

            // If we found it, we continue in the same way as before
            if (rc != NLC_EOF && (epeek == 'e' || epeek == 'E')) {
                // Report position of exponent
                int e_line = src_line_no;
                int e_col  = src_col_no;

                // Add to lexeme (always lower case)
                t.lexeme.push_back('e'); 

                // Optional sign
                char sign{};

                // Try to find optional sign
                rc = buffer_get_next_char(sign);

                // If EOF, number is invalid, recall that we found an exponent, but 
                // this means their is no digits that follow
                if (rc == NLC_EOF) {
                    // Report invalid number
                    err.error = NLC_INVALID_NUMBER;
                    err.line = e_line;
                    err.col  = e_col;
                    return err;
                }

                // Sign has been found
                if (sign == '+' || sign == '-') {
                    // Push it
                    t.lexeme.push_back(sign);

                    // Record position
                    int s_line = src_line_no;
                    int s_col  = src_col_no;

                    // Try to find first digit after exponent
                    char first_exp_digit{};
                    rc = buffer_get_next_char(first_exp_digit);

                    // If no digit after exponent, invalid number
                    if (rc == NLC_EOF || !is_digit(first_exp_digit)) {
                        // Might be the start of something new, back up
                        if (rc != NLC_EOF) buffer_back_char();

                        // Report invalid number, cant end with a sign after exponent
                        // must have at least one digit
                        err.error = NLC_INVALID_NUMBER;
                        err.line = s_line;
                        err.col  = s_col; 

                        return err;
                    }
                    // Otherwise, we found our first digit, push it
                    t.lexeme.push_back(first_exp_digit);
                // Recall that the sign char could actually be a digit
                } else if (is_digit(sign)) {
                    t.lexeme.push_back(sign);
                // Otherwise, e followed by no sign and no digits, followed by junk
                } else {
                    // Back up
                    buffer_back_char();

                    // Report invalid number
                    err.error = NLC_INVALID_NUMBER;
                    err.line = e_line;
                    err.col  = e_col;
                     
                    return err;
                }

                // Consume remaining exponent digits
                for (;;) {
                    rc = buffer_get_next_char(curr_char);

                    // Break at EOF, our number is valid
                    if (rc == NLC_EOF) break;

                    // Consume digits
                    if (is_digit(curr_char)) t.lexeme.push_back(curr_char);

                    // Otherwise no more digits, we are done
                    else { 
                        buffer_back_char();
                        break;
                    }
                }
            // No exponent, no digits, done with number, go back one
            } else {
                if (rc != NLC_EOF) buffer_back_char();
            }
        }

        // If we saw dot or exponent, it's real. Otherwise integer.
        bool is_real = saw_dot;

        // Also real if exponent exists in lexeme
        for (char c : t.lexeme) {
            if (c == 'e' || c == 'E') { 
                is_real = true;
                break;
            }
        }

        // If its real, report real
        if (is_real) {
            t.id = TOKEN_REAL;

            // Cast string to double (its valid)
            t.fl = std::stod(t.lexeme);
        // Otherwise, it's a plain integer
        } else {
            // Report integer
            t.id = TOKEN_INTEGER;

            // This will work
            t.integer = std::stoul(t.lexeme);
        }
    }

    last_token = t;

    // Return our NLC_OK Error
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
    Error buf_error = create_error(buffer_get_next_char(curr_char));

    if (curr_char == '=') {
        t.lexeme.push_back(curr_char);
        t.set_id(TOKEN_LESS_EQ);
        return Error{};
    } 

    else if (curr_char != '<') {
        t.set_id(TOKEN_LESS);
        buffer_back_char();
        return Error{};
    }

    // From this point on we have <<

    buf_error = create_error(buffer_get_next_char(curr_char)); 

    // Not a block comment
    if (curr_char != '-') {
        buffer_back_char();
        t.set_id(TOKEN_NULL);
        return Error{};
    } 

    Error err = get_end_of_block_comment(t, curr_char);

    if (err.is_not(NLC_NO_MATCH)) {
        return get_token(t); 
    }

    else if (err.is(NLC_UNEXPECTED_EOF)) {
        return err;
    }

    return Error{};
}

Error lex_char_greater(Token& t, char& curr_char) {
    Error err = create_error(buffer_get_next_char(curr_char));

    if (err.is_eof() || char_is_whitespace(curr_char)) {
        t.set_id(TOKEN_GREATER);
        return Error{};
    } 

    if (curr_char == '=') {
        t.lexeme.push_back(curr_char);
        t.set_id(TOKEN_GREATER_EQ);

        return Error{};
    } 

    buffer_back_char();
    t.set_id(TOKEN_GREATER); 

    return Error{};
}

Error lex_char_equal(Token& t, char& curr_char) {
    return token_either_this_or_that(t, TOKEN_EQUAL, TOKEN_ASSIGN, curr_char, '=');
}

Error lex_char_not(Token& t, char& curr_char) {
    return token_either_this_or_that(t, TOKEN_NOT_EQUAL, TOKEN_NULL, curr_char, '=');
}

Error lex_char_pound(Token& t, char& curr_char) {
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

        if (!valid_identifier_poststart(curr_char)) {
            buffer_back_char();
            t.set_identifier(t.lexeme);
            break;
        } 

        t.lexeme.push_back(curr_char);
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
            t.lexeme += hex; 
            continue;
        }

        t.lexeme.push_back(curr_char);
    }

    t.set_id(TOKEN_STRING);
    t.set_str(t.lexeme);

    return Error{};
}
