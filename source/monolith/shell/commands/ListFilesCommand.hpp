//
// Created by vadim on 12.10.2024.
//

#ifndef LISTFILESCOMMAND_H
#define LISTFILESCOMMAND_H

#include "../Command.hpp"

class ListFilesCommand : public Command {
public:
  ListFilesCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif  // LISTFILESCOMMAND_H
