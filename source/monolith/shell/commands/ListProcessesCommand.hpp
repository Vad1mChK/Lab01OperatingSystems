//
// Created by vad1mchk on 17/11/24.
//

#ifndef LISTPROCESSESCOMMAND_HPP
#define LISTPROCESSESCOMMAND_HPP
#include <string>
#include <vector>

#include "monolith/shell/Command.hpp"

class ListProcessesCommand : public Command {
public:
  ListProcessesCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif //LISTPROCESSESCOMMAND_HPP
