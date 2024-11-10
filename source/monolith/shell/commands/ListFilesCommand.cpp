#include "ListFilesCommand.hpp"

#include <iostream>

#include "monolith/shell/util/path_functions.hpp"
//
// Created by vadim on 12.10.2024.
//

void printList(std::vector<std::string> strings);

int ListFilesCommand::Run() {
  if (this->shell_ == nullptr) {
    std::cout << "This command requires a shell to run." << '\n';
    return -1;
  }

  try {
    switch (this->args_.size()) {
      case 0: {
        auto [ls_files, ls_status_code] = ls();
        std::cout << "Listing files and directories at path: " << shell_->GetWorkingDirectory()
                  << "" << '\n';
        if (ls_status_code != 0) {
          std::cout << "Error loading list of files." << '\n';
          return ls_status_code;
        }
        printList(ls_files);
        return 0;
      }
      case 1: {
        auto [ls_files, ls_status_code] = ls(this->args_[0]);
        std::cout << "Listing files and directories at path: " << this->args_[0] << "" << '\n';
        if (ls_status_code != 0) {
          std::cout << "Error loading list of files." << '\n';
          return ls_status_code;
        }
        printList(ls_files);
        return 0;
      }
      default: {
        std::cout << "Error: Wrong arguments count for command `cd` (<= 1 expected)" << '\n';
        return 3;
      }
    }
  } catch (const std::exception& e) {
    std::cout << "An error has occurred when running the command: " << e.what() << '\n';
    return -2;
  }
}

void printList(std::vector<std::string> strings) {
  for (auto str : strings) {
    std::cout << "- " << str << '\n';
  }
}