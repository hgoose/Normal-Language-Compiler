#ifndef NLC_LEX_UTILS_H
#define NLC_LEX_UTILS_H

#include "types.h"

struct Token;
struct Error;

inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
inline bool valid_identifier_start(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
inline bool valid_identifier_poststart(char c) { return isdigit(c) || valid_identifier_start(c); }
inline bool char_is_exponent(char c) { return c == 'e' || c == 'E'; }
inline bool char_is_radix_point(char c) { return c == '.'; } 
inline bool char_is_sign(char c) { return c == '+' || c == '-'; }

Error skip_whitespace(char&);
Error skip_current_and_whitespace(char&);

Error get_end_of_block_comment(char&);
Error token_either_this_or_that(Token&, TokenValue, TokenValue, char&, char);

bool char_is_exponent(char c);
bool char_is_sign(char c);
std::string consume_digits_after_current(char&);

Error handle_exponent(Token&, char&);
Error build_float_after_dot(Token&, char&);

bool unknown_token_name(TokenValue token_value);
std::string get_token_name(TokenValue);

#endif
