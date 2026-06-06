// Nate warner z2004109
// CS-490D/515
// buffio.cc

#include "buffio.h"
#include "error.h"

#include <vector>
#include <fstream>
#include <string>

using std::vector;
using std::string;

int src_line_no = 1;
int src_col_no = 1;

static int curr_pos = -1;
static char* buffer = nullptr;
static int buff_size = 0;

static vector<size_t> line_start;
static vector<size_t> line_sizes;

// All int return values are error codes

// Initializes buffer
int buffer_init(const char* filename) {

    // Reset states
    buffer_cleanup();

    // Line numbers start at one, we don't care about index zero
    line_start.push_back(0);
    line_start.push_back(0);
    line_sizes.push_back(0);

    // open file for reading
    std::ifstream file(filename, std::ios_base::binary);

    // Check if any error flags were set equivalent to checking (file.fail() or file.bad())
    if (!file) return NCC_FILE_NOT_FOUND;

    // Get the size of the file in bytes, which will be the size of the buffer

    // Place pointer at end of file
    file.seekg(0, std::ios::end);

    // Retrieve the position
    std::streampos end_pos = file.tellg();

    // If either the stream buffer associated to the stream does not support the operation, or if it fails, the tellg returns -1, return error
    if (end_pos == std::streampos(-1)) return NCC_IO_ERROR;

    // tellg returned last position of stream, aka the size of the stream
    buff_size = static_cast<size_t>(end_pos);

    // Empty file, empty buffer
    if (buff_size == 0) {
        // No error here
        return NCC_OK;
    }

    // Place pointer back at start
    file.seekg(0, std::ios::beg);

    // Size the buffer dynamically
    buffer = new char[buff_size];

    // Read buff_size bytes of file into the buffer of size buff_size
    file.read(buffer, static_cast<std::streamsize>(buff_size));

    // Check if any error flags were set, (file.bad() or file.fail())
    if (!file) {
        delete[] buffer;
        return NCC_FILE_NOT_FOUND;
    }

    // Read through the buffer, if \n is encountered, record index of start of next line 
    size_t curr_line_size = 0;
    for (size_t i = 0; i < buff_size; ++i) {
        if (buffer[i] == '\n') {
            line_sizes.push_back(curr_line_size);
            curr_line_size = 0;

            // We want to make sure the start of the next line actually exists
            if ((i + 1) < buff_size) {
                // New line, record position and size of previous line
                line_start.push_back(i+1); 

                // New line, reset current line size
            }
        } else {
            // Still on same line, increment line size
            ++curr_line_size;
        }

    }
    // Size of final line
    if (buffer[buff_size-1] != '\n') {
        line_sizes.push_back(curr_line_size);
    }

    // No error
    return NCC_OK;
}

// Return the character at the current input position.  But don't
// advance in the input
int buffer_get_cur_char(char& c) {
    // Check if we are outside of the buffer
    if (curr_pos >= buff_size) return NCC_EOF;

    // Grab the character at current buffer position
    c = buffer[curr_pos];

    // No error
    return NCC_OK;
}

// Advance to next position in the input
int buffer_next_char(void) {
    // Check if we can advance (not at end of buffer)
    if (curr_pos >= buff_size) return NCC_EOF;

    // Otherwise, advance
    char current_char{};
    if (curr_pos >= 0) {
        current_char = buffer[curr_pos];
    }
    ++curr_pos;

    if (current_char == '\n') {
        ++src_line_no; 
        src_col_no = 1;
    } else {
        if (curr_pos != 0) ++src_col_no;
    }

    // No error
    return NCC_OK;
}

// Advance to the next position and get the character there.
int buffer_get_next_char(char& c) {
    // See if advancing would fall off the buffer, 
    if (buffer_next_char() != NCC_OK) return NCC_EOF;
    if (buffer_eof()) return NCC_EOF;

    // Otherwise, position is advanced and we get the character
    c = buffer[curr_pos];

    // No error
    return NCC_OK;
}

bool buffer_eof(void) {
    // Buffer runs from index zero to buff_size - 1
    return curr_pos >= buff_size;
}

// Go back one position in the input
int buffer_back_char(void) {
    // Can't back up from the start of the buffer
    if (curr_pos == 0) return NCC_BOF;

    // Past start of buffer, we can move backwards
    --curr_pos;

    // If we back up to a \n character, decrement current line number, and
    // set current column number to the last column position of previous line
    if (buffer[curr_pos] == '\n') {
        // Move back the line number to the previous line
        --src_line_no;
        src_col_no = static_cast<int>(line_sizes[src_line_no] + 1);
    } else {
        // Move back the column number to the previous column
        --src_col_no;
    }

    // No error
    return NCC_OK;
}


// Looks ahead one position in the buffer, and returns the character
int buffer_peek_next(char& c) {
    if (curr_pos + 1 >= buff_size) return NCC_EOF;

    c = buffer[curr_pos + 1];

    return NCC_OK;
}

int buffer_consume_k(size_t k, string& next_k) {
    // Make sure this is cleared first
    next_k.clear();

    // Make sure we aren't going to hit EOF before we peek k characters
    if ((curr_pos + k) >= buff_size) return NCC_EOF;

    // Construct a string with the next k characters.
    // This loop runs k times
    for (size_t i = 0; i<k; ++i) {
        ++curr_pos;
        next_k+=buffer[curr_pos];
    }

    // Ending position of curr_pos is curr_pos + k

    return NCC_OK;
}

int buffer_cleanup(void) {
    // Free allocated memory for char buffer
    delete[] buffer;

    // Set to nullptr
    buffer = nullptr;

    // Reset  size
    buff_size = 0;

    // Reset line_no and col_no
    src_line_no = 1;
    src_col_no = 1;

    // Current position in buffer is zero
    curr_pos = -1;

    // Clear line starts
    line_start.clear();
    line_sizes.clear();

    // No error
    return NCC_OK;
}

// Return a specified line of the source code.  Intended for error
// messages
int get_src_line(size_t line_no, string& line) {
    line.clear();

    // If passed in line_no doesn't exist for some reason
    if (line_no <= 0 || line_no >= line_start.size()) return NCC_NOT_FOUND;

    // Get start and line size
    size_t start = line_start[line_no];
    size_t line_size = line_sizes[line_no];

    // Create the line
    for (size_t i = start; i<start+line_size; ++i) {
        line+=buffer[i];
    }

    return NCC_OK;
}
