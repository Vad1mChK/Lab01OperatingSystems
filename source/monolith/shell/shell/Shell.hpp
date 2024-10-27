//
// Created by vadim on 11.10.2024.
//

#ifndef SHELL_HPP
#define SHELL_HPP
#include <stack>
#include <string>

class Shell {
private:
  bool running_;
  inline static const std::string PromptString = "> ";
  std::stack<std::string> history_;
  std::string working_directory_;
  static std::string ReadNextCommandString();

public:
  Shell();
  void Start();
  void Run();
  void Stop();
  std::stack<std::string>& GetHistory();
  std::string GetWorkingDirectory();
  void SetWorkingDirectory(const std::string&);
};

#endif  // SHELL_HPP
