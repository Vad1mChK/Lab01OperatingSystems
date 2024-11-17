//
// Created by vad1mchk on 17/11/24.
//

#ifndef STRING_FUNCTIONS_HPP
#define STRING_FUNCTIONS_HPP
#include <string>
#include <vector>

std::vector<std::string> SplitString(const std::string& source, char delimiter);

std::string JoinString(const std::vector<std::string>& parts, char delimiter);

std::string ShortenEachPart(
  const std::string source,
  char delimiter,
  size_t max_length,
  bool modify_last
);
#endif //STRING_FUNCTIONS_HPP
