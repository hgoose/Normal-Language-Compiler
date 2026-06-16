#include "symtable.h"
#include "table_structures.h"
#include "scope_stack.h"

#include "types.h"

#include <string>
#include <cstddef>

SYMLOCATION::SYMLOCATION(LOCATION_TYPE location_type, Offset int_table_offset, std::int32_t stack_offset, const std::string& reg_label) 
    : location_type(location_type),
    int_table_offset(int_table_offset),
    stack_offset(stack_offset),
    reg_label(reg_label) 
{}

FunctionInfo::FunctionInfo(Label label, AST_NODE* parameter_pack, AST_NODE* return_value, AST_NODE* block)
    : label(label),
    parameter_pack(parameter_pack),
    return_value(return_value),
    block(block)
{}

SYMINFO::SYMINFO(const std::string& name, TYPE data_type, SYMTYPE type) 
    : name(name),
    data_type(data_type),
    type(type)
{
    exists = true;
}

SYMINFO::SYMINFO(const std::string& name, SYMTYPE type) 
    : name(name),
    type(type)
{
    exists = true;
}

SYMINFO::SYMINFO(const std::string& name, SYMTYPE type, const SYMLOCATION& location)
    : name(name),
    type(type),
    location(location)
{
    exists = true;
}

SYMINFO::SYMINFO(const std::string& name, TYPE data_type, SYMTYPE type, ScopeLevel scope_level) 
    : name(name),
    type(type),
    data_type(data_type),
    scope_level(scope_level)
{}

SYMINFO::SYMINFO(const std::string& name, SYMTYPE type, ScopeLevel scope_level)
    : name(name),
    type(type),
    scope_level(scope_level)
{
    exists = true;
}

SYMINFO::SYMINFO(const std::string& name, SYMTYPE type, const SYMLOCATION& location, ScopeLevel scope_level)
    : name(name),
    type(type),
    location(location),
    scope_level(scope_level)
{
    exists = true;
}

void SYMINFO::set_scope_level(int level) {
    scope_level = level;
}

bool SYMINFO::is_same_as(const SYMINFO* other) const {
    return name == other->name && type == other->type && scope_level == other->scope_level;
}

bool SYMINFO::is_same_as_no_scope(const SYMINFO* other) const {
    return name == other->name && type == other->type; 
}

bool SYMINFO::is_same_as_scope_at_most(const SYMINFO* other) const {
    return name == other->name && type == other->type && scope_level <= other->scope_level; 
}

void SYMINFO::install_function(Label label, AST_NODE* parameter_pack,  AST_NODE* return_value, AST_NODE* block) {
    function_info = FunctionInfo(label, parameter_pack, return_value, block);
}

bool SYMINFO::in_int_table() const {
    return location.location_type == LOCATION_TYPE::MEMORY;
}

bool SYMINFO::in_stack() const {
    return location.location_type == LOCATION_TYPE::STACK;
}

bool SYMINFO::in_reg() const {
    return location.location_type == LOCATION_TYPE::REG;
}

bool SYMINFO::is_function() const {
    return type == SYMTYPE::FN;
}

bool SYMINFO::is_in_function() const {
    return in_function;
}

void SYMINFO::set_in_function() {
    in_function = true;
}

void SYMINFO::set_location_static_memory(Offset int_table_offset, std::size_t address) {
    location.int_table_offset = int_table_offset;
    location.address = address;
    location.location_type = LOCATION_TYPE::MEMORY;
}

void SYMINFO::set_location_type_stack() {
    location.location_type = LOCATION_TYPE::STACK;
}

void SYMINFO::set_fn_return_type(TYPE return_type) {
    function_info.return_type = return_type;
}

TYPE SYMINFO::get_fn_return_type() {
    return function_info.return_type;
}

SYMINFO* SYMTABLE::get_symbol(const std::string& name, const SYMTYPE& symbol_type, ScopeLevel level) {
    // Get bucket number
    size_t hx = hash(name);
    SYMINFO syminfo = SYMINFO(name, symbol_type, level);

    // Search for symbol in bucket
    for (auto& member : symbol_table[hx]) {
        if (member->is_same_as_scope_at_most(&syminfo)) {
            return member;
        }
    }

    // No symbol found
    return nullptr;
}

SYMINFO* SYMTABLE::get_symbol(const SYMINFO* other) {
    if (!other) return nullptr;
    return get_symbol(other->name, other->type, other->scope_level);
}

SYMINFO* SYMTABLE::get_symbol(const std::string& name, const SYMTYPE& symbol_type) {
    // Get bucket number
    size_t hx = hash(name);
    SYMINFO syminfo = SYMINFO(name, symbol_type);

    // Search for symbol in bucket
    for (auto& member : symbol_table[hx]) {
        if (member->is_same_as_no_scope(&syminfo)) {
            return member;
        }
    }

    // No symbol found
    return nullptr;
}

SYMINFO* SYMTABLE::add_symbol(SYMINFO* syminfo) {
    size_t hx = hash(syminfo->name);

    // Check if it already exists
    for (auto& member : symbol_table[hx]) {
        if (member->is_same_as(syminfo)) {
            return nullptr;
        }
    }

    // Doesn't exist, add it
    Scope::add_to_top_level(syminfo);
    return symbol_table[hx].emplace_front(syminfo);
}

void SYMTABLE::remove_symbol(SYMINFO* syminfo) {
    size_t hx = hash(syminfo->name); 

    SymbolBucket& bucket = symbol_table[hx];
    for (auto start = bucket.begin(); start != bucket.end();) {
        if ((*start)->is_same_as(syminfo)) {
            delete *start;
            start = bucket.erase(start);
        } else ++start;
    }
}

unsigned long long SYMTABLE::sym_value(const char c) {
    // Lower case a-z map into [0,26]
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 1;
    } 

    // [A-Z] map into [27, 52]
    else if (c >= 'A' && c <= 'Z') {
        return (unsigned long long) c - 'A' + 27;
    } 

    // [0-9] map into [53,62]
    else if (c >= '0' && c <= '9') {
        return (unsigned long long) c - '0' + 53;
    } 

    // So this is 63
    else if (c == '_') {
        return 63ull;
    } 

    // Invalid character is annihilated under this map
    else return 0;
}

// Polynomial rolling hash function
// \sum_{i=0}^{k-1} sym_value(name[i]) * prime^i, 
// where k is the size of the name
size_t SYMTABLE::hash(const std::string& name) {
    unsigned long long hash = 0;
    unsigned long long power = 1;

    // Horner's rule
    for (char symbol : name) {
        unsigned long long symbol_value = sym_value(symbol);
        hash = hash * PRIME + symbol_value;
    }

    // Equivalent to hash % SYM_TABLE_SIZE since the 
    // table size is a power of two
    return hash & (SYM_TABLE_SIZE - 1);
}

void SYMTABLE::free_symbol_table() {
    for (std::size_t i{}; i < SYMTABLE::SYM_TABLE_SIZE; ++i) {
        for (auto& symbol : symbol_table[i]) {
            delete symbol;
        }
    }
}
