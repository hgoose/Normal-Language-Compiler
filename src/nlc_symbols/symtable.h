#ifndef NLC_SYMTABLE_H
#define NLC_SYMTABLE_H

#include "types.h"
#include "symtable_structures.h"
#include "ast_structures.h" /* TYPE */

#include <string>
#include <cstddef>

struct SYMLOCATION {
    LOCATION_TYPE location_type{};
    size_t int_table_offset{};
    size_t address{};
    size_t stack_offset{};
    std::string reg_label{};

    SYMLOCATION() = default;
    SYMLOCATION(LOCATION_TYPE, size_t, size_t, const std::string&);
};

struct SYMINFO {
    std::string name{};
    SYMTYPE type{};
    SYMLOCATION location{};

    TYPE data_type{};

    bool exists{};

    int scope_level{};

    SYMINFO() = default;
    SYMINFO(const std::string& name, TYPE data_type, SYMTYPE type);
    SYMINFO(const std::string& name, SYMTYPE type);
    SYMINFO(const std::string& name, SYMTYPE type, const SYMLOCATION& location);
    SYMINFO(const std::string& name, SYMTYPE type, ScopeLevel scope_level);
    SYMINFO(const std::string& name, SYMTYPE type, const SYMLOCATION& location, ScopeLevel scope_level);

    void set_scope_level(int);
    bool is_same_as(const SYMINFO* other);
    bool is_same_as_no_scope(const SYMINFO* other);
};

struct SYMTABLE {
    static inline constexpr std::size_t SYM_TABLE_SIZE{1024};

    static SYMINFO* get_symbol(const std::string&, const SYMTYPE&, ScopeLevel);
    static SYMINFO* get_symbol(const std::string&, const SYMTYPE&);
    static SYMINFO* add_symbol(SYMINFO*);
    static void remove_symbol(SYMINFO*);
    static void free_symbol_table();

private:
    static inline constexpr std::size_t PRIME{67};
    static inline SymbolBucket symbol_table[SYM_TABLE_SIZE]{};
    static unsigned long long sym_value(const char);
    static size_t hash(const std::string&);
};

#endif
