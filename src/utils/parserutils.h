#ifndef NCC_PARSER_UTILS
#define NCC_PARSER_UTILS

#include <functional>

struct Error;
struct Token;
struct AST_NODE;

typedef std::function<void(int)> MOVE_PROCEDURE;

void skip_block();
void skip_if(int);
void skip_while(int);
void goto_next_semicolon();
void goto_next_rbrace();
void onepast_next_token(int);
void onepast_next_semicolon(int=0);
void onepast_next_rbrace(int=0);
void free_tree(AST_NODE*&);
void parser_cleanup();

bool skip_if_invalid_or_lexerr(const Error&, MOVE_PROCEDURE mv_proc=onepast_next_semicolon, int lbrace_count=0);
bool unexpected_token(int, int, MOVE_PROCEDURE mv_proc=onepast_next_semicolon, int lbrace_count=0);
bool wrong_next_token(int, int, MOVE_PROCEDURE mv_proc=onepast_next_semicolon, int lbrace_count=0);


#endif
