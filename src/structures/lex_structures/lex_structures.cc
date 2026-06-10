#include "lex_structures.h"

bool replace_char_with_escape(char& c, char& next) {
    if (escapes_map.find(next) != escapes_map.end()) {
        c = escapes_map.at(next);
        return true;
    }
    return false;
}

const EscapesMap<char> escapes_map = {
    {'n', ESC_NEWLINE},
    {'t', ESC_TAB},
    {'r', ESC_CARRIAGE_RETURN},
    {'"', ESC_QUOTE},
    {'\\', ESC_BACKSLASH},
    {'a', ESC_ALERT},
    {'b', ESC_BACKSPACE},
    {'\n', EMPTY}
};
