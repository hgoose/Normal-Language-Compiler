#include "scope_stack.h"
#include "utility"
#include "types.h"
#include "symtable.h"
#include "ast_node.h"

SymbolMemory::SymbolMemory(SYMINFO* symbol, NodeList symbol_holders) 
    : symbol(symbol),
    symbol_holders(symbol_holders) 
{}

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

AdvancedSymbolBucket Scope::get_top_bucket() {
    return (scope_stack.size()
        ? scope_stack.back().second : AdvancedSymbolBucket{}
    );
}

ScopeLevelPair Scope::make_level() {
    return std::make_pair(level()+1, get_top_bucket());
}

ScopeLevelPair Scope::make_empty_level() {
    return std::make_pair(level()+1, AdvancedSymbolBucket{});
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

void Scope::add_to_top_level(SYMINFO* syminfo, AST_NODE* holder) {
    if (scope_stack.empty()) return;

    scope_stack.back().second.push_back(
        new SymbolMemory{syminfo, NodeList{holder}}
    );
}

void Scope::pop_level() {
    if (scope_stack.empty()) return;

    scope_stack.pop_back();
}

void Scope::tear_down_frame(AdvancedSymbolBucket& bucket) {
    for (auto& symbol : bucket) {
        SYMTABLE::remove_symbol(symbol->symbol);
        for (auto& holder : symbol->symbol_holders) {
            holder->syminfo = nullptr;
        }
    }

    pop_level();
}
