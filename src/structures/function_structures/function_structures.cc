#include "function_structures.h"

Labels labels{};

size_t label_get_offset(Label label) {
    if (labels.find(label) == labels.end()) return 0;
    return labels.at(label);
}

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
