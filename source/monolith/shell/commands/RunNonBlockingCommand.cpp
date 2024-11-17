#include "RunNonBlockingCommand.hpp"

#include <iostream>

#include "../util/run_functions.hpp"

int RunNonBlockingCommand::Run() {
  if (shell_ == nullptr) {
    std::cout << "This command requires a shell to run." << '\n';
    return -1;
  }

  if (args_.empty()) {
    std::cout << "No binary specified to run." << '\n';
    return 1;
  }

  auto [success, process_info] = RunNonBlockingProgram(args_);
  if (!success) {
    std::cout << "Failed to start process." << '\n';
    return -1;
  }

  auto pid = process_info.pid;
  shell_->AddActiveProcess(pid, process_info);
  return 0;
}