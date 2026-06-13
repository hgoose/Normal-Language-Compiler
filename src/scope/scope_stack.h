#ifndef NLC_SCOPE_STACK_H
#define NLC_SCOPE_STACK_H

#include "types.h"

struct Scope {
private:
    static inline ScopeStack scope_stack{}; 
    static inline ScopeLevel current_scope_level{};

    static ScopeLevelPair make_level();
    static void push_level(const ScopeLevelPair&);
    static void pop_level();
public:
    static SymbolBucket get_top_bucket();
    static ScopeLevel level();
    static void down_level();
    static void up_level();
    static void enter_level();
    static void tear_down_frame(SymbolBucket&);
    static void add_to_top_level(SYMINFO*);
};

#endif
