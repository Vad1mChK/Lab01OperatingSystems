//
// Created by vadim on 14.10.2024.
//
#include <iostream>
#include <string>

#include "DirectIoRamMemorySorter.hpp"
#include "../../common/unistd_check.hpp"
#include "../util/ema_ram_sorter_cli_constants.hpp"

int main(int argc, char* argv[]) {
  if (argc < ArgcMin) {
    DirectIoRamMemorySorter::printHelp();
    return 1;
  }

  DirectIoRamMemorySorter sorter(1024, LAB2_BLOCK_SIZE);

  std::string command = argv[1];

  if (command == "generate") {
    if (argc != ArgcForGenerate) {
      std::cout << "Usage: prog generate <output_file> <size_mb>" << '\n';
      return 1;
    }
    std::string output_file = argv[2];
    size_t size_mb = std::stoull(argv[3]);
    sorter.generateRandomFile(output_file, size_mb);
  } else if (command == "sort") {
    if (argc != ArgcForRamSort) {
      std::cout << "Usage: prog sort <input_file> <output_file>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    sorter.sortInMemory(input_file, output_file);
  } else if (command == "check") {
    if (argc != ArgcForCheck) {
      std::cout << "Usage: prog check <input_file>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    sorter.checkFileSorted(input_file);
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
      sorter.generateRandomFile(input_file, FullBenchmarkFileSizeMb);
      sorter.sortInMemory(input_file, output_file);
      sorter.checkFileSorted(output_file);
    }
  } else if (command == "help") {
    DirectIoRamMemorySorter::printHelp();
  } else {
    std::cout << "Unknown command: " << command << '\n';
    DirectIoRamMemorySorter::printHelp();
    return 1;
  }

  return 0;
}