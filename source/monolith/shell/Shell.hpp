//
// Created by vadim on 11.10.2024.
//

#ifndef SHELL_HPP
#define SHELL_HPP
#include <stack>
#include <string>

class Shell {
private:
  std::istream& input_;
  std::ostream& output_;
  bool running_;
  inline static const std::string PromptString = "> ";
  std::stack<std::string> history_;
  std::string working_directory_;
  std::string ReadNextCommandString();

public:
  Shell();
  Shell(std::istream& input, std::ostream& output);
  void Start();
  void Run();
  void Stop();
  std::stack<std::string>& GetHistory();
  std::string GetWorkingDirectory();
  void SetWorkingDirectory(const std::string&);
  void PrintPlatform();
};

#endif  // SHELL_HPP
