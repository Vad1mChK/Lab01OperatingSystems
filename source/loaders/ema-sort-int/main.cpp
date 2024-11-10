//
// Created by vadim on 13.10.2024.
//
#include "ExternalMemorySorter.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    ExternalMemorySorter::printHelp();
    return 1;
  }

  std::string command = argv[1];

  if (command == "generate") {
    if (argc != 4) {
      std::cout << "Usage: prog generate <output_file> <size_mb>" << '\n';
      return 1;
    }
    std::string output_file = argv[2];
    size_t size_mb = std::stoull(argv[3]);
    ExternalMemorySorter::generateRandomFile(output_file, size_mb);
  }
  else if (command == "sort") {
    if (argc != 5) {
      std::cout << "Usage: prog sort <input_file> <output_file> <chunk_size_mb>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    size_t chunk_size_mb = std::stoull(argv[4]);
    ExternalMemorySorter::externalMemorySort(input_file, output_file, chunk_size_mb);
  }
  else if (command == "check") {
    if (argc != 3) {
      std::cout << "Usage: prog check <input_file>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    ExternalMemorySorter::checkFileSorted(input_file);
  }
  else if (command == "help") {
    ExternalMemorySorter::printHelp();
  }
  else if (command == "full-benchmark") {
    if (argc != 5) {
      std::cout << "Usage: full-benchmark <input_file> <output_file> <repeat-count>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    size_t repeat_count = std::stoull(argv[4]);

    for (size_t i = 0; i < repeat_count; ++i) {
      ExternalMemorySorter::generateRandomFile(input_file, 256);
      ExternalMemorySorter::externalMemorySort(input_file, output_file, 32);
      ExternalMemorySorter::checkFileSorted(output_file);
    }
  }
  else {
    std::cout << "Unknown subcommand: " << command << '\n';
    ExternalMemorySorter::printHelp();
    return 1;
  }

  return 0;
}
