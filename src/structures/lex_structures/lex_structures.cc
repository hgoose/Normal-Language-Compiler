#include "lex_structures.h"
#include "token.h"
#include "lex.h"

const EscapesMap<char> escapes_map {
    {'n', ESC_NEWLINE},
    {'t', ESC_TAB},
    {'r', ESC_CARRIAGE_RETURN},
    {'"', ESC_QUOTE},
    {'\\', ESC_BACKSLASH},
    {'a', ESC_ALERT},
    {'b', ESC_BACKSPACE},
    {'\n', EMPTY}
};

const WhitespaceSet whitespace {
    '\n', ' ', '\t', '\r'
};

const CharSet simple_lex_set {
    '*', '^', '(', ')', 
    '{', '}', '[', ']', 
    '&', '|', '@', ';', ','
};

// Logically tied to simple_lex_set
const CharToTokenMap char_to_token {
        {'*', TOKEN_MULT},
        {'^', TOKEN_EXP},
        {'(', TOKEN_LPAREN},
        {')', TOKEN_RPAREN},
        {'{', TOKEN_RBRACE},
        {'}', TOKEN_LBRACE},
        {'[', TOKEN_LBRACKET},
        {']', TOKEN_RBRACKET},
        {'&', TOKEN_AND},
        {'|', TOKEN_OR},
        {';', TOKEN_SEMICOLON},
        {',', TOKEN_COMMA}
};

const LexMethodMap<char> char_to_lex_method {
    {'<', lex_char_less},
    {'>', lex_char_greater},
    {'=', lex_char_equal},
    {'!', lex_char_not},
    {'/', lex_char_slash},
    {'"', lex_char_quote},
    {'.', lex_char_dot}
};

bool replace_char_with_escape(char& c, char& next) {
    if (escapes_map.find(next) == escapes_map.end()) {
        return false;
    }

    c = escapes_map.at(next);
    return true;
}

bool char_is_whitespace(char c) {
    return whitespace.find(c) != whitespace.end();
}

bool char_in_simple_lex_set(char c) {
    return simple_lex_set.find(c) != simple_lex_set.end();
}

LexMethod map_char_to_lex_method(char c) {
    if (char_to_lex_method.find(c) == char_to_lex_method.end()) {
        return nullptr;
    }

    return char_to_lex_method.at(c);
}
