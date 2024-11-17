//
// Created by vad1mchk on 17/11/24.
//
#include "string_functions.hpp"
#include <sstream>
#include <iostream>

std::vector<std::string> SplitString(const std::string& source, const char delimiter) {
  std::vector<std::string> parts;
  std::stringstream strstr(source);
  std::string part;

  while (std::getline(strstr, part, delimiter)) {
    parts.push_back(part);
  }
  return parts;
}

std::string JoinString(const std::vector<std::string>& parts, const char delimiter) {
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i > 0) {
      result += delimiter;
    }
    result += parts[i];
  }
  return result;
}

std::string ShortenEachPart(
  const std::string source,
  char delimiter,
  size_t max_length,
  bool modify_last
) {
  if (source.empty()) {
    return source;
  }

  std::vector<std::string> parts = SplitString(source, delimiter);
  for (size_t i = 0; i < parts.size(); ++i) {
    // If it's the last part and modify_last is false, skip shortening
    if (i == parts.size() - 1 && !modify_last) continue;
    // Truncate the part if it exceeds max_length
    if (parts[i].length() > max_length) {
      parts[i] = parts[i].substr(0, max_length);
    }
  }

  if (!parts.empty() && parts[parts.size() - 1].empty()) {
    parts.pop_back();
  }

  std::string result;
  result += JoinString(parts, delimiter);
  if (source[source.length() - 1] == delimiter) {
    result += delimiter;
  }
  return result;
}