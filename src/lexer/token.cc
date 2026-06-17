#include "token.h"
#include "ast_node.h"
#include "lex_utils.h"
#include "token_structures.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using std::cerr;
using std::cout;
using std::string;

// Prints a token
void print_token(const Token &t) {
    if (t.is_null())
        return;

    // Guaranteed values of interest
    if (unknown_token_name(t.id)) {
        cerr << "Tried to access a token id that does not exist\n\n";
        return;
    }

    // Custom output depending on token type
    cout << t.get_name() << ": " << t.lexeme << " at " << t.line_no << ":"
        << t.col_no << '\n';
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

void Token::append_lexeme(std::string s) { 
    lexeme += s; 
}

void Token::append_lexeme(char c) { 
    lexeme += c; 
}

std::string Token::get_name() const { 
    return get_token_name(id); 
}

void Token::set_eof() { 
    set_id(TOKEN_EOF); 
}

void Token::set_null() { 
    reset(); 
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

bool Token::is_ident_call() const {
    return is_ident() && identifier == "call" || identifier == "invoke";
}

bool Token::is_ident_true() const {
    return is_ident() && identifier == "true"; 
}

bool Token::is_ident_false() const {
    return is_ident() && identifier == "false";
}

bool Token::is_boolean() const { 
    return is_ident_true() || is_ident_false(); 
}

bool Token::is_lexeme_mod() const {
    return lexeme == "mod"; 
}

void Token::set_mod_if_mod() {
  if (!is_lexeme_mod()) return;

  set_id(TOKEN_MOD);
  identifier = "";
}

bool Token::is_type() const {
  return is_ident() && 
      TOKEN_STRUCTURES::types.find(identifier) 
      != TOKEN_STRUCTURES::types.end();
}

bool Token::is_not_type() const {
    return !is_type();
}

bool Token::is_type_int() const { 
    return is_ident() && identifier == "int"; 
}

bool Token::is_semicolon() const { 
    return is(TOKEN_SEMICOLON); 
}

bool Token::is_not_semicolon() const { 
    return !is_semicolon(); 
}

bool Token::is_lbrace() const { 
    return is(TOKEN_LBRACE); 
}

bool Token::is_block_begin() const { 
    return is_lbrace(); 
}

bool Token::is_block_end() const { 
    return is_rbrace(); 
}

bool Token::is_rbrace() const { 
    return is(TOKEN_RBRACE); 
}

bool Token::is_comma() const { 
    return is(TOKEN_COMMA); 
}

bool Token::is_lparen() const {
    return is(TOKEN_LPAREN);
}

bool Token::is_rparen() const {
    return is(TOKEN_RPAREN);
}

bool Token::is_not_rparen() const {
    return !is_rparen();
}

bool Token::is_arrow() const {
    return is(TOKEN_ARROW);
}

bool Token::is_assign() const {
    return is(TOKEN_ASSIGN);
}

bool Token::is_eof() const {
    return is(TOKEN_EOF); 
}

bool Token::is_operator() const {
    return TOKEN_STRUCTURES::operators.find(id)
        != TOKEN_STRUCTURES::operators.end();
}

bool Token::is_relational() const {
    return TOKEN_STRUCTURES::binary_relational_operators.find(id)
        != TOKEN_STRUCTURES::binary_relational_operators.end();
}

bool Token::is_logical() const {
    return TOKEN_STRUCTURES::binary_logical_operators.find(id) 
        != TOKEN_STRUCTURES::binary_logical_operators.end() 
        || TOKEN_STRUCTURES::unary_logical_operators.find(id) 
        != TOKEN_STRUCTURES::unary_logical_operators.end();
}

bool Token::is_arithmetic() const {
    return TOKEN_STRUCTURES::binary_arithmetic_operators.find(id) 
        != TOKEN_STRUCTURES::binary_arithmetic_operators.end() 
        || TOKEN_STRUCTURES::unary_arithmetic_operators.find(id) 
        != TOKEN_STRUCTURES::unary_arithmetic_operators.end();
}

bool Token::is_non_data_terminal() const {
    return TOKEN_STRUCTURES::non_data_terminals.find(id) !=
        TOKEN_STRUCTURES::non_data_terminals.end();
}

bool Token::legal_before_uplus_or_uneg() const {
    return TOKEN_STRUCTURES::before_uplus_or_uneg.find(id) !=
        TOKEN_STRUCTURES::before_uplus_or_uneg.end();
}

bool Token::in(const TokenSet &set) const {
    return set.find(id) != set.end(); 
}

bool Token::set_id_from_predicate(TokenValue token_value, TokenMethod predicate) {
    if ((this->*predicate)()) {
        id = token_value;
        return true;
    }
    return false;
}

TYPE Token::get_type() const {
    return (is_type() ? TOKEN_STRUCTURES::type_map.at(identifier) : TYPE::null);
}
