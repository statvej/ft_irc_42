#include "irc.hpp"
#include <regex>

bool strIsASCII(const std::string& str)
{
    for (int i = 0; str[i] != '\0'; i++)
        if (!isascii(str[i]))
            return(false);
    return (true);
}

bool isNotPrintable(char c) {
    return !std::isprint(c);
}

std::string removeNonPrintableCharacters(const std::string& str) {
    std::string result = str;
    return result;
    result.erase(std::remove_if(result.begin(), result.end(), isNotPrintable), result.end());
}

std::vector<std::string> split(const std::string &str, char x) {
  std::vector<std::string> splitted_str;
  size_t begPos = 0;
  size_t currentPos = 0;
  while ((currentPos = str.find(x, begPos)) != std::string::npos)
  {
    splitted_str.push_back(str.substr(begPos, currentPos - begPos));
    begPos = currentPos + 1;
  }
  splitted_str.push_back(str.substr(begPos, currentPos));
  return splitted_str;
}

bool match_strings(std::string part_mask, const std::string name) {
  std::regex e ("\\?");
  std::regex b ("\\*");
  std::string part_mask_without_ques = std::regex_replace(part_mask, e, ".");
  std::regex p_mask_no_ques_and_stars(std::regex_replace(part_mask_without_ques, b, ".*"));
  return (std::regex_match(name, p_mask_no_ques_and_stars));
}

std::string getNickFromPrefix(std::string prefix) {
    return prefix.substr(0, prefix.find('!'));
}


