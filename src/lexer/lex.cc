// Nate Warner z2004109
// CS 490D/515
// Assignment 4
// lex.cc

#include "token.h"
#include "error.h"
#include "buffio.h"
#include "util.h"
#include "lex.h"

#include <string>
#include <algorithm>
#include <cctype>

using std::string;

// Helper function to check if a character is a digit
static inline bool is_digit(char c) { return c >= '0' && c <= '9'; }

static Token last_token{};

// Checks for escape sequence and replaces character
void check_replace_escape(char& c, int& rc, string& utf8) {
    // Clear this just in case its not already empty
    utf8.clear();

    // Start return code as OK
    rc = NCC_OK;

    char next{};

    // EOF is not good here, unexpected, didn't finish string token yet
    if (buffer_get_next_char(next) == NCC_EOF) {
        rc = NCC_UNEXPECTED_EOF;
        return;
    }

    if (next == 'n') {
        c = ESC_NEWLINE;
    } else if (next == 't') {
        c = ESC_TAB;
    } else if (next == 'r') {
        c = ESC_CARRIAGE_RETURN;
    } else if (next == '"') {
        c = ESC_QUOTE;
    } else if (next == '\\') {
        c = ESC_BACKSLASH;
    } else if (next == 'a') {
        c = ESC_ALERT;
    } else if (next == 'b') {
        c = ESC_BACKSPACE;
    // If unicode escape, call helper functions
    } else  if (next == 'u') {
        string hex{};

        // Try to consume the next six, should be hex digits
        // If, return error
        if (buffer_consume_k(6,hex) == NCC_EOF) {
            rc = NCC_UNEXPECTED_EOF;
            return;
        }
        // Get the utf8 rep
        utf8 = hex6_to_utf8(hex);
    // Line continuation, replace with black
    } else if (next == '\n') {
        c = EMPTY;
    // Unknown escape sequence, get rid of it
    } else {
        rc = NCC_ILLEGAL_ESCAPE;
    }

}


