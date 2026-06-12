#ifndef NLC_ERROR_STRUCTURES_H
#define NLC_ERROR_STRUCTURES_H

#include "types.h"

extern const ErrorMap error_map;

bool error_unknown(ErrorValue error_value);
std::string error_string(ErrorValue);

#endif
