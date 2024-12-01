//
// Created by vad1mchk on 01/12/24.
//

#ifndef RUNNBFORCOMMAND_H
#define RUNNBFORCOMMAND_H

#include "../Command.hpp"

class RunNonBlockingForCommand : public Command {
public:
  RunNonBlockingForCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif  // RUNNBFORCOMMAND_H