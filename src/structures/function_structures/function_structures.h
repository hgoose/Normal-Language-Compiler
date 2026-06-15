#ifndef NLC_FUNCTION_STRUCTURES_H
#define NLC_FUNCTION_STRUCTURES_H

#include "types.h"

extern Labels labels;

size_t label_get_offset(Label);
bool label_exists(Label);
bool label_create(Label, size_t);
bool label_remove(Label);

#endif
