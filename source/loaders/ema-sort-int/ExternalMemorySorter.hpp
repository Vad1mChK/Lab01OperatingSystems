#ifndef EXTERNAL_MEMORY_SORT_HPP
#define EXTERNAL_MEMORY_SORT_HPP

#include <string>

class ExternalMemorySorter {
public:
  // Generate a random binary file of uint32_t values
  static void generateRandomFile(const std::string& filename, size_t size_in_mb);

  // Sort a large file in chunks, and write sorted chunks to the output file
  static void externalMemorySort(
    const std::string& input_filename,
    const std::string& output_filename,
    size_t chunk_size_in_mb
  );

  // Check if the file is sorted
  static void checkFileSorted(const std::string& input_filename, size_t chunk_size_in_mb);

  // Print help information
  static void printHelp();
};

#endif // EXTERNAL_MEMORY_SORT_HPP
