#ifndef NCC_PARSER_UTILS
#define NCC_PARSER_UTILS

#include <functional>

struct Error;
struct Token;
struct AST_NODE;

typedef std::function<void(int)> MOVE_PROCEDURE;

inline constexpr int LBRACE_COUNT_ZERO{};
inline constexpr int LBRACE_COUNT_ONE{};

void skip_block();
void skip_if(int);
void skip_while(int);
void skip_else(int);

void onepast_semi_or_block(int);
void goto_next_semicolon();
void onepast_next_token(int);
void onepast_next_semicolon(int=0);

void free_tree(AST_NODE*&);
void parser_cleanup();

bool skip_if_invalid_or_lexerr(const Error&, MOVE_PROCEDURE mv_proc=onepast_semi_or_block, int lbrace_count=LBRACE_COUNT_ZERO);
bool unexpected_token(int, int, MOVE_PROCEDURE mv_proc=onepast_semi_or_block, int lbrace_count=LBRACE_COUNT_ZERO);
bool wrong_next_token(int, int, MOVE_PROCEDURE mv_proc=onepast_semi_or_block, int lbrace_count=LBRACE_COUNT_ZERO);


#endif
