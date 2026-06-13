#ifndef NLC_SCOPE_STACK_H
#define NLC_SCOPE_STACK_H

#include "types.h"

struct Scope {
    static inline ScopeStack scope_stack{}; 
    static inline int current_scope_level{};

    static void pop_current_level();
    static void down_level();
    static void up_level();
};

#endif
