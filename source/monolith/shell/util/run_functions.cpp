//
// Created by vad1mchk on 2024/11/10.
//
#include "run_functions.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "../../../common/unistd_check.hpp"

#if UNISTD_AVAILABLE
// Unix (Linux, macOS, ...): clone3
#include <linux/sched.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <cstring>

std::pair<bool, int> RunProgramUtil(std::vector<const char*> argv);

std::pair<bool, pid_t> RunNonBlockingProgramUtil(
  std::vector<const char*> argv
);

bool FileExistsInCurrentDir(const std::string& filename) {
  struct stat buffer{};
  return (stat(filename.c_str(), &buffer) == 0);
}

std::string ProcessInfo::ToString() {
  std::stringstream strstr;
  auto elapsed = std::chrono::duration<double>(
    std::chrono::high_resolution_clock::now() - this->t_start
  );
  strstr << this->pid << '\n'
    << '\t' << "name: " << this->name << '\n'
    << '\t' << "elapsed: " << elapsed.count() << 's' << '\n'
    << '\t' << "minor page faults: " << this->minor_page_faults << '\n'
    << '\t' << "major page faults: " << this->major_page_faults << '\n'
    << '\t' << "voluntary context switches: " << this->voluntary_context_switches << '\n'
    << '\t' << "nonvoluntary context switches: " << this->nonvoluntary_context_switches << '\n';

  return strstr.str();
}

int RunProgram(std::vector<std::string>& args) {
  if (!args[0].starts_with("./") && FileExistsInCurrentDir(args[0])) {
    args[0] = "./" + args[0];  // Prepend ./ if the file exists in the current directory
  }

  std::vector<const char*> argv;
  argv.reserve(args.size());
  for (const auto& arg : args) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

  auto start_time = std::chrono::high_resolution_clock::now();

  auto [hasExecuted, statusCode] = RunProgramUtil(argv);
  if (!hasExecuted) {
    std::cout << "Failed to execute program." << '\n';
  } else {
    std::cout << "Program executed with status code " << statusCode << '\n';
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time);
  std::cout << "Execution of " << (!args.empty() ? args[0] : "this program") << " took "
            << duration_ms.count() << "ms" << '\n';
  return statusCode;
}

//int RunNonBlockingProgram(std::vector<std::string>& args) {
//  if (!args[0].starts_with("./") && FileExistsInCurrentDir(args[0])) {
//    args[0] = "./" + args[0];  // Prepend ./ if the file exists in the current directory
//  }
//
//  std::vector<const char*> argv;
//  argv.reserve(args.size());
//  for (const auto& arg : args) {
//    argv.push_back(arg.c_str());
//  }
//  argv.push_back(nullptr);
//
//
//}

std::pair<bool, int> RunProgramUtil(std::vector<const char*> argv) {
  struct clone_args cl_args = {};
  cl_args.flags = 0; // No special flags
  cl_args.exit_signal = SIGCHLD;

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe failed");
    return {false, 2};
  }

  auto pid = static_cast<pid_t>(syscall(SYS_clone3, &cl_args, sizeof(cl_args)));
  if (pid < 0) {
    perror("clone3 failed");
    return {false, 2};
  }

  if (pid == 0) {
    // Child process
    close(pipefd[0]); // Close read end of the pipe
    execvp(argv[0], const_cast<char* const*>(argv.data()));

    // If execvp fails, write an error to the pipe
    int exec_error = errno;
    write(pipefd[1], &exec_error, sizeof(exec_error));
    close(pipefd[1]);
    _exit(1);
  }

  // Parent process
  close(pipefd[1]); // Close write end of the pipe

  // Check if the child reported an exec error
  int exec_error = 0;
  read(pipefd[0], &exec_error, sizeof(exec_error));
  close(pipefd[0]);

  if (exec_error != 0) {
    std::cout << "execvp failed: " << strerror(exec_error) << '\n';
    waitpid(pid, nullptr, 0); // Clean up child process
    return {false, 3};
  }

  // Wait for the child process to complete
  int status = -1;
  waitpid(pid, &status, 0);

  if (WIFEXITED(status)) {
    return {true, WEXITSTATUS(status)};
  }

  std::cout << "Process terminated abnormally.\n";
  return {false, 4};
}

std::pair<bool, ProcessInfo> RunNonBlockingProgram(
  std::vector<std::string>& args
) {
  if (!args[0].starts_with("./") && FileExistsInCurrentDir(args[0])) {
    args[0] = "./" + args[0];  // Prepend "./" if necessary
  }

  std::vector<const char*> argv;
  argv.reserve(args.size());
  for (const auto& arg : args) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

  auto [success, pid] = RunNonBlockingProgramUtil(argv);
  if (!success) {
    std::cerr << "Failed to start non-blocking program: " << args[0] << '\n';
    return { false, {} };
  }

  std::cout << "Started non-blocking process with PID: " << pid << '\n';

  return { true, {
    .pid = pid,
    .name = argv[0],
    .t_start = std::chrono::high_resolution_clock::now(),
    .voluntary_context_switches = 0,
    .nonvoluntary_context_switches = 0,
    .major_page_faults = 0,
    .minor_page_faults = 0
  } };
}

std::pair<bool, int> RunNonBlockingProgramUtil(
    std::vector<const char*> argv
) {
  struct clone_args cl_args = {};
  cl_args.flags = 0;
  cl_args.exit_signal = SIGCHLD;

  pid_t pid = static_cast<pid_t>(syscall(SYS_clone3, &cl_args, sizeof(cl_args)));
  if (pid < 0) {
    perror("clone3 failed");
    return {false, -1};
  }

  if (pid == 0) {
    execvp(argv[0], const_cast<char* const*>(argv.data()));
    perror("execvp failed");
    _exit(1);
  }

  return {true, pid};
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
          &pi
      )) {
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

pid_t RunNonBlockingProgram(std::vector<std::string>& args) {
  std::cout << "Cannot run on platforms other than Unix." << '\n';
}
#else
// TempleOS or worse
int RunProgram(std::vector<std::string>& args) {
  std::cout << "Cannot run on platforms other than Unix and Windows." << '\n';
  return 1;
}

pid_t RunNonBlockingProgram(std::vector<std::string>& args) {
  std::cout << "Cannot run on platforms other than Unix." << '\n';
}
#endif
#endif