#ifndef EXTERNAL_MEMORY_SORT_HPP
#define EXTERNAL_MEMORY_SORT_HPP

#include <cstdint>
#include <string>
#include "lab2_library.hpp"

class DirectIoExternalMemorySorter {
private:
  Lab2 lab2_;

  void sortByChunksAndSave(
      const std::string& input_filename, const std::string& temp_directory, size_t chunk_size_mb
  );

  void mergeChunksAndSave(
      const std::string& temp_directory,
      const std::string& input_filename, // To retrieve chunk file names
      const std::string& output_filename,
      size_t num_chunks
  );

public:
  DirectIoExternalMemorySorter(): lab2_(1024, LAB2_BLOCK_SIZE) {};

  // Generate a random binary file of uint32_t values
  void generateRandomFile(const std::string& filename, size_t size_mb);

  // Sort a large file in chunks and write sorted chunks to the output file
  void externalMemorySort(
      const std::string& input_filename, const std::string& output_filename, size_t chunk_size_mb
  );

  // Check if the file is sorted
  void checkFileSorted(const std::string& input_filename);

  // Print help information
  static void printHelp();

  static void echo(std::string message);
};

#endif  // EXTERNAL_MEMORY_SORT_HPP