#ifndef  NLC_LEX_H
#define  NLC_LEX_H

#include "error.h"

struct Token;
struct LexState;

Error lex_init(const char*  src_code);

Error get_token(Token& t);

bool lex_eof(void);  

void lex_cleanup();

void replace_escape_character(char& c);

LexState lex_save();
void lex_goto_last_save(const LexState&);

Error lex_char_less(Token&, char&);
Error lex_char_greater(Token&, char&);
Error lex_char_equal(Token&, char&);
Error lex_char_not(Token&, char&);
Error lex_char_pound(Token&, char&);
Error lex_char_alpha(Token&, char&);
Error lex_char_quote(Token&, char&);
Error lex_char_dot(Token&, char&);
Error lex_char_numeric(Token&, char&);

#endif 
