//
// Created by vadim on 14.10.2024.
//
#include <iostream>
#include <string>

#include "RamMemorySorter.hpp"
#include "../../common/unistd_check.hpp"
#include "../util/ema_ram_sorter_cli_constants.hpp"

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

    // Fallback to sequential execution
    for (size_t i = 0; i < repeat_count; ++i) {
      RamMemorySorter::generateRandomFile(input_file, FullBenchmarkFileSizeMb);
      RamMemorySorter::sortInMemory(input_file, output_file);
      RamMemorySorter::checkFileSorted(output_file);
    }
  } else if (command == "help") {
    RamMemorySorter::printHelp();
  } else {
    std::cout << "Unknown command: " << command << '\n';
    RamMemorySorter::printHelp();
    return 1;
  }

  return 0;
}