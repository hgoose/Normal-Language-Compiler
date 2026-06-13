#include "symtable.h"
#include "table_structures.h"

#include <string>
#include <cstddef>

SYMLOCATION::SYMLOCATION(LOCATION_TYPE location_type, size_t int_table_offset, size_t stack_offset, const std::string& reg_label) 
    : location_type(location_type),
    int_table_offset(int_table_offset),
    stack_offset(stack_offset),
    reg_label(reg_label) 
{}

SYMINFO::SYMINFO(const std::string& name, TYPE data_type, SYMTYPE type) 
    : name(name),
    data_type(data_type),
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

void SYMINFO::set_scope_level(int level) {
    scope_level = level;
}

SYMINFO* SYMTABLE::get_symbol(const std::string& name, const SYMTYPE& symbol_type) {
    // Get bucket number
    size_t hx = hash(name);

    // Search for symbol in bucket
    for (auto& member : symbol_table[hx]) {
        if (member.name == name && member.type == symbol_type) {
            return &member;
        }
    }

    // No symbol found
    return nullptr;
}

SYMINFO* SYMTABLE::add_symbol(const SYMINFO& syminfo) {
    size_t hx = hash(syminfo.name);
    // Check if it already exists
    for (auto& member : symbol_table[hx]) {
        if (member.name == syminfo.name && member.type == syminfo.type) {
            return nullptr;
        }
    }

    // Doesn't exist, add it
    return &symbol_table[hx].emplace_back(syminfo);

}

unsigned long long SYMTABLE::sym_value(const char c) {
    // Lower case a-z map into [0,26]
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 1;
    // A-Z map into [27, 52]
    } else if (c >= 'A' && c <= 'Z') {
        return (unsigned long long) c - 'A' + 27;
    // 0-9 map into [53,62]
    } else if (c >= '0' && c <= '9') {
        return (unsigned long long) c - '0' + 53;
    // So this is 63
    } else if (c == '_') {
        return 63ull;
    // Invalid character is annihilated under this map
    } else return 0;
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
