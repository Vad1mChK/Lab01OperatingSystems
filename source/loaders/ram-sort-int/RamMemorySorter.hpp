#ifndef RAM_MEMORY_SORTER_HPP
#define RAM_MEMORY_SORTER_HPP

#include <string>

class RamMemorySorter {
public:
  // Generate a random binary file of uint32_t values
  static void generateRandomFile(const std::string& filename, size_t size_mb);

  // Sort the entire file in memory and write the sorted data to the output file
  static void sortInMemory(const std::string& input_filename, const std::string& output_filename);

  // Check if the file is sorted
  static void checkFileSorted(const std::string& filename);

  // Print help information
  static void printHelp();
};

#endif  // RAM_MEMORY_SORTER_HPP
