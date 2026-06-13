#include "scope_stack.h"
#include "utility"
#include "types.h"
#include "symtable.h"

void Scope::down_level() {
    if (current_scope_level == 0) return;

    --current_scope_level;
}

void Scope::up_level() {
    ++current_scope_level;
}

ScopeLevel Scope::level() {
    return current_scope_level;
}

SymbolBucket Scope::get_top_bucket() {
    return (scope_stack.size()
        ? scope_stack.back().second : SymbolBucket{}
    );
}

ScopeLevelPair Scope::make_level() {
    return std::make_pair(level()+1, get_top_bucket());
}

void Scope::push_level(const ScopeLevelPair& pair) {
    scope_stack.push_back(pair);
}

void Scope::enter_level() {
    SymbolBucket current_bucket = get_top_bucket();
    ScopeLevelPair new_level = make_level();

    up_level();
    push_level(new_level);
}

void Scope::add_to_top_level(SYMINFO* syminfo) {
    if (scope_stack.empty()) return;

    scope_stack.back().second.push_back(syminfo);
}

void Scope::pop_level() {
    if (scope_stack.empty()) return;

    scope_stack.pop_back();
}

void Scope::exit_level() {
    // Get symbols on level
    SymbolBucket current_bucket = get_top_bucket();

    for (auto& member : current_bucket) {
        SYMTABLE::remove_symbol(member);
    }

    pop_level();
    down_level();
}
