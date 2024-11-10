//
// Created by vadim on 12.10.2024.
//

#include "ChangeDirectoryCommand.hpp"

#include <iostream>

#include "../util/path_functions.hpp"

int ChangeDirectoryCommand::Run() {
  if (this->shell_ == nullptr) {
    std::cout << "This command requires a shell to run." << '\n';
    return -1;
  }

  if (this->args_.size() != 1) {
    std::cout << "Error: Wrong arguments count for command `cd` (1 expected)" << '\n';
    return 3;
  }

  auto [pwd_string, pwd_status_code] = Cd(this->args_[0]);

  if (pwd_status_code != 0) {
    std::cout << "Error: " << pwd_string << '\n';
    return pwd_status_code;
  }

  this->shell_->SetWorkingDirectory(pwd_string);
  return 0;
}