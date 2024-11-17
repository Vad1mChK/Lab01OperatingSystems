//
// Created by vad1mchk on 17/11/24.
//
#include "ListProcessesCommand.hpp"

#include <iostream>

int ListProcessesCommand::Run() {
  if (shell_ == nullptr) {
    std::cout << "This command requires a shell to run." << '\n';
    return -1;
  }

  if (shell_->GetActiveProcesses().empty()) {
    std::cout << "No non-blocking processes are currently running." << '\n';
    return 0;
  }

  for (auto& [pid, info] : shell_->GetActiveProcesses()) {
    UpdateProcessStats(info);
    auto duration = std::chrono::high_resolution_clock::now() - info.t_start;
    auto seconds = std::chrono::duration<double>(duration).count();
    std::cout << pid << ":\n"
      << "\tname: "<< info.name << '\n'
      << "\tmin_flt: " << info.minor_page_faults << '\n'
      << "\tmaj_flt: " << info.major_page_faults << '\n'
      << "\tstarted: " << seconds << "s ago\n";
  }

  return -1;
}
