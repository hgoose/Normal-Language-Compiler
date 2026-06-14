#ifndef NLC_TYPES_H
#define NLC_TYPES_H

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <list>
#include <utility>

struct BufState;
struct AST_NODE;
struct Token;
struct Error;
struct SYMINFO;

enum class NODE_TYPE : unsigned int;
enum class TYPE : unsigned int;

typedef int ReturnCode;

typedef std::list<AST_NODE*> StatementReturns;

typedef int TokenValue;
typedef int ErrorValue;

typedef std::function<void(int)> MoveProcedure;

typedef std::unordered_set<TokenValue> TokenSet;
typedef std::vector<std::string> TokenNames;
template<typename T> using TokenMap = std::unordered_map<TokenValue, T>;
template<typename T> using NodeTypeMap = std::unordered_map<NODE_TYPE, T>;
template<typename T> using IdentMap = std::unordered_map<std::string, T>;

typedef std::unordered_map<std::string, std::function<StatementReturns(void)>> ParseMap;
typedef std::unordered_map<NODE_TYPE, std::function<bool(AST_NODE*)>> EvalMap;

typedef std::unordered_set<std::string> ReservedWords;
typedef std::unordered_set<std::string> Types;

typedef std::list<AST_NODE*> Children;

template<typename T> using EscapesMap = std::unordered_map<T, TokenValue>;
typedef std::unordered_map<char, TokenValue> CharToTokenMap;

typedef std::unordered_set<char> WhitespaceSet;
typedef std::unordered_set<char> CharSet;

typedef bool (Token::*TokenMethod)() const;
typedef Error(*LexMethod)(Token&, char&);

template<typename T> using LexMethodMap = std::unordered_map<T, LexMethod>;

typedef std::unordered_map<ErrorValue, std::string> ErrorMap;

typedef bool(*CodegenFn)(AST_NODE*);
typedef std::unordered_map<NODE_TYPE, CodegenFn> NodeToCodegenFnMap;

typedef std::unordered_set<NODE_TYPE> StatementNodes;

typedef int ScopeLevel;
typedef std::pair<ScopeLevel, std::list<SYMINFO*>> ScopeLevelPair;
typedef std::list<ScopeLevelPair> ScopeStack;
typedef std::list<SYMINFO*> SymbolBucket;

template<typename T> using TypeMap = std::unordered_map<T, TYPE>;
template<typename T> using InvTypeMap = std::unordered_map<TYPE,T>;

#endif
