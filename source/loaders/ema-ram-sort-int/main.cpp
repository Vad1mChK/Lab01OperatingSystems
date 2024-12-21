#include <iostream>

#include "../ema-sort-int/ExternalMemorySorter.hpp"
#include "../ram-sort-int/RamMemorySorter.hpp"
#include "../util/ema_ram_sorter_cli_constants.hpp"
#include "UnifiedMemorySorter.hpp"

int main(int argc, char* argv[]) {
  if (argc < ArgcMin) {
    UnifiedMemorySorter::printHelp();
    return 1;
  }

  std::string const command = argv[1];

  if (command == "generate") {
    if (argc != ArgcForGenerate) {
      std::cout << "Usage: prog generate <output_file> <size_mb>" << '\n';
      return 1;
    }
    std::string const output_file = argv[2];
    size_t const size_mb = std::stoull(argv[3]);
    RamMemorySorter::generateRandomFile(output_file, size_mb);
  } else if (command == "sort") {
    if (argc != ArgcForUnifiedSort) {
      std::cout << "Usage: prog sort <input_file> <output_file_prefix> <chunk_size_mb> <ema_sorters> <ram_sorters>" << '\n';
      return 1;
    }
    std::string const input_file = argv[2];
    std::string const output_file_prefix = argv[3];
    size_t const chunk_size_mb = std::stoull(argv[4]);
    size_t const ram_sorters_count = std::stoull(argv[5]);
    size_t const ema_sorters_count = std::stoull(argv[6]);

    std::vector<std::string> output_files;
    std::vector<std::thread> threads;
    output_files.reserve(ram_sorters_count + ema_sorters_count);
    for (size_t i = 0; i < ram_sorters_count; ++i) {
      std::string output_file = output_file_prefix + ".ram." + std::to_string(i);
      output_files.push_back(output_file);
      threads.push_back(
        std::thread(
          [input_file, output_file]() {
            RamMemorySorter::sortInMemory(input_file, output_file);
          }
        )
      );
    }
    for (size_t i = 0; i < ema_sorters_count; ++i) {
      std::string output_file = output_file_prefix + ".ema." + std::to_string(i);
      output_files.push_back(output_file);
      threads.push_back(
        std::thread(
          [input_file, output_file, chunk_size_mb]() {
            ExternalMemorySorter::externalMemorySort(input_file, output_file, chunk_size_mb);
          }
        )
      );
    }

    for (auto& thread: threads) {
      thread.join();
    }
  } else if (command == "check") {
    if (argc != ArgcForCheck) {
      std::cout << "Usage: prog check <input_file>" << '\n';
      return 1;
    }
    std::string input_file = argv[2];
    RamMemorySorter::checkFileSorted(input_file);
  } else if (command == "help") {
    UnifiedMemorySorter::printHelp();
  } else {
    std::cout << "Unknown subcommand: " << command << '\n';
    UnifiedMemorySorter::printHelp();
    return 1;
  }

  return 0;
}