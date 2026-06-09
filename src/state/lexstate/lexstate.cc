#include "lexstate.h"

LexState::LexState(Token next_token, BufState bufstate) 
    : next_token(next_token), bufstate(bufstate) 
{} 
