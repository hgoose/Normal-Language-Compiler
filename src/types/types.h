#ifndef NLC_TYPES_H
#define NLC_TYPES_H

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <string>

struct BufState;
struct AST_NODE;

enum class NODE_TYPE : unsigned int;

typedef int TokenValue;
typedef int ErrorValue;

typedef std::function<void(int)> MoveProcedure;

typedef std::unordered_set<TokenValue> TokenSet;
typedef std::vector<std::string> TokenNames;
template<typename T> using TokenMap = std::unordered_map<TokenValue, T>;
template<typename T> using IdentMap = std::unordered_map<std::string, T>;

typedef std::unordered_map<std::string, std::function<AST_NODE*(void)>> ParseMap;
typedef std::unordered_map<NODE_TYPE, std::function<bool(AST_NODE*)>> EvalMap;

typedef std::unordered_set<std::string> ReservedWords;

#endif
