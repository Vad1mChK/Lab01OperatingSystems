//
// Created by vadim on 11.10.2024.
//
#include "Command.hpp"

#include <functional>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "Shell.hpp"

void Command::BindToShell(Shell* shell) {
  shell_ = shell;
}

std::vector<std::string> Command::GetArgs() const {
  return args_;
}