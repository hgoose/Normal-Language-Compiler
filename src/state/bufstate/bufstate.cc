#include "bufstate.h"

BufState::BufState(
    int src_line_no, 
    int src_col_no,
    int curr_pos,
    int buff_size,
    std::vector<size_t> line_start,
    std::vector<size_t> line_sizes
) : src_line_no(src_line_no), 
    src_col_no(src_col_no),
    curr_pos(curr_pos),
    buff_size(buff_size),
    line_start(line_start),
    line_sizes(line_sizes)
{}
