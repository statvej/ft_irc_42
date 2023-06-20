#ifndef UTILS_HPP
# define UTILS_HPP


bool strIsASCII(const std::string& str);

bool isNotPrintable(char c);

std::string removeNonPrintableCharacters(const std::string& str);

std::vector<std::string> split(const std::string &str, char x);

bool match_strings(std::string part_mask, const std::string name);

#endif

