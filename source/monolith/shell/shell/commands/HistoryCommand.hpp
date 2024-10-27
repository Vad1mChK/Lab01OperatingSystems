//
// Created by vadim on 12.10.2024.
//

#ifndef HISTORYCOMMAND_HPP
#define HISTORYCOMMAND_HPP

#include "../Command.hpp"

class HistoryCommand : public Command {
public:
  HistoryCommand(const std::vector<std::string>& args) : Command(args) {
  }

  int Run() override;
};

#endif  // HISTORYCOMMAND_HPP
