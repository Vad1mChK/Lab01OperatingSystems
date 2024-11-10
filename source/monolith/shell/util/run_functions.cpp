//
// Created by vad1mchk on 2024/11/10.
//
#include "run_functions.hpp"
#include "unistd_check.h"
#include <iostream>
#include <string>
#include <vector>

#if UNISTD_AVAILABLE
  // Unix (Linux, macOS, ...): clone3
  #include <cstring>
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <sys/syscall.h>
  #include <linux/sched.h>
  #include <unistd.h>
  #include <chrono>

  static std::pair<bool, int> _runProgram(std::vector<const char*> argv);

  int RunProgram(std::vector<std::string>& args) {
    std::vector<const char*> argv;
    argv.reserve(args.size());
    for (const auto& arg: args) {
      argv.push_back(arg.c_str());
    }
    argv.push_back(nullptr);

    auto start_time = std::chrono::high_resolution_clock::now();

    auto [hasExecuted, statusCode] = _runProgram(argv);
    if (!hasExecuted) {
      std::cout << "Failed to execute program." << '\n';
    } else {
      std::cout << "Program executed with status code " << statusCode << '\n';
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time);
    std::cout << "Execution of " <<
        (!args.empty() ? args[0] : "this program")
              << " took " << duration_ms.count() << "ms" << '\n';
    return statusCode;
  }

  static std::pair<bool, int> _runProgram(std::vector<const char*> argv) {
    // Use clone3 for process creation
    struct clone_args cl_args = {};
    cl_args.flags = 0; // No special flags
    cl_args.pidfd = 0; // No PID file descriptor
    pid_t pid = syscall(SYS_clone3, &cl_args, sizeof(cl_args));
    if (pid < 0) {
      perror("clone3 failed");
      return { false, 2 };
    }

    if (pid == 0) {
      // Child process: Execute the binary
      execvp(argv[0], const_cast<char* const*>(argv.data()));
      perror("execvp failed");
      _exit(1); // Exit if execvp fails
    }

    int status;
    waitpid(pid, &status, 0);

    if (!WIFEXITED(status)) {
      std::cout << "Process terminated abnormally.\n";
      return { false, 3 };
    }

    return {true, WEXITSTATUS(status)};
  }

#else
#ifdef _WIN32
  // Windows: CreateProcess
  #include <windows.h>
  static LONGLONG GetCurrentTimeInMicroseconds() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return (uli.QuadPart / 10);  // Convert to microseconds
  }

  int RunProgram(const std::vector<std::string>& args) {
    std::string command_line;
    for (size_t i = 0; i < args.size(); ++i) {
      command_line += args[i];
      if (i != args.size() - 1) {
        command_line += " ";
      }
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    LONGLONG start_time = GetCurrentTimeInMicroseconds();

    if (!CreateProcess(
            nullptr,
            const_cast<char*>(command_line.c_str()),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi)) {
      DWORD return_code = GetLastError();
      std::cout << "CreateProcess failed with error code " << return_code << '\n';
      return 2;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    LONGLONG end_time = GetCurrentTimeInMicroseconds();
    LONGLONG exec_time = end_time - start_time;

    std::cout << "Execution time: " << (exec_time / 1000.0) << " ms" << '\n';

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
  }
#else
  // TempleOS or worse
  int RunProgram(std::vector<std::string>& args) {
    std::cout << "Cannot run on platforms other than Unix and Windows." << '\n';
    return 1;
  }
#endif
#endif