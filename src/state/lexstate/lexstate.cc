#include "lexstate.h"

LexState::LexState(Token next_token, Token last_token, BufState bufstate) 
    : next_token(next_token), last_token(last_token), bufstate(bufstate)
{} 
