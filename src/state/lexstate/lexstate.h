#ifndef NLC_LEXSTATE_H
#define NLC_LEXSTATE_H

#include "token.h"
#include "bufstate.h"

struct LexState {
    Token next_token{};
    BufState bufstate{};

    LexState() = default;
    LexState(Token, BufState);
};

#endif
