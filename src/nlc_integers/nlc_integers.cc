#include "table_structures.h"
#include "nlc_integers.h"
#include "error.h"

INT_TABLE_ENTRY::INT_TABLE_ENTRY(bool vi, size_t offset) 
    : vi(vi),
    offset(offset) 
{}

int* INT_TABLE::emit_int(size_t offset) {
    return emit_int(INT_TABLE_ENTRY{true, offset});
}

int* INT_TABLE::emit_int(const INT_TABLE_ENTRY& entry) {
    return (entry.vi 
            ? &int_table[entry.offset] 
            : nullptr
   );
}

INT_TABLE_ENTRY INT_TABLE::add_int(int x) {
    size_t off = used;
    if (overflow()) {
        set_print_token_error(Error{}, NLC_INT_TABLE_OVERFLOW);
        return INT_TABLE_ENTRY{};
    }
    int_table[used++] = x;

    return INT_TABLE_ENTRY(VALID, off);
}

bool INT_TABLE::overflow() {
    return used == INT_TABLE_SIZE;
}
