#include <windows.h>

#include <iostream>
#include <string>
#include <vector>

//
// Created by vadim on 12.10.2024.
//
std::pair<std::string, int> pwd() {
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

// `cd` function: Changes the current directory
std::pair<std::string, int> cd(const std::string& path) {
  // Attempt to change the current directory
  if (SetCurrentDirectory(path.c_str()) == 0) {
    // If the function fails, get the error message and return a failure status
    return {"Failed to change directory.", 1};
  }

  // If successful, return the new directory (using `pwd` to retrieve it)
  auto [new_directory, status] = pwd();
  if (status != 0) {
    return {"Directory changed, but failed to retrieve the new directory.", 2};
  }

  return {new_directory, 0};  // Return the new current directory and success status
}