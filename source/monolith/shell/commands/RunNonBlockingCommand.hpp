//
// Created by vad1mchk on 2024/11/11.
//

#ifndef RUNNBCOMMAND_HPP
#define RUNNBCOMMAND_HPP

#include "../Command.hpp"

class RunNonBlockingCommand : public Command {
public:
  RunNonBlockingCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif  // RUNNBCOMMAND_HPP
