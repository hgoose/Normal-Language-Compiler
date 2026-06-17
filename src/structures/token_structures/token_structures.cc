#include "token_structures.h"
#include "ast_structures.h"
#include "types.h"
#include "token.h"

namespace TOKEN_STRUCTURES {
    const Types types {
        "int", "void", "string", "bool"
    };

    const TypeMap<std::string> type_map {
        {"int", TYPE::INT},
        {"bool", TYPE::BOOL},
        {"string", TYPE::STRING},
        {"void", TYPE::null}, 
    };

    const ReservedWords reserved_words {
        "print", "read", "int", "while", "if", "else", "true", "false",
        "string", "void", "bool", "call", "fn", "proc", "procedure", "return",
        "for", "invoke"
    };

    const TokenMap<std::string> operator_names {
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
        {TOKEN_OR, "or"},
        {TOKEN_NOT, "not"}
    };

    const TokenMap<NODE_TYPE> node_types {
        {TOKEN_NOT, NODE_TYPE::NOT},
        {TOKEN_AND, NODE_TYPE::AND},
        {TOKEN_OR, NODE_TYPE::OR},

        {TOKEN_LESS, NODE_TYPE::LESS},
        {TOKEN_LESS_EQ, NODE_TYPE::LEQ},
        {TOKEN_GREATER, NODE_TYPE::GREATER},
        {TOKEN_GREATER_EQ, NODE_TYPE::GEQ},
        {TOKEN_EQUAL, NODE_TYPE::EQ},
        {TOKEN_NOT_EQUAL, NODE_TYPE::NEQ},

        {TOKEN_PLUS, NODE_TYPE::ADD},
        {TOKEN_MINUS, NODE_TYPE::SUB},
        {TOKEN_UPLUS, NODE_TYPE::UPLUS},
        {TOKEN_UNEG, NODE_TYPE::UNEG},

        {TOKEN_MULT, NODE_TYPE::MULT},
        {TOKEN_DIV, NODE_TYPE::DIV},
        {TOKEN_MOD, NODE_TYPE::MOD},

        {TOKEN_INTEGER, NODE_TYPE::INT},
        {TOKEN_STRING, NODE_TYPE::STR},
        {TOKEN_IDENT, NODE_TYPE::VAR}
    };

    const IdentMap<bool> booleans {
        {"true", true},
        {"false", false}
    };

    const TokenMap<std::string> token_name_map {
        {TOKEN_NULL, "TOKEN_NULL"},
        {TOKEN_EOF, "TOKEN_EOF"},
        {TOKEN_PLUS, "TOKEN_PLUS"},
        {TOKEN_MINUS, "TOKEN_MINUS"},
        {TOKEN_MULT, "TOKEN_MULT"},
        {TOKEN_DIV, "TOKEN_DIV"},
        {TOKEN_IDENT, "TOKEN_IDENT"},
        {TOKEN_EXP, "TOKEN_EXP"},
        {TOKEN_LESS, "TOKEN_LESS"},
        {TOKEN_LESS_EQ, "TOKEN_LESS_EQ"},
        {TOKEN_GREATER, "TOKEN_GREATER"},
        {TOKEN_GREATER_EQ, "TOKEN_GREATER_EQ"},
        {TOKEN_EQUAL, "TOKEN_EQUAL"},
        {TOKEN_NOT_EQUAL, "TOKEN_NOT_EQUAL"},
        {TOKEN_ASSIGN, "TOKEN_ASSIGN" },
        {TOKEN_NOT, "TOKEN_NOT" },
        {TOKEN_LPAREN, "TOKEN_LPAREN"},
        {TOKEN_RPAREN, "TOKEN_RPAREN" },
        {TOKEN_LBRACE, "TOKEN_LBRACE" },
        {TOKEN_RBRACE, "TOKEN_RBRACE"},
        {TOKEN_LBRACKET, "TOKEN_LBRACKET" },
        {TOKEN_RBRACKET, "TOKEN_RBRACKET" },
        {TOKEN_AND, "TOKEN_AND"},
        {TOKEN_OR, "TOKEN_OR" },
        {TOKEN_DOT, "TOKEN_DOT" },
        {TOKEN_INTEGER, "TOKEN_INTEGER" },
        {TOKEN_STRING, "TOKEN_STRING" },
        {TOKEN_SEMICOLON, "TOKEN_SEMICOLON" },
        {TOKEN_COMMA, "TOKEN_COMMA" },
        {TOKEN_REAL, "TOKEN_REAL"},
        {TOKEN_MOD, "TOKEN_MOD" },
        {TOKEN_UNEG, "TOKEN_UNEG" },
        {TOKEN_UPLUS, "TOKEN_UPLUS"}, 
        {TOKEN_ARROW, "TOKEN_ARROW"}
    };

