//
// Created by vadim on 11.10.2024.
//

#ifndef SHELL_HPP
#define SHELL_HPP
#include <functional>
#include <map>
#include <mutex>
#include <stack>
#include <string>
#include <thread>

#include "util/run_functions.hpp"

class Shell {
private:
  std::istream& input_;
  std::ostream& output_;
  bool running_;
  std::map<pid_t, ProcessInfo> active_processes_;
  std::mutex active_processes_mutex_;
  std::thread monitor_thread_;
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
  void StartProcessMonitor();
  std::map<pid_t, ProcessInfo>& GetActiveProcesses();
  void AddActiveProcess(pid_t pid, ProcessInfo info);
  void UpdateActiveProcess(pid_t pid, ProcessInfo info);
  void RemoveActiveProcess(pid_t pid);
  std::stack<std::string>& GetHistory();
  std::string GetWorkingDirectory();
  void SetWorkingDirectory(const std::string&);
  void PrintPlatform();
};

#endif  // SHELL_HPP
