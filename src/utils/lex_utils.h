#ifndef NLC_LEX_UTILS_H
#define NLC_LEX_UTILS_H

#include "types.h"

struct Token;
struct Error;

Error get_end_of_block_comment(Token&, char&);
Error token_either_this_or_that(Token&, TokenValue, TokenValue, char&, char);

#endif