    const TokenNames token_names {
        "TOKEN_NULL", "TOKEN_EOF", "TOKEN_PLUS",
        "TOKEN_MINUS", "TOKEN_MULT", "TOKEN_DIV",
        "TOKEN_IDENT", "TOKEN_EXP", "TOKEN_LESS",
        "TOKEN_LESS_EQ", "TOKEN_GREATER", "TOKEN_GREATER_EQ",
        "TOKEN_EQUAL", "TOKEN_NOT_EQUAL", "TOKEN_ASSIGN",
        "TOKEN_NOT", "TOKEN_LPAREN", "TOKEN_RPAREN",
        "TOKEN_LBRACE", "TOKEN_RBRACE", "TOKEN_LBRACKET",
        "TOKEN_RBRACKET", "TOKEN_AND", "TOKEN_OR",
        "TOKEN_DOT", "TOKEN_INTEGER", "TOKEN_STRING", 
        "TOKEN_SEMICOLON", "TOKEN_COMMA", "TOKEN_REAL", 
        "TOKEN_MOD", "TOKEN_UNEG", "TOKEN_UPLUS", "TOKEN_ARROW" 
    };

    const TokenSet terminals {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULT,
        TOKEN_DIV, TOKEN_IDENT, TOKEN_EXP,
        TOKEN_LESS, TOKEN_LESS_EQ, TOKEN_GREATER,
        TOKEN_GREATER_EQ, TOKEN_EQUAL, TOKEN_NOT_EQUAL,
        TOKEN_ASSIGN, TOKEN_NOT, TOKEN_LPAREN,
        TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE,
        TOKEN_LBRACKET, TOKEN_RBRACKET, TOKEN_AND,
        TOKEN_OR, TOKEN_DOT, TOKEN_INTEGER, TOKEN_STRING, 
        TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_REAL,
        TOKEN_MOD, TOKEN_UNEG, TOKEN_UPLUS, TOKEN_ARROW
    };

    const TokenSet data_terminals {
        TOKEN_INTEGER, TOKEN_STRING, TOKEN_IDENT,
        TOKEN_REAL
    };

    const TokenSet non_data_terminals {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_UNEG, 
        TOKEN_UPLUS, TOKEN_MULT, TOKEN_DIV,
        TOKEN_EXP, TOKEN_MOD, TOKEN_LESS,
        TOKEN_LESS_EQ, TOKEN_GREATER, TOKEN_GREATER_EQ,
        TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_ASSIGN, 
        TOKEN_NOT, TOKEN_AND, TOKEN_OR, 
        TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, 
        TOKEN_RBRACE, TOKEN_LBRACKET, TOKEN_RBRACKET, 
        TOKEN_DOT, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_NULL,
        TOKEN_ARROW
    };

    const TokenSet before_uplus_or_uneg {
        TOKEN_NULL, TOKEN_PLUS, TOKEN_UNEG,
        TOKEN_UPLUS, TOKEN_MINUS, TOKEN_MULT,
        TOKEN_DIV, TOKEN_EXP, TOKEN_MOD,
        TOKEN_LESS, TOKEN_LESS_EQ, TOKEN_GREATER,
        TOKEN_GREATER_EQ, TOKEN_EQUAL, TOKEN_NOT_EQUAL,
        TOKEN_ASSIGN, TOKEN_NOT, TOKEN_AND,
        TOKEN_OR, TOKEN_LPAREN, TOKEN_COMMA,
        TOKEN_DOT, 
    };

    const TokenSet operators {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_UNEG,
        TOKEN_UPLUS, TOKEN_MULT, TOKEN_DIV,
        TOKEN_MOD, TOKEN_EXP, TOKEN_LESS,
        TOKEN_LESS_EQ, TOKEN_GREATER, TOKEN_GREATER_EQ,
        TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_NOT,
        TOKEN_AND, TOKEN_OR
    };

    const TokenSet binary_arithmetic_operators {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULT,
        TOKEN_DIV, TOKEN_EXP,
        TOKEN_MOD
    };

    const TokenSet unary_arithmetic_operators {
        TOKEN_UNEG, TOKEN_UPLUS
    };

    const TokenSet binary_relational_operators {
        TOKEN_LESS, TOKEN_LESS_EQ, TOKEN_GREATER,
        TOKEN_GREATER_EQ, TOKEN_EQUAL, TOKEN_NOT_EQUAL
    };

    const TokenSet binary_logical_operators {
        TOKEN_OR, TOKEN_AND
    };

    const TokenSet unary_logical_operators {
        TOKEN_NOT
    };
};
