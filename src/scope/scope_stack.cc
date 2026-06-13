#include "scope_stack.h"

void Scope::pop_current_level() {

}

void Scope::down_level() {
    if (current_scope_level == 0) return;

    --current_scope_level;
}

void Scope::up_level() {
    ++current_scope_level;
}
