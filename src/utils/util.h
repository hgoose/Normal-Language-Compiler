#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cstdint>

struct Error;

bool compare_two_files(const char* file_name1, const char* file_name2);
void read_file_to_file(const char* infile, const char* outfile);
int parse_hex6_codepoint(const std::string& hex6, uint32_t& value);
int encode_utf8(uint32_t cp, std::string& out);
std::string hex6_to_utf8(const std::string& hex6);

#endif
