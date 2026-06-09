#ifndef NLC_PARSER_UTILS
#define NLC_PARSER_UTILS

#include "types.h"

struct Error;
struct Token;
struct AST_NODE;

inline constexpr int LBRACE_COUNT_ZERO{};
inline constexpr int LBRACE_COUNT_ONE{};

void skip_block();
void skip_if(int);
void skip_while(int);
void skip_else(int);

void onepast_semi_or_block(int);
void onepast_next_token(int);

void free_tree(AST_NODE*&);
void parser_cleanup();

bool skip_if_invalid_or_lexerr(const Error&, MoveProcedure mv_proc=onepast_semi_or_block, int lbrace_count=LBRACE_COUNT_ZERO);
bool unexpected_token(TokenValue, ErrorValue, MoveProcedure mv_proc=onepast_semi_or_block, int lbrace_count=LBRACE_COUNT_ZERO);
bool wrong_next_token(TokenValue, ErrorValue, MoveProcedure mv_proc=onepast_semi_or_block, int lbrace_count=LBRACE_COUNT_ZERO);

bool try_expression();

#endif
