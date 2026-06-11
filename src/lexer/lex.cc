#include "token.h"
#include "error.h"
#include "buffio.h"
#include "util.h"
#include "lex.h"
#include "lexstate.h"
#include "parser.h"
#include "types.h"
#include "lex_structures.h"

#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace {
    Token last_token{};

    inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
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
Error lex_init(const char*  src_code) {
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

    // Set default token properties
    std::string lexeme{};
    std::string identifier{};
    std::string str{};
    unsigned long integer{};
    double fl{};

    char curr_char{};

    // Either we get first valid (non white space) character, or hit EOF
    for (;;) {
        Error error = create_error(buffer_get_next_char(curr_char));

        // At this point EOF is fine, haven't started token yet
        if (error.is_eof()) {
            t.set_eof();
            t.set_line_and_col(src_line_no, src_col_no);

            return error;
        }

        if (char_is_whitespace(curr_char)) {
            continue;
        }

        // Found first non white space character
        break;
    }

    // Record line and column number of first character of token
    t.set_line_and_col(src_line_no, src_col_no);

    // Add to lexeme string, we don't want to push the opening quote for a string though
    if (curr_char != '"') lexeme.push_back(curr_char);

    // Now we decide token class based on first character and consume the rest

    ReturnCode rc{};
    if (char_in_simple_lex_set(curr_char)) {
        t.set_id(char_to_token.at(curr_char));
        t.set_lexeme(lexeme);
    }

    else if (curr_char == '+') {
        t = last_token;
        // Either unary plus or plus
        if (!t.set_id_from_predicate(TOKEN_UPLUS, &Token::legal_before_uplus_or_uneg)) {
            t.set_id(TOKEN_PLUS);
        }

        t.set_lexeme(lexeme);
    } 

    else if (curr_char == '-') {
        t = last_token;
        // Either unary negation or minus
        if (!t.set_id_from_predicate(TOKEN_UNEG, &Token::legal_before_uplus_or_uneg)) {
            t.set_id(TOKEN_MINUS);
        }

        t.set_lexeme(lexeme);
    } 

    // With character <, could be <, could be <=, could be block comment
    else if (curr_char == '<') {
        // Get next char
        rc = buffer_get_next_char(curr_char);

        // In this case its just TOKEN_LESS 
        if (rc == NLC_EOF || curr_char == ' ' || curr_char == '\n' || curr_char == '\t') {
            t.id = TOKEN_LESS; 
            t.lexeme = lexeme;
        // In this case its TOKEN_LESS_EQ
        } else if (curr_char == '=') {
            lexeme.push_back(curr_char);
            t.id = TOKEN_LESS_EQ;
            t.lexeme = lexeme;

        // Could be a block comment, now we have <<
        } else if (curr_char == '<') {
            // Consume next char
            rc = buffer_get_next_char(curr_char); 

            // Not a block comment
            if (curr_char != '-') {
                buffer_back_char();
            // We have a block comment
            } else {
                // Read until -
                for (;;) {
                    rc = buffer_get_next_char(curr_char);

                    // Unexpected EOF, block comment was not closed
                    if (rc == NLC_EOF) {
                        err.error = NLC_UNEXPECTED_EOF;
                        err.line = src_line_no;
                        err.col = src_col_no;

                        return err;
                    }

                    int rc2{};
                    // If we find a -, read the next two characters, should be >>
                    if (curr_char == '-') {
                        std::string next_two{};

                        // Consume next two characters into string
                        rc2 = buffer_consume_k(2, next_two);

                        // No end of block comment
                        if (rc2 == NLC_EOF) {
                            err.error = NLC_UNEXPECTED_EOF;
                            err.line = src_line_no;
                            err.col = src_col_no;

                            return err;
                        }

                        // Found end of block comment
                        if (next_two != ">>") {
                            // Back up those two characters
                            buffer_back_char();
                            buffer_back_char();
                        } else {
                            get_token(t);
                            break;
                        }
                    }
                }
            }
        // Back up, could be the start of something new, we have TOKEN_LESS
        } else {
            t.id = TOKEN_LESS; 
            t.lexeme = lexeme;
            buffer_back_char();
        }
    } 

    // Could be TOKEN_GREATER or TOKEN_GREATER_EQ
    else if (curr_char == '>') {
        // Get next char
        rc = buffer_get_next_char(curr_char);

        // In this case its TOKEN_GREATER
        if (rc == NLC_EOF || curr_char == ' ' || curr_char == '\n' || curr_char == '\t') {
            t.id = TOKEN_GREATER; 
            t.lexeme = lexeme;
        } else {
            // Found TOKEN_GREATER_EQ
            if (curr_char == '=') {
                lexeme.push_back(curr_char);
                t.id = TOKEN_GREATER_EQ;
                t.lexeme = lexeme;
            // Just TOKEN_GREATER
            } else {
                t.id = TOKEN_GREATER; 
                t.lexeme = lexeme;

                // Back up one, we found our token, go back to end of current token
                buffer_back_char();
            }
        }
    } 

    else if (curr_char == '=') {
        rc = buffer_get_next_char(curr_char);

        if (curr_char == '=') {
            t.id = TOKEN_EQUAL; 
        }

        else {
            t.id = TOKEN_ASSIGN;
            buffer_back_char();
        }

        t.lexeme = lexeme;
    } 

    // Either TOKEN_NOT_EQUAL or undefined token
    else if (curr_char == '~') {
        // Get next char, expecting =
        rc = buffer_get_next_char(curr_char); 

        // It's =, we found TOKEN_NOT_EQUAL
        if (curr_char == '=') {
            lexeme.push_back(curr_char);
            t.id = TOKEN_NOT_EQUAL;
            t.lexeme = lexeme;
        // If EOF, report unexpected EOF
        } else if (rc == NLC_EOF) {
            err.error = NLC_UNEXPECTED_EOF;
            err.line = t.line_no;
            err.col = t.col_no;

            return err;
        } 
        // Otherwise, token is undefined, just a lone ~
        else {
            buffer_back_char();
            err.error = NLC_UNDEFINED_TOKEN; 
            err.line = src_line_no;
            err.col = src_col_no;

            return err;
        }
    } 

    // Line comment, consume rest of line without doing anything
    else if (curr_char == '#') {
        // Consume rest of line
        for (;;) {
            rc = buffer_get_next_char(curr_char);

            // End at newline, comment is disregarded
            if (curr_char == '\n') break;

            // Must end with newline, otherwise it's unexpected EOF
            if (rc == NLC_EOF) {
                err.error = NLC_UNEXPECTED_EOF;
                err.line = src_line_no;
                err.col = src_col_no;

                return err;
            }

        }
        get_token(t);

    } 

    // Possible identifier
    else if (('a' <= curr_char && curr_char <= 'z') || ('A' <= curr_char && curr_char <= 'Z') || (curr_char == '_')) {
        // Assume identifier
        t.id = TOKEN_IDENT;

        // Start consuming characters to build identifier until whitespace
        for (;;) {
            rc = buffer_get_next_char(curr_char);

            // This may not be correct, can't remember what I was thinking here. I'll come 
            // back to this later if there are problems
            if (rc == NLC_EOF) {
                t.id = TOKEN_EOF;
                t.lexeme = lexeme;
                t.identifier = lexeme;

                err.line = src_line_no;
                err.col = src_col_no;

                // Might want this to be unexpected EOF
                err.error = NLC_EOF;

                return err;
            }

            // Identifier is growing
            if (('a' <= curr_char  && curr_char <= 'z') || ('A' <= curr_char  && curr_char <= 'Z') || ('0' <= curr_char && curr_char <= '9') || curr_char == '_') {
                lexeme.push_back(curr_char);
            // End of identifier
            } else {
                buffer_back_char();

                t.lexeme = lexeme;
                t.identifier = lexeme;
                break;
            }
        } 

        // It could be that our "identifier" is really the mod operator, so we check for that.
        // First, clean the string by converting everything to lower case
        std::transform(lexeme.begin(), lexeme.end(), lexeme.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        if (lexeme == "mod") {
            t.id = TOKEN_MOD; 
            t.lexeme = lexeme;
        }
    } 

    // Handle strings with escape sequences
    else if (curr_char == '"') {
        t.id = TOKEN_NULL;
        // Consume characters and replace their escape sequences
        for (;;) {
            rc = buffer_get_next_char(curr_char);

            // Reached EOF before closing string
            if (rc == NLC_EOF) {
                err.error = NLC_UNEXPECTED_EOF;
                err.line = src_line_no;
                err.col = src_col_no;

                return err;
            }

            // End of string
            if (curr_char == '"') {
                t.lexeme = lexeme;
                t.str = lexeme;
                break;
                // Some character in string
            } else {
                std::string hex{};
                ReturnCode rc{};

                if (curr_char == '\\') {
                    // Might get a hex value back from this
                    rc = check_replace_escape(curr_char, hex);
                }

                // Unexpected EOF
                if (rc == NLC_UNEXPECTED_EOF) {
                    err.error = NLC_UNEXPECTED_EOF;
                    err.line = src_line_no;
                    err.col = src_col_no;

                    return err;
                    // Illegal escape sequence
                } else if (rc == NLC_ILLEGAL_ESCAPE) {
                    err.error = NLC_ILLEGAL_ESCAPE;
                    err.line = src_line_no;
                    err.col = src_col_no;

                }

                // Escape was unicode \u
                if (!hex.empty()) {
                    // Add that to the string
                    lexeme += hex; 
                } else {
                    // Push back the maybe replaced character
                    lexeme.push_back(curr_char);
                }

            }
        }
        // String token was built successfully
        t.id = TOKEN_STRING;
        t.lexeme = lexeme;
        t.str = lexeme;
    } 

    else if (curr_char == '.') {
        // Peek next char to decide between TOKEN_DOT and TOKEN_REAL
        char next{};
        rc = buffer_get_next_char(next);

        // Lone '.' at EOF, report token as dot and EOF
        if (rc == NLC_EOF) {
            t.id = TOKEN_DOT;
            t.lexeme = lexeme; 
            err.error = NLC_EOF; 

            return err;
        }

        if (!is_digit(next)) {
            // not a real, put back what we grabbed
            buffer_back_char();  
            t.id = TOKEN_DOT;
            t.lexeme = lexeme; 
        } else {
            // It's a real like .37 or .7e5 build lexeme as 0.digits
            lexeme = "0.";
            lexeme.push_back(next);

            // Consume remaining fractional digits
            for (;;) {
                rc = buffer_get_next_char(curr_char);
                if (rc == NLC_EOF) break;

                if (is_digit(curr_char)) {
                    lexeme.push_back(curr_char);
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
                lexeme.push_back('e'); 

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
                    lexeme.push_back(sign);

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
                    lexeme.push_back(first_exp_digit);
                // It could be that the sign char is actually a digit
                } else if (is_digit(sign)) {
                    lexeme.push_back(sign);
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
                    if (is_digit(curr_char)) lexeme.push_back(curr_char);

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
            t.lexeme = lexeme;

            // This will work no doubt
            t.fl = std::stod(lexeme);
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
                lexeme.push_back(curr_char);
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
            lexeme.push_back('.');

            // Fractional digits (optional)
            char d{};
            rc = buffer_get_next_char(d);

            // Consume fractional digits if they exist
            if (rc != NLC_EOF && is_digit(d)) {

                // Report that we saw a fractional digit
                saw_frac_digit = true;

                // Push the digit
                lexeme.push_back(d);

                // Consume more
                for (;;) {
                    rc = buffer_get_next_char(curr_char);

                    // Break at EOF, done
                    if (rc == NLC_EOF) break;

                    // Consume digit
                    if (is_digit(curr_char)) {
                        lexeme.push_back(curr_char);
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
                lexeme.push_back('e'); 

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
                    lexeme.push_back(sign);

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
                    lexeme.push_back(first_exp_digit);
                // Recall that the sign char could actually be a digit
                } else if (is_digit(sign)) {
                    lexeme.push_back(sign);
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
                    if (is_digit(curr_char)) lexeme.push_back(curr_char);

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
        for (char c : lexeme) {
            if (c == 'e' || c == 'E') { 
                is_real = true;
                break;
            }
        }

        // If its real, report real
        if (is_real) {
            t.id = TOKEN_REAL;
            t.lexeme = lexeme;

            // Cast string to double (its valid)
            t.fl = std::stod(lexeme);
        // Otherwise, it's a plain integer
        } else {
            // Report integer
            t.id = TOKEN_INTEGER;
            t.lexeme = lexeme;

            // This will work
            t.integer = std::stoul(lexeme);
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
