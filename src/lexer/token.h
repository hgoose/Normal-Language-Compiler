#ifndef NLC_TOKEN_H
#define NLC_TOKEN_H

#include "types.h"

#include <string>
#include <vector>

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
#define TOKEN_AT	             25
#define TOKEN_INTEGER		     26
#define TOKEN_STRING	         27
#define TOKEN_COLON	             28
#define TOKEN_SEMICOLON	         29
#define TOKEN_COMMA	             30
#define TOKEN_REAL               31
#define TOKEN_MOD                32
#define TOKEN_UNEG               33
#define TOKEN_UPLUS              34

// Mapping these to their ascii values
#define ESC_NEWLINE              10
#define ESC_TAB                  9
#define ESC_CARRIAGE_RETURN      13
#define ESC_QUOTE                34
#define ESC_BACKSLASH            92
#define ESC_ALERT                7
#define ESC_BACKSPACE            8
#define EMPTY                    0

struct Token
{
    // Guaranteed to have
    int id{0};
    std::string lexeme{};
    int line_no{-1};
    int col_no{-1};

    // Dependent on token type
    unsigned long integer{0};
    double fl{-1.0f};
    std::string str{};
    std::string identifier{};

    bool is(TokenValue);
    bool is_not(TokenValue);
    bool is_ident();
    bool is_ident_if();
    bool is_ident_else();
    bool is_ident_while();
    bool is_ident_true();
    bool is_ident_false();
    bool is_boolean();
    bool is_semicolon();
    bool is_lbrace();
    bool is_rbrace();
    bool is_eof();

    bool in(const TokenSet&); 

    template<typename...Sets>
    bool in_union(Sets...set) {
        return (in(set) || ...);
    }
};

void print_token(const Token& t);

#endif /* NCC_TOKEN_H */
