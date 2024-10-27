//
// Created by vadim on 12.10.2024.
//

#ifndef PATH_FUNCTIONS_HPP
#define PATH_FUNCTIONS_HPP
#include <string>

std::pair<std::string, int> pwd();
std::pair<std::string, int> cd(const std::string& path);
std::pair<std::vector<std::string>, int> ls();
std::pair<std::vector<std::string>, int> ls(const std::string& path);

#endif  // PATH_FUNCTIONS_HPP
