#include "HelpCommand.hpp"
//
// Created by vadim on 12.10.2024.
//

std::map<std::string, std::string> CommandNameToDescription() {
  return {
      {    "nop","No operation. Same as inputting a blank string. Always succeeds."                 },
      {   "exit",                                                                 "Exit the shell."},
      {   "help",
       "Output the list of available commands or info about a specific command. Usage: `help "
       "[command_name]`"                                                                           },
      {"history",                     "Output last N commands (default N: 5). Usage: `history [n]`"},
      {     "ls",                                       "Lists all files in the working directory."},
      {     "cd", "Changes the working directory. Usage: `cd <directory_path>` (path is mandatory)"},
      {    "pwd",                                       "Prints the path to the working directory."},
      {    "run",            "Runs the executable and times execution. Usage: `run <program_name>`"},
      { "runnb", "Runs the executable in non-blocking mode and times execution. Usage: `runnb <program_name>`"},
    { "runnbfor", "Runs the executables in non-blocking mode several times, substituting the"
        " variable name, and times execution. Usage: `runnbfor <name>=<start>:<end>[:<step>] <program_name> [...args]`"}
  };
}

int HelpCommand::Run() {
  auto cmd_map = CommandNameToDescription();
  try {
    switch (this->args_.size()) {
      case 0: {
        for (const auto& entry : cmd_map) {
          std::cout << "- " << entry.first << ": " << entry.second << '\n';
        }
        return 0;
      }
      case 1: {
        auto command_name = this->args_[0];
        if (cmd_map.contains(command_name)) {
          std::cout << "- " << command_name << ": " << cmd_map.at(command_name)
                    << '\n';
          return 0;
        }
        std::cout << "No command name was found: " << command_name << '\n';
        return 1;
      }
      default: {
        std::cout << "Too many arguments for `help` command (<= 1 expected)" << '\n';
        return 2;
      }
    }
  } catch (const std::exception& e) {
    std::cout << "An error has occurred when running the command: " << e.what() << '\n';
    return -2;
  }
}
