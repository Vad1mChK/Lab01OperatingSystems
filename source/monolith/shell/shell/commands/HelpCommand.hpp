//
// Created by vadim on 12.10.2024.
//

#ifndef HELPCOMMAND_HPP
#define HELPCOMMAND_HPP

#include <iostream>
#include <map>

#include "../Command.hpp"

class HelpCommand : public Command {
public:
  explicit HelpCommand(const std::vector<std::string>& args) : Command(args) {}

  int Run() override;
};

#endif // HELP_COMMAND_HPP