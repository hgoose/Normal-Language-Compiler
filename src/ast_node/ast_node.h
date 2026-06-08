// Nate warner 
// CS 515
// Assignment 4
#ifndef NCC_ASTNODE_H
#define NCC_ASTNODE_H

#include <list>
#include <string>
#include <unordered_set>

#include "token.h"
#include "ncc_strings.h"
#include "symtable.h"

#define OPERATOR true

struct AST_NODE;

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

extern std::unordered_set<std::string> reserved_words;

bool is_reserved(const Token& t);
NODE_TYPE get_node_type(const Token& t);

// Returns the offender for error info (or nullptr if the types comply)
AST_NODE* binary_arithmetic_type_compliance(AST_NODE* left, AST_NODE* right);
AST_NODE* unary_arithmetic_type_compliance(AST_NODE* child);
AST_NODE* binary_relational_type_compliance(AST_NODE* left, AST_NODE* right);
AST_NODE* binary_logical_type_compliance(AST_NODE* left, AST_NODE* right);
AST_NODE* unary_logical_type_compliance(AST_NODE* child);

struct AST_NODE {
    // Always set upon consumption
    Token token{};

    // Set upon consume if consumed token is an operator
    bool is_operator{};

    // Set upon consumption
    NODE_TYPE node_type{};

    // Set if going in symbol table
    SYMTYPE symbol_type{};

    // Data type will be found later for operators,
    // for var, int, bool, or string nodes set this
    TYPE data_type{};

    bool boolean{};
    bool is_boolean{};

    // If consumed token is a string constant
    STR_TABLE_ENTRY entry{};

    // If consumed token is a variable
    SYMINFO* syminfo{};

    std::list<AST_NODE*> children;

    AST_NODE() = default;

    AST_NODE(Token token) : token(token) {}

    AST_NODE(Token token, NODE_TYPE node_type) 
        : token(token),
          node_type(node_type)
    {}

    AST_NODE(Token token, NODE_TYPE node_type, bool is_operator=false) 
        : token(token),
          node_type(node_type),
          is_operator(is_operator)
    {}

    AST_NODE(Token token, NODE_TYPE node_type, TYPE data_type) 
        : token(token),
          node_type(node_type),
          data_type(data_type)
    {}

    AST_NODE(const AST_NODE& other) {
        token = other.token;
        data_type = other.data_type;
        node_type = other.node_type;
        entry = other.entry;
        syminfo = other.syminfo;
        is_operator = other.is_operator;
        symbol_type = other.symbol_type;
        boolean = other.boolean;
        is_boolean = other.is_boolean;
    }

    void add_child(AST_NODE* child) {
        children.push_back(child);
    }

    template<typename... ARGS>
    void add_children(ARGS... args) {
        ((children.push_back(args)), ...);
    }

    void clear();
    std::string str_node_type();
    bool is_statement();
    bool operator_is_arithmetic();
    bool operator_is_binary_arithmetic();
    bool operator_is_unary_arithmetic();
    bool operator_is_relational();
    bool operator_is_binary_relational();
    bool operator_is_logical();
    bool operator_is_binary_logical();
    bool operator_is_unary_logical();
    bool operator_is_binary();
    bool operator_is_unary();
    bool is_terminal();

    bool is_type_integral(); 
    bool is_type_logical();

    bool is_op();
    bool is_nop();
};

#endif
