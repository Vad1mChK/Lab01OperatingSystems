#include <iostream>
#include <string>
#include <vector>

#include "unistd_check.hpp"

#if UNISTD_AVAILABLE  // If the target system is Unix (Linux, macOS, etc)
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>

static std::pair<std::vector<std::string>, int> ListFiles(const std::string& path);

std::pair<std::string, int> Pwd() {
  char buffer[PATH_MAX];
  if (getcwd(buffer, sizeof buffer) == nullptr) {
    return {"Failed to get the current working directory.", 1};
  }
  return {std::string(buffer), 0};
}

// `Cd` function: Changes the current directory
std::pair<std::string, int> Cd(const std::string& path) {
  if (chdir(path.c_str()) != 0) {
    // If changing the directory fails, return an error
    return {"Failed to change directory.", 1};
  }

  auto [new_directory, status] = Pwd();
  if (status != 0) {
    return {"Directory changed, but failed to retrieve the new directory.", 2};
  }

  return {new_directory, 0};  // Return the new directory and success status
}

std::pair<std::vector<std::string>, int> Ls() {
  // Get the current directory
  auto [current_directory, status] = Pwd();
  if (status != 0) {
    return {{}, 1};  // Error: unable to retrieve current directory
  }

  // List files in the current directory
  return ListFiles(current_directory);
}

std::pair<std::vector<std::string>, int> Ls(const std::string& path) {
  return ListFiles(path);
}

static std::pair<std::vector<std::string>, int> ListFiles(const std::string& path) {
  std::vector<std::string> files;
  DIR* dir = opendir(path.c_str());  // Open the directory
  if (dir == nullptr) {
    // If the directory couldn't be opened, return an error
    perror("opendir");
    return {{}, 1};
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr) {
    // Skip the special entries "." and ".."
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      files.emplace_back(entry->d_name);
    }
  }

  closedir(dir);  // Close the directory after use

  return {files, 0};  // Return the list of files and success status
}

#else
#ifdef _WIN32  // If the target system is Windows
#include <windows.h>

//
// Created by vadim on 12.10.2024.
//

static std::pair<std::vector<std::string>, int> ListFiles(const std::string& path);

std::pair<std::string, int> Pwd() {
  // Get the required buffer size for the current directory
  DWORD buffer_size = GetCurrentDirectory(0, nullptr);
  if (buffer_size == 0) {
    // If the buffer size couldn't be retrieved, return an error message and status code 1
    return {"Failed to get the current working directory.", 1};
  }

  // Allocate a buffer of the appropriate size
  std::vector<char> buffer(buffer_size);

  // Get the current working directory
  if (GetCurrentDirectory(buffer_size, buffer.data()) == 0) {
    // If the directory couldn't be retrieved, return an error message and status code 1
    return {"Failed to retrieve the current working directory.", 2};
  }

  // On success, return the directory path and status code 0
  return {std::string(buffer.data()), 0};
}

// `Cd` function: Changes the current directory
std::pair<std::string, int> Cd(const std::string& path) {
  // Attempt to change the current directory
  if (SetCurrentDirectory(path.c_str()) == 0) {
    // If the function fails, get the error message and return a failure status
    return {"Failed to change directory.", 1};
  }

  // If successful, return the new directory (using `Pwd` to retrieve it)
  auto [new_directory, status] = Pwd();
  if (status != 0) {
    return {"Directory changed, but failed to retrieve the new directory.", 2};
  }

  return {new_directory, 0};  // Return the new current directory and success status
}

std::pair<std::vector<std::string>, int> Ls() {
  // Get the current directory
  DWORD buffer_size = GetCurrentDirectory(0, nullptr);
  if (buffer_size == 0) {
    return {{}, 1};  // Error: unable to retrieve current directory
  }

  std::vector<char> buffer(buffer_size);
  if (GetCurrentDirectory(buffer_size, buffer.data()) == 0) {
    return {{}, 2};  // Error: unable to retrieve current directory
  }

  // List files in the current directory
  return ListFiles(std::string(buffer.data()));
}

std::pair<std::vector<std::string>, int> Ls(const std::string& path) {
  return ListFiles(path);
}

static std::pair<std::vector<std::string>, int> ListFiles(const std::string& path) {
  std::vector<std::string> files;
  WIN32_FIND_DATA find_file_data;
  HANDLE hFind;

  // Prepare search path (append \* to search for all files and directories)
  std::string search_path = path + "\\*";

  // Find the first file in the directory
  hFind = FindFirstFile(search_path.c_str(), &find_file_data);
  if (hFind == INVALID_HANDLE_VALUE) {
    // If the handle is invalid, return an error
    std::cout << "Handle is invalid." << '\n';
    return {{}, 1};
  }

  // Iterate through all the files and directories
  do {
    files.emplace_back(find_file_data.cFileName);  // Add the file or directory name to the list
  } while (FindNextFile(hFind, &find_file_data) != 0);  // Continue to the next file

  // Close the handle after use
  FindClose(hFind);

  // Return the list of files and success status (0)
  return {files, 0};
}

#else
// If the target system is TempleOS or worse
std::pair<std::string, int> Pwd() {
  return {"This command requires a Windows or Unix system call to run.", 1};
}

std::pair<std::string, int> Cd(const std::string&) {
  return {"This command requires a Windows or Unix system call to run.", 1};
}

std::pair<std::vector<std::string>, int> Ls() {
  return {{"This command requires a Windows or Unix system call to run."}, 1};
}

std::pair<std::vector<std::string>, int> Ls(const std::string&) {
  return {{"This command requires a Windows or Unix system call to run."}, 1};
}
#endif
#endif