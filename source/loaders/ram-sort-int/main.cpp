//
// Created by vadim on 14.10.2024.
//
#include "RamMemorySorter.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    RamMemorySorter::printHelp();
    return 1;
  }

  std::string command = argv[1];

  if (command == "generate") {
    if (argc != 4) {
      std::cout << "Usage: prog generate <output_file> <size_mb>" << std::endl;
      return 1;
    }
    std::string output_file = argv[2];
    size_t size_mb = std::stoull(argv[3]);
    RamMemorySorter::generateRandomFile(output_file, size_mb);
  }
  else if (command == "sort") {
    if (argc != 4) {
      std::cout << "Usage: prog sort <input_file> <output_file>" << std::endl;
      return 1;
    }
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    RamMemorySorter::sortInMemory(input_file, output_file);
  }
  else if (command == "check") {
    if (argc != 3) {
      std::cout << "Usage: prog check <input_file>" << std::endl;
      return 1;
    }
    std::string input_file = argv[2];
    RamMemorySorter::checkFileSorted(input_file);
  }
  else if (command == "full-benchmark") {
    if (argc != 5) {
      std::cout << "Usage: prog full-benchmark <input_file> <output-file> <repeat-count>"
        << std::endl;
      return 1;
    }
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    size_t repeat_count = std::stoull(argv[4]);
    for (size_t i = 0; i < repeat_count; ++i) {
      RamMemorySorter::generateRandomFile(input_file, 256);
      RamMemorySorter::sortInMemory(input_file, output_file);
      RamMemorySorter::checkFileSorted(output_file);
    }
  }
  else if (command == "help") {
    RamMemorySorter::printHelp();
  }
  else {
    std::cout << "Unknown command: " << command << std::endl;
    RamMemorySorter::printHelp();
    return 1;
  }

  return 0;
}
