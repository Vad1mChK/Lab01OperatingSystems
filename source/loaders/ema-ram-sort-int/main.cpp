#include <iostream>

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
  } else if (command == "full-benchmark") {
    if (argc != ArgcForEmaSort) {
      std::cout << "Usage: prog full-benchmark <input_file_prefix> <output_file_prefix> <file_size_mb> <chunk_size_mb> <ema-sorters> <ram-sorters>" << '\n';
      return 1;
    }
    std::string const input_file = argv[2];
    std::string const output_file_prefix = argv[3];
    size_t const file_size_mb = std::stoull(argv[4]);
    size_t const chunk_size_mb = std::stoull(argv[5]);
    size_t const ram_sorters_count = std::stoull(argv[6]);
    size_t const ema_sorters_count = std::stoull(argv[7]);


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