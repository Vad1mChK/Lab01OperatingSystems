//
// Created by vadim on 11.10.2024.
//

#include "Shell.hpp"

#include <sys/wait.h>

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
  running_ = false;
}

void Shell::StartProcessMonitor() {
    monitor_thread_ = std::thread([this]() {
        while (running_) {
            std::vector<pid_t> pids_to_check;

            // Copy the list of PIDs to check without holding the lock
            {
                std::lock_guard<std::mutex> lock(active_processes_mutex_);
                for (const auto& entry : active_processes_) {
                    pids_to_check.push_back(entry.first);
                }
            }

            bool any_process_terminated = false;

            // Iterate over the PIDs and check their status
            for (pid_t pid : pids_to_check) {
                int status;
                pid_t result = waitpid(pid, &status, WNOHANG);
                if (result == 0) {
                    // Process is still running
                    continue;
                }
                if (result == pid) {
                    // Process terminated
                    std::lock_guard<std::mutex> lock(active_processes_mutex_);
                    auto it = active_processes_.find(pid);
                    if (it != active_processes_.end()) {
                        auto end_time = std::chrono::high_resolution_clock::now();
                        auto duration_ms = std::chrono::duration<double, std::milli>(
                            end_time - it->second.t_start
                        );
                        std::cout << "Process " << it->second.name << " (PID " << pid
                                  << ") terminated after " << duration_ms.count() << " ms\n";
                        RemoveActiveProcess(pid);
                    }
                    any_process_terminated = true;
                } else if (result == -1) {
                    // Error occurred
                    if (errno == ECHILD) {
                        // No such child process; remove from map
                        std::lock_guard<std::mutex> lock(active_processes_mutex_);
                        active_processes_.erase(pid);
                    } else {
                        perror("waitpid failed");
                    }
                }
            }

            if (!any_process_terminated) {
                // Sleep for a short duration before checking again
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    });
    monitor_thread_.detach();
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

std::stack<std::string>& Shell::GetHistory() {
  return history_;
}

std::string Shell::GetWorkingDirectory() {
  return working_directory_;
}

void Shell::SetWorkingDirectory(const std::string& pwd_string) {
  working_directory_ = pwd_string;
}