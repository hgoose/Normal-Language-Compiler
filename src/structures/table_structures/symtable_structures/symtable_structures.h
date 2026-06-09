#ifndef NLC_SYMTABLE_STRUCTURES_H
#define NLC_SYMTABLE_STRUCTURES_H

enum class SYMTYPE : unsigned int {
    null, VAR
};

enum class LOCATION_TYPE : unsigned int {
    MEMORY, REG, STACK
};

#endif
