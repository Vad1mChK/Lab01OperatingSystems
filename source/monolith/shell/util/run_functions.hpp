//
// Created by vad1mchk on 2024/11/10.
//

#ifndef RUN_FUNCTIONS_H
#define RUN_FUNCTIONS_H
#include <chrono>
#include <map>
#include <csignal>
#include <string>
#include <vector>

struct ProcessInfo {
  pid_t pid;
  std::string name;
  std::chrono::high_resolution_clock::time_point t_start;
  size_t voluntary_context_switches;
  size_t nonvoluntary_context_switches;
  size_t major_page_faults;
  size_t minor_page_faults;
};

int RunProgram(std::vector<std::string>& args);

void UpdateProcessStats(ProcessInfo& info);

std::pair<bool, ProcessInfo> RunNonBlockingProgram(
  std::vector<std::string>& args
);

#endif  // RUN_FUNCTIONS_H
