#include "function_structures.h"

Labels labels{};

bool label_exists(Label label) {
    return labels.find(label) != labels.end();
}

bool label_create(Label label, size_t offset) {
    if (label_exists(label)) return false;

    labels.insert(std::make_pair(label, offset));
    return true;
}

bool label_remove(Label label) {
    auto pos = labels.find(label);

    if (pos != labels.end()) {
        labels.erase(pos);
        return true;
    }

    return false;
}
