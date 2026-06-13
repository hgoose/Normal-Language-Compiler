#ifndef NLC_PARSERUTILS_H
#define NLC_PARSERUTILS_H

#include "types.h"
#include "scope_stack.h"

#include <concepts>

struct Error;
struct Token;
struct AST_NODE;

inline constexpr int LBRACE_COUNT_ZERO{};
inline constexpr int LBRACE_COUNT_ONE{};

inline constexpr bool EAT_SEMICLOON{true};
inline constexpr bool DONT_EAT_SEMICLOON{};

void skip_block(int);
void skip_if(int);
void skip_while(int);
void skip_else(int);

void onepast_semi_or_block(int);
void onepast_next_token(int);

void free_statement_return_list(StatementReturns& returns);

StatementReturns get_all_statements_in_block(Error&);

void free_tree(AST_NODE*&);
void parser_cleanup();

bool skip_if_lexerr(const Error&, MoveProcedure=onepast_semi_or_block, int=LBRACE_COUNT_ZERO);
bool unexpected_token(TokenValue, ErrorValue, MoveProcedure=onepast_semi_or_block, int=LBRACE_COUNT_ZERO);
bool unexpected_token(const Token&, TokenValue, ErrorValue, MoveProcedure=onepast_semi_or_block, int=LBRACE_COUNT_ZERO);
bool wrong_next_token(TokenValue, ErrorValue, MoveProcedure=onepast_semi_or_block, int=LBRACE_COUNT_ZERO);

AST_NODE* try_expression(bool=EAT_SEMICLOON);

AST_NODE* get_initial_value();

AST_NODE* create_assign(AST_NODE*, AST_NODE*);

void merge_statement_returns(StatementReturns&, StatementReturns);

Error munch();
void munch_all_semicolons();

template<typename...TREES>
requires (std::same_as<TREES, AST_NODE*> && ...)
void free_trees(TREES...trees) {
    (free_tree(trees),...);
}

#endif
