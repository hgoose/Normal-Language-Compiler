#include "ast_node.h"

#include <algorithm>
#include <cctype>

#include "token.h"
#include "token_structures.h"
#include "ast_structures.h"

bool is_reserved(const Token& t) {
    std::string lexeme = t.lexeme;
    std::transform(lexeme.begin(), lexeme.end(), lexeme.begin(), 
            [](unsigned char c) {return std::tolower(c);
    });

    return reserved_words.find(lexeme) != reserved_words.end();
}

void AST_NODE::add_all_statements(StatementReturns returns) {
    while (returns.size()) {
        AST_NODE* front = returns.front();
        returns.pop_front();

        add_children(front);
    }
}

AST_NODE* binary_arithmetic_type_compliance(AST_NODE* left, AST_NODE* right) {
    if (!left->is_type_integral())
        return left;
    else if (!right->is_type_integral()) {
        return right;
    }

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

void AST_NODE::clear() {
    token = Token{};
    is_operator = false;
    node_type = NODE_TYPE{};
    data_type = TYPE{};
    symbol_type = SYMTYPE{};
    syminfo = nullptr;
    entry = STR_TABLE_ENTRY{};
}

std::string AST_NODE::str_node_type() {
    return std::vector<std::string>{
            "_null", "ADD", "SUB", "MULT", 
            "NOT", "AND", "OR",
            "LESS", "LEQ", "GREATER", "GEQ",
            "EQUAL", "NEQ", 
            "DIV", "MOD", "EXP", "UPLUS", 
            "UNEG", "declare", "assign", "print", 
            "read", "Statement block", "INT", "VAR", "STR",
            "bool", "if", "else", "while" 
    }[(int)node_type];
}

bool AST_NODE::is_statement() {
    return node_type == NODE_TYPE::PRINT ||
        node_type == NODE_TYPE::DECL ||
        node_type == NODE_TYPE::ASSIGN ||
        node_type == NODE_TYPE::READ ||
        node_type == NODE_TYPE::BLOCK ||
        node_type == NODE_TYPE::IF ||
        node_type == NODE_TYPE::ELSE ||
        node_type == NODE_TYPE::WHILE;
}

bool AST_NODE::operator_is_arithmetic() {
    return operator_is_binary_arithmetic() || operator_is_unary_arithmetic();
}

bool AST_NODE::operator_is_binary_arithmetic() {
    return TOKEN_STRUCTURES::binary_arithmetic_operators.find(token.id) 
        != TOKEN_STRUCTURES::binary_arithmetic_operators.end();
}

bool AST_NODE::operator_is_unary_arithmetic() {
    return TOKEN_STRUCTURES::unary_arithmetic_operators.find(token.id) 
        != TOKEN_STRUCTURES::unary_arithmetic_operators.end();
}

bool AST_NODE::operator_is_relational() {
    return operator_is_binary_relational();
}

bool AST_NODE::operator_is_binary_relational() {
    return TOKEN_STRUCTURES::binary_relational_operators.find(token.id) 
        != TOKEN_STRUCTURES::binary_relational_operators.end();
}

bool AST_NODE::operator_is_logical() {
    return operator_is_binary_logical() || operator_is_unary_logical();
}

bool AST_NODE::operator_is_binary_logical() {
    return TOKEN_STRUCTURES::binary_logical_operators.find(token.id) 
        != TOKEN_STRUCTURES::binary_logical_operators.end();
}

bool AST_NODE::operator_is_unary_logical() {
    return TOKEN_STRUCTURES::unary_logical_operators.find(token.id) 
        != TOKEN_STRUCTURES::unary_logical_operators.end();
}

bool AST_NODE::operator_is_binary() {
    return operator_is_binary_arithmetic() ||
        operator_is_binary_relational() ||
        operator_is_binary_logical();
}

bool AST_NODE::operator_is_unary() {
    return operator_is_unary_arithmetic() ||
        operator_is_unary_logical();
}

bool AST_NODE::is_terminal() {
    return TOKEN_STRUCTURES::terminals.find(token.id) 
        != TOKEN_STRUCTURES::terminals.end();
}

bool AST_NODE::is_type_integral() {
    return data_type == TYPE::INT4;
}

bool AST_NODE::is_type_logical() {
    return data_type == TYPE::BOOL;
}

bool AST_NODE::is_op() {
    return is_operator;
}

bool AST_NODE::is_nop() {
    return !is_op();
}

void AST_NODE::set_boolean() {
    if (!token.is_ident_true() && !token.is_ident_false()) return;

    boolean = TOKEN_STRUCTURES::booleans.at(token.identifier);
    is_boolean = true;
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
