#include "ast_node.h"

#include "token.h"
#include "token_structures.h"
#include "ast_structures.h"

bool is_reserved(const Token& t) {
    return TOKEN_STRUCTURES::reserved_words.find(t.lexeme) 
        != TOKEN_STRUCTURES::reserved_words.end();
}

AST_NODE* binary_arithmetic_type_compliance(AST_NODE* left, AST_NODE* right) {
    if (!left->is_type_integral())
        return left;
    else if (!right->is_type_integral())
        return right;

    return nullptr;
}

AST_NODE* unary_arithmetic_type_compliance(AST_NODE* child) {
    return (child->is_type_integral() ? nullptr : child);
}

AST_NODE* binary_relational_type_compliance(AST_NODE* left, AST_NODE* right) {
    return binary_arithmetic_type_compliance(left, right);
}

AST_NODE* binary_logical_type_compliance(AST_NODE* left, AST_NODE* right) {
    if (!left->is_type_logical())
        return left;
    else if (!right->is_type_logical())
        return right;

    return nullptr;
}

AST_NODE* unary_logical_type_compliance(AST_NODE* child) {
    return (child->is_type_logical() ? nullptr : child);
}

NODE_TYPE get_node_type(const Token& t) {
    auto find = TOKEN_STRUCTURES::node_types.find(t.id);
    auto end = TOKEN_STRUCTURES::node_types.end();

    if (find == end) return NODE_TYPE::null;

    return TOKEN_STRUCTURES::node_types.at(t.id);
}

AST_NODE::AST_NODE(ScopeLevel scope_level) 
    : statement_scope_level(scope_level) 
{}

AST_NODE::AST_NODE(Token token, ScopeLevel scope_level) 
    : token(token),
    statement_scope_level(scope_level) 
{}

AST_NODE::AST_NODE(NODE_TYPE node_type, ScopeLevel scope_level) 
    : node_type(node_type),
    statement_scope_level(scope_level)
{}

AST_NODE::AST_NODE(Token token, NODE_TYPE node_type, ScopeLevel scope_level) 
    : token(token),
      node_type(node_type),
      statement_scope_level(scope_level)
{}

AST_NODE::AST_NODE(Token token, NODE_TYPE node_type, bool is_operator, ScopeLevel scope_level) 
    : token(token),
      node_type(node_type),
      is_operator(is_operator),
      statement_scope_level(scope_level)
{}

AST_NODE::AST_NODE(Token token, NODE_TYPE node_type, TYPE data_type, ScopeLevel scope_level) 
    : token(token),
      node_type(node_type),
      data_type(data_type),
      statement_scope_level(scope_level)
{
    set_boolean();
}

AST_NODE::AST_NODE(Token token, NODE_TYPE node_type, SYMTYPE symbol_type, ScopeLevel scope_level) 
    : token(token),
      node_type(node_type),
      symbol_type(symbol_type),
      statement_scope_level(scope_level)
{}

AST_NODE::AST_NODE(Token token, NODE_TYPE node_type, TYPE data_type, SYMTYPE symbol_type, ScopeLevel scope_level) 
    : token(token),
    node_type(node_type),
    data_type(data_type),
    symbol_type(symbol_type),
    statement_scope_level(scope_level)
{
    set_boolean();
}


AST_NODE::AST_NODE(const AST_NODE& other, bool copy_children) {
    token = other.token;
    data_type = other.data_type;
    node_type = other.node_type;
    entry = other.entry;
    syminfo = other.syminfo;
    is_operator = other.is_operator;
    symbol_type = other.symbol_type;
    boolean = other.boolean;
    is_boolean = other.is_boolean;
    statement_scope_level = other.statement_scope_level;

    if (copy_children) {
        deep_copy_children(other);
    }
}

AST_NODE& AST_NODE::operator=(const AST_NODE& other) {
    token = other.token;
    data_type = other.data_type;
    node_type = other.node_type;
    entry = other.entry;
    syminfo = other.syminfo;
    is_operator = other.is_operator;
    symbol_type = other.symbol_type;
    boolean = other.boolean;
    is_boolean = other.is_boolean;
    statement_scope_level = other.statement_scope_level;

    deep_copy_children(other);

    return *this;
}

void AST_NODE::add_all_statements(StatementReturns returns) {
    while (returns.size()) {
        AST_NODE* front = returns.front();
        returns.pop_front();

        add_children(front);
    }
}

