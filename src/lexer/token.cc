// Nate Warner z2004109
// CS-490D/515
// Assignment 4

#include "token.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using std::cout;
using std::cerr;
using std::string;

// Will come in handy
namespace TOKEN_STRUCTURES {
    const std::unordered_map<int, std::string> operator_names = {
        {TOKEN_PLUS, "add"},
        {TOKEN_MINUS, "sub"},
        {TOKEN_MULT, "mul"},
        {TOKEN_DIV, "div"},
        {TOKEN_MOD, "mod"},
        {TOKEN_EXP, "exp"},
        {TOKEN_UNEG, "neg"},
        {TOKEN_UPLUS, "pos"},
        {TOKEN_LESS, "less"},
        {TOKEN_LESS_EQ, "leq"},
        {TOKEN_GREATER, "greater"},
        {TOKEN_GREATER_EQ, "geq"},
        {TOKEN_EQUAL, "equal"},
        {TOKEN_NOT_EQUAL, "neq"},
        {TOKEN_AND, "and"},
        {TOKEN_OR, "or"}
    };

    const std::vector<std::string> token_names = {
        "TOKEN_NULL", "TOKEN_EOF", "TOKEN_PLUS",
        "TOKEN_MINUS", "TOKEN_MULT", "TOKEN_DIV",
        "TOKEN_IDENT", "TOKEN_EXP", "TOKEN_LESS",
        "TOKEN_LESS_EQ", "TOKEN_GREATER", "TOKEN_GREATER_EQ",
        "TOKEN_EQUAL", "TOKEN_NOT_EQUAL", "TOKEN_ASSIGN",
        "TOKEN_NOT", "TOKEN_LPAREN", "TOKEN_RPAREN",
        "TOKEN_LBRACE", "TOKEN_RBRACE", "TOKEN_LBRACKET",
        "TOKEN_RBRACKET", "TOKEN_AND", "TOKEN_OR",
        "TOKEN_DOT", "TOKEN_AT", "TOKEN_INTEGER",
        "TOKEN_STRING", "TOKEN_COLON", "TOKEN_SEMICOLON",
        "TOKEN_COMMA", "TOKEN_REAL", "TOKEN_MOD",
        "TOKEN_UNEG", "TOKEN_UPLUS" 
    };

    const std::unordered_set<int> terminals = {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULT,
        TOKEN_DIV, TOKEN_IDENT, TOKEN_EXP,
        TOKEN_LESS, TOKEN_LESS_EQ, TOKEN_GREATER,
        TOKEN_GREATER_EQ, TOKEN_EQUAL, TOKEN_NOT_EQUAL,
        TOKEN_ASSIGN, TOKEN_NOT, TOKEN_LPAREN,
        TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE,
        TOKEN_LBRACKET, TOKEN_RBRACKET, TOKEN_AND,
        TOKEN_OR, TOKEN_DOT, TOKEN_AT,
        TOKEN_INTEGER, TOKEN_STRING, TOKEN_COLON,
        TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_REAL,
        TOKEN_MOD, TOKEN_UNEG, TOKEN_UPLUS
    };

    const std::unordered_set<int> binary_arithmetic_operators = {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULT,
        TOKEN_DIV, TOKEN_EXP,
        TOKEN_MOD
    };

    const std::unordered_set<int> unary_arithmetic_operators = {
        TOKEN_UNEG, TOKEN_UPLUS
    };

    const std::unordered_set<int> binary_relational_operators = {
        TOKEN_LESS, TOKEN_LESS_EQ, TOKEN_GREATER,
        TOKEN_GREATER_EQ, TOKEN_EQUAL, TOKEN_NOT_EQUAL
    };

    const std::unordered_set<int> binary_logical_operators = {
        TOKEN_OR, TOKEN_AND
    };

    const std::unordered_set<int> unary_logical_operators = {
        TOKEN_NOT
    };
};

bool at_eof(const Token& t) {
    return t.id == TOKEN_EOF;
}

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

bool Token::is(int token) {
    return id == token;
}

bool Token::is_not(int token) {
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

bool Token::is_semicolon() {
    return is(TOKEN_SEMICOLON);
}

bool Token::is_lbrace() {
    return is(TOKEN_LBRACE);
}

bool Token::is_rbrace() {
    return is(TOKEN_RBRACE);
}

bool Token::is_eof() {
    return is(TOKEN_EOF);
}
