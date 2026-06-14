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

ScopeLevelPair Scope::make_empty_level() {
    return std::make_pair(level()+1, SymbolBucket{});
}

void Scope::push_level(const ScopeLevelPair& pair) {
    scope_stack.push_back(pair);
}

void Scope::enter_level() {
    ScopeLevelPair new_level = make_level();

    up_level();
    push_level(new_level);
}

void Scope::enter_function() {
    ScopeLevelPair new_level = make_empty_level();

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

void Scope::tear_down_frame(SymbolBucket& bucket, ScopeLevel scope_level) {
    for (auto& symbol : bucket) {
        if (symbol->scope_level != scope_level) continue;
        SYMTABLE::remove_symbol(symbol);
    }

    pop_level();
}
