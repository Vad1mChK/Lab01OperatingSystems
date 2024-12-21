#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

#include "loaders/ema-sort-int/ExternalMemorySorter.hpp"

// Test fixture class
class ExternalMemorySorterTest : public ::testing::Test {
protected:
  std::string temp_dir = "./";  // Use current directory for temp files

  // Helper function to read a binary file into a vector
  std::vector<uint32_t> readBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      throw std::runtime_error("Failed to open file: " + filename);
    }

    std::vector<uint32_t> data;
    uint32_t value;
    while (file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t))) {
      data.push_back(value);
    }
    return data;
  }

  // Helper function to clean up temporary files
  void deleteFile(const std::string& filename) {
    std::remove(filename.c_str());
  }
};

// Test case: Generate a random file
TEST_F(ExternalMemorySorterTest, GenerateRandomFile) {
  std::string filename = temp_dir + "test_random_file.dat";
  size_t file_size_mb = 8;

  ASSERT_NO_THROW(DirectIoExternalMemorySorter::generateRandomFile(filename, file_size_mb));

  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  ASSERT_TRUE(file.is_open()) << "Generated file could not be opened.";
  ASSERT_EQ(file.tellg(), file_size_mb * 1024 * 1024) << "File size mismatch.";

  deleteFile(filename);
}

// Test case: External memory sort
TEST_F(ExternalMemorySorterTest, ExternalMemorySort) {
  std::string input_filename = temp_dir + "test_input.dat";
  std::string output_filename = temp_dir + "test_output.dat";
  size_t file_size_mb = 8;
  size_t chunk_size_mb = 2;

  // Step 1: Generate random file
  ASSERT_NO_THROW(DirectIoExternalMemorySorter::generateRandomFile(input_filename, file_size_mb));

  // Step 2: Sort the file
  ASSERT_NO_THROW(
      DirectIoExternalMemorySorter::externalMemorySort(input_filename, output_filename, chunk_size_mb)
  );

  // Step 3: Verify the file is sorted
  std::vector<uint32_t> sorted_data = readBinaryFile(output_filename);
  ASSERT_TRUE(std::is_sorted(sorted_data.begin(), sorted_data.end()))
      << "Output file is not sorted.";

  // Cleanup
  deleteFile(input_filename);
  deleteFile(output_filename);
}

// Test case: Check if file is sorted
TEST_F(ExternalMemorySorterTest, CheckFileSorted) {
  std::string filename = temp_dir + "test_sorted.dat";

  // Create a sorted file
  std::ofstream file(filename, std::ios::binary);
  ASSERT_TRUE(file.is_open()) << "Failed to create sorted file.";
  for (uint32_t i = 0; i < 1024; ++i) {
    file.write(reinterpret_cast<const char*>(&i), sizeof(uint32_t));
  }
  file.close();

  // Check if the file is sorted
  ASSERT_NO_THROW(DirectIoExternalMemorySorter::checkFileSorted(filename));

  deleteFile(filename);
}

// Test case: Print help
TEST_F(ExternalMemorySorterTest, PrintHelp) {
  ASSERT_NO_THROW(DirectIoExternalMemorySorter::printHelp());
}