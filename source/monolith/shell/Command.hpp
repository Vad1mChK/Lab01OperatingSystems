#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <string>
#include <vector>

#include "Shell.hpp"

class Command {
protected:
  std::vector<std::string> args_;  // Arguments passed to the command
  Shell* shell_;                   // Pointer to Shell

public:
  Command(const std::vector<std::string>& args) : args_(args), shell_(nullptr) {
  }

  virtual ~Command() = default;

  void BindToShell(Shell* shell);

  virtual int Run() = 0;  // Pure virtual function to be implemented by derived classes

  std::vector<std::string> GetArgs() const;
};

#endif  // COMMAND_HPP
