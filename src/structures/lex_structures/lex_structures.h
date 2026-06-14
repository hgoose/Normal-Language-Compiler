#ifndef NLC_LEX_STRUCTURES_H
#define NLC_LEX_STRUCTURES_H

#include "types.h"

#define ESC_NEWLINE              10
#define ESC_TAB                  9
#define ESC_CARRIAGE_RETURN      13
#define ESC_QUOTE                34
#define ESC_BACKSLASH            92
#define ESC_ALERT                7
#define ESC_BACKSPACE            8
#define EMPTY                    0

extern const EscapesMap<char> escapes_map;
extern const WhitespaceSet whitespace;
extern const CharSet simple_lex_set;
extern const CharToTokenMap char_to_token;
extern const LexMethodMap<char> char_to_lex_method;

bool replace_char_with_escape(char&, char&);
bool char_is_whitespace(char);
bool char_in_simple_lex_set(char);
LexMethod map_char_to_lex_method(char c);

#endif
