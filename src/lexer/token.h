#ifndef NLC_TOKEN_H
#define NLC_TOKEN_H

#include "types.h"

#include <string>

/*******     Token  id  values   ******/
#define TOKEN_NULL               0
#define TOKEN_EOF	             1
#define TOKEN_PLUS	             2
#define TOKEN_MINUS	             3
#define TOKEN_MULT	             4
#define TOKEN_DIV	             5
#define TOKEN_IDENT	             6
#define TOKEN_EXP	             7
#define TOKEN_LESS               8
#define TOKEN_LESS_EQ            9 
#define TOKEN_GREATER            10
#define TOKEN_GREATER_EQ         11 
#define TOKEN_EQUAL	             12
#define TOKEN_NOT_EQUAL	         13
#define TOKEN_ASSIGN             14
#define TOKEN_NOT	             15
#define TOKEN_LPAREN             16
#define TOKEN_RPAREN             17
#define TOKEN_LBRACE             18
#define TOKEN_RBRACE             19
#define TOKEN_LBRACKET           20
#define TOKEN_RBRACKET           21
#define TOKEN_AND	             22
#define TOKEN_OR	             23
#define TOKEN_DOT	             24
#define TOKEN_INTEGER		     26
#define TOKEN_STRING	         27
#define TOKEN_SEMICOLON	         29
#define TOKEN_COMMA	             30
#define TOKEN_REAL               31
#define TOKEN_MOD                32
#define TOKEN_UNEG               33
#define TOKEN_UPLUS              34
#define TOKEN_ARROW              35

struct Token
{
    // Guaranteed to have
    int id{0};
    std::string lexeme{};
    int line_no{-1};
    int col_no{-1};

    // Dependent on token type
    unsigned long integer{};
    double fl{-1.0f};
    std::string str{};
    std::string identifier{};

    void reset();
    void set_line_and_col(int, int);
    void set_id(TokenValue);
    void set_integer(int);
    void set_float(double);
    void set_str(std::string);
    void set_identifier(std::string);
    void set_lexeme(std::string);
    void append_lexeme(std::string);
    void append_lexeme(char);

    std::string get_name() const;

    void set_eof();
    void set_null();

    bool is(TokenValue) const;
    bool is_not(TokenValue) const;

    bool is_null() const;

    bool is_ident() const;
    bool is_not_ident() const;
    bool is_ident_reserved() const;

    bool is_ident_if() const;
    bool is_ident_else() const;
    bool is_ident_while() const;

    bool is_ident_true() const;
    bool is_ident_false() const;
    bool is_boolean() const;

    bool is_lexeme_mod() const;
    void set_mod_if_mod();

    bool is_type() const;
    bool is_not_type() const;
    bool is_type_int() const;

    bool is_semicolon() const;
    bool is_lbrace() const;
    bool is_block_begin() const;
    bool is_block_end() const;
    bool is_rbrace() const;
    bool is_comma() const;
    bool is_lparen() const;
    bool is_rparen() const;
    bool is_arrow() const;

    bool is_eof() const;

    bool is_relational() const;
    bool is_logical() const;
    bool is_arithmetic() const;

    bool is_operator() const;

    bool is_non_data_terminal() const;

    bool legal_before_uplus_or_uneg() const;

    bool in(const TokenSet&) const; 

    template<typename...Sets>
    bool in_union(Sets...set) const {
        return (in(set) || ...);
    }

    bool set_id_from_predicate(TokenValue, TokenMethod);

    TYPE get_type() const;
};

void print_token(const Token& t);

#endif /* NCC_TOKEN_H */
