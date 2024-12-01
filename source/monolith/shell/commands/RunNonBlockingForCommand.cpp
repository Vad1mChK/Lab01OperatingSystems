#include "RunNonBlockingForCommand.hpp"

#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../util/run_functions.hpp"

int RunNonBlockingForCommand::Run() {
  if (shell_ == nullptr) {
    std::cout << "This command requires a shell to run." << '\n';
    return -1;
  }

  if (args_.size() < 2) {
    std::cout << "Usage: runnbfor <name>=<start>:<end>[:<step>] <program> [...args]" << '\n';
    return 1;
  }

  // Parse the range parameter
  std::string range_param = args_[0];
  std::regex range_regex(R"(^(\w+)=(-?\d+):(-?\d+)(?::(-?\d+))?$)");
  std::smatch match;
  if (!std::regex_match(range_param, match, range_regex)) {
    std::cout << "Invalid range format. Expected format: <name>=<start>:<end>[:<step>]." << '\n';
    return 1;
  }

  std::string var_name = match[1];
  int start = std::stoi(match[2]);
  int end = std::stoi(match[3]);
  int step = match[4].matched ? std::stoi(match[4]) : 1;

  if (step == 0) {
    std::cout << "Step value cannot be zero." << '\n';
    return 1;
  }

  // Prepare the program and arguments
  std::vector<std::string> program_args(args_.begin() + 1, args_.end());

  // Iterate over the range and launch processes
  for (int i = start; (step > 0 ? i <= end : i >= end); i += step) {  // End is inclusive
    std::vector<std::string> current_args = program_args;
    for (auto& arg : current_args) {
      // Replace all occurrences of <var_name> with the current value
      size_t pos;
      while ((pos = arg.find('<' + var_name + '>')) != std::string::npos) {
        arg.replace(pos, var_name.length() + 2, std::to_string(i));
      }
    }

    auto [success, process_info] = RunNonBlockingProgram(current_args);
    if (!success) {
      std::cout << "Failed to start process for iteration " << i << "." << '\n';
      continue;
    }

    shell_->AddActiveProcess(process_info.pid, process_info);
    std::cout << "Started process PID: " << process_info.pid << " for iteration " << i << "."
              << '\n';
  }

  return 0;
}
