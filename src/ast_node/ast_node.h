#ifndef NLC_ASTNODE_H
#define NLC_ASTNODE_H

#include "token.h"
#include "nlc_strings.h"
#include "symtable.h"
#include "ast_structures.h"
#include "token_structures.h"
#include "types.h"

#include <list>
#include <string>
#include <concepts>

struct AST_NODE;

inline constexpr bool OPERATOR{true};
inline constexpr bool COPY_CHILDREN{true};
inline constexpr bool NO_COPY_CHILDREN{false};

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

    // Data type will be found later for operators.
    // For var, int, bool, or string nodes set this
    TYPE data_type{};

    bool boolean{};
    bool is_boolean{};
    
    bool is_function_arg{};

    ScopeLevel statement_scope_level{};

    // If consumed token is a string constant
    STR_TABLE_ENTRY entry{};

    // If consumed token is a variable
    SYMINFO* syminfo{};

    Children children;

    SymbolBucket scope_stack_frame{};

    AST_NODE() = default;
    AST_NODE(ScopeLevel scope_level);
    AST_NODE(Token token, ScopeLevel scope_level);
    AST_NODE(NODE_TYPE node_type, ScopeLevel scope_level);
    AST_NODE(Token token, NODE_TYPE node_type, ScopeLevel scope_level);
    AST_NODE(Token token, NODE_TYPE node_type, bool is_operator, ScopeLevel scope_level);
    AST_NODE(Token token, NODE_TYPE node_type, TYPE data_type, ScopeLevel scope_level);
    AST_NODE(Token token, NODE_TYPE node_type, SYMTYPE symbol_type, ScopeLevel scope_level);
    AST_NODE(Token token, NODE_TYPE node_type, TYPE data_type, SYMTYPE symbol_type, ScopeLevel scope_level);
    AST_NODE(const AST_NODE& other, bool copy_children=false);
    AST_NODE& operator=(const AST_NODE& other);

    void add_child(AST_NODE* child) {
        children.push_back(child);
    }

    template<typename... ARGS>
    requires (std::same_as<ARGS, AST_NODE*> && ...)
    void add_children(ARGS... args) {
        ((children.push_back(args)), ...);
    }

    void add_all_statements(StatementReturns);

    void clear();
    std::string str_node_type() const;
    bool is_statement() const;
    bool operator_is_arithmetic() const;
    bool operator_is_binary_arithmetic() const;
    bool operator_is_unary_arithmetic() const;
    bool operator_is_relational() const;
    bool operator_is_binary_relational() const;
    bool operator_is_logical() const;
    bool operator_is_binary_logical() const;
    bool operator_is_unary_logical() const;
    bool operator_is_binary() const;
    bool operator_is_unary() const;

    bool is_terminal() const;
    bool is_var() const;
    bool is_fn_name() const;
    bool is_bool() const;
    bool is_parameter_pack() const;
    bool is_argument_pack() const;
    bool is_return_value() const;
    bool is_fn_call() const;
    bool is_return_statement() const;
    bool is_for_init() const;
    bool is_for_cond() const;
    bool is_for_update() const;

    bool is_type_integral() const; 
    bool is_type_logical() const;

    bool is_op() const;
    bool is_nop() const;

    void set_boolean();
    bool is_fn_arg() const;
    void set_is_fn_arg();
    void set_token(Token);

    void install_symbol(SYMINFO*);

    void deep_copy_children(const AST_NODE&);

    void set_scope_stack_frame(const SymbolBucket&);
    SymbolBucket& get_scope_stack_frame();

    std::string get_type() const;

    bool symbol_freed() const;

    bool is_type_assignable() const;
    size_t type_size() const;
};

#endif
