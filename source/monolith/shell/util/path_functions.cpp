#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>

//
// Created by vadim on 12.10.2024.
//

static std::pair<std::vector<std::string>, int> listFiles(const std::string& path);

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

std::pair<std::vector<std::string>, int> ls() {
  // Get the current directory
  DWORD buffer_size = GetCurrentDirectory(0, nullptr);
  if (buffer_size == 0) {
    return { {}, 1 };  // Error: unable to retrieve current directory
  }

  std::vector<char> buffer(buffer_size);
  if (GetCurrentDirectory(buffer_size, buffer.data()) == 0) {
    return { {}, 2 };  // Error: unable to retrieve current directory
  }

  // List files in the current directory
  return listFiles(std::string(buffer.data()));
}

std::pair<std::vector<std::string>, int> ls(const std::string& path) {
  return listFiles(path);
}

static std::pair<std::vector<std::string>, int> listFiles(const std::string& path) {
  std::vector<std::string> files;
  WIN32_FIND_DATA find_file_data;
  HANDLE hFind;

  // Prepare search path (append \* to search for all files and directories)
  std::string search_path = path + "\\*";

  // Find the first file in the directory
  hFind = FindFirstFile(search_path.c_str(), &find_file_data);
  if (hFind == INVALID_HANDLE_VALUE) {
    // If the handle is invalid, return an error
    std::cout << "Handle is invalid." << std::endl;
    return { {}, 1 };
  }

  // Iterate through all the files and directories
  do {
    files.emplace_back(find_file_data.cFileName);  // Add the file or directory name to the list
  } while (FindNextFile(hFind, &find_file_data) != 0);  // Continue to the next file

  // Close the handle after use
  FindClose(hFind);

  // Return the list of files and success status (0)
  return { files, 0 };
}

#else

std::pair<std::string, int> pwd() {
  return { "This command requires a Windows system call to run.", 1 };
}

std::pair<std::string, int> cd(const std::string&) {
  return { "This command requires a Windows system call to run.", 1 };
}

std::pair<std::vector<std::string>, int> ls(const std::string&) {
  return { { "This command requires a Windows system call to run." }, 1 };
}

#endif