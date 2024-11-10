//
// Created by vadim on 12.10.2024.
//

#ifndef PATH_FUNCTIONS_HPP
#define PATH_FUNCTIONS_HPP
#include <string>

std::pair<std::string, int> Pwd();
std::pair<std::string, int> Cd(const std::string& path);
std::pair<std::vector<std::string>, int> Ls();
std::pair<std::vector<std::string>, int> Ls(const std::string& path);

#endif  // PATH_FUNCTIONS_HPP
