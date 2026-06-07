// Nate Warner z2004109

// main.cc: Take a file as a command line argument and pass it to the parser

#include "drivers.h"
#include "error.h"
#include "lex.h"
#include <iostream> 
#include <cstdlib> 

#include "token.h"

using std::cout;

int main(int argc, const char* argv[]) {

    // Check that a filename was provided
    // if (argc < 2) {
    //     std::cerr << "Usage: " << argv[0] << " <filename>\n";
    //     return 1;
    // }

    // The filename is the first argument after program name
    // const char* filename = argv[1];
    const char* filename = "/home/datura/nlc/tests/pet_testdata/test";

    Error err = lex_init(filename);
    print_error(err);

    // Examine the token stream
    // Token t; while (!lex_eof()) { get_token(t); cout << (t.id != -1 ? TOKEN_STRUCTURES::token_names[t.id] : "stop") << ", " << t.lexeme << "\n"; }

    // Run the parser test driver
    parser_test(filename);

    return EXIT_SUCCESS;
}
