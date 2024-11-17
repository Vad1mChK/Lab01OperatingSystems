//
// Created by vadim on 11.10.2024.
//

#include "Shell.hpp"

#include <sys/wait.h>
#include <sys/resource.h>

#include <csignal>
#include <iostream>
#include <string>

#include "../../common/unistd_check.hpp"
#include "Command.hpp"
#include "CommandFactory.hpp"
#include "util/path_functions.hpp"
#include "util/string_functions.hpp"

Shell::Shell() : input_(std::cin), output_(std::cout), running_(true) {
}

Shell::Shell(std::istream& input, std::ostream& output)
    : input_(input), output_(output), running_(true) {
}

Shell::~Shell() {
  Stop();
}

void Shell::PrintPlatform() {
  output_ << "Running on platform: " <<
#ifdef UNISTD_AVAILABLE
      "Unix"
#else
#ifdef _WIN32
      "Windows"
#else
      "unknown"
#endif
#endif
          << '\n';
}

void Shell::Start() {
  output_ << "Starting up shell." << '\n';
  PrintPlatform();

  auto [pwd_string, pwd_status_code] = Pwd();
  if (pwd_status_code != 0) {
    output_ << "Failed to initialize working directory." << '\n';
    return;
  }
  output_ << "Current working directory is: " << pwd_string << '\n';
  working_directory_ = pwd_string;
  active_processes_ = {};

  StartProcessMonitor();
  Run();
}

void Shell::StartProcessMonitor() {
  monitor_thread_ = std::thread([this]() {
      while (running_) {
          CheckChildProcesses();

          std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
  });
  monitor_thread_.detach();
}

void Shell::Run() {
  while (this->running_) {
    std::string prompt = "[" + ShortenEachPart(working_directory_, '/', 1, false) + "]> ";
    output_ << prompt;
    std::string command_string = ReadNextCommandString();
    if (IsBlank(command_string)) {
      continue;
    }
    auto token_list = SplitString(command_string);
    if (token_list.empty() || token_list[0].empty()) {
      continue;
    }
    auto command =
        CommandFactory(token_list.front(), std::vector(token_list.begin() + 1, token_list.end()));

    if (command != nullptr) {
      command->BindToShell(this);
      command->Run();
    } else {
      output_ << "Unknown command: `" << command_string << '`' << '\n';
    }

    this->history_.push(command_string);
  }
}

std::string Shell::ReadNextCommandString() {
  output_ << "";
  std::string command_string;
  std::getline(input_, command_string);
  return command_string;
}

void Shell::Stop() {
  StopProcessMonitor();
  running_ = false;
}

void Shell::StopProcessMonitor() {
  if (monitor_thread_.joinable()) {
    monitor_thread_.join();
  }
}

void Shell::CheckChildProcesses() {
    std::vector<pid_t> terminated_pids; // To store PIDs of terminated processes

    for (auto& [pid, info] : active_processes_) {
        int status;
        struct rusage usage; // To collect resource usage statistics

        pid_t result = wait4(pid, &status, WNOHANG, &usage); // Non-blocking wait for child process

        if (result == 0) {
            // Process is still running
            continue;
        } else if (result == pid) {
            // Process has terminated
            CollectUsageStatistics(info, usage);

            // Print statistics
            std::cout << info.ToString() << '\n';

            // Mark process for removal
            terminated_pids.push_back(pid);
        } else if (result == -1) {
            // Error handling
            if (errno == ECHILD) {
                // Process has already been cleaned up
                terminated_pids.push_back(pid);
            } else {
                perror("wait4 failed");
            }
        }
    }

    // Remove terminated processes from the map
    for (pid_t pid : terminated_pids) {
        active_processes_.erase(pid);
    }
}


std::map<pid_t, ProcessInfo>& Shell::GetActiveProcesses() {
  return active_processes_;
}

void Shell::AddActiveProcess(pid_t pid, ProcessInfo info) {
  active_processes_[pid] = info;
}

void Shell::RemoveActiveProcess(pid_t pid) {
  active_processes_.erase(pid);
}

void Shell::CollectUsageStatistics(
    ProcessInfo& info,
    const struct rusage& usage
) {
  info.minor_page_faults = usage.ru_minflt;
  info.major_page_faults = usage.ru_majflt;
  info.voluntary_context_switches = usage.ru_nvcsw;
  info.nonvoluntary_context_switches = usage.ru_nivcsw;
}


std::stack<std::string>& Shell::GetHistory() {
  return history_;
}

std::string Shell::GetWorkingDirectory() {
  return working_directory_;
}

void Shell::SetWorkingDirectory(const std::string& pwd_string) {
  working_directory_ = pwd_string;
}