#include "RunNonBlockingCommand.hpp"

#include <iostream>

#include "../util/run_functions.hpp"

int RunNonBlockingCommand::Run() {
  if (args_.empty()) {
    std::cout << "No binary specified to run." << '\n';
    return 1;
  }

  return RunNonBlockingProgram(args_);
}