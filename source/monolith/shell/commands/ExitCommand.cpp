#include "ExitCommand.hpp"

#include <iostream>
//
// Created by vadim on 12.10.2024.
//
int ExitCommand::Run() {
  if (this->shell_ == nullptr) {
    std::cout << "This command requires a shell to run." << '\n';
    return -1;
  }
  std::cout << "Exiting shell..." << '\n';
  shell_->Stop();
  return 0;
}