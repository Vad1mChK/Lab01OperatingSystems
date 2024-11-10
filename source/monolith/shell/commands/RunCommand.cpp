#include "RunCommand.hpp"

#include <iostream>
#include <string>

#include "../util/run_functions.hpp"

int RunCommand::Run() {
  if (args_.empty()) {
    std::cout << "No binary specified to run." << '\n';
    return 1;
  }

  return RunProgram(args_);
}