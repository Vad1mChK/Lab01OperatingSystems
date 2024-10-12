//
// Created by vadim on 12.10.2024.
//

#ifndef CHANGEDIRECTORYCOMMAND_HPP
#define CHANGEDIRECTORYCOMMAND_HPP

#include "../Command.hpp"

class ChangeDirectoryCommand : public Command {
public:
  ChangeDirectoryCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif  // CHANGEDIRECTORYCOMMAND_HPP
