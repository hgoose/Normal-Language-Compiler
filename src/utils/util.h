#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cstdint>

bool compare_two_files(const char* file_name1, const char* file_name2);
void read_file_to_file(const char* infile, const char* outfile);
int parse_hex6_codepoint(const std::string& hex6, uint32_t& value);
int encode_utf8(uint32_t cp, std::string& out);
std::string hex6_to_utf8(const std::string& hex6);
void goto_next_semicolon();
void goto_next_rbrace();
void skip_if();
void skip_while();

#endif
