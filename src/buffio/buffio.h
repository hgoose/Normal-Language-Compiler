#ifndef NLC_BUFFIO_H
#define NLC_BUFFIO_H

#include "types.h"

#include <string>

struct BufState;

extern int src_line_no;
extern int src_col_no;

// All int return values are error codes

ErrorValue buffer_init(const char * filename);

BufState buffer_save_state();
void buffer_load_state(const BufState&);

ErrorValue buffer_get_cur_char(char & c);
// Return the character at the current input position.  But don't
// advance in the input

ErrorValue buffer_next_char();
// Advance to next position in the input

ErrorValue buffer_get_next_char(char & c);
// Advance to the next position and get the character there.

bool buffer_eof();

ErrorValue buffer_back_char();
// Go back one position in the input

ErrorValue buffer_cleanup();

ErrorValue get_src_line(size_t line_no, std::string& line);
// Return a specified line of the source code.  Intended for error
// messages

// Looks ahead one position in the buffer, and returns the character
ErrorValue buffer_peek_next(char& c);
ErrorValue buffer_consume_k(size_t k, std::string& next_k);

#endif /* BUFFERED_INPUT_H */
