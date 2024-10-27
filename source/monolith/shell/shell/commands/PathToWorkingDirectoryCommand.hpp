//
// Created by vadim on 12.10.2024.
//

#ifndef PATHTOWORKINGDIRECTORYCOMMAND_HPP
#define PATHTOWORKINGDIRECTORYCOMMAND_HPP

#include "../Command.hpp"

class PathToWorkingDirectoryCommand : public Command {
public:
  PathToWorkingDirectoryCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif //PATHTOWORKINGDIRECTORYCOMMAND_HPP
