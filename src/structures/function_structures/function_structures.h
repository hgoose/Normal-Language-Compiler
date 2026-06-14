#ifndef NLC_FUNCTION_STRUCTURES_H
#define NLC_FUNCTION_STRUCTURES_H

#include "types.h"

extern Labels labels;

bool label_exists(std::string label);
bool label_create(std::string label, Byte byte);

#endif
