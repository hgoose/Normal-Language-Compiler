#include "table_structures.h"
#include "nlc_strings.h"
#include "error.h"

STR_TABLE_ENTRY::STR_TABLE_ENTRY(bool vi, size_t offset) 
    : vi(vi),
    offset(offset) 
{}

const char* STR_TABLE::emit_string(STR_TABLE_ENTRY& entry) {
    return (entry.vi 
            ? &str_table[entry.offset] 
            : nullptr
   );
}

STR_TABLE_ENTRY STR_TABLE::add_string(std::string& str) {
    size_t off = used;
    bool _overflow{};
    for (const auto& ch : str) {
        if (overflow()) { _overflow = true; break; } 
        str_table[used++] = ch; 
    } 

    if (overflow() || _overflow) {
        set_print_token_error(Error{}, NLC_STR_TABLE_OVERFLOW);

        return STR_TABLE_ENTRY{};
    }
    str_table[used++] = '\0';

    return STR_TABLE_ENTRY(VALID, off);
}

bool STR_TABLE::overflow() {
    return used == STR_TABLE_SIZE-1;
}
