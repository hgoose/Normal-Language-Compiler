#ifndef NCC_STRINGS_H
#define NCC_STRINGS_H

#include <cstddef>
#include <string>

#define VALID true
#define INVALID false

struct STR_TABLE_ENTRY {
    bool vi{};
    size_t offset{};

    STR_TABLE_ENTRY() = default;
    STR_TABLE_ENTRY(bool vi, size_t offset);
};

struct STR_TABLE {
    // 1 meg string table
    static constexpr size_t STR_TABLE_SIZE{1024ull * 1024ull};
    static inline char str_table[STR_TABLE_SIZE]{};
    static inline size_t used{0};

    static const char* emit_string(STR_TABLE_ENTRY&);
    static STR_TABLE_ENTRY add_string(std::string&);

private:
    static bool overflow();
};

#endif
