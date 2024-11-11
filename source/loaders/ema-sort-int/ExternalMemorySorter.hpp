#ifndef EXTERNAL_MEMORY_SORT_HPP
#define EXTERNAL_MEMORY_SORT_HPP

#include <cstdint>
#include <string>

class ExternalMemorySorter {
private:
  static void sortByChunksAndSave(
      const std::string& input_filename, const std::string& temp_directory, size_t chunk_size_mb
  );

  static void mergeChunksAndSave(
      const std::string& temp_directory,
      const std::string& input_filename, // To retrieve chunk file names
      const std::string& output_filename,
      size_t num_chunks
  );

public:
  // Generate a random binary file of uint32_t values
  static void generateRandomFile(const std::string& filename, size_t size_mb);

  // Sort a large file in chunks and write sorted chunks to the output file
  static void externalMemorySort(
      const std::string& input_filename, const std::string& output_filename, size_t chunk_size_mb
  );

  // Check if the file is sorted
  static void checkFileSorted(const std::string& input_filename);

  // Print help information
  static void printHelp();
};

#endif  // EXTERNAL_MEMORY_SORT_HPP