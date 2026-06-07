#ifndef NCC_ERROR_H
#define NCC_ERROR_H

#define CRITICAL                    1

#define  NCC_OK                     0
#define  NCC_FILE_NOT_FOUND        -1
#define  NCC_EOF                   -2
#define  NCC_BOF                   -3
#define  NCC_IO_ERROR              -4
#define  NCC_NOT_FOUND             -5
#define  NCC_UNEXPECTED_EOF        -6
#define  NCC_ILLEGAL_COMMENT       -7
#define  NCC_ILLEGAL_ESCAPE        -8
#define  NCC_UNDEFINED_TOKEN       -9
#define  NCC_INVALID_NUMBER       -10
#define  NCC_SYNTAX_ERROR         -11
#define  NCC_EXPECTED_RPAREN      -12
#define  NCC_EXPECTED_EXPRESSION  -13
#define  NCC_STR_TABLE_OVERFLOW   -14
#define  NCC_INVALID_OPERAND_TYPE -15
#define  NCC_INT_TABLE_OVERFLOW   -16
#define  NCC_VARIABLE_NAME_RESERVED -17
#define  NCC_SYMBOL_ALREADY_EXISTS -18
#define  NCC_INVALID_IDENTIFIER    -19
#define  NCC_UNKNOWN_VARIABLE      -20
#define  NCC_EXPECTED_VAR          -21
#define  NCC_UNACCEPTABLE_TYPE_MISMATCH -22
#define  NCC_EXPECTED_STATEMENT    -23
#define  NCC_NON_LOGICAL_CONDITION -24
#define  NCC_EXPECTED_SEMICOLON    -25

struct Token;

struct Error
{
  int error{NCC_OK};
  int line, col;
};

bool invalid_lookahead();
bool handle_lex_error(const Error& err);

const char* error_string(int);
void print_error(const Error&);

void get_next_token_and_print_error();

void set_print_token_error(Error&, int);
void set_print_token_error(Error&&, int);
void set_print_token_error(Error&, const Token&, int);
void set_print_token_error(Error&&, const Token&, int);


#endif /* NCC_ERROR_H */
