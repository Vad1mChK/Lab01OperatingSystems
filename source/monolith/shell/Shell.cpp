//
// Created by vadim on 11.10.2024.
//

#include "Shell.hpp"

#include <iostream>
#include <string>

#include "Command.hpp"
#include "CommandFactory.hpp"
#include "util/path_functions.hpp"
#include "util/unistd_check.hpp"

Shell::Shell() : input_(std::cin), output_(std::cout), running_(true), working_directory_("") {
}

Shell::Shell(std::istream& input, std::ostream& output)
    : input_(input), output_(output), running_(true), working_directory_("") {
}

void Shell::PrintPlatform() {
  output_ << "Running on platform: " <<
#ifdef UNISTD_AVAILABLE
      "Unix"
#else
#ifdef _WIN32
      "Windows"
#else
      "unknown"
#endif
#endif
          << '\n';
}

void Shell::Start() {
  output_ << "Starting up shell." << '\n';
  PrintPlatform();

  auto [pwd_string, pwd_status_code] = Pwd();
  if (pwd_status_code != 0) {
    output_ << "Failed to initialize working directory." << '\n';
    return;
  }
  output_ << "Current working directory is: " << pwd_string << '\n';
  working_directory_ = pwd_string;
  this->Run();
}

void Shell::Run() {
  while (this->running_) {
    output_ << "[" + working_directory_ + "]>";
    std::string command_string = ReadNextCommandString();
    if (IsBlank(command_string)) {
      continue;
    }
    auto token_list = SplitString(command_string);
    if (token_list.empty() || token_list[0].empty()) {
      continue;
    }
    auto command =
        CommandFactory(token_list.front(), std::vector(token_list.begin() + 1, token_list.end()));

    if (command != nullptr) {
      command->BindToShell(this);
      command->Run();
    } else {
      output_ << "Unknown command: `" << command_string << '`' << '\n';
    }

    this->history_.push(command_string);
  }
}

std::string Shell::ReadNextCommandString() {
  output_ << "";
  std::string command_string;
  std::getline(input_, command_string);
  return command_string;
}

void Shell::Stop() {
  running_ = false;
}

std::stack<std::string>& Shell::GetHistory() {
  return history_;
}

std::string Shell::GetWorkingDirectory() {
  return working_directory_;
}

void Shell::SetWorkingDirectory(const std::string& pwd_string) {
  working_directory_ = pwd_string;
}