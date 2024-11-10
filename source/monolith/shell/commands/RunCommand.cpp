#include <iostream>
#include "RunCommand.hpp"
#include "../util/run_functions.hpp"
#include <string>

int RunCommand::Run() {
  if (args_.empty()) {
    std::cout << "No binary specified to run." << '\n';
    return 1;
  }

  return RunProgram(args_);
}