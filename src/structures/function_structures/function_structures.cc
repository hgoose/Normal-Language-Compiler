#include "function_structures.h"

Labels labels{};

bool label_exists(std::string label) {
    return labels.find(label) != labels.end();
}

bool label_create(std::string label, Byte byte) {
    if (label_exists(label)) return false;

    labels.insert(std::make_pair(label, byte));
    return true;
}
