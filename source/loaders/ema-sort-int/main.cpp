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
      std::cerr << "Usage: prog generate <output_file> <size_mb>" << std::endl;
      return 1;
    }
    std::string output_file = argv[2];
    size_t size_mb = std::stoull(argv[3]);
    ExternalMemorySorter::generateRandomFile(output_file, size_mb);
  }
  else if (command == "sort") {
    if (argc != 5) {
      std::cerr << "Usage: prog sort <input_file> <output_file> <chunk_size_mb>" << std::endl;
      return 1;
    }
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    size_t chunk_size_mb = std::stoull(argv[4]);
    ExternalMemorySorter::externalMemorySort(input_file, output_file, chunk_size_mb);
  }
  else if (command == "check") {
    if (argc != 4) {
      std::cerr << "Usage: prog check <input_file> <chunk_size_mb>" << std::endl;
      return 1;
    }
    std::string input_file = argv[2];
    size_t chunk_size_mb = std::stoull(argv[3]);
    ExternalMemorySorter::checkFileSorted(input_file, chunk_size_mb);
  }
  else if (command == "help") {
    ExternalMemorySorter::printHelp();
  }
  else {
    std::cerr << "Unknown subcommand: " << command << std::endl;
    ExternalMemorySorter::printHelp();
    return 1;
  }

  return 0;
}
