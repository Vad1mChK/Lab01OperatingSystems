//
// Created by vadim on 12.10.2024.
//
#include "PathToWorkingDirectoryCommand.hpp"

#include <iostream>

#include "../util/path_functions.hpp"

int PathToWorkingDirectoryCommand::Run() {
  auto [pwd_string, pwd_status_code] = Pwd();

  if (pwd_status_code != 0) {
    std::cout << "Error: ";
  }

  std::cout << pwd_string << '\n';

  return pwd_status_code;
}