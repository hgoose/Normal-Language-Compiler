#include "sets.h"
#include "token.h"

namespace First {
    TokenSet orBAP = {TOKEN_OR};
    TokenSet andCBP = {TOKEN_AND};
    TokenSet notC = {TOKEN_NOT};

    TokenSet equalE = {TOKEN_EQUAL};
    TokenSet neqE = {TOKEN_NOT_EQUAL};
    TokenSet lessE = {TOKEN_LESS};
    TokenSet leqE = {TOKEN_LESS_EQ};
    TokenSet greaterE = {TOKEN_GREATER};
    TokenSet geqE = {TOKEN_GREATER_EQ};

    // Used in E folds. E folds replace EP(), 
    // which use first(+TEP), first(-TEP)
    TokenSet plusTEP = {TOKEN_PLUS};
    TokenSet minusTEP = {TOKEN_MINUS};

    TokenSet multNTP = {TOKEN_MULT};
    TokenSet divNTP = {TOKEN_DIV};
    TokenSet modNTP = {TOKEN_MOD};

    TokenSet uplusN = {TOKEN_UPLUS};
    TokenSet unegN = {TOKEN_UNEG};

    TokenSet expSFP = {TOKEN_EXP};

    TokenSet lpArp = {TOKEN_LPAREN};

    TokenSet integer = {TOKEN_INTEGER};
    TokenSet ident = {TOKEN_IDENT};
    TokenSet string = {TOKEN_STRING};

    TokenSet BAP = {
        TOKEN_NOT, TOKEN_UPLUS, TOKEN_UNEG,
        TOKEN_LPAREN, TOKEN_INTEGER, TOKEN_IDENT,
        TOKEN_STRING
    };

    TokenSet CBP {
        TOKEN_NOT, TOKEN_UPLUS, TOKEN_UNEG,
        TOKEN_LPAREN, TOKEN_INTEGER, TOKEN_IDENT,
        TOKEN_STRING
    };

    TokenSet D = {
        TOKEN_UPLUS, TOKEN_UNEG, TOKEN_LPAREN,
        TOKEN_INTEGER, TOKEN_IDENT, TOKEN_STRING
    };

    TokenSet EDP = {
        TOKEN_UPLUS, TOKEN_UNEG, TOKEN_LPAREN,
        TOKEN_INTEGER, TOKEN_IDENT, TOKEN_STRING
    };

    TokenSet TEP = {
        TOKEN_UPLUS, TOKEN_UNEG, TOKEN_LPAREN,
        TOKEN_INTEGER, TOKEN_IDENT, TOKEN_STRING
    };


    TokenSet NTP = {
        TOKEN_UPLUS, TOKEN_UNEG, TOKEN_LPAREN,
        TOKEN_INTEGER, TOKEN_IDENT, TOKEN_STRING
    };


    TokenSet SFP = {
        TOKEN_LPAREN, TOKEN_INTEGER, TOKEN_IDENT,
        TOKEN_STRING
    };

}
