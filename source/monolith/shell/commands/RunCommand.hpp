//
// Created by vadim on 12.10.2024.
//

#ifndef RUNCOMMAND_HPP
#define RUNCOMMAND_HPP

#include "../Command.hpp"

class RunCommand : public Command {
public:
  RunCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif  // RUNCOMMAND_HPP
