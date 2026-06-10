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

void free_statement_return_list(StatementReturns& returns);

void free_tree(AST_NODE*&);
void parser_cleanup();

bool skip_if_invalid_or_lexerr(const Error&, MoveProcedure=onepast_semi_or_block, int=LBRACE_COUNT_ZERO);
bool unexpected_token(TokenValue, ErrorValue, MoveProcedure=onepast_semi_or_block, int=LBRACE_COUNT_ZERO);
bool unexpected_token(const Token&, TokenValue, ErrorValue, MoveProcedure=onepast_semi_or_block, int=LBRACE_COUNT_ZERO);
bool wrong_next_token(TokenValue, ErrorValue, MoveProcedure=onepast_semi_or_block, int=LBRACE_COUNT_ZERO);

AST_NODE* try_expression();

AST_NODE* get_initial_value();

AST_NODE* create_assign(AST_NODE*, AST_NODE*);

void merge_statement_returns(StatementReturns&, StatementReturns);

Error munch();

#endif
