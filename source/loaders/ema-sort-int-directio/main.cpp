//
// Created by vadim on 13.10.2024.
//
#include <iostream>
#include <string>

#include "../util/ema_ram_sorter_cli_constants.hpp"
#include "DirectIoExternalMemorySorter.hpp"

int main(int argc, char* argv[]) {
  if (argc < ArgcMin) {
    DirectIoExternalMemorySorter::printHelp();
    return 1;
  }

  if (std::string const command = argv[1]; command == "generate") {
    if (argc != ArgcForGenerate) {
      std::cout << "Usage: prog generate <output_file> <size_mb>" << '\n';
      return 1;
    }
    std::string const output_file = argv[2];
    size_t size_mb = std::stoull(argv[3]);
    DirectIoExternalMemorySorter::generateRandomFile(output_file, size_mb);
  } else if (command == "sort") {
    if (argc != ArgcForEmaSort) {
      std::cout << "Usage: prog sort <input_file> <output_file> <chunk_size_mb>" << '\n';
      return 1;
    }
    std::string const input_file = argv[2];
    std::string const output_file = argv[3];
    size_t const chunk_size_mb = std::stoull(argv[4]);
    DirectIoExternalMemorySorter::externalMemorySort(input_file, output_file, chunk_size_mb);
  } else if (command == "check") {
    if (argc != ArgcForCheck) {
      std::cout << "Usage: prog check <input_file>" << '\n';
      return 1;
    }
    std::string const input_file = argv[2];
    DirectIoExternalMemorySorter::checkFileSorted(input_file);
  } else if (command == "help") {
    DirectIoExternalMemorySorter::printHelp();
  } else if (command == "full-benchmark") {
    if (argc != ArgcForFull) {
      std::cout << "Usage: prog full-benchmark <input_file> <output_file> <repeat-count>" << '\n';
      return 1;
    }
    std::string const input_file = argv[2];
    std::string const output_file = argv[3];
    size_t const repeat_count = std::stoull(argv[4]);

    // Fallback to sequential execution
    for (size_t i = 0; i < repeat_count; ++i) {
      DirectIoExternalMemorySorter::generateRandomFile(input_file, FullBenchmarkFileSizeMb);
      DirectIoExternalMemorySorter::externalMemorySort(input_file, output_file, FullBenchmarkChunkSizeMb);
      DirectIoExternalMemorySorter::checkFileSorted(output_file);
    }
  } else {
    std::cout << "Unknown subcommand: " << command << '\n';
    DirectIoExternalMemorySorter::printHelp();
    return 1;
  }

  return 0;
}