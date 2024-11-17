//
// Created by vadim on 14.10.2024.
//
#include <iostream>
#include <string>

#include "RamMemorySorter.hpp"
#include "../../common/unistd_check.hpp"
#include "../util/ema_ram_sorter_cli_constants.hpp"

#ifdef UNISTD_AVAILABLE
#include <linux/sched.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#endif

constexpr size_t MaxProcesses = 32;

void RunFullBenchmarkParallel(
    const std::string& input_file,
    const std::string& output_file,
    size_t repeat_count,
    size_t file_size_mb
);

int main(int argc, char* argv[]) {
  if (argc < ArgcMin) {
    RamMemorySorter::printHelp();
    return 1;
  }

  std::string command = argv[1];

  if (command == "generate") {
    if (argc != ArgcForGenerate) {
      std::cout << "Usage: prog generate <output_file> <size_mb>" << '\n';
      return 1;
    }
    std::string output_file = argv[2];
    size_t size_mb = std::stoull(argv[3]);
    RamMemorySorter::generateRandomFile(output_file, size_mb);
  } else if (command == "sort") {
    if (argc != ArgcForRamSort) {
      std::cout << "Usage: prog sort <input_file> <output_file>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    RamMemorySorter::sortInMemory(input_file, output_file);
  } else if (command == "check") {
    if (argc != ArgcForCheck) {
      std::cout << "Usage: prog check <input_file>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    RamMemorySorter::checkFileSorted(input_file);
  } else if (command == "full-benchmark") {
    if (argc != ArgcForFull) {
      std::cout << "Usage: prog full-benchmark <input_file> <output-file> <repeat-count>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    size_t repeat_count = std::stoull(argv[4]);
#ifdef UNISTD_AVAILABLE
    RunFullBenchmarkParallel(
        input_file,
        output_file,
        repeat_count,
        FullBenchmarkFileSizeMb
    );
#else
    // Fallback to sequential execution
    for (size_t i = 0; i < repeat_count; ++i) {
      RamMemorySorter::generateRandomFile(input_file, FullBenchmarkFileSizeMb);
      RamMemorySorter::sortInMemory(input_file, output_file);
      RamMemorySorter::checkFileSorted(output_file);
    }
#endif
  } else if (command == "help") {
    RamMemorySorter::printHelp();
  } else {
    std::cout << "Unknown command: " << command << '\n';
    RamMemorySorter::printHelp();
    return 1;
  }

  return 0;
}

#ifdef UNISTD_AVAILABLE
void RunFullBenchmarkParallel(
    const std::string& input_file,
    const std::string& output_file,
    size_t repeat_count,
    size_t file_size_mb
) {
  std::vector<pid_t> child_pids;

  for (size_t i = 0; i < repeat_count && i < MaxProcesses; ++i) {
    struct clone_args cl_args = {};
    cl_args.flags = 0; // Flags for the clone
    cl_args.pidfd = 0;
    cl_args.exit_signal = SIGCHLD;                         // Signal sent to parent

    // Create a new child process using clone3
    pid_t pid = syscall(SYS_clone3, &cl_args, sizeof(cl_args));
    if (pid < 0) {
      perror("clone3 failed");
      continue; // Skip waiting for this child
    }

    if (pid == 0) {
      // Child process
      std::string input_file_name = input_file + "." + std::to_string(i);
      std::string output_file_name = output_file + "." + std::to_string(i);

      try {
        // Generate, sort, and check in the child
        RamMemorySorter::generateRandomFile(input_file_name, file_size_mb);
        RamMemorySorter::sortInMemory(input_file_name, output_file_name);
        RamMemorySorter::checkFileSorted(output_file_name);
      } catch (const std::exception& e) {
        std::cerr << "Error in child process: " << e.what() << '\n';
        _exit(1); // Exit child process with error
      }

      _exit(0); // Exit child process
    } else {
      // Parent process: record the child PID
      child_pids.push_back(pid);
    }
  }

  // Parent process waits for all children to complete
  for (pid_t child_pid : child_pids) {
    int status = 0;
    pid_t waited_pid = waitpid(child_pid, &status, 0);
    if (waited_pid == -1) {
      perror("waitpid failed");
    } else if (WIFEXITED(status)) {
      std::cout << "Child " << waited_pid << " exited with status " << WEXITSTATUS(status) << '\n';
    } else if (WIFSIGNALED(status)) {
      std::cout << "Child " << waited_pid << " was terminated by signal " << WTERMSIG(status) << '\n';
    }
  }
}
#endif