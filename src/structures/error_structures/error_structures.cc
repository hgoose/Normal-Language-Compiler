#include "error_structures.h"
#include "error.h"

const ErrorMap error_map {
    {NLC_FILE_NOT_FOUND, "file not found"},
    {NLC_UNEXPECTED_EOF, "unexpected eof"},
    {NLC_IO_ERROR, "I/O error"},
    {NLC_NOT_FOUND, "line not found"},
    {NLC_ILLEGAL_COMMENT, "illegal comment"},
    {NLC_ILLEGAL_ESCAPE, "illegal escape"},
    {NLC_UNDEFINED_TOKEN, "undefined token"},
    {NLC_INVALID_NUMBER, "invalid number"},
    {NLC_SYNTAX_ERROR, "Syntax error"},
    {NLC_EXPECTED_RPAREN, "expected )"},
    {NLC_EXPECTED_EXPRESSION, "expected expression, none provided"},
    {NLC_EXPECTED_STATEMENT, "expected statement, none provided"},
    {NLC_EXPECTED_SEMICOLON, "expected semicolon"},
    {NLC_STR_TABLE_OVERFLOW, "string table overrun"},
    {NLC_INVALID_OPERAND_TYPE, "invalid operand type"},
    {NLC_INT_TABLE_OVERFLOW, "integer table overrun"},
    {NLC_VARIABLE_NAME_RESERVED, "attempted to create a variable using a reserved name"},
    {NLC_SYMBOL_ALREADY_EXISTS, "symbol already exists"},
    {NLC_INVALID_IDENTIFIER, "invalid identifier"},
    {NLC_UNKNOWN_VARIABLE, "variable has not been declared"},
    {NLC_EXPECTED_VAR, "expected variable, none provided"},
    {NLC_UNACCEPTABLE_TYPE_MISMATCH, "type mismatch cannot be justified"},
    {NLC_NON_LOGICAL_CONDITION, "the provided expression cannot be interpreted as logical"},
    {NLC_EXPECTED_IDENTIFIER, "expected identifier"},
    {NLC_EXPECTED_PARAMETER_TYPE, "expected parameter type"},
    {NLC_EXPECTED_FN_BODY, "expected function definition"},
};

bool error_unknown(ErrorValue error_value) {
    return error_map.find(error_value) == error_map.end();
}

std::string error_string(ErrorValue error_value) {
    return (error_unknown(error_value) 
        ? "Unknown error" : error_map.at(error_value) 
    );
}
