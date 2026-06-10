#include "token.h"
#include "token_structures.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using std::cout;
using std::cerr;
using std::string;

// Prints a token
void print_token(const Token& t) {
    // Guaranteed values of interest
    if (t.id < 0 || static_cast<size_t>(t.id) >= TOKEN_STRUCTURES::token_names.size()) {
        cerr << "Tried to access a token id that does not exist\n\n";
        return;
    }

    // For TOKEN_NULL just return, we want to ignore it 
    if (t.id == TOKEN_NULL) return;

    // Custom output depending on token type
    if (t.id == TOKEN_INTEGER) {
        cout << TOKEN_STRUCTURES::token_names[t.id] << ": " << t.integer 
            << " at " << t.line_no << ":" << t.col_no << '\n';
    } else if (t.id == TOKEN_IDENT) {
        cout << TOKEN_STRUCTURES::token_names[t.id] << ": "<< t.identifier 
            << " at " << t.line_no << ":" << t.col_no << '\n';
    } else if (t.id == TOKEN_REAL) {
        cout << TOKEN_STRUCTURES::token_names[t.id] << ": " << t.fl 
            << " at " << t.line_no << ":" << t.col_no << '\n';
    } else if (t.id == TOKEN_STRING) {
        cout << TOKEN_STRUCTURES::token_names[t.id] << ": " << t.str 
            << " at " << t.line_no << ":" << t.col_no << '\n';
    } else {
        cout << TOKEN_STRUCTURES::token_names[t.id] 
            << " at " << t.line_no << ":" << t.col_no << '\n';
    }
}

bool Token::is(TokenValue token) {
    return id == token;
}

bool Token::is_not(TokenValue token) {
    return !is(token);
}

bool Token::is_ident() {
    return is(TOKEN_IDENT);
}

bool Token::is_ident_if() {
    return is_ident() && identifier == "if";
}

bool Token::is_ident_else() {
    return is_ident() && identifier == "else";
}

bool Token::is_ident_while() {
    return is_ident() && identifier == "while";
}

bool Token::is_ident_true() {
    return is_ident() && identifier == "true";
}

bool Token::is_ident_false() {
    return is_ident() && identifier == "false";
}

bool Token::is_type() {
    return is_ident() && 
        TOKEN_STRUCTURES::types.find(identifier) 
        != TOKEN_STRUCTURES::types.end(); 
}

bool Token::is_type_int() {
    return is_ident() && identifier == "int";
}

bool Token::is_boolean() {
    return is_ident_true() || is_ident_false();
}

bool Token::is_semicolon() {
    return is(TOKEN_SEMICOLON);
}

bool Token::is_lbrace() {
    return is(TOKEN_LBRACE);
}

bool Token::is_rbrace() {
    return is(TOKEN_RBRACE);
}

bool Token::is_comma() {
    return is(TOKEN_COMMA);
}

bool Token::is_eof() {
    return is(TOKEN_EOF);
}

bool Token::in(const TokenSet& set)  { 
    return set.find(id) != set.end(); 
}
