#ifndef NLC_ERROR_H
#define NLC_ERROR_H

#include "types.h"

#define  NLC_OK                     0
#define  NLC_FILE_NOT_FOUND        -1
#define  NLC_EOF                   -2
#define  NLC_BOF                   -3
#define  NLC_IO_ERROR              -4
#define  NLC_NOT_FOUND             -5
#define  NLC_UNEXPECTED_EOF        -6
#define  NLC_ILLEGAL_COMMENT       -7
#define  NLC_ILLEGAL_ESCAPE        -8
#define  NLC_UNDEFINED_TOKEN       -9
#define  NLC_INVALID_NUMBER       -10
#define  NLC_SYNTAX_ERROR         -11
#define  NLC_EXPECTED_RPAREN      -12
#define  NLC_EXPECTED_EXPRESSION  -13
#define  NLC_STR_TABLE_OVERFLOW   -14
#define  NLC_INVALID_OPERAND_TYPE -15
#define  NLC_INT_TABLE_OVERFLOW   -16
#define  NLC_VARIABLE_NAME_RESERVED -17
#define  NLC_SYMBOL_ALREADY_EXISTS -18
#define  NLC_INVALID_IDENTIFIER    -19
#define  NLC_UNKNOWN_VARIABLE      -20
#define  NLC_EXPECTED_VAR          -21
#define  NLC_UNACCEPTABLE_TYPE_MISMATCH -22
#define  NLC_EXPECTED_STATEMENT    -23
#define  NLC_NON_LOGICAL_CONDITION -24
#define  NLC_EXPECTED_SEMICOLON    -25
#define  NLC_NO_MATCH              -26

struct Token;

inline constexpr int CRITICAL{1};

struct Error
{
  ErrorValue error{NLC_OK};

  int line, col;

  Error() = default;

  Error(ErrorValue error) : error(error) {}

  Error(ErrorValue error, int line, int col) 
      : error(error), line(line), col(col) 
  {}

  bool is(ErrorValue);
  bool is_not(ErrorValue);
  bool is_eof();
  bool is_bof();
  bool is_ok();
  bool is_unexpected_eof();
};

bool invalid_lookahead();
bool handle_lex_error(const Error&);

const char* error_string(ErrorValue);
void print_error(const Error&);

void get_next_token_and_print_error();

void set_print_token_error(Error&, ErrorValue);
void set_print_token_error(Error&&, ErrorValue);
void set_print_token_error(Error&, const Token&, ErrorValue);
void set_print_token_error(Error&&, const Token&, ErrorValue);

Error create_error(ErrorValue);

#endif /* NCC_ERROR_H */
