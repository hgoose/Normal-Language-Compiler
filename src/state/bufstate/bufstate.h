#ifndef NLC_BUFSTATE_H
#define NLC_BUFSTATE_H

#include <vector>

struct BufState {
    int src_line_no{1};
    int src_col_no{1};

    int curr_pos{-1};
    int buff_size{};

    std::vector<size_t> line_start;
    std::vector<size_t> line_sizes;

    BufState() = default;
    BufState(int, int, int, int, std::vector<size_t>, std::vector<size_t>);
};

#endif
