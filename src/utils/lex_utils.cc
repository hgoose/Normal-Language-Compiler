#include "lex_utils.h"
#include "lex.h"
#include "buffio.h"
#include "error.h"
#include "token.h"
#include "types.h"

#include <string>

Error get_end_of_block_comment(Token& t, char& curr_char) {
    Error buf_error = create_error(buffer_get_next_char(curr_char));
    while (curr_char != '-') {
        if (buf_error.is_eof()) {
            return Error{NLC_UNEXPECTED_EOF, src_line_no, src_col_no};
        }

        buf_error = create_error(buffer_get_next_char(curr_char));
    }

    std::string next_two{};
    buf_error = create_error(buffer_consume_k(2, next_two));

    if (buf_error.is_eof()) {
        return Error{NLC_UNEXPECTED_EOF, src_line_no, src_col_no};
    }

    if (next_two != ">>") {
        buffer_unconsume_k(2);
        return Error{NLC_NO_MATCH};
    } 

    return Error{};
}

Error token_either_this_or_that(Token& t, TokenValue if_char_is, TokenValue if_char_is_not, char& curr_char, char expected_char) {
    Error err = create_error(buffer_get_next_char(curr_char));

    if (curr_char == expected_char) {
        t.set_id(if_char_is);
        return Error{};
    }

    buffer_back_char();
    t.set_id(if_char_is_not);

    return Error{};
}
