#ifndef NLC_LEX_STRUCTURES_H
#define NLC_LEX_STRUCTURES_H

#include "types.h"

#include <string>

#define ESC_NEWLINE              10
#define ESC_TAB                  9
#define ESC_CARRIAGE_RETURN      13
#define ESC_QUOTE                34
#define ESC_BACKSLASH            92
#define ESC_ALERT                7
#define ESC_BACKSPACE            8
#define EMPTY                    0

extern const EscapesMap<char> escapes_map;

bool replace_char_with_escape(char&, char&);

#endif
