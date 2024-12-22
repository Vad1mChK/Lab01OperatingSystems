#ifndef RAM_MEMORY_SORTER_HPP
#define RAM_MEMORY_SORTER_HPP

#include <lab2_library.hpp>
#include <string>

class DirectIoRamMemorySorter {
private:
  Lab2 lab2_;
public:
  explicit DirectIoRamMemorySorter(size_t cacheCapacity, size_t blockSize):
    lab2_(cacheCapacity, blockSize) {}

  // Generate a random binary file of uint32_t values
  void generateRandomFile(const std::string& filename, size_t size_mb);

  // Sort the entire file in memory and write the sorted data to the output file
  void sortInMemory(const std::string& input_filename, const std::string& output_filename);

  // Check if the file is sorted
  void checkFileSorted(const std::string& filename);

  // Print help information
  static void printHelp();
};

#endif  // RAM_MEMORY_SORTER_HPP
