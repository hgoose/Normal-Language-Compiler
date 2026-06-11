#ifndef NLC_BUFFIO_H
#define NLC_BUFFIO_H

#include "types.h"

#include <string>

struct BufState;

extern int src_line_no;
extern int src_col_no;

ErrorValue buffer_init(const char*);

BufState buffer_save_state();
void buffer_load_state(const BufState&);

ErrorValue buffer_get_cur_char(char&);

ErrorValue buffer_next_char();

ErrorValue buffer_get_next_char(char&);

bool buffer_eof();

ErrorValue buffer_back_char();

ErrorValue buffer_cleanup();

ErrorValue get_src_line(size_t, std::string&);

ErrorValue buffer_peek_next(char&);
ErrorValue buffer_consume_k(size_t, std::string&);
void buffer_unconsume_k(size_t);

#endif 
