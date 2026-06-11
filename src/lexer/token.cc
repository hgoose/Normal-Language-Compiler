#include "token.h"
#include "token_structures.h"
#include "ast_node.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using std::cout;
using std::cerr;
using std::string;

// Prints a token
void print_token(const Token& t) {
    if (t.is_null()) return;

    // Guaranteed values of interest
    if (t.id < 0 || static_cast<size_t>(t.id) >= TOKEN_STRUCTURES::token_names.size()) {
        cerr << "Tried to access a token id that does not exist\n\n";
        return;
    }

    // Custom output depending on token type
    cout << TOKEN_STRUCTURES::token_names[t.id] << ": " << t.lexeme 
        << " at " << t.line_no << ":" << t.col_no << '\n';
}

void Token::reset() {
    id = TOKEN_NULL;

    line_no = -1;
    col_no = -1;
    integer = -1;

    fl = -1.f;

    lexeme = "";
    str = "";
    identifier = "";
}

void Token::set_id(TokenValue token_value) {
    id = token_value;
}

void Token::set_integer(int integer) {
    this->integer = integer;
}

void Token::set_float(double fl) {
    this->fl = fl;
}

void Token::set_str(std::string str) {
    this->str = str;
}

void Token::set_identifier(std::string identifier) {
    this->identifier = identifier;
}

void Token::set_lexeme(std::string lexeme) {
    this->lexeme = lexeme;
}

void Token::set_eof() {
    set_id(TOKEN_EOF);
}

void Token::set_line_and_col(int line, int col) {
    line_no = line;
    col_no = col;
}

bool Token::is(TokenValue token) const {
    return id == token;
}

bool Token::is_not(TokenValue token) const {
    return !is(token);
}

bool Token::is_null() const {
    return is(TOKEN_NULL);
}

bool Token::is_ident() const {
    return is(TOKEN_IDENT);
}

bool Token::is_not_ident() const {
    return !is_ident();
}

bool Token::is_ident_reserved() const {
    return is_ident() && is_reserved(*this);
}

bool Token::is_ident_if() const {
    return is_ident() && identifier == "if";
}

bool Token::is_ident_else() const {
    return is_ident() && identifier == "else";
}

bool Token::is_ident_while() const {
    return is_ident() && identifier == "while";
}

bool Token::is_ident_true() const {
    return is_ident() && identifier == "true";
}

bool Token::is_ident_false() const {
    return is_ident() && identifier == "false";
}

bool Token::is_type() const {
    return is_ident() && 
        TOKEN_STRUCTURES::types.find(identifier) 
        != TOKEN_STRUCTURES::types.end(); 
}

bool Token::is_type_int() const {
    return is_ident() && identifier == "int";
}

bool Token::is_boolean() const {
    return is_ident_true() || is_ident_false();
}

bool Token::is_semicolon() const {
    return is(TOKEN_SEMICOLON);
}

bool Token::is_lbrace() const {
    return is(TOKEN_LBRACE);
}

bool Token::is_rbrace() const {
    return is(TOKEN_RBRACE);
}

bool Token::is_comma() const {
    return is(TOKEN_COMMA);
}

bool Token::is_eof() const {
    return is(TOKEN_EOF);
}

bool Token::is_operator() const {
    return TOKEN_STRUCTURES::operators.find(id) 
        != TOKEN_STRUCTURES::operators.end();
}

bool Token::is_non_data_terminal() const {
    return TOKEN_STRUCTURES::non_data_terminals.find(id) 
        != TOKEN_STRUCTURES::non_data_terminals.end();
}

bool Token::legal_before_uplus_or_uneg() const {
    return TOKEN_STRUCTURES::before_uplus_or_uneg.find(id)
        != TOKEN_STRUCTURES::before_uplus_or_uneg.end();
}

bool Token::in(const TokenSet& set) const  { 
    return set.find(id) != set.end(); 
}

bool Token::set_id_from_predicate(TokenValue token_value, TokenMethod predicate) {
    if ((this->*predicate)()) {
        id = token_value; 
        return true;
    }
    return false;
}

