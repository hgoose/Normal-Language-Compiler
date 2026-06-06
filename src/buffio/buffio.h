#ifndef BUFFERED_INPUT_H
#define BUFFERED_INPUT_H

#include <string>
using std::string;

extern int src_line_no;
extern int src_col_no;

// All int return values are error codes

int buffer_init(const char * filename);

int buffer_get_cur_char(char & c);
// Return the character at the current input position.  But don't
// advance in the input

int buffer_next_char(void);
// Advance to next position in the input

int buffer_get_next_char(char & c);
// Advance to the next position and get the character there.

bool buffer_eof(void);

int buffer_back_char(void);
// Go back one position in the input

int buffer_cleanup(void);

int get_src_line(size_t line_no, string& line);
// Return a specified line of the source code.  Intended for error
// messages

// Looks ahead one position in the buffer, and returns the character
int buffer_peek_next(char& c);
int buffer_consume_k(size_t k, string& next_k);

#endif /* BUFFERED_INPUT_H */
