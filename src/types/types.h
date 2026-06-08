#ifndef NLC_TYPES_H
#define NLC_TYPES_H

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <string>

struct AST_NODE;
enum class NODE_TYPE : unsigned int;

typedef int TokenValue;
typedef int ErrorValue;

typedef std::function<void(int)> MoveProcedure;

typedef std::unordered_set<TokenValue> TokenSet;
typedef std::vector<std::string> TokenNames;
template<typename T> using TokenMap = std::unordered_map<TokenValue, T>;

typedef std::unordered_map<std::string, std::function<AST_NODE*(void)>> ParseMap;
typedef std::unordered_map<NODE_TYPE, std::function<bool(AST_NODE*)>> EvalMap;

typedef std::unordered_set<std::string> ReservedWords;

enum class TYPE : unsigned int;

enum class SYMTYPE : unsigned int {
    null, VAR
};

enum LOCATION_TYPE : unsigned int {
    MEMORY, REG, STACK
};

enum class NODE_TYPE : unsigned int {
    null, NOT, AND, OR,
    LESS, LEQ, GREATER,
    GEQ, EQ, NEQ,
    ADD, SUB, MULT, 
    DIV, MOD, EXP, 
    UPLUS, UNEG, DECL, 
    ASSIGN, PRINT, READ, 
    BLOCK, INT, VAR, STR, 
    BOOL, IF, ELSE, WHILE
};

enum class TYPE : unsigned int {
    null, INT4, STRING, BOOL, TYPE_MISMATCH
};

#endif