void AST_NODE::clear() {
    token = Token{};
    is_operator = false;
    node_type = NODE_TYPE{};
    data_type = TYPE{};
    symbol_type = SYMTYPE{};
    syminfo = nullptr;
    entry = STR_TABLE_ENTRY{};
}

std::string AST_NODE::str_node_type() const {
    return node_type_to_string_map.at(node_type);
}

bool AST_NODE::is_statement() const {
    return statement_nodes.find(node_type)
        != statement_nodes.end();
}

bool AST_NODE::operator_is_arithmetic() const {
    return operator_is_binary_arithmetic() || operator_is_unary_arithmetic();
}

bool AST_NODE::operator_is_binary_arithmetic() const {
    return TOKEN_STRUCTURES::binary_arithmetic_operators.find(token.id) 
        != TOKEN_STRUCTURES::binary_arithmetic_operators.end();
}

bool AST_NODE::operator_is_unary_arithmetic() const {
    return TOKEN_STRUCTURES::unary_arithmetic_operators.find(token.id) 
        != TOKEN_STRUCTURES::unary_arithmetic_operators.end();
}

bool AST_NODE::operator_is_relational() const {
    return operator_is_binary_relational();
}

bool AST_NODE::operator_is_binary_relational() const {
    return TOKEN_STRUCTURES::binary_relational_operators.find(token.id) 
        != TOKEN_STRUCTURES::binary_relational_operators.end();
}

bool AST_NODE::operator_is_logical() const {
    return operator_is_binary_logical() || operator_is_unary_logical();
}

bool AST_NODE::operator_is_binary_logical() const {
    return TOKEN_STRUCTURES::binary_logical_operators.find(token.id) 
        != TOKEN_STRUCTURES::binary_logical_operators.end();
}

bool AST_NODE::operator_is_unary_logical() const {
    return TOKEN_STRUCTURES::unary_logical_operators.find(token.id) 
        != TOKEN_STRUCTURES::unary_logical_operators.end();
}

bool AST_NODE::operator_is_binary() const {
    return operator_is_binary_arithmetic() ||
        operator_is_binary_relational() ||
        operator_is_binary_logical();
}

bool AST_NODE::operator_is_unary() const {
    return operator_is_unary_arithmetic() ||
        operator_is_unary_logical();
}

bool AST_NODE::is_terminal() const {
    return TOKEN_STRUCTURES::terminals.find(token.id) 
        != TOKEN_STRUCTURES::terminals.end();
}

bool AST_NODE::is_var() const {
    return node_type == NODE_TYPE::VAR;
}

bool AST_NODE::is_fn_name() const {
    return node_type == NODE_TYPE::FUNCTION_IDENT;
}

bool AST_NODE::is_parameter_pack() const {
    return node_type == NODE_TYPE::PARAMETER_PACK;
}

bool AST_NODE::is_argument_pack() const {
    return node_type == NODE_TYPE::ARGUMENT_PACK;
}

bool AST_NODE::is_return_value() const {
    return node_type == NODE_TYPE::RETURN_VALUE;
}

bool AST_NODE::is_bool() const {
    return is_boolean;
}

bool AST_NODE::is_type_integral() const {
    return data_type == TYPE::INT;
}

bool AST_NODE::is_type_logical() const {
    return data_type == TYPE::BOOL;
}

bool AST_NODE::is_op() const {
    return is_operator;
}

bool AST_NODE::is_nop() const {
    return !is_op();
}

void AST_NODE::set_boolean() {
    if (!token.is_ident_true() && !token.is_ident_false()) return;

    boolean = TOKEN_STRUCTURES::booleans.at(token.identifier);
    is_boolean = true;
}

void AST_NODE::set_token(Token t) {
    token = t;
}

void AST_NODE::install_symbol(SYMINFO* info) {
    syminfo = info; 
    data_type = info->data_type;
}

void AST_NODE::deep_copy_children(const AST_NODE& other) {
    Children children_copy = other.children;
    while (children_copy.size() && children_copy.front()) {
        AST_NODE* front = children_copy.front(); 
        children_copy.pop_front();

        children.push_back(new AST_NODE(*front));
    }
}

void AST_NODE::set_scope_stack_frame(const SymbolBucket& bucket) {
    scope_stack_frame = bucket;
}

std::string AST_NODE::get_type() const {
    return inv_type_map.at(data_type);
}

bool AST_NODE::symbol_freed() const {
    if (!SYMTABLE::get_symbol(syminfo->name, syminfo->type, syminfo->scope_level)) return true;
    return false;
}
