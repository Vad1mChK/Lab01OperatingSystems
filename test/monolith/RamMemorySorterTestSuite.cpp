#include <gtest/gtest.h>

#include <cstdint>
#include <fstream>
#include <vector>

#include "loaders/ram-sort-int/RamMemorySorter.hpp"

// Helper function to read binary file into a vector
std::vector<uint32_t> readBinaryFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  std::vector<uint32_t> data;
  uint32_t value;
  while (file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
    data.push_back(value);
  }
  return data;
}

class RamMemorySorterTest : public ::testing::Test {
protected:
  std::string testInputFile = "test_input.bin";
  std::string testOutputFile = "test_output.bin";

  void TearDown() override {
    // Cleanup test files
    std::remove(testInputFile.c_str());
    std::remove(testOutputFile.c_str());
  }
};

TEST_F(RamMemorySorterTest, GenerateRandomFile) {
  size_t sizeMb = 1;  // Generate a 1 MB file
  RamMemorySorter::generateRandomFile(testInputFile, sizeMb);

  // Check if the file exists and has the expected size
  std::ifstream file(testInputFile, std::ios::binary | std::ios::ate);
  ASSERT_TRUE(file.is_open()) << "Failed to open generated file.";
  ASSERT_EQ(file.tellg(), sizeMb * 1024 * 1024) << "Generated file size mismatch.";
}

TEST_F(RamMemorySorterTest, SortInMemory) {
  size_t sizeMb = 1;
  RamMemorySorter::generateRandomFile(testInputFile, sizeMb);

  // Sort the generated file
  RamMemorySorter::sortInMemory(testInputFile, testOutputFile);

  // Read and validate the sorted file
  auto sortedData = readBinaryFile(testOutputFile);
  ASSERT_FALSE(sortedData.empty()) << "Sorted file is empty.";
  ASSERT_TRUE(std::is_sorted(sortedData.begin(), sortedData.end()))
      << "File is not sorted correctly.";
}

TEST_F(RamMemorySorterTest, CheckFileSorted) {
  size_t sizeMb = 1;
  RamMemorySorter::generateRandomFile(testInputFile, sizeMb);

  // Sort the generated file
  RamMemorySorter::sortInMemory(testInputFile, testOutputFile);

  // Check if the file is sorted
  testing::internal::CaptureStdout();
  RamMemorySorter::checkFileSorted(testOutputFile);
  std::string output = testing::internal::GetCapturedStdout();
  ASSERT_NE(output.find("File is sorted."), std::string::npos)
      << "Expected 'File is sorted.' message not found.";
}

TEST_F(RamMemorySorterTest, CheckFileNotSorted) {
  // Generate an unsorted file
  std::ofstream file(testInputFile, std::ios::binary);
  ASSERT_TRUE(file.is_open()) << "Failed to open test file for writing.";
  std::vector<uint32_t> unsortedData = {5, 3, 8, 1, 7};
  file.write(
      reinterpret_cast<const char*>(unsortedData.data()), unsortedData.size() * sizeof(uint32_t)
  );
  file.close();

  // Check if the file is detected as unsorted
  testing::internal::CaptureStdout();
  RamMemorySorter::checkFileSorted(testInputFile);
  std::string output = testing::internal::GetCapturedStdout();
  ASSERT_NE(output.find("File is not sorted."), std::string::npos)
      << "Expected 'File is not sorted.' message not found.";
}