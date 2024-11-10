//
// Created by vadim on 12.10.2024.
//

#ifndef EXITCOMMAND_HPP
#define EXITCOMMAND_HPP
#include "../Command.hpp"

class ExitCommand : public Command {
public:
  ExitCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif  // EXITCOMMAND_HPP