// Initializes lexer
Error lex_init(const char*  src_code) {
    // Call buffer init and get the error code
    int rc = buffer_init(src_code); 

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
    t.id = TOKEN_NULL;
    t.lexeme = "";
    t.line_no = src_line_no;
    t.col_no = src_col_no;
    t.integer = -1;
    t.fl = -1.f;
    t.str = "";
    t.identifier = "";

    // Start out with NCC_OK
    Error err;
    err.error = NCC_OK;

    // Set default token properties
    string lexeme{};
    string identifier{};
    string str{};

    unsigned long integer{};
    double fl{};

    // Hold current char from buffer position
    char curr_char{};

    // Either we get first valid (non white space) character, or hit EOF
    for (;;) {
        int rc{};
        rc = buffer_get_next_char(curr_char);

        // At this point EOF is fine, haven't started token yet
        if (rc == NCC_EOF) {
            // Build EOF token
            t.id = TOKEN_EOF;
            t.line_no = src_line_no;
            t.col_no = src_col_no;

            err.error = NCC_EOF;
            return err;
        }

        // Move past whitespace
        if (curr_char == '\n' || curr_char == ' ' || curr_char == '\t' || curr_char == '\r') {
            continue;
        }


        // Found first non white space character
        break;
    }

    // Record line and column number of first character of token
    t.col_no = src_col_no;
    t.line_no = src_line_no;

    // Add to lexeme string, we don't want to push the opening quote for a string though
    if (curr_char != '"') lexeme.push_back(curr_char);

    // Now we decide token class based on first character and consume the rest

    int rc{};
    if (curr_char == '+') {
        // Its unary plus. I acknowledge that it would be better to have a isoperator field
        // on the token struct, maybe ill do that later.
        if (last_token.id == TOKEN_NULL || last_token.id == TOKEN_PLUS
            || last_token.id == TOKEN_MINUS || last_token.id == TOKEN_MULT
            || last_token.id == TOKEN_DIV || last_token.id == TOKEN_EXP
            || last_token.id == TOKEN_LESS || last_token.id == TOKEN_GREATER
            || last_token.id == TOKEN_LESS_EQ || last_token.id == TOKEN_GREATER_EQ
            || last_token.id == TOKEN_EQUAL || last_token.id == TOKEN_NOT_EQUAL
            || last_token.id == TOKEN_ASSIGN || last_token.id == TOKEN_NOT
            || last_token.id == TOKEN_LPAREN || last_token.id == TOKEN_COMMA
            || last_token.id == TOKEN_DOT || last_token.id == TOKEN_MOD
            || last_token.id == TOKEN_OR || last_token.id == TOKEN_AND 
            || last_token.id == TOKEN_AT || last_token.id == TOKEN_UNEG
            || last_token.id == TOKEN_UPLUS
        ) {
            t.id = TOKEN_UPLUS;
        } else {
            t.id = TOKEN_PLUS; 
        }
        t.lexeme = lexeme;
    // Its unary negation
    } else if (curr_char == '-') {
        if (last_token.id == TOKEN_NULL || last_token.id == TOKEN_PLUS
            || last_token.id == TOKEN_MINUS || last_token.id == TOKEN_MULT
            || last_token.id == TOKEN_DIV || last_token.id == TOKEN_EXP
            || last_token.id == TOKEN_LESS || last_token.id == TOKEN_GREATER
            || last_token.id == TOKEN_LESS_EQ || last_token.id == TOKEN_GREATER_EQ
            || last_token.id == TOKEN_EQUAL || last_token.id == TOKEN_NOT_EQUAL
            || last_token.id == TOKEN_ASSIGN || last_token.id == TOKEN_NOT
            || last_token.id == TOKEN_LPAREN || last_token.id == TOKEN_COMMA
            || last_token.id == TOKEN_DOT || last_token.id == TOKEN_MOD
            || last_token.id == TOKEN_OR || last_token.id == TOKEN_AND 
            || last_token.id == TOKEN_AT || last_token.id == TOKEN_UNEG
            || last_token.id == TOKEN_UPLUS 
        ) {
            t.id = TOKEN_UNEG;
        } else {
            t.id = TOKEN_MINUS; 
        }
        t.lexeme = lexeme;
    } else if (curr_char == '*') {
        t.id = TOKEN_MULT; 
        t.lexeme = lexeme;
    } else if (curr_char == '/') {
        t.id = TOKEN_DIV; 
        t.lexeme = lexeme;
    } else if (curr_char == '^') {
        t.id = TOKEN_EXP; 
        t.lexeme = lexeme;
    // With character <, could be <, could be <=, could be block comment
    } else if (curr_char == '<') {
        // Get next char
        rc = buffer_get_next_char(curr_char);

        // In this case its just TOKEN_LESS 
        if (rc == NCC_EOF || curr_char == ' ' || curr_char == '\n' || curr_char == '\t') {
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
                    if (rc == NCC_EOF) {
                        err.error = NCC_UNEXPECTED_EOF;
                        err.line = src_line_no;
                        err.col = src_col_no;

                        return err;
                    }

                    int rc2{};
                    // If we find a -, read the next two characters, should be >>
                    if (curr_char == '-') {
                        string next_two{};

                        // Consume next two characters into string
                        rc2 = buffer_consume_k(2, next_two);

                        // No end of block comment
                        if (rc2 == NCC_EOF) {
                            err.error = NCC_UNEXPECTED_EOF;
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
    // Could be TOKEN_GREATER or TOKEN_GREATER_EQ
    } else if (curr_char == '>') {
        // Get next char
        rc = buffer_get_next_char(curr_char);

        // In this case its TOKEN_GREATER
        if (rc == NCC_EOF || curr_char == ' ' || curr_char == '\n' || curr_char == '\t') {
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
    } else if (curr_char == '=') {
        t.id = TOKEN_EQUAL;
        t.lexeme = lexeme;
    } else if (curr_char == '!') {
        t.id = TOKEN_NOT;
        t.lexeme = lexeme;
    } else if (curr_char == '(') {
        t.id = TOKEN_LPAREN;
        t.lexeme = lexeme;
    } else if (curr_char == ')') {
        t.id = TOKEN_RPAREN;
        t.lexeme = lexeme;
    } else if (curr_char == '{') {
        t.id = TOKEN_LBRACE;
        t.lexeme = lexeme;
    } else if (curr_char == '}') {
        t.id = TOKEN_RBRACE;
        t.lexeme = lexeme;
    } else if (curr_char == '[') {
        t.id = TOKEN_LBRACKET; 
        t.lexeme = lexeme;
    } else if (curr_char == ']') {
        t.id = TOKEN_RBRACKET; 
        t.lexeme = lexeme;
    } else if (curr_char == '&') {
        t.id = TOKEN_AND; 
        t.lexeme = lexeme;
    } else if (curr_char == '|') {
        t.id = TOKEN_OR; 
        t.lexeme = lexeme;
    } else if (curr_char == '@') {
        t.id = TOKEN_AT; 
        t.lexeme = lexeme;
    } else if (curr_char == ';') {
        t.id = TOKEN_SEMICOLON; 
        t.lexeme = lexeme;
    // Could be TOKEN_COLON or TOKEN_ASSIGN
    } else if (curr_char == ':') {
        // Get next char
        rc = buffer_get_next_char(curr_char);

        // In this case its TOKEN_COLON
        if (rc == NCC_EOF || curr_char == ' ' || curr_char == '\n' || curr_char == '\t') {
            t.id = TOKEN_COLON; 
            t.lexeme = lexeme;
        } else {
            // In this case its TOKEN_ASSIGN
            if (curr_char == '=') {
                lexeme.push_back(curr_char);
                t.id = TOKEN_ASSIGN;
                t.lexeme = lexeme;
            // Its TOKEN_COLON, back up to end of token
            } else {
                t.id = TOKEN_COLON; 
                t.lexeme = lexeme;
                buffer_back_char();
            }
        }
    } else if (curr_char == ',') {
        t.id = TOKEN_COMMA; 
        t.lexeme = lexeme;
    // Either TOKEN_NOT_EQUAL or undefined token
    } else if (curr_char == '~') {
        // Get next char, expecting =
        rc = buffer_get_next_char(curr_char); 

        // It's =, we found TOKEN_NOT_EQUAL
        if (curr_char == '=') {
            lexeme.push_back(curr_char);
            t.id = TOKEN_NOT_EQUAL;
            t.lexeme = lexeme;
        // If EOF, report unexpected EOF
        } else if (rc == NCC_EOF) {
            err.error = NCC_UNEXPECTED_EOF;
            err.line = t.line_no;
            err.col = t.col_no;

            return err;
        } 
        // Otherwise, token is undefined, just a lone ~
        else {
            buffer_back_char();
            err.error = NCC_UNDEFINED_TOKEN; 
            err.line = src_line_no;
            err.col = src_col_no;

            return err;
        }
    // Line comment, consume rest of line without doing anything
    } else if (curr_char == '#') {
        // Consume rest of line
        for (;;) {
            rc = buffer_get_next_char(curr_char);

            // End at newline, comment is disregarded
            if (curr_char == '\n') break;

            // Must end with newline, otherwise it's unexpected EOF
            if (rc == NCC_EOF) {
                err.error = NCC_UNEXPECTED_EOF;
                err.line = src_line_no;
                err.col = src_col_no;

                return err;
            }

        }
        get_token(t);

    // Possible identifier
    } else if (('a' <= curr_char && curr_char <= 'z') || ('A' <= curr_char && curr_char <= 'Z') || (curr_char == '_')) {
        // Assume identifier
        t.id = TOKEN_IDENT;

        // Start consuming characters to build identifier until whitespace
        for (;;) {
            rc = buffer_get_next_char(curr_char);

            // This may not be correct, can't remember what I was thinking here. I'll come 
            // back to this later if there are problems
            if (rc == NCC_EOF) {
                t.id = TOKEN_EOF;
                t.lexeme = lexeme;
                t.identifier = lexeme;

                err.line = src_line_no;
                err.col = src_col_no;

                // Might want this to be unexpected EOF
                err.error = NCC_EOF;

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
    // Handle strings with escape sequences
    } else if (curr_char == '"') {
        t.id = TOKEN_NULL;
        // Consume characters and replace their escape sequences
        for (;;) {
            rc = buffer_get_next_char(curr_char);

            // Reached EOF before closing string
            if (rc == NCC_EOF) {
                err.error = NCC_UNEXPECTED_EOF;
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
                string hex{};
                int return_code{};
                if (curr_char == '\\') {
                    // Might get a hex value back from this
                    check_replace_escape(curr_char, return_code, hex);
                }

                // Unexpected EOF
                if (return_code == NCC_UNEXPECTED_EOF) {
                    err.error = NCC_UNEXPECTED_EOF;
                    err.line = src_line_no;
                    err.col = src_col_no;

                    return err;
                    // Illegal escape sequence
                } else if (return_code == NCC_ILLEGAL_ESCAPE) {
                    err.error = NCC_ILLEGAL_ESCAPE;
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
    } else if (curr_char == '.') {
        // Peek next char to decide between TOKEN_DOT and TOKEN_REAL
        char next{};
        rc = buffer_get_next_char(next);

        // Lone '.' at EOF, report token as dot and EOF
        if (rc == NCC_EOF) {
            t.id = TOKEN_DOT;
            t.lexeme = lexeme; 
            err.error = NCC_EOF; 

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
                if (rc == NCC_EOF) break;

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
            if (rc != NCC_EOF && (epeek == 'e' || epeek == 'E')) {
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
                if (rc == NCC_EOF) {
                    err.error = NCC_INVALID_NUMBER;
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
                    if (rc == NCC_EOF || !is_digit(first_exp_digit)) {
                        // If its not EOF, back up, might be the start of something new
                        if (rc != NCC_EOF) buffer_back_char();

                        // Report invalid number
                        err.error = NCC_INVALID_NUMBER;
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
                    err.error = NCC_INVALID_NUMBER;
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
                    if (rc == NCC_EOF) break;

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
                if (rc != NCC_EOF) buffer_back_char();
            }

            t.id = TOKEN_REAL;
            t.lexeme = lexeme;

            // This will work no doubt
            t.fl = std::stod(lexeme);
        }
    
    // Start of token is a digit, could be an integer or a float
    } else if (is_digit(curr_char)) {
        // Start consuming integer-part digits
        for (;;) {
            rc = buffer_get_next_char(curr_char);

            // End at EOF
            if (rc == NCC_EOF) break;

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
        if (rc != NCC_EOF && peek == '.') {

            // Report that we saw a dot and push dot to lexeme
            saw_dot = true;
            lexeme.push_back('.');

            // Fractional digits (optional)
            char d{};
            rc = buffer_get_next_char(d);

            // Consume fractional digits if they exist
            if (rc != NCC_EOF && is_digit(d)) {

                // Report that we saw a fractional digit
                saw_frac_digit = true;

                // Push the digit
                lexeme.push_back(d);

                // Consume more
                for (;;) {
                    rc = buffer_get_next_char(curr_char);

                    // Break at EOF, done
                    if (rc == NCC_EOF) break;

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
                if (rc != NCC_EOF) buffer_back_char();
            }
        // not '.', put it back
        } else {
            if (rc != NCC_EOF) buffer_back_char();
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
            if (rc != NCC_EOF && (epeek == 'e' || epeek == 'E')) {
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
                if (rc == NCC_EOF) {
                    // Report invalid number
                    err.error = NCC_INVALID_NUMBER;
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
                    if (rc == NCC_EOF || !is_digit(first_exp_digit)) {
                        // Might be the start of something new, back up
                        if (rc != NCC_EOF) buffer_back_char();

                        // Report invalid number, cant end with a sign after exponent
                        // must have at least one digit
                        err.error = NCC_INVALID_NUMBER;
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
                    err.error = NCC_INVALID_NUMBER;
                    err.line = e_line;
                    err.col  = e_col;
                     
                    return err;
                }

                // Consume remaining exponent digits
                for (;;) {
                    rc = buffer_get_next_char(curr_char);

                    // Break at EOF, our number is valid
                    if (rc == NCC_EOF) break;

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
                if (rc != NCC_EOF) buffer_back_char();
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

    // Return our NCC_OK Error
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
