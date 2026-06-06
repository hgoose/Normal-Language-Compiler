#ifndef NCC_INTEGERS_H
#define NCC_INTEGERS_H

#include <cstddef>

#define VALID true
#define INVALID false

struct INT_TABLE_ENTRY {
    bool vi{};
    size_t offset{};
    size_t addr{};

    INT_TABLE_ENTRY() = default;
    INT_TABLE_ENTRY(bool vi, size_t offset);

    inline size_t get_addr() {
        return offset * 4;
    }
};

struct INT_TABLE {
    // 1 meg integer table
    static constexpr size_t INT_TABLE_SIZE{1024*1024/4};
    static inline int int_table[INT_TABLE_SIZE]{};
    static inline size_t used{0};

    static int* emit_int(size_t);
    static int* emit_int(const INT_TABLE_ENTRY&);
    static INT_TABLE_ENTRY add_int(int);

private:
    static bool overflow();
};

#endif
